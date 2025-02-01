/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C)      2025  Benjamin David Lunt
//  Copyright (C) 2003-2025  The Bochs Project
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

#include "iodev.h"

#if BX_USB_DEBUGGER

#include "usb_debug.h"

#define LOG_THIS genlog->

const char *hc_param_str[] = {
  "",
  BXPN_USB_UHCI,
  BXPN_USB_OHCI,
  BXPN_USB_EHCI,
  BXPN_USB_XHCI
};

///////////////////////////////////////////////////////////////////////////////////////////////
//  UHCI
//
struct S_ATTRIBUTES attribs_u_command[] = {
                                             //          |      31 chars + null          | <- max
  { (1<<7),                  (1<<7),                  7, "Max Packet"                     , {-1, } },
  { (1<<6),                  (1<<6),                  6, "Configure Flag"                 , {-1, } },
  { (1<<5),                  (1<<5),                  5, "Software Debug"                 , {-1, } },
  { (1<<4),                  (1<<4),                  4, "Force Global Resume"            , {-1, } },
  { (1<<3),                  (1<<3),                  3, "Enter Global Suspend Mode"      , {-1, } },
  { (1<<2),                  (1<<2),                  2, "Global Reset"                   , {-1, } },
  { (1<<1),                  (1<<1),                  1, "Host Controller Reset"          , {-1, } },
  { (1<<0),                  (1<<0),                  0, "Run/Stop"                       , {-1, } },
  { 0,                   (Bit32u) -1,                 -1, "\0"                             , {-1, } }
};

struct S_ATTRIBUTES attribs_u_status[] = {
                                             //          |      31 chars + null          | <- max
  { (1<<5),                  (1<<5),                  5, "HCHalted"                       , {-1, } },
  { (1<<4),                  (1<<4),                  4, "Host Controller Process Error"  , {-1, } },
  { (1<<3),                  (1<<3),                  3, "Host System Error"              , {-1, } },
  { (1<<2),                  (1<<2),                  2, "Resume Detect"                  , {-1, } },
  { (1<<1),                  (1<<1),                  1, "USB Error Interrupt"            , {-1, } },
  { (1<<0),                  (1<<0),                  0, "USB Interrupt"                  , {-1, } },
  { 0,                   (Bit32u) -1,                 -1, "\0"                             , {-1, } }
};

struct S_ATTRIBUTES attribs_u_interrupt[] = {
                                             //          |      31 chars + null          | <- max
  { (1<<3),                  (1<<3),                  3, "Short packet Interrupt Enable"  , {-1, } },
  { (1<<2),                  (1<<2),                  2, "Interrupt On Complete (IOC)"    , {-1, } },
  { (1<<1),                  (1<<1),                  1, "Resume Interrupt Enable"        , {-1, } },
  { (1<<0),                  (1<<0),                  0, "Timeout/CRC Interrupt Enable"   , {-1, } },
  { 0,                   (Bit32u) -1,                 -1, "\0"                             , {-1, } }
};

struct S_ATTRIBUTES attribs_u_ports[] = {
                                             //          |      31 chars + null          | <- max
  { (1<<15),                 (1<<15),                15, "Zero (bit 15)"                  , {-1, } },
  { (1<<14),                 (1<<14),                14, "Zero (bit 14)"                  , {-1, } },
  { (1<<13),                 (1<<13),                13, "Zero (bit 13)"                  , {-1, } },
  { (1<<12),                 (1<<12),                12, "Suspend"                        , {-1, } },
  { (1<<11),                 (1<<11),                11, "Over-current Changed"           , {-1, } },
  { (1<<10),                 (1<<10),                10, "Over-current Status"            , {-1, } },
  { (1<< 9),                 (1<< 9),                 9, "Port Reset"                     , {-1, } },
  { (1<< 8),                 (1<< 8),                 8, "Low Speed Device Attached"      , {-1, } },
  { (1<< 7),                 (1<< 7),                 7, "Reserved (must be 1)"           , {-1, } },
  { (1<< 6),                 (1<< 6),                 6, "Resume Detect"                  , {-1, } },
  { (1<< 5),                 (1<< 5),                 5, "Line Status: D-"                , {-1, } },
  { (1<< 4),                 (1<< 4),                 4, "Line Status: D+"                , {-1, } },
  { (1<< 3),                 (1<< 3),                 3, "Port Enable/Disable Change"     , {-1, } },
  { (1<< 2),                 (1<< 2),                 2, "Port Enabled/Disabled"          , {-1, } },
  { (1<< 1),                 (1<< 1),                 1, "Current Status Change"          , {-1, } },
  { (1<< 0),                 (1<< 0),                 0, "Current Connect Status"         , {-1, } },
  { 0,                   (Bit32u) -1,                 -1, "\0"                             , {-1, } }
};

