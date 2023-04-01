/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2010-2023  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2011-2023  The Bochs Project
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
//   (the NEC/Renesas uPD720202 is v1.00. If you change this version and use a NEC/Renesas specific,
//     driver the emulation may have undefined results)
//   (also, the emulation for v1.10 is untested. I don't have a test-system that uses that version)
#define VERSION_MAJOR     0x01
#define VERSION_MINOR     0x00

// HCSPARAMS1
#define MAX_SLOTS           32   // (1 based)
#define INTERRUPTERS         8   //

// Each controller supports its own number of ports.
// Note: USB_XHCI_PORTS should be defined as twice the amount of sockets wanted.
//  ie.: Typically each physical port (socket) has two defined port register sets.  One for USB3, one for USB2.
// Only one paired port type may be used at a time.
// If we support two sockets, we need four ports. (In our emulation) the first half will be USB3, the last half will be USB2.
// With a count of four, if Port1 is used, then Port3 must be vacant. If Port2 is used, then Port4 must be vacant.
// The uPD720202 supports 2 sockets (4 port register sets).
// The uPD720201 supports 4 sockets (8 port register sets).
// You can change USB_XHCI_PORTS below to another value (must be an even number).
#define USB_XHCI_PORTS       4   // Default number of port Registers, each supporting USB3 or USB2 (0x08 = uPD720201, 0x04 = uPD720202)
#define USB_XHCI_PORTS_MAX  10   // we don't support more than this many ports

#if ((USB_XHCI_PORTS < 2) || (USB_XHCI_PORTS > USB_XHCI_PORTS_MAX) || ((USB_XHCI_PORTS & 1) == 1))
  #error "USB_XHCI_PORTS must be at least 2 and no more than USB_XHCI_PORTS_MAX and must be an even number."
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
#define PORT_POWER_CTRL          1  // 1 = port power is controlled by port register's power bit, 0 = power always on
#define PORT_INDICATORS          0
#define LIGHT_HC_RESET           0  // Do we support the Light HC Reset function
#define LAT_TOL_MSGING_CAP       1  // Latency Tolerance Messaging Capability (v1.00+)
#define NO_SSD_SUPPORT           1  // No Secondary SID Support (v1.00+)
#define PARSE_ALL_EVENT          1  // version 0.96 and below only (MUST BE 1 in v1.00+)
#define SEC_DOMAIN_BAND          1  // version 0.96 and below only (MUST BE 1 in v1.00+)
#define STOPPED_EDTLA            0
#define CONT_FRAME_ID            0
#define MAX_PSA_SIZE          0x05  // 2^(5+1) = 63 Primary Streams (first one is reserved)
  #define MAX_PSA_SIZE_NUM      (1 << (MAX_PSA_SIZE + 1))
#define EXT_CAPS_OFFSET      0x500
  #define EXT_CAPS_SIZE        144

// doorbell masks
#define PSA_MAX_SIZE_NUM(m)       (1UL << ((m) + 1))
#define PSA_PRIMARY_MASK(d, m)    (((d) >> 16) & ((1 << ((m) + 1)) - 1))
#define PSA_SECONDARY_MASK(d, m)  (((d) >> 16) >> ((m) + 1))

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
#  error "LAT_TOL_MSGING_CAP must be used with in version 1.10 and above"
#endif

#if ((NO_SSD_SUPPORT == 1) && ((VERSION_MAJOR < 1) || (VERSION_MINOR < 0)))
#  error "NO_SSD_SUPPORT must be used with in version 1.10 and above"
#endif

// xHCI speed values
#define XHCI_SPEED_FULL   1
#define XHCI_SPEED_LOW    2
#define XHCI_SPEED_HIGH   3
#define XHCI_SPEED_SUPER  4

#define USB2 0
#define USB3 1

// Port Status Change Bits
#define PSCEG_CSC  (1<<0)
#define PSCEG_PEC  (1<<1)
#define PSCEG_WRC  (1<<2)
#define PSCEG_OCC  (1<<3)
#define PSCEG_PRC  (1<<4)
#define PSCEG_PLC  (1<<5)
#define PSCEG_CEC  (1<<6)

