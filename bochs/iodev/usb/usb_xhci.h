/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2010-2014  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2011-2014  The Bochs Project
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

#ifndef BX_IODEV_USB_XHCI_H
#define BX_IODEV_USB_XHCI_H

#if BX_USE_USB_XHCI_SMF
#  define BX_XHCI_THIS theUSB_XHCI->
#  define BX_XHCI_THIS_PTR theUSB_XHCI
#else
#  define BX_XHCI_THIS this->
#  define BX_XHCI_THIS_PTR this
#endif

// If in 64bit mode, print 64bits, else only print 32 bit addresses
#if BX_PHY_ADDRESS_LONG
  #define FORMATADDRESS   FMT_ADDRX64
#else
  #define FORMATADDRESS   "%08X"
#endif

/************************************************************************************************
 * Actual configuration of the card
 */
#define IO_SPACE_SIZE     8192

#define OPS_REGS_OFFSET   0x20
// Change this to 0.95, 0.96, 1.00, 1.10, according to the desired effects (LINK chain bit, etc)
//   uPD720202 is 1.00
#define VERSION_MAJOR     0x01
#define VERSION_MINOR     0x00

// HCSPARAMS1
#define MAX_SLOTS           32   // (1 based)
#define INTERRUPTERS         8   //
#define USB_XHCI_PORTS       4   // physical sockets, each supporting USB3 or USB2 (0x08 = uPD720201, 0x04 = uPD720202)

#if (USB_XHCI_PORTS != 4)
  #error "USB_XHCI_PORTS must equal 4"
#endif

// HCSPARAMS2
#define ISO_SECH_THRESHOLD   1
#define MAX_SEG_TBL_SZ_EXP   1
#define SCATCH_PAD_RESTORE   1  // 1 = uses system memory and must be maintained.  0 = uses controller's internal memory
#define MAX_SCRATCH_PADS     4  // 0 to 1023

// HCSPARAMS3
#define U1_DEVICE_EXIT_LAT   0
#define U2_DEVICE_EXIT_LAT   0

// HCCPARAMS1
#define ADDR_CAP_64              1
#define BW_NEGOTIATION           1
#define CONTEXT_SIZE            64  // Size of the CONTEXT blocks (32 or 64)
#define PORT_POWER_CTRL          1
#define PORT_INDICATORS          0
#define LIGHT_HC_RESET           0  // Do we support the Light HC Reset function
#define LAT_TOL_MSGING_CAP       1  // Latency Tolerance Messaging Capability (v1.00+)
#define NO_SSD_SUPPORT           1  // No Secondary SID Support (v1.00+)
#define PARSE_ALL_EVENT          1  // version 0.96 and below only (MUST BE 1 in v1.00+)
#define SEC_DOMAIN_BAND          1  // version 0.96 and below only (MUST BE 1 in v1.00+)
#define STOPPED_EDTLA            0
#define CONT_FRAME_ID            0
#define MAX_PSA_SIZE          0x05
#define EXT_CAPS_OFFSET      0x500
  #define EXT_CAPS_SIZE        144

// HCCPARAMS2 (v1.10+)
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  #define U3_ENTRY_CAP             0
  #define CONFIG_EP_CMND_CAP       0
  #define FORCE_SAVE_CONTEXT_CAP   0
  #define COMPLNC_TRANS_CAP        0
  #define LARGE_ESIT_PAYLOAD_CAP   0
  #define CONFIG_INFO_CAP          0
#endif

#define XHCI_PAGE_SIZE    1  // Page size operational register value

#define DOORBELL_OFFSET   0x800

#define RUNTIME_OFFSET    0x600

#define PORT_SET_OFFSET  (0x400 + OPS_REGS_OFFSET)

/************************************************************************************************/

#if ((VERSION_MAJOR > 1) ||                                                              \
    ((VERSION_MAJOR == 0) && ((VERSION_MINOR != 0x95) && (VERSION_MINOR != 0x96))) ||    \
    ((VERSION_MAJOR == 1) && ((VERSION_MINOR != 0x00) && (VERSION_MINOR != 0x10))))
#  error "Unknown Controller Version number specified."
#endif

#if (MAX_SCRATCH_PADS > 1023)
#  error "MAX_SCRATCH_PADS must be 0 to 1023."
#endif

