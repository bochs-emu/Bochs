/////////////////////////////////////////////////////////////////////////
// $Id: pciusb.cc,v 1.2 2003-02-02 10:24:26 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2003  MandrakeSoft S.A.
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

//
// Experimental PCI USB adapter
// Benjamin D Lunt (fys@cybertrails.com) coded most of this usb emulation.
//  I hope to add to this code to make it more functionable.
//

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#if BX_PCI_SUPPORT && BX_PCI_USB_SUPPORT

#define LOG_THIS theUSBDevice->

bx_pciusb_c* theUSBDevice = NULL;

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
  // nothing for now
  BX_DEBUG(("Exit."));
}


  void
bx_pciusb_c::init(void)
{
  // called once when bochs initializes

  if (!bx_options.usb[0].Oenabled->get()) return;

  Bit16u base_ioaddr = bx_options.usb[0].Oioaddr->get();
  Bit8u irq = bx_options.usb[0].Oirq->get();

  DEV_register_irq(irq, "USB Hub #1");
  BX_USB_THIS hub[0].irq = irq;

  // Call our timer routine every 1mS (1,000uS)
  // Continuous and active
  BX_USB_THIS hub[0].timer_index =
                   bx_pc_system.register_timer(this, usb_timer_handler, 1000, 1,1, "usb.timer");

  for (unsigned addr=base_ioaddr; addr<=(unsigned)(base_ioaddr+0x14); addr++) {
    BX_DEBUG(("register read/write: 0x%04x", addr));
    DEV_register_ioread_handler(this, read_handler, addr, "USB Hub #1", 7);
    DEV_register_iowrite_handler(this, write_handler, addr, "USB Hub #1", 7);
  }
  BX_USB_THIS hub[0].base_ioaddr = base_ioaddr;

  DEV_register_pci_handlers(this,
                            pci_read_handler,
                            pci_write_handler,
                            BX_PCI_DEVICE(1,2),
                            "Experimental PCI USB");

  for (unsigned i=0; i<256; i++) {
    BX_USB_THIS hub[0].pci_conf[i] = 0x0;
  }

  BX_INFO(("usb1 at 0x%04x-0x%04x irq %d", base_ioaddr, base_ioaddr+0x13, irq));
}

  void
