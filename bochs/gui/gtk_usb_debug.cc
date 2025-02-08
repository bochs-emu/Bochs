/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C)      2023  Benjamin David Lunt
//  Copyright (C) 2003-2025  The Bochs Project
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
// hacks for iodev / USB internals
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

const char *chkTxt[7] = {"Reset", "Enable", "Doorbell", "Event", "Data", "Start of Frame", "Non-Exist"};
const char *chkBXPN[7] = {BXPN_USB_DEBUG_RESET, BXPN_USB_DEBUG_ENABLE, BXPN_USB_DEBUG_DOORBELL,
                          BXPN_USB_DEBUG_EVENT, BXPN_USB_DEBUG_DATA,
                          BXPN_USB_DEBUG_START_FRAME, BXPN_USB_DEBUG_NON_EXIST};

int usbdbg_break_type, usbdbg_param0, usbdbg_param1, usbdbg_param2;

GtkWidget *main_dialog, *td_dialog, *trb_dialog, *ctx_dialog;
GtkWidget *uhci_entry[UHCI_REG_COUNT];
GtkWidget *DFchkbox[7];
GtkWidget *apply_button;

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

GtkWidget* usbdlg_create_label(GtkWidget *grid, const char *text, int x, int y)
{
  GtkWidget *label = gtk_label_new(text);
  gtk_grid_attach(GTK_GRID(grid), label, x, y, 1, 1);
  return label;
}

GtkWidget* usbdlg_create_entry_with_label(GtkWidget *grid, const char *text,
                                                  int x, int y)
{
  gtk_grid_attach(GTK_GRID(grid), gtk_label_new(text), x, y, 1, 1);
  GtkWidget *entry = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid), entry, x + 1, y, 1, 1);
  return entry;
}

GtkWidget* usbdlg_create_ro_entry(GtkWidget *grid, int x, int y)
{
  GtkWidget *entry = gtk_entry_new();
  gtk_widget_set_sensitive(entry, 0);
  gtk_grid_attach(GTK_GRID(grid), entry, x, y, 1, 1);
  return entry;
}

GtkWidget* usbdlg_create_ro_entry_with_label(GtkWidget *grid, const char *text,
                                                  int x, int y)
{
  gtk_grid_attach(GTK_GRID(grid), gtk_label_new(text), x, y, 1, 1);
  GtkWidget *entry = gtk_entry_new();
  gtk_widget_set_sensitive(entry, 0);
  gtk_grid_attach(GTK_GRID(grid), entry, x + 1, y, 1, 1);
  return entry;
}

void usbdlg_create_debug_flags(GtkWidget *grid, int x, int y)
{
  GtkWidget *DFframe = gtk_frame_new("Debug Flags");
  gtk_grid_attach(GTK_GRID(grid), DFframe, x, y, 1, 5);
  GtkWidget *DFvbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(DFframe), DFvbox);
  for (int i = 0; i < 7; i++) {
    DFchkbox[i] = gtk_check_button_new_with_label(chkTxt[i]);
    if (SIM->get_param_num(chkBXPN[i])->get() > 0) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(DFchkbox[i]), TRUE);
    }
    gtk_box_pack_start(GTK_BOX(DFvbox), DFchkbox[i], FALSE, FALSE, 2);
  }
}

void usbdlg_set_debug_flags()
{
  for (int i = 0; i < 7; i++) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(DFchkbox[i]))) {
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
    gtk_dialog_new_with_buttons(usb_reg_def->name, GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                GTK_DIALOG_MODAL,
                                g_dgettext("gtk30", "_OK"), GTK_RESPONSE_OK,
                                g_dgettext("gtk30", "_Cancel"), GTK_RESPONSE_CANCEL,
                                NULL);
  gtk_window_set_default_size(GTK_WINDOW(dialog), 250, 250);
  gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
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

// Save UHCI state to controller

int hc_uhci_save()
{
  char buffer[COMMON_STR_SIZE];

  if (u_changed[UHCI_REG_COMMAND]) {
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(uhci_entry[UHCI_REG_COMMAND])));
    usb_io_write(pci_bar_address + 0, strtol(buffer, NULL, 0), 2);
  }
  if (u_changed[UHCI_REG_STATUS]) {
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(uhci_entry[UHCI_REG_STATUS])));
    usb_io_write(pci_bar_address + 2, strtol(buffer, NULL, 0), 2);
  }
  if (u_changed[UHCI_REG_INTERRUPT]) {
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(uhci_entry[UHCI_REG_INTERRUPT])));
    usb_io_write(pci_bar_address + 4, strtol(buffer, NULL, 0), 2);
  }
  if (u_changed[UHCI_REG_FRAME_NUM]) {
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(uhci_entry[UHCI_REG_FRAME_NUM])));
    usb_io_write(pci_bar_address + 6, strtol(buffer, NULL, 0), 2);
  }
  if (u_changed[UHCI_REG_FRAME_ADDR]) {
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(uhci_entry[UHCI_REG_FRAME_ADDR])));
    usb_io_write(pci_bar_address + 8, strtol(buffer, NULL, 0), 4);
  }
  if (u_changed[UHCI_REG_SOF]) {
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(uhci_entry[UHCI_REG_SOF])));
    usb_io_write(pci_bar_address + 12, strtol(buffer, NULL, 0), 1);
  }
  if (u_changed[UHCI_REG_PORT0]) {
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(uhci_entry[UHCI_REG_PORT0])));
    usb_io_write(pci_bar_address + 16, strtol(buffer, NULL, 0), 2);
  }
  if (u_changed[UHCI_REG_PORT1]) {
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(uhci_entry[UHCI_REG_PORT1])));
    usb_io_write(pci_bar_address + 18, strtol(buffer, NULL, 0), 2);
  }

  memset(u_changed, 0, sizeof(u_changed));
  gtk_widget_set_sensitive(apply_button, 0);

  return 0;
}

// Apply button support

static void on_entry_changed(GtkWidget *widget, gpointer data)
{
  *((bool*)data) = 1;
  gtk_widget_set_sensitive(apply_button, 1);
}

static void apply_changes(GtkWidget *widget, gpointer data)
{
  if (usb_debug_type == USB_DEBUG_UHCI) {
    hc_uhci_save();
  } else {
    GtkWidget* error = gtk_message_dialog_new(
      GTK_WINDOW(main_dialog), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
      "Saving changes to controller not implemented yet");
    gtk_window_set_title(GTK_WINDOW(error), "WARNING");
    gtk_dialog_run(GTK_DIALOG(error));
    gtk_widget_destroy(error);
  }
}

void usbdlg_create_apply_button(GtkWidget *grid, int x, int y)
{
  apply_button = gtk_button_new_with_label(g_dgettext("gtk30", "_Apply"));
  gtk_grid_attach(GTK_GRID(grid), apply_button, x, y, 1, 1);
  gtk_widget_set_sensitive(apply_button, 0);
  g_signal_connect(apply_button, "clicked", G_CALLBACK(apply_changes), NULL);
}

// Tree view support
int tree_items;

void cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer,
                    GtkTreeModel *model, GtkTreeIter *iter, gpointer data)
{
  gchar *name;
  gboolean bold;

  gtk_tree_model_get(model, iter, 0, &name, 2, &bold, -1);
  if (bold) {
    g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD, "weight-set", TRUE, NULL);
  } else {
    g_object_set(renderer, "weight", PANGO_WEIGHT_NORMAL, "weight-set", TRUE, NULL);
  }
  g_free(name);
}

void treeview_init(GtkWidget *treeview, char *str)
{
  GtkTreeStore *treestore;
  GtkTreeViewColumn *treecol;
  GtkCellRenderer *renderer;

  treecol = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(treecol, str);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), treecol);
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(treecol, renderer, TRUE);
  gtk_tree_view_column_add_attribute(treecol, renderer, "text", 0); // pull display text from treestore col 0
  gtk_tree_view_column_set_cell_data_func(treecol, renderer, cell_data_func, NULL, NULL);
  gtk_widget_set_can_focus(treeview, FALSE);
  treestore = gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_ULONG, G_TYPE_BOOLEAN);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(treestore));
  g_object_unref(treestore);
  tree_items = 0;
}

void treeview_insert(GtkWidget *treeview, GtkTreeIter *parent, GtkTreeIter *child, char *str, Bit64u param, bool bold)
{
  GtkTreeStore *treestore;
  GtkTreeIter item;

  treestore = (GtkTreeStore*)gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
  memset(&item, 0, sizeof(GtkTreeIter));
  if (treestore != NULL) {
    gtk_tree_store_append(treestore, &item, parent);
    gtk_tree_store_set(treestore, &item, 0, str, 1, param, 2, bold, -1);
    tree_items++;
  }
  if (child != NULL) {
    *child = item;
  }
}

void hc_uhci_do_item(GtkWidget *treeview, Bit32u FrameAddr, Bit32u FrameNum)
{
  struct USB_UHCI_QUEUE_STACK queue_stack;
  Bit32u item, queue_addr = 0;
  struct QUEUE queue;
  struct TD td;
  char str[COMMON_STR_SIZE];
  GtkTreeIter cur, next, *nextp = NULL;
  GtkTreeSelection *selection;
  bool state;

  // get the frame pointer
  DEV_MEM_READ_PHYSICAL(FrameAddr, sizeof(Bit32u), (Bit8u *) &item);
  sprintf(str, "Frame Pointer(%i): 0x%08X", FrameNum, item);
  treeview_init(treeview, str);

  queue_stack.queue_cnt = 0;

  // A catch to make sure we don't do too many
  while (tree_items < 50) {
    if (!USB_UHCI_IS_LINK_VALID(item))  // the the T bit is set, we are done
      break;

    // is it a queue?
    if (USB_UHCI_IS_LINK_QUEUE(item)) {
      // add it to our current list of queues
      if (uhci_add_queue(&queue_stack, item & ~0xF)) {
        // if this queue has been added before, stop.
        break;
      }

      // read in the queue
      DEV_MEM_READ_PHYSICAL(item & ~0xF, sizeof(struct QUEUE), (Bit8u *) &queue);
      sprintf(str, "0x%08X: Queue Head: (0x%08X 0x%08X)", item & ~0xF, queue.horz, queue.vert);
      treeview_insert(treeview, nextp, &next, str, (item & ~0xF) | 1, 0);
      nextp = &next;

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

    // we processed another td within this queue line
    state = false; // clear the state
    DEV_MEM_READ_PHYSICAL(item & ~0xF, sizeof(struct TD), (Bit8u *) &td);
    const bool depthbreadth = (td.dword0 & 0x0004) ? true : false;     // 1 = depth first, 0 = breadth first
    sprintf(str, "0x%08X: TD: (0x%08X)", item & ~0xF, td.dword0);
    if ((item & ~0xF) == (Bit32u) usbdbg_param1)
      state = true;
    treeview_insert(treeview, nextp, &cur, str, (item & ~0xF), state);
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    if ((item & ~0xF) == (Bit32u) usbdbg_param1)
      gtk_tree_selection_select_iter(selection, &cur);
    item = td.dword0;
    if (queue_addr != 0) {
      // if breadth first or last in the element list, move on to next queue item
      if (!depthbreadth || !USB_UHCI_IS_LINK_VALID(item)) {
        item = queue.horz;
        queue_addr = 0;
      }
    }
  }
}

static void uhci_queue_dialog(Bit32u addr)
{
  int ret;
  struct QUEUE queue;
  char buffer[COMMON_STR_SIZE];
  GtkWidget *mainVbox, *button[2], *label[2], *entry[2], *checkbox[4];
  GtkWidget *Hframe, *Vframe, *grid[2];

  DEV_MEM_READ_PHYSICAL(addr, sizeof(struct QUEUE), (Bit8u*)&queue);
  GtkWidget *dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog), "Queue");
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(main_dialog));
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_window_set_default_size(GTK_WINDOW(dialog), 200, 250);
  button[0] = gtk_dialog_add_button(GTK_DIALOG(dialog), g_dgettext("gtk30", "_Save"), GTK_RESPONSE_OK);
  button[1] = gtk_dialog_add_button(GTK_DIALOG(dialog), g_dgettext("gtk30", "_Cancel"), GTK_RESPONSE_CANCEL);
  gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_window_set_focus(GTK_WINDOW(dialog), button[0]);
  mainVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), mainVbox, TRUE, TRUE, 2);
  Hframe = gtk_frame_new("Horizontal Pointer");
  gtk_box_pack_start(GTK_BOX(mainVbox), Hframe, FALSE, FALSE, 2);
  grid[0] = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(Hframe), grid[0]);
  label[0] = gtk_label_new("Pointer");
  gtk_grid_attach(GTK_GRID(grid[0]), label[0], 0, 0, 1, 1);
  entry[0] = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid[0]), entry[0], 1, 0, 1, 1);
  checkbox[0] = gtk_check_button_new_with_label("Queue");
  gtk_grid_attach(GTK_GRID(grid[0]), checkbox[0], 1, 1, 1, 1);
  checkbox[1] = gtk_check_button_new_with_label("Terminate");
  gtk_grid_attach(GTK_GRID(grid[0]), checkbox[1], 1, 2, 1, 1);
  Vframe = gtk_frame_new("Vertical Pointer");
  gtk_box_pack_start(GTK_BOX(mainVbox), Vframe, FALSE, FALSE, 2);
  grid[1] = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(Vframe), grid[1]);
  label[1] = gtk_label_new("Pointer");
  gtk_grid_attach(GTK_GRID(grid[1]), label[1], 0, 0, 1, 1);
  entry[1] = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid[1]), entry[1], 1, 0, 1, 1);
  checkbox[2] = gtk_check_button_new_with_label("Queue");
  gtk_grid_attach(GTK_GRID(grid[1]), checkbox[2], 1, 1, 1, 1);
  checkbox[3] = gtk_check_button_new_with_label("Terminate");
  gtk_grid_attach(GTK_GRID(grid[1]), checkbox[3], 1, 2, 1, 1);
  // Set values
  sprintf(buffer, "0x%04X", queue.horz & ~0xF);
  gtk_entry_set_text(GTK_ENTRY(entry[0]), buffer);
  if (queue.horz & 2) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[0]), TRUE);
  }
  if (queue.horz & 1) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[1]), TRUE);
  }
  sprintf(buffer, "0x%04X", queue.vert & ~0xF);
  gtk_entry_set_text(GTK_ENTRY(entry[1]), buffer);
  if (queue.vert & 2) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[2]), TRUE);
  }
  if (queue.vert & 1) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[3]), TRUE);
  }
  // Show dialog
  gtk_widget_show_all(dialog);
  ret = gtk_dialog_run(GTK_DIALOG(dialog));
  if (ret == GTK_RESPONSE_OK) {
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(entry[0])));
    queue.horz = strtol(buffer, NULL, 0);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[0]))) {
      queue.horz |= 2;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[1]))) {
      queue.horz |= 1;
    }
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(entry[1])));
    queue.vert = strtol(buffer, NULL, 0);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[2]))) {
      queue.vert |= 2;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[3]))) {
      queue.vert |= 1;
    }
    DEV_MEM_WRITE_PHYSICAL(addr, sizeof(struct QUEUE), (Bit8u*)&queue);
  }
  gtk_widget_destroy(dialog);
}

static void depth_breadth_sel(GtkWidget *widget, gpointer data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
    gtk_button_set_label(GTK_BUTTON(widget), "(Depth first) Vf");
  } else {
    gtk_button_set_label(GTK_BUTTON(widget), "(Breadth first) Vf");
  }
}

static void dump_buffer(GtkWidget *widget, gpointer data)
{
  DUMP_PARAMS *params = (DUMP_PARAMS*)data;
  GtkTextBuffer *txtbuf;
  GtkWidget *text_view, *scrolled_window;

  if (params->size > 512) {
    params->size = 512;
  }
  GtkWidget *dialog =
    gtk_dialog_new_with_buttons(params->title, GTK_WINDOW(td_dialog), GTK_DIALOG_MODAL,
                                g_dgettext("gtk30", "_OK"), GTK_RESPONSE_OK,
                                NULL);
  gtk_window_set_default_size(GTK_WINDOW(dialog), 420, 400);
  gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  txtbuf = gtk_text_buffer_new(NULL);
  text_view = gtk_text_view_new_with_buffer(txtbuf);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
  gtk_widget_set_can_focus(text_view, FALSE);
  scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
  gtk_container_set_border_width(GTK_CONTAINER (scrolled_window), 5);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                     scrolled_window, TRUE, TRUE, 2);
  if (params->size > 0) {
    // 3 bytes per char printed, ~18 chars at the first of each line, 20 of chars, 2 chars at the end of each line, 2 at last line
    int str_size = (params->size * 3) + ((18 + 2) * ((params->size + 15) / 16)) + 2;
    // make sure we are at least COMMON_STR_SIZE so we can use it to set the title
    if (str_size < COMMON_STR_SIZE)
      str_size = COMMON_STR_SIZE;
    char *str = new char[str_size];
    char temp_str[COMMON_STR_SIZE];
    if (params->big)
      sprintf(str, "%s--Address: 0x" FMT_ADDRX64 ": size = %i\n", params->title, params->address, params->size);
    else
      sprintf(str, "%s--Address: 0x%08X: size = %i\n", params->title, (Bit32u) params->address, params->size);
    gtk_window_set_title(GTK_WINDOW(dialog), str);

    // read in the buffer
    Bit8u *buffer = new Bit8u[params->size];
    DEV_MEM_READ_PHYSICAL(params->address, params->size, buffer);

    // dump it
    int j = 0;
    strcpy(str, "");
    for (int i = 0; i < params->size; i++) {
      if (j == 0) {
        if (params->big)
          sprintf(temp_str, "0x" FMT_PHY_ADDRX64 ": ", params->address);
        else
          sprintf(temp_str, "0x%08X: ", (Bit32u) params->address);
        strcat(str, temp_str);
        params->address += 16;
      }
      j++;
      if ((j == 8) && ((i + 1) != params->size)) {
        sprintf(temp_str, "%02X-", buffer[i]);
      } else {
        sprintf(temp_str, "%02X ", buffer[i]);
      }
      strcat(str, temp_str);
      if (j == 16) {
        strcat(str, "\n");
        j = 0;
      }
    }
    strcat(str, "\n");
    gtk_text_buffer_set_text(txtbuf, str, strlen(str));

    delete [] buffer;
    delete [] str;
  } else {
    gtk_text_buffer_set_text(txtbuf, "Nothing to do...", 16);
  }
  // Show dialog
  gtk_widget_show_all(dialog);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