///////////////////////////////////////////////////////////////////////////////////////////////
//  XHCI
//
struct S_ATTRIBUTES attribs_x_ports[] = {
                                             //          |      31 chars + null          | <- max
  { (1ULL<<31),              (1ULL<<31),             31, "Warm Port Reset"                , {-1, } },
  { (1<<30),                 (1<<30),                30, "Device Removable"               , {-1, } },
  { (1<<29),                 (1<<29),                29, "Reserved (bit 29)"              , {-1, } },
  { (1<<28),                 (1<<28),                28, "Reserved (bit 28)"              , {-1, } },
  { (1<<27),                 (1<<27),                27, "Wake on Over-current Enable"    , {-1, } },
  { (1<<26),                 (1<<26),                26, "Wake on Disconnect Enable"      , {-1, } },
  { (1<<25),                 (1<<25),                25, "Wake on Connect Enable"         , {-1, } },
  { (1<<24),                 (1<<24),                24, "Cold Attach Status"             , {-1, } },
  { (1<<23),                 (1<<23),                23, "Port Config Error Change"       , {-1, } },
  { (1<<22),                 (1<<22),                22, "Port Link State Change"         , {-1, } },
  { (1<<21),                 (1<<21),                21, "Port Reset Change"              , {-1, } },
  { (1<<20),                 (1<<20),                20, "Over-current Change"            , {-1, } },
  { (1<<19),                 (1<<19),                19, "Warm Port Reset Change"         , {-1, } },
  { (1<<18),                 (1<<18),                18, "Port Enable/Disable Change"     , {-1, } },
  { (1<<17),                 (1<<17),                17, "Connect Status Change"          , {-1, } },
  { (1<<16),                 (1<<16),                16, "Port Link State Write Strobe"   , {-1, } },
  { (1<<15),                 (1<<15),                15, "Port Indicator (bit 1)"         , {-1, } },
  { (1<<14),                 (1<<14),                14, "Port Indicator (bit 0)"         , {-1, } },
  { (1<<13),                 (1<<13),                13, "Port Speed (bit 3)"             , {-1, } },
  { (1<<12),                 (1<<12),                12, "Port Speed (bit 2)"             , {-1, } },
  { (1<<11),                 (1<<11),                11, "Port Speed (bit 1)"             , {-1, } },
  { (1<<10),                 (1<<10),                10, "Port Speed (bit 0)"             , {-1, } },
  { (1<< 9),                 (1<< 9),                 9, "Port Power"                     , {-1, } },
  { (1<< 8),                 (1<< 8),                 8, "Port Link State (bit 3)"        , {-1, } },
  { (1<< 7),                 (1<< 7),                 7, "Port Link State (bit 2)"        , {-1, } },
  { (1<< 6),                 (1<< 6),                 6, "Port Link State (bit 1)"        , {-1, } },
  { (1<< 5),                 (1<< 5),                 5, "Port Link State (bit 0)"        , {-1, } },
  { (1<< 4),                 (1<< 4),                 4, "Port Reset"                     , {-1, } },
  { (1<< 3),                 (1<< 3),                 3, "Over-current Status"            , {-1, } },
  { (1<< 2),                 (1<< 2),                 2, "Reserved"                       , {-1, } },
  { (1<< 1),                 (1<< 1),                 1, "Port Enabled/Disabled"          , {-1, } },
  { (1<< 0),                 (1<< 0),                 0, "Current Connect Status"         , {-1, } },
  { 0,                   (Bit32u) -1,                 -1, "\0"                             , {-1, } }
};

