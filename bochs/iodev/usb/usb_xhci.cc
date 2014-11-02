/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2010-2014  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2011-2014  The Bochs Project
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

// Experimental USB xHCI adapter

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

/* Notes by Ben Lunt:
 *  This emulates a NEC/Renesas uPD720202 2-port (2 socket, 4-port register sets)
 *   Root Hub xHCI Host Controller.
 *  Many, many thanks to Renesas for their work and effort in helping my research.
 *
 *  I have tested in with my own tests and WinXP Home Edition SP3.
 *
 *  Use port1 and port2 to emulate a Super-speed device, and use port3 and port4
 *   to emulate Low-, Full-, or High-speed devices.
 *
 *  Examples:
 *   usb_xhci: enabled=1, port3=disk:"usbdisk.img", options3=speed:low
 *   usb_xhci: enabled=1, port3=disk:"usbdisk.img", options3=speed:full
 *   usb_xhci: enabled=1, port3=disk:"usbdisk.img", options3=speed:high
 *   usb_xhci: enabled=1, port1=disk:"usbdisk.img", options1=speed:super
 *
 *  Currently, only a USB MSD Thumb Drive device is emulated as a Super-speed device.
 *  All other emulated devices are Low- or Full-speed devices.
 *
 * This code is 32- and 64-bit addressing aware (uses: bx_phy_address & ADDR_CAP_64)
 *
 * This emulation is coded with the new registers of xHCI version 1.10.  However,
 *  nothing more has been done to emulate 1.10 features.  YET.
 * This emulation is set with VERSION_MAJOR and VERSION_MINOR, 0x01 and 0x10 
 *  respectively.
 *
 * This emulation is experimental.  It is not completly accorate, though it is
 *  my intention to make it as close as possible.  If you find an error or would
 *  like to add to the emulation, please let me know at fys [at] fysnet [dot] net.
 *
 *
 */

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_USB_XHCI

#include "pci.h"
#include "usb_common.h"
#include "usb_xhci.h"

#define LOG_THIS theUSB_XHCI->

bx_usb_xhci_c* theUSB_XHCI = NULL;

Bit8u port_speed_allowed[USB_XHCI_PORTS] = { USB3, USB3, USB2, USB2 };
Bit8u ext_caps[EXT_CAPS_SIZE] = {
  /* 0x500 */ 0x01,                    // Legacy Support
              0x04,                    // next = 0x04 * 4 = 0x510
              0x00, 0x00,              // Semaphores
              0x00, 0x00, 0x00, 0xE0,  // SMI on Bar, SMI on Command, SMI on Semaphore
              0x00, 0x00, 0x00, 0x00,  // reserved
              0x00, 0x00, 0x00, 0x00,  // filler

  /* 0x510 */ 0x02,                    // Port Protocol
              0x05,                    // next = 0x05 * 4 = 0x524
              0x00, 0x03,              // Version 3.0
              0x55, 0x53, 0x42, 0x20,  // "USB "
              0x01,                    // 1-based starting index
              0x02,                    // count of 2 port registers starting at base above
              0x00, 0x00,              // reserved
              0x00, 0x00, 0x00, 0x00,  // reserved
              0x00, 0x00, 0x00, 0x00,  // filler

  /* 0x524 */ 0x02,                    // Port Protocol
              0x07,                    // next = 0x07 * 4 = 0x540
              0x00, 0x02,              // Version 2.0
              0x55, 0x53, 0x42, 0x20,  // "USB "
              0x03,                    // 1-based starting index
              0x02,                    // count of 2 port registers starting at base above
              0x00, 0x00,              // 
              0x00, 0x00, 0x00, 0x00,  //
              0x00, 0x00, 0x00, 0x00,  //
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

/* See Section 6.2.6 of xHCI version 1.00
 *  Bandwidth of each port (4) of each speed.
 */
Bit8u port_band_width[4][1 + USB_XHCI_PORTS] = {
/*                   rsvd, port1, port2, port3, port4 */
/*  Full Speed */  { 0x00,     0,     0,    90,    90 },
/*   Low Speed */  { 0x00,     0,     0,    90,    90 },
/*  High Speed */  { 0x00,     0,     0,    80,    80 },
/* Super Speed */  { 0x00,    90,    90,     0,     0 }
};

// builtin configuration handling functions

Bit32s usb_xhci_options_parser(const char *context, int num_params, char *params[])
{
  if (!strcmp(params[0], "usb_xhci")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_USB_XHCI);
    for (int i = 1; i < num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        SIM->get_param_bool(BXPN_XHCI_ENABLED)->set(atol(&params[i][8]));
      } else if (!strncmp(params[i], "port", 4)) {
        if (SIM->parse_usb_port_params(context, 0, params[i], BX_N_USB_XHCI_PORTS, base) < 0) {
          return -1;
        }
      } else if (!strncmp(params[i], "options", 7)) {
        if (SIM->parse_usb_port_params(context, 1, params[i], BX_N_USB_XHCI_PORTS, base) < 0) {
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
  SIM->write_usb_options(fp, BX_N_USB_XHCI_PORTS, base);
  return 0;
}

// device plugin entry points

int CDECL libusb_xhci_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theUSB_XHCI = new bx_usb_xhci_c();
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theUSB_XHCI, BX_PLUGIN_USB_XHCI);
  // add new configuration parameter for the config interface
  SIM->init_usb_options("xHCI", "xhci", BX_N_USB_XHCI_PORTS);
  // register add-on option for bochsrc and command line
  SIM->register_addon_option("usb_xhci", usb_xhci_options_parser, usb_xhci_options_save);
  return 0; // Success
}

void CDECL libusb_xhci_LTX_plugin_fini(void)
{
  SIM->unregister_addon_option("usb_xhci");
  bx_list_c *menu = (bx_list_c*)SIM->get_param("ports.usb");
  delete theUSB_XHCI;
  menu->remove("xhci");
}

// the device object

bx_usb_xhci_c::bx_usb_xhci_c()
{
  put("usb_xhci", "XHCI");
  memset((void*)&hub, 0, sizeof(bx_usb_xhci_t));
  device_buffer = NULL;
  //hub.frame_timer_index = BX_NULL_TIMER_HANDLE;
}

bx_usb_xhci_c::~bx_usb_xhci_c()
{
  char pname[16];

  if (BX_XHCI_THIS device_buffer != NULL)
    delete [] BX_XHCI_THIS device_buffer;

  for (int i=0; i<USB_XHCI_PORTS; i++) {
    sprintf(pname, "port%d.device", i+1);
    SIM->get_param_string(pname, SIM->get_param(BXPN_USB_XHCI))->set_handler(NULL);
    remove_device(i);
  }

  SIM->get_bochs_root()->remove("usb_xhci");
  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  usb_rt->remove("xhci");
  BX_DEBUG(("Exit"));
}

void bx_usb_xhci_c::init(void)
{
  unsigned i;
  char pname[6];
  bx_list_c *xhci, *port;
  bx_param_string_c *device;

  /*  If you wish to set DEBUG=report in the code, instead of
   *  in the configuration, simply uncomment this line.  I use
   *  it when I am working on this emulation.
   */
  //LOG_THIS setonoff(LOGLEV_DEBUG, ACT_REPORT);

  // Read in values from config interface
  xhci = (bx_list_c*) SIM->get_param(BXPN_USB_XHCI);
  // Check if the device is disabled or not configured
  if (!SIM->get_param_bool("enabled", xhci)->get()) {
    BX_INFO(("USB xHCI disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("usb_xhci"))->set(0);
    return;
  }

  BX_XHCI_THIS device_buffer = new Bit8u[65536];

  // TODO: Use this to decrement the Interrupter:count down value
  // Call our frame timer routine every 1mS (1,024uS)
  // Continuous and active
  //  BX_XHCI_THIS hub.frame_timer_index =
  //                   bx_pc_system.register_timer(this, usb_frame_handler, 1024, 1, 1, "xhci.frame_timer");

  BX_XHCI_THIS hub.devfunc = 0x00;
  DEV_register_pci_handlers(this, &BX_XHCI_THIS hub.devfunc, BX_PLUGIN_USB_XHCI,
                            "Experimental USB xHCI");

  // initialize readonly registers
  // 0x1912 = vendor (Renesas)
  // 0x0015 = device (0x0014 = uPD720201, 0x0015 = uPD720202)
  // revision number (0x03 = uPD720201, 0x02 = uPD720202)
  init_pci_conf(0x1912, 0x0015, 0x02, 0x0c0330, 0x00);
  BX_XHCI_THIS pci_conf[0x3d] = BX_PCI_INTD;

  BX_XHCI_THIS pci_base_address[0] = 0x0;

  //FIXME: for now, we want a status bar // hub zero, port zero
  BX_XHCI_THIS hub.statusbar_id = bx_gui->register_statusitem("xHCI", 1);

  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  bx_list_c *xhci_rt = new bx_list_c(usb_rt, "xhci", "xHCI Runtime Options");
  xhci_rt->set_options(xhci_rt->SHOW_PARENT | xhci_rt->USE_BOX_TITLE);
  for (i=0; i<USB_XHCI_PORTS; i++) {
    sprintf(pname, "port%d", i+1);
    port = (bx_list_c*)SIM->get_param(pname, xhci);
    xhci_rt->add(port);
    device = (bx_param_string_c*)port->get_by_name("device");
    device->set_handler(usb_param_handler);
    BX_XHCI_THIS hub.usb_port[i].device = NULL;
    BX_XHCI_THIS hub.usb_port[i].portsc.ccs = 0;
    BX_XHCI_THIS hub.usb_port[i].portsc.csc = 0;
  }

  // register handler for correct device connect handling after runtime config
  SIM->register_runtime_config_handler(BX_XHCI_THIS_PTR, runtime_config_handler);
  BX_XHCI_THIS hub.device_change = 0;

  for (i=0; i<USB_XHCI_PORTS; i++)
    BX_XHCI_THIS hub.usb_port[i].is_usb3 = (port_speed_allowed[i] == USB3);

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

//      { 0x51, 0x70 },                 //  Pointer to next item (0x00 = no more)
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
  int i;
  char pname[6];

  // Capability registers
  BX_XHCI_THIS hub.cap_regs.HcCapLength  = (VERSION_MAJOR << 24) | (VERSION_MINOR << 16) | OPS_REGS_OFFSET;
  BX_XHCI_THIS hub.cap_regs.HcSParams1   = (USB_XHCI_PORTS << 24) | (INTERRUPTERS << 8) | MAX_SLOTS;
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

  BX_DEBUG((" CAPLENGTH: 0x%02X", BX_XHCI_THIS hub.cap_regs.HcCapLength & 0xFF));
  BX_DEBUG(("HC VERSION: %X.%02X", ((BX_XHCI_THIS hub.cap_regs.HcCapLength & 0xFF000000) >> 24), 
    ((BX_XHCI_THIS hub.cap_regs.HcCapLength & 0x00FF0000) >> 16)));
  BX_DEBUG(("HCSPARAMS1: 0x%08X", BX_XHCI_THIS hub.cap_regs.HcSParams1));
  BX_DEBUG(("HCSPARAMS2: 0x%08X", BX_XHCI_THIS hub.cap_regs.HcSParams2));
  BX_DEBUG(("HCSPARAMS3: 0x%08X", BX_XHCI_THIS hub.cap_regs.HcSParams3));
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  BX_DEBUG(("HCCPARAMS1: 0x%08X", BX_XHCI_THIS hub.cap_regs.HcCParams1));
#else
  BX_DEBUG(("HCCPARAMS: 0x%08X", BX_XHCI_THIS hub.cap_regs.HcCParams1));
#endif
  BX_DEBUG(("     DBOFF: 0x%08X", BX_XHCI_THIS hub.cap_regs.DBOFF));
  BX_DEBUG(("    RTSOFF: 0x%08X", BX_XHCI_THIS hub.cap_regs.RTSOFF));
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  BX_DEBUG(("HCCPARAMS2: 0x%08X", BX_XHCI_THIS hub.cap_regs.HcCParams2));
#endif

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
  for (i=0; i<USB_XHCI_PORTS; i++) {
    reset_port(i);
    if (BX_XHCI_THIS hub.usb_port[i].device == NULL) {
      sprintf(pname, "port%d", i+1);
      init_device(i, (bx_list_c*)SIM->get_param(pname, SIM->get_param(BXPN_USB_XHCI)));
    } else {
      usb_set_connect_status(i, BX_XHCI_THIS hub.usb_port[i].device->get_type(), 1);
    }
  }

  for (i=0; i<USB_XHCI_PORTS; i++)
    BX_XHCI_THIS hub.usb_port[i].is_usb3 = (port_speed_allowed[i] == USB3);

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
    BX_XHCI_THIS hub.slots[i].enabled = 0;
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
bx_bool bx_usb_xhci_c::save_hc_state(void)
{
  int i, j;
  Bit64u addr;
  Bit8u *ptr = (Bit8u *) &BX_XHCI_THIS hub;
  Bit8u temp[16 | (8 * MAX_SCRATCH_PADS)];  // at least the larger of 16 or (8 * MAX_SCRATCH_PADS)
  Bit32u crc;

  // do we use the scratch pad buffers to save the controller state?
#if (SCATCH_PAD_RESTORE == 1)
  // get pointer to scratch pad buffers from DCBAAP
  DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap, 16, temp);
  ReadHostQWordFromLittleEndian(&temp[0], addr);

  // get MAX_SCRATCH_PADS worth of pointers
  DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) addr, MAX_SCRATCH_PADS * 8, temp);

  for (i=0; i<MAX_SCRATCH_PADS; i++) {
    crc = 0;
    for (j=0; j<((XHCI_PAGE_SIZE << 12) >> 2) - 1; j++)
      crc += * (Bit32u *) ptr[j * 4];
    ReadHostQWordFromLittleEndian(&temp[i * 8], addr);
    DEV_MEM_WRITE_PHYSICAL_DMA((bx_phy_address)  addr, (XHCI_PAGE_SIZE << 12) - sizeof(Bit32u), ptr);
    DEV_MEM_WRITE_PHYSICAL_DMA((bx_phy_address) (addr + ((XHCI_PAGE_SIZE << 12) - sizeof(Bit32u))), sizeof(Bit32u), (Bit8u *) &crc);
    ptr += ((XHCI_PAGE_SIZE << 12) - sizeof(Bit32u));
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
bx_bool bx_usb_xhci_c::restore_hc_state(void)
{
  int i, j;
  Bit64u addr;
  Bit8u temp[16 | (8 * MAX_SCRATCH_PADS)];  // at least the larger of 16 or (8 * MAX_SCRATCH_PADS)
  Bit8u temp_buffer[(XHCI_PAGE_SIZE << 12)];
  Bit32u crc;

  // if we are to use the scratch pad buffers to restore the controller state
#if (SCATCH_PAD_RESTORE == 1)
  // get pointer to scratch pad buffers from DCBAAP
  DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap, 16, temp);
  ReadHostQWordFromLittleEndian(&temp[0], addr);

  // get MAX_SCRATCH_PADS worth of pointers
  DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) addr, MAX_SCRATCH_PADS * 8, temp);

  // we read it in to a temp buffer just to check the crc.
  for (i=0; i<MAX_SCRATCH_PADS; i++) {
    ReadHostQWordFromLittleEndian(&temp[i * 8], addr);
    DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) addr, (XHCI_PAGE_SIZE << 12), temp_buffer);
    crc = 0;
    for (j=0; j<((XHCI_PAGE_SIZE << 12) >> 2) - 1; j++)
      crc += * (Bit32u *) temp_buffer[j * 4];
    if (crc != * (Bit32u *) temp_buffer[(XHCI_PAGE_SIZE << 12) - sizeof(Bit32u)])
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
  unsigned i;
  char tmpname[16];
  bx_list_c *hub, *port, *reg, *reg_grp, *reg_grp1;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "usb_xhci", "USB xHCI State");
  hub = new bx_list_c(list, "hub");
  reg_grp = new bx_list_c(hub, "cap_regs");
  new bx_shadow_num_c(reg_grp, "HcCapLength", &BX_XHCI_THIS hub.cap_regs.HcCapLength, BASE_HEX);
  new bx_shadow_num_c(reg_grp, "HcSParams1", &BX_XHCI_THIS hub.cap_regs.HcSParams1, BASE_HEX);
  new bx_shadow_num_c(reg_grp, "HcSParams2", &BX_XHCI_THIS hub.cap_regs.HcSParams2, BASE_HEX);
  new bx_shadow_num_c(reg_grp, "HcSParams3", &BX_XHCI_THIS hub.cap_regs.HcSParams3, BASE_HEX);
  new bx_shadow_num_c(reg_grp, "HcCParams1", &BX_XHCI_THIS hub.cap_regs.HcCParams1, BASE_HEX);
  new bx_shadow_num_c(reg_grp, "DBOFF", &BX_XHCI_THIS hub.cap_regs.DBOFF, BASE_HEX);
  new bx_shadow_num_c(reg_grp, "RTSOFF", &BX_XHCI_THIS hub.cap_regs.RTSOFF, BASE_HEX);
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  new bx_shadow_num_c(reg_grp, "HcCParams2", &BX_XHCI_THIS hub.cap_regs.HcCParams2, BASE_HEX);
#endif

  reg_grp = new bx_list_c(hub, "op_regs");
  reg = new bx_list_c(reg_grp, "HcCommand");
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  new bx_shadow_bool_c(reg, "cme", &BX_XHCI_THIS hub.op_regs.HcCommand.cme);
  new bx_shadow_bool_c(reg, "spe", &BX_XHCI_THIS hub.op_regs.HcCommand.spe);
