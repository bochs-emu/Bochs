/////////////////////////////////////////////////////////////////////////
// $Id: pciusb.h,v 1.1 2003-01-28 16:58:10 vruppert Exp $
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

// Benjamin D Lunt (fys@cybertrails.com) coded most of this usb emulation.
//  I hope to add to this code to make it more functionable.
//

#if BX_USE_PCIUSB_SMF
#  define BX_USB_THIS theUSBDevice->
#else
#  define BX_USB_THIS this->
#endif

#define BX_USB_MAXDEV   1
#define BX_USB_CONFDEV  1   /* only 1 USB hub currently */

#define USB_NUM_PORTS   2 /* UHCI supports 2 ports per root hub */

typedef struct {

  Bit16u base_ioaddr;
  Bit8u  irq;
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
    bx_bool suspend;
    bx_bool reset;
    bx_bool low_speed;
    bx_bool resume;
    bx_bool line_dplus;
    bx_bool line_dminus;
    bx_bool able_changed;
    bx_bool enabled;
    bx_bool connect_changed;
    bx_bool status;
  } usb_port[USB_NUM_PORTS];

  Bit8u pci_conf[256];

} bx_usb_t;


class bx_pciusb_c : public bx_devmodel_c
{
public:
  bx_pciusb_c(void);
  ~bx_pciusb_c(void);
  virtual void   init(void);
  virtual void   reset(unsigned type);

private:

  bx_usb_t hub[BX_USB_MAXDEV];
  Bit8u  global_reset;

  static void usb_timer_handler(void *);
  void usb_timer(void);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  static Bit32u pci_read_handler(void *this_ptr, Bit8u address, unsigned io_len);
  static void   pci_write_handler(void *this_ptr, Bit8u address, Bit32u value, unsigned io_len);
#if !BX_USE_PCIUSB_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
  Bit32u pci_read(Bit8u address, unsigned io_len);
  void   pci_write(Bit8u address, Bit32u value, unsigned io_len);
#endif
};