#if (SCATCH_PAD_RESTORE && (MAX_SCRATCH_PADS == 0))
#  error "Must specify amount of scratch pad buffers to use."
#endif

#if ((MAX_SCRATCH_PADS > 0) && !SCATCH_PAD_RESTORE)
#  error "Must set SCATCH_PAD_RESTORE to 1 if MAX_SCRATCH_PADS > 0"
#endif

#if ((PARSE_ALL_EVENT == 0) && (VERSION_MAJOR > 0))
#  error "PARSE_ALL_EVENT must be 1 in version 1.0 and above"
#endif

#if ((SEC_DOMAIN_BAND == 0) && (VERSION_MAJOR > 0))
#  error "SEC_DOMAIN_BAND must be 1 in version 1.0 and above"
#endif

#if ((LAT_TOL_MSGING_CAP == 1) && ((VERSION_MAJOR < 1) || (VERSION_MINOR < 0)))
#  error "LAT_TOL_MSGING_CAP must be used with in version 1.1 and above"
#endif

#if ((NO_SSD_SUPPORT == 1) && ((VERSION_MAJOR < 1) || (VERSION_MINOR < 0)))
#  error "NO_SSD_SUPPORT must be used with in version 1.1 and above"
#endif

// Each controller supports its own number of ports.  We must adhere to that for now.
//  (The Extended Capabilities register set is hardcoded for this as of now.)
//  As long as BX_N_USB_XHCI_PORTS was defined as greater than what we have now, then
//  we are fine.
// Note: BX_N_USB_XHCI_PORTS should have been defined as twice the amount of ports wanted.
//  ie.: Each physical port (socket) has two defined port register sets.  One for USB3, one for USB2
// Only one port type may be used at a time.  Port0 or Port1, not both.  If Port0 is used, then
//  Port1 must be vacant.
#ifndef BX_N_USB_XHCI_PORTS
  #error "BX_N_USB_XHCI_PORTS was not defined in bochs.h"
#else
  #if (BX_N_USB_XHCI_PORTS < USB_XHCI_PORTS)
    #error "BX_N_USB_XHCI_PORTS is less than USB_XHCI_PORTS."
  #endif
#endif

// xHCI speed values
#define SPEED_FULL   1
#define SPEED_LOW    2
#define SPEED_HI     3
#define SPEED_SUPER  4

#define USB2 0
#define USB3 1

// our saved ring members
struct RING_MEMBERS {
  struct {
    Bit64u  dq_pointer;
    bx_bool rcs;
  } command_ring;
  struct {
    bx_bool  rcs;
    unsigned trb_count;
    unsigned count;
    Bit64u   cur_trb;
    struct {
      Bit64u addr;
      Bit32u size;
      Bit32u resv;
    } entrys[(1<<MAX_SEG_TBL_SZ_EXP)];
  } event_rings[INTERRUPTERS];
};

struct SLOT_CONTEXT {
  unsigned entries;
  bx_bool  hub;
  bx_bool  mtt;
  unsigned speed;
  Bit32u   route_string;
  unsigned num_ports;
  unsigned rh_port_num;
  unsigned max_exit_latency;
  unsigned int_target;
  unsigned ttt;
  unsigned tt_port_num;
  unsigned tt_hub_slot_id;
  unsigned slot_state;
  unsigned device_address;
};

struct EP_CONTEXT {
  unsigned interval;
  bx_bool  lsa;
  unsigned max_pstreams;
  unsigned mult;
  unsigned ep_state;
  unsigned max_packet_size;
  unsigned max_burst_size;
  bx_bool  hid;
  unsigned ep_type;
  unsigned cerr;
  Bit64u   tr_dequeue_pointer;
  bx_bool  dcs;
  unsigned max_esit_payload;
  unsigned average_trb_len;
};

struct HC_SLOT_CONTEXT {
  bx_bool enabled;
  bx_bool sent_address;  // have we sent a SET_ADDRESS command yet?
  struct SLOT_CONTEXT slot_context;
  struct {
    struct EP_CONTEXT   ep_context;
    // our internal registers follow
    Bit32u edtla;
    Bit64u enqueue_pointer;
    bx_bool  rcs;
  } ep_context[32];  // first one is ignored by controller.
};