#endif
  new bx_shadow_bool_c(reg, "eu3s", &BX_XHCI_THIS hub.op_regs.HcCommand.eu3s);
  new bx_shadow_bool_c(reg, "ewe", &BX_XHCI_THIS hub.op_regs.HcCommand.ewe);
  new bx_shadow_bool_c(reg, "crs", &BX_XHCI_THIS hub.op_regs.HcCommand.crs);
  new bx_shadow_bool_c(reg, "css", &BX_XHCI_THIS hub.op_regs.HcCommand.css);
  new bx_shadow_bool_c(reg, "lhcrst", &BX_XHCI_THIS hub.op_regs.HcCommand.lhcrst);
  new bx_shadow_bool_c(reg, "hsee", &BX_XHCI_THIS hub.op_regs.HcCommand.hsee);
  new bx_shadow_bool_c(reg, "inte", &BX_XHCI_THIS hub.op_regs.HcCommand.inte);
  new bx_shadow_bool_c(reg, "hcrst", &BX_XHCI_THIS hub.op_regs.HcCommand.hcrst);
  new bx_shadow_bool_c(reg, "rs", &BX_XHCI_THIS hub.op_regs.HcCommand.rs);
  reg = new bx_list_c(reg_grp, "HcStatus");
  new bx_shadow_bool_c(reg, "hce", &BX_XHCI_THIS hub.op_regs.HcStatus.hce);
  new bx_shadow_bool_c(reg, "cnr", &BX_XHCI_THIS hub.op_regs.HcStatus.cnr);
  new bx_shadow_bool_c(reg, "sre", &BX_XHCI_THIS hub.op_regs.HcStatus.sre);
  new bx_shadow_bool_c(reg, "rss", &BX_XHCI_THIS hub.op_regs.HcStatus.rss);
  new bx_shadow_bool_c(reg, "sss", &BX_XHCI_THIS hub.op_regs.HcStatus.sss);
  new bx_shadow_bool_c(reg, "pcd", &BX_XHCI_THIS hub.op_regs.HcStatus.pcd);
  new bx_shadow_bool_c(reg, "eint", &BX_XHCI_THIS hub.op_regs.HcStatus.eint);
  new bx_shadow_bool_c(reg, "hse", &BX_XHCI_THIS hub.op_regs.HcStatus.hse);
  new bx_shadow_bool_c(reg, "hch", &BX_XHCI_THIS hub.op_regs.HcStatus.hch);
  new bx_shadow_num_c(reg_grp, "HcPageSize", &BX_XHCI_THIS hub.op_regs.HcPageSize.pagesize, BASE_HEX);
  reg = new bx_list_c(reg_grp, "HcNotification");
  new bx_shadow_bool_c(reg, "n15", &BX_XHCI_THIS hub.op_regs.HcNotification.n15);
  new bx_shadow_bool_c(reg, "n14", &BX_XHCI_THIS hub.op_regs.HcNotification.n14);
  new bx_shadow_bool_c(reg, "n13", &BX_XHCI_THIS hub.op_regs.HcNotification.n13);
  new bx_shadow_bool_c(reg, "n12", &BX_XHCI_THIS hub.op_regs.HcNotification.n12);
  new bx_shadow_bool_c(reg, "n11", &BX_XHCI_THIS hub.op_regs.HcNotification.n11);
  new bx_shadow_bool_c(reg, "n10", &BX_XHCI_THIS hub.op_regs.HcNotification.n10);
  new bx_shadow_bool_c(reg, "n9", &BX_XHCI_THIS hub.op_regs.HcNotification.n9);
  new bx_shadow_bool_c(reg, "n8", &BX_XHCI_THIS hub.op_regs.HcNotification.n8);
  new bx_shadow_bool_c(reg, "n7", &BX_XHCI_THIS hub.op_regs.HcNotification.n7);
  new bx_shadow_bool_c(reg, "n6", &BX_XHCI_THIS hub.op_regs.HcNotification.n6);
  new bx_shadow_bool_c(reg, "n5", &BX_XHCI_THIS hub.op_regs.HcNotification.n5);
  new bx_shadow_bool_c(reg, "n4", &BX_XHCI_THIS hub.op_regs.HcNotification.n4);
  new bx_shadow_bool_c(reg, "n3", &BX_XHCI_THIS hub.op_regs.HcNotification.n3);
  new bx_shadow_bool_c(reg, "n2", &BX_XHCI_THIS hub.op_regs.HcNotification.n2);
  new bx_shadow_bool_c(reg, "n1", &BX_XHCI_THIS hub.op_regs.HcNotification.n1);
  new bx_shadow_bool_c(reg, "n0", &BX_XHCI_THIS hub.op_regs.HcNotification.n0);
  reg = new bx_list_c(reg_grp, "HcCrcr");
  new bx_shadow_num_c(reg, "crc", &BX_XHCI_THIS hub.op_regs.HcCrcr.crc, BASE_HEX);
  new bx_shadow_bool_c(reg, "crr", &BX_XHCI_THIS hub.op_regs.HcCrcr.crr);
  new bx_shadow_bool_c(reg, "ca", &BX_XHCI_THIS hub.op_regs.HcCrcr.ca);
  new bx_shadow_bool_c(reg, "cs", &BX_XHCI_THIS hub.op_regs.HcCrcr.cs);
  new bx_shadow_bool_c(reg, "rcs", &BX_XHCI_THIS hub.op_regs.HcCrcr.rcs);
  new bx_shadow_num_c(reg_grp, "HcDCBAAP", &BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap, BASE_HEX);
  new bx_shadow_num_c(reg_grp, "HcConfig_MaxSlotsEn", &BX_XHCI_THIS hub.op_regs.HcConfig.MaxSlotsEn, BASE_HEX);
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  new bx_shadow_num_c(reg_grp, "HcConfig_u3e", &BX_XHCI_THIS hub.op_regs.HcConfig.u3e, BASE_HEX);
  new bx_shadow_num_c(reg_grp, "HcConfig_cie", &BX_XHCI_THIS hub.op_regs.HcConfig.cie, BASE_HEX);