static void uhci_td_dialog(Bit32u addr)
{
  int ret, value;
  struct TD td;
  char buffer[COMMON_STR_SIZE];
  GtkWidget *mainVbox, *entry[7], *checkbox[16], *grid[4], *label[7];
  GtkWidget *Shbox, *Svbox, *Bhbox, *LPframe, *Sframe[2], *PHframe;
  GtkWidget *combo, *button[3];
  DUMP_PARAMS dump_params;

  DEV_MEM_READ_PHYSICAL(addr, sizeof(struct TD), (Bit8u*)&td);
  td_dialog = gtk_dialog_new();
  if (usbdbg_param2 & USB_LPARAM_FLAG_BEFORE) {
    gtk_window_set_title(GTK_WINDOW(td_dialog), "Transfer Descriptor: *Before*");
  } else if (usbdbg_param2 & USB_LPARAM_FLAG_AFTER) {
    gtk_window_set_title(GTK_WINDOW(td_dialog), "Transfer Descriptor: *After*");
  } else {
    gtk_window_set_title(GTK_WINDOW(td_dialog), "Transfer Descriptor");
  }
  gtk_window_set_transient_for(GTK_WINDOW(td_dialog), GTK_WINDOW(main_dialog));
  gtk_window_set_modal(GTK_WINDOW(td_dialog), TRUE);
  gtk_window_set_default_size(GTK_WINDOW(td_dialog), 200, 250);
  button[0] = gtk_dialog_add_button(GTK_DIALOG(td_dialog), g_dgettext("gtk30", "_Save"), GTK_RESPONSE_OK);
  button[1] = gtk_dialog_add_button(GTK_DIALOG(td_dialog), g_dgettext("gtk30", "_Cancel"), GTK_RESPONSE_CANCEL);
  gtk_dialog_set_default_response(GTK_DIALOG(td_dialog), GTK_RESPONSE_OK);
  gtk_window_set_focus(GTK_WINDOW(td_dialog), button[0]);
  mainVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(td_dialog))), mainVbox, TRUE, TRUE, 2);
  LPframe = gtk_frame_new("Link Pointer");
  gtk_box_pack_start(GTK_BOX(mainVbox), LPframe, FALSE, FALSE, 2);
  grid[0] = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(LPframe), grid[0]);
  label[0] = gtk_label_new("Link Pointer");
  gtk_grid_attach(GTK_GRID(grid[0]), label[0], 0, 0, 1, 1);
  entry[0] = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid[0]), entry[0], 1, 0, 1, 1);
  checkbox[0] = gtk_check_button_new_with_label("(Breadth first) Vf");
  g_signal_connect(checkbox[0], "clicked", G_CALLBACK(depth_breadth_sel), NULL);
  gtk_grid_attach(GTK_GRID(grid[0]), checkbox[0], 1, 1, 1, 1);
  checkbox[1] = gtk_check_button_new_with_label("Queue");
  gtk_grid_attach(GTK_GRID(grid[0]), checkbox[1], 1, 2, 1, 1);
  checkbox[2] = gtk_check_button_new_with_label("Terminate");
  gtk_grid_attach(GTK_GRID(grid[0]), checkbox[2], 1, 3, 1, 1);
  Sframe[0] = gtk_frame_new("Status");
  gtk_box_pack_start(GTK_BOX(mainVbox), Sframe[0], FALSE, FALSE, 2);
  Svbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(Sframe[0]), Svbox);
  Shbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(Svbox), Shbox, FALSE, FALSE, 2);
  label[1] = gtk_label_new("Actual Len");
  gtk_box_pack_start(GTK_BOX(Shbox), label[1], TRUE, TRUE, 2);
  entry[1] = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(Shbox), entry[1], TRUE, TRUE, 2);
  Sframe[1] = gtk_frame_new("Status");
  gtk_box_pack_start(GTK_BOX(Svbox), Sframe[1], FALSE, FALSE, 2);
  grid[1] = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(Sframe[1]), grid[1]);
  checkbox[3] = gtk_check_button_new_with_label("Active");
  if ((usbdbg_param2 & USB_LPARAM_FLAG_AFTER) == 0) {
    gtk_widget_set_sensitive(checkbox[3], 0);
  }
  gtk_grid_attach(GTK_GRID(grid[1]), checkbox[3], 0, 0, 1, 1);
  checkbox[4] = gtk_check_button_new_with_label("Stalled");
  gtk_grid_attach(GTK_GRID(grid[1]), checkbox[4], 0, 1, 1, 1);
  checkbox[5] = gtk_check_button_new_with_label("Data Buffer Error");
  gtk_grid_attach(GTK_GRID(grid[1]), checkbox[5], 0, 2, 1, 1);
  checkbox[6] = gtk_check_button_new_with_label("Babble Detect");
  gtk_grid_attach(GTK_GRID(grid[1]), checkbox[6], 0, 3, 1, 1);
  checkbox[7] = gtk_check_button_new_with_label("NAK Received");
  gtk_grid_attach(GTK_GRID(grid[1]), checkbox[7], 1, 0, 1, 1);
  checkbox[8] = gtk_check_button_new_with_label("CRC / Timeout");
  gtk_grid_attach(GTK_GRID(grid[1]), checkbox[8], 1, 1, 1, 1);
  checkbox[9] = gtk_check_button_new_with_label("Bitstuff Error");
  gtk_grid_attach(GTK_GRID(grid[1]), checkbox[9], 1, 2, 1, 1);
  checkbox[10] = gtk_check_button_new_with_label("Reserved");
  gtk_grid_attach(GTK_GRID(grid[1]), checkbox[10], 1, 3, 1, 1);
  grid[2] = gtk_grid_new();
  gtk_box_pack_start(GTK_BOX(Svbox), grid[2], FALSE, FALSE, 2);
  checkbox[11] = gtk_check_button_new_with_label("Interrupt on Comp.");
  gtk_grid_attach(GTK_GRID(grid[2]), checkbox[11], 0, 0, 1, 1);
  checkbox[12] = gtk_check_button_new_with_label("Isosynchronus");
  gtk_grid_attach(GTK_GRID(grid[2]), checkbox[12], 0, 1, 1, 1);
  checkbox[13] = gtk_check_button_new_with_label("Low Speed");
  gtk_grid_attach(GTK_GRID(grid[2]), checkbox[13], 2, 0, 1, 1);
  checkbox[14] = gtk_check_button_new_with_label("Short Packet");
  gtk_grid_attach(GTK_GRID(grid[2]), checkbox[14], 2, 1, 1, 1);
  label[2] = gtk_label_new("C_ERR");
  gtk_grid_attach(GTK_GRID(grid[2]), label[2], 1, 2, 1, 1);
  entry[2] = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid[2]), entry[2], 2, 2, 1, 1);
  PHframe = gtk_frame_new("Packet Header");
  gtk_box_pack_start(GTK_BOX(mainVbox), PHframe, FALSE, FALSE, 2);
  grid[3] = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(PHframe), grid[3]);
  label[3] = gtk_label_new("PID");
  gtk_grid_attach(GTK_GRID(grid[3]), label[3], 1, 0, 1, 1);
  combo = gtk_combo_box_text_new();
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), NULL, "IN (0x69)");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), NULL, "OUT (0xE1)");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), NULL, "SETUP (0x2D)");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), NULL, "ERROR");
  gtk_grid_attach(GTK_GRID(grid[3]), combo, 2, 0, 1, 1);
  label[4] = gtk_label_new("Device Address");
  gtk_grid_attach(GTK_GRID(grid[3]), label[4], 1, 1, 1, 1);
  entry[3] = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid[3]), entry[3], 2, 1, 1, 1);
  label[5] = gtk_label_new("End Point");
  gtk_grid_attach(GTK_GRID(grid[3]), label[5], 1, 2, 1, 1);
  entry[4] = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid[3]), entry[4], 2, 2, 1, 1);
  label[6] = gtk_label_new("Max Length");
  gtk_grid_attach(GTK_GRID(grid[3]), label[6], 1, 3, 1, 1);
  entry[5] = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid[3]), entry[5], 2, 3, 1, 1);
  checkbox[15] = gtk_check_button_new_with_label("Toggle Bit");
  gtk_grid_attach(GTK_GRID(grid[3]), checkbox[15], 0, 2, 1, 1);
  Bhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(mainVbox), Bhbox, FALSE, FALSE, 2);
  label[7] = gtk_label_new("Buffer");
  gtk_box_pack_start(GTK_BOX(Bhbox), label[7], FALSE, FALSE, 2);
  entry[6] = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(Bhbox), entry[6], FALSE, FALSE, 2);
  button[2] = gtk_button_new_with_label(">");
  gtk_box_pack_start(GTK_BOX(Bhbox), button[2], FALSE, FALSE, 2);
  // Set values
  sprintf(buffer, "0x%04X", td.dword0 & ~0xF);
  gtk_entry_set_text(GTK_ENTRY(entry[0]), buffer);
  if (td.dword0 & 4) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[0]), TRUE);
  }
  if (td.dword0 & 2) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[1]), TRUE);
  }
  if (td.dword0 & 1) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[2]), TRUE);
  }
  sprintf(buffer, "%i", td.dword1 & 0x7FF);
  gtk_entry_set_text(GTK_ENTRY(entry[1]), buffer);
  if (td.dword1 & (1 << 23)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[3]), TRUE);
  }
  if (td.dword1 & (1 << 22)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[4]), TRUE);
  }
  if (td.dword1 & (1 << 21)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[5]), TRUE);
  }
  if (td.dword1 & (1 << 20)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[6]), TRUE);
  }
  if (td.dword1 & (1 << 19)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[7]), TRUE);
  }
  if (td.dword1 & (1 << 18)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[8]), TRUE);
  }
  if (td.dword1 & (1 << 17)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[9]), TRUE);
  }
  if (td.dword1 & (1 << 16)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[10]), TRUE);
  }
  if (td.dword1 & (1 << 24)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[11]), TRUE);
  }
  if (td.dword1 & (1 << 25)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[12]), TRUE);
  }
  if (td.dword1 & (1 << 26)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[13]), TRUE);
  }
  if (td.dword1 & (1 << 29)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[14]), TRUE);
  }
  sprintf(buffer, "%i", (td.dword1 >> 27) & 3);
  gtk_entry_set_text(GTK_ENTRY(entry[2]), buffer);
  switch (td.dword2 & 0xff) {
    case 0x69:
      gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
      break;
    case 0xE1:
      gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 1);
      break;
    case 0x2D:
      gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 2);
      break;
    default:
      gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 3);
  }
  sprintf(buffer, "%i", (td.dword2 >> 8) & 0x7F);
  gtk_entry_set_text(GTK_ENTRY(entry[3]), buffer);
  sprintf(buffer, "%i", (td.dword2 >> 15) & 0x0F);
  gtk_entry_set_text(GTK_ENTRY(entry[4]), buffer);
  sprintf(buffer, "%i", (td.dword2 >> 21) & 0x7FF);
  gtk_entry_set_text(GTK_ENTRY(entry[5]), buffer);
  if (td.dword2 & (1 << 19)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox[15]), TRUE);
  }
  sprintf(buffer, "0x%08X", td.dword3);
  gtk_entry_set_text(GTK_ENTRY(entry[6]), buffer);
  strcpy(dump_params.title, "UHCI: Transfer Descriptor Buffer");
  dump_params.address = td.dword3;
  if (usbdbg_param2 & USB_LPARAM_FLAG_AFTER) {
    dump_params.size = (td.dword1 & 0x7FF) + 1;
  } else {
    dump_params.size = ((td.dword2 >> 21) + 1) & 0x7FF;
  }
  dump_params.big = 0;
  g_signal_connect(button[2], "clicked", G_CALLBACK(dump_buffer), &dump_params);
  // Show dialog
  gtk_widget_show_all(td_dialog);
  ret = gtk_dialog_run(GTK_DIALOG(td_dialog));
  if (ret == GTK_RESPONSE_OK) {
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(entry[0])));
    td.dword0 = strtol(buffer, NULL, 0);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[0]))) {
      td.dword0 |= 4;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[1]))) {
      td.dword0 |= 2;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[2]))) {
      td.dword0 |= 1;
    }
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(entry[1])));
    td.dword1 = strtol(buffer, NULL, 0) & 0x7FF;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[3]))) {
      td.dword1 |= (1 << 23);
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[4]))) {
      td.dword1 |= (1 << 22);
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[5]))) {
      td.dword1 |= (1 << 21);
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[6]))) {
      td.dword1 |= (1 << 20);
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[7]))) {
      td.dword1 |= (1 << 19);
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[8]))) {
      td.dword1 |= (1 << 18);
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[9]))) {
      td.dword1 |= (1 << 17);
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[10]))) {
      td.dword1 |= (1 << 16);
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[11]))) {
      td.dword1 |= (1 << 24);
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[12]))) {
      td.dword1 |= (1 << 25);
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[13]))) {
      td.dword1 |= (1 << 26);
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox[14]))) {
      td.dword1 |= (1 << 29);
    }
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(entry[2])));
    td.dword1 |= ((strtol(buffer, NULL, 0) & 3) << 27);
    value = gtk_combo_box_get_active(GTK_COMBO_BOX(combo));
    switch (value) {
      case 0:
        td.dword2 = 0x69;
        break;
      case 1:
        td.dword2 = 0xE1;
        break;
      case 2:
        td.dword2 = 0x2D;
        break;
      default:
        td.dword2 = 0;
    }
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(entry[3])));
    td.dword2 |= ((strtol(buffer, NULL, 0) & 0x7F) << 8);
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(entry[4])));
    td.dword2 |= ((strtol(buffer, NULL, 0) & 0x0F) << 15);
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(entry[5])));
    td.dword2 |= ((strtol(buffer, NULL, 0) & 0x7FF) << 21);
    strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(entry[6])));
    td.dword3 = strtol(buffer, NULL, 0);
    DEV_MEM_WRITE_PHYSICAL(addr, sizeof(struct TD), (Bit8u*)&td);
  }
  gtk_widget_destroy(td_dialog);
}

static void uhci_display_td(GtkWidget *widget, gpointer data)
{
  GtkTreeSelection *selection;
  GtkTreeModel     *model;
  GtkTreeIter       iter;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(data));
  if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
    gulong value;

    gtk_tree_model_get(model, &iter, 1, &value, -1);
    Bit32u address = (Bit32u)(value & ~0xF);
    if (value & 1) {
      uhci_queue_dialog(address);
    } else {
      uhci_td_dialog(address);
    }
  } else {
    GtkWidget* error = gtk_message_dialog_new(
      GTK_WINDOW(main_dialog), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
    "No TD selected");
    gtk_window_set_title(GTK_WINDOW(error), "WARNING");
    gtk_dialog_run(GTK_DIALOG(error));
    gtk_widget_destroy(error);
  }
}

// UHCI main dialog

