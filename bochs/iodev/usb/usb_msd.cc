/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  USB mass storage device support (ported from QEMU)
//
//  Copyright (c) 2006 CodeSourcery.
//  Written by Paul Brook
//  Copyright (C) 2009-2020  The Bochs Project
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
#include "hdimage/cdrom.h"
#include "hdimage/hdimage.h"
#include "scsi_device.h"
#include "usb_msd.h"

#define LOG_THIS

// USB device plugin entry points

int CDECL libusb_msd_dev_plugin_init(plugin_t *plugin, plugintype_t type)
{
  return 0; // Success
}

void CDECL libusb_msd_dev_plugin_fini(void)
{
  // Nothing here yet
}

//
// Define the static class that registers the derived USB device class,
// and allocates one on request.
//
class bx_usb_msd_locator_c : public usbdev_locator_c {
public:
  bx_usb_msd_locator_c(void) : usbdev_locator_c("usb_msd") {}
protected:
  usb_device_c *allocate(usbdev_type devtype, const char *args) {
    return (new usb_msd_device_c(devtype, args));
  }
} bx_usb_msd_match;

enum USBMSDMode {
  USB_MSDM_CBW,
  USB_MSDM_DATAOUT,
  USB_MSDM_DATAIN,
  USB_MSDM_CSW
};

struct usb_msd_cbw {
  Bit32u sig;
  Bit32u tag;
  Bit32u data_len;
  Bit8u flags;
  Bit8u lun;
  Bit8u cmd_len;
  Bit8u cmd[16];
};

struct usb_msd_csw {
  Bit32u sig;
  Bit32u tag;
  Bit32u residue;
  Bit8u status;
};

// USB requests
#define MassStorageReset  0xff
#define GetMaxLun         0xfe

