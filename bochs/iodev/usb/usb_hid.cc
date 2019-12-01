/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// USB HID emulation support (mouse and tablet) ported from QEMU
// USB keypad emulation based on code by Benjamin D Lunt (fys [at] fysnet [dot] net)
//
// Copyright (c) 2005       Fabrice Bellard
// Copyright (c) 2007       OpenMoko, Inc.  (andrew@openedhand.com)
// Copyright (C) 2009-2017  The Bochs Project
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
// Copyright (C) 2004-2016  Benjamin D Lunt (fys [at] fysnet [dot] net)
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

// USB device plugin entry points

int CDECL libusb_hid_dev_plugin_init(plugin_t *plugin, plugintype_t type)
{
  return 0; // Success
}

void CDECL libusb_hid_dev_plugin_fini(void)
{
  // Nothing here yet
}

//
// Define the static class that registers the derived USB device class,
// and allocates one on request.
//
class bx_usb_hid_locator_c : public usbdev_locator_c {
public:
  bx_usb_hid_locator_c(void) : usbdev_locator_c("usb_hid") {}
protected:
  usb_device_c *allocate(usbdev_type devtype, const char *args) {
    return (new usb_hid_device_c(devtype));
  }
} bx_usb_hid_match;

/* HID interface requests */
#define GET_REPORT   0xa101
#define GET_IDLE     0xa102
#define GET_PROTOCOL 0xa103
#define SET_REPORT   0x2109
#define SET_IDLE     0x210a
#define SET_PROTOCOL 0x210b

#define KEYPAD_LEN   16
struct KEYPAD {
  Bit32u bxkey;
  Bit8u  keypad_packet[8];
};

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
  0x40,       /*  u8  bMaxPacketSize0; 64 Bytes */

  0x27, 0x06, /*  u16 idVendor; */
  0x01, 0x00, /*  u16 idProduct; */
  0x00, 0x00, /*  u16 bcdDevice */

  0x01,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x03,       /*  u8  iSerialNumber; */
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
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  50, 0,       /*  u16 len */

  /* one endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x03, 0x00, /*  u16 ep_wMaxPacketSize; */
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
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  50, 0,       /*  u16 len */

  /* one endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x03, 0x00, /*  u16 ep_wMaxPacketSize; */
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
  50, 0        /*  u16 len */
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
  0x02,       /*  u8  if_bInterfaceProtocol; [usb1.1 or single tt] */
  0x05,       /*  u8  if_iInterface; */

  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  74, 0,       /*  u16 len */

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
  0x02,       /*  u8  if_bInterfaceProtocol; [usb1.1 or single tt] */
  0x05,       /*  u8  if_iInterface; */

  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  74, 0,       /*  u16 len */

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
  74, 0,       /*  u16 len */
};

static const Bit8u bx_tablet_hid_report_descriptor[] = {
  0x05, 0x01, /* Usage Page Generic Desktop */
  0x09, 0x01, /* Usage Pointer */
  0xA1, 0x01, /* Collection Application */
  0x09, 0x01, /* Usage Pointer */
  0xA1, 0x00, /* Collection Physical */
  0x05, 0x09, /* Usage Page Button */
  0x19, 0x01, /* Usage Minimum Button 1 */
  0x29, 0x03, /* Usage Maximum Button 3 */
  0x15, 0x00, /* Logical Minimum 0 */
  0x25, 0x01, /* Logical Maximum 1 */
  0x95, 0x03, /* Report Count 3 */
  0x75, 0x01, /* Report Size 1 */
  0x81, 0x02, /* Input (Data, Var, Abs) */
  0x95, 0x01, /* Report Count 1 */
  0x75, 0x05, /* Report Size 5 */
  0x81, 0x01, /* Input (Cnst, Var, Abs) */
  0x05, 0x01, /* Usage Page Generic Desktop */
  0x09, 0x30, /* Usage X */
  0x09, 0x31, /* Usage Y */
  0x15, 0x00, /* Logical Minimum 0 */
  0x26, 0xFF, 0x7F, /* Logical Maximum 0x7fff */
  0x35, 0x00, /* Physical Minimum 0 */
  0x46, 0xFF, 0x7F, /* Physical Maximum 0x7fff */
  0x75, 0x10, /* Report Size 16 */
  0x95, 0x02, /* Report Count 2 */
  0x81, 0x02, /* Input (Data, Var, Abs) */
  0x05, 0x01, /* Usage Page Generic Desktop */
  0x09, 0x38, /* Usage Wheel */
  0x15, 0x81, /* Logical Minimum -127 */
  0x25, 0x7F, /* Logical Maximum 127 */
  0x35, 0x00, /* Physical Minimum 0 (same as logical) */
  0x45, 0x00, /* Physical Maximum 0 (same as logical) */
  0x75, 0x08, /* Report Size 8 */
  0x95, 0x01, /* Report Count 1 */
  0x81, 0x06, /* Input (Data, Var, Rel) */
  0xC0,       /* End Collection */
  0xC0,       /* End Collection */
};

