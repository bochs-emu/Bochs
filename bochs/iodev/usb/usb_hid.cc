/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// USB HID emulation support (mouse and tablet) originally ported from QEMU
// Current USB emulation based on code by Benjamin D Lunt (fys [at] fysnet [dot] net)
//
// Copyright (c) 2005       Fabrice Bellard
// Copyright (c) 2007       OpenMoko, Inc.  (andrew@openedhand.com)
// Copyright (C) 2004-2023  Benjamin D Lunt (fys [at] fysnet [dot] net)
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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
#include "usb_common.h"
#include "usb_hid.h"
#include <cstddef>

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

// If you change any of the Max Packet Size, or other fields within these
//  descriptors, you must also change the d.endpoint_info[] array
//  to match your changes.

////////////////////////////////////////////////
// Mouse
static const Bit8u bx_mouse_dev_descriptor[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x01, 0x01, /*  u16 bcdUSB; v1.1 */

  0x00,       /*  u8  bDeviceClass; */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; */
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
  0x00,       /*  u8  bDeviceProtocol; */
  0x40,       /*  u8  bMaxPacketSize; 64 Bytes */

  0x27, 0x06, /*  u16 idVendor; */
  0x01, 0x00, /*  u16 idProduct; */
  0x00, 0x00, /*  u16 bcdDevice */

  0x01,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x03,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

// The following hid report descriptors are for the mouse/tablet
//  depending on the model given.

// hid_mouse_2x2x8
// default 2-button, 3-byte, X and Y coords, 8-bit (+ report id)
// 00000001 (report id)
// 000000BB
// XXXXXXXX
// YYYYYYYY
static const Bit8u bx_mouse_hid_report_descriptor_228[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x02,        // Usage (Mouse)
  0xA1, 0x01,        // Collection (Application)
  0x09, 0x01,        //   Usage (Pointer)
  0x85, HID_REPORT_ID, //   Report ID (HID_REPORT_ID)
  0x89, HID_CLASS_STR4, // Starting String Index (4)
  0x99, HID_CLASS_STR5, // Ending String Index (5)
  0xA1, 0x00,        //   Collection (Physical)
  0x05, 0x09,        //     Usage Page (Button)
  0x19, 0x01,        //     Usage Minimum (0x01)
  0x29, 0x02,        //     Usage Maximum (0x02)
  0x15, 0x00,        //     Logical Minimum (0)
  0x25, 0x01,        //     Logical Maximum (1)
  0x95, 0x02,        //     Report Count (2)
  0x75, 0x01,        //     Report Size (1)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x01,        //     Report Count (1)
  0x75, 0x06,        //     Report Size (6)
  0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
  0x09, 0x30,        //     Usage (X)
  0x09, 0x31,        //     Usage (Y)
  0x95, 0x02,        //     Report Count (2)
  0x15, 0x80,        //     Logical Minimum (-128)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x75, 0x08,        //     Report Size (8)
  0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              //   End Collection
  0xC0               // End Collection
};

// hid_mouse_3x3x8
// 3-button, 4-byte X, Y, and Wheel coords, 8-bit
// 00000BBB
// XXXXXXXX
// YYYYYYYY
// WWWWWWWW
static const Bit8u bx_mouse_hid_report_descriptor_338[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x02,        // Usage (Mouse)
  0xA1, 0x01,        // Collection (Application)
  0x09, 0x01,        //   Usage (Pointer)
  0x89, HID_CLASS_STR4, // Starting String Index (4)
  0x99, HID_CLASS_STR5, // Ending String Index (5)
  0xA1, 0x00,        //   Collection (Physical)
  0x05, 0x09,        //     Usage Page (Button)
  0x15, 0x00,        //     Logical Minimum (0)
  0x25, 0x01,        //     Logical Maximum (1)
  0x19, 0x01,        //     Usage Minimum (0x01)
  0x29, 0x03,        //     Usage Maximum (0x03)
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
  0x95, 0x03,        //     Report Count (3)
  0x15, 0x80,        //     Logical Minimum (-128)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x75, 0x08,        //     Report Size (8)
  0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              //   End Collection
  0xC0,              // End Collection
};

// this model is deliberately irregular by design, so that a Guest can test its HID Report Descriptor Parser.
//  - the button fields are not consecutive and are at arbitrary positions in the report.
//  - the coords fields are not byte aligned or consecutively spaced.
//  - the coords fields are of an irregular size, each a different size.
//  - there are padding fields between entries that *do not* align the next field on a byte boundary.
//  - this also uses he push/pop mechanism to test the function of your parser.
// (Again, this is deliberate. A correctly written parser will extract the neccessary fields no matter the irregularity)
// hid_mouse_3x3xDebug
// 3-button, 5-byte X, Y, and Wheel coords (debug model)
// YYYYYYY0 - 10 bit Y displacement
// WWWW0YYY - 8 bit W displacement
// 0B00WWWW - bit 6 is Button #2 (right button)
// XXXXX0B0 - 9 bit X displacement, bit 1 is Button #1 (left button)
// 0B00XXXX - bit 6 is Button #3 (middle button)
static const Bit8u bx_mouse_hid_report_descriptor_33debug[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x02,        // Usage (Mouse)
  0xA1, 0x01,        // Collection (Application)
  0x09, 0x01,        //   Usage (Pointer)
  0xA1, 0x00,        //   Collection (Physical)
  0x95, 0x01,        //     Report Count (1)
  0x75, 0x01,        //     Report Size (1)
  0x15, 0x00,        //     Logical Minimum (0)
  0x25, 0x00,        //     Logical Maximum (0)
  0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x09, 0x31,        //     Usage (Y)
  0x75, 0x0A,        //     Report Size (10)
  0x16, 0x00, 0xFE,  //     Logical Minimum (-512)
  0x26, 0xFF, 0x01,  //     Logical Maximum (511)
  0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0x75, 0x01,        //     Report Size (1)
  0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x09, 0x38,        //     Usage (Wheel)
  0x75, 0x08,        //     Report Size (8)
  0x15, 0x80,        //     Logical Minimum (-128)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0xA4,              //     Push
  0x75, 0x01,        //       Report Size (1)
  0x95, 0x02,        //       Report Count (2)
  0xA4,              //       Push
  0x81, 0x01,        //         Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x09,        //         Usage Page (Button)
  0x15, 0x00,        //         Logical Minimum (0)
  0x25, 0x01,        //         Logical Maximum (1)
  0x09, 0x02,        //         Usage (0x02)
  0x95, 0x01,        //         Report Count (1)
  0x81, 0x02,        //         Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xB4,              //       Pop
  0x81, 0x01,        //       Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x09,        //       Usage Page (Button)
  0x09, 0x01,        //       Usage (0x01)
  0x95, 0x01,        //       Report Count (1)
  0x15, 0x00,        //       Logical Minimum (0)
  0x25, 0x01,        //       Logical Maximum (1)
  0x81, 0x02,        //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x81, 0x01,        //       Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xB4,              //     Pop
  0x09, 0x30,        //     Usage (X)
  0x75, 0x09,        //     Report Size (9)
  0x16, 0x00, 0xFF,  //     Logical Minimum (-256)
  0x26, 0xFF, 0x00,  //     Logical Maximum (255)
  0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x02,        //     Report Count (2)
  0x75, 0x01,        //     Report Size (1)
  0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x09,        //     Usage Page (Button)
  0x09, 0x03,        //     Usage (0x03)
  0x95, 0x01,        //     Report Count (1)
  0x15, 0x00,        //     Logical Minimum (0)
  0x25, 0x01,        //     Logical Maximum (1)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              //   End Collection
  0xC0,              // End Collection
};

// hid_mouse_3x3x12
// 3-button, 5-byte X, Y, and Wheel coords, 12-bit
// 00000BBB
// XXXXXXXX (lsb)
// YYYYXXXX (lsb of y, msb of x)
// YYYYYYYY (msb)
// WWWWWWWW
static const Bit8u bx_mouse_hid_report_descriptor_3312[] = {
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
  0x95, 0x02,        //     Report Count (2)
  0x16, 0x00, 0xF8,  //     Logical Minimum (-2048)
  0x26, 0xFF, 0x07,  //     Logical Maximum (2047)
  0x75, 0x0C,        //     Report Size (12)
  0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0x09, 0x38,        //     Usage (Wheel)
  0x95, 0x01,        //     Report Count (1)
  0x15, 0x80,        //     Logical Minimum (-128)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x75, 0x08,        //     Report Size (8)
  0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              //   End Collection
  0xC0,              // End Collection
};

// hid_mouse_3x3x16
// 3-button, 6-byte X, Y, and Wheel coords, 16-bit
// 00000BBB
// XXXXXXXX  (lsb)
// XXXXXXXX  (msb)
// YYYYYYYY  (lsb)
// YYYYYYYY  (msb)
// WWWWWWWW
static const Bit8u bx_mouse_hid_report_descriptor_3316[] = {
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
  0x95, 0x02,        //     Report Count (2)
  0x16, 0x00, 0x80,  //     Logical Minimum (-32768)
  0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
  0x75, 0x10,        //     Report Size (16)
  0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0x09, 0x38,        //     Usage (Wheel)
  0x95, 0x01,        //     Report Count (1)
  0x15, 0x80,        //     Logical Minimum (-128)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x75, 0x08,        //     Report Size (8)
  0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              //   End Collection
  0xC0,              // End Collection
};

// This Report Descriptor uses Designator values to point to an entry in the
//  HID Physical Descriptor. WinXP doesn't like the Designator entries, so
//  don't use this model when using a WinXP guest.
// hid_mouse_3x3x8phy
// 3-button, 3-byte X, Y, and Wheel coords, 8-bit
// 00000BBB
// XXXXXXXX
// YYYYYYYY
// WWWWWWWW
static const Bit8u bx_mouse_hid_report_descriptor_338phy[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x02,        // Usage (Mouse)
  0xA1, 0x01,        // Collection (Application)
  0x09, 0x01,        //   Usage (Pointer)
  0x89, HID_CLASS_STR4, // Starting String Index (4)
  0x99, HID_CLASS_STR5, // Ending String Index (5)
  0xA1, 0x00,        //   Collection (Physical)
  0x05, 0x09,        //     Usage Page (Button)
  0x15, 0x00,        //     Logical Minimum (0)
  0x25, 0x01,        //     Logical Maximum (1)
  0x19, 0x01,        //     Usage Minimum (0x01)
  0x29, 0x03,        //     Usage Maximum (0x03)
  0x95, 0x03,        //     Report Count (3)
  0x75, 0x01,        //     Report Size (1)
  0x49, 0x01,        //     Designator Minimum (1)
  0x59, 0x03,        //     Designator Maximum (3)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x01,        //     Report Count (1)
  0x75, 0x05,        //     Report Size (5)
  0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
  0x09, 0x30,        //     Usage (X)
  0x09, 0x31,        //     Usage (Y)
  0x09, 0x38,        //     Usage (Wheel)
  0x95, 0x03,        //     Report Count (3)
  0x15, 0x81,        //     Logical Minimum (-127)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x75, 0x08,        //     Report Size (8)
  0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              //   End Collection
  0xC0,              // End Collection
};

// standard low-, full-speed configuration (w/o physical descriptor)
// (this define must be the zero based byte offset of the HID length field below)
#define BX_Mouse_Config_Descriptor0_pos 25
static Bit8u bx_mouse_config_descriptor0[] = {
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
  0x01, 0x01,  /*  u16 HID_class (0x0101) */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  0x00, 0x00,  /*  u16 len (updated on the fly) */
  
  /* one endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x08, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x0A,       /*  u8  ep_bInterval; (10 - 255ms -- usb 2.0 spec, 5.7.4) */
              /*           however: (11 - 255ms -- usb 2.0 compliance 1.2.84, pg 11 */
};

#define HID_PHYS_DESC_SET_LEN   7
static const Bit8u bx_mouse_phys_descriptor[] = {
  /* HID Physical descriptor */
  /* Descriptor set 0 */
  0x02,                  /*  u8  Number of physical Descriptor sets to follow */
  HID_PHYS_DESC_SET_LEN, /*  u8  Length of each descriptor set */
  0x00, 0x00, 0x00, 0x00, 0x00,  /* pad to HID_PHYS_DESC_SET_LEN bytes */
  
  /* Descriptor set 1 */
  (HID_PHYS_BIAS_RIGHT_HAND << 5) | 0, /*  u8  bPhysicalInfo; (right hand, 0 = most prefered) */
  IndexFinger,  (Right << 5) | 0,  /*  u8*2 DescritorData[0] (index 1) (Index Finger,  right, 0 = easy */
  MiddleFinger, (Right << 5) | 0,  /*  u8*2 DescritorData[1] (index 2) (Middle Finger, right, 0 = easy */
  IndexFinger,  (Right << 5) | 0,  /*  u8*2 DescritorData[2] (index 3) (Index Finger,  right, 0 = easy */
  
  /* Descriptor set 2 */
  (HID_PHYS_BIAS_LEFT_HAND << 5) | 1, /*  u8  bPhysicalInfo; (left hand, 1 = next prefered) */
  MiddleFinger, (Left << 5) | 0,  /*  u8*2 DescritorData[0] (index 1) (Middle Finger, left, 0 = easy */
  IndexFinger,  (Left << 5) | 0,  /*  u8*2 DescritorData[1] (index 2) (Index Finger,  left, 0 = easy */
  IndexFinger,  (Left << 5) | 0,  /*  u8*2 DescritorData[2] (index 3) (Index Finger,  left, 0 = easy */
};

// standard low-, full-speed configuration (w/ physical descriptor)
// (this define must be the zero based byte offset of the HID length field below)
#define BX_Mouse_Config_Descriptor1_pos 25
static Bit8u bx_mouse_config_descriptor1[] = {
  /* one configuration */
  0x09,       /*  u8  bLength; */
  0x02,       /*  u8  bDescriptorType; Configuration */
  0x25, 0x00, /*  u16 wTotalLength; */
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
  0x0C,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x01, 0x01,  /*  u16 HID_class (0x0101) */
  0x00,        /*  u8 country_code */
  0x02,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  0x00, 0x00,  /*  u16 len (updated on the fly) */
  0x23,        /*  u8 type; Physical */
  sizeof(bx_mouse_phys_descriptor),
        0x00,  /*  u16 len */
  
  /* one endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x08, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x0a,       /*  u8  ep_bInterval; (0 - 255ms -- usb 2.0 spec) */
};

// standard high-speed configuration (w/o physical descriptor)
// (this define must be the zero based byte offset of the HID length field below)
#define BX_Mouse_Config_Descriptor2_pos 25
static Bit8u bx_mouse_config_descriptor2[] = {
  /* one configuration */
  0x09,       /*  u8  bLength; */
  0x02,       /*  u8  bDescriptorType; Configuration */
  0x25, 0x00, /*  u16 wTotalLength; */
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
  0x01, 0x01,  /*  u16 HID_class (0x0101) */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  0x00, 0x00,  /*  u16 len (updated on the fly) */

  /* one endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x08, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x07,       /*  u8  ep_bInterval; (2 ^ (8-1) * 125 usecs = 8 ms) */
};

// standard hid descriptor (w/o physical descriptor)
// (this define must be the zero based byte offset of the HID length field below)
#define BX_Mouse_Hid_Descriptor0 7
static Bit8u bx_mouse_hid_descriptor0[] = {
  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x01, 0x01,  /*  u16 HID_class (0x0101) */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  0x00, 0x00,  /*  u16 len (updated on the fly) */
};

// standard hid descriptor (w/ physical descriptor)
// (this define must be the zero based byte offset of the HID length field below)
#define BX_Mouse_Hid_Descriptor1 7
static Bit8u bx_mouse_hid_descriptor1[] = {
  /* HID descriptor */
  0x0C,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x01, 0x01,  /*  u16 HID_class (0x0101) */
  0x00,        /*  u8 country_code */
  0x02,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  0x00, 0x00,  /*  u16 len (updated on the fly) */
  0x23,        /*  u8 type; Physical */
  sizeof(bx_mouse_phys_descriptor),
        0x00,  /*  u16 len */
};

////////////////////////////////////////////////
// tablet
static const Bit8u bx_tablet_hid_report_descriptor[] = {
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
  0x01, 0x01,  /*  u16 HID_class (0x0101) */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_tablet_hid_report_descriptor),
        0x00,  /*  u16 len */

  /* one endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x06, 0x00, /*  u16 ep_wMaxPacketSize; */
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
  0x01, 0x01,  /*  u16 HID_class (0x0101) */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_tablet_hid_report_descriptor),
        0x00,  /*  u16 len */

  /* one endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x06, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x04,       /*  u8  ep_bInterval; (2 ^ (4-1) * 125 usecs = 1 ms) */
};

