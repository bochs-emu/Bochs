/////////////////////////////////////////////////////////////////////////
// $Id: pciusb.h,v 1.4 2004-12-11 08:35:33 vruppert Exp $
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
#  define BX_USB_THIS_PTR theUSBDevice
#else
#  define BX_USB_THIS this->
#  define BX_USB_THIS_PTR this
#endif

#define BX_USB_MAXDEV   1
#define BX_USB_CONFDEV  1   /* only 1 USB hub currently */

#define USB_NUM_PORTS   2 /* UHCI supports 2 ports per root hub */
#define USB_CUR_DEVS    1

#define TOKEN_IN    0x69
#define TOKEN_OUT   0xE1
#define TOKEN_SETUP 0x2D

// Bus Line Status States
#define LO_IDLE  BX_USB_THIS hub[0].usb_port[0].line_dminus = 1; \
                 BX_USB_THIS hub[0].usb_port[0].line_dplus = 0;
#define LO_K     BX_USB_THIS hub[0].usb_port[0].line_dminus = 0; \
                 BX_USB_THIS hub[0].usb_port[0].line_dplus = 1;
#define LO_J     BX_USB_THIS hub[0].usb_port[0].line_dminus = 1; \
                 BX_USB_THIS hub[0].usb_port[0].line_dplus = 0;
#define HI_IDLE  BX_USB_THIS hub[0].usb_port[0].line_dminus = 0; \
                 BX_USB_THIS hub[0].usb_port[0].line_dplus = 1;
#define HI_K     BX_USB_THIS hub[0].usb_port[0].line_dminus = 1; \
                 BX_USB_THIS hub[0].usb_port[0].line_dplus = 0;
#define HI_J     BX_USB_THIS hub[0].usb_port[0].line_dminus = 0; \
                 BX_USB_THIS hub[0].usb_port[0].line_dplus = 1;
#define SE0      BX_USB_THIS hub[0].usb_port[0].line_dminus = 0; \
                 BX_USB_THIS hub[0].usb_port[0].line_dplus = 0;
#define SE1      BX_USB_THIS hub[0].usb_port[0].line_dminus = 1; \
                 BX_USB_THIS hub[0].usb_port[0].line_dplus = 1;
#define LO_SOP   LO_K
#define LO_EOP   SE0 LO_K LO_IDLE

// device requests
enum { GET_STATUS=0, CLEAR_FEATURE, SET_FEATURE=3, SET_ADDRESS=5, GET_DESCRIPTOR=6, SET_DESCRIPTOR,
       GET_CONFIGURATION, SET_CONFIGURATION,
// interface requests
       GET_INTERFACE, SET_INTERFACE,
// standard endpoint requests
       SYNCH_FRAME
};

// Descriptor types
enum {  DEVICE=1, CONFIG, STRING, INTERFACE, ENDPOINT,         // USB 1.1
        DEVICE_QUALIFIER, OTHER_SPEED_CONFIG, INTERFACE_POWER  // USB 2.0
};

// setup packets
struct REQUEST_PACKET {
  Bit8u  request_type;
  Bit8u  request;
  Bit16u value;
  Bit16u index;
  Bit16u length;
};

// set it to 1 (align on byte) and save so we can pop it
// (( MS Specific ???? ))
#pragma pack(push, 1)
struct USB_DEVICE {
  Bit8u   address;        // 7 bit address
  Bit8u   descriptor;     // which descriptor to use
  Bit8u   config;         // which configuration to use
  Bit8u   Interface;      // which interface to use
  Bit8u   alt_interface;  // which alt interface to use ???
  Bit8u   functions;      // how many functions does this device have
  struct {
    Bit8u   direction;
    Bit8u   *in;
    Bit8u   *out;
    Bit16u  in_cnt;
    Bit16u  out_cnt;
    struct {
      Bit8u  len;
      Bit8u  type;
      Bit16u usb_ver;
      Bit8u  _class;
      Bit8u  subclass;
      Bit8u  protocol;
      Bit8u  max_packet_size;
      Bit16u vendorid;
      Bit16u productid;
      Bit16u device_rel;
      Bit8u  manuf_indx;
      Bit8u  prod_indx;
      Bit8u  serial_indx;
      Bit8u  configs;
    } device_descr;
    unsigned configs;
    struct {
      Bit8u  len;
      Bit8u  type;
      Bit16u tot_len;
      Bit8u  interfaces;
      Bit8u  config_val;
      Bit8u  config_indx;
      Bit8u  attrbs;
      Bit8u  max_power;
      Bit8u  remaining[247];
    } device_config[2];
    struct {
      Bit8u  size;
      Bit8u  type;
      Bit16u langid[3];
    } str_descriptor;
    struct {
      Bit8u  size;
      Bit8u  type;
      Bit8u  unicode_str[64];
    } string[4];
    struct {
      Bit8u  size;
      Bit8u  type;
      Bit16u HID_class;
      Bit8u  country_code;
      Bit8u  num_descriptors;
      Bit8u  descriptor_type;
      Bit16u descriptor_len;
      Bit8u  op_descriptor_type;
      Bit16u op_descriptor_len;
    } dev_hid_descript;
    struct {
      Bit8u  size;
      Bit8u  type;
      Bit8u  unknown[3];
    } dev_hid_descript_report;
    Bit8u interfaces[129];
  } function[1];     // this device has 1 function
};
#pragma pack(pop)

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

  struct USB_DEVICE device[USB_CUR_DEVS];  // two ports per hub, 127 devices per port (max)

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

class bx_pciusb_c : public bx_usb_stub_c
{
public:
  bx_pciusb_c(void);
  ~bx_pciusb_c(void);
  virtual void   init(void);
  virtual void   reset(unsigned type);
  virtual void   usb_mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state);

private:

  bx_usb_t hub[BX_USB_MAXDEV];
  Bit8u    global_reset;

  int      mouse_delayed_dx;
  int      mouse_delayed_dy;
  int      mouse_delayed_dz;
  unsigned button_state;

  static void set_irq_level(bx_bool level);
  static void usb_timer_handler(void *);
  void usb_timer(void);
  void DoTransfer(struct TD *);
  void GetDescriptor(struct USB_DEVICE *, Bit8u, struct REQUEST_PACKET *, Bit8u);
  void set_status(struct TD *td, bx_bool stalled, bx_bool data_buffer_error, bx_bool babble,
    bx_bool nak, bx_bool crc_time_out, bx_bool bitstuff_error, Bit16u act_len);

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
