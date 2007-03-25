/////////////////////////////////////////////////////////////////////////
// $Id: pciusb.h,v 1.25 2007-03-25 17:37:59 vruppert Exp $
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

// Benjamin D Lunt (fys at frontiernet net) coded most of this usb emulation.
// USB mass storage device support and SCSI emulation layer ported from Qemu

#ifndef BX_IODEV_PCIUSB_H
#define BX_IODEV_PCIUSB_H

#if BX_USE_PCIUSB_SMF
#  define BX_USB_THIS theUSBDevice->
#  define BX_USB_THIS_PTR theUSBDevice
#else
#  define BX_USB_THIS this->
#  define BX_USB_THIS_PTR this
#endif

#define BX_USB_MAXDEV   1
#define BX_USB_CONFDEV  1   /* only 1 USB hub currently */

#define USB_NUM_PORTS   2   /* UHCI supports 2 ports per root hub */

#define USB_TOKEN_IN    0x69
#define USB_TOKEN_OUT   0xE1
#define USB_TOKEN_SETUP 0x2D

#define USB_MSG_ATTACH   0x100
#define USB_MSG_DETACH   0x101
#define USB_MSG_RESET    0x102

#define USB_RET_NODEV  (-1) 
#define USB_RET_NAK    (-2)
#define USB_RET_STALL  (-3)
#define USB_RET_BABBLE (-4)
#define USB_RET_ASYNC  (-5)

#define USB_SPEED_LOW   0
#define USB_SPEED_FULL  1
#define USB_SPEED_HIGH  2

#define USB_STATE_NOTATTACHED 0
#define USB_STATE_ATTACHED    1
//#define USB_STATE_POWERED     2
#define USB_STATE_DEFAULT     3
//#define USB_STATE_ADDRESS     4
//#define USB_STATE_CONFIGURED  5
#define USB_STATE_SUSPENDED   6

#define USB_DIR_OUT  0
#define USB_DIR_IN   0x80

#define USB_TYPE_MASK			(0x03 << 5)
#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENDOR			(0x02 << 5)
#define USB_TYPE_RESERVED		(0x03 << 5)

#define USB_RECIP_MASK			0x1f
#define USB_RECIP_DEVICE		0x00
#define USB_RECIP_INTERFACE		0x01
#define USB_RECIP_ENDPOINT		0x02
#define USB_RECIP_OTHER			0x03

#define DeviceRequest ((USB_DIR_IN|USB_TYPE_STANDARD|USB_RECIP_DEVICE)<<8)
#define DeviceOutRequest ((USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_DEVICE)<<8)
#define InterfaceRequest \
        ((USB_DIR_IN|USB_TYPE_STANDARD|USB_RECIP_INTERFACE)<<8)
#define InterfaceOutRequest \
        ((USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_INTERFACE)<<8)
#define EndpointRequest ((USB_DIR_IN|USB_TYPE_STANDARD|USB_RECIP_ENDPOINT)<<8)
#define EndpointOutRequest \
        ((USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_ENDPOINT)<<8)

#define USB_REQ_GET_STATUS		0x00
#define USB_REQ_CLEAR_FEATURE		0x01
#define USB_REQ_SET_FEATURE		0x03
#define USB_REQ_SET_ADDRESS		0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_DESCRIPTOR		0x07
#define USB_REQ_GET_CONFIGURATION	0x08
#define USB_REQ_SET_CONFIGURATION	0x09
#define USB_REQ_GET_INTERFACE		0x0A
#define USB_REQ_SET_INTERFACE		0x0B
#define USB_REQ_SYNCH_FRAME		0x0C

#define USB_DEVICE_SELF_POWERED		0
#define USB_DEVICE_REMOTE_WAKEUP	1

// USB 1.1
#define USB_DT_DEVICE			0x01
#define USB_DT_CONFIG			0x02
#define USB_DT_STRING			0x03
#define USB_DT_INTERFACE		0x04
#define USB_DT_ENDPOINT			0x05
// USB 2.0
#define USB_DT_DEVICE_QUALIFIER         0x06
#define USB_DT_OTHER_SPEED_CONFIG       0x07
#define USB_DT_INTERFACE_POWER          0x08

