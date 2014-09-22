/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// Generic USB emulation code
//
// Copyright (c) 2005       Fabrice Bellard
// Copyright (C) 2009       Benjamin D Lunt (fys at frontiernet net)
//               2009-2014  The Bochs Project
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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB

#include "usb_common.h"
#include "usb_hid.h"
#include "usb_hub.h"
#include "usb_msd.h"
#include "usb_printer.h"

#define LOG_THIS

bx_usb_devctl_c* theUsbDevCtl = NULL;

int CDECL libusb_common_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  if (type == PLUGTYPE_CORE) {
    theUsbDevCtl = new bx_usb_devctl_c;
    bx_devices.pluginUsbDevCtl = theUsbDevCtl;
    return 0; // Success
  } else {
    return -1;
  }
}

void CDECL libusb_common_LTX_plugin_fini(void)
{
  delete theUsbDevCtl;
}

bx_usb_devctl_c::bx_usb_devctl_c()
{
  put("usbdevctl", "USBCTL");
}

int bx_usb_devctl_c::init_device(bx_list_c *portconf, logfunctions *hub, void **dev, bx_list_c *sr_list)
{
  usbdev_type type = USB_DEV_TYPE_NONE;
  int ports;
  usb_device_c **device = (usb_device_c**)dev;
  const char *devname = NULL;
  size_t dnlen;

  devname = ((bx_param_string_c*)portconf->get_by_name("device"))->getptr();
  dnlen = strlen(devname);
  if (!strcmp(devname, "mouse")) {
    type = USB_DEV_TYPE_MOUSE;
    *device = new usb_hid_device_c(type);
  } else if (!strcmp(devname, "tablet")) {
    type = USB_DEV_TYPE_TABLET;
    *device = new usb_hid_device_c(type);
  } else if (!strcmp(devname, "keypad")) {
    type = USB_DEV_TYPE_KEYPAD;
    *device = new usb_hid_device_c(type);
  } else if (!strncmp(devname, "disk", 4)) {
    if ((dnlen > 5) && (devname[4] == ':')) {
      type = USB_DEV_TYPE_DISK;
      *device = new usb_msd_device_c(type, devname+5);
    } else {
      hub->panic("USB device 'disk' needs a filename separated with a colon");
      return type;
    }
  } else if (!strncmp(devname, "cdrom", 5)) {
    if ((dnlen == 5) || (devname[5] == ':')) {
      type = USB_DEV_TYPE_CDROM;
      if (dnlen > 6) {
        *device = new usb_msd_device_c(type, devname+6);
      } else {
        *device = new usb_msd_device_c(type, devname+dnlen);
      }
    } else {
      hub->panic("USB device 'cdrom' needs a filename separated with a colon");
      return type;
    }
  } else if (!strncmp(devname, "hub", 3)) {
    type = USB_DEV_TYPE_HUB;
    ports = 4;
    if (dnlen > 3) {
      if (devname[3] == ':') {
        ports = atoi(&devname[4]);
        if ((ports < 2) || (ports > BX_N_USB_HUB_PORTS)) {
          hub->panic("USB device 'hub': invalid number of ports");
        }
      } else {
        hub->panic("USB device 'hub' needs the port count separated with a colon");
      }
    }
    *device = new usb_hub_device_c(ports);
  } else if (!strncmp(devname, "printer", 7)) {
    if ((dnlen > 8) && (devname[7] == ':')) {
      type = USB_DEV_TYPE_PRINTER;
      *device = new usb_printer_device_c(type, devname+8);
    } else {
      hub->panic("USB device 'printer' needs a filename separated with a colon");
      return type;
    }
  } else {
    hub->panic("unknown USB device: %s", devname);
    return type;
  }
  if (*device != NULL) {
    (*device)->register_state(sr_list);
    parse_port_options(*device, portconf);
  }
  return type;
}

void bx_usb_devctl_c::parse_port_options(usb_device_c *device, bx_list_c *portconf)
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
        BX_ERROR(("unknown USB device speed: '%s'", opts[i]+6));
      }
      if (speed <= device->get_maxspeed()) {
        device->set_speed(speed);
      } else {
        BX_ERROR(("unsupported USB device speed: '%s'", opts[i]+6));
      }
    } else if (!device->set_option(opts[i])) {
      BX_ERROR(("unknown USB device option: '%s'", opts[i]));
    }
  }
  for (i = 1; i < (unsigned)optc; i++) {
    if (opts[i] != NULL) {
      free(opts[i]);
      opts[i] = NULL;
    }
  }
}

void bx_usb_devctl_c::usb_send_msg(void *dev, int msg)
{
  ((usb_device_c*)dev)->usb_send_msg(msg);
}

// Dumps the contents of a buffer to the log file
void usb_device_c::usb_dump_packet(Bit8u *data, unsigned size)
{
  char the_packet[256], str[16];
  strcpy(the_packet, "Packet contents (in hex):");
  unsigned offset = 0;
  for (unsigned p=0; p<size; p++) {
    if (!(p & 0x0F)) {
      BX_DEBUG(("%s", the_packet));
      sprintf(the_packet, "  0x%04X ", offset);
      offset += 16;
    }
    sprintf(str, " %02X", data[p]);
    strcat(the_packet, str);
  }
  if (strlen(the_packet))
    BX_DEBUG(("%s", the_packet));
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

// generic USB packet handler

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

void usb_device_c::register_state(bx_list_c *parent)
{
  bx_list_c *list = new bx_list_c(parent, "d", "Common USB Device State");
  new bx_shadow_num_c(list, "addr", &d.addr);
  new bx_shadow_num_c(list, "state", &d.state);
  new bx_shadow_num_c(list, "remote_wakeup", &d.remote_wakeup);
  register_state_specific(parent);
}

// base class for USB devices
usb_device_c::usb_device_c(void)
{
  memset((void*)&d, 0, sizeof(d));
}

// Send an internal message to a USB device
void usb_device_c::usb_send_msg(int msg)
{
  USBPacket p;
  memset(&p, 0, sizeof(p));
  p.pid = msg;
  handle_packet(&p);
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