#endif

  reg_grp = new bx_list_c(hub, "runtime_regs");
  new bx_shadow_num_c(reg_grp, "mfindex", &BX_XHCI_THIS hub.runtime_regs.mfindex.index, BASE_HEX);
  for (i = 0; i < INTERRUPTERS; i++) {
    sprintf(tmpname, "interrupter%d", i+1);
    reg_grp1 = new bx_list_c(reg_grp, tmpname);
    reg = new bx_list_c(reg_grp1, "iman");
    new bx_shadow_bool_c(reg, "ie", &BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.ie);
    new bx_shadow_bool_c(reg, "ip", &BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.ip);
    reg = new bx_list_c(reg_grp1, "imod");
    new bx_shadow_num_c(reg, "imodc", &BX_XHCI_THIS hub.runtime_regs.interrupter[i].imod.imodc, BASE_HEX);
    new bx_shadow_num_c(reg, "imodi", &BX_XHCI_THIS hub.runtime_regs.interrupter[i].imod.imodi, BASE_HEX);
    new bx_shadow_num_c(reg_grp1, "erstabsize", &BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstsz.erstabsize, BASE_HEX);
    new bx_shadow_num_c(reg_grp1, "erstabadd", &BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.erstabadd, BASE_HEX);
    reg = new bx_list_c(reg_grp1, "erdp");
    new bx_shadow_num_c(reg, "eventadd", &BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.eventadd, BASE_HEX);
    new bx_shadow_bool_c(reg, "ehb", &BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.ehb);
    new bx_shadow_num_c(reg, "desi", &BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.desi, BASE_HEX);
  }

  for (i = 0; i < USB_XHCI_PORTS; i++) {
    sprintf(tmpname, "port%d", i+1);
    port = new bx_list_c(hub, tmpname);
    new bx_shadow_bool_c(port, "is_usb3", &BX_XHCI_THIS hub.usb_port[i].is_usb3);
    new bx_shadow_bool_c(port, "has_been_reset", &BX_XHCI_THIS hub.usb_port[i].has_been_reset);
    reg = new bx_list_c(port, "portsc");
    new bx_shadow_bool_c(reg, "wpr", &BX_XHCI_THIS hub.usb_port[i].portsc.wpr);
    new bx_shadow_bool_c(reg, "dr", &BX_XHCI_THIS hub.usb_port[i].portsc.dr);
    new bx_shadow_bool_c(reg, "woe", &BX_XHCI_THIS hub.usb_port[i].portsc.woe);
    new bx_shadow_bool_c(reg, "wde", &BX_XHCI_THIS hub.usb_port[i].portsc.wde);
    new bx_shadow_bool_c(reg, "wce", &BX_XHCI_THIS hub.usb_port[i].portsc.wce);
    new bx_shadow_bool_c(reg, "cas", &BX_XHCI_THIS hub.usb_port[i].portsc.cas);
    new bx_shadow_bool_c(reg, "cec", &BX_XHCI_THIS hub.usb_port[i].portsc.cec);
    new bx_shadow_bool_c(reg, "plc", &BX_XHCI_THIS hub.usb_port[i].portsc.plc);
    new bx_shadow_bool_c(reg, "prc", &BX_XHCI_THIS hub.usb_port[i].portsc.prc);
    new bx_shadow_bool_c(reg, "occ", &BX_XHCI_THIS hub.usb_port[i].portsc.occ);
    new bx_shadow_bool_c(reg, "wrc", &BX_XHCI_THIS hub.usb_port[i].portsc.wrc);
    new bx_shadow_bool_c(reg, "pec", &BX_XHCI_THIS hub.usb_port[i].portsc.pec);
    new bx_shadow_bool_c(reg, "csc", &BX_XHCI_THIS hub.usb_port[i].portsc.csc);
    new bx_shadow_bool_c(reg, "lws", &BX_XHCI_THIS hub.usb_port[i].portsc.lws);
    new bx_shadow_num_c(reg, "pic", &BX_XHCI_THIS hub.usb_port[i].portsc.pic, BASE_HEX);
    new bx_shadow_num_c(reg, "speed", &BX_XHCI_THIS hub.usb_port[i].portsc.speed);
    new bx_shadow_bool_c(reg, "pp", &BX_XHCI_THIS hub.usb_port[i].portsc.pp);
    new bx_shadow_num_c(reg, "pls", &BX_XHCI_THIS hub.usb_port[i].portsc.pls, BASE_HEX);
    new bx_shadow_bool_c(reg, "pr", &BX_XHCI_THIS hub.usb_port[i].portsc.pr);
    new bx_shadow_bool_c(reg, "oca", &BX_XHCI_THIS hub.usb_port[i].portsc.oca);
    new bx_shadow_bool_c(reg, "ped", &BX_XHCI_THIS hub.usb_port[i].portsc.ped);
    new bx_shadow_bool_c(reg, "ccs", &BX_XHCI_THIS hub.usb_port[i].portsc.ccs);

    // TODO: handle USB2/USB3 cases
    reg = new bx_list_c(port, "portpmsc");

    new bx_shadow_num_c(port, "portli_lec", &BX_XHCI_THIS hub.usb_port[i].usb3.portli.lec, BASE_HEX);
    reg = new bx_list_c(port, "porthlpmc");
    new bx_shadow_num_c(reg, "hirdm", &BX_XHCI_THIS hub.usb_port[i].porthlpmc.hirdm, BASE_HEX);
    new bx_shadow_num_c(reg, "l1timeout", &BX_XHCI_THIS hub.usb_port[i].porthlpmc.l1timeout, BASE_HEX);
    new bx_shadow_num_c(reg, "hirdd", &BX_XHCI_THIS hub.usb_port[i].porthlpmc.hirdd, BASE_HEX);
    // empty list for USB device state
    new bx_list_c(port, "device");
  }
  register_pci_state(hub);
}

void bx_usb_xhci_c::after_restore_state(void)
{
  if (DEV_pci_set_base_mem(BX_XHCI_THIS_PTR, read_handler, write_handler,
                         &BX_XHCI_THIS pci_base_address[0],
                         &BX_XHCI_THIS pci_conf[0x10],
                         4096))  {
     BX_INFO(("new base address: 0x%04X", BX_XHCI_THIS pci_base_address[0]));
  }
  for (int j=0; j<USB_XHCI_PORTS; j++) {
    if (BX_XHCI_THIS hub.usb_port[j].device != NULL) {
      BX_XHCI_THIS hub.usb_port[j].device->after_restore_state();
    }
  }
}

void bx_usb_xhci_c::init_device(Bit8u port, bx_list_c *portconf)
{
  usbdev_type type;
  char pname[BX_PATHNAME_LEN];
  const char *devname = NULL;

  devname = ((bx_param_string_c*)portconf->get_by_name("device"))->getptr(); 
  if (devname == NULL) return; 
  if (!strlen(devname) || !strcmp(devname, "none")) return;

  if (BX_XHCI_THIS hub.usb_port[port].device != NULL) {
    BX_ERROR(("init_device(): port%d already in use", port+1));
    return;
  }
  sprintf(pname, "usb_xhci.hub.port%d.device", port+1);
  bx_list_c *sr_list = (bx_list_c*)SIM->get_param(pname, SIM->get_bochs_root());
  type = DEV_usb_init_device(portconf, BX_XHCI_THIS_PTR, &BX_XHCI_THIS hub.usb_port[port].device, sr_list);
  if (BX_XHCI_THIS hub.usb_port[port].device != NULL) {
    usb_set_connect_status(port, type, 1);
  }
}

void bx_usb_xhci_c::remove_device(Bit8u port)
{
  char pname[BX_PATHNAME_LEN];

  if (BX_XHCI_THIS hub.usb_port[port].device != NULL) {
    delete BX_XHCI_THIS hub.usb_port[port].device;
    BX_XHCI_THIS hub.usb_port[port].device = NULL;
    sprintf(pname, "usb_xhci.hub.port%d.device", port+1);
    bx_list_c *devlist = (bx_list_c*)SIM->get_param(pname, SIM->get_bochs_root());
    if (devlist) devlist->clear();
  }
}

void bx_usb_xhci_c::update_irq(unsigned interrupter)
{
  bx_bool level = 0;

  if ((BX_XHCI_THIS hub.op_regs.HcCommand.inte) &&
      (BX_XHCI_THIS hub.runtime_regs.interrupter[interrupter].iman.ie)) {
    level = 1;
    BX_DEBUG(("Interrupt Fired."));
  }
  DEV_pci_set_irq(BX_XHCI_THIS hub.devfunc, BX_XHCI_THIS pci_conf[0x3d], level);
}

bx_bool bx_usb_xhci_c::read_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  Bit32u val = 0, val_hi = 0;
  Bit64u val64;
  int i, speed = 0;

  const Bit32u offset = (Bit32u) (addr - BX_XHCI_THIS pci_base_address[0]);

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
  if ((offset >= PORT_SET_OFFSET) && (offset < (PORT_SET_OFFSET + (USB_XHCI_PORTS * 16)))) {
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
          BX_ERROR(("Read from Reserved Register in Port Register Set %i", port));
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
        ReadHostWordFromLittleEndian(&BX_XHCI_THIS hub.extended_caps[caps_offset], val);
        break;
      case 4:
        ReadHostDWordFromLittleEndian(&BX_XHCI_THIS hub.extended_caps[caps_offset], val);
        break;
      case 8:
        ReadHostQWordFromLittleEndian(&BX_XHCI_THIS hub.extended_caps[caps_offset], val64);
        val = (Bit32u) val64;
        val_hi = (Bit32u) (val64 >> 32);
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
    BX_ERROR(("register read from unknown offset 0x%08X:  0x%08X%08X (len=%i)", offset, (Bit32u) val_hi, (Bit32u) val, len));
#else
    BX_DEBUG(("register read from unknown offset 0x%08X:  0x%08X (len=%i)", offset, (Bit32u) val, len));
    if (len > 4)
      BX_DEBUG(("Ben: In 32-bit mode, len > 4! (len=%i)", len));
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
    BX_DEBUG(("register read from offset 0x%04X:  0x%08X%08X (len=%i)", offset, (Bit32u) val_hi, (Bit32u) val, len));
#else
    BX_DEBUG(("register read from offset 0x%04X:  0x%08X (len=%i)", offset, (Bit32u) val, len));
    if (len > 4)
      BX_DEBUG(("Ben: In 32-bit mode, len > 4! (len=%i)", len));
#endif
  }
  return 1;
}

bx_bool bx_usb_xhci_c::write_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  Bit32u value = *((Bit32u *) data);
  Bit32u value_hi = *((Bit32u *) ((Bit8u *) data + 4));
  const Bit32u offset = (Bit32u) (addr - BX_XHCI_THIS pci_base_address[0]);
  Bit32u temp;
  int i;
  bx_bool reset_type;

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
    BX_DEBUG(("register write to  offset 0x%04X:  0x%08X%08X (len=%i)", offset, value_hi, value, len));
