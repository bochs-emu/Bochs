/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2023  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2009-2023  The Bochs Project
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

#ifndef BX_IODEV_USB_UHCI_H
#define BX_IODEV_USB_UHCI_H

#if BX_USE_USB_UHCI_SMF
#  define BX_UHCI_THIS theUSB_UHCI->
#  define BX_UHCI_THIS_PTR theUSB_UHCI
#else
#  define BX_UHCI_THIS this->
#  define BX_UHCI_THIS_PTR this
#endif

class bx_usb_uhci_c : public bx_uhci_core_c {
public:
  bx_usb_uhci_c();
  virtual ~bx_usb_uhci_c();
  virtual void init(void);
  virtual void reset(unsigned);
  virtual void register_state(void);
  virtual void after_restore_state(void);

private:
  Bit8u device_change;
  int rt_conf_id;

  void init_device(Bit8u port, bx_list_c *portconf);
  static void remove_device(Bit8u port);

  static void runtime_config_handler(void *);
  void runtime_config(void);

  static Bit64s usb_param_handler(bx_param_c *param, bool set, Bit64s val);
  static bool usb_param_enable_handler(bx_param_c *param, bool en);
};

#endif
