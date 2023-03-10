/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2023  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2009-2023  The Bochs Project
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
//
/////////////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
#include "usb_common.h"
#include "usb_printer.h"

#define LOG_THIS

// USB device plugin entry point

PLUGIN_ENTRY_FOR_MODULE(usb_printer)
{
  if (mode == PLUGIN_PROBE) {
    return (int) PLUGTYPE_USB;
  }
  return 0; // Success
}

//
// Define the static class that registers the derived USB device class,
// and allocates one on request.
//
class bx_usb_printer_locator_c : public usbdev_locator_c {
public:
  bx_usb_printer_locator_c(void) : usbdev_locator_c("usb_printer") {}
protected:
  usb_device_c *allocate(const char *devname) {
    return (new usb_printer_device_c());
  }
} bx_usb_printer_match;

// If you change any of the Max Packet Size, or other fields within these
//  descriptors, you must also change the d.endpoint_info[] array
//  to match your changes.

static const Bit8u bx_printer_dev_descriptor[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x10, 0x01, /*  u16 bcdUSB; v1.10 */

  0x00,       /*  u8  bDeviceClass; */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; [ low/full speeds only ] */
  0x40,       /*  u8  bMaxPacketSize; 64 Bytes */

  0xF0, 0x03, /*  u16 idVendor; */
  0x04, 0x15, /*  u16 idProduct; */
  0x00, 0x01, /*  u16 bcdDevice */

  0x01,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x03,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

static const Bit8u bx_printer_config_descriptor[] = {
  /* one configuration */
  0x09,       /*  u8  bLength; */
  0x02,       /*  u8  bDescriptorType; Configuration */
  0x20, 0x00, /*  u16 wTotalLength; */
  0x01,       /*  u8  bNumInterfaces; (1) */
  0x01,       /*  u8  bConfigurationValue; */
  0x00,       /*  u8  iConfiguration string desc.; */
  0xC0,       /*  u8  bmAttributes;
			 Bit 7: must be set,
			     6: Self-powered,
			     5: Remote wakeup,
			     4..0: resvd */
  0x02,       /*  u8  MaxPower; */

  /* one interface */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x02,       /*  u8  if_bNumEndpoints; */
  0x07,       /*  u8  if_bInterfaceClass; */
  0x01,       /*  u8  if_bInterfaceSubClass; */
  0x02,       /*  u8  if_bInterfaceProtocol; */
  0x00,       /*  u8  if_iInterface; string desc. */
  
  /* first endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x02,       /*  u8  ep_bmAttributes; Bulk */
  0x40, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x00,       /*  u8  ep_bInterval; */

  /* second endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x02,       /*  u8  ep_bEndpointAddress; OUT Endpoint 2 */
  0x02,       /*  u8  ep_bmAttributes; Bulk */
  0x40, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x00,       /*  u8  ep_bInterval; */
};

// Length word calculated at run time
static const Bit8u bx_device_id_string[] =
  "\0\0"  // len field is calculated at run-time
  "MFG:HEWLETT-PACKARD;"
  "MDL:DESKJET 920C;"
  "CMD:MLC,PCL,PML;"
  "CLASS:PRINTER;"
  "DESCRIPTION:Hewlett-Packard DeskJet 920C;"
  "SERN:CN21R1C0BPIS;"
  "VSTATUS:$HBO,$NCO,ff,DN,IDLE,CUT,K0,C0,SM,NR,KP093,CP097;"
  "VP:0800,FL,B0;"
  "VJ: ;";

static Bit8u usb_printer_count = 0;

usb_printer_device_c::usb_printer_device_c()
{
  char pname[12];
  char label[32];
  bx_param_string_c *fname;

  d.speed = d.minspeed = d.maxspeed = USB_SPEED_FULL;
  memset((void *) &s, 0, sizeof(s));
  strcpy(d.devname, "USB Printer");
  d.dev_descriptor = bx_printer_dev_descriptor;
  d.config_descriptor = bx_printer_config_descriptor;
  d.device_desc_size = sizeof(bx_printer_dev_descriptor);
  d.config_desc_size = sizeof(bx_printer_config_descriptor);
  d.endpoint_info[USB_CONTROL_EP].max_packet_size = 64; // Control ep0
  d.endpoint_info[USB_CONTROL_EP].max_burst_size = 0;
  d.endpoint_info[1].max_packet_size = 64;  // In ep1
  d.endpoint_info[1].max_burst_size = 0;
  d.endpoint_info[2].max_packet_size = 64;  // Out ep2
  d.endpoint_info[2].max_burst_size = 0;
  d.vendor_desc = "Hewlett-Packard";
  d.product_desc = "Deskjet 920C";
  d.serial_num = "HU18L6P2DNBI";
  s.fname[0] = 0;
  s.fp = NULL;
  // config options
  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  sprintf(pname, "printer%u", ++usb_printer_count);
  sprintf(label, "USB Printer #%u Configuration", usb_printer_count);
  s.config = new bx_list_c(usb_rt, pname, label);
  s.config->set_options(bx_list_c::SHOW_PARENT | bx_list_c::USE_BOX_TITLE);
  s.config->set_device_param(this);
  fname = new bx_param_filename_c(s.config, "file", "File", "", "", BX_PATHNAME_LEN);
  fname->set_handler(printfile_handler);
  if (SIM->is_wx_selected()) {
    bx_list_c *usb = (bx_list_c *) SIM->get_param("ports.usb");
    usb->add(s.config);
  }
  put("usb_printer", "USBPRN");
}

usb_printer_device_c::~usb_printer_device_c(void)
{
  if (s.fp != NULL) {
    fclose(s.fp);
  }
  if (SIM->is_wx_selected()) {
    bx_list_c *usb = (bx_list_c *) SIM->get_param("ports.usb");
    usb->remove(s.config->get_name());
  }
  bx_list_c *usb_rt = (bx_list_c *) SIM->get_param(BXPN_MENU_RUNTIME_USB);
  usb_rt->remove(s.config->get_name());
}

bool usb_printer_device_c::set_option(const char *option)
{
  if (!strncmp(option, "file:", 5)) {
    strcpy(s.fname, option + 5);
    SIM->get_param_string("file", s.config)->set(s.fname);
    return 1;
  }
  return 0;
}

bool usb_printer_device_c::init()
{
  if (strlen(s.fname) > 0) {
    s.fp = fopen(s.fname, "w+b");
    if (s.fp == NULL) {
      BX_ERROR(("Could not create/open '%s'", s.fname));
    } else {
      sprintf(s.info_txt, "USB printer: file='%s'", s.fname);
      d.connected = 1;
      return 1;
    }
  } else {
    BX_ERROR(("USB printer: missing output file"));
  }
  d.alt_iface_max = 0;
  return 0;
}

const char *usb_printer_device_c::get_info()
{
  return s.info_txt;
}

void usb_printer_device_c::register_state_specific(bx_list_c *parent)
{
  bx_list_c *list = new bx_list_c(parent, "s", "USB PRINTER Device State");
  BXRS_HEX_PARAM_FIELD(list, printer_status, s.printer_status);
}

void usb_printer_device_c::handle_reset()
{
  BX_DEBUG(("Reset"));
}

int usb_printer_device_c::handle_control(int request, int value, int index, int length, Bit8u *data)
{
  int ret = 0;

  BX_DEBUG(("Printer: request: 0x%04X  value: 0x%04X  index: 0x%04X  len: %d", request, value, index, length));

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
          switch (value & 0xFF) {
            case 0xEE:
              // Microsoft OS Descriptor check
              // We don't support this check, so fail
              BX_INFO(("USB Printer handle_control: Microsoft OS specific 0xEE string descriptor"));
              goto fail;
              break;
            default:
              BX_ERROR(("USB Printer handle_control: unknown string descriptor 0x%02x", value & 0xff));
              goto fail;
              break;
          }
          break;
        default:
          BX_ERROR(("USB Printer handle_control: unknown descriptor type 0x%02x", value >> 8));
          goto fail;
      }
      break;

    /* printer specific requests */
    case InterfaceInClassRequest | 0x00:  // 1284 get device id string
      memcpy(data, bx_device_id_string, sizeof(bx_device_id_string));
      ret = sizeof(bx_device_id_string);
      data[0] = (Bit8u) (ret >> 8);   // len word is big endian
      data[1] = (Bit8u) (ret & 0xFF); //
      break;
    case InterfaceInClassRequest | 0x01:  // Get Port Status
      s.printer_status = (0<<5) | (1<<4) | (1<<3);
      memcpy(data, &s.printer_status, 1);
      ret = 1;
      break;
    case InterfaceOutClassRequest | 0x02:  // soft reset
      // for now, just return
      ret = 0;
      break;
    default:
      BX_ERROR(("USB PRINTER handle_control: unknown request 0x%04x", request));
    fail:
      d.stall = 1;
      ret = USB_RET_STALL;
      break;
  }
  return ret;
}

