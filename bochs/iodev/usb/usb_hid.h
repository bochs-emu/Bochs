/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// USB HID emulation support (mouse and tablet) originally ported from QEMU
// Current USB emulation based on code by Benjamin D Lunt (fys [at] fysnet [dot] net)
//
// Copyright (c) 2005       Fabrice Bellard
// Copyright (c) 2007       OpenMoko, Inc.  (andrew@openedhand.com)
// Copyright (C) 2004-2023  Benjamin D Lunt (fys [at] fysnet [dot] net)
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

// Physical Descriptor Items
enum {
  HID_PHYS_BIAS_NOT_APP     = 0,
  HID_PHYS_BIAS_RIGHT_HAND  = 1,
  HID_PHYS_BIAS_LEFT_HAND   = 2,
  HID_PHYS_BIAS_BOTH_HANDS  = 3,
  HID_PHYS_BIAS_EITHER_HAND = 4
};

typedef enum
{
  None = 0, 
  Hand, Eyeball, Eyebrow, Eyelid, Ear, Nose, Mouth, UpperLip, LowerLip,
  Jaw, Neck, UpperArm, Elbow, Forearm, Wrist, Palm, 
  Thumb, IndexFinger, MiddleFinger, RingFinger, LittleFinger,
  Head, Shoulder, Hip, Waist, Thigh, Knee, Calf, Ankle, Foot, Heel,
  BallofFoot, BigToe, SecondToe, ThirdToe, FourthToe, LittleToe,
  Brow, Cheek,
} HID_PHYS_DESIGNATOR;

typedef enum
{
  NotApp = 0, 
  Right, Left, Both, Either, Center
} HID_PHYS_QUALIFIER;

// Device Model types
typedef enum
{
  // mice (w/o physical descriptor)
  hid_mouse_2x2x8 = 0,      // 2-button, 2-coords: X and Y coords, 8-bit
  hid_mouse_3x3x8,          // 3-button, 3-coords: X, Y, and Wheel coords, 8-bit (default)
  hid_mouse_3x3x12,         // 3-button, 3-coords: X, Y, and Wheel coords, 12-bit
  hid_mouse_3x3xDebug,      // 3-button, 3-coords: X, Y, and Wheel coords (debug)
  hid_mouse_3x3x16,         // 3-button, 3-coords: X, Y, and Wheel coords, 16-bit
  // mice (w/ physical descriptor)
  hid_mouse_3x3x8phy = 10,  // 3-button, 3-coords: X, Y, and Wheel coords, 8-bit, Physical Descriptor included
  
  // keyboards

  // keypads

  // tablets

} HID_MODEL;

// one (or more) of our models uses the Report ID field. This is the ID value used.
#define HID_REPORT_ID  1

// our HID device(s) return two class specific strings, index 4 and index 5
#define HID_CLASS_STR4   4
#define HID_CLASS_STR5   5

#define BX_M_ELEMENTS_SIZE   8

struct USBKBD {
  Bit8u code;
  Bit8u modkey;
};

#define BX_MODS_NONE             0
#define BX_MODS_LEFT_CTRL    (1<<0)
#define BX_MODS_LEFT_SHIFT   (1<<1)
#define BX_MODS_LEFT_ALT     (1<<2)
#define BX_MODS_LEFT_GUI     (1<<3)
#define BX_MODS_RIGHT_CTRL   (1<<4)
#define BX_MODS_RIGHT_SHIFT  (1<<5)
#define BX_MODS_RIGHT_ALT    (1<<6)
#define BX_MODS_RIGHT_GUI    (1<<7)

class usb_hid_device_c : public usb_device_c {
public:
  usb_hid_device_c(const char *devname);
  virtual ~usb_hid_device_c();

  virtual bool init();
  virtual bool set_option(const char *option);
  virtual const char *get_info();
  virtual void handle_reset();
  virtual int handle_control(int request, int value, int index, int length, Bit8u *data);
  virtual int handle_data(USBPacket *p);
  virtual void register_state_specific(bx_list_c *parent);

private:
  struct HID_STATE {
    bool has_events;
    Bit8u idle;
    int mouse_delayed_dx;
    int mouse_delayed_dy;
    Bit16s mouse_x;
    Bit16s mouse_y;
    Bit8s mouse_z;
    Bit8u b_state;
    Bit8u mouse_event_count;
    Bit8u mouse_event_buf[BX_KBD_ELEMENTS][BX_M_ELEMENTS_SIZE];
    int   mouse_event_buf_len[BX_KBD_ELEMENTS];
    Bit8u kbd_packet[8];
    int   kbd_packet_indx;
    Bit8u indicators;
    Bit8u kbd_event_count;
    Bit32u kbd_event_buf[BX_KBD_ELEMENTS];
    // the remaining does not get cleared on a handle_reset()
    HID_MODEL model;
    Bit8u report_use_id; // id that we will use as soon as the HID report has been requested
    Bit8u report_id;     // id that we will use after the HID report has been requested
    bool boot_protocol;  // 0 = boot protocol, 1 = report protocol
    int bx_mouse_hid_report_descriptor_len;
    const Bit8u *bx_mouse_hid_report_descriptor;
  } s;

  int timer_index;

  static bool gen_scancode_static(void *dev, Bit32u key);
  bool gen_scancode(Bit32u key);
  static Bit8u kbd_get_elements_static(void *dev);
  Bit8u kbd_get_elements(void);
  static void mouse_enabled_changed(void *dev, bool enabled);
  static void mouse_enq_static(void *dev, int delta_x, int delta_y, int delta_z, unsigned button_state, bool absxy);
  void mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state, bool absxy);
  int mouse_poll(Bit8u *buf, bool force);
  int create_mouse_packet(Bit8u *buf);
  int get_mouse_packet(Bit8u *buf);
  int keyboard_poll(Bit8u *buf, bool force);

  static void hid_timer_handler(void *);
  void start_idle_timer(void);
  void hid_idle_timer(void);
};

#endif