// TRB Types
enum { NORMAL=1, SETUP_STAGE, DATA_STAGE, STATUS_STAGE, ISOCH, LINK, EVENT_DATA, NO_OP,
       ENABLE_SLOT=9, DISABLE_SLOT, ADDRESS_DEVICE, CONFIG_EP, EVALUATE_CONTEXT, RESET_EP,
       STOP_EP=15, SET_TR_DEQUEUE, RESET_DEVICE, FORCE_EVENT, DEG_BANDWIDTH, SET_LAT_TOLERANCE,
       GET_PORT_BAND=21, FORCE_HEADER, NO_OP_CMD,  // 24 - 31 = reserved
       TRANS_EVENT=32, COMMAND_COMPLETION, PORT_STATUS_CHANGE, BANDWIDTH_REQUEST, DOORBELL_EVENT,
       HOST_CONTROLLER_EVENT=37, DEVICE_NOTIFICATION, MFINDEX_WRAP, 
       // 40 - 47 = reserved
       // 48 - 63 = Vendor Defined
};

// event completion codes
enum { TRB_SUCCESS=1, DATA_BUFFER_ERROR, BABBLE_DETECTION, TRANSACTION_ERROR, TRB_ERROR, STALL_ERROR,
       RESOURCE_ERROR=7, BANDWIDTH_ERROR, NO_SLOTS_ERROR, INVALID_STREAM_TYPE, SLOT_NOT_ENABLED, EP_NOT_ENABLED,
       SHORT_PACKET=13, RING_UNDERRUN, RUNG_OVERRUN, VF_EVENT_RING_FULL, PARAMETER_ERROR, BANDWITDH_OVERRUN,
       CONTEXT_STATE_ERROR=19, NO_PING_RESPONSE, EVENT_RING_FULL, INCOMPATIBLE_DEVICE, MISSED_SERVICE,
       COMMAND_RING_STOPPED=24, COMMAND_ABORTED, STOPPED, STOPPER_LENGTH_ERROR, RESERVED, ISOCH_BUFFER_OVERRUN,
       EVERN_LOST=32, UNDEFINED, INVALID_STREAM_ID, SECONDARY_BANDWIDTH, SPLIT_TRANSACTION
       /* 37 - 191 reserved */
       /* 192 - 223 vender defined errors */
       /* 224 - 225 vendor defined info */
};

// Port Link States
enum { PLS_U0 = 0, PLS_U1, PLS_U2, PLS_U3_SUSPENDED, PLS_DISABLED, PLS_RXDETECT, PLS_INACTIVE, PLS_POLLING,
       PLS_RECOVERY = 8, PLS_HOT_RESET, PLS_COMPLIANCE, PLS_TEST_MODE,
       /* 12 - 14 reserved */
       PLS_RESUME = 15
};


// Reset type
#define HOT_RESET   0
#define WARM_RESET  1

// Direction
#define EP_DIR_OUT  0
#define EP_DIR_IN   1

// Slot State
#define SLOT_STATE_DISABLED_ENABLED  0
#define SLOT_STATE_DEFAULT           1
#define SLOT_STATE_ADRESSED          2
#define SLOT_STATE_CONFIGURED        3

// EP State
#define EP_STATE_DISABLED 0
#define EP_STATE_RUNNING  1
#define EP_STATE_HALTED   2
#define EP_STATE_STOPPED  3
#define EP_STATE_ERROR    4

// NEC Vendor specific TRB types
#define NEC_TRB_TYPE_CMD_COMP 48
#define NEC_TRB_TYPE_GET_FW   49
#define NEC_TRB_TYPE_GET_UN   50
  #define NEC_MAGIC           0x49434878
#define NEC_FW_MAJOR(v)       (((v) & 0x0000FF00) >> 8)
#define NEC_FW_MINOR(v)       (((v) & 0x000000FF) >> 0)

#define TRB_GET_STYPE(x)     (((x) & (0x1F << 16)) >> 16)
#define TRB_SET_STYPE(x)     (((x) & 0x1F) << 16)
#define TRB_GET_TYPE(x)      (((x) & (0x3F << 10)) >> 10)
#define TRB_SET_TYPE(x)      (((x) & 0x3F) << 10)
#define TRB_GET_COMP_CODE(x) (((x) & (0xFF << 24)) >> 24)
#define TRB_SET_COMP_CODE(x) (((x) & 0xFF) << 24)
#define TRB_GET_SLOT(x)      (((x) & (0xFF << 24)) >> 24)
#define TRB_SET_SLOT(x)      (((x) & 0xFF) << 24)
#define TRB_GET_TDSIZE(x)    (((x) & (0x1F << 17)) >> 17)
#define TRB_SET_TDSIZE(x)    (((x) & 0x1F) << 17)
#define TRB_GET_EP(x)        (((x) & (0x1F << 16)) >> 16)
#define TRB_SET_EP(x)        (((x) & 0x1F) << 16)

