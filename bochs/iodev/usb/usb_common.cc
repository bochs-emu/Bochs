/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// Generic USB emulation code
//
// Copyright (c) 2005       Fabrice Bellard
// Copyright (C) 2009-2015  Benjamin D Lunt (fys at fysnet net)
//               2009-2021  The Bochs Project
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

#include "usb_pcap.h"
#include "usb_common.h"

#define LOG_THIS bx_usbdev_ctl.

bx_usbdev_ctl_c bx_usbdev_ctl;

const char **usb_module_names;
const char **usb_device_names;
Bit8u *usb_module_id;

bx_usbdev_ctl_c::bx_usbdev_ctl_c()
{
  put("usbdevctl", "USBCTL");
}

void bx_usbdev_ctl_c::init(void)
{
  Bit8u i, j, count;

  count = PLUG_get_plugins_count(PLUGTYPE_USB);
  usb_module_names = (const char**) malloc(count * sizeof(char*));
  usb_device_names = (const char**) malloc((count + 6) * sizeof(char*));
  usb_module_id = (Bit8u*) malloc((count + 5) * sizeof(Bit8u));
  usb_device_names[0] = "none";
  usb_module_id[0] = 0xff;
  j = 1;
  for (i = 0; i < count; i++) {
    usb_module_names[i] = PLUG_get_plugin_name(PLUGTYPE_USB, i);
    if (!strcmp(usb_module_names[i], "usb_hid")) {
      usb_device_names[j] = "mouse";
      usb_module_id[j++] = i;
      usb_device_names[j] = "tablet";
      usb_module_id[j++] = i;
      usb_device_names[j] = "keypad";
      usb_module_id[j++] = i;
      usb_device_names[j] = "keyboard";
      usb_module_id[j] = i;
    } else if (!strcmp(usb_module_names[i], "usb_msd")) {
      usb_device_names[j] = "disk";
      usb_module_id[j++] = i;
      usb_device_names[j] = "cdrom";
      usb_module_id[j] = i;
    } else {
      if (!strncmp(usb_module_names[i], "usb_", 4)) {
        usb_device_names[j] = &usb_module_names[i][4];
      } else {
        usb_device_names[j] = usb_module_names[i];
      }
      usb_module_id[j] = i;
    }
    j++;
  }
  usb_device_names[j] = NULL;
}

void bx_usbdev_ctl_c::exit(void)
{
  free(usb_module_names);
  free(usb_device_names);
  free(usb_module_id);
  usbdev_locator_c::cleanup();
}

const char **bx_usbdev_ctl_c::get_device_names(void)
{
  return usb_device_names;
}

void bx_usbdev_ctl_c::list_devices(void)
{
  char list[60];
  Bit8u i = 1; // skip "none"
  size_t len = 0, len1;

  BX_INFO(("Pluggable USB devices"));
  list[0] = 0;
  while (usb_device_names[i] != NULL) {
    len1 = strlen(usb_device_names[i]);
    if ((len + len1 + 1) > 58) {
      BX_INFO((" %s", list));
      list[0] = 0;
      len = 0;
    }
    strcat(list, " ");
    strcat(list, usb_device_names[i]);
    len = strlen(list);
    i++;
  }
  if (len > 0) {
    BX_INFO((" %s", list));
  }
}

bool bx_usbdev_ctl_c::init_device(bx_list_c *portconf, logfunctions *hub, void **dev)
{
  Bit8u devtype, modtype;
  usb_device_c **device = (usb_device_c**)dev;
  const char *options, *options2;
  char new_options[BX_PATHNAME_LEN];
  bx_param_string_c *opts2;

  devtype = (Bit8u)((bx_param_enum_c*)portconf->get_by_name("device"))->get();
  if (devtype == 0) return 0;
  modtype = usb_module_id[devtype];
  options = ((bx_param_string_c*)portconf->get_by_name("options"))->getptr();
  opts2 = (bx_param_string_c*)portconf->get_by_name("options2");
  if (opts2 != NULL) {
    // backward compatibility code
    options2 = opts2->getptr();
    if (strlen(options) > 0) {
      sprintf(new_options, "%s, %s", options2, options);
    } else {
      sprintf(new_options, "%s", options2);
    }
    ((bx_param_string_c*)portconf->get_by_name("options"))->set(new_options);
    portconf->remove("options2");
  }
  if (!usbdev_locator_c::module_present(usb_module_names[modtype])) {
#if BX_PLUGINS
    PLUG_load_plugin_var(usb_module_names[modtype], PLUGTYPE_USB);
#else
    BX_PANIC(("could not find USB module '%s'", usb_module_names[modtype]));
#endif
  }
  *device = usbdev_locator_c::create(usb_module_names[modtype],
                                     usb_device_names[devtype]);
  if (*device != NULL) {
    parse_port_options(*device, portconf);
  }
  return (*device != NULL);
}