#else
    BX_DEBUG(("register write to  offset 0x%04X:  0x%08X (len=%i)", offset, value, len));
    if (len > 4)
      BX_DEBUG(("Ben: In 32-bit mode, len > 4! (len=%i)", len));
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
        //FIXME: should this line go where system software clears the IP bit, or here when it clears the status:eint bit?
        if (value & (1 << 3))  // acknowledging the interrupt
          DEV_pci_set_irq(BX_XHCI_THIS hub.devfunc, BX_XHCI_THIS pci_conf[0x3d], 0);
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
  if ((offset >= PORT_SET_OFFSET) && (offset < (PORT_SET_OFFSET + (USB_XHCI_PORTS * 16)))) {
    unsigned port = (((offset - PORT_SET_OFFSET) >> 4) & 0x3F); // calculate port number
    switch (offset & 0x0000000F) {
      case 0x00:
        if (value & (1<<9)) {  // port power
          if (value & ((1<<30) | (1<<24) | (1<<3) | (1<<0)))
            BX_ERROR(("Write to one or more Read-only bits in PORTSC[%i] Register (0x%08X)", port, value));
          if (value & ((3<<28) | (1<<2)))
            BX_ERROR(("Write non-zero to a RsvdZ member of PORTSC[%i] Register", port));
          if (BX_XHCI_THIS hub.usb_port[port].is_usb3) {
            BX_XHCI_THIS hub.usb_port[port].portsc.wpr = (value & (1 << 31)) ? 1 : 0;
            BX_XHCI_THIS hub.usb_port[port].portsc.cec = (value & (1 << 23)) ? 1 : 0;
            BX_XHCI_THIS hub.usb_port[port].portsc.wrc = (value & (1 << 20)) ? 0 : BX_XHCI_THIS hub.usb_port[port].portsc.wrc;
            if (value & (1<<18))
              BX_ERROR(("Write to USB3 port: bit 18"));
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
          BX_XHCI_THIS hub.usb_port[port].portsc.pp    = 1;
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
            reset_type = (value & (1 << 4)) ? HOT_RESET : WARM_RESET;
            BX_INFO(("Reset port #%i (PORTSC[%i])", port + 1, port));
            BX_XHCI_THIS hub.usb_port[port].portsc.pr = 0;
            BX_XHCI_THIS hub.usb_port[port].has_been_reset = 1;
            if (BX_XHCI_THIS hub.usb_port[port].portsc.ccs) {
              BX_XHCI_THIS hub.usb_port[port].portsc.prc = 1;
              BX_XHCI_THIS hub.usb_port[port].portsc.pls = PLS_U0;
              BX_XHCI_THIS hub.usb_port[port].portsc.ped = 1;
              if (BX_XHCI_THIS hub.usb_port[port].device != NULL) {
                DEV_usb_send_msg(BX_XHCI_THIS hub.usb_port[port].device, USB_MSG_RESET);
                if (BX_XHCI_THIS hub.usb_port[port].is_usb3 && (reset_type == WARM_RESET))
                  BX_XHCI_THIS hub.usb_port[port].portsc.wrc = 1;
                BX_XHCI_THIS hub.usb_port[port].portsc.prc = 1;
              }
            } else {
              BX_XHCI_THIS hub.usb_port[port].portsc.pls = PLS_RXDETECT;
              BX_XHCI_THIS hub.usb_port[port].portsc.ped = 0;
              BX_XHCI_THIS hub.usb_port[port].portsc.speed = 0;
            }
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
              BX_ERROR(("bits 31:17 in PORTPMSC[%i] Register were not preserved", port));
            BX_XHCI_THIS hub.usb_port[port].usb3.portpmsc.fla   = (value & (1 << 16)) ? 1 : 0;
            BX_XHCI_THIS hub.usb_port[port].usb3.portpmsc.u2timeout = (value & (0xFF << 8)) >> 8;
            BX_XHCI_THIS hub.usb_port[port].usb3.portpmsc.u1timeout = (value & (0xFF << 0)) >> 0;
          } else {
            if (value & (7<<0))
              BX_ERROR(("Write to one or more Read-only bits in PORTPMSC[%i] Register", port));
            BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.tmode     = (value & (0xF << 28)) >> 28;
            temp = BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.RsvdP;
            BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.RsvdP     = (value & (0x1FFF << 15)) >> 15;
            if (temp != BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.RsvdP)
              BX_ERROR(("bits 27:15 in PORTPMSC[%i] Register were not preserved", port));
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
              BX_ERROR(("Write to one or more Read-only bits in PORTLI[%i] Register", port));
            temp = BX_XHCI_THIS hub.usb_port[port].usb3.portli.RsvdP;
            BX_XHCI_THIS hub.usb_port[port].usb3.portli.RsvdP = value >> 16;
            if (temp != BX_XHCI_THIS hub.usb_port[port].usb3.portli.RsvdP)
              BX_ERROR(("bits 31:16 in PORTLI[%i] Register were not preserved", port));
          } else {
            temp = BX_XHCI_THIS hub.usb_port[port].usb2.portli.RsvdP;
            BX_XHCI_THIS hub.usb_port[port].usb2.portli.RsvdP = value;
            if (temp != BX_XHCI_THIS hub.usb_port[port].usb2.portli.RsvdP)
              BX_ERROR(("bits 31:0 in PORTLI[%i] Register were not preserved", port));
          }
        }
        break;
      case 0x0C:
#if ((VERSION_MAJOR < 1) || ((VERSION_MAJOR == 1) && (VERSION_MINOR == 0)))
        BX_ERROR(("Write to Reserved Register in Port Register Set %i", port));
#else
        temp = BX_XHCI_THIS hub.usb_port[port].porthlpmc.RsvdP;
        BX_XHCI_THIS hub.usb_port[port].porthlpmc.RsvdP = (value >> 14);
        if (temp != BX_XHCI_THIS hub.usb_port[port].porthlpmc.RsvdP)
          BX_ERROR(("bits 31:14 in PORTHLPMC[%i] Register were not preserved", port));
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
      BX_DEBUG(("Rang Doorbell:  slot = %i  ep = %i (%s)", doorbell, ep, (ep & 1) ? "IN" : "OUT"));
      if (ep > 31)
        BX_ERROR(("Doorbell rang with EP > 31  (ep = %i)", ep));
      else
        process_transfer_ring(doorbell, ep);
    }
  } else
    BX_ERROR(("register write to unknown offset 0x%08X:  0x%08X%08X (len=%i)", offset, (Bit32u) value_hi, (Bit32u) value, len));

  return 1;
}

