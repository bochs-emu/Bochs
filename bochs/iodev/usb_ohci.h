/////////////////////////////////////////////////////////////////////////
// $Id: usb_ohci.h,v 1.1 2009-01-19 17:18:57 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009  Benjamin D Lunt (fys at frontiernet net)
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef BX_IODEV_USB_OHCI_H
#define BX_IODEV_USB_OHCI_H

#if BX_USE_USB_OHCI_SMF
#  define BX_OHCI_THIS theUSB_OHCI->
#  define BX_OHCI_THIS_PTR theUSB_OHCI
#else
#  define BX_OHCI_THIS this->
#  define BX_OHCI_THIS_PTR this
#endif

#define USB_NUM_PORTS   2

typedef struct {
  bx_phy_address base_addr;

  int   timer_index;

  struct OHCI_OP_REGS {
    struct {                     //   size                                on reset     HCD  HC
      Bit32u reserved;           // 24 bit reserved                    = 0x000001 (?)   R   R
      Bit8u  rev;                // 8 bit revision                     = 0x10           R   R
    } HcRevision;                //                                    = 0x00000110
    struct {
      Bit32u reserved;           // 21 bit reserved                    = 0x000000       R   R
      bx_bool rwe;               //  1 bit RemoteWakeupEnable          = 0b             RW  R
      bx_bool rwc;               //  1 bit RemoteWakeupConnected       = 0b             RW  RW
      bx_bool ir;                //  1 bit InterruptRouting            = 0b             RW  R
      Bit8u   hcfs;              //  2 bit HostControllerFuncState     = 00b            RW  RW
      bx_bool ble;               //  1 bit BulkListEnable              = 0b             RW  R
      bx_bool cle;               //  1 bit ControlListEnable           = 0b             RW  R
      bx_bool ie;                //  1 bit IsochronousEnable           = 0b             RW  R
      bx_bool ple;               //  1 bit PeriodicListEnable          = 0b             RW  R
      Bit8u   cbsr;              //  2 bit ControlBulkService Ratio    = 00b            RW  R
    } HcControl;                 //                                    = 0x00000000
    struct {
      Bit16u reserved0;          // 14 bit reserved                    = 0x000000       R   R
      Bit8u  soc;                //  2 bit SchedulingOverrunCount      = 00b            R   RW
      Bit16u reserved1;          // 12 bit reserved                    = 0x000000       R   R
      bx_bool ocr;               //  1 bit OwnershipChangeRequest      = 0b             RW  RW
      bx_bool blf;               //  1 bit BulkListFilled              = 0b             RW  RW
      bx_bool clf;               //  1 bit ControlListFilled           = 0b             RW  RW
      bx_bool hcr;               //  1 bit HostControllerReset         = 0b             RW  RW
    } HcCommandStatus;           //                                    = 0x00000000
    struct {
      bx_bool zero;              //  1 bit zero                        = 0b             R   R
      bx_bool oc;                //  1 bit OwnershipChange             = 0b             RWC RW
      Bit32u  reserved;          // 23 bit reserved                    = 0x000000       R   R
      bx_bool rhsc;              //  1 bit RootHubStatusChange         = 0b             RWC RW
      bx_bool fno;               //  1 bit FrameNumberOverflow         = 0b             RWC RW
      bx_bool ue;                //  1 bit UnrecoverableError          = 0b             RWC RW
      bx_bool rd;                //  1 bit ResumeDetected              = 0b             RWC RW
      bx_bool sf;                //  1 bit StartifFrame                = 0b             RWC RW
      bx_bool wdh;               //  1 bit WritebackDoneHead           = 0b             RWC RW
      bx_bool so;                //  1 bit SchedulingOverrun           = 0b             RWC RW
    } HcInterruptStatus;         //                                    = 0x00000000
    struct {
      bx_bool mie;               //  1 bit MasterInterruptEnable       = 0b             RW  R
      bx_bool oc;                //  1 bit OwnershipChange             = 0b             RW  R
      Bit32u  reserved;          // 23 bit reserved                    = 0x000000       R   R
      bx_bool rhsc;              //  1 bit RootHubStatusChange         = 0b             RW  RW
      bx_bool fno;               //  1 bit FrameNumberOverflow         = 0b             RW  RW
      bx_bool ue;                //  1 bit UnrecoverableError          = 0b             RW  RW
      bx_bool rd;                //  1 bit ResumeDetected              = 0b             RW  RW
      bx_bool sf;                //  1 bit StartifFrame                = 0b             RW  RW
      bx_bool wdh;               //  1 bit WritebackDoneHead           = 0b             RW  RW
      bx_bool so;                //  1 bit SchedulingOverrun           = 0b             RW  RW
    } HcInterruptEnable;         //                                    = 0x00000000
    struct {
      bx_bool mie;               //  1 bit MasterInterruptEnable       = 0b             RW  R
      bx_bool oc;                //  1 bit OwnershipChange             = 0b             RWC R
      Bit32u  reserved;          // 23 bit reserved                    = 0x000000       R   R
      bx_bool rhsc;              //  1 bit RootHubStatusChange         = 0b             RWC RW
      bx_bool fno;               //  1 bit FrameNumberOverflow         = 0b             RWC RW
      bx_bool ue;                //  1 bit UnrecoverableError          = 0b             RWC RW
      bx_bool rd;                //  1 bit ResumeDetected              = 0b             RWC RW
      bx_bool sf;                //  1 bit StartifFrame                = 0b             RWC RW
      bx_bool wdh;               //  1 bit WritebackDoneHead           = 0b             RWC RW
      bx_bool so;                //  1 bit SchedulingOverrun           = 0b             RWC RW
    } HcInterruptDisable;        //                                    = 0x00000000
    Bit32u HcHCCA;               // 32 bit addr HCCommunicationArea    = 0x00000000     RW  R
    struct {
      Bit32u  pced;              // 28 bit PeriodCurrentED             = 0x00000000     R   RW
      Bit8u   zero;              //  4 bit zero                        = 0000b          R   R
    } HcPeriodCurrentED;         //                                    = 0x00000000
    struct {
      Bit32u  ched;              // 28 bit ControlHeadED               = 0x00000000     RW  R
      Bit8u   zero;              //  4 bit zero                        = 0000b          R   R
    } HcControlHeadED;           //                                    = 0x00000000
    struct {
      Bit32u  cced;              // 28 bit ControlCurrentED            = 0x00000000     RW  R
      Bit8u   zero;              //  4 bit zero                        = 0000b          R   R
    } HcControlCurrentED;        //                                    = 0x00000000
    struct {
      Bit32u  bhed;              // 28 bit BulkHeadED                  = 0x00000000     RW  R
      Bit8u   zero;              //  4 bit zero                        = 0000b          R   R
    } HcBulkHeadED;              //                                    = 0x00000000
    struct {
      Bit32u  bced;              // 28 bit BulkCurrentED               = 0x00000000     RW  RW
      Bit8u   zero;              //  4 bit zero                        = 0000b          R   R
    } HcBulkCurrentED;           //                                    = 0x00000000
    struct {
      Bit32u  dh;                // 28 bit BulkCurrentED               = 0x00000000     RW  RW
      Bit8u   zero;              //  4 bit zero                        = 0000b          R   R
    } HcDoneHead;                //                                    = 0x00000000
    struct {
      bx_bool fit;               //  1 bit FrameIntervalToggle         = 0b             RW  R
      Bit16u  fsmps;             // 15 bit FSLargestDataPacket         = TBD (0)        RW  R
      Bit8u   reserved;          //  2 bit reserved                    = 00b            R   R
      Bit16u  fi;                // 14 bit FrameInterval               = 0x2EDF         RW  R
    } HcFmInterval;              //                                    = 0x00002EDF
    struct {
      bx_bool frt;               //  1 bit FrameRemainingToggle        = 0b             R   RW
      Bit8u   reserved;          // 17 bit reserved                    = 0x00000        R   R
      Bit16u  fr;                // 14 bit FrameRemaining              = 0x0000         RW  R
    } HcFmRemaining;             //                                    = 0x00000000
    struct {
      Bit16u  reserved;          // 16 bit reserved                    = 0x00000        R   R
      Bit16u  fn;                // 16 bit FrameNumber                 = 0x00000        R   RW
    } HcFmNumber;                //                                    = 0x00000000
    struct {
      Bit32u  reserved;          // 18 bit reserved                    = 0x00000        R   R
      Bit16u  ps;                // 14 bit PeriodicStart               = 0x00000        RW  R
    } HcPeriodicStart;           //                                    = 0x00000000
    struct {
      Bit32u  reserved;          // 22 bit reserved                    = 0x00000        R   R
      Bit16u  lst;               // 12 bit LSThreshold                 = 0x0628         RW  R
    } HcLSThreshold;             //                                    = 0x00000628
    struct {
      Bit8u   potpgt;            //  8 bit PowerOnToPowerGoodTime      = 0x10           RW  R
      Bit16u  reserved;          // 11 bit reserved                    = 0x000          R   R
      bx_bool nocp;              //  1 bit NoOverCurrentProtection     = 0b             RW  R
      bx_bool ocpm;              //  1 bit OverCurrentProtectionMode   = 1b             RW  R
      bx_bool dt;                //  1 bit DeviceType                  = 0b             R   R
      bx_bool nps;               //  1 bit NoPowerSwitching            = 0b             RW  R
      bx_bool psm;               //  1 bit PowerSwitchingMode          = 1b             RW  R
      Bit8u   ndp;               //  8 bit NumberDownstreamPorts       = NUMPORTS       RW  R
    } HcRhDescriptorA;           //                                    = 0x100009xx
    struct {
      Bit16u  ppcm;              // 16 bit PortPowerControlMask        = 0x0002         RW  R
      Bit16u  dr;                // 16 bit DeviceRemovable             = 0x0000         RW  R
    } HcRhDescriptorB;           //                                    = 0x00020000
    struct {
      bx_bool crwe;              //  1 bit ClearRemoteWakeupEnable     = 0b             WC  R
      Bit16u  reserved0;         // 13 bit reserved                    = 0x000000       R   R
      bx_bool ocic;              //  1 bit OverCurrentIndicatorChange  = 0b             RW  RW
      bx_bool lpsc;              //  1 bit LocalPowerStatusChange(r)   = 0b             RW  R
      bx_bool drwe;              //  1 bit DeviceRemoteWakeupEnable(r) = 0b             RW  R
      Bit16u  reserved1;         // 13 bit reserved                    = 0x000000       R   R
      bx_bool oci;               //  1 bit OverCurrentIndicator        = 0b             R   RW
      bx_bool lps;               //  1 bit LocalPowerStatus(r)         = 0b             RW  R
    } HcRhStatus;                //                                    = 0x00000000
    struct {
      Bit16u  reserved0;         // 11 bit reserved                    = 0x000000       R   R
      bx_bool prsc;              //  1 bit PortResetStatusChange       = 0b             RW  RW
      bx_bool ocic;              //  1 bit OverCurrentIndicatorChange  = 0b             RW  RW
      bx_bool pssc;              //  1 bit PortSuspendStatusChange     = 0b             RW  RW
      bx_bool pesc;              //  1 bit PortEnableStatusChange      = 0b             RW  RW
      bx_bool csc;               //  1 bit ConnectStatusChange         = 0b             RW  RW
      Bit8u   reserved1;         //  6 bit reserved                    = 0x00           R   R
      bx_bool lsda;              //  1 bit LowSpeedDeviceAttached      = 0b             RW  RW
      bx_bool pps;               //  1 bit PortPowerStatus             = 0b             RW  RW
      Bit8u   reserved2;         //  3 bit reserved                    = 0x0            R   R
      bx_bool prs;               //  1 bit PortResetStatus             = 0b             RW  RW
      bx_bool poci;              //  1 bit PortOverCurrentIndicator    = 0b             RW  RW
      bx_bool pss;               //  1 bit PortSuspendStatus           = 0b             RW  RW
      bx_bool pes;               //  1 bit PortEnableStatus            = 0b             RW  RW
      bx_bool ccs;               //  1 bit CurrentConnectStatus        = 0b             RW  RW
    } HcRhPortStatus[1003];      //                                    = 0x00000000
  } op_regs;

  struct {
    // our data
    usb_device_c *device;   // device connected to this port

  } usb_port[USB_NUM_PORTS];

  Bit8u pci_conf[256];
  Bit8u devfunc;

  int statusbar_id[2]; // IDs of the status LEDs
} bx_usb_ohci_t;



