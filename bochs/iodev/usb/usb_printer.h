/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009       Benjamin D Lunt (fys at frontiernet net)
//                2009-2012  The Bochs Project
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

#ifndef BX_IODEV_USB_PRINTER_H
#define BX_IODEV_USB_PRINTER_H

class usb_printer_device_c : public usb_device_c {
public:
  usb_printer_device_c(usbdev_type type, const char *filename);
  virtual ~usb_printer_device_c(void);

  virtual bx_bool init();
  virtual const char* get_info();

  virtual void handle_reset();
  virtual int handle_control(int request, int value, int index, int length, Bit8u *data);
  virtual int handle_data(USBPacket *p);
  virtual void register_state_specific(bx_list_c *parent);

private:
  struct {
    Bit8u printer_status;
    const char *fname;
    FILE *fp;
    char info_txt[BX_PATHNAME_LEN];
  } s;
};

#endif