// This function checks and processes all enqueued TRB's in the EP's transfer ring
void bx_usb_xhci_c::process_transfer_ring(const int slot, const int ep)
{
  struct TRB trb;
  Bit64u address = 0, org_addr;
  int int_target/*, td_size*/;
  Bit32u transfer_length;
  int ret, len;
  int port_num = BX_XHCI_THIS hub.slots[slot].slot_context.rh_port_num;
  USBPacket packet;
  Bit8u cur_direction = (ep & 1) ? USB_TOKEN_IN : USB_TOKEN_OUT; // for NORMAL without SETUP
  bx_bool is_transfer_trb, is_immed_data, ioc, spd_occurred = 0;
  bx_bool first_event_trb_encountered = 0;
  Bit32u bytes_not_transferred = 0;
  int comp_code = 0;

  // this assumes that we are starting at the first of the TD when this function is called.
  // this is usually the case, and rarely isn't.
  int trb_count = 0;
  BX_XHCI_THIS hub.slots[slot].ep_context[ep].edtla = 0;

  // if the ep is disabled, return an error event trb.
  if ((BX_XHCI_THIS hub.slots[slot].slot_context.slot_state == SLOT_STATE_DISABLED_ENABLED)
    || (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_DISABLED)) {
    org_addr = BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer;
    write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(EP_NOT_ENABLED), 
      TRB_SET_SLOT(slot) | TRB_SET_EP(ep) | TRB_SET_TYPE(TRANS_EVENT), 1);
    return;
  }

  // if the ep is in the halted or error state, ignore the doorbell ring.
  if ((BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_HALTED)
    || (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_ERROR))
    return;

  // if the ep_context::type::direction field is not correct for the ep type of this ep, then ignore the doorbell
  if (ep >= 2) {
    static int endpoint_dir[8] = { -1, EP_DIR_OUT, EP_DIR_OUT, EP_DIR_OUT, -1, EP_DIR_IN, EP_DIR_IN, EP_DIR_IN };
    int ep_type = (ep & 1) ? EP_DIR_IN : EP_DIR_OUT;
    if (endpoint_dir[BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_type] != ep_type) {
      BX_ERROR(("Endpoint_context::Endpoint_type::direction is not correct for this endpoint number.  Ignoring doorbell ring."));
      return;
    }
  }

  // If an endpoint is in the Stopped state when the doorbell is rung, it will transition to the Running state (page 126)
  // The output Context (*slot_addr) should be updated before any other transfer events are made
  if (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_STOPPED) {
    BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state = EP_STATE_RUNNING;
    update_ep_context(slot, ep);
  }

  // read in the TRB
  read_TRB((bx_phy_address) BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer, &trb);
  BX_DEBUG(("Found TRB: address = 0x" FORMATADDRESS " 0x" FMT_ADDRX64 " 0x%08X 0x%08X  %i",
    (bx_phy_address) BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer,
    trb.parameter, trb.status, trb.command, BX_XHCI_THIS hub.slots[slot].ep_context[ep].rcs));
  while ((trb.command & 1) == BX_XHCI_THIS hub.slots[slot].ep_context[ep].rcs) {
    org_addr = BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer;
    BX_DEBUG(("Found TRB: address = 0x" FORMATADDRESS " 0x" FMT_ADDRX64 " 0x%08X 0x%08X  %i (SPD occurred = %i)",
      (bx_phy_address) org_addr, trb.parameter, trb.status, trb.command, BX_XHCI_THIS hub.slots[slot].ep_context[ep].rcs, spd_occurred));
    trb_count++;
    // these are used in some/most items.
    // If not used, won't hurt to extract bad data.
    int_target = TRB_GET_TARGET(trb.status);
//  td_size = TRB_GET_TDSIZE(trb.status);
    transfer_length = TRB_GET_TX_LEN(trb.status);
    is_transfer_trb = 0;  // assume not a transfer
    ioc = TRB_IOC(trb.command);

    // if a SPD occurred, we only process the LINK and EVENT TRB's in this TD, until either on of these two or end of TD
    if (!spd_occurred ||
       (spd_occurred && ((TRB_GET_TYPE(trb.command) == LINK) || (TRB_GET_TYPE(trb.command) == EVENT_DATA)))) {
      // is the data in trb.parameter? (Immediate data?)
      is_immed_data = TRB_IS_IMMED_DATA(trb.command);
      if (is_immed_data)
        memcpy(BX_XHCI_THIS device_buffer, &trb.parameter, 8);
      else
        address = trb.parameter;

      switch (TRB_GET_TYPE(trb.command)) {
        // is a LINK trb.
        case LINK:
          if (ioc)
            write_event_TRB(int_target, org_addr, TRB_SET_COMP_CODE(TRB_SUCCESS), TRB_SET_TYPE(LINK), 1);
          if (TRB_TOGGLE(trb.command))
            BX_XHCI_THIS hub.slots[slot].ep_context[ep].rcs ^= 1;
          BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer = trb.parameter & (Bit64u) ~0xF;
          BX_DEBUG(("0x" FORMATADDRESS ": Transfer Ring (slot = %i) (ep = %i): LINK TRB:  New dq_pointer = 0x" FMT_ADDRX64 " (%i)",
            (bx_phy_address) org_addr, slot, ep, BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer, BX_XHCI_THIS hub.slots[slot].ep_context[ep].rcs));
#if ((VERSION_MAJOR == 0) && (VERSION_MINOR == 0x95))
          // https://patchwork.kernel.org/patch/51191/
          if (!TRB_CHAIN(trb.command))
            BX_DEBUG(("Chain Bit in Link TRB not set."));
#endif
          read_TRB((bx_phy_address) BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer, &trb);
          continue;

        // Setup Stage TRB
        case SETUP_STAGE:
          cur_direction = USB_TOKEN_SETUP;
          is_transfer_trb = 1;
          BX_DEBUG(("0x" FORMATADDRESS ": Transfer Ring (slot = %i) (ep = %i) (len = %i): Found SETUP TRB", 
            (bx_phy_address) org_addr, slot, ep, transfer_length));
          break;

        // Data Stage TRB
        case DATA_STAGE:
          cur_direction = TRB_GET_DIR(trb.command);
          is_transfer_trb = 1;
          BX_DEBUG(("0x" FORMATADDRESS ": Transfer Ring (slot = %i) (ep = %i) (len = %i): Found DATA STAGE TRB", 
            (bx_phy_address) org_addr, slot, ep, transfer_length));
          break;

        // Status Stage TRB
        case STATUS_STAGE:
          cur_direction = TRB_GET_DIR(trb.command);
          is_transfer_trb = 1;
          transfer_length = 0;
          BX_DEBUG(("0x" FORMATADDRESS ": Transfer Ring (slot = %i) (ep = %i): Found STATUS STAGE TRB", 
            (bx_phy_address) org_addr, slot, ep));
          break;

        // Normal TRB
        case NORMAL:
          is_transfer_trb = 1;
          BX_DEBUG(("0x" FORMATADDRESS ": Transfer Ring (slot = %i) (ep = %i) (len = %i): Found NORMAL TRB", 
            (bx_phy_address) org_addr, slot, ep, transfer_length));
          break;

        // Event TRB
        case EVENT_DATA:
          if (!spd_occurred || (spd_occurred && !first_event_trb_encountered)) {
            comp_code = (spd_occurred) ? SHORT_PACKET : TRB_SUCCESS;
            if (spd_occurred && !first_event_trb_encountered) {
              write_event_TRB(int_target, trb.parameter, 
                TRB_SET_COMP_CODE(comp_code) | (BX_XHCI_THIS hub.slots[slot].ep_context[ep].edtla & 0x00FFFFFF),
                TRB_SET_SLOT(slot) | TRB_SET_EP(ep) | TRB_SET_TYPE(TRANS_EVENT) | (1<<2),
                ioc);
            } else {
              write_event_TRB(int_target, trb.parameter, TRB_SET_COMP_CODE(comp_code), 
                TRB_SET_SLOT(slot) | TRB_SET_EP(ep) | TRB_SET_TYPE(TRANS_EVENT) | (1<<2),
                ioc);
            }
            BX_XHCI_THIS hub.slots[slot].ep_context[ep].edtla = 0;
          }
          if (spd_occurred)
            first_event_trb_encountered = 1;

          BX_DEBUG(("0x" FORMATADDRESS ": Transfer Ring (slot = %i) (ep = %i) (trnsfrd = %i): Found EVENT_DATA TRB: (returning %i)", 
            (bx_phy_address) org_addr, slot, ep, BX_XHCI_THIS hub.slots[slot].ep_context[ep].edtla, comp_code));
          break;

        // unknown TRB type
        default:
          BX_ERROR(("0x" FORMATADDRESS ": Transfer Ring (slot = %i) (ep = %i): Unknown TRB found.", 
            (bx_phy_address) org_addr, slot, ep));
          BX_ERROR(("Unknown trb type found: %i(dec)  (0x" FMT_ADDRX64 " 0x%08X 0x%08X)", TRB_GET_TYPE(trb.command), 
            trb.parameter, trb.status, trb.command));
      }

      // is there a transfer to be done?
      if (is_transfer_trb) {
        // set status bar conditions for device
        if ((transfer_length > 0) && (BX_XHCI_THIS hub.statusbar_id >= 0)) {
          if (cur_direction == USB_TOKEN_IN)
            bx_gui->statusbar_setitem(BX_XHCI_THIS hub.statusbar_id, 1);     // read
          else
            bx_gui->statusbar_setitem(BX_XHCI_THIS hub.statusbar_id, 1, 1);  // write
        }

        comp_code = TRB_SUCCESS;  // assume good trans event
        packet.pid = cur_direction;
        packet.devaddr = BX_XHCI_THIS hub.slots[slot].slot_context.device_address;
        packet.devep = (ep >> 1);
        packet.data = BX_XHCI_THIS device_buffer;
        packet.len = transfer_length;
        switch (cur_direction) {
          case USB_TOKEN_OUT:
          case USB_TOKEN_SETUP:
            if ((is_immed_data == 0) && (transfer_length > 0))
              DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) address, transfer_length, BX_XHCI_THIS device_buffer);
            // The XHCI should block all SET_ADDRESS SETUP TOKEN's
            if ((cur_direction == USB_TOKEN_SETUP)   &&
                (BX_XHCI_THIS device_buffer[0] == 0) &&  // Request type
                (BX_XHCI_THIS device_buffer[1] == 5)) {  // SET_ADDRESS
              len = 0;
              comp_code = TRB_ERROR;
              BX_ERROR(("SETUP_TOKEN: System Software should not send SET_ADDRESS command on the xHCI."));
            } else {
              ret = BX_XHCI_THIS broadcast_packet(&packet, port_num - 1);
              len = transfer_length;
              BX_DEBUG(("OUT: Transferred %i bytes (ret = %i)", len, ret));
            }
            break;
          case USB_TOKEN_IN:
            ret = BX_XHCI_THIS broadcast_packet(&packet, port_num - 1);
            if (ret >= 0) {
              len = ret;
              BX_XHCI_THIS hub.slots[slot].ep_context[ep].edtla += len;
              if (len > 0)
                DEV_MEM_WRITE_PHYSICAL_DMA((bx_phy_address) address, len, BX_XHCI_THIS device_buffer);
              BX_DEBUG(("IN: Transferred %i bytes, requested %i bytes", len, transfer_length));
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
            break;
        }

        // 4.10.1 paragraph 4 
        // 4.10.1.1
        if (ioc) {
          if ((comp_code == TRB_SUCCESS) && spd_occurred && TRB_SPD(trb.command)) {
            comp_code = SHORT_PACKET;
            BX_DEBUG(("Sending Short Packet Detect Event TRB (%i)", bytes_not_transferred));
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
    BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer += 16;
    read_TRB((bx_phy_address) BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer, &trb);
  }

  BX_DEBUG(("Process Transfer Ring: Processed %i TRB's", trb_count));
  if (trb_count == 0)
    BX_ERROR(("Process Transfer Ring: Doorbell rang, but no TRB's were enqueued in the ring."));
}

// This function call starts at the current position in the Command Ring,
//  processes that command, then moves to the next one, until CCS != SCS;
//    BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer = current position in ring
//    BX_XHCI_THIS hub.op_regs.HcCrcr.rcs  = current ring cycle state
//    BX_XHCI_THIS hub.op_regs.HcCrcr.crr  = command ring running
void bx_usb_xhci_c::process_command_ring(void)
{
  struct TRB trb;
  int i, slot, /*slot_type,*/ ep, comp_code = 0, new_addr = 0, bsr = 0;
  Bit32u a_flags = 0, d_flags, tmpval1, tmpval2;
  Bit64u org_addr;
  Bit8u buffer[CONTEXT_SIZE + (32 * CONTEXT_SIZE)];
  struct SLOT_CONTEXT slot_context;
  struct EP_CONTEXT   ep_context;

  if (!BX_XHCI_THIS hub.op_regs.HcCrcr.crr)
    return;

  // read in the TRB
  read_TRB((bx_phy_address) BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer, &trb);
  BX_DEBUG(("Dump command trb: %i(dec)  (0x" FMT_ADDRX64 " 0x%08X 0x%08X) (%i)", TRB_GET_TYPE(trb.command),
    trb.parameter, trb.status, trb.command, BX_XHCI_THIS hub.ring_members.command_ring.rcs));
  while ((trb.command & 1) == BX_XHCI_THIS hub.ring_members.command_ring.rcs) {
    org_addr = BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer;
    switch (TRB_GET_TYPE(trb.command)) {
      // is a LINK trb.
      case LINK:
        // Chain bit and Interrupter Target fields are ignored in Command Rings (Page 370)
        BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer = trb.parameter & (Bit64u) ~0xF;
        if (TRB_IOC(trb.command))
          write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(TRB_SUCCESS), TRB_SET_TYPE(LINK), 1);
        if (TRB_TOGGLE(trb.command))
          BX_XHCI_THIS hub.ring_members.command_ring.rcs ^= 1;
        BX_INFO(("0x" FORMATADDRESS ": Command Ring: Found LINK TRB:  New dq_pointer = 0x" FORMATADDRESS " (%i)", 
          (bx_phy_address) org_addr, (bx_phy_address) BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer, 
          BX_XHCI_THIS hub.ring_members.command_ring.rcs));
        read_TRB((bx_phy_address) BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer, &trb);
        continue;

      // NEC: Get Firmware version
      case NEC_TRB_TYPE_GET_FW:
        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(1) | 0x3021, TRB_SET_TYPE(NEC_TRB_TYPE_CMD_COMP), 1);
        BX_INFO(("NEC GET Firmware Version TRB found.  Returning 0x3021"));
        break;

      /* NEC: Get Verification
       * The NEC/Renesas driver sends a vendor specific TRB to the controller to
       *  verify that the controller is indeed a NEC/Renesas controller.  I do not
       *  have permission from NEC/Renesas to show the code that returns the correct
       *  verification values.  Therefore, this simply returns.  As long as you are
       *  not using a Windows driver, everything will be fine.
       */
      case NEC_TRB_TYPE_GET_UN:
        BX_INFO(("NEC GET Verification TRB found."));
        break;

      case ENABLE_SLOT:
        comp_code = NO_SLOTS_ERROR;  // assume no slots
        slot = 0;
//      slot_type = TRB_GET_STYPE(trb.command);  // currently not used
        for (i=1; i<MAX_SLOTS; i++) {  // slots are one based
          if (BX_XHCI_THIS hub.slots[i].enabled == 0) {
            memset(&BX_XHCI_THIS hub.slots[i], 0, sizeof(struct HC_SLOT_CONTEXT));
            BX_XHCI_THIS hub.slots[i].slot_context.slot_state = SLOT_STATE_DISABLED_ENABLED;
            BX_XHCI_THIS hub.slots[i].enabled = 1;
            slot = i;
            for (i=1; i<32; i++)
              BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.ep_state = EP_STATE_DISABLED;
            comp_code = TRB_SUCCESS;
            break;
          }
        }
        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        BX_INFO(("0x" FORMATADDRESS ": Command Ring: Found Enable Slot TRB (slot = %i) (returning %i)", 
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
        BX_INFO(("0x" FORMATADDRESS ": Command Ring: Found Disable Slot TRB (slot = %i) (returning %i)", 
          (bx_phy_address) org_addr, slot, comp_code));
        break;

      case ADDRESS_DEVICE:
        slot = TRB_GET_SLOT(trb.command);  // slots are 1 based
        if (BX_XHCI_THIS hub.slots[slot].enabled == 1) {
          DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) trb.parameter, (CONTEXT_SIZE + (CONTEXT_SIZE * 2)), buffer);
          bsr = ((trb.command & (1<<9)) == (1<<9));
          ReadHostDWordFromLittleEndian(&buffer[0], tmpval1);
          ReadHostDWordFromLittleEndian(&buffer[4], tmpval2);
          if ((tmpval1 == 0x00) && (tmpval2 == 0x03)) {
            // Use temporary slot and ep context incase there is an error we don't modify the main contexts
            copy_slot_from_buffer(&slot_context, &buffer[CONTEXT_SIZE]);
            copy_ep_from_buffer(&ep_context, &buffer[CONTEXT_SIZE + CONTEXT_SIZE]);
            // Only check for valid context on the first issued ADDRESS_DEVICE command
            if ((BX_XHCI_THIS hub.slots[slot].sent_address == 1) || 
                ((validate_slot_context(&slot_context) && validate_ep_context(&ep_context, -1, 1)))) {
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
                  ReadHostDWordFromLittleEndian(&buffer[CONTEXT_SIZE + 4], tmpval1);
                  int port_num = ((tmpval1 & (0xFF<<16)) >> 16) - 1;  // slot:port_num is 1 based
                  new_addr = create_unique_address(slot);
                  if (send_set_address(new_addr, port_num) == 0) {
                    slot_context.slot_state = SLOT_STATE_ADRESSED;
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
          // mark that we have done this once before
          BX_XHCI_THIS hub.slots[slot].sent_address = 1;
        }

        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        //BX_INFO(("ADDRESS_DEVICE TRB: 0x" FMT_ADDRX64 "  0x%08X 0x%08X", trb.parameter, trb.status, trb.command));
        BX_INFO(("0x" FORMATADDRESS ": Command Ring: SetAddress TRB (bsr = %i) (addr = %i) (slot = %i) (returning %i)", 
          (bx_phy_address) org_addr, bsr, new_addr, slot, comp_code));
        break;

      case EVALUATE_CONTEXT: {
          slot = TRB_GET_SLOT(trb.command);  // slots are 1 based
          if (BX_XHCI_THIS hub.slots[slot].enabled == 1) {
            DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) trb.parameter, (CONTEXT_SIZE + (CONTEXT_SIZE * 32)), buffer);
            ReadHostDWordFromLittleEndian(&buffer[4], a_flags);
            // only the Slot context and EP1 (control EP) contexts are evaluated. Section 6.2.3.3
            // If the slot is not addresses or configured, then return error
            // FIXME: XHCI specs 1.0, page 102 says DEFAULT or higher, while page 321 states higher than DEFAULT!!!
            if (BX_XHCI_THIS hub.slots[slot].slot_context.slot_state >= SLOT_STATE_DEFAULT) {
              comp_code = TRB_SUCCESS;  // assume good completion
              if (a_flags & (1<<0)) {
                copy_slot_from_buffer(&slot_context, &buffer[CONTEXT_SIZE]);
                if (!validate_slot_context(&slot_context))
                  comp_code = PARAMETER_ERROR;
              }
              if (a_flags & (1<<1)) {
                copy_ep_from_buffer(&ep_context, &buffer[CONTEXT_SIZE + CONTEXT_SIZE]);
                if (!validate_ep_context(&ep_context, BX_XHCI_THIS hub.slots[slot].slot_context.speed, 1))
                  comp_code = PARAMETER_ERROR;
              }
            } else
              comp_code = CONTEXT_STATE_ERROR;

            // if all were good, go ahead and update our contexts
            if (comp_code == TRB_SUCCESS) {
              for (i=0; i<32; i++) {
                if (a_flags & (1<<i)) {
                  if (i == 0) {
                    copy_slot_from_buffer(&slot_context, &buffer[CONTEXT_SIZE]);
                    // only the Interrupter Target and Max Exit Latency are updated by this command
                    BX_XHCI_THIS hub.slots[slot].slot_context.int_target = slot_context.int_target;
                    BX_XHCI_THIS hub.slots[slot].slot_context.max_exit_latency = slot_context.max_exit_latency;
                    // update the DCBAAP slot
                    update_slot_context(slot);
                  } else { // is an ep
                    // See section 4.8.2 for what fields will be updated
                    copy_ep_from_buffer(&ep_context, &buffer[CONTEXT_SIZE + (i * CONTEXT_SIZE)]);
                    // All types get these four updated
                    BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.ep_type = ep_context.ep_type;
                    //BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.tr_dequeue_pointer = ep_context.tr_dequeue_pointer;
                    BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.dcs = 1;

                    // the Control EP0 endpoint's MPS field can not be modified by system software.
                    // it should remain the mps for the device attached.
                    if (i == 1) {
                      int port_num = BX_XHCI_THIS hub.slots[slot].slot_context.rh_port_num - 1;
                      switch (BX_XHCI_THIS hub.usb_port[port_num].device->get_speed()) {
                        case USB_SPEED_LOW:
                          BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.max_packet_size = 8;
                          break;
                        case USB_SPEED_FULL:
                        case USB_SPEED_HIGH:
                          BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.max_packet_size = 64;
                          break;
                        case USB_SPEED_SUPER:
                          BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.max_packet_size = 512;
                          break;
                      }
                    } else
                      BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.max_packet_size = ep_context.max_packet_size;

                    switch (ep_context.ep_type) {
                      case 4: // control
                        BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.cerr = ep_context.cerr;
                        break;
                      case 2:  // Bulk out
                      case 6:  // Bulk in
                        BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.max_burst_size = ep_context.max_burst_size;
                        BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.cerr = ep_context.cerr;
                        BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.max_pstreams = 0; // we don't support streams yet
                        break;
                      default:  // ISO or interrupt
                        BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.max_burst_size = ep_context.max_burst_size;
                        BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.mult = ep_context.mult;
                        BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.cerr = 3;
                        BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.max_pstreams = 0; // we don't support streams yet
                        break;
                    }
                    // update our internal enqueue pointer
                    //BX_XHCI_THIS hub.slots[slot].ep_context[i].enqueue_pointer = ep_context.tr_dequeue_pointer;
                    BX_XHCI_THIS hub.slots[slot].ep_context[i].rcs = ep_context.dcs;

                    // update the DCBAAP slot's ep
                    update_ep_context(slot, i);
                  }
                }
              }
            }
          } else
            comp_code = SLOT_NOT_ENABLED;

          write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
          BX_INFO(("0x" FORMATADDRESS ": Command Ring: Evaluate TRB (slot = %i) (a_flags = 0x%08X) (returning %i)", 
            (bx_phy_address) org_addr, slot, a_flags, comp_code));
        }
        break;

      case CONFIG_EP: {
        slot = TRB_GET_SLOT(trb.command);  // slots are 1 based
        bx_bool dc = TRB_DC(trb.command);
        if (BX_XHCI_THIS hub.slots[slot].enabled) {
          DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) trb.parameter, (CONTEXT_SIZE + (CONTEXT_SIZE * 32)), buffer);
          ReadHostDWordFromLittleEndian(&buffer[0], d_flags);
          ReadHostDWordFromLittleEndian(&buffer[4], a_flags);
          copy_slot_from_buffer(&slot_context, &buffer[CONTEXT_SIZE]);  // so we get entry_count

          if (BX_XHCI_THIS hub.slots[slot].slot_context.slot_state == SLOT_STATE_CONFIGURED) {
            for (i=2; i<32; i++) {
              if (dc || (d_flags & (1<<i))) {
                BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.ep_state = EP_STATE_DISABLED;
                // TODO: Bandwidth stuff
                update_ep_context(slot, i);
              }
            }
            if (dc) {
              BX_XHCI_THIS hub.slots[slot].slot_context.entries = 1;
              update_slot_context(slot);
            }
          }

          if (!dc && (BX_XHCI_THIS hub.slots[slot].slot_context.slot_state >= SLOT_STATE_ADRESSED)) {
            comp_code = TRB_SUCCESS;  // assume good completion
            // Check all the input context entries with an a_flag == 1
            for (i=2; i<32; i++) {
              if (a_flags & (1<<i)) {
                copy_ep_from_buffer(&ep_context, &buffer[CONTEXT_SIZE + (CONTEXT_SIZE * i)]);
                if ((i > (int) slot_context.entries) ||
                     !validate_ep_context(&ep_context, BX_XHCI_THIS hub.slots[slot].slot_context.speed, i)) {
                  comp_code = PARAMETER_ERROR;
                  break;  // no need to check the rest
                }
              }
            }

            // if all were good, go ahead and update our contexts
            if (comp_code == TRB_SUCCESS) {
              for (i=2; i<32; i++) {
                if (d_flags & (1<<i)) {
                  BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.ep_state = EP_STATE_DISABLED;
                  // TODO: Bandwidth stuff
                  update_ep_context(slot, i);
                }
                if (a_flags & (1<<i)) {
                  copy_ep_from_buffer(&BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context, &buffer[CONTEXT_SIZE + (i * CONTEXT_SIZE)]);
                  BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.ep_state = EP_STATE_RUNNING;
                  // initialize our internal enqueue pointer
                  BX_XHCI_THIS hub.slots[slot].ep_context[i].enqueue_pointer = 
                    BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.tr_dequeue_pointer;
                  BX_XHCI_THIS hub.slots[slot].ep_context[i].rcs = BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.dcs;
                  BX_XHCI_THIS hub.slots[slot].ep_context[i].edtla = 0;
                  // TODO: Bandwidth stuff
                  update_ep_context(slot, i);
                }
              }

              // if all EP's are in the disabled state, then set to slot_state = ADDRESSED, else slot_state = CONFIGURED
              BX_XHCI_THIS hub.slots[slot].slot_context.slot_state = SLOT_STATE_ADRESSED; // assume all disabled
              for (i=2; i<32; i++) {
                if (BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.ep_state > EP_STATE_DISABLED) {
                  BX_XHCI_THIS hub.slots[slot].slot_context.slot_state = SLOT_STATE_CONFIGURED;
                  break;
                }
              }
              BX_XHCI_THIS hub.slots[slot].slot_context.entries = slot_context.entries;  ///////
              update_slot_context(slot);
            }
          } else
            comp_code = CONTEXT_STATE_ERROR;
        } else
          comp_code = SLOT_NOT_ENABLED;

        // TODO: Page 101 (change to context entries)

        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        BX_INFO(("0x" FORMATADDRESS ": Command Ring: Found Config_EP TRB (slot = %i) (returning %i)", 
          (bx_phy_address) org_addr, slot, comp_code));
      }
      break;

      case SET_TR_DEQUEUE:
        slot = TRB_GET_SLOT(trb.command);  // slots are 1 based
        ep = TRB_GET_EP(trb.command);
        if (BX_XHCI_THIS hub.slots[slot].enabled == 1) {
          if ((BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_STOPPED) ||
              (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_ERROR)) {
            BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.tr_dequeue_pointer = 
              BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer = (trb.parameter & (Bit64u) ~0xF);
            BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.dcs =
              BX_XHCI_THIS hub.slots[slot].ep_context[ep].rcs = (bx_bool) (trb.parameter & 1);
            BX_XHCI_THIS hub.slots[slot].ep_context[ep].edtla = 0;
            update_ep_context(slot, ep);
            comp_code = TRB_SUCCESS;
          } else
            comp_code = CONTEXT_STATE_ERROR;
        } else
          comp_code = SLOT_NOT_ENABLED;

        // if the state is in error, move it to stopped
        if (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_ERROR)
          BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state = EP_STATE_STOPPED;

        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        BX_INFO(("0x" FORMATADDRESS ": Command Ring: Found Set_tr_Dequeue TRB (slot = %i) (ep = %i) (returning %i)", 
          (bx_phy_address) org_addr, slot, ep, comp_code));
        if (comp_code == TRB_SUCCESS)
          BX_INFO(("  New address: 0x" FORMATADDRESS " state = %i", (bx_phy_address) BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer, 
            BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.dcs));
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
        BX_INFO(("0x" FORMATADDRESS ": Command Ring: Found Stop EP TRB (slot = %i) (ep = %i) (sp = %i) (returning 1)", 
          (bx_phy_address) org_addr, slot, ep, ((trb.command & (1<<23)) == (1<<23))));
        break;

      // Get Port Bandwidth (only available on version 0.96 or 0.95.)
      case GET_PORT_BAND:
        {
          unsigned hub_id = TRB_GET_SLOT(trb.command);
          unsigned band_speed = ((trb.command & (0x0F << 16)) >> 16) - 1;
          if (hub_id == 0) { // root hub
            if (band_speed < 4) {
              DEV_MEM_WRITE_PHYSICAL_DMA((bx_phy_address) trb.parameter, 1 + USB_XHCI_PORTS, port_band_width[band_speed]);
              comp_code = TRB_SUCCESS;
            } else {
              comp_code = TRB_ERROR;
              BX_ERROR(("Get Port Bandwidth with unknown speed of %i", band_speed + 1));
            }
          } else {
#if SEC_DOMAIN_BAND
            // TODO: External HUB support
            comp_code = TRB_ERROR;
            BX_ERROR(("Get Secondary Port Bandwidth not implemented yet."));
#else
        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(TRB_ERROR), TRB_SET_TYPE(COMMAND_COMPLETION), 1);
#endif
          }

          write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_TYPE(COMMAND_COMPLETION), 1);
          BX_INFO(("0x" FORMATADDRESS ": Command Ring: GetPortBandwidth TRB (speed = %i) (hub_id = %i) (returning %i)", 
            (bx_phy_address) org_addr, band_speed, hub_id, comp_code));
        }
        break;

      // unknown TRB type
      default:
        BX_ERROR(("0x" FORMATADDRESS ": Command Ring: Unknown TRB found.", (bx_phy_address) org_addr));
        BX_ERROR(("Unknown trb type found: %i(dec)  (0x" FMT_ADDRX64 " 0x%08X 0x%08X)", TRB_GET_TYPE(trb.command), 
          trb.parameter, trb.status, trb.command));
        write_event_TRB(0, 0x0, TRB_SET_COMP_CODE(TRB_ERROR), TRB_SET_TYPE(COMMAND_COMPLETION), 1);
    }

    // advance the Dequeue pointer and continue;
    BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer += 16;
    read_TRB((bx_phy_address) BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer, &trb);
  }
}