static const Bit8u bx_keypad_dev_descriptor[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x10, 0x01, /*  u16 bcdUSB; v1.1 */

  0x00,       /*  u8  bDeviceClass; */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; [ low/full speeds only ] */
  0x08,       /*  u8  bMaxPacketSize0; 8 Bytes */

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
  0x40,       /*  u8  bMaxPacketSize0; 64 Bytes */

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
  0x3B, 0x00, /*  u16 wTotalLength; */
  0x02,       /*  u8  bNumInterfaces; (2) */
  0x01,       /*  u8  bConfigurationValue; */
  0x04,       /*  u8  iConfiguration; */
  0xa0,       /*  u8  bmAttributes;
			 Bit 7: must be set,
			     6: Self-powered,
			     5: Remote wakeup,
			     4..0: resvd */
  50,         /*  u8  MaxPower; */

  /* interface #1 of 2 */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x01,       /*  u8  if_bNumEndpoints; */
  0x03,       /*  u8  if_bInterfaceClass; */
  0x01,       /*  u8  if_bInterfaceSubClass; */
  0x01,       /*  u8  if_bInterfaceProtocol; [usb1.1 or single tt] */
  0x05,       /*  u8  if_iInterface; */

  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  65, 0,       /*  u16 len */

  /* one endpoint (status change endpoint) */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x08, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x0a,       /*  u8  ep_bInterval; (255ms -- usb 2.0 spec) */

  /* interface #2 of 2 */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x01,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x01,       /*  u8  if_bNumEndpoints; */
  0x03,       /*  u8  if_bInterfaceClass; */
  0x01,       /*  u8  if_bInterfaceSubClass; */
  0x02,       /*  u8  if_bInterfaceProtocol; [usb1.1 or single tt] */
  0x06,       /*  u8  if_iInterface; */

  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  50, 0,       /*  u16 len */

  /* one endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x82,       /*  u8  ep_bEndpointAddress; IN Endpoint 2 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x08, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x0a,       /*  u8  ep_bInterval; (0 - 255ms -- usb 2.0 spec) */
};

static const Bit8u bx_keypad_config_descriptor2[] = {
  /* one configuration */
  0x09,       /*  u8  bLength; */
  0x02,       /*  u8  bDescriptorType; Configuration */
  0x3B, 0x00, /*  u16 wTotalLength; */
  0x02,       /*  u8  bNumInterfaces; (2) */
  0x01,       /*  u8  bConfigurationValue; */
  0x04,       /*  u8  iConfiguration; */
  0xa0,       /*  u8  bmAttributes;
			 Bit 7: must be set,
			     6: Self-powered,
			     5: Remote wakeup,
			     4..0: resvd */
  50,         /*  u8  MaxPower; */

  /* interface #1 of 2 */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x01,       /*  u8  if_bNumEndpoints; */
  0x03,       /*  u8  if_bInterfaceClass; */
  0x01,       /*  u8  if_bInterfaceSubClass; */
  0x01,       /*  u8  if_bInterfaceProtocol; [usb1.1 or single tt] */
  0x05,       /*  u8  if_iInterface; */

  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  65, 0,       /*  u16 len */

  /* one endpoint (status change endpoint) */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x08, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x07,       /*  u8  ep_bInterval; (2 ^ (8-1) * 125 usecs = 8 ms) */

  /* interface #2 of 2 */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x01,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x01,       /*  u8  if_bNumEndpoints; */
  0x03,       /*  u8  if_bInterfaceClass; */
  0x01,       /*  u8  if_bInterfaceSubClass; */
  0x02,       /*  u8  if_bInterfaceProtocol; [usb1.1 or single tt] */
  0x06,       /*  u8  if_iInterface; */

  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x00, 0x01,  /*  u16 HID_class */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  50, 0,       /*  u16 len */

  /* one endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x82,       /*  u8  ep_bEndpointAddress; IN Endpoint 2 */
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
  50, 0,       /*  u16 len */
};

