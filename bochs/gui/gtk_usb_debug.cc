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
#include "siminterface.h"
#include "param_names.h"

#if BX_USB_DEBUGGER

#include "usb_debug.h"
// hack for xHCI
#include "iodev.h"
#include "iodev/usb/usb_common.h"
#include "iodev/usb/usb_xhci.h"

#include <gtk/gtk.h>
#include <glib.h>

typedef struct {
  const char *name;
  GtkWidget *entry;
  Bit8u fmt_size;
  const struct S_ATTRIBUTES *attr;
} usb_reg_t;

const char *chkTxt[6] = {"Reset", "Enable", "Event", "Doorbell", "Start of Frame", "Non-Exist"};
const char *chkBXPN[6] = {BXPN_USB_DEBUG_RESET, BXPN_USB_DEBUG_ENABLE, BXPN_USB_DEBUG_EVENT,
                          BXPN_USB_DEBUG_DOORBELL, BXPN_USB_DEBUG_START_FRAME, BXPN_USB_DEBUG_NON_EXIST};

GtkWidget *main_dialog;
GtkWidget *DFframe, *DFvbox, *checkbox[6];
GtkWidget *apply_hbox, *apply_button;

// multithreading using pure posix threads -- not glib threads
static void * EventLp(void *data)
{
  gtk_main();
  return NULL;
}

static void MakeGTKthreads()
{
  pthread_t hThread;
  pthread_create(&hThread, NULL, EventLp, NULL);
}

GtkWidget* usbdlg_create_label(GtkWidget *box, const char *text, bool expand = true)
{
  GtkWidget *label = gtk_label_new(text);
  gtk_box_pack_start(GTK_BOX(box), label, expand, expand, 2);
  return label;
}

GtkWidget* usbdlg_create_entry_with_label(GtkWidget **box, const char *text, 
                                          bool expand = true)
{
  gtk_box_pack_start(GTK_BOX(box[0]), gtk_label_new(text), expand, expand, 2);
  GtkWidget *entry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(box[1]), entry, expand, expand, 2);
  return entry;
}

GtkWidget* usbdlg_create_ro_entry(GtkWidget *box, bool expand = true)
{
  GtkWidget *entry = gtk_entry_new();
  gtk_widget_set_sensitive(entry, 0);
  gtk_box_pack_start(GTK_BOX(box), entry, expand, expand, 2);
  return entry;
}

GtkWidget* usbdlg_create_ro_entry_with_label(GtkWidget **box, const char *text,
                                             bool expand = true)
{
  gtk_box_pack_start(GTK_BOX(box[0]), gtk_label_new(text), expand, expand, 2);
  GtkWidget *entry = gtk_entry_new();
  gtk_widget_set_sensitive(entry, 0);
  gtk_box_pack_start(GTK_BOX(box[1]), entry, expand, expand, 2);
  return entry;
}

void usbdlg_create_debug_flags(GtkWidget *vbox)
{
  DFframe = gtk_frame_new("Debug Flags");
  gtk_box_pack_start(GTK_BOX(vbox), DFframe, FALSE, FALSE, 2);
  DFvbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(DFframe), DFvbox);
  for (int i = 0; i < 6; i++) {
    checkbox[i] = gtk_check_button_new_with_label(chkTxt[i]);
    if (SIM->get_param_num(chkBXPN[i])->get() > 0) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[i]), TRUE);
    }
    gtk_box_pack_start(GTK_BOX(DFvbox), checkbox[i], FALSE, FALSE, 2);
  }
}

void usbdlg_set_debug_flags()
{
  for (int i = 0; i < 6; i++) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[i]))) {
      SIM->get_param_num(chkBXPN[i])->set(1);
    } else {
      SIM->get_param_num(chkBXPN[i])->set(0);
    }
  }
}

