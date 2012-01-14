/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2012  The Bochs Project
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

// USB HID emulation support ported from the Qemu project

#ifndef BX_IODEV_USB_HID_H
#define BX_IODEV_USB_HID_H


class usb_hid_device_c : public usb_device_c {
public:
  usb_hid_device_c(usbdev_type type);
  virtual ~usb_hid_device_c(void);

  virtual void handle_reset();
  virtual int handle_control(int request, int value, int index, int length, Bit8u *data);
  virtual int handle_data(USBPacket *p);
  virtual void register_state_specific(bx_list_c *parent);

private:
  struct {
    int mouse_delayed_dx;
    int mouse_delayed_dy;
    int mouse_delayed_dz;
    Bit16s mouse_x;
    Bit16s mouse_y;
    Bit8s mouse_z;
    Bit8u b_state;
    Bit8u saved_key[8];
    Bit8u key_pad_packet[8];
  } s;

  static bx_bool key_enq_static(void *dev, Bit8u *scan_code);
  bx_bool key_enq(Bit8u *scan_code);
  static void mouse_enabled_changed(void *dev, bx_bool enabled);
  static void mouse_enq_static(void *dev, int delta_x, int delta_y, int delta_z, unsigned button_state);
  void mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state);
  int mouse_poll(Bit8u *buf, int len);
  int keypad_poll(Bit8u *buf, int len);
};

#endif
