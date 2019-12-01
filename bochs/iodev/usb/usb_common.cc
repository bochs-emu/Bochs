/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// Generic USB emulation code
//
// Copyright (c) 2005       Fabrice Bellard
// Copyright (C) 2009-2015  Benjamin D Lunt (fys at fysnet net)
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

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB

#include "usb_common.h"

#define LOG_THIS bx_usbdev_ctl.

bx_usbdev_ctl_c bx_usbdev_ctl;

bx_usbdev_ctl_c::bx_usbdev_ctl_c()
{
  put("usbdevctl", "USBCTL");
}

void bx_usbdev_ctl_c::init(void)
{
  // Nothing here yet
}

void bx_usbdev_ctl_c::exit(void)
{
  usbdev_locator_c::cleanup();
}

const char *usbmod_names[] =
{
  "none",
  "usb_cbi",
  "usb_hid",
  "usb_hub",
  "usb_msd",
  "usb_printer"
};

const char *usbdev_names[] =
{
  "none",
  "mouse",
  "tablet",
  "keypad",
  "disk",
  "cdrom",
  "hub",
  "printer",
  "floppy"
};

int bx_usbdev_ctl_c::init_device(bx_list_c *portconf, logfunctions *hub, void **dev, bx_list_c *sr_list)
{
  usbmod_type modtype = USB_MOD_TYPE_NONE;
  usbdev_type devtype = USB_DEV_TYPE_NONE;
  usb_device_c **device = (usb_device_c**)dev;
  const char *raw_devname = NULL;
  const char *args;
  char *devname, *ptr;
  size_t len;

  raw_devname = ((bx_param_string_c*)portconf->get_by_name("device"))->getptr();
  len = strlen(raw_devname);
  devname = new char[len + 1];
  strcpy(devname, raw_devname);
  ptr = strtok(devname, ":");
  ptr = strtok(NULL, "\n");
  if (ptr == NULL) {
    args = raw_devname+strlen(devname);
  } else {
    args = raw_devname+(ptr-devname);
  }
  if (!strcmp(devname, "mouse")) {
    modtype = USB_MOD_TYPE_HID;
    devtype = USB_DEV_TYPE_MOUSE;
  } else if (!strcmp(devname, "tablet")) {
    modtype = USB_MOD_TYPE_HID;
    devtype = USB_DEV_TYPE_TABLET;
  } else if (!strcmp(devname, "keypad")) {
    modtype = USB_MOD_TYPE_HID;
    devtype = USB_DEV_TYPE_KEYPAD;
  } else if (!strcmp(devname, "disk")) {
    if (ptr != NULL) {
      modtype = USB_MOD_TYPE_MSD;
      devtype = USB_DEV_TYPE_DISK;
    } else {
      delete [] devname;
      hub->panic("USB device 'disk' needs a filename separated with a colon");
      return devtype;
    }
  } else if (!strcmp(devname, "cdrom")) {
    modtype = USB_MOD_TYPE_MSD;
    devtype = USB_DEV_TYPE_CDROM;
  } else if (!strcmp(devname, "hub")) {
    modtype = USB_MOD_TYPE_HUB;
    devtype = USB_DEV_TYPE_HUB;
  } else if (!strcmp(devname, "printer")) {
    if (ptr != NULL) {
      modtype = USB_MOD_TYPE_PRINTER;
      devtype = USB_DEV_TYPE_PRINTER;
    } else {
      hub->panic("USB device 'printer' needs a filename separated with a colon");
      delete [] devname;
      return devtype;
    }
  } else if (!strncmp(devname, "floppy", 6)) {
    modtype = USB_MOD_TYPE_CBI;
    devtype = USB_DEV_TYPE_FLOPPY;
  } else {
    hub->panic("unknown USB device: %s", devname);
    delete [] devname;
    return devtype;
  }
  if (!usbdev_locator_c::module_present(usbmod_names[modtype])) {
#if BX_PLUGINS
    PLUG_load_usb_plugin(usbmod_names[modtype]);
#else
    BX_PANIC(("could not find USB device '%s'", usbmod_names[modtype]));
#endif
  }
  *device = usbdev_locator_c::create(usbmod_names[modtype], devtype, args);
  if (*device != NULL) {
    (*device)->register_state(sr_list);
    parse_port_options(*device, portconf);
  }
  delete [] devname;
  return devtype;
}

