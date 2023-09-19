/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C)      2023  Benjamin David Lunt
//  Copyright (C) 2003-2021  The Bochs Project
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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"

#if BX_USE_WIN32USBDEBUG

#include "win32dialog.h"

#include "iodev.h"
#include "param_names.h"

#include "win32usbres.h"
#include "win32usb.h"

#include "iodev/usb/usb_common.h"

static const int dlg_resource[5] = {
  0,
  USB_DEBUG_UHCI_DLG,
  USB_DEBUG_OHCI_DLG,
  USB_DEBUG_EHCI_DLG,
  USB_DEBUG_XHCI_DLG
};

bx_param_c *host_param = NULL;
static const char *hc_param_str[5] = {
  "",
  BXPN_USB_UHCI,
  BXPN_USB_OHCI,
  BXPN_USB_EHCI,
  BXPN_USB_XHCI
};

static const DLGPROC usb_debug_callbacks[5] = {
  NULL,
  hc_uhci_callback,
  hc_ohci_callback,
  hc_ehci_callback,
  hc_xhci_callback
};

Bit32u pci_bar_address = 0;
struct CALLBACK_PARAMS g_params;

HFONT hTreeViewFont;

///////////////////////////////////////////////////////////////////////////////////////////////
//  Common to all HC types
//
void SetMouseCapture();
BOOL GetMouseCaptureMode();

// return 0 to continue with emulation
// return -1 to quit emulation
int win32_usb_start(HWND hwnd, int break_type, int wParam, int lParam)
{
  char str[COMMON_STR_SIZE];
  int ret;

  // get the (host controller) type we are to debug
  Bit32s type = SIM->get_param_enum(BXPN_USB_DEBUG_TYPE)->get();
  if ((type < USB_DEBUG_UHCI) || (type > USB_DEBUG_XHCI)) {
    sprintf(str, "Unknown host controller type given: %d", type);
    MessageBox(hwnd, str, NULL, MB_ICONINFORMATION);
    return 0;
  }

  // check to make sure the specified HC was enabled and in use
  host_param = SIM->get_param(hc_param_str[type]);
  if ((host_param == NULL) || !host_param->get_enabled()) {
    sprintf(str, "Parameter not found or enabled: %s", hc_param_str[type]);
    MessageBox(hwnd, str, NULL, MB_ICONINFORMATION);
    return 0;
  }

  // create a font for the TreeView
  hTreeViewFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
    OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
    DEFAULT_PITCH | FF_DONTCARE, TEXT("Cascadia"));
  if (hTreeViewFont == NULL) {
    hTreeViewFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
      OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
  }
  if (hTreeViewFont == NULL) {
    hTreeViewFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
      OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Courier New"));
  }
  if (hTreeViewFont == NULL) {
    MessageBox(hwnd, "Could not create a font for the Tree View Control", NULL, MB_ICONINFORMATION);
    return 0;
  }
  
  // if the mouse is currently being captured, we need to pause capture mode
  // (This is Win32 specific, though we are already Win32 specific, so it shouldn't matter)
  BOOL capture = GetMouseCaptureMode();
  if (capture) {
    bx_gui->mouse_enabled_changed_specific(0);
    SetMouseCapture();
  }

  // create the dialog and wait for it to return
  g_params.type = type;
  g_params.break_type = break_type;
  g_params.wParam = wParam;
  g_params.lParam = lParam;
  ret = (int) DialogBoxParam(NULL, MAKEINTRESOURCE(dlg_resource[type]), hwnd,
                             usb_debug_callbacks[type], (LPARAM) 0);
  // destroy the font
  DeleteObject(hTreeViewFont);

  // re-capture the mouse?
  if (capture) {
    bx_gui->mouse_enabled_changed_specific(1);
    SetMouseCapture();
  }
  
  return ret;
}

// one of the controllers has triggered a debug item.
void win32_usb_trigger(int type, int trigger, int wParam, int lParam) {

  // check that we are the correct controller type
  bx_param_enum_c *cntlr_type = SIM->get_param_enum(BXPN_USB_DEBUG_TYPE);
  if ((cntlr_type == NULL) || (cntlr_type->get() != type))
    return;
  
  bx_param_bool_c *bool_trigger;
  bx_param_num_c *num_trigger;
  switch (trigger) {
    case USB_DEBUG_FRAME:
      num_trigger = SIM->get_param_num(BXPN_USB_DEBUG_START_FRAME);
      if (num_trigger && (num_trigger->get() == BX_USB_DEBUG_SOF_TRIGGER)) {
        SIM->usb_config_interface(USB_DEBUG_FRAME, wParam, lParam);
        num_trigger->set(BX_USB_DEBUG_SOF_SET);
      }
      break;
      
    case USB_DEBUG_COMMAND:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_DOORBELL);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_config_interface(USB_DEBUG_COMMAND, wParam, lParam);
      break;
      
    case USB_DEBUG_EVENT:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_EVENT);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_config_interface(USB_DEBUG_EVENT, wParam, lParam);
      break;
      
    case USB_DEBUG_NONEXIST:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_NON_EXIST);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_config_interface(USB_DEBUG_NONEXIST, wParam, lParam);
      break;
      
    case USB_DEBUG_RESET:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_RESET);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_config_interface(USB_DEBUG_RESET, wParam, lParam);
      break;
      
    case USB_DEBUG_ENABLE:
      bool_trigger = SIM->get_param_bool(BXPN_USB_DEBUG_ENABLE);
      if (bool_trigger && bool_trigger->get())
        SIM->usb_config_interface(USB_DEBUG_ENABLE, wParam, lParam);
      break;
  }
}

HWND TreeView = NULL;
int tree_items = 0;
HTREEITEM TreeViewInsert(HWND TreeView, HTREEITEM Parent, HTREEITEM After, char *str, LPARAM lParam, Bit32u state) {
  TVINSERTSTRUCT tvis;

  // if Parent != NULL, change Parent to have children
  if (Parent != NULL) {
    tvis.item.mask = TVIF_HANDLE | TVIF_CHILDREN;
    tvis.item.hItem = Parent;
    tvis.item.cChildren = 1;
    TreeView_SetItem(TreeView, &tvis.item);
  }
  tvis.hParent = Parent;
  tvis.hInsertAfter = After;
  tvis.item.mask = TVIF_TEXT | TVIF_STATE | TVIF_PARAM;
  tvis.item.pszText = str;
  tvis.item.state = TVIS_EXPANDED | state;
  tvis.item.stateMask = TVIS_EXPANDED | state;
  tvis.item.lParam = lParam;
  tree_items++;
  return TreeView_InsertItem(TreeView, &tvis);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  UHCI
//
struct S_ATTRIBUTES attribs_u_command[] = {
                                           //            |                               | <- max (col 67)
  { (1<<7),                  (1<<7),                  7, "Max Packet"                     , {-1, } },
  { (1<<6),                  (1<<6),                  6, "Configure Flag"                 , {-1, } },
  { (1<<5),                  (1<<5),                  5, "Software Debug"                 , {-1, } },
  { (1<<4),                  (1<<4),                  4, "Force Global Resume"            , {-1, } },
  { (1<<3),                  (1<<3),                  3, "Enter Global Suspend Mode"      , {-1, } },
  { (1<<2),                  (1<<2),                  2, "Global Reset"                   , {-1, } },
  { (1<<1),                  (1<<1),                  1, "Host Controller Reset"          , {-1, } },
  { (1<<0),                  (1<<0),                  0, "Run/Stop"                       , {-1, } },
  { 0,                   (DWORD) -1,                 -1, NULL                             , {-1, } }
};

struct S_ATTRIBUTES attribs_u_status[] = {
  { (1<<5),                  (1<<5),                  5, "HCHalted"                       , {-1, } },
  { (1<<4),                  (1<<4),                  4, "Host Controller Process Error"  , {-1, } },
  { (1<<3),                  (1<<3),                  3, "Host System Error"              , {-1, } },
  { (1<<2),                  (1<<2),                  2, "Resume Detect"                  , {-1, } },
  { (1<<1),                  (1<<1),                  1, "USB Error Interrupt"            , {-1, } },
  { (1<<0),                  (1<<0),                  0, "USB Interrupt"                  , {-1, } },
  { 0,                   (DWORD) -1,                 -1, NULL                             , {-1, } }
};

struct S_ATTRIBUTES attribs_u_interrupt[] = {
  { (1<<3),                  (1<<3),                  3, "Short packet Interrupt Enable"  , {-1, } },
  { (1<<2),                  (1<<2),                  2, "Interrupt On Complete (IOC)"    , {-1, } },
  { (1<<1),                  (1<<1),                  1, "Resume Interrupt Enable"        , {-1, } },
  { (1<<0),                  (1<<0),                  0, "Timeout/CRC Interrupt Enable"   , {-1, } },
  { 0,                   (DWORD) -1,                 -1, NULL                             , {-1, } }
};

struct S_ATTRIBUTES attribs_u_ports[] = {
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
  { 0,                   (DWORD) -1,                 -1, NULL                             , {-1, } }
};

BOOL u_changed[IDC_U_EN_END - IDC_U_EN_START + 1];

// lParam: type is in low 8 bits, break_type in high 8-bits of low word
static INT_PTR CALLBACK hc_uhci_callback(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];

  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "Bochs for Windows -- USB Debug: UHCI Host Controller");
      SetWindowText(hDlg, str);
      
      // call the initializer
      TreeView = GetDlgItem(hDlg, IDC_STACK);
      SNDMSG(TreeView, WM_SETFONT, (WPARAM) hTreeViewFont, FALSE);
      if (hc_uhci_init(hDlg) < 0) {
        MessageBox(hDlg, "Error initializing dialog", NULL, MB_ICONINFORMATION);
      }
      
      memset(u_changed, 0, sizeof(u_changed));
      EnableWindow(GetDlgItem(hDlg, ID_APPLY), 0);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case ID_CONTINUE_EMU:
              if (TreeView)
                TreeView_DeleteAllItems(TreeView);
              TreeView = NULL;
              EndDialog(hDlg, 1);
              break;
            case ID_QUIT_EMU:
              if (TreeView)
                TreeView_DeleteAllItems(TreeView);
              TreeView = NULL;
              EndDialog(hDlg, -1);  // -1 to quit the SIM
              break;
            case ID_APPLY:
              hc_uhci_save(hDlg);
              break;
            case IDC_U_REG_COMMAND_B:
              do_attributes(hDlg, IDC_U_REG_COMMAND, 4, "Command Register", attribs_u_command);
              break;
            case IDC_U_REG_STATUS_B:
              do_attributes(hDlg, IDC_U_REG_STATUS, 4, "Status Register", attribs_u_status);
              break;
            case IDC_U_REG_INTERRUPT_B:
              do_attributes(hDlg, IDC_U_REG_INTERRUPT, 4, "Interrupt Register", attribs_u_interrupt);
              break;
            case IDC_U_REG_PORT0_B:
              do_attributes(hDlg, IDC_U_REG_PORT0, 4, "Port0 Register", attribs_u_ports);
              break;
            case IDC_U_REG_PORT1_B:
              do_attributes(hDlg, IDC_U_REG_PORT1, 4, "Port1 Register", attribs_u_ports);
              break;
          }
          break;
        // one of the edit controls changed
        case EN_CHANGE:
          if ((LOWORD(wParam) >= IDC_U_EN_START) && (LOWORD(wParam) <= IDC_U_EN_END)) {
            u_changed[LOWORD(wParam) - IDC_U_EN_START] = 1;
            EnableWindow(GetDlgItem(hDlg, ID_APPLY), 1);
          }
          break;
      }
  }
  
  return 0;
}

#include "iodev/usb/uhci_core.h"
#include "iodev/usb/usb_uhci.h"

extern bx_usb_uhci_c *theUSB_UHCI;

static int hc_uhci_init(HWND hwnd)
{
  char str[COMMON_STR_SIZE];
  Bit32u frame_addr, frame_num;
  
  if (theUSB_UHCI == NULL)
    return -1;

  bool sof = (SIM->get_param_num(BXPN_USB_DEBUG_START_FRAME)->get() > 0);

  pci_bar_address = theUSB_UHCI->get_bar_addr(4);
  sprintf(str, "0x%04X", pci_bar_address);
  SetDlgItemText(hwnd, IDC_PORT_ADDR, str);
  
  sprintf(str, "0x%04X", theUSB_UHCI->read(pci_bar_address + 0, 2));
  SetDlgItemText(hwnd, IDC_U_REG_COMMAND, str);
  sprintf(str, "0x%04X", theUSB_UHCI->read(pci_bar_address + 2, 2));
  SetDlgItemText(hwnd, IDC_U_REG_STATUS, str);
  sprintf(str, "0x%04X", theUSB_UHCI->read(pci_bar_address + 4, 2));
  SetDlgItemText(hwnd, IDC_U_REG_INTERRUPT, str);
  sprintf(str, "0x%04X", frame_num = theUSB_UHCI->read(pci_bar_address + 6, 2));
  SetDlgItemText(hwnd, IDC_U_REG_FRAME_NUM, str);
  sprintf(str, "0x%08X", frame_addr = theUSB_UHCI->read(pci_bar_address + 8, 4));
  SetDlgItemText(hwnd, IDC_U_REG_FRAME_ADDRESS, str);
  sprintf(str, "0x%02X", theUSB_UHCI->read(pci_bar_address + 12, 1));
  SetDlgItemText(hwnd, IDC_U_REG_SOF, str);
  
  sprintf(str, "0x%04X", theUSB_UHCI->read(pci_bar_address + 16, 2));
  SetDlgItemText(hwnd, IDC_U_REG_PORT0, str);
  sprintf(str, "0x%04X", theUSB_UHCI->read(pci_bar_address + 18, 2));
  SetDlgItemText(hwnd, IDC_U_REG_PORT1, str);

  // display the port types
  SIM->get_param_enum("port1.device", host_param)->dump_param(str, COMMON_STR_SIZE, 1);
  SetDlgItemText(hwnd, IDC_U_REG_PORT0_TYPE, str);
  SIM->get_param_enum("port2.device", host_param)->dump_param(str, COMMON_STR_SIZE, 1);
  SetDlgItemText(hwnd, IDC_U_REG_PORT1_TYPE, str);

  frame_addr += (frame_num * sizeof(Bit32u));
  sprintf(str, "0x%08X", frame_addr);
  SetDlgItemText(hwnd, IDC_FRAME_ADDRESS, str);

  tree_items = 0;
  TreeView_DeleteAllItems(TreeView);
  if (sof && (frame_addr != 0x00000000)) {
    hc_uhci_do_item(frame_addr, frame_num);
  } else {
    TreeView_SetBkColor(TreeView, COLORREF(0x00A9A9A9));
    SetDlgItemText(hwnd, IDC_TREE_COMMENT, "(start_frame trigger not specified)");
  }

  return 0;
}

