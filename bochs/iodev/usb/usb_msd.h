/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  USB mass storage device support (ported from QEMU)
//
//  Copyright (c) 2006 CodeSourcery.
//  Written by Paul Brook
//  Copyright (C) 2009-2014  The Bochs Project
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

#ifndef BX_IODEV_USB_MSD_H
#define BX_IODEV_USB_MSD_H

class device_image_t;
class cdrom_base_c;
class scsi_device_t;

class usb_msd_device_c : public usb_device_c {
public:
  usb_msd_device_c(usbdev_type type, const char *filename);
  virtual ~usb_msd_device_c(void);

  virtual bx_bool init();
  virtual bx_bool set_option(const char *option);
  virtual const char* get_info();

  virtual void handle_reset();
  virtual int handle_control(int request, int value, int index, int length, Bit8u *data);
  virtual int handle_data(USBPacket *p);
  virtual void register_state_specific(bx_list_c *parent);
  virtual void cancel_packet(USBPacket *p);
  bx_bool set_inserted(bx_bool value);
  bx_bool get_inserted();

protected:
  void copy_data();
  void send_status();
  static void usb_msd_command_complete(void *this_ptr, int reason, Bit32u tag, Bit32u arg);
  void command_complete(int reason, Bit32u tag, Bit32u arg);

private:
  struct {
    Bit8u mode;
    Bit32u scsi_len;
    Bit8u *scsi_buf;
    Bit32u usb_len;
    Bit8u *usb_buf;
    Bit32u data_len;
    Bit32u residue;
    Bit32u tag;
    int result;
    Bit8u image_mode;
    device_image_t *hdimage;
    cdrom_base_c *cdrom;
    scsi_device_t *scsi_dev;
    USBPacket *packet;
    bx_list_c *sr_list;
    const char *fname;
    bx_list_c *config;
    char info_txt[BX_PATHNAME_LEN];
    char journal[BX_PATHNAME_LEN]; // undoable / volatile disk only
    int size; // VVFAT disk only
  } s;

  static const char *cd_param_string_handler(bx_param_string_c *param, int set,
                                             const char *oldval, const char *val, int maxlen);
  static Bit64s cd_param_handler(bx_param_c *param, int set, Bit64s val);
};

#endif
