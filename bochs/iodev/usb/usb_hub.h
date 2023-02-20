/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// USB hub emulation support (ported from QEMU)
//
// Copyright (C) 2005       Fabrice Bellard
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

#ifndef BX_IODEV_USB_HUB_H
#define BX_IODEV_USB_HUB_H


#define USB_HUB_MAX_PORTS 8
#define USB_HUB_DEF_PORTS 4

class usb_hub_device_c : public usb_device_c {
public:
  usb_hub_device_c(void);
  virtual ~usb_hub_device_c(void);

  virtual bool init();
  virtual bool set_option(const char *option);
  virtual const char* get_info();

  virtual usb_device_c* find_device(Bit8u addr);
  virtual int handle_packet(USBPacket *p);
  virtual void handle_reset();
  virtual int handle_control(int request, int value, int index, int length, Bit8u *data);
  virtual int handle_data(USBPacket *p);
  virtual void register_state_specific(bx_list_c *parent);
  virtual void after_restore_state();
  virtual void runtime_config();
  void restore_handler(bx_list_c *conf);
  void event_handler(int event, USBPacket *packet, int port);

private:
  struct {
    Bit8u n_ports;
    bx_list_c *config;
    bx_list_c *state;
    char serial_number[16];
    char info_txt[18];
    struct {
      // our data
      usb_device_c *device;  // device connected to this port

      Bit16u PortStatus;
      Bit16u PortChange;
    } usb_port[USB_HUB_MAX_PORTS];
    Bit16u device_change;
  } hub;

  int broadcast_packet(USBPacket *p);
  void init_device(Bit8u port, bx_list_c *portconf);
  void remove_device(Bit8u port);
  bool usb_set_connect_status(Bit8u port, bool connected);

  static Bit64s hub_param_handler(bx_param_c *param, bool set, Bit64s val);
  static bool hub_param_enable_handler(bx_param_c *param, bool en);
};

#endif