static int hc_uhci_save(HWND hwnd)
{
  char str[COMMON_STR_SIZE];

  if (u_changed[IDC_U_REG_COMMAND - IDC_U_EN_START]) {
    GetDlgItemText(hwnd, IDC_U_REG_COMMAND, str, COMMON_STR_SIZE);
    theUSB_UHCI->write(pci_bar_address + 0, strtol(str, NULL, 0), 2);
  }
  if (u_changed[IDC_U_REG_STATUS - IDC_U_EN_START]) {
    GetDlgItemText(hwnd, IDC_U_REG_STATUS, str, COMMON_STR_SIZE);
    theUSB_UHCI->write(pci_bar_address + 2, strtol(str, NULL, 0), 2);
  }
  if (u_changed[IDC_U_REG_INTERRUPT - IDC_U_EN_START]) {
    GetDlgItemText(hwnd, IDC_U_REG_INTERRUPT, str, COMMON_STR_SIZE);
    theUSB_UHCI->write(pci_bar_address + 4, strtol(str, NULL, 0), 2);
  }
  if (u_changed[IDC_U_REG_FRAME_NUM - IDC_U_EN_START]) {
    GetDlgItemText(hwnd, IDC_U_REG_FRAME_NUM, str, COMMON_STR_SIZE);
    theUSB_UHCI->write(pci_bar_address + 6, strtol(str, NULL, 0), 2);
  }
  if (u_changed[IDC_U_REG_FRAME_ADDRESS - IDC_U_EN_START]) {
    GetDlgItemText(hwnd, IDC_U_REG_FRAME_ADDRESS, str, COMMON_STR_SIZE);
    theUSB_UHCI->write(pci_bar_address + 8, strtol(str, NULL, 0), 4);
  }
  if (u_changed[IDC_U_REG_SOF - IDC_U_EN_START]) {
    GetDlgItemText(hwnd, IDC_U_REG_SOF, str, COMMON_STR_SIZE);
    theUSB_UHCI->write(pci_bar_address + 12, strtol(str, NULL, 0), 1);
  }
  
  if (u_changed[IDC_U_REG_PORT0 - IDC_U_EN_START]) {
    GetDlgItemText(hwnd, IDC_U_REG_PORT0, str, COMMON_STR_SIZE);
    theUSB_UHCI->write(pci_bar_address + 16, strtol(str, NULL, 0), 2);
  }
  if (u_changed[IDC_U_REG_PORT1 - IDC_U_EN_START]) {
    GetDlgItemText(hwnd, IDC_U_REG_PORT1, str, COMMON_STR_SIZE);
    theUSB_UHCI->write(pci_bar_address + 18, strtol(str, NULL, 0), 2);
  }
  
  memset(u_changed, 0, sizeof(u_changed));
  EnableWindow(GetDlgItem(hwnd, ID_APPLY), 0);

  return 0;
}

static bool uhci_add_queue(struct USB_UHCI_QUEUE_STACK *stack, const Bit32u addr)
{
  // check to see if this queue has been processed before
  for (int i=0; i<stack->queue_cnt; i++) {
    if (stack->queue_stack[i] == addr)
      return 1;
  }

  // if the stack is full, we return TRUE anyway
  if (stack->queue_cnt == USB_UHCI_QUEUE_STACK_SIZE)
    return 1;

  // add the queue's address
  stack->queue_stack[stack->queue_cnt] = addr;
  stack->queue_cnt++;
  
  return 0;
}

void hc_uhci_do_item(Bit32u FrameAddr, Bit32u FrameNum)
{
  struct USB_UHCI_QUEUE_STACK queue_stack;
  int  td_count = 0; // count of TD's processed under a queue
  Bit32u item, queue_addr = 0;
  struct QUEUE queue;
  struct TD td;
  char str[COMMON_STR_SIZE];
  HTREEITEM Next;

  // get the frame pointer
  DEV_MEM_READ_PHYSICAL(FrameAddr, sizeof(Bit32u), (Bit8u *) &item);
  sprintf(str, "Frame Pointer(%i): 0x%08X", FrameNum, item);
  Next = TreeViewInsert(TreeView, TVI_ROOT, TVI_FIRST, str, 0, 0);

  queue_stack.queue_cnt = 0;

  // A catch to make sure we don't do too many
  while (tree_items < 50) {
    if (!USB_UHCI_IS_LINK_VALID(item))  // the the T bit is set, we are done
      break;

    // is it a queue?
    if (USB_UHCI_IS_LINK_QUEUE(item)) {
      // add it to our current list of queues
      if (uhci_add_queue(&queue_stack, item & ~0xF)) {
        // this queue has been processed before. Did we process
        //  any TD's between the last time and now? If not, be done.
        if (td_count == 0) {
          break;
        } else {
          // reset the queue stack to start here
          td_count = 0;
          queue_stack.queue_cnt = 0;
          uhci_add_queue(&queue_stack, item & ~0xF);
        }
      }
      
      // read in the queue
      DEV_MEM_READ_PHYSICAL(item & ~0xF, sizeof(struct QUEUE), (Bit8u *) &queue);
      sprintf(str, "0x%08X: Queue Head: (0x%08X 0x%08X)", item & ~0xF, queue.horz, queue.vert);
      Next = TreeViewInsert(TreeView, Next, TVI_LAST, str, 0, 0);
      
      // if the vert pointer is valid, there are td's in it to process
      //  else only the head pointer may be valid
      if (!USB_UHCI_IS_LINK_VALID(queue.vert)) {
        // no vertical elements to process
        // (clear queue_addr to indicate we are not processing
        //  elements of the vertical part of a queue)
        queue_addr = 0;
        item = queue.horz;
      } else {
        // there are vertical elements to process
        // (save the address of the horz pointer in queue_addr
        //  so that we may update the queue's vertical pointer
        //  member with the successfully processed TD's link pointer)
        queue_addr = item;
        item = queue.vert;
      }
      continue;
    }

    DEV_MEM_READ_PHYSICAL(item & ~0xF, sizeof(struct TD), (Bit8u *) &td);
    const bool depthbreadth = (td.dword0 & 0x0004) ? 1 : 0;     // 1 = depth first, 0 = breadth first
    const bool is_active = (td.dword1 & (1<<23)) > 0;
    if (is_active) {  // is it an active TD
      sprintf(str, "0x%08X: TD: (0x%08X)", item & ~0xF, td.dword0);
      TreeViewInsert(TreeView, Next, TVI_LAST, str, 0, 0);

      // we processed another td within this queue line
      td_count++;

      item = td.dword0;
      if (queue_addr != 0) {
        // if breadth first or last in the element list, move on to next queue item
        if (!depthbreadth || !USB_UHCI_IS_LINK_VALID(item)) {
          item = queue.horz;
          queue_addr = 0;
        }
      }
      continue;
    }

    // move to next item (no queues) or queue head (queues found)
    item = (queue_addr != 0) ? queue.horz : td.dword0;
  }
}




///////////////////////////////////////////////////////////////////////////////////////////////
//  OHCI
//

// lParam: type is in low 8 bits, break_type in high 8-bits of low word
static INT_PTR CALLBACK hc_ohci_callback(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{

  return 0;
}

static int hc_ohci_init(HWND hwnd) {

  return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//  EHCI
//

// lParam: type is in low 8 bits, break_type in high 8-bits of low word
static INT_PTR CALLBACK hc_ehci_callback(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{

  return 0;
}

static int hc_ehci_init(HWND hwnd)
{

  return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//  XHCI
//

struct S_ATTRIBUTES attribs_x_ports[] = {
  
                                           //            |                               | <- max (col 67)
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
  { 0,                   (DWORD) -1,                 -1, NULL                             , {-1, } }
};

BOOL x_changed[IDC_X_EN_END - IDC_X_EN_START + 1];

// lParam: type is in low 8 bits, break_type in high 8-bits of low word
static INT_PTR CALLBACK hc_xhci_callback(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "Bochs for Windows -- USB Debug: xHCI Host Controller");
      SetWindowText(hDlg, str);
      
      // call the initializer
      TreeView = GetDlgItem(hDlg, IDC_STACK);
      SNDMSG(TreeView, WM_SETFONT, (WPARAM) hTreeViewFont, FALSE);
      if (hc_xhci_init(hDlg) < 0) {
        MessageBox(hDlg, "Error initializing dialog", NULL, MB_ICONINFORMATION);
      }
      
      memset(x_changed, 0, sizeof(x_changed));
      EnableWindow(GetDlgItem(hDlg, ID_APPLY), 0);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case ID_CONTINUE_EMU:
              if (TreeView)
                TreeView_DeleteAllItems(TreeView);
              TreeView = NULL;
              SIM->get_param_bool(BXPN_USB_DEBUG_RESET)->set((IsDlgButtonChecked(hDlg, IDC_DEBUG_RESET) == BST_CHECKED) ? 1 : 0);
              SIM->get_param_bool(BXPN_USB_DEBUG_ENABLE)->set((IsDlgButtonChecked(hDlg, IDC_DEBUG_ENABLE) == BST_CHECKED) ? 1 : 0);
              SIM->get_param_bool(BXPN_USB_DEBUG_DOORBELL)->set((IsDlgButtonChecked(hDlg, IDC_DEBUG_DOORBELL) == BST_CHECKED) ? 1 : 0);
              SIM->get_param_bool(BXPN_USB_DEBUG_EVENT)->set((IsDlgButtonChecked(hDlg, IDC_DEBUG_EVENT) == BST_CHECKED) ? 1 : 0);
              SIM->get_param_num(BXPN_USB_DEBUG_START_FRAME)->set((IsDlgButtonChecked(hDlg, IDC_DEBUG_SOF) == BST_CHECKED) ? BX_USB_DEBUG_SOF_SET : BX_USB_DEBUG_SOF_NONE);
              SIM->get_param_bool(BXPN_USB_DEBUG_NON_EXIST)->set((IsDlgButtonChecked(hDlg, IDC_DEBUG_NONEXIST) == BST_CHECKED) ? 1 : 0);
              EndDialog(hDlg, 1);
              break;
            case ID_QUIT_EMU:
              if (TreeView)
                TreeView_DeleteAllItems(TreeView);
              TreeView = NULL;
              EndDialog(hDlg, -1);  // -1 to quit the SIM
              break;
            case ID_APPLY:
              hc_xhci_save(hDlg);
              break;
              /*
            case IDC_X_REG_COMMAND_B:
              do_attributes(hDlg, IDC_X_REG_COMMAND, 4, "Command Register", attribs_u_command);
              break;
            case IDC_X_REG_STATUS_B:
              do_attributes(hDlg, IDC_X_REG_STATUS, 4, "Status Register", attribs_u_status);
              break;
            case IDC_X_REG_INTERRUPT_B:
              do_attributes(hDlg, IDC_X_REG_INTERRUPT, 4, "Interrupt Register", attribs_u_interrupt);
              break;
              */
            case IDC_X_REG_PORT0_B:
              do_attributes(hDlg, IDC_X_REG_PORT0, 8, "Port0 Register", attribs_x_ports);
              break;
            case IDC_X_REG_PORT1_B:
              do_attributes(hDlg, IDC_X_REG_PORT1, 8, "Port1 Register", attribs_x_ports);
              break;
            case IDC_X_REG_PORT2_B:
              do_attributes(hDlg, IDC_X_REG_PORT2, 8, "Port2 Register", attribs_x_ports);
              break;
            case IDC_X_REG_PORT3_B:
              do_attributes(hDlg, IDC_X_REG_PORT3, 8, "Port3 Register", attribs_x_ports);
              break;
            case IDC_X_REG_PORT4_B:
              do_attributes(hDlg, IDC_X_REG_PORT4, 8, "Port4 Register", attribs_x_ports);
              break;
            case IDC_X_REG_PORT5_B:
              do_attributes(hDlg, IDC_X_REG_PORT5, 8, "Port5 Register", attribs_x_ports);
              break;
            case IDC_X_REG_PORT6_B:
              do_attributes(hDlg, IDC_X_REG_PORT6, 8, "Port6 Register", attribs_x_ports);
              break;
            case IDC_X_REG_PORT7_B:
              do_attributes(hDlg, IDC_X_REG_PORT7, 8, "Port7 Register", attribs_x_ports);
              break;
            case IDC_X_REG_PORT8_B:
              do_attributes(hDlg, IDC_X_REG_PORT8, 8, "Port8 Register", attribs_x_ports);
              break;
            case IDC_X_REG_PORT9_B:
              do_attributes(hDlg, IDC_X_REG_PORT9, 8, "Port9 Register", attribs_x_ports);
              break;
            case IDC_VIEW_TRB:
              switch (g_params.break_type) {
                case USB_DEBUG_COMMAND:
                  xhci_display_trb(hDlg, VIEW_TRB_TYPE_COMMAND);
                  break;
                case USB_DEBUG_EVENT:
                  xhci_display_trb(hDlg, VIEW_TRB_TYPE_EVENT);
                  break;
                case USB_DEBUG_FRAME:
                  xhci_display_trb(hDlg, VIEW_TRB_TYPE_TRANSFER);
                  break;
              }
              break;
          }
          break;
        // one of the edit controls changed
        case EN_CHANGE:
          if ((LOWORD(wParam) >= IDC_X_EN_START) && (LOWORD(wParam) <= IDC_X_EN_END)) {
            u_changed[LOWORD(wParam) - IDC_X_EN_START] = 1;
            EnableWindow(GetDlgItem(hDlg, ID_APPLY), 1);
          }
          break;
      }
      /*
      * https://social.msdn.microsoft.com/Forums/en-US/60402c1d-d7a6-4406-9b83-67fa74c24104/select-an-item-in-a-treeview-with-a-right-click?forum=vcmfcatl
    case WM_NOTIFY:
      switch (((LPNMHDR) lParam)->code) {
        case NM_RCLICK:
          MessageBox(hDlg, "Right Click", NULL, MB_ICONINFORMATION);
          break;
      }
      break;
      */
  }
  
  return 0;
}

