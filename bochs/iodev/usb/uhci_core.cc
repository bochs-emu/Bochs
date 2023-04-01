/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2023  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2009-2023  The Bochs Project
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
   - 10 Feb 2023:
     I have re-written some of this code, especially the stack process (uhci_timer()).
     The control/bulk reclamation was not working, so a re-write was in order.
     It is amazing what you will learn, especially nearly 20 years later :-)
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
  /*  If you wish to set DEBUG=report in the code, instead of
   *  in the configuration, simply uncomment this line.  I use
   *  it when I am working on this emulation.
   */
  //LOG_THIS setonoff(LOGLEV_DEBUG, ACT_REPORT);

  // Call our timer routine every 1mS (1,000uS)
  // Continuous and active
  hub.timer_index =
    DEV_register_timer(this, uhci_timer_handler, 1000, 1, 1, "usb.timer");

  hub.devfunc = devfunc;
  DEV_register_pci_handlers(this, &hub.devfunc, BX_PLUGIN_USB_UHCI,
                            "USB UHCI");

  // initialize readonly registers
  init_pci_conf(0x8086, devid, 0x01, 0x0C0300, headt, intp);
  init_bar_io(4, 32, read_handler, write_handler, &uhci_iomask[0]);

  for (int i=0; i<USB_UHCI_PORTS; i++) {
    hub.usb_port[i].device = NULL;
  }
  packets = NULL;

  hub.max_bandwidth = USB_UHCI_STD_MAX_BANDWIDTH;
  hub.loop_reached = 0;
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
    hub.usb_port[j].over_current_change = 0;
    hub.usb_port[j].over_current = 0;
    hub.usb_port[j].enabled = 0;
    hub.usb_port[j].enable_changed = 0;
    hub.usb_port[j].status = 0;
    if (hub.usb_port[j].device != NULL) {
      set_connect_status(j, 1);
    }
  }
  while (packets != NULL) {
    usb_cancel_packet(&packets->packet);
    remove_async_packet(&packets, packets);
  }
}