// Extended Capabilities: Protocol
struct XHCI_PROTOCOL {
  Bit8u  id;
  Bit8u  next;
  Bit16u version;
  Bit8u  name[4];
  Bit8u  start_index;
  Bit8u  count;
  Bit16u flags;
};

// our saved ring members
struct RING_MEMBERS {
  struct {
    Bit64u dq_pointer;
    bool   rcs;
  } command_ring;
  struct {
    bool     rcs;
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
  bool     hub;
  bool     mtt;
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
  bool     lsa;
  unsigned max_pstreams;
  unsigned mult;
  unsigned ep_state;
  unsigned max_packet_size;
  unsigned max_burst_size;
  bool     hid;
  unsigned ep_type;
  unsigned cerr;
  Bit64u   tr_dequeue_pointer;
  bool     dcs;
  unsigned max_esit_payload;
  unsigned average_trb_len;
};

struct STREAM_CONTEXT {
  bool     valid; // is this context valid
  Bit64u   tr_dequeue_pointer;
  bool     dcs;
  int      sct;
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  Bit32u   stopped_EDTLA;
#endif
};

struct HC_SLOT_CONTEXT {
  bool enabled;
  struct SLOT_CONTEXT slot_context;
  struct {
    struct EP_CONTEXT   ep_context;
    // our internal registers follow
    Bit32u  edtla;
    Bit64u  enqueue_pointer;
    bool    rcs;
    bool    retry;
    int     retry_counter;
    struct STREAM_CONTEXT stream[MAX_PSA_SIZE_NUM]; // first one is reserved
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
       // 48, 49, & 50 are used for the NEC Vendor Defined commands
       NEC_TRB_TYPE_CMD_COMP = 48,
       NEC_TRB_TYPE_GET_FW = 49,
       NEC_TRB_TYPE_GET_UN = 50,
       // 60 is used for the Bochs Dump vendor command
       BX_TRB_TYPE_DUMP = 60,
};

// NEC Vendor specific TRB types
#define NEC_FW_MAJOR(v)       (((v) & 0x0000FF00) >> 8)
#define NEC_FW_MINOR(v)       (((v) & 0x000000FF) >> 0)


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
#define SLOT_STATE_ADDRESSED         2
#define SLOT_STATE_CONFIGURED        3

// EP State
#define EP_STATE_DISABLED 0
#define EP_STATE_RUNNING  1
#define EP_STATE_HALTED   2
#define EP_STATE_STOPPED  3
#define EP_STATE_ERROR    4

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
#define TRB_GET_STREAM(x)    (((x) & (0xFFFF << 16)) >> 16)

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

enum {
  XHCI_HC_uPD720202,     // Renesas/NEC uPD720202 (2 sockets)  (default)
  XHCI_HC_uPD720201      // Renesas/NEC uPD720201 (4 sockets)
};

typedef struct {
  Bit32u HostController;
  unsigned int n_ports;

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
      Bit32u RsvdP1;         // 18/20 bit reserved and preserved   = 0x000000       RW
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
      bool   cme;            //  1 bit Max Exit Latecy to Large    = 0b             RW
      bool   spe;            //  1 bit Generate Short Packet Comp  = 0b             RW
#endif
      bool   eu3s;           //  1 bit Enable U3 MFINDEX Stop      = 0b             RW
      bool   ewe;            //  1 bit Enable Wrap Event           = 0b             RW
      bool   crs;            //  1 bit Controller Restore State    = 0b             RW
      bool   css;            //  1 bit Controller Save State       = 0b             RW
      bool   lhcrst;         //  1 bit Light HC Reset              = 0b             RW or RO (HCCPARAMS:LHRC)
      Bit8u  RsvdP0;         //  1 bit reserved and preserved      = 000b           RW
      bool   hsee;           //  1 bit Host System Error Enable    = 0b             RW
      bool   inte;           //  1 bit Interrupter Enable          = 0b             RW
      bool   hcrst;          //  1 bit HC Reset                    = 0b             RW
      bool   rs;             //  1 bit Run Stop                    = 0b             RW
    } HcCommand;             //                                    = 0x00000000
    struct {
      Bit32u RsvdZ1;         // 19 bit reserved and zero'd         = 0x000000       RW
      bool   hce;            //  1 bit Host Controller Error       = 0b             RO
      bool   cnr;            //  1 bit Controller Not Ready        = 0b             R0
      bool   sre;            //  1 bit Save/Restore Error          = 0b             RW1C
      bool   rss;            //  1 bit Restore State Status        = 0b             RO
      bool   sss;            //  1 bit Save State Status           = 0b             RO
      Bit8u  RsvdZ0;         //  3 bit reserved and zero'd         = 0x0            RW
      bool   pcd;            //  1 bit Port Change Detect          = 0b             RW1C
      bool   eint;           //  1 bit Event Interrupt             = 0b             RW1C
      bool   hse;            //  1 bit Host System Error           = 0b             RW1C
      bool   RsvdZ2;         //  1 bit reserved and zero'd         = 0b             RW
      bool   hch;            //  1 bit HCHalted                    = 1b             RO
    } HcStatus;              //                                    = 0x00000001
    struct {
      Bit16u  Rsvd;          // 16 bit reserved                    = 0x0000         RO
      Bit16u  pagesize;      // 16 bit reserved                    = 0x0001         RO
    } HcPageSize;            //                                    = 0x00000001
    struct {
      Bit16u  RsvdP;         // 16 bit reserved and presserved     = 0x0000         RW
      bool n15;              //  1 bit N15                         = 0              RW
      bool n14;              //  1 bit N14                         = 0              RW
      bool n13;              //  1 bit N13                         = 0              RW
      bool n12;              //  1 bit N12                         = 0              RW
      bool n11;              //  1 bit N11                         = 0              RW
      bool n10;              //  1 bit N10                         = 0              RW
      bool n9;               //  1 bit N9                          = 0              RW
      bool n8;               //  1 bit N8                          = 0              RW
      bool n7;               //  1 bit N7                          = 0              RW
      bool n6;               //  1 bit N6                          = 0              RW
      bool n5;               //  1 bit N5                          = 0              RW
      bool n4;               //  1 bit N4                          = 0              RW
      bool n3;               //  1 bit N3                          = 0              RW
      bool n2;               //  1 bit N2                          = 0              RW
      bool n1;               //  1 bit N1                          = 0              RW
      bool n0;               //  1 bit N0                          = 0              RW
    } HcNotification;        //                                    = 0x00000000
    struct {
      Bit64u crc;            // 64 bit hi order address            = 0x00000000     RW
      Bit8u  RsvdP;          //  2 bit reserved and preserved      = 00b            RW
      bool   crr;            //  1 bit Command Ring Running        = 0              RO
      bool   ca;             //  1 bit Command Abort               = 0              RW1S
      bool   cs;             //  1 bit Command Stop                = 0              RW1S
      bool   rcs;            //  1 bit Ring Cycle State            = 0              RW
    } HcCrcr;
    struct {
      Bit64u dcbaap;         // 64 bit hi order address            = 0x00000000     RW
      Bit8u  RsvdZ;          //  6 bit reserved and zero'd         = 000000b        RW
    } HcDCBAAP;
    struct {
      Bit32u RsvdP;          // 22/24 bit reserved and preserved   = 0x000000       RW
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
      bool   u3e;            //  1 bit U3 Entry Enable             = 0              RW
      bool   cie;            //  1 bit Config Info Enable          = 0              RW
#endif
      Bit8u   MaxSlotsEn;    //  8 bit Max Device Slots Enabled    = 0x00           RW
    } HcConfig;
  } op_regs;