#define TRB_GET_TARGET(x)    (((x) & (0x3FF << 22)) >> 22)
#define TRB_GET_TX_LEN(x)     ((x) & 0x1FFFF)
#define TRB_GET_TOGGLE(x)    (((x) & (1<<1)) >> 1)

#define TRB_DC(x)            (((x) & (1<<9)) >> 9)
#define TRB_IS_IMMED_DATA(x) (((x) & (1<<6)) >> 6)
#define TRB_IOC(x)           (((x) & (1<<5)) >> 5)
#define TRB_CHAIN(x)         (((x) & (1<<4)) >> 4)
#define TRB_SPD(x)           (((x) & (1<<2)) >> 2)
#define TRB_TOGGLE(x)        (((x) & (1<<1)) >> 1)
#define TRB_TX_TYPE(x)       (((x) == 2) ? USB_TOKEN_OUT : USB_TOKEN_IN)
#define TRB_GET_DIR(x)       (((x) & (1<<16)) ? USB_TOKEN_IN : USB_TOKEN_OUT)

struct TRB {
  Bit64u parameter;
  Bit32u status;
  Bit32u command;
};

typedef struct {

  struct XHCI_CAP_REGS {
    Bit32u HcCapLength;
    Bit32u HcSParams1;
    Bit32u HcSParams2;
    Bit32u HcSParams3;
    Bit32u HcCParams1;
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 1))
    Bit32u HcCParams2;