struct USBPacket {
  int pid;
  Bit8u devaddr;
  Bit8u devep;
  Bit8u *data;
  int len;
};

// setup packets
struct REQUEST_PACKET {
  Bit8u  request_type;
  Bit8u  request;
  Bit16u value;
  Bit16u index;
  Bit16u length;
};

enum usbdev_type {
  USB_DEV_TYPE_NONE=0,
  USB_DEV_TYPE_MOUSE,
  USB_DEV_TYPE_TABLET,
  USB_DEV_TYPE_KEYPAD,
  USB_DEV_TYPE_DISK
};

class usb_hid_device_t;

void usb_dump_packet(Bit8u *data, unsigned size);
int set_usb_string(Bit8u *buf, const char *str);


class usb_device_t : public logfunctions {
public:
  usb_device_t(void);
  virtual ~usb_device_t(void) {}

  virtual int handle_packet(USBPacket *p);
  virtual void handle_reset() {}
  virtual int handle_control(int request, int value, int index, int length, Bit8u *data) {return 0;}
  virtual int handle_data(USBPacket *p) {return 0;}

  bx_bool get_connected() {return d.connected;}
  usbdev_type get_type() {return d.type;}
  int get_speed() {return d.speed;}
  Bit8u get_address() {return d.addr;}

protected:
  struct {
    enum usbdev_type type;
    bx_bool connected;
    int speed;
    Bit8u addr;
    Bit8u config;
    char devname[32];

    int state;
    Bit8u setup_buf[8];
    Bit8u data_buf[1024];
    int remote_wakeup;
    int setup_state;
    int setup_len;
    int setup_index;
  } d;
};

typedef struct {
  Bit32u base_ioaddr;
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
    // our data
    int     device_num;     // device number on this hub
    usb_device_t *device;   // device connected to this port

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
  } usb_port[USB_NUM_PORTS];

  Bit8u pci_conf[256];
  Bit8u devfunc;

  int statusbar_id[2]; // IDs of the status LEDs

} bx_usb_t;

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

class bx_pciusb_c : public bx_pci_usb_stub_c {
public:
  bx_pciusb_c();
  virtual ~bx_pciusb_c();
  virtual void init(void);
  virtual void reset(unsigned);
  virtual void usb_mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state);
  virtual void usb_mouse_enabled_changed(bx_bool enable);
  virtual bx_bool usb_key_enq(Bit8u *scan_code);
  virtual bx_bool usb_keyboard_connected();
  virtual bx_bool usb_mouse_connected();
#if BX_SUPPORT_SAVE_RESTORE
  virtual void register_state(void);
  virtual void after_restore_state(void);
#endif
  virtual Bit32u  pci_read_handler(Bit8u address, unsigned io_len);
  virtual void    pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  static const char *usb_param_handler(bx_param_string_c *param, int set, const char *val, int maxlen);

private:
  bx_bool  busy;

  bx_usb_t hub[BX_USB_CONFDEV];
  Bit8u    global_reset;

  static void set_irq_level(bx_bool level);
  Bit8u  *device_buffer;

  usb_hid_device_t *mousedev;
  usb_hid_device_t *keybdev;

  USBPacket usb_packet;

  static void init_device(Bit8u port, const char *devname);
  static void usb_set_connect_status(Bit8u port, int type, bx_bool connected);

  static void usb_timer_handler(void *);
  void usb_timer(void);
  bx_bool DoTransfer(Bit32u address, Bit32u queue_num, struct TD *);
  void set_status(struct TD *td, bx_bool stalled, bx_bool data_buffer_error, bx_bool babble,
    bx_bool nak, bx_bool crc_time_out, bx_bool bitstuff_error, Bit16u act_len);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_PCIUSB_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  void usb_send_msg(usb_device_t *dev, int msg);
};

#endif