void bx_usb_xhci_c::init_event_ring(const unsigned interrupter)
{
  const Bit64u addr = BX_XHCI_THIS hub.runtime_regs.interrupter[interrupter].erstba.erstabadd;
  Bit8u entry[16];
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
  BX_DEBUG(("Interrupter %02i: Event Ring Table (at 0x" FMT_ADDRX64 ") has %i entries:", interrupter, 
    addr, BX_XHCI_THIS hub.runtime_regs.interrupter[interrupter].erstsz.erstabsize));
  for (int i=0; i<BX_XHCI_THIS hub.runtime_regs.interrupter[interrupter].erstsz.erstabsize; i++) {
    DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) addr + (i * 16), 16, entry);
    ReadHostQWordFromLittleEndian(&entry[0], val64);
    ReadHostDWordFromLittleEndian(&entry[8], val32);
    BX_DEBUG((" %02i:  address = 0x" FMT_ADDRX64 "  Count = %i", i, val64, val32 & 0x0000FFFF));
  }
}

void bx_usb_xhci_c::write_event_TRB(const unsigned interrupter, const Bit64u parameter, const Bit32u status, 
                                    const Bit32u command, const bx_bool fire_int)
{
  // write the TRB
  write_TRB((bx_phy_address) BX_XHCI_THIS hub.ring_members.event_rings[interrupter].cur_trb, parameter, status, 
    command | BX_XHCI_THIS hub.ring_members.event_rings[interrupter].rcs); // set the cycle bit

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
  Bit8u buffer[16];

  DEV_MEM_READ_PHYSICAL_DMA(addr, 16, buffer);
  ReadHostQWordFromLittleEndian(&buffer[0], trb->parameter);
  ReadHostDWordFromLittleEndian(&buffer[8], trb->status);
  ReadHostDWordFromLittleEndian(&buffer[12], trb->command);
}