int uhci_debug_dialog(int type, int param1)
{
  bx_list_c *UHCI_state = NULL;
  int i, ret;
  Bit32u frame_addr, frame_num;
  char buffer[COMMON_STR_SIZE];
  GtkWidget *mainVbox, *mainGrid, *ro_entry[4];
  GtkWidget *ORframe, *ORgrid, *PRframe, *PRgrid;
  GtkWidget *button[8], *FNlabel, *treeview;
  usb_reg_t uhci_reg_def[5] = {
    {"Command Register", NULL, 4, attribs_u_command},
    {"Status Register", NULL, 4, attribs_u_status},
    {"Interrupt Enable Register", NULL, 4, attribs_u_interrupt},
    {"Port 0 Register", NULL, 4, attribs_u_ports},
    {"Port 1 Register", NULL, 4, attribs_u_ports}
  };

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
  gtk_window_set_keep_above(GTK_WINDOW(main_dialog), TRUE);
  button[0] = gtk_dialog_add_button(GTK_DIALOG(main_dialog), "Continue", GTK_RESPONSE_OK);
  button[1] = gtk_dialog_add_button(GTK_DIALOG(main_dialog), "Quit", GTK_RESPONSE_CANCEL);
  gtk_dialog_set_default_response(GTK_DIALOG(main_dialog), GTK_RESPONSE_OK);
  gtk_window_set_focus(GTK_WINDOW(main_dialog), button[0]);
  mainVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(main_dialog))), mainVbox, TRUE, FALSE, 5);
  mainGrid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(mainGrid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(mainGrid), 5);
  gtk_container_add(GTK_CONTAINER(mainVbox), mainGrid);

  ro_entry[0] = usbdlg_create_ro_entry_with_label(mainGrid, "UHCI at Base IO address", 0, 0);
  ORframe = gtk_frame_new("Operational Registers");
  gtk_grid_attach(GTK_GRID(mainGrid), ORframe, 0, 1, 3, 7);
  ORgrid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(ORgrid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(ORgrid), 5);
  gtk_container_add(GTK_CONTAINER(ORframe), ORgrid);
  gtk_container_set_border_width(GTK_CONTAINER(ORgrid), 5);
  uhci_entry[UHCI_REG_COMMAND] = usbdlg_create_entry_with_label(ORgrid, "Command", 0, 0);
  button[2] = gtk_button_new_with_label("<>");
  uhci_reg_def[0].entry = uhci_entry[UHCI_REG_COMMAND];
  g_signal_connect(button[2], "clicked", G_CALLBACK(usb_regview_dialog), &uhci_reg_def[0]);
  gtk_grid_attach(GTK_GRID(ORgrid), button[2], 2, 0, 1, 1);
  uhci_entry[UHCI_REG_STATUS] = usbdlg_create_entry_with_label(ORgrid, "Status", 0, 1);
  button[3] = gtk_button_new_with_label("<>");
  uhci_reg_def[1].entry = uhci_entry[UHCI_REG_STATUS];
  g_signal_connect(button[3], "clicked", G_CALLBACK(usb_regview_dialog), &uhci_reg_def[1]);
  gtk_grid_attach(GTK_GRID(ORgrid), button[3], 2, 1, 1, 1);
  uhci_entry[UHCI_REG_INTERRUPT] = usbdlg_create_entry_with_label(ORgrid, "Interrupt Enable", 0, 2);
  button[4] = gtk_button_new_with_label("<>");
  uhci_reg_def[2].entry = uhci_entry[UHCI_REG_INTERRUPT];
  g_signal_connect(button[4], "clicked", G_CALLBACK(usb_regview_dialog), &uhci_reg_def[2]);
  gtk_grid_attach(GTK_GRID(ORgrid), button[4], 2, 2, 1, 1);
  uhci_entry[UHCI_REG_FRAME_NUM] = usbdlg_create_entry_with_label(ORgrid, "Frame Number", 0, 3);
  uhci_entry[UHCI_REG_FRAME_ADDR] = usbdlg_create_entry_with_label(ORgrid, "Frame Address", 0, 4);
  uhci_entry[UHCI_REG_SOF] = usbdlg_create_entry_with_label(ORgrid, "Start of Frame", 0, 5);
  PRframe = gtk_frame_new("Port Registers");
  gtk_grid_attach(GTK_GRID(mainGrid), PRframe, 0, 8, 3, 5);
  PRgrid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(PRgrid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(PRgrid), 5);
  gtk_container_add(GTK_CONTAINER(PRframe), PRgrid);
  gtk_container_set_border_width(GTK_CONTAINER(PRgrid), 5);
  for (i = 0; i < 2; i++) {
    sprintf(buffer, "Port %d", i);
    uhci_entry[UHCI_REG_PORT0 + i] = usbdlg_create_entry_with_label(PRgrid, buffer, 0, i * 2);
    ro_entry[i + 1] = usbdlg_create_ro_entry_with_label(PRgrid, "Emulation Type", 0, i * 2 + 1);
    button[i + 5] = gtk_button_new_with_label("<>");
    uhci_reg_def[i + 3].entry = uhci_entry[UHCI_REG_PORT0 + i];
    g_signal_connect(button[i + 5], "clicked", G_CALLBACK(usb_regview_dialog), &uhci_reg_def[i + 3]);
    gtk_grid_attach(GTK_GRID(PRgrid), button[i + 5], 2, i * 2, 1, 1);
  }
  FNlabel = usbdlg_create_label(mainGrid, "Frame at Address", 4, 7);
  ro_entry[3] = usbdlg_create_ro_entry(mainGrid, 5, 7);
  button[7] = gtk_button_new_with_label("View Item");
  gtk_widget_set_sensitive(button[7], 0);
  gtk_grid_attach(GTK_GRID(mainGrid), button[7], 6, 7, 1, 1);
  treeview = gtk_tree_view_new();
  gtk_grid_attach(GTK_GRID(mainGrid), treeview, 3, 8, 5, 8);
  usbdlg_create_apply_button(mainGrid, 1, 13);
  usbdlg_create_debug_flags(mainGrid, 6, 1);
  // Set values
  pci_bar_address = get_pci_bar_addr((bx_shadow_data_c*)SIM->get_param("hub.pci_conf", UHCI_state), 4);
  sprintf(buffer, "0x%04X", pci_bar_address);
  gtk_entry_set_text(GTK_ENTRY(ro_entry[0]), buffer);
  sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 0, 2));
  gtk_entry_set_text(GTK_ENTRY(uhci_entry[UHCI_REG_COMMAND]), buffer);
  g_signal_connect(GTK_EDITABLE(uhci_entry[UHCI_REG_COMMAND]), "changed",
                   G_CALLBACK(on_entry_changed), &u_changed[UHCI_REG_COMMAND]);
  sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 2, 2));
  gtk_entry_set_text(GTK_ENTRY(uhci_entry[UHCI_REG_STATUS]), buffer);
  g_signal_connect(GTK_EDITABLE(uhci_entry[UHCI_REG_STATUS]), "changed",
                   G_CALLBACK(on_entry_changed), &u_changed[UHCI_REG_STATUS]);
  sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 4, 2));
  gtk_entry_set_text(GTK_ENTRY(uhci_entry[UHCI_REG_INTERRUPT]), buffer);
  g_signal_connect(GTK_EDITABLE(uhci_entry[UHCI_REG_INTERRUPT]), "changed",
                   G_CALLBACK(on_entry_changed), &u_changed[UHCI_REG_INTERRUPT]);
  frame_num = usb_io_read(pci_bar_address + 6, 2);
  sprintf(buffer, "0x%04X", frame_num);
  gtk_entry_set_text(GTK_ENTRY(uhci_entry[UHCI_REG_FRAME_NUM]), buffer);
  g_signal_connect(GTK_EDITABLE(uhci_entry[UHCI_REG_FRAME_NUM]), "changed",
                   G_CALLBACK(on_entry_changed), &u_changed[UHCI_REG_FRAME_NUM]);
  frame_addr = usb_io_read(pci_bar_address + 8, 4);
  sprintf(buffer, "0x%08X", frame_addr);
  gtk_entry_set_text(GTK_ENTRY(uhci_entry[UHCI_REG_FRAME_ADDR]), buffer);
  g_signal_connect(GTK_EDITABLE(uhci_entry[UHCI_REG_FRAME_ADDR]), "changed",
                   G_CALLBACK(on_entry_changed), &u_changed[UHCI_REG_FRAME_ADDR]);
  sprintf(buffer, "0x%02X", usb_io_read(pci_bar_address + 12, 1));
  gtk_entry_set_text(GTK_ENTRY(uhci_entry[UHCI_REG_SOF]), buffer);
  g_signal_connect(GTK_EDITABLE(uhci_entry[UHCI_REG_SOF]), "changed",
                   G_CALLBACK(on_entry_changed), &u_changed[UHCI_REG_SOF]);
  sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 16, 2));
  gtk_entry_set_text(GTK_ENTRY(uhci_entry[UHCI_REG_PORT0]), buffer);
  g_signal_connect(GTK_EDITABLE(uhci_entry[UHCI_REG_PORT0]), "changed",
                   G_CALLBACK(on_entry_changed), &u_changed[UHCI_REG_PORT0]);
  SIM->get_param_enum("port1.device", host_param)->dump_param(buffer, COMMON_STR_SIZE, 1);
  gtk_entry_set_text(GTK_ENTRY(ro_entry[1]), buffer);
  sprintf(buffer, "0x%04X", usb_io_read(pci_bar_address + 18, 2));
  gtk_entry_set_text(GTK_ENTRY(uhci_entry[UHCI_REG_PORT1]), buffer);
  g_signal_connect(GTK_EDITABLE(uhci_entry[UHCI_REG_PORT1]), "changed",
                   G_CALLBACK(on_entry_changed), &u_changed[UHCI_REG_PORT1]);
  SIM->get_param_enum("port2.device", host_param)->dump_param(buffer, COMMON_STR_SIZE, 1);
  gtk_entry_set_text(GTK_ENTRY(ro_entry[2]), buffer);

  frame_addr += (frame_num * sizeof(Bit32u));
  sprintf(buffer, "0x%08X", frame_addr);
  gtk_entry_set_text(GTK_ENTRY(ro_entry[3]), buffer);

  bool valid = 0;
  switch (type) {
    // The DoTransfer() function was called
    case USB_DEBUG_COMMAND:
    // The start of a frame timer was triggered
    case USB_DEBUG_FRAME:
      gtk_label_set_text(GTK_LABEL(FNlabel), "SOF Frame Address");
      if (frame_addr != 0x00000000) {
        hc_uhci_do_item(treeview, frame_addr, frame_num);
        gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview));
        g_signal_connect(button[7], "clicked", G_CALLBACK(uhci_display_td), treeview);
        gtk_widget_set_sensitive(button[7], 1);
        valid = 1;
      }
      break;

    // an event triggered. We ignore these in the uhci
    //case USB_DEBUG_EVENT:
    //  break;

    // first byte (word, dword, qword) of first non-existant port was written to
    case USB_DEBUG_NONEXIST:
    // port reset (non-root reset)
    case USB_DEBUG_RESET:
    // enable changed
    case USB_DEBUG_ENABLE:
      gtk_label_set_text(GTK_LABEL(FNlabel), "None");
      gtk_entry_set_text(GTK_ENTRY(ro_entry[3]), "None");
      break;
  }

  if (!valid) {
    gtk_widget_set_sensitive(treeview, 0);
    gtk_grid_attach(GTK_GRID(mainGrid), gtk_label_new("This trigger does not populate the tree view"), 4, 16, 1, 1);
  } else {
    gtk_grid_attach(GTK_GRID(mainGrid), gtk_label_new("Tree view populated"), 4, 16, 1, 1);
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

// xHCI

enum {
  ID_TRB_DATA_PTR,
  ID_TRB_INT_TARGET,
  ID_TRB_TD_SIZE,
  ID_TRB_TRANS_LEN,
  ID_TRB_TYPE,
  ID_TRB_BEI,
  ID_TRB_IDT,
  ID_TRB_IOC,
  ID_TRB_CH,
  ID_TRB_NS,
  ID_TRB_ISP,
  ID_TRB_ENT,
  ID_TRB_C,
  ID_TRB_WVALUE,
  ID_TRB_BREQUEST,
  ID_TRB_BREQUESTTYPE,
  ID_TRB_WLENGTH,
  ID_TRB_WINDEX,
  ID_TRB_TRT,
  ID_TRB_DIR,
  ID_TRB_TC,
  ID_TRB_SLOT_TYPE,
  ID_TRB_SLOT_ID,
  ID_TRB_BSR,
  ID_TRB_DECONFIG,
  ID_TRB_EP_ID,
  ID_TRB_TSP,
  ID_TRB_SP,
  ID_TRB_STREAMID,
  ID_TRB_VF_ID,
  ID_TRB_BELT,
  ID_TRB_DEV_SPEED,
  ID_TRB_SCT,
  ID_TRB_DCS,
  ID_TRB_HDR_HI,
  ID_TRB_FTYPE,
  ID_TRB_COMP_CODE,
  ID_TRB_ED,
  ID_TRB_COMP_LPARAM,
  ID_TRB_NOT_TYPE,
  TRB_N_ITEMS
};

enum {
  ID_CONTEXT_NEXT,
  ID_CONTEXT_PREV,
  ID_CONTEXT_DROP,
  ID_CONTEXT_DROP_B,
  ID_CONTEXT_ADD,
  ID_CONTEXT_ADD_B,
  ID_CONTEXT_ALT_SETTING,
  ID_CONTEXT_INTFACE_NUM,
  ID_CONTEXT_CONFIG_VALUE,
  ID_CONTEXT_OF_STR,
  ID_CONTEXT_STREAM_CONTEXT,

  ID_CONTEXT_ENTRIES,
  ID_CONTEXT_HUB,
  ID_CONTEXT_MTT,
  ID_CONTEXT_SPEED,
  ID_CONTEXT_ROUTE_STRING,
  ID_CONTEXT_NUM_PORTS,
  ID_CONTEXT_RH_PORT_NUM,
  ID_CONTEXT_MAX_EXIT_LAT,
  ID_CONTEXT_INT_TARGET,
  ID_CONTEXT_TTT,
  ID_CONTEXT_TT_PORT_NUM,
  ID_CONTEXT_TT_HUB_SLOT_ID,
  ID_CONTEXT_SLOT_STATE,
  ID_CONTEXT_DEV_ADDRESS,
  ID_CONTEXT_RSVDO_SLOT_0,
  ID_CONTEXT_RSVDO_SLOT_1,
  ID_CONTEXT_RSVDO_SLOT_2,
  ID_CONTEXT_RSVDO_SLOT_3,
  ID_CONTEXT_RSVDO_SLOT_4,
  ID_CONTEXT_RSVDO_SLOT_5,
  ID_CONTEXT_RSVDO_SLOT_6,
  ID_CONTEXT_RSVDO_SLOT_7,
  ID_CONTEXT_RSVDO_SLOT_8,
  ID_CONTEXT_RSVDO_SLOT_9,
  ID_CONTEXT_RSVDO_SLOT_10,
  ID_CONTEXT_RSVDO_SLOT_11,
  ID_CONTEXT_SPEED_STR,
  ID_CONTEXT_SLOT_STATE_STR,

  ID_CONTEXT_MAX_ESIT_HI,
  ID_CONTEXT_INTERVAL,
  ID_CONTEXT_LSA,
  ID_CONTEXT_MAX_PSTREAMS,
  ID_CONTEXT_MULT,
  ID_CONTEXT_EP_STATE,
  ID_CONTEXT_MAX_PACKET_SIZE,
  ID_CONTEXT_MAX_BURST_SIZE,
  ID_CONTEXT_HID,
  ID_CONTEXT_EP_TYPE,
  ID_CONTEXT_CERR,
  ID_CONTEXT_TR_DEQUEUE_PTR,
  ID_CONTEXT_DCS,
  ID_CONTEXT_MAX_ESIT_LO,
  ID_CONTEXT_AVERAGE_LEN,
  ID_CONTEXT_RSVDO_EP_0,
  ID_CONTEXT_RSVDO_EP_1,
  ID_CONTEXT_RSVDO_EP_2,
  ID_CONTEXT_RSVDO_EP_3,
  ID_CONTEXT_RSVDO_EP_4,
  ID_CONTEXT_RSVDO_EP_5,
  ID_CONTEXT_RSVDO_EP_6,
  ID_CONTEXT_RSVDO_EP_7,
  ID_CONTEXT_RSVDO_EP_8,
  ID_CONTEXT_RSVDO_EP_9,
  ID_CONTEXT_RSVDO_EP_10,
  ID_CONTEXT_EP_STATE_STR,
  ID_CONTEXT_EP_TYPE_STR,
  ID_CONTEXT_MAXPS_STR,
  CTX_N_ITEMS
};

enum {
  ID_SCONTEXT_NEXT,
  ID_SCONTEXT_PREV,
  ID_SCONTEXT_OF_STR,
  ID_STR_CONTEXT_DQPTR,
  ID_STR_CONTEXT_SCT,
  ID_STR_CONTEXT_DCS,
  ID_STR_CONTEXT_STOPPED,
  ID_STR_CONTEXT_RSVDO_0,
  ID_STR_CONTEXT_RSVDO_1,
  ID_CONTEXT_SCT_STR,
  SCTX_N_ITEMS
};

bx_list_c *xHCI_state = NULL;

void hc_xhci_do_ring(GtkWidget *treeview, const char *ring_str, Bit64u RingPtr, Bit64u dequeue_ptr)
{
  char str[COMMON_STR_SIZE];
  int  trb_count = 0; // count of TRB's processed
  Bit64u address = RingPtr;
  struct TRB trb;
  bool state;
  Bit8u type;

  sprintf(str, "%s Ring: 0x" FMT_ADDRX64, ring_str, address);
  treeview_init(treeview, str);

  do {
    state = false; // clear the state
    DEV_MEM_READ_PHYSICAL(address, sizeof(struct TRB), (Bit8u *) &trb);
    type = TRB_GET_TYPE(trb.command);
    if (type <= 47)
      sprintf(str, "0x" FMT_ADDRX64 " %08X 0x%08X (%i) (%s)", trb.parameter, trb.status, trb.command, trb.command & 1, trb_types[type].name);
    else
      sprintf(str, "0x" FMT_ADDRX64 " %08X 0x%08X (%i) (Vendor Specific)", trb.parameter, trb.status, trb.command, trb.command & 1);
    if (address == dequeue_ptr) {
      strcat(str, " <--- dq_pointer");
      state = true;
    }
    treeview_insert(treeview, NULL, NULL, str, address, state);
    if (type == LINK) {
      address = trb.parameter & (Bit64u) ~0xF;
    } else
      address += sizeof(struct TRB);

    if (++trb_count > MAX_TRBS_ALLOWED)  // safety catch
      break;
  } while (address != RingPtr);
}

void hc_xhci_do_event_ring(GtkWidget *treeview, const char *ring_str, int interrupter)
{
  char str[COMMON_STR_SIZE];
  char pname[BX_PATHNAME_LEN];
  int  trb_count = 0; // count of TRB's processed
  Bit64u address;
  Bit32u size;
  struct TRB trb;
  GtkTreeIter segment, *parentp = NULL;
  bool state = false;
  Bit8u type;

  sprintf(str, "%s Ring: Interrupter: %i", ring_str, interrupter);
  treeview_init(treeview, str);

  for (unsigned i = 0; i < (1 << MAX_SEG_TBL_SZ_EXP); i++) {
    sprintf(pname, "hub.ring_members.event_rings.ring%d.entries.entry%d.addr", interrupter, i);
    address = SIM->get_param_num(pname, xHCI_state)->get();
    sprintf(pname, "hub.ring_members.event_rings.ring%d.entries.entry%d.size", interrupter, i);
    size = SIM->get_param_num(pname, xHCI_state)->get();
    sprintf(str, "Event Ring Segment %i (0x" FMT_ADDRX64 "), size %i", i, address, size);
    treeview_insert(treeview, parentp, &segment, str, 0, 0);
    for (unsigned j = 0; j < size; j++) {
      state = 0; // clear the state
      DEV_MEM_READ_PHYSICAL(address, sizeof(struct TRB), (Bit8u *) &trb);
      type = TRB_GET_TYPE(trb.command);
      if (type <= 47)
        sprintf(str, "0x" FMT_ADDRX64 " %08X 0x%08X (%i) (%s)", trb.parameter, trb.status, trb.command, trb.command & 1, trb_types[type].name);
      else
        sprintf(str, "0x" FMT_ADDRX64 " %08X 0x%08X (%i) (Vendor Specific)", trb.parameter, trb.status, trb.command, trb.command & 1);
      sprintf(pname, "hub.ring_members.event_rings.ring%d.cur_trb", interrupter);
      if (address == (Bit64u)SIM->get_param_num(pname, xHCI_state)->get()) {
        strcat(str, " <--- eq_pointer");
        state = true;
      }
      treeview_insert(treeview, &segment, NULL, str, address, state);

      if (++trb_count > MAX_TRBS_ALLOWED)  // safety catch
        break;

      address += sizeof(struct TRB);
    }
    if (trb_count > MAX_TRBS_ALLOWED)  // safety catch
      break;
  }
}

void xhci_message_dialog(GtkWindow *parent, const char *msg)
{
  GtkWidget *error = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL,
    GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "%s", msg);
  gtk_window_set_title(GTK_WINDOW(error), "WARNING");
  gtk_dialog_run(GTK_DIALOG(error));
  gtk_widget_destroy(error);
}