static void usb_regview_dialog(GtkWidget *widget, gpointer data)
{
  int i, ret;
  char buffer[32], format[10];
  Bit32u oldval, value;

  usb_reg_t *usb_reg_def = (usb_reg_t*)data;
  GtkWidget *dialog =
    gtk_dialog_new_with_buttons(usb_reg_def->name, GTK_WINDOW(main_dialog), GTK_DIALOG_MODAL,
                                g_dgettext("gtk30", "_OK"), GTK_RESPONSE_OK,
                                g_dgettext("gtk30", "_Cancel"), GTK_RESPONSE_CANCEL,
                                NULL);
  gtk_window_set_default_size(GTK_WINDOW(dialog), 250, 250);
  GtkWidget *listbox = gtk_list_box_new();
  gtk_list_box_set_selection_mode(GTK_LIST_BOX(listbox), GTK_SELECTION_MULTIPLE);
  gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(listbox), 1);
  i = 0;
  while (usb_reg_def->attr[i].index > -1) {
    gtk_list_box_insert(GTK_LIST_BOX(listbox), gtk_label_new(usb_reg_def->attr[i].str), -1);
    i++;
  }
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), listbox, TRUE, TRUE, 2);
  // Show dialog
  gtk_widget_show_all(dialog);
  oldval = strtoul(gtk_entry_get_text(GTK_ENTRY(usb_reg_def->entry)), NULL, 16);
  i = 0;
  while (usb_reg_def->attr[i].index > -1) {
    if ((oldval & usb_reg_def->attr[i].mask) != 0) {
      gtk_list_box_select_row(GTK_LIST_BOX(listbox),
                              gtk_list_box_get_row_at_index(GTK_LIST_BOX(listbox), i));
    } else {
      gtk_list_box_unselect_row(GTK_LIST_BOX(listbox),
                                gtk_list_box_get_row_at_index(GTK_LIST_BOX(listbox), i));
    }
    i++;
  }
  ret = gtk_dialog_run(GTK_DIALOG(dialog));
  if (ret == GTK_RESPONSE_OK) {
    value = 0;
    i = 0;
    while (usb_reg_def->attr[i].index > -1) {
      if (gtk_list_box_row_is_selected(gtk_list_box_get_row_at_index(GTK_LIST_BOX(listbox), i))) {
        value |= usb_reg_def->attr[i].mask;
      }
      i++;
    }
    if (value != oldval) {
      sprintf(format, "0x%%0%iX", usb_reg_def->fmt_size);
      sprintf(buffer, format, value);
      gtk_entry_set_text(GTK_ENTRY(usb_reg_def->entry), buffer);
    }
  }
  gtk_widget_destroy(dialog);
}

static void on_entry_changed(GtkWidget *widget, gpointer data)
{
  gtk_widget_set_sensitive(apply_button, 1);
}

static void apply_changes(GtkWidget *widget, gpointer data)
{
  GtkWidget* error = gtk_message_dialog_new(
    GTK_WINDOW(main_dialog), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
    "Saving changes to controller not implemented yet");
  gtk_window_set_title(GTK_WINDOW(error), "WARNING");
  gtk_dialog_run(GTK_DIALOG(error));
  gtk_widget_destroy(error);
}

