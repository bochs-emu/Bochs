/////////////////////////////////////////////////////////////////////////
// $Id: pciusb.cc,v 1.19 2005-01-14 18:28:47 vruppert Exp $
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
     I have been doing.  Many printf()'s here and there.
   - My purpose of coding this emulation was/is to learn about the USB.
     It has been a challenge, but I have learned a lot.
   - 
   -
   -
   - If I forget, there are a lot of BX_INFO's that can be changed to BX_DEBUG's.
  */

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#if BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB

#define LOG_THIS theUSBDevice->

bx_pciusb_c* theUSBDevice = NULL;

const Bit8u usb_iomask[32] = {2, 1, 2, 1, 2, 1, 2, 1, 4, 0, 0, 0, 1, 0, 0, 0,
                              2, 1, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  int
libpciusb_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theUSBDevice = new bx_pciusb_c ();
  bx_devices.pluginPciUSBAdapter = theUSBDevice;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theUSBDevice, BX_PLUGIN_PCIUSB);
  return 0; // Success
}

  void
libpciusb_LTX_plugin_fini(void)
{
}


bx_pciusb_c::bx_pciusb_c(void)
{
  put("USB");
  settype(PCIUSBLOG);
}

bx_pciusb_c::~bx_pciusb_c(void)
{

  //TODO:  free  BX_USB_THIS device_buffer

  // nothing for now
  BX_DEBUG(("Exit."));
}


  void
bx_pciusb_c::init(void)
{
  // called once when bochs initializes

  if (!bx_options.usb[0].Oenabled->get()) return;

  BX_USB_THIS device_buffer = new Bit8u[65536];

  // Call our timer routine every 1mS (1,000uS)
  // Continuous and active
  BX_USB_THIS hub[0].timer_index =
                   bx_pc_system.register_timer(this, usb_timer_handler, 1000, 1,1, "usb.timer");

  BX_USB_THIS hub[0].devfunc = BX_PCI_DEVICE(1,2);
  DEV_register_pci_handlers(this,
                            pci_read_handler,
                            pci_write_handler,
                            &BX_USB_THIS hub[0].devfunc, BX_PLUGIN_PCIUSB,
                            "Experimental PCI USB");

  for (unsigned i=0; i<256; i++) {
    BX_USB_THIS hub[0].pci_conf[i] = 0x0;
  }

  BX_USB_THIS hub[0].base_ioaddr = 0x0;

  Bit16u base_ioaddr = bx_options.usb[0].Oioaddr->get();

  BX_INFO(("usb1 at 0x%04x-0x%04x irq assigned by BIOS", base_ioaddr, base_ioaddr+0x13));

  //FIXME: for now, we want a status bar // hub zero, port zero
  BX_USB_THIS hub[0].statusbar_id[0] = bx_gui->register_statusitem("USB");
}

  void
bx_pciusb_c::reset(unsigned type)
{
  unsigned i, j;

  if (!bx_options.usb[0].Oenabled->get()) return;

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
      { 0x20, ((bx_options.usb[0].Oioaddr->get() & 0xE0) | 0x01) },
      { 0x21, (bx_options.usb[0].Oioaddr->get() >> 8) },
      { 0x22, 0x00 }, { 0x23, 0x00 },
      { 0x3c, 0x00 },                 // IRQ
      { 0x3d, BX_PCI_INTD },          // INT
      { 0x6a, 0x01 },                 // USB clock
      { 0xc1, 0x20 }                  // PIRQ enable

    };
    for (i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); ++i) {
        BX_USB_THIS hub[0].pci_conf[reset_vals[i].addr] = reset_vals[i].val;
    }
    // This should be done by the PCI BIOS
    DEV_pci_set_base_io(BX_USB_THIS_PTR, read_handler, write_handler,
                        &BX_USB_THIS hub[0].base_ioaddr,
                        &BX_USB_THIS hub[0].pci_conf[0x20],
                        32, &usb_iomask[0], "USB Hub #1");
  }

  // reset locals
  BX_USB_THIS busy = 0;
  BX_USB_THIS last_connect = 0xFF;
  BX_USB_THIS global_reset = 0;
  BX_USB_THIS set_address_stk = 0;
  memset(BX_USB_THIS saved_key, 0, 8);
  memset(BX_USB_THIS key_pad_packet, 0, 8);

  // mouse packet stuff
  BX_USB_THIS mouse_delayed_dx = 0;
  BX_USB_THIS mouse_delayed_dy = 0;
  BX_USB_THIS mouse_delayed_dz = 0;
  BX_USB_THIS button_state = 0;

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
    }
  }

  // First, clear out the device(s)
  for (i=0; i<BX_USB_CONFDEV; i++)
    for (j=0; j<USB_CUR_DEVS; j++)
      memset(&BX_USB_THIS hub[i].device[j], 0, sizeof(USB_DEVICE));

  BX_USB_THIS keyboard_connected = 0;

  // include the device(s) initialize code
  #include "pciusb_devs.h"

  init_device(0, bx_options.usb[0].Oport1->getptr());
  init_device(1, bx_options.usb[0].Oport2->getptr());

}

  void
bx_pciusb_c::init_device(Bit8u port, char *devname)
{
  Bit8u type = USB_DEV_TYPE_NONE;
  bx_bool connected = 0;

  if (!strlen(devname)) return;

  if (!strcmp(devname, "mouse")) {
    type = USB_DEV_TYPE_MOUSE;
    connected = bx_options.Omouse_enabled->get();
    if (bx_options.Omouse_type->get() != BX_MOUSE_TYPE_USB) {
      BX_ERROR(("USB mouse present, but other mouse type configured"));
    }
  } else if (!strcmp(devname, "keypad")) {
    type = USB_DEV_TYPE_KEYPAD;
    connected = 1;
    BX_USB_THIS keyboard_connected = 1;
  } else {
    BX_PANIC(("unknown USB device: %s", devname));
    return;
  }
  for (int i=0; i<USB_CUR_DEVS; i++) {
    if (BX_USB_THIS hub[0].device[i].dev_type == type) {
      BX_USB_THIS hub[0].usb_port[port].device_num = i;
    }
  }
  usb_set_connect_status(type, connected);
}

  void
bx_pciusb_c::set_irq_level(bx_bool level)
{
  DEV_pci_set_irq(BX_USB_THIS hub[0].devfunc, BX_USB_THIS hub[0].pci_conf[0x3d], level);
}

  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_pciusb_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PCIUSB_SMF
  bx_pciusb_c *class_ptr = (bx_pciusb_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}


  Bit32u
