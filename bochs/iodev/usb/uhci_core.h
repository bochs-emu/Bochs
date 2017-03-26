/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2016  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2009-2017  The Bochs Project
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

#ifndef BX_IODEV_UHCI_CORE_H
#define BX_IODEV_UHCI_CORE_H

#define USB_UHCI_PORTS 2

typedef struct {
  int    timer_index;

  // Registers
  // Base + 0x00  Command register
  // Base + 0x02  Status register
  // Base + 0x04  Interrupt Enable register
  // Base + 0x06  Frame Number register
  // Base + 0x08  Frame Base Register (32-bit)
  // Base + 0x0C  Start of Frame Modify register
  // Base + 0x0D
  // Base + 0x0E
  // Base + 0x0F
  // Base + 0x10  Eight(?) 16-bit ports (one for each port on hub)

  // Bit reps of registers above
  // Command Register
  //  Bits 15-8 are reserved
  //  Bit 7 = Maximum packet size
  //  Bit 6 = Host Controller has been configured (set by software)
  //  Bit 5 = software debug mode
  //  Bit 4 = force global resume
  //  Bit 3 = enter global suspend mode
  //  Bit 2 = global reset
  //  Bit 1 = host controller reset
  //  Bit 0 = run/stop schedule
  struct {
    bx_bool max_packet_size; //(bit 7) 0 = 32 bytes, 1 = 64 bytes
    bx_bool configured;      //(bit 6)
    bx_bool debug;           //(bit 5)
    bx_bool resume;          //(bit 4)
    bx_bool suspend;         //(bit 3)
    bx_bool reset;           //(bit 2)
    bx_bool host_reset;      //(bit 1)
    bx_bool schedule;        //(bit 0) 0 = Stop, 1 = Run
  } usb_command;

  // Status Register
  //  Bits 15-6 are reserved
  //  Bit 5 = Host controller halted
  //  Bit 4 = Host controller process error
  //  Bit 3 = PCI Bus error
  //  Bit 2 = resume received
  //  Bit 1 = USB error interrupt
  //  Bit 0 = USB interrupt
  struct {
    bx_bool host_halted;     //(bit 5)
    bx_bool host_error;      //(bit 4)
    bx_bool pci_error;       //(bit 3)
    bx_bool resume;          //(bit 2)
    bx_bool error_interrupt; //(bit 1)
    bx_bool interrupt;       //(bit 0)
    Bit8u   status2; // bit 0 and 1 are used to generate the interrupt
  } usb_status;

  // Interrupt Enable Register
  //  Bits 15-4 are reserved
  //  Bit 3 = enable short packet interrupts
  //  Bit 2 = enable interrupt On Complete
  //  Bit 1 = enable resume
  //  Bit 0 = enable timeout/crc
  struct {
    bx_bool short_packet; //(bit 3)
    bx_bool on_complete;  //(bit 2)
    bx_bool resume;       //(bit 1)
    bx_bool timeout_crc;  //(bit 0)
  } usb_enable;

  // Frame Number Register
  //  Bits 15-11 are reserved
  //  Bits 10-0  Frame List Current Index/Frame Number
  struct {
    Bit16u frame_num;
  } usb_frame_num;

  // Frame List Base Address Register
  //  Bits 31-12  Base
  //  Bits 11-0   *must* be zeros when written to
  struct {
    Bit32u frame_base;
  } usb_frame_base;

  // Start of Frame Modify Register
  //  Bit    7 reserved
  //  Bits 6-0 SOF timing value (default 64)
  // SOF cycle time equals 11936+timing value
  struct {
    Bit8u sof_timing;
  } usb_sof;

  // Port Register (0-1)
  //  Bits 15-13  are reserved
  //  Bit     12  suspend port
  //  Bit  11-10  are reserved
  //  Bit      9  port in reset state
  //  Bit      8  low-speed device is attached (read-only)
  //  Bit      7  reserved
  //  Bit      6  resume detected (read-only)
  //  Bit      5  line-status D+ (read-only)
  //  Bit      4  line-status D- (read-only)
  //  Bit      3  port enabled/disable status has changed
  //               (write 1 to this bit to clear it)
  //  Bit      2  port is enabled
  //  Bit      1  connect status has changed
  //               (write 1 to this bit to clear it)
  //  Bit      0  current connect status (read-only)
  //  Can only write in WORD sizes (Read in byte sizes???)
  struct {
    // our data
    usb_device_c *device;   // device connected to this port

    // bit reps of actual port
    bx_bool suspend;
    bx_bool reset;
    bx_bool low_speed;
    bx_bool resume;
    bx_bool line_dminus;
    bx_bool line_dplus;
    bx_bool able_changed;
    bx_bool enabled;
    bx_bool connect_changed;
    bx_bool status;
  } usb_port[USB_UHCI_PORTS];

  Bit8u   devfunc;
} bx_uhci_core_t;

#pragma pack (push, 1)
struct TD {
  Bit32u dword0;
  Bit32u dword1;
  Bit32u dword2;
  Bit32u dword3;
  Bit32u resv[4];
};
#pragma pack (pop)

#define HC_HORZ    0x80
#define HC_VERT    0x81
struct HCSTACK {
  Bit32u  next;
  Bit8u   d;   // if queue, denotes VERT or HORZ
  bx_bool q;
  bx_bool t;
};

class bx_uhci_core_c : public bx_pci_device_c {
public:
  bx_uhci_core_c();
  virtual ~bx_uhci_core_c();
  virtual void init_uhci(Bit8u devfunc, Bit16u devid, Bit8u headt, Bit8u intp);
  virtual void reset_uhci(unsigned);
  virtual void register_state(bx_list_c *parent);
  virtual void after_restore_state(void);
  virtual void set_port_device(int port, usb_device_c *dev);
  virtual void pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  void event_handler(int event, USBPacket *packet, int port);

protected:
  bx_uhci_core_t hub;
  Bit8u          global_reset;
  bx_bool        busy;

  USBAsync *packets;

  void update_irq(void);

  int  broadcast_packet(USBPacket *p);
  void set_connect_status(Bit8u port, int type, bx_bool connected);

  static void uhci_timer_handler(void *);
  void uhci_timer(void);
  bx_bool DoTransfer(Bit32u address, Bit32u queue_num, struct TD *);
  void set_status(struct TD *td, bx_bool stalled, bx_bool data_buffer_error, bx_bool babble,
    bx_bool nak, bx_bool crc_time_out, bx_bool bitstuff_error, Bit16u act_len);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
};

#endif
