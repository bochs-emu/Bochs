/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2026  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2009-2026  The Bochs Project
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

#ifndef BX_IODEV_OHCI_CORE_H
#define BX_IODEV_OHCI_CORE_H

#define USB_OHCI_PORTS 2

// HCFS values
#define  OHCI_USB_RESET       0x00
#define  OHCI_USB_RESUME      0x01
#define  OHCI_USB_OPERATIONAL 0x02
#define  OHCI_USB_SUSPEND     0x03

#define OHCI_INTR_SO          (1<<0) // Scheduling overrun
#define OHCI_INTR_WD          (1<<1) // HcDoneHead writeback
#define OHCI_INTR_SF          (1<<2) // Start of frame
#define OHCI_INTR_RD          (1<<3) // Resume detect
#define OHCI_INTR_UE          (1<<4) // Unrecoverable error
#define OHCI_INTR_FNO         (1<<5) // Frame number overflow
#define OHCI_INTR_RHSC        (1<<6) // Root hub status change
#define OHCI_INTR_OC          (1<<30) // Ownership change
#define OHCI_INTR_MIE         (1<<31) // Master Interrupt Enable

// HcControl
#define OHCI_CTL_CBSR         (3<<0)  // ControlBulkServiceRatio
#define OHCI_CTL_PLE          (1<<2)  // PeriodicListEnable
#define OHCI_CTL_IE           (1<<3)  // IsochronousEnable
#define OHCI_CTL_CLE          (1<<4)  // ControlListEnable
#define OHCI_CTL_BLE          (1<<5)  // BulkListEnable
#define OHCI_CTL_HCFS_SH      6       // HostControllerFunctionalState
#define OHCI_CTL_HCFS         (3<<OHCI_CTL_HCFS_SH)
#define OHCI_CTL_IR           (1<<8)  // InterruptRouting
#define OHCI_CTL_RWC          (1<<9)  // RemoteWakeupConnected
#define OHCI_CTL_RWE          (1<<10) // RemoteWakeupEnable
#define OHCI_CTL_RW_MASK      0x000007FF
#define OHCI_GET_HCFS(x)      (((x) & OHCI_CTL_HCFS) >> OHCI_CTL_HCFS_SH)
#define OHCI_SET_HCFS(x,y)    ((x) = ((x) & ~OHCI_CTL_HCFS) | ((y) << OHCI_CTL_HCFS_SH))

// HcCommandStatus
#define OHCI_CS_HCR           (1<<0)  // HostControllerReset
#define OHCI_CS_CLF           (1<<1)  // ControlListFilled
#define OHCI_CS_BLF           (1<<2)  // BulkListFilled
#define OHCI_CS_OCR           (1<<3)  // OwnershipChangeRequest
#define OHCI_CS_SOC           (3<<16) // SchedulingOverrunCount

// HcFmInterval
#define OHCI_FMI_FI           0x00003FFF  // FrameInterval
#define OHCI_FMI_FSMPS        0x7FFF0000  // FSLargestDataPacket
#define OHCI_FMI_FIT          0x80000000  // FrameIntervalToggle
#define OHCI_FMI_RW_MASK      (OHCI_FMI_FIT | OHCI_FMI_FSMPS | OHCI_FMI_FI)

// HcRhDescriptorA
#define OHCI_RHA_NDP          0x000000FF  // NumberDownstreamPorts
#define OHCI_RHA_PSM          (1<<8)  // PowerSwitchingMode
#define OHCI_RHA_NPS          (1<<9)  // NoPowerSwitching
#define OHCI_RHA_DT           (1<<10) // DeviceType
#define OHCI_RHA_OCPM         (1<<11) // OverCurrentProtectionMode
#define OHCI_RHA_NOCP         (1<<12) // NoOverCurrentProtection
#define OHCI_RHA_POTPGT_SH    24      // PowerOnToPowerGoodTime
#define OHCI_RHA_POTPGT       (0xFFU<<OHCI_RHA_POTPGT_SH)
#define OHCI_RHA_RW_MASK      (OHCI_RHA_POTPGT | OHCI_RHA_NOCP | OHCI_RHA_OCPM | OHCI_RHA_NPS | OHCI_RHA_PSM)

// HcRhDescriptorB
#define OHCI_RHB_PPCM_SH      16      // PortPowerControlMask (DeviceRemovable in bits 15:0)

