/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2010-2026  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2011-2026  The Bochs Project
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

#ifndef BX_IODEV_USB_XHCI_DEFS_H
#define BX_IODEV_USB_XHCI_DEFS_H

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
#define ADDR_CAP_64              0  // default to 32-bit only
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

#define XHCI_PORT_SET_OFFSET  (0x400 + OPS_REGS_OFFSET)

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
enum {
  SLOT_STATE_DISABLED_ENABLED = 0,
  SLOT_STATE_DEFAULT          = 1,
  SLOT_STATE_ADDRESSED        = 2,
  SLOT_STATE_CONFIGURED       = 3
};

// EP State
enum {
  EP_STATE_DISABLED = 0,
  EP_STATE_RUNNING  = 1,
  EP_STATE_HALTED   = 2,
  EP_STATE_STOPPED  = 3,
  EP_STATE_ERROR    = 4
};

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

#endif  // BX_IODEV_USB_XHCI_DEFS_H