  struct {
    // our data
    usb_device_c *device; // device connected to this port
    bool is_usb3;         // set if usb3 port, cleared if usb2 port.
    bool has_been_reset;  // set if the port has been reset aftet powered up.
    Bit8u psceg;          // current port status change event

    struct {
      bool  wpr;               //  1 bit Warm Port Reset             = 0b             RW or RsvdZ
      bool  dr;                //  1 bit Device Removable            = 0b             RO
      Bit8u RsvdZ1;            //  2 bit Reserved and Zero'd         = 00b            RW
      bool  woe;               //  1 bit Wake on Over Current Enable = 0b             RW
      bool  wde;               //  1 bit Wake on Disconnect Enable   = 0b             RW
      bool  wce;               //  1 bit Wake on Connect Enable      = 0b             RW
      bool  cas;               //  1 bit Cold Attach Status          = 0b             RO
      bool  cec;               //  1 bit Port Config Error Change    = 0b             RW1C or RsvdZ
      bool  plc;               //  1 bit Port Link State Change      = 0b             RW1C
      bool  prc;               //  1 bit Port Reset Change           = 0b             RW1C
      bool  occ;               //  1 bit Over Current Change         = 0b             RW1C
      bool  wrc;               //  1 bit Warm Port Reset Change      = 0b             RW1C or RsvdZ
      bool  pec;               //  1 bit Port Enabled/Disabled Change= 0b             RW1C
      bool  csc;               //  1 bit Connect Status Change       = 0b             RW1C
      bool  lws;               //  1 bit Port Link State Write Strobe= 0b             RW
      Bit8u pic;               //  2 bit Port Indicator Control      = 00b            RW
      Bit8u speed;             //  4 bit Port Speed                  = 0000b          RO
      bool  pp;                //  1 bit Port Power                  = 0b             RW
      Bit8u pls;               //  4 bit Port Link State             = 0x00           RW
      bool  pr;                //  1 bit Port Reset                  = 0b             RW
      bool  oca;               //  1 bit Over Current Active         = 0b             RO
      bool  RsvdZ0;            //  1 bit Reserved and Zero'd         = 0b             RW
      bool  ped;               //  1 bit Port Enabled/Disabled       = 0b             RW1C
      bool  ccs;               //  1 bit Current Connect Status      = 0b             RO
    } portsc;
    union {
      // if usb3 port
      struct {
        struct {
          Bit16u  RsvdP;         // 15 bit Reserved and Preserved      = 0x0000         RW
          bool    fla;           //  1 bit Force Link PM Accept        = 0x0000         RW
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
          bool    hle;           //  1 bit hardware LPM enable         = 0b             RW
          Bit8u   l1dslot;       //  8 bit L1 Device Slot              = 0x00           RW
          Bit8u   hird;          //  4 bit Host Initiated Resume Durat = 0x0            RW
          bool    rwe;           //  1 bit Remote Wakeup Enable        = 0b             RW
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
  } usb_port[USB_XHCI_PORTS_MAX];

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
        bool    ie;              //  1 bit Interrupt Enable            = 0b             RW
        bool    ip;              //  1 bit Interrupt Pending           = 0b             RW1C
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
        bool    ehb;             //  1 bit Event Handler Busy          = 0b             RW1C
        Bit8u   desi;            //  2 bit Dequeue ERST Seg Index      = 00b            RW
      } erdp;
    } interrupter[INTERRUPTERS];
  } runtime_regs;

