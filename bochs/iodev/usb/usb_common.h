/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// Generic USB emulation code
//
// Copyright (c) 2005       Fabrice Bellard
// Copyright (C) 2009-2016  Benjamin D Lunt (fys [at] fysnet [dot] net)
//               2009-2017  The Bochs Project
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
/////////////////////////////////////////////////////////////////////////

#ifndef BX_IODEV_USB_COMMON_H
#define BX_IODEV_USB_COMMON_H

#define USB_TOKEN_IN    0x69
#define USB_TOKEN_OUT   0xE1
#define USB_TOKEN_SETUP 0x2D

#define USB_MSG_ATTACH   0x100
#define USB_MSG_DETACH   0x101
#define USB_MSG_RESET    0x102

#define USB_RET_NODEV   (-1)
#define USB_RET_NAK     (-2)
#define USB_RET_STALL   (-3)
#define USB_RET_BABBLE  (-4)
#define USB_RET_IOERROR (-5)
#define USB_RET_ASYNC   (-6)

#define USB_SPEED_LOW   0
#define USB_SPEED_FULL  1
#define USB_SPEED_HIGH  2
#define USB_SPEED_SUPER 3

#define USB_STATE_NOTATTACHED 0
#define USB_STATE_ATTACHED    1
//#define USB_STATE_POWERED     2
#define USB_STATE_DEFAULT     3
#define USB_STATE_ADDRESS     4
#define USB_STATE_CONFIGURED  5
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
#define InterfaceInClassRequest \
   ((USB_DIR_IN|USB_TYPE_CLASS|USB_RECIP_INTERFACE)<<8)
#define InterfaceOutRequest \
   ((USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_INTERFACE)<<8)
#define InterfaceOutClassRequest \
   ((USB_DIR_OUT|USB_TYPE_CLASS|USB_RECIP_INTERFACE)<<8)
#define EndpointRequest ((USB_DIR_IN|USB_TYPE_STANDARD|USB_RECIP_ENDPOINT)<<8)
#define EndpointOutRequest \
   ((USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_ENDPOINT)<<8)

#define USB_REQ_GET_STATUS        0x00
#define USB_REQ_CLEAR_FEATURE     0x01
#define USB_REQ_SET_FEATURE       0x03
#define USB_REQ_SET_ADDRESS       0x05
#define USB_REQ_GET_DESCRIPTOR    0x06
#define USB_REQ_SET_DESCRIPTOR    0x07
#define USB_REQ_GET_CONFIGURATION 0x08
#define USB_REQ_SET_CONFIGURATION 0x09
#define USB_REQ_GET_INTERFACE     0x0A
#define USB_REQ_SET_INTERFACE     0x0B
#define USB_REQ_SYNCH_FRAME       0x0C
#define USB_REQ_SET_SEL           0x30

#define USB_DEVICE_SELF_POWERED    0
#define USB_DEVICE_REMOTE_WAKEUP   1
#define USB_DEVICE_U1_ENABLE      48
#define USB_DEVICE_U2_ENABLE      49

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
// USB 3.0
#define USB_DT_BIN_DEV_OBJ_STORE        0x0F

typedef struct USBPacket USBPacket;

#define USB_EVENT_WAKEUP 0
#define USB_EVENT_ASYNC  1

typedef void USBCallback(int event, USBPacket *packet, void *dev, int port);

class usb_device_c;

struct USBPacket {
  int pid;
  Bit8u devaddr;
  Bit8u devep;
  Bit8u *data;
  int len;
  USBCallback *complete_cb;
  void *complete_dev;
  usb_device_c *dev;
};

typedef struct USBAsync {
  USBPacket packet;
  Bit64u    td_addr;
  bx_bool done;
  Bit16u  slot_ep;
  struct USBAsync *next;
} USBAsync;

enum usbmod_type {
  USB_MOD_TYPE_NONE=0,
  USB_MOD_TYPE_CBI,
  USB_MOD_TYPE_HID,
  USB_MOD_TYPE_HUB,
  USB_MOD_TYPE_MSD,
  USB_MOD_TYPE_PRINTER
};

enum usbdev_type {
  USB_DEV_TYPE_NONE=0,
  USB_DEV_TYPE_MOUSE,
  USB_DEV_TYPE_TABLET,
  USB_DEV_TYPE_KEYPAD,
  USB_DEV_TYPE_DISK,
  USB_DEV_TYPE_CDROM,
  USB_DEV_TYPE_HUB,
  USB_DEV_TYPE_PRINTER,
  USB_DEV_TYPE_FLOPPY
};

class BOCHSAPI bx_usbdev_ctl_c : public logfunctions {
public:
  bx_usbdev_ctl_c();
  virtual ~bx_usbdev_ctl_c() {}
  void init(void);
  void exit(void);
  virtual int init_device(bx_list_c *portconf, logfunctions *hub, void **dev, bx_list_c *sr_list);
private:
  void parse_port_options(usb_device_c *dev, bx_list_c *portconf);
};

BOCHSAPI extern bx_usbdev_ctl_c bx_usbdev_ctl;

class BOCHSAPI usb_device_c : public logfunctions {
public:
  usb_device_c(void);
  virtual ~usb_device_c() {}

  virtual bx_bool init() {return d.connected;}
  virtual const char* get_info() {return NULL;}
  virtual usb_device_c* find_device(Bit8u addr);

