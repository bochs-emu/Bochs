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

#if BX_HAVE_GTK_VERSION == 2
#define GTK_DISABLE_DEPRECATED
#endif

#include "usb_debug.h"

#include <gtk/gtk.h>
#include <glib.h>

// multithreading using pure posix threads -- not glib threads
void * EventLp(void *data)
{
  gtk_main();
  return NULL;
}

void MakeGTKthreads()
{
  pthread_t hThread;
  pthread_create(&hThread, NULL, EventLp, NULL);
}

void gtk_entry_set_readonly(GtkWidget *entry)
{
  GValue val = G_VALUE_INIT;
  g_value_init(&val, G_TYPE_INT);
  g_value_set_int(&val, 0);
  g_object_set_property(G_OBJECT(entry), "editable", &val);
}

int usb_debug_dialog(int type, int param1, int param2)
{
  static bool first_call = true;
  int argc = 1, i, ret;
  Bit32u pci_bar_address;
  char *argv[2], **argvp, buffer[128];
  bx_list_c *UHCI_state = NULL;
  const char *chkTxt[6] = {"Reset", "Enable", "Event", "Doorbell", "Start of Frame", "Non-Exist"};
  const char *chkBXPN[6] = {BXPN_USB_DEBUG_RESET, BXPN_USB_DEBUG_ENABLE, BXPN_USB_DEBUG_EVENT,
                            BXPN_USB_DEBUG_DOORBELL, BXPN_USB_DEBUG_START_FRAME, BXPN_USB_DEBUG_NON_EXIST};
  GtkWidget *hbox1, *BAhbox, *ORhbox, *PRhbox, *entry[11], *label[11], *checkbox[6];
  GtkWidget *vbox[3], *ORvbox[2], *PRvbox[2], *DFvbox;
  GtkWidget *ORframe, *PRframe, *DFframe;

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
    UHCI_state = (bx_list_c*)SIM->get_param("usb_uhci", SIM->get_bochs_root());
    if (UHCI_state == NULL)
      return -1;

    GtkWidget *dialog = gtk_dialog_new();
    switch (type) {
      case USB_DEBUG_FRAME:
        gtk_window_set_title(GTK_WINDOW(dialog), "UHCI Debug Dialog: Break Type: Start of Frame");
        break;
      default:
        gtk_window_set_title(GTK_WINDOW(dialog), "UHCI Debug Dialog");
    }
    gtk_window_set_default_size(GTK_WINDOW(dialog), 600, 500);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Continue", GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Quit", GTK_RESPONSE_CANCEL);
    gtk_dialog_set_default_response(GTK_DIALOG (dialog), GTK_RESPONSE_OK);
    hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show(hbox1);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), hbox1, TRUE, TRUE, 2);
    for (i = 0; i < 3; i++) {
      vbox[i] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
      gtk_widget_show(vbox[i]);
      gtk_box_pack_start(GTK_BOX(hbox1), vbox[i], TRUE, TRUE, 2);
    }

    BAhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show(BAhbox);
    gtk_box_pack_start(GTK_BOX(vbox[0]), BAhbox, FALSE, FALSE, 2);
    label[0] = gtk_label_new("UHCI Base IO address");
    gtk_widget_show(label[0]);
    gtk_box_pack_start(GTK_BOX(BAhbox), label[0], TRUE, TRUE, 2);
    entry[0] = gtk_entry_new();
    gtk_entry_set_readonly(entry[0]);
    gtk_widget_show(entry[0]);
    gtk_box_pack_start(GTK_BOX(BAhbox), entry[0], TRUE, TRUE, 2);

    ORframe = gtk_frame_new("Operational Registers");
    gtk_widget_show(ORframe);
    gtk_box_pack_start(GTK_BOX(vbox[0]), ORframe, FALSE, FALSE, 2);
    ORhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show(ORhbox);
    gtk_container_add(GTK_CONTAINER(ORframe), ORhbox);
    ORvbox[0] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_show(ORvbox[0]);
    gtk_box_pack_start(GTK_BOX(ORhbox), ORvbox[0], FALSE, FALSE, 2);
    ORvbox[1] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_show(ORvbox[1]);
    gtk_box_pack_start(GTK_BOX(ORhbox), ORvbox[1], FALSE, FALSE, 2);
    label[1] = gtk_label_new("Command");
    gtk_widget_show(label[1]);
    gtk_box_pack_start(GTK_BOX(ORvbox[0]), label[1], TRUE, TRUE, 2);
    entry[1] = gtk_entry_new();
    gtk_entry_set_readonly(entry[1]);
    gtk_widget_show(entry[1]);
    gtk_box_pack_start(GTK_BOX(ORvbox[1]), entry[1], TRUE, TRUE, 2);
    label[2] = gtk_label_new("Status");
    gtk_widget_show(label[2]);
    gtk_box_pack_start(GTK_BOX(ORvbox[0]), label[2], TRUE, TRUE, 2);
    entry[2] = gtk_entry_new();
    gtk_entry_set_readonly(entry[2]);
    gtk_widget_show(entry[2]);
    gtk_box_pack_start(GTK_BOX(ORvbox[1]), entry[2], TRUE, TRUE, 2);
    label[3] = gtk_label_new("Interrupt Enable");
    gtk_widget_show(label[3]);
    gtk_box_pack_start(GTK_BOX(ORvbox[0]), label[3], TRUE, TRUE, 2);
    entry[3] = gtk_entry_new();
    gtk_entry_set_readonly(entry[3]);
    gtk_widget_show(entry[3]);
    gtk_box_pack_start(GTK_BOX(ORvbox[1]), entry[3], TRUE, TRUE, 2);
    label[4] = gtk_label_new("Frame Number");
    gtk_widget_show(label[4]);
    gtk_box_pack_start(GTK_BOX(ORvbox[0]), label[4], TRUE, TRUE, 2);
    entry[4] = gtk_entry_new();
    gtk_entry_set_readonly(entry[4]);
    gtk_widget_show(entry[4]);
    gtk_box_pack_start(GTK_BOX(ORvbox[1]), entry[4], TRUE, TRUE, 2);
    label[5] = gtk_label_new("Frame Address");
    gtk_widget_show(label[5]);
    gtk_box_pack_start(GTK_BOX(ORvbox[0]), label[5], TRUE, TRUE, 2);
    entry[5] = gtk_entry_new();
    gtk_entry_set_readonly(entry[5]);
    gtk_widget_show(entry[5]);
    gtk_box_pack_start(GTK_BOX(ORvbox[1]), entry[5], TRUE, TRUE, 2);
    label[6] = gtk_label_new("Start of Frame");
    gtk_widget_show(label[6]);
    gtk_box_pack_start(GTK_BOX(ORvbox[0]), label[6], TRUE, TRUE, 2);
    entry[6] = gtk_entry_new();
    gtk_entry_set_readonly(entry[6]);
    gtk_widget_show(entry[6]);
    gtk_box_pack_start(GTK_BOX(ORvbox[1]), entry[6], TRUE, TRUE, 2);

    PRframe = gtk_frame_new("Port Registers");
    gtk_widget_show(PRframe);
    gtk_box_pack_start(GTK_BOX(vbox[0]), PRframe, FALSE, FALSE, 2);
    PRhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show(PRhbox);
    gtk_container_add(GTK_CONTAINER(PRframe), PRhbox);
    PRvbox[0] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_show(PRvbox[0]);
    gtk_box_pack_start(GTK_BOX(PRhbox), PRvbox[0], FALSE, FALSE, 2);
    PRvbox[1] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_show(PRvbox[1]);
    gtk_box_pack_start(GTK_BOX(PRhbox), PRvbox[1], FALSE, FALSE, 2);
    label[7] = gtk_label_new("Port 0");
    gtk_widget_show(label[7]);
    gtk_box_pack_start(GTK_BOX(PRvbox[0]), label[7], TRUE, TRUE, 2);
    entry[7] = gtk_entry_new();
    gtk_entry_set_readonly(entry[7]);
    gtk_widget_show(entry[7]);
    gtk_box_pack_start(GTK_BOX(PRvbox[1]), entry[7], TRUE, TRUE, 2);
    label[8] = gtk_label_new("Emulation Type");
    gtk_widget_show(label[8]);
    gtk_box_pack_start(GTK_BOX(PRvbox[0]), label[8], TRUE, TRUE, 2);
    entry[8] = gtk_entry_new();
    gtk_entry_set_readonly(entry[8]);
    gtk_widget_show(entry[8]);
    gtk_box_pack_start(GTK_BOX(PRvbox[1]), entry[8], TRUE, TRUE, 2);
    label[9] = gtk_label_new("Port 1");
    gtk_widget_show(label[9]);
    gtk_box_pack_start(GTK_BOX(PRvbox[0]), label[9], TRUE, TRUE, 2);
    entry[9] = gtk_entry_new();
    gtk_entry_set_readonly(entry[9]);
    gtk_widget_show(entry[9]);
    gtk_box_pack_start(GTK_BOX(PRvbox[1]), entry[9], TRUE, TRUE, 2);
    label[10] = gtk_label_new("Emulation Type");
    gtk_widget_show(label[10]);
    gtk_box_pack_start(GTK_BOX(PRvbox[0]), label[10], TRUE, TRUE, 2);
    entry[10] = gtk_entry_new();
    gtk_entry_set_readonly(entry[10]);
    gtk_widget_show(entry[10]);
    gtk_box_pack_start(GTK_BOX(PRvbox[1]), entry[10], TRUE, TRUE, 2);

    DFframe = gtk_frame_new("Debug Flags");
    gtk_widget_show(DFframe);
    gtk_box_pack_start(GTK_BOX(vbox[2]), DFframe, FALSE, FALSE, 2);
    DFvbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_show(DFvbox);
    gtk_container_add(GTK_CONTAINER(DFframe), DFvbox);
    for (i = 0; i < 6; i++) {
      checkbox[i] = gtk_check_button_new_with_label(chkTxt[i]);
      if (SIM->get_param_num(chkBXPN[i])->get() > 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[i]), TRUE);
      }
      gtk_widget_show(checkbox[i]);
      gtk_box_pack_start(GTK_BOX(DFvbox), checkbox[i], FALSE, FALSE, 2);
    }
    // Set values
    pci_bar_address = get_pci_bar_addr((bx_shadow_data_c*)SIM->get_param("hub.pci_conf", UHCI_state), 4);
    sprintf(buffer, "0x%04X", pci_bar_address);
    gtk_entry_set_text(GTK_ENTRY(entry[0]), buffer);
    sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 0, 2));
    gtk_entry_set_text(GTK_ENTRY(entry[1]), buffer);
    sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 2, 2));
    gtk_entry_set_text(GTK_ENTRY(entry[2]), buffer);
    sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 4, 2));
    gtk_entry_set_text(GTK_ENTRY(entry[3]), buffer);
    sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 6, 2));
    gtk_entry_set_text(GTK_ENTRY(entry[4]), buffer);
    sprintf(buffer, "0x%08X", usb_io_read(pci_bar_address + 8, 4));
    gtk_entry_set_text(GTK_ENTRY(entry[5]), buffer);
    sprintf(buffer, "0x%02X", usb_io_read(pci_bar_address + 12, 1));
    gtk_entry_set_text(GTK_ENTRY(entry[6]), buffer);
    sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 16, 2));
    gtk_entry_set_text(GTK_ENTRY(entry[7]), buffer);
    SIM->get_param_enum("port1.device", host_param)->dump_param(buffer, 128, 1);
    gtk_entry_set_text(GTK_ENTRY(entry[8]), buffer);
    sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 18, 2));
    gtk_entry_set_text(GTK_ENTRY(entry[9]), buffer);
    SIM->get_param_enum("port2.device", host_param)->dump_param(buffer, 128, 1);
    gtk_entry_set_text(GTK_ENTRY(entry[10]), buffer);
    // Show dialog
    gtk_widget_show(dialog);
    ret = gtk_dialog_run(GTK_DIALOG(dialog));
    if (ret == GTK_RESPONSE_OK) {
      // Update settings
      for (i = 0; i < 6; i++) {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[i]))) {
          SIM->get_param_num(chkBXPN[i])->set(1);
        } else {
          SIM->get_param_num(chkBXPN[i])->set(0);
        }
      }
    }
    gtk_widget_destroy(dialog);
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
