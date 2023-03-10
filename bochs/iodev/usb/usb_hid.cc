/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// USB HID emulation support (mouse and tablet) ported from QEMU
// USB keypad emulation based on code by Benjamin D Lunt (fys [at] fysnet [dot] net)
// USB keyboard emulation is an extension to the keypad based on specs
//
// Copyright (c) 2005       Fabrice Bellard
// Copyright (c) 2007       OpenMoko, Inc.  (andrew@openedhand.com)
// Copyright (C) 2009-2023  The Bochs Project
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
//  Portions of this file contain code released under the LGPL.
//
// Copyright (C) 2004-2023  Benjamin D Lunt (fys [at] fysnet [dot] net)
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
/////////////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
#include "usb_common.h"
#include "usb_hid.h"

#define LOG_THIS

// USB device plugin entry point

PLUGIN_ENTRY_FOR_MODULE(usb_hid)
{
  if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_USB;
  }
  return 0; // Success
}

//
// Define the static class that registers the derived USB device class,
// and allocates one on request.
//
class bx_usb_hid_locator_c : public usbdev_locator_c {
public:
  bx_usb_hid_locator_c(void) : usbdev_locator_c("usb_hid") {}
protected:
  usb_device_c *allocate(const char *devtype) {
    return (new usb_hid_device_c(devtype));
  }
} bx_usb_hid_match;

/* supported HID device types */
#define USB_HID_TYPE_MOUSE    0
#define USB_HID_TYPE_TABLET   1
#define USB_HID_TYPE_KEYPAD   2
#define USB_HID_TYPE_KEYBOARD 3

/* HID IDLE time constant */
#define HID_IDLE_TIME 4000

/* HID interface requests */
#define GET_REPORT   0x01
#define GET_IDLE     0x02
#define GET_PROTOCOL 0x03
#define SET_REPORT   0x09
#define SET_IDLE     0x0A
#define SET_PROTOCOL 0x0B

/* BOOT Protocol or Report Protocol */
#define PROTOCOL_BOOT    0
#define PROTOCOL_REPORT  1

struct USBKBD {
  Bit8u code;
  bool modkey;
};

// If you change any of the Max Packet Size, or other fields within these
//  descriptors, you must also change the d.endpoint_info[] array
//  to match your changes.

////////////////////////////////////////////////
// Mouse
static const Bit8u bx_mouse_dev_descriptor[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x00, 0x01, /*  u16 bcdUSB; v1.0 */

  0x00,       /*  u8  bDeviceClass; */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; [ low/full speeds only ] */
  0x08,       /*  u8  bMaxPacketSize; 8 Bytes */

  0x27, 0x06, /*  u16 idVendor; */
  0x01, 0x00, /*  u16 idProduct; */
  0x00, 0x00, /*  u16 bcdDevice */

  0x01,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x03,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

static const Bit8u bx_mouse_dev_descriptor2[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x00, 0x02, /*  u16 bcdUSB; v2.0 */

  0x00,       /*  u8  bDeviceClass; */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; [ low/full speeds only ] */
  0x40,       /*  u8  bMaxPacketSize; 64 Bytes */

  0x27, 0x06, /*  u16 idVendor; */
  0x01, 0x00, /*  u16 idProduct; */
  0x00, 0x00, /*  u16 bcdDevice */

  0x01,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x03,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

static const Bit8u bx_mouse_hid_report_descriptor[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x02,        // Usage (Mouse)
  0xA1, 0x01,        // Collection (Application)
  0x09, 0x01,        //   Usage (Pointer)
  0xA1, 0x00,        //   Collection (Physical)
  0x05, 0x09,        //     Usage Page (Button)
  0x19, 0x01,        //     Usage Minimum (0x01)
  0x29, 0x03,        //     Usage Maximum (0x03)
  0x15, 0x00,        //     Logical Minimum (0)
  0x25, 0x01,        //     Logical Maximum (1)
  0x95, 0x03,        //     Report Count (3)
  0x75, 0x01,        //     Report Size (1)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x01,        //     Report Count (1)
  0x75, 0x05,        //     Report Size (5)
  0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
  0x09, 0x30,        //     Usage (X)
  0x09, 0x31,        //     Usage (Y)
  0x09, 0x38,        //     Usage (Wheel)
  0x15, 0x81,        //     Logical Minimum (-127)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x75, 0x08,        //     Report Size (8)
  0x95, 0x03,        //     Report Count (3)
  0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              //   End Collection
  0xC0,              // End Collection
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

  /* one interface */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x01,       /*  u8  if_bNumEndpoints; */
  0x03,       /*  u8  if_bInterfaceClass; */
  0x01,       /*  u8  if_bInterfaceSubClass; */
  0x02,       /*  u8  if_bInterfaceProtocol; */
  0x05,       /*  u8  if_iInterface; */

  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_mouse_hid_report_descriptor), 0, /*  u16 len */

  /* one endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x04, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x0a,       /*  u8  ep_bInterval; (0 - 255ms -- usb 2.0 spec) */
};

static const Bit8u bx_mouse_config_descriptor2[] = {
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

  /* one interface */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x01,       /*  u8  if_bNumEndpoints; */
  0x03,       /*  u8  if_bInterfaceClass; */
  0x01,       /*  u8  if_bInterfaceSubClass; */
  0x02,       /*  u8  if_bInterfaceProtocol; */
  0x05,       /*  u8  if_iInterface; */

  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_mouse_hid_report_descriptor), 0, /*  u16 len */

  /* one endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x04, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x07,       /*  u8  ep_bInterval; (2 ^ (8-1) * 125 usecs = 8 ms) */
};