  struct HC_SLOT_CONTEXT slots[MAX_SLOTS];  // first one is ignored by controller.

  struct RING_MEMBERS ring_members;
  
  // filled at runtime with ex: { USB3, USB3, USB2, USB2 };
  //Bit8u port_speed_allowed[USB_XHCI_PORTS_MAX];
  // four speeds of: 'reserved' + a port count of bytes rounded up to and 8 byte size (ie: 8, 16, 24, 32 bytes each speed)
  Bit8u port_band_width[4 * ((1 + USB_XHCI_PORTS_MAX) + 8)]; // + 8 gives us ample room for a boundary of 8-byte entries per speed
  // the port's paired port num. i.e., with 4 ports, 1 is paired with 3, 2 is paired with 4
  int   paired_portnum[USB_XHCI_PORTS_MAX];
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

class bx_usb_xhci_c : public bx_pci_device_c {
public:
  bx_usb_xhci_c();
  virtual ~bx_usb_xhci_c();
  virtual void init(void);
  virtual void reset(unsigned);
  virtual void register_state(void);
  virtual void after_restore_state(void);

  virtual void pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  int event_handler(int event, void *ptr, int port);

private:
  bx_usb_xhci_t hub;
  Bit8u         devfunc;
  Bit8u         device_change;
  int           rt_conf_id;
  int           xhci_timer_index;
  USBAsync      *packets;