void bx_usb_xhci_c::write_TRB(bx_phy_address addr, const Bit64u parameter, const Bit32u status, const Bit32u command)
{
  Bit8u buffer[16];

  WriteHostQWordToLittleEndian(&buffer[0], parameter);
  WriteHostDWordToLittleEndian(&buffer[8], status);
  WriteHostDWordToLittleEndian(&buffer[12], command);
  DEV_MEM_WRITE_PHYSICAL_DMA(addr, 16, buffer);
}

void bx_usb_xhci_c::update_slot_context(const int slot)
{
  Bit8u buffer[64];

  memset(buffer, 0, 64);
  copy_slot_to_buffer(buffer, slot);
  Bit64u slot_addr = (BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap + (slot * sizeof(Bit64u)));
  DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) slot_addr, sizeof(Bit64u), (Bit8u *) &slot_addr);
  DEV_MEM_WRITE_PHYSICAL_DMA((bx_phy_address) slot_addr, CONTEXT_SIZE, buffer);
}

void bx_usb_xhci_c::update_ep_context(const int slot, const int ep)
{
  Bit8u buffer[64];

  memset(buffer, 0, 64);
  copy_ep_to_buffer(buffer, slot, ep);
  Bit64u slot_addr = (BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap + (slot * sizeof(Bit64u)));
  DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) slot_addr, sizeof(Bit64u), (Bit8u *) &slot_addr);
  DEV_MEM_WRITE_PHYSICAL_DMA((bx_phy_address) (slot_addr + (ep * CONTEXT_SIZE)), CONTEXT_SIZE, buffer);
}

void bx_usb_xhci_c::dump_slot_context(const Bit32u *context, const int slot)
{
  BX_INFO((" -=-=-=-=-=-=-=- Slot Context -=-=-=-=-=-=-=-"));
  BX_INFO((" Context Entries: %i (%i)", (context[0] & (0x1F<<27)) >> 27, BX_XHCI_THIS hub.slots[slot].slot_context.entries));
  BX_INFO(("             Hub: %i (%i)", (context[0] & (1   <<26)) >> 26, BX_XHCI_THIS hub.slots[slot].slot_context.hub));
  BX_INFO(("             MTT: %i (%i)", (context[0] & (1   <<25)) >> 25, BX_XHCI_THIS hub.slots[slot].slot_context.mtt));
  BX_INFO(("       ReservedZ: %02X",    (context[0] & (1   <<24)) >> 24));
  BX_INFO(("           Speed: %i (%i)", (context[0] & (0x0F<<20)) >> 20, BX_XHCI_THIS hub.slots[slot].slot_context.speed));
  BX_INFO(("    Route String: %05X (%05X)", (context[0] & 0xFFFFF)    >>  0, BX_XHCI_THIS hub.slots[slot].slot_context.route_string));
  BX_INFO(("       Num Ports: %i (%i)", (context[1] & (0xFF<<24)) >> 24, BX_XHCI_THIS hub.slots[slot].slot_context.num_ports));
  BX_INFO(("     RH Port Num: %i (%i)", (context[1] & (0xFF<<16)) >> 16, BX_XHCI_THIS hub.slots[slot].slot_context.rh_port_num));
  BX_INFO(("Max Exit Latency: %i (%i)", (context[1] & 0xFFFF)     >>  0, BX_XHCI_THIS hub.slots[slot].slot_context.max_exit_latency));
  BX_INFO(("      Int Target: %i (%i)", (context[2] & (0x3F<<22)) >> 22, BX_XHCI_THIS hub.slots[slot].slot_context.int_target));
  BX_INFO(("       ReservedZ: %02X",    (context[2] & (0x0F<<18)) >> 18));
  BX_INFO(("             TTT: %i (%i)", (context[2] & (0x03<<16)) >> 16, BX_XHCI_THIS hub.slots[slot].slot_context.ttt));
  BX_INFO(("     TT Port Num: %i (%i)", (context[2] & (0xFF<< 8)) >>  8, BX_XHCI_THIS hub.slots[slot].slot_context.tt_port_num));
  BX_INFO(("     TT Hub Slot: %i (%i)", (context[2] & 0xFF)       >>  0, BX_XHCI_THIS hub.slots[slot].slot_context.tt_hub_slot_id));
  BX_INFO(("      Slot State: %i (%i)", (context[3] & (0x1F<<27)) >> 27, BX_XHCI_THIS hub.slots[slot].slot_context.slot_state));
  BX_INFO(("       ReservedZ: %06X",    (context[3] & (0x7FFFF<<8)) >> 8));
  BX_INFO(("     Dev Address: %i (%i)", (context[3] & 0xFF)       >>  0, BX_XHCI_THIS hub.slots[slot].slot_context.device_address));
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
  BX_INFO(("        Interval: %i (%i)", (context[0] & (0x0F<<20)) >> 20, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.interval));
  BX_INFO(("             LSA: %i (%i)", (context[0] & (1   <<15)) >> 15, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.lsa));
  BX_INFO(("     MaxPStreams: %i (%i)", (context[0] & (0x1F<<10)) >> 10, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_pstreams));
  BX_INFO(("            Mult: %i (%i)", (context[0] & (0x03<< 8)) >>  8, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.mult));
  BX_INFO(("       ReservedZ: %02x",    (context[0] & (0x1F<< 3)) >>  3));
  BX_INFO(("        EP State: %i (%i)", (context[0] & 0x7)        >>  0, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state));
  BX_INFO((" Max Packet Size: %i (%i)", (context[1] & (0xFFFF<<16)) >> 16, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_packet_size));
  BX_INFO(("  Max Burst Size: %i (%i)", (context[1] & (0xFF<< 8)) >>  8, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_burst_size));
  BX_INFO(("             HID: %i (%i)", (context[1] & (1   << 7)) >>  7, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.hid));
  BX_INFO(("       ReservedZ: %01x",    (context[1] & (1   << 6)) >>  6));
  BX_INFO(("         EP Type: %i (%i)", (context[1] & (0x07<< 3)) >>  3, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_type));
  BX_INFO(("            CErr: %i (%i)", (context[1] & (0x03<< 1)) >>  1, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.cerr));
  BX_INFO(("       ReservedZ: %01x",    (context[1] & (1   << 0)) >>  0));
  BX_INFO(("  TR Dequeue Ptr: " FMT_ADDRX64 " (" FMT_ADDRX64 ")", (*(Bit64u *) &context[2] & (Bit64u) ~0x0F), BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.tr_dequeue_pointer));
  BX_INFO(("       ReservedZ: %01x",    (context[2] & (0x07  << 1)) >>  1));
  BX_INFO(("             DCS: %i (%i)", (context[2] & (1     << 0)) >>  0, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.dcs));
  BX_INFO(("Avg ESIT Payload: %i (%i)", (context[4] & (0xFFFF<<16)) >> 16, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_esit_payload));
  BX_INFO(("  Avg TRB Length: %i (%i)", (context[4] & (0xFFFF<< 0)) >>  0, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.average_trb_len));
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

void bx_usb_xhci_c::copy_slot_from_buffer(struct SLOT_CONTEXT *slot_context, const Bit8u *buffer)
{
  slot_context->entries =          (*(Bit32u *) &buffer[0]) >> 27;
  slot_context->hub =             ((*(Bit32u *) &buffer[0]) & (1<<26)) ? 1 : 0;
  slot_context->mtt =             ((*(Bit32u *) &buffer[0]) & (1<<25)) ? 1 : 0;
  slot_context->speed =           ((*(Bit32u *) &buffer[0]) & (0x0F<<20)) >> 20;
  slot_context->route_string =    ((*(Bit32u *) &buffer[0]) & 0x0007FFFF);
  slot_context->num_ports =        (*(Bit32u *) &buffer[4]) >> 24;
  slot_context->rh_port_num =     ((*(Bit32u *) &buffer[4]) & (0xFF<<16)) >> 16;
  slot_context->max_exit_latency = (*(Bit32u *) &buffer[4]) & 0xFFFF;
  slot_context->int_target =       (*(Bit32u *) &buffer[8]) >> 22;
  slot_context->ttt =             ((*(Bit32u *) &buffer[8]) & (0x3<<16)) >> 16;
  slot_context->tt_port_num =     ((*(Bit32u *) &buffer[8]) & (0xFF<<8)) >> 8;
  slot_context->tt_hub_slot_id =   (*(Bit32u *) &buffer[8]) & 0xFF;
  slot_context->slot_state =       (*(Bit32u *) &buffer[12]) >> 27;
  slot_context->device_address =   (*(Bit32u *) &buffer[12]) & 0xFF;
}

void bx_usb_xhci_c::copy_ep_from_buffer(struct EP_CONTEXT *ep_context, const Bit8u *buffer)
{
  ep_context->interval =           ((*(Bit32u *) &buffer[0]) & (0xFF<<16)) >> 16;
  ep_context->lsa =                ((*(Bit32u *) &buffer[0]) & (1<<15)) ? 1 : 0;
  ep_context->max_pstreams =       ((*(Bit32u *) &buffer[0]) & (0x1F<<10)) >> 10;
  ep_context->mult =               ((*(Bit32u *) &buffer[0]) & (0x03<<8)) >> 8;
  ep_context->ep_state =            (*(Bit32u *) &buffer[0]) & 0x07;
  ep_context->max_packet_size =     (*(Bit32u *) &buffer[4]) >> 16;
  ep_context->max_burst_size =     ((*(Bit32u *) &buffer[4]) & (0xFF<<8)) >> 8;
  ep_context->hid =                ((*(Bit32u *) &buffer[4]) & (1<<7)) ? 1 : 0;
  ep_context->ep_type =            ((*(Bit32u *) &buffer[4]) & (0x07<<3)) >> 3;
  ep_context->cerr =               ((*(Bit32u *) &buffer[4]) & (0x03<<1)) >> 1;
  ep_context->tr_dequeue_pointer =  (*(Bit64u *) &buffer[8]) & (Bit64u) ~0xF;
  ep_context->dcs =                 (*(Bit64u *) &buffer[8]) & (1<<0);
  ep_context->max_esit_payload =    (*(Bit32u *) &buffer[16]) >> 16;
  ep_context->average_trb_len =     (*(Bit32u *) &buffer[16]) & 0xFFFF;
}

void bx_usb_xhci_c::copy_slot_to_buffer(const Bit8u *buffer, const int slot)
{
  (*(Bit32u *) &buffer[0]) =  (BX_XHCI_THIS hub.slots[slot].slot_context.entries << 27) |
                              (BX_XHCI_THIS hub.slots[slot].slot_context.hub << 26) |
                              (BX_XHCI_THIS hub.slots[slot].slot_context.mtt << 25) |
                              (BX_XHCI_THIS hub.slots[slot].slot_context.speed << 20) |
                               BX_XHCI_THIS hub.slots[slot].slot_context.route_string;
  (*(Bit32u *) &buffer[4]) =  (BX_XHCI_THIS hub.slots[slot].slot_context.num_ports << 24) |
                              (BX_XHCI_THIS hub.slots[slot].slot_context.rh_port_num << 16) |
                               BX_XHCI_THIS hub.slots[slot].slot_context.max_exit_latency;
  (*(Bit32u *) &buffer[8]) =  (BX_XHCI_THIS hub.slots[slot].slot_context.int_target << 22) |
                              (BX_XHCI_THIS hub.slots[slot].slot_context.ttt << 16) |
                              (BX_XHCI_THIS hub.slots[slot].slot_context.tt_port_num << 8) |
                               BX_XHCI_THIS hub.slots[slot].slot_context.tt_hub_slot_id;
  (*(Bit32u *) &buffer[12]) = (BX_XHCI_THIS hub.slots[slot].slot_context.slot_state << 27) |
                               BX_XHCI_THIS hub.slots[slot].slot_context.device_address;
}

void bx_usb_xhci_c::copy_ep_to_buffer(const Bit8u *buffer, const int slot, const int ep)
{
  (*(Bit32u *) &buffer[0]) =  (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.interval << 16) |
                              (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.lsa << 15) |
                              (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_pstreams << 10) |
                              (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.mult << 8) |
                               BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state;
  (*(Bit32u *) &buffer[4]) =  (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_packet_size << 16) |
                              (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_burst_size << 8) |
                              (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.hid << 7) |
                              (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_type << 3) |
                              (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.cerr << 1);
  (*(Bit64u *) &buffer[8]) =   BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.tr_dequeue_pointer |
                               BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.dcs;
  (*(Bit32u *) &buffer[16]) = (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_esit_payload << 16) |
                               BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.average_trb_len;
}

bx_bool bx_usb_xhci_c::validate_slot_context(const struct SLOT_CONTEXT *slot_context)
{
  // specs:6.2.2.3:   Only checks the Interrupter Target and Max Latency fields for validity
  //  See also 4.6.7
  // only the Interrupter Target and Max Exit Latency are evaluated

  BX_INFO(("   slot_context->int_target = %i, slot_context->max_exit_latency = %i", slot_context->int_target, slot_context->max_exit_latency));

  return 1;
}

bx_bool bx_usb_xhci_c::validate_ep_context(const struct EP_CONTEXT *ep_context, int speed, int ep_num)
{
  // Only the Max_packet Size is evaluated (for an evaluate ep command) ???
  // max_packet_size is assumed to be a multiple of 8  

  BX_INFO(("   ep_num = %i, speed = %i, ep_context->max_packet_size = %i", ep_num, speed, ep_context->max_packet_size));

  // if speed == -1, don't check the speed
  if ((ep_num == 1) && (speed != -1)) {
    return (
      ((speed == SPEED_LOW) && (ep_context->max_packet_size == 8)) ||
      ((speed == SPEED_FULL) && (ep_context->max_packet_size <= 64)) ||  // this may need to retrieve the mps from usb_common.c
      ((speed == SPEED_HI) && (ep_context->max_packet_size == 64)) ||
      ((speed == SPEED_SUPER) && (ep_context->max_packet_size == 512))
      );
  } else
    return 1;
}

// The Specs say that the address is only unique to the RH Port Number
//  For now, we simply return the slot number (+1);
int bx_usb_xhci_c::create_unique_address(const int slot)
{
  return (slot + 1);  // Windows may need the first one to be 2 (though it shouldn't know the difference for xHCI)
}

int bx_usb_xhci_c::send_set_address(const int addr, const int port_num)
{
  int ret;
  USBPacket packet;
  static Bit8u setup_address[8] = { 0, 0x05, 0, 0, 0, 0, 0 };

  WriteHostWordToLittleEndian(&setup_address[2], addr);

  packet.pid = USB_TOKEN_SETUP;
  packet.devep = 0;
  packet.devaddr = 0;  // default address
  packet.len = 8;
  packet.data = setup_address;
  ret = BX_XHCI_THIS broadcast_packet(&packet, port_num);
  if (ret == 0) {
    packet.pid = USB_TOKEN_IN;
    packet.len = 0;
    ret = BX_XHCI_THIS broadcast_packet(&packet, port_num);
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

/*
void bx_usb_xhci_c::usb_frame_handler(void *this_ptr)
{
  bx_usb_xhci_c *class_ptr = (bx_usb_xhci_c *) this_ptr;
  class_ptr->usb_frame_timer();
}

// Called once every ????
void bx_usb_xhci_c::usb_frame_timer(void)
{
  // Nothing for now
}
*/

void bx_usb_xhci_c::runtime_config_handler(void *this_ptr)
{
  bx_usb_xhci_c *class_ptr = (bx_usb_xhci_c *) this_ptr;
  class_ptr->runtime_config();
}

void bx_usb_xhci_c::runtime_config(void)
{
  int i;
  char pname[6];

  for (i = 0; i < BX_N_USB_XHCI_PORTS; i++) {
    // device change support
    if ((BX_XHCI_THIS hub.device_change & (1 << i)) != 0) {
      BX_INFO(("USB port #%d: device connect", i+1));
      sprintf(pname, "port%d", i + 1);
      init_device(i, (bx_list_c*)SIM->get_param(pname, SIM->get_param(BXPN_USB_XHCI)));
      BX_XHCI_THIS hub.device_change &= ~(1 << i);
    }
    // forward to connected device
    if (BX_XHCI_THIS hub.usb_port[i].device != NULL) {
      BX_XHCI_THIS hub.usb_port[i].device->runtime_config();
    }
  }
}

// pci configuration space read callback handler
Bit32u bx_usb_xhci_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_XHCI_THIS pci_conf[address+i] << (i*8));
  }

  if (io_len == 1)
    BX_DEBUG(("read  PCI register 0x%02X value 0x%02X (len=1)", address, value));
  else if (io_len == 2)
    BX_DEBUG(("read  PCI register 0x%02X value 0x%04X (len=2)", address, value));
  else if (io_len == 4)
    BX_DEBUG(("read  PCI register 0x%02X value 0x%08X (len=4)", address, value));

  return value;
}

