/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// Generic USB emulation code
//
// Copyright (c) 2005       Fabrice Bellard
// Copyright (C) 2009-2023  Benjamin D Lunt (fys [at] fysnet [dot] net)
//               2009-2023  The Bochs Project
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

// for the Packet Capture code to work, these four must remain as is
#define USB_TRANS_TYPE_ISO      0
#define USB_TRANS_TYPE_INT      1
#define USB_TRANS_TYPE_CONTROL  2
#define USB_TRANS_TYPE_BULK     3

#define USB_CONTROL_EP     0

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

// these must remain in this order, 0 -> 3
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

#define USB_TYPE_MASK			  (0x03 << 5)
#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENDOR			(0x02 << 5)
#define USB_TYPE_RESERVED		(0x03 << 5)

#define USB_RECIP_MASK			  0x1f
#define USB_RECIP_DEVICE		  0x00
#define USB_RECIP_INTERFACE		0x01
#define USB_RECIP_ENDPOINT		0x02
#define USB_RECIP_OTHER			  0x03

#define DeviceRequest ((USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE) << 8)     // Host to device / Standard Type / Recipient:Device
#define DeviceOutRequest ((USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE) << 8) // Device to host / Standard Type / Recipient:Device
#define DeviceClassInRequest \
   ((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_DEVICE) << 8)
#define InterfaceRequest \
   ((USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_INTERFACE) << 8)