  virtual int handle_packet(USBPacket *p);
  virtual void handle_reset() {}
  virtual int handle_control(int request, int value, int index, int length, Bit8u *data) {return -1;}
  virtual int handle_data(USBPacket *p) {return 0;}
  void register_state(bx_list_c *parent);
  virtual void register_state_specific(bx_list_c *parent) {}
  virtual void after_restore_state() {}
  virtual void cancel_packet(USBPacket *p) {}
  virtual bx_bool set_option(const char *option) {return 0;}
  virtual void runtime_config() {}

  bx_bool get_connected() {return d.connected;}
  usbdev_type get_type() {return d.type;}
  int get_speed() {return d.speed;}
  bx_bool set_speed(int speed)
  {
    if ((speed >= d.minspeed) && (speed <= d.maxspeed)) {
      d.speed = speed;
      return 1;
    } else {
      return 0;
    }
  }

  Bit8u get_address() {return d.addr;}
  void set_async_mode(bx_bool async) {d.async_mode = async;}
  void set_event_handler(void *dev, USBCallback *cb, int port)
  {
    d.event.dev = dev;
    d.event.cb = cb;
    d.event.port = port;
  }
  void set_debug_mode();

  void usb_send_msg(int msg);

protected:
  struct {
    enum usbdev_type type;
    bx_bool connected;
    int minspeed;
    int maxspeed;
    int speed;
    Bit8u addr;
    Bit8u config;
    Bit8u interface;
    char devname[32];

    const Bit8u *dev_descriptor;
    const Bit8u *config_descriptor;
    int device_desc_size;
    int config_desc_size;
    const char *vendor_desc;
    const char *product_desc;
    const char *serial_num;

    int state;
    Bit8u setup_buf[8];
    Bit8u data_buf[1024];
    int remote_wakeup;
    int setup_state;
    int setup_len;
    int setup_index;
    bx_bool stall;
    bx_bool async_mode;
    struct {
      USBCallback *cb;
      void *dev;
      int port;
    } event;
    bx_list_c *sr;
  } d;

  int handle_control_common(int request, int value, int index, int length, Bit8u *data);
  void usb_dump_packet(Bit8u *data, unsigned size);
  int set_usb_string(Bit8u *buf, const char *str);
};

static BX_CPP_INLINE void usb_packet_init(USBPacket *p, int size)
{
  memset(p, 0, sizeof(USBPacket));
  if (size > 0) {
    p->data = new Bit8u[size];
    if (!p->data) {
      return;
    }
  }
  p->len = size;
}

static BX_CPP_INLINE void usb_packet_cleanup(USBPacket *p)
{
  if (p->data) {
    delete [] p->data;
    p->data = NULL;
  }
}

static BX_CPP_INLINE void usb_defer_packet(USBPacket *p, usb_device_c *dev)
{
  p->dev = dev;
}

static BX_CPP_INLINE void usb_cancel_packet(USBPacket *p)
{
  p->dev->cancel_packet(p);
}

static BX_CPP_INLINE void usb_packet_complete(USBPacket *p)
{
    p->complete_cb(USB_EVENT_ASYNC, p, p->complete_dev, 0);
}

// Async packet support

static BX_CPP_INLINE USBAsync* create_async_packet(USBAsync **base, Bit64u addr, int maxlen)
{
  USBAsync *p;

  p = new USBAsync;
  usb_packet_init(&p->packet, maxlen);
  p->td_addr = addr;
  p->done = 0;
  p->next = *base;
  *base = p;
  return p;
}

static BX_CPP_INLINE void remove_async_packet(USBAsync **base, USBAsync *p)
{
  USBAsync *last;

  if (*base == p) {
    *base = p->next;
  } else {
    last = *base;
    while (last != NULL) {
      if (last->next != p)
        last = last->next;
      else
        break;
    }
    if (last) {
      last->next = p->next;
    } else {
      return;
    }
  }
  usb_packet_cleanup(&p->packet);
  delete p;
}

static BX_CPP_INLINE USBAsync* find_async_packet(USBAsync **base, Bit64u addr)
{
  USBAsync *p = *base;

  while (p != NULL) {
    if (p->td_addr != addr)
      p = p->next;
    else
      break;
  }
  return p;
}

static BX_CPP_INLINE struct USBAsync *container_of_usb_packet(void *ptr)
{
  return reinterpret_cast<struct USBAsync*>(static_cast<char*>(ptr) -
    reinterpret_cast<size_t>(&(static_cast<struct USBAsync*>(0)->packet)));
}

// dword read / write helper functions

static BX_CPP_INLINE void get_dwords(bx_phy_address addr, Bit32u *buf, int num)
{
  for (int i = 0; i < num; i++, buf++, addr += sizeof(*buf)) {
    DEV_MEM_READ_PHYSICAL(addr, 4, (Bit8u*)buf);
  }
}

static BX_CPP_INLINE void put_dwords(bx_phy_address addr, Bit32u *buf, int num)
{
  for (int i = 0; i < num; i++, buf++, addr += sizeof(*buf)) {
    DEV_MEM_WRITE_PHYSICAL(addr, 4, (Bit8u*)buf);
  }
}

//
// The usbdev_locator class is used by usb_device_c classes to register
// their name. USB HC emulations use the static 'create' method
// to locate and instantiate a usb_device_c class.
//
class BOCHSAPI_MSVCONLY usbdev_locator_c {
public:
  static bx_bool module_present(const char *type);
  static void cleanup();
  static usb_device_c *create(const char *type, usbdev_type devtype, const char *args);
protected:
  usbdev_locator_c(const char *type);
  virtual ~usbdev_locator_c();
  virtual usb_device_c *allocate(usbdev_type devtype, const char *args) = 0;
private:
  static usbdev_locator_c *all;
  usbdev_locator_c *next;
  const char *type;
};

#endif
