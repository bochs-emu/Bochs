/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  USB mass storage device support (ported from QEMU)
//
//  Copyright (c) 2006 CodeSourcery.
//  Written by Paul Brook
//  Copyright (C) 2009-2013  The Bochs Project
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

static const Bit8u bx_msd_dev_descriptor[] = {
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
  0x40, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x00,       /*  u8  ep_bInterval; */

  /* Bulk-Out endpoint */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x02,       /*  u8  ep_bEndpointAddress; OUT Endpoint 2 */
  0x02,       /*  u8  ep_bmAttributes; Bulk */
  0x40, 0x00, /*  u16 ep_wMaxPacketSize; */
  0x00        /*  u8  ep_bInterval; */
};

static int usb_cdrom_count = 0;


usb_msd_device_c::usb_msd_device_c(usbdev_type type, const char *filename)
{
  char pname[10];
  char label[32];
  char tmpfname[BX_PATHNAME_LEN];
  char *ptr1, *ptr2;
  bx_param_string_c *path;
  bx_param_enum_c *status;

  d.type = type;
  d.maxspeed = USB_SPEED_FULL;
  d.speed = d.maxspeed;
  // TODO: d.max_packet_size = ???
  memset((void*)&s, 0, sizeof(s));
  if (d.type == USB_DEV_TYPE_DISK) {
    strcpy(d.devname, "BOCHS USB HARDDRIVE");
    strcpy(tmpfname, filename);
    ptr1 = strtok(tmpfname, ":");
    ptr2 = strtok(NULL, ":");
    if ((ptr2 == NULL) || (strlen(ptr1) < 2)) {
      s.image_mode = BX_HDIMAGE_MODE_FLAT;
      s.fname = filename;
    } else {
      s.image_mode = SIM->hdimage_get_mode(ptr1);
      s.fname = filename+strlen(ptr1)+1;
    }
    s.journal[0] = 0;
  } else if (d.type == USB_DEV_TYPE_CDROM) {
    strcpy(d.devname, "BOCHS USB CDROM");
    s.fname = filename;
    // config options
    bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
    sprintf(pname, "cdrom%d", ++usb_cdrom_count);
    sprintf(label, "USB CD-ROM #%d Configuration", usb_cdrom_count);
    s.config = new bx_list_c(usb_rt, pname, label);
    s.config->set_options(bx_list_c::SERIES_ASK | bx_list_c::USE_BOX_TITLE);
    s.config->set_device_param(this);
    path = new bx_param_string_c(s.config, "path", "Path", "", "", BX_PATHNAME_LEN);
    path->set(s.fname);
    path->set_handler(cd_param_string_handler);
    status = new bx_param_enum_c(s.config,
      "status",
      "Status",
      "CD-ROM media status (inserted / ejected)",
      media_status_names,
      BX_EJECTED,
      BX_EJECTED);
    status->set_handler(cd_param_handler);
    status->set_ask_format("Is the device inserted or ejected? [%s] ");
#if BX_WITH_WX
    bx_list_c *usb = (bx_list_c*)SIM->get_param("ports.usb");
    usb->add(s.config);
#endif
  }

  put("usb_msd", "USBMSD");
}

usb_msd_device_c::~usb_msd_device_c(void)
{
  if (s.scsi_dev != NULL)
    delete s.scsi_dev;
  if (s.hdimage != NULL) {
    delete s.hdimage;
  } else if (s.cdrom != NULL) {
    delete s.cdrom;
#if BX_WITH_WX
    bx_list_c *usb = (bx_list_c*)SIM->get_param("ports.usb");
    usb->remove(s.config->get_name());
#endif
    bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
    usb_rt->remove(s.config->get_name());
  }
}

bx_bool usb_msd_device_c::set_option(const char *option)
{
  if (!strncmp(option, "journal:", 8)) {
    strcpy(s.journal, option+8);
    return 1;
  }
  return 0;
}

bx_bool usb_msd_device_c::init()
{
  if (d.type == USB_DEV_TYPE_DISK) {
    s.hdimage = DEV_hdimage_init_image(s.image_mode, 0, s.journal);
    if (s.hdimage->open(s.fname) < 0) {
      BX_ERROR(("could not open hard drive image file '%s'", s.fname));
      return 0;
    } else {
      s.scsi_dev = new scsi_device_t(s.hdimage, 0, usb_msd_command_complete, (void*)this);
    }
    sprintf(s.info_txt, "USB HD: path='%s', mode='%s'", s.fname, hdimage_mode_names[s.image_mode]);
  } else if (d.type == USB_DEV_TYPE_CDROM) {
    s.cdrom = DEV_hdimage_init_cdrom(s.fname);
    if (!s.cdrom->insert_cdrom()) {
      BX_ERROR(("could not open cdrom image file '%s'", s.fname));
      return 0;
    } else {
      s.scsi_dev = new scsi_device_t(s.cdrom, 0, usb_msd_command_complete, (void*)this);
    }
    sprintf(s.info_txt, "USB CD: path='%s'", s.fname);
  }
  s.scsi_dev->register_state(s.sr_list, "scsidev");
  s.mode = USB_MSDM_CBW;
  d.connected = 1;
  return 1;
}