#define InterfaceInClassRequest \
   ((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
#define OtherInClassRequest \
   ((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_OTHER) << 8)
#define InterfaceOutRequest \
   ((USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_INTERFACE) << 8)
#define DeviceOutClassRequest \
   ((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_DEVICE) << 8)
#define InterfaceOutClassRequest \
   ((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8)
#define OtherOutClassRequest \
   ((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_OTHER) << 8)
#define EndpointRequest ((USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_ENDPOINT) << 8)
#define EndpointOutRequest \
   ((USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_ENDPOINT) << 8)

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
#define USB_REQ_SET_ISO_DELAY     0x31

#define USB_DEVICE_SELF_POWERED    0
#define USB_DEVICE_REMOTE_WAKEUP   1
#define USB_DEVICE_U1_ENABLE      48
#define USB_DEVICE_U2_ENABLE      49

// USB 1.1
#define USB_DT_DEVICE			  0x01
#define USB_DT_CONFIG			  0x02
#define USB_DT_STRING			  0x03
#define USB_DT_INTERFACE		0x04
#define USB_DT_ENDPOINT			0x05
// USB 2.0
#define USB_DT_DEVICE_QUALIFIER         0x06
#define USB_DT_OTHER_SPEED_CONFIG       0x07
#define USB_DT_INTERFACE_POWER          0x08
// USB 3.0
#define USB_DT_BIN_DEV_OBJ_STORE        0x0F

typedef struct USBPacket USBPacket;

// packet events
#define USB_EVENT_WAKEUP        0
#define USB_EVENT_ASYNC         1
// controller events
#define USB_EVENT_CHECK_SPEED  10

// set this to 1 to monitor the TD's toggle bit
// setting to 0 will speed up the emualtion slightly
#define HANDLE_TOGGLE_CONTROL 1

typedef int USBCallback(int event, void *ptr, void *dev, int port);

class usb_device_c;

#include "usb_pcap.h"

struct USBPacket {
  int pid;
  Bit8u devaddr;
  Bit8u devep;
  Bit8u speed;           // packet's speed definition
#if HANDLE_TOGGLE_CONTROL
  int   toggle;          // packet's toggle bit (0, 1, or -1 for xHCI)
#endif
  Bit8u *data;
  int len;
  USBCallback *complete_cb;
  void *complete_dev;
  usb_device_c *dev;
  int strm_pid;         // stream primary id
};

typedef struct USBAsync {
  USBPacket packet;
  Bit64u    td_addr;
  bool done;
  Bit16u  slot_ep;
  struct USBAsync *next;
} USBAsync;

// Items about the endpoint gathered from various places
// These values are set at init() time, this is so we
//  don't have to parse the descriptors at runtime.
typedef struct USBEndPoint {
  int  max_packet_size;  // endpoint max packet size
  int  max_burst_size;   // endpoint max burst size (super-speed endpoint companion only)
#if HANDLE_TOGGLE_CONTROL
  int  toggle;           // the current toggle for the endpoint (0, 1, or -1 for xHCI)
#endif
} USBEndPoint;

class BOCHSAPI bx_usbdev_ctl_c : public logfunctions {
public:
  bx_usbdev_ctl_c();
  virtual ~bx_usbdev_ctl_c() {}
  void init(void);
  void exit(void);
  const char **get_device_names(void);
  void list_devices(void);
  virtual bool init_device(bx_list_c *portconf, logfunctions *hub, void **dev, USBCallback *cb, int port);
private:
  void parse_port_options(usb_device_c *dev, bx_list_c *portconf);
};

BOCHSAPI extern bx_usbdev_ctl_c bx_usbdev_ctl;

class BOCHSAPI usb_device_c : public logfunctions {
public:
  usb_device_c(void);
  virtual ~usb_device_c();

  virtual bool init() {return d.connected;}
  virtual const char *get_info() { return NULL; }
  virtual usb_device_c *find_device(Bit8u addr);

  virtual int handle_packet(USBPacket *p);
  virtual void handle_reset() {}
  virtual int handle_control(int request, int value, int index, int length, Bit8u *data) { return -1; }
  virtual int handle_data(USBPacket *p) { return 0; }
  virtual void handle_iface_change(int iface) {}
  void register_state(bx_list_c *parent);
  virtual void register_state_specific(bx_list_c *parent) {}
  virtual void after_restore_state() {}
  virtual void cancel_packet(USBPacket *p) {}
  virtual bool set_option(const char *option) { return 0; }
  virtual void runtime_config() {}

  bool get_connected() { return d.connected; }
  int get_speed() { return d.speed; }
  bool set_speed(int speed)
  {
    if ((speed >= d.minspeed) && (speed <= d.maxspeed)) {
      d.speed = speed;
      return 1;
    } else {
      return 0;
    }
  }
  int get_min_speed() { return d.minspeed; }
  
  // return information for the specified ep of the current device
#define USB_MAX_ENDPOINTS   5   // we currently don't use more than 5 endpoints (ep0, ep1, ep2, ep3, and ep4)
  int get_mps(const int ep) {
    return (ep < USB_MAX_ENDPOINTS) ? d.endpoint_info[ep].max_packet_size : 0;
  }
  int get_max_burst_size(const int ep) {
    return (ep < USB_MAX_ENDPOINTS) ? d.endpoint_info[ep].max_burst_size : 0;
  }

#if HANDLE_TOGGLE_CONTROL
  int get_toggle(const int ep) {
    return (ep < USB_MAX_ENDPOINTS) ? d.endpoint_info[ep].toggle : 0;
  }
  void set_toggle(const int ep, const int toggle) {
    if (ep < USB_MAX_ENDPOINTS) 
      d.endpoint_info[ep].toggle = toggle;
  }
#endif

  Bit8u get_type() {
    return d.type;
  }
  Bit8u get_aIface() {
    return d.alt_iface;
  }

  Bit8u get_address() {return d.addr;}
  void set_async_mode(bool async) { d.async_mode = async; }
  void set_event_handler(void *dev, USBCallback *cb, int port)
  {
    d.event.dev = dev;
    d.event.cb = cb;
    d.event.port = port;
  }
  void set_debug_mode();
  void set_pcap_mode(const char *pcap_name);
  
  void usb_send_msg(int msg);

  // manually invoke a HC event
  int hc_event(int event, usb_device_c *device) {
    if (d.event.cb != NULL)
      return d.event.cb(event, device, d.event.dev, d.event.port);
    return -1;
  }

protected:
  struct {
    Bit8u type;
    bool connected;
    int minspeed;
    int maxspeed;
    int speed;
    Bit8u addr;
    Bit8u config;
    Bit8u alt_iface;
    Bit8u alt_iface_max;
    char devname[32];
    USBEndPoint endpoint_info[USB_MAX_ENDPOINTS];

    bool first8;
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
    bool stall;
    bool async_mode;
    struct {
      USBCallback *cb;
      void *dev;
      int port;
    } event;
    bx_list_c *sr;

    bool pcap_mode;
    pcap_image_t pcapture;
  } d;

  int handle_control_common(int request, int value, int index, int length, Bit8u *data);
  void usb_dump_packet(Bit8u *data, int size, int bus, int dev_addr, int ep, int type, bool is_setup, bool can_append);
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
    DEV_MEM_READ_PHYSICAL(addr, 4, (Bit8u *) buf);
  }
}

static BX_CPP_INLINE void put_dwords(bx_phy_address addr, Bit32u *buf, int num)
{
  for (int i = 0; i < num; i++, buf++, addr += sizeof(*buf)) {
    DEV_MEM_WRITE_PHYSICAL(addr, 4, (Bit8u *) buf);
  }
}

//
// The usbdev_locator class is used by usb_device_c classes to register
// their name. USB HC emulations use the static 'create' method
// to locate and instantiate a usb_device_c class.
//
class BOCHSAPI_MSVCONLY usbdev_locator_c {
public:
  static bool module_present(const char *type);
  static void cleanup();
  static usb_device_c *create(const char *type, const char *devname);
protected:
  usbdev_locator_c(const char *type);
  virtual ~usbdev_locator_c();
  virtual usb_device_c *allocate(const char *devname) = 0;
private:
  static usbdev_locator_c *all;
  usbdev_locator_c *next;
  const char *type;
};

#endif