// HcRhStatus
#define OHCI_RHS_LPS          (1<<0)  // LocalPowerStatus
#define OHCI_RHS_OCI          (1<<1)  // OverCurrentIndicator
#define OHCI_RHS_DRWE         (1<<15) // DeviceRemoteWakeupEnable
#define OHCI_RHS_LPSC         (1<<16) // LocalPowerStatusChange
#define OHCI_RHS_OCIC         (1<<17) // OverCurrentIndicatorChange
#define OHCI_RHS_CRWE         (1U<<31) // ClearRemoteWakeupEnable

// HcRhPortStatus
#define OHCI_PS_CCS           (1<<0)  // CurrentConnectStatus
#define OHCI_PS_PES           (1<<1)  // PortEnableStatus
#define OHCI_PS_PSS           (1<<2)  // PortSuspendStatus
#define OHCI_PS_POCI          (1<<3)  // PortOverCurrentIndicator
#define OHCI_PS_PRS           (1<<4)  // PortResetStatus
#define OHCI_PS_PPS           (1<<8)  // PortPowerStatus
#define OHCI_PS_LSDA          (1<<9)  // LowSpeedDeviceAttached
#define OHCI_PS_CSC           (1<<16) // ConnectStatusChange
#define OHCI_PS_PESC          (1<<17) // PortEnableStatusChange
#define OHCI_PS_PSSC          (1<<18) // PortSuspendStatusChange
#define OHCI_PS_OCIC          (1<<19) // OverCurrentIndicatorChange
#define OHCI_PS_PRSC          (1<<20) // PortResetStatusChange

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
  int   timer_index;

  struct OHCI_OP_REGS {
    Bit16u HcRevision;
    Bit32u HcControl;
    Bit32u HcCommandStatus;
    Bit32u HcInterruptStatus;
    Bit32u HcInterruptEnable;
    Bit32u HcHCCA;
    Bit32u HcPeriodCurrentED;
    Bit32u HcControlHeadED;
    Bit32u HcControlCurrentED;
    Bit32u HcBulkHeadED;
    Bit32u HcBulkCurrentED;
    Bit32u HcDoneHead;
    Bit32u HcFmInterval;
    bool   HcFmRemainingToggle; //  1 bit FrameRemainingToggle        = 0b             R   RW
    Bit32u HcFmNumber;
    Bit32u HcPeriodicStart;
    Bit16u HcLSThreshold;
    Bit32u HcRhDescriptorA;
    Bit32u HcRhDescriptorB;
    Bit32u HcRhStatus;
  } op_regs;

  struct {
    // our data
    usb_device_c *device;   // device connected to this port
    Bit32u HcRhPortStatus;
  } usb_port[USB_OHCI_PORTS];

  Bit8u devfunc;
  unsigned ohci_done_count;
  bool     use_control_head;
  bool     use_bulk_head;
  Bit64u   sof_time;
} bx_ohci_core_t;

class bx_ohci_core_c : public bx_pci_device_c {
public:
  bx_ohci_core_c();
  virtual ~bx_ohci_core_c();
  virtual void init_ohci(Bit8u devfunc, Bit16u venid, Bit16u devid, Bit8u rev, Bit8u headt, Bit8u intp);
  virtual void reset_ohci(unsigned);
  void    ohci_register_state(const char *pname, bx_list_c *parent);
  virtual void after_restore_state(void);
  virtual void set_port_device(int port, usb_device_c *dev);
  virtual void pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  int event_handler(int event, void *ptr, int port);

#if BX_USB_DEBUGGER
  void enable_usbdbg() {usb_debug = true;}
#endif

protected:
  bx_ohci_core_t hub;
#if BX_USB_DEBUGGER
  bool           usb_debug;
#endif

  USBAsync *packets;

  void reset_hc();
  void reset_port(int);
  void update_irq();
  void set_interrupt(Bit32u value);

  int  broadcast_packet(USBPacket *p);
  bool set_connect_status(Bit8u port, bool connected);

  static void ohci_timer_handler(void *);
  void ohci_timer(void);

  Bit32u get_frame_remaining(void);

  void process_lists();
  bool process_ed(struct OHCI_ED *, const Bit32u);
  int  process_td(struct OHCI_TD *, struct OHCI_ED *, int);

  static bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  bool mem_read(bx_phy_address addr, unsigned len, void *data);
  bool mem_write(bx_phy_address addr, unsigned len, void *data);
};

#endif
