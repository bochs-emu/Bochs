/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// USB HID emulation support (mouse and tablet) ported from QEMU
// USB keypad emulation based on code by Benjamin D Lunt (fys at frontiernet net)
//
// Copyright (c) 2005       Fabrice Bellard
// Copyright (c) 2007       OpenMoko, Inc.  (andrew@openedhand.com)
// Copyright (C) 2009-2012  The Bochs Project
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
  static void mouse_enq_static(void *dev, int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy);
  void mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy);
  int mouse_poll(Bit8u *buf, int len);
  int keypad_poll(Bit8u *buf, int len);
};

#endif
