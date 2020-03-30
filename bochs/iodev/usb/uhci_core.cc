/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2017  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2009-2020  The Bochs Project
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

// USB UHCI adapter core
// - PIIX3/PIIX4 function 2
// - ICH4 EHCI function 0, 1 and 2

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
#include "uhci_core.h"

#define LOG_THIS

//#define UHCI_FULL_DEBUG

const Bit8u uhci_iomask[32] = {2, 1, 2, 1, 2, 1, 2, 0, 4, 0, 0, 0, 1, 0, 0, 0,
                              3, 1, 3, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// the device object

bx_uhci_core_c::bx_uhci_core_c()
{
  put("uhci_core", "UHCIC");
  memset((void*)&hub, 0, sizeof(bx_uhci_core_t));
  hub.timer_index = BX_NULL_TIMER_HANDLE;
}

bx_uhci_core_c::~bx_uhci_core_c()
{
  BX_DEBUG(("Exit"));
}

void bx_uhci_core_c::init_uhci(Bit8u devfunc, Bit16u devid, Bit8u headt, Bit8u intp)
{
  // Call our timer routine every 1mS (1,000uS)
  // Continuous and active
  hub.timer_index =
    DEV_register_timer(this, uhci_timer_handler, 1000, 1,1, "usb.timer");

  hub.devfunc = devfunc;
  DEV_register_pci_handlers(this, &hub.devfunc, BX_PLUGIN_USB_UHCI,
                            "USB UHCI");

  // initialize readonly registers
  init_pci_conf(0x8086, devid, 0x01, 0x0c0300, headt, intp);
  init_bar_io(4, 32, read_handler, write_handler, &uhci_iomask[0]);

  for (int i=0; i<USB_UHCI_PORTS; i++) {
    hub.usb_port[i].device = NULL;
  }
  packets = NULL;
}

void bx_uhci_core_c::reset_uhci(unsigned type)
{
  unsigned i, j;

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
        pci_conf[reset_vals[i].addr] = reset_vals[i].val;
    }
  }

  // reset locals
  busy = 0;
  global_reset = 0;

  // Put the USB registers into their RESET state
  hub.usb_command.max_packet_size = 0;
  hub.usb_command.configured = 0;
  hub.usb_command.debug = 0;
  hub.usb_command.resume = 0;
  hub.usb_command.suspend = 0;
  hub.usb_command.reset = 0;
  hub.usb_command.host_reset = 0;
  hub.usb_command.schedule = 0;
  hub.usb_status.error_interrupt = 0;
  hub.usb_status.host_error = 0;
  hub.usb_status.host_halted = 0;
  hub.usb_status.interrupt = 0;
  hub.usb_status.status2 = 0;
  hub.usb_status.pci_error = 0;
  hub.usb_status.resume = 0;
  hub.usb_enable.short_packet = 0;
  hub.usb_enable.on_complete = 0;
  hub.usb_enable.resume = 0;
  hub.usb_enable.timeout_crc = 0;
  hub.usb_frame_num.frame_num = 0x0000;
  hub.usb_frame_base.frame_base = 0x00000000;
  hub.usb_sof.sof_timing = 0x40;
  for (j=0; j<USB_UHCI_PORTS; j++) {
    hub.usb_port[j].connect_changed = 0;
    hub.usb_port[j].line_dminus = 0;
    hub.usb_port[j].line_dplus = 0;
    hub.usb_port[j].low_speed = 0;
    hub.usb_port[j].reset = 0;
    hub.usb_port[j].resume = 0;
    hub.usb_port[j].suspend = 0;
    hub.usb_port[j].enabled = 0;
    hub.usb_port[j].able_changed = 0;
    hub.usb_port[j].status = 0;
    if (hub.usb_port[j].device != NULL) {
      set_connect_status(j, hub.usb_port[j].device->get_type(), 1);
    }
  }
  while (packets != NULL) {
    usb_cancel_packet(&packets->packet);
    remove_async_packet(&packets, packets);
  }
}

