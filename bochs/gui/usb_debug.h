/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C)      2023  Benjamin David Lunt
//  Copyright (C) 2003-2024 The Bochs Project
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

#ifndef BX_USB_DEBUG_H
#define BX_USB_DEBUG_H

#if BX_USB_DEBUGGER

#define COMMON_STR_SIZE 128

struct S_ATTRIBUTES {
  Bit64u attrb;
  Bit64u mask;
  int   index;
  char  str[32];
  int   groups[10];  // up to 10 items can be grouped.  Increase if we need more.
};

enum {
  UHCI_REG_COMMAND,
  UHCI_REG_STATUS,
  UHCI_REG_INTERRUPT,
  UHCI_REG_FRAME_NUM,
  UHCI_REG_FRAME_ADDR,
  UHCI_REG_SOF,
  UHCI_REG_PORT0,
  UHCI_REG_PORT1,
  UHCI_REG_COUNT
};

// copy&paste start
// from uhci_core.h
#define USB_UHCI_QUEUE_STACK_SIZE  256

#define USB_UHCI_IS_LINK_VALID(item)  ((item & 1) == 0)  // return TRUE if valid link address
#define USB_UHCI_IS_LINK_QUEUE(item)  ((item & 2) == 2)  // return TRUE if link is a queue pointer

struct USB_UHCI_QUEUE_STACK {
  int    queue_cnt;
  Bit32u queue_stack[USB_UHCI_QUEUE_STACK_SIZE];
};

#pragma pack (push, 1)
struct TD {
  Bit32u dword0;
  Bit32u dword1;
  Bit32u dword2;
  Bit32u dword3;
};

struct QUEUE {
  Bit32u horz;
  Bit32u vert;
};
#pragma pack (pop)

// from usb_xhci.h
#define OPS_REGS_OFFSET   0x20
#define XHCI_PORT_SET_OFFSET  (0x400 + OPS_REGS_OFFSET)
// copy&paste end

extern const char *hc_param_str[];

extern int usb_debug_type;
extern bx_param_c *host_param;
extern Bit32u pci_bar_address;

// UHCI
extern struct S_ATTRIBUTES attribs_u_command[];
extern struct S_ATTRIBUTES attribs_u_status[];
extern struct S_ATTRIBUTES attribs_u_interrupt[];
extern struct S_ATTRIBUTES attribs_u_ports[];

extern bool u_changed[UHCI_REG_COUNT];

bool uhci_add_queue(struct USB_UHCI_QUEUE_STACK *stack, const Bit32u addr);

// xHCI
extern struct S_ATTRIBUTES attribs_x_ports[];

// USB debug API
void usb_dbg_register_type(int type);

int usb_dbg_interface(int type, int param1, int param2);

void usb_dbg_trigger(int type, int trigger, int param1, int param2);

int usb_debug_dialog(int break_type, int param1, int param2);

Bit32u get_pci_bar_addr(bx_shadow_data_c *pci_conf, Bit8u bar_num);

Bit32u usb_io_read(Bit16u addr, unsigned io_len);

void usb_io_write(Bit16u addr, Bit32u value, unsigned io_len);

Bit32u xhci_read_dword(const Bit32u address);

#endif  // BX_USB_DEBUGGER
#endif  // BX_USB_DEBUG_H