bx_pciusb_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIUSB_SMF
  Bit32u val = 0x0;
  Bit8u  offset,port;

  //BX_DEBUG(("register read from address 0x%04x - ", (unsigned) address));

  offset = address - BX_USB_THIS hub[0].base_ioaddr;

  switch (offset) {
    case 0x00: // command register (16-bit)
      val = /*BX_USB_THIS hub[0].usb_command.max_packet_size*/ 0 << 7  // always return as 0 (max packet = 32) lowspeed
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
    case 0x12: // port #2
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
        break;
      } // else fall through to default
    default:
      val = 0xFF7F; // keep compiler happy
      BX_PANIC(("unsupported io read from address=0x%04x!", (unsigned) address));
      break;
  }

  //BX_DEBUG(("val =  0x%08x", (Bit32u) val));
  BX_DEBUG(("register read from address 0x%04X:  0x%08X (%i bits)", (unsigned) address, (Bit32u) val, io_len * 8));

  return(val);
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_pciusb_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PCIUSB_SMF
  bx_pciusb_c *class_ptr = (bx_pciusb_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

  void
bx_pciusb_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIUSB_SMF
  Bit8u  offset,port;

//  BX_DEBUG(("register write to address 0x%04x - ", (unsigned) address));
  BX_DEBUG(("register write to  address 0x%04X:  0x%04X  (%i bits)", (unsigned) address, (unsigned) value, io_len * 8));

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
          usb_set_connect_status(BX_USB_THIS hub[0].device[BX_USB_THIS hub[0].usb_port[i].device_num].dev_type,
            BX_USB_THIS hub[0].device[BX_USB_THIS hub[0].usb_port[i].device_num].connect_status);
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
          BX_USB_THIS reset(0);
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

      BX_USB_THIS hub[0].usb_status.host_halted = (value & 0x20) ? 0: BX_USB_THIS hub[0].usb_status.host_halted;
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

      break;

    case 0x06: // frame number register (16-bit)
      if (value & 0xF800)
        BX_DEBUG(("write to frame number register with bits 15:11 not zero: 0x%04x", value));

      if (BX_USB_THIS hub[0].usb_status.host_halted)
        BX_USB_THIS hub[0].usb_frame_num.frame_num = value;
      else
        // ignored by the hardward, but lets report it anyway
        BX_DEBUG(("write to frame number register with STATUS.HALTED == 0"));
      break;

    case 0x08: // frame base register (32-bit)
      if (value & 0xFFF)
        BX_PANIC(("write to frame base register with bits 11:0 not zero: 0x%08x", value));

      BX_USB_THIS hub[0].usb_frame_base.frame_base = value;
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
      if (port < USB_NUM_PORTS) {
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
          // are we are currently connected/disconnected
          BX_USB_THIS hub[0].device[BX_USB_THIS hub[0].usb_port[port].device_num].connect_status = 0;
          usb_set_connect_status(BX_USB_THIS hub[0].device[BX_USB_THIS hub[0].usb_port[port].device_num].dev_type, 1);
          BX_DEBUG(("Port #%d: Reset", port+1));
        }
        break;
      }
      // else fall through to default
    default:
      BX_PANIC(("unsupported io write to address=0x%04x!", (unsigned) address));
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
  
  // *** Does Bochs interrupt this timer, or does it wait for the return before it fires the next timer????

  if (BX_USB_THIS busy) {
    BX_PANIC(("Did not complete last frame before the 1ms was over. Starting next frame."));
    BX_USB_THIS busy = 0;
  }
  if (BX_USB_THIS hub[0].usb_command.schedule) {
    BX_USB_THIS busy = 1;
    bx_bool fire_int = 0;
    set_irq_level(0);  // make sure it is low
    bx_bool interrupt = 0, shortpacket = 0;
    struct TD td;
    struct HCSTACK stack[USB_STACK_SIZE+1];  // queue stack for this item only
    Bit32s stk = 0;
    Bit32u item, address, lastvertaddr = 0;
    Bit32u frame, frm_addr = BX_USB_THIS hub[0].usb_frame_base.frame_base + 
                                (BX_USB_THIS hub[0].usb_frame_num.frame_num << 2);
    BX_MEM_READ_PHYSICAL(frm_addr, 4, &frame);
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
          BX_MEM_READ_PHYSICAL(address, 4, &item);
          stack[stk].next = item & ~0xF;
          stack[stk].d = HC_HORZ;
          stack[stk].q = (item & 0x0002) ? 1 : 0;
          stack[stk].t = (item & 0x0001) ? 1 : 0;
          // get VERT slot
          stk++;
          BX_MEM_READ_PHYSICAL(lastvertaddr, 4, &item);
          stack[stk].next = item & ~0xF;
          stack[stk].d = HC_VERT;
          stack[stk].q = (item & 0x0002) ? 1 : 0;
          stack[stk].t = (item & 0x0001) ? 1 : 0;
        } else {  // else is a TD
          address = stack[stk].next;
          BX_MEM_READ_PHYSICAL(address, 32, &td);
          bx_bool spd = (td.dword1 & (1<<29)) ? 1 : 0;
          stack[stk].next = td.dword0 & ~0xF;
          bx_bool depthbreadth = (td.dword0 & 0x0004) ? 1 : 0;     // 1 = depth first, 0 = breadth first
          stack[stk].q = (td.dword0 & 0x0002) ? 1 : 0;
          stack[stk].t = (td.dword0 & 0x0001) ? 1 : 0;
          if (td.dword1 & (1<<24)) interrupt = 1;
          if (td.dword1 & (1<<23)) {  // is it an active TD
            BX_DEBUG(("Frame: %04i", BX_USB_THIS hub[0].usb_frame_num.frame_num));
            BX_USB_THIS DoTransfer(&td);
            
            // issue short packet?
            Bit16u r_actlen = (((td.dword1 & 0x7FF)+1) & 0x7FF);
            Bit16u r_maxlen = (((td.dword2>>21)+1) & 0x7FF);
            BX_DEBUG((" r_actlen = 0x%04X r_maxlen = 0x%04X", r_actlen, r_maxlen));
            if (((td.dword2 & 0xFF) == TOKEN_IN) && spd && stk && (r_actlen < r_maxlen) && ((td.dword1 & 0x00FF0000) == 0)) {
              shortpacket = 1;
              td.dword1 |= (1<<29);
            }
            
            BX_MEM_WRITE_PHYSICAL(address+4, 4, &td.dword1);  // write back the status
            // copy pointer for next queue item, in to vert queue head
            if ((stk > 0) && !shortpacket && (stack[stk].d == HC_VERT))
              BX_MEM_WRITE_PHYSICAL(lastvertaddr, 4, &td.dword0);
          }
          
          if (stk > 0) {
            // if last TD in HORZ queue pointer, then we are done.
            if (stack[stk].t && (stack[stk].d == HC_HORZ)) break;
            // if Breadth first or last item in queue, move to next queue.
            if (!depthbreadth || stack[stk].t) stk--;
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
        BX_DEBUG((" [SPD] We wan't it to fire here (Frame: %04i)", BX_USB_THIS hub[0].usb_frame_num.frame_num));
      }
      
      // if one of the TD's in this frame had the ioc bit set, we need to
      //   raise an interrupt, if interrupts are not masked via interrupt register.
      //   always set the status register if IOC.
      if (interrupt && BX_USB_THIS hub[0].usb_enable.on_complete) {
        fire_int = 1;
        BX_DEBUG((" [IOC] We wan't it to fire here (Frame: %04i)", BX_USB_THIS hub[0].usb_frame_num.frame_num));
      }
    
    }

    // The Frame Number Register is incremented every 1ms
    BX_USB_THIS hub[0].usb_frame_num.frame_num++;
    BX_USB_THIS hub[0].usb_frame_num.frame_num &= (1024-1);

    // if we needed to fire an interrupt now, lets do it *after* we increment the frame_num register
    if (fire_int) {
      BX_USB_THIS hub[0].usb_status.interrupt = 1;
      set_irq_level(1);
    }

    BX_USB_THIS busy = 0;  // ready to do next frame item
  }  // end run schedule

    // TODO:
  //  If in Global_Suspend mode and any of usb_port[i] bits 6,3, or 1 are set,
  //    we need to issue a Global_Resume (set the global resume bit).
  //    However, since we don't do anything, let's not.
}

