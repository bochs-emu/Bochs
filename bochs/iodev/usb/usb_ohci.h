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

#ifndef BX_IODEV_USB_OHCI_H
#define BX_IODEV_USB_OHCI_H

#if BX_USE_USB_OHCI_SMF
#  define BX_OHCI_THIS theUSB_OHCI->
#  define BX_OHCI_THIS_PTR theUSB_OHCI
#else
#  define BX_OHCI_THIS this->
#  define BX_OHCI_THIS_PTR this
#endif

// number of ports defined in bochs.h

#define OHCI_INTR_SO          (1<<0) // Scheduling overrun
#define OHCI_INTR_WD          (1<<1) // HcDoneHead writeback
#define OHCI_INTR_SF          (1<<2) // Start of frame
#define OHCI_INTR_RD          (1<<3) // Resume detect
#define OHCI_INTR_UE          (1<<4) // Unrecoverable error
#define OHCI_INTR_FNO         (1<<5) // Frame number overflow
#define OHCI_INTR_RHSC        (1<<6) // Root hub status change
#define OHCI_INTR_OC          (1<<30) // Ownership change
#define OHCI_INTR_MIE         (1<<31) // Master Interrupt Enable

// Completion Codes
enum {
  NoError = 0,
  CRC,
  BitStuffing,
  DataToggleMismatch,
  Stall,
  DeviceNotResponding,
  PIDCheckFailure,
  UnexpectedPID,
  DataOverrun,
  DataUnderrun,
  BufferOverrun = 0xC,
  BufferUnderrun,
  NotAccessed
};


#define ED_GET_MPS(x)     (((x)->dword0 & 0x07FF0000) >> 16)
#define ED_GET_F(x)       (((x)->dword0 & 0x00008000) >> 15)
#define ED_GET_K(x)       (((x)->dword0 & 0x00004000) >> 14)
#define ED_GET_S(x)       (((x)->dword0 & 0x00002000) >> 13)
#define ED_GET_D(x)       (((x)->dword0 & 0x00001800) >> 11)
#define ED_GET_EN(x)      (((x)->dword0 & 0x00000780) >>  7)
#define ED_GET_FA(x)      (((x)->dword0 & 0x0000007F) >>  0)
#define ED_GET_TAILP(x)    ((x)->dword1 & 0xFFFFFFF0)
#define ED_GET_HEADP(x)    ((x)->dword2 & 0xFFFFFFF0)
#define ED_SET_HEADP(x, y) ((x)->dword2 = ((x)->dword2 & 0x0000000F) | ((y) & 0xFFFFFFF0))
#define ED_GET_C(x)       (((x)->dword2 & 0x00000002) >>  1)
#define ED_SET_C(x,y)      ((x)->dword2 = ((x)->dword2 & ~0x00000002) | ((y)<<1))
#define ED_GET_H(x)       (((x)->dword2 & 0x00000001) >>  0)
#define ED_SET_H(x,y)      ((x)->dword2 = ((x)->dword2 & ~0x00000001) | ((y)<<0))
#define ED_GET_NEXTED(x)   ((x)->dword3 & 0xFFFFFFF0)

struct OHCI_ED {
  Bit32u   dword0;
  Bit32u   dword1;
  Bit32u   dword2;
  Bit32u   dword3;
};


#define TD_GET_R(x)       (((x)->dword0 & 0x00040000) >> 18)
#define TD_GET_DP(x)      (((x)->dword0 & 0x00180000) >> 19)
#define TD_GET_DI(x)      (((x)->dword0 & 0x00E00000) >> 21)
#define TD_GET_T(x)       (((x)->dword0 & 0x03000000) >> 24)
#define TD_SET_T(x,y)      ((x)->dword0 = ((x)->dword0 & ~0x03000000) | ((y)<<24))
#define TD_GET_EC(x)      (((x)->dword0 & 0x0C000000) >> 26)
#define TD_SET_EC(x,y)     ((x)->dword0 = ((x)->dword0 & ~0x0C000000) | ((y)<<26))
#define TD_GET_CC(x)      (((x)->dword0 & 0xF0000000) >> 28)
#define TD_SET_CC(x,y)     ((x)->dword0 = ((x)->dword0 & ~0xF0000000) | ((y)<<28))
#define TD_GET_CBP(x)      ((x)->dword1)
#define TD_SET_CBP(x,y)    ((x)->dword1 = y)
#define TD_GET_NEXTTD(x)   ((x)->dword2 & 0xFFFFFFF0)
#define TD_SET_NEXTTD(x,y) ((x)->dword2 = (y & 0xFFFFFFF0))
#define TD_GET_BE(x)       ((x)->dword3)

struct OHCI_TD {
  Bit32u   dword0;
  Bit32u   dword1;
  Bit32u   dword2;
  Bit32u   dword3;
};