GtkWidget *apply_button_3;

static void xhci_str_context_apply(GtkWidget *widget, gpointer data)
{
  GtkWidget **SCTXitem = (GtkWidget**)data;
  char str[COMMON_STR_SIZE];
  Bit32u *p;

  // String Context
  p = (Bit32u *) &xhci_str_context[xhci_str_current_context * 32];
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(SCTXitem[ID_STR_CONTEXT_DQPTR])));
  p[0]  = strtol(str, NULL, 0) & ~BX_CONST64(0xF);
  p[1]  = (Bit64u) strtol(str, NULL, 0) >> 32;
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(SCTXitem[ID_STR_CONTEXT_SCT])));
  p[0] |= (strtol(str, NULL, 0) & 0x7) << 1;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(SCTXitem[ID_STR_CONTEXT_DCS]))) {
    p[0] |= (1<<0);
  }
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(SCTXitem[ID_STR_CONTEXT_STOPPED])));
  p[1]  = (strtol(str, NULL, 0) & 0x00FFFFFF) << 0;
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(SCTXitem[ID_STR_CONTEXT_RSVDO_0])));
  p[1] |= (strtol(str, NULL, 0) & 0xFF) << 24;
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(SCTXitem[ID_STR_CONTEXT_RSVDO_1])));
  p[2]  = strtol(str, NULL, 0);

  xhci_str_context_changed = 0;
  gtk_widget_set_sensitive(apply_button_3, 0);
}

bool xhci_str_context_change_dlg(GtkWindow *parent)
{
  GtkWidget *ask = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL,
    GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", "Save context changes ?");
  int ret = gtk_dialog_run(GTK_DIALOG(ask));
  gtk_widget_destroy(ask);
  return (ret == GTK_RESPONSE_YES);
}

static void xhci_str_context_select(GtkWidget *widget, gpointer data)
{
  GtkWidget **SCTXitem = (GtkWidget**)data;
  char str[COMMON_STR_SIZE];
  Bit32u *p;

  if (widget == SCTXitem[ID_SCONTEXT_PREV]) {
    if (xhci_str_current_context > 1) {
      if (xhci_str_context_changed) {
        if (xhci_str_context_change_dlg(GTK_WINDOW(gtk_widget_get_toplevel(widget)))) {
          xhci_str_context_apply(widget, data);
        }
      }
      xhci_str_current_context--;
    } else {
      return;
    }
  } else if (widget == SCTXitem[ID_SCONTEXT_NEXT]) {
    if (xhci_str_current_context < 31) {
      if (xhci_str_context_changed) {
        if (xhci_str_context_change_dlg(GTK_WINDOW(gtk_widget_get_toplevel(widget)))) {
          xhci_str_context_apply(widget, data);
        }
      }
      xhci_str_current_context++;
    } else {
      return;
    }
  }

  // display the context
  sprintf(str, "Context %i of %i", xhci_str_current_context, xhci_max_streams - 1);
  if (xhci_str_current_context == 0)
    strcat(str, "(Reserved)");
  gtk_entry_set_text(GTK_ENTRY(SCTXitem[ID_SCONTEXT_OF_STR]), str);

  // String Context
  p = (Bit32u *) &xhci_str_context[xhci_str_current_context * 32];
  sprintf(str, "0x" FMT_ADDRX64, ((Bit64u) p[1] << 32) | (Bit64u)(p[0] & ~BX_CONST64(0xF)));
  gtk_entry_set_text(GTK_ENTRY(SCTXitem[ID_STR_CONTEXT_DQPTR]), str);
  sprintf(str, "%i", (p[0] & (0x7 << 1)) >> 1);
  gtk_entry_set_text(GTK_ENTRY(SCTXitem[ID_STR_CONTEXT_SCT]), str);
  if (p[0] & (1<<0)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(SCTXitem[ID_STR_CONTEXT_DCS]), TRUE);
  }
  sprintf(str, "0x%08X", p[2] & 0x00FFFFFF);
  gtk_entry_set_text(GTK_ENTRY(SCTXitem[ID_STR_CONTEXT_STOPPED]), str);
  sprintf(str, "0x%04X", (p[2] & 0xFF000000) >> 24);
  gtk_entry_set_text(GTK_ENTRY(SCTXitem[ID_STR_CONTEXT_RSVDO_0]), str);
  sprintf(str, "0x%08X", p[3]);
  gtk_entry_set_text(GTK_ENTRY(SCTXitem[ID_STR_CONTEXT_RSVDO_1]), str);

  xhci_str_context_changed = 0;
  gtk_widget_set_sensitive(apply_button_3, 0);
}

static void st_context_string_changed(GtkWidget *widget, gpointer data)
{
  GtkWidget **SCTXitem = (GtkWidget**)data;
  char str[COMMON_STR_SIZE];
  int i;

  strcpy(str, gtk_entry_get_text(GTK_ENTRY(widget)));
  if (widget == SCTXitem[ID_STR_CONTEXT_SCT]) {
    i = strtol(str, NULL, 0);
    if (i > 7) i = 8;
    gtk_entry_set_text(GTK_ENTRY(SCTXitem[ID_CONTEXT_SCT_STR]), string_sct_str[i]);
  }
}

static void st_context_entry_changed(GtkWidget *widget, gpointer data)
{
  GtkWidget **SCTXitem = (GtkWidget**)data;

  xhci_str_context_changed = 1;
  gtk_widget_set_sensitive(apply_button_3, 1);
  if (widget == SCTXitem[ID_STR_CONTEXT_SCT]) {
    st_context_string_changed(widget, data);
  }
}

static void xhci_string_context_dialog(GtkWidget *widget, gpointer data)
{
  GtkWidget **CTXitem = (GtkWidget**)data;
  GtkWidget *SCTXitem[SCTX_N_ITEMS], *mainVbox, *grid;
  GtkWidget *SCframe, *SCgrid;
  char str[COMMON_STR_SIZE];
  int ret;

  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_TR_DEQUEUE_PTR])));
  xhci_str_context_address = strtol(str, NULL, 0) & ~BX_CONST64(0x0F);
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAX_PSTREAMS])));
  xhci_max_streams = strtol(str, NULL, 0);
  if (xhci_max_streams > MAX_PSA_SIZE)
    xhci_max_streams = MAX_PSA_SIZE;
  if (xhci_max_streams > 0)
    xhci_max_streams = (1 << (xhci_max_streams + 1));

  GtkWidget *dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog), "String Context");
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(ctx_dialog));
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_window_set_default_size(GTK_WINDOW(dialog), 200, 200);
  gtk_dialog_add_button(GTK_DIALOG(dialog), g_dgettext("gtk30", "_OK"), GTK_RESPONSE_OK);
  gtk_dialog_add_button(GTK_DIALOG(dialog), g_dgettext("gtk30", "_Cancel"), GTK_RESPONSE_CANCEL);
  gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  mainVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), mainVbox, TRUE, TRUE, 2);
  grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
  gtk_box_pack_start(GTK_BOX(mainVbox), grid, TRUE, TRUE, 2);

  apply_button_3 = gtk_button_new_with_label(g_dgettext("gtk30", "_Apply"));
  g_signal_connect(apply_button_3, "clicked", G_CALLBACK(xhci_str_context_apply), &SCTXitem);
  gtk_grid_attach(GTK_GRID(grid), apply_button_3, 0, 0, 1, 1);
  SCTXitem[ID_SCONTEXT_PREV] = gtk_button_new_with_label("<<");
  g_signal_connect(SCTXitem[ID_SCONTEXT_PREV], "clicked", G_CALLBACK(xhci_str_context_select), &SCTXitem);
  gtk_grid_attach(GTK_GRID(grid), SCTXitem[ID_SCONTEXT_PREV], 1, 0, 1, 1);
  SCTXitem[ID_SCONTEXT_OF_STR] = usbdlg_create_ro_entry(grid, 2, 0);
  SCTXitem[ID_SCONTEXT_NEXT] = gtk_button_new_with_label(">>");
  g_signal_connect(SCTXitem[ID_SCONTEXT_NEXT], "clicked", G_CALLBACK(xhci_str_context_select), &SCTXitem);
  gtk_grid_attach(GTK_GRID(grid), SCTXitem[ID_CONTEXT_NEXT], 3, 0, 1, 1);

  SCframe = gtk_frame_new("Stream Context");
  gtk_grid_attach(GTK_GRID(grid), SCframe, 0, 2, 3, 26);
  SCgrid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(SCgrid), 2);
  gtk_grid_set_column_spacing(GTK_GRID(SCgrid), 5);
  gtk_container_add(GTK_CONTAINER(SCframe), SCgrid);

  SCTXitem[ID_STR_CONTEXT_DQPTR] = usbdlg_create_entry_with_label(SCgrid, "TR Dequeue Pointer", 0, 0);
  g_signal_connect(GTK_EDITABLE(SCTXitem[ID_STR_CONTEXT_DQPTR]), "changed",
                   G_CALLBACK(st_context_entry_changed), &SCTXitem);
  SCTXitem[ID_STR_CONTEXT_SCT] = usbdlg_create_entry_with_label(SCgrid, "SCT", 0, 1);
  g_signal_connect(GTK_EDITABLE(SCTXitem[ID_STR_CONTEXT_SCT]), "changed",
                   G_CALLBACK(st_context_entry_changed), &SCTXitem);
  SCTXitem[ID_CONTEXT_SCT_STR] = usbdlg_create_ro_entry(SCgrid, 2, 1);
  SCTXitem[ID_STR_CONTEXT_DCS] = gtk_check_button_new_with_label("DCS");
  g_signal_connect(GTK_TOGGLE_BUTTON(SCTXitem[ID_STR_CONTEXT_DCS]), "toggled",
                   G_CALLBACK(st_context_entry_changed), &SCTXitem);
  gtk_grid_attach(GTK_GRID(SCgrid), SCTXitem[ID_STR_CONTEXT_DCS], 1, 2, 1, 1);
  SCTXitem[ID_STR_CONTEXT_STOPPED] = usbdlg_create_entry_with_label(SCgrid, "Stopped EDTLA", 0, 3);
  g_signal_connect(GTK_EDITABLE(SCTXitem[ID_STR_CONTEXT_STOPPED]), "changed",
                   G_CALLBACK(st_context_entry_changed), &SCTXitem);
  SCTXitem[ID_STR_CONTEXT_RSVDO_0] = usbdlg_create_entry_with_label(SCgrid, "RsvdO", 0, 4);
  g_signal_connect(GTK_EDITABLE(SCTXitem[ID_STR_CONTEXT_RSVDO_0]), "changed",
                   G_CALLBACK(st_context_entry_changed), &SCTXitem);
  SCTXitem[ID_STR_CONTEXT_RSVDO_1] = usbdlg_create_entry_with_label(SCgrid, "RsvdO", 0, 5);
  g_signal_connect(GTK_EDITABLE(SCTXitem[ID_STR_CONTEXT_RSVDO_1]), "changed",
                   G_CALLBACK(st_context_entry_changed), &SCTXitem);

  xhci_str_current_context = 0;
  xhci_str_context = new Bit8u[CONTEXT_SIZE + (32 * CONTEXT_SIZE)];
  DEV_MEM_READ_PHYSICAL(xhci_str_context_address, MAX_PSA_SIZE_NUM * 32, xhci_str_context);

  // String Context
  xhci_str_context_select(NULL, &SCTXitem);

  xhci_str_context_changed = 0;
  gtk_widget_set_sensitive(apply_button_3, 0);
  gtk_window_set_focus(GTK_WINDOW(dialog), SCTXitem[ID_STR_CONTEXT_DQPTR]);

  // Show dialog
  gtk_widget_show_all(dialog);
  ret = gtk_dialog_run(GTK_DIALOG(dialog));
  if (ret == GTK_RESPONSE_OK) {
    if (xhci_str_context_changed) {
      if (xhci_str_context_change_dlg(GTK_WINDOW(dialog))) {
        xhci_str_context_apply(apply_button_3, &SCTXitem);
      }
    }
  }
  delete [] xhci_str_context;
  gtk_widget_destroy(dialog);
}

static void xhci_context_flags_dialog(GtkWidget *widget, gpointer data)
{
  int i, j;
  usb_reg_t *xhci_ctx_flags_def = (usb_reg_t*)data;
  struct S_ATTRIBUTES *attribs = (struct S_ATTRIBUTES *)xhci_ctx_flags_def->attr;
  if (!strcmp(xhci_ctx_flags_def->name, "Drop Flags")) {
    for (i=0,j=31; i<32; i++,j--) {
      attribs[i].attrb = (BX_CONST64(1)<<i);
      attribs[i].mask = (BX_CONST64(1)<<i);
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
  } else {
    for (i=0,j=31; i<32; i++,j--) {
      attribs[i].attrb = (BX_CONST64(1)<<i);
      attribs[i].mask = (BX_CONST64(1)<<i);
      attribs[i].index = j;
      sprintf(attribs[i].str, "A%i", i);
      attribs[i].groups[0] = -1;
    }
    attribs[32].attrb = 0;
    attribs[32].mask = -1;
    attribs[32].index = -1;
    strcpy(attribs[32].str, "");
    attribs[32].groups[0] = -1;
  }
  usb_regview_dialog(widget, xhci_ctx_flags_def);
}

GtkWidget *apply_button_2;

static void xhci_ep_context_apply(GtkWidget *widget, gpointer data)
{
  GtkWidget **CTXitem = (GtkWidget**)data;
  char str[COMMON_STR_SIZE];
  Bit32u *p;
  int i;

  // EP Context
  p = (Bit32u *) &xhci_context[CONTEXT_SIZE + (xhci_current_ep_context * CONTEXT_SIZE)];
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAX_ESIT_HI])));
  p[0]  = (strtol(str, NULL, 0) & 0xFF) << 24;
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_INTERVAL])));
  p[0] |= (strtol(str, NULL, 0) & 0xFF) << 16;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(CTXitem[ID_CONTEXT_LSA]))) {
    p[0] |= (1<<15);
  }
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAX_PSTREAMS])));
  p[0] |= (strtol(str, NULL, 0) & 0x1F) << 10;
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MULT])));
  p[0] |= (strtol(str, NULL, 0) & 0x3) << 8;
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_EP_STATE])));
  p[0] |= (strtol(str, NULL, 0) & 0x7) << 0;
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAX_PACKET_SIZE])));
  p[1]  = (strtol(str, NULL, 0) & 0xFFFF) << 16;
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAX_BURST_SIZE])));
  p[1] |= (strtol(str, NULL, 0) & 0xFF) << 8;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(CTXitem[ID_CONTEXT_HID]))) {
    p[1] |= (1<<7);
  }
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_EP_TYPE])));
  p[1] |= (strtol(str, NULL, 0) & 0x7) << 3;
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_CERR])));
  p[1] |= (strtol(str, NULL, 0) & 0x3) << 1;
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_TR_DEQUEUE_PTR])));
  p[2]  = strtol(str, NULL, 0) & ~BX_CONST64(0xF);
  p[3]  = (Bit64u) strtol(str, NULL, 0) >> 32;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(CTXitem[ID_CONTEXT_DCS]))) {
    p[2] |= (1<<0);
  }
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAX_ESIT_LO])));
  p[4]  = (strtol(str, NULL, 0) & 0xFFFF) << 16;
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_AVERAGE_LEN])));
  p[4] |= (strtol(str, NULL, 0) & 0xFFFF) << 0;

  for (i=0; i<3; i++) {
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_RSVDO_EP_0 + i])));
    p[5+i] = strtol(str, NULL, 0);
  }
#if (CONTEXT_SIZE == 64)
  for (i=3; i<11; i++) {
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_RSVDO_EP_0 + i])));
    p[5+i] = strtol(str, NULL, 0);
  }
#endif

  xhci_context_changed = 0;
  gtk_widget_set_sensitive(apply_button_2, 0);
}

bool xhci_ep_context_change_dlg(GtkWindow *parent)
{
  GtkWidget *ask = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL,
    GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", "Save EP context changes ?");
  int ret = gtk_dialog_run(GTK_DIALOG(ask));
  gtk_widget_destroy(ask);
  return (ret == GTK_RESPONSE_YES);
}