void bx_pciusb_c::DoTransfer(struct TD *td) {
  
  int i, j;
  Bit8u protocol = 0;
  bx_bool fnd;

  Bit16u maxlen = (td->dword2 >> 21);
  Bit8u  addr   = (td->dword2 >> 8) & 0x7F;
  Bit8u  endpt  = (td->dword2 >> 15) & 0x0F;
  Bit8u  pid    =  td->dword2 & 0xFF;
//Bit8u  d      = (td->dword2 & 0x00080000) ? 1 : 0;
  Bit8u  data[64];
  struct REQUEST_PACKET *rp = (struct REQUEST_PACKET *) data;

  // if packet, read in the packet data
  if (pid == TOKEN_SETUP) {
    //LO_SOP
    if (td->dword3) BX_MEM_READ_PHYSICAL(td->dword3, 8, data);
    // the '8' above may need to be maxlen (unless maxlen == 0)
    //LO_EOP
  }

  BX_DEBUG(("TD found:  %08x   %08x   %08x   %08x", td->dword0, td->dword1, td->dword2, td->dword3));
  BX_DEBUG(("           %02x %02x %02x %02x %02x %02x %02x %02x", data[0], data[1], data[2], 
                                                     data[3], data[4], data[5], data[6], data[7]));
  // check TD to make sure it is valid
  // A max length 0x500 to 0x77E is illegal
  if (((td->dword2 >> 21) >= 0x500) && ((td->dword2 >> 21) != 0x7FF)) {
    BX_ERROR(("error at 11111111111"));
    return;  // error = consistency check failure
  }
  //if (td->dword0 & 0x8) return; // error = reserved bits in dword0 set
  // other error checks here

  // find device address
  bx_bool at_least_one = 0;
  struct USB_DEVICE *dev = NULL;
  for (i=0; i<USB_CUR_DEVS; i++) {
    if (BX_USB_THIS hub[0].device[i].connect_status) at_least_one = 1;
    if ((BX_USB_THIS hub[0].device[i].address == addr) && BX_USB_THIS hub[0].device[i].connect_status){
      dev = &BX_USB_THIS hub[0].device[i];
      break;
    }
  }
  if (!at_least_one) {
    BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received, but stalled
    return;
  }
  if (dev == NULL) {
    BX_PANIC(("Device not found for addr: %i", addr));
    BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received, but stalled
    return;  // device not found
  }
  
  maxlen++;
  maxlen &= 0x7FF;
  if (maxlen > dev->function.device_descr.max_packet_size)
    maxlen = dev->function.device_descr.max_packet_size;
  
  // parse and get command
  Bit16u cnt;
  switch (pid) {
    case TOKEN_IN:   // Data came from HC to Host
      // if an interrupt in, do the protocol packet.
      if (endpt > 0) {
        fnd = 0;
        for (i=0; (i<dev->function.device_config[dev->config].interfaces) && !fnd; i++) {
          for (j=0; j<dev->function.device_config[dev->config].Interface[i].num_endpts; j++) {
            if (endpt == (dev->function.device_config[dev->config].Interface[i].endpts[j].endpt & 0x7F)) {
              protocol = dev->function.device_config[dev->config].Interface[i].protocol;
              fnd = 1;
              break;
            }
          }
        }
        //BX_INFO(("Interface %i  endpt %i protocol %i", j, endpt, protocol));
        if (fnd) {
          cnt = dev->function.device_config[dev->config].Interface[dev->Interface].endpts[0].max_size;
          switch (protocol) {
            case 1:  // keypad
             
              memcpy(device_buffer, BX_USB_THIS key_pad_packet, 8);

              BX_MEM_WRITE_PHYSICAL(td->dword3, cnt, device_buffer);
              BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, cnt-1);
              break;

            case 2:  // mouse
              if (!BX_USB_THIS mouse_x && !BX_USB_THIS mouse_y) {
                // if there's no new movement, handle delayed one
                BX_USB_THIS usb_mouse_enq(0, 0, BX_USB_THIS mouse_z, BX_USB_THIS b_state);
              }
              device_buffer[0] = (Bit8u) BX_USB_THIS b_state;
              device_buffer[1] = (Bit8s) BX_USB_THIS mouse_x;
              device_buffer[2] = (Bit8s) BX_USB_THIS mouse_y;
              device_buffer[3] = (Bit8s) BX_USB_THIS mouse_z;  // if wheel mouse
              BX_USB_THIS b_state = 0;
              BX_USB_THIS mouse_x = 0;
              BX_USB_THIS mouse_y = 0;
              BX_USB_THIS mouse_z = 0;
              BX_MEM_WRITE_PHYSICAL(td->dword3, cnt, device_buffer);
              BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, cnt-1);
              break;

            default:
              BX_PANIC(("Unknown/unsupported endpt protocol issued an Interrupt In packet.  protocol %i", protocol));
          }       
        } else {
          BX_PANIC(("Unknown endpt issued an Interrupt In packet.  ID 0x%04x Interface %i  endpt %i", i, endpt, dev->function.device_descr.vendorid));
        }
      } else {
        bx_gui->statusbar_setitem(BX_USB_THIS hub[0].statusbar_id[0], 1);
        BX_DEBUG(("TOKEN_IN: len = %i", maxlen));
        BX_DEBUG(("          address = 0x%08X, Depth = %i, QH = %i, Terminate = %i\n"
                 "                              spd=%i, c_err=%i, LS=%i, IOS=%i, IOC=%i, status=0x%02X, act_len=0x%03X\n"
                 "                              max_len=0x%03X, D=%i, EndPt=%i, addr=%i, PID=0x%02X\n"
                 "                              buffer address = 0x%08X",
        td->dword0 & ~0x0000000F, (td->dword0 & 0x00000004)?1:0, (td->dword0 & 0x00000002)?1:0, (td->dword0 & 0x00000001)?1:0,
        (td->dword1 & 0x20000000)?1:0, (td->dword1 & 0x18000000)>>27, (td->dword1 & 0x04000000)?1:0, (td->dword1 & 0x02000000)?1:0, (td->dword1 & 0x01000000)?1:0, (td->dword1 & 0x00FF0000)>>16, td->dword1 & 0x000007FF,
        (td->dword2 & 0xFFE00000)>>21, (td->dword2 & 0x00080000)?1:0, (td->dword2 & 0x00078000)>>15, (td->dword2 & 0x00007F00)>>8, td->dword2 & 0x000000FF,
        td->dword3));
        // After a set address, the Host will send an empty IN packet for the status stage.
        //  The address on the IN packet will still be zero (0).
        //  So we need to set the address for this device *after* the in packet.
        if (maxlen == 0) {
          BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x7FF);
          if (BX_USB_THIS set_address_stk && !addr) {
            BX_USB_THIS set_address_stk--;
            dev->address = BX_USB_THIS set_address[BX_USB_THIS set_address_stk];
            dev->state = STATE_ADDRESS;
          }
          break;
        }
        
        if (dev->function.in_cnt > 0) {
          cnt = (dev->function.in_cnt < maxlen) ? dev->function.in_cnt : maxlen;
          //LO_SOP
          BX_MEM_WRITE_PHYSICAL(td->dword3, cnt, dev->function.in);
          //LO_EOP
        
          BX_DEBUG((" Packet: %02X %02X %02X %02X %02X %02X %02X %02X", 
            dev->function.in[0], dev->function.in[1], dev->function.in[2],
            dev->function.in[3], dev->function.in[4], dev->function.in[5],
            dev->function.in[6], dev->function.in[7]));
        
          dev->function.in += cnt;
          dev->function.in_cnt -= cnt;
          BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, cnt-1);
        } else
          BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x7FF);
        bx_gui->statusbar_setitem(BX_USB_THIS hub[0].statusbar_id[0], 0);
      }
      break;
    case TOKEN_OUT: // data should go from Host to HC
      BX_DEBUG(("TOKEN_OUT: maxlen = 0x%03X", maxlen));
      BX_DEBUG(("          address = 0x%08X, Depth = %i, QH = %i, Terminate = %i\n"
               "                              spd=%i, c_err=%i, LS=%i, IOS=%i, IOC=%i, status=0x%02X, act_len=0x%03X\n"
               "                              max_len=0x%03X, D=%i, EndPt=%i, addr=%i, PID=0x%02X\n"
               "                              buffer address = 0x%08X",
      td->dword0 & ~0x0000000F, (td->dword0 & 0x00000004)?1:0, (td->dword0 & 0x00000002)?1:0, (td->dword0 & 0x00000001)?1:0,
      (td->dword1 & 0x20000000)?1:0, (td->dword1 & 0x18000000)>>27, (td->dword1 & 0x04000000)?1:0, (td->dword1 & 0x02000000)?1:0, (td->dword1 & 0x01000000)?1:0, (td->dword1 & 0x00FF0000)>>16, td->dword1 & 0x000007FF,
      (td->dword2 & 0xFFE00000)>>21, (td->dword2 & 0x00080000)?1:0, (td->dword2 & 0x00078000)>>15, (td->dword2 & 0x00007F00)>>8, td->dword2 & 0x000000FF,
      td->dword3));
      
      bx_gui->statusbar_setitem(BX_USB_THIS hub[0].statusbar_id[0], 1);
      if (maxlen == 0) {
        BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x7FF);
      } else {
      
      }
      bx_gui->statusbar_setitem(BX_USB_THIS hub[0].statusbar_id[0], 0);
      break;
    case TOKEN_SETUP:
      BX_DEBUG((" SETUP packet: %02X %02X %02X %02X %02X %02X %02X %02X", 
        data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]));
      switch (rp->request) {
        case GET_STATUS:
          switch (dev->state) {
            case STATE_DEFAULT:
              BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
              BX_ERROR(("Request: GET_STATUS returned an error"));
              break;
            case STATE_ADDRESS:
              if (rp->index > 0) {
                BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
                BX_ERROR(("Request: GET_STATUS returned an error"));
                break;
              } // else fall through
            case STATE_CONFIGURED:

              BX_PANIC(("Request: GET_STATUS not implemented yet"));

              // If the interface or endpoint does not exist, return error

              BX_DEBUG(("Request: GET_STATUS"));
              BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x007); // an 8 byte packet was received
          }
          break;
        case CLEAR_FEATURE:
          switch (dev->state) {
            case STATE_DEFAULT:
              BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
              BX_ERROR(("Request: CLEAR_FEATURE returned an error"));
              break;
            case STATE_ADDRESS:
              if (rp->index > 0) {
                BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
                BX_ERROR(("Request: CLEAR_FEATURE returned an error"));
                break;
              } // else fall through
            case STATE_CONFIGURED:
              if (rp->length) {
                BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
                BX_ERROR(("Request: CLEAR_FEATURE returned an error:  length > 0"));
                break;
              }
              // At this point, we don't implement features.  So until we do, there isn't anything
              //  to clear.  Just log an error and continue.
              BX_ERROR(("Request: CLEAR_FEATURE:  Not implemented yet."));
              BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x007); // an 8 byte packet was received
              break;
          }
          break;
        case SET_FEATURE:
          switch (dev->state) {
            case STATE_DEFAULT:
              if (((rp->index >> 8) == SET_FEATURE_TEST_MODE) && (rp->request_type == 0) && ((rp->index & 0xFF) == 0)) {
                BX_DEBUG(("Request: SET_FEATURE in TEST MODE"));
              } else {
                BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
                BX_ERROR(("Request: SET_FEATURE returned an error"));
              }
              break;
            case STATE_ADDRESS:
              if ((rp->index & 0xFF) > 0) {
                BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
                BX_ERROR(("Request: SET_FEATURE returned an error"));
                break;
              } // else fall through
            case STATE_CONFIGURED:

              BX_PANIC(("Request: SET_FEATURE not implemented yet"));

              BX_DEBUG(("Request: SET_FEATURE"));
              BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x007); // an 8 byte packet was received
          }
          break;
        case SET_ADDRESS:
          BX_DEBUG(("Request: SET_ADDRESS: %04x", rp->value));
          BX_DEBUG(("          address = 0x%08X, Depth = %i, QH = %i, Terminate = %i\n"
                   "                              spd=%i, c_err=%i, LS=%i, IOS=%i, IOC=%i, status=0x%02X, act_len=0x%03X\n"
                   "                              max_len=0x%03X, D=%i, EndPt=%i, addr=%i, PID=0x%02X\n"
                   "                              buffer address = 0x%08X",
          td->dword0 & ~0x0000000F, (td->dword0 & 0x00000004)?1:0, (td->dword0 & 0x00000002)?1:0, (td->dword0 & 0x00000001)?1:0,
          (td->dword1 & 0x20000000)?1:0, (td->dword1 & 0x18000000)>>27, (td->dword1 & 0x04000000)?1:0, (td->dword1 & 0x02000000)?1:0, (td->dword1 & 0x01000000)?1:0, (td->dword1 & 0x00FF0000)>>16, td->dword1 & 0x000007FF,
          (td->dword2 & 0xFFE00000)>>21, (td->dword2 & 0x00080000)?1:0, (td->dword2 & 0x00078000)>>15, (td->dword2 & 0x00007F00)>>8, td->dword2 & 0x000000FF,
          td->dword3));
          if ((rp->value > 127) || rp->index || rp->length) {
            BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received, but errored
            BX_ERROR(("Request: SET_ADDRESS returned an error"));
          } else {
            switch (dev->state) {
              case STATE_DEFAULT:
                BX_USB_THIS set_address[BX_USB_THIS set_address_stk] = (Bit8u) rp->value;
                BX_USB_THIS set_address_stk++;
                BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x007);
                break;
              case STATE_ADDRESS:
                if (rp->value > 0) {
                  BX_USB_THIS set_address[BX_USB_THIS set_address_stk] = (Bit8u) rp->value;
                  BX_USB_THIS set_address_stk++;
                } else {
                  BX_USB_THIS set_address[BX_USB_THIS set_address_stk] = 0;
                  BX_USB_THIS set_address_stk++;
                  dev->state = STATE_DEFAULT;
                }
                BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x007);
                break;
              case STATE_CONFIGURED:
                BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received, but errored
                BX_ERROR(("Request: SET_ADDRESS returned an error"));
            }
          }
          break;
        case GET_DESCRIPTOR:
          BX_DEBUG(("Request: GET_DESCRIPTOR: 0x%02x  0x%02x  %i", rp->value >> 8, rp->value & 0xFF, rp->length));
          BX_DEBUG(("          address = 0x%08X, Depth = %i, QH = %i, Terminate = %i\n"
                   "                              spd=%i, c_err=%i, LS=%i, IOS=%i, IOC=%i, status=0x%02X, act_len=0x%03X\n"
                   "                              max_len=0x%03X, D=%i, EndPt=%i, addr=%i, PID=0x%02X\n"
                   "                              buffer address = 0x%08X",
          td->dword0 & ~0x0000000F, (td->dword0 & 0x00000004)?1:0, (td->dword0 & 0x00000002)?1:0, (td->dword0 & 0x00000001)?1:0,
          (td->dword1 & 0x20000000)?1:0, (td->dword1 & 0x18000000)>>27, (td->dword1 & 0x04000000)?1:0, (td->dword1 & 0x02000000)?1:0, (td->dword1 & 0x01000000)?1:0, (td->dword1 & 0x00FF0000)>>16, td->dword1 & 0x000007FF,
          (td->dword2 & 0xFFE00000)>>21, (td->dword2 & 0x00080000)?1:0, (td->dword2 & 0x00078000)>>15, (td->dword2 & 0x00007F00)>>8, td->dword2 & 0x000000FF,
          td->dword3));
          switch (dev->state) {
            case STATE_DEFAULT:
            case STATE_ADDRESS:
            case STATE_CONFIGURED:
              BX_USB_THIS set_status(td, 0, 0, BX_USB_THIS GetDescriptor(dev, rp), 0, 0, 0, 0x007);
          }
          break;
        case SET_DESCRIPTOR:
          switch (dev->state) {
            case STATE_DEFAULT:
              BX_ERROR(("Request: SET_DESCRIPTOR returned an error"));
              BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
              break;
            case STATE_ADDRESS:
            case STATE_CONFIGURED:
              
              BX_PANIC(("Request: SET_DESCRIPTOR not implemented yet"));

              //dev->descriptor = (Bit8u) rp->value >> 8;

              BX_DEBUG(("Request: SET_DESCRIPTOR: %02x", rp->value >> 8));
              BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x007); // an 8 byte packet was received
          }
          break;
        case GET_CONFIGURATION:
          BX_DEBUG(("Request: GET_CONFIGURATION"));
          BX_DEBUG(("          address = 0x%08X, Depth = %i, QH = %i, Terminate = %i\n"
                   "                              spd=%i, c_err=%i, LS=%i, IOS=%i, IOC=%i, status=0x%02X, act_len=0x%03X\n"
                   "                              max_len=0x%03X, D=%i, EndPt=%i, addr=%i, PID=0x%02X\n"
                   "                              buffer address = 0x%08X",
          td->dword0 & ~0x0000000F, (td->dword0 & 0x00000004)?1:0, (td->dword0 & 0x00000002)?1:0, (td->dword0 & 0x00000001)?1:0,
          (td->dword1 & 0x20000000)?1:0, (td->dword1 & 0x18000000)>>27, (td->dword1 & 0x04000000)?1:0, (td->dword1 & 0x02000000)?1:0, (td->dword1 & 0x01000000)?1:0, (td->dword1 & 0x00FF0000)>>16, td->dword1 & 0x000007FF,
          (td->dword2 & 0xFFE00000)>>21, (td->dword2 & 0x00080000)?1:0, (td->dword2 & 0x00078000)>>15, (td->dword2 & 0x00007F00)>>8, td->dword2 & 0x000000FF,
          td->dword3));
          switch (dev->state) {
            case STATE_DEFAULT:
              BX_ERROR(("Request: GET_CONFIGURATION returned an error"));
              BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
              break;
            case STATE_ADDRESS:
              // must return zero if in the address state
              *device_buffer = 0;
              break;
            case STATE_CONFIGURED:
              *device_buffer = dev->function.device_config[dev->config].config_val;
          }
          dev->function.in = device_buffer;
          dev->function.in_cnt = 1;
          BX_DEBUG(("Request: GET_CONFIGURATION"));
          BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x007); // an 8 byte packet was received
          break;
        case SET_CONFIGURATION:
          switch (dev->state) {
            case STATE_DEFAULT:
              BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received, but stalled
              BX_ERROR(("Request: SET_CONFIGURATION: returned an error"));
              break;
            case STATE_CONFIGURED:
              if ((rp->value & 0xFF) == 0) {
                dev->state = STATE_DEFAULT;
                BX_DEBUG(("Request: SET_CONFIGURATION = 0 (set to default state)"));
                BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x007); // an 8 byte packet was received
                break;
              } // else fall through ????
            case STATE_ADDRESS:
              dev->config = 0xFF;
              unsigned configs;
              for (configs=0; configs<dev->function.configs; configs++) {
                if ((rp->value & 0xFF) == dev->function.device_config[configs].config_val) {
                  dev->config = configs;
                  dev->state = STATE_CONFIGURED;
                  break;
                }
              }
              if (dev->config == 0xFF) {
                dev->config = 0;
                BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
                BX_ERROR(("Request: SET_CONFIGURATION: returned an error"));
              } else {
                BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x007); // an 8 byte packet was received
                BX_DEBUG(("Request: SET_CONFIGURATION: %02x", rp->value));
              }
              break;
          }
          break;
        case GET_INTERFACE:
          switch (dev->state) {
            case STATE_DEFAULT:
            case STATE_ADDRESS:
              BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
              BX_ERROR(("Request: GET_INTERFACE returned and error: not in configured state"));
              break;
            case STATE_CONFIGURED:
              if ((unsigned)(rp->index + 1) > dev->function.device_config[dev->config].interfaces) {
                BX_PANIC(("GET_INTERFACE: wanted interface number is greater than interface count."));
                BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
              }
              // The USB specs say that I can return a STALL if only one interface is used. (ie, no alternate)
              // However, Win98SE doesn't like it at all....
              //if (dev->function.device_config[dev->config].interfaces > 1) {
                *device_buffer = dev->function.device_config[dev->config].Interface[rp->value & 0xFF].alternate;
                dev->function.in = device_buffer;
                dev->function.in_cnt = 1;
                if (dev->function.in_cnt > rp->length)
                  dev->function.in_cnt = rp->length;
                BX_DEBUG(("Request: GET_INTERFACE  %02X  %02X  %02X", rp->value >> 8, rp->value & 0xFF, rp->length));
                BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x007); // an 8 byte packet was received
              //} else {
              //  BX_DEBUG(("Request: GET_INTERFACE  %02X  %02X  %02X (Only 1 interface, returning STALL)", rp->value >> 8, rp->value & 0xFF, rp->length));
              //  BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
              //}
              break;
          }
          break;
        case SET_INTERFACE:
          switch (dev->state) {
            case STATE_DEFAULT:
            case STATE_ADDRESS:
              BX_ERROR(("Request: SET_INTERFACE returned and error: not in configured state"));
              BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
              break;
            case STATE_CONFIGURED:
              if ((unsigned)(rp->index + 1) > dev->function.device_config[dev->config].interfaces) {
                BX_PANIC(("SET_INTERFACE: wanted interface number is greater than interface count."));
                BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
              }
              dev->Interface = (Bit8u) rp->index;
              dev->alt_interface = (Bit8u) rp->value;
              BX_DEBUG(("Request: SET_INTERFACE: %02x (alt %02x)", rp->index, rp->value));
              BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x007); // an 8 byte packet was received
          }
          break;
        case SYNCH_FRAME:
          switch (dev->state) {
            case STATE_DEFAULT:
            case STATE_ADDRESS:
              BX_ERROR(("Request: SYNCH_FRAME returned and error: not in configured state"));
              BX_USB_THIS set_status(td, 0, 0, 1, 0, 0, 0, 0x007); // an 8 byte packet was received
              break;
            case STATE_CONFIGURED:

              BX_PANIC(("Request: SYNCH_FRAME not implemented yet"));

              BX_DEBUG(("Request: SYNCH_FRAME"));
              BX_USB_THIS set_status(td, 0, 0, 0, 0, 0, 0, 0x007); // an 8 byte packet was received
          }
          break;
        default:
          BX_PANIC((" **** illegal or unknown REQUEST sent to Host Controller:  %02x", data[1]));
      }
      break;
    default:
      BX_PANIC(("illegal PID sent to Host Controller:  %02x", pid));
  }
}

