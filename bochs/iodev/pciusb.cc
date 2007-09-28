/////////////////////////////////////////////////////////////////////////
// $Id: pciusb.cc,v 1.60 2007-09-28 19:52:03 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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

// Experimental PCI USB adapter
// Benjamin D Lunt (fys at frontiernet net) coded most of this usb emulation.

/* Notes:
   - I have coded this to be able to use more than one HUB and each
     device to have more than one function.  *However*, at the moment,
     not all of the code will work with multiple hubs and/or functions.
     I thought of removing this functionallity to make it easier, but
     thought that later when multiple hubs and/or functions were desired,
     it would already be half way there.
   - Currently, this code is quite messy.  This is for all of the debugging
     I have been doing.  Many BX_INFO()'s here and there.
   - My purpose of coding this emulation was/is to learn about the USB.
     It has been a challenge, but I have learned a lot.
   - If I forget, there are a lot of BX_INFO's that can be changed to BX_DEBUG's.
   - 31 July 2006:
     I now have a Beagle USB Protocol Analyzer from Total Phase for my research.
     (http://www.totalphase.com/products/beagle/usb/)
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
#if BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
#include "usb_hid.h"
#include "usb_msd.h"

#define LOG_THIS theUSBDevice->

bx_pciusb_c* theUSBDevice = NULL;

const Bit8u usb_iomask[32] = {2, 1, 2, 1, 2, 1, 2, 0, 4, 0, 0, 0, 1, 0, 0, 0,
                              3, 1, 3, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Dumps the contents of a buffer to the log file
void usb_dump_packet(Bit8u *data, unsigned size)
{
  char the_packet[256], str[16];
  strcpy(the_packet, "Packet contents (in hex):");
  unsigned offset = 0;
  for (unsigned p=0; p<size; p++) {
    if (!(p & 0x0F)) {
      BX_DEBUG(("%s", the_packet));
      sprintf(the_packet, "  0x%04X ", offset);
      offset += 16;
    }
    sprintf(str, " %02X", data[p]);
    strcat(the_packet, str);
  }
  if (strlen(the_packet))
    BX_DEBUG(("%s", the_packet));
}

int set_usb_string(Bit8u *buf, const char *str)
{
  int len, i;
  Bit8u *q;

  q = buf;
  len = strlen(str);
  if (len > 32) {
    *q = 0;
    return 0;
  }
  *q++ = 2 * len + 2;
  *q++ = 3;
  for(i = 0; i < len; i++) {
    *q++ = str[i];
    *q++ = 0;
  }
  return q - buf;
}

int libpciusb_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theUSBDevice = new bx_pciusb_c();
  bx_devices.pluginPciUSBAdapter = theUSBDevice;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theUSBDevice, BX_PLUGIN_PCIUSB);
  return 0; // Success
}

void libpciusb_LTX_plugin_fini(void)
{
  delete theUSBDevice;
}

bx_pciusb_c::bx_pciusb_c()
{
  put("USB");
  settype(PCIUSBLOG);
  for (int i=0; i<BX_USB_CONFDEV; i++) {
    memset((void*)&hub[i], 0, sizeof(bx_usb_t));
  }
  device_buffer = NULL;
}

bx_pciusb_c::~bx_pciusb_c()
{
  if (BX_USB_THIS device_buffer != NULL)
    delete [] BX_USB_THIS device_buffer;

  for (int i=0; i<BX_USB_CONFDEV; i++) {
    for (int j=0; j<USB_NUM_PORTS; j++) {
      if (BX_USB_THIS hub[i].usb_port[j].device != NULL) {
        delete BX_USB_THIS hub[i].usb_port[j].device;
      }
    }
  }

  SIM->get_param_string(BXPN_USB1_PORT1)->set_handler(NULL);
  SIM->get_param_string(BXPN_USB1_PORT2)->set_handler(NULL);

  BX_DEBUG(("Exit"));
}

void bx_pciusb_c::init(void)
{
  // called once when bochs initializes

  if (!SIM->get_param_bool(BXPN_USB1_ENABLED)->get()) return;

  BX_USB_THIS device_buffer = new Bit8u[65536];

  // Call our timer routine every 1mS (1,000uS)
  // Continuous and active
  BX_USB_THIS hub[0].timer_index =
                   bx_pc_system.register_timer(this, usb_timer_handler, 1000, 1,1, "usb.timer");

  BX_USB_THIS hub[0].devfunc = BX_PCI_DEVICE(1,2);
  DEV_register_pci_handlers(this, &BX_USB_THIS hub[0].devfunc, BX_PLUGIN_PCIUSB,
                            "Experimental PCI USB");

  for (unsigned i=0; i<256; i++) {
    BX_USB_THIS hub[0].pci_conf[i] = 0x0;
  }

  BX_USB_THIS hub[0].base_ioaddr = 0x0;

  BX_INFO(("usb1 initialized - I/O base and IRQ assigned by PCI BIOS"));

  //FIXME: for now, we want a status bar // hub zero, port zero
  BX_USB_THIS hub[0].statusbar_id[0] = bx_gui->register_statusitem("USB");

  SIM->get_param_string(BXPN_USB1_PORT1)->set_handler(usb_param_handler);
  SIM->get_param_string(BXPN_USB1_PORT1)->set_runtime_param(1);
  SIM->get_param_string(BXPN_USB1_PORT2)->set_handler(usb_param_handler);
  SIM->get_param_string(BXPN_USB1_PORT2)->set_runtime_param(1);

  //HACK: Turn on debug messages from the start
  //BX_USB_THIS setonoff(LOGLEV_DEBUG, ACT_REPORT);
}

void bx_pciusb_c::reset(unsigned type)
{
  unsigned i, j;

  if (!SIM->get_param_bool(BXPN_USB1_ENABLED)->get()) return;

  if (type == BX_RESET_HARDWARE) {
    static const struct reset_vals_t {
      unsigned      addr;
      unsigned char val;
    } reset_vals[] = {
      { 0x00, 0x86 }, { 0x01, 0x80 }, // 0x8086 = vendor
      { 0x02, 0x20 }, { 0x03, 0x70 }, // 0x7020 = device
      { 0x04, 0x05 }, { 0x05, 0x00 }, // command_io
      { 0x06, 0x80 }, { 0x07, 0x02 }, // status
      { 0x08, 0x01 },                 // revision number
      { 0x09, 0x00 },                 // interface
      { 0x0a, 0x03 },                 // class_sub  USB Host Controller
      { 0x0b, 0x0c },                 // class_base Serial Bus Controller
      { 0x0D, 0x20 },                 // bus latency
      { 0x0e, 0x00 },                 // header_type_generic
      // address space 0x20 - 0x23
      { 0x20, 0x01 }, { 0x21, 0x00 },
      { 0x22, 0x00 }, { 0x23, 0x00 },
      { 0x3c, 0x00 },                 // IRQ
      { 0x3d, BX_PCI_INTD },          // INT
      { 0x60, 0x10 },                 // USB revision 1.0
      { 0x6a, 0x01 },                 // USB clock
      { 0xc1, 0x20 }                  // PIRQ enable

    };
    for (i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); ++i) {
        BX_USB_THIS hub[0].pci_conf[reset_vals[i].addr] = reset_vals[i].val;
    }
  }

  // reset locals
  BX_USB_THIS busy = 0;
  BX_USB_THIS global_reset = 0;

  // Put the USB registers into their RESET state
  for (i=0; i<BX_USB_CONFDEV; i++) {
    BX_USB_THIS hub[i].usb_command.max_packet_size = 0;
    BX_USB_THIS hub[i].usb_command.configured = 0;
    BX_USB_THIS hub[i].usb_command.debug = 0;
    BX_USB_THIS hub[i].usb_command.resume = 0;
    BX_USB_THIS hub[i].usb_command.suspend = 0;
    BX_USB_THIS hub[i].usb_command.reset = 0;
    BX_USB_THIS hub[i].usb_command.host_reset = 0;
    BX_USB_THIS hub[i].usb_command.schedule = 0;
    BX_USB_THIS hub[i].usb_status.error_interrupt = 0;
    BX_USB_THIS hub[i].usb_status.host_error = 0;
    BX_USB_THIS hub[i].usb_status.host_halted = 0;
    BX_USB_THIS hub[i].usb_status.interrupt = 0;
    BX_USB_THIS hub[i].usb_status.pci_error = 0;
    BX_USB_THIS hub[i].usb_status.resume = 0;
    BX_USB_THIS hub[i].usb_enable.short_packet = 0;
    BX_USB_THIS hub[i].usb_enable.on_complete = 0;
    BX_USB_THIS hub[i].usb_enable.resume = 0;
    BX_USB_THIS hub[i].usb_enable.timeout_crc = 0;
    BX_USB_THIS hub[i].usb_frame_num.frame_num = 0x0000;
    BX_USB_THIS hub[i].usb_frame_base.frame_base = 0x00000000;
    BX_USB_THIS hub[i].usb_sof.sof_timing = 0x40;
    for (j=0; j<USB_NUM_PORTS; j++) {
      BX_USB_THIS hub[i].usb_port[j].connect_changed = 0;
      BX_USB_THIS hub[i].usb_port[j].line_dminus = 0;
      BX_USB_THIS hub[i].usb_port[j].line_dplus = 0;
      BX_USB_THIS hub[i].usb_port[j].low_speed = 0;
      BX_USB_THIS hub[i].usb_port[j].reset = 0;
      BX_USB_THIS hub[i].usb_port[j].resume = 0;
      BX_USB_THIS hub[i].usb_port[j].suspend = 0;
      BX_USB_THIS hub[i].usb_port[j].enabled = 0;
      BX_USB_THIS hub[i].usb_port[j].able_changed = 0;
      BX_USB_THIS hub[i].usb_port[j].status = 0;
      if (BX_USB_THIS hub[i].usb_port[j].device != NULL) {
        delete BX_USB_THIS hub[i].usb_port[j].device;
        BX_USB_THIS hub[i].usb_port[j].device = NULL;
      }
    }
  }

  BX_USB_THIS mousedev = NULL;
  BX_USB_THIS keybdev = NULL;

  init_device(0, SIM->get_param_string(BXPN_USB1_PORT1)->getptr());
  init_device(1, SIM->get_param_string(BXPN_USB1_PORT2)->getptr());
}

void bx_pciusb_c::register_state(void)
{
  unsigned i, j;
  char hubnum[8], portnum[8];
  bx_list_c *hub, *usb_cmd, *usb_st, *usb_en, *port;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "pciusb", "PCI USB Controller State", BX_USB_CONFDEV + 2);
  for (i=0; i<BX_USB_CONFDEV; i++) {
    sprintf(hubnum, "hub%d", i+1);
    hub = new bx_list_c(list, hubnum, USB_NUM_PORTS + 7);
    usb_cmd = new bx_list_c(hub, "usb_command", 8);
    new bx_shadow_bool_c(usb_cmd, "max_packet_size", &BX_USB_THIS hub[i].usb_command.max_packet_size);
    new bx_shadow_bool_c(usb_cmd, "configured", &BX_USB_THIS hub[i].usb_command.configured);
    new bx_shadow_bool_c(usb_cmd, "debug", &BX_USB_THIS hub[i].usb_command.debug);
    new bx_shadow_bool_c(usb_cmd, "resume", &BX_USB_THIS hub[i].usb_command.resume);
    new bx_shadow_bool_c(usb_cmd, "suspend", &BX_USB_THIS hub[i].usb_command.suspend);
    new bx_shadow_bool_c(usb_cmd, "reset", &BX_USB_THIS hub[i].usb_command.reset);
    new bx_shadow_bool_c(usb_cmd, "host_reset", &BX_USB_THIS hub[i].usb_command.host_reset);
    new bx_shadow_bool_c(usb_cmd, "schedule", &BX_USB_THIS hub[i].usb_command.schedule);
    usb_st = new bx_list_c(hub, "usb_status", 6);
    new bx_shadow_bool_c(usb_st, "host_halted", &BX_USB_THIS hub[i].usb_status.host_halted);
    new bx_shadow_bool_c(usb_st, "host_error", &BX_USB_THIS hub[i].usb_status.host_error);
    new bx_shadow_bool_c(usb_st, "pci_error", &BX_USB_THIS hub[i].usb_status.pci_error);
    new bx_shadow_bool_c(usb_st, "resume", &BX_USB_THIS hub[i].usb_status.resume);
    new bx_shadow_bool_c(usb_st, "error_interrupt", &BX_USB_THIS hub[i].usb_status.error_interrupt);
    new bx_shadow_bool_c(usb_st, "interrupt", &BX_USB_THIS hub[i].usb_status.interrupt);
    usb_en = new bx_list_c(hub, "usb_enable", 4);
    new bx_shadow_bool_c(usb_en, "short_packet", &BX_USB_THIS hub[i].usb_enable.short_packet);
    new bx_shadow_bool_c(usb_en, "on_complete", &BX_USB_THIS hub[i].usb_enable.on_complete);
    new bx_shadow_bool_c(usb_en, "resume", &BX_USB_THIS hub[i].usb_enable.resume);
    new bx_shadow_bool_c(usb_en, "timeout_crc", &BX_USB_THIS hub[i].usb_enable.timeout_crc);
    new bx_shadow_num_c(hub, "frame_num", &BX_USB_THIS hub[i].usb_frame_num.frame_num, BASE_HEX);
    new bx_shadow_num_c(hub, "frame_base", &BX_USB_THIS hub[i].usb_frame_base.frame_base, BASE_HEX);
    new bx_shadow_num_c(hub, "sof_timing", &BX_USB_THIS hub[i].usb_sof.sof_timing, BASE_HEX);
    for (j=0; j<USB_NUM_PORTS; j++) {
      sprintf(portnum, "port%d", j+1);
      port = new bx_list_c(hub, portnum, 11);
      new bx_shadow_bool_c(port, "suspend", &BX_USB_THIS hub[i].usb_port[j].suspend);
      new bx_shadow_bool_c(port, "reset", &BX_USB_THIS hub[i].usb_port[j].reset);
      new bx_shadow_bool_c(port, "low_speed", &BX_USB_THIS hub[i].usb_port[j].low_speed);
      new bx_shadow_bool_c(port, "resume", &BX_USB_THIS hub[i].usb_port[j].resume);
      new bx_shadow_bool_c(port, "line_dminus", &BX_USB_THIS hub[i].usb_port[j].line_dminus);
      new bx_shadow_bool_c(port, "line_dplus", &BX_USB_THIS hub[i].usb_port[j].line_dplus);
      new bx_shadow_bool_c(port, "able_changed", &BX_USB_THIS hub[i].usb_port[j].able_changed);
      new bx_shadow_bool_c(port, "enabled", &BX_USB_THIS hub[i].usb_port[j].enabled);
      new bx_shadow_bool_c(port, "connect_changed", &BX_USB_THIS hub[i].usb_port[j].connect_changed);
      new bx_shadow_bool_c(port, "status", &BX_USB_THIS hub[i].usb_port[j].status);
      // empty list for USB device state
      new bx_list_c(port, "device", 20);
    }
    register_pci_state(hub, BX_USB_THIS hub[i].pci_conf);
  }
  new bx_shadow_bool_c(list, "busy", &BX_USB_THIS busy);
  new bx_shadow_num_c(list, "global_reset", &BX_USB_THIS global_reset);
}

void bx_pciusb_c::after_restore_state(void)
{
  if (DEV_pci_set_base_io(BX_USB_THIS_PTR, read_handler, write_handler,
                         &BX_USB_THIS hub[0].base_ioaddr,
                         &BX_USB_THIS hub[0].pci_conf[0x20],
                         32, &usb_iomask[0], "USB Hub #1"))
  {
     BX_INFO(("new base address: 0x%04x", BX_USB_THIS hub[0].base_ioaddr));
  }
  for (int i=0; i<BX_USB_CONFDEV; i++) {
    for (int j=0; j<USB_NUM_PORTS; j++) {
      if (BX_USB_THIS hub[i].usb_port[j].device != NULL) {
        BX_USB_THIS hub[i].usb_port[j].device->after_restore_state();
      }
    }
  }
}

void bx_pciusb_c::init_device(Bit8u port, const char *devname)
{
  usbdev_type type = USB_DEV_TYPE_NONE;
  char pname[BX_PATHNAME_LEN];

  if (!strlen(devname) || !strcmp(devname, "none")) return;

  if (!strcmp(devname, "mouse")) {
    type = USB_DEV_TYPE_MOUSE;
    BX_USB_THIS hub[0].usb_port[port].device = new usb_hid_device_t(type);
    if (BX_USB_THIS mousedev == NULL) {
      BX_USB_THIS mousedev = (usb_hid_device_t*)BX_USB_THIS hub[0].usb_port[port].device;
    }
  } else if (!strcmp(devname, "tablet")) {
    type = USB_DEV_TYPE_TABLET;
    BX_USB_THIS hub[0].usb_port[port].device = new usb_hid_device_t(type);
    if (BX_USB_THIS mousedev == NULL) {
      BX_USB_THIS mousedev = (usb_hid_device_t*)BX_USB_THIS hub[0].usb_port[port].device;
    }
  } else if (!strcmp(devname, "keypad")) {
    type = USB_DEV_TYPE_KEYPAD;
    BX_USB_THIS hub[0].usb_port[port].device = new usb_hid_device_t(type);
    if (BX_USB_THIS keybdev == NULL) {
      BX_USB_THIS keybdev = (usb_hid_device_t*)BX_USB_THIS hub[0].usb_port[port].device;
    }
  } else if (!strncmp(devname, "disk:", 5)) {
    type = USB_DEV_TYPE_DISK;
    BX_USB_THIS hub[0].usb_port[port].device = new usb_msd_device_t();
  } else {
    BX_PANIC(("unknown USB device: %s", devname));
    return;
  }
  sprintf(pname, "pciusb.hub1.port%d.device", port+1);
  bx_list_c *devlist = (bx_list_c*)SIM->get_param(pname, SIM->get_bochs_root());
  BX_USB_THIS hub[0].usb_port[port].device->register_state(devlist);
  usb_set_connect_status(port, type, 1);
}

void bx_pciusb_c::set_irq_level(bx_bool level)
{
  DEV_pci_set_irq(BX_USB_THIS hub[0].devfunc, BX_USB_THIS hub[0].pci_conf[0x3d], level);
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_pciusb_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PCIUSB_SMF
  bx_pciusb_c *class_ptr = (bx_pciusb_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_pciusb_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIUSB_SMF
  Bit32u val = 0x0;
  Bit8u  offset,port;

  offset = address - BX_USB_THIS hub[0].base_ioaddr;

  switch (offset) {
    case 0x00: // command register (16-bit)
      val =   BX_USB_THIS hub[0].usb_command.max_packet_size << 7
            | BX_USB_THIS hub[0].usb_command.configured << 6
            | BX_USB_THIS hub[0].usb_command.debug << 5
            | BX_USB_THIS hub[0].usb_command.resume << 4
            | BX_USB_THIS hub[0].usb_command.suspend << 3
            | BX_USB_THIS hub[0].usb_command.reset << 2
            | BX_USB_THIS hub[0].usb_command.host_reset << 1
            | BX_USB_THIS hub[0].usb_command.schedule;
      break;

    case 0x02: // status register (16-bit)
      val = BX_USB_THIS hub[0].usb_status.host_halted << 5
            | BX_USB_THIS hub[0].usb_status.host_error << 4
            | BX_USB_THIS hub[0].usb_status.pci_error << 3
            | BX_USB_THIS hub[0].usb_status.resume << 2
            | BX_USB_THIS hub[0].usb_status.error_interrupt << 1
            | BX_USB_THIS hub[0].usb_status.interrupt;
      break;

    case 0x04: // interrupt enable register (16-bit)
      val = BX_USB_THIS hub[0].usb_enable.short_packet << 3
            | BX_USB_THIS hub[0].usb_enable.on_complete << 2
            | BX_USB_THIS hub[0].usb_enable.resume << 1
            | BX_USB_THIS hub[0].usb_enable.timeout_crc;
      break;

    case 0x06: // frame number register (16-bit)
      val = BX_USB_THIS hub[0].usb_frame_num.frame_num;
      break;

    case 0x08: // frame base register (32-bit)
      val = BX_USB_THIS hub[0].usb_frame_base.frame_base;
      break;

    case 0x0C: // start of Frame Modify register (8-bit)
      val = BX_USB_THIS hub[0].usb_sof.sof_timing;
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
      if (port < USB_NUM_PORTS) {
        val = BX_USB_THIS hub[0].usb_port[port].suspend << 12
              |                                       1 << 10  // some Root Hubs have bit 10 set ?????
              | BX_USB_THIS hub[0].usb_port[port].reset << 9
              | BX_USB_THIS hub[0].usb_port[port].low_speed << 8
              | 1 << 7
              | BX_USB_THIS hub[0].usb_port[port].resume << 6
              | BX_USB_THIS hub[0].usb_port[port].line_dminus << 5
              | BX_USB_THIS hub[0].usb_port[port].line_dplus << 4
              | BX_USB_THIS hub[0].usb_port[port].able_changed << 3
              | BX_USB_THIS hub[0].usb_port[port].enabled << 2
              | BX_USB_THIS hub[0].usb_port[port].connect_changed << 1
              | BX_USB_THIS hub[0].usb_port[port].status;
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

void bx_pciusb_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PCIUSB_SMF
  bx_pciusb_c *class_ptr = (bx_pciusb_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_pciusb_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIUSB_SMF
  Bit8u  offset,port;

  BX_DEBUG(("register write to  address 0x%04X:  0x%08X (%2i bits)", (unsigned) address, (unsigned) value, io_len * 8));

  offset = address - BX_USB_THIS hub[0].base_ioaddr;

  switch (offset) {
    case 0x00: // command register (16-bit) (R/W)
      if (value & 0xFF00)
        BX_DEBUG(("write to command register with bits 15:8 not zero: 0x%04x", value));

      BX_USB_THIS hub[0].usb_command.max_packet_size = (value & 0x80) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.configured = (value & 0x40) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.debug = (value & 0x20) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.resume = (value & 0x10) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.suspend = (value & 0x08) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.reset = (value & 0x04) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.host_reset = (value & 0x02) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.schedule = (value & 0x01) ? 1: 0;

      // HCRESET 
      if (BX_USB_THIS hub[0].usb_command.host_reset) {
        BX_USB_THIS reset(0);
        for (unsigned i=0; i<USB_NUM_PORTS; i++) {
          if (BX_USB_THIS hub[0].usb_port[i].status) {
            if (BX_USB_THIS hub[0].usb_port[i].device != NULL) {
              BX_USB_THIS usb_send_msg(BX_USB_THIS hub[0].usb_port[i].device, USB_MSG_RESET);
            }
          }
          BX_USB_THIS hub[0].usb_port[i].connect_changed = 1;
          BX_USB_THIS hub[0].usb_port[i].enabled = 0;
          BX_USB_THIS hub[0].usb_port[i].able_changed = 1;
        }
      }

      // If software set the GRESET bit, we need to send the reset to all USB.
      // The software should guarentee that the reset is for at least 10ms.
      // We hold the reset until software resets this bit
      if (BX_USB_THIS hub[0].usb_command.reset) {
        BX_USB_THIS global_reset = 1;
        BX_DEBUG(("Global Reset"));
      } else {
        // if software cleared the reset, then we need to reset the usb registers.
        if (BX_USB_THIS global_reset) {
          BX_USB_THIS global_reset = 0;
          unsigned int running = BX_USB_THIS hub[0].usb_command.schedule;
          BX_USB_THIS reset(0);
          BX_USB_THIS hub[0].usb_status.host_halted = (running) ? 1 : 0;
        }
      }

      // If Run/Stop, identify in log
      if (BX_USB_THIS hub[0].usb_command.schedule) {
        BX_USB_THIS hub[0].usb_status.host_halted = 0;
        BX_DEBUG(("Schedule bit set in Command register"));
      } else {
        BX_USB_THIS hub[0].usb_status.host_halted = 1;
        BX_DEBUG(("Schedule bit clear in Command register"));
      }

      // If Debug mode set, panic.  Not implemented
      if (BX_USB_THIS hub[0].usb_command.debug)
        BX_PANIC(("Software set DEBUG bit in Command register. Not implemented"));

      break;

    case 0x02: // status register (16-bit) (R/WC)
      if (value & 0xFFC0)
        BX_DEBUG(("write to status register with bits 15:6 not zero: 0x%04x", value));

      // host_halted, even though not specified in the specs, is read only
      //BX_USB_THIS hub[0].usb_status.host_halted = (value & 0x20) ? 0: BX_USB_THIS hub[0].usb_status.host_halted;
      BX_USB_THIS hub[0].usb_status.host_error = (value & 0x10) ? 0: BX_USB_THIS hub[0].usb_status.host_error;
      BX_USB_THIS hub[0].usb_status.pci_error = (value & 0x08) ? 0: BX_USB_THIS hub[0].usb_status.pci_error;
      BX_USB_THIS hub[0].usb_status.resume = (value & 0x04) ? 0: BX_USB_THIS hub[0].usb_status.resume;
      BX_USB_THIS hub[0].usb_status.error_interrupt = (value & 0x02) ? 0: BX_USB_THIS hub[0].usb_status.error_interrupt;
      BX_USB_THIS hub[0].usb_status.interrupt = (value & 0x01) ? 0: BX_USB_THIS hub[0].usb_status.interrupt;
      break;

    case 0x04: // interrupt enable register (16-bit)
      if (value & 0xFFF0)
        BX_DEBUG(("write to interrupt enable register with bits 15:4 not zero: 0x%04x", value));

      BX_USB_THIS hub[0].usb_enable.short_packet  = (value & 0x08) ? 1: 0;
      BX_USB_THIS hub[0].usb_enable.on_complete  = (value & 0x04) ? 1: 0;
      BX_USB_THIS hub[0].usb_enable.resume  = (value & 0x02) ? 1: 0;
      BX_USB_THIS hub[0].usb_enable.timeout_crc = (value & 0x01) ? 1: 0;

      if (value & 0x08) {
        BX_DEBUG(("Host set Enable Interrupt on Short Packet"));
      }
      if (value & 0x04) {
        BX_DEBUG(("Host set Enable Interrupt on Complete"));
      }
      if (value & 0x02) {
        BX_DEBUG(("Host set Enable Interrupt on Resume"));
      }
      break;

    case 0x06: // frame number register (16-bit)
      if (value & 0xF800)
        BX_DEBUG(("write to frame number register with bits 15:11 not zero: 0x%04x", value));

      if (BX_USB_THIS hub[0].usb_status.host_halted)
        BX_USB_THIS hub[0].usb_frame_num.frame_num = (value & 0x07FF);
      else
        // ignored by the hardward, but lets report it anyway
        BX_DEBUG(("write to frame number register with STATUS.HALTED == 0"));
      break;

    case 0x08: // frame base register (32-bit)
      if (value & 0xFFF)
        BX_PANIC(("write to frame base register with bits 11:0 not zero: 0x%08x", value));

      BX_USB_THIS hub[0].usb_frame_base.frame_base = (value & ~0xfff);
      break;

    case 0x0C: // start of Frame Modify register (8-bit)
      if (value & 0x80)
        BX_DEBUG(("write to SOF Modify register with bit 7 not zero: 0x%04x", value));

       BX_USB_THIS hub[0].usb_sof.sof_timing = value;
       break;

    case 0x14: // port #3 non existant, but linux systems check it to see if there are more than 2
      BX_ERROR(("write to non existant offset 0x14 (port #3)"));
      break;

    case 0x10: // port #1
    case 0x12: // port #2
      port = (offset & 0x0F) >> 1;
      if ((port < USB_NUM_PORTS) && (io_len == 2)) {
        // If the ports reset bit is set, don't allow any writes unless the new write will clear the reset bit
        if (BX_USB_THIS hub[0].usb_port[port].reset & (value & (1<<9)))
          break;
        if (value & ((1<<5) | (1<<4) | (1<<0)))
          BX_DEBUG(("write to one or more read-only bits in port #%d register: 0x%04x", port+1, value));
        if (!(value & (1<<7)))
          BX_DEBUG(("write to port #%d register bit 7 = 0", port+1));
        if (value & (1<<8))
          BX_DEBUG(("write to bit 8 in port #%d register ignored", port+1));
        if ((value & (1<<12)) && BX_USB_THIS hub[0].usb_command.suspend)
          BX_DEBUG(("write to port #%d register bit 12 when in Global-Suspend", port+1));

        BX_USB_THIS hub[0].usb_port[port].suspend = (value & (1<<12)) ? 1 : 0;
        BX_USB_THIS hub[0].usb_port[port].reset = (value & (1<<9)) ? 1 : 0;
        BX_USB_THIS hub[0].usb_port[port].resume = (value & (1<<6)) ? 1 : 0;
        if (!BX_USB_THIS hub[0].usb_port[port].enabled && (value & (1<<2)))
          BX_USB_THIS hub[0].usb_port[port].able_changed = 0;
        else
          BX_USB_THIS hub[0].usb_port[port].able_changed = (value & (1<<3)) ? 0 : BX_USB_THIS hub[0].usb_port[port].able_changed;
        BX_USB_THIS hub[0].usb_port[port].enabled = (value & (1<<2)) ? 1 : 0;
        BX_USB_THIS hub[0].usb_port[port].connect_changed = (value & (1<<1)) ? 0 : BX_USB_THIS hub[0].usb_port[port].connect_changed;

        // if port reset, reset function(s)
        //TODO: only reset items on the downstream...
        // for now, reset the one and only
        // TODO: descriptors, etc....
        if (BX_USB_THIS hub[0].usb_port[port].reset) {
          BX_USB_THIS hub[0].usb_port[port].suspend = 0;
          BX_USB_THIS hub[0].usb_port[port].resume = 0;
          BX_USB_THIS hub[0].usb_port[port].enabled = 0;
          // are we are currently connected/disconnected
          if (BX_USB_THIS hub[0].usb_port[port].status) {
            if (BX_USB_THIS hub[0].usb_port[port].device != NULL) {
              BX_USB_THIS hub[0].usb_port[port].low_speed =
                (BX_USB_THIS hub[0].usb_port[port].device->get_speed() == USB_SPEED_LOW);
              usb_set_connect_status(port, BX_USB_THIS hub[0].usb_port[port].device->get_type(), 1);
              BX_USB_THIS usb_send_msg(BX_USB_THIS hub[0].usb_port[port].device, USB_MSG_RESET);
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

void bx_pciusb_c::usb_timer_handler(void *this_ptr)
{
  bx_pciusb_c *class_ptr = (bx_pciusb_c *) this_ptr;
  class_ptr->usb_timer();
}

// Called once every 1ms
#define USB_STACK_SIZE  256
void bx_pciusb_c::usb_timer(void)
{
  int i;

  // If the "global reset" bit was set by software
  if (BX_USB_THIS global_reset) {
    for (i=0; i<USB_NUM_PORTS; i++) {
      BX_USB_THIS hub[0].usb_port[i].able_changed = 0;
      BX_USB_THIS hub[0].usb_port[i].connect_changed = 0;
      BX_USB_THIS hub[0].usb_port[i].enabled = 0;
      BX_USB_THIS hub[0].usb_port[i].line_dminus = 0;
      BX_USB_THIS hub[0].usb_port[i].line_dplus = 0;
      BX_USB_THIS hub[0].usb_port[i].low_speed = 0;
      BX_USB_THIS hub[0].usb_port[i].reset = 0;
      BX_USB_THIS hub[0].usb_port[i].resume = 0;
      BX_USB_THIS hub[0].usb_port[i].status = 0;
      BX_USB_THIS hub[0].usb_port[i].suspend = 0;
    }
    return;
  }

  // If command.schedule = 1, then run schedule
  //  *** This assumes that we can complete the frame within the 1ms time allowed ***
  // Actually, not complete, but reach the end of the frame.  This means that there may still 
  //  be TDs and QHs that were BREADTH defined and will be executed on the next cycle/iteration.

  if (BX_USB_THIS busy) {
    BX_PANIC(("Did not complete last frame before the 1ms was over. Starting next frame."));
    BX_USB_THIS busy = 0;
  }
  if (BX_USB_THIS hub[0].usb_command.schedule) {
    BX_USB_THIS busy = 1;
    bx_bool fire_int = 0;
    set_irq_level(0);  // make sure it is low
    bx_bool interrupt = 0, shortpacket = 0, stalled = 0;
    struct TD td;
    struct HCSTACK stack[USB_STACK_SIZE+1];  // queue stack for this item only
    Bit32s stk = 0;
    Bit32u item, address, lastvertaddr = 0, queue_num = 0;
    Bit32u frame, frm_addr = BX_USB_THIS hub[0].usb_frame_base.frame_base + 
                                (BX_USB_THIS hub[0].usb_frame_num.frame_num << 2);
    DEV_MEM_READ_PHYSICAL(frm_addr, 4, &frame);
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
          DEV_MEM_READ_PHYSICAL(address, 4, &item);
          stack[stk].next = item & ~0xF;
          stack[stk].d = HC_HORZ;
          stack[stk].q = (item & 0x0002) ? 1 : 0;
          stack[stk].t = (item & 0x0001) ? 1 : 0;
          // get VERT slot
          stk++;
          DEV_MEM_READ_PHYSICAL(lastvertaddr, 4, &item);
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
          DEV_MEM_READ_PHYSICAL(address, 32, &td);
          bx_bool spd = (td.dword1 & (1<<29)) ? 1 : 0;
          stack[stk].next = td.dword0 & ~0xF;
          bx_bool depthbreadth = (td.dword0 & 0x0004) ? 1 : 0;     // 1 = depth first, 0 = breadth first
          stack[stk].q = (td.dword0 & 0x0002) ? 1 : 0;
          stack[stk].t = (td.dword0 & 0x0001) ? 1 : 0;
          if (td.dword1 & (1<<24)) interrupt = 1;
          if (td.dword1 & (1<<23)) {  // is it an active TD
            BX_DEBUG(("Frame: %04i (0x%04X)", BX_USB_THIS hub[0].usb_frame_num.frame_num, BX_USB_THIS hub[0].usb_frame_num.frame_num));
            if (BX_USB_THIS DoTransfer(address, queue_num, &td)) {
              // issue short packet?
              Bit16u r_actlen = (((td.dword1 & 0x7FF)+1) & 0x7FF);
              Bit16u r_maxlen = (((td.dword2>>21)+1) & 0x7FF);
              BX_DEBUG((" r_actlen = 0x%04X r_maxlen = 0x%04X", r_actlen, r_maxlen));
              if (((td.dword2 & 0xFF) == USB_TOKEN_IN) && spd && stk && (r_actlen < r_maxlen) && ((td.dword1 & 0x00FF0000) == 0)) {
                shortpacket = 1;
                td.dword1 |= (1<<29);
              }
              if (td.dword1 & (1<<22)) stalled = 1;

              DEV_MEM_WRITE_PHYSICAL(address+4, 4, &td.dword1);  // write back the status
              // copy pointer for next queue item, in to vert queue head
              if ((stk > 0) && !shortpacket && (stack[stk].d == HC_VERT))
                DEV_MEM_WRITE_PHYSICAL(lastvertaddr, 4, &td.dword0);
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
      if (shortpacket && BX_USB_THIS hub[0].usb_enable.short_packet) {
        fire_int = 1;
        BX_DEBUG((" [SPD] We want it to fire here (Frame: %04i)", BX_USB_THIS hub[0].usb_frame_num.frame_num));
      }

      // if one of the TD's in this frame had the ioc bit set, we need to
      //   raise an interrupt, if interrupts are not masked via interrupt register.
      //   always set the status register if IOC.
      if (interrupt && BX_USB_THIS hub[0].usb_enable.on_complete) {
        fire_int = 1;
        BX_DEBUG((" [IOC] We want it to fire here (Frame: %04i)", BX_USB_THIS hub[0].usb_frame_num.frame_num));
      }

      if (stalled && BX_USB_THIS hub[0].usb_enable.timeout_crc) {
        fire_int = 1;
        BX_DEBUG((" [stalled] We want it to fire here (Frame: %04i)", BX_USB_THIS hub[0].usb_frame_num.frame_num));
      }
    }

    // The Frame Number Register is incremented every 1ms
    BX_USB_THIS hub[0].usb_frame_num.frame_num++;
    BX_USB_THIS hub[0].usb_frame_num.frame_num &= (1024-1);

    // if we needed to fire an interrupt now, lets do it *after* we increment the frame_num register
    if (fire_int) {
      BX_USB_THIS hub[0].usb_status.interrupt = 1;
      BX_USB_THIS hub[0].usb_status.error_interrupt = stalled;
      set_irq_level(1);
    }

    BX_USB_THIS busy = 0;  // ready to do next frame item
  }  // end run schedule

  // if host turned off the schedule, set the halted bit in the status register
  // Note: Can not use an else from the if() above since the host can changed this bit
  //  while we are processing a frame.
  if (BX_USB_THIS hub[0].usb_command.schedule == 0)
    BX_USB_THIS hub[0].usb_status.host_halted = 1;

  // TODO:
  //  If in Global_Suspend mode and any of usb_port[i] bits 6,3, or 1 are set,
  //    we need to issue a Global_Resume (set the global resume bit).
  //    However, since we don't do anything, let's not.
}

bx_bool bx_pciusb_c::DoTransfer(Bit32u address, Bit32u queue_num, struct TD *td) {
  
  int i, len = 0, ret = 0;
  usb_device_t *dev = NULL;

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
    if (BX_USB_THIS hub[0].usb_port[i].device != NULL) {
      if (BX_USB_THIS hub[0].usb_port[i].device->get_connected()) {
        at_least_one = 1;
        if (BX_USB_THIS hub[0].usb_port[i].device->get_address() == addr) {
          dev = BX_USB_THIS hub[0].usb_port[i].device;
          break;
        }
      }
    }
  }
  if (!at_least_one) {
    BX_USB_THIS set_status(td, 1, 0, 0, 0, (pid==USB_TOKEN_SETUP)?1:0, 0, 0x007); // an 8 byte packet was received, but stalled
    return 1;
  }
  if (dev == NULL) {
    if ((pid == USB_TOKEN_OUT) && (maxlen == 0x7FF) && (addr == 0)) {
      // This is the "keep awake" packet that Windows sends once a schedule cycle.
      // For now, let it pass through to the code below.
    } else {
      BX_PANIC(("Device not found for addr: %i", addr));
      BX_USB_THIS set_status(td, 1, 0, 0, 0, (pid==USB_TOKEN_SETUP)?1:0, 0, 0x007); // an 8 byte packet was received, but stalled
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
    BX_USB_THIS usb_packet.pid = pid;
    BX_USB_THIS usb_packet.devaddr = addr;
    BX_USB_THIS usb_packet.devep = endpt;
    BX_USB_THIS usb_packet.data = device_buffer;
    BX_USB_THIS usb_packet.len = maxlen;
    switch (pid) {
      case USB_TOKEN_OUT:
      case USB_TOKEN_SETUP:
        if (maxlen > 0) {
          DEV_MEM_READ_PHYSICAL(td->dword3, maxlen, device_buffer);
        }
        ret = dev->handle_packet(&BX_USB_THIS usb_packet);
        len = maxlen;
        break;
      case USB_TOKEN_IN:
        ret = dev->handle_packet(&BX_USB_THIS usb_packet);
        if (ret >= 0) {
          len = ret;
          if (len > maxlen) {
            len = maxlen;
            ret = USB_RET_BABBLE;
          }
          if (len > 0) {
            DEV_MEM_WRITE_PHYSICAL(td->dword3, len, device_buffer);
          }
        } else {
          len = 0;
        }
        break;
      default:
        BX_USB_THIS hub[i].usb_status.host_error = 1;
        BX_USB_THIS set_irq_level(1);
    }
    if (ret >= 0) {
      BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, len-1);
    } else {
      BX_USB_THIS set_status(td, 1, 0, 0, 0, 0, 0, 0x007); // stalled
    }
    return 1;
  }
  return 0;
}

// If the request fails, set the stall bit ????
void bx_pciusb_c::set_status(struct TD *td, bx_bool stalled, bx_bool data_buffer_error, bx_bool babble, 
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

// pci configuration space read callback handler
Bit32u bx_pciusb_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  if (io_len > 4 || io_len == 0) {
    BX_ERROR(("Experimental USB PCI read register 0x%02x, len=%u !",
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
    value |= (BX_USB_THIS hub[0].pci_conf[address+i] << (i*8));
    sprintf(szTmp2, "%02x", (BX_USB_THIS hub[0].pci_conf[address+i]));
    strrev(szTmp2);
    strcat(szTmp, szTmp2);
  }
  strrev(szTmp);
  BX_DEBUG(("USB PCI read  register 0x%02x %svalue 0x%s", address, pszName, szTmp));
  return value;
}


// pci configuration space write callback handler
void bx_pciusb_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  bx_bool baseaddr_change = 0;

  if (((address >= 0x10) && (address < 0x20)) ||
      ((address > 0x23) && (address < 0x34)))
    return;

  // This odd code is to display only what bytes actually were written.
  char szTmp[9];
  char szTmp2[3];
  szTmp[0] = '\0';
  szTmp2[0] = '\0';
  if (io_len <= 4) {
    for (unsigned i=0; i<io_len; i++) {
      value8 = (value >> (i*8)) & 0xFF;
      oldval = BX_USB_THIS hub[0].pci_conf[address+i];
      switch (address+i) {
        case 0x04:
          value8 &= 0x05;
          BX_USB_THIS hub[0].pci_conf[address+i] = value8;
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
            BX_USB_THIS hub[0].pci_conf[address+i] = value8;
          }
          sprintf(szTmp2, "%02x", value8);
          break;
        case 0x20:
          value8 = (value8 & 0xfc) | 0x01;
        case 0x21:
        case 0x22:
        case 0x23:
          baseaddr_change |= (value8 != oldval);
        default:
          BX_USB_THIS hub[0].pci_conf[address+i] = value8;
          sprintf(szTmp2, "%02x", value8);
      }
      strrev(szTmp2);
      strcat(szTmp, szTmp2);
    }
    if (baseaddr_change) {
      if (DEV_pci_set_base_io(BX_USB_THIS_PTR, read_handler, write_handler,
                             &BX_USB_THIS hub[0].base_ioaddr,
                             &BX_USB_THIS hub[0].pci_conf[0x20],
                             32, &usb_iomask[0], "USB Hub #1")) {
         BX_INFO(("new base address: 0x%04x", BX_USB_THIS hub[0].base_ioaddr));
      }
    }
  }
  strrev(szTmp);
  BX_DEBUG(("USB PCI write register 0x%02x                   value 0x%s", address, szTmp));
}

void bx_pciusb_c::usb_mouse_enabled_changed(bx_bool enable)
{
  if (enable && (BX_USB_THIS mousedev != NULL)) {
    mousedev->handle_reset();
  }
}

void bx_pciusb_c::usb_set_connect_status(Bit8u port, int type, bx_bool connected)
{
  char pname[BX_PATHNAME_LEN];
  char fname[BX_PATHNAME_LEN];

  if (BX_USB_THIS hub[0].usb_port[port].device != NULL) {
    if (BX_USB_THIS hub[0].usb_port[port].device->get_type() == type) {
      if (connected) {
        if (!BX_USB_THIS hub[0].usb_port[port].device->get_connected()) {
          BX_USB_THIS hub[0].usb_port[port].low_speed =
            (BX_USB_THIS hub[0].usb_port[port].device->get_speed() == USB_SPEED_LOW);
        }
        if (BX_USB_THIS hub[0].usb_port[port].low_speed) {
          BX_USB_THIS hub[0].usb_port[port].line_dminus = 1;  //  dminus=1 & dplus=0 = low speed  (at idle time)
          BX_USB_THIS hub[0].usb_port[port].line_dplus = 0;   //  dminus=0 & dplus=1 = high speed (at idle time)
        } else {
          BX_USB_THIS hub[0].usb_port[port].line_dminus = 0;  //  dminus=1 & dplus=0 = low speed  (at idle time)
          BX_USB_THIS hub[0].usb_port[port].line_dplus = 1;   //  dminus=0 & dplus=1 = high speed (at idle time)
        }
        BX_USB_THIS hub[0].usb_port[port].status = 1;       // 
        BX_USB_THIS hub[0].usb_port[port].connect_changed = 1;
        BX_USB_THIS hub[0].usb_port[port].able_changed = 1;

        // if in suspend state, signal resume
        if (BX_USB_THIS hub[0].usb_command.suspend) {
          BX_USB_THIS hub[0].usb_port[port].resume = 1;
          BX_USB_THIS hub[0].usb_status.resume = 1;
          if (BX_USB_THIS hub[0].usb_enable.resume) {
            BX_USB_THIS hub[0].usb_status.interrupt = 1;
            set_irq_level(1);
          }
        }

        if ((type == USB_DEV_TYPE_DISK) &&
            (!BX_USB_THIS hub[0].usb_port[port].device->get_connected())) {
          if (port == 0) {
            strcpy(pname, BXPN_USB1_PORT1);
          } else {
            strcpy(pname, BXPN_USB1_PORT2);
          }
          strcpy(fname, SIM->get_param_string(pname)->getptr() + 5);
          if (!((usb_msd_device_t*)BX_USB_THIS hub[0].usb_port[port].device)->init(fname)) {
            usb_set_connect_status(port, USB_DEV_TYPE_DISK, 0);
          } else {
            BX_INFO(("HD on USB port #%d: '%s'", port+1, fname));
          }
        }
      } else {
        BX_USB_THIS hub[0].usb_port[port].status = 0;
        BX_USB_THIS hub[0].usb_port[port].connect_changed = 1;
        BX_USB_THIS hub[0].usb_port[port].enabled = 0;
        BX_USB_THIS hub[0].usb_port[port].able_changed = 1;
        BX_USB_THIS hub[0].usb_port[port].low_speed = 0;
        BX_USB_THIS hub[0].usb_port[port].line_dminus = 0;  //  dminus=1 & dplus=0 = low speed  (at idle time)
        BX_USB_THIS hub[0].usb_port[port].line_dplus = 0;   //  dminus=0 & dplus=1 = high speed (at idle time)
        if ((type == USB_DEV_TYPE_MOUSE) ||
            (type == USB_DEV_TYPE_TABLET)) {
          if (BX_USB_THIS hub[0].usb_port[port].device == BX_USB_THIS mousedev) {
            BX_USB_THIS mousedev = NULL;
          }
        } else if (type == USB_DEV_TYPE_KEYPAD) {
          if (BX_USB_THIS hub[0].usb_port[port].device == BX_USB_THIS keybdev) {
            BX_USB_THIS keybdev = NULL;
          }
        }
        if (BX_USB_THIS hub[0].usb_port[port].device != NULL) {
          delete BX_USB_THIS hub[0].usb_port[port].device;
          BX_USB_THIS hub[0].usb_port[port].device = NULL;
          sprintf(pname, "pciusb.hub1.port%d.device", port+1);
          bx_list_c *devlist = (bx_list_c*)SIM->get_param(pname, SIM->get_bochs_root());
          devlist->clear();
        }
      }
    }
  }
}

void bx_pciusb_c::usb_mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state)
{
  if (BX_USB_THIS mousedev != NULL) {
    mousedev->mouse_enq(delta_x, delta_y, delta_z, button_state);
  }
}

bx_bool bx_pciusb_c::usb_key_enq(Bit8u *scan_code)
{
  if (BX_USB_THIS keybdev != NULL) {
    return keybdev->key_enq(scan_code);
  }
  return 0;
}

bx_bool bx_pciusb_c::usb_keyboard_connected()
{
  return (BX_USB_THIS keybdev != NULL);
}

bx_bool bx_pciusb_c::usb_mouse_connected()
{
  return (BX_USB_THIS mousedev != NULL);
}

// Send an internal message to a USB device
void bx_pciusb_c::usb_send_msg(usb_device_t *dev, int msg)
{
    USBPacket p;
    memset(&p, 0, sizeof(p));
    p.pid = msg;
    dev->handle_packet(&p);
}

// base class for USB devices

usb_device_t::usb_device_t(void)
{
  memset((void*)&d, 0, sizeof(d));
}

void usb_device_t::register_state(bx_list_c *parent)
{
  bx_list_c *list = new bx_list_c(parent, "d", "Common USB Device State");
  new bx_shadow_num_c(list, "addr", &d.addr);
  new bx_shadow_num_c(list, "state", &d.state);
  new bx_shadow_num_c(list, "remote_wakeup", &d.remote_wakeup);
  register_state_specific(parent);
}

// generic USB packet handler

#define SETUP_STATE_IDLE 0
#define SETUP_STATE_DATA 1
#define SETUP_STATE_ACK  2

int usb_device_t::handle_packet(USBPacket *p)
{
  int l, ret = 0;
  int len = p->len;
  Bit8u *data = p->data;

  switch(p->pid) {
    case USB_MSG_ATTACH:
      d.state = USB_STATE_ATTACHED;
      break;
    case USB_MSG_DETACH:
      d.state = USB_STATE_NOTATTACHED;
      break;
    case USB_MSG_RESET:
      d.remote_wakeup = 0;
      d.addr = 0;
      d.state = USB_STATE_DEFAULT;
      handle_reset();
      break;
    case USB_TOKEN_SETUP:
      if (d.state < USB_STATE_DEFAULT || p->devaddr != d.addr)
        return USB_RET_NODEV;
      if (len != 8)
        goto fail;
      memcpy(d.setup_buf, data, 8);
      d.setup_len = (d.setup_buf[7] << 8) | d.setup_buf[6];
      d.setup_index = 0;
      if (d.setup_buf[0] & USB_DIR_IN) {
        ret = handle_control((d.setup_buf[0] << 8) | d.setup_buf[1],
                             (d.setup_buf[3] << 8) | d.setup_buf[2],
                             (d.setup_buf[5] << 8) | d.setup_buf[4],
                             d.setup_len, d.data_buf);
        if (ret < 0)
          return ret;
        if (ret < d.setup_len)
          d.setup_len = ret;
        d.setup_state = SETUP_STATE_DATA;
      } else {
        if (d.setup_len == 0)
          d.setup_state = SETUP_STATE_ACK;
        else
          d.setup_state = SETUP_STATE_DATA;
      }
      break;
    case USB_TOKEN_IN:
      if (d.state < USB_STATE_DEFAULT || p->devaddr != d.addr)
        return USB_RET_NODEV;
      switch(p->devep) {
        case 0:
          switch(d.setup_state) {
            case SETUP_STATE_ACK:
              if (!(d.setup_buf[0] & USB_DIR_IN)) {
                d.setup_state = SETUP_STATE_IDLE;
                ret = handle_control((d.setup_buf[0] << 8) | d.setup_buf[1],
                                     (d.setup_buf[3] << 8) | d.setup_buf[2],
                                     (d.setup_buf[5] << 8) | d.setup_buf[4],
                                     d.setup_len, d.data_buf);
                if (ret > 0)
                  ret = 0;
              } else {
                // return 0 byte
              }
              break;
            case SETUP_STATE_DATA:
              if (d.setup_buf[0] & USB_DIR_IN) {
                l = d.setup_len - d.setup_index;
                if (l > len)
                  l = len;
                memcpy(data, d.data_buf + d.setup_index, l);
                d.setup_index += l;
                if (d.setup_index >= d.setup_len)
                  d.setup_state = SETUP_STATE_ACK;
                ret = l;
              } else {
                d.setup_state = SETUP_STATE_IDLE;
                goto fail;
              }
              break;
            default:
                goto fail;
            }
            break;
        default:
            ret = handle_data(p);
            break;
        }
        break;
    case USB_TOKEN_OUT:
        if (d.state < USB_STATE_DEFAULT || p->devaddr != d.addr)
          return USB_RET_NODEV;
        switch(p->devep) {
        case 0:
          switch(d.setup_state) {
            case SETUP_STATE_ACK:
              if (d.setup_buf[0] & USB_DIR_IN) {
                d.setup_state = SETUP_STATE_IDLE;
                // transfer OK
              } else {
                // ignore additionnal output
              }
              break;
            case SETUP_STATE_DATA:
              if (!(d.setup_buf[0] & USB_DIR_IN)) {
                l = d.setup_len - d.setup_index;
                if (l > len)
                  l = len;
                memcpy(d.data_buf + d.setup_index, data, l);
                d.setup_index += l;
                if (d.setup_index >= d.setup_len)
                  d.setup_state = SETUP_STATE_ACK;
                ret = l;
              } else {
                d.setup_state = SETUP_STATE_IDLE;
                goto fail;
              }
              break;
            default:
              goto fail;
          }
          break;
        default:
          ret = handle_data(p);
          break;
      }
      break;
    default:
    fail:
      ret = USB_RET_STALL;
      break;
  }
  return ret;
}


// USB runtime parameter handler
 
const char *bx_pciusb_c::usb_param_handler(bx_param_string_c *param, int set, const char *val, int maxlen)
{
  usbdev_type type = USB_DEV_TYPE_NONE;

  // handler for USB runtime parameters
  if (set) {
    char pname[BX_PATHNAME_LEN];
    param->get_param_path(pname, BX_PATHNAME_LEN);
    if (!strcmp(pname, BXPN_USB1_PORT1)) {
      BX_INFO(("USB port #1 experimental device change"));
      if (!strcmp(val, "none") && BX_USB_THIS hub[0].usb_port[0].status) {
        if (BX_USB_THIS hub[0].usb_port[0].device != NULL) {
          type = BX_USB_THIS hub[0].usb_port[0].device->get_type();
        }
        usb_set_connect_status(0, type, 0);
      } else if (strcmp(val, "none") && !BX_USB_THIS hub[0].usb_port[0].status) {
        init_device(0, val);
      }
    } else if (!strcmp(pname, BXPN_USB1_PORT2)) {
      BX_INFO(("USB port #2 experimental device change"));
      if (!strcmp(val, "none") && BX_USB_THIS hub[0].usb_port[1].status) {
        if (BX_USB_THIS hub[0].usb_port[1].device != NULL) {
          type = BX_USB_THIS hub[0].usb_port[1].device->get_type();
        }
        usb_set_connect_status(1, type, 0);
      } else if (strcmp(val, "none") && !BX_USB_THIS hub[0].usb_port[1].status) {
        init_device(1, val);
      }
    } else {
      BX_PANIC(("usb_param_handler called with unexpected parameter '%s'", pname));
    }
  }
  return val;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
