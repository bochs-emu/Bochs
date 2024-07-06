/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C)      2023  Benjamin David Lunt
//  Copyright (C) 2003-2024  The Bochs Project
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

#include "bochs.h"

#if BX_USB_DEBUGGER

#include "bx_debug/debug.h"
#include "usb_debug.h"
#include "siminterface.h"
#include "gui.h"
#include "param_names.h"

#define LOG_THIS genlog->

int usb_debug_type = USB_DEBUG_NONE;

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

int usb_dbg_interface(int type, int wParam, int lParam)
{
  if (!bx_gui->has_gui_console()) {
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
        if (win32_usb_start(GetForegroundWindow(), type, wParam, lParam) < 0) {
          bx_user_quit = 1;
#if !BX_DEBUGGER
          bx_atexit();
          SIM->quit_sim(1);
#else
          bx_dbg_exit(1);
#endif
          return -1;
        }
      }
    }
  }
  return 0;
}

// one of the controllers has triggered a debug item.
void usb_dbg_trigger(int type, int trigger, int wParam, int lParam)
{
  if ((usb_debug_type == USB_DEBUG_NONE) || (type != usb_debug_type))
    return;

  bx_param_bool_c *bool_trigger;
  bx_param_num_c *num_trigger;
  switch (trigger) {
    case USB_DEBUG_FRAME:
      num_trigger = SIM->get_param_num(BXPN_USB_DEBUG_START_FRAME);
      if (num_trigger && (num_trigger->get() == BX_USB_DEBUG_SOF_TRIGGER)) {
        SIM->usb_debug_interface(USB_DEBUG_FRAME, wParam, lParam);
        num_trigger->set(BX_USB_DEBUG_SOF_SET);
      }
      break;

    case USB_DEBUG_COMMAND:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_DOORBELL);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_debug_interface(USB_DEBUG_COMMAND, wParam, lParam);
      break;

    case USB_DEBUG_EVENT:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_EVENT);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_debug_interface(USB_DEBUG_EVENT, wParam, lParam);
      break;

    case USB_DEBUG_NONEXIST:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_NON_EXIST);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_debug_interface(USB_DEBUG_NONEXIST, wParam, lParam);
      break;

    case USB_DEBUG_RESET:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_RESET);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_debug_interface(USB_DEBUG_RESET, wParam, lParam);
      break;

    case USB_DEBUG_ENABLE:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_ENABLE);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_debug_interface(USB_DEBUG_ENABLE, wParam, lParam);
      break;
  }
}

#endif