unsigned bx_pciusb_c::GetDescriptor(struct USB_DEVICE *dev, struct REQUEST_PACKET *packet) {

  Bit8u *p = device_buffer;
  unsigned i, j, fnd, ret = 0; // ret = 0 = no error

  BX_DEBUG(("GET DESCRIPTOR  0x%02X 0x%02X  value=0x%04X  len=%i index=%i", 
    packet->request, packet->request_type, packet->value, packet->length, packet->index));
    
  switch (packet->value >> 8) {
    case DEVICE:
      dev->function.in = (Bit8u *) &dev->function.device_descr;
      dev->function.in_cnt = dev->function.device_descr.len;
      ret = 0;
      break;
    case CONFIG:
      memcpy(p, &dev->function.device_config[dev->config], 9); p += 9;  // config descriptor
      for (i=0; i<dev->function.device_config[dev->config].interfaces; i++) {
        memcpy(p, &dev->function.device_config[dev->config].Interface[i], 9); p += 9;
        for (j=0; j<dev->function.device_config[dev->config].Interface[i].num_endpts; j++) {
          memcpy(p, &dev->function.device_config[dev->config].Interface[i].endpts[j], 7); p += 7;
        }
        memcpy(p, &dev->function.device_config[dev->config].Interface[i].dev_hid_descript, 6); p += 6;
        for (j=0; j<dev->function.device_config[dev->config].Interface[i].dev_hid_descript.num_descriptors; j++) {
          memcpy(p, &dev->function.device_config[dev->config].Interface[i].dev_hid_descript.descriptor[j], 3); p += 3;
        }
      }
      dev->function.in = device_buffer;
      dev->function.in_cnt = (p - device_buffer);
      ret = 0;
      break;
    case STRING:
      switch (packet->value & 0xFF) {
        case 0: // string descriptor table
          dev->function.in = (Bit8u *) &dev->function.str_descriptor;
          dev->function.in_cnt = dev->function.str_descriptor.size;
          ret = 0;
          break;
        case 1: case 2: case 3:
        case 4: case 5: case 6:
          dev->function.in = (Bit8u *) &dev->function.string[(packet->value & 0xFF)-1];
          dev->function.in_cnt = dev->function.string[(packet->value & 0xFF)-1].size;
          break;
          ret = 0;
        default:
          BX_ERROR(("STRING:  %i", packet->value & 0xFF));
          ret = 1;
      }
      break;
    case INTERFACE:
      BX_PANIC(("GET_DESCRIPTOR: INTERFACE not implemented yet."));
      ret = 1;
      break;
    case ENDPOINT:
      BX_PANIC(("GET_DESCRIPTOR: ENDPOINT not implemented yet."));
      ret = 1;
      break;
    case DEVICE_QUALIFIER:
      BX_PANIC(("GET_DESCRIPTOR: DEVICE_QUALIFIER not implemented yet."));
      ret = 1;
      break;
    case OTHER_SPEED_CONFIG:
      BX_PANIC(("GET_DESCRIPTOR: OTHER_SPEED_CONFIG not implemented yet."));
      ret = 1;
      break;
    case INTERFACE_POWER:
      BX_PANIC(("GET_DESCRIPTOR: INTERFACE_POWER not implemented yet."));
      ret = 1;
      break;
    case 0x21:  // (HID device descriptor)
      dev->function.in = (Bit8u *) &dev->function.device_config[dev->config].Interface[packet->index-1].dev_hid_descript;
      dev->function.in_cnt = dev->function.device_config[dev->config].Interface[packet->index-1].dev_hid_descript.size;
      ret = 0;
      break;
    case 0x22:  // (HID report descriptor)
      // Win98SE passes packet->index as endpoint number (packet->request_type = 0x82)
      // SuSE Linux passes packet->index as interface number (packet->request_type = 0x81)
      switch (packet->request_type & 0x1F) {
        case 2: // endpoint
          fnd = 0;
          for (i=0; (i<dev->function.device_config[dev->config].interfaces) && !fnd; i++) {
            for (j=0; (j<dev->function.device_config[dev->config].Interface[i].num_endpts) && !fnd; j++) {
              if ((dev->function.device_config[dev->config].Interface[i].endpts[j].endpt & ~0x80) == packet->index) {
                packet->index = i;
                fnd = 1;
              }
            }
          }
          if (!fnd) {
            BX_ERROR(("Get Descriptor:  HID Report:  index = endpint.  did not find endpoint"));
            ret = 1;
            break;
          } // else fall through to interface
        case 1: // interface
          for (i=0; i<dev->function.device_config[dev->config].Interface[packet->index].dev_hid_descript.num_descriptors; i++) {
            if (dev->function.device_config[dev->config].Interface[packet->index].dev_hid_descript.descriptor[i].type == 0x22) {
              dev->function.in = dev->function.device_config[dev->config].Interface[packet->index].dev_hid_descript.descriptor[i].dev_hid_descript_report;
              dev->function.in_cnt = dev->function.device_config[dev->config].Interface[packet->index].dev_hid_descript.descriptor[i].len;
              break;
            }
          }
          ret = 0;
          break;
        case 0:  // device
        case 3:  // other
        default: // reserved
          BX_ERROR(("Get Descriptor:  HID Report:  index = reserved.  Should be Interface or Endpoint"));
          ret = 1;
          break;
      }
      break;
    default:
      BX_PANIC((" **** illegal or unknown GET_DESCRIPTOR::DEVICE sent to Host Controller:  %02x", packet->value >> 8));
      return 1;
  }
  if (dev->function.in_cnt > packet->length)
    dev->function.in_cnt = packet->length;
  
  return ret;
}