#endif
    Bit32u DBOFF;
    Bit32u RTSOFF;
  } cap_regs;

  struct XHCI_OP_REGS {
    struct {
      Bit32u  RsvdP1;            // 18/20 bit reserved and preserved   = 0x000000       RW
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
      bx_bool cme;               //  1 bit Max Exit Latecy to Large    = 0b             RW
      bx_bool spe;               //  1 bit Generate Short Packet Comp  = 0b             RW
#endif
      bx_bool eu3s;              //  1 bit Enable U3 MFINDEX Stop      = 0b             RW
      bx_bool ewe;               //  1 bit Enable Wrap Event           = 0b             RW
      bx_bool crs;               //  1 bit Controller Restore State    = 0b             RW
      bx_bool css;               //  1 bit Controller Save State       = 0b             RW
      bx_bool lhcrst;            //  1 bit Light HC Reset              = 0b             RW or RO (HCCPARAMS:LHRC)
      Bit8u   RsvdP0;            //  1 bit reserved and preserved      = 000b           RW
      bx_bool hsee;              //  1 bit Host System Error Enable    = 0b             RW
      bx_bool inte;              //  1 bit Interrupter Enable          = 0b             RW
      bx_bool hcrst;             //  1 bit HC Reset                    = 0b             RW
      bx_bool rs;                //  1 bit Run Stop                    = 0b             RW
    } HcCommand;                 //                                    = 0x00000000
    struct {
      Bit32u  RsvdZ1;            // 19 bit reserved and zero'd         = 0x000000       RW
      bx_bool hce;               //  1 bit Host Controller Error       = 0b             RO
      bx_bool cnr;               //  1 bit Controller Not Ready        = 0b             R0
      bx_bool sre;               //  1 bit Save/Restore Error          = 0b             RW1C
      bx_bool rss;               //  1 bit Restore State Status        = 0b             RO
      bx_bool sss;               //  1 bit Save State Status           = 0b             RO
      Bit8u   RsvdZ0;            //  3 bit reserved and zero'd         = 0x0            RW
      bx_bool pcd;               //  1 bit Port Change Detect          = 0b             RW1C
      bx_bool eint;              //  1 bit Event Interrupt             = 0b             RW1C
      bx_bool hse;               //  1 bit Host System Error           = 0b             RW1C
      bx_bool RsvdZ2;            //  1 bit reserved and zero'd         = 0b             RW
      bx_bool hch;               //  1 bit HCHalted                    = 1b             RO
    } HcStatus;                  //                                    = 0x00000001
    struct {
      Bit16u  Rsvd;              // 16 bit reserved                    = 0x0000         RO
      Bit16u  pagesize;          // 16 bit reserved                    = 0x0001         RO
    } HcPageSize;                //                                    = 0x00000001
    struct {
      Bit16u  RsvdP;             // 16 bit reserved and presserved     = 0x0000         RW
      bx_bool n15;               //  1 bit N15                         = 0              RW
      bx_bool n14;               //  1 bit N14                         = 0              RW
      bx_bool n13;               //  1 bit N13                         = 0              RW
      bx_bool n12;               //  1 bit N12                         = 0              RW
      bx_bool n11;               //  1 bit N11                         = 0              RW
      bx_bool n10;               //  1 bit N10                         = 0              RW
      bx_bool n9;                //  1 bit N9                          = 0              RW
      bx_bool n8;                //  1 bit N8                          = 0              RW
      bx_bool n7;                //  1 bit N7                          = 0              RW
      bx_bool n6;                //  1 bit N6                          = 0              RW
      bx_bool n5;                //  1 bit N5                          = 0              RW
      bx_bool n4;                //  1 bit N4                          = 0              RW
      bx_bool n3;                //  1 bit N3                          = 0              RW
      bx_bool n2;                //  1 bit N2                          = 0              RW
      bx_bool n1;                //  1 bit N1                          = 0              RW
      bx_bool n0;                //  1 bit N0                          = 0              RW
    } HcNotification;            //                                    = 0x00000000
    struct {
      Bit64u  crc;               // 64 bit hi order address            = 0x00000000     RW
      Bit8u   RsvdP;             //  2 bit reserved and preserved      = 00b            RW
      bx_bool crr;               //  1 bit Command Ring Running        = 0              RO
      bx_bool ca;                //  1 bit Command Abort               = 0              RW1S
      bx_bool cs;                //  1 bit Command Stop                = 0              RW1S
      bx_bool rcs;               //  1 bit Ring Cycle State            = 0              RW
    } HcCrcr;
    struct {
      Bit64u  dcbaap;            // 64 bit hi order address            = 0x00000000     RW
      Bit8u   RsvdZ;             //  6 bit reserved and zero'd         = 000000b        RW
    } HcDCBAAP;
    struct {
      Bit32u  RsvdP;             // 22/24 bit reserved and preserved   = 0x000000       RW
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
      bx_bool u3e;               //  1 bit U3 Entry Enable             = 0              RW
      bx_bool cie;               //  1 bit Config Info Enable          = 0              RW
#endif
      Bit8u   MaxSlotsEn;        //  8 bit Max Device Slots Enabled    = 0x00           RW
    } HcConfig;
  } op_regs;

  struct {
    // our data
    usb_device_c *device;   // device connected to this port
    bx_bool is_usb3;        // set if usb3 port, cleared if usb2 port.
    bx_bool has_been_reset; // set if the port has been reset aftet powered up.

    struct {
      bx_bool wpr;               //  1 bit Warm Port Reset             = 0b             RW or RsvdZ
      bx_bool dr;                //  1 bit Device Removable            = 0b             RO
      Bit8u   RsvdZ1;            //  2 bit Reserved and Zero'd         = 00b            RW
      bx_bool woe;               //  1 bit Wake on Over Current Enable = 0b             RW
      bx_bool wde;               //  1 bit Wake on Disconnect Enable   = 0b             RW
      bx_bool wce;               //  1 bit Wake on Connect Enable      = 0b             RW
      bx_bool cas;               //  1 bit Cold Attach Status          = 0b             RO
      bx_bool cec;               //  1 bit Port Config Error Change    = 0b             RW1C or RsvdZ
      bx_bool plc;               //  1 bit Port Link State Change      = 0b             RW1C
      bx_bool prc;               //  1 bit Port Reset Change           = 0b             RW1C
      bx_bool occ;               //  1 bit Over Current Change         = 0b             RW1C
      bx_bool wrc;               //  1 bit Warm Port Reset Change      = 0b             RW1C or RsvdZ
      bx_bool pec;               //  1 bit Port Enabled/Disabled Change= 0b             RW1C
      bx_bool csc;               //  1 bit Connect Status Change       = 0b             RW1C
      bx_bool lws;               //  1 bit Port Link State Write Strobe= 0b             RW
      Bit8u   pic;               //  2 bit Port Indicator Control      = 00b            RW
      Bit8u   speed;             //  4 bit Port Speed                  = 0000b          RO
      bx_bool pp;                //  1 bit Port Power                  = 0b             RW
      Bit8u   pls;               //  4 bit Port Link State             = 0x00           RW
      bx_bool pr;                //  1 bit Port Reset                  = 0b             RW
      bx_bool oca;               //  1 bit Over Current Active         = 0b             RO
      bx_bool RsvdZ0;            //  1 bit Reserved and Zero'd         = 0b             RW
      bx_bool ped;               //  1 bit Port Enabled/Disabled       = 0b             RW1C
      bx_bool ccs;               //  1 bit Current Connect Status      = 0b             RO
    } portsc;
    union {
      // if usb3 port
      struct {
        struct {
          Bit16u  RsvdP;         // 15 bit Reserved and Preserved      = 0x0000         RW
          bx_bool fla;           //  1 bit Force Link PM Accept        = 0x0000         RW
          Bit8u   u2timeout;     //  8 bit U2 Timeout                  = 0x0000         RW
          Bit8u   u1timeout;     //  8 bit U1 Timeout                  = 0x0000         RW
        } portpmsc;
        struct {
          Bit16u  RsvdP;         // 16 bit Reserved and Preserved      = 0x0000         RW
          Bit16u  lec;           // 16 bit Link Error Count            = 0x0000         RO
        } portli;
      } usb3;
      // if usb2 port
      struct {
        struct {
          Bit8u   tmode;         //  4 bit Test Mode                   = 0x0            RO
          Bit16u  RsvdP;         // 11 bit reserved and preseved       = 0x000          RW
          bx_bool hle;           //  1 bit hardware LPM enable         = 0b             RW
          Bit8u   l1dslot;       //  8 bit L1 Device Slot              = 0x00           RW
          Bit8u   hird;          //  4 bit Host Initiated Resume Durat = 0x0            RW
          bx_bool rwe;           //  1 bit Remote Wakeup Enable        = 0b             RW
          Bit8u   l1s;           //  3 bit L1 Status                   = 000b           RO
        } portpmsc;
        struct {
          Bit32u  RsvdP;         // 32 bit reserved and preseved       = 0x00000000     RW
        } portli;
      } usb2;
    };
    struct {
      Bit8u   hirdm;             //  2 bit host initiated resume duration mode
      Bit8u   l1timeout;         //  8 bit L1 timeout
      Bit8u   hirdd;             //  4 bit host initiated resume duration deep
      Bit32u  RsvdP;             // 18 bit reserved and preseved       = 0x00000000     RW
    } porthlpmc;
  } usb_port[USB_XHCI_PORTS];

  // Extended Caps Registers
  Bit8u extended_caps[EXT_CAPS_SIZE];

  struct XHCI_RUNTIME_REGS {
    struct {
      Bit32u RsvdP;              // 18 bit reserved and preseved       = 0x00000        RW
      Bit16u index;              // 14 bit index                       = 0x0000         RO
    } mfindex;
    struct {
      struct {
        Bit32u  RsvdP;           // 30 bit reserved and preseved       = 0x00000000     RW
        bx_bool ie;              //  1 bit Interrupt Enable            = 0b             RW
        bx_bool ip;              //  1 bit Interrupt Pending           = 0b             RW1C
      } iman;
      struct {
        Bit16u  imodc;           // 16 bit Interrupter Mod Counter     = 0x0000         RW
        Bit16u  imodi;           // 16 bit Interrupter Mod Interval    = 0x0000         RW
      } imod;
      struct {
        Bit16u  RsvdP;           // 16 bit reserved and preseved       = 0x0000         RW
        Bit16u  erstabsize;      // 16 bit Event Ring Seg Table Size   = 0x0000         RW
      } erstsz;
      Bit32u  RsvdP;             // 32 bit reserved and preseved       = 0x00000000     RW
      struct {
        Bit64u  erstabadd;       // 64 bit Event Ring Seg Tab Addy     = 0x00000000     RW  (See #define below)
        Bit16u  RsvdP;           //  6 bit reserved and preseved       = 0x0000         RW
      } erstba;
      struct {
        Bit64u  eventadd;        // 64 bit Event Ring Addy hi          = 0x00000000     RW
        bx_bool ehb;             //  1 bit Event Handler Busy          = 0b             RW1C
        Bit8u   desi;            //  2 bit Dequeue ERST Seg Index      = 00b            RW
      } erdp;
    } interrupter[INTERRUPTERS];
  } runtime_regs;

  struct HC_SLOT_CONTEXT slots[MAX_SLOTS];  // first one is ignored by controller.

  struct RING_MEMBERS ring_members;

  Bit8u devfunc;

  int statusbar_id; // ID of the status LEDs
  Bit8u device_change;
} bx_usb_xhci_t;

