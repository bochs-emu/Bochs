/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// USB HID emulation support (mouse and tablet) ported from QEMU
// USB keypad emulation based on code by Benjamin D Lunt (fys [at] fysnet [dot] net)
// USB keyboard emulation is an extension to the keypad based on the specs
//
// Copyright (c) 2005       Fabrice Bellard
// Copyright (c) 2007       OpenMoko, Inc.  (andrew@openedhand.com)
// Copyright (C) 2009-2023  The Bochs Project
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
  usb_hid_device_c(const char *devname);
  virtual ~usb_hid_device_c();

  virtual bool init();
  virtual const char *get_info();
  virtual void handle_reset();
  virtual int handle_control(int request, int value, int index, int length, Bit8u *data);
  virtual int handle_data(USBPacket *p);
  virtual void register_state_specific(bx_list_c *parent);

private:
  struct {
    bool has_events;
    Bit8u idle;
    bool boot_protocol;  // 0 = boot protocol, 1 = report protocol
    int mouse_delayed_dx;
    int mouse_delayed_dy;
    Bit16s mouse_x;
    Bit16s mouse_y;
    Bit8s mouse_z;
    Bit8u b_state;
    Bit8u mouse_event_count;
    Bit8u mouse_event_buf[BX_KBD_ELEMENTS][6];
    Bit8u kbd_packet[8];
    Bit8u indicators;
    Bit8u kbd_event_count;
    Bit32u kbd_event_buf[BX_KBD_ELEMENTS];
  } s;

  int timer_index;

  static bool gen_scancode_static(void *dev, Bit32u key);
  bool gen_scancode(Bit32u key);
  static Bit8u kbd_get_elements_static(void *dev);
  Bit8u kbd_get_elements(void);
  static void mouse_enabled_changed(void *dev, bool enabled);
  static void mouse_enq_static(void *dev, int delta_x, int delta_y, int delta_z, unsigned button_state, bool absxy);
  void mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state, bool absxy);
  int mouse_poll(Bit8u *buf, int len, bool force);
  int create_mouse_packet(Bit8u *buf, int len);
  int get_mouse_packet(Bit8u *buf, int len);
  int keyboard_poll(Bit8u *buf, int len, bool force);

  static void hid_timer_handler(void *);
  void start_idle_timer(void);
  void hid_idle_timer(void);
};

#endif
