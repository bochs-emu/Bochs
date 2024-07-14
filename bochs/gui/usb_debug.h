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

extern const char *hc_param_str[];

extern int usb_debug_type;
extern bx_param_c *host_param;

void usb_dbg_register_type(int type);

int usb_dbg_interface(int type, int param1, int param2);

void usb_dbg_trigger(int type, int trigger, int param1, int param2);

int usb_debug_dialog(int break_type, int param1, int param2);

Bit32u get_pci_bar_addr(bx_shadow_data_c *pci_conf, Bit8u bar_num);

Bit32u usb_io_read(Bit16u addr, unsigned io_len);

void usb_io_write(Bit16u addr, Bit32u value, unsigned io_len);

#endif  // BX_USB_DEBUGGER
#endif  // BX_USB_DEBUG_H
