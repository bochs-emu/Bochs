/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  UFI/CBI floppy disk storage device support
//
//  Copyright (c) 2015  Benjamin David Lunt
//  Copyright (C) 2015  The Bochs Project
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
#include "usb_cbi.h"

#define LOG_THIS

// maximum size of the read buffer in sectors
#define CBI_MAX_SECTORS   16

// Set this to zero if you only support CB interface.  Set to 1 if you support CBI.
#define USB_CBI_USE_INTERRUPT  1

/* A well known, somewhat older, but still widely used Operating System
 *  must have the Vendor ID as the TEAC external drive emulated below.  If
 *  the VendorID is not the TEAC id, this operating system doesn't know what
 *  to do with the drive.  Go figure.  Therefore, use the optionsX="model=teac"
 *  options in the bochsrc.txt file to set this model.  If you do not, a
 *  default model will be used.
 */

// USB requests
#define GetMaxLun         0xfe

// Full-speed only
static Bit8u bx_cbi_dev_descriptor[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x00, 0x02, /*  u16 bcdUSB; v2.0 */

  0x00,       /*  u8  bDeviceClass; */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; [ low/full speeds only ] */
  0x08,       /*  u8  bMaxPacketSize0; 8 Bytes */

  /* Vendor and product id are arbitrary.  */
  0x00, 0x00, /*  u16 idVendor; */
  0x00, 0x00, /*  u16 idProduct; */
  0x00, 0x00, /*  u16 bcdDevice */

  0x01,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x00,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

// Full-speed only
static const Bit8u bx_cbi_config_descriptor[] = {

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
#if USB_CBI_USE_INTERRUPT
  0x03,       /*  u8  if_bNumEndpoints; (CBI has 3)*/
#else
  0x02,       /*  u8  if_bNumEndpoints; (CB has 2) */
#endif
  0x08,       /*  u8  if_bInterfaceClass; MASS STORAGE */
  0x04,       /*  u8  if_bInterfaceSubClass; CB(I) */
#if USB_CBI_USE_INTERRUPT
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

#if USB_CBI_USE_INTERRUPT
  /* Interrupt endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x83,       /*  u8  ep_bEndpointAddress; IN Endpoint 3 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x02, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x20        /*  u8  ep_bInterval; */
#endif
};

static const Bit8u bx_cbi_dev_inquiry_teac[] = {
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

static const Bit8u bx_cbi_dev_inquiry_bochs[] = {
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

static const Bit8u bx_cbi_dev_frmt_capacity[] = {
  0x00, 0x00, 0x00,    // reserved
  32,                  // remaining list in bytes
  // current: 1.44meg
    0x00, 0x00, 0x0B, 0x3f,  // lba's 
    0x02,                    // descriptor code
    0x00, 0x02, 0x00,        // 512-byte sectors
  // allowed #1: 1.44meg
    0x00, 0x00, 0x0B, 0x3f,  // lba's 
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

static const Bit8u bx_cbi_dev_capacity[] = {
  0x00, 0x00, 0x0B, 0x3f,  // lba's
  0x00, 0x00, 0x02, 0x00   // 512-byte sectors
};

static const Bit8u bx_cbi_dev_req_sense[] = {
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
static Bit8u bx_cbi_dev_mode_sense_cur[] = {
  /////////////////////////////////////////
  // header  (8 bytes)
  0x00, 0x46,   // length of remaining data (72 - 2)
  // medium type code (See page 25 of ufi v1.0 specification)
  // 0x1E,      // (720k) 
  // 0x93,      // (1.25meg)
  0x94,         // (1.44meg)
  // WP (bit 7), reserved
  0x00, // ben
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
  0x02,        // head count
  0x12,        // spt
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

usb_cbi_device_c::usb_cbi_device_c(const char *filename)
{
  char tmpfname[BX_PATHNAME_LEN];
  char *ptr1, *ptr2;

  d.type = USB_DEV_TYPE_FLOPPY;
  d.maxspeed = USB_SPEED_FULL;
  memset((void*)&s, 0, sizeof(s));
  strcpy(d.devname, "BOCHS USB CBI FLOPPY");
  strcpy(tmpfname, filename);
  ptr1 = strtok(tmpfname, ":");
  ptr2 = strtok(NULL, ":");
  if ((ptr2 == NULL) || (strlen(ptr1) < 2)) {
    s.image_mode = BX_HDIMAGE_MODE_FLAT;
    s.fname = filename;
  } else {
    s.image_mode = SIM->hdimage_get_mode(ptr1);
    s.fname = filename+strlen(ptr1)+1;
    if ((s.image_mode != BX_HDIMAGE_MODE_FLAT) &&
        (s.image_mode != BX_HDIMAGE_MODE_VVFAT)) {
      BX_PANIC(("USB floppy only supports image modes 'flat' and 'vvfat'"));
    }
  }

  s.dev_buffer = NULL;
  s.statusbar_id = -1;

  put("usb_cbi", "USBCBI");
}

usb_cbi_device_c::~usb_cbi_device_c(void)
{
  bx_gui->unregister_statusitem(s.statusbar_id);
  if (s.dev_buffer != NULL)
    delete [] s.dev_buffer;
  if (s.hdimage != NULL) {
    s.hdimage->close();
    delete s.hdimage;
  }
}

bx_bool usb_cbi_device_c::set_option(const char *option)
{
  if (!strncmp(option, "write_protected:", 16)) {
    s.wp = atol(&option[16]);
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

bx_bool usb_cbi_device_c::init()
{
  if ((strlen(s.fname) > 0) && (strcmp(s.fname, "none"))) {
    s.hdimage = DEV_hdimage_init_image(s.image_mode, 1474560, "");
    if ((s.hdimage->open(s.fname)) < 0) {
      BX_PANIC(("could not open floppy drive image file '%s'", s.fname));
      return 0;
    }
  } else {
    BX_ERROR(("USB floppy without media not supported yet"));
    return 0;
  }
  sprintf(s.info_txt, "USB CBI: path='%s', mode='%s'", s.fname, hdimage_mode_names[s.image_mode]);
  d.connected = 1;
  s.dev_buffer = new Bit8u[CBI_MAX_SECTORS * 512];

  s.statusbar_id = bx_gui->register_statusitem("USB-FD", 1);
  s.did_inquiry_fail = 0;
  s.fail_count = 0;

  return 1;
}

const char* usb_cbi_device_c::get_info()
{
  // set the write protected bit given by parameter in bochsrc.txt file
  bx_cbi_dev_mode_sense_cur[3] &= ~0x80;
  bx_cbi_dev_mode_sense_cur[3] |= ((s.wp > 0) << 7);

  // set the model information
  //  s.model == 1 if use teac model, else use bochs model
  if (s.model) {
    bx_cbi_dev_descriptor[8] = 0x44;
    bx_cbi_dev_descriptor[9] = 0x06;
  } else {
    bx_cbi_dev_descriptor[8] = 0x00;
    bx_cbi_dev_descriptor[9] = 0x00;
  }

  return s.info_txt;
}

void usb_cbi_device_c::register_state_specific(bx_list_c *parent)
{
  bx_list_c *list = new bx_list_c(parent, "s", "UFI/CBI Floppy Disk State");
  new bx_shadow_num_c(list, "usb_len", &s.usb_len);
  new bx_shadow_num_c(list, "data_len", &s.data_len);
  new bx_shadow_num_c(list, "sector_count", &s.sector_count);
  new bx_shadow_num_c(list, "cur_command", &s.cur_command);
  new bx_shadow_num_c(list, "fail_count", &s.fail_count);
  new bx_shadow_bool_c(list, "did_inquiry_fail", &s.did_inquiry_fail);
  // TODO
}

void usb_cbi_device_c::handle_reset()
{
  BX_DEBUG(("Reset"));
}

int usb_cbi_device_c::handle_control(int request, int value, int index, int length, Bit8u *data)
{
  int ret = 0;

  switch (request) {
    case DeviceRequest | USB_REQ_GET_STATUS:
    case EndpointRequest | USB_REQ_GET_STATUS:
      BX_DEBUG(("USB_REQ_GET_STATUS:"));
      data[0] = (1 << USB_DEVICE_SELF_POWERED) |
        (d.remote_wakeup << USB_DEVICE_REMOTE_WAKEUP);
      data[1] = 0x00;
      ret = 2;
      break;
    case DeviceOutRequest | USB_REQ_CLEAR_FEATURE:
      BX_DEBUG(("USB_REQ_CLEAR_FEATURE:"));
      if (value == USB_DEVICE_REMOTE_WAKEUP) {
        d.remote_wakeup = 0;
      } else {
        BX_INFO(("USB_REQ_CLEAR_FEATURE: Not handled: %i %i %i %i", request, value, index, length ));
        // It's okay that we don't handle this.  Most likely the guest is just
        //  clearing the toggle bit.  Since we don't worry about the toggle bit (yet?),
        //  we can just ignore and continue.
        //goto fail;
      }
      ret = 0;
      break;
    case DeviceOutRequest | USB_REQ_SET_FEATURE:
      BX_DEBUG(("USB_REQ_SET_FEATURE:"));
      switch (value) {
        case USB_DEVICE_REMOTE_WAKEUP:
          d.remote_wakeup = 1;
          break;
        case USB_DEVICE_U1_ENABLE:
        case USB_DEVICE_U2_ENABLE:
          break;
        default:
        BX_DEBUG(("USB_REQ_SET_FEATURE: Not handled: %i %i %i %i", request, value, index, length ));
        goto fail;
      }
      ret = 0;
      break;
    case DeviceOutRequest | USB_REQ_SET_ADDRESS:
      BX_DEBUG(("USB_REQ_SET_ADDRESS:"));
      d.addr = value;
      ret = 0;
      break;
    case DeviceRequest | USB_REQ_GET_DESCRIPTOR:
      switch (value >> 8) {
        case USB_DT_DEVICE:
          BX_DEBUG(("USB_REQ_GET_DESCRIPTOR: Device"));
          memcpy(data, bx_cbi_dev_descriptor, sizeof(bx_cbi_dev_descriptor));
          ret = sizeof(bx_cbi_dev_descriptor);
          break;
        case USB_DT_CONFIG:
          BX_DEBUG(("USB_REQ_GET_DESCRIPTOR: Config"));
          memcpy(data, bx_cbi_config_descriptor, sizeof(bx_cbi_config_descriptor));
          ret = sizeof(bx_cbi_config_descriptor);
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
              if (s.model)
                ret = set_usb_string(data, "TEAC    ");
              else
                ret = set_usb_string(data, "BOCHS   ");
              break;
            case 2:
              // product description
              if (s.model)
                ret = set_usb_string(data, "TEAC FD-05PUW   ");
              else
                ret = set_usb_string(data, d.devname);
              break;
            case 3:
              // serial number
              if (s.model)
                ret = set_usb_string(data, "3000");
              else
                ret = set_usb_string(data, "00.10");
              break;
            case 0xEE:
              // Microsoft OS Descriptor check
              // We don't support this check, so fail
              goto fail;
            default:
              BX_ERROR(("USB CBI handle_control: unknown string descriptor 0x%02x", value & 0xff));
              goto fail;
          }
          break;
        case USB_DT_DEVICE_QUALIFIER:
          BX_DEBUG(("USB_REQ_GET_DESCRIPTOR: Device Qualifier"));
          // device qualifier
          // a low- or full-speed only device (i.e.: a non high-speed device) must return
          //  request error on this function
          BX_ERROR(("USB CBI handle_control: full-speed only device returning stall on Device Qualifier."));
          goto fail;
        default:
          BX_ERROR(("USB CBI handle_control: unknown descriptor type 0x%02x", value >> 8));
          goto fail;
      }
      break;
    case DeviceRequest | USB_REQ_GET_CONFIGURATION:
      BX_DEBUG(("USB_REQ_GET_CONFIGURATION:"));
      data[0] = 1;
      ret = 1;
      break;
    case DeviceOutRequest | USB_REQ_SET_CONFIGURATION:
      BX_DEBUG(("USB_REQ_SET_CONFIGURATION:"));
      ret = 0;
      break;
    case DeviceRequest | USB_REQ_GET_INTERFACE:
      BX_DEBUG(("USB_REQ_GET_INFTERFACE:"));
      data[0] = 0;
      ret = 1;
      break;
    case DeviceOutRequest | USB_REQ_SET_INTERFACE:
    case InterfaceOutRequest | USB_REQ_SET_INTERFACE:
      BX_DEBUG(("USB_REQ_SET_INFTERFACE:"));
      ret = 0;
      break;
    case EndpointOutRequest | USB_REQ_CLEAR_FEATURE:
      BX_DEBUG(("USB_REQ_CLEAR_FEATURE:"));
      // It's okay that we don't handle this.  Most likely the guest is just
      //  clearing the toggle bit.  Since we don't worry about the toggle bit (yet?),
      //  we can just ignore and continue.
      //if (value == 0 && index != 0x81) { /* clear ep halt */
      //  goto fail;
      //}
      ret = 0;
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
    case InterfaceOutClassRequest:
      if (!handle_command(data))
        goto fail;
      break;

    default:
      BX_ERROR(("USB CBI handle_control: unknown request 0x%04X", request));
    fail:
      BX_ERROR(("USB CBI handle_control: stalled on request: 0x%04X", request));
      d.stall = 1;
      ret = USB_RET_STALL;
  }
  return ret;
}

bx_bool usb_cbi_device_c::handle_command(Bit8u *command)
{
  Bit32u lba, count;
  int pc, pagecode;
  bx_bool ret = 1; // assume valid return

  s.cur_command = command[0];
  s.usb_buf = s.dev_buffer;
  s.sector_count = 0;

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
    s.fail_count--;
    return 0;
  }

  switch (s.cur_command) {
    case UFI_INQUIRY:
      BX_DEBUG(("UFI INQUIRY COMMAND"));
      if (s.model) {
        memcpy(s.usb_buf, bx_cbi_dev_inquiry_teac, sizeof(bx_cbi_dev_inquiry_teac));
        s.usb_len = sizeof(bx_cbi_dev_inquiry_teac);
      } else {
        memcpy(s.usb_buf, bx_cbi_dev_inquiry_bochs, sizeof(bx_cbi_dev_inquiry_bochs));
        s.usb_len = sizeof(bx_cbi_dev_inquiry_bochs);
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
      memcpy(s.usb_buf, bx_cbi_dev_frmt_capacity, sizeof(bx_cbi_dev_frmt_capacity));
      s.usb_len = sizeof(bx_cbi_dev_frmt_capacity);
      s.data_len = (unsigned) ((command[7] << 8) | command[8]);
      if (s.data_len > s.usb_len)
        s.data_len = s.usb_len;
      break;

    case UFI_REQUEST_SENSE:
      BX_DEBUG(("UFI_REQUEST_SENSE COMMAND"));
      memcpy(s.usb_buf, bx_cbi_dev_req_sense, sizeof(bx_cbi_dev_req_sense));
      s.usb_len = sizeof(bx_cbi_dev_req_sense);
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
      BX_DEBUG(("UFI_READ%i COMMAND (lba = %i, count = %i)", (s.cur_command == UFI_READ_12) ? 12 : 10, lba, count));
      if (count > CBI_MAX_SECTORS) {
        s.sector_count = count - CBI_MAX_SECTORS;
        count = CBI_MAX_SECTORS;
      }
      if (s.hdimage->lseek(lba * 512, SEEK_SET) < 0) {
        BX_ERROR(("could not lseek() floppy drive image file"));
        s.usb_len = 0;
      } else {
        s.usb_len = (unsigned) s.hdimage->read((bx_ptr_t) s.usb_buf, count * 512);
      }
      s.data_len = s.usb_len;
      break;

    case UFI_WRITE_12:
      count = ((command[6] << 24) |
               (command[7] << 16) |
               (command[8] <<  8) |
               (command[9] <<  0));
    case UFI_WRITE_10:
      BX_DEBUG(("UFI_WRITE%i COMMAND (lba = %i, count = %i)", (s.cur_command == UFI_WRITE_12) ? 12 : 10, lba, count));
      if (s.hdimage->lseek(lba * 512, SEEK_SET) < 0)
        BX_ERROR(("could not lseek() floppy drive image file"));
      s.data_len = (count * 512);
      s.usb_len = 0;
      break;

    case UFI_READ_CAPACITY:
      BX_DEBUG(("UFI_READ_CAPACITY COMMAND"));
      memcpy(s.usb_buf, bx_cbi_dev_capacity, sizeof(bx_cbi_dev_capacity));
      s.usb_len = sizeof(bx_cbi_dev_capacity);
      s.data_len = sizeof(bx_cbi_dev_capacity);
      break;

    case UFI_TEST_UNIT_READY:
      // This is a zero data command. It simply sets the status bytes for
      //  the interrupt in part of the CBI
      BX_DEBUG(("UFI_TEST_UNIT_READY COMMAND"));
      s.usb_len = 0;
      s.data_len = 0;
      break;

    case UFI_PREVENT_ALLOW_REMOVAL:
      BX_DEBUG(("UFI_PREVENT_ALLOW_REMOVAL COMMAND (prevent = %i)", (command[4] & 1) > 0));
      s.usb_len = 0;
      s.data_len = 0;
      break;

    case UFI_MODE_SENSE:
      pc = command[2] >> 6;
      pagecode = command[2] & 0x3F;
      BX_DEBUG(("UFI_MODE_SENSE COMMAND.  PC = %i, PageCode = %02X", pc, pagecode));
      switch (pc) {
        case 0:  // current values
          switch (pagecode) {
            case 0x01:
              memcpy(s.usb_buf, bx_cbi_dev_mode_sense_cur, 8);  // header first
              memcpy(s.usb_buf + 8, bx_cbi_dev_mode_sense_cur + PAGE_CODE_01_OFF, 12);
              s.usb_len = 8 + 12;
              break;
            case 0x05:
              memcpy(s.usb_buf, bx_cbi_dev_mode_sense_cur, 8);  // header first
              memcpy(s.usb_buf + 8, bx_cbi_dev_mode_sense_cur + PAGE_CODE_05_OFF, 32);
              s.usb_len = 8 + 32;
              break;
            case 0x1B:
              memcpy(s.usb_buf, bx_cbi_dev_mode_sense_cur, 8);  // header first
              memcpy(s.usb_buf + 8, bx_cbi_dev_mode_sense_cur + PAGE_CODE_1B_OFF, 12);
              s.usb_len = 8 + 12;
              break;
            case 0x1C:
              memcpy(s.usb_buf, bx_cbi_dev_mode_sense_cur, 8);  // header first
              memcpy(s.usb_buf + 8, bx_cbi_dev_mode_sense_cur + PAGE_CODE_1C_OFF, 8);
              s.usb_len = 8 + 8;
              break;
            case 0x3F:
              memcpy(s.usb_buf, bx_cbi_dev_mode_sense_cur, 8);  // header first
              memcpy(s.usb_buf + 8, bx_cbi_dev_mode_sense_cur + PAGE_CODE_01_OFF, 64);
              s.usb_len = 8 + 64;
              break;
            default:
              ret = 0;
          }
          break;
        case 1:  // changable values
        case 2:  // default values
        case 3:  // saved values
          s.usb_len = 0;
          s.data_len = 0;
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
      // The UFI specs say that access to the media is allowed
      //  even if the start/stop command is used to stop the device.
      // However, we'll allow the command to return valid return.
      break;

    case UFI_REZERO:
    case UFI_FORMAT_UNIT:
    case UFI_SEND_DIAGNOSTIC:
    case UFI_SEEK_10:
    case UFI_WRITE_VERIFY:
    case UFI_VERIFY:
    case UFI_MODE_SELECT:
    default:
      BX_ERROR(("Unknown UFI/CBI Command: 0x%02X", s.cur_command));
      usb_dump_packet(command, 12);
      ret = 0;
  }

  return ret;
}

int usb_cbi_device_c::handle_data(USBPacket *p)
{
  int ret = 0;
  Bit8u devep = p->devep;
  Bit8u *data = p->data;
  int len = p->len;
  Bit32u count;

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

          bx_gui->statusbar_setitem(s.statusbar_id, 1, 1);     // write
          if (len > (int) s.data_len)
            goto fail;
          if (len > 0) {
            memcpy(s.usb_buf+s.usb_len, data, len);
            s.usb_len += len;
            s.data_len -= len;
          }
          if ((s.data_len == 0) || (s.usb_len >= 512)) {
            s.hdimage->write((bx_ptr_t) s.usb_buf, 512);
            s.usb_len = 0;
          }
          ret = len;

          if (ret > 0) usb_dump_packet(data, ret);
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
            bx_gui->statusbar_setitem(s.statusbar_id, 1);     // read
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
            ret = len;
            if ((s.data_len == 0) && (s.sector_count > 0)) {
              count = s.sector_count;
              if (count > CBI_MAX_SECTORS) {
                s.sector_count = count - CBI_MAX_SECTORS;
                count = CBI_MAX_SECTORS;
              } else {
                s.sector_count = 0;
              }
              s.usb_buf = s.dev_buffer;
              s.usb_len = (unsigned) s.hdimage->read((bx_ptr_t) s.usb_buf, count * 512);
              s.data_len = s.usb_len;
            }

            if (ret > 0) usb_dump_packet(data, ret);
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
#if USB_CBI_USE_INTERRUPT
      } else if (devep == 3) { // Interrupt In EP
        BX_DEBUG(("Interrupt IN: 2 bytes"));
        // We currently do not support error reporting.
        // We currently assume all transfers are successful
        memset(data, 0, 2);
        ret = 2;
#endif
      } else
        goto fail;
      break;

    default:
      BX_ERROR(("USB CBI handle_data: bad token"));
fail:
      d.stall = 1;
      ret = USB_RET_STALL;
      BX_ERROR(("USB CBI handle_data: stalled"));
      break;
  }

  return ret;
}

void usb_cbi_device_c::cancel_packet(USBPacket *p)
{
  s.packet = NULL;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
