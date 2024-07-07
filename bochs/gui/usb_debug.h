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

extern int usb_debug_type;

void usb_dbg_register_type(int type);

int usb_dbg_interface(int type, int param1, int param2);

void usb_dbg_trigger(int type, int trigger, int param1, int param2);

int usb_debug_dialog(int break_type, int param1, int param2);

#endif  // BX_USB_DEBUGGER
#endif  // BX_USB_DEBUG_H