void bx_uhci_core_c::register_state(bx_list_c *parent)
{
  unsigned j;
  char portnum[8];
  bx_list_c *hub1, *usb_cmd, *usb_st, *usb_en, *port;

  bx_list_c *list = new bx_list_c(parent, "usb_uhci", "USB UHCI State");
  hub1 = new bx_list_c(list, "hub");
  usb_cmd = new bx_list_c(hub1, "usb_command");
  BXRS_PARAM_BOOL(usb_cmd, max_packet_size, hub.usb_command.max_packet_size);
  BXRS_PARAM_BOOL(usb_cmd, configured, hub.usb_command.configured);
  BXRS_PARAM_BOOL(usb_cmd, debug, hub.usb_command.debug);
  BXRS_PARAM_BOOL(usb_cmd, resume, hub.usb_command.resume);
  BXRS_PARAM_BOOL(usb_cmd, suspend, hub.usb_command.suspend);
  BXRS_PARAM_BOOL(usb_cmd, reset, hub.usb_command.reset);
  BXRS_PARAM_BOOL(usb_cmd, host_reset, hub.usb_command.host_reset);
  BXRS_PARAM_BOOL(usb_cmd, schedule, hub.usb_command.schedule);
  usb_st = new bx_list_c(hub1, "usb_status");
  BXRS_PARAM_BOOL(usb_st, host_halted, hub.usb_status.host_halted);
  BXRS_PARAM_BOOL(usb_st, host_error, hub.usb_status.host_error);
  BXRS_PARAM_BOOL(usb_st, pci_error, hub.usb_status.pci_error);
  BXRS_PARAM_BOOL(usb_st, resume, hub.usb_status.resume);
  BXRS_PARAM_BOOL(usb_st, error_interrupt, hub.usb_status.error_interrupt);
  BXRS_PARAM_BOOL(usb_st, interrupt, hub.usb_status.interrupt);
  BXRS_HEX_PARAM_FIELD(usb_st, status2, hub.usb_status.status2);
  usb_en = new bx_list_c(hub1, "usb_enable");
  BXRS_PARAM_BOOL(usb_en, short_packet, hub.usb_enable.short_packet);
  BXRS_PARAM_BOOL(usb_en, on_complete, hub.usb_enable.on_complete);
  BXRS_PARAM_BOOL(usb_en, resume, hub.usb_enable.resume);
  BXRS_PARAM_BOOL(usb_en, timeout_crc, hub.usb_enable.timeout_crc);
  BXRS_HEX_PARAM_FIELD(hub1, frame_num, hub.usb_frame_num.frame_num);
  BXRS_HEX_PARAM_FIELD(hub1, frame_base, hub.usb_frame_base.frame_base);
  BXRS_HEX_PARAM_FIELD(hub1, sof_timing, hub.usb_sof.sof_timing);
  for (j=0; j<USB_UHCI_PORTS; j++) {
    sprintf(portnum, "port%d", j+1);
    port = new bx_list_c(hub1, portnum);
    BXRS_PARAM_BOOL(port, suspend, hub.usb_port[j].suspend);
    BXRS_PARAM_BOOL(port, reset, hub.usb_port[j].reset);
    BXRS_PARAM_BOOL(port, low_speed, hub.usb_port[j].low_speed);
    BXRS_PARAM_BOOL(port, resume, hub.usb_port[j].resume);
    BXRS_PARAM_BOOL(port, line_dminus, hub.usb_port[j].line_dminus);
    BXRS_PARAM_BOOL(port, line_dplus, hub.usb_port[j].line_dplus);
    BXRS_PARAM_BOOL(port, able_changed, hub.usb_port[j].able_changed);
    BXRS_PARAM_BOOL(port, enabled, hub.usb_port[j].enabled);
    BXRS_PARAM_BOOL(port, connect_changed, hub.usb_port[j].connect_changed);
    BXRS_PARAM_BOOL(port, status, hub.usb_port[j].status);
    // empty list for USB device state
    new bx_list_c(port, "device");
  }
  // TODO: handle async packets
  register_pci_state(hub1);

  BXRS_PARAM_BOOL(list, busy, busy);
  BXRS_DEC_PARAM_FIELD(list, global_reset, global_reset);
}

void bx_uhci_core_c::after_restore_state(void)
{
  bx_pci_device_c::after_restore_pci_state(NULL);
  for (int j=0; j<USB_UHCI_PORTS; j++) {
    if (hub.usb_port[j].device != NULL) {
      hub.usb_port[j].device->after_restore_state();
    }
  }
}