// If the request fails, set the stall bit ????
void bx_pciusb_c::set_status(struct TD *td, bx_bool stalled, bx_bool data_buffer_error, bx_bool babble, 
                             bx_bool nak, bx_bool crc_time_out, bx_bool bitstuff_error, Bit16u act_len) {

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
}
 
 
  // static pci configuration space read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_pciusb_c::pci_read_handler(void *this_ptr, Bit8u address, unsigned io_len)
{
#if !BX_USE_PCIUSB_SMF
  bx_pciusb_c *class_ptr = (bx_pciusb_c *) this_ptr;

  return class_ptr->pci_read(address, io_len);
}


  Bit32u
bx_pciusb_c::pci_read(Bit8u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIUSB_SMF

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
  BX_DEBUG(("USB PCI read register 0x%02x %svalue 0x%s", address, pszName, szTmp));
  return value;
}


  // static pci configuration space write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_pciusb_c::pci_write_handler(void *this_ptr, Bit8u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PCIUSB_SMF
  bx_pciusb_c *class_ptr = (bx_pciusb_c *) this_ptr;

  class_ptr->pci_write(address, value, io_len);
}

  void
bx_pciusb_c::pci_write(Bit8u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIUSB_SMF

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
        case 0x3d: //
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
        case 0x21:
          baseaddr_change |= (value8 != oldval);
        default:
          BX_USB_THIS hub[0].pci_conf[address+i] = value8;
          sprintf(szTmp2, "%02x", value8);
      }
      strrev(szTmp2);
      strcat(szTmp, szTmp2);
    }
    if (baseaddr_change) {
      DEV_pci_set_base_io(BX_USB_THIS_PTR, read_handler, write_handler,
                          &BX_USB_THIS hub[0].base_ioaddr,
                          &BX_USB_THIS hub[0].pci_conf[0x20],
                          32, &usb_iomask[0], "USB Hub #1");
      BX_INFO(("new base address: 0x%04x", BX_USB_THIS hub[0].base_ioaddr));
    }
  }
  strrev(szTmp);
  BX_DEBUG(("USB PCI write register 0x%02x value 0x%s", address, szTmp));
}

  void