static const Bit8u bx_keypad_hid_report_descriptor1[] = {
  0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x05, 0x07,
  0x19, 0xE0, 0x29, 0xE7, 0x15, 0x00, 0x25, 0x01,
  0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x95, 0x01,
  0x75, 0x08, 0x81, 0x01, 0x95, 0x03, 0x75, 0x01,
  0x05, 0x08, 0x19, 0x01, 0x29, 0x03, 0x91, 0x02,
  0x95, 0x05, 0x75, 0x01, 0x91, 0x01, 0x95, 0x06,
  0x75, 0x08, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x05,
  0x07, 0x19, 0x00, 0x2A, 0xFF, 0x00, 0x81, 0x00,
  0xC0
};

static const Bit8u bx_keypad_hid_report_descriptor2[] = {
  0x05, 0x0C, 0x09, 0x01, 0xA1, 0x01, 0x85, 0x01,
  0x19, 0x00, 0x2A, 0x3C, 0x02, 0x15, 0x00, 0x26,
  0x3C, 0x02, 0x95, 0x01, 0x75, 0x10, 0x81, 0x00,
  0xC0, 0x05, 0x01, 0x09, 0x80, 0xA1, 0x01, 0x85,
  0x02, 0x19, 0x81, 0x29, 0x83, 0x25, 0x01, 0x75,
  0x01, 0x95, 0x03, 0x81, 0x02, 0x95, 0x05, 0x81,
  0x01, 0xC0
};