static const Bit8u bx_mouse_hid_descriptor[] = {
  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_mouse_hid_report_descriptor), 0, /*  u16 len */
};

////////////////////////////////////////////////
// tablet
static const Bit8u bx_tablet_hid_report_descriptor[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x01,        // Usage (Pointer)
  0xA1, 0x01,        // Collection (Application)
  0x09, 0x01,        //   Usage (Pointer)
  0xA1, 0x00,        //   Collection (Physical)
  0x05, 0x09,        //     Usage Page (Button)
  0x19, 0x01,        //     Usage Minimum (0x01)
  0x29, 0x03,        //     Usage Maximum (0x03)
  0x15, 0x00,        //     Logical Minimum (0)
  0x25, 0x01,        //     Logical Maximum (1)
  0x95, 0x03,        //     Report Count (3)
  0x75, 0x01,        //     Report Size (1)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x01,        //     Report Count (1)
  0x75, 0x05,        //     Report Size (5)
  0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
  0x09, 0x30,        //     Usage (X)
  0x09, 0x31,        //     Usage (Y)
  0x15, 0x00,        //     Logical Minimum (0)
  0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
  0x35, 0x00,        //     Physical Minimum (0)
  0x46, 0xFF, 0x7F,  //     Physical Maximum (32767)
  0x75, 0x10,        //     Report Size (16)
  0x95, 0x02,        //     Report Count (2)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
  0x09, 0x38,        //     Usage (Wheel)
  0x15, 0x81,        //     Logical Minimum (-127)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x35, 0x00,        //     Physical Minimum (0)
  0x45, 0x00,        //     Physical Maximum (0)
  0x75, 0x08,        //     Report Size (8)
  0x95, 0x01,        //     Report Count (1)
  0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              //   End Collection
  0xC0,              // End Collection
};

static const Bit8u bx_tablet_config_descriptor[] = {
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

  /* one interface */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x01,       /*  u8  if_bNumEndpoints; */
  0x03,       /*  u8  if_bInterfaceClass; */
  0x01,       /*  u8  if_bInterfaceSubClass; */
  0x02,       /*  u8  if_bInterfaceProtocol; */
  0x05,       /*  u8  if_iInterface; */

  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_tablet_hid_report_descriptor), 0,       /*  u16 len */

  /* one endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x08, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x0a,       /*  u8  ep_bInterval; (0 - 255ms -- usb 2.0 spec) */
};

static const Bit8u bx_tablet_config_descriptor2[] = {
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

  /* one interface */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x01,       /*  u8  if_bNumEndpoints; */
  0x03,       /*  u8  if_bInterfaceClass; */
  0x01,       /*  u8  if_bInterfaceSubClass; */
  0x02,       /*  u8  if_bInterfaceProtocol; */
  0x05,       /*  u8  if_iInterface; */

  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_tablet_hid_report_descriptor), 0,       /*  u16 len */

  /* one endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x08, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x04,       /*  u8  ep_bInterval; (2 ^ (4-1) * 125 usecs = 1 ms) */
};

static const Bit8u bx_tablet_hid_descriptor[] = {
  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_tablet_hid_report_descriptor), 0,       /*  u16 len */
};

////////////////////////////////////////////////
// keypad
static const Bit8u bx_keypad_hid_report_descriptor[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x06,        // Usage (Keyboard)
  0xA1, 0x01,        // Collection (Application)
  0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
  0x19, 0xE0,        //   Usage Minimum (0xE0)
  0x29, 0xE7,        //   Usage Maximum (0xE7)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x08,        //   Report Count (8)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x01,        //   Report Count (1)
  0x75, 0x08,        //   Report Size (8)
  0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x03,        //   Report Count (3)
  0x75, 0x01,        //   Report Size (1)
  0x05, 0x08,        //   Usage Page (LEDs)
  0x19, 0x01,        //   Usage Minimum (Num Lock)
  0x29, 0x03,        //   Usage Maximum (Scroll Lock)
  0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x95, 0x05,        //   Report Count (5)
  0x75, 0x01,        //   Report Size (1)
  0x91, 0x01,        //   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x95, 0x06,        //   Report Count (6)
  0x75, 0x08,        //   Report Size (8)
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
  0x19, 0x00,        //   Usage Minimum (0x00)
  0x2A, 0xFF, 0x00,  //   Usage Maximum (0xFF)
  0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              // End Collection
};