static const Bit8u bx_tablet_hid_descriptor[] = {
  /* HID descriptor */
  0x09,        /*  u8  bLength; */
  0x21,        /*  u8 bDescriptorType; */
  0x01, 0x01,  /*  u16 HID_class (0x0101) */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_tablet_hid_report_descriptor),
        0x00,  /*  u16 len */
};

////////////////////////////////////////////////
// keyboard/keypad
static const Bit8u bx_keypad_hid_report_descriptor[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x06,        // Usage (Keyboard)
  0xA1, 0x01,        // Collection (Application)
  0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
  0x19, 0xE0,        //   Usage Minimum (Keyboard Left Control)
  0x29, 0xE7,        //   Usage Maximum (Keyboard Right GUI)
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
  0x19, 0x00,        //   Usage Minimum (0)
  0x29, 0xE7,        //   Usage Maximum (Keyboard Right GUI)
  0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              // End Collection
};

static const Bit8u bx_keypad_dev_descriptor[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x01, 0x01, /*  u16 bcdUSB; v1.1 */

  0x00,       /*  u8  bDeviceClass; */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; */
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
  0x00,       /*  u8  bDeviceProtocol; */
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
  0x01, 0x01,  /*  u16 HID_class (0x0101) */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_keypad_hid_report_descriptor),
        0x00,  /*  u16 len */

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
  0x01, 0x01,  /*  u16 HID_class (0x0101) */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_keypad_hid_report_descriptor),
        0x00,  /*  u16 len */

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
  0x01, 0x01,  /*  u16 HID_class (0x0101) */
  0x00,        /*  u8 country_code */
  0x01,        /*  u8 num_descriptors */
  0x22,        /*  u8 type; Report */
  sizeof(bx_keypad_hid_report_descriptor),
        0x00,  /*  u16 len */
};