void bx_usbdev_ctl_c::parse_port_options(usb_device_c *device, bx_list_c *portconf)
{
  const char *raw_options;
  char *options;
  unsigned i, string_i;
  int optc, speed = USB_SPEED_LOW;  // assume LOW speed device if parameter not given.
  char *opts[16];
  char *ptr;
  char string[512];
  size_t len;

  memset(opts, 0, sizeof(opts));
  optc = 0;
  raw_options = ((bx_param_string_c*)portconf->get_by_name("options"))->getptr();
  len = strlen(raw_options);
  if ((len > 0) && (strcmp(raw_options, "none"))) {
    options = new char[len + 1];
    strcpy(options, raw_options);
    ptr = strtok(options, ",");
    while (ptr) {
      string_i = 0;
      for (i=0; i<strlen(ptr); i++) {
        if (!isspace(ptr[i])) string[string_i++] = ptr[i];
      }
      string[string_i] = '\0';
      if (opts[optc] != NULL) {
        free(opts[optc]);
        opts[optc] = NULL;
      }
      if (optc < 16) {
        opts[optc++] = strdup(string);
      } else {
        BX_ERROR(("too many parameters, max is 16"));
        break;
      }
      ptr = strtok(NULL, ",");
    }
    delete [] options;
  }

  for (i = 0; i < (unsigned)optc; i++) {
    if (!strncmp(opts[i], "speed:", 6)) {
      if (!strcmp(opts[i]+6, "low")) {
        speed = USB_SPEED_LOW;
      } else if (!strcmp(opts[i]+6, "full")) {
        speed = USB_SPEED_FULL;
      } else if (!strcmp(opts[i]+6, "high")) {
        speed = USB_SPEED_HIGH;
      } else if (!strcmp(opts[i]+6, "super")) {
        speed = USB_SPEED_SUPER;
      } else {
        BX_ERROR(("ignoring unknown USB device speed: '%s'", opts[i]+6));
      }
      if (!device->set_speed(speed)) {
        BX_PANIC(("USB device '%s' doesn't support '%s' speed",
                  usbdev_names[device->get_type()], opts[i]+6));
      }
    } else if (!strcmp(opts[i], "debug")) {
      device->set_debug_mode();
    } else if (!device->set_option(opts[i])) {
      BX_ERROR(("ignoring unknown USB device option: '%s'", opts[i]));
    }
  }
  for (i = 1; i < (unsigned)optc; i++) {
    if (opts[i] != NULL) {
      free(opts[i]);
      opts[i] = NULL;
    }
  }
}

usbdev_locator_c *usbdev_locator_c::all;

//
// Each USB device module has a static locator class that registers
// here
//
usbdev_locator_c::usbdev_locator_c(const char *type)
{
  next = all;
  all  = this;
  this->type = type;
}

usbdev_locator_c::~usbdev_locator_c()
{
  usbdev_locator_c *ptr = 0;

  if (this == all) {
    all = all->next;
  } else {
    ptr = all;
    while (ptr != NULL) {
      if (ptr->next != this) {
        ptr = ptr->next;
      } else {
        break;
      }
    }
  }
  if (ptr) {
    ptr->next = this->next;
  }
}

bx_bool usbdev_locator_c::module_present(const char *type)
{
  usbdev_locator_c *ptr = 0;

  for (ptr = all; ptr != NULL; ptr = ptr->next) {
    if (strcmp(type, ptr->type) == 0)
      return 1;
  }
  return 0;
}

void usbdev_locator_c::cleanup()
{
#if BX_PLUGINS
  while (all != NULL) {
    PLUG_unload_usb_plugin(all->type);
  }
#endif
}

//
// Called by USB HC emulations to locate and create a usb_device_c
// object
//
usb_device_c*
usbdev_locator_c::create(const char *type, usbdev_type devtype, const char *args)
{
  usbdev_locator_c *ptr = 0;

  for (ptr = all; ptr != NULL; ptr = ptr->next) {
    if (strcmp(type, ptr->type) == 0)
      return (ptr->allocate(devtype, args));
  }
  return NULL;
}

// Base class for USB devices

usb_device_c::usb_device_c(void)
{
  memset((void*)&d, 0, sizeof(d));
  d.async_mode = 1;
}

// Find device with given address
usb_device_c* usb_device_c::find_device(Bit8u addr)
{
  if (addr == d.addr) {
    return this;
  } else {
    return NULL;
  }
}

// Generic USB packet handler

#define SETUP_STATE_IDLE 0
#define SETUP_STATE_DATA 1
#define SETUP_STATE_ACK  2