// this interface has a key conversion table of len = 16
struct KEYPAD keypad_lookup[KEYPAD_LEN] = {
  { BX_KEY_KP_HOME,      { 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // 7
  { BX_KEY_KP_LEFT,      { 0x00, 0x00, 0x5C, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // 4
  { BX_KEY_KP_END,       { 0x00, 0x00, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // 1
  { BX_KEY_KP_INSERT,    { 0x00, 0x00, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // 0
  { BX_KEY_KP_DIVIDE,    { 0x00, 0x00, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // /
  { BX_KEY_KP_UP,        { 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // 8
  { BX_KEY_KP_5,         { 0x00, 0x00, 0x5D, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // 5
  { BX_KEY_KP_DOWN,      { 0x00, 0x00, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // 2
  { BX_KEY_KP_MULTIPLY,  { 0x00, 0x00, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // *
  { BX_KEY_KP_PAGE_UP,   { 0x00, 0x00, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // 9
  { BX_KEY_KP_RIGHT,     { 0x00, 0x00, 0x5E, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // 6
  { BX_KEY_KP_PAGE_DOWN, { 0x00, 0x00, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // 3
  { BX_KEY_KP_SUBTRACT,  { 0x00, 0x00, 0x56, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // -
  { BX_KEY_KP_ADD,       { 0x00, 0x00, 0x57, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // +
  { BX_KEY_KP_DELETE,    { 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // .
  { BX_KEY_KP_ENTER,     { 0x00, 0x00, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00 } },  // Enter
};

usb_hid_device_c::usb_hid_device_c(usbdev_type type)
{
  d.type = type;
  d.minspeed = USB_SPEED_LOW;
  d.maxspeed = USB_SPEED_HIGH;
  d.speed = d.minspeed;
  if (d.type == USB_DEV_TYPE_MOUSE) {
    strcpy(d.devname, "USB Mouse");
    if (d.speed == USB_SPEED_HIGH) {
      d.dev_descriptor = bx_mouse_dev_descriptor2;
      d.config_descriptor = bx_mouse_config_descriptor2;
      d.device_desc_size = sizeof(bx_mouse_dev_descriptor2);
      d.config_desc_size = sizeof(bx_mouse_config_descriptor2);
    } else {
      d.dev_descriptor = bx_mouse_dev_descriptor;
      d.config_descriptor = bx_mouse_config_descriptor;
      d.device_desc_size = sizeof(bx_mouse_dev_descriptor);
      d.config_desc_size = sizeof(bx_mouse_config_descriptor);
    }
    DEV_register_removable_mouse((void*)this, mouse_enq_static, mouse_enabled_changed);
  } else if (d.type == USB_DEV_TYPE_TABLET) {
    strcpy(d.devname, "USB Tablet");
    if (d.speed == USB_SPEED_HIGH) {
      d.dev_descriptor = bx_mouse_dev_descriptor2;
      d.config_descriptor = bx_tablet_config_descriptor2;
      d.device_desc_size = sizeof(bx_mouse_dev_descriptor2);
      d.config_desc_size = sizeof(bx_tablet_config_descriptor2);
    } else {
      d.dev_descriptor = bx_mouse_dev_descriptor;
      d.config_descriptor = bx_tablet_config_descriptor;
      d.device_desc_size = sizeof(bx_mouse_dev_descriptor);
      d.config_desc_size = sizeof(bx_tablet_config_descriptor);
    }
    DEV_register_removable_mouse((void*)this, mouse_enq_static, mouse_enabled_changed);
    bx_gui->set_mouse_mode_absxy(1);
  } else if (d.type == USB_DEV_TYPE_KEYPAD) {
    strcpy(d.devname, "USB/PS2 Keypad");
    if (d.speed == USB_SPEED_HIGH) {
      d.dev_descriptor = bx_keypad_dev_descriptor2;
      d.config_descriptor = bx_keypad_config_descriptor2;
      d.device_desc_size = sizeof(bx_keypad_dev_descriptor2);
      d.config_desc_size = sizeof(bx_keypad_config_descriptor2);
    } else {
      d.dev_descriptor = bx_keypad_dev_descriptor;
      d.config_descriptor = bx_keypad_config_descriptor;
      d.device_desc_size = sizeof(bx_keypad_dev_descriptor);
      d.config_desc_size = sizeof(bx_keypad_config_descriptor);
    }
    DEV_register_removable_keyboard((void*)this, gen_scancode_static);
//    DEV_register_removable_mouse((void*)this, mouse_enq_static, mouse_enabled_changed);
  }
  d.vendor_desc = "BOCHS";
  d.product_desc = d.devname;
  d.serial_num = "1";
  d.connected = 1;
  memset((void*)&s, 0, sizeof(s));
  if (d.type == USB_DEV_TYPE_KEYPAD) {
    s.saved_key = BX_KEY_UNHANDLED;
  }

  put("usb_hid", "USBHID");
}

usb_hid_device_c::~usb_hid_device_c(void)
{
  d.sr->clear();
  if ((d.type == USB_DEV_TYPE_MOUSE) ||
      (d.type == USB_DEV_TYPE_TABLET)) {
    bx_gui->set_mouse_mode_absxy(0);
    DEV_unregister_removable_mouse((void*)this);
  } else if (d.type == USB_DEV_TYPE_KEYPAD) {
    DEV_unregister_removable_keyboard((void*)this);
//    DEV_unregister_removable_mouse((void*)this);
  }
}

void usb_hid_device_c::register_state_specific(bx_list_c *parent)
{
  bx_list_c *list = new bx_list_c(parent, "s", "USB HID Device State");
  BXRS_DEC_PARAM_FIELD(list, mouse_delayed_dx, s.mouse_delayed_dx);
  BXRS_DEC_PARAM_FIELD(list, mouse_delayed_dy, s.mouse_delayed_dy);
  BXRS_DEC_PARAM_FIELD(list, mouse_delayed_dz, s.mouse_delayed_dz);
  BXRS_DEC_PARAM_FIELD(list, mouse_x, s.mouse_x);
  BXRS_DEC_PARAM_FIELD(list, mouse_y, s.mouse_y);
  BXRS_DEC_PARAM_FIELD(list, mouse_z, s.mouse_z);
  BXRS_HEX_PARAM_FIELD(list, b_state, s.b_state);
  BXRS_HEX_PARAM_FIELD(list, idle, s.idle);
  BXRS_PARAM_BOOL(list, has_events, s.has_events);
  if (d.type == USB_DEV_TYPE_KEYPAD) {
    BXRS_DEC_PARAM_FIELD(list, saved_key, s.saved_key);
    new bx_shadow_data_c(list, "key_pad_packet", s.key_pad_packet, 8, 1);
  }
}

void usb_hid_device_c::handle_reset()
{
  memset((void*)&s, 0, sizeof(s));
  BX_DEBUG(("Reset"));
}

int usb_hid_device_c::handle_control(int request, int value, int index, int length, Bit8u *data)
{
  int ret;

  ret = handle_control_common(request, value, index, length, data);
  if (ret >= 0) {
    return ret;
  }

  ret = 0;
  switch(request) {
    case DeviceOutRequest | USB_REQ_CLEAR_FEATURE:
      goto fail;
      break;
    case DeviceOutRequest | USB_REQ_SET_FEATURE:
      goto fail;
      break;
    case DeviceRequest | USB_REQ_GET_DESCRIPTOR:
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
      switch(value >> 8) {
        case 0x21:
          if (d.type == USB_DEV_TYPE_MOUSE) {
            memcpy(data, bx_mouse_hid_descriptor,
                   sizeof(bx_mouse_hid_descriptor));
            ret = sizeof(bx_mouse_hid_descriptor);
          } else if (d.type == USB_DEV_TYPE_TABLET) {
            memcpy(data, bx_tablet_hid_descriptor,
                   sizeof(bx_tablet_hid_descriptor));
            ret = sizeof(bx_tablet_hid_descriptor);
          } else if (d.type == USB_DEV_TYPE_KEYPAD) {
            memcpy(data, bx_keypad_hid_descriptor,
                   sizeof(bx_keypad_hid_descriptor));
            ret = sizeof(bx_keypad_hid_descriptor);
          } else {
            goto fail;
          }
          break;
        case 0x22:
          if (d.type == USB_DEV_TYPE_MOUSE) {
            memcpy(data, bx_mouse_hid_report_descriptor,
                   sizeof(bx_mouse_hid_report_descriptor));
            ret = sizeof(bx_mouse_hid_report_descriptor);
          } else if (d.type == USB_DEV_TYPE_TABLET) {
            memcpy(data, bx_tablet_hid_report_descriptor,
                   sizeof(bx_tablet_hid_report_descriptor));
            ret = sizeof(bx_tablet_hid_report_descriptor);
          } else if (d.type == USB_DEV_TYPE_KEYPAD) {
            if (index == 0) {
              memcpy(data, bx_keypad_hid_report_descriptor1,
                     sizeof(bx_keypad_hid_report_descriptor1));
              ret = sizeof(bx_keypad_hid_report_descriptor1);
            } else {
              memcpy(data, bx_keypad_hid_report_descriptor2,
                     sizeof(bx_keypad_hid_report_descriptor2));
              ret = sizeof(bx_keypad_hid_report_descriptor2);
            }
          } else {
            goto fail;
          }
          break;
        case 0x23:
          BX_ERROR(("USB HID handle_control: Host requested the HID Physical Descriptor"));
          goto fail;
        default:
          BX_ERROR(("USB HID handle_control: unknown HID descriptor 0x%02x", value >> 8));
          goto fail;
        }
        break;
    case EndpointOutRequest | USB_REQ_CLEAR_FEATURE:
      if ((value == 0) && (index != 0x81)) { /* clear EP halt */
        goto fail;
      }
      break;
    case GET_REPORT:
      if ((d.type == USB_DEV_TYPE_MOUSE) ||
          (d.type == USB_DEV_TYPE_TABLET)) {
        ret = mouse_poll(data, length, 1);
      } else if (d.type == USB_DEV_TYPE_KEYPAD) {
        if (index == 0) {
          ret = keypad_poll(data, length, 1);
        } else {
          ret = mouse_poll(data, length, 1);
        }
      } else {
        goto fail;
      }
      break;
    case SET_REPORT:
      if ((d.type == USB_DEV_TYPE_KEYPAD) && (value = 0x200)) {
        BX_INFO(("keypad NUMLOCK %s", (data[0] & 0x01) ? "on" : "off"));
        ret = 0;
      } else {
        goto fail;
      }
      break;
    case GET_IDLE:
      data[0] = s.idle;
      ret = 1;
      break;
    case SET_IDLE:
      s.idle = (value >> 8);
      ret = 0;
      break;
    case SET_PROTOCOL:
      ret = 0;
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

  switch(p->pid) {
    case USB_TOKEN_IN:
      if (p->devep == 1) {
        if ((d.type == USB_DEV_TYPE_MOUSE) ||
            (d.type == USB_DEV_TYPE_TABLET)) {
          ret = mouse_poll(p->data, p->len, 0);
        } else if (d.type == USB_DEV_TYPE_KEYPAD) {
          ret = keypad_poll(p->data, p->len, 0);
        } else {
          goto fail;
        }
      } else if (p->devep == 2) {
        if (d.type == USB_DEV_TYPE_KEYPAD) {
          ret = mouse_poll(p->data, p->len, 0);
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

int usb_hid_device_c::mouse_poll(Bit8u *buf, int len, bx_bool force)
{
  int l = USB_RET_NAK;

  if ((d.type == USB_DEV_TYPE_MOUSE) ||
      (d.type == USB_DEV_TYPE_KEYPAD)) {
    if (!s.has_events) {
      // if there's no new movement, handle delayed one
      mouse_enq(0, 0, s.mouse_z, s.b_state, 0);
    }
    if (s.has_events || (s.idle != 0) || force) {
      buf[0] = (Bit8u) s.b_state;
      buf[1] = (Bit8s) s.mouse_x;
      buf[2] = (Bit8s) s.mouse_y;
      s.mouse_x = 0;
      s.mouse_y = 0;
      l = 3;
      if (len >= 4) {
        buf[3] = (Bit8s) s.mouse_z; // if wheel mouse
        s.mouse_z = 0;
        l = 4;
      }
      s.has_events = 0;
    }
  } else if (d.type == USB_DEV_TYPE_TABLET) {
    if (s.has_events || (s.idle != 0) || force) {
      buf[0] = (Bit8u) s.b_state;
      buf[1] = (Bit8u)(s.mouse_x & 0xff);
      buf[2] = (Bit8u)(s.mouse_x >> 8);
      buf[3] = (Bit8u)(s.mouse_y & 0xff);
      buf[4] = (Bit8u)(s.mouse_y >> 8);
      buf[5] = (Bit8s) s.mouse_z;
      s.mouse_z = 0;
      l = 6;
      s.has_events = 0;
    }
  }
  return l;
}

void usb_hid_device_c::mouse_enabled_changed(void *dev, bx_bool enabled)
{
  if (enabled) ((usb_hid_device_c*)dev)->handle_reset();
}

void usb_hid_device_c::mouse_enq_static(void *dev, int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy)
{
  ((usb_hid_device_c*)dev)->mouse_enq(delta_x, delta_y, delta_z, button_state, absxy);
}

void usb_hid_device_c::mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy)
{
  Bit16s prev_x, prev_y;

  if (d.type == USB_DEV_TYPE_MOUSE) {
    // scale down the motion
    if ((delta_x < -1) || (delta_x > 1))
      delta_x /= 2;
    if ((delta_y < -1) || (delta_y > 1))
      delta_y /= 2;

    if (delta_x>127) delta_x=127;
    if (delta_y>127) delta_y=127;
    if (delta_x<-128) delta_x=-128;
    if (delta_y<-128) delta_y=-128;

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
    if ((s.mouse_x != 0) || (s.mouse_y != 0) || (button_state != s.b_state)) {
      s.has_events = 1;
    }
  } else if (d.type == USB_DEV_TYPE_TABLET) {
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
    if ((s.mouse_x != prev_x) || (s.mouse_y != prev_y) || (button_state != s.b_state)) {
      s.has_events = 1;
    }
  }
  s.mouse_z = (Bit8s) delta_z;
  s.b_state = (Bit8u) button_state;
}

int usb_hid_device_c::keypad_poll(Bit8u *buf, int len, bx_bool force)
{
  int l = USB_RET_NAK;

  if (d.type == USB_DEV_TYPE_KEYPAD) {
    if (s.has_events || (s.idle != 0) || force) {
      memcpy(buf, s.key_pad_packet, len);
      l = 8;
      s.has_events = 0;
    }
  }
  return l;
}

bx_bool usb_hid_device_c::gen_scancode_static(void *dev, Bit32u key)
{
  return ((usb_hid_device_c*)dev)->gen_scancode(key);
}

bx_bool usb_hid_device_c::gen_scancode(Bit32u key)
{
  // if it is the break code of the saved key, then clear our packet key.
  if (key & BX_KEY_RELEASED) {
    key &= ~BX_KEY_RELEASED;
    if (key == s.saved_key) {
      s.saved_key = BX_KEY_UNHANDLED;
      memset(s.key_pad_packet, 0, 8);
      s.has_events = 1;
      BX_DEBUG(("Routing Bochs key release (%d) to USB keypad", key));
      return 1; // tell the keyboard handler that we used it, and to return with out processing key
    }
    return 0;
  }

  bx_bool fnd = 0;
  for (int m = 0; m < KEYPAD_LEN; m++) {
    if (key == keypad_lookup[m].bxkey) {
      memcpy(s.key_pad_packet, keypad_lookup[m].keypad_packet, 8);
      fnd = 1;
      break;
    }
  }

  if (fnd) {
    s.saved_key = key;
    s.has_events = 1;
    BX_DEBUG(("Routing Bochs key press (%d) to USB keypad", key));
  }

  // tell the keyboard handler whether we used it or not.  (0 = no, 1 = yes and keyboard.cc ignores keystoke)
  return fnd;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