bx_pciusb_c::usb_mouse_enable(bx_bool enable)
{
  if (BX_USB_THIS last_connect != enable) {
    usb_set_connect_status(USB_DEV_TYPE_MOUSE, enable);
    BX_USB_THIS last_connect = enable;
  }
}

  void
bx_pciusb_c::usb_set_connect_status(int type, bx_bool connected)
{
  for (int i=0; i<BX_USB_CONFDEV; i++) {
    for (int j=0; j<USB_NUM_PORTS; j++) {
      if (BX_USB_THIS hub[i].device[BX_USB_THIS hub[i].usb_port[j].device_num].dev_type == type) {
        if (connected) {
          if (!BX_USB_THIS hub[i].device[BX_USB_THIS hub[i].usb_port[j].device_num].connect_status) {
            BX_USB_THIS hub[i].device[BX_USB_THIS hub[i].usb_port[j].device_num].state = STATE_DEFAULT;
            BX_USB_THIS hub[i].device[BX_USB_THIS hub[i].usb_port[j].device_num].address = 0;
            BX_USB_THIS hub[i].device[BX_USB_THIS hub[i].usb_port[j].device_num].alt_interface = 0;
            BX_USB_THIS hub[i].device[BX_USB_THIS hub[i].usb_port[j].device_num].Interface = 0;
            BX_USB_THIS hub[i].device[BX_USB_THIS hub[i].usb_port[j].device_num].config = 0;
          }
          BX_USB_THIS hub[i].usb_port[j].low_speed = 1;    // all devices we support are low speed
          BX_USB_THIS hub[i].usb_port[j].line_dminus = 1;  //  dminus=1 & dplus=0 = low speed  (at idle time)
          BX_USB_THIS hub[i].usb_port[j].line_dplus = 0;   //  dminus=0 & dplus=1 = high speed (at idle time)
          BX_USB_THIS hub[i].usb_port[j].status = 1;       // 
          BX_USB_THIS hub[i].usb_port[j].connect_changed = 1;
          BX_USB_THIS hub[i].device[BX_USB_THIS hub[i].usb_port[j].device_num].connect_status = 1;
        } else {
          BX_USB_THIS hub[i].usb_port[j].low_speed = 0;    // all devices we support are low speed
          BX_USB_THIS hub[i].usb_port[j].line_dminus = 0;  //  dminus=1 & dplus=0 = low speed  (at idle time)
          BX_USB_THIS hub[i].usb_port[j].line_dplus = 0;   //  dminus=0 & dplus=1 = high speed (at idle time)
          BX_USB_THIS hub[i].usb_port[j].status = 0;       // 
          BX_USB_THIS hub[i].usb_port[j].enabled = 0;      // 
          BX_USB_THIS hub[i].usb_port[j].able_changed = 
            BX_USB_THIS hub[i].device[BX_USB_THIS hub[i].usb_port[j].device_num].connect_status ? 1 : 0;
          BX_USB_THIS hub[i].usb_port[j].connect_changed = 
            BX_USB_THIS hub[i].device[BX_USB_THIS hub[i].usb_port[j].device_num].connect_status ? 1 : 0;
          BX_USB_THIS hub[i].device[BX_USB_THIS hub[i].usb_port[j].device_num].connect_status = 0;
        }
      }
    }
  }
}

  void