int usb_device_c::handle_packet(USBPacket *p)
{
  int l, ret = 0;
  int len = p->len;
  Bit8u *data = p->data;

  switch(p->pid) {
    case USB_MSG_ATTACH:
      d.state = USB_STATE_ATTACHED;
      break;
    case USB_MSG_DETACH:
      d.state = USB_STATE_NOTATTACHED;
      break;
    case USB_MSG_RESET:
      d.remote_wakeup = 0;
      d.addr = 0;
      d.state = USB_STATE_DEFAULT;
      handle_reset();
      break;
    case USB_TOKEN_SETUP:
      if (d.state < USB_STATE_DEFAULT || p->devaddr != d.addr)
        return USB_RET_NODEV;
      if (len != 8)
        goto fail;
      d.stall = 0;
      memcpy(d.setup_buf, data, 8);
      d.setup_len = (d.setup_buf[7] << 8) | d.setup_buf[6];
      d.setup_index = 0;
      if (d.setup_buf[0] & USB_DIR_IN) {
        ret = handle_control((d.setup_buf[0] << 8) | d.setup_buf[1],
                             (d.setup_buf[3] << 8) | d.setup_buf[2],
                             (d.setup_buf[5] << 8) | d.setup_buf[4],
                             d.setup_len, d.data_buf);
        if (ret < 0)
          return ret;
        if (ret < d.setup_len)
          d.setup_len = ret;
        d.setup_state = SETUP_STATE_DATA;
      } else {
        if (d.setup_len == 0)
          d.setup_state = SETUP_STATE_ACK;
        else
          d.setup_state = SETUP_STATE_DATA;
      }
      break;
    case USB_TOKEN_IN:
      if (d.state < USB_STATE_DEFAULT || p->devaddr != d.addr)
        return USB_RET_NODEV;
      if (d.stall) goto fail;
      switch(p->devep) {
        case 0:
          switch(d.setup_state) {
            case SETUP_STATE_ACK:
              if (!(d.setup_buf[0] & USB_DIR_IN)) {
                d.setup_state = SETUP_STATE_IDLE;
                ret = handle_control((d.setup_buf[0] << 8) | d.setup_buf[1],
                                     (d.setup_buf[3] << 8) | d.setup_buf[2],
                                     (d.setup_buf[5] << 8) | d.setup_buf[4],
                                     d.setup_len, d.data_buf);
                if (ret > 0)
                  ret = 0;
              } else {
                // return 0 byte
              }
              break;
            case SETUP_STATE_DATA:
              if (d.setup_buf[0] & USB_DIR_IN) {
                l = d.setup_len - d.setup_index;
                if (l > len)
                  l = len;
                memcpy(data, d.data_buf + d.setup_index, l);
                d.setup_index += l;
                if (d.setup_index >= d.setup_len)
                  d.setup_state = SETUP_STATE_ACK;
                ret = l;
              } else {
                d.setup_state = SETUP_STATE_IDLE;
                goto fail;
              }
              break;
            default:
                goto fail;
            }
            break;
        default:
            ret = handle_data(p);
            break;
        }
        break;
    case USB_TOKEN_OUT:
        if (d.state < USB_STATE_DEFAULT || p->devaddr != d.addr)
          return USB_RET_NODEV;
        if (d.stall) goto fail;
        switch(p->devep) {
        case 0:
          switch(d.setup_state) {
            case SETUP_STATE_ACK:
              if (d.setup_buf[0] & USB_DIR_IN) {
                d.setup_state = SETUP_STATE_IDLE;
                // transfer OK
              } else {
                // ignore additionnal output
              }
              break;
            case SETUP_STATE_DATA:
              if (!(d.setup_buf[0] & USB_DIR_IN)) {
                l = d.setup_len - d.setup_index;
                if (l > len)
                  l = len;
                memcpy(d.data_buf + d.setup_index, data, l);
                d.setup_index += l;
                if (d.setup_index >= d.setup_len)
                  d.setup_state = SETUP_STATE_ACK;
                ret = l;
              } else {
                // it is okay for a host to send an OUT before it reads
                //  all of the expected IN.  It is telling the controller
                //  that it doesn't want any more from that particular call.
                ret = 0;
                d.setup_state = SETUP_STATE_IDLE;
              }
              break;
            default:
              goto fail;
          }
          break;
        default:
          ret = handle_data(p);
          break;
      }
      break;
    default:
    fail:
      d.stall = 1;
      ret = USB_RET_STALL;
      break;
  }
  return ret;
}

