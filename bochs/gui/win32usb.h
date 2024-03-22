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

#ifndef BX_WIN32_USB_H
#define BX_WIN32_USB_H

#if BX_USE_WIN32USBDEBUG

#define COMMON_STR_SIZE  128

#define USB_DEBUG_NONE   0
#define USB_DEBUG_UHCI   1
#define USB_DEBUG_OHCI   2
#define USB_DEBUG_EHCI   3
#define USB_DEBUG_XHCI   4

BOCHSAPI_MSVCONLY int win32_usb_start(HWND hwnd, int break_type, int wParam, int lParam);

// USB debug break_type
#define USB_DEBUG_FRAME    1
#define USB_DEBUG_COMMAND  2
#define USB_DEBUG_EVENT    3
#define USB_DEBUG_NONEXIST 4
#define USB_DEBUG_RESET    5
#define USB_DEBUG_ENABLE   6

BOCHSAPI_MSVCONLY void win32_usb_trigger(int type, int trigger, int wParam, int lParam);

// lParam flags
#define USB_LPARAM_FLAG_BEFORE  0x00000001
#define USB_LPARAM_FLAG_AFTER   0x00000002

struct CALLBACK_PARAMS {
  int type;
  int break_type;
  int wParam;
  int lParam;
};

struct DUMP_PARAMS {
  char title[COMMON_STR_SIZE];
  bx_phy_address address;
  int size;  // amount to dump (no more than 512)
  bool big;  // use 64-bit addresses?
};
INT_PTR CALLBACK dump_dialog_callback(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////////////////////////////
//  UHCI
//
INT_PTR CALLBACK hc_uhci_callback(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void hc_uhci_do_item(Bit32u FrameAddr, Bit32u FrameNum);
int hc_uhci_init(HWND hwnd);
int hc_uhci_save(HWND hwnd);
void uhci_display_td(HWND hwnd);

INT_PTR CALLBACK hc_uhci_callback_queue(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_uhci_callback_td(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////////////////////////////
//  OHCI
//
INT_PTR CALLBACK hc_ohci_callback(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
int hc_ohci_init(HWND hwnd);

///////////////////////////////////////////////////////////////////////////////////////////////
//  EHCI
//
INT_PTR CALLBACK hc_ehci_callback(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
int hc_ehci_init(HWND hwnd);


///////////////////////////////////////////////////////////////////////////////////////////////
//  XHCI
//
#define VIEW_TRB_TYPE_NONE      0
#define VIEW_TRB_TYPE_COMMAND   1
#define VIEW_TRB_TYPE_EVENT     2
#define VIEW_TRB_TYPE_TRANSFER  4

struct VIEW_TRB_TYPE {
  Bit8u allowed_mask;
  char  name[22];
};

#define MAX_TRBS_ALLOWED 4096

INT_PTR CALLBACK hc_xhci_callback(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
int hc_xhci_init(HWND hwnd);
int hc_xhci_save(HWND hwnd);
void hc_xhci_do_ring(const char *ring_str, Bit64u RingPtr, Bit64u dequeue_ptr);
void hc_xhci_do_event_ring(const char *ring_str, int interrupter);
void xhci_display_trb(HWND hwnd, int type_mask);
INT_PTR CALLBACK hc_xhci_callback_trb_normal(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_setup(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_data(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_status(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_link(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_event(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_noop(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_enslot(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_disslot(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_address(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_configep(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_evaluate(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_resetep(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_stopep(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_settrptr(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_resetdev(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_forceevent(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_setlat(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_getband(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_forcehdr(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_transevent(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_compcompletion(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_pschange(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_bandrequ(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_doorbell(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_hostevent(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_devnotevent(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK hc_xhci_callback_trb_necfw(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_necun(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_trb_necfwevent(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK hc_xhci_callback_context(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK hc_xhci_callback_str_context(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);


///////////////////////////////////////////////////////////////////////////////////////////////
//  Attributes
struct S_ATTRIBUTES {
  DWORD64 attrb;
  DWORD64 mask;
  int   index;
  char  str[32];
  int   groups[10];  // up to 10 items can be grouped.  Increase if we need more.
};
void do_attributes(HWND hwnd, DWORD id, const int size, const char *title, const struct S_ATTRIBUTES *attribs);


#endif  // BX_USE_WIN32USBDEBUG
#endif  // BX_WIN32_USB_H