static const Bit8u bx_keypad_dev_descriptor[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x10, 0x01, /*  u16 bcdUSB; v1.1 */

  0x00,       /*  u8  bDeviceClass; */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; [ low/full speeds only ] */
  0x08,       /*  u8  bMaxPacketSize; 8 Bytes */

  0xB4, 0x04, /*  u16 idVendor; */
  0x01, 0x01, /*  u16 idProduct; */
  0x01, 0x00, /*  u16 bcdDevice */

  0x01,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x03,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

static const Bit8u bx_keypad_dev_descriptor2[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x00, 0x02, /*  u16 bcdUSB; v2.0 */

  0x00,       /*  u8  bDeviceClass; */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; [ low/full speeds only ] */
  0x40,       /*  u8  bMaxPacketSize; 64 Bytes */

  0xB4, 0x04, /*  u16 idVendor; */
  0x01, 0x01, /*  u16 idProduct; */
  0x01, 0x00, /*  u16 bcdDevice */

  0x01,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x03,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

static const Bit8u bx_keypad_config_descriptor[] = {
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

  /* one interface */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x01,       /*  u8  if_bNumEndpoints; */
  0x03,       /*  u8  if_bInterfaceClass; */
  0x01,       /*  u8  if_bInterfaceSubClass; */
  0x01,       /*  u8  if_bInterfaceProtocol; */
  0x05,       /*  u8  if_iInterface; */

  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_keypad_hid_report_descriptor), 0,  /*  u16 len */

  /* one endpoint (status change endpoint) */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x08, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x0a,       /*  u8  ep_bInterval; (255ms -- usb 2.0 spec) */
};

static const Bit8u bx_keypad_config_descriptor2[] = {
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

  /* one interface */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x01,       /*  u8  if_bNumEndpoints; */
  0x03,       /*  u8  if_bInterfaceClass; */
  0x01,       /*  u8  if_bInterfaceSubClass; */
  0x01,       /*  u8  if_bInterfaceProtocol; */
  0x05,       /*  u8  if_iInterface; */

  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_keypad_hid_report_descriptor), 0,  /*  u16 len */

  /* one endpoint (status change endpoint) */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x08, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x07,       /*  u8  ep_bInterval; (2 ^ (8-1) * 125 usecs = 8 ms) */
};

static const Bit8u bx_keypad_hid_descriptor[] = {
  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_keypad_hid_report_descriptor), 0,  /*  u16 len */
};

struct USBKBD usbkbd_conv[BX_KEY_NBKEYS] = {
  0x01, 1,
  0x02, 1,
  0x3a, 0, /* F1 ... F12 */
  0x3b, 0,
  0x3c, 0,
  0x3d, 0,
  0x3e, 0,
  0x3f, 0,
  0x40, 0,
  0x41, 0,
  0x42, 0,
  0x43, 0,
  0x44, 0,
  0x45, 0,
  0x10, 1,
  0x20, 1,
  0x39, 0,
  0x53, 0,
  0x04, 1,
  0x40, 1,
  0x04, 0, /* a ... z */
  0x05, 0,
  0x06, 0,
  0x07, 0,
  0x08, 0,
  0x09, 0,
  0x0a, 0,
  0x0b, 0,
  0x0c, 0,
  0x0d, 0,
  0x0e, 0,
  0x0f, 0,
  0x10, 0,
  0x11, 0,
  0x12, 0,
  0x13, 0,
  0x14, 0,
  0x15, 0,
  0x16, 0,
  0x17, 0,
  0x18, 0,
  0x19, 0,
  0x1a, 0,
  0x1b, 0,
  0x1c, 0,
  0x1d, 0,
  0x27, 0, /* 0 ... 9 */
  0x1e, 0,
  0x1f, 0,
  0x20, 0,
  0x21, 0,
  0x22, 0,
  0x23, 0,
  0x24, 0,
  0x25, 0,
  0x26, 0,
  0x29, 0,
  0x2c, 0,
  0x34, 0,
  0x36, 0,
  0x37, 0,
  0x38, 0,
  0x33, 0,
  0x2e, 0,
  0x2f, 0,
  0x31, 0,
  0x30, 0,
  0x2d, 0,
  0x35, 0,
  0x2a, 0,
  0x28, 0,
  0x2b, 0,
  0x64, 0,
  0x46, 0,
  0x47, 0,
  0x48, 0,
  0x49, 0,
  0x4c, 0,
  0x4a, 0,
  0x4d, 0,
  0x4b, 0,
  0x4e, 0,
  0x57, 0,
  0x56, 0,
  0x59, 0,
  0x5a, 0,
  0x5b, 0,
  0x5c, 0,
  0x5e, 0,
  0x5f, 0,
  0x60, 0,
  0x61, 0,
  0x62, 0,
  0x63, 0,
  0x5d, 0,
  0x52, 0,
  0x51, 0,
  0x50, 0,
  0x4f, 0,
  0x58, 0,
  0x55, 0,
  0x54, 0,
  0xe3, 0,
  0xe7, 0,
  0x65, 0,
  0x00, 0, /* BX_KEY_ALT_SYSREQ */
  0x00, 0, /* BX_KEY_CTRL_BREAK */
  0x00, 0, /* BX_KEY_INT_BACK */
  0x00, 0, /* BX_KEY_INT_FORWARD */
  0x00, 0, /* BX_KEY_INT_STOP */
  0x00, 0, /* BX_KEY_INT_MAIL */
  0x00, 0, /* BX_KEY_INT_SEARCH */
  0x00, 0, /* BX_KEY_INT_FAV */
  0x00, 0, /* BX_KEY_INT_HOME */
  0x00, 0, /* BX_KEY_MYCOMP */
  0x00, 0, /* BX_KEY_CALC */
  0x00, 0, /* BX_KEY_SLEEP */
  0x66, 0,
  0x00, 0  /* BX_KEY_WAKE */
};

