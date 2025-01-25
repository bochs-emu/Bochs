/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C)      2023  Benjamin David Lunt
//  Copyright (C) 2003-2025 The Bochs Project
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

#if BX_USB_DEBUGGER

struct CALLBACK_PARAMS {
  int type;
  int break_type;
  Bit64u zParam;
  int wParam;
  int lParam;
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
void do_attributes(HWND hwnd, DWORD id, const int size, const char *title, const struct S_ATTRIBUTES *attribs);


#endif  // BX_USB_DEBUGGER
#endif  // BX_WIN32_USB_H