void bx_usbdev_ctl_c::parse_port_options(usb_device_c *device, bx_list_c *portconf)
{
  const char *raw_options;
  int i, optc, speed = USB_SPEED_LOW;  // assume LOW speed device if parameter not given.
  Bit8u devtype;
  char *opts[16];

  memset(opts, 0, sizeof(opts));
  devtype = ((bx_param_enum_c*)portconf->get_by_name("device"))->get();
  raw_options = ((bx_param_string_c*)portconf->get_by_name("options"))->getptr();
  optc = bx_split_option_list("USB port options", raw_options, opts, 16);
  for (i = 0; i < optc; i++) {
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
                  usb_device_names[devtype], opts[i]+6));
      }
    } else if (!strcmp(opts[i], "debug")) {
      device->set_debug_mode();
    } else if (!strncmp(opts[i], "pcap:", 5)) {
      device->set_pcap_mode(opts[i]+5);
    } else if (!device->set_option(opts[i])) {
      BX_ERROR(("ignoring unknown USB device option: '%s'", opts[i]));
    }
  }
  for (i = 0; i < optc; i++) {
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

bool usbdev_locator_c::module_present(const char *type)
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
    PLUG_unload_plugin_type(all->type, PLUGTYPE_USB);
  }
#endif
}

//
// Called by USB HC emulations to locate and create a usb_device_c
// object
//
usb_device_c* usbdev_locator_c::create(const char *type, const char *devname)
{
  usbdev_locator_c *ptr = 0;

  for (ptr = all; ptr != NULL; ptr = ptr->next) {
    if (strcmp(type, ptr->type) == 0)
      return (ptr->allocate(devname));
  }
  return NULL;
}

#undef LOG_THIS
#define LOG_THIS

// Base class for USB devices

usb_device_c::usb_device_c(void)
{
  memset((void*)&d, 0, sizeof(d));
  d.async_mode = 1;
}

usb_device_c::~usb_device_c()
{
  if (d.sr != NULL)
    d.sr->clear();
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
      usb_dump_packet(data, 8, 0, p->devaddr, USB_DIR_OUT | p->devep, USB_TRANS_TYPE_CONTROL, true, false);
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
                usb_dump_packet(d.data_buf, ret, 0, p->devaddr, USB_DIR_IN | p->devep, USB_TRANS_TYPE_CONTROL, false, true);
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
                usb_dump_packet(data, ret, 0, p->devaddr, USB_DIR_IN | p->devep, USB_TRANS_TYPE_CONTROL, false, true);
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
              usb_dump_packet(p->data, p->len, 0, p->devaddr, USB_DIR_OUT | p->devep, USB_TRANS_TYPE_CONTROL, false, true);
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
                usb_dump_packet(data, ret, 0, p->devaddr, USB_DIR_OUT | p->devep, USB_TRANS_TYPE_CONTROL, false, true);
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
      data[0] = d.iface;
      ret = 1;
      break;
    case InterfaceOutRequest | USB_REQ_SET_INTERFACE:
      BX_DEBUG(("USB_REQ_SET_INTERFACE: value=%d", value));
      d.iface = value;
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
  BXRS_DEC_PARAM_FIELD(list, interface, d.iface);
  BXRS_DEC_PARAM_FIELD(list, state, d.state);
  BXRS_DEC_PARAM_FIELD(list, remote_wakeup, d.remote_wakeup);
  register_state_specific(parent);
}

// Turn on BX_DEBUG messages at connection time
void usb_device_c::set_debug_mode()
{
  setonoff(LOGLEV_DEBUG, ACT_REPORT);
}

void usb_device_c::set_pcap_mode(const char *pcap_name)
{
  d.pcap_mode = (d.pcapture.create_pcap(pcap_name) >= 0);
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
void usb_device_c::usb_dump_packet(Bit8u *data, unsigned size, int bus, int dev_addr, int ep, int type, bool is_setup, bool can_append)
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

  if (d.pcap_mode) {
    d.pcapture.write_packet(data, size, bus, dev_addr, ep, type, is_setup, can_append);
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
  *q++ = (Bit8u)(2 * len + 2);
  *q++ = 3;
  for(i = 0; i < len; i++) {
    *q++ = str[i];
    *q++ = 0;
  }
  return q - buf;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