usb_hid_device_c::usb_hid_device_c(const char *devname)
{
  if (!strcmp(devname, "mouse")) {
    d.type = USB_HID_TYPE_MOUSE;
  } else if (!strcmp(devname, "tablet")) {
    d.type = USB_HID_TYPE_TABLET;
  } else if (!strcmp(devname, "keypad")) {
    d.type = USB_HID_TYPE_KEYPAD;
  } else {
    d.type = USB_HID_TYPE_KEYBOARD;
  }
  d.minspeed = USB_SPEED_LOW;
  d.maxspeed = USB_SPEED_HIGH;
  d.speed = d.minspeed;
  if (d.type == USB_HID_TYPE_MOUSE) {
    strcpy(d.devname, "USB Mouse");
    DEV_register_removable_mouse((void *) this, mouse_enq_static, mouse_enabled_changed);
  } else if (d.type == USB_HID_TYPE_TABLET) {
    strcpy(d.devname, "USB Tablet");
    DEV_register_removable_mouse((void *) this, mouse_enq_static, mouse_enabled_changed);
    bx_gui->set_mouse_mode_absxy(1);
  } else if ((d.type == USB_HID_TYPE_KEYPAD) || (d.type == USB_HID_TYPE_KEYBOARD)) {
    Bit8u led_mask;
    if (d.type == USB_HID_TYPE_KEYPAD) {
      strcpy(d.devname, "USB/PS2 Keypad");
    } else {
      strcpy(d.devname, "USB/PS2 Keyboard");
    }
    if (d.type == USB_HID_TYPE_KEYPAD) {
      led_mask = BX_KBD_LED_MASK_NUM;
    } else {
      led_mask = BX_KBD_LED_MASK_ALL;
    }
    DEV_register_removable_keyboard((void *) this, gen_scancode_static,
                                    kbd_get_elements_static, led_mask);
  }
  timer_index = DEV_register_timer(this, hid_timer_handler, HID_IDLE_TIME, 0, 0,
                                   "HID idle timer");
  d.vendor_desc = "BOCHS";
  d.product_desc = d.devname;
  d.serial_num = "1";
  memset((void *) &s, 0, sizeof(s));
  
  // HID 1.11, section 7.2.6, page 54(64):
  //  "When initialized, all devices default to report protocol."
  s.boot_protocol = PROTOCOL_REPORT;

  put("usb_hid", "USBHID");
}

usb_hid_device_c::~usb_hid_device_c(void)
{
  if ((d.type == USB_HID_TYPE_MOUSE) ||
      (d.type == USB_HID_TYPE_TABLET)) {
    bx_gui->set_mouse_mode_absxy(0);
    DEV_unregister_removable_mouse((void *) this);
  } else if ((d.type == USB_HID_TYPE_KEYPAD) ||
             (d.type == USB_HID_TYPE_KEYBOARD)) {
    DEV_unregister_removable_keyboard((void *) this);
  }
  bx_pc_system.unregisterTimer(timer_index);
}

bool usb_hid_device_c::init()
{
  /*  If you wish to set DEBUG=report in the code, instead of
   *  in the configuration, simply uncomment this line.  I use
   *  it when I am working on this emulation.
   */
  //LOG_THIS setonoff(LOGLEV_DEBUG, ACT_REPORT);

  if ((d.type == USB_HID_TYPE_MOUSE) ||
      (d.type == USB_HID_TYPE_TABLET)) {
    if (d.speed == USB_SPEED_HIGH) {
      d.dev_descriptor = bx_mouse_dev_descriptor2;
      d.device_desc_size = sizeof(bx_mouse_dev_descriptor2);
      d.endpoint_info[USB_CONTROL_EP].max_packet_size = 64; // Control ep0
      d.endpoint_info[USB_CONTROL_EP].max_burst_size = 0;
      d.endpoint_info[1].max_packet_size = 4;  // In ep1
      d.endpoint_info[1].max_burst_size = 0;
    } else {
      d.dev_descriptor = bx_mouse_dev_descriptor;
      d.device_desc_size = sizeof(bx_mouse_dev_descriptor);
      d.endpoint_info[USB_CONTROL_EP].max_packet_size = 8; // Control ep0
      d.endpoint_info[USB_CONTROL_EP].max_burst_size = 0;
      d.endpoint_info[1].max_packet_size = 4;  // In ep1
      d.endpoint_info[1].max_burst_size = 0;
    }
    if (d.type == USB_HID_TYPE_MOUSE) {
      if (d.speed == USB_SPEED_HIGH) {
        d.config_descriptor = bx_mouse_config_descriptor2;
        d.config_desc_size = sizeof(bx_mouse_config_descriptor2);
      } else {
        d.config_descriptor = bx_mouse_config_descriptor;
        d.config_desc_size = sizeof(bx_mouse_config_descriptor);
      }
    } else {
      if (d.speed == USB_SPEED_HIGH) {
        d.config_descriptor = bx_tablet_config_descriptor2;
        d.config_desc_size = sizeof(bx_tablet_config_descriptor2);
      } else {
        d.config_descriptor = bx_tablet_config_descriptor;
        d.config_desc_size = sizeof(bx_tablet_config_descriptor);
      }
    }
  } else {
    if (d.speed == USB_SPEED_HIGH) {
      d.dev_descriptor = bx_keypad_dev_descriptor2;
      d.device_desc_size = sizeof(bx_keypad_dev_descriptor2);
      d.config_descriptor = bx_keypad_config_descriptor2;
      d.config_desc_size = sizeof(bx_keypad_config_descriptor2);
      d.endpoint_info[USB_CONTROL_EP].max_packet_size = 64; // Control ep0
      d.endpoint_info[USB_CONTROL_EP].max_burst_size = 0;
      d.endpoint_info[1].max_packet_size = 4;  // In ep1
      d.endpoint_info[1].max_burst_size = 0;
    } else {
      d.dev_descriptor = bx_keypad_dev_descriptor;
      d.device_desc_size = sizeof(bx_keypad_dev_descriptor);
      d.config_descriptor = bx_keypad_config_descriptor;
      d.config_desc_size = sizeof(bx_keypad_config_descriptor);
      d.endpoint_info[USB_CONTROL_EP].max_packet_size = 8; // Control ep0
      d.endpoint_info[USB_CONTROL_EP].max_burst_size = 0;
      d.endpoint_info[1].max_packet_size = 8;  // In ep1
      d.endpoint_info[1].max_burst_size = 0;
    }
  }
  d.connected = 1;
  d.alt_iface_max = 0;
  return 1;
}