static void xhci_ep_context_select(GtkWidget *widget, gpointer data)
{
  GtkWidget **CTXitem = (GtkWidget**)data;
  char str[COMMON_STR_SIZE];
  Bit32u *p;
  int i;

  if (widget == CTXitem[ID_CONTEXT_PREV]) {
    if (xhci_current_ep_context > 1) {
      if (xhci_context_changed) {
        if (xhci_ep_context_change_dlg(GTK_WINDOW(gtk_widget_get_toplevel(widget)))) {
          xhci_ep_context_apply(widget, data);
        }
      }
      xhci_current_ep_context--;
    } else {
      return;
    }
  } else if (widget == CTXitem[ID_CONTEXT_NEXT]) {
    if (xhci_current_ep_context < 31) {
      if (xhci_context_changed) {
        if (xhci_ep_context_change_dlg(GTK_WINDOW(gtk_widget_get_toplevel(widget)))) {
          xhci_ep_context_apply(widget, data);
        }
      }
      xhci_current_ep_context++;
    } else {
      return;
    }
  }

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
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_OF_STR]), str);

  // EP Context
  p = (Bit32u *) &xhci_context[CONTEXT_SIZE + (xhci_current_ep_context * CONTEXT_SIZE)];
  sprintf(str, "%i", (p[0] & (0xFF << 24)) >> 24);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAX_ESIT_HI]), str);
  sprintf(str, "%i", (p[0] & (0xFF << 16)) >> 16);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_INTERVAL]), str);
  if (p[0] & (1<<15)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CTXitem[ID_CONTEXT_LSA]), TRUE);
  }
  sprintf(str, "%i", (p[0] & (0x1F << 10)) >> 10);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAX_PSTREAMS]), str);
  sprintf(str, "%i", (p[0] & (0x3 << 8)) >> 8);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MULT]), str);
  sprintf(str, "%i", (p[0] & (0x7 << 0)) >> 0);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_EP_STATE]), str);
  sprintf(str, "%i", (p[1] & (0xFFFF << 16)) >> 16);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAX_PACKET_SIZE]), str);
  sprintf(str, "%i", (p[1] & (0xFF << 8)) >> 8);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAX_BURST_SIZE]), str);
  if (p[1] & (1<<7)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CTXitem[ID_CONTEXT_HID]), TRUE);
  }
  sprintf(str, "%i", (p[1] & (0x7 << 3)) >> 3);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_EP_TYPE]), str);
  sprintf(str, "%i", (p[1] & (0x3 << 1)) >> 1);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_CERR]), str);
  sprintf(str, "0x" FMT_ADDRX64, ((Bit64u) p[3] << 32) | (Bit64u)(p[2] & ~BX_CONST64(0xF)));
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_TR_DEQUEUE_PTR]), str);
  if (p[2] & (1<<0)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CTXitem[ID_CONTEXT_HID]), TRUE);
  }
  sprintf(str, "%i", (p[4] & (0xFFFF << 16)) >> 16);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAX_ESIT_LO]), str);
  sprintf(str, "%i", (p[4] & (0xFFFF << 0)) >> 0);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_AVERAGE_LEN]), str);

  gtk_widget_set_sensitive(CTXitem[ID_CONTEXT_STREAM_CONTEXT],
                           (((p[0] & (0x1F << 10)) >> 10) > 0));

  for (i=0; i<3; i++) {
    sprintf(str, "0x%08X", p[5+i]);
    gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_RSVDO_EP_0 + i]), str);
  }
#if (CONTEXT_SIZE == 64)
  for (i=3; i<11; i++) {
    sprintf(str, "0x%08X", p[5+i]);
    gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_RSVDO_EP_0 + i]), str);
    gtk_widget_set_sensitive(CTXitem[ID_CONTEXT_RSVDO_EP_0 + i], 1);
  }
#endif

  xhci_context_changed = 0;
  gtk_widget_set_sensitive(apply_button_2, 0);
}

static void context_string_changed(GtkWidget *widget, gpointer data)
{
  GtkWidget **CTXitem = (GtkWidget**)data;
  char str[COMMON_STR_SIZE];
  int i;

  strcpy(str, gtk_entry_get_text(GTK_ENTRY(widget)));
  if (widget == CTXitem[ID_CONTEXT_SPEED]) {
    i = strtol(str, NULL, 0);
    if (i > 15) i = 16;
    gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_SPEED_STR]), slot_speed_str[i]);
  } else if (widget == CTXitem[ID_CONTEXT_SLOT_STATE]) {
    i = strtol(str, NULL, 0);
    if (i > 3) i = 4;
    gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_SLOT_STATE_STR]), slot_type_str[i]);
  } else if (widget == CTXitem[ID_CONTEXT_EP_STATE]) {
    i = strtol(str, NULL, 0);
    if (i > 7) i = 7;
    gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_EP_STATE_STR]), ep_state_str[i]);
  } else if (widget == CTXitem[ID_CONTEXT_EP_TYPE]) {
    i = strtol(str, NULL, 0);
    if (i > 7) i = 7;
    gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_EP_TYPE_STR]), ep_type_str[i]);
  } else if (widget == CTXitem[ID_CONTEXT_MAX_PSTREAMS]) {
    i = strtol(str, NULL, 0);
    if (i == 0)
      gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAXPS_STR]), "None");
    else if (i <= 15) {
      sprintf(str, "%i", (1 << (i + 1)));
      gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAXPS_STR]), str);
    } else
      gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAXPS_STR]), "Error");
    gtk_widget_set_sensitive(GTK_WIDGET(CTXitem[ID_CONTEXT_STREAM_CONTEXT]), (i > 0) && (i <=15));
  }
}

static void ep_context_entry_changed(GtkWidget *widget, gpointer data)
{
  GtkWidget **CTXitem = (GtkWidget**)data;

  xhci_context_changed = 1;
  gtk_widget_set_sensitive(apply_button_2, 1);
  if ((widget == CTXitem[ID_CONTEXT_MAX_PSTREAMS]) ||
      (widget == CTXitem[ID_CONTEXT_EP_STATE]) ||
      (widget == CTXitem[ID_CONTEXT_EP_TYPE])) {
    context_string_changed(widget, data);
  }
}

static void xhci_context_dialog(GtkWidget *widget, gpointer data)
{
  GtkWidget *ICitem = (GtkWidget*)data;
  GtkWidget *CTXitem[CTX_N_ITEMS], *mainVbox, *grid, *button[2];
  GtkWidget *SLframe, *EPframe, *SLgrid, *EPgrid;
  struct S_ATTRIBUTES *attribs;
  usb_reg_t xhci_ctx_flags_def[2] = {
    {"Drop Flags", NULL, 4, NULL},
    {"Add Flags", NULL, 4, NULL}
  };
  char str[COMMON_STR_SIZE];
  Bit32u *p;
  int i, ret;

  attribs = (struct S_ATTRIBUTES *) calloc(sizeof(struct S_ATTRIBUTES), 33);
  xhci_ctx_flags_def[0].attr = attribs;
  xhci_ctx_flags_def[1].attr = attribs;
  strcpy(str, gtk_entry_get_text(GTK_ENTRY(ICitem)));
  xhci_context_address  = strtol(str, NULL, 0) & ~BX_CONST64(0x0F);
  xhci_current_ep_context = 1;
  xhci_context = new Bit8u[CONTEXT_SIZE + (32 * CONTEXT_SIZE)];
  DEV_MEM_READ_PHYSICAL(xhci_context_address, CONTEXT_SIZE + (32 * CONTEXT_SIZE), xhci_context);
  p = (Bit32u *) &xhci_context[0];

  ctx_dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(ctx_dialog), "Context");
  gtk_window_set_transient_for(GTK_WINDOW(ctx_dialog), GTK_WINDOW(trb_dialog));
  gtk_window_set_modal(GTK_WINDOW(ctx_dialog), TRUE);
  gtk_window_set_default_size(GTK_WINDOW(ctx_dialog), 200, 200);
  button[0] = gtk_dialog_add_button(GTK_DIALOG(ctx_dialog), g_dgettext("gtk30", "_OK"), GTK_RESPONSE_OK);
  button[1] = gtk_dialog_add_button(GTK_DIALOG(ctx_dialog), g_dgettext("gtk30", "_Cancel"), GTK_RESPONSE_CANCEL);
  gtk_dialog_set_default_response(GTK_DIALOG(ctx_dialog), GTK_RESPONSE_OK);
  gtk_window_set_focus(GTK_WINDOW(ctx_dialog), button[0]);
  mainVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(ctx_dialog))), mainVbox, TRUE, TRUE, 2);
  grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
  gtk_box_pack_start(GTK_BOX(mainVbox), grid, TRUE, TRUE, 2);

  CTXitem[ID_CONTEXT_DROP] = usbdlg_create_entry_with_label(grid, "Drop Context Flags", 0, 0);
  CTXitem[ID_CONTEXT_DROP_B] = gtk_button_new_with_label("<>");
  xhci_ctx_flags_def[0].entry = CTXitem[ID_CONTEXT_DROP];
  g_signal_connect(CTXitem[ID_CONTEXT_DROP_B], "clicked", G_CALLBACK(xhci_context_flags_dialog), &xhci_ctx_flags_def[0]);
  gtk_grid_attach(GTK_GRID(grid), CTXitem[ID_CONTEXT_DROP_B], 2, 0, 1, 1);
  CTXitem[ID_CONTEXT_ADD] = usbdlg_create_entry_with_label(grid, "Add Context Flags", 0, 1);
  CTXitem[ID_CONTEXT_ADD_B] = gtk_button_new_with_label("<>");
  xhci_ctx_flags_def[1].entry = CTXitem[ID_CONTEXT_ADD];
  g_signal_connect(CTXitem[ID_CONTEXT_ADD_B], "clicked", G_CALLBACK(xhci_context_flags_dialog), &xhci_ctx_flags_def[1]);
  gtk_grid_attach(GTK_GRID(grid), CTXitem[ID_CONTEXT_ADD_B], 2, 1, 1, 1);
  CTXitem[ID_CONTEXT_ALT_SETTING] = usbdlg_create_entry_with_label(grid, "Alt Setting", 3, 0);
  CTXitem[ID_CONTEXT_INTFACE_NUM] = usbdlg_create_entry_with_label(grid, "Interface Num", 3, 1);
  CTXitem[ID_CONTEXT_CONFIG_VALUE] = usbdlg_create_entry_with_label(grid, "Config Value", 3, 2);
  CTXitem[ID_CONTEXT_PREV] = gtk_button_new_with_label("<<");
  g_signal_connect(CTXitem[ID_CONTEXT_PREV], "clicked", G_CALLBACK(xhci_ep_context_select), &CTXitem);
  gtk_grid_attach(GTK_GRID(grid), CTXitem[ID_CONTEXT_PREV], 5, 2, 1, 1);
  CTXitem[ID_CONTEXT_OF_STR] = usbdlg_create_ro_entry(grid, 6, 2);
  CTXitem[ID_CONTEXT_NEXT] = gtk_button_new_with_label(">>");
  g_signal_connect(CTXitem[ID_CONTEXT_NEXT], "clicked", G_CALLBACK(xhci_ep_context_select), &CTXitem);
  gtk_grid_attach(GTK_GRID(grid), CTXitem[ID_CONTEXT_NEXT], 7, 2, 1, 1);

  SLframe = gtk_frame_new("Slot Context");
  gtk_grid_attach(GTK_GRID(grid), SLframe, 0, 3, 3, 26);
  SLgrid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(SLgrid), 2);
  gtk_grid_set_column_spacing(GTK_GRID(SLgrid), 5);
  gtk_container_add(GTK_CONTAINER(SLframe), SLgrid);
  CTXitem[ID_CONTEXT_ENTRIES] = usbdlg_create_entry_with_label(SLgrid, "Context Entries", 0, 0);
  CTXitem[ID_CONTEXT_HUB] = gtk_check_button_new_with_label("Hub");
  gtk_grid_attach(GTK_GRID(SLgrid), CTXitem[ID_CONTEXT_HUB], 1, 1, 1, 1);
  CTXitem[ID_CONTEXT_MTT] = gtk_check_button_new_with_label("MTT");
  gtk_grid_attach(GTK_GRID(SLgrid), CTXitem[ID_CONTEXT_MTT], 1, 2, 1, 1);
  CTXitem[ID_CONTEXT_SPEED] = usbdlg_create_entry_with_label(SLgrid, "Speed", 0, 3);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_SPEED]), "changed",
                   G_CALLBACK(context_string_changed), &CTXitem);
  CTXitem[ID_CONTEXT_SPEED_STR] = usbdlg_create_ro_entry(SLgrid, 2, 3);
  CTXitem[ID_CONTEXT_ROUTE_STRING] = usbdlg_create_entry_with_label(SLgrid, "Route String", 0, 4);
  CTXitem[ID_CONTEXT_NUM_PORTS] = usbdlg_create_entry_with_label(SLgrid, "Number of Ports", 0, 5);
  CTXitem[ID_CONTEXT_RH_PORT_NUM] = usbdlg_create_entry_with_label(SLgrid, "Root Hub Port Number", 0, 6);
  CTXitem[ID_CONTEXT_MAX_EXIT_LAT] = usbdlg_create_entry_with_label(SLgrid, "Max Exit Latency", 0, 7);
  CTXitem[ID_CONTEXT_INT_TARGET] = usbdlg_create_entry_with_label(SLgrid, "Interrupter Target", 0, 8);
  CTXitem[ID_CONTEXT_TTT] = usbdlg_create_entry_with_label(SLgrid, "TTT", 0, 9);
  CTXitem[ID_CONTEXT_TT_PORT_NUM] = usbdlg_create_entry_with_label(SLgrid, "TT Port Number", 0, 10);
  CTXitem[ID_CONTEXT_TT_HUB_SLOT_ID] = usbdlg_create_entry_with_label(SLgrid, "TT Hub Slot ID", 0, 11);
  CTXitem[ID_CONTEXT_SLOT_STATE] = usbdlg_create_entry_with_label(SLgrid, "Slot State", 0, 12);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_SLOT_STATE]), "changed",
                   G_CALLBACK(context_string_changed), &CTXitem);
  CTXitem[ID_CONTEXT_SLOT_STATE_STR] = usbdlg_create_ro_entry(SLgrid, 2, 12);
  CTXitem[ID_CONTEXT_DEV_ADDRESS] = usbdlg_create_entry_with_label(SLgrid, "USB Device Address", 0, 13);
  for (i = 0; i < 12; i++) {
    sprintf(str, "RsvdO (%02Xh-%02Xh)", 16 + (i << 2) + 3, 16 + (i << 2));
    CTXitem[ID_CONTEXT_RSVDO_SLOT_0 + i] = usbdlg_create_entry_with_label(SLgrid, str, 0, 14 + i);
    if (i > 3) {
      gtk_widget_set_sensitive(CTXitem[ID_CONTEXT_RSVDO_SLOT_0 + i], 0);
    }
  }

  EPframe = gtk_frame_new("EP Context");
  gtk_grid_attach(GTK_GRID(grid), EPframe, 3, 3, 3, 26);
  EPgrid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(EPgrid), 2);
  gtk_grid_set_column_spacing(GTK_GRID(EPgrid), 5);
  gtk_container_add(GTK_CONTAINER(EPframe), EPgrid);
  apply_button_2 = gtk_button_new_with_label(g_dgettext("gtk30", "_Apply"));
  g_signal_connect(apply_button_2, "clicked", G_CALLBACK(xhci_ep_context_apply), &CTXitem);
  gtk_grid_attach(GTK_GRID(EPgrid), apply_button_2, 2, 0, 1, 1);
  CTXitem[ID_CONTEXT_MAX_ESIT_HI] = usbdlg_create_entry_with_label(EPgrid, "Max ESIT Payload Hi", 0, 1);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_MAX_ESIT_HI]), "changed",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  CTXitem[ID_CONTEXT_INTERVAL] = usbdlg_create_entry_with_label(EPgrid, "Interval", 0, 2);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_INTERVAL]), "changed",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  CTXitem[ID_CONTEXT_LSA] = gtk_check_button_new_with_label("LSA");
  g_signal_connect(GTK_TOGGLE_BUTTON(CTXitem[ID_CONTEXT_LSA]), "toggled",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  gtk_grid_attach(GTK_GRID(EPgrid), CTXitem[ID_CONTEXT_LSA], 1, 3, 1, 1);
  CTXitem[ID_CONTEXT_MAX_PSTREAMS] = usbdlg_create_entry_with_label(EPgrid, "MaxPStreams", 0, 4);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_MAX_PSTREAMS]), "changed",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  CTXitem[ID_CONTEXT_MAXPS_STR] = usbdlg_create_ro_entry(EPgrid, 2, 4);
  CTXitem[ID_CONTEXT_MULT] = usbdlg_create_entry_with_label(EPgrid, "Mult", 0, 5);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_MULT]), "changed",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  CTXitem[ID_CONTEXT_EP_STATE] = usbdlg_create_entry_with_label(EPgrid, "EP State", 0, 6);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_EP_STATE]), "changed",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  CTXitem[ID_CONTEXT_EP_STATE_STR] = usbdlg_create_ro_entry(EPgrid, 2, 6);
  CTXitem[ID_CONTEXT_MAX_PACKET_SIZE] = usbdlg_create_entry_with_label(EPgrid, "Max Packet Size", 0, 7);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_MAX_PACKET_SIZE]), "changed",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  CTXitem[ID_CONTEXT_MAX_BURST_SIZE] = usbdlg_create_entry_with_label(EPgrid, "Max Burst Size", 0, 8);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_MAX_BURST_SIZE]), "changed",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  CTXitem[ID_CONTEXT_HID] = gtk_check_button_new_with_label("HID");
  g_signal_connect(GTK_TOGGLE_BUTTON(CTXitem[ID_CONTEXT_HID]), "toggled",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  gtk_grid_attach(GTK_GRID(EPgrid), CTXitem[ID_CONTEXT_HID], 1, 9, 1, 1);
  CTXitem[ID_CONTEXT_EP_TYPE] = usbdlg_create_entry_with_label(EPgrid, "EP Type", 0, 10);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_EP_TYPE]), "changed",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  CTXitem[ID_CONTEXT_EP_TYPE_STR] = usbdlg_create_ro_entry(EPgrid, 2, 10);
  CTXitem[ID_CONTEXT_CERR] = usbdlg_create_entry_with_label(EPgrid, "CErr", 0, 11);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_CERR]), "changed",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  CTXitem[ID_CONTEXT_TR_DEQUEUE_PTR] = usbdlg_create_entry_with_label(EPgrid, "TR Dequeue Pointer", 0, 12);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_TR_DEQUEUE_PTR]), "changed",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  CTXitem[ID_CONTEXT_STREAM_CONTEXT] = gtk_button_new_with_label(">");
  g_signal_connect(CTXitem[ID_CONTEXT_STREAM_CONTEXT], "clicked", G_CALLBACK(xhci_string_context_dialog), &CTXitem);
  gtk_grid_attach(GTK_GRID(EPgrid), CTXitem[ID_CONTEXT_STREAM_CONTEXT], 2, 12, 1, 1);
  CTXitem[ID_CONTEXT_DCS] = gtk_check_button_new_with_label("DCS");
  g_signal_connect(GTK_TOGGLE_BUTTON(CTXitem[ID_CONTEXT_DCS]), "toggled",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  gtk_grid_attach(GTK_GRID(EPgrid), CTXitem[ID_CONTEXT_DCS], 1, 13, 1, 1);
  CTXitem[ID_CONTEXT_MAX_ESIT_LO] = usbdlg_create_entry_with_label(EPgrid, "Max ESIT Payload Lo", 0, 14);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_MAX_ESIT_LO]), "changed",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  CTXitem[ID_CONTEXT_AVERAGE_LEN] = usbdlg_create_entry_with_label(EPgrid, "Average TRB Length", 0, 15);
  g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_AVERAGE_LEN]), "changed",
                   G_CALLBACK(ep_context_entry_changed), &CTXitem);
  for (i = 0; i < 11; i++) {
    sprintf(str, "RsvdO (%02Xh-%02Xh)", 20 + (i << 2) + 3, 20 + (i << 2));
    CTXitem[ID_CONTEXT_RSVDO_EP_0 + i] = usbdlg_create_entry_with_label(EPgrid, str, 0, 16 + i);
    g_signal_connect(GTK_EDITABLE(CTXitem[ID_CONTEXT_RSVDO_EP_0 + i]), "changed",
                     G_CALLBACK(ep_context_entry_changed), &CTXitem);
    if (i > 3) {
      gtk_widget_set_sensitive(CTXitem[ID_CONTEXT_RSVDO_EP_0 + i], 0);
    }
  }

  // Context structure
  sprintf(str, "0x%08X", p[0]);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_DROP]), str);
  sprintf(str, "0x%08X", p[1]);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_ADD]), str);
  sprintf(str, "%i", (p[7] & 0x00FF0000) >> 16);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_ALT_SETTING]), str);
  sprintf(str, "%i", (p[7] & 0x0000FF00) >> 8);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_INTFACE_NUM]), str);
  sprintf(str, "%i", (p[7] & 0x000000FF) >> 0);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_CONFIG_VALUE]), str);

  // Slot Context
  p = (Bit32u *) &xhci_context[CONTEXT_SIZE];
  sprintf(str, "%i", (p[0] & (0x1F << 27)) >> 27);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_ENTRIES]), str);
  if (p[0] & (1<<26)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CTXitem[ID_CONTEXT_HUB]), TRUE);
  }
  if (p[0] & (1<<25)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CTXitem[ID_CONTEXT_MTT]), TRUE);
  }
  sprintf(str, "%i", (p[0] & (0xF << 20)) >> 20);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_SPEED]), str);
  sprintf(str, "0x%05X", (p[0] & (0xFFFFF << 0)) >> 0);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_ROUTE_STRING]), str);
  sprintf(str, "%i", (p[1] & (0xFF << 24)) >> 24);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_NUM_PORTS]), str);
  sprintf(str, "%i", (p[1] & (0xFF << 16)) >> 16);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_RH_PORT_NUM]), str);
  sprintf(str, "%i", (p[1] & (0xFFFF << 0)) >> 0);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAX_EXIT_LAT]), str);
  sprintf(str, "%i", (p[2] & (0x3FF << 22)) >> 22);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_INT_TARGET]), str);
  sprintf(str, "%i", (p[2] & (0x3 << 16)) >> 16);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_TTT]), str);
  sprintf(str, "%i", (p[2] & (0xFF << 8)) >> 8);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_TT_PORT_NUM]), str);
  sprintf(str, "%i", (p[2] & (0xFF << 0)) >> 0);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_TT_HUB_SLOT_ID]), str);
  sprintf(str, "%i", (p[3] & (0x1F << 27)) >> 27);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_SLOT_STATE]), str);
  sprintf(str, "%i", (p[3] & (0xFF << 0)) >> 0);
  gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_DEV_ADDRESS]), str);
  for (i=0; i<4; i++) {
    sprintf(str, "0x%08X", p[4+i]);
    gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_RSVDO_SLOT_0 + i]), str);
  }