const char* usb_msd_device_c::get_info()
{
  return s.info_txt;
}

void usb_msd_device_c::register_state_specific(bx_list_c *parent)
{
  s.sr_list = new bx_list_c(parent, "s", "USB MSD Device State");
  if ((d.type == USB_DEV_TYPE_DISK) && (s.hdimage != NULL)) {
    s.hdimage->register_state(s.sr_list);
  }
  new bx_shadow_num_c(s.sr_list, "mode", &s.mode);
  new bx_shadow_num_c(s.sr_list, "scsi_len", &s.scsi_len);
  new bx_shadow_num_c(s.sr_list, "usb_len", &s.usb_len);
  new bx_shadow_num_c(s.sr_list, "data_len", &s.data_len);
  new bx_shadow_num_c(s.sr_list, "residue", &s.residue);
  new bx_shadow_num_c(s.sr_list, "tag", &s.tag);
  new bx_shadow_num_c(s.sr_list, "result", &s.result);
}

void usb_msd_device_c::handle_reset()
{
  BX_DEBUG(("Reset"));
  s.mode = USB_MSDM_CBW;
}

int usb_msd_device_c::handle_control(int request, int value, int index, int length, Bit8u *data)
{
  int ret = 0;

  switch (request) {
    case DeviceRequest | USB_REQ_GET_STATUS:
    case EndpointRequest | USB_REQ_GET_STATUS:
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
          memcpy(data, bx_msd_dev_descriptor, sizeof(bx_msd_dev_descriptor));
          ret = sizeof(bx_msd_dev_descriptor);
          break;
        case USB_DT_CONFIG:
          memcpy(data, bx_msd_config_descriptor, sizeof(bx_msd_config_descriptor));
          ret = sizeof(bx_msd_config_descriptor);
          break;
        case USB_DT_STRING:
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
              ret = set_usb_string(data, "BOCHS");
              break;
            case 2:
              // product description
              if (strlen(d.devname) > 0) {
                ret = set_usb_string(data, d.devname);
              } else {
                goto fail;
              }
              break;
            case 3:
              // serial number
              ret = set_usb_string(data, s.scsi_dev->get_serial_number());
              break;
            default:
              BX_ERROR(("USB MSD handle_control: unknown string descriptor 0x%02x", value & 0xff));
              goto fail;
          }
          break;
        case USB_DT_DEVICE_QUALIFIER:
          // device qualifier
          data[0] = 10;
          data[1] = USB_DT_DEVICE_QUALIFIER;
          memcpy(data+2, bx_msd_dev_descriptor+2, 6);
          data[8] = 1;
          data[9] = 0;
          ret = 10;
          break;
        default:
          BX_ERROR(("USB MSD handle_control: unknown descriptor type 0x%02x", value >> 8));
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
    case InterfaceOutRequest | USB_REQ_SET_INTERFACE:
      ret = 0;
      break;
    case EndpointOutRequest | USB_REQ_CLEAR_FEATURE:
      if (value == 0 && index != 0x81) { /* clear ep halt */
        goto fail;
      }
      ret = 0;
      break;
      // Class specific requests
    case InterfaceOutClassRequest | MassStorageReset:
    case MassStorageReset:
      s.mode = USB_MSDM_CBW;
      ret = 0;
      break;
    case InterfaceInClassRequest | GetMaxLun:
    case GetMaxLun:
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
            BX_ERROR(("bad signature %08x", dtoh32(cbw.sig)));
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
          BX_DEBUG(("command tag 0x%x flags %08x len %d data %d",
                   s.tag, cbw.flags, cbw.cmd_len, s.data_len));
          s.residue = 0;
          s.scsi_dev->scsi_send_command(s.tag, cbw.cmd, cbw.lun);
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
          if (s.scsi_len) {
            copy_data();
          }
          if (s.residue && s.usb_len) {
            s.data_len -= s.usb_len;
            if (s.data_len == 0)
              s.mode = USB_MSDM_CSW;
            s.usb_len = 0;
          }
          if (s.usb_len) {
            BX_INFO(("deferring packet %p", p));
            usb_defer_packet(p, this);
            s.packet = p;
            ret = USB_RET_ASYNC;
          } else {
            ret = len;
          }
          break;

        default:
          BX_ERROR(("USB MSD handle_data: unexpected mode at USB_TOKEN_OUT"));
          goto fail;
      }
      break;

    case USB_TOKEN_IN:
      if (devep != 1)
        goto fail;

      switch (s.mode) {
        case USB_MSDM_DATAOUT:
          if ((s.data_len > 0) && (len == 13)) {
            s.usb_len = len;
            s.usb_buf = data;
            send_status();
            ret = 13;
            break;
          }
          if (s.data_len != 0 || len < 13)
            goto fail;
          usb_defer_packet(p, this);
          s.packet = p;
          ret = USB_RET_ASYNC;
          break;

        case USB_MSDM_CSW:
          BX_DEBUG(("command status %d tag 0x%x, len %d",
                    s.result, s.tag, len));
          if (len < 13)
            return ret;

          s.usb_len = len;
          s.usb_buf = data;
          send_status();
          s.mode = USB_MSDM_CBW;
          ret = 13;
          break;

        case USB_MSDM_DATAIN:
          BX_DEBUG(("data in %d/%d", len, s.data_len));
          if (len > (int)s.data_len)
            len = s.data_len;
          s.usb_buf = data;
          s.usb_len = len;
          if (s.scsi_len) {
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
            BX_INFO(("deferring packet %p", p));
            usb_defer_packet(p, this);
            s.packet = p;
            ret = USB_RET_ASYNC;
          } else {
            ret = len;
          }
          break;

        default:
          BX_ERROR(("USB MSD handle_data: unexpected mode at USB_TOKEN_IN"));
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

void usb_msd_device_c::send_status()
{
  struct usb_msd_csw csw;

  csw.sig = htod32(0x53425355);
  csw.tag = htod32(s.tag);
  csw.residue = s.residue;
  csw.status = s.result;
  memcpy(s.usb_buf, &csw, 13);
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
      if (s.data_len == 0 && s.mode == USB_MSDM_DATAOUT) {
        send_status();
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
    } else if (s.data_len == 0) {
      s.mode = USB_MSDM_CSW;
    }
    return;
  }
  s.scsi_len = arg;
  s.scsi_buf = s.scsi_dev->scsi_get_buf(tag);
  if (p) {
    copy_data();
    if (s.usb_len == 0) {
      BX_INFO(("packet complete %p", p));
      s.packet = NULL;
    }
  }
}