const char *usb_hid_device_c::get_info()
{
  return d.devname;
}

void usb_hid_device_c::register_state_specific(bx_list_c *parent)
{
  bx_list_c *list = new bx_list_c(parent, "s", "USB HID Device State");
  BXRS_PARAM_BOOL(list, has_events, s.has_events);
  BXRS_HEX_PARAM_FIELD(list, idle, s.idle);
  BXRS_PARAM_BOOL(list, report_protocol, s.boot_protocol);
  BXRS_DEC_PARAM_FIELD(list, mouse_delayed_dx, s.mouse_delayed_dx);
  BXRS_DEC_PARAM_FIELD(list, mouse_delayed_dy, s.mouse_delayed_dy);
  BXRS_DEC_PARAM_FIELD(list, mouse_x, s.mouse_x);
  BXRS_DEC_PARAM_FIELD(list, mouse_y, s.mouse_y);
  BXRS_DEC_PARAM_FIELD(list, mouse_z, s.mouse_z);
  BXRS_HEX_PARAM_FIELD(list, b_state, s.b_state);
  BXRS_DEC_PARAM_FIELD(list, mouse_event_count, s.mouse_event_count);
  new bx_shadow_data_c(list, "mouse_event_buf", (Bit8u*)s.mouse_event_buf, 6 * BX_KBD_ELEMENTS, 1);
  if ((d.type == USB_HID_TYPE_KEYPAD) || (d.type == USB_HID_TYPE_KEYBOARD)) {
    new bx_shadow_data_c(list, "kbd_packet", s.kbd_packet, 8, 1);
    BXRS_HEX_PARAM_FIELD(list, indicators, s.indicators);
    BXRS_DEC_PARAM_FIELD(list, kbd_event_count, s.kbd_event_count);
    bx_list_c *evbuf = new bx_list_c(list, "kbd_event_buf", "");
    char pname[16];
    for (Bit8u i = 0; i < BX_KBD_ELEMENTS; i++) {
      sprintf(pname, "%u", i);
      new bx_shadow_num_c(evbuf, pname, &s.kbd_event_buf[i], BASE_HEX);
    }
  }
}

void usb_hid_device_c::handle_reset()
{
  memset((void *) &s, 0, sizeof(s));
  BX_DEBUG(("Reset"));
  
  // HID 1.11, section 7.2.6, page 54(64):
  //  "When initialized, all devices default to report protocol."
  s.boot_protocol = PROTOCOL_REPORT;
}

