/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  UFI/CBI floppy disk storage device support
//
//  Copyright (c) 2015-2023  Benjamin David Lunt
//  Copyright (C) 2015-2023  The Bochs Project
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

/* Notes by Ben Lunt:
   - My purpose of coding this emulation was/is to test some other code.
   - This emulation will emulate an external USB Floppy Disk Drive with
     an inserted 1.44 meg *only* disk.
   - This emulation shows different formats allowed, but does not support
     changing to them.  Use with caution if you change formats.
   - If you have any questions, send me a note at fys [at] fysnet [dot] net
  */

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
#include "usb_common.h"
#include "hdimage/hdimage.h"
#include "usb_floppy.h"

#define LOG_THIS

// USB device plugin entry point

PLUGIN_ENTRY_FOR_MODULE(usb_floppy)
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
class bx_usb_floppy_locator_c : public usbdev_locator_c {
public:
  bx_usb_floppy_locator_c(void) : usbdev_locator_c("usb_floppy") {}
protected:
  usb_device_c *allocate(const char *devname) {
    return (new usb_floppy_device_c());
  }
} bx_usb_floppy_match;

// maximum size of the read buffer in sectors
#define USB_FLOPPY_MAX_SECTORS   18

// useconds per sector at 300 RPM
#define USB_FLOPPY_SECTOR_TIME  11111

// Set this to zero if you only support CB interface.  Set to 1 if you support CBI.
#define USB_FLOPPY_USE_INTERRUPT  1

/* A well known, somewhat older, but still widely used Operating System
 *  must have the Vendor ID as the TEAC external drive emulated below.  If
 *  the VendorID is not the TEAC id, this operating system doesn't know what
 *  to do with the drive.  Go figure.  Therefore, use the optionsX="model=teac"
 *  options in the bochsrc.txt file to set this model.  If you do not, a
 *  default model will be used.
 */

// USB requests
#define GetMaxLun         0xfe

// If you change any of the Max Packet Size, or other fields within these
//  descriptors, you must also change the d.endpoint_info[] array
//  to match your changes.