// pci configuration space write callback handler
void bx_usb_xhci_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  bx_bool baseaddr_change = 0;

  if (((address >= 0x14) && (address <= 0x34)))
    return;

  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
    oldval = BX_XHCI_THIS pci_conf[address+i];
    switch (address+i) {
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
      case 0x3c:
        if (value8 != oldval) {
          BX_INFO(("new irq line = %d", value8));
          BX_XHCI_THIS pci_conf[address+i] = value8;
        }
        break;
      case 0x10:  // low 12 bits of BAR are R/O
        value8 = 0x00;
      case 0x11:  // low 12 bits of BAR are R/O
        value8 &= 0xF0;
      case 0x12:
      case 0x13:
        baseaddr_change |= (value8 != oldval);
        BX_XHCI_THIS pci_conf[address+i] = value8;
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
  if (baseaddr_change) {
    if (DEV_pci_set_base_mem(BX_XHCI_THIS_PTR, read_handler, write_handler,
                             &BX_XHCI_THIS pci_base_address[0],
                             &BX_XHCI_THIS pci_conf[0x10],
                             IO_SPACE_SIZE)) {
      BX_INFO(("new base address: 0x%04X", BX_XHCI_THIS pci_base_address[0]));
    }
  }

  if (io_len == 1)
    BX_DEBUG(("write PCI register 0x%02X value 0x%02X (len=1)", address, value));
  else if (io_len == 2)
    BX_DEBUG(("write PCI register 0x%02X value 0x%04X (len=2)", address, value));
  else if (io_len == 4)
    BX_DEBUG(("write PCI register 0x%02X value 0x%08X (len=4)", address, value));
}

void bx_usb_xhci_c::usb_set_connect_status(Bit8u port, int type, bx_bool connected)
{
  const bx_bool ccs_org = BX_XHCI_THIS hub.usb_port[port].portsc.ccs;
  const bx_bool ped_org = BX_XHCI_THIS hub.usb_port[port].portsc.ped;

  usb_device_c *device = BX_XHCI_THIS hub.usb_port[port].device;
  if (device != NULL) {
    if (device->get_type() == type) {
      if (connected) {
        if ((device->get_speed() == USB_SPEED_SUPER) &&
            !BX_XHCI_THIS hub.usb_port[port].is_usb3) {
          BX_PANIC(("Super-speed device not supported on USB2 port."));
        }
        if ((device->get_speed() != USB_SPEED_SUPER) &&
            BX_XHCI_THIS hub.usb_port[port].is_usb3) {
          BX_PANIC(("Only super-speed devices supported on USB3 port."));
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
            BX_ERROR(("device->get_speed() returned invalid speed value"));
        }
        BX_XHCI_THIS hub.usb_port[port].portsc.ccs = 1;
        if (!device->get_connected()) {
          if (!device->init()) {
            usb_set_connect_status(port, type, 0);
            BX_ERROR(("port #%d: connect failed", port+1));
          } else {
            BX_INFO(("port #%d: connect: %s", port+1, device->get_info()));
          }
        }
      } else { // not connected
        BX_XHCI_THIS hub.usb_port[port].portsc.ccs = 0;
        BX_XHCI_THIS hub.usb_port[port].portsc.ped = 0;
        BX_XHCI_THIS hub.usb_port[port].portsc.speed = 0;
        remove_device(port);
      }
    }
    if (ccs_org != BX_XHCI_THIS hub.usb_port[port].portsc.ccs)
      BX_XHCI_THIS hub.usb_port[port].portsc.csc = 1;
    if (ped_org != BX_XHCI_THIS hub.usb_port[port].portsc.ped)
      BX_XHCI_THIS hub.usb_port[port].portsc.pec = 1;

    // we changed the value of the port, so show it
    BX_INFO(("Port Status Change Event."));
    write_event_TRB(0, ((port + 1) << 24), TRB_SET_COMP_CODE(1), TRB_SET_TYPE(PORT_STATUS_CHANGE), 1);
  }
}

// USB runtime parameter handler
const char *bx_usb_xhci_c::usb_param_handler(bx_param_string_c *param, int set,
                                           const char *oldval, const char *val, int maxlen)
{
  usbdev_type type = USB_DEV_TYPE_NONE;
  int portnum;

  if (set) {
    portnum = atoi((param->get_parent())->get_name()+4) - 1;
    bx_bool empty = ((strlen(val) == 0) || (!strcmp(val, "none")));
    if ((portnum >= 0) && (portnum < USB_XHCI_PORTS)) {
      if (empty && BX_XHCI_THIS hub.usb_port[portnum].portsc.ccs) {
        BX_INFO(("USB port #%d: device disconnect", portnum+1));
        if (BX_XHCI_THIS hub.usb_port[portnum].device != NULL) {
          type = BX_XHCI_THIS hub.usb_port[portnum].device->get_type();
        }
        usb_set_connect_status(portnum, type, 0);
      } else if (!empty && !BX_XHCI_THIS hub.usb_port[portnum].portsc.ccs) {
        BX_XHCI_THIS hub.device_change |= (1 << portnum);
      }
    } else {
      BX_PANIC(("usb_param_handler called with unexpected parameter '%s'", param->get_name()));
    }
  }
  return val;
}

void bx_usb_xhci_c::dump_xhci_core(const int slots, const int eps)
{
  bx_phy_address addr = BX_XHCI_THIS pci_base_address[0];
  Bit32u dword;
  Bit64u qword, slot_addr;
  int p, i;
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

  for (i=0, p=0; i<USB_XHCI_PORTS; i++, p+=16) {
    BX_XHCI_THIS read_handler(addr + 0x420 + (p + 0), 4, &dword, NULL);
    BX_INFO(("    Port %i: 0x%08X", i, dword));
    BX_XHCI_THIS read_handler(addr + 0x420 + (p + 4), 4, &dword, NULL);
    BX_INFO(("            0x%08X", dword));
    BX_XHCI_THIS read_handler(addr + 0x420 + (p + 8), 4, &dword, NULL);
    BX_INFO(("            0x%08X", dword));
    BX_XHCI_THIS read_handler(addr + 0x420 + (p + 12), 4, &dword, NULL);
    BX_INFO(("            0x%08X", dword));
  }

  slot_addr = BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap;
  DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) slot_addr, sizeof(Bit64u), (Bit8u *) &slot_addr);
  BX_INFO((" SCRATCH PADS:  0x" FMT_ADDRX64, slot_addr));
  for (i=1; i<slots+1; i++) {
    slot_addr = (BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap + (i * sizeof(Bit64u)));
    DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) slot_addr, sizeof(Bit64u), (Bit8u *) &slot_addr);
    DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) slot_addr, 2048, buffer);
    dump_slot_context((Bit32u *) &buffer[0], i);
    for (p=1; p<eps+1; p++)
      dump_ep_context((Bit32u *) &buffer[p * CONTEXT_SIZE], i, p);
  }
}
#endif // BX_SUPPORT_PCI && BX_SUPPORT_USB_XHCI