int usb_hid_device_c::handle_control(int request, int value, int index, int length, Bit8u *data)
{
  int ret;
  Bit8u modchange;

  // let the common handler try to handle it first
  ret = handle_control_common(request, value, index, length, data);
  if (ret >= 0) {
    return ret;
  }

  ret = 0;
  switch(request) {
    case DeviceOutRequest | USB_REQ_CLEAR_FEATURE:
      BX_DEBUG(("HID: DeviceRequest | CLEAR_FEATURE:"));
      goto fail;
      break;
    case DeviceOutRequest | USB_REQ_SET_FEATURE:
      BX_DEBUG(("HID: DeviceRequest | SET_FEATURE:"));
      goto fail;
      break;
    case DeviceRequest | USB_REQ_GET_DESCRIPTOR:
      BX_DEBUG(("HID: DeviceRequest | USB_REQ_GET_DESCRIPTOR:"));
      switch(value >> 8) {
        case USB_DT_STRING:
          switch(value & 0xff) {
            case 4:
              ret = set_usb_string(data, "HID Mouse");
              break;
            case 5:
              ret = set_usb_string(data, "Endpoint1 Interrupt Pipe");
              break;
            default:
              BX_ERROR(("USB HID handle_control: unknown string descriptor 0x%02x", value & 0xff));
              goto fail;
          }
          break;
        default:
          BX_ERROR(("USB HID handle_control: unknown descriptor type 0x%02x", value >> 8));
          goto fail;
      }
      break;
      /* hid specific requests */
    case InterfaceRequest | USB_REQ_GET_DESCRIPTOR:
      BX_DEBUG(("HID: InterfaceRequest | USB_REQ_GET_DESCRIPTOR:"));
      switch(value >> 8) {
        case 0x21:  // HID Descriptor
          if ((value & 0xFF) != 0) {
            BX_ERROR(("USB_REQ_GET_DESCRIPTOR: The Descriptor Index must be zero for this request."));
          }
          if (d.type == USB_HID_TYPE_MOUSE) {
            memcpy(data, bx_mouse_hid_descriptor,
                   sizeof(bx_mouse_hid_descriptor));
            ret = sizeof(bx_mouse_hid_descriptor);
          } else if (d.type == USB_HID_TYPE_TABLET) {
            memcpy(data, bx_tablet_hid_descriptor,
                   sizeof(bx_tablet_hid_descriptor));
            ret = sizeof(bx_tablet_hid_descriptor);
          } else if ((d.type == USB_HID_TYPE_KEYPAD) ||
                     (d.type == USB_HID_TYPE_KEYBOARD)) {
            memcpy(data, bx_keypad_hid_descriptor,
                   sizeof(bx_keypad_hid_descriptor));
            ret = sizeof(bx_keypad_hid_descriptor);
          } else {
            goto fail;
          }
          break;
        case 0x22:  // HID Report Descriptor
          if ((value & 0xFF) != 0) {
            BX_ERROR(("USB HID handle_control: The Descriptor Index must be zero for this request."));
          }
          if (d.type == USB_HID_TYPE_MOUSE) {
            memcpy(data, bx_mouse_hid_report_descriptor,
                   sizeof(bx_mouse_hid_report_descriptor));
            ret = sizeof(bx_mouse_hid_report_descriptor);
          } else if (d.type == USB_HID_TYPE_TABLET) {
            memcpy(data, bx_tablet_hid_report_descriptor,
                   sizeof(bx_tablet_hid_report_descriptor));
            ret = sizeof(bx_tablet_hid_report_descriptor);
          } else if ((d.type == USB_HID_TYPE_KEYPAD) ||
                     (d.type == USB_HID_TYPE_KEYBOARD)) {
            memcpy(data, bx_keypad_hid_report_descriptor,
                   sizeof(bx_keypad_hid_report_descriptor));
            ret = sizeof(bx_keypad_hid_report_descriptor);
          } else {
            goto fail;
          }
          break;
        case 0x23:  // HID Physical Descriptor
          BX_ERROR(("USB HID handle_control: Host requested the HID Physical Descriptor"));
          goto fail;
        default:  // 0x24 -> 0x2F
          BX_ERROR(("USB HID handle_control: unknown HID descriptor 0x%02x", value >> 8));
          goto fail;
        }
        break;
    case EndpointOutRequest | USB_REQ_CLEAR_FEATURE:
      BX_DEBUG(("HID: CLEAR_FEATURE:"));
      if ((value == 0) && (index != 0x81)) { /* clear EP halt */
        goto fail;
      }
      break;
    case InterfaceInClassRequest | GET_REPORT:
      BX_DEBUG(("HID: GET_REPORT:"));
      if ((d.type == USB_HID_TYPE_MOUSE) ||
          (d.type == USB_HID_TYPE_TABLET)) {
        ret = mouse_poll(data, length, 1);
      } else if ((d.type == USB_HID_TYPE_KEYPAD) ||
                 (d.type == USB_HID_TYPE_KEYBOARD)) {
        ret = keyboard_poll(data, length, 1);
      } else {
        goto fail;
      }
      break;
    case InterfaceOutClassRequest | SET_REPORT:
      BX_DEBUG(("HID: SET_REPORT:"));
      if (((d.type == USB_HID_TYPE_KEYPAD) ||
           (d.type == USB_HID_TYPE_KEYBOARD)) && (value == 0x0200)) { // 0x02 = Report Type: Output, 0x00 = ID
        modchange = (data[0] ^ s.indicators);
        if (modchange != 0) {
          if (modchange & 0x01) {
            DEV_kbd_set_indicator(1, BX_KBD_LED_NUM, data[0] & 0x01);
            BX_DEBUG(("NUM_LOCK %s", (data[0] & 0x01) ? "on" : "off"));
          } else if (d.type == USB_HID_TYPE_KEYBOARD) {
            if (modchange & 0x02) {
              DEV_kbd_set_indicator(1, BX_KBD_LED_CAPS, data[0] & 0x02);
              BX_DEBUG(("CAPS_LOCK %s", (data[0] & 0x02) ? "on" : "off"));
            } else if (modchange & 0x04) {
              DEV_kbd_set_indicator(1, BX_KBD_LED_SCRL, data[0] & 0x04);
              BX_DEBUG(("SCRL_LOCK %s", (data[0] & 0x04) ? "on" : "off"));
            }
          }
          s.indicators = data[0];
        }
        ret = 0;
      } else {
        goto fail;
      }
      break;
    case InterfaceInClassRequest | GET_IDLE:
      BX_DEBUG(("HID: GET_IDLE:"));
      // The wLength field should be 1 for this request
      if (length != 1) {
        BX_ERROR(("USB HID handle_control: The wLength field should be 1 for this request."));
      }
      data[0] = s.idle;
      ret = 1;
      break;
    case InterfaceOutClassRequest | SET_IDLE:
      BX_DEBUG(("HID: SET_IDLE:"));
      // The wLength field should be 0 for this request
      if (length != 0) {
        BX_ERROR(("USB HID handle_control: The wLength field should be 0 for this request."));
      }
      s.idle = (value >> 8);
      start_idle_timer();
      ret = 0;
      break;
    case InterfaceOutClassRequest | SET_PROTOCOL:
      BX_DEBUG(("HID: SET_PROTOCOL:"));
      // The wLength field should be 0 for this request
      if (length != 0) {
        BX_ERROR(("HID SET_PROTOCOL: The wLength field should be 0 for this request."));
      }
      if ((value != 0) && (value != 1)) {
        BX_ERROR(("HID SET_PROTOCOL: The wValue field must be 0 or 1 for this request."));
      }
      if (value == 0) {
        BX_DEBUG(("HID SET_PROTOCOL: SET_PROTOCOL: Boot Protocol"));
        s.boot_protocol = PROTOCOL_BOOT;
        ret = 0;
      } else if (value == 1) {
        BX_DEBUG(("HID SET_PROTOCOL: SET_PROTOCOL: Report Protocol"));
        s.boot_protocol = PROTOCOL_REPORT;
        ret = 0;
      } else
        goto fail;
      break;
    case InterfaceInClassRequest | GET_PROTOCOL:
      BX_DEBUG(("HID: GET_PROTOCOL:"));
      // The wLength field should be 1 for this request
      if (length != 1) {
        BX_ERROR(("HID GET_PROTOCOL: The wLength field should be 1 for this request."));
      }
      data[0] = (s.boot_protocol == PROTOCOL_BOOT) ? 0 : 1;
      ret = 1;
      break;
    default:
      BX_ERROR(("USB HID handle_control: unknown request 0x%04x", request));
    fail:
      d.stall = 1;
      ret = USB_RET_STALL;
      break;
  }
  return ret;
}