class bx_usb_ohci_c : public bx_pci_usb_stub_c {
public:
  bx_usb_ohci_c();
  virtual ~bx_usb_ohci_c();
  virtual void init(void);
  virtual void reset(unsigned);
  virtual bx_bool usb_mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state);
  virtual bx_bool usb_mouse_enabled_changed(bx_bool enable);
  virtual bx_bool usb_key_enq(Bit8u *scan_code);
  virtual void register_state(void);
  virtual void after_restore_state(void);
  virtual Bit32u  pci_read_handler(Bit8u address, unsigned io_len);
  virtual void    pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  static const char *usb_param_handler(bx_param_string_c *param, int set,
                                       const char *oldval, const char *val, int maxlen);

private:

  bx_usb_ohci_t hub;
  Bit8u         global_reset;
  bx_bool       busy;
  Bit8u         *device_buffer;

  usb_hid_device_c *mousedev;
  usb_hid_device_c *keybdev;

  USBPacket usb_packet;

  static void set_irq_level(bx_bool level);

  static void init_device(Bit8u port, const char *devname);
  static void usb_set_connect_status(Bit8u port, int type, bx_bool connected);

  static void usb_timer_handler(void *);
  void usb_timer(void);
  bx_bool DoTransfer(Bit32u address, Bit32u queue_num, struct TD *);
  void set_status(struct TD *td, bx_bool stalled, bx_bool data_buffer_error, bx_bool babble,
    bx_bool nak, bx_bool crc_time_out, bx_bool bitstuff_error, Bit16u act_len);
#if BX_USE_USB_OHCI_SMF
  static bx_bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bx_bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#else
  bx_bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  bx_bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#endif
  void usb_send_msg(usb_device_c *dev, int msg);
};

#endif  // BX_IODEV_USB_OHCI_H