// Full-speed only
static Bit8u bx_floppy_dev_descriptor[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x00, 0x02, /*  u16 bcdUSB; v2.0 */

  0x00,       /*  u8  bDeviceClass; */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; [ low/full speeds only ] */
  0x40,       /*  u8  bMaxPacketSize; 64 Bytes */

  /* Vendor and product id are arbitrary.  */
  0x00, 0x00, /*  u16 idVendor; */
  0x00, 0x00, /*  u16 idProduct; */
  0x00, 0x00, /*  u16 bcdDevice */

  0x01,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x03,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

// Full-speed only
static const Bit8u bx_floppy_config_descriptor[] = {

  /* one configuration */
  0x09,       /*  u8  bLength; */
  0x02,       /*  u8  bDescriptorType; Configuration */
  0x27, 0x00, /*  u16 wTotalLength; */
  0x01,       /*  u8  bNumInterfaces; (1) */
  0x01,       /*  u8  bConfigurationValue; */
  0x00,       /*  u8  iConfiguration; */
  0x80,       /*  u8  bmAttributes;
                        Bit 7: must be set,
                            6: Self-powered,
                            5: Remote wakeup,
                            4..0: resvd */
  0xFA,       /*  u8  MaxPower; */

  /* one interface */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
#if USB_FLOPPY_USE_INTERRUPT
  0x03,       /*  u8  if_bNumEndpoints; (CBI has 3)*/
#else
  0x02,       /*  u8  if_bNumEndpoints; (CB has 2) */
#endif
  0x08,       /*  u8  if_bInterfaceClass; MASS STORAGE */
  0x04,       /*  u8  if_bInterfaceSubClass; CB(I) */
#if USB_FLOPPY_USE_INTERRUPT
  0x00,       /*  u8  if_bInterfaceProtocol; CB with Interrupt */
#else
  0x01,       /*  u8  if_bInterfaceProtocol; CB with out Interrupt */
#endif
  0x00,       /*  u8  if_iInterface; */

  /* Bulk-In endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x02,       /*  u8  ep_bmAttributes; Bulk */
  0x40, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x00,       /*  u8  ep_bInterval; */

  /* Bulk-Out endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x02,       /*  u8  ep_bEndpointAddress; OUT Endpoint 2 */
  0x02,       /*  u8  ep_bmAttributes; Bulk */
  0x40, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x00,       /*  u8  ep_bInterval; */

#if USB_FLOPPY_USE_INTERRUPT
  /* Interrupt endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x83,       /*  u8  ep_bEndpointAddress; IN Endpoint 3 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x02, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x20        /*  u8  ep_bInterval; */
#endif
};

static const Bit8u bx_floppy_dev_inquiry_teac[] = {
  0x00,       /*  perifpheral device type */
  0x80,       /*  RMB = 1, reserved = 0 */
  0x00,       /*  ISO version, ecma version , ansi version */
  0x01,       /*  response data format */
  0x1F,       /*  additional length */
  0x00, 0x00, 0x00,  /* reserved */
  0x54, 0x45, 0x41, 0x43, 0x20, 0x20, 0x20, 0x20,  /* vendor information  */
  0x46, 0x44, 0x2D, 0x30, 0x35, 0x50, 0x55, 0x57,  /* product information */
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,  /*    (cont.)          */
  0x33, 0x30, 0x30, 0x30                           /* revision level      */
};

static const Bit8u bx_floppy_dev_inquiry_bochs[] = {
  0x00,       /*  perifpheral device type */
  0x80,       /*  RMB = 1, reserved = 0 */
  0x00,       /*  ISO version, ecma version , ansi version */
  0x01,       /*  response data format */
  0x1F,       /*  additional length */
  0x00, 0x00, 0x00,  /* reserved */
  0x42, 0x4F, 0x43, 0x48, 0x53, 0x20, 0x20, 0x20,  /* vendor information  */
  0x55, 0x53, 0x42, 0x20, 0x43, 0x42, 0x49, 0x20,  /* product information */
  0x46, 0x4C, 0x4F, 0x50, 0x50, 0x59, 0x20, 0x20,  /*    (cont.)          */
  0x30, 0x30, 0x31, 0x30                           /* revision level      */
};

static Bit8u bx_floppy_dev_frmt_capacity[] = {
  0x00, 0x00, 0x00,    // reserved
  32,                  // remaining list in bytes
  // current: 1.44meg
    0x00, 0x00, 0x0B, 0x40,  // lba's
    0x02,                    // descriptor code
    0x00, 0x02, 0x00,        // 512-byte sectors
  // allowed #1: 1.44meg
    0x00, 0x00, 0x0B, 0x40,  // lba's
    0x00,                    // descriptor code
    0x00, 0x02, 0x00,        // 512-byte sectors
  // allowed #2: 1.25meg
    0x00, 0x00, 0x04, 0xD0,  // lba's
    0x02,                    // descriptor code
    0x00, 0x04, 0x00,        // 1024-byte sectors
  // allowed #3: 1.28meg
    0x00, 0x00, 0x09, 0x60,  // lba's (2400)
    0x02,                    // descriptor code
    0x00, 0x02, 0x00         // 512-byte sectors
};

static const Bit8u bx_floppy_dev_capacity[] = {
  0x00, 0x00, 0x0B, 0x3f,  // lba's
  0x00, 0x00, 0x02, 0x00   // 512-byte sectors
};

static Bit8u bx_floppy_dev_req_sense[] = {
  0x70,        // valid and error code
  0x00,        // reserved
  0x00,        // sense key
  0x00, 0x00, 0x00, 0x00,  // (lba ?) information
  0x0A,        // additional sense length
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x00,        // additional sense code
  0x00,        // additional sense code qualifier
  0x00, 0x00, 0x00, 0x00   // reserved
};

#define PAGE_CODE_01_OFF   8
#define PAGE_CODE_05_OFF  20
#define PAGE_CODE_1B_OFF  52
#define PAGE_CODE_1C_OFF  64
static Bit8u bx_floppy_dev_mode_sense_cur[] = {
  /////////////////////////////////////////
  // header  (8 bytes)
  0x00, 0x46,   // length of remaining data (72 - 2)
  // medium type code (See page 25 of ufi v1.0 specification)
  // 0x1E,      // (720k)
  // 0x93,      // (1.25meg)
  0x94,         // (1.44meg)
  // WP (bit 7), reserved
  0x00,
  // reserved
  0x00, 0x00, 0x00, 0x00,

  /////////////////////////////////////////
  // Page Code 01 starts here
  //   (12 bytes)
  0x01, // page code 01
  0x0A, // 10 more bytes
  0x05, // error recovery parameters (5 = PER | DCR) (PER = Post Error, DCR = Disable Correction)
  0x03, // read re-try count
  0x00, 0x00, 0x00, 0x00,  // reserved
  0x03, // write re-try count
  0x00, 0x00, 0x00,        // reserved

  /////////////////////////////////////////
  // Page Code 05 starts here
  //   (32 bytes)
  0x05,  // page code 5
  0x1E,  // 30 more bytes
  0x03, 0xE8,  // transfer rate (0x03E8 = 1000d = 1,000 Kbits per second)
  0x02,        // head count (2)
  0x12,        // spt (18)
  0x02, 0x00,  // bytes per sector (512)
  0x00, 0x50,  // cylinder count (80)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // reserved
  0x08,        // motor on delay (10ths of a second)
  0x1E,        // motor off delay (10ths of a second)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // reserved
  0x02, 0x58,  // medium rotation rate (0x0258)
  0x00, 0x00,  // reserved

  /////////////////////////////////////////
  // Page Code 1B starts here
  //   (12 bytes)
  0x1B,  // page code 1B
  0x0A,  // 10 more bytes
  0x80,  // System Floppy = 1, Format Progress = 0
  0x01,  // 1 LUN supported
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // reserved

  /////////////////////////////////////////
  // Page Code 1C starts here (Timer and Protect Page)
  //   (8 bytes)
  0x1C,  // page code 1C
  0x06,  // 6 more bytes
  0x00,  // reserved
  0x05,  // inactivity multiplier
  0x00,  // reserved in UFI
  0x00, 0x00, 0x00  // reserved
};

void usb_floppy_restore_handler(void *dev, bx_list_c *conf);

const char *fdimage_mode_names[] = {
  "flat",
  "vvfat",
  NULL
};

static Bit8u usb_floppy_count = 0;

usb_floppy_device_c::usb_floppy_device_c()
{
  char pname[10];
  char label[32];
  bx_param_string_c *path;
  bx_param_bool_c *readonly;
  bx_param_enum_c *status, *mode;

  d.speed = d.minspeed = d.maxspeed = USB_SPEED_FULL;
  memset((void *) &s, 0, sizeof(s));
  strcpy(d.devname, "BOCHS UFI/CBI FLOPPY");
  d.dev_descriptor = bx_floppy_dev_descriptor;
  d.config_descriptor = bx_floppy_config_descriptor;
  d.device_desc_size = sizeof(bx_floppy_dev_descriptor);
  d.config_desc_size = sizeof(bx_floppy_config_descriptor);
  d.endpoint_info[USB_CONTROL_EP].max_packet_size = 64; // Control ep0
  d.endpoint_info[USB_CONTROL_EP].max_burst_size = 0;
  d.endpoint_info[1].max_packet_size = 64;  // In ep1
  d.endpoint_info[1].max_burst_size = 0;
  d.endpoint_info[2].max_packet_size = 64;  // Out ep2
  d.endpoint_info[2].max_burst_size = 0;
#if USB_FLOPPY_USE_INTERRUPT
  d.endpoint_info[3].max_packet_size = 2;  // In ep3
  d.endpoint_info[3].max_burst_size = 0;
#endif
  s.inserted = 0;
  s.dev_buffer = new Bit8u[USB_FLOPPY_MAX_SECTORS * 512];
  s.statusbar_id = bx_gui->register_statusitem("USB-FD", 1);
  s.floppy_timer_index =
    DEV_register_timer(this, floppy_timer_handler, USB_FLOPPY_SECTOR_TIME, 0, 0, "USB FD timer");
  // config options
  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  sprintf(pname, "floppy%u", ++usb_floppy_count);
  sprintf(label, "USB floppy #%u Configuration", usb_floppy_count);
  s.config = new bx_list_c(usb_rt, pname, label);
  s.config->set_options(bx_list_c::SERIES_ASK | bx_list_c::USE_BOX_TITLE);
  s.config->set_device_param(this);
  path = new bx_param_string_c(s.config, "path", "Path", "", "", BX_PATHNAME_LEN);
  path->set_handler(floppy_path_handler);
  mode = new bx_param_enum_c(s.config,
    "mode",
    "Image mode",
    "Mode of the floppy image",
    fdimage_mode_names, 0, 0);
  mode->set_handler(floppy_param_handler);
  mode->set_ask_format("Enter mode of floppy image, (flat or vvfat): [%s] ");
  readonly = new bx_param_bool_c(s.config,
    "readonly",
    "Write Protection",
    "Floppy media write protection",
    0);
  readonly->set_handler(floppy_param_handler);
  readonly->set_ask_format("Is media write protected? [%s] ");
  status = new bx_param_enum_c(s.config,
    "status",
    "Status",
    "Floppy media status (inserted / ejected)",
    media_status_names,
    BX_INSERTED,
    BX_EJECTED);
  status->set_handler(floppy_param_handler);
  status->set_ask_format("Is the device inserted or ejected? [%s] ");
  if (SIM->is_wx_selected()) {
    bx_list_c *usb = (bx_list_c*)SIM->get_param("ports.usb");
    usb->add(s.config);
  }

  put("usb_floppy", "USBFDD");
}

usb_floppy_device_c::~usb_floppy_device_c(void)
{
  bx_gui->unregister_statusitem(s.statusbar_id);
  set_inserted(0);
  if (s.dev_buffer != NULL)
    delete [] s.dev_buffer;
  free(s.image_mode);
  if (SIM->is_wx_selected()) {
    bx_list_c *usb = (bx_list_c*)SIM->get_param("ports.usb");
    usb->remove(s.config->get_name());
  }
  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  usb_rt->remove(s.config->get_name());
  bx_pc_system.deactivate_timer(s.floppy_timer_index);
  bx_pc_system.unregisterTimer(s.floppy_timer_index);
}

bool usb_floppy_device_c::set_option(const char *option)
{
  char filename[BX_PATHNAME_LEN];
  char *ptr1, *ptr2;

  if (!strncmp(option, "path:", 5)) {
    strcpy(filename, option+5);
    ptr1 = strtok(filename, ":");
    ptr2 = strtok(NULL, ":");
    if ((ptr2 == NULL) || (strlen(ptr1) < 2)) {
      s.image_mode = strdup("flat");
      s.fname = option+5;
    } else {
      s.image_mode = strdup(ptr1);
      s.fname = option+strlen(ptr1)+6;
      if (strcmp(s.image_mode, "flat") && strcmp(s.image_mode, "vvfat")) {
        BX_PANIC(("USB floppy only supports image modes 'flat' and 'vvfat'"));
      }
    }
    SIM->get_param_string("path", s.config)->set(s.fname);
    if (!strcmp(s.image_mode, "vvfat")) {
      SIM->get_param_enum("mode", s.config)->set(1);
    }
    return 1;
  } else if (!strncmp(option, "write_protected:", 16)) {
    SIM->get_param_bool("readonly", s.config)->set(atol(&option[16]));
    return 1;
  } else if (!strncmp(option, "model:", 6)) {
    if (!strcmp(option+6, "teac")) {
      s.model = 1;
    } else {
      s.model = 0;
    }
    return 1;
  }
  return 0;
}

bool usb_floppy_device_c::init()
{
  /*  If you wish to set DEBUG=report in the code, instead of
   *  in the configuration, simply uncomment this line.  I use
   *  it when I am working on this emulation.
   */
  //LOG_THIS setonoff(LOGLEV_DEBUG, ACT_REPORT);

  // set the model information
  //  s.model == 1 if use teac model, else use bochs model
  if (s.model) {
    bx_floppy_dev_descriptor[8] = 0x44;
    bx_floppy_dev_descriptor[9] = 0x06;
    d.vendor_desc = "TEAC    ";
    d.product_desc = "TEAC FD-05PUW   ";
    d.serial_num = "3000";
  } else {
    bx_floppy_dev_descriptor[8] = 0x00;
    bx_floppy_dev_descriptor[9] = 0x00;
    d.vendor_desc = "BOCHS   ";
    d.product_desc = d.devname;
    d.serial_num = "00.10";
  }
  if (set_inserted(1)) {
    sprintf(s.info_txt, "USB floppy: path='%s', mode='%s'", s.fname, s.image_mode);
  } else {
    sprintf(s.info_txt, "USB floppy: media not present");
  }
  d.connected = 1;
  d.alt_iface_max = 0;

  s.did_inquiry_fail = 0;
  s.fail_count = 0;
  s.status_changed = 0;

  return 1;
}

const char *usb_floppy_device_c::get_info()
{
  // set the write protected bit given by parameter in bochsrc.txt file
  bx_floppy_dev_mode_sense_cur[3] &= ~0x80;
  bx_floppy_dev_mode_sense_cur[3] |= s.wp ? (1 << 7) : 0;

  return s.info_txt;
}

void usb_floppy_device_c::register_state_specific(bx_list_c *parent)
{
  bx_list_c *list = new bx_list_c(parent, "s", "UFI/CBI Floppy Disk State");
  bx_list_c *rt_config = new bx_list_c(list, "rt_config");
  rt_config->add(s.config->get_by_name("path"));
  rt_config->add(s.config->get_by_name("readonly"));
  rt_config->add(s.config->get_by_name("status"));
  rt_config->set_restore_handler(this, usb_floppy_restore_handler);
  BXRS_DEC_PARAM_FIELD(list, usb_len, s.usb_len);
  BXRS_DEC_PARAM_FIELD(list, data_len, s.data_len);
  BXRS_DEC_PARAM_FIELD(list, sector, s.sector);
  BXRS_DEC_PARAM_FIELD(list, sector_count, s.sector_count);
  BXRS_DEC_PARAM_FIELD(list, cur_command, s.cur_command);
  BXRS_DEC_PARAM_FIELD(list, cur_track, s.cur_track);
  BXRS_DEC_PARAM_FIELD(list, sense, s.sense);
  BXRS_DEC_PARAM_FIELD(list, asc, s.asc);
  BXRS_DEC_PARAM_FIELD(list, fail_count, s.fail_count);
  BXRS_PARAM_BOOL(list, did_inquiry_fail, s.did_inquiry_fail);
  BXRS_PARAM_BOOL(list, seek_pending, s.seek_pending);
  BXRS_PARAM_SPECIAL32(list, usb_buf, param_save_handler, param_restore_handler);
  new bx_shadow_data_c(list, "dev_buffer", s.dev_buffer, USB_FLOPPY_MAX_SECTORS * 512);
  // TODO: async usb packet
}

void usb_floppy_device_c::handle_reset()
{
  BX_DEBUG(("Reset"));
}

int usb_floppy_device_c::handle_control(int request, int value, int index, int length, Bit8u *data)
{
  int ret;

  ret = handle_control_common(request, value, index, length, data);
  if (ret >= 0) {
    return ret;
  }

  ret = 0;
  switch (request) {
    case DeviceOutRequest | USB_REQ_CLEAR_FEATURE:
      BX_INFO(("USB_REQ_CLEAR_FEATURE: Not handled: %d %d %d %d", request, value, index, length ));
      // It's okay that we don't handle this.  Most likely the guest is just
      //  clearing the toggle bit.  Since we don't worry about the toggle bit (yet?),
      //  we can just ignore and continue.
      ret = 0;
      break;
    case DeviceOutRequest | USB_REQ_SET_FEATURE:
      BX_DEBUG(("USB_REQ_SET_FEATURE:"));
      switch (value) {
        case USB_DEVICE_REMOTE_WAKEUP:
        case USB_DEVICE_U1_ENABLE:
        case USB_DEVICE_U2_ENABLE:
          break;
        default:
          BX_DEBUG(("USB_REQ_SET_FEATURE: Not handled: %d %d %d %d", request, value, index, length ));
          goto fail;
      }
      ret = 0;
      break;
    case DeviceRequest | USB_REQ_GET_DESCRIPTOR:
      switch (value >> 8) {
        case USB_DT_STRING:
          BX_DEBUG(("USB_REQ_GET_DESCRIPTOR: String"));
          switch(value & 0xff) {
            case 0xEE:
              // Microsoft OS Descriptor check
              // We don't support this check, so fail
              BX_INFO(("USB floppy handle_control: Microsoft OS specific 0xEE string descriptor"));
              goto fail;
            default:
              BX_ERROR(("USB floppy handle_control: unknown string descriptor 0x%02x", value & 0xff));
              goto fail;
          }
          break;
        case USB_DT_DEVICE_QUALIFIER:
          BX_DEBUG(("USB_REQ_GET_DESCRIPTOR: Device Qualifier"));
          // device qualifier
          // a low- or full-speed only device (i.e.: a non high-speed device) must return
          //  request error on this function
          BX_ERROR(("USB floppy handle_control: full-speed only device returning stall on Device Qualifier."));
          goto fail;
        default:
          BX_ERROR(("USB floppy handle_control: unknown descriptor type 0x%02x", value >> 8));
          goto fail;
      }
      break;
    case EndpointOutRequest | USB_REQ_CLEAR_FEATURE:
      BX_DEBUG(("USB_REQ_CLEAR_FEATURE:"));
      // Value == 0 == Endpoint Halt (the Guest wants to reset the endpoint)
      if (value == 0) { /* clear ep halt */
#if HANDLE_TOGGLE_CONTROL
        set_toggle(index, 0);
#endif
        ret = 0;
      } else
        goto fail;
      break;
    case DeviceOutRequest | USB_REQ_SET_SEL:
      // Set U1 and U2 System Exit Latency
      BX_DEBUG(("SET_SEL (U1 and U2):"));
      ret = 0;
      break;
    // Class specific requests
    case InterfaceInClassRequest | GetMaxLun:
    case GetMaxLun:
      BX_DEBUG(("MASS STORAGE: GET MAX LUN"));
      data[0] = 0;
      ret = 1;
      break;
    // this is the request where we should receive 12 bytes
    //  of command data.
    case InterfaceOutClassRequest | 0x00:
      if (!handle_command(data))
        goto fail;
      break;

    default:
      BX_ERROR(("USB floppy handle_control: unknown request 0x%04X", request));
    fail:
      BX_ERROR(("USB floppy handle_control: stalled on request: 0x%04X", request));
      d.stall = 1;
      ret = USB_RET_STALL;
  }
  return ret;
}

bool usb_floppy_device_c::handle_command(Bit8u *command)
{
  Bit32u lba, count;
  int pc, pagecode;
  bool ret = 1; // assume valid return

  s.cur_command = command[0];
  s.usb_buf = s.dev_buffer;
  s.usb_len = 0;
  s.data_len = 0;

  // most commands that use the LBA field, use the same place for this field
  lba = ((command[2] << 24) |
         (command[3] << 16) |
         (command[4] <<  8) |
         (command[5] <<  0));
  // then assume the 16-bit count field (32-bit commands will update it first)
  count = ((command[7] << 8) |
           (command[8] << 0));

  // get the LUN from the command
  if (((command[1] >> 5) & 0x07) != 0) {
    BX_ERROR(("Command sent a lun value != 0"));
    return 0;
  }

  // to be consistant with real hardware, we need to fail with
  //  a STALL twice for any command after the first Inquiry except
  //  for the inquiry and request_sense commands.
  //  (I don't know why, and will document further when I know more)
  if ((s.fail_count > 0) &&
      (s.cur_command != UFI_INQUIRY) && (s.cur_command != UFI_REQUEST_SENSE)) {
    BX_DEBUG(("Consistant stall of %d of 2.", 2 - s.fail_count + 1));
    s.fail_count--;
    return 0;
  }

  if (s.cur_command != UFI_REQUEST_SENSE) {
    s.sense = 0;
    s.asc = 0;
  }

  switch (s.cur_command) {
    case UFI_INQUIRY:
      BX_DEBUG(("UFI INQUIRY COMMAND"));
      if (s.model) {
        memcpy(s.usb_buf, bx_floppy_dev_inquiry_teac, sizeof(bx_floppy_dev_inquiry_teac));
        s.usb_len = sizeof(bx_floppy_dev_inquiry_teac);
      } else {
        memcpy(s.usb_buf, bx_floppy_dev_inquiry_bochs, sizeof(bx_floppy_dev_inquiry_bochs));
        s.usb_len = sizeof(bx_floppy_dev_inquiry_bochs);
      }
      s.data_len = command[4];
      if (s.data_len > s.usb_len)
        s.data_len = s.usb_len;
      if (s.did_inquiry_fail == 0) {
        s.fail_count = 2;
        s.did_inquiry_fail = 1;
      }
      break;

    case UFI_READ_FORMAT_CAPACITIES:
      BX_DEBUG(("UFI_READ_FORMAT_CAPACITIES COMMAND"));
      if (s.inserted) {
        bx_floppy_dev_frmt_capacity[3] = 32;
        bx_floppy_dev_frmt_capacity[8] = 0x02;
        memcpy(s.usb_buf, bx_floppy_dev_frmt_capacity, sizeof(bx_floppy_dev_frmt_capacity));
        s.usb_len = sizeof(bx_floppy_dev_frmt_capacity);
      } else {
        bx_floppy_dev_frmt_capacity[3] = 8;
        bx_floppy_dev_frmt_capacity[8] = 0x03;
        memcpy(s.usb_buf, bx_floppy_dev_frmt_capacity, 12);
        s.usb_len = 12;
      }
      s.data_len = (unsigned) ((command[7] << 8) | command[8]);
      if (s.data_len > s.usb_len)
        s.data_len = s.usb_len;
      break;

    case UFI_REQUEST_SENSE:
      BX_DEBUG(("UFI_REQUEST_SENSE COMMAND"));
      bx_floppy_dev_req_sense[2] = s.sense;
      bx_floppy_dev_req_sense[12] = s.asc;
      if (s.sense == 6) {
        s.sense = 0;
      }
      memcpy(s.usb_buf, bx_floppy_dev_req_sense, sizeof(bx_floppy_dev_req_sense));
      s.usb_len = sizeof(bx_floppy_dev_req_sense);
      s.data_len = command[4];
      if (s.data_len > s.usb_len)
        s.data_len = s.usb_len;
      break;

    case UFI_READ_12:
      count = ((command[6] << 24) |
               (command[7] << 16) |
               (command[8] <<  8) |
               (command[9] <<  0));
    case UFI_READ_10:
      BX_DEBUG(("UFI_READ_%d COMMAND (lba = %d, count = %d)", (s.cur_command == UFI_READ_12) ? 12 : 10, lba, count));
      if (!s.inserted) {
        s.sense = 2;
        s.asc = 0x3a;
        break;
      }
      s.sector = lba;
      s.sector_count = count;
      s.data_len = (count * 512);
      s.usb_len = 0;
      if (s.hdimage->lseek(s.sector * 512, SEEK_SET) < 0) {
        BX_ERROR(("could not lseek() floppy drive image file"));
        ret = 0;
      }
      if (d.async_mode) {
        s.seek_pending = 1;
        start_timer(0);
      } else {
        bx_gui->statusbar_setitem(s.statusbar_id, 1); // read
      }
      break;

    case UFI_WRITE_12:
      count = ((command[6] << 24) |
               (command[7] << 16) |
               (command[8] <<  8) |
               (command[9] <<  0));
    case UFI_WRITE_10:
      BX_DEBUG(("UFI_WRITE_%d COMMAND (lba = %d, count = %d)", (s.cur_command == UFI_WRITE_12) ? 12 : 10, lba, count));
      if (!s.inserted) {
        s.sense = 2;
        s.asc = 0x3a;
        break;
      }
      s.sector = lba;
      s.data_len = (count * 512);
      s.usb_len = 0;
      if (s.hdimage->lseek(s.sector * 512, SEEK_SET) < 0) {
        BX_ERROR(("could not lseek() floppy drive image file"));
        ret = 0;
      }
      if (d.async_mode) {
        s.seek_pending = 1;
      }
      break;

    case UFI_READ_CAPACITY:
      BX_DEBUG(("UFI_READ_CAPACITY COMMAND"));
      if (!s.inserted) {
        s.sense = 2;
        s.asc = 0x3a;
        break;
      }
      memcpy(s.usb_buf, bx_floppy_dev_capacity, sizeof(bx_floppy_dev_capacity));
      s.usb_len = sizeof(bx_floppy_dev_capacity);
      s.data_len = sizeof(bx_floppy_dev_capacity);
      break;

    case UFI_TEST_UNIT_READY:
      // This is a zero data command. It simply sets the status bytes for
      //  the interrupt in part of the floppy.
      BX_DEBUG(("UFI_TEST_UNIT_READY COMMAND"));
      if (!s.inserted) {
        s.sense = 2;
        s.asc = 0x3a;
        break;
      }
      break;

    case UFI_PREVENT_ALLOW_REMOVAL:
      BX_DEBUG(("UFI_PREVENT_ALLOW_REMOVAL COMMAND (prevent = %d)", (command[4] & 1) > 0));
      if (command[4] & 1) {
        s.sense = 5;
        s.asc = 0x24;
        break;
      }
      break;

    case UFI_MODE_SENSE:
      pc = command[2] >> 6;
      pagecode = command[2] & 0x3F;
      BX_DEBUG(("UFI_MODE_SENSE COMMAND.  PC = %d, PageCode = %02X", pc, pagecode));
      switch (pc) {
        case 0:  // current values
          switch (pagecode) {
            case 0x01:
              memcpy(s.usb_buf, bx_floppy_dev_mode_sense_cur, 8);  // header first
              memcpy(s.usb_buf + 8, bx_floppy_dev_mode_sense_cur + PAGE_CODE_01_OFF, 12);
              s.usb_len = 8 + 12;
              break;
            case 0x05:
              memcpy(s.usb_buf, bx_floppy_dev_mode_sense_cur, 8);  // header first
              memcpy(s.usb_buf + 8, bx_floppy_dev_mode_sense_cur + PAGE_CODE_05_OFF, 32);
              s.usb_len = 8 + 32;
              break;
            case 0x1B:
              memcpy(s.usb_buf, bx_floppy_dev_mode_sense_cur, 8);  // header first
              memcpy(s.usb_buf + 8, bx_floppy_dev_mode_sense_cur + PAGE_CODE_1B_OFF, 12);
              s.usb_len = 8 + 12;
              break;
            case 0x1C:
              memcpy(s.usb_buf, bx_floppy_dev_mode_sense_cur, 8);  // header first
              memcpy(s.usb_buf + 8, bx_floppy_dev_mode_sense_cur + PAGE_CODE_1C_OFF, 8);
              s.usb_len = 8 + 8;
              break;
            case 0x3F:
              memcpy(s.usb_buf, bx_floppy_dev_mode_sense_cur, 8);  // header first
              memcpy(s.usb_buf + 8, bx_floppy_dev_mode_sense_cur + PAGE_CODE_01_OFF, 64);
              s.usb_len = 8 + 64;
              break;
            default:
              ret = 0;
          }
          break;
        case 1:  // changable values
        case 2:  // default values
        case 3:  // saved values
          ret = 0;
          break;
      }
      s.data_len = (unsigned) ((command[7] << 8) | command[8]);
      if (s.data_len > s.usb_len)
        s.data_len = s.usb_len;
      // set the length of the data returned
      s.usb_buf[0] = 0;
      s.usb_buf[1] = (s.usb_len & 0xFF);
      break;

    case UFI_START_STOP_UNIT:
      BX_DEBUG(("UFI_START_STOP_UNIT COMMAND (start = %d)", command[4] & 1));
      // The UFI specs say that access to the media is allowed
      //  even if the start/stop command is used to stop the device.
      // However, we'll allow the command to return valid return.
      if ((command[4] & 2) != 0) {
        s.sense = 5;
        s.asc = 0x24;
        break;
      }
      break;

    case UFI_FORMAT_UNIT:
      BX_DEBUG(("UFI_FORMAT_UNIT COMMAND (track = %d)", command[2]));
      if (!s.inserted) {
        s.sense = 2;
        s.asc = 0x3a;
        break;
      }
      s.sector = command[2] * 36;
      s.data_len = (unsigned) ((command[7] << 8) | command[8]);
      if (d.async_mode) {
        s.seek_pending = 1;
      }
      break;

    case UFI_REZERO:
    case UFI_SEND_DIAGNOSTIC:
    case UFI_SEEK_10:
    case UFI_WRITE_VERIFY:
    case UFI_VERIFY:
    case UFI_MODE_SELECT:
    default:
      BX_ERROR(("Unknown UFI/CBI Command: 0x%02X", s.cur_command));
      usb_dump_packet(command, 12, 0, d.addr, USB_DIR_OUT | 0, USB_TRANS_TYPE_BULK, false, false);
      ret = 0;
  }

  return ret;
}

int usb_floppy_device_c::handle_data(USBPacket *p)
{
  int ret = 0;
  Bit8u devep = p->devep;
  Bit8u *data = p->data, *tmpbuf;
  int len = p->len, len1;
  Bit32u count, max_sectors;
  
  // check that the length is <= the max packet size of the device
  if (p->len > get_mps(p->devep)) {
    BX_DEBUG(("EP%d transfer length (%d) is greater than Max Packet Size (%d).", p->devep, p->len, get_mps(p->devep)));
  }

  switch (p->pid) {
    case USB_TOKEN_OUT:
      if (devep != 2)
        goto fail;

      BX_DEBUG(("Bulk OUT: %d/%d", len, s.data_len));

      switch (s.cur_command) {
        case UFI_WRITE_10:
        case UFI_WRITE_12:
          if (s.wp)
            goto fail;

          if (len > (int) s.data_len)
            goto fail;
          if (len > 0) {
            memcpy(s.usb_buf+s.usb_len, data, len);
            s.usb_len += len;
            s.data_len -= len;
          }
          ret = len;
          if ((s.data_len == 0) || (s.usb_len >= 512)) {
            if (d.async_mode) {
              start_timer(1);
              BX_DEBUG(("deferring packet %p", p));
              usb_defer_packet(p, this);
              s.packet = p;
              ret = USB_RET_ASYNC;
            } else {
              if (floppy_write_sector() < 0) {
                ret = 0;
                break;
              } else {
                bx_gui->statusbar_setitem(s.statusbar_id, 1, 1); // write
              }
            }
          }
          if (ret > 0) usb_dump_packet(data, len, 0, p->devaddr, USB_DIR_OUT | p->devep, USB_TRANS_TYPE_BULK, false, true);
          break;

        case UFI_FORMAT_UNIT:
          if (s.wp)
            goto fail;

          if (len > (int) s.data_len)
            goto fail;
          BX_DEBUG(("FORMAT UNIT: single track = %d, side = %d", (data[1] >> 4) & 1, data[1] & 1));
          if ((data[1] >> 4) & 1) {
            if (data[1] & 1) {
              s.sector += 18;
            }
            if (s.hdimage->lseek(s.sector * 512, SEEK_SET) < 0) {
              BX_ERROR(("could not lseek() floppy drive image file"));
              break;
            }
            if (d.async_mode) {
              start_timer(2);
              BX_DEBUG(("deferring packet %p", p));
              usb_defer_packet(p, this);
               s.packet = p;
              ret = USB_RET_ASYNC;
            } else {
              bx_gui->statusbar_setitem(s.statusbar_id, 1, 1); // write
              memset(s.dev_buffer, 0xff, 18 * 512);
              if (s.hdimage->write((bx_ptr_t) s.dev_buffer, 18 * 512) < 0) {
                BX_ERROR(("write error"));
                break;
              }
              ret = len;
            }
          } else {
            BX_ERROR(("FORMAT UNIT with no SINGLE TRACK bit set not yet supported"));
          }
          if (ret > 0) usb_dump_packet(data, len, 0, p->devaddr, p->devep, USB_TRANS_TYPE_BULK, false, true);
          break;

        default:
          goto fail;
      }
      break;

    case USB_TOKEN_IN:
      if (devep == 1) { // Bulk In EP
        BX_DEBUG(("Bulk IN: %d/%d", len, s.data_len));

        switch (s.cur_command) {
          case UFI_READ_10:
          case UFI_READ_12:
            if (len > (int) s.data_len)
              len = s.data_len;
            if (d.async_mode) {
              if (len > (int) s.usb_len) {
                BX_DEBUG(("deferring packet %p", p));
                usb_defer_packet(p, this);
                s.packet = p;
                ret = USB_RET_ASYNC;
              } else {
                copy_data(p);
                ret = len;
              }
            } else {
              tmpbuf = data;
              len1 = len;
              while (len1 > 0) {
                if ((int)s.usb_len < len1) {
                  count = s.sector_count;
                  max_sectors = USB_FLOPPY_MAX_SECTORS - (s.usb_len + 511) / 512;
                  if (count > max_sectors) {
                    count = max_sectors;
                  }
                  s.sector_count -= count;
                  ret = (int)s.hdimage->read((bx_ptr_t) s.usb_buf, count * 512);
                  if (ret > 0) {
                    s.usb_len += ret;
                    s.usb_buf += ret;
                  } else {
                    BX_ERROR(("read error"));
                    ret = 0;
                    break;
                  }
                }
                if (len1 <= (int)s.usb_len) {
                  memcpy(tmpbuf, s.dev_buffer, len1);
                  s.data_len -= len1;
                  if (s.data_len > 0) {
                    if ((int)s.usb_len > len1) {
                      s.usb_len -= len1;
                      memmove(s.dev_buffer, s.dev_buffer+len1, s.usb_len);
                      s.usb_buf -= len1;
                    } else {
                      s.usb_len = 0;
                      s.usb_buf = s.dev_buffer;
                    }
                  }
                  len1 = 0;
                }
              }
              if (s.data_len > 0) {
                bx_gui->statusbar_setitem(s.statusbar_id, 1); // read
              }
              ret = len;
            }
            if (ret > 0) usb_dump_packet(data, ret, 0, p->devaddr, USB_DIR_IN | p->devep, USB_TRANS_TYPE_BULK, false, true);
            break;

          case UFI_READ_CAPACITY:
          case UFI_MODE_SENSE:
          case UFI_READ_FORMAT_CAPACITIES:
          case UFI_INQUIRY:
          case UFI_REQUEST_SENSE:
            if (len > (int) s.data_len)
              len = s.data_len;
            memcpy(data, s.usb_buf, len);
            s.usb_buf += len;
            s.data_len -= len;
            usb_dump_packet(data, len, 0, p->devaddr, USB_DIR_IN | p->devep, USB_TRANS_TYPE_BULK, false, true);
            ret = len;
            break;

          case UFI_START_STOP_UNIT:
          case UFI_TEST_UNIT_READY:
          case UFI_PREVENT_ALLOW_REMOVAL:
          case UFI_REZERO:
          case UFI_FORMAT_UNIT:
          case UFI_SEND_DIAGNOSTIC:
          case UFI_SEEK_10:
          case UFI_VERIFY:
          case UFI_MODE_SELECT:
          default:
            goto fail;
        }
#if USB_FLOPPY_USE_INTERRUPT
      } else if (devep == 3) { // Interrupt In EP
        BX_DEBUG(("Interrupt IN: 2 bytes"));
        // We currently do not support error reporting.
        // We currently assume all transfers are successful
        memset(data, 0, 2);
        data[0] = s.asc;
        ret = 2;
#endif
      } else
        goto fail;
      break;

    default:
      BX_ERROR(("USB floppy handle_data: bad token"));
fail:
      d.stall = 1;
      ret = USB_RET_STALL;
      BX_ERROR(("USB floppy handle_data: stalled"));
      break;
  }

  return ret;
}

void usb_floppy_device_c::start_timer(Bit8u mode)
{
  Bit32u delay = USB_FLOPPY_SECTOR_TIME;
  Bit8u new_track, steps;

  if (mode == 2) {
    delay *= 18;
  }
  bx_gui->statusbar_setitem(s.statusbar_id, 1, (mode > 0));
  if (s.seek_pending) {
    new_track = (s.sector / 36);
    steps = abs(new_track - s.cur_track);
    if (steps == 0) steps = 1;
    // FIXME: this is okay for selected data rate 1000 kbps
    delay += (steps * 4000);
    s.cur_track = new_track;
    s.seek_pending = 0;
  }
  bx_pc_system.activate_timer(s.floppy_timer_index, delay, 0);
}

void usb_floppy_device_c::floppy_timer_handler(void *this_ptr)
{
  usb_floppy_device_c *class_ptr = (usb_floppy_device_c *) this_ptr;
  class_ptr->floppy_timer();
}

void usb_floppy_device_c::floppy_timer()
{
  USBPacket *p = s.packet;
  int ret = 1;

  switch (s.cur_command) {
    case UFI_READ_10:
    case UFI_READ_12:
      ret = floppy_read_sector();
      break;
    case UFI_WRITE_10:
    case UFI_WRITE_12:
      ret = floppy_write_sector();
      break;
    case UFI_FORMAT_UNIT:
      memset(s.dev_buffer, 0xff, 18 * 512);
      if (s.hdimage->write((bx_ptr_t) s.dev_buffer, 18 * 512) < 0) {
        BX_ERROR(("write error"));
        ret = -1;
      }
      break;
    default:
      BX_ERROR(("floppy_timer(): unsupported command"));
      ret = -1;
  }
  if (ret < 0) {
    p->len = 0;
  }
  // ret: 0 = not complete / 1 = complete / -1 = error
  if ((s.packet != NULL) && (ret != 0)) {
    usb_dump_packet(p->data, p->len, 0, p->devaddr, USB_DIR_OUT | p->devep, USB_TRANS_TYPE_BULK, false, true);
    s.packet = NULL;
    usb_packet_complete(p);
  }
}

int usb_floppy_device_c::floppy_read_sector()
{
  ssize_t ret;
  USBPacket *p = s.packet;

  BX_DEBUG(("floppy_read_sector(): sector = %d", s.sector));
  if (((USB_FLOPPY_MAX_SECTORS * 512) - s.usb_len) >= 512) {
    ret = s.hdimage->read((bx_ptr_t) s.usb_buf, 512);
    if (ret > 0) {
      s.usb_len += (Bit32u)ret;
      s.usb_buf += ret;
    } else {
      BX_ERROR(("read error"));
      s.usb_len = 0;
    }
  } else {
    BX_ERROR(("buffer overflow"));
    s.usb_len = 0;
  }
  if (s.usb_len > 0) {
    s.sector++;
    s.cur_track = (s.sector / 36);
    if (--s.sector_count > 0) {
      start_timer(0);
    }
    if (s.packet != NULL) {
      if (p->len <= (int)s.usb_len) {
        copy_data(p);
      } else {
        return 0;
      }
    }
    return 1;
  } else {
    return -1;
  }
}

int usb_floppy_device_c::floppy_write_sector()
{
  BX_DEBUG(("floppy_write_sector(): sector = %d", s.sector));
  if (s.hdimage->write((bx_ptr_t) s.usb_buf, 512) < 0) {
    BX_ERROR(("write error"));
    return -1;
  } else {
    s.sector++;
    s.cur_track = (s.sector / 36);
    if (s.usb_len > 512) {
      s.usb_len -= 512;
      memmove(s.usb_buf, s.usb_buf+512, s.usb_len);
    } else {
      s.usb_len = 0;
    }
    return 1;
  }
}

void usb_floppy_device_c::copy_data(USBPacket *p)
{
  int len = p->len;

  memcpy(p->data, s.dev_buffer, len);
  s.data_len -= len;
  if (s.data_len > 0) {
    if ((int)s.usb_len > len) {
      s.usb_len -= len;
      memmove(s.dev_buffer, s.dev_buffer+len, s.usb_len);
      s.usb_buf -= len;
    } else {
      s.usb_len = 0;
      s.usb_buf = s.dev_buffer;
    }
  }
}

void usb_floppy_device_c::cancel_packet(USBPacket *p)
{
  bx_pc_system.deactivate_timer(s.floppy_timer_index);
  s.packet = NULL;
}

bool usb_floppy_device_c::set_inserted(bool value)
{
  s.inserted = value;
  if (value) {
    s.fname = SIM->get_param_string("path", s.config)->getptr();
    if ((strlen(s.fname) > 0) && (strcmp(s.fname, "none"))) {
      s.image_mode = strdup(SIM->get_param_enum("mode", s.config)->get_selected());
      s.hdimage = DEV_hdimage_init_image(s.image_mode, 1474560, "");
      if ((s.hdimage->open(s.fname)) < 0) {
        BX_ERROR(("could not open floppy image file '%s'", s.fname));
        set_inserted(0);
        SIM->get_param_enum("status", s.config)->set(BX_EJECTED);
      } else {
        s.wp = SIM->get_param_bool("readonly", s.config)->get();
        s.sense = 6;
        s.asc = 0x28;
      }
    } else {
      set_inserted(0);
      SIM->get_param_enum("status", s.config)->set(BX_EJECTED);
    }
  } else {
    if (s.hdimage != NULL) {
      s.hdimage->close();
      delete s.hdimage;
      s.hdimage = NULL;
    }
  }
  return s.inserted;
}

void usb_floppy_device_c::runtime_config(void)
{
  if (s.status_changed) {
    set_inserted(0);
    if (SIM->get_param_enum("status", s.config)->get() == BX_INSERTED) {
      set_inserted(1);
    }
    s.status_changed = 0;
  }
}

#undef LOG_THIS
#define LOG_THIS floppy->

// USB floppy runtime parameter handlers
const char *usb_floppy_device_c::floppy_path_handler(bx_param_string_c *param, bool set,
                                                  const char *oldval, const char *val, int maxlen)
{
  usb_floppy_device_c *floppy;

  if (set) {
    if (strlen(val) < 1) {
      val = "none";
    }
    floppy = (usb_floppy_device_c*) param->get_parent()->get_device_param();
    if (floppy != NULL) {
      floppy->s.status_changed = 1;
    } else {
      BX_PANIC(("floppy_path_handler: floppy not found"));
    }
  }
  return val;
}

Bit64s usb_floppy_device_c::floppy_param_handler(bx_param_c *param, bool set, Bit64s val)
{
  usb_floppy_device_c *floppy;

  if (set) {
    floppy = (usb_floppy_device_c*) param->get_parent()->get_device_param();
    if (floppy != NULL) {
      floppy->s.status_changed = 1;
    } else {
      BX_PANIC(("floppy_status_handler: floppy not found"));
    }
  }
  return val;
}

Bit64s usb_floppy_device_c::param_save_handler(void *devptr, bx_param_c *param)
{
  Bit64s val = 0;
  usb_floppy_device_c *floppy = (usb_floppy_device_c*) devptr;

  if (!strcmp(param->get_name(), "usb_buf")) {
    if (floppy->s.usb_buf != NULL) {
      val = (Bit32u)(floppy->s.usb_buf - floppy->s.dev_buffer);
    } else {
      val = 0;
    }
  } else {
    val = 0;
  }
  return val;
}

void usb_floppy_device_c::param_restore_handler(void *devptr, bx_param_c *param, Bit64s val)
{
  usb_floppy_device_c *floppy = (usb_floppy_device_c*) devptr;

  if (!strcmp(param->get_name(), "usb_buf")) {
    floppy->s.usb_buf = floppy->s.dev_buffer + val;
  }
}

void usb_floppy_restore_handler(void *dev, bx_list_c *conf)
{
  ((usb_floppy_device_c*)dev)->restore_handler(conf);
}

void usb_floppy_device_c::restore_handler(bx_list_c *conf)
{
  runtime_config();
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