#if (CONTEXT_SIZE == 64)
  for (i=4; i<12; i++) {
    sprintf(str, "0x%08X", p[4+i]);
    gtk_entry_set_text(GTK_ENTRY(CTXitem[ID_CONTEXT_RSVDO_SLOT_0 + i]), str);
    gtk_widget_set_sensitive(CTXitem[ID_CONTEXT_RSVDO_SLOT_0 + i], 1);
  }
#endif

  // Endpoint Context
  xhci_ep_context_select(NULL, &CTXitem);

  // Show dialog
  gtk_widget_show_all(ctx_dialog);
  ret = gtk_dialog_run(GTK_DIALOG(ctx_dialog));
  if (ret == GTK_RESPONSE_OK) {
    if (xhci_context_changed) {
      if (xhci_ep_context_change_dlg(GTK_WINDOW(ctx_dialog))) {
        xhci_ep_context_apply(apply_button_2, &CTXitem);
      }
    }
    // Context structure
    p = (Bit32u *) &xhci_context[0];
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_DROP])));
    p[0] = strtol(str, NULL, 0) & ~0x3;
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_ADD])));
    p[1] = strtol(str, NULL, 0);

    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_ALT_SETTING])));
    p[7]  = (strtol(str, NULL, 0) & 0xFF) << 16;
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_INTFACE_NUM])));
    p[7] |= (strtol(str, NULL, 0) & 0xFF) << 8;
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_CONFIG_VALUE])));
    p[7] |= (strtol(str, NULL, 0) & 0xFF) << 0;

    // Slot Context
    p = (Bit32u *) &xhci_context[CONTEXT_SIZE];
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_ENTRIES])));
    p[0]  = (strtol(str, NULL, 0) & 0x1F) << 27;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(CTXitem[ID_CONTEXT_HUB]))) {
      p[0] |= (1<<26);
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(CTXitem[ID_CONTEXT_MTT]))) {
      p[0] |= (1<<25);
    }
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_SPEED])));
    p[0] |= (strtol(str, NULL, 0) & 0xF) << 20;
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_ROUTE_STRING])));
    p[0] |= (strtol(str, NULL, 0) & 0xFFFFF) << 0;
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_NUM_PORTS])));
    p[1]  = (strtol(str, NULL, 0) & 0xFF) << 24;
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_RH_PORT_NUM])));
    p[1] |= (strtol(str, NULL, 0) & 0xFF) << 16;
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_MAX_EXIT_LAT])));
    p[1] |= (strtol(str, NULL, 0) & 0xFFFF) << 0;
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_INT_TARGET])));
    p[2]  = (strtol(str, NULL, 0) & 0x3FF) << 22;
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_TTT])));
    p[2] |= (strtol(str, NULL, 0) & 0x3) << 16;
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_TT_PORT_NUM])));
    p[2] |= (strtol(str, NULL, 0) & 0xFF) << 8;
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_TT_HUB_SLOT_ID])));
    p[2] |= (strtol(str, NULL, 0) & 0xFF) << 0;
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_SLOT_STATE])));
    p[3]  = (strtol(str, NULL, 0) & 0x1F) << 27;
    strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_DEV_ADDRESS])));
    p[3] |= (strtol(str, NULL, 0) & 0xFF) << 0;
    for (i=0; i<4; i++) {
      strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_RSVDO_SLOT_0 + i])));
      p[4+i] = strtol(str, NULL, 0);
    }
#if (CONTEXT_SIZE == 64)
    for (i=4; i<12; i++) {
      strcpy(str, gtk_entry_get_text(GTK_ENTRY(CTXitem[ID_CONTEXT_RSVDO_SLOT_0 + i])));
      p[4+i] = strtol(str, NULL, 0);
    }
#endif
    DEV_MEM_WRITE_PHYSICAL(xhci_context_address, CONTEXT_SIZE + (32 * CONTEXT_SIZE), xhci_context);
  }
  delete [] xhci_context;
  gtk_widget_destroy(ctx_dialog);
  free(attribs);
}