void bx_uhci_core_c::uhci_register_state(bx_list_c *parent)
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
    BXRS_PARAM_BOOL(port, over_current_change, hub.usb_port[j].over_current_change);
    BXRS_PARAM_BOOL(port, over_current, hub.usb_port[j].over_current);
    BXRS_PARAM_BOOL(port, reset, hub.usb_port[j].reset);
    BXRS_PARAM_BOOL(port, low_speed, hub.usb_port[j].low_speed);
    BXRS_PARAM_BOOL(port, resume, hub.usb_port[j].resume);
    BXRS_PARAM_BOOL(port, line_dminus, hub.usb_port[j].line_dminus);
    BXRS_PARAM_BOOL(port, line_dplus, hub.usb_port[j].line_dplus);
    BXRS_PARAM_BOOL(port, enable_changed, hub.usb_port[j].enable_changed);
    BXRS_PARAM_BOOL(port, enabled, hub.usb_port[j].enabled);
    BXRS_PARAM_BOOL(port, connect_changed, hub.usb_port[j].connect_changed);
    BXRS_PARAM_BOOL(port, status, hub.usb_port[j].status);
    // empty list for USB device state
    new bx_list_c(port, "device");
  }
  // TODO: handle async packets
  register_pci_state(hub1);

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
  bool level;

  if (((hub.usb_status.status2 & STATUS2_IOC) && hub.usb_enable.on_complete) ||
      ((hub.usb_status.status2 & STATUS2_SPD) && hub.usb_enable.short_packet) ||
      (hub.usb_status.error_interrupt && hub.usb_enable.timeout_crc) ||
      (hub.usb_status.resume && hub.usb_enable.resume) ||
       hub.usb_status.pci_error ||
       hub.usb_status.host_error) {
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
            | (Bit16u)hub.usb_command.schedule;
      break;

    case 0x02: // status register (16-bit)
      val = hub.usb_status.host_halted << 5
            | hub.usb_status.host_error << 4
            | hub.usb_status.pci_error << 3
            | hub.usb_status.resume << 2
            | hub.usb_status.error_interrupt << 1
            | (Bit16u)hub.usb_status.interrupt;
       break;

    case 0x04: // interrupt enable register (16-bit)
      val = hub.usb_enable.short_packet << 3
            | hub.usb_enable.on_complete << 2
            | hub.usb_enable.resume << 1
            | (Bit16u)hub.usb_enable.timeout_crc;
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
              | hub.usb_port[port].over_current_change << 11
              | hub.usb_port[port].over_current << 10
              | hub.usb_port[port].reset << 9
              | hub.usb_port[port].low_speed << 8
              | 1 << 7
              | hub.usb_port[port].resume << 6
              | hub.usb_port[port].line_dminus << 5
              | hub.usb_port[port].line_dplus << 4
              | hub.usb_port[port].enable_changed << 3
              | hub.usb_port[port].enabled << 2
              | hub.usb_port[port].connect_changed << 1
              | (Bit16u)hub.usb_port[port].status;
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
              hub.usb_port[i].enable_changed = 1;
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
      if (value & 0x01) {
        BX_DEBUG(("Host set Enable Interrupt on Timeout/CRC"));
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
        if (hub.usb_port[port].reset && ((value & (1 << 9)) != 0))
          break;
        if (value & ((1<<5) | (1<<4) | (1<<0)))
          BX_DEBUG(("write to one or more read-only bits in port #%d register: 0x%04x", port+1, value));
        if (!(value & (1<<7)))
          BX_DEBUG(("write to port #%d register bit 7 = 0", port+1));
        if (value & (1<<8))
          BX_DEBUG(("write to bit 8 in port #%d register ignored", port+1));
        if ((value & (1<<12)) && hub.usb_command.suspend)
          BX_DEBUG(("write to port #%d register bit 12 when in Global-Suspend", port+1));

        // some controllers don't successfully reset if the CSC bit is
        //  cleared during a reset. i.e.: if the CSC bit is cleared at
        //  the same time the reset bit is cleared, the controller may
        //  not successfully reset. If the guest is clearing the CSC
        //  bit at the same time it is clearing the reset bit, let's give
        //  an INFO message.
        if (hub.usb_port[port].reset && !(value & (1<<9)) && (value & (1<<1))) {
          BX_INFO(("UHCI Core: Clearing the CSC while clearing the Reset may not successfully reset the port."));
          BX_INFO(("UHCI Core: Clearing the CSC after the Reset has been cleared will ensure a successful reset."));
        }

        hub.usb_port[port].suspend = (value & (1<<12)) ? 1 : 0;
        if (value & (1<<11)) hub.usb_port[port].over_current_change = 0;
        hub.usb_port[port].reset = (value & (1<<9)) ? 1 : 0;
        hub.usb_port[port].resume = (value & (1<<6)) ? 1 : 0;
        if (!hub.usb_port[port].enabled && (value & (1<<2)))
          hub.usb_port[port].enable_changed = 0;
        else
          if (value & (1<<3)) hub.usb_port[port].enable_changed = 0;
        hub.usb_port[port].enabled = (value & (1<<2)) ? 1 : 0;
        if (value & (1<<1)) hub.usb_port[port].connect_changed = 0;

        // if port reset, reset function(s)
        //TODO: only reset items on the downstream...
        // for now, reset the one and only
        // TODO: descriptors, etc....
        if (hub.usb_port[port].reset) {
          hub.usb_port[port].suspend = 0;
          hub.usb_port[port].over_current_change = 0;
          hub.usb_port[port].over_current = 0;
          hub.usb_port[port].resume = 0;
          hub.usb_port[port].enabled = 0;
          // are we are currently connected/disconnected
          if (hub.usb_port[port].status) {
            if (hub.usb_port[port].device != NULL) {
              hub.usb_port[port].low_speed =
                (hub.usb_port[port].device->get_speed() == USB_SPEED_LOW);
              set_connect_status(port, 1);
              hub.usb_port[port].device->usb_send_msg(USB_MSG_RESET);
            }
          }
          BX_DEBUG(("Port%d: Reset", port + 1));
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

// For control and bulk reclamation, most guests will loop back to a certain
//  queue and try to execute more TDs (Queues that are using Breadth First processing).
//  This is by design. However, if there are no TDs to be processed within this loop,
//  we can loop indefinitely, never ending the 1ms frame.
// Therefore, we save a list of queue heads we execute, possibly updating to the
//  next queue head when a loop is found.
// 
// Let's try to add this queue's address to our stack of processed queues.
//  if the queue has already been processed, it will be in this list (return TRUE)
//  if the queue has not been processed yet, return FALSE
// (when we find that we are in a loop, this list gets dumped and starts a new list
//  starting with this one.)
bool bx_uhci_core_c::uhci_add_queue(struct USB_UHCI_QUEUE_STACK *stack, const Bit32u addr) {
  // check to see if this queue has been processed before
  for (int i=0; i<stack->queue_cnt; i++) {
    if (stack->queue_stack[i] == addr)
      return 1;
  }

  // if the stack is full, we return TRUE anyway
  if (stack->queue_cnt == USB_UHCI_QUEUE_STACK_SIZE) {
    if (hub.loop_reached == 0) {
      BX_ERROR(("Ben: We reached our UHCI bandwidth loop limit. Probably should increase it."));
      hub.loop_reached = 1; // don't print it again
    }
    return 1;
  }

  // add the queue's address
  stack->queue_stack[stack->queue_cnt] = addr;
  stack->queue_cnt++;
  
  return 0;
}

// Called once every 1ms
void bx_uhci_core_c::uhci_timer(void)
{
  int i;

  // If the "global reset" bit was set by software
  if (global_reset) {
    for (i=0; i<USB_UHCI_PORTS; i++) {
      hub.usb_port[i].enable_changed = 0;
      hub.usb_port[i].connect_changed = 0;
      hub.usb_port[i].enabled = 0;
      hub.usb_port[i].line_dminus = 0;
      hub.usb_port[i].line_dplus = 0;
      hub.usb_port[i].low_speed = 0;
      hub.usb_port[i].reset = 0;
      hub.usb_port[i].resume = 0;
      hub.usb_port[i].status = 0;
      hub.usb_port[i].over_current = 0;
      hub.usb_port[i].over_current_change = 0;
      hub.usb_port[i].suspend = 0;
    }
    return;
  }
  
  // if the run bit is set, let's see if we can process a few TDs
  if (hub.usb_command.schedule) {
    // our stack of queues we have processed
    struct USB_UHCI_QUEUE_STACK queue_stack;
    int  td_count = 0; // count of TD's processed under a queue
    int  count = USB_UHCI_LOOP_COUNT;
    int  bytes_processed = 0; // The UHCI (USB 1.1) allows up to 1280 bytes to be processed per frame.
    bool interrupt = 0, shortpacket = 0, stalled = 0;
    Bit32u item, queue_addr = 0;
    struct QUEUE queue;
    struct TD td;
    Bit32u address = hub.usb_frame_base.frame_base +
                   ((hub.usb_frame_num.frame_num & 0x3FF) * sizeof(Bit32u));
    
    // reset our queue stack to zero
    queue_stack.queue_cnt = 0;
    
    // read in the frame pointer
    DEV_MEM_READ_PHYSICAL(address, sizeof(Bit32u), (Bit8u *) &item);
    
    //BX_DEBUG(("Start of Frame %d", hub.usb_frame_num.frame_num & 0x3FF));
    
    // start the loop. we allow USB_UHCI_LOOP_COUNT queues to be processed
    while (count--) {
      if (!USB_UHCI_IS_LINK_VALID(item))  // the the T bit is set, we are done
        break;
      
      // is it a queue?
      if (USB_UHCI_IS_LINK_QUEUE(item)) {
        // add it to our current list of queues
        if (uhci_add_queue(&queue_stack, item & ~0xF)) {
          // this queue has been processed before. Did we process
          //  any TD's between the last time and now? If not, be done.
          if (td_count == 0) {
            break;
          } else {
            // reset the queue stack to start here
            td_count = 0;
            queue_stack.queue_cnt = 0;
            uhci_add_queue(&queue_stack, item & ~0xF);
          }
        }
        
        // read in the queue
        DEV_MEM_READ_PHYSICAL(item & ~0xF, sizeof(struct QUEUE), (Bit8u *) &queue);
        
        // this massively populates the log file, so I keep it commented out
        //BX_DEBUG(("Queue at 0x%08X:  horz = 0x%08X, vert = 0x%08X", item & ~0xF, queue.horz, queue.vert));
        
        // if the vert pointer is valid, there are td's in it to process
        //  else only the head pointer may be valid
        if (!USB_UHCI_IS_LINK_VALID(queue.vert)) {
          // no vertical elements to process
          // (clear queue_addr to indicate we are not processing
          //  elements of the vertical part of a queue)
          queue_addr = 0;
          item = queue.horz;
        } else {
          // there are vertical elements to process
          // (save the address of the horz pointer in queue_addr
          //  so that we may update the queue's vertical pointer
          //  member with the successfully processed TD's link pointer)
          queue_addr = item;
          item = queue.vert;
        }
        continue;
      }
      
      // else, we found a Transfer Descriptor
      address = item & ~0xF;
      DEV_MEM_READ_PHYSICAL(address, sizeof(struct TD), (Bit8u *) &td);
      const bool depthbreadth = (td.dword0 & 0x0004) ? 1 : 0;     // 1 = depth first, 0 = breadth first
      const bool is_active = (td.dword1 & (1<<23)) > 0;
      bool was_short = 0, was_stall = 0;
      if (td.dword1 & (1<<24)) interrupt = 1;
      if (is_active) {  // is it an active TD
        const bool spd = (td.dword1 & (1<<29)) > 0;
        if (DoTransfer(address, &td)) {
          // issue short packet?
          const int r_actlen = (((td.dword1 & 0x7FF) + 1) & 0x7FF);
          const int r_maxlen = (((td.dword2 >> 21) + 1) & 0x7FF);
          BX_DEBUG((" r_actlen = %d r_maxlen = %d", r_actlen, r_maxlen));
          if (((td.dword2 & 0xFF) == USB_TOKEN_IN) && (queue_addr != 0) && (r_actlen < r_maxlen) && ((td.dword1 & 0x00FF0000) == 0)) {
            if (spd) {
              BX_DEBUG(("Short Packet Detected"));
              shortpacket = was_short = 1;
              td.dword1 |= (1<<29);
            } else {
              BX_DEBUG(("A Short Packet was detected, but the SPD bit in DWORD1 was clear"));
            }
          }
          if (td.dword1 & (1<<22)) stalled = was_stall = 1;
          
          // write back the status to the TD
          DEV_MEM_WRITE_PHYSICAL(address + sizeof(Bit32u), sizeof(Bit32u), (Bit8u *) &td.dword1);
          
          // we processed another td within this queue line
          td_count++;
          
          // The UHCI (USB 1.1) only allows so many bytes to be transfered per frame.
          // Due to control/bulk reclamation, we need to catch this and stop transferring 
          //  or this code will just keep processing TDs.
          bytes_processed += r_actlen;
          if (bytes_processed >= hub.max_bandwidth) {
            BX_DEBUG(("Process Bandwidth Limits for this frame (%d with a limit of %d).", bytes_processed, hub.max_bandwidth));
            break;
          }
          
          // move to the next item
          if (!was_stall) {
            item = td.dword0;
            if (queue_addr != 0) {
              if (!was_short) {
                // copy pointer for next queue item into vert queue head
                DEV_MEM_WRITE_PHYSICAL((queue_addr & ~0xF) + sizeof(Bit32u), sizeof(Bit32u), (Bit8u *) &item);
              }
              // if breadth first, short packet, or last in the element list, move on to next queue item
              if (!depthbreadth || !USB_UHCI_IS_LINK_VALID(item) || was_short) {
                item = queue.horz;
                queue_addr = 0;
              }
            }
            continue;
          } else {
            // this is where we would check the CC member.
            // if it is non-zero, decrement it.
            // if it is still non-zero, or was originally zero,
            //  set the TD back to active so that we can try it again.
            // *however*, since it failed in our emulation, it will fail again.
            // so, fall through to below to move to the next queue
          }
        }
      }
      
      // move to next item (no queues) or queue head (queues found)
      item = (queue_addr != 0) ? queue.horz : td.dword0;
    } // while loop
    
    // set the status register bit:0 to 1 if SPD is enabled
    // and if interrupts not masked via interrupt register, raise irq interrupt.
    if (shortpacket) hub.usb_status.status2 |= STATUS2_SPD;
    if (shortpacket && hub.usb_enable.short_packet) {
      BX_DEBUG((" [SPD] We want it to fire here (Frame: %04i)", hub.usb_frame_num.frame_num));
    }

    // if one of the TD's in this frame had the ioc bit set, we need to
    //   raise an interrupt, if interrupts are not masked via interrupt register.
    //   always set the status register if IOC.
    hub.usb_status.status2 |= interrupt ? STATUS2_IOC : 0;
    if (interrupt && hub.usb_enable.on_complete) {
      BX_DEBUG((" [IOC] We want it to fire here (Frame: %04i)", hub.usb_frame_num.frame_num));
    }

    hub.usb_status.error_interrupt |= stalled;
    if (stalled && hub.usb_enable.timeout_crc) {
      BX_DEBUG((" [stalled] We want it to fire here (Frame: %04i)", hub.usb_frame_num.frame_num));
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
  }  // end run schedule

  // if host turned off the schedule, set the halted bit in the status register
  // Note: Can not use an else from the if() above since the host can changed this bit
  //  while we are processing a frame.
  if (hub.usb_command.schedule == 0)
    hub.usb_status.host_halted = 1;

  // TODO ?:
  //  If in Global_Suspend mode and any of usb_port[i] bits 6,3, or 1 are set,
  //    we need to issue a Global_Resume (set the global resume bit).
  //    However, since we don't do anything, let's not.
}

int uhci_event_handler(int event, void *ptr, void *dev, int port)
{
  if (dev != NULL) {
    return ((bx_uhci_core_c *) dev)->event_handler(event, ptr, port);
  }
  return -1;
}

int bx_uhci_core_c::event_handler(int event, void *ptr, int port)
{
  int ret = 0;
  USBAsync *p;

  switch (event) {
    // packet events start here
    case USB_EVENT_ASYNC:
      BX_DEBUG(("Async packet completion"));
      p = container_of_usb_packet(ptr);
      p->done = 1;
      break;
    case USB_EVENT_WAKEUP:
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
      break;

    // host controller events start here
    case USB_EVENT_CHECK_SPEED:
      if (ptr != NULL) {
        usb_device_c *usb_device = (usb_device_c *) ptr;
        if (usb_device->get_speed() <= USB_SPEED_FULL)
          ret = 1;
      }
      break;
    default:
      BX_ERROR(("unknown/unsupported event (id=%d) on port #%d", event, port+1));
      ret = -1; // unknown event, event not handled
  }
  
  return ret;
}

bool bx_uhci_core_c::DoTransfer(Bit32u address, struct TD *td) {

  int len = 0, ret = 0;
  USBAsync *p;
  bool completion;

  Bit16u maxlen = (td->dword2 >> 21);
  Bit8u  addr   = (td->dword2 >> 8) & 0x7F;
  Bit8u  endpt  = (td->dword2 >> 15) & 0x0F;
  Bit8u  pid    =  td->dword2 & 0xFF;

  p = find_async_packet(&packets, address);
  completion = (p != NULL);
  if (completion && !p->done) {
    return 0;
  }

  BX_DEBUG(("TD found at address 0x%08X:  0x%08X  0x%08X  0x%08X  0x%08X", address, td->dword0, td->dword1, td->dword2, td->dword3));

  // check TD to make sure it is valid
  // A max length 0x500 to 0x77E is illegal
  if ((maxlen >= 0x500) && (maxlen != 0x7FF)) {
    BX_ERROR(("invalid max. length value 0x%04x", maxlen ));
    return 0;  // error = consistency check failure
  }

  // when the active bit is set, all others in the 'Status' byte must be zero
  // (active bit is set or we wouldn't be here)
  if (td->dword1 & (0x7F << 16)) {
    BX_ERROR(("UHCI Core: When Active bit is set, all others in the 'Status' byte must be zero. (0x%02X)",
      (td->dword1 & (0x7F << 16)) >> 16));
  }

  // we don't support ISO transfers, so if the IOS bit is set, give an error
  if (td->dword1 & (1 << 25)) {
    BX_ERROR(("UHCI Core: ISO bit is set..."));
  }
  
  // the reserved bit in the Link Pointer should be zero
  if (td->dword0 & (1<<3)) {
    BX_INFO(("UHCI Core: Reserved bit in the Link Pointer is not zero."));
  }

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
    p->packet.speed = (td->dword1 & (1<<26)) ? USB_SPEED_LOW : USB_SPEED_FULL;
#if HANDLE_TOGGLE_CONTROL
    p->packet.toggle = (td->dword2 & (1<<19)) > 0;
#endif
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
        remove_async_packet(&packets, p);
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
void bx_uhci_core_c::set_status(struct TD *td, bool stalled, bool data_buffer_error, bool babble,
                             bool nak, bool crc_time_out, bool bitstuff_error, Bit16u act_len)
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

// these must match USB_SPEED_*
const char *usb_speed[4] = {
  "low",     // USB_SPEED_LOW   = 0
  "full",    // USB_SPEED_FULL  = 1
  "high",    // USB_SPEED_HIGH  = 2
  "super"    // USB_SPEED_SUPER = 3
};

bool bx_uhci_core_c::set_connect_status(Bit8u port, bool connected)
{
  usb_device_c *device = hub.usb_port[port].device;
  if (device != NULL) {
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
          return 0;
        default:
          BX_PANIC(("USB device returned invalid speed value"));
          return 0;
      }
      if (hub.usb_port[port].low_speed) {
        hub.usb_port[port].line_dminus = 1;  //  dminus=1 & dplus=0 = low speed  (at idle time)
        hub.usb_port[port].line_dplus = 0;   //  dminus=0 & dplus=1 = full speed (at idle time)
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
          BX_ERROR(("port #%d: connect failed", port+1));
          return 0;
        } else {
          BX_INFO(("port #%d: connect: %s", port+1, device->get_info()));
        }
      }
    } else {
      BX_INFO(("port #%d: device disconnect", port+1));
      hub.usb_port[port].status = 0;
      hub.usb_port[port].connect_changed = 1;
      if (hub.usb_port[port].enabled) {
        hub.usb_port[port].enable_changed = 1;
        hub.usb_port[port].enabled = 0;
      }
      hub.usb_port[port].low_speed = 0;
      hub.usb_port[port].line_dminus = 0;
      hub.usb_port[port].line_dplus = 0;
    }
  }
  return connected;
}

void bx_uhci_core_c::set_port_device(int port, usb_device_c *dev)
{
  usb_device_c *olddev = hub.usb_port[port].device;
  if ((dev != NULL) && (olddev == NULL)) {
    hub.usb_port[port].device = dev;
    set_connect_status(port, 1);
  } else if ((dev == NULL) && (olddev != NULL)) {
    set_connect_status(port, 0);
    hub.usb_port[port].device = dev;
  }
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_USB_UHCI