void bx_uhci_core_c::update_irq()
{
  bx_bool level;

  if (((hub.usb_status.status2 & 1) && (hub.usb_enable.on_complete)) ||
      ((hub.usb_status.status2 & 2) && (hub.usb_enable.short_packet)) ||
      ((hub.usb_status.error_interrupt) && (hub.usb_enable.timeout_crc)) ||
      ((hub.usb_status.resume) && (hub.usb_enable.resume)) ||
      (hub.usb_status.pci_error) ||
      (hub.usb_status.host_error)) {
    level = 1;
  } else {
    level = 0;
  }
  DEV_pci_set_irq(hub.devfunc, pci_conf[0x3d], level);
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_uhci_core_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
  bx_uhci_core_c *class_ptr = (bx_uhci_core_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_uhci_core_c::read(Bit32u address, unsigned io_len)
{
  Bit32u val = 0x0;
  Bit8u  offset,port;

  // if the host driver has not cleared the reset bit, do nothing (reads are
  // undefined)
  if (hub.usb_command.reset)
    return 0;

  offset = address - pci_bar[4].addr;

  switch (offset) {
    case 0x00: // command register (16-bit)
      val =   hub.usb_command.max_packet_size << 7
            | hub.usb_command.configured << 6
            | hub.usb_command.debug << 5
            | hub.usb_command.resume << 4
            | hub.usb_command.suspend << 3
            | hub.usb_command.reset << 2
            | hub.usb_command.host_reset << 1
            | hub.usb_command.schedule;
      break;

    case 0x02: // status register (16-bit)
      val = hub.usb_status.host_halted << 5
            | hub.usb_status.host_error << 4
            | hub.usb_status.pci_error << 3
            | hub.usb_status.resume << 2
            | hub.usb_status.error_interrupt << 1
            | hub.usb_status.interrupt;
      break;

    case 0x04: // interrupt enable register (16-bit)
      val = hub.usb_enable.short_packet << 3
            | hub.usb_enable.on_complete << 2
            | hub.usb_enable.resume << 1
            | hub.usb_enable.timeout_crc;
      break;

    case 0x06: // frame number register (16-bit)
      val = hub.usb_frame_num.frame_num;
      break;

    case 0x08: // frame base register (32-bit)
      val = hub.usb_frame_base.frame_base;
      break;

    case 0x0C: // start of Frame Modify register (8-bit)
      val = hub.usb_sof.sof_timing;
      break;

    case 0x14: // port #3 non existent, but linux systems check it to see if there are more than 2
      BX_ERROR(("read from non existent offset 0x14 (port #3)"));
      val = 0xFF7F;
      break;

    case 0x10: // port #1
    case 0x11:
    case 0x12: // port #2
    case 0x13:
      port = (offset & 0x0F) >> 1;
      if (port < USB_UHCI_PORTS) {
        val = hub.usb_port[port].suspend << 12
              |                                       1 << 10  // some Root Hubs have bit 10 set ?????
              | hub.usb_port[port].reset << 9
              | hub.usb_port[port].low_speed << 8
              | 1 << 7
              | hub.usb_port[port].resume << 6
              | hub.usb_port[port].line_dminus << 5
              | hub.usb_port[port].line_dplus << 4
              | hub.usb_port[port].able_changed << 3
              | hub.usb_port[port].enabled << 2
              | hub.usb_port[port].connect_changed << 1
              | hub.usb_port[port].status;
        if (offset & 1) val >>= 8;
        break;
      } // else fall through to default
    default:
      val = 0xFF7F; // keep compiler happy
      BX_ERROR(("unsupported io read from address=0x%04x!", (unsigned) address));
      break;
  }

  // don't flood the log with reads from the Frame Register
  if (offset != 0x06)
    BX_DEBUG(("register read from address 0x%04X:  0x%08X (%2i bits)", (unsigned) address, (Bit32u) val, io_len * 8));

  return(val);
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_uhci_core_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
  bx_uhci_core_c *class_ptr = (bx_uhci_core_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_uhci_core_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
  Bit8u offset, port;

  offset = address - pci_bar[4].addr;

  // if the reset bit is not cleared and this write is not clearing the bit,
  // do nothing
  if (hub.usb_command.reset && ((offset != 0) || (value & 0x04)))
    return;

  BX_DEBUG(("register write to  address 0x%04X:  0x%08X (%2i bits)", (unsigned) address, (unsigned) value, io_len * 8));

  switch (offset) {
    case 0x00: // command register (16-bit) (R/W)
      if (value & 0xFF00)
        BX_DEBUG(("write to command register with bits 15:8 not zero: 0x%04x", value));

      hub.usb_command.max_packet_size = (value & 0x80) ? 1: 0;
      hub.usb_command.configured = (value & 0x40) ? 1: 0;
      hub.usb_command.debug = (value & 0x20) ? 1: 0;
      hub.usb_command.resume = (value & 0x10) ? 1: 0;
      hub.usb_command.suspend = (value & 0x08) ? 1: 0;
      hub.usb_command.reset = (value & 0x04) ? 1: 0;
      hub.usb_command.host_reset = (value & 0x02) ? 1: 0;
      hub.usb_command.schedule = (value & 0x01) ? 1: 0;

      // HCRESET
      if (hub.usb_command.host_reset) {
        reset_uhci(0);
        for (unsigned i=0; i<USB_UHCI_PORTS; i++) {
          if (hub.usb_port[i].status) {
            if (hub.usb_port[i].device != NULL) {
              hub.usb_port[i].device->usb_send_msg(USB_MSG_RESET);
            }
            hub.usb_port[i].connect_changed = 1;
            if (hub.usb_port[i].enabled) {
              hub.usb_port[i].able_changed = 1;
              hub.usb_port[i].enabled = 0;
            }
          }
        }
      }

      // If software set the GRESET bit, we need to send the reset to all USB.
      // The software should guarentee that the reset is for at least 10ms.
      // We hold the reset until software resets this bit
      if (hub.usb_command.reset) {
        global_reset = 1;
        BX_DEBUG(("Global Reset"));
      } else {
        // if software cleared the reset, then we need to reset the usb registers.
        if (global_reset) {
          global_reset = 0;
          unsigned int running = hub.usb_command.schedule;
          reset_uhci(0);
          hub.usb_status.host_halted = (running) ? 1 : 0;
        }
      }

      // If Run/Stop, identify in log
      if (hub.usb_command.schedule) {
        hub.usb_status.host_halted = 0;
        BX_DEBUG(("Schedule bit set in Command register"));
      } else {
        hub.usb_status.host_halted = 1;
        BX_DEBUG(("Schedule bit clear in Command register"));
      }

      // If Debug mode set, panic.  Not implemented
      if (hub.usb_command.debug)
        BX_PANIC(("Software set DEBUG bit in Command register. Not implemented"));

      break;

    case 0x02: // status register (16-bit) (R/WC)
      if (value & 0xFFC0)
        BX_DEBUG(("write to status register with bits 15:6 not zero: 0x%04x", value));

      // host_halted, even though not specified in the specs, is read only
      //hub.usb_status.host_halted = (value & 0x20) ? 0: hub.usb_status.host_halted;
      hub.usb_status.host_error = (value & 0x10) ? 0: hub.usb_status.host_error;
      hub.usb_status.pci_error = (value & 0x08) ? 0: hub.usb_status.pci_error;
      hub.usb_status.resume = (value & 0x04) ? 0: hub.usb_status.resume;
      hub.usb_status.error_interrupt = (value & 0x02) ? 0: hub.usb_status.error_interrupt;
      hub.usb_status.interrupt = (value & 0x01) ? 0: hub.usb_status.interrupt;
      if (value & 0x01) {
        hub.usb_status.status2 = 0;
      }
      update_irq();
      break;

    case 0x04: // interrupt enable register (16-bit)
      if (value & 0xFFF0)
        BX_DEBUG(("write to interrupt enable register with bits 15:4 not zero: 0x%04x", value));

      hub.usb_enable.short_packet  = (value & 0x08) ? 1: 0;
      hub.usb_enable.on_complete  = (value & 0x04) ? 1: 0;
      hub.usb_enable.resume  = (value & 0x02) ? 1: 0;
      hub.usb_enable.timeout_crc = (value & 0x01) ? 1: 0;

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

      if (hub.usb_status.host_halted)
        hub.usb_frame_num.frame_num = (value & 0x07FF);
      else
        // ignored by the hardward, but lets report it anyway
        BX_DEBUG(("write to frame number register with STATUS.HALTED == 0"));
      break;

    case 0x08: // frame base register (32-bit)
      if (value & 0xFFF)
        BX_DEBUG(("write to frame base register with bits 11:0 not zero: 0x%08x", value));

      hub.usb_frame_base.frame_base = (value & ~0xfff);
      break;

    case 0x0C: // start of Frame Modify register (8-bit)
      if (value & 0x80)
        BX_DEBUG(("write to SOF Modify register with bit 7 not zero: 0x%04x", value));

       hub.usb_sof.sof_timing = value;
       break;

    case 0x14: // port #3 non existent, but linux systems check it to see if there are more than 2
      BX_ERROR(("write to non existent offset 0x14 (port #3)"));
      break;

    case 0x10: // port #1
    case 0x12: // port #2
      port = (offset & 0x0F) >> 1;
      if ((port < USB_UHCI_PORTS) && (io_len == 2)) {
        // If the ports reset bit is set, don't allow any writes unless the new write will clear the reset bit
        if (hub.usb_port[port].reset & (value & (1<<9)))
          break;
        if (value & ((1<<5) | (1<<4) | (1<<0)))
          BX_DEBUG(("write to one or more read-only bits in port #%d register: 0x%04x", port+1, value));
        if (!(value & (1<<7)))
          BX_DEBUG(("write to port #%d register bit 7 = 0", port+1));
        if (value & (1<<8))
          BX_DEBUG(("write to bit 8 in port #%d register ignored", port+1));
        if ((value & (1<<12)) && hub.usb_command.suspend)
          BX_DEBUG(("write to port #%d register bit 12 when in Global-Suspend", port+1));

        hub.usb_port[port].suspend = (value & (1<<12)) ? 1 : 0;
        hub.usb_port[port].reset = (value & (1<<9)) ? 1 : 0;
        hub.usb_port[port].resume = (value & (1<<6)) ? 1 : 0;
        if (!hub.usb_port[port].enabled && (value & (1<<2)))
          hub.usb_port[port].able_changed = 0;
        else
          if ((value & (1<<3)) != 0) hub.usb_port[port].able_changed = 0;
        hub.usb_port[port].enabled = (value & (1<<2)) ? 1 : 0;
        if ((value & (1<<1)) != 0) hub.usb_port[port].connect_changed = 0;

        // if port reset, reset function(s)
        //TODO: only reset items on the downstream...
        // for now, reset the one and only
        // TODO: descriptors, etc....
        if (hub.usb_port[port].reset) {
          hub.usb_port[port].suspend = 0;
          hub.usb_port[port].resume = 0;
          hub.usb_port[port].enabled = 0;
          // are we are currently connected/disconnected
          if (hub.usb_port[port].status) {
            if (hub.usb_port[port].device != NULL) {
              hub.usb_port[port].low_speed =
                (hub.usb_port[port].device->get_speed() == USB_SPEED_LOW);
              set_connect_status(port, hub.usb_port[port].device->get_type(), 1);
              hub.usb_port[port].device->usb_send_msg(USB_MSG_RESET);
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

void bx_uhci_core_c::uhci_timer_handler(void *this_ptr)
{
  bx_uhci_core_c *class_ptr = (bx_uhci_core_c *) this_ptr;
  class_ptr->uhci_timer();
}

// Called once every 1ms
#define USB_STACK_SIZE  256
void bx_uhci_core_c::uhci_timer(void)
{
  int i;

  // If the "global reset" bit was set by software
  if (global_reset) {
    for (i=0; i<USB_UHCI_PORTS; i++) {
      hub.usb_port[i].able_changed = 0;
      hub.usb_port[i].connect_changed = 0;
      hub.usb_port[i].enabled = 0;
      hub.usb_port[i].line_dminus = 0;
      hub.usb_port[i].line_dplus = 0;
      hub.usb_port[i].low_speed = 0;
      hub.usb_port[i].reset = 0;
      hub.usb_port[i].resume = 0;
      hub.usb_port[i].status = 0;
      hub.usb_port[i].suspend = 0;
    }
    return;
  }

  // If command.schedule = 1, then run schedule
  //  *** This assumes that we can complete the frame within the 1ms time allowed ***
  // Actually, not complete, but reach the end of the frame.  This means that there may still
  //  be TDs and QHs that were BREADTH defined and will be executed on the next cycle/iteration.

  if (busy) {
    BX_PANIC(("Did not complete last frame before the 1ms was over. Starting next frame."));
    busy = 0;
  }
  if (hub.usb_command.schedule) {
    busy = 1;
    bx_bool interrupt = 0, shortpacket = 0, stalled = 0, was_inactive = 0;
    struct TD td;
    struct HCSTACK stack[USB_STACK_SIZE+1];  // queue stack for this item only
    Bit32s stk = 0;
    Bit32u item, address, lastvertaddr = 0, queue_num = 0;
    Bit32u frame, frm_addr = hub.usb_frame_base.frame_base +
                                (hub.usb_frame_num.frame_num << 2);
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
#if UHCI_FULL_DEBUG
          BX_DEBUG(("Queue %3i: 0x%08X %i %i  0x%08X %i %i", queue_num,
            stack[stk-1].next, stack[stk-1].q, stack[stk-1].t,
            stack[stk].next, stack[stk].q, stack[stk].t));
#endif
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
          if (td.dword1 & (1<<23)) {  // is it an active TD
            BX_DEBUG(("Frame: %04i (0x%04X)", hub.usb_frame_num.frame_num, hub.usb_frame_num.frame_num));
            if (DoTransfer(address, queue_num, &td)) {
              if (td.dword1 & (1<<24)) interrupt = 1;
              // issue short packet?
              Bit16u r_actlen = (((td.dword1 & 0x7FF)+1) & 0x7FF);
              Bit16u r_maxlen = (((td.dword2>>21)+1) & 0x7FF);
              BX_DEBUG((" r_actlen = 0x%04X r_maxlen = 0x%04X", r_actlen, r_maxlen));
              if (((td.dword2 & 0xFF) == USB_TOKEN_IN) && spd && stk && (r_actlen < r_maxlen) && ((td.dword1 & 0x00FF0000) == 0)) {
                BX_DEBUG(("Short Packet Detected"));
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
            was_inactive = 0;
          } else
            was_inactive = 1;

          if (stk > 0) {
            // if last TD in HORZ queue pointer, then we are done.
            if (stack[stk].t && (stack[stk].d == HC_HORZ)) break;
            // if Breadth first or last item in queue, move to next queue.
            if (was_inactive || !depthbreadth || stack[stk].t) {
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
      if (shortpacket) hub.usb_status.status2 |= 2;
      if (shortpacket && hub.usb_enable.short_packet) {
        BX_DEBUG((" [SPD] We want it to fire here (Frame: %04i)", hub.usb_frame_num.frame_num));
      }

      // if one of the TD's in this frame had the ioc bit set, we need to
      //   raise an interrupt, if interrupts are not masked via interrupt register.
      //   always set the status register if IOC.
      hub.usb_status.status2 |= interrupt;
      if (interrupt && hub.usb_enable.on_complete) {
        BX_DEBUG((" [IOC] We want it to fire here (Frame: %04i)", hub.usb_frame_num.frame_num));
      }

      hub.usb_status.error_interrupt |= stalled;
      if (stalled && hub.usb_enable.timeout_crc) {
        BX_DEBUG((" [stalled] We want it to fire here (Frame: %04i)", hub.usb_frame_num.frame_num));
      }
    }

    // The Frame Number Register is incremented every 1ms
    hub.usb_frame_num.frame_num++;
    hub.usb_frame_num.frame_num &= (1024-1);

    // The status.interrupt bit should be set regardless of the enable bits if a IOC or SPD is found
    if (interrupt || shortpacket) {
      hub.usb_status.interrupt = 1;
    }
    // if we needed to fire an interrupt now, lets do it *after* we increment the frame_num register
    update_irq();

    busy = 0;  // ready to do next frame item
  }  // end run schedule

  // if host turned off the schedule, set the halted bit in the status register
  // Note: Can not use an else from the if() above since the host can changed this bit
  //  while we are processing a frame.
  if (hub.usb_command.schedule == 0)
    hub.usb_status.host_halted = 1;

  // TODO:
  //  If in Global_Suspend mode and any of usb_port[i] bits 6,3, or 1 are set,
  //    we need to issue a Global_Resume (set the global resume bit).
  //    However, since we don't do anything, let's not.
}

void uhci_event_handler(int event, USBPacket *packet, void *dev, int port)
{
  ((bx_uhci_core_c*)dev)->event_handler(event, packet, port);
}

void bx_uhci_core_c::event_handler(int event, USBPacket *packet, int port)
{
  if (event == USB_EVENT_ASYNC) {
    BX_DEBUG(("Async packet completion"));
    USBAsync *p = container_of_usb_packet(packet);
    p->done = 1;
  } else if (event == USB_EVENT_WAKEUP) {
    if (hub.usb_port[port].suspend && !hub.usb_port[port].resume) {
      hub.usb_port[port].resume = 1;
    }
    // if in suspend state, signal resume
    if (hub.usb_command.suspend) {
      hub.usb_command.resume = 1;
      hub.usb_status.resume = 1;
      if (hub.usb_enable.resume) {
        hub.usb_status.interrupt = 1;
      }
      update_irq();
    }
  } else {
    BX_ERROR(("unknown/unsupported event (id=%d) on port #%d", event, port+1));
  }
}

bx_bool bx_uhci_core_c::DoTransfer(Bit32u address, Bit32u queue_num, struct TD *td) {

  int len = 0, ret = 0;
  USBAsync *p;
  bx_bool completion;

  Bit16u maxlen = (td->dword2 >> 21);
  Bit8u  addr   = (td->dword2 >> 8) & 0x7F;
  Bit8u  endpt  = (td->dword2 >> 15) & 0x0F;
  Bit8u  pid    =  td->dword2 & 0xFF;

  p = find_async_packet(&packets, address);
  completion = (p != NULL);
  if (completion && !p->done) {
    return 0;
  }

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

  if (completion) {
    ret = p->packet.len;
  } else {
    p = create_async_packet(&packets, address, maxlen);
    p->packet.pid = pid;
    p->packet.devaddr = addr;
    p->packet.devep = endpt;
    p->packet.complete_cb = uhci_event_handler;
    p->packet.complete_dev = this;
    switch (pid) {
      case USB_TOKEN_OUT:
      case USB_TOKEN_SETUP:
        if (maxlen > 0) {
          DEV_MEM_READ_PHYSICAL_DMA(td->dword3, maxlen, p->packet.data);
        }
        ret = broadcast_packet(&p->packet);
        len = maxlen;
        break;
      case USB_TOKEN_IN:
        ret = broadcast_packet(&p->packet);
        break;
      default:
        hub.usb_status.host_error = 1;
        update_irq();
        return 0;
    }
    if (ret == USB_RET_ASYNC) {
      BX_DEBUG(("Async packet deferred"));
      return 0;
    }
  }
  if (pid == USB_TOKEN_IN) {
    if (ret >= 0) {
      len = ret;
      if (len > maxlen) {
        len = maxlen;
        ret = USB_RET_BABBLE;
      }
      if (len > 0) {
        DEV_MEM_WRITE_PHYSICAL_DMA(td->dword3, len, p->packet.data);
      }
    } else {
      len = 0;
    }
  }
  if (ret >= 0) {
    set_status(td, 0, 0, 0, 0, 0, 0, len-1);
  } else if (ret == USB_RET_NAK) {
    set_status(td, 0, 0, 0, 1, 0, 0, len-1); // NAK
  } else {
    set_status(td, 1, 0, 0, 0, 0, 0, 0x007); // stalled
  }
  remove_async_packet(&packets, p);
  return 1;
}

int bx_uhci_core_c::broadcast_packet(USBPacket *p)
{
  int i, ret;

  ret = USB_RET_NODEV;
  for (i = 0; i < USB_UHCI_PORTS && ret == USB_RET_NODEV; i++) {
    if ((hub.usb_port[i].device != NULL) &&
        (hub.usb_port[i].enabled)) {
      ret = hub.usb_port[i].device->handle_packet(p);
    }
  }
  return ret;
}

// If the request fails, set the stall bit ????
void bx_uhci_core_c::set_status(struct TD *td, bx_bool stalled, bx_bool data_buffer_error, bx_bool babble,
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
  if (stalled || data_buffer_error || babble || crc_time_out || bitstuff_error)
    td->dword1 &= ~((1<<28) | (1<<27));  // clear the c_err field in there was an error
}


// pci configuration space write callback handler
void bx_uhci_core_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  if (((address >= 0x10) && (address < 0x20)) ||
      ((address > 0x23) && (address < 0x34)))
    return;

  BX_DEBUG_PCI_WRITE(address, value, io_len);
  for (unsigned i=0; i<io_len; i++) {
    Bit8u value8 = (value >> (i*8)) & 0xFF;
//  Bit8u oldval = pci_conf[address+i];
    switch (address+i) {
      case 0x04:
        value8 &= 0x05;
        pci_conf[address+i] = value8;
        break;
      case 0x3d: //
      case 0x3e: //
      case 0x3f: //
      case 0x05: // disallowing write to command hi-byte
      case 0x06: // disallowing write to status lo-byte (is that expected?)
        break;
      default:
        pci_conf[address+i] = value8;
    }
  }
}

const char *usb_speed[4] = {
  "low",
  "full",
  "high",
  "super"
};

void bx_uhci_core_c::set_connect_status(Bit8u port, int type, bx_bool connected)
{
  usb_device_c *device = hub.usb_port[port].device;
  if (device != NULL) {
    if (device->get_type() == type) {
      if (connected) {
        BX_DEBUG(("port #%d: speed = %s", port+1, usb_speed[device->get_speed()]));
        switch (device->get_speed()) {
          case USB_SPEED_LOW:
            hub.usb_port[port].low_speed = 1;
            break;
          case USB_SPEED_FULL:
            hub.usb_port[port].low_speed = 0;
            break;
          case USB_SPEED_HIGH:
          case USB_SPEED_SUPER:
            BX_ERROR(("HC ignores device with unsupported speed"));
            return;
          default:
            BX_PANIC(("USB device returned invalid speed value"));
            set_connect_status(port, type, 0);
            return;
        }
        if (hub.usb_port[port].low_speed) {
          hub.usb_port[port].line_dminus = 1;  //  dminus=1 & dplus=0 = low speed  (at idle time)
          hub.usb_port[port].line_dplus = 0;   //  dminus=0 & dplus=1 = high speed (at idle time)
        } else {
          hub.usb_port[port].line_dminus = 0;
          hub.usb_port[port].line_dplus = 1;
        }
        hub.usb_port[port].status = 1;
        hub.usb_port[port].connect_changed = 1;

        // if in suspend state, signal resume
        if (hub.usb_command.suspend) {
          hub.usb_port[port].resume = 1;
          hub.usb_status.resume = 1;
          if (hub.usb_enable.resume) {
            hub.usb_status.interrupt = 1;
          }
          update_irq();
        }

        if (!device->get_connected()) {
          if (!device->init()) {
            set_connect_status(port, type, 0);
            BX_ERROR(("port #%d: connect failed", port+1));
            return;
          } else {
            BX_INFO(("port #%d: connect: %s", port+1, device->get_info()));
          }
        }
        device->set_event_handler(this, uhci_event_handler, port);
      } else {
        hub.usb_port[port].status = 0;
        hub.usb_port[port].connect_changed = 1;
        if (hub.usb_port[port].enabled) {
          hub.usb_port[port].able_changed = 1;
          hub.usb_port[port].enabled = 0;
        }
        hub.usb_port[port].low_speed = 0;
        hub.usb_port[port].line_dminus = 0;
        hub.usb_port[port].line_dplus = 0;
      }
    }
  }
}

void bx_uhci_core_c::set_port_device(int port, usb_device_c *dev)
{
  usb_device_c *olddev = hub.usb_port[port].device;
  if ((dev != NULL) && (olddev == NULL)) {
    hub.usb_port[port].device = dev;
    set_connect_status(port, dev->get_type(), 1);
  } else if ((dev == NULL) && (olddev != NULL)) {
    set_connect_status(port, olddev->get_type(), 0);
    hub.usb_port[port].device = dev;
  }
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_USB_UHCI