// Full-speed
static const Bit8u bx_msd_dev_descriptor[] = {
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
  0x00, 0x01, /*  u16 bcdDevice */

  0x01,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x03,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

// Full-speed
static const Bit8u bx_msd_config_descriptor[] = {

  /* one configuration */
  0x09,       /*  u8  bLength; */
  0x02,       /*  u8  bDescriptorType; Configuration */
  0x20, 0x00, /*  u16 wTotalLength; */
  0x01,       /*  u8  bNumInterfaces; (1) */
  0x01,       /*  u8  bConfigurationValue; */
  0x00,       /*  u8  iConfiguration; */
  0xc0,       /*  u8  bmAttributes;
                        Bit 7: must be set,
                            6: Self-powered,
                            5: Remote wakeup,
                            4..0: resvd */
  0x00,       /*  u8  MaxPower; */

  /* one interface */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x02,       /*  u8  if_bNumEndpoints; */
  0x08,       /*  u8  if_bInterfaceClass; MASS STORAGE */
  0x06,       /*  u8  if_bInterfaceSubClass; SCSI */
  0x50,       /*  u8  if_bInterfaceProtocol; Bulk Only */
  0x00,       /*  u8  if_iInterface; */

  /* Bulk-In endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x02,       /*  u8  ep_bmAttributes; Bulk */
  0x40, 0x00, /*  u16 ep_wMaxPacketSize; 64 */
  0x00,       /*  u8  ep_bInterval; */

  /* Bulk-Out endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x02,       /*  u8  ep_bEndpointAddress; OUT Endpoint 2 */
  0x02,       /*  u8  ep_bmAttributes; Bulk */
  0x40, 0x00, /*  u16 ep_wMaxPacketSize; 64 */
  0x00        /*  u8  ep_bInterval; */
};

// High-speed
static const Bit8u bx_msd_dev_descriptor2[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x00, 0x02, /*  u16 bcdUSB; v2.0 */

  0x00,       /*  u8  bDeviceClass; */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; [ low/full speeds only ] */
  0x40,       /*  u8  bMaxPacketSize0; 64 Bytes */

  /* Vendor and product id are arbitrary.  */
  0x00, 0x00, /*  u16 idVendor; */
  0x00, 0x00, /*  u16 idProduct; */
  0x00, 0x01, /*  u16 bcdDevice */

  0x01,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x03,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

// High-speed
static const Bit8u bx_msd_config_descriptor2[] = {

  /* one configuration */
  0x09,       /*  u8  bLength; */
  0x02,       /*  u8  bDescriptorType; Configuration */
  0x20, 0x00, /*  u16 wTotalLength; */
  0x01,       /*  u8  bNumInterfaces; (1) */
  0x01,       /*  u8  bConfigurationValue; */
  0x00,       /*  u8  iConfiguration; */
  0xc0,       /*  u8  bmAttributes;
                        Bit 7: must be set,
                            6: Self-powered,
                            5: Remote wakeup,
                            4..0: resvd */
  0x00,       /*  u8  MaxPower; */

  /* one interface */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x02,       /*  u8  if_bNumEndpoints; */
  0x08,       /*  u8  if_bInterfaceClass; MASS STORAGE */
  0x06,       /*  u8  if_bInterfaceSubClass; SCSI */
  0x50,       /*  u8  if_bInterfaceProtocol; Bulk Only */
  0x00,       /*  u8  if_iInterface; */

  /* Bulk-In endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x02,       /*  u8  ep_bmAttributes; Bulk */
  0x00, 0x02, /*  u16 ep_wMaxPacketSize; 512 */
  0x00,       /*  u8  ep_bInterval; */

  /* Bulk-Out endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x02,       /*  u8  ep_bEndpointAddress; OUT Endpoint 2 */
  0x02,       /*  u8  ep_bmAttributes; Bulk */
  0x00, 0x02, /*  u16 ep_wMaxPacketSize; 512 */
  0x00        /*  u8  ep_bInterval; */
};

// Super-speed
static const Bit8u bx_msd_dev_descriptor3[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x00, 0x03, /*  u16 bcdUSB; v3.0 */

  0x00,       /*  u8  bDeviceClass; */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; */
  0x09,       /*  u8  bMaxPacketSize0; 2^^9 = 512 Bytes */

  /* Vendor and product id are arbitrary.  */
  0x00, 0x00, /*  u16 idVendor; */
  0x00, 0x00, /*  u16 idProduct; */
  0x00, 0x01, /*  u16 bcdDevice */

  0x01,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x03,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

static const Bit8u bx_msd_config_descriptor3[] = {
  /* one configuration */
  0x09,       /*  u8  bLength; */
  0x02,       /*  u8  bDescriptorType; Configuration */
  0x2C, 0x00, /*  u16 wTotalLength; */
  0x01,       /*  u8  bNumInterfaces; (1) */
  0x01,       /*  u8  bConfigurationValue; */
  0x00,       /*  u8  iConfiguration; */
  0x80,       /*  u8  bmAttributes;
                        Bit 7: must be set,
                            6: Self-powered,
                            5: Remote wakeup,
                            4..0: resvd */
  0x3F,       /*  u8  MaxPower; */

  /* one interface */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x02,       /*  u8  if_bNumEndpoints; */
  0x08,       /*  u8  if_bInterfaceClass; MASS STORAGE */
  0x06,       /*  u8  if_bInterfaceSubClass; SCSI */
  0x50,       /*  u8  if_bInterfaceProtocol; Bulk Only */
  0x00,       /*  u8  if_iInterface; */

  /* Bulk-In endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x02,       /*  u8  ep_bmAttributes; Bulk */
  0x00, 0x04, /*  u16 ep_wMaxPacketSize; 1024 */
  0x00,       /*  u8  ep_bInterval; */

  /* Bulk-In companion descriptor */
  0x06,       /*  u8  epc_bLength; */
  0x30,       /*  u8  epc_bDescriptorType; Endpoint Companion */
  0x0F,       /*  u8  epc_bMaxPerBurst; */
  0x00,       /*  u8  epc_bmAttributes; */
  0x00, 0x00, /*  u16 epc_reserved; */

  /* Bulk-Out endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x02,       /*  u8  ep_bEndpointAddress; OUT Endpoint 2 */
  0x02,       /*  u8  ep_bmAttributes; Bulk */
  0x00, 0x04, /*  u16 ep_wMaxPacketSize; 1024 */
  0x00,       /*  u8  ep_bInterval; */

  /* Bulk-Out companion descriptor */
  0x06,       /*  u8  epc_bLength; */
  0x30,       /*  u8  epc_bDescriptorType; Endpoint Companion */
  0x0F,       /*  u8  epc_bMaxPerBurst; */
  0x00,       /*  u8  epc_bmAttributes; */
  0x00, 0x00 /*  u16  epc_reserved; */
};

// BOS Descriptor
static const Bit8u bx_msd_bos_descriptor3[] = {
  /* stub */
  0x05,       /*  u8  bos_bLength; */
  0x0F,       /*  u8  bos_bDescriptorType; BOS */
  0x16, 0x00, /* u16  bos_wTotalLength; */
  0x02,       /*  u8  bos_bNumCapEntries; BOS */

  /* USB 2.0 Extention */
  0x07,       /*  u8  bss_bLength; */
  0x10,       /*  u8  bss_bType; Device Cap */
  0x02,       /*  u8  bss_bCapType; USB 2.0 Ext */
  0x02, 0x00, /* u32  bss_bmAttributes; */
  0x00, 0x00,

  /* USB 3.0 */
  0x0A,       /*  u8  bss_bLength; */
  0x10,       /*  u8  bss_bType; Device Cap */
  0x03,       /*  u8  bss_bCapType; USB 3.0 */
  0x00,       /*  u8  bss_bmAttributes; */
  0x0E, 0x00, /* u16  bss_bmSupSpeeds; */
  0x01,       /*  u8  bss_bSupFunct; */
  0x0A,       /*  u8  bss_bU1DevExitLat; */
  0x20, 0x00  /* u16  bss_wU2DevExitLat; */
};

void usb_msd_restore_handler(void *dev, bx_list_c *conf);

static Bit8u usb_cdrom_count = 0;

usb_msd_device_c::usb_msd_device_c(usbdev_type type, const char *filename)
{
  char pname[10];
  char label[32];
  char tmpfname[BX_PATHNAME_LEN];
  char *ptr1, *ptr2;
  bx_param_string_c *path;
  bx_param_enum_c *status;

  d.type = type;
  d.minspeed = USB_SPEED_FULL;
  d.maxspeed = USB_SPEED_SUPER;
  d.speed = d.minspeed;
  memset((void*)&s, 0, sizeof(s));
  if (d.type == USB_DEV_TYPE_DISK) {
    strcpy(d.devname, "BOCHS USB HARDDRIVE");
    strcpy(tmpfname, filename);
    ptr1 = strtok(tmpfname, ":");
    ptr2 = strtok(NULL, ":");
    if ((ptr2 == NULL) || (strlen(ptr1) < 2)) {
      s.image_mode = BX_HDIMAGE_MODE_FLAT;
      strcpy(s.fname, filename);
    } else {
      s.image_mode = SIM->hdimage_get_mode(ptr1);
      strcpy(s.fname, filename+strlen(ptr1)+1);
    }
    s.journal[0] = 0;
    s.size = 0;
    s.sect_size = 512;
  } else if (d.type == USB_DEV_TYPE_CDROM) {
    strcpy(d.devname, "BOCHS USB CDROM");
    strcpy(s.fname, filename);
    // config options
    bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
    sprintf(pname, "cdrom%u", ++usb_cdrom_count);
    sprintf(label, "USB CD-ROM #%u Configuration", usb_cdrom_count);
    s.config = new bx_list_c(usb_rt, pname, label);
    s.config->set_options(bx_list_c::SERIES_ASK | bx_list_c::USE_BOX_TITLE);
    s.config->set_device_param(this);
    path = new bx_param_string_c(s.config, "path", "Path", "", "", BX_PATHNAME_LEN);
    path->set(s.fname);
    path->set_handler(cdrom_path_handler);
    status = new bx_param_enum_c(s.config,
      "status",
      "Status",
      "CD-ROM media status (inserted / ejected)",
      media_status_names,
      BX_INSERTED,
      BX_EJECTED);
    status->set_handler(cdrom_status_handler);
    status->set_ask_format("Is the device inserted or ejected? [%s] ");
    if (SIM->is_wx_selected()) {
      bx_list_c *usb = (bx_list_c*)SIM->get_param("ports.usb");
      usb->add(s.config);
    }
  }
  d.vendor_desc = "BOCHS";
  d.product_desc = d.devname;

  put("usb_msd", "USBMSD");
}

usb_msd_device_c::~usb_msd_device_c(void)
{
  d.sr->clear();
  if (s.scsi_dev != NULL)
    delete s.scsi_dev;
  if (s.hdimage != NULL) {
    s.hdimage->close();
    delete s.hdimage;
  } else if (s.cdrom != NULL) {
    delete s.cdrom;
    if (SIM->is_wx_selected()) {
      bx_list_c *usb = (bx_list_c*)SIM->get_param("ports.usb");
      usb->remove(s.config->get_name());
    }
    bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
    usb_rt->remove(s.config->get_name());
  }
}

bx_bool usb_msd_device_c::set_option(const char *option)
{
  char *suffix;

  if (!strncmp(option, "journal:", 8)) {
    if (d.type == USB_DEV_TYPE_DISK) {
      strcpy(s.journal, option+8);
      return 1;
    } else {
      BX_ERROR(("Option 'journal' is only valid for USB disks"));
    }
  } else if (!strncmp(option, "size:", 5)) {
    if ((d.type == USB_DEV_TYPE_DISK) && (s.image_mode == BX_HDIMAGE_MODE_VVFAT)) {
      s.size = (int)strtol(option+5, &suffix, 10);
      if (!strcmp(suffix, "G")) {
        s.size <<= 10;
      } else if (strcmp(suffix, "M")) {
        BX_ERROR(("Unknown VVFAT disk size suffix '%s' - using default", suffix));
        s.size = 0;
        return 0;
      }
      if ((s.size < 128) || (s.size >= 131072)) {
        BX_ERROR(("Invalid VVFAT disk size value - using default"));
        s.size = 0;
        return 0;
      }
      return 1;
    } else {
      BX_ERROR(("Option 'size' is only valid for USB VVFAT disks"));
    }
  } else if (!strncmp(option, "sect_size:", 10)) {
    if (d.type == USB_DEV_TYPE_DISK) {
      s.sect_size = (unsigned)strtol(option+10, &suffix, 10);
      if (strlen(suffix) > 0) {
        BX_ERROR(("Option 'sect_size': ignoring extra data"));
      }
      if ((s.sect_size != 512) && (s.sect_size != 1024) && (s.sect_size != 4096)) {
        BX_ERROR(("Option 'sect_size': invalid value, using default"));
        s.sect_size = 512;
      }
      return 1;
    } else {
      BX_ERROR(("Option 'sect_size' is only valid for USB disks"));
    }
  }
  return 0;
}

bx_bool usb_msd_device_c::init()
{
  if (d.type == USB_DEV_TYPE_DISK) {
    s.hdimage = DEV_hdimage_init_image(s.image_mode, 0, s.journal);
    if (s.image_mode == BX_HDIMAGE_MODE_VVFAT) {
      Bit64u hdsize = ((Bit64u)s.size) << 20;
      s.hdimage->cylinders = (unsigned)(hdsize/16.0/63.0/512.0);
      s.hdimage->heads = 16;
      s.hdimage->spt = 63;
      s.hdimage->sect_size = 512;
    } else {
      s.hdimage->sect_size = s.sect_size;
    }
    if (s.hdimage->open(s.fname) < 0) {
      BX_ERROR(("could not open hard drive image file '%s'", s.fname));
      return 0;
    } else {
      s.scsi_dev = new scsi_device_t(s.hdimage, 0, usb_msd_command_complete, (void*)this);
    }
    sprintf(s.info_txt, "USB HD: path='%s', mode='%s', sect_size=%d", s.fname,
            hdimage_mode_names[s.image_mode], s.hdimage->sect_size);
  } else if (d.type == USB_DEV_TYPE_CDROM) {
    s.cdrom = DEV_hdimage_init_cdrom(s.fname);
    s.scsi_dev = new scsi_device_t(s.cdrom, 0, usb_msd_command_complete, (void*)this);
    if (set_inserted(1)) {
      sprintf(s.info_txt, "USB CD: path='%s'", s.fname);
    } else {
      sprintf(s.info_txt, "USB CD: media not present");
    }
  }
  s.scsi_dev->register_state(s.sr_list, "scsidev");
  if (getonoff(LOGLEV_DEBUG) == ACT_REPORT) {
    s.scsi_dev->set_debug_mode();
  }
  if (get_speed() == USB_SPEED_SUPER) {
    d.dev_descriptor = bx_msd_dev_descriptor3;
    d.config_descriptor = bx_msd_config_descriptor3;
    d.device_desc_size = sizeof(bx_msd_dev_descriptor3);
    d.config_desc_size = sizeof(bx_msd_config_descriptor3);
  } else if (get_speed() == USB_SPEED_HIGH) {
    d.dev_descriptor = bx_msd_dev_descriptor2;
    d.config_descriptor = bx_msd_config_descriptor2;
    d.device_desc_size = sizeof(bx_msd_dev_descriptor2);
    d.config_desc_size = sizeof(bx_msd_config_descriptor2);
  } else {
    d.dev_descriptor = bx_msd_dev_descriptor;
    d.config_descriptor = bx_msd_config_descriptor;
    d.device_desc_size = sizeof(bx_msd_dev_descriptor);
    d.config_desc_size = sizeof(bx_msd_config_descriptor);
  }
  d.serial_num = s.scsi_dev->get_serial_number();
  s.mode = USB_MSDM_CBW;
  d.connected = 1;
  s.status_changed = 0;
  return 1;
}

const char* usb_msd_device_c::get_info()
{
  return s.info_txt;
}

void usb_msd_device_c::register_state_specific(bx_list_c *parent)
{
  s.sr_list = new bx_list_c(parent, "s", "USB MSD Device State");
  if (d.type == USB_DEV_TYPE_CDROM) {
    bx_list_c *rt_config = new bx_list_c(s.sr_list, "rt_config");
    rt_config->add(s.config->get_by_name("path"));
    rt_config->add(s.config->get_by_name("status"));
    rt_config->set_restore_handler(this, usb_msd_restore_handler);
  } else if ((d.type == USB_DEV_TYPE_DISK) && (s.hdimage != NULL)) {
    s.hdimage->register_state(s.sr_list);
  }
  BXRS_DEC_PARAM_FIELD(s.sr_list, mode, s.mode);
  BXRS_DEC_PARAM_FIELD(s.sr_list, scsi_len, s.scsi_len);
  BXRS_DEC_PARAM_FIELD(s.sr_list, usb_len, s.usb_len);
  BXRS_DEC_PARAM_FIELD(s.sr_list, data_len, s.data_len);
  BXRS_DEC_PARAM_FIELD(s.sr_list, residue, s.residue);
  BXRS_DEC_PARAM_FIELD(s.sr_list, tag, s.tag);
  BXRS_DEC_PARAM_FIELD(s.sr_list, result, s.result);
}

void usb_msd_device_c::handle_reset()
{
  BX_DEBUG(("Reset"));
  s.mode = USB_MSDM_CBW;
}

int usb_msd_device_c::handle_control(int request, int value, int index, int length, Bit8u *data)
{
  int ret = 0;

  ret = handle_control_common(request, value, index, length, data);
  if (ret >= 0) {
    return ret;
  }

  ret = 0;
  switch (request) {
    case DeviceOutRequest | USB_REQ_CLEAR_FEATURE:
      BX_DEBUG(("USB_REQ_CLEAR_FEATURE: Not handled: %i %i %i %i", request, value, index, length ));
      goto fail;
      break;
    case DeviceOutRequest | USB_REQ_SET_FEATURE:
      BX_DEBUG(("USB_REQ_SET_FEATURE:"));
      switch (value) {
        case USB_DEVICE_REMOTE_WAKEUP:
        case USB_DEVICE_U1_ENABLE:
        case USB_DEVICE_U2_ENABLE:
          break;
        default:
          BX_DEBUG(("USB_REQ_SET_FEATURE: Not handled: %i %i %i %i", request, value, index, length ));
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
              goto fail;
            default:
              BX_ERROR(("USB MSD handle_control: unknown string descriptor 0x%02x", value & 0xff));
              goto fail;
          }
          break;
        case USB_DT_DEVICE_QUALIFIER:
          BX_DEBUG(("USB_REQ_GET_DESCRIPTOR: Device Qualifier"));
          // device qualifier
          if (d.speed == USB_SPEED_HIGH) {
            data[0] = 10;
            data[1] = USB_DT_DEVICE_QUALIFIER;
            memcpy(data+2, bx_msd_dev_descriptor+2, 6);
            data[8] = 1;
            data[9] = 0;
            ret = 10;
          } else {
            // a low-, full- or super-speed device (i.e.: a non high-speed device)
            // must return request error on this function
            BX_ERROR(("USB MSD handle_control: full-speed only device returning stall on Device Qualifier."));
            goto fail;
          }
          break;
        case USB_DT_BIN_DEV_OBJ_STORE:
          BX_DEBUG(("USB_REQ_GET_DESCRIPTOR: BOS"));
          if (get_speed() == USB_SPEED_SUPER) {
            memcpy(data, bx_msd_bos_descriptor3, sizeof(bx_msd_bos_descriptor3));
            ret = sizeof(bx_msd_bos_descriptor3);
          } else
            goto fail;
          break;
        default:
          BX_ERROR(("USB MSD handle_control: unknown descriptor type 0x%02x", value >> 8));
          goto fail;
      }
      break;
    case EndpointOutRequest | USB_REQ_CLEAR_FEATURE:
      BX_DEBUG(("USB_REQ_CLEAR_FEATURE:"));
      if (value == 0 && index != 0x81) { /* clear ep halt */
        goto fail;
      }
      ret = 0;
      break;
    case DeviceOutRequest | USB_REQ_SET_SEL:
      // Set U1 and U2 System Exit Latency
      BX_DEBUG(("SET_SEL (U1 and U2):"));
      ret = 0;
      break;
    // Class specific requests
    case InterfaceOutClassRequest | MassStorageReset:
    case MassStorageReset:
      BX_DEBUG(("MASS STORAGE RESET:"));
      s.mode = USB_MSDM_CBW;
      ret = 0;
      break;
    case InterfaceInClassRequest | GetMaxLun:
    case GetMaxLun:
      BX_DEBUG(("MASS STORAGE: GET MAX LUN"));
      data[0] = 0;
      ret = 1;
      break;
    default:
      BX_ERROR(("USB MSD handle_control: unknown request 0x%04x", request));
    fail:
      d.stall = 1;
      ret = USB_RET_STALL;
      break;
  }
  return ret;
}

int usb_msd_device_c::handle_data(USBPacket *p)
{
  struct usb_msd_cbw cbw;
  int ret = 0;
  Bit8u devep = p->devep;
  Bit8u *data = p->data;
  int len = p->len;

  switch (p->pid) {
    case USB_TOKEN_OUT:
      usb_dump_packet(data, len);
      if (devep != 2)
        goto fail;

      switch (s.mode) {
        case USB_MSDM_CBW:
          if (len != 31) {
            BX_ERROR(("bad CBW len"));
            goto fail;
          }
          memcpy(&cbw, data, 31);
          if (dtoh32(cbw.sig) != 0x43425355) {
            BX_ERROR(("bad signature %08X", dtoh32(cbw.sig)));
            goto fail;
          }
          BX_DEBUG(("command on LUN %d", cbw.lun));
          s.tag = dtoh32(cbw.tag);
          s.data_len = dtoh32(cbw.data_len);
          if (s.data_len == 0) {
            s.mode = USB_MSDM_CSW;
          } else if (cbw.flags & 0x80) {
            s.mode = USB_MSDM_DATAIN;
          } else {
            s.mode = USB_MSDM_DATAOUT;
          }
          BX_DEBUG(("command tag 0x%X flags %08X len %d data %d",
                   s.tag, cbw.flags, cbw.cmd_len, s.data_len));
          s.residue = 0;
          s.scsi_dev->scsi_send_command(s.tag, cbw.cmd, cbw.lun, d.async_mode);
          if (s.residue == 0) {
            if (s.mode == USB_MSDM_DATAIN) {
              s.scsi_dev->scsi_read_data(s.tag);
            } else if (s.mode == USB_MSDM_DATAOUT) {
              s.scsi_dev->scsi_write_data(s.tag);
            }
          }
          ret = len;
          break;

        case USB_MSDM_DATAOUT:
          BX_DEBUG(("data out %d/%d", len, s.data_len));
          if (len > (int)s.data_len)
            goto fail;

          s.usb_buf = data;
          s.usb_len = len;
          while (s.usb_len && s.scsi_len) {
            copy_data();
          }
          if (s.residue && s.usb_len) {
            s.data_len -= s.usb_len;
            if (s.data_len == 0)
              s.mode = USB_MSDM_CSW;
            s.usb_len = 0;
          }
          if (s.usb_len) {
            BX_DEBUG(("deferring packet %p", p));
            usb_defer_packet(p, this);
            s.packet = p;
            ret = USB_RET_ASYNC;
          } else {
            ret = len;
          }
          break;

        default:
          BX_ERROR(("USB MSD handle_data: unexpected mode at USB_TOKEN_OUT: (0x%02X)", s.mode));
          goto fail;
      }
      break;

    case USB_TOKEN_IN:
      if (devep != 1)
        goto fail;

      switch (s.mode) {
        case USB_MSDM_DATAOUT:
          if (s.data_len != 0 || len < 13)
            goto fail;
          BX_DEBUG(("deferring packet %p", p));
          usb_defer_packet(p, this);
          s.packet = p;
          ret = USB_RET_ASYNC;
          break;

        case USB_MSDM_CSW:
          BX_DEBUG(("command status %d tag 0x%x, len %d",
                    s.result, s.tag, len));
          if (len < 13)
            return ret;

          send_status(p);
          s.mode = USB_MSDM_CBW;
          ret = 13;
          break;

        case USB_MSDM_DATAIN:
          BX_DEBUG(("data in %d/%d", len, s.data_len));
          if (len > (int)s.data_len)
            len = s.data_len;
          s.usb_buf = data;
          s.usb_len = len;
          while (s.usb_len && s.scsi_len) {
            copy_data();
          }
          if (s.residue && s.usb_len) {
            s.data_len -= s.usb_len;
            memset(s.usb_buf, 0, s.usb_len);
            if (s.data_len == 0)
              s.mode = USB_MSDM_CSW;
            s.usb_len = 0;
          }
          if (s.usb_len) {
            BX_DEBUG(("deferring packet %p", p));
            usb_defer_packet(p, this);
            s.packet = p;
            ret = USB_RET_ASYNC;
          } else {
            ret = len;
          }
          break;

        default:
          BX_ERROR(("USB MSD handle_data: unexpected mode at USB_TOKEN_IN: (0x%02X)", s.mode));
          goto fail;
      }
      if (ret > 0) usb_dump_packet(data, ret);
      break;

    default:
      BX_ERROR(("USB MSD handle_data: bad token"));
fail:
      d.stall = 1;
      ret = USB_RET_STALL;
      break;
  }

  return ret;
}

void usb_msd_device_c::copy_data()
{
  Bit32u len = s.usb_len;
  if (len > s.scsi_len)
    len = s.scsi_len;
  if (s.mode == USB_MSDM_DATAIN) {
    memcpy(s.usb_buf, s.scsi_buf, len);
  } else {
    memcpy(s.scsi_buf, s.usb_buf, len);
  }
  s.usb_len -= len;
  s.scsi_len -= len;
  s.usb_buf += len;
  s.scsi_buf += len;
  s.data_len -= len;
  if (s.scsi_len == 0) {
    if (s.mode == USB_MSDM_DATAIN) {
      s.scsi_dev->scsi_read_data(s.tag);
    } else if (s.mode == USB_MSDM_DATAOUT) {
      s.scsi_dev->scsi_write_data(s.tag);
    }
  }
}

void usb_msd_device_c::send_status(USBPacket *p)
{
  struct usb_msd_csw csw;

  csw.sig = htod32(0x53425355);
  csw.tag = htod32(s.tag);
  csw.residue = htod32(s.residue);
  csw.status = s.result;
  memcpy(p->data, &csw, BX_MIN(p->len, 13));
}

void usb_msd_device_c::usb_msd_command_complete(void *this_ptr, int reason, Bit32u tag, Bit32u arg)
{
  usb_msd_device_c *class_ptr = (usb_msd_device_c *) this_ptr;
  class_ptr->command_complete(reason, tag, arg);
}

void usb_msd_device_c::command_complete(int reason, Bit32u tag, Bit32u arg)
{
  USBPacket *p = s.packet;

  if (tag != s.tag) {
    BX_ERROR(("usb-msd_command_complete: unexpected SCSI tag 0x%x", tag));
  }
  if (reason == SCSI_REASON_DONE) {
    BX_DEBUG(("command complete %d", arg));
    s.residue = s.data_len;
    s.result = arg != 0;
    if (s.packet) {
      if ((s.data_len == 0) && (s.mode == USB_MSDM_DATAOUT)) {
        send_status(p);
        s.mode = USB_MSDM_CBW;
        usb_dump_packet(p->data, p->len);
      } else if (s.mode == USB_MSDM_CSW) {
        send_status(p);
        s.mode = USB_MSDM_CBW;
      } else {
        if (s.data_len) {
          s.data_len -= s.usb_len;
          if (s.mode == USB_MSDM_DATAIN)
            memset(s.usb_buf, 0, s.usb_len);
          s.usb_len = 0;
        }
        if (s.data_len == 0)
          s.mode = USB_MSDM_CSW;
      }
      s.packet = NULL;
      usb_packet_complete(p);
    } else if (s.data_len == 0) {
      s.mode = USB_MSDM_CSW;
    }
    return;
  }
  s.scsi_len = arg;
  s.scsi_buf = s.scsi_dev->scsi_get_buf(tag);
  if (p) {
    if ((s.scsi_len > 0) && (s.mode == USB_MSDM_DATAIN)) {
      usb_dump_packet(s.scsi_buf, p->len);
    }
    copy_data();
    if (s.usb_len == 0) {
      BX_DEBUG(("packet complete %p", p));
      if (s.packet != NULL) {
        s.packet = NULL;
        usb_packet_complete(p);
      }
    }
  }
}

void usb_msd_device_c::cancel_packet(USBPacket *p)
{
  s.scsi_dev->scsi_cancel_io(s.tag);
  s.packet = NULL;
  s.scsi_len = 0;
}

bx_bool usb_msd_device_c::set_inserted(bx_bool value)
{
  const char *path;

  if (value) {
    path = SIM->get_param_string("path", s.config)->getptr();
    if ((strlen(path) > 0) && (strcmp(path, "none"))) {
      if (!s.cdrom->insert_cdrom(path)) {
        value = 0;
      }
    } else {
      value = 0;
    }
    if (!value) {
      SIM->get_param_enum("status", s.config)->set(BX_EJECTED);
      s.status_changed = 0;
    }
  } else {
    s.cdrom->eject_cdrom();
  }
  s.scsi_dev->set_inserted(value);
  return value;
}

bx_bool usb_msd_device_c::get_inserted()
{
  return s.scsi_dev->get_inserted();
}

bx_bool usb_msd_device_c::get_locked()
{
  return s.scsi_dev->get_locked();
}

void usb_msd_device_c::runtime_config(void)
{
  if (d.type == USB_DEV_TYPE_CDROM) {
    if (s.status_changed) {
      set_inserted(0);
      if (SIM->get_param_enum("status", s.config)->get() == BX_INSERTED) {
        set_inserted(1);
      }
      s.status_changed = 0;
    }
  }
}

#undef LOG_THIS
#define LOG_THIS cdrom->

// USB cdrom runtime parameter handlers
const char *usb_msd_device_c::cdrom_path_handler(bx_param_string_c *param, int set,
                                                 const char *oldval, const char *val, int maxlen)
{
  usb_msd_device_c *cdrom;

  if (set) {
    if (strlen(val) < 1) {
      val = "none";
    }
    cdrom = (usb_msd_device_c*) param->get_parent()->get_device_param();
    if (cdrom != NULL) {
      if (!cdrom->get_locked()) {
        cdrom->s.status_changed = 1;
      } else {
        val = oldval;
        BX_ERROR(("cdrom tray locked: path change failed"));
      }
    } else {
      BX_PANIC(("cdrom_path_handler: cdrom not found"));
    }
  }
  return val;
}

Bit64s usb_msd_device_c::cdrom_status_handler(bx_param_c *param, int set, Bit64s val)
{
  usb_msd_device_c *cdrom;

  if (set) {
    cdrom = (usb_msd_device_c*) param->get_parent()->get_device_param();
    if (cdrom != NULL) {
      if ((val == 1) || !cdrom->get_locked()) {
        cdrom->s.status_changed = 1;
      } else if (cdrom->get_locked()) {
        BX_ERROR(("cdrom tray locked: eject failed"));
        return BX_INSERTED;
      }
    } else {
      BX_PANIC(("cdrom_status_handler: cdrom not found"));
    }
  }
  return val;
}

void usb_msd_restore_handler(void *dev, bx_list_c *conf)
{
  ((usb_msd_device_c*)dev)->restore_handler(conf);
}

void usb_msd_device_c::restore_handler(bx_list_c *conf)
{
  runtime_config();
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