int usb_device_c::handle_control_common(int request, int value, int index, int length, Bit8u *data)
{
  int ret = -1;

  switch (request) {
    case DeviceOutRequest | USB_REQ_SET_ADDRESS:
      BX_DEBUG(("USB_REQ_SET_ADDRESS:"));
      d.state = USB_STATE_ADDRESS;
      d.addr = value;
      ret = 0;
      break;
    case DeviceRequest | USB_REQ_GET_DESCRIPTOR:
      switch (value >> 8) {
        case USB_DT_DEVICE:
          BX_DEBUG(("USB_REQ_GET_DESCRIPTOR: Device"));
          memcpy(data, d.dev_descriptor, d.device_desc_size);
          ret = d.device_desc_size;
          break;
        case USB_DT_CONFIG:
          BX_DEBUG(("USB_REQ_GET_DESCRIPTOR: Config"));
          memcpy(data, d.config_descriptor, d.config_desc_size);
          ret = d.config_desc_size;
          break;
        case USB_DT_STRING:
          BX_DEBUG(("USB_REQ_GET_DESCRIPTOR: String"));
          switch(value & 0xff) {
            case 0:
              // language IDs
              data[0] = 4;
              data[1] = 3;
              data[2] = 0x09;
              data[3] = 0x04;
              ret = 4;
              break;
            case 1:
              // vendor description
              ret = set_usb_string(data, d.vendor_desc);
              break;
            case 2:
              // product description
              ret = set_usb_string(data, d.product_desc);
              break;
            case 3:
              // serial number
              ret = set_usb_string(data, d.serial_num);
              break;
          }
          break;
      }
      break;
    case DeviceRequest | USB_REQ_GET_STATUS:
      BX_DEBUG(("USB_REQ_GET_STATUS:"));
      data[0] = 0x00;
      if (d.config_descriptor[7] & 0x40) {
        data[0] |= (1 << USB_DEVICE_SELF_POWERED);
      }
      if (d.remote_wakeup) {
        data[0] |= (1 << USB_DEVICE_REMOTE_WAKEUP);
      }
      data[1] = 0x00;
      ret = 2;
      break;
    case DeviceRequest | USB_REQ_GET_CONFIGURATION:
      BX_DEBUG(("USB_REQ_GET_CONFIGURATION:"));
      data[0] = d.config;
      ret = 1;
      break;
    case DeviceOutRequest | USB_REQ_SET_CONFIGURATION:
      BX_DEBUG(("USB_REQ_SET_CONFIGURATION: value=%d", value));
      d.config = value;
      d.state = USB_STATE_CONFIGURED;
      ret = 0;
      break;
    case DeviceOutRequest | USB_REQ_CLEAR_FEATURE:
      if (value == USB_DEVICE_REMOTE_WAKEUP) {
        d.remote_wakeup = 0;
        ret = 0;
      }
      break;
    case DeviceOutRequest | USB_REQ_SET_FEATURE:
      if (value == USB_DEVICE_REMOTE_WAKEUP) {
        d.remote_wakeup = 1;
        ret = 0;
      }
      break;
    case InterfaceRequest | USB_REQ_GET_INTERFACE:
      BX_DEBUG(("USB_REQ_GET_INTERFACE:"));
      data[0] = d.interface;
      ret = 1;
      break;
    case InterfaceOutRequest | USB_REQ_SET_INTERFACE:
      BX_DEBUG(("USB_REQ_SET_INTERFACE: value=%d", value));
      d.interface = value;
      ret = 0;
      break;
  }
  return ret;
}

void usb_device_c::register_state(bx_list_c *parent)
{
  d.sr = parent;
  bx_list_c *list = new bx_list_c(parent, "d", "Common USB Device State");
  BXRS_DEC_PARAM_FIELD(list, addr, d.addr);
  BXRS_DEC_PARAM_FIELD(list, config, d.config);
  BXRS_DEC_PARAM_FIELD(list, interface, d.interface);
  BXRS_DEC_PARAM_FIELD(list, state, d.state);
  BXRS_DEC_PARAM_FIELD(list, remote_wakeup, d.remote_wakeup);
  register_state_specific(parent);
}

// Turn on BX_DEBUG messages at connection time
void usb_device_c::set_debug_mode()
{
  setonoff(LOGLEV_DEBUG, ACT_REPORT);
}

// Send an internal message to a USB device
void usb_device_c::usb_send_msg(int msg)
{
  USBPacket p;
  memset(&p, 0, sizeof(p));
  p.pid = msg;
  handle_packet(&p);
}

// Dumps the contents of a buffer to the log file
void usb_device_c::usb_dump_packet(Bit8u *data, unsigned size)
{
  char buf_str[1025], temp_str[17];

  if (getonoff(LOGLEV_DEBUG) == ACT_REPORT) {
    BX_DEBUG(("packet hexdump (%i bytes)", size));
    buf_str[0] = 0;
    for (unsigned i = 0; i < size; i++) {
      sprintf(temp_str, "%02X ", data[i]);
      strcat(buf_str, temp_str);
      if ((i % 16) == 15) {
        BX_DEBUG(("%s", buf_str));
        buf_str[0] = 0;
      }
    }
    if (strlen(buf_str) > 0) BX_DEBUG(("%s", buf_str));
  }
}

int usb_device_c::set_usb_string(Bit8u *buf, const char *str)
{
  size_t len, i;
  Bit8u *q;

  q = buf;
  len = strlen(str);
  if (len > 32) {
    *q = 0;
    return 0;
  }
  *q++ = 2 * len + 2;
  *q++ = 3;
  for(i = 0; i < len; i++) {
    *q++ = str[i];
    *q++ = 0;
  }
  return q - buf;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