const struct VIEW_TRB_TYPE trb_types[] = {
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 0
  { VIEW_TRB_TYPE_TRANSFER,  "Normal"                }, // 1
  { VIEW_TRB_TYPE_TRANSFER,  "Setup"                 }, // 2
  { VIEW_TRB_TYPE_TRANSFER,  "Data"                  }, // 3
  { VIEW_TRB_TYPE_TRANSFER,  "Status"                }, // 4
  { VIEW_TRB_TYPE_TRANSFER,  "ISO"                   }, // 5
  { VIEW_TRB_TYPE_TRANSFER |                            // 6
     VIEW_TRB_TYPE_COMMAND,  "LINK"                  },
  { VIEW_TRB_TYPE_TRANSFER,  "Event Data"            }, // 7
  { VIEW_TRB_TYPE_TRANSFER,  "No-Op Transfer"        }, // 8
  { VIEW_TRB_TYPE_COMMAND,   "Enable Slot"           }, // 9
  { VIEW_TRB_TYPE_COMMAND,   "Disable Slot"          }, // 10
  { VIEW_TRB_TYPE_COMMAND,   "Address Dev"           }, // 11
  { VIEW_TRB_TYPE_COMMAND,   "Config EP"             }, // 12
  { VIEW_TRB_TYPE_COMMAND,   "Eval Context"          }, // 13
  { VIEW_TRB_TYPE_COMMAND,   "Reset EP"              }, // 14
  { VIEW_TRB_TYPE_COMMAND,   "Stop EP"               }, // 15
  { VIEW_TRB_TYPE_COMMAND,   "Set TR Deque"          }, // 16
  { VIEW_TRB_TYPE_COMMAND,   "Reset Device"          }, // 17
  { VIEW_TRB_TYPE_COMMAND,   "Force Event"           }, // 18
  { VIEW_TRB_TYPE_COMMAND,   "Negotiate Bandwidth"   }, // 19
  { VIEW_TRB_TYPE_COMMAND,   "Set Latency Tolerance" }, // 20
  { VIEW_TRB_TYPE_COMMAND,   "Get Port Bandwidth"    }, // 21
  { VIEW_TRB_TYPE_COMMAND,   "Force Header"          }, // 22
  { VIEW_TRB_TYPE_COMMAND,   "No-Op Command"         }, // 23
  { VIEW_TRB_TYPE_COMMAND,   "Get Extended Property" }, // 24
  { VIEW_TRB_TYPE_COMMAND,   "Set Extended Property" }, // 25
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 26
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 27
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 28
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 29
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 30
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 31
  { VIEW_TRB_TYPE_EVENT,     "Transfer Event"        }, // 32
  { VIEW_TRB_TYPE_EVENT,     "Command Completion"    }, // 33
  { VIEW_TRB_TYPE_EVENT,     "Port Status Change"    }, // 34
  { VIEW_TRB_TYPE_EVENT,     "Bandwidth Request"     }, // 35
  { VIEW_TRB_TYPE_EVENT,     "Doorbell Event"        }, // 36
  { VIEW_TRB_TYPE_EVENT,     "Host Controller Event" }, // 37
  { VIEW_TRB_TYPE_EVENT,     "Device Notification"   }, // 38
  { VIEW_TRB_TYPE_EVENT,     "MFINDEX Wrap Event"    }, // 39
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 40
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 41
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 42
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 43
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 44
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 45
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 46
  { VIEW_TRB_TYPE_NONE,      "Reserved"              }, // 47
  // remaining are vendor defined (allowed in any ring)
};

// these match the VIEW_TRB_TYPE_ masks
const char *ring_type[] = {
  "",         // VIEW_TRB_TYPE_NONE
  "Command",  // VIEW_TRB_TYPE_COMMAND
  "Event",    // VIEW_TRB_TYPE_EVENT
  "",         //
  "Transfer"  // VIEW_TRB_TYPE_TRANSFER
};

Bit64u xhci_context_address = 0;
Bit8u  *xhci_context = NULL;
int    xhci_current_ep_context = 1;  // 0 through 30 (slot, control_ep, ep1_out, ep1_in, ep2_out, ep2_in, etc)
bool   xhci_context_changed = 0;

Bit64u xhci_str_context_address = 0;
Bit8u *xhci_str_context = NULL;
int    xhci_str_current_context = 0;
bool   xhci_str_context_changed = 0;
int    xhci_max_streams = 0;

const char *slot_speed_str[] = {
  "Undefined",
  "Full",
  "Low",
  "High",
  "Super Gen1x1",
  "Super Gen2x1",
  "Super Gen1x2",
  "Super Gen2x2",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Undefined",
  "Not Valid"
};

const char *slot_type_str[] = {
  "Disabled/Enabled",
  "Default",
  "Addressed",
  "Configured",
  "Reserved"
};

const char *ep_type_str[] = {
  "n/a",
  "ISO Out",
  "Bulk Out",
  "Int Out",
  "Control",
  "ISO In",
  "Bulk In",
  "Int In",
  "Not Valid"
};

const char *ep_state_str[] = {
  "Disabled",
  "Running",
  "Halted",
  "Stopped",
  "Error",
  "Reserved",
  "Reserved",
  "Reserved",
  "Not Valid"
};

const char *string_sct_str[] = {
  "Secondary / Transfer Ring / N/A",
  "Primary / Transfer Ring / N/A",
  "Primary / SSA / 8",
  "Primary / SSA / 16",
  "Primary / SSA / 32",
  "Primary / SSA / 64",
  "Primary / SSA / 128",
  "Primary / SSA / 256",
  "Error"
};

int usb_debug_type = USB_DEBUG_NONE;
bx_param_c *host_param = NULL;
Bit32u pci_bar_address;
bool u_changed[UHCI_REG_COUNT];