bool xhci_view_trb_dialog(Bit8u type, struct TRB *trb)
{
  GtkWidget *mainVbox, *grid, *TRBitem[TRB_N_ITEMS], *button[3];
  int ret, row = 0;
  char str[COMMON_STR_SIZE];

  // Using the type of trb, display an associated dialog
  sprintf(str, "%s TRB", trb_types[type].name);
  trb_dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(trb_dialog), str);
  gtk_window_set_transient_for(GTK_WINDOW(trb_dialog), GTK_WINDOW(main_dialog));
  gtk_window_set_modal(GTK_WINDOW(trb_dialog), TRUE);
  gtk_window_set_default_size(GTK_WINDOW(trb_dialog), 200, 200);
  button[0] = gtk_dialog_add_button(GTK_DIALOG(trb_dialog), g_dgettext("gtk30", "_Save"), GTK_RESPONSE_OK);
  button[1] = gtk_dialog_add_button(GTK_DIALOG(trb_dialog), g_dgettext("gtk30", "_Cancel"), GTK_RESPONSE_CANCEL);
  gtk_dialog_set_default_response(GTK_DIALOG(trb_dialog), GTK_RESPONSE_OK);
  gtk_window_set_focus(GTK_WINDOW(trb_dialog), button[0]);
  mainVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(trb_dialog))), mainVbox, TRUE, TRUE, 2);
  grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
  gtk_box_pack_start(GTK_BOX(mainVbox), grid, TRUE, TRUE, 2);
  memset(TRBitem, 0, sizeof(GtkWidget*) * TRB_N_ITEMS);
  switch (type) {
    case NORMAL:
    case DATA_STAGE:
      TRBitem[ID_TRB_DATA_PTR] = usbdlg_create_entry_with_label(grid, "Data Pointer", 0, row++);
      sprintf(str, "0x" FMT_ADDRX64, trb->parameter);
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR]), str);
      break;
    case SETUP_STAGE:
      TRBitem[ID_TRB_BREQUESTTYPE] = usbdlg_create_entry_with_label(grid, "bRequestType", 0, row++);
      sprintf(str, "0x%02X", (Bit8u) ((trb->parameter & BX_CONST64(0x00000000000000FF)) >> 0));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_BREQUESTTYPE]), str);
      TRBitem[ID_TRB_BREQUEST] = usbdlg_create_entry_with_label(grid, "bRequest", 0, row++);
      sprintf(str, "0x%02X", (Bit8u) ((trb->parameter & BX_CONST64(0x000000000000FF00)) >> 8));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_BREQUEST]), str);
      TRBitem[ID_TRB_WVALUE] = usbdlg_create_entry_with_label(grid, "wValue", 0, row++);
      sprintf(str, "0x%04X", (Bit16u) ((trb->parameter & BX_CONST64(0x00000000FFFF0000)) >> 16));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_WVALUE]), str);
      TRBitem[ID_TRB_WINDEX] = usbdlg_create_entry_with_label(grid, "wIndex", 0, row++);
      sprintf(str, "0x%04X", (Bit16u) ((trb->parameter & BX_CONST64(0x0000FFFF00000000)) >> 32));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_WINDEX]), str);
      TRBitem[ID_TRB_WLENGTH] = usbdlg_create_entry_with_label(grid, "wLength", 0, row++);
      sprintf(str, "0x%04X", (Bit16u) ((trb->parameter & BX_CONST64(0xFFFF000000000000)) >> 48));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_WLENGTH]), str);
      break;
    case LINK:
      TRBitem[ID_TRB_DATA_PTR] = usbdlg_create_entry_with_label(grid, "Ring Pointer", 0, row++);
      sprintf(str, "0x" FMT_ADDRX64, (Bit64u)(trb->parameter & ~BX_CONST64(0x0F)));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR]), str);
      break;
    case EVENT_DATA:
      TRBitem[ID_TRB_DATA_PTR] = usbdlg_create_entry_with_label(grid, "Event Data", 0, row++);
      sprintf(str, "0x" FMT_ADDRX64, trb->parameter);
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR]), str);
      break;
    case ENABLE_SLOT:
      TRBitem[ID_TRB_SLOT_TYPE] = usbdlg_create_entry_with_label(grid, "Slot Type", 0, row++);
      sprintf(str, "%i", TRB_GET_STYPE(trb->command));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_TYPE]), str);
      break;
    case ADDRESS_DEVICE:
    case CONFIG_EP:
    case EVALUATE_CONTEXT:
      TRBitem[ID_TRB_DATA_PTR] = usbdlg_create_entry_with_label(grid, "Input Context Pointer", 0, row);
      sprintf(str, "0x" FMT_ADDRX64, (Bit64u)(trb->parameter & ~BX_CONST64(0x0F)));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR]), str);
      button[2] = gtk_button_new_with_label(">");
      g_signal_connect(button[2], "clicked", G_CALLBACK(xhci_context_dialog), TRBitem[ID_TRB_DATA_PTR]);
      gtk_grid_attach(GTK_GRID(grid), button[2], 2, row++, 1, 1);
      break;
    case SET_TR_DEQUEUE:
      TRBitem[ID_TRB_DATA_PTR] = usbdlg_create_entry_with_label(grid, "New TR Dequeue Pointer", 0, row++);
      sprintf(str, "0x" FMT_ADDRX64, (Bit64u)(trb->parameter & ~BX_CONST64(0x0F)));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR]), str);
      TRBitem[ID_TRB_SCT] = usbdlg_create_entry_with_label(grid, "Stream Context Type", 0, row++);
      sprintf(str, "%i", (Bit8u)(trb->parameter >> 1) & 0x07);
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_SCT]), str);
      TRBitem[ID_TRB_DCS] = gtk_check_button_new_with_label("Dequeue Cycle Bit");
      if (trb->parameter & 1) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_DCS]), TRUE);
      }
      gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_DCS], 1, row++, 1, 1);
      TRBitem[ID_TRB_STREAMID] = usbdlg_create_entry_with_label(grid, "Stream ID", 0, row++);
      sprintf(str, "%i", TRB_GET_STREAM(trb->status));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_STREAMID]), str);
      break;
    case FORCE_EVENT:
      TRBitem[ID_TRB_DATA_PTR] = usbdlg_create_entry_with_label(grid, "Event TRB Pointer", 0, row++);
      sprintf(str, "0x" FMT_ADDRX64, (Bit64u)(trb->parameter & ~BX_CONST64(0x0F)));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR]), str);
      break;
    case SET_LAT_TOLERANCE:
      TRBitem[ID_TRB_BELT] = usbdlg_create_entry_with_label(grid, "Best Effort Value", 0, row++);
      sprintf(str, "%i", (trb->command & (0xFFF << 16)) >> 16);
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_BELT]), str);
      break;
    case GET_PORT_BAND:
      TRBitem[ID_TRB_DATA_PTR] = usbdlg_create_entry_with_label(grid, "Port Bandwidth Context Pointer", 0, row++);
      sprintf(str, "0x" FMT_ADDRX64, (Bit64u)(trb->parameter & ~BX_CONST64(0x0F)));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR]), str);
      break;
    case FORCE_HEADER:
      TRBitem[ID_TRB_DATA_PTR] = usbdlg_create_entry_with_label(grid, "Header Lo/Mid", 0, row++);
      sprintf(str, "0x" FMT_ADDRX64, (Bit64u)(trb->parameter & ~BX_CONST64(0x1F)));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR]), str);
      TRBitem[ID_TRB_HDR_HI] = usbdlg_create_entry_with_label(grid, "Header Hi", 0, row++);
      sprintf(str, "0x%08X", trb->status);
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_HDR_HI]), str);
      TRBitem[ID_TRB_FTYPE] = usbdlg_create_entry_with_label(grid, "Type", 0, row++);
      sprintf(str, "%i", (Bit8u)(trb->parameter & 0x1F));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_FTYPE]), str);
      break;
    case TRANS_EVENT:
    case COMMAND_COMPLETION:
      TRBitem[ID_TRB_DATA_PTR] = usbdlg_create_entry_with_label(grid, "TRB Pointer", 0, row++);
      sprintf(str, "0x" FMT_ADDRX64, trb->parameter);
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR]), str);
      break;
    case PORT_STATUS_CHANGE:
      TRBitem[ID_TRB_DATA_PTR] = usbdlg_create_entry_with_label(grid, "Port ID", 0, row++);
      sprintf(str, "0x" FMT_ADDRX64, (Bit64u)((trb->parameter & BX_CONST64(0x00000000FF000000)) >> 24));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR]), str);
      break;
    case DOORBELL_EVENT:
      TRBitem[ID_TRB_DATA_PTR] = usbdlg_create_entry_with_label(grid, "DB Reason", 0, row++);
      sprintf(str, "0x" FMT_ADDRX64, trb->parameter & 0x1f);
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR]), str);
      break;
    case DEVICE_NOTIFICATION:
      TRBitem[ID_TRB_DATA_PTR] = usbdlg_create_entry_with_label(grid, "Dev Notification Data", 0, row++);
      sprintf(str, "0x" FMT_ADDRX64, trb->parameter >> 8);
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR]), str);
      TRBitem[ID_TRB_NOT_TYPE] = usbdlg_create_entry_with_label(grid, "Notification Type", 0, row++);
      sprintf(str, "%i", (Bit8u)(trb->parameter & 0xf0) >> 4);
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_NOT_TYPE]), str);
      break;
  }
  switch (type) {
    case TRANS_EVENT:
    case COMMAND_COMPLETION:
    case PORT_STATUS_CHANGE:
    case BANDWIDTH_REQUEST:
    case DOORBELL_EVENT:
    case HOST_CONTROLLER_EVENT:
    case DEVICE_NOTIFICATION:
      TRBitem[ID_TRB_COMP_CODE] = usbdlg_create_entry_with_label(grid, "Completion Code", 0, row++);
      sprintf(str, "%i", TRB_GET_COMP_CODE(trb->status));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_COMP_CODE]), str);
      if (type == COMMAND_COMPLETION) {
        TRBitem[ID_TRB_COMP_LPARAM] = usbdlg_create_entry_with_label(grid, "Completion Parameter", 0, row++);
        sprintf(str, "%i", trb->status & 0x00FFFFFF);
        gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_COMP_LPARAM]), str);
      }
      break;
  }
  switch (type) {
    case DISABLE_SLOT:
    case ADDRESS_DEVICE:
    case CONFIG_EP:
    case EVALUATE_CONTEXT:
    case RESET_EP:
    case STOP_EP:
    case SET_TR_DEQUEUE:
    case RESET_DEVICE:
    case TRANS_EVENT:
    case COMMAND_COMPLETION:
    case BANDWIDTH_REQUEST:
    case DOORBELL_EVENT:
    case DEVICE_NOTIFICATION:
      TRBitem[ID_TRB_SLOT_ID] = usbdlg_create_entry_with_label(grid, "Slot ID", 0, row++);
      sprintf(str, "%i", TRB_GET_SLOT(trb->command));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID]), str);
      break;
    case GET_PORT_BAND:
      TRBitem[ID_TRB_SLOT_ID] = usbdlg_create_entry_with_label(grid, "Hub Slot ID", 0, row++);
      sprintf(str, "%i", TRB_GET_SLOT(trb->command));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID]), str);
      TRBitem[ID_TRB_DEV_SPEED] = usbdlg_create_entry_with_label(grid, "Device Speed", 0, row++);
      sprintf(str, "%i", (trb->command & (0x0F << 16)) >> 16);
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_DEV_SPEED]), str);
      break;
    case FORCE_HEADER:
      TRBitem[ID_TRB_SLOT_ID] = usbdlg_create_entry_with_label(grid, "Root Hub Port Number", 0, row++);
      sprintf(str, "%i", TRB_GET_SLOT(trb->command));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID]), str);
      break;
  }
  if (type == STOP_EP) {
    TRBitem[ID_TRB_SP] = gtk_check_button_new_with_label("Suspend");
    if (trb->command & (1 << 23)) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_SP]), TRUE);
    }
    gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_SP], 1, row++, 1, 1);
  }
  switch (type) {
    case RESET_EP:
    case STOP_EP:
    case SET_TR_DEQUEUE:
    case TRANS_EVENT:
      TRBitem[ID_TRB_EP_ID] = usbdlg_create_entry_with_label(grid, "Endpoint ID", 0, row++);
      sprintf(str, "%i", TRB_GET_EP(trb->command));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_EP_ID]), str);
      break;
  }
  switch (type) {
    case NORMAL:
    case SETUP_STAGE:
    case DATA_STAGE:
    case STATUS_STAGE:
    case LINK:
    case EVENT_DATA:
    case NO_OP:
      TRBitem[ID_TRB_INT_TARGET] = usbdlg_create_entry_with_label(grid, "Interrupter Target", 0, row++);
      sprintf(str, "%i", TRB_GET_TARGET(trb->status));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_INT_TARGET]), str);
      break;
    case FORCE_EVENT:
      TRBitem[ID_TRB_INT_TARGET] = usbdlg_create_entry_with_label(grid, "VF Interrupter Target", 0, row++);
      sprintf(str, "%i", TRB_GET_TARGET(trb->status));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_INT_TARGET]), str);
      break;
  }
  switch (type) {
    case NORMAL:
    case SETUP_STAGE:
    case DATA_STAGE:
      if (type != SETUP_STAGE) {
        TRBitem[ID_TRB_TD_SIZE] = usbdlg_create_entry_with_label(grid, "TD Size", 0, row++);
        sprintf(str, "%i", TRB_GET_TDSIZE(trb->status));
        gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_TD_SIZE]), str);
      }
      TRBitem[ID_TRB_TRANS_LEN] = usbdlg_create_entry_with_label(grid, "TRB Transfer Length", 0, row++);
      sprintf(str, "%i", TRB_GET_TX_LEN(trb->status));
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_TRANS_LEN]), str);
      if (type == SETUP_STAGE) {
        TRBitem[ID_TRB_TRT] = usbdlg_create_entry_with_label(grid, "Transfer Type", 0, row++);
        sprintf(str, "%i", (trb->command & 0x00030000) >> 16);
        gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_TRT]), str);
      }
      break;
  }
  switch (type) {
    case DATA_STAGE:
    case STATUS_STAGE:
      TRBitem[ID_TRB_DIR] = gtk_check_button_new_with_label("Direction");
      if (trb->command & 0x10000) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_DIR]), TRUE);
      }
      gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_DIR], 1, row++, 1, 1);
      break;
    case FORCE_EVENT:
    case COMMAND_COMPLETION:
    case DOORBELL_EVENT:
      TRBitem[ID_TRB_VF_ID] = usbdlg_create_entry_with_label(grid, "VF ID", 0, row++);
      sprintf(str, "%i", (trb->command & (0xFF << 16)) >> 16);
      gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_VF_ID]), str);
      break;
  }
  TRBitem[ID_TRB_TYPE] = usbdlg_create_ro_entry_with_label(grid, "TRB Type", 0, row++);
  sprintf(str, "%i", TRB_GET_TYPE(trb->command));
  gtk_entry_set_text(GTK_ENTRY(TRBitem[ID_TRB_TYPE]), str);
  if ((type == NORMAL) || (type == EVENT_DATA)) {
    TRBitem[ID_TRB_BEI] = gtk_check_button_new_with_label("Block Event Interrupt");
    if (TRB_DC(trb->command)) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_BEI]), TRUE);
    }
    gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_BEI], 1, row++, 1, 1);
  }
  switch (type) {
    case NORMAL:
    case SETUP_STAGE:
    case DATA_STAGE:
      TRBitem[ID_TRB_IDT] = gtk_check_button_new_with_label("Immediate Data");
      if (TRB_IS_IMMED_DATA(trb->command)) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_IDT]), TRUE);
      }
      gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_IDT], 1, row++, 1, 1);
      break;
  }
  switch (type) {
    case NORMAL:
    case SETUP_STAGE:
    case DATA_STAGE:
    case STATUS_STAGE:
    case LINK:
    case EVENT_DATA:
    case NO_OP:
      TRBitem[ID_TRB_IOC] = gtk_check_button_new_with_label("Interrupt On Complete");
      if (TRB_IOC(trb->command)) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_IOC]), TRUE);
      }
      gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_IOC], 1, row++, 1, 1);
      if (type != SETUP_STAGE) {
        TRBitem[ID_TRB_CH] = gtk_check_button_new_with_label("Chain Bit");
        if (TRB_CHAIN(trb->command)) {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_CH]), TRUE);
        }
        gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_CH], 1, row++, 1, 1);
      }
      if ((type == NORMAL) || (type == DATA_STAGE)) {
        TRBitem[ID_TRB_NS] = gtk_check_button_new_with_label("No Snoop");
        if (trb->command & 8) {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_NS]), TRUE);
        }
        gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_NS], 1, row++, 1, 1);
        TRBitem[ID_TRB_ISP] = gtk_check_button_new_with_label("Interrupt On Short Packet");
        if (TRB_SPD(trb->command)) {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_ISP]), TRUE);
        }
        gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_ISP], 1, row++, 1, 1);
      }
      if ((type != SETUP_STAGE) && (type != LINK)) {
        TRBitem[ID_TRB_ENT] = gtk_check_button_new_with_label("Evaluate Next TRB");
        if (TRB_TOGGLE(trb->command)) {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_ENT]), TRUE);
        }
        gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_ENT], 1, row++, 1, 1);
      }
      break;
  }
  if (type == LINK) {
    TRBitem[ID_TRB_TC] = gtk_check_button_new_with_label("Toggle Cycle");
    if (TRB_GET_TOGGLE(trb->command)) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_TC]), TRUE);
    }
    gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_TC], 1, row++, 1, 1);
  } else if (type == TRANS_EVENT) {
    TRBitem[ID_TRB_ED] = gtk_check_button_new_with_label("Event Data");
    if (trb->command & 4) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_ED]), TRUE);
    }
    gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_ED], 1, row++, 1, 1);
  } else if (type == CONFIG_EP) {
    TRBitem[ID_TRB_DECONFIG] = gtk_check_button_new_with_label("Deconfigure");
    if (trb->command & (1 << 9)) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_DECONFIG]), TRUE);
    }
    gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_DECONFIG], 1, row++, 1, 1);
  } else if (type == RESET_EP) {
    TRBitem[ID_TRB_TSP] = gtk_check_button_new_with_label("Transfer State Preserve");
    if (trb->command & (1 << 9)) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_TSP]), TRUE);
    }
    gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_TSP], 1, row++, 1, 1);
  } else if (type == ADDRESS_DEVICE) {
    TRBitem[ID_TRB_BSR] = gtk_check_button_new_with_label("Block Set Address");
    if (trb->command & (1 << 9)) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_BSR]), TRUE);
    }
    gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_BSR], 1, row++, 1, 1);
  }
  TRBitem[ID_TRB_C] = gtk_check_button_new_with_label("Cycle Bit");
  if (trb->command & 1) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_C]), TRUE);
  }
  gtk_grid_attach(GTK_GRID(grid), TRBitem[ID_TRB_C], 1, row++, 1, 1);
  // Show dialog
  gtk_widget_show_all(trb_dialog);
  ret = gtk_dialog_run(GTK_DIALOG(trb_dialog));
  if (ret == GTK_RESPONSE_OK) {
    switch (type) {
      case NORMAL:
      case DATA_STAGE:
      case STATUS_STAGE:
      case LINK:
      case EVENT_DATA:
      case NO_OP:
        if ((type == STATUS_STAGE) || (type == NO_OP)) {
          trb->parameter = 0;
        } else {
          strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR])));
          if (type == LINK) {
            trb->parameter = strtol(str, NULL, 0) & ~BX_CONST64(0x0F);
          } else {
            trb->parameter = strtol(str, NULL, 0);
          }
        }
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_INT_TARGET])));
        trb->status = (strtol(str, NULL, 0) & 0x3FF) << 22;
        if ((type == NORMAL) || (type == DATA_STAGE)) {
          strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_TD_SIZE])));
          trb->status |= TRB_SET_TDSIZE(strtol(str, NULL, 0));
          strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_TRANS_LEN])));
          trb->status |= strtol(str, NULL, 0) & 0x1FFFF;
        }

        if ((type == DATA_STAGE) || (type == STATUS_STAGE)) {
          if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_DIR]))) {
            trb->command = (1<<16);
          }
        } else if (type != LINK) {
          if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_BEI]))) {
            trb->command = (1<<9);
          }
        } else {
            trb->command = 0;
        }
        if ((type == NORMAL) || (type == DATA_STAGE)) {
          if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_IDT]))) {
            trb->command |= (1<<6);
          }
        }
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_IOC]))) {
          trb->command |= (1<<5);
        }
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_CH]))) {
          trb->command |= (1<<4);
        }
        if ((type == NORMAL) || (type == DATA_STAGE)) {
          if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_NS]))) {
            trb->command |= (1<<3);
          }
          if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_ISP]))) {
            trb->command |= (1<<2);
          }
        }
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_ENT]))) {
          trb->command |= (1<<1);
        }
        break;
    case SETUP_STAGE:
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_BREQUESTTYPE])));
        trb->parameter  = (((Bit64u) strtol(str, NULL, 0) << 0)  & BX_CONST64(0x00000000000000FF));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_BREQUEST])));
        trb->parameter |= (((Bit64u) strtol(str, NULL, 0) << 8)  & BX_CONST64(0x000000000000FF00));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_WVALUE])));
        trb->parameter |= (((Bit64u) strtol(str, NULL, 0) << 16) & BX_CONST64(0x00000000FFFF0000));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_WINDEX])));
        trb->parameter |= (((Bit64u) strtol(str, NULL, 0) << 32) & BX_CONST64(0x0000FFFF00000000));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_WLENGTH])));
        trb->parameter |= (((Bit64u) strtol(str, NULL, 0) << 48) & BX_CONST64(0xFFFF000000000000));

        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_INT_TARGET])));
        trb->status = (strtol(str, NULL, 0) & 0x3FF) << 22;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_TRANS_LEN])));
        trb->status |= strtol(str, NULL, 0) & 0x1FFFF;

        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_TRT])));
        trb->command = (strtol(str, NULL, 0) << 16) & 0x00030000;

        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_IDT]))) {
          trb->command |= (1<<6);
        }
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_IOC]))) {
          trb->command |= (1<<5);
        }
        break;
      case ENABLE_SLOT:
        trb->parameter = 0;
        trb->status = 0;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_TYPE])));
        trb->command = TRB_SET_STYPE(strtol(str, NULL, 0));
        break;
      case DISABLE_SLOT:
        trb->parameter = 0;
        trb->status = 0;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID])));
        trb->command = TRB_SET_SLOT(strtol(str, NULL, 0));
        break;
      case ADDRESS_DEVICE:
      case CONFIG_EP:
      case EVALUATE_CONTEXT:
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR])));
        trb->parameter = strtol(str, NULL, 0) & ~BX_CONST64(0x0F);
        trb->status = 0;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID])));
        trb->command = TRB_SET_SLOT(strtol(str, NULL, 0));
        if (type == ADDRESS_DEVICE) {
          if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_BSR]))) {
            trb->command |= (1<<9);
          }
        } else if (type == CONFIG_EP) {
          if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_DECONFIG]))) {
            trb->command |= (1<<9);
          }
        }
        break;
      case RESET_EP:
      case STOP_EP:
      case SET_TR_DEQUEUE:
        if (type == SET_TR_DEQUEUE) {
          strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR])));
          trb->parameter = strtol(str, NULL, 0) & ~BX_CONST64(0x0F);
          strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_STREAMID])));
          trb->status = (strtol(str, NULL, 0) & 0xFFFF) << 16;
        } else {
          trb->parameter = 0;
          trb->status = 0;
        }
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID])));
        trb->command = TRB_SET_SLOT(strtol(str, NULL, 0));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_EP_ID])));
        trb->command |= TRB_SET_EP(strtol(str, NULL, 0));
        if (type == RESET_EP) {
          if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_TSP]))) {
            trb->command |= (1<<9);
          }
        } else if (type == STOP_EP) {
          if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_SP]))) {
            trb->command |= (1<<23);
          }
        }
        break;
      case RESET_DEVICE:
        trb->parameter = 0;
        trb->status = 0;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID])));
        trb->command = TRB_SET_SLOT(strtol(str, NULL, 0));
        break;
      case FORCE_EVENT:
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR])));
        trb->parameter = strtol(str, NULL, 0) & ~BX_CONST64(0x0F);
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_INT_TARGET])));
        trb->status = (strtol(str, NULL, 0) & 0x3FF) << 22;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_VF_ID])));
        trb->command = (strtol(str, NULL, 0) & 0xFF) << 16;
        break;
      case SET_LAT_TOLERANCE:
        trb->parameter = 0;
        trb->status = 0;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_BELT])));
        trb->command = (strtol(str, NULL, 0) & 0xFFF) << 16;
        break;
      case GET_PORT_BAND:
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR])));
        trb->parameter = strtol(str, NULL, 0) & ~BX_CONST64(0x0F);
        trb->status = 0;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID])));
        trb->command = TRB_SET_SLOT(strtol(str, NULL, 0));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_DEV_SPEED])));
        trb->command |= (strtol(str, NULL, 0) & 0x0F) << 16;
        break;
      case FORCE_HEADER:
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR])));
        trb->parameter = strtol(str, NULL, 0) & ~BX_CONST64(0x1F);
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_HDR_HI])));
        trb->status = strtol(str, NULL, 0);
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_FTYPE])));
        trb->parameter |= (strtol(str, NULL, 0) & 0x1F);
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID])));
        trb->command = TRB_SET_SLOT(strtol(str, NULL, 0));
        break;
      case TRANS_EVENT:
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR])));
        trb->parameter = strtol(str, NULL, 0);
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_COMP_CODE])));
        trb->status = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID])));
        trb->command = TRB_SET_SLOT(strtol(str, NULL, 0));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_EP_ID])));
        trb->command |= TRB_SET_EP(strtol(str, NULL, 0));
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_ED]))) {
          trb->command |= (1<<2);
        }
        break;
      case COMMAND_COMPLETION:
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR])));
        trb->parameter = strtol(str, NULL, 0);
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_COMP_CODE])));
        trb->status = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_COMP_LPARAM])));
        trb->status |= strtol(str, NULL, 0) & 0x00FFFFFF;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID])));
        trb->command = TRB_SET_SLOT(strtol(str, NULL, 0));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_VF_ID])));
        trb->command |= (strtol(str, NULL, 0) & 0xFF) << 16;
        break;
      case PORT_STATUS_CHANGE:
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR])));
        trb->parameter = (Bit64u) (strtol(str, NULL, 0) & 0xFF) << 24;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_COMP_CODE])));
        trb->status = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
        trb->command = 0;
        break;
      case BANDWIDTH_REQUEST:
        trb->parameter = 0;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_COMP_CODE])));
        trb->status = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID])));
        trb->command = TRB_SET_SLOT(strtol(str, NULL, 0));
        break;
      case DOORBELL_EVENT:
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR])));
        trb->parameter = strtol(str, NULL, 0) & 0x1F;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_COMP_CODE])));
        trb->status = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID])));
        trb->command = TRB_SET_SLOT(strtol(str, NULL, 0));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_VF_ID])));
        trb->command |= (strtol(str, NULL, 0) & 0xFF) << 16;
        break;
      case HOST_CONTROLLER_EVENT:
        trb->parameter = 0;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_COMP_CODE])));
        trb->status = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
        trb->command = 0;
        break;
      case DEVICE_NOTIFICATION:
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_DATA_PTR])));
        trb->parameter = (Bit64u) strtol(str, NULL, 0) << 8;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_NOT_TYPE])));
        trb->parameter |= (strtol(str, NULL, 0) & 0xF) << 4;
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_COMP_CODE])));
        trb->status = TRB_SET_COMP_CODE(strtol(str, NULL, 0));
        strcpy(str, gtk_entry_get_text(GTK_ENTRY(TRBitem[ID_TRB_SLOT_ID])));
        trb->command = TRB_SET_SLOT(strtol(str, NULL, 0));
        break;
      default:
        xhci_message_dialog(GTK_WINDOW(trb_dialog),
                            "Saving changes to TRB type not supported yet");
    }
    if (TRBitem[ID_TRB_C] != NULL) {
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(TRBitem[ID_TRB_C]))) {
        trb->command |= (1<<0);
      }
    }
  }
  gtk_widget_destroy(trb_dialog);
  return (ret == GTK_RESPONSE_OK);
}

