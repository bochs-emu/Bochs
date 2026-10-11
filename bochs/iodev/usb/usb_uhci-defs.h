/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2023  Benjamin D Lunt (fys [at] fysnet [dot] net)
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

#ifndef BX_IODEV_USB_UHCI_DEFS_H
#define BX_IODEV_USB_UHCI_DEFS_H

#define USB_UHCI_PORTS 2

#define STATUS2_IOC (1<<0)
#define STATUS2_SPD (1<<1)

#define USB_UHCI_QUEUE_STACK_SIZE  256
#define USB_UHCI_LOOP_COUNT        256

#define USB_UHCI_IS_LINK_VALID(item)  ((item & 1) == 0)  // return TRUE if valid link address
#define USB_UHCI_IS_LINK_QUEUE(item)  ((item & 2) == 2)  // return TRUE if link is a queue pointer

// the standard max bandwidth (bytes per frame) for the UHCI is 1280 bytes
#define USB_UHCI_STD_MAX_BANDWIDTH  1280

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

#endif