int uhci_debug_dialog(int type, int param1)
{
  bx_list_c *UHCI_state = NULL;
  int i, ret;
  Bit32u pci_bar_address, frame_addr, frame_num;
  char buffer[COMMON_STR_SIZE];
  GtkWidget *mainHbox, *BAhbox, *ORhbox, *PRhbox;
  GtkWidget *entry[11], *button[7];
  GtkWidget *vbox[3], *ORvbox[3], *PRvbox[3];
  GtkWidget *ORframe, *PRframe;
  usb_reg_t usb_reg_def[5];

  UHCI_state = (bx_list_c*)SIM->get_param("usb_uhci", SIM->get_bochs_root());
  if (UHCI_state == NULL)
    return -1;

  main_dialog = gtk_dialog_new();
  switch (type) {
    case USB_DEBUG_FRAME:
      gtk_window_set_title(GTK_WINDOW(main_dialog), "UHCI Debug Dialog: Break Type: Start of Frame");
      break;
    case USB_DEBUG_COMMAND:
      gtk_window_set_title(GTK_WINDOW(main_dialog), "UHCI Debug Dialog: Break Type: Doorbell");
      break;
    case USB_DEBUG_NONEXIST:
      gtk_window_set_title(GTK_WINDOW(main_dialog), "UHCI Debug Dialog: Break Type: Non-Existant Port Write");
      break;
    case USB_DEBUG_RESET:
      sprintf(buffer, "UHCI Debug Dialog: Break Type: Port %d Reset", param1);
      gtk_window_set_title(GTK_WINDOW(main_dialog), buffer);
      break;
    case USB_DEBUG_ENABLE:
      sprintf(buffer, "UHCI Debug Dialog: Break Type: Port %d Enable", param1);
      gtk_window_set_title(GTK_WINDOW(main_dialog), buffer);
      break;
    default:
      gtk_window_set_title(GTK_WINDOW(main_dialog), "UHCI Debug Dialog");
  }
  gtk_window_set_default_size(GTK_WINDOW(main_dialog), 600, 500);
  button[0] = gtk_dialog_add_button(GTK_DIALOG(main_dialog), "Continue", GTK_RESPONSE_OK);
  button[1] = gtk_dialog_add_button(GTK_DIALOG(main_dialog), "Quit", GTK_RESPONSE_CANCEL);
  gtk_dialog_set_default_response(GTK_DIALOG(main_dialog), GTK_RESPONSE_OK);
  gtk_window_set_focus(GTK_WINDOW(main_dialog), button[0]);
  mainHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(main_dialog))), mainHbox, TRUE, TRUE, 2);
  for (i = 0; i < 3; i++) {
    vbox[i] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(mainHbox), vbox[i], TRUE, TRUE, 2);
  }

  BAhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(vbox[0]), BAhbox, FALSE, FALSE, 2);
  usbdlg_create_label(BAhbox, "UHCI at Base IO address", false);
  entry[0] = usbdlg_create_ro_entry(BAhbox, false);

  ORframe = gtk_frame_new("Operational Registers");
  gtk_box_pack_start(GTK_BOX(vbox[0]), ORframe, FALSE, FALSE, 2);
  ORhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_container_add(GTK_CONTAINER(ORframe), ORhbox);
  for (i = 0; i < 3; i++) {
    ORvbox[i] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(ORhbox), ORvbox[i], FALSE, FALSE, 2);
  }
  entry[1] = usbdlg_create_entry_with_label(ORvbox, "Command");
  button[2] = gtk_button_new_with_label("<>");
  usb_reg_def[0] = {"Command Register", entry[1], 4, attribs_u_command};
  g_signal_connect(button[2], "clicked", G_CALLBACK(usb_regview_dialog), &usb_reg_def[0]);
  gtk_box_pack_start(GTK_BOX(ORvbox[2]), button[2], FALSE, FALSE, 2);
  entry[2] = usbdlg_create_entry_with_label(ORvbox, "Status");
  button[3] = gtk_button_new_with_label("<>");
  usb_reg_def[1] = {"Status Register", entry[2], 4, attribs_u_status};
  g_signal_connect(button[3], "clicked", G_CALLBACK(usb_regview_dialog), &usb_reg_def[1]);
  gtk_box_pack_start(GTK_BOX(ORvbox[2]), button[3], FALSE, FALSE, 2);
  entry[3] = usbdlg_create_entry_with_label(ORvbox, "Interrupt Enable");
  button[4] = gtk_button_new_with_label("<>");
  usb_reg_def[2] = {"Interrupt Enable Register", entry[3], 4, attribs_u_interrupt};
  g_signal_connect(button[4], "clicked", G_CALLBACK(usb_regview_dialog), &usb_reg_def[2]);
  gtk_box_pack_start(GTK_BOX(ORvbox[2]), button[4], FALSE, FALSE, 2);
  entry[4] = usbdlg_create_ro_entry_with_label(ORvbox, "Frame Number");
  entry[5] = usbdlg_create_ro_entry_with_label(ORvbox, "Frame Address");
  entry[6] = usbdlg_create_ro_entry_with_label(ORvbox, "Start of Frame");

  PRframe = gtk_frame_new("Port Registers");
  gtk_box_pack_start(GTK_BOX(vbox[0]), PRframe, FALSE, FALSE, 2);
  PRhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_container_add(GTK_CONTAINER(PRframe), PRhbox);
  for (i = 0; i < 3; i++) {
    PRvbox[i] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(PRhbox), PRvbox[i], FALSE, FALSE, 2);
  }
  usb_reg_def[3] = {"Port 0 Register", NULL, 4, attribs_u_ports};
  usb_reg_def[4] = {"Port 1 Register", NULL, 4, attribs_u_ports};
  for (i = 0; i < 2; i++) {
    sprintf(buffer, "Port %d", i);
    entry[i * 2 + 7] = usbdlg_create_entry_with_label(PRvbox, buffer);
    entry[i * 2 + 8] = usbdlg_create_ro_entry_with_label(PRvbox, "Emulation Type");
    button[i + 5] = gtk_button_new_with_label("<>");
    usb_reg_def[i + 3].entry = entry[i * 2 + 7];
    g_signal_connect(button[i + 5], "clicked", G_CALLBACK(usb_regview_dialog), &usb_reg_def[i + 3]);
    gtk_box_pack_start(GTK_BOX(PRvbox[2]), button[i + 5], FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(PRvbox[2]), gtk_label_new(" "), FALSE, FALSE, 8); // spacer
  }

  apply_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(vbox[0]), apply_hbox, FALSE, FALSE, 2);
  apply_button = gtk_button_new_with_label(g_dgettext("gtk30", "_Apply"));
  gtk_widget_set_sensitive(apply_button, 0);
  g_signal_connect(apply_button, "clicked", G_CALLBACK(apply_changes), NULL);
  gtk_box_pack_start(GTK_BOX(apply_hbox), apply_button, FALSE, FALSE, 2);
  usbdlg_create_debug_flags(vbox[2]);
  // Set values
  pci_bar_address = get_pci_bar_addr((bx_shadow_data_c*)SIM->get_param("hub.pci_conf", UHCI_state), 4);
  sprintf(buffer, "0x%04X", pci_bar_address);
  gtk_entry_set_text(GTK_ENTRY(entry[0]), buffer);
  sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 0, 2));
  gtk_entry_set_text(GTK_ENTRY(entry[1]), buffer);
  g_signal_connect(GTK_EDITABLE(entry[1]), "changed", G_CALLBACK(on_entry_changed), NULL);
  sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 2, 2));
  gtk_entry_set_text(GTK_ENTRY(entry[2]), buffer);
  g_signal_connect(GTK_EDITABLE(entry[2]), "changed", G_CALLBACK(on_entry_changed), NULL);
  sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 4, 2));
  gtk_entry_set_text(GTK_ENTRY(entry[3]), buffer);
  g_signal_connect(GTK_EDITABLE(entry[3]), "changed", G_CALLBACK(on_entry_changed), NULL);
  frame_num = usb_io_read(pci_bar_address + 6, 2);
  sprintf(buffer, "0x%04X", frame_num);
  gtk_entry_set_text(GTK_ENTRY(entry[4]), buffer);
  frame_addr = usb_io_read(pci_bar_address + 8, 4);
  sprintf(buffer, "0x%08X", frame_addr);
  gtk_entry_set_text(GTK_ENTRY(entry[5]), buffer);
  sprintf(buffer, "0x%02X", usb_io_read(pci_bar_address + 12, 1));
  gtk_entry_set_text(GTK_ENTRY(entry[6]), buffer);
  sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 16, 2));
  gtk_entry_set_text(GTK_ENTRY(entry[7]), buffer);
  g_signal_connect(GTK_EDITABLE(entry[7]), "changed", G_CALLBACK(on_entry_changed), NULL);
  SIM->get_param_enum("port1.device", host_param)->dump_param(buffer, COMMON_STR_SIZE, 1);
  gtk_entry_set_text(GTK_ENTRY(entry[8]), buffer);
  sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 18, 2));
  gtk_entry_set_text(GTK_ENTRY(entry[9]), buffer);
  g_signal_connect(GTK_EDITABLE(entry[9]), "changed", G_CALLBACK(on_entry_changed), NULL);
  SIM->get_param_enum("port2.device", host_param)->dump_param(buffer, COMMON_STR_SIZE, 1);
  gtk_entry_set_text(GTK_ENTRY(entry[10]), buffer);
  // Show dialog
  gtk_widget_show_all(main_dialog);
  ret = gtk_dialog_run(GTK_DIALOG(main_dialog));
  if (ret == GTK_RESPONSE_OK) {
    usbdlg_set_debug_flags();
  }
  gtk_widget_destroy(main_dialog);
  return ret;
}