// just for fun: if it is a modifying key, the high-nibble will be 0xE,
//  while the low-nibble is the bit position in the packet.Modifier byte :-)
struct USBKBD usbkbd_conv[BX_KEY_NBKEYS] = {
  0xE0, BX_MODS_LEFT_CTRL,   // BX_KEY_CTRL_L  (Bit 0 in packet.Modifier byte)
  0xE1, BX_MODS_LEFT_SHIFT,  // BX_KEY_SHIFT_L (Bit 1 in packet.Modifier byte)
  
  0x3A, BX_MODS_NONE, // F1 ... F12
  0x3B, BX_MODS_NONE,
  0x3C, BX_MODS_NONE,
  0x3D, BX_MODS_NONE,
  0x3E, BX_MODS_NONE,
  0x3F, BX_MODS_NONE,
  0x40, BX_MODS_NONE,
  0x41, BX_MODS_NONE,
  0x42, BX_MODS_NONE,
  0x43, BX_MODS_NONE,
  0x44, BX_MODS_NONE,
  0x45, BX_MODS_NONE,
  
  0xE4, BX_MODS_RIGHT_CTRL,  // BX_KEY_CTRL_R  (Bit 4 in packet.Modifier byte)
  0xE5, BX_MODS_RIGHT_SHIFT, // BX_KEY_SHIFT_R (Bit 5 in packet.Modifier byte)
  0x39, BX_MODS_NONE,        // BX_KEY_CAPS_LOCK
  0x53, BX_MODS_NONE,        // BX_KEY_NUM_LOCK
  0xE2, BX_MODS_LEFT_ALT,    // BX_KEY_ALT_L (Bit 2 in packet.Modifier byte)
  0xE6, BX_MODS_RIGHT_ALT,   // BX_KEY_ALT_R (Bit 6 in packet.Modifier byte)
  