#define ISO_TD_GET_CC(x)      (((x)->dword0 & 0xF0000000) >> 28)
#define ISO_TD_GET_FC(x)      (((x)->dword0 & 0x0F000000) >> 24)
#define ISO_TD_GET_DI(x)      (((x)->dword0 & 0x00E00000) >> 21)
#define ISO_TD_GET_SF(x)       ((x)->dword0 & 0x0000FFFF)
#define ISO_TD_GET_BP0(x)     (((x)->dword1 & 0xFFFFF000) >> 12)
#define ISO_TD_GET_NEXTTD(x)   ((x)->dword2 & 0xFFFFFFF0)
#define ISO_TD_GET_BE(x)       ((x)->dword3)
#define ISO_TD_GET_PSW0(x)     ((x)->dword4 & 0x0000FFFF)
#define ISO_TD_GET_PSW1(x)    (((x)->dword4 & 0xFFFF0000) >> 16)
#define ISO_TD_GET_PSW2(x)     ((x)->dword5 & 0x0000FFFF)
#define ISO_TD_GET_PSW3(x)    (((x)->dword5 & 0xFFFF0000) >> 16)
#define ISO_TD_GET_PSW4(x)     ((x)->dword6 & 0x0000FFFF)
#define ISO_TD_GET_PSW5(x)    (((x)->dword6 & 0xFFFF0000) >> 16)
#define ISO_TD_GET_PSW6(x)     ((x)->dword7 & 0x0000FFFF)
#define ISO_TD_GET_PSW7(x)    (((x)->dword7 & 0xFFFF0000) >> 16)

struct OHCI_ISO_TD {
  Bit32u   dword0;
  Bit32u   dword1;
  Bit32u   dword2;
  Bit32u   dword3;
  Bit32u   dword4;
  Bit32u   dword5;
  Bit32u   dword6;
  Bit32u   dword7;
};


typedef struct {
  int   frame_timer_index;

  struct OHCI_OP_REGS {
    Bit16u HcRevision;
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
    Bit32u HcInterruptStatus;
    Bit32u HcInterruptEnable;
    Bit32u HcHCCA;
    Bit32u HcPeriodCurrentED;
    Bit32u HcControlHeadED;
    Bit32u HcControlCurrentED;
    Bit32u HcBulkHeadED;
    Bit32u HcBulkCurrentED;
    Bit32u HcDoneHead;
    struct {
      bx_bool fit;               //  1 bit FrameIntervalToggle         = 0b             RW  R
      Bit16u  fsmps;             // 15 bit FSLargestDataPacket         = TBD (0)        RW  R
      Bit8u   reserved;          //  2 bit reserved                    = 00b            R   R
      Bit16u  fi;                // 14 bit FrameInterval               = 0x2EDF         RW  R
    } HcFmInterval;              //                                    = 0x00002EDF
    bx_bool HcFmRemainingToggle; //  1 bit FrameRemainingToggle        = 0b             R   RW
    Bit32u HcFmNumber;
    Bit32u HcPeriodicStart;
    Bit16u HcLSThreshold;
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
  } op_regs;

  struct {
    // our data
    usb_device_c *device;   // device connected to this port

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
    } HcRhPortStatus;
  } usb_port[BX_N_USB_OHCI_PORTS];

  Bit8u devfunc;
  unsigned ohci_done_count;
  bx_bool  use_control_head;
  bx_bool  use_bulk_head;
  Bit64u   sof_time;

  int statusbar_id; // ID of the status LEDs
  Bit8u device_change;
} bx_usb_ohci_t;



class bx_usb_ohci_c : public bx_devmodel_c, public bx_pci_device_stub_c {
public:
  bx_usb_ohci_c();
  virtual ~bx_usb_ohci_c();
  virtual void init(void);
  virtual void reset(unsigned);
  virtual void register_state(void);
  virtual void after_restore_state(void);
  virtual Bit32u  pci_read_handler(Bit8u address, unsigned io_len);
  virtual void    pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  static const char *usb_param_handler(bx_param_string_c *param, int set,
                                       const char *oldval, const char *val, int maxlen);

private:

  bx_usb_ohci_t hub;
  Bit8u         *device_buffer;

  USBPacket usb_packet;

  static void reset_hc();
  static void reset_port(int);
  static void update_irq();
  static void set_interrupt(Bit32u value);

  static void init_device(Bit8u port, bx_list_c *portconf);
  static void remove_device(Bit8u port);
  static int  broadcast_packet(USBPacket *p);
  static void usb_set_connect_status(Bit8u port, int type, bx_bool connected);

  static void usb_frame_handler(void *);
  void usb_frame_timer(void);

  static Bit32u get_frame_remaining(void);

  void process_ed(struct OHCI_ED *, const Bit32u);
  bx_bool process_td(struct OHCI_TD *, struct OHCI_ED *);

#if BX_USE_USB_OHCI_SMF
  static bx_bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bx_bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#else
  bx_bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  bx_bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#endif

  static void runtime_config_handler(void *);
  void runtime_config(void);
};

#endif  // BX_IODEV_USB_OHCI_H