int xhci_debug_dialog(int type, int param1)
{
  bx_list_c *xHCI_state = NULL;
  int i, n_ports, ret;
  Bit32u pci_bar_address, dword, offset;
  char buffer[COMMON_STR_SIZE], tmpbuf[32];
  GtkWidget *mainVbox, *BAhbox, *hbox[2], *vbox[4], *entry[28];
  GtkWidget *CRframe, *CRhbox, *CRvbox[2];
  GtkWidget *ORframe, *ORhbox, *ORvbox[2];
  GtkWidget *RTframe, *RThbox, *RTvbox[2];
  GtkWidget *PRframe, *PRhbox, *PRvbox[2];

  xHCI_state = (bx_list_c*)SIM->get_param("usb_xhci", SIM->get_bochs_root());
  if (xHCI_state == NULL)
    return -1;

  main_dialog = gtk_dialog_new();
  switch (type) {
    case USB_DEBUG_COMMAND:
      gtk_window_set_title(GTK_WINDOW(main_dialog), "xHCI Debug Dialog: Break Type: Command Ring");
      break;
    case USB_DEBUG_EVENT:
      gtk_window_set_title(GTK_WINDOW(main_dialog), "xHCI Debug Dialog: Break Type: Event Ring");
      break;
    case USB_DEBUG_FRAME:
      gtk_window_set_title(GTK_WINDOW(main_dialog), "xHCI Debug Dialog: Break Type: Frame");
      break;
    default:
      gtk_window_set_title(GTK_WINDOW(main_dialog), "xHCI Debug Dialog");
  }
  gtk_window_set_default_size(GTK_WINDOW(main_dialog), 600, 500);
  gtk_dialog_add_button(GTK_DIALOG(main_dialog), "Continue", GTK_RESPONSE_OK);
  gtk_dialog_add_button(GTK_DIALOG(main_dialog), "Quit", GTK_RESPONSE_CANCEL);
  gtk_dialog_set_default_response(GTK_DIALOG(main_dialog), GTK_RESPONSE_OK);
  mainVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(main_dialog))), mainVbox, TRUE, TRUE, 2);

  BAhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(mainVbox), BAhbox, FALSE, FALSE, 2);
  usbdlg_create_label(BAhbox, "xHCI at Base MMIO address", false);
  entry[0] = usbdlg_create_ro_entry(BAhbox, false);

  hbox[0] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(mainVbox), hbox[0], TRUE, TRUE, 2);
  hbox[1] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(mainVbox), hbox[1], TRUE, TRUE, 2);
  for (i = 0; i < 4; i++) {
    vbox[i] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox[0]), vbox[i], TRUE, TRUE, 2);
  }

  CRframe = gtk_frame_new("Capability Registers");
  gtk_box_pack_start(GTK_BOX(vbox[0]), CRframe, FALSE, FALSE, 2);
  CRhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_container_add(GTK_CONTAINER(CRframe), CRhbox);
  CRvbox[0] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(CRhbox), CRvbox[0], FALSE, FALSE, 2);
  CRvbox[1] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(CRhbox), CRvbox[1], FALSE, FALSE, 2);
  entry[1] = usbdlg_create_ro_entry_with_label(CRvbox, "Cap Length");
  entry[2] = usbdlg_create_ro_entry_with_label(CRvbox, "HCSParams1");
  entry[3] = usbdlg_create_ro_entry_with_label(CRvbox, "HCSParams2");
  entry[4] = usbdlg_create_ro_entry_with_label(CRvbox, "HCSParams3");
  entry[5] = usbdlg_create_ro_entry_with_label(CRvbox, "HCCParams1");
  entry[6] = usbdlg_create_ro_entry_with_label(CRvbox, "DB Offset");
  entry[7] = usbdlg_create_ro_entry_with_label(CRvbox, "RTS Offset");
  entry[8] = usbdlg_create_ro_entry_with_label(CRvbox, "HCCParams2");

  ORframe = gtk_frame_new("Operational Registers");
  gtk_box_pack_start(GTK_BOX(vbox[1]), ORframe, FALSE, FALSE, 2);
  ORhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_container_add(GTK_CONTAINER(ORframe), ORhbox);
  ORvbox[0] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(ORhbox), ORvbox[0], FALSE, FALSE, 2);
  ORvbox[1] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(ORhbox), ORvbox[1], FALSE, FALSE, 2);
  entry[9] = usbdlg_create_ro_entry_with_label(ORvbox, "Command");
  entry[10] = usbdlg_create_ro_entry_with_label(ORvbox, "Status");
  entry[11] = usbdlg_create_ro_entry_with_label(ORvbox, "Page Size");
  entry[12] = usbdlg_create_ro_entry_with_label(ORvbox, "Device Notification");
  entry[13] = usbdlg_create_ro_entry_with_label(ORvbox, "Command Ring");
  entry[14] = usbdlg_create_ro_entry_with_label(ORvbox, "Device Context Base");
  entry[15] = usbdlg_create_ro_entry_with_label(ORvbox, "Configure");

  RTframe = gtk_frame_new("Runtime Registers");
  gtk_box_pack_start(GTK_BOX(vbox[2]), RTframe, FALSE, FALSE, 2);
  RThbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_container_add(GTK_CONTAINER(RTframe), RThbox);
  RTvbox[0] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(RThbox), RTvbox[0], FALSE, FALSE, 2);
  RTvbox[1] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(RThbox), RTvbox[1], FALSE, FALSE, 2);
  entry[16] = usbdlg_create_ro_entry_with_label(RTvbox, "Microframe Index");

  PRframe = gtk_frame_new("Port Registers");
  gtk_box_pack_start(GTK_BOX(hbox[1]), PRframe, FALSE, FALSE, 2);
  PRhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_container_add(GTK_CONTAINER(PRframe), PRhbox);
  PRvbox[0] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(PRhbox), PRvbox[0], FALSE, FALSE, 2);
  PRvbox[1] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(PRhbox), PRvbox[1], FALSE, FALSE, 2);
  n_ports = SIM->get_param_num(BXPN_XHCI_N_PORTS)->get();
  for (i = 0; i < n_ports; i++) {
    sprintf(buffer, "Port %d", i);
    entry[i * 2 + 17] = usbdlg_create_entry_with_label(PRvbox, buffer);
    entry[i * 2 + 18] = usbdlg_create_ro_entry_with_label(PRvbox, "Emulation Type");
  }
  usbdlg_create_debug_flags(vbox[3]);
  // Set values
  pci_bar_address = get_pci_bar_addr((bx_shadow_data_c*)SIM->get_param("hub.pci_conf", xHCI_state), 0);
  sprintf(buffer, "0x%08X", pci_bar_address);
  gtk_entry_set_text(GTK_ENTRY(entry[0]), buffer);
  for (i = 0; i < 8; i++) {
    dword = xhci_read_dword(pci_bar_address + (i * 4));
    sprintf(buffer, "0x%08X", dword);
    gtk_entry_set_text(GTK_ENTRY(entry[i + 1]), buffer);
  }
  offset = xhci_read_dword(pci_bar_address + 0) & 0xFF;

  dword = xhci_read_dword(pci_bar_address + offset + 0x00);
  sprintf(buffer, "0x%08X", dword);
  gtk_entry_set_text(GTK_ENTRY(entry[9]), buffer);
  dword = xhci_read_dword(pci_bar_address + offset + 0x04);
  sprintf(buffer, "0x%08X", dword);
  gtk_entry_set_text(GTK_ENTRY(entry[10]), buffer);
  dword = xhci_read_dword(pci_bar_address + offset + 0x08);
  sprintf(buffer, "0x%08X", dword);
  gtk_entry_set_text(GTK_ENTRY(entry[11]), buffer);
  dword = xhci_read_dword(pci_bar_address + offset + 0x14);
  sprintf(buffer, "0x%08X", dword);
  gtk_entry_set_text(GTK_ENTRY(entry[12]), buffer);
  // we can't read this using DEV_MEM_READ_PHYSICAL since the handler will return zero
  sprintf(buffer, "0x" FMT_ADDRX64, SIM->get_param_num("hub.op_regs.HcCrcr.actual", xHCI_state)->get64());
  gtk_entry_set_text(GTK_ENTRY(entry[13]), buffer);
  Bit64u qword = xhci_read_dword(pci_bar_address + offset + 0x30) |
   ((Bit64u) xhci_read_dword(pci_bar_address + offset + 0x34) << 32);
  sprintf(buffer, "0x" FMT_ADDRX64, qword);
  gtk_entry_set_text(GTK_ENTRY(entry[14]), buffer);
  dword = xhci_read_dword(pci_bar_address + offset + 0x38);
  sprintf(buffer, "0x%08X", dword);
  gtk_entry_set_text(GTK_ENTRY(entry[15]), buffer);

  offset = xhci_read_dword(pci_bar_address + 0x18);
  dword = xhci_read_dword(pci_bar_address + offset + 0);
  sprintf(buffer, "0x%08X", dword);
  gtk_entry_set_text(GTK_ENTRY(entry[16]), buffer);
  // show up to 10 port register sets
  for (i = 0; i < n_ports; i++) {
    dword = xhci_read_dword(pci_bar_address + XHCI_PORT_SET_OFFSET + (i * 16));
    sprintf(buffer, "0x%08X", dword);
    gtk_entry_set_text(GTK_ENTRY(entry[i * 2 + 17]), buffer);
    sprintf(tmpbuf, "port%d.device", i + 1);
    SIM->get_param_enum(tmpbuf, host_param)->dump_param(buffer, COMMON_STR_SIZE, 1);
    gtk_entry_set_text(GTK_ENTRY(entry[i * 2 + 18]), buffer);
  }
  // Show dialog
  gtk_widget_show_all(main_dialog);
  ret = gtk_dialog_run(GTK_DIALOG(main_dialog));
  if (ret == GTK_RESPONSE_OK) {
    usbdlg_set_debug_flags();
  }
  gtk_widget_destroy(main_dialog);
  return ret;
}