  0x04, BX_MODS_NONE, // A ... Z
  0x05, BX_MODS_NONE,
  0x06, BX_MODS_NONE,
  0x07, BX_MODS_NONE,
  0x08, BX_MODS_NONE,
  0x09, BX_MODS_NONE,
  0x0A, BX_MODS_NONE,
  0x0B, BX_MODS_NONE,
  0x0C, BX_MODS_NONE,
  0x0D, BX_MODS_NONE,
  0x0E, BX_MODS_NONE,
  0x0F, BX_MODS_NONE,
  0x10, BX_MODS_NONE,
  0x11, BX_MODS_NONE,
  0x12, BX_MODS_NONE,
  0x13, BX_MODS_NONE,
  0x14, BX_MODS_NONE,
  0x15, BX_MODS_NONE,
  0x16, BX_MODS_NONE,
  0x17, BX_MODS_NONE,
  0x18, BX_MODS_NONE,
  0x19, BX_MODS_NONE,
  0x1A, BX_MODS_NONE,
  0x1B, BX_MODS_NONE,
  0x1C, BX_MODS_NONE,
  0x1D, BX_MODS_NONE,
  
  0x27, BX_MODS_NONE, // 0 ... 9
  0x1E, BX_MODS_NONE,
  0x1F, BX_MODS_NONE,
  0x20, BX_MODS_NONE,
  0x21, BX_MODS_NONE,
  0x22, BX_MODS_NONE,
  0x23, BX_MODS_NONE,
  0x24, BX_MODS_NONE,
  0x25, BX_MODS_NONE,
  0x26, BX_MODS_NONE,
  
  0x29, BX_MODS_NONE, // BX_KEY_ESC
  0x2C, BX_MODS_NONE, // BX_KEY_SPACE
  0x34, BX_MODS_NONE, // BX_KEY_SINGLE_QUOTE
  0x36, BX_MODS_NONE, // BX_KEY_COMMA
  0x37, BX_MODS_NONE, // BX_KEY_PERIOD
  0x38, BX_MODS_NONE, // BX_KEY_SLASH
  0x33, BX_MODS_NONE, // BX_KEY_SEMICOLON
  0x2E, BX_MODS_NONE, // BX_KEY_EQUALS
  0x2F, BX_MODS_NONE, // BX_KEY_LEFT_BRACKET
  0x31, BX_MODS_NONE, // BX_KEY_BACKSLASH
  0x30, BX_MODS_NONE, // BX_KEY_RIGHT_BRACKET
  0x2D, BX_MODS_NONE, // BX_KEY_MINUS
  0x35, BX_MODS_NONE, // BX_KEY_GRAVE
  0x2A, BX_MODS_NONE, // BX_KEY_BACKSPACE
  0x28, BX_MODS_NONE, // BX_KEY_ENTER
  0x2B, BX_MODS_NONE, // BX_KEY_TAB
  0x64, BX_MODS_NONE, // BX_KEY_LEFT_BACKSLASH
  0x46, BX_MODS_NONE, // BX_KEY_PRINT
  0x47, BX_MODS_NONE, // BX_KEY_SCRL_LOCK
  0x48, BX_MODS_NONE, // BX_KEY_PAUSE
  0x49, BX_MODS_NONE, // BX_KEY_INSERT
  0x4C, BX_MODS_NONE, // BX_KEY_DELETE
  0x4A, BX_MODS_NONE, // BX_KEY_HOME
  0x4D, BX_MODS_NONE, // BX_KEY_END
  0x4B, BX_MODS_NONE, // BX_KEY_PAGE_UP
  0x4E, BX_MODS_NONE, // BX_KEY_PAGE_DOWN

  0x57, BX_MODS_NONE, // BX_KEY_KP_ADD
  0x56, BX_MODS_NONE, // BX_KEY_KP_SUBTRACT
  0x59, BX_MODS_NONE, // BX_KEY_KP_END
  0x5A, BX_MODS_NONE, // BX_KEY_KP_DOWN
  0x5B, BX_MODS_NONE, // BX_KEY_KP_PAGE_DOWN
  0x5C, BX_MODS_NONE, // BX_KEY_KP_LEFT
  0x5E, BX_MODS_NONE, // BX_KEY_KP_RIGHT
  0x5F, BX_MODS_NONE, // BX_KEY_KP_HOME
  0x60, BX_MODS_NONE, // BX_KEY_KP_UP
  0x61, BX_MODS_NONE, // BX_KEY_KP_PAGE_UP
  0x62, BX_MODS_NONE, // BX_KEY_KP_INSERT
  0x63, BX_MODS_NONE, // BX_KEY_KP_DELETE
  0x5D, BX_MODS_NONE, // BX_KEY_KP_5

  0x52, BX_MODS_NONE, // BX_KEY_UP
  0x51, BX_MODS_NONE, // BX_KEY_DOWN
  0x50, BX_MODS_NONE, // BX_KEY_LEFT
  0x4F, BX_MODS_NONE, // BX_KEY_RIGHT
  
  0x58, BX_MODS_NONE, // BX_KEY_KP_ENTER
  0x55, BX_MODS_NONE, // BX_KEY_KP_MULTIPLY
  0x54, BX_MODS_NONE, // BX_KEY_KP_DIVIDE

