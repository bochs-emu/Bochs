/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Experimental USB EHCI adapter (partly ported from Qemu)
//
//  Copyright (C) 2015  The Bochs Project
//
//  Copyright(c) 2008  Emutex Ltd. (address@hidden)
//  Copyright(c) 2011-2012 Red Hat, Inc.
//
//  Red Hat Authors:
//  Gerd Hoffmann <kraxel@redhat.com>
//  Hans de Goede <hdegoede@redhat.com>
//
//  EHCI project was started by Mark Burkley, with contributions by
//  Niels de Vos.  David S. Ahern continued working on it.  Kevin Wolf,
//  Jan Kiszka and Vincent Palatin contributed bugfixes.
//
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

#ifndef BX_IODEV_USB_EHCI_H
#define BX_IODEV_USB_EHCI_H

#if BX_USE_USB_EHCI_SMF
#  define BX_EHCI_THIS theUSB_EHCI->
#  define BX_EHCI_THIS_PTR theUSB_EHCI
#else
#  define BX_EHCI_THIS this->
#  define BX_EHCI_THIS_PTR this
#endif

#define USB_EHCI_PORTS  6

typedef struct {
  int frame_timer_index;

  Bit8u  usbsts_pending;
  Bit32u usbsts_frindex;
  Bit64u last_run_usec;

  struct {
    Bit8u  CapLength;
    Bit8u  Reserved;
    Bit16u HciVersion;
    Bit32u HcsParams;
    Bit32u HccParams;
  } cap_regs;

  struct {
    struct {
      Bit8u   itc;
      bx_bool iaad;
      bx_bool ase;
      bx_bool pse;
      bx_bool hcreset;
      bx_bool rs;
    } UsbCmd;
    struct {
      bx_bool ass;
      bx_bool pss;
      bx_bool recl;
      bx_bool hchalted;
      Bit8u   inti;
    } UsbSts;
    Bit8u  UsbIntr;
    Bit32u FrIndex;
    Bit32u CtrlDsSegment;
    Bit32u PeriodicListBase;
    Bit32u AsyncListAddr;
    Bit32u ConfigFlag;
  } op_regs;

  struct {
    // our data
    usb_device_c *device;   // device connected to this port
    bx_bool owner_change;
    struct {
      bx_bool woe;
      bx_bool wde;
      bx_bool wce;
      Bit8u   ptc;
      Bit8u   pic;
      bx_bool po;
      bx_bool pp;
      Bit8u   ls;
      bx_bool pr;
      bx_bool sus;
      bx_bool fpr;
      bx_bool occ;
      bx_bool oca;
      bx_bool pec;
      bx_bool ped;
      bx_bool csc;
      bx_bool ccs;
    } portsc;
  } usb_port[USB_EHCI_PORTS];

} bx_usb_ehci_t;

class bx_usb_ehci_c : public bx_devmodel_c, public bx_pci_device_stub_c {
public:
  bx_usb_ehci_c();
  virtual ~bx_usb_ehci_c();
  virtual void init(void);
  virtual void reset(unsigned);
  virtual void register_state(void);
  virtual void after_restore_state(void);
  virtual Bit32u  pci_read_handler(Bit8u address, unsigned io_len);
  virtual void    pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  static const char *usb_param_handler(bx_param_string_c *param, int set,
                                       const char *oldval, const char *val, int maxlen);

private:
  bx_uhci_core_c *uhci[3];
  bx_usb_ehci_t hub;
  Bit8u         devfunc;
  Bit8u         device_change;
  int           rt_conf_id;
  Bit8u         *device_buffer;

  void reset_hc();
  void reset_port(int);

  static void init_device(Bit8u port, bx_list_c *portconf);
  static void remove_device(Bit8u port);
  static void set_connect_status(Bit8u port, int type, bx_bool connected);
  static void change_port_owner(int port);
  static int  broadcast_packet(USBPacket *p, const int port);

  void update_irq(void);
  void raise_irq(Bit8u intr);
  void commit_irq(void);
  void update_frindex(int frames);

  static void ehci_frame_handler(void *);
  void ehci_frame_timer(void);

#if BX_USE_USB_EHCI_SMF
  static bx_bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bx_bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#else
  bx_bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  bx_bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#endif

  static void runtime_config_handler(void *);
  void runtime_config(void);
};

#endif  // BX_IODEV_USB_EHCI_H