// Version 3.0.23.0 of the Renesas uPD720202 driver, even though the card is
//  version 1.00, the driver still uses bits 3:0 as RsvdP as with version 0.96
//  instead of bits 5:0 as RsvdP as with version 1.00+
#define RENESAS_ERSTABADD_BUG 1
#if ((VERSION_MAJOR < 1) || (RENESAS_ERSTABADD_BUG == 1))
  #define ERSTABADD_MASK   0x0F  // versions before 1.0 use 3:0 as preserved
#elif ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x00))
  #define ERSTABADD_MASK   0x3F  // versions 1.0 and above use 5:0 as preserved
#else
  #error "ERSTABADD_MASK not defined"
#endif

class bx_usb_xhci_c : public bx_devmodel_c, public bx_pci_device_stub_c {
public:
  bx_usb_xhci_c();
  virtual ~bx_usb_xhci_c();
  virtual void init(void);
  virtual void reset(unsigned);
  virtual void register_state(void);
  virtual void after_restore_state(void);
  virtual Bit32u  pci_read_handler(Bit8u address, unsigned io_len);
  virtual void    pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  static const char *usb_param_handler(bx_param_string_c *param, int set,
                                       const char *oldval, const char *val, int maxlen);

private:
  bx_usb_xhci_t hub;
  Bit8u         *device_buffer;