  static void reset_hc();
  static void reset_port(int);
  static void reset_port_usb3(int, const int);
  static bool save_hc_state(void);
  static bool restore_hc_state(void);

  static void update_irq(unsigned interrupter);

  static void init_device(Bit8u port, bx_list_c *portconf);
  static void remove_device(Bit8u port);
  static bool set_connect_status(Bit8u port, bool connected);

  static int  broadcast_speed(const int slot);
  static int  broadcast_packet(USBPacket *p, const int port);
  static Bit8u get_psceg(const int port);
  static void xhci_timer_handler(void *);
  void xhci_timer(void);

  static Bit64u process_transfer_ring(const int slot, const int ep, Bit64u ring_addr, bool *rcs, const int primary_sid);
  static void process_command_ring(void);
  static void get_stream_info(struct STREAM_CONTEXT *context, const Bit64u address, const int index);
  static void put_stream_info(struct STREAM_CONTEXT *context, const Bit64u address, const int index);
  static void write_event_TRB(const unsigned interrupter, const Bit64u parameter, const Bit32u status,
                              const Bit32u command, const bool fire_int);
  static Bit32u NEC_verification(const Bit64u parameter);
  static void init_event_ring(const unsigned interrupter);
  static void read_TRB(bx_phy_address addr, struct TRB *trb);
  static void write_TRB(bx_phy_address addr, const Bit64u parameter, const Bit32u status, const Bit32u command);
  static void update_slot_context(const int slot);
  static void update_ep_context(const int slot, const int ep);
  static void dump_slot_context(const Bit32u *context, const int slot);
  static void dump_ep_context(const Bit32u *context, const int slot, const int ep);
  static void dump_stream_context(const Bit32u *context);
  static void copy_slot_from_buffer(struct SLOT_CONTEXT *slot_context, const Bit8u *buffer);
  static void copy_ep_from_buffer(struct EP_CONTEXT *ep_context, const Bit8u *buffer);
  static void copy_slot_to_buffer(Bit32u *buffer, const int slot);
  static void copy_ep_to_buffer(Bit32u *buffer, const int slot, const int ep);
  static void copy_stream_from_buffer(struct STREAM_CONTEXT *context, const Bit8u *buffer);
  static void copy_stream_to_buffer(Bit8u *buffer, const struct STREAM_CONTEXT *context);
  static int  validate_slot_context(const struct SLOT_CONTEXT *slot_context, const int trb_command, const int slot);
  static int  validate_ep_context(const struct EP_CONTEXT *ep_context, const int trb_command, const Bit32u a_flags, int port_num, int ep_num);
  static int  create_unique_address(const int slot);
  static int  send_set_address(const int addr, const int port_num, const int slot);

  static void dump_xhci_core(const unsigned int slots, const unsigned int eps);

#if BX_USE_USB_XHCI_SMF
  static bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#else
  bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#endif

  static void runtime_config_handler(void *);
  void runtime_config(void);

  static Bit64s usb_param_handler(bx_param_c *param, bool set, Bit64s val);
  static Bit64s usb_param_oc_handler(bx_param_c *param, bool set, Bit64s val);
  static bool usb_param_enable_handler(bx_param_c *param, bool en);
};

#endif  // BX_IODEV_USB_XHCI_H
