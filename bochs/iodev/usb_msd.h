/////////////////////////////////////////////////////////////////////////
// $Id: usb_msd.h,v 1.2 2007-03-24 11:43:41 vruppert Exp $
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

// USB mass storage device support ported from the Qemu project

#ifndef BX_IODEV_USB_MSD_H
#define BX_IODEV_USB_MSD_H

class scsi_device_t;

enum USBMSDMode {
  USB_MSDM_CBW,
  USB_MSDM_DATAOUT,
  USB_MSDM_DATAIN,
  USB_MSDM_CSW
};


class usb_msd_device_t : public usb_device_t {
public:
  usb_msd_device_t(void);
  virtual ~usb_msd_device_t(void);

  bx_bool init(const char *filename);
  virtual void handle_reset();
  virtual int handle_control(int request, int value, int index, int length, Bit8u *data);
  virtual int handle_data(USBPacket *p);
protected:
  void copy_data();
  void send_status();
  static void usb_msd_command_complete(void *this_ptr, int reason, Bit32u tag, Bit32u arg);
  void command_complete(int reason, Bit32u tag, Bit32u arg);

private:
  struct {
    enum USBMSDMode mode;
    Bit32u scsi_len;
    Bit8u *scsi_buf;
    Bit32u usb_len;
    Bit8u *usb_buf;
    Bit32u data_len;
    Bit32u residue;
    Bit32u tag;
    int result;
    device_image_t *hdimage;
    scsi_device_t *scsi_dev;
    USBPacket *packet;
  } s;
};

#endif