bool uhci_add_queue(struct USB_UHCI_QUEUE_STACK *stack, const Bit32u addr)
{
  // check to see if this queue has been processed before
  for (int i=0; i<stack->queue_cnt; i++) {
    if (stack->queue_stack[i] == addr)
      return true;
  }

  // if the stack is full, we return TRUE anyway
  if (stack->queue_cnt == USB_UHCI_QUEUE_STACK_SIZE)
    return true;

  // add the queue's address
  stack->queue_stack[stack->queue_cnt] = addr;
  stack->queue_cnt++;

  return false;
}

void usb_dbg_register_type(int type)
{
  if (type != USB_DEBUG_NONE) {
    if ((type != USB_DEBUG_UHCI) && (type != USB_DEBUG_XHCI)) {
      BX_PANIC(("USB debugger does not yet support type %d", type));
    } else {
      usb_debug_type = type;
      bx_gui->set_usbdbg_bitmap(0);
    }
  }
}

int usb_dbg_interface(int type, Bit64u param0, int param1, int param2)
{
  if (SIM->get_param_enum(BXPN_USB_DEBUG_TYPE)->get() > 0) {
    // if "start_frame" is 0, do the debug_window
    // if "start_frame" is 1, wait for the trigger from the HC
    //  (set the value to 2, then return, allowing the trigger to envoke it)
    // if "start_frame" is 2, the HC triggered the debug
    if (SIM->get_param_num(BXPN_USB_DEBUG_START_FRAME)->get() == BX_USB_DEBUG_SOF_SET) {
      SIM->get_param_num(BXPN_USB_DEBUG_START_FRAME)->set(BX_USB_DEBUG_SOF_TRIGGER);
      bx_gui->set_usbdbg_bitmap(1);
    } else {
      bx_gui->set_usbdbg_bitmap(0);
      if (usb_debug_dialog(type, param0, param1, param2) < 0) {
        bx_user_quit = 1;
        bx_exit(1);
        return -1;
      }
    }
  }
  return 0;
}

// one of the controllers has triggered a debug item.
void usb_dbg_trigger(int type, int trigger, Bit64u param0, int param1, int param2)
{
  if ((usb_debug_type == USB_DEBUG_NONE) || (type != usb_debug_type))
    return;

  bx_param_bool_c *bool_trigger;
  bx_param_num_c *num_trigger;
  switch (trigger) {
    case USB_DEBUG_FRAME:
      num_trigger = SIM->get_param_num(BXPN_USB_DEBUG_START_FRAME);
      if (num_trigger && (num_trigger->get() == BX_USB_DEBUG_SOF_TRIGGER)) {
        SIM->usb_debug_interface(USB_DEBUG_FRAME, param0, param1, param2);
        num_trigger->set(BX_USB_DEBUG_SOF_SET);
      }
      break;

    case USB_DEBUG_COMMAND:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_DOORBELL);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_debug_interface(USB_DEBUG_COMMAND, param0, param1, param2);
      break;

    case USB_DEBUG_DATA:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_DATA);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_debug_interface(USB_DEBUG_DATA, param0, param1, param2);
      break;

    case USB_DEBUG_EVENT:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_EVENT);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_debug_interface(USB_DEBUG_EVENT, param0, param1, param2);
      break;

    case USB_DEBUG_NONEXIST:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_NON_EXIST);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_debug_interface(USB_DEBUG_NONEXIST, param0, param1, param2);
      break;

    case USB_DEBUG_RESET:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_RESET);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_debug_interface(USB_DEBUG_RESET, param0, param1, param2);
      break;

    case USB_DEBUG_ENABLE:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_ENABLE);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_debug_interface(USB_DEBUG_ENABLE, param0, param1, param2);
      break;
  }
}

Bit32u get_pci_bar_addr(bx_shadow_data_c *pci_conf, Bit8u bar_num)
{
  if ((pci_conf != NULL) && (bar_num < 5)) {
    Bit8u *data = pci_conf->getptr() + 0x10 + (bar_num << 2);
    Bit32u value = ReadHostDWordFromLittleEndian((Bit32u*)data);
    if (value & 1) {
      return (value & 0xfffc);
    } else {
      return (value & 0xfffffff0);
    }
  } else {
    return 0;
  }
}

Bit32u usb_io_read(Bit16u addr, unsigned io_len)
{
  return bx_devices.inp(addr, io_len);
}

void usb_io_write(Bit16u addr, Bit32u value, unsigned io_len)
{
  bx_devices.outp(addr, value, io_len);
}

Bit32u xhci_read_dword(const Bit32u address)
{
  Bit32u value = 0;

  if (address > 0) {
    DEV_MEM_READ_PHYSICAL(address, 4, (Bit8u*)&value);
  }
  return value;
}

#endif