#include "iodev/usb/usb_xhci.h"

extern bx_usb_xhci_c *theUSB_XHCI;

static Bit32u xhci_read_dword(const Bit32u address)
{
  Bit32u value;
  
  theUSB_XHCI->read_handler(address, 4, &value, NULL);
  return value;
}

static int hc_xhci_init(HWND hwnd)
{
  char str[COMMON_STR_SIZE];
  char str1[COMMON_STR_SIZE];
  Bit64u RingPtr = 0, qword;
  Bit32u dword, offset;
  unsigned i;

  if (theUSB_XHCI == NULL)
    return -1;

  // set the dialog title to the break type
  switch (g_params.break_type) {
    case USB_DEBUG_COMMAND:
      SetWindowText(hwnd, "xHCI Debug Dialog: Break Type: Command Ring");
      break;
    case USB_DEBUG_EVENT:
      SetWindowText(hwnd, "xHCI Debug Dialog: Break Type: Event Ring");
      break;
    case USB_DEBUG_FRAME:
      SetWindowText(hwnd, "xHCI Debug Dialog: Break Type: Frame");
      break;
  }
  
  //bool sof = (SIM->get_param_num(BXPN_USB_DEBUG_START_FRAME)->get() > 0);
  CheckDlgButton(hwnd, IDC_DEBUG_RESET,    SIM->get_param_bool(BXPN_USB_DEBUG_RESET)->get() ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(hwnd, IDC_DEBUG_ENABLE,   SIM->get_param_bool(BXPN_USB_DEBUG_ENABLE)->get() ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(hwnd, IDC_DEBUG_DOORBELL, SIM->get_param_bool(BXPN_USB_DEBUG_DOORBELL)->get() ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(hwnd, IDC_DEBUG_EVENT,    SIM->get_param_bool(BXPN_USB_DEBUG_EVENT)->get() ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(hwnd, IDC_DEBUG_SOF,     (SIM->get_param_num(BXPN_USB_DEBUG_START_FRAME)->get() > BX_USB_DEBUG_SOF_NONE) ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(hwnd, IDC_DEBUG_NONEXIST, SIM->get_param_bool(BXPN_USB_DEBUG_NON_EXIST)->get() ? BST_CHECKED : BST_UNCHECKED);

  pci_bar_address = theUSB_XHCI->get_bar_addr(0);
  sprintf(str, "0x%08X", pci_bar_address);
  SetDlgItemText(hwnd, IDC_PORT_ADDR, str);
  
  for (i=0; i<8; i++) {
    dword = xhci_read_dword(pci_bar_address + (i * 4));
    sprintf(str, "0x%08X", dword);
    SetDlgItemText(hwnd, IDC_X_REG_CAPLENGTH + i, str);
  }
  offset = xhci_read_dword(pci_bar_address + 0) & 0xFF;

  dword = xhci_read_dword(pci_bar_address + offset + 0x00);
  sprintf(str, "0x%08X", dword);
  SetDlgItemText(hwnd, IDC_X_REG_COMMAND, str);
  dword = xhci_read_dword(pci_bar_address + offset + 0x04);
  sprintf(str, "0x%08X", dword);
  SetDlgItemText(hwnd, IDC_X_REG_STATUS, str);
  dword = xhci_read_dword(pci_bar_address + offset + 0x08);
  sprintf(str, "0x%08X", dword);
  SetDlgItemText(hwnd, IDC_X_REG_PAGESIZE, str);
  dword = xhci_read_dword(pci_bar_address + offset + 0x14);
  sprintf(str, "0x%08X", dword);
  SetDlgItemText(hwnd, IDC_X_REG_DEVICE_NOTE, str);
  sprintf(str, "0x" FMT_ADDRX64, theUSB_XHCI->hub.op_regs.HcCrcr.actual); // we can't read this using read_handler() since the handler will return zero
  SetDlgItemText(hwnd, IDC_X_REG_COMMAND_RING, str);
  qword = xhci_read_dword(pci_bar_address + offset + 0x30) |
   ((Bit64u) xhci_read_dword(pci_bar_address + offset + 0x34) << 32);
  sprintf(str, "0x" FMT_ADDRX64, qword);
  SetDlgItemText(hwnd, IDC_X_REG_DEV_CONTEXT_BASE, str);
  dword = xhci_read_dword(pci_bar_address + offset + 0x38);
  sprintf(str, "0x%08X", dword);
  SetDlgItemText(hwnd, IDC_X_REG_CONFIGURE, str);
  
  offset = xhci_read_dword(pci_bar_address + 0x18);
  dword = xhci_read_dword(pci_bar_address + offset + 0);
  sprintf(str, "0x%08X", dword);
  SetDlgItemText(hwnd, IDC_X_REG_MFINDEX, str);
  
  // show up to 10 port register sets
  for (i=0; i<theUSB_XHCI->hub.n_ports; i++) {
    dword = xhci_read_dword(pci_bar_address + XHCI_PORT_SET_OFFSET + (i * 16));
    sprintf(str, "0x%08X", dword);
    SetDlgItemText(hwnd, IDC_X_REG_PORT0 + i, str);
    sprintf(str1, "port%d.device", i + 1);
    SIM->get_param_enum(str1, host_param)->dump_param(str, COMMON_STR_SIZE, 1);
    SetDlgItemText(hwnd, IDC_X_REG_PORT0_TYPE + i, str);
  }
  for (; i<USB_XHCI_PORTS_MAX; i++) {
    EnableWindow(GetDlgItem(hwnd, IDC_X_REG_EPORT0 + i), 0);
    EnableWindow(GetDlgItem(hwnd, IDC_X_REG_PORT0 + i), 0);
    EnableWindow(GetDlgItem(hwnd, IDC_X_REG_PORT0_B + i), 0);
    EnableWindow(GetDlgItem(hwnd, IDC_X_REG_PORT0_ETYPE + i), 0);
    EnableWindow(GetDlgItem(hwnd, IDC_X_REG_PORT0_TYPE + i), 0);
  }
  
  tree_items = 0;
  TreeView_DeleteAllItems(TreeView);

  bool valid = 0;
  switch (g_params.break_type) {
    // a command TRB was placed on the command ring
    case USB_DEBUG_COMMAND:
      SetDlgItemText(hwnd, IDC_RING_TYPE, "Command Ring Address:");
      RingPtr = theUSB_XHCI->hub.op_regs.HcCrcr.crc;
      sprintf(str, "0x" FMT_ADDRX64, RingPtr);
      SetDlgItemText(hwnd, IDC_FRAME_ADDRESS, str);
      if (RingPtr != 0) {
        hc_xhci_do_ring("Command", RingPtr, theUSB_XHCI->hub.ring_members.command_ring.dq_pointer);
        EnableWindow(GetDlgItem(hwnd, IDC_VIEW_TRB), 1);
        valid = 1;
      }
      break;
    
    // an event TRB was placed on an event ring
    case USB_DEBUG_EVENT:
      SetDlgItemText(hwnd, IDC_RING_TYPE, "Event Ring:");
      sprintf(str, "Interrupter %i", (int) g_params.wParam);
      SetDlgItemText(hwnd, IDC_FRAME_ADDRESS, str);
      hc_xhci_do_event_ring("Event", (int) g_params.wParam);
      EnableWindow(GetDlgItem(hwnd, IDC_VIEW_TRB), 1);
      valid = 1;
      break;
      
    case USB_DEBUG_FRAME:
      
      SetDlgItemText(hwnd, IDC_RING_TYPE, "SOF Ring Address:");

      break;
    
    // first byte (word, dword, qword) of first non-existant port was written to
    case USB_DEBUG_NONEXIST:
      SetDlgItemText(hwnd, IDC_RING_TYPE, "None");
      SetDlgItemText(hwnd, IDC_FRAME_ADDRESS, "None");
      EnableWindow(GetDlgItem(hwnd, IDC_VIEW_TRB), 0);
      valid = 0;
      break;

    // port reset (non-root reset)
    case USB_DEBUG_RESET:
      SetDlgItemText(hwnd, IDC_RING_TYPE, "None");
      SetDlgItemText(hwnd, IDC_FRAME_ADDRESS, "None");
      EnableWindow(GetDlgItem(hwnd, IDC_VIEW_TRB), 0);
      valid = 0;
      break;
      
    // enable changed
    case USB_DEBUG_ENABLE:
      SetDlgItemText(hwnd, IDC_RING_TYPE, "None");
      SetDlgItemText(hwnd, IDC_FRAME_ADDRESS, "None");
      EnableWindow(GetDlgItem(hwnd, IDC_VIEW_TRB), 0);
      valid = 0;
      break;
  }
  
  if (!valid) {
    TreeView_SetBkColor(TreeView, COLORREF(0x00A9A9A9));
    //SetDlgItemText(hwnd, IDC_TREE_COMMENT, "(start_frame trigger not specified)");
  }

  return 0;
}

static int hc_xhci_save(HWND hwnd)
{
  
  MessageBox(hwnd, "xHCI: Save to controller is not yet implemented!", NULL, MB_ICONINFORMATION);
  
  return 0;
}

static const struct VIEW_TRB_TYPE trb_types[] = {
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
static const char *ring_type[] = {
  "",         // VIEW_TRB_TYPE_NONE
  "Command",  // VIEW_TRB_TYPE_COMMAND
  "Event",    // VIEW_TRB_TYPE_EVENT
  "",         //
  "Transfer"  // VIEW_TRB_TYPE_TRANSFER
};

static void hc_xhci_do_ring(char *ring_str, Bit64u RingPtr, Bit64u dequeue_ptr)
{
  char str[COMMON_STR_SIZE];
  int  trb_count = 0; // count of TRB's processed
  Bit64u address = RingPtr;
  struct TRB trb;
  HTREEITEM Parent;
  Bit32u state = 0;
  Bit8u type;

  sprintf(str, "%s Ring: 0x" FMT_ADDRX64, ring_str, address);
  Parent = TreeViewInsert(TreeView, TVI_ROOT, TVI_FIRST, str, 0, 0);

  do {
    state = 0; // clear the state
    DEV_MEM_READ_PHYSICAL(address, sizeof(struct TRB), (Bit8u *) &trb);
    type = TRB_GET_TYPE(trb.command);
    if (type <= 47)
      sprintf(str, "0x" FMT_ADDRX64 " %08X 0x%08X (%i) (%s)", trb.parameter, trb.status, trb.command, trb.command & 1, trb_types[type].name);
    else 
      sprintf(str, "0x" FMT_ADDRX64 " %08X 0x%08X (%i) (Vendor Specific)", trb.parameter, trb.status, trb.command, trb.command & 1);
    if (address == dequeue_ptr) {
      strcat(str, " <--- dq_pointer");
      state |= TVIS_BOLD;
    }
    TreeViewInsert(TreeView, Parent, TVI_LAST, str, (LPARAM) address, state);
    if (type == LINK) {
      address = trb.parameter & (Bit64u) ~0xF;
    } else
      address += sizeof(struct TRB);

    if (++trb_count > MAX_TRBS_ALLOWED)  // safety catch
      break;
  } while (address != RingPtr);
}

static void hc_xhci_do_event_ring(char *ring_str, int interrupter)
{
  char str[COMMON_STR_SIZE];
  int  trb_count = 0; // count of TRB's processed
  Bit64u address;
  struct TRB trb;
  HTREEITEM Parent, Segment;
  Bit32u state = 0;
  Bit8u type;

  sprintf(str, "%s Ring: Interrupter: %i", ring_str, interrupter);
  Parent = TreeViewInsert(TreeView, TVI_ROOT, TVI_FIRST, str, 0, 0);

  for (unsigned i=0; i<(1<<MAX_SEG_TBL_SZ_EXP); i++) {
    address = theUSB_XHCI->hub.ring_members.event_rings[interrupter].entrys[i].addr;
    sprintf(str, "Event Ring Segment %i (0x" FMT_ADDRX64 "), size %i", i, address, theUSB_XHCI->hub.ring_members.event_rings[interrupter].entrys[i].size);
    Segment = TreeViewInsert(TreeView, Parent, TVI_LAST, str, 0, 0);
    for (unsigned j=0; j<theUSB_XHCI->hub.ring_members.event_rings[interrupter].entrys[i].size; j++) {
      state = 0; // clear the state
      DEV_MEM_READ_PHYSICAL(address, sizeof(struct TRB), (Bit8u *) &trb);
      type = TRB_GET_TYPE(trb.command);
      if (type <= 47)
        sprintf(str, "0x" FMT_ADDRX64 " %08X 0x%08X (%i) (%s)", trb.parameter, trb.status, trb.command, trb.command & 1, trb_types[type].name);
      else 
        sprintf(str, "0x" FMT_ADDRX64 " %08X 0x%08X (%i) (Vendor Specific)", trb.parameter, trb.status, trb.command, trb.command & 1);
      if (address == theUSB_XHCI->hub.ring_members.event_rings[interrupter].cur_trb) {
        strcat(str, " <--- eq_pointer");
        state |= TVIS_BOLD;
      }
      TreeViewInsert(TreeView, Segment, TVI_LAST, str, (LPARAM) address, state);

      if (++trb_count > MAX_TRBS_ALLOWED)  // safety catch
        break;
      
      address += sizeof(struct TRB);
    }
    if (trb_count > MAX_TRBS_ALLOWED)  // safety catch
      break;
  }
}

struct TRB g_trb;
static void xhci_display_trb(HWND hwnd, int type_mask) {
  char str[COMMON_STR_SIZE];
  TVITEM tvitem;
  INT_PTR ret = -1;

  HTREEITEM item = TreeView_GetSelection(TreeView);
  if (item != NULL) {
    tvitem.mask = TVIF_PARAM | TVIF_HANDLE;
    tvitem.hItem = item;
    TreeView_GetItem(TreeView, &tvitem);
    if (tvitem.lParam > 0) {
      DEV_MEM_READ_PHYSICAL(tvitem.lParam, sizeof(struct TRB), (Bit8u *) &g_trb);
      const Bit8u type = TRB_GET_TYPE(g_trb.command);
    
      // check to see if this type of TRB is allowed in this type of ring
      if ((type > 0) && (type <= 47) && ((trb_types[type].allowed_mask & type_mask) == 0)) {
        sprintf(str, "TRB type %i not allowed in a %s ring!", type, ring_type[type_mask]);
        MessageBox(hwnd, str, NULL, MB_ICONINFORMATION);
      }

      // using the type of trb, display an associated dialog
      switch (type) {
        case NORMAL:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_NORM_TRB), hwnd, hc_xhci_callback_trb_normal, 0);
          break;
        case SETUP_STAGE:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_SETUP_TRB), hwnd, hc_xhci_callback_trb_setup, 0);
          break;
        case DATA_STAGE:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_DATA_TRB), hwnd, hc_xhci_callback_trb_data, 0);
          break;
        case STATUS_STAGE:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_STATUS_TRB), hwnd, hc_xhci_callback_trb_status, 0);
          break;
        case LINK:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_LINK_TRB), hwnd, hc_xhci_callback_trb_link, 0);
          break;
        case EVENT_DATA:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_EVENT_TRB), hwnd, hc_xhci_callback_trb_event, 0);
          break;
        case NO_OP:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_NOOP_TRB), hwnd, hc_xhci_callback_trb_noop, 0);
          break;
        case ENABLE_SLOT:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_ENSLOT_TRB), hwnd, hc_xhci_callback_trb_enslot, 0);
          break;
        case DISABLE_SLOT:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_DISSLOT_TRB), hwnd, hc_xhci_callback_trb_disslot, 0);
          break;
        case ADDRESS_DEVICE:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_ADDRESS_TRB), hwnd, hc_xhci_callback_trb_address, 0);
          break;
        case CONFIG_EP:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_CONFIGEP_TRB), hwnd, hc_xhci_callback_trb_configep, 0);
          break;
        case EVALUATE_CONTEXT:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_EVALUATE_TRB), hwnd, hc_xhci_callback_trb_evaluate, 0);
          break;
        case RESET_EP:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_RESETEP_TRB), hwnd, hc_xhci_callback_trb_resetep, 0);
          break;
        case STOP_EP:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_STOPEP_TRB), hwnd, hc_xhci_callback_trb_stopep, 0);
          break;
        case SET_TR_DEQUEUE:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_SETTRPTR_TRB), hwnd, hc_xhci_callback_trb_settrptr, 0);
          break;
        case RESET_DEVICE:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_RESETDEV_TRB), hwnd, hc_xhci_callback_trb_resetdev, 0);
          break;
        case FORCE_EVENT:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_FORCE_TRB), hwnd, hc_xhci_callback_trb_forceevent, 0);
          break;
        case DEG_BANDWIDTH:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_DISSLOT_TRB), hwnd, hc_xhci_callback_trb_disslot, 0);
          break;
        case SET_LAT_TOLERANCE:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_SETLAT_TRB), hwnd, hc_xhci_callback_trb_setlat, 0);
          break;
        case GET_PORT_BAND:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_GETBAND_TRB), hwnd, hc_xhci_callback_trb_getband, 0);
          break;
        case FORCE_HEADER:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_FORCEHDR_TRB), hwnd, hc_xhci_callback_trb_forcehdr, 0);
          break;
        case NO_OP_CMD:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_RESETDEV_TRB), hwnd, hc_xhci_callback_trb_resetdev, 0);
          break;
        case TRANS_EVENT:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_TRANSEVENT_TRB), hwnd, hc_xhci_callback_trb_transevent, 0);
          break;
        case COMMAND_COMPLETION:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_CMNDCOMP_TRB), hwnd, hc_xhci_callback_trb_compcompletion, 0);
          break;
        case PORT_STATUS_CHANGE:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_PSCHANGE_TRB), hwnd, hc_xhci_callback_trb_pschange, 0);
          break;
        case BANDWIDTH_REQUEST:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_BANDREQU_TRB), hwnd, hc_xhci_callback_trb_bandrequ, 0);
          break;
        case DOORBELL_EVENT:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_DOORBELL_TRB), hwnd, hc_xhci_callback_trb_doorbell, 0);
          break;
        case HOST_CONTROLLER_EVENT:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_HOSTEVENT_TRB), hwnd, hc_xhci_callback_trb_hostevent, 0);
          break;
        case DEVICE_NOTIFICATION:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_DEVNOTEVENT_TRB), hwnd, hc_xhci_callback_trb_devnotevent, 0);
          break;
        case MFINDEX_WRAP:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_HOSTEVENT_TRB), hwnd, hc_xhci_callback_trb_hostevent, 0);
          break;
        case NEC_TRB_TYPE_CMD_COMP:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_NECFWEVENT_TRB), hwnd, hc_xhci_callback_trb_necfwevent, 0);
          break;
        case NEC_TRB_TYPE_GET_FW:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_NECFW_TRB), hwnd, hc_xhci_callback_trb_necfw, 0);
          break;
        case NEC_TRB_TYPE_GET_UN:
          ret = DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_NECUN_TRB), hwnd, hc_xhci_callback_trb_necun, 0);
          break;
        case BX_TRB_TYPE_DUMP:
          break;
        default:
          sprintf(str, "Unsupported or Reserved TRB type %i found!", type);
          MessageBox(hwnd, str, NULL, MB_ICONINFORMATION);
      }

      if (ret == 1) {
        // make sure the type is still correct
        g_trb.command &= ~(0x3F << 10);
        g_trb.command |= TRB_SET_TYPE(type);
        // write back the trb
        DEV_MEM_WRITE_PHYSICAL(tvitem.lParam, sizeof(struct TRB), (Bit8u *) &g_trb);
      }
    } else
      MessageBox(hwnd, "Item selected has no TRB attached.", NULL, MB_ICONINFORMATION);

  } else
    MessageBox(hwnd, "No TRB selected", NULL, MB_ICONINFORMATION);
}


// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_normal(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);
      
      sprintf(str, "%i", TRB_GET_TARGET(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str);
      sprintf(str, "%i", TRB_GET_TX_LEN(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_TD_SIZE, str);
      sprintf(str, "%i", TRB_GET_TX_LEN(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_TRANS_LEN, str);

      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_BEI, TRB_DC(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_IDT, TRB_IS_IMMED_DATA(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_IOC, TRB_IOC(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_CH,  TRB_CHAIN(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_NS,  (g_trb.command & 8) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_ISP, TRB_SPD(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_ENT, TRB_TOGGLE(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_C,   (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0);

              GetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str, COMMON_STR_SIZE);
              g_trb.status = (strtol(str, NULL, 0) & 0x3FF) << 22;
              GetDlgItemText(hDlg, IDC_TRB_TD_SIZE, str, COMMON_STR_SIZE);
              g_trb.status |= TRB_SET_TDSIZE(strtol(str, NULL, 0));
              GetDlgItemText(hDlg, IDC_TRB_TRANS_LEN, str, COMMON_STR_SIZE);
              g_trb.status |= strtol(str, NULL, 0) & 0x1FFFF;
              
              g_trb.command  = (IsDlgButtonChecked(hDlg, IDC_TRB_BEI) == BST_CHECKED) ? (1<<9) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_IDT) == BST_CHECKED) ? (1<<6) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_IOC) == BST_CHECKED) ? (1<<5) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_CH) == BST_CHECKED) ? (1<<4) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_NS) == BST_CHECKED) ? (1<<3) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_ISP) == BST_CHECKED) ? (1<<2) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_ENT) == BST_CHECKED) ? (1<<1) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_setup(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x%04X", (Bit16u) ((g_trb.parameter & 0x00000000000000FFULL) >> 0));
      SetDlgItemText(hDlg, IDC_TRB_BREQUESTTYPE, str);
      sprintf(str, "0x%04X", (Bit16u) ((g_trb.parameter & 0x000000000000FF00ULL) >> 8));
      SetDlgItemText(hDlg, IDC_TRB_BREQUEST, str);
      sprintf(str, "0x%04X", (Bit16u) ((g_trb.parameter & 0x00000000FFFF0000ULL) >> 16));
      SetDlgItemText(hDlg, IDC_TRB_WVALUE, str);
      sprintf(str, "0x%04X", (Bit16u) ((g_trb.parameter & 0x0000FFFF00000000ULL) >> 32));
      SetDlgItemText(hDlg, IDC_TRB_WINDEX, str);
      sprintf(str, "0x%04X", (Bit16u) ((g_trb.parameter & 0xFFFF000000000000ULL) >> 48));
      SetDlgItemText(hDlg, IDC_TRB_WLENGTH, str);
      
      sprintf(str, "%i", TRB_GET_TARGET(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str);
      sprintf(str, "%i", TRB_GET_TX_LEN(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_TRANS_LEN, str);

      sprintf(str, "%i", (g_trb.command & 0x00030000) >> 16);
      SetDlgItemText(hDlg, IDC_TRB_TRT, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_IDT, TRB_IS_IMMED_DATA(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_IOC, TRB_IOC(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_C,   (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_BREQUESTTYPE, str, COMMON_STR_SIZE);
              g_trb.parameter  = (((Bit64u) strtol(str, NULL, 0) << 0) & 0x00000000000000FFULL);
              GetDlgItemText(hDlg, IDC_TRB_BREQUEST, str, COMMON_STR_SIZE);
              g_trb.parameter |= (((Bit64u) strtol(str, NULL, 0) << 8) & 0x000000000000FF00ULL);
              GetDlgItemText(hDlg, IDC_TRB_WVALUE, str, COMMON_STR_SIZE);
              g_trb.parameter |= (((Bit64u) strtol(str, NULL, 0) << 16) & 0x00000000FFFF0000ULL);
              GetDlgItemText(hDlg, IDC_TRB_WINDEX, str, COMMON_STR_SIZE);
              g_trb.parameter |= (((Bit64u) strtol(str, NULL, 0) << 32) & 0x0000FFFF00000000ULL);
              GetDlgItemText(hDlg, IDC_TRB_WLENGTH, str, COMMON_STR_SIZE);
              g_trb.parameter |= (((Bit64u) strtol(str, NULL, 0) << 48) & 0xFFFF000000000000ULL);

              GetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str, COMMON_STR_SIZE);
              g_trb.status  = (strtol(str, NULL, 0) & 0x3FF) << 22;
              GetDlgItemText(hDlg, IDC_TRB_TRANS_LEN, str, COMMON_STR_SIZE);
              g_trb.status |= strtol(str, NULL, 0) & 0x1FFFF;
              
              GetDlgItemText(hDlg, IDC_TRB_TRT, str, COMMON_STR_SIZE);
              g_trb.command  = (strtol(str, NULL, 0) << 16) & 0x00030000;

              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_IDT) == BST_CHECKED) ? (1<<6) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_IOC) == BST_CHECKED) ? (1<<5) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_data(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);
      
      sprintf(str, "%i", TRB_GET_TARGET(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str);
      sprintf(str, "%i", TRB_GET_TX_LEN(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_TD_SIZE, str);
      sprintf(str, "%i", TRB_GET_TX_LEN(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_TRANS_LEN, str);

      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_DIR, (g_trb.command & 0x10000) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_IDT, TRB_IS_IMMED_DATA(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_IOC, TRB_IOC(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_CH,  TRB_CHAIN(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_NS,  (g_trb.command & 8) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_ISP, TRB_SPD(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_ENT, TRB_TOGGLE(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_C,   (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0);

              GetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str, COMMON_STR_SIZE);
              g_trb.status = (strtol(str, NULL, 0) & 0x3FF) << 22;
              GetDlgItemText(hDlg, IDC_TRB_TD_SIZE, str, COMMON_STR_SIZE);
              g_trb.status |= TRB_SET_TDSIZE(strtol(str, NULL, 0));
              GetDlgItemText(hDlg, IDC_TRB_TRANS_LEN, str, COMMON_STR_SIZE);
              g_trb.status |= strtol(str, NULL, 0) & 0x1FFFF;
              
              g_trb.command  = (IsDlgButtonChecked(hDlg, IDC_TRB_DIR) == BST_CHECKED) ? (1<<16) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_IDT) == BST_CHECKED) ? (1<<6) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_IOC) == BST_CHECKED) ? (1<<5) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_CH) == BST_CHECKED) ? (1<<4) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_NS) == BST_CHECKED) ? (1<<3) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_ISP) == BST_CHECKED) ? (1<<2) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_ENT) == BST_CHECKED) ? (1<<1) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_status(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "%i", TRB_GET_TARGET(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str);

      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_DIR, (g_trb.command & 0x10000) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_IOC, TRB_IOC(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_CH,  TRB_CHAIN(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_ENT, TRB_TOGGLE(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_C,   (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              g_trb.parameter = 0;

              GetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str, COMMON_STR_SIZE);
              g_trb.status = (strtol(str, NULL, 0) & 0x3FF) << 22;
              
              g_trb.command  = (IsDlgButtonChecked(hDlg, IDC_TRB_DIR) == BST_CHECKED) ? (1<<16) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_IOC) == BST_CHECKED) ? (1<<5) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_CH) == BST_CHECKED) ? (1<<4) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_ENT) == BST_CHECKED) ? (1<<1) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_link(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter & ~0x0FULL);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);
      
      sprintf(str, "%i", TRB_GET_TARGET(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str);

      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_IOC, TRB_IOC(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_CH,  TRB_CHAIN(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_TC,  TRB_GET_TOGGLE(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_C,   (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0) & ~0x0FULL;

              GetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str, COMMON_STR_SIZE);
              g_trb.status = (strtol(str, NULL, 0) & 0x3FF) << 22;
              
              g_trb.command  = (IsDlgButtonChecked(hDlg, IDC_TRB_IOC) == BST_CHECKED) ? (1<<5) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_CH) == BST_CHECKED) ? (1<<4) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_TC) == BST_CHECKED) ? (1<<1) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_event(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);
      
      sprintf(str, "%i", TRB_GET_TARGET(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str);

      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_BEI, TRB_DC(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_IOC, TRB_IOC(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_CH,  TRB_CHAIN(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_ENT, TRB_TOGGLE(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_C,   (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0);

              GetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str, COMMON_STR_SIZE);
              g_trb.status = (strtol(str, NULL, 0) & 0x3FF) << 22;
              
              g_trb.command  = (IsDlgButtonChecked(hDlg, IDC_TRB_BEI) == BST_CHECKED) ? (1<<9) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_IOC) == BST_CHECKED) ? (1<<5) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_CH) == BST_CHECKED) ? (1<<4) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_ENT) == BST_CHECKED) ? (1<<1) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_noop(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "%i", TRB_GET_TARGET(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str);

      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_IOC, TRB_IOC(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_CH,  TRB_CHAIN(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_ENT, TRB_TOGGLE(g_trb.command) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_C,   (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              g_trb.parameter = 0;

              GetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str, COMMON_STR_SIZE);
              g_trb.status = (strtol(str, NULL, 0) & 0x3FF) << 22;
              
              g_trb.command  = (IsDlgButtonChecked(hDlg, IDC_TRB_IOC) == BST_CHECKED) ? (1<<5) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_CH) == BST_CHECKED) ? (1<<4) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_ENT) == BST_CHECKED) ? (1<<1) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_enslot(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "%i", TRB_GET_STYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_TYPE, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,   (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              g_trb.parameter = 0;
              g_trb.status = 0;
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_TYPE, str, COMMON_STR_SIZE);
              g_trb.command = TRB_SET_STYPE(strtol(str, NULL, 0));
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_disslot(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,   (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      // if this is the Negotiate Bandwidth Command, we need to change the title
      if (TRB_GET_TYPE(g_trb.command) == DEG_BANDWIDTH)
        SetWindowText(hDlg, "Negotiate Bandwidth TRB");

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              g_trb.parameter = 0;
              g_trb.status = 0;
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command = TRB_SET_SLOT(strtol(str, NULL, 0));
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_address(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  Bit64u address;

  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter & ~0x0FULL);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);

      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_BSR, (g_trb.command & (1<<9)) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_C,   (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDC_IN_CONTEXT:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              address = strtol(str, NULL, 0) & ~0x0FULL;
              DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_CONTEXT), hDlg, hc_xhci_callback_context, (LPARAM) address);
              break;
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0) & ~0x0FULL;

              g_trb.status = 0;
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command = TRB_SET_SLOT(strtol(str, NULL, 0));
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_BSR) == BST_CHECKED) ? (1<<9) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_configep(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  Bit64u address;
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter & ~0x0FULL);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);

      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_DECONFIG, (g_trb.command & (1<<9)) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_C,   (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDC_IN_CONTEXT:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              address = strtol(str, NULL, 0) & ~0x0FULL;
              DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_CONTEXT), hDlg, hc_xhci_callback_context, (LPARAM) address);
              break;
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0) & ~0x0FULL;

              g_trb.status = 0;
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command = TRB_SET_SLOT(strtol(str, NULL, 0));
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_DECONFIG) == BST_CHECKED) ? (1<<9) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_evaluate(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  Bit64u address;
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter & ~0x0FULL);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);

      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_BSR, (g_trb.command & (1<<9)) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_C,   (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDC_IN_CONTEXT:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              address = strtol(str, NULL, 0) & ~0x0FULL;
              DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_CONTEXT), hDlg, hc_xhci_callback_context, (LPARAM) address);
              break;
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0) & ~0x0FULL;

              g_trb.status = 0;
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command = TRB_SET_SLOT(strtol(str, NULL, 0));
              //g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_BSR) == BST_CHECKED) ? (1<<9) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_resetep(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", TRB_GET_EP(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_EP_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_TSP, (g_trb.command & (1<<9)) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_C,   (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              g_trb.parameter = 0;
              g_trb.status = 0;
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command  = TRB_SET_SLOT(strtol(str, NULL, 0));
              GetDlgItemText(hDlg, IDC_TRB_EP_ID, str, COMMON_STR_SIZE);
              g_trb.command |= TRB_SET_EP(strtol(str, NULL, 0));
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_TSP) == BST_CHECKED) ? (1<<9) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_stopep(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", TRB_GET_EP(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_EP_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_SP, (g_trb.command & (1<<23)) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              g_trb.parameter = 0;
              g_trb.status = 0;
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command  = TRB_SET_SLOT(strtol(str, NULL, 0));
              GetDlgItemText(hDlg, IDC_TRB_EP_ID, str, COMMON_STR_SIZE);
              g_trb.command |= TRB_SET_EP(strtol(str, NULL, 0));
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_SP) == BST_CHECKED) ? (1<<23) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_settrptr(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter & ~0x0FULL);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);
      
      sprintf(str, "%i", TRB_GET_STREAM(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_STREAMID, str);

      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", TRB_GET_EP(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_EP_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0) & ~0x0FULL;

              GetDlgItemText(hDlg, IDC_TRB_STREAMID, str, COMMON_STR_SIZE);
              g_trb.status = (strtol(str, NULL, 0) & 0xFFFF) << 16;
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command  = TRB_SET_SLOT(strtol(str, NULL, 0));
              GetDlgItemText(hDlg, IDC_TRB_EP_ID, str, COMMON_STR_SIZE);
              g_trb.command |= TRB_SET_EP(strtol(str, NULL, 0));
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_resetdev(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      // if this is the NoOp Command, we need to change the title
      if (TRB_GET_TYPE(g_trb.command) == NO_OP_CMD)
        SetWindowText(hDlg, "No Op Command TRB");

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              g_trb.parameter = 0;
              g_trb.status = 0;
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command  = TRB_SET_SLOT(strtol(str, NULL, 0));
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_forceevent(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter & ~0x0FULL);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);
      
      sprintf(str, "%i", TRB_GET_TARGET(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str);
      
      sprintf(str, "%i", (g_trb.command & (0xFF << 16)) >> 16);
      SetDlgItemText(hDlg, IDC_TRB_VF_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0) & ~0x0FULL;

              GetDlgItemText(hDlg, IDC_TRB_INT_TARGET, str, COMMON_STR_SIZE);
              g_trb.status = (strtol(str, NULL, 0) & 0x3FF) << 22;
              
              GetDlgItemText(hDlg, IDC_TRB_VF_ID, str, COMMON_STR_SIZE);
              g_trb.command  = (strtol(str, NULL, 0) & 0xFF) << 16;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_setlat(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "%i", (g_trb.command & (0xFFF << 16)) >> 16);
      SetDlgItemText(hDlg, IDC_TRB_BELT, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              g_trb.parameter = 0;
              g_trb.status = 0;
              
              GetDlgItemText(hDlg, IDC_TRB_BELT, str, COMMON_STR_SIZE);
              g_trb.command  = (strtol(str, NULL, 0) & 0xFFF) << 16;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_getband(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter & ~0x0FULL);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);

      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", (g_trb.command & (0x0F << 16)) >> 16);
      SetDlgItemText(hDlg, IDC_TRB_DEV_SPEED, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0) & ~0x0FULL;
              
              g_trb.status = 0;
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command  = TRB_SET_SLOT(strtol(str, NULL, 0));
              GetDlgItemText(hDlg, IDC_TRB_DEV_SPEED, str, COMMON_STR_SIZE);
              g_trb.command |= (strtol(str, NULL, 0) & 0x0F) << 16;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_forcehdr(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter & ~0x1FULL);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);

      sprintf(str, "0x%08X", g_trb.status);
      SetDlgItemText(hDlg, IDC_TRB_HDR_HI, str);

      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0) & ~0x1FULL;
              
              GetDlgItemText(hDlg, IDC_TRB_HDR_HI, str, COMMON_STR_SIZE);
              g_trb.status = strtol(str, NULL, 0);
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command  = TRB_SET_SLOT(strtol(str, NULL, 0));
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_transevent(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);

      sprintf(str, "%i", TRB_GET_COMP_CODE(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str);

      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", TRB_GET_EP(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_EP_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_ED, (g_trb.command & 4) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0);
              
              GetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str, COMMON_STR_SIZE);
              g_trb.status = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command  = TRB_SET_SLOT(strtol(str, NULL, 0));
              GetDlgItemText(hDlg, IDC_TRB_EP_ID, str, COMMON_STR_SIZE);
              g_trb.command |= TRB_SET_EP(strtol(str, NULL, 0));
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_ED) == BST_CHECKED) ? (1<<2) : 0;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_compcompletion(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);

      sprintf(str, "%i", TRB_GET_COMP_CODE(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str);
      sprintf(str, "%i", g_trb.status & 0x00FFFFFF);
      SetDlgItemText(hDlg, IDC_TRB_COMP_LPARAM, str);

      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", (g_trb.command & (0xFF << 16)) >> 16);
      SetDlgItemText(hDlg, IDC_TRB_VF_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0);
              
              GetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str, COMMON_STR_SIZE);
              g_trb.status  = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
              GetDlgItemText(hDlg, IDC_TRB_COMP_LPARAM, str, COMMON_STR_SIZE);
              g_trb.status |= strtol(str, NULL, 0) & 0x00FFFFFF;
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command  = TRB_SET_SLOT(strtol(str, NULL, 0));
              GetDlgItemText(hDlg, IDC_TRB_VF_ID, str, COMMON_STR_SIZE);
              g_trb.command |= (strtol(str, NULL, 0) & 0xFF) << 16;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_pschange(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, (g_trb.parameter & 0x00000000FF000000ULL) >> 24);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);

      sprintf(str, "%i", TRB_GET_COMP_CODE(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str);

      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = (Bit64u) (strtol(str, NULL, 0) & 0xFF) << 24;
              
              GetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str, COMMON_STR_SIZE);
              g_trb.status  = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
              
              g_trb.command = (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_bandrequ(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "%i", TRB_GET_COMP_CODE(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str);

      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              g_trb.parameter = 0;
              
              GetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str, COMMON_STR_SIZE);
              g_trb.status  = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command  = TRB_SET_SLOT(strtol(str, NULL, 0));
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_doorbell(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter & 0x1F);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);
      
      sprintf(str, "%i", TRB_GET_COMP_CODE(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str);

      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", (g_trb.command & (0xFF << 16)) >> 16);
      SetDlgItemText(hDlg, IDC_TRB_VF_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0) & 0x1F;
              
              GetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str, COMMON_STR_SIZE);
              g_trb.status  = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command  = TRB_SET_SLOT(strtol(str, NULL, 0));
              GetDlgItemText(hDlg, IDC_TRB_VF_ID, str, COMMON_STR_SIZE);
              g_trb.command |= (strtol(str, NULL, 0) & 0xFF) << 16;
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_hostevent(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "%i", TRB_GET_COMP_CODE(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str);

      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      // if this is the MFINDEX Wrap Event, we need to change the title
      if (TRB_GET_TYPE(g_trb.command) == MFINDEX_WRAP)
        SetWindowText(hDlg, "MFINDEX Wrap Event TRB");

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              g_trb.parameter = 0;
              
              GetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str, COMMON_STR_SIZE);
              g_trb.status  = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
              
              g_trb.command  = (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_devnotevent(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter >> 8);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);
      sprintf(str, "%i", (Bit8u) (g_trb.parameter & 0xF0) >> 4);
      SetDlgItemText(hDlg, IDC_TRB_NOT_TYPE, str);
      
      sprintf(str, "%i", TRB_GET_COMP_CODE(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str);

      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter  = (Bit64u) strtol(str, NULL, 0) << 8;
              GetDlgItemText(hDlg, IDC_TRB_NOT_TYPE, str, COMMON_STR_SIZE);
              g_trb.parameter |= (strtol(str, NULL, 0) & 0xF) << 4;
              
              GetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str, COMMON_STR_SIZE);
              g_trb.status  = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
              
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command  = TRB_SET_SLOT(strtol(str, NULL, 0));
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}





// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_necfw(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:

              g_trb.parameter = 0;
              g_trb.status = 0;
              
              g_trb.command = (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_necun(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);
      
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0);
              
              g_trb.status = 0;
              
              g_trb.command = (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

// lParam = address to the TRB
static INT_PTR CALLBACK hc_xhci_callback_trb_necfwevent(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  
  switch (msg) {
    case WM_INITDIALOG:
      sprintf(str, "0x" FMT_ADDRX64, g_trb.parameter);
      SetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str);

      sprintf(str, "%i", TRB_GET_COMP_CODE(g_trb.status));
      SetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str);
      sprintf(str, "0x%04X", g_trb.status & 0x0000FFFF);
      SetDlgItemText(hDlg, IDC_TRB_COMP_LPARAM, str);

      sprintf(str, "0x%04X", (g_trb.command & 0xFFFF0000) >> 16);
      SetDlgItemText(hDlg, IDC_TRB_COMP_HPARAM, str);
      sprintf(str, "%i", TRB_GET_SLOT(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str);
      sprintf(str, "%i", TRB_GET_TYPE(g_trb.command));
      SetDlgItemText(hDlg, IDC_TRB_TYPE, str);
      
      CheckDlgButton(hDlg, IDC_TRB_C,  (g_trb.command & 1) ? BST_CHECKED : BST_UNCHECKED);

      SetFocus(GetDlgItem(hDlg, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDOK:
              GetDlgItemText(hDlg, IDC_TRB_DATA_PTR, str, COMMON_STR_SIZE);
              g_trb.parameter = strtol(str, NULL, 0);
              
              GetDlgItemText(hDlg, IDC_TRB_COMP_CODE, str, COMMON_STR_SIZE);
              g_trb.status  = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
              GetDlgItemText(hDlg, IDC_TRB_COMP_LPARAM, str, COMMON_STR_SIZE);
              g_trb.status |= strtol(str, NULL, 0) & 0x0000FFFF;
              
              GetDlgItemText(hDlg, IDC_TRB_COMP_HPARAM, str, COMMON_STR_SIZE);
              g_trb.command  = (strtol(str, NULL, 0) & 0x0000FFFF) << 16;
              GetDlgItemText(hDlg, IDC_TRB_SLOT_ID, str, COMMON_STR_SIZE);
              g_trb.command |= TRB_SET_SLOT(strtol(str, NULL, 0));
              g_trb.command |= (IsDlgButtonChecked(hDlg, IDC_TRB_C) == BST_CHECKED) ? (1<<0) : 0;
              
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
          break;
      }
  }
  
  return 0;
}

static Bit64u xhci_context_address = 0;
static Bit8u  *xhci_context = NULL;
static int    xhci_current_ep_context = 1;  // 0 through 30 (slot, control_ep, ep1_out, ep1_in, ep2_out, ep2_in, etc)
static bool   xhci_context_changed = 0;

static Bit64u xhci_str_context_address = 0;
static Bit8u *xhci_str_context = NULL;
static int    xhci_str_current_context = 0;
static bool   xhci_str_context_changed = 0;
static int    xhci_max_streams = 0;

static char *slot_speed_str[] = {
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

static char *slot_type_str[] = {
  "Disabled/Enabled",
  "Default",
  "Addressed",
  "Configured",
  "Reserved"
};

static char *ep_type_str[] = {
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

static char *ep_state_str[] = {
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

static char *string_sct_str[] = {
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

// xHCI 1.2: 6.2.5.1, Page 461
static void hc_xhci_callback_context_ep_get(HWND hDlg)
{
  char str[COMMON_STR_SIZE];
  Bit32u *p;
  int i;
  
  // display the EP (with Drop and Add bits)
  p = (Bit32u *) &xhci_context[0];
  if (xhci_current_ep_context == 1) {
    strcpy(str, "Control EP ");
  } else {
    sprintf(str, "EP%i %s ", (xhci_current_ep_context >> 1), (xhci_current_ep_context & 1) ? "IN" : "OUT");
  }
  if (p[0] & (1 << xhci_current_ep_context))
    strcat(str, "(D)");
  if (p[1] & (1 << xhci_current_ep_context))
    strcat(str, "(A)");
  SetDlgItemText(hDlg, IDC_CONTEXT_OF_STR, str);
  
  // EP Context
  p = (Bit32u *) &xhci_context[CONTEXT_SIZE + (xhci_current_ep_context * CONTEXT_SIZE)];
  sprintf(str, "%i", (p[0] & (0xFF << 24)) >> 24);
  SetDlgItemText(hDlg, IDC_CONTEXT_MAX_ESIT_HI, str);
  sprintf(str, "%i", (p[0] & (0xFF << 16)) >> 16);
  SetDlgItemText(hDlg, IDC_CONTEXT_INTERVAL, str);
  CheckDlgButton(hDlg, IDC_CONTEXT_LSA, (p[0] & (1<<15)) ? BST_CHECKED : BST_UNCHECKED);
  sprintf(str, "%i", (p[0] & (0x1F << 10)) >> 10);
  SetDlgItemText(hDlg, IDC_CONTEXT_MAX_PSTREAMS, str);
  sprintf(str, "%i", (p[0] & (0x3 << 8)) >> 8);
  SetDlgItemText(hDlg, IDC_CONTEXT_MULT, str);
  sprintf(str, "%i", (p[0] & (0x7 << 0)) >> 0);
  SetDlgItemText(hDlg, IDC_CONTEXT_EP_STATE, str);
  sprintf(str, "%i", (p[1] & (0xFFFF << 16)) >> 16);
  SetDlgItemText(hDlg, IDC_CONTEXT_MAX_PACKET_SIZE, str);
  sprintf(str, "%i", (p[1] & (0xFF << 8)) >> 8);
  SetDlgItemText(hDlg, IDC_CONTEXT_MAX_BURST_SIZE, str);
  CheckDlgButton(hDlg, IDC_CONTEXT_HID, (p[1] & (1<<7)) ? BST_CHECKED : BST_UNCHECKED);
  sprintf(str, "%i", (p[1] & (0x7 << 3)) >> 3);
  SetDlgItemText(hDlg, IDC_CONTEXT_EP_TYPE, str);
  sprintf(str, "%i", (p[1] & (0x3 << 1)) >> 1);
  SetDlgItemText(hDlg, IDC_CONTEXT_CERR, str);
  sprintf(str, "0x" FMT_ADDRX64, ((Bit64u) p[3] << 32) | (p[2] & ~0xFULL));
  SetDlgItemText(hDlg, IDC_CONTEXT_TR_DEQUEUE_PTR, str);
  CheckDlgButton(hDlg, IDC_CONTEXT_DCS, (p[2] & (1<<0)) ? BST_CHECKED : BST_UNCHECKED);
  sprintf(str, "%i", (p[4] & (0xFFFF << 16)) >> 16);
  SetDlgItemText(hDlg, IDC_CONTEXT_MAX_ESIT_LO, str);
  sprintf(str, "%i", (p[4] & (0xFFFF << 0)) >> 0);
  SetDlgItemText(hDlg, IDC_CONTEXT_AVERAGE_LEN, str);
  
  EnableWindow(GetDlgItem(hDlg, IDC_CONTEXT_STREAM_CONTEXT), (((p[0] & (0x1F << 10)) >> 10) > 0));
  
  for (i=0; i<3; i++) {
    sprintf(str, "0x%08X", p[5+i]);
    SetDlgItemText(hDlg, IDC_CONTEXT_RSVDO_EP_0 + i, str);
  }
#if (CONTEXT_SIZE == 64)
  for (i=3; i<11; i++) {
    sprintf(str, "0x%08X", p[5+i]);
    SetDlgItemText(hDlg, IDC_CONTEXT_RSVDO_EP_0 + i, str);
    EnableWindow(GetDlgItem(hDlg, IDC_CONTEXT_RSVDO_EP_0 + i), 1);
  }
#endif
  
  xhci_context_changed = 0;
  EnableWindow(GetDlgItem(hDlg, ID_APPLY), 0);
}

static void hc_xhci_callback_context_ep_put(HWND hDlg)
{
  char str[COMMON_STR_SIZE];
  Bit32u *p;
  int i;
  
  // EP Context
  p = (Bit32u *) &xhci_context[CONTEXT_SIZE + (xhci_current_ep_context * CONTEXT_SIZE)];
  GetDlgItemText(hDlg, IDC_CONTEXT_MAX_ESIT_HI, str, COMMON_STR_SIZE);
  p[0]  = (strtol(str, NULL, 0) & 0xFF) << 24;
  GetDlgItemText(hDlg, IDC_CONTEXT_INTERVAL, str, COMMON_STR_SIZE);
  p[0] |= (strtol(str, NULL, 0) & 0xFF) << 16;
  p[0] |= (IsDlgButtonChecked(hDlg, IDC_CONTEXT_LSA) == BST_CHECKED) ? (1<<15) : 0;
  GetDlgItemText(hDlg, IDC_CONTEXT_MAX_PSTREAMS, str, COMMON_STR_SIZE);
  p[0] |= (strtol(str, NULL, 0) & 0x1F) << 10;
  GetDlgItemText(hDlg, IDC_CONTEXT_MULT, str, COMMON_STR_SIZE);
  p[0] |= (strtol(str, NULL, 0) & 0x3) << 8;
  GetDlgItemText(hDlg, IDC_CONTEXT_EP_STATE, str, COMMON_STR_SIZE);
  p[0] |= (strtol(str, NULL, 0) & 0x7) << 0;
  GetDlgItemText(hDlg, IDC_CONTEXT_MAX_PACKET_SIZE, str, COMMON_STR_SIZE);
  p[1]  = (strtol(str, NULL, 0) & 0xFFFF) << 16;
  GetDlgItemText(hDlg, IDC_CONTEXT_MAX_BURST_SIZE, str, COMMON_STR_SIZE);
  p[1] |= (strtol(str, NULL, 0) & 0xFF) << 8;
  p[1] |= (IsDlgButtonChecked(hDlg, IDC_CONTEXT_HID) == BST_CHECKED) ? (1<<7) : 0;
  GetDlgItemText(hDlg, IDC_CONTEXT_EP_TYPE, str, COMMON_STR_SIZE);
  p[1] |= (strtol(str, NULL, 0) & 0x7) << 3;
  GetDlgItemText(hDlg, IDC_CONTEXT_CERR, str, COMMON_STR_SIZE);
  p[1] |= (strtol(str, NULL, 0) & 0x3) << 1;
  GetDlgItemText(hDlg, IDC_CONTEXT_TR_DEQUEUE_PTR, str, COMMON_STR_SIZE);
  p[2]  = strtol(str, NULL, 0) & ~0xFULL;
  p[3]  = (Bit64u) strtol(str, NULL, 0) >> 32;
  p[2] |= (IsDlgButtonChecked(hDlg, IDC_CONTEXT_DCS) == BST_CHECKED) ? (1<<0) : 0;
  GetDlgItemText(hDlg, IDC_CONTEXT_MAX_ESIT_LO, str, COMMON_STR_SIZE);
  p[4]  = (strtol(str, NULL, 0) & 0xFFFF) << 16;
  GetDlgItemText(hDlg, IDC_CONTEXT_AVERAGE_LEN, str, COMMON_STR_SIZE);
  p[4] |= (strtol(str, NULL, 0) & 0xFFFF) << 0;

  for (i=0; i<3; i++) {
    GetDlgItemText(hDlg, IDC_CONTEXT_RSVDO_EP_0 + i, str, COMMON_STR_SIZE);
    p[5+i] = strtol(str, NULL, 0);
  }
#if (CONTEXT_SIZE == 64)
  for (i=3; i<11; i++) {
    GetDlgItemText(hDlg, IDC_CONTEXT_RSVDO_EP_0 + i, str, COMMON_STR_SIZE);
    p[5+i] = strtol(str, NULL, 0);
  }
#endif

  xhci_context_changed = 0;
  EnableWindow(GetDlgItem(hDlg, ID_APPLY), 0);
}

static INT_PTR CALLBACK hc_xhci_callback_context(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  struct S_ATTRIBUTES *attribs;
  Bit32u *p;
  int i, j;

  switch (msg) {
    case WM_INITDIALOG:
      xhci_context_address = lParam;
      xhci_current_ep_context = 1;
      xhci_context = new Bit8u[CONTEXT_SIZE + (32 * CONTEXT_SIZE)];
      DEV_MEM_READ_PHYSICAL(xhci_context_address, CONTEXT_SIZE + (32 * CONTEXT_SIZE), xhci_context);
      p = (Bit32u *) &xhci_context[0];

      // Context structure
      sprintf(str, "0x%08X", p[0]);
      SetDlgItemText(hDlg, IDC_CONTEXT_DROP, str);
      sprintf(str, "0x%08X", p[1]);
      SetDlgItemText(hDlg, IDC_CONTEXT_ADD, str);

      sprintf(str, "%i", (p[7] & 0x00FF0000) >> 16);
      SetDlgItemText(hDlg, IDC_CONTEXT_ALT_SETTING, str);
      sprintf(str, "%i", (p[7] & 0x0000FF00) >> 8);
      SetDlgItemText(hDlg, IDC_CONTEXT_INTFACE_NUM, str);
      sprintf(str, "%i", (p[7] & 0x000000FF) >> 0);
      SetDlgItemText(hDlg, IDC_CONTEXT_CONFIG_VALUE, str);

      // Slot Context
      p = (Bit32u *) &xhci_context[CONTEXT_SIZE];
      sprintf(str, "%i", (p[0] & (0x1F << 27)) >> 27);
      SetDlgItemText(hDlg, IDC_CONTEXT_ENTRIES, str);
      CheckDlgButton(hDlg, IDC_CONTEXT_HUB, (p[0] & (1<<26)) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_CONTEXT_MTT, (p[0] & (1<<25)) ? BST_CHECKED : BST_UNCHECKED);
      sprintf(str, "%i", (p[0] & (0xF << 20)) >> 20);
      SetDlgItemText(hDlg, IDC_CONTEXT_SPEED, str);
      sprintf(str, "0x%05X", (p[0] & (0xFFFFF << 0)) >> 0);
      SetDlgItemText(hDlg, IDC_CONTEXT_ROUTE_STRING, str);
      sprintf(str, "%i", (p[1] & (0xFF << 24)) >> 24);
      SetDlgItemText(hDlg, IDC_CONTEXT_NUM_PORTS, str);
      sprintf(str, "%i", (p[1] & (0xFF << 16)) >> 16);
      SetDlgItemText(hDlg, IDC_CONTEXT_RH_PORT_NUM, str);
      sprintf(str, "%i", (p[1] & (0xFFFF << 0)) >> 0);
      SetDlgItemText(hDlg, IDC_CONTEXT_MAX_EXIT_LAT, str);
      sprintf(str, "%i", (p[2] & (0x3FF << 22)) >> 22);
      SetDlgItemText(hDlg, IDC_CONTEXT_INT_TARGET, str);
      sprintf(str, "%i", (p[2] & (0x3 << 16)) >> 16);
      SetDlgItemText(hDlg, IDC_CONTEXT_TTT, str);
      sprintf(str, "%i", (p[2] & (0xFF << 8)) >> 8);
      SetDlgItemText(hDlg, IDC_CONTEXT_TT_PORT_NUM, str);
      sprintf(str, "%i", (p[2] & (0xFF << 0)) >> 0);
      SetDlgItemText(hDlg, IDC_CONTEXT_TT_HUB_SLOT_ID, str);
      sprintf(str, "%i", (p[3] & (0x1F << 27)) >> 27);
      SetDlgItemText(hDlg, IDC_CONTEXT_SLOT_STATE, str);
      sprintf(str, "%i", (p[3] & (0xFF << 0)) >> 0);
      SetDlgItemText(hDlg, IDC_CONTEXT_DEV_ADDRESS, str);
      for (i=0; i<4; i++) {
        sprintf(str, "0x%08X", p[4+i]);
        SetDlgItemText(hDlg, IDC_CONTEXT_RSVDO_SLOT_0 + i, str);
      }
#if (CONTEXT_SIZE == 64)
      for (i=4; i<12; i++) {
        sprintf(str, "0x%08X", p[4+i]);
        SetDlgItemText(hDlg, IDC_CONTEXT_RSVDO_SLOT_0 + i, str);
        EnableWindow(GetDlgItem(hDlg, IDC_CONTEXT_RSVDO_SLOT_0 + i), 1);
      }
#endif
      
      // Endpoint Context
      hc_xhci_callback_context_ep_get(hDlg);

      xhci_context_changed = 0;
      EnableWindow(GetDlgItem(hDlg, ID_APPLY), 0);
      SetFocus(GetDlgItem(hDlg, IDC_CONTEXT_DROP));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDC_CONTEXT_DROP_B:
              // create an attributes structure
              attribs = (struct S_ATTRIBUTES *) calloc(sizeof(struct S_ATTRIBUTES), 33);
              for (i=0,j=31; i<32; i++,j--) {
                attribs[i].attrb = (1ULL<<i);
                attribs[i].mask = (1ULL<<i);
                attribs[i].index = j;
                if (i<2)
                  sprintf(attribs[i].str, "D%i (RsvdZ)", i);
                else
                  sprintf(attribs[i].str, "D%i", i);
                attribs[i].groups[0] = -1;
              }
              attribs[32].attrb = 0;
              attribs[32].mask = -1;
              attribs[32].index = -1;
              strcpy(attribs[32].str, "");
              attribs[32].groups[0] = -1;
              do_attributes(hDlg, IDC_CONTEXT_DROP, 8, "Drop Flags", attribs);
              free(attribs);
              break;
            case IDC_CONTEXT_ADD_B:
              // create an attributes structure
              attribs = (struct S_ATTRIBUTES *) calloc(sizeof(struct S_ATTRIBUTES), 33);
              for (i=0,j=31; i<32; i++,j--) {
                attribs[i].attrb = (1ULL<<i);
                attribs[i].mask = (1ULL<<i);
                attribs[i].index = j;
                sprintf(attribs[i].str, "A%i", i);
                attribs[i].groups[0] = -1;
              }
              attribs[32].attrb = 0;
              attribs[32].mask = -1;
              attribs[32].index = -1;
              strcpy(attribs[32].str, "");
              attribs[32].groups[0] = -1;
              do_attributes(hDlg, IDC_CONTEXT_ADD, 8, "Add Flags", attribs);
              free(attribs);
              break;
            case ID_APPLY:
              hc_xhci_callback_context_ep_put(hDlg);
              break;
            case IDC_CONTEXT_PREV:
              if (xhci_current_ep_context > 1) {
                if (xhci_context_changed) {
                  int ret = MessageBox(hDlg, "EP has changed. Save?", NULL, MB_ICONQUESTION | MB_YESNOCANCEL);
                  if (ret == IDYES)
                    hc_xhci_callback_context_ep_put(hDlg);
                  else if (ret == IDCANCEL)
                    break;
                }
                xhci_current_ep_context--;
                hc_xhci_callback_context_ep_get(hDlg);
              }
              break;
            case IDC_CONTEXT_NEXT:
              if (xhci_current_ep_context < 31) {
                if (xhci_context_changed) {
                  int ret = MessageBox(hDlg, "EP has changed. Save?", NULL, MB_ICONQUESTION | MB_YESNOCANCEL);
                  if (ret == IDYES)
                    hc_xhci_callback_context_ep_put(hDlg);
                  else if (ret == IDCANCEL)
                    break;
                }
                xhci_current_ep_context++;
                hc_xhci_callback_context_ep_get(hDlg);
              }
              break;
            case IDC_CONTEXT_STREAM_CONTEXT:
              GetDlgItemText(hDlg, IDC_CONTEXT_TR_DEQUEUE_PTR, str, COMMON_STR_SIZE);
              xhci_str_context_address = strtol(str, NULL, 0) & ~0x0FULL;
              GetDlgItemText(hDlg, IDC_CONTEXT_MAX_PSTREAMS, str, COMMON_STR_SIZE);
              xhci_max_streams = strtol(str, NULL, 0);
              if (xhci_max_streams > MAX_PSA_SIZE)
                xhci_max_streams = MAX_PSA_SIZE;
              if (xhci_max_streams > 0)
                xhci_max_streams = (1 << (xhci_max_streams + 1));
              DialogBoxParam(NULL, MAKEINTRESOURCE(USB_DEBUG_XHCI_DLG_STR_CONTEXT), hDlg, hc_xhci_callback_str_context, (LPARAM) 0);
              break;
            case IDOK:
              if (xhci_context_changed) {
                int ret = MessageBox(hDlg, "EP has changed. Save?", NULL, MB_ICONQUESTION | MB_YESNOCANCEL);
                if (ret == IDYES)
                  hc_xhci_callback_context_ep_put(hDlg);
                else if (ret == IDCANCEL)
                  break;
              }
              
              // Context structure
              p = (Bit32u *) &xhci_context[0];
              GetDlgItemText(hDlg, IDC_CONTEXT_DROP, str, COMMON_STR_SIZE);
              p[0] = strtol(str, NULL, 0) & ~0x3;
              GetDlgItemText(hDlg, IDC_CONTEXT_ADD, str, COMMON_STR_SIZE);
              p[1] = strtol(str, NULL, 0);

              GetDlgItemText(hDlg, IDC_CONTEXT_ALT_SETTING, str, COMMON_STR_SIZE);
              p[7]  = (strtol(str, NULL, 0) & 0xFF) << 16;
              GetDlgItemText(hDlg, IDC_CONTEXT_INTFACE_NUM, str, COMMON_STR_SIZE);
              p[7] |= (strtol(str, NULL, 0) & 0xFF) << 8;
              GetDlgItemText(hDlg, IDC_CONTEXT_CONFIG_VALUE, str, COMMON_STR_SIZE);
              p[7] |= (strtol(str, NULL, 0) & 0xFF) << 0;

              // Slot Context
              p = (Bit32u *) &xhci_context[CONTEXT_SIZE];
              GetDlgItemText(hDlg, IDC_CONTEXT_ENTRIES, str, COMMON_STR_SIZE);
              p[0]  = (strtol(str, NULL, 0) & 0x1F) << 27;
              p[0] |= (IsDlgButtonChecked(hDlg, IDC_CONTEXT_HUB) == BST_CHECKED) ? (1<<26) : 0;
              p[0] |= (IsDlgButtonChecked(hDlg, IDC_CONTEXT_MTT) == BST_CHECKED) ? (1<<25) : 0;
              GetDlgItemText(hDlg, IDC_CONTEXT_SPEED, str, COMMON_STR_SIZE);
              p[0] |= (strtol(str, NULL, 0) & 0xF) << 20;
              GetDlgItemText(hDlg, IDC_CONTEXT_ROUTE_STRING, str, COMMON_STR_SIZE);
              p[0] |= (strtol(str, NULL, 0) & 0xFFFFF) << 0;
              GetDlgItemText(hDlg, IDC_CONTEXT_NUM_PORTS, str, COMMON_STR_SIZE);
              p[1]  = (strtol(str, NULL, 0) & 0xFF) << 24;
              GetDlgItemText(hDlg, IDC_CONTEXT_RH_PORT_NUM, str, COMMON_STR_SIZE);
              p[1] |= (strtol(str, NULL, 0) & 0xFF) << 16;
              GetDlgItemText(hDlg, IDC_CONTEXT_MAX_EXIT_LAT, str, COMMON_STR_SIZE);
              p[1] |= (strtol(str, NULL, 0) & 0xFFFF) << 0;
              GetDlgItemText(hDlg, IDC_CONTEXT_INT_TARGET, str, COMMON_STR_SIZE);
              p[2]  = (strtol(str, NULL, 0) & 0x3FF) << 22;
              GetDlgItemText(hDlg, IDC_CONTEXT_TTT, str, COMMON_STR_SIZE);
              p[2] |= (strtol(str, NULL, 0) & 0x3) << 16;
              GetDlgItemText(hDlg, IDC_CONTEXT_TT_PORT_NUM, str, COMMON_STR_SIZE);
              p[2] |= (strtol(str, NULL, 0) & 0xFF) << 8;
              GetDlgItemText(hDlg, IDC_CONTEXT_TT_HUB_SLOT_ID, str, COMMON_STR_SIZE);
              p[2] |= (strtol(str, NULL, 0) & 0xFF) << 0;
              GetDlgItemText(hDlg, IDC_CONTEXT_SLOT_STATE, str, COMMON_STR_SIZE);
              p[3]  = (strtol(str, NULL, 0) & 0x1F) << 27;
              GetDlgItemText(hDlg, IDC_CONTEXT_DEV_ADDRESS, str, COMMON_STR_SIZE);
              p[3] |= (strtol(str, NULL, 0) & 0xFF) << 0;
              for (i=0; i<4; i++) {
                GetDlgItemText(hDlg, IDC_CONTEXT_RSVDO_SLOT_0 + i, str, COMMON_STR_SIZE);
                p[4+i] = strtol(str, NULL, 0);
              }
#if (CONTEXT_SIZE == 64)
              for (i=4; i<12; i++) {
                GetDlgItemText(hDlg, IDC_CONTEXT_RSVDO_SLOT_0 + i, str, COMMON_STR_SIZE);
                p[4+i] = strtol(str, NULL, 0);
              }
#endif
              DEV_MEM_WRITE_PHYSICAL(xhci_context_address, CONTEXT_SIZE + (32 * CONTEXT_SIZE), xhci_context);
              
              delete [] xhci_context;
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              delete [] xhci_context;
              EndDialog(hDlg, 0);
              break;
          }
          break;
        case EN_CHANGE:
          if ((LOWORD(wParam) >= IDC_CONTEXT_MAX_ESIT_HI) && (LOWORD(wParam) <= IDC_CONTEXT_RSVDO_EP_10)) {
            xhci_context_changed = 1;
            EnableWindow(GetDlgItem(hDlg, ID_APPLY), 1);
          }
          switch (LOWORD(wParam)) {
            case IDC_CONTEXT_SPEED:
              GetDlgItemText(hDlg, IDC_CONTEXT_SPEED, str, COMMON_STR_SIZE);
              i = strtol(str, NULL, 0);
              if (i > 15) i = 16;
              SetDlgItemText(hDlg, IDC_CONTEXT_SPEED_STR, slot_speed_str[i]);
              break;
            case IDC_CONTEXT_SLOT_STATE:
              GetDlgItemText(hDlg, IDC_CONTEXT_SLOT_STATE, str, COMMON_STR_SIZE);
              i = strtol(str, NULL, 0);
              if (i > 3) i = 4;
              SetDlgItemText(hDlg, IDC_CONTEXT_SLOT_STATE_STR, slot_type_str[i]);
              break;
            case IDC_CONTEXT_EP_STATE:
              GetDlgItemText(hDlg, IDC_CONTEXT_EP_STATE, str, COMMON_STR_SIZE);
              i = strtol(str, NULL, 0);
              if (i > 7) i = 7;
              SetDlgItemText(hDlg, IDC_CONTEXT_EP_STATE_STR, ep_state_str[i]);
              break;
            case IDC_CONTEXT_EP_TYPE:
              GetDlgItemText(hDlg, IDC_CONTEXT_EP_TYPE, str, COMMON_STR_SIZE);
              i = strtol(str, NULL, 0);
              if (i > 7) i = 7;
              SetDlgItemText(hDlg, IDC_CONTEXT_EP_TYPE_STR, ep_type_str[i]);
              break;
            case IDC_CONTEXT_MAX_PSTREAMS:
              GetDlgItemText(hDlg, IDC_CONTEXT_MAX_PSTREAMS, str, COMMON_STR_SIZE);
              i = strtol(str, NULL, 0);
              if (i == 0)
                SetDlgItemText(hDlg, IDC_CONTEXT_MAXPS_STR, "None");
              else if (i <= 15)
                SetDlgItemInt(hDlg, IDC_CONTEXT_MAXPS_STR, (1 << (i + 1)), 0);
              else
                SetDlgItemText(hDlg, IDC_CONTEXT_MAXPS_STR, "Error");
              EnableWindow(GetDlgItem(hDlg, IDC_CONTEXT_STREAM_CONTEXT), (i > 0) && (i <=15));
              break;
          }
          break;
      }
  }
  
  return 0;
}

static void CALLBACK hc_xhci_callback_str_context_get(HWND hDlg)
{
  char str[COMMON_STR_SIZE];
  Bit32u *p;

  // display the EP (with Drop and Add bits)
  sprintf(str, "Context %i of %i", xhci_str_current_context, xhci_max_streams - 1);
  if (xhci_str_current_context == 0)
    strcat(str, "(Reserved)");
  SetDlgItemText(hDlg, IDC_CONTEXT_OF_STR, str);
  
  // String Context
  p = (Bit32u *) &xhci_str_context[xhci_str_current_context * 32];
  sprintf(str, "0x" FMT_ADDRX64, ((Bit64u) p[1] << 32) | (p[0] & ~0xFULL));
  SetDlgItemText(hDlg, IDC_STR_CONTEXT_DQPTR, str);
  sprintf(str, "%i", (p[0] & (0x7 << 1)) >> 1);
  SetDlgItemText(hDlg, IDC_STR_CONTEXT_SCT, str);
  CheckDlgButton(hDlg, IDC_STR_CONTEXT_DCS, (p[0] & (1<<0)) ? BST_CHECKED : BST_UNCHECKED);
  sprintf(str, "0x%08X", p[2] & 0x00FFFFFF);
  SetDlgItemText(hDlg, IDC_STR_CONTEXT_STOPPED, str);
  sprintf(str, "0x%04X", (p[2] & 0xFF000000) >> 24);
  SetDlgItemText(hDlg, IDC_STR_CONTEXT_RSVDO_0, str);
  sprintf(str, "0x%08X", p[3]);
  SetDlgItemText(hDlg, IDC_STR_CONTEXT_RSVDO_1, str);
  
  xhci_str_context_changed = 0;
  EnableWindow(GetDlgItem(hDlg, ID_APPLY), 0);
}

static void CALLBACK hc_xhci_callback_str_context_put(HWND hDlg)
{
  char str[COMMON_STR_SIZE];
  Bit32u *p;

  // String Context
  p = (Bit32u *) &xhci_str_context[xhci_str_current_context * 32];
  GetDlgItemText(hDlg, IDC_STR_CONTEXT_DQPTR, str, COMMON_STR_SIZE);
  p[0]  = strtol(str, NULL, 0) & ~0xFULL;
  p[1]  = (Bit64u) strtol(str, NULL, 0) >> 32;
  GetDlgItemText(hDlg, IDC_STR_CONTEXT_SCT, str, COMMON_STR_SIZE);
  p[0] |= (strtol(str, NULL, 0) & 0x7) << 1;
  p[0] |= (IsDlgButtonChecked(hDlg, IDC_STR_CONTEXT_DCS) == BST_CHECKED) ? (1<<0) : 0;
  GetDlgItemText(hDlg, IDC_STR_CONTEXT_STOPPED, str, COMMON_STR_SIZE);
  p[1]  = (strtol(str, NULL, 0) & 0x00FFFFFF) << 0;
  GetDlgItemText(hDlg, IDC_STR_CONTEXT_RSVDO_0, str, COMMON_STR_SIZE);
  p[1] |= (strtol(str, NULL, 0) & 0xFF) << 24;
  GetDlgItemText(hDlg, IDC_STR_CONTEXT_RSVDO_1, str, COMMON_STR_SIZE);
  p[2]  = strtol(str, NULL, 0);
  
  xhci_str_context_changed = 0;
  EnableWindow(GetDlgItem(hDlg, ID_APPLY), 0);
}

// xHCI 1.2: 6.2.4.1, Page 458
static INT_PTR CALLBACK hc_xhci_callback_str_context(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  char str[COMMON_STR_SIZE];
  int i;
  
  switch (msg) {
    case WM_INITDIALOG:
      xhci_str_current_context = 0;
      xhci_str_context = new Bit8u[CONTEXT_SIZE + (32 * CONTEXT_SIZE)];
      DEV_MEM_READ_PHYSICAL(xhci_str_context_address, MAX_PSA_SIZE_NUM * 32, xhci_str_context);

      // String Context
      hc_xhci_callback_str_context_get(hDlg);

      xhci_str_context_changed = 0;
      EnableWindow(GetDlgItem(hDlg, ID_APPLY), 0);
      SetFocus(GetDlgItem(hDlg, IDC_STR_CONTEXT_DQPTR));
      return TRUE;
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case ID_APPLY:
              hc_xhci_callback_str_context_put(hDlg);
              break;
            case IDC_CONTEXT_PREV:
              if (xhci_str_current_context > 0) {
                if (xhci_str_context_changed) {
                  int ret = MessageBox(hDlg, "Context has changed. Save?", NULL, MB_ICONQUESTION | MB_YESNOCANCEL);
                  if (ret == IDYES)
                    hc_xhci_callback_str_context_put(hDlg);
                  else if (ret == IDCANCEL)
                    break;
                }
                xhci_str_current_context--;
                hc_xhci_callback_str_context_get(hDlg);
              }
              break;
            case IDC_CONTEXT_NEXT:
              if (xhci_str_current_context < (xhci_max_streams - 1)) {
                if (xhci_str_context_changed) {
                  int ret = MessageBox(hDlg, "Context has changed. Save?", NULL, MB_ICONQUESTION | MB_YESNOCANCEL);
                  if (ret == IDYES)
                    hc_xhci_callback_str_context_put(hDlg);
                  else if (ret == IDCANCEL)
                    break;
                }
                xhci_str_current_context++;
                hc_xhci_callback_str_context_get(hDlg);
              }
              break;
            case IDOK:
              if (xhci_str_context_changed) {
                int ret = MessageBox(hDlg, "Context has changed. Save?", NULL, MB_ICONQUESTION | MB_YESNOCANCEL);
                if (ret == IDYES)
                  hc_xhci_callback_str_context_put(hDlg);
                else if (ret == IDCANCEL)
                  break;
              }
              
              DEV_MEM_WRITE_PHYSICAL(xhci_str_context_address, MAX_PSA_SIZE_NUM * 32, xhci_str_context);
              
              delete [] xhci_str_context;
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              delete [] xhci_str_context;
              EndDialog(hDlg, 0);
              break;
          }
          break;
        case EN_CHANGE:
          if ((LOWORD(wParam) >= IDC_STR_CONTEXT_DQPTR) && (LOWORD(wParam) <= IDC_STR_CONTEXT_RSVDO_1)) {
            xhci_str_context_changed = 1;
            EnableWindow(GetDlgItem(hDlg, ID_APPLY), 1);
          }
          switch (LOWORD(wParam)) {
            case IDC_STR_CONTEXT_SCT:
              GetDlgItemText(hDlg, IDC_STR_CONTEXT_SCT, str, COMMON_STR_SIZE);
              i = strtol(str, NULL, 0);
              if (i > 7) i = 8;
              SetDlgItemText(hDlg, IDC_CONTEXT_SCT_STR, string_sct_str[i]);
              break;
          }
          break;
      }
  }
  
  return 0;
}



///////////////////////////////////////////////////////////////////////////////////////////////
//  Attributes
char    a_title[COMMON_STR_SIZE];
BOOL    a_single;
DWORD64 a_attrib;
const struct S_ATTRIBUTES *a_attributes;
HWND ListBox = NULL;

#include <windowsx.h>

static INT_PTR CALLBACK attribute_callback(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  int bCaption = SW_HIDE;
  int i = 0, j = 0;
  RECT rect;

  switch (msg) {
    case WM_INITDIALOG:
      SetWindowText(hDlg, a_title);
      // move the window to be "within" the parent window
      GetWindowRect((HWND) lParam, &rect);
      SetWindowPos(hDlg, HWND_TOP, rect.right, rect.top - 3, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
      // initialize the ListBox
      ListBox = GetDlgItem(hDlg, IDC_LIST);
      ListBox_ResetContent(ListBox);
      while (a_attributes[i].index > -1) {
        ListBox_AddString(ListBox, a_attributes[i].str);
        if ((a_attrib & a_attributes[i].mask) == a_attributes[i].attrb)
          ListBox_SetSel(ListBox, TRUE, i);
        if (a_attributes[i].str[0] == '*')
          bCaption = SW_SHOW;
        i++;
      }

      // no need to show the caption if no 'single-only' entries
      ShowWindow(GetDlgItem(hDlg, IDC_CAPTION), bCaption);
      break;
      
    case WM_COMMAND:
      switch (HIWORD(wParam)) {
        case BN_CLICKED:
          switch (LOWORD(wParam)) {
            case IDCANCEL:
              EndDialog(hDlg, IDCANCEL);
              break;
            case IDOK:
              a_attrib = 0;
              while (a_attributes[i].index > -1) {
                if (ListBox_GetSel(ListBox, i))
                  a_attrib |= a_attributes[i].attrb;
                i++;
              }
              EndDialog(hDlg, IDOK);
              break;
          }
          break;
          
        case LBN_SELCHANGE:
          i = ListBox_GetCaretIndex(ListBox); // index to just clicked item
          if (ListBox_GetSel(ListBox, i)) {
            if (a_single) {
              for (j=0; j<ListBox_GetCount(ListBox); j++)
                if (j != i)
                  ListBox_SetSel(ListBox, FALSE, j);
            } else {
              while (a_attributes[i].groups[j] > -1) {
                ListBox_SetSel(ListBox, FALSE, a_attributes[i].groups[j]);
                j++;
              }
            }
          }
          break;
      }
  }

  return 0;
}

void do_attributes(HWND hwnd, DWORD id, const int size, const char *title, const struct S_ATTRIBUTES *attribs)
{
  char str[COMMON_STR_SIZE];
  char str1[COMMON_STR_SIZE];

  GetDlgItemText(hwnd, id, str, COMMON_STR_SIZE);
  strncpy(a_title, title, COMMON_STR_SIZE);
  a_single = FALSE;
  a_attrib = strtoul(str, NULL, 0);
  a_attributes = attribs;

  // create the dialog and wait for it to return
  if (DialogBoxParam(NULL, MAKEINTRESOURCE(IDD_ATTRIBUTE), hwnd, attribute_callback, (LPARAM) GetDlgItem(hwnd, id)) == IDOK) {
    sprintf(str1, "0x%%0%iX", size);
    sprintf(str, str1, (DWORD) a_attrib);
    SetDlgItemText(hwnd, id, str);
  }
}


#endif

// ben: do Software Debug in UHCI (command reg: bit 5)
//   add 'stop' to trigger options.