static void xhci_display_trb(GtkWidget *widget, gpointer data)
{
  GtkTreeSelection *selection;
  GtkTreeModel     *model;
  GtkTreeIter       iter;
  int type_mask = 0;
  gulong value;
  struct TRB trb;
  char str[COMMON_STR_SIZE];

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(data));
  if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
    switch (usbdbg_break_type) {
      case USB_DEBUG_COMMAND:
        type_mask = VIEW_TRB_TYPE_COMMAND;
        break;
      case USB_DEBUG_DATA:
        type_mask = VIEW_TRB_TYPE_TRANSFER;
        break;
      case USB_DEBUG_EVENT:
        type_mask = VIEW_TRB_TYPE_EVENT;
        break;
      case USB_DEBUG_FRAME:
        type_mask = VIEW_TRB_TYPE_TRANSFER;
        break;
    }
    gtk_tree_model_get(model, &iter, 1, &value, -1);
    if (value > 0) {
      DEV_MEM_READ_PHYSICAL(value, sizeof(struct TRB), (Bit8u *) &trb);
      const Bit8u type = TRB_GET_TYPE(trb.command);

      // check to see if this type of TRB is allowed in this type of ring
      if ((type > 0) && (type <= 47) && ((trb_types[type].allowed_mask & type_mask) == 0)) {
        sprintf(str, "TRB type %i not allowed in a %s ring!", type, ring_type[type_mask]);
        xhci_message_dialog(GTK_WINDOW(gtk_widget_get_toplevel(widget)), (const char*)str);
      }
      if ((type > 0) && (type < 40) && (trb_types[type].allowed_mask != VIEW_TRB_TYPE_NONE)) {
        if (xhci_view_trb_dialog(type, &trb)) {
          // make sure the type is still correct
          trb.command &= ~(0x3F << 10);
          trb.command |= TRB_SET_TYPE(type);
          // write back the trb
          DEV_MEM_WRITE_PHYSICAL(value, sizeof(struct TRB), (Bit8u *) &trb);
        }
      } else {
        sprintf(str, "Unsupported or Reserved TRB type %i found!", type);
        xhci_message_dialog(GTK_WINDOW(gtk_widget_get_toplevel(widget)), (const char*)str);
      }
    } else {
      xhci_message_dialog(GTK_WINDOW(gtk_widget_get_toplevel(widget)), "Item selected has no TRB attached");
    }
  } else {
    xhci_message_dialog(GTK_WINDOW(gtk_widget_get_toplevel(widget)), "No TRB selected");
  }
}

// xHCI main dialog

int xhci_debug_dialog(int type, int param1)
{
  int i, n_ports, ret;
  Bit32u dword, offset;
  Bit64u RingPtr = 0;
  char buffer[COMMON_STR_SIZE], tmpbuf[32];
  GtkWidget *mainVbox, *mainGrid;
  GtkWidget *CRframe, *CRgrid;
  GtkWidget *ORframe, *ORgrid;
  GtkWidget *RTframe, *RTgrid;
  GtkWidget *PRframe, *PRgrid;
  GtkWidget *entry[38], *button[13], *FNlabel;
  GtkWidget *treeview, *scrlwin, *FNgrid;
  usb_reg_t xhci_reg_def[10] = {
    {"Port 0 Register", NULL, 8, attribs_x_ports},
    {"Port 1 Register", NULL, 8, attribs_x_ports},
    {"Port 2 Register", NULL, 8, attribs_x_ports},
    {"Port 3 Register", NULL, 8, attribs_x_ports},
    {"Port 4 Register", NULL, 8, attribs_x_ports},
    {"Port 5 Register", NULL, 8, attribs_x_ports},
    {"Port 6 Register", NULL, 8, attribs_x_ports},
    {"Port 7 Register", NULL, 8, attribs_x_ports},
    {"Port 8 Register", NULL, 8, attribs_x_ports},
    {"Port 9 Register", NULL, 8, attribs_x_ports}
  };

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
  gtk_window_set_keep_above(GTK_WINDOW(main_dialog), TRUE);
  button[0] = gtk_dialog_add_button(GTK_DIALOG(main_dialog), "Continue", GTK_RESPONSE_OK);
  button[1] = gtk_dialog_add_button(GTK_DIALOG(main_dialog), "Quit", GTK_RESPONSE_CANCEL);
  gtk_dialog_set_default_response(GTK_DIALOG(main_dialog), GTK_RESPONSE_OK);
  gtk_window_set_focus(GTK_WINDOW(main_dialog), button[0]);
  mainVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(main_dialog))), mainVbox, TRUE, TRUE, 2);
  mainGrid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(mainGrid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(mainGrid), 5);
  gtk_container_add(GTK_CONTAINER(mainVbox), mainGrid);

  entry[0] = usbdlg_create_ro_entry_with_label(mainGrid, "xHCI at Base MMIO address", 0, 0);
  CRframe = gtk_frame_new("Capability Registers");
  gtk_grid_attach(GTK_GRID(mainGrid), CRframe, 0, 1, 3, 9);
  CRgrid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(CRgrid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(CRgrid), 5);
  gtk_container_add(GTK_CONTAINER(CRframe), CRgrid);
  gtk_container_set_border_width(GTK_CONTAINER(CRgrid), 5);
  entry[1] = usbdlg_create_entry_with_label(CRgrid, "Cap Length", 0, 0);
  entry[2] = usbdlg_create_entry_with_label(CRgrid, "HCSParams1", 0, 1);
  entry[3] = usbdlg_create_ro_entry_with_label(CRgrid, "HCSParams2", 0, 2);
  entry[4] = usbdlg_create_entry_with_label(CRgrid, "HCSParams3", 0, 3);
  entry[5] = usbdlg_create_entry_with_label(CRgrid, "HCCParams1", 0, 4);
  entry[6] = usbdlg_create_entry_with_label(CRgrid, "DB Offset", 0, 5);
  entry[7] = usbdlg_create_entry_with_label(CRgrid, "RTS Offset", 0, 6);
  entry[8] = usbdlg_create_entry_with_label(CRgrid, "HCCParams2", 0, 7);

  ORframe = gtk_frame_new("Operational Registers");
  gtk_grid_attach(GTK_GRID(mainGrid), ORframe, 3, 1, 2, 8);
  ORgrid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(ORgrid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(ORgrid), 5);
  gtk_container_add(GTK_CONTAINER(ORframe), ORgrid);
  gtk_container_set_border_width(GTK_CONTAINER(ORgrid), 5);
  entry[9] = usbdlg_create_ro_entry_with_label(ORgrid, "Command", 0, 0);
  entry[10] = usbdlg_create_entry_with_label(ORgrid, "Status", 0, 1);
  entry[11] = usbdlg_create_entry_with_label(ORgrid, "Page Size", 0, 2);
  entry[12] = usbdlg_create_entry_with_label(ORgrid, "Device Notification", 0, 3);
  entry[13] = usbdlg_create_entry_with_label(ORgrid, "Command Ring", 0, 4);
  entry[14] = usbdlg_create_entry_with_label(ORgrid, "Device Context Base", 0, 5);
  entry[15] = usbdlg_create_entry_with_label(ORgrid, "Configure", 0, 6);

  RTframe = gtk_frame_new("Runtime Registers");
  gtk_grid_attach(GTK_GRID(mainGrid), RTframe, 5, 1, 2, 1);
  RTgrid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(RTgrid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(RTgrid), 5);
  gtk_container_add(GTK_CONTAINER(RTframe), RTgrid);
  gtk_container_set_border_width(GTK_CONTAINER(RTgrid), 5);
  entry[16] = usbdlg_create_entry_with_label(RTgrid, "Microframe Index", 0, 0);

  PRframe = gtk_frame_new("Port Registers");
  n_ports = SIM->get_param_num(BXPN_XHCI_N_PORTS)->get();
  gtk_grid_attach(GTK_GRID(mainGrid), PRframe, 0, 10, 3, n_ports * 2);
  PRgrid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(PRgrid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(PRgrid), 5);
  gtk_container_add(GTK_CONTAINER(PRframe), PRgrid);
  gtk_container_set_border_width(GTK_CONTAINER(PRgrid), 5);
  for (i = 0; i < n_ports; i++) {
    sprintf(buffer, "Port %d", i);
    entry[i * 2 + 17] = usbdlg_create_entry_with_label(PRgrid, buffer, 0, i * 2);
    entry[i * 2 + 18] = usbdlg_create_ro_entry_with_label(PRgrid, "Emulation Type", 0, i * 2 + 1);
    button[i + 2] = gtk_button_new_with_label("<>");
    xhci_reg_def[i].entry = entry[i * 2 + 17];
    g_signal_connect(button[i + 2], "clicked", G_CALLBACK(usb_regview_dialog), &xhci_reg_def[i]);
    gtk_grid_attach(GTK_GRID(PRgrid), button[i + 2], 2, i * 2, 1, 1);
  }
  FNgrid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(FNgrid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(FNgrid), 5);
  gtk_container_set_border_width(GTK_CONTAINER(FNgrid), 5);
  gtk_grid_attach(GTK_GRID(mainGrid), FNgrid, 3, 9, 3, 1);
  FNlabel = usbdlg_create_label(FNgrid, "Ring Address", 0, 0);
  entry[n_ports * 2 + 17] = usbdlg_create_ro_entry(FNgrid, 1, 0);
  button[12] = gtk_button_new_with_label("View TRB");
  gtk_widget_set_sensitive(button[12], 0);
  gtk_grid_attach(GTK_GRID(FNgrid), button[12], 2, 0, 1, 1);
  scrlwin = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrlwin), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_grid_attach(GTK_GRID(mainGrid), scrlwin, 3, 10, 12, 10);
  treeview = gtk_tree_view_new();
  gtk_container_add(GTK_CONTAINER(scrlwin), treeview);
  usbdlg_create_apply_button(mainGrid, 1, n_ports * 2 + 10);
  usbdlg_create_debug_flags(mainGrid, 7, 1);
  // Set values
  pci_bar_address = get_pci_bar_addr((bx_shadow_data_c*)SIM->get_param("hub.pci_conf", xHCI_state), 0);
  sprintf(buffer, "0x%08X", pci_bar_address);
  gtk_entry_set_text(GTK_ENTRY(entry[0]), buffer);
  for (i = 0; i < 8; i++) {
    dword = xhci_read_dword(pci_bar_address + (i * 4));
    sprintf(buffer, "0x%08X", dword);
    gtk_entry_set_text(GTK_ENTRY(entry[i + 1]), buffer);
    g_signal_connect(GTK_EDITABLE(entry[i + 1]), "changed", G_CALLBACK(on_entry_changed), NULL);
  }
  offset = xhci_read_dword(pci_bar_address + 0) & 0xFF;
  dword = xhci_read_dword(pci_bar_address + offset + 0x00);
  sprintf(buffer, "0x%08X", dword);
  gtk_entry_set_text(GTK_ENTRY(entry[9]), buffer);
  g_signal_connect(GTK_EDITABLE(entry[9]), "changed", G_CALLBACK(on_entry_changed), NULL);
  dword = xhci_read_dword(pci_bar_address + offset + 0x04);
  sprintf(buffer, "0x%08X", dword);
  gtk_entry_set_text(GTK_ENTRY(entry[10]), buffer);
  g_signal_connect(GTK_EDITABLE(entry[10]), "changed", G_CALLBACK(on_entry_changed), NULL);
  dword = xhci_read_dword(pci_bar_address + offset + 0x08);
  sprintf(buffer, "0x%08X", dword);
  gtk_entry_set_text(GTK_ENTRY(entry[11]), buffer);
  g_signal_connect(GTK_EDITABLE(entry[11]), "changed", G_CALLBACK(on_entry_changed), NULL);
  dword = xhci_read_dword(pci_bar_address + offset + 0x14);
  sprintf(buffer, "0x%08X", dword);
  gtk_entry_set_text(GTK_ENTRY(entry[12]), buffer);
  g_signal_connect(GTK_EDITABLE(entry[12]), "changed", G_CALLBACK(on_entry_changed), NULL);
  // we can't read this using DEV_MEM_READ_PHYSICAL since the handler will return zero
  sprintf(buffer, "0x" FMT_ADDRX64, SIM->get_param_num("hub.op_regs.HcCrcr.actual", xHCI_state)->get64());
  gtk_entry_set_text(GTK_ENTRY(entry[13]), buffer);
  g_signal_connect(GTK_EDITABLE(entry[13]), "changed", G_CALLBACK(on_entry_changed), NULL);
  Bit64u qword = xhci_read_dword(pci_bar_address + offset + 0x30) |
   ((Bit64u) xhci_read_dword(pci_bar_address + offset + 0x34) << 32);
  sprintf(buffer, "0x" FMT_ADDRX64, qword);
  gtk_entry_set_text(GTK_ENTRY(entry[14]), buffer);
  g_signal_connect(GTK_EDITABLE(entry[14]), "changed", G_CALLBACK(on_entry_changed), NULL);
  dword = xhci_read_dword(pci_bar_address + offset + 0x38);
  sprintf(buffer, "0x%08X", dword);
  gtk_entry_set_text(GTK_ENTRY(entry[15]), buffer);
  g_signal_connect(GTK_EDITABLE(entry[15]), "changed", G_CALLBACK(on_entry_changed), NULL);

  offset = xhci_read_dword(pci_bar_address + 0x18);
  dword = xhci_read_dword(pci_bar_address + offset + 0);
  sprintf(buffer, "0x%08X", dword);
  gtk_entry_set_text(GTK_ENTRY(entry[16]), buffer);
  g_signal_connect(GTK_EDITABLE(entry[16]), "changed", G_CALLBACK(on_entry_changed), NULL);
  // show up to 10 port register sets
  for (i = 0; i < n_ports; i++) {
    dword = xhci_read_dword(pci_bar_address + XHCI_PORT_SET_OFFSET + (i * 16));
    sprintf(buffer, "0x%08X", dword);
    gtk_entry_set_text(GTK_ENTRY(entry[i * 2 + 17]), buffer);
    g_signal_connect(GTK_EDITABLE(entry[i * 2 + 17]), "changed", G_CALLBACK(on_entry_changed), NULL);
    sprintf(tmpbuf, "port%d.device", i + 1);
    SIM->get_param_enum(tmpbuf, host_param)->dump_param(buffer, COMMON_STR_SIZE, 1);
    gtk_entry_set_text(GTK_ENTRY(entry[i * 2 + 18]), buffer);
  }

  bool valid = 0;
  switch (type) {
    // a command TRB was placed on the command ring
    case USB_DEBUG_COMMAND:
      gtk_label_set_text(GTK_LABEL(FNlabel), "Command Ring Address");
      RingPtr = SIM->get_param_num("hub.op_regs.HcCrcr.crc", xHCI_state)->get();
      sprintf(buffer, "0x" FMT_ADDRX64, RingPtr);
      gtk_entry_set_text(GTK_ENTRY(entry[n_ports * 2 + 17]), buffer);
      if (RingPtr != 0) {
        hc_xhci_do_ring(treeview, "Command", RingPtr, SIM->get_param_num("hub.ring_members.command_ring.dq_pointer", xHCI_state)->get());
        gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview));
        g_signal_connect(button[12], "clicked", G_CALLBACK(xhci_display_trb), treeview);
        gtk_widget_set_sensitive(button[12], 1);
        valid = 1;
      }
      break;

    // an event TRB was placed on an event ring
    case USB_DEBUG_EVENT:
      gtk_label_set_text(GTK_LABEL(FNlabel), "Event Ring");
      sprintf(buffer, "Interrupter %i", param1);
      gtk_entry_set_text(GTK_ENTRY(entry[n_ports * 2 + 17]), buffer);
      hc_xhci_do_event_ring(treeview, "Event", (int) param1);
      gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview));
      g_signal_connect(button[12], "clicked", G_CALLBACK(xhci_display_trb), treeview);
      gtk_widget_set_sensitive(button[12], 1);
      valid = 1;
      break;

    // a TRB was placed on a data ring
    case USB_DEBUG_DATA:
      gtk_label_set_text(GTK_LABEL(FNlabel), "Data Ring Address");
      RingPtr = usbdbg_param0;
      sprintf(buffer, "0x" FMT_ADDRX64, RingPtr);
      gtk_entry_set_text(GTK_ENTRY(entry[n_ports * 2 + 17]), buffer);
      if (RingPtr != 0) {
        hc_xhci_do_ring(treeview, "Data", RingPtr, RingPtr);
        g_signal_connect(button[12], "clicked", G_CALLBACK(xhci_display_trb), treeview);
        gtk_widget_set_sensitive(button[12], 1);
        valid = 1;
      }
      break;

    case USB_DEBUG_FRAME:

      gtk_label_set_text(GTK_LABEL(FNlabel), "SOF Frame Address");

      break;

    // first byte (word, dword, qword) of first non-existant port was written to
    case USB_DEBUG_NONEXIST:
    // port reset (non-root reset)
    case USB_DEBUG_RESET:
    // enable changed
    case USB_DEBUG_ENABLE:
      gtk_label_set_text(GTK_LABEL(FNlabel), "None");
      gtk_entry_set_text(GTK_ENTRY(entry[n_ports * 2 + 17]), "None");
      break;
  }

  if (!valid) {
    gtk_widget_set_sensitive(treeview, 0);
    gtk_grid_attach(GTK_GRID(mainGrid), gtk_label_new("This trigger does not populate the tree view"), 4, 21, 1, 1);
  } else {
    gtk_grid_attach(GTK_GRID(mainGrid), gtk_label_new("Tree view populated"), 4, 21, 1, 1);
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

// USB debug dialog entry point

int usb_debug_dialog(int type, Bit64u param0, int param1, int param2)
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
  usbdbg_break_type = type;
  usbdbg_param0 = param0;
  usbdbg_param1 = param1;
  usbdbg_param2 = param2;
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