  static void reset_hc();
  static void reset_port(int);
  static bx_bool save_hc_state(void);
  static bx_bool restore_hc_state(void);

  static void update_irq(unsigned interrupter);

  static void init_device(Bit8u port, bx_list_c *portconf);
  static void remove_device(Bit8u port);
  static void usb_set_connect_status(Bit8u port, int type, bx_bool connected);

  static int  broadcast_packet(USBPacket *p, const int port);
  //static void usb_frame_handler(void *);
  //void usb_frame_timer(void);

  static void process_transfer_ring(const int slot, const int ep);
  static void process_command_ring(void);
  static void write_event_TRB(const unsigned interrupter, const Bit64u parameter, const Bit32u status, 
                              const Bit32u command, const bx_bool fire_int);
  static Bit32u NEC_verification(const Bit64u parameter);
  static void init_event_ring(const unsigned interrupter);
  static void read_TRB(bx_phy_address addr, struct TRB *trb);
  static void write_TRB(bx_phy_address addr, const Bit64u parameter, const Bit32u status, const Bit32u command);
  static void update_slot_context(const int slot);
  static void update_ep_context(const int slot, const int ep);
  static void dump_slot_context(const Bit32u *context, const int slot);
  static void dump_ep_context(const Bit32u *context, const int slot, const int ep);
  static void copy_slot_from_buffer(struct SLOT_CONTEXT *slot_context, const Bit8u *buffer);
  static void copy_ep_from_buffer(struct EP_CONTEXT *ep_context, const Bit8u *buffer);
  static void copy_slot_to_buffer(const Bit8u *buffer, const int slot);
  static void copy_ep_to_buffer(const Bit8u *buffer, const int slot, const int ep);
  static bx_bool validate_slot_context(const struct SLOT_CONTEXT *slot_context);
  static bx_bool validate_ep_context(const struct EP_CONTEXT *ep_context, int speed, int ep_num);
  static int  create_unique_address(const int slot);
  static int  send_set_address(const int addr, const int port_num);

  static void dump_xhci_core(const int slots, const int eps);

#if BX_USE_USB_XHCI_SMF
  static bx_bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bx_bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#else
  bx_bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  bx_bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#endif

  static void runtime_config_handler(void *);
  void runtime_config(void);
};

#endif  // BX_IODEV_USB_XHCI_H