  0xE3, BX_MODS_LEFT_GUI,  // BX_KEY_WIN_L (Bit 3 in packet.Modifier byte)
  0xE7, BX_MODS_RIGHT_GUI, // BX_KEY_WIN_R (Bit 7 in packet.Modifier byte)
  0x65, BX_MODS_NONE, // BX_KEY_MENU

  0x9A, BX_MODS_NONE, // BX_KEY_ALT_SYSREQ
  0x00, BX_MODS_NONE, // BX_KEY_CTRL_BREAK
  0x00, BX_MODS_NONE, // BX_KEY_INT_BACK
  0x00, BX_MODS_NONE, // BX_KEY_INT_FORWARD
  0x78, BX_MODS_NONE, // BX_KEY_INT_STOP
  0x00, BX_MODS_NONE, // BX_KEY_INT_MAIL
  0x00, BX_MODS_NONE, // BX_KEY_INT_SEARCH
  0x00, BX_MODS_NONE, // BX_KEY_INT_FAV
  0x00, BX_MODS_NONE, // BX_KEY_INT_HOME
  0x00, BX_MODS_NONE, // BX_KEY_MYCOMP
  0x00, BX_MODS_NONE, // BX_KEY_CALC
  0x00, BX_MODS_NONE, // BX_KEY_SLEEP
  0x66, BX_MODS_NONE, // BX_KEY_POWER_POWER
  0x00, BX_MODS_NONE  // BX_KEY_WAKE
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
  s.report_id = 0;
  if (d.type == USB_HID_TYPE_MOUSE)
    s.model = hid_mouse_3x3x8; // default model
  // next will be byte 2 in the 8 byte packet
  s.kbd_packet_indx = 1;

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
  // LOG_THIS setonoff(LOGLEV_DEBUG, ACT_REPORT);