int usb_debug_dialog(int type, int param1, int param2)
{
  static bool first_call = true;
  int argc = 1, ret;
  char *argv[2], **argvp, buffer[COMMON_STR_SIZE];

  if (first_call) {
    if (!SIM->is_wx_selected() && !SIM->has_debug_gui()) {
      // you MUST call gtk_init, even with faked arguments, because it inits GDK and Glib
      *buffer = 0;        // gtk flames out if you pass in a NULL -- sheesh
      argv[0] = buffer;   // so I really do have to fake up an "argv" list
      argv[1] = NULL;
      argvp = argv;
      if (gtk_init_check(&argc, &argvp) == false) {
        fprintf(stderr, "gtk init failed, can not access display?\n");
        return -1;
      }
      MakeGTKthreads();
    }
    first_call = false;
  }
  host_param = SIM->get_param(hc_param_str[usb_debug_type]);
  if (usb_debug_type == USB_DEBUG_UHCI) {
    ret = uhci_debug_dialog(type, param1);
    return (ret == GTK_RESPONSE_OK) ? 0 : -1;
  } else if (usb_debug_type == USB_DEBUG_XHCI) {
    ret = xhci_debug_dialog(type, param1);
    return (ret == GTK_RESPONSE_OK) ? 0 : -1;
  } else {
    GtkWidget* error = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK_CANCEL,
        "GTK version of USB debugger dialog not implemented yet");
    gtk_window_set_title(GTK_WINDOW(error), "WARNING");
    ret = gtk_dialog_run(GTK_DIALOG(error));
    gtk_widget_destroy(error);
    return (ret == GTK_RESPONSE_OK) ? 0 : -1;
  }
}

#endif