int usb_hid_device_c::handle_data(USBPacket *p)
{
  int ret = 0;
  
  // check that the length is <= the max packet size of the device
  if (p->len > get_mps(p->devep)) {
    BX_DEBUG(("EP%d transfer length (%d) is greater than Max Packet Size (%d).", p->devep, p->len, get_mps(p->devep)));
  }

  switch(p->pid) {
    case USB_TOKEN_IN:
      if (p->devep == 1) {
        if ((d.type == USB_HID_TYPE_MOUSE) ||
            (d.type == USB_HID_TYPE_TABLET)) {
          ret = mouse_poll(p->data, p->len, 0);
        } else if ((d.type == USB_HID_TYPE_KEYPAD) ||
                   (d.type == USB_HID_TYPE_KEYBOARD)) {
          ret = keyboard_poll(p->data, p->len, 0);
        } else {
          goto fail;
        }
      } else {
        goto fail;
      }
      break;
    case USB_TOKEN_OUT:
      BX_ERROR(("USB HID handle_data: unexpected pid TOKEN_OUT"));
    default:
    fail:
      d.stall = 1;
      ret = USB_RET_STALL;
      break;
  }
  return ret;
}

int usb_hid_device_c::mouse_poll(Bit8u *buf, int len, bool force)
{
  int l = USB_RET_NAK;

  if (d.type == USB_HID_TYPE_MOUSE) {
    if (!s.has_events) {
      // if there's no new movement, handle delayed one
      mouse_enq(0, 0, s.mouse_z, s.b_state, 0);
    }
    if (s.has_events || force) {
      if (s.mouse_event_count > 0) {
        l = get_mouse_packet(buf, len);
      } else {
        l = create_mouse_packet(buf, len);
      }
      s.has_events = (s.mouse_event_count > 0);
      start_idle_timer();
    }
  } else if (d.type == USB_HID_TYPE_TABLET) {
    if (s.has_events || force) {
      if (s.mouse_event_count > 0) {
        l = get_mouse_packet(buf, len);
      } else {
        l = create_mouse_packet(buf, len);
      }
      s.has_events = (s.mouse_event_count > 0);
      start_idle_timer();
    }
  }
  return l;
}

int usb_hid_device_c::create_mouse_packet(Bit8u *buf, int len)
{
  int l = 0;

  if (d.type == USB_HID_TYPE_TABLET) {
    buf[0] = (Bit8u) s.b_state;
    buf[1] = (Bit8u)(s.mouse_x & 0xff);
    buf[2] = (Bit8u)(s.mouse_x >> 8);
    buf[3] = (Bit8u)(s.mouse_y & 0xff);
    buf[4] = (Bit8u)(s.mouse_y >> 8);
    buf[5] = (Bit8s) s.mouse_z;
    l = 6;
  } else {
    // The HID Boot Protocol report is only three bytes long
    if (s.boot_protocol == PROTOCOL_BOOT) {
      buf[0] = (Bit8u) s.b_state;
      buf[1] = (Bit8s) s.mouse_x;
      buf[2] = (Bit8s) s.mouse_y;
      s.mouse_x = 0;
      s.mouse_y = 0;
      l = 3;
      
    // our HID report includes the first three Boot Protocol bytes,
    // followed by the z corrd if a wheel mouse is present
    } else { //  PROTOCOL_REPORT
      buf[0] = (Bit8u) s.b_state;
      buf[1] = (Bit8s) s.mouse_x;
      buf[2] = (Bit8s) s.mouse_y;
      s.mouse_x = 0;
      s.mouse_y = 0;
      if (len >= 4) {
        buf[3] = (Bit8s) s.mouse_z; // if wheel mouse
        s.mouse_z = 0;
      } else
        buf[3] = 0;
      l = 4;
    }
  }
  
  return l;
}

int usb_hid_device_c::get_mouse_packet(Bit8u *buf, int len)
{
  int l = USB_RET_NAK;

  if (s.mouse_event_count > 0) {
    if (d.type == USB_HID_TYPE_TABLET) {
      l = 6;
    } else if (len >= 4) {
      l = 4;
    } else {
      l = 3;
    }
    memcpy(buf, s.mouse_event_buf[0], l);
    if (--s.mouse_event_count > 0) {
      memmove(s.mouse_event_buf[0], s.mouse_event_buf[1],
              s.mouse_event_count * 6);
    }
  }
  return l;
}

void usb_hid_device_c::mouse_enabled_changed(void *dev, bool enabled)
{
  if (enabled && (dev != NULL)) {
    ((usb_hid_device_c *) dev)->handle_reset();
  }
}