int usb_printer_device_c::handle_data(USBPacket *p)
{
  int ret = 0;

  // check that the length is <= the max packet size of the device
  if (p->len > get_mps(p->devep)) {
    BX_DEBUG(("EP%d transfer length (%d) is greater than Max Packet Size (%d).", p->devep, p->len, get_mps(p->devep)));
  }
  
  switch(p->pid) {
    case USB_TOKEN_IN:
      if (p->devep == 1) {
        BX_INFO(("Printer: handle_data: IN: len = %d", p->len));
        BX_INFO(("Printer: Ben: We need to find out what this is and send valid status back"));
        ret = p->len;
      } else {
        goto fail;
      }
      break;
    case USB_TOKEN_OUT:
      if (p->devep == 2) {
        BX_DEBUG(("Sent %d bytes to the 'usb printer': %s", p->len, s.fname));
        usb_dump_packet(p->data, p->len, 0, p->devaddr, USB_DIR_OUT | p->devep, USB_TRANS_TYPE_CONTROL, false, true);
        if (s.fp != NULL) {
          fwrite(p->data, 1, p->len, s.fp);
        }
        ret = p->len;
      } else {
        goto fail;
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

#undef LOG_THIS
#define LOG_THIS printer->

// USB printer runtime parameter handlers
const char *usb_printer_device_c::printfile_handler(bx_param_string_c *param, bool set,
                                                    const char *oldval, const char *val,
                                                    int maxlen)
{
  usb_printer_device_c *printer;

  if (set) {
    if (strlen(val) < 1) {
      val = "none";
    }
    printer = (usb_printer_device_c*) param->get_parent()->get_device_param();
    if (printer != NULL) {
      if (printer->s.fp != NULL) {
        fclose(printer->s.fp);
      }
      if (strcmp(val, "none")) {
        printer->s.fp = fopen(val, "w+b");
        if (printer->s.fp != NULL) {
          strcpy(printer->s.fname, val);
          sprintf(printer->s.info_txt, "USB printer: file='%s'", printer->s.fname);
        } else {
          BX_ERROR(("Could not create/open '%s'", val));
          printer->s.fname[0] = 0;
        }
      } else {
        printer->s.fname[0] = 0;
      }
    } else {
      BX_PANIC(("printfile_handler: printer not found"));
    }
  }
  return val;
}
#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