bx_pciusb_c::usb_mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state)
{
  // scale down the motion
  if ( (delta_x < -1) || (delta_x > 1) )
    delta_x /= 2;
  if ( (delta_y < -1) || (delta_y > 1) )
    delta_y /= 2;

  if(delta_x>127) delta_x=127;
  if(delta_y>127) delta_y=127;
  if(delta_x<-128) delta_x=-128;
  if(delta_y<-128) delta_y=-128;

  BX_USB_THIS mouse_delayed_dx+=delta_x;
  BX_USB_THIS mouse_delayed_dy-=delta_y;

  if (BX_USB_THIS mouse_delayed_dx > 127) {
    delta_x = 127;
    BX_USB_THIS mouse_delayed_dx -= 127;
  } else if (BX_USB_THIS mouse_delayed_dx < -128) {
    delta_x = -128;
    BX_USB_THIS mouse_delayed_dx += 128;
  } else {
    delta_x = BX_USB_THIS mouse_delayed_dx;
    BX_USB_THIS mouse_delayed_dx = 0;
  }
  if (BX_USB_THIS mouse_delayed_dy > 127) {
    delta_y = 127;
    BX_USB_THIS mouse_delayed_dy -= 127;
  } else if (BX_USB_THIS mouse_delayed_dy < -128) {
    delta_y = -128;
    BX_USB_THIS mouse_delayed_dy += 128;
  } else {
    delta_y = BX_USB_THIS mouse_delayed_dy;
    BX_USB_THIS mouse_delayed_dy = 0;
  }

  BX_USB_THIS mouse_x = (Bit8s) delta_x;
  BX_USB_THIS mouse_y = (Bit8s) delta_y;
  BX_USB_THIS mouse_z = (Bit8s) delta_z;
  BX_USB_THIS b_state = (Bit8u) button_state;
}

  bx_bool
