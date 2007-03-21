/////////////////////////////////////////////////////////////////////////
// $Id: usb_hid.cc,v 1.1 2007-03-21 18:54:41 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2007  Volker Ruppert
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

// USB HID emulation support ported from the Qemu project


// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#if BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
#include "usb_hid.h"

#define LOG_THIS

/* HID interface requests */
#define GET_REPORT   0xa101
#define GET_IDLE     0xa102
#define GET_PROTOCOL 0xa103
#define SET_IDLE     0x210a
#define SET_PROTOCOL 0x210b

static const Bit8u bx_mouse_dev_descriptor[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x00, 0x01, /*  u16 bcdUSB; v1.0 */

  0x00,       /*  u8  bDeviceClass; */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; [ low/full speeds only ] */
  0x08,       /*  u8  bMaxPacketSize0; 8 Bytes */

  0x27, 0x06, /*  u16 idVendor; */
  0x01, 0x00, /*  u16 idProduct; */
  0x00, 0x00, /*  u16 bcdDevice */

  0x03,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x01,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

static const Bit8u bx_mouse_config_descriptor[] = {
  /* one configuration */
  0x09,       /*  u8  bLength; */
  0x02,       /*  u8  bDescriptorType; Configuration */
  0x22, 0x00, /*  u16 wTotalLength; */
  0x01,       /*  u8  bNumInterfaces; (1) */
  0x01,       /*  u8  bConfigurationValue; */
  0x04,       /*  u8  iConfiguration; */
  0xa0,       /*  u8  bmAttributes; 
			 Bit 7: must be set,
			     6: Self-powered,
			     5: Remote wakeup,
			     4..0: resvd */
  50,         /*  u8  MaxPower; */

  /* USB 1.1:
   * USB 2.0, single TT organization (mandatory):
   *	one interface, protocol 0
   *
   * USB 2.0, multiple TT organization (optional):
   *	two interfaces, protocols 1 (like single TT)
   *	and 2 (multiple TT mode) ... config is
   *	sometimes settable
   *	NOT IMPLEMENTED
   */

  /* one interface */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x01,       /*  u8  if_bNumEndpoints; */
  0x03,       /*  u8  if_bInterfaceClass; */
  0x01,       /*  u8  if_bInterfaceSubClass; */
  0x02,       /*  u8  if_bInterfaceProtocol; [usb1.1 or single tt] */
  0x05,       /*  u8  if_iInterface; */

  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x01, 0x00,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  50, 0,       /*  u16 len */

  /* one endpoint (status change endpoint) */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x03, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x0a,       /*  u8  ep_bInterval; (255ms -- usb 2.0 spec) */
};

static const Bit8u bx_mouse_hid_report_descriptor[] = {
  0x05, 0x01, 0x09, 0x02, 0xA1, 0x01, 0x09, 0x01, 
  0xA1, 0x00, 0x05, 0x09, 0x19, 0x01, 0x29, 0x03,
  0x15, 0x00, 0x25, 0x01, 0x95, 0x03, 0x75, 0x01, 
  0x81, 0x02, 0x95, 0x01, 0x75, 0x05, 0x81, 0x01,
  0x05, 0x01, 0x09, 0x30, 0x09, 0x31, 0x15, 0x81, 
  0x25, 0x7F, 0x75, 0x08, 0x95, 0x02, 0x81, 0x06,
  0xC0, 0xC0,
};

usb_hid_device_t::usb_hid_device_t(usbdev_type type)
{
  d.type = type;
  d.speed = USB_SPEED_FULL;
  d.connected = 1;
  memset((void*)&s, 0, sizeof(s));

  put("USBHI");
  settype(PCIUSBLOG);
}

usb_hid_device_t::~usb_hid_device_t(void)
{
}

void usb_hid_device_t::handle_reset()
{
  memset((void*)&s, 0, sizeof(s));
  BX_DEBUG(("Reset"));
}

int usb_hid_device_t::handle_control(int request, int value, int index, int length, Bit8u *data)
{
  int ret = 0;

  switch(request) {
    case DeviceRequest | USB_REQ_GET_STATUS:
      data[0] = (1 << USB_DEVICE_SELF_POWERED) |
        (d.remote_wakeup << USB_DEVICE_REMOTE_WAKEUP);
      data[1] = 0x00;
      ret = 2;
      break;
    case DeviceOutRequest | USB_REQ_CLEAR_FEATURE:
      if (value == USB_DEVICE_REMOTE_WAKEUP) {
        d.remote_wakeup = 0;
      } else {
        goto fail;
      }
      ret = 0;
      break;
    case DeviceOutRequest | USB_REQ_SET_FEATURE:
      if (value == USB_DEVICE_REMOTE_WAKEUP) {
        d.remote_wakeup = 1;
      } else {
        goto fail;
      }
      ret = 0;
      break;
    case DeviceOutRequest | USB_REQ_SET_ADDRESS:
      d.addr = value;
      ret = 0;
      break;
    case DeviceRequest | USB_REQ_GET_DESCRIPTOR:
      switch(value >> 8) {
        case USB_DT_DEVICE:
          memcpy(data, bx_mouse_dev_descriptor, 
                 sizeof(bx_mouse_dev_descriptor));
          ret = sizeof(bx_mouse_dev_descriptor);
          break;
        case USB_DT_CONFIG:
          if (d.type == USB_DEV_TYPE_MOUSE) {
            memcpy(data, bx_mouse_config_descriptor, 
	           sizeof(bx_mouse_config_descriptor));
            ret = sizeof(bx_mouse_config_descriptor);
	  } else {
            goto fail;
	  }		
          break;
        case USB_DT_STRING:
          switch(value & 0xff) {
            case 0:
              /* language ids */
              data[0] = 4;
              data[1] = 3;
              data[2] = 0x09;
              data[3] = 0x04;
              ret = 4;
              break;
            case 1:
              /* serial number */
              ret = set_usb_string(data, "1");
              break;
            case 2:
              /* product description */
              if (d.type == USB_DEV_TYPE_MOUSE) {
                ret = set_usb_string(data, "BOCHS USB Mouse");
              } else {
                goto fail;
              }
              break;
            case 3:
              /* vendor description */
              ret = set_usb_string(data, "BOCHS");
              break;
            case 4:
              ret = set_usb_string(data, "HID Mouse");
              break;
            case 5:
              ret = set_usb_string(data, "Endpoint1 Interrupt Pipe");
              break;
            default:
              goto fail;
          }
          break;
        default:
          goto fail;
      }
      break;
    case DeviceRequest | USB_REQ_GET_CONFIGURATION:
      data[0] = 1;
      ret = 1;
      break;
    case DeviceOutRequest | USB_REQ_SET_CONFIGURATION:
      ret = 0;
      break;
    case DeviceRequest | USB_REQ_GET_INTERFACE:
      data[0] = 0;
      ret = 1;
      break;
    case DeviceOutRequest | USB_REQ_SET_INTERFACE:
      ret = 0;
      break;
      /* hid specific requests */
    case InterfaceRequest | USB_REQ_GET_DESCRIPTOR:
      switch(value >> 8) {
        case 0x22:
          if (d.type == USB_DEV_TYPE_MOUSE) {
            memcpy(data, bx_mouse_hid_report_descriptor, 
                   sizeof(bx_mouse_hid_report_descriptor));
            ret = sizeof(bx_mouse_hid_report_descriptor);
          } else {
            goto fail;
	  }
	  break;
        default:
          goto fail;
        }
        break;
    case GET_REPORT:
      if (d.type == USB_DEV_TYPE_MOUSE) {
        ret = mouse_poll(data, length);
      } else {
        goto fail;
      }
      break;
    case SET_IDLE:
      ret = 0;
      break;
    default:
    fail:
      ret = USB_RET_STALL;
      break;
  }
  return ret;
}

int usb_hid_device_t::handle_data(USBPacket *p)
{
  int ret = 0;

  switch(p->pid) {
    case USB_TOKEN_IN:
      if (p->devep == 1) {
        if (d.type == USB_DEV_TYPE_MOUSE) {
          ret = mouse_poll(p->data, p->len);
        } else {
          goto fail;
        }
      } else {
        goto fail;
      }
      break;
    case USB_TOKEN_OUT:
    default:
    fail:
      ret = USB_RET_STALL;
      break;
  }
  return ret;
}

int usb_hid_device_t::mouse_poll(Bit8u *buf, int len)
{
  int l;

  if (!s.mouse_x && !s.mouse_y) {
    // if there's no new movement, handle delayed one
    mouse_enq(0, 0, s.mouse_z, s.b_state);
  }
  buf[0] = (Bit8u) s.b_state;
  buf[1] = (Bit8s) s.mouse_x;
  buf[2] = (Bit8s) s.mouse_y;
  s.b_state = 0;
  s.mouse_x = 0;
  s.mouse_y = 0;
  l = 3;
  if (len >= 4) {
    buf[3] = (Bit8s) s.mouse_z; // if wheel mouse
    s.mouse_z = 0;
    l = 4;
  }
  return l;
}

void usb_hid_device_t::mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state)
{
  // scale down the motion
  if ( (delta_x < -1) || (delta_x > 1) )
    delta_x /= 2;
  if ( (delta_y < -1) || (delta_y > 1) )
    delta_y /= 2;

  if(delta_x>127) delta_x=127;
  if(delta_y>127) delta_y=127;
  if(delta_x<-128) delta_x=-128;
  if(delta_y<-128) delta_y=-128;

  s.mouse_delayed_dx+=delta_x;
  s.mouse_delayed_dy-=delta_y;

  if (s.mouse_delayed_dx > 127) {
    delta_x = 127;
    s.mouse_delayed_dx -= 127;
  } else if (s.mouse_delayed_dx < -128) {
    delta_x = -128;
    s.mouse_delayed_dx += 128;
  } else {
    delta_x = s.mouse_delayed_dx;
    s.mouse_delayed_dx = 0;
  }
  if (s.mouse_delayed_dy > 127) {
    delta_y = 127;
    s.mouse_delayed_dy -= 127;
  } else if (s.mouse_delayed_dy < -128) {
    delta_y = -128;
    s.mouse_delayed_dy += 128;
  } else {
    delta_y = s.mouse_delayed_dy;
    s.mouse_delayed_dy = 0;
  }

  s.mouse_x = (Bit8s) delta_x;
  s.mouse_y = (Bit8s) delta_y;
  s.mouse_z = (Bit8s) delta_z;
  s.b_state = (Bit8u) button_state;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