bx_pciusb_c::reset(unsigned type)
{
  unsigned i;

  static const struct reset_vals_t {
    unsigned      addr;
    unsigned char val;
  } reset_vals[] = {
    { 0x00, 0x86 }, { 0x01, 0x80 }, // 0x8086 = vendor
    { 0x02, 0x20 }, { 0x03, 0x70 }, // 0x7020 = device
    { 0x04, 0x05 }, { 0x05, 0x00 },	// command_io
    { 0x06, 0x80 }, { 0x07, 0x02 },	// status
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
    { 0x3c, bx_options.usb[0].Oirq->get() }, // IRQ
    { 0x3d, 0x04 },                 // INT
    { 0x6a, 0x01 },                 // USB clock
    { 0xc1, 0x20 }                  // PIRQ enable

  };
  for (i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); ++i) {
      BX_USB_THIS hub[0].pci_conf[reset_vals[i].addr] = reset_vals[i].val;
  }

  // reset locals
  BX_USB_THIS global_reset = 0;

  // Put the USB registers into their RESET state
  for (i=0; i<BX_USB_CONFDEV; i++) {
    BX_USB_THIS hub[i].usb_command.max_packet_size = 0;
    BX_USB_THIS hub[i].usb_command.configured = 0;
    BX_USB_THIS hub[i].usb_command.debug = 0;
    BX_USB_THIS hub[i].usb_command.resume = 0;
    BX_USB_THIS hub[i].usb_command.suspend = 1;
    BX_USB_THIS hub[i].usb_command.host_reset = 0;
    BX_USB_THIS hub[i].usb_command.reset = 0;
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
    for (unsigned j=0; j<USB_NUM_PORTS; j++) {
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

  BX_DEBUG(("register read from address 0x%04x - ", (unsigned) address));

  offset = address - BX_USB_THIS hub[0].base_ioaddr;

  switch (offset) {
    case 0x0C: // Start of Frame Modify
    case 0x11: // port0 (high byte read)
    case 0x13: // port1 (high byte read)
      if (io_len != 1)
        BX_PANIC(("io read from port 0x%04x, bad len=%u", (unsigned) address, (unsigned) io_len));
      break;
    case 0x10: // port0
    case 0x12: // port1
      if ((io_len < 1) || (io_len > 2))
        BX_PANIC(("io read from port 0x%04x, bad len=%u", (unsigned) address, (unsigned) io_len));
      break;
    case 0x00: // command register (16-bit)
    case 0x02: // status register (16-bit)
    case 0x04: // interrupt enable register (1-bit)
    case 0x06: // frame number register (16-bit)
      if (io_len != 2)
        BX_PANIC(("io read from port 0x%04x, bad len=%u", (unsigned) address, (unsigned) io_len));
      break;
    case 0x08: // frame base register (32-bit)
      if (io_len != 4)
        BX_PANIC(("io read from port 0x%04x, bad len=%u", (unsigned) address, (unsigned) io_len));
      break;
  }

  switch (offset) {
    case 0x00: // command register (16-bit)
      val = BX_USB_THIS hub[0].usb_command.max_packet_size << 7
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

    case 0x10: // port0
    case 0x12: // port1
      port = (offset & 0x0F) >> 1;
      if (port < USB_NUM_PORTS) {
        val = BX_USB_THIS hub[0].usb_port[port].suspend << 12
              | BX_USB_THIS hub[0].usb_port[port].reset << 9
              | BX_USB_THIS hub[0].usb_port[port].low_speed << 8
              | 1 << 7
              | BX_USB_THIS hub[0].usb_port[port].resume << 6
              | BX_USB_THIS hub[0].usb_port[port].line_dplus << 5
              | BX_USB_THIS hub[0].usb_port[port].line_dminus << 4
              | BX_USB_THIS hub[0].usb_port[port].able_changed << 3
              | BX_USB_THIS hub[0].usb_port[port].enabled << 2
              | BX_USB_THIS hub[0].usb_port[port].connect_changed << 1
              | BX_USB_THIS hub[0].usb_port[port].status;
        break;
      } // else fall through to default

    default:
      val = 0; // keep compiler happy
      BX_PANIC(("unsupported io read from address=0x%04x!", (unsigned) address));
      break;
  }

  BX_DEBUG(("val =  0x%08x", (Bit32u) val));

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

  BX_DEBUG(("register write to address 0x%04x - ", (unsigned) address));

  offset = address - BX_USB_THIS hub[0].base_ioaddr;

  switch (offset) {
    case 0x0C: // Start of Frame Modify
      if (io_len != 1)
        BX_PANIC(("io write to port 0x%04x, bad len=%u", (unsigned) address, (unsigned) io_len));
      break;
    case 0x00: // command register (16-bit)
    case 0x02: // status register (16-bit)
    case 0x04: // interrupt enable register (1-bit)
    case 0x06: // frame number register (16-bit)
    case 0x10: // port0
    case 0x12: // port1
      if (io_len != 2)
        BX_PANIC(("io write to port 0x%04x, bad len=%u", (unsigned) address, (unsigned) io_len));
      break;
    case 0x08: // frame base register (32-bit)
      if (io_len != 4)
        BX_PANIC(("io write to port 0x%04x, bad len=%u", (unsigned) address, (unsigned) io_len));
      break;
  }

  switch (offset) {
    case 0x00: // command register (16-bit) (R/W)
      if (value & 0xFF00)
        BX_ERROR(("write to command register with bits 15:8 not zero: 0x%04x", value));

      BX_USB_THIS hub[0].usb_command.max_packet_size = (value & 0x80) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.configured = (value & 0x40) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.debug = (value & 0x20) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.resume = (value & 0x10) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.suspend = (value & 0x08) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.reset = (value & 0x04) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.host_reset = (value & 0x02) ? 1: 0;
      BX_USB_THIS hub[0].usb_command.schedule = (value & 0x01) ? 1: 0;

      // If software set the reset bit, we need to set reset bit of each port for 10ms.
      if (BX_USB_THIS hub[0].usb_command.reset)
        BX_USB_THIS global_reset = 10;

      // If host_reset then reset all registers, etc.
      if (BX_USB_THIS hub[0].usb_command.host_reset)
        BX_USB_THIS reset(0);

      // If Run/Stop, identify in log and ignore
      if (BX_USB_THIS hub[0].usb_command.schedule)
        BX_INFO(("Software set Schedule bit in Command register"));

      // If Debug mode set, panic.  Not implemented
      if (BX_USB_THIS hub[0].usb_command.debug)
        BX_PANIC(("Software set DEBUG bit in Command register. Not implemented"));

      break;

    case 0x02: // status register (16-bit) (R/WC)
      if (value & 0xFFC0)
        BX_ERROR(("write to status register with bits 15:6 not zero: 0x%04x", value));

      BX_USB_THIS hub[0].usb_status.host_halted = (value & 0x20) ? 0: BX_USB_THIS hub[0].usb_status.host_halted;
      BX_USB_THIS hub[0].usb_status.host_error = (value & 0x10) ? 0: BX_USB_THIS hub[0].usb_status.host_error;
      BX_USB_THIS hub[0].usb_status.pci_error = (value & 0x08) ? 0: BX_USB_THIS hub[0].usb_status.pci_error;
      BX_USB_THIS hub[0].usb_status.resume = (value & 0x04) ? 0: BX_USB_THIS hub[0].usb_status.resume;
      BX_USB_THIS hub[0].usb_status.error_interrupt = (value & 0x02) ? 0: BX_USB_THIS hub[0].usb_status.error_interrupt;
      BX_USB_THIS hub[0].usb_status.interrupt = (value & 0x01) ? 0: BX_USB_THIS hub[0].usb_status.interrupt;
      break;

    case 0x04: // interrupt enable register (16-bit)
      if (value & 0xFFF0)
        BX_ERROR(("write to interrupt enable register with bits 15:4 not zero: 0x%04x", value));

      BX_USB_THIS hub[0].usb_enable.short_packet  = (value & 0x08) ? 1: 0;
      BX_USB_THIS hub[0].usb_enable.on_complete  = (value & 0x04) ? 1: 0;
      BX_USB_THIS hub[0].usb_enable.resume  = (value & 0x02) ? 1: 0;
      BX_USB_THIS hub[0].usb_enable.timeout_crc = (value & 0x01) ? 1: 0;

      // For now, we will just ignore these being set since we never raise the IRQ

      break;

    case 0x06: // frame number register (16-bit)
      if (value & 0xF800)
        BX_ERROR(("write to frame number register with bits 15:11 not zero: 0x%04x", value));

      if (BX_USB_THIS hub[0].usb_status.host_halted)
        BX_USB_THIS hub[0].usb_frame_num.frame_num = value;
      else
        // ignored by the hardward, but lets report it anyway
        BX_ERROR(("write to frame number register with STATUS.HALTED == 0"));

      break;

    case 0x08: // frame base register (32-bit)
      if (value & 0xFFF)
        BX_PANIC(("write to frame base register with bits 11:0 not zero: 0x%08x", value));

      BX_USB_THIS hub[0].usb_frame_base.frame_base = value;
      break;

    case 0x0C: // start of Frame Modify register (8-bit)
      if (value & 0x80)
        BX_ERROR(("write to SOF Modify register with bit 7 not zero: 0x%04x", value));

       BX_USB_THIS hub[0].usb_sof.sof_timing = value;
       break;

    case 0x10: // port0
    case 0x12: // port1
      port = (offset & 0x0F) >> 1;
      if (port < USB_NUM_PORTS) {
        if (value & ((1<<5) | (1<<4) | (1<<0)))
          BX_PANIC(("write to one or more read-only bits in port%d register: 0x%04x", port, value));
        if (!(value & (1<<7)))
          BX_ERROR(("write to port%d register bit 7 = 0", port));
        if (value & (1<<8))
          BX_INFO(("write to bit 8 in port%d register ignored", port));
        if (value & (1<<2))
          BX_INFO(("port%d enabled ignored.  Not implemented", port));
        if ((value & (1<<12)) && BX_USB_THIS hub[0].usb_command.suspend)
          BX_ERROR(("write to port%d register bit 12 when in Global-Suspend", port));

        BX_USB_THIS hub[0].usb_port[port].suspend = (value & (1<<12)) ? 1 : 0;
        BX_USB_THIS hub[0].usb_port[port].reset = (value & (1<<9)) ? 1 : 0;
        BX_USB_THIS hub[0].usb_port[port].resume = (value & (1<<6)) ? 1 : 0;
        BX_USB_THIS hub[0].usb_port[port].able_changed = (value & (1<<3)) ? 0 : BX_USB_THIS hub[0].usb_port[0].able_changed;
        BX_USB_THIS hub[0].usb_port[port].enabled = (value & (1<<2)) ? 1 : 0;
        BX_USB_THIS hub[0].usb_port[port].connect_changed = (value & (1<<1)) ? 0 : BX_USB_THIS hub[0].usb_port[0].connect_changed;
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
void bx_pciusb_c::usb_timer(void)
{
  int i;

  // The Frame Number Register is incremented every 1ms  ?????????
  // Needs more work and investigation on this.
  BX_USB_THIS hub[0].usb_frame_num.frame_num++;
  BX_USB_THIS hub[0].usb_frame_num.frame_num &= (1024-1);

  // If the "global reset" bit was set by software, we need
  // to set the reset bit in each "active" port for 10ms
  if (BX_USB_THIS global_reset) {
    for (i=0; i<USB_NUM_PORTS; i++) {
      BX_USB_THIS hub[0].usb_port[i].able_changed = 0;
      BX_USB_THIS hub[0].usb_port[i].connect_changed = 0;
      BX_USB_THIS hub[0].usb_port[i].enabled = 0;
      BX_USB_THIS hub[0].usb_port[i].line_dminus = 0;
      BX_USB_THIS hub[0].usb_port[i].line_dplus = 0;
      BX_USB_THIS hub[0].usb_port[i].low_speed = 0;
      BX_USB_THIS hub[0].usb_port[i].reset = 1;
      BX_USB_THIS hub[0].usb_port[i].resume = 0;
      BX_USB_THIS hub[0].usb_port[i].status = 0;
      BX_USB_THIS hub[0].usb_port[i].suspend = 0;
    }
    BX_USB_THIS global_reset--;
  } else {
    for (i=0; i<USB_NUM_PORTS; i++)
      BX_USB_THIS hub[0].usb_port[i].reset = 0;
  }

  // If command.schedule = 0, then we need to set Status.Halted
  if (!BX_USB_THIS hub[0].usb_command.schedule)
    BX_USB_THIS hub[0].usb_status.host_halted = 1;


  // TODO:
  //  If ins Global_Suspend mode and any of usb_port[i] bits 6,3, or 1 are set,
  //    we need to issue a Global_Resume (set the global resume bit).
  //    However, since we don't do anything, let's not.

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
  BX_DEBUG(("Experimental USB PCI read register 0x%02x %svalue 0x%s",
            address, pszName, szTmp));
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

  if (io_len > 4 || io_len == 0) {
    BX_ERROR(("Experimental USB PCI write register 0x%02x, len=%u !",
             (unsigned) address, (unsigned) io_len));
    return;
  }

  // This odd code is to display only what bytes actually were written.
  char szTmp[9];
  char szTmp2[3];
  szTmp[0] = '\0';
  szTmp2[0] = '\0';
  for (unsigned i=0; i<io_len; i++) {
    const Bit8u value8 = (value >> (i*8)) & 0xFF;
    switch (address+i) {
      case 0x20: // Base address
        BX_USB_THIS hub[0].pci_conf[address+i] = (value8 & 0xe0) | 0x01;
        sprintf(szTmp2, "%02x", (value8 & 0xe0) | 0x01);
        break;
      case 0x10: // Reserved
      case 0x11: //
      case 0x12: //
      case 0x13: //
      case 0x14: //
      case 0x15: //
      case 0x16: //
      case 0x17: //
      case 0x18: //
      case 0x19: //
      case 0x1a: //
      case 0x1b: //
      case 0x1c: //
      case 0x1d: //
      case 0x1e: //
      case 0x1f: //
      case 0x22: // Always 0
      case 0x23: //
      case 0x24: // Reserved
      case 0x25: //
      case 0x26: //
      case 0x27: //
      case 0x30: // Oh, no, you're not writing to rom_base!
      case 0x31: //
      case 0x32: //
      case 0x33: //
      case 0x3d: //
      case 0x05: // disallowing write to command hi-byte
      case 0x06: // disallowing write to status lo-byte (is that expected?)
        strcpy(szTmp2, "..");
        break;
      default:
        BX_USB_THIS hub[0].pci_conf[address+i] = value8;
        sprintf(szTmp2, "%02x", value8);
    }
    strrev(szTmp2);
    strcat(szTmp, szTmp2);
  }
  strrev(szTmp);
  BX_DEBUG(("Experimental USB PCI write register 0x%02x value 0x%s", address, szTmp));
}

#endif // BX_PCI_SUPPORT && BX_PCI_USB_SUPPORT