bx_pciusb_c::usb_key_enq(Bit8u *scan_code)
{

//  BX_INFO(("scan code: len = %i   0x%02x  0x%02x 0x%02x", strlen((char *) scan_code), scan_code[0], scan_code[1], scan_code[2]));
//  return 0;

  bx_bool is_break_code = 0;
  Bit8u our_scan_code[8];

  memset(our_scan_code, 0, 8);
  int os = 0;
  for (int s=0; s<8; s++) {
    if ((scan_code[s] == 0xF0) && ((s == 0) || ((s == 1) && (scan_code[0] == 0xE0)))) {
      is_break_code = 1;
    } else {
      if (!(our_scan_code[os++] = scan_code[s])) break;
    }
  }
  
  // if it is the break code of the saved key, then clear our packet key.
  if (is_break_code && !memcmp(BX_USB_THIS saved_key, our_scan_code, 8)) {
    memset(BX_USB_THIS saved_key, 0, 8);
    memset(BX_USB_THIS key_pad_packet, 0, 8);
    return 1; // tell the keyboard handler that we used it, and to return with out processing key
  }

  // find an attached keyboard device and one that has a conversion table filled.
  // we assume the (bochs) user has only one device attached with these specs.
  bx_bool fnd = 0;
  struct USB_DEVICE *dev;
  for (int i=0; i<BX_USB_CONFDEV && !fnd; i++) {
    for (int j=0; j<USB_NUM_PORTS && !fnd; j++) {
      // is there something plugged in to this port?
      if (BX_USB_THIS hub[i].usb_port[j].status) {
        dev = &BX_USB_THIS hub[i].device[BX_USB_THIS hub[i].usb_port[j].device_num];
        for (int k=0; k<dev->function.device_descr.configs && !fnd; k++) {
          for (int l=0; l<dev->function.device_config[k].interfaces && !fnd; l++) {
            if ((dev->function.device_config[k].Interface[l].protocol == 1) 
              && dev->function.device_config[k].Interface[l].lookup_cnt) {
              // Only do this if the keypad is in the configured state
              if (dev->state == STATE_CONFIGURED) {
                for (int m=0; m<dev->function.device_config[k].Interface[l].lookup_cnt; m++) {
                  if (!memcmp(dev->function.device_config[k].Interface[l].lookup[m].scan_code, our_scan_code, 8)) {
                    memcpy(BX_USB_THIS key_pad_packet, dev->function.device_config[k].Interface[l].lookup[m].keypad_packet, 8);
                    fnd = 1;
                    break;
                  }
                }
              } else {
                memset(BX_USB_THIS saved_key, 0, 8);
                return 0;  // if keypad is not configured, it can't send scan codes. 
              }
            }
          }
        }
      }
    }
  }
  
  if (!fnd) {
    memset(BX_USB_THIS key_pad_packet, 0, 8);
    memset(BX_USB_THIS saved_key, 0, 8);
  } else {
    memcpy(BX_USB_THIS saved_key, our_scan_code, 8);
    // print a debug line to the log file
    char bx_debug_code[128] = "";
    char value[8];
    for (unsigned i=0; i<strlen((char *) our_scan_code); i++) {
      sprintf(value, "0x%02x", our_scan_code[i]);
      if (i) strcat(bx_debug_code, "  ");
      strcat(bx_debug_code, value);
    }
    BX_DEBUG(("Re-routing scan code (%s) to USB keypad", bx_debug_code));
  }

  // tell the keyboard handler whether we used it or not.  (0 = no, 1 = yes and keyboard.cc ignores keystoke)
  return fnd;
}

  bx_bool
bx_pciusb_c::usb_keyboard_connected()
{
  return BX_USB_THIS keyboard_connected;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