void usb_msd_device_c::cancel_packet(USBPacket *p)
{
  s.scsi_dev->scsi_cancel_io(s.tag);
  s.packet = NULL;
  s.scsi_len = 0;
}

void usb_msd_device_c::set_inserted(bx_bool value)
{
  const char *path;

  if (value) {
    path = SIM->get_param_string("path", s.config)->getptr();
    if (!s.cdrom->insert_cdrom(path)) {
      SIM->get_param_bool("status", s.config)->set(0);
      return;
    }
  } else {
    s.cdrom->eject_cdrom();
  }
  s.scsi_dev->set_inserted(value);
}

bx_bool usb_msd_device_c::get_inserted()
{
  return s.scsi_dev->get_inserted();
}

#undef LOG_THIS
#define LOG_THIS cdrom->

// USB hub runtime parameter handlers
const char *usb_msd_device_c::cd_param_string_handler(bx_param_string_c *param, int set,
                                                      const char *oldval, const char *val, int maxlen)
{
  usb_msd_device_c *cdrom;

  if (set) {
    cdrom = (usb_msd_device_c*) param->get_parent()->get_device_param();
    if (cdrom != NULL) {
      bx_bool empty = ((strlen(val) == 0) || (!strcmp(val, "none")));
      if (!empty) {
        if (cdrom->get_inserted()) {
          BX_ERROR(("direct path change not supported (setting to 'none')"));
          param->set("none");
        }
      } else {
        SIM->get_param_bool("status", param->get_parent())->set(0);
      }
    } else {
      BX_PANIC(("cd_param_string_handler: cdrom not found"));
    }
  }
  return val;
}

Bit64s usb_msd_device_c::cd_param_handler(bx_param_c *param, int set, Bit64s val)
{
  usb_msd_device_c *cdrom;
  const char *path;

  if (set) {
    cdrom = (usb_msd_device_c*) param->get_parent()->get_device_param();
    if (cdrom != NULL) {
      path = SIM->get_param_string("path", param->get_parent())->getptr();
      val &= ((strlen(path) > 0) && (strcmp(path, "none")));
      if (val != cdrom->get_inserted()) {
        cdrom->set_inserted(val == BX_INSERTED);
      }
    } else {
      BX_PANIC(("cd_param_string_handler: cdrom not found"));
    }
  }
  return val;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