void usb_hid_device_c::mouse_enq_static(void *dev, int delta_x, int delta_y, int delta_z, unsigned button_state, bool absxy)
{
  if (dev != NULL) {
    ((usb_hid_device_c *) dev)->mouse_enq(delta_x, delta_y, delta_z, button_state, absxy);
  }
}

void usb_hid_device_c::mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state, bool absxy)
{
  Bit16s prev_x, prev_y;

  if (d.type == USB_HID_TYPE_MOUSE) {
    // scale down the motion
    if ((delta_x < -1) || (delta_x > 1))
      delta_x /= 2;
    if ((delta_y < -1) || (delta_y > 1))
      delta_y /= 2;

    if (delta_x > 127) delta_x = 127;
    if (delta_y > 127) delta_y = 127;
    if (delta_x < -128) delta_x = -128;
    if (delta_y < -128) delta_y = -128;

    s.mouse_delayed_dx += delta_x;
    s.mouse_delayed_dy -= delta_y;

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
    if ((s.mouse_x != 0) || (s.mouse_y != 0) || (s.mouse_z != 0) || (button_state != s.b_state)) {
      s.b_state = (Bit8u) button_state;
      if (s.mouse_event_count < BX_KBD_ELEMENTS) {
        create_mouse_packet(s.mouse_event_buf[s.mouse_event_count++], 4);
      }
      s.has_events = 1;
    }
  } else if (d.type == USB_HID_TYPE_TABLET) {
    prev_x = s.mouse_x;
    prev_y = s.mouse_y;
    if (absxy) {
      s.mouse_x = delta_x;
      s.mouse_y = delta_y;
    } else {
      s.mouse_x += delta_x;
      s.mouse_y -= delta_y;
    }
    if (s.mouse_x < 0)
      s.mouse_x = 0;
    if (s.mouse_y < 0)
      s.mouse_y = 0;
    if ((s.mouse_x != prev_x) || (s.mouse_y != prev_y) || (delta_z != s.mouse_z) ||
        (button_state != s.b_state)) {
      if (((button_state ^ s.b_state) != 0) || (delta_z != s.mouse_z)) {
        s.mouse_z = (Bit8s) delta_z;
        s.b_state = (Bit8u) button_state;
        if (s.mouse_event_count < BX_KBD_ELEMENTS) {
          create_mouse_packet(s.mouse_event_buf[s.mouse_event_count++], 6);
        }
      }
      s.has_events = 1;
    }
  }
}

int usb_hid_device_c::keyboard_poll(Bit8u *buf, int len, bool force)
{
  int l = USB_RET_NAK;

  if ((d.type == USB_HID_TYPE_KEYPAD) ||
      (d.type == USB_HID_TYPE_KEYBOARD)) {
    if (s.has_events || force) {
      memcpy(buf, s.kbd_packet, len);
      l = 8;
      s.has_events = 0;
      if (s.kbd_event_count > 0) {
        gen_scancode(s.kbd_event_buf[0]);
        s.kbd_event_count--;
        for (Bit8u i = 0; i < s.kbd_event_count; i++) {
          s.kbd_event_buf[i] = s.kbd_event_buf[i + 1];
        }
      }
      start_idle_timer();
    }
  }
  return l;
}

bool usb_hid_device_c::gen_scancode_static(void *dev, Bit32u key)
{
  if (dev != NULL) {
    return ((usb_hid_device_c *) dev)->gen_scancode(key);
  } else {
    return 0;
  }
}

bool usb_hid_device_c::gen_scancode(Bit32u key)
{
  bool modkey, released = (key & BX_KEY_RELEASED) != 0;
  Bit8u code;

  code = usbkbd_conv[key & ~BX_KEY_RELEASED].code;
  modkey = usbkbd_conv[key & ~BX_KEY_RELEASED].modkey;
  if (d.type == USB_HID_TYPE_KEYPAD) {
    if ((code < 0x53) || (code > 0x63)) {
      return 0;
    }
  } else if (code == 0) {
    return 1;
  }
  if (s.has_events) {
    if (s.kbd_event_count < BX_KBD_ELEMENTS) {
      s.kbd_event_buf[s.kbd_event_count++] = key;
    }
    return 1;
  }
  if (modkey) {
    if (released) {
      s.kbd_packet[0] &= ~code;
    } else {
      s.kbd_packet[0] |= code;
    }
  } else {
    if (released) {
      if (code == s.kbd_packet[2]) {
        s.kbd_packet[2] = 0;
        s.has_events = 1;
      }
    } else {
      s.kbd_packet[2] = code;
      s.has_events = 1;
    }
  }
  return 1;
}

Bit8u usb_hid_device_c::kbd_get_elements_static(void *dev)
{
  if (dev != NULL) {
    return ((usb_hid_device_c *) dev)->kbd_get_elements();
  } else {
    return 0;
  }
}

Bit8u usb_hid_device_c::kbd_get_elements()
{
  return s.kbd_event_count;
}

void usb_hid_device_c::start_idle_timer()
{
  if (s.idle > 0) {
    bx_pc_system.activate_timer(timer_index, HID_IDLE_TIME * s.idle, 0);
  } else {
    bx_pc_system.deactivate_timer(timer_index);
  }
}

void usb_hid_device_c::hid_timer_handler(void *this_ptr)
{
  if (this_ptr != NULL) {
    usb_hid_device_c *class_ptr = (usb_hid_device_c *) this_ptr;
    class_ptr->hid_idle_timer();
  }
}

void usb_hid_device_c::hid_idle_timer()
{
  s.has_events = 1;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