  if ((d.type == USB_HID_TYPE_MOUSE) ||
      (d.type == USB_HID_TYPE_TABLET)) {
    if (d.speed == USB_SPEED_HIGH) {
      d.dev_descriptor = bx_mouse_dev_descriptor2;
      d.device_desc_size = sizeof(bx_mouse_dev_descriptor2);
      d.endpoint_info[USB_CONTROL_EP].max_packet_size = 64; // Control ep0
      d.endpoint_info[USB_CONTROL_EP].max_burst_size = 0;
      d.endpoint_info[1].max_packet_size = 8;  // In ep1
      d.endpoint_info[1].max_burst_size = 0;
    } else {
      d.dev_descriptor = bx_mouse_dev_descriptor;
      d.device_desc_size = sizeof(bx_mouse_dev_descriptor);
      d.endpoint_info[USB_CONTROL_EP].max_packet_size = 8; // Control ep0
      d.endpoint_info[USB_CONTROL_EP].max_burst_size = 0;
      d.endpoint_info[1].max_packet_size = 8;  // In ep1
      d.endpoint_info[1].max_burst_size = 0;
    }
    if (d.type == USB_HID_TYPE_MOUSE) {
      if (d.speed == USB_SPEED_HIGH) {
        d.config_descriptor = bx_mouse_config_descriptor2;
        d.config_desc_size = sizeof(bx_mouse_config_descriptor2);
      } else {
        if (s.model >= hid_mouse_3x3x8phy) {
          d.config_descriptor = bx_mouse_config_descriptor1;
          d.config_desc_size = sizeof(bx_mouse_config_descriptor1);
        } else {
          d.config_descriptor = bx_mouse_config_descriptor0;
          d.config_desc_size = sizeof(bx_mouse_config_descriptor0);
        }
      }
    } else {
      // Tablet
      if (d.speed == USB_SPEED_HIGH) {
        d.config_descriptor = bx_tablet_config_descriptor2;
        d.config_desc_size = sizeof(bx_tablet_config_descriptor2);
      } else {
        d.config_descriptor = bx_tablet_config_descriptor;
        d.config_desc_size = sizeof(bx_tablet_config_descriptor);
      }
    }
    // initialize the correct hid descriptor
    if (s.model == hid_mouse_2x2x8) {
      s.bx_mouse_hid_report_descriptor = bx_mouse_hid_report_descriptor_228;
      s.bx_mouse_hid_report_descriptor_len = sizeof(bx_mouse_hid_report_descriptor_228);
    } else if (s.model == hid_mouse_3x3x8) {
      s.bx_mouse_hid_report_descriptor = bx_mouse_hid_report_descriptor_338;
      s.bx_mouse_hid_report_descriptor_len = sizeof(bx_mouse_hid_report_descriptor_338);
    } else if (s.model == hid_mouse_3x3xDebug) {
      s.bx_mouse_hid_report_descriptor = bx_mouse_hid_report_descriptor_33debug;
      s.bx_mouse_hid_report_descriptor_len = sizeof(bx_mouse_hid_report_descriptor_33debug);
    } else if (s.model == hid_mouse_3x3x12) {
      s.bx_mouse_hid_report_descriptor = bx_mouse_hid_report_descriptor_3312;
      s.bx_mouse_hid_report_descriptor_len = sizeof(bx_mouse_hid_report_descriptor_3312);
    } else if (s.model == hid_mouse_3x3x16) {
      s.bx_mouse_hid_report_descriptor = bx_mouse_hid_report_descriptor_3316;
      s.bx_mouse_hid_report_descriptor_len = sizeof(bx_mouse_hid_report_descriptor_3316);
    } else if (s.model == hid_mouse_3x3x8phy) {
      s.bx_mouse_hid_report_descriptor = bx_mouse_hid_report_descriptor_338phy;
      s.bx_mouse_hid_report_descriptor_len = sizeof(bx_mouse_hid_report_descriptor_338phy);
    } else {
      BX_PANIC(("Unknown mouse model type used"));
    }
    // update the hid descriptor length fields
    * (Bit16u *) &bx_mouse_config_descriptor0[BX_Mouse_Config_Descriptor0_pos] =
    * (Bit16u *) &bx_mouse_config_descriptor1[BX_Mouse_Config_Descriptor1_pos] =
    * (Bit16u *) &bx_mouse_config_descriptor2[BX_Mouse_Config_Descriptor2_pos] =
    * (Bit16u *) &bx_mouse_hid_descriptor0[BX_Mouse_Hid_Descriptor0] =
    * (Bit16u *) &bx_mouse_hid_descriptor1[BX_Mouse_Hid_Descriptor1] =
      s.bx_mouse_hid_report_descriptor_len;
  } else { // keyboard or keypad
    if (d.speed == USB_SPEED_HIGH) {
      d.dev_descriptor = bx_keypad_dev_descriptor2;
      d.device_desc_size = sizeof(bx_keypad_dev_descriptor2);
      d.config_descriptor = bx_keypad_config_descriptor2;
      d.config_desc_size = sizeof(bx_keypad_config_descriptor2);
      d.endpoint_info[USB_CONTROL_EP].max_packet_size = 64; // Control ep0
      d.endpoint_info[USB_CONTROL_EP].max_burst_size = 0;
      d.endpoint_info[1].max_packet_size = 8;  // In ep1
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

bool usb_hid_device_c::set_option(const char *option)
{
  if (!strncmp(option, "model:", 6)) {
    s.report_use_id = 0; // assume no report id
    // mice
    if (!strcmp(option+6, "m228")) {
      s.model = hid_mouse_2x2x8;
      s.report_use_id = HID_REPORT_ID;  // this model uses a report id
    } else if (!strcmp(option+6, "m338")) {
      s.model = hid_mouse_3x3x8;
    } else if (!strcmp(option+6, "m33xDebug")) {
      s.model = hid_mouse_3x3xDebug;
    } else if (!strcmp(option+6, "m3312")) {
      s.model = hid_mouse_3x3x12;
    } else if (!strcmp(option+6, "m3316")) {
      s.model = hid_mouse_3x3x16;
    } else if (!strcmp(option+6, "m338phy")) {
      s.model = hid_mouse_3x3x8phy;
      if (d.speed > USB_SPEED_FULL)
        BX_PANIC(("The Physical Descriptor model must be used on a Low- or Full-speed device only."));
    }
    // keyboards
       // only the default model (none)
    // keypads
       // only the default model (none)
    // tablets
       // only the default model (none)
    return 1;
  }
  
  return 0;
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
  new bx_shadow_data_c(list, "mouse_event_buf", (Bit8u *) s.mouse_event_buf, BX_KBD_ELEMENTS * BX_M_ELEMENTS_SIZE, 1);
  new bx_shadow_data_c(list, "mouse_event_buf_len", (Bit8u *) s.mouse_event_buf_len, BX_KBD_ELEMENTS, 1);
  if ((d.type == USB_HID_TYPE_KEYPAD) || (d.type == USB_HID_TYPE_KEYBOARD)) {
    new bx_shadow_data_c(list, "kbd_packet", s.kbd_packet, 8, 1);
    BXRS_HEX_PARAM_FIELD(list, kbd_packet_indx, s.kbd_packet_indx);
    BXRS_HEX_PARAM_FIELD(list, indicators, s.indicators);
    BXRS_DEC_PARAM_FIELD(list, kbd_event_count, s.kbd_event_count);
    BXRS_DEC_PARAM_FIELD(list, report_use_id, s.report_use_id);
    BXRS_DEC_PARAM_FIELD(list, report_id, s.report_id);
    BXRS_DEC_PARAM_FIELD(list, bx_mouse_hid_report_descriptor_len, s.bx_mouse_hid_report_descriptor_len);
    new bx_shadow_data_c(list, "bx_mouse_hid_report_descriptor", (Bit8u *) s.bx_mouse_hid_report_descriptor, s.bx_mouse_hid_report_descriptor_len, 0);
    new bx_shadow_data_c(list, "model", (Bit8u *) &s.model, sizeof(s.model), 0);
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
  memset((void *) &s, 0, offsetof(struct HID_STATE, model));
  BX_DEBUG(("Reset"));
  
  // HID 1.11, section 7.2.6, page 54(64):
  //  "When initialized, all devices default to report protocol."
  s.boot_protocol = PROTOCOL_REPORT;
  // next will be byte 2 in the 8 byte packet
  s.kbd_packet_indx = 1;
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
    case EndpointRequest | USB_REQ_GET_STATUS:
      BX_DEBUG(("USB_REQ_GET_STATUS: Endpoint."));
      // if the endpoint is currently halted, return bit 0 = 1
      if (value == USB_ENDPOINT_HALT) {
        if (index == 0x81) {
          data[0] = 0x00 | (get_halted(index) ? 1 : 0);
          data[1] = 0x00;
          ret = 2;
        } else {
          BX_ERROR(("EndpointRequest | USB_REQ_GET_STATUS: index > ep count: %d", index));
          goto fail;
        }
      } else {
        BX_ERROR(("EndpointRequest | USB_REQ_SET_FEATURE: Unknown Get Status Request found: %d", value));
        goto fail;
      }
      break;
    case DeviceRequest | USB_REQ_GET_DESCRIPTOR:
      BX_DEBUG(("HID: DeviceRequest | USB_REQ_GET_DESCRIPTOR:"));
      switch(value >> 8) {
        case USB_DT_STRING:
          switch(value & 0xff) {
            case HID_CLASS_STR4:
              ret = set_usb_string(data, "HID Mouse");
              break;
            case HID_CLASS_STR5:
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
            if (s.model >= hid_mouse_3x3x8phy) {
              memcpy(data, bx_mouse_hid_descriptor1,
                     sizeof(bx_mouse_hid_descriptor1));
              ret = sizeof(bx_mouse_hid_descriptor1);
            } else {
              memcpy(data, bx_mouse_hid_descriptor0,
                     sizeof(bx_mouse_hid_descriptor0));
              ret = sizeof(bx_mouse_hid_descriptor0);
            }
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
            memcpy(data, s.bx_mouse_hid_report_descriptor, s.bx_mouse_hid_report_descriptor_len);
            ret = s.bx_mouse_hid_report_descriptor_len;
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
          // now the guest knows the report id, so we need to use it
          s.report_id = s.report_use_id;
          break;
        case 0x23:  // HID Physical Descriptor
          BX_ERROR(("USB HID handle_control: Host requested the HID Physical Descriptor"));
          if (d.type == USB_HID_TYPE_MOUSE) {
            int set = (value & 0xFF);
            if ((set >= 0) && (set <= 2)) {
              memcpy(data, bx_mouse_phys_descriptor + (set * HID_PHYS_DESC_SET_LEN), HID_PHYS_DESC_SET_LEN);
              ret = HID_PHYS_DESC_SET_LEN;
            } else {
              goto fail;
            }
          } else {
            goto fail;
          }
          break;
        default:  // 0x24 -> 0x2F
          BX_ERROR(("USB HID handle_control: unknown HID descriptor 0x%02x", value >> 8));
          goto fail;
        }
        break;
    case InterfaceInClassRequest | GET_REPORT:
      BX_DEBUG(("HID: GET_REPORT:"));
      if ((value >> 8) == 1) { // Input report
        if ((value & 0xFF) == s.report_id) {
          if ((d.type == USB_HID_TYPE_MOUSE) ||
              (d.type == USB_HID_TYPE_TABLET)) {
            ret = mouse_poll(data, 1);
            if (ret > length)
              ret = length;
          } else if ((d.type == USB_HID_TYPE_KEYPAD) ||
                     (d.type == USB_HID_TYPE_KEYBOARD)) {
            ret = keyboard_poll(data, 1);
            if (ret > length)
              ret = length;
          } else {
            goto fail;
          }
        } else {
          BX_ERROR(("USB HID handle_control: Report ID (%d) doesn't match requested ID (%d)", s.report_id, value & 0xFF));
          goto fail;
        }
      } else {
        BX_ERROR(("USB HID handle_control: Requested report type (%d) must be Input(1)", (value >> 8) & 0xFF));
        goto fail;
      }
      break;
    case InterfaceOutClassRequest | SET_REPORT:
      BX_DEBUG(("HID: SET_REPORT:"));
      if (((d.type == USB_HID_TYPE_KEYPAD) ||
           (d.type == USB_HID_TYPE_KEYBOARD)) && (value == 0x0200)) { // 0x02 = Report Type: Output, 0x00 = ID (our keyboard/keypad use an ID of zero)
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
      if ((value & 0xFF00) != 0) {
        BX_ERROR(("USB HID handle_control: High byte of Value must be 0."));
      }
      if ((value & 0xFF) == s.report_id) {
        data[0] = s.idle;
        ret = 1;
      } else {
        BX_ERROR(("USB HID handle_control: Report ID (%d) doesn't match requested ID (%d)", s.report_id, value & 0xFF));
        goto fail;
      }
      break;
    case InterfaceOutClassRequest | SET_IDLE:
      BX_DEBUG(("HID: SET_IDLE:"));
      // The wLength field should be 0 for this request
      if (length != 0) {
        BX_ERROR(("USB HID handle_control: The wLength field should be 0 for this request."));
      }
      if ((value & 0xFF) == s.report_id) {
        s.idle = (value >> 8);
        start_idle_timer();
        ret = 0;
      } else {
        BX_ERROR(("USB HID handle_control: Report ID (%d) doesn't match requested ID (%d)", s.report_id, value & 0xFF));
        goto fail;
      }
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
          ret = mouse_poll(p->data, 0);
        } else if ((d.type == USB_HID_TYPE_KEYPAD) ||
                   (d.type == USB_HID_TYPE_KEYBOARD)) {
          ret = keyboard_poll(p->data, 0);
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

  if (ret > 0) usb_dump_packet(p->data, ret, 0, p->devaddr, p->devep, USB_TRANS_TYPE_BULK, false, true);

  return ret;
}

int usb_hid_device_c::mouse_poll(Bit8u *buf, bool force)
{
  int l = USB_RET_NAK;

  if (d.type == USB_HID_TYPE_MOUSE) {
    if (!s.has_events) {
      // if there's no new movement, handle delayed one
      mouse_enq(0, 0, s.mouse_z, s.b_state, 0);
    }
    if (s.has_events || force) {
      if (s.mouse_event_count > 0) {
        l = get_mouse_packet(buf);
      } else {
        l = create_mouse_packet(buf);
      }
      s.has_events = (s.mouse_event_count > 0);
      start_idle_timer();
    }
  } else if (d.type == USB_HID_TYPE_TABLET) {
    if (s.has_events || force) {
      if (s.mouse_event_count > 0) {
        l = get_mouse_packet(buf);
      } else {
        l = create_mouse_packet(buf);
      }
      s.has_events = (s.mouse_event_count > 0);
      start_idle_timer();
    }
  }
  return l;
}

// must not return more than BX_M_ELEMENTS_SIZE bytes
int usb_hid_device_c::create_mouse_packet(Bit8u *buf)
{
  int l = 0;
  
  // The HID Boot Protocol report is only three bytes long
  if (s.boot_protocol == PROTOCOL_BOOT) {
    buf[0] = (Bit8u) s.b_state;
    buf[1] = (Bit8s) s.mouse_x;
    buf[2] = (Bit8s) s.mouse_y;
    l = 3;
  } else { // do the Report Protocol
    
    // do we add a Report ID field?
    if (s.report_id > 0) {
      *buf++ = s.report_id;
      l = 1;
    }
    
    if (d.type == USB_HID_TYPE_TABLET) {
      *buf++ = (Bit8u)  s.b_state;
      *buf++ = (Bit8u) (s.mouse_x & 0xff);
      *buf++ = (Bit8u) (s.mouse_x >> 8);
      *buf++ = (Bit8u) (s.mouse_y & 0xff);
      *buf++ = (Bit8u) (s.mouse_y >> 8);
      *buf++ = (Bit8s)  s.mouse_z;
      l += 6;
    } else { 
      // USB_HID_TYPE_MOUSE
      switch (s.model) {
        // default 2-button, X and Y coords, 8-bit
        // 000000BB
        // XXXXXXXX
        // YYYYYYYY
        case hid_mouse_2x2x8:
          *buf++ = (s.b_state & 3);
          *buf++ = (Bit8s) s.mouse_x;
          *buf++ = (Bit8s) s.mouse_y;
          l += 3;
          break;
        
        // 3-button, X, Y, and Wheel coords, 8-bit
        // 00000BBB
        // XXXXXXXX
        // YYYYYYYY
        // WWWWWWWW
        case hid_mouse_3x3x8:
        case hid_mouse_3x3x8phy:
          *buf++ = s.b_state & 7;
          *buf++ = (Bit8s) s.mouse_x;
          *buf++ = (Bit8s) s.mouse_y;
          *buf++ = (Bit8s) s.mouse_z;
          l += 4;
          break;
        
        // 3-button, 5-byte X, Y, and Wheel coords (debug model)
        // YYYYYYY0 - 10 bit Y displacement
        // WWWW0YYY - 8 bit W displacement
        // 0B00WWWW - bit 6 is Button #2 (right button)
        // XXXXX0B0 - 9 bit X displacement, bit 1 is Button #1 (left button)
        // 0B00XXXX - bit 6 is Button #3 (middle button)
        case hid_mouse_3x3xDebug:
          *buf++ = (Bit8u)  (((Bit16u) s.mouse_y & 0x7F) << 1);
          *buf++ = (Bit8u) ((((Bit16u) s.mouse_y >> 7) & 0x07) |
                            (((Bit16u) s.mouse_z & 0x0F) << 4));
          *buf++ = (Bit8u) ((((Bit16u) s.mouse_z >> 4) & 0x0F) |
                            (        ((s.b_state & 2) >> 1) << 6));
          *buf++ = (Bit8u) ((        ((s.b_state & 1) >> 0) << 1) |
                           (((Bit16u) s.mouse_x & 0x1F) << 3));
          *buf++ = (Bit8u) ((((Bit16u) s.mouse_x >> 5) & 0x0F) |
                            (        ((s.b_state & 4) >> 2) << 6));
          l += 5;
          break;
        
        // 3-button, X, Y, and Wheel coords, 12-bit
        // 00000BBB
        // XXXXXXXX (lsb)
        // YYYYXXXX (lsb of y, msb of x)
        // YYYYYYYY (msb)
        // WWWWWWWW
        case hid_mouse_3x3x12:
          *buf++ = s.b_state & 7;
          *buf++ = (Bit8u)   ((Bit16u) s.mouse_x & 0xFF);
          *buf++ = (Bit8u) ((((Bit16u) s.mouse_x >> 8) & 0x0F) |
                            (((Bit16u) s.mouse_y & 0x0F) << 4));
          *buf++ = (Bit8u)  (((Bit16u) s.mouse_y >> 4) & 0xFF);
          *buf++ = (Bit8s) s.mouse_z;
          l += 5;
          break;
        
        // 3-button, X, Y, and Wheel coords, 16-bit
        // 00000BBB
        // XXXXXXXX  (lsb)
        // XXXXXXXX  (msb)
        // YYYYYYYY  (lsb)
        // YYYYYYYY  (msb)
        // WWWWWWWW
        case hid_mouse_3x3x16:
          *buf++ = s.b_state & 7;
          *buf++ = (Bit8u) ((Bit16u) s.mouse_x & 0xFF);
          *buf++ = (Bit8u) ((Bit16u) s.mouse_x >> 8) & 0xFF;
          *buf++ = (Bit8u) ((Bit16u) s.mouse_y & 0xFF);
          *buf++ = (Bit8u) ((Bit16u) s.mouse_y >> 8) & 0xFF;
          *buf++ = (Bit8s) s.mouse_z;
          l += 6;
          break;
      }
      s.mouse_x = 0;
      s.mouse_y = 0;
      s.mouse_z = 0;
    }
  }
  
  return l;
}

int usb_hid_device_c::get_mouse_packet(Bit8u *buf)
{
  int l = USB_RET_NAK;

  if (s.mouse_event_count > 0) {
    memcpy(buf, s.mouse_event_buf[0], s.mouse_event_buf_len[0]);
    l = s.mouse_event_buf_len[0];
    if (--s.mouse_event_count > 0) {
      memmove(s.mouse_event_buf[0], s.mouse_event_buf[1],
              s.mouse_event_count * BX_M_ELEMENTS_SIZE);
      memmove(&s.mouse_event_buf_len[0], &s.mouse_event_buf_len[1],
              s.mouse_event_count * sizeof(s.mouse_event_buf_len[0]));
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
        s.mouse_event_buf_len[s.mouse_event_count] = 
          create_mouse_packet(s.mouse_event_buf[s.mouse_event_count]);
        s.mouse_event_count++;
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
        s.mouse_event_buf_len[s.mouse_event_count] = 
          create_mouse_packet(s.mouse_event_buf[s.mouse_event_count]);
        s.mouse_event_count++;
        }
      }
      s.has_events = 1;
    }
  }
}

int usb_hid_device_c::keyboard_poll(Bit8u *buf, bool force)
{
  int l = USB_RET_NAK;

  if ((d.type == USB_HID_TYPE_KEYPAD) ||
      (d.type == USB_HID_TYPE_KEYBOARD)) {
    if (s.has_events || force) {
      memcpy(buf, s.kbd_packet, 8);
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

// return 1 to indicate that we handled the key.
// return 0 to indicate that the underlining keyboard needs to handle the key.
bool usb_hid_device_c::gen_scancode(Bit32u key)
{
  Bit8u released = (key & BX_KEY_RELEASED) != 0;
  Bit8u code = usbkbd_conv[key & ~BX_KEY_RELEASED].code;
  Bit8u modkey = usbkbd_conv[key & ~BX_KEY_RELEASED].modkey;
  
  // if the keypad, only process the numpad keys
  if (d.type == USB_HID_TYPE_KEYPAD) {
    // else only allow the standard keypad keys
    if ((code < 0x53) || (code > 0x63)) {
      return 0;
    }
  }
  // if we don't support this key, just return
  if (code == 0)
    return 1;
  
  // printed 'modkey' will bit the "modifying bit" position, else -1
  BX_DEBUG(("  key: 0x%08X, code: 0x%02X, modkey = %i", 
    key, code, ((code & 0xF0) == 0xE0) ? (code & 0x0F) : -1));

  // if we already have keys in the 'queue', add this one and return
  if (s.has_events) {
    if (s.kbd_event_count < BX_KBD_ELEMENTS) {
      s.kbd_event_buf[s.kbd_event_count++] = key;
    }
    return 1;
  }
  
  // was the previous packet a 'Phantom' packet?
  // if so, reset the packet
  if (s.kbd_packet_indx > 7) {
    memset(s.kbd_packet, 0, 8);
    s.kbd_packet_indx = 1;
  }
  
  // was it a modifying key?
  if (modkey != BX_MODS_NONE) {
    if (released) {
      s.kbd_packet[0] &= ~modkey;
    } else {
      s.kbd_packet[0] |= modkey;
    }
  // else it was a normal key
  } else {
    // if releasing a key, remove from the packet buffer
    if (released) {
      for (int i=2; i<8; i++) {
        if (s.kbd_packet[i] == code) {
          for (int j=i; j<7; j++)
            s.kbd_packet[j] = s.kbd_packet[j+1];
          s.kbd_packet[7] = 0;
          if (s.kbd_packet_indx > 1)
            s.kbd_packet_indx--;
          s.has_events = 1;
          break;
        }
      }
    // if pressing a key, add to the packet buffer
    } else {
      s.kbd_packet_indx++;
      // if we overflow, create a 'Phantom' packet (0, 0, 1, 1, 1, 1, 1, 1)
      if (s.kbd_packet_indx > 7) {
        memset(&s.kbd_packet[0], 0, 2);
        memset(&s.kbd_packet[2], 1, 6);
      } else
        s.kbd_packet[s.kbd_packet_indx] = code;
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
