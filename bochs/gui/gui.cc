/////////////////////////////////////////////////////////////////////////
// $Id: gui.cc,v 1.53 2002-10-24 21:06:20 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA


#include <signal.h>
#include "bochs.h"
#include "gui/bitmaps/floppya.h"
#include "gui/bitmaps/floppyb.h"
#include "gui/bitmaps/mouse.h"
#include "gui/bitmaps/reset.h"
#include "gui/bitmaps/power.h"
#include "gui/bitmaps/snapshot.h"
#include "gui/bitmaps/copy.h"
#include "gui/bitmaps/paste.h"
#include "gui/bitmaps/configbutton.h"
#include "gui/bitmaps/cdromd.h"
#include "gui/bitmaps/userbutton.h"
#if BX_WITH_MACOS
#  include <Disks.h>
#endif

bx_gui_c *bx_gui = NULL;

#define BX_GUI_THIS bx_gui->
#define LOG_THIS BX_GUI_THIS

bx_gui_c::bx_gui_c(void)
{
  put("GUI"); // Init in specific_init
  settype(GUILOG);
}

bx_gui_c::~bx_gui_c()
{
}

  void
bx_gui_c::init(int argc, char **argv, unsigned tilewidth, unsigned tileheight)
{
  specific_init(argc, argv, tilewidth, tileheight, BX_HEADER_BAR_Y);

  // Define some bitmaps to use in the headerbar
  BX_GUI_THIS floppyA_bmap_id = create_bitmap(bx_floppya_bmap,
                          BX_FLOPPYA_BMAP_X, BX_FLOPPYA_BMAP_Y);
  BX_GUI_THIS floppyA_eject_bmap_id = create_bitmap(bx_floppya_eject_bmap,
                          BX_FLOPPYA_BMAP_X, BX_FLOPPYA_BMAP_Y);
  BX_GUI_THIS floppyB_bmap_id = create_bitmap(bx_floppyb_bmap,
                          BX_FLOPPYB_BMAP_X, BX_FLOPPYB_BMAP_Y);
  BX_GUI_THIS floppyB_eject_bmap_id = create_bitmap(bx_floppyb_eject_bmap,
                          BX_FLOPPYB_BMAP_X, BX_FLOPPYB_BMAP_Y);
  BX_GUI_THIS cdromD_bmap_id = create_bitmap(bx_cdromd_bmap,
                          BX_CDROMD_BMAP_X, BX_CDROMD_BMAP_Y);
  BX_GUI_THIS cdromD_eject_bmap_id = create_bitmap(bx_cdromd_eject_bmap,
                          BX_CDROMD_BMAP_X, BX_CDROMD_BMAP_Y);
  BX_GUI_THIS mouse_bmap_id = create_bitmap(bx_mouse_bmap,
                          BX_MOUSE_BMAP_X, BX_MOUSE_BMAP_Y);
  BX_GUI_THIS nomouse_bmap_id = create_bitmap(bx_nomouse_bmap,
                          BX_MOUSE_BMAP_X, BX_MOUSE_BMAP_Y);


  BX_GUI_THIS power_bmap_id = create_bitmap(bx_power_bmap, BX_POWER_BMAP_X, BX_POWER_BMAP_Y);
  BX_GUI_THIS reset_bmap_id = create_bitmap(bx_reset_bmap, BX_RESET_BMAP_X, BX_RESET_BMAP_Y);
  BX_GUI_THIS snapshot_bmap_id = create_bitmap(bx_snapshot_bmap, BX_SNAPSHOT_BMAP_X, BX_SNAPSHOT_BMAP_Y);
  BX_GUI_THIS copy_bmap_id = create_bitmap(bx_copy_bmap, BX_COPY_BMAP_X, BX_COPY_BMAP_Y);
  BX_GUI_THIS paste_bmap_id = create_bitmap(bx_paste_bmap, BX_PASTE_BMAP_X, BX_PASTE_BMAP_Y);
  BX_GUI_THIS config_bmap_id = create_bitmap(bx_config_bmap, BX_CONFIG_BMAP_X, BX_CONFIG_BMAP_Y);
  BX_GUI_THIS user_bmap_id = create_bitmap(bx_user_bmap, BX_USER_BMAP_X, BX_USER_BMAP_Y);


  // Add the initial bitmaps to the headerbar, and enable callback routine, for use
  // when that bitmap is clicked on

  // Floppy A:
  BX_GUI_THIS floppyA_status = DEV_floppy_get_media_status(0);
  if (BX_GUI_THIS floppyA_status)
    BX_GUI_THIS floppyA_hbar_id = headerbar_bitmap(BX_GUI_THIS floppyA_bmap_id,
                          BX_GRAVITY_LEFT, floppyA_handler);
  else
    BX_GUI_THIS floppyA_hbar_id = headerbar_bitmap(BX_GUI_THIS floppyA_eject_bmap_id,
                          BX_GRAVITY_LEFT, floppyA_handler);

  // Floppy B:
  BX_GUI_THIS floppyB_status = DEV_floppy_get_media_status(1);
  if (BX_GUI_THIS floppyB_status)
    BX_GUI_THIS floppyB_hbar_id = headerbar_bitmap(BX_GUI_THIS floppyB_bmap_id,
                          BX_GRAVITY_LEFT, floppyB_handler);
  else
    BX_GUI_THIS floppyB_hbar_id = headerbar_bitmap(BX_GUI_THIS floppyB_eject_bmap_id,
                          BX_GRAVITY_LEFT, floppyB_handler);

  // CDROM
  if (DEV_hd_present()) {
    Bit32u handle = DEV_hd_get_first_cd_handle();
    BX_GUI_THIS cdromD_status = DEV_hd_get_cd_media_status(handle);
  }

  if (BX_GUI_THIS cdromD_status)
    BX_GUI_THIS cdromD_hbar_id = headerbar_bitmap(BX_GUI_THIS cdromD_bmap_id,
                          BX_GRAVITY_LEFT, cdromD_handler);
  else
    BX_GUI_THIS cdromD_hbar_id = headerbar_bitmap(BX_GUI_THIS cdromD_eject_bmap_id,
                          BX_GRAVITY_LEFT, cdromD_handler);

  // Mouse button
  if (bx_options.Omouse_enabled->get ())
    BX_GUI_THIS mouse_hbar_id = headerbar_bitmap(BX_GUI_THIS mouse_bmap_id,
                          BX_GRAVITY_LEFT, toggle_mouse_enable);
  else
    BX_GUI_THIS mouse_hbar_id = headerbar_bitmap(BX_GUI_THIS nomouse_bmap_id,
                          BX_GRAVITY_LEFT, toggle_mouse_enable);

  // These are the buttons on the right side.  They are created in order
  // of right to left.

  // Power button
  BX_GUI_THIS power_hbar_id = headerbar_bitmap(BX_GUI_THIS power_bmap_id,
                          BX_GRAVITY_RIGHT, power_handler);
  // Reset button
  BX_GUI_THIS reset_hbar_id = headerbar_bitmap(BX_GUI_THIS reset_bmap_id,
                          BX_GRAVITY_RIGHT, reset_handler);
  // Configure button
  BX_GUI_THIS config_hbar_id = headerbar_bitmap(BX_GUI_THIS config_bmap_id,
                          BX_GRAVITY_RIGHT, config_handler);
  // Snapshot button
  BX_GUI_THIS snapshot_hbar_id = headerbar_bitmap(BX_GUI_THIS snapshot_bmap_id,
                          BX_GRAVITY_RIGHT, snapshot_handler);
  // Paste button
  BX_GUI_THIS paste_hbar_id = headerbar_bitmap(BX_GUI_THIS paste_bmap_id,
                          BX_GRAVITY_RIGHT, paste_handler);
  // Copy button
  BX_GUI_THIS copy_hbar_id = headerbar_bitmap(BX_GUI_THIS copy_bmap_id,
                          BX_GRAVITY_RIGHT, copy_handler);
  // User button
  BX_GUI_THIS user_hbar_id = headerbar_bitmap(BX_GUI_THIS user_bmap_id,
                          BX_GRAVITY_RIGHT, userbutton_handler);

  show_headerbar();
}

void
bx_gui_c::update_drive_status_buttons (void) {
  BX_GUI_THIS floppyA_status = 
    DEV_floppy_get_media_status(0)
    && bx_options.floppya.Ostatus->get ();
  BX_GUI_THIS floppyB_status = 
      DEV_floppy_get_media_status(1)
      && bx_options.floppyb.Ostatus->get ();
  Bit32u handle = DEV_hd_get_first_cd_handle();
  BX_GUI_THIS cdromD_status = DEV_hd_get_cd_media_status(handle);
  if (BX_GUI_THIS floppyA_status)
    replace_bitmap(BX_GUI_THIS floppyA_hbar_id, BX_GUI_THIS floppyA_bmap_id);
  else {
#if BX_WITH_MACOS
    // If we are using the Mac floppy driver, eject the disk
    // from the floppy drive.  This doesn't work in MacOS X.
    if (!strcmp(bx_options.floppya.Opath->get (), SuperDrive))
      DiskEject(1);
#endif
    replace_bitmap(BX_GUI_THIS floppyA_hbar_id, BX_GUI_THIS floppyA_eject_bmap_id);
    }
  if (BX_GUI_THIS floppyB_status)
    replace_bitmap(BX_GUI_THIS floppyB_hbar_id, BX_GUI_THIS floppyB_bmap_id);
  else {
#if BX_WITH_MACOS
    // If we are using the Mac floppy driver, eject the disk
    // from the floppy drive.  This doesn't work in MacOS X.
    if (!strcmp(bx_options.floppyb.Opath->get (), SuperDrive))
      DiskEject(1);
#endif
    replace_bitmap(BX_GUI_THIS floppyB_hbar_id, BX_GUI_THIS floppyB_eject_bmap_id);
    }
  if (BX_GUI_THIS cdromD_status)
    replace_bitmap(BX_GUI_THIS cdromD_hbar_id, BX_GUI_THIS cdromD_bmap_id);
  else {
    replace_bitmap(BX_GUI_THIS cdromD_hbar_id, BX_GUI_THIS cdromD_eject_bmap_id);
    }
}

  void
bx_gui_c::floppyA_handler(void)
{
#if BX_WITH_WX
  // instead of just toggling the status, call wxWindows to bring up 
  // a dialog asking what disk image you want to switch to.
  int ret = SIM->ask_param (BXP_FLOPPYA_PATH);
  if (ret < 0) return;  // cancelled
  // eject and then insert the disk.  If the new path is invalid,
  // the status will return 0.
  unsigned new_status = DEV_floppy_set_media_status(0, 0);
  printf ("eject disk, new_status is %d\n", new_status);
  new_status = DEV_floppy_set_media_status(0, 1);
  printf ("insert disk, new_status is %d\n", new_status);
  fflush (stdout);
  BX_GUI_THIS floppyA_status = new_status;
#else
  BX_GUI_THIS floppyA_status = !BX_GUI_THIS floppyA_status;
  DEV_floppy_set_media_status(0, BX_GUI_THIS floppyA_status);
#endif
  BX_GUI_THIS update_drive_status_buttons ();
}

  void
bx_gui_c::floppyB_handler(void)
{
#if BX_WITH_WX
  // instead of just toggling the status, call wxWindows to bring up 
  // a dialog asking what disk image you want to switch to.
  int ret = SIM->ask_param (BXP_FLOPPYB_PATH);
  if (ret < 0) return;  // cancelled
  // eject and then insert the disk.  If the new path is invalid,
  // the status will return 0.
  unsigned new_status = DEV_floppy_set_media_status(1, 0);
  printf ("eject disk, new_status is %d\n", new_status);
  new_status = DEV_floppy_set_media_status(1, 1);
  printf ("insert disk, new_status is %d\n", new_status);
  fflush (stdout);
  BX_GUI_THIS floppyB_status = new_status;
#else
  BX_GUI_THIS floppyB_status = !BX_GUI_THIS floppyB_status;
  DEV_floppy_set_media_status(1, BX_GUI_THIS floppyB_status);
#endif
  BX_GUI_THIS update_drive_status_buttons ();
}

  void
bx_gui_c::cdromD_handler(void)
{
  Bit32u handle = DEV_hd_get_first_cd_handle();
#if BX_WITH_WX
  // instead of just toggling the status, call wxWindows to bring up 
  // a dialog asking what disk image you want to switch to.
  // BBD: for now, find the first cdrom and call ask_param on that.
  // Since we could have multiple cdroms now, maybe we should be adding
  // one cdrom button for each?
  bx_param_c *cdrom = SIM->get_first_cdrom ();
  if (cdrom == NULL)
    return;  // no cdrom found
  int ret = SIM->ask_param (cdrom->get_id ());
  if (ret < 0) return;  // cancelled
  // eject and then insert the disk.  If the new path is invalid,
  // the status will return 0.
  unsigned status = DEV_hd_set_cd_media_status(handle, 0);
  printf ("eject disk, new_status is %d\n", status);
  status = DEV_hd_set_cd_media_status(handle, 1);
  printf ("insert disk, new_status is %d\n", status);
  fflush (stdout);
  BX_GUI_THIS cdromD_status = status;
#else
  BX_GUI_THIS cdromD_status =
    DEV_hd_set_cd_media_status(handle, !BX_GUI_THIS cdromD_status);
#endif
  BX_GUI_THIS update_drive_status_buttons ();
}

  void
bx_gui_c::reset_handler(void)
{
  BX_INFO(( "system RESET callback." ));
  bx_pc_system.ResetSignal( PCS_SET ); /* XXX is this right? */
  for (int i=0; i<BX_SMP_PROCESSORS; i++)
      BX_CPU(i)->reset(BX_RESET_HARDWARE);
}

  void
bx_gui_c::power_handler(void)
{
  // the user pressed power button, so there's no doubt they want bochs
  // to quit.  Change panics to fatal for the GUI and then do a panic.
  LOG_THIS setonoff(LOGLEV_PANIC, ACT_FATAL);
  BX_PANIC (("POWER button turned off."));
  // shouldn't reach this point, but if you do, QUIT!!!
  fprintf (stderr, "Bochs is exiting because you pressed the power button.\n");
  BX_EXIT (1);
}

Bit32s
bx_gui_c::make_text_snapshot (char **snapshot, Bit32u *length)
{
  Bit8u* raw_snap = NULL;
  char *clean_snap;
  unsigned line_addr, txt_addr, txHeight, txWidth;

  DEV_vga_get_text_snapshot(&raw_snap, &txHeight, &txWidth);
  if (txHeight <= 0) return -1;
  clean_snap = (char*) malloc(txHeight*(txWidth+2)+1);
  txt_addr = 0;
  for (unsigned i=0; i<txHeight; i++) {
    line_addr = i * txWidth * 2;
    for (unsigned j=0; j<(txWidth*2); j+=2) {
      clean_snap[txt_addr++] = raw_snap[line_addr+j];
    }
  while ((txt_addr > 0) && (clean_snap[txt_addr-1] == ' ')) txt_addr--;
#ifdef WIN32
    clean_snap[txt_addr++] = 13;
#endif
    clean_snap[txt_addr++] = 10;
  }
  clean_snap[txt_addr] = 0;
  *snapshot = clean_snap;
  *length = txt_addr;
  return 0;
}

// create a text snapshot and copy to the system clipboard.  On guis that
// we haven't figured out how to support yet, dump to a file instead.
  void
bx_gui_c::copy_handler(void)
{
  Bit32u len;
  char *text_snapshot;
  if (make_text_snapshot (&text_snapshot, &len) < 0) {
    BX_INFO(( "copy button failed, mode not implemented"));
    return;
  }
  if (!BX_GUI_THIS set_clipboard_text(text_snapshot, len)) {
    // platform specific code failed, use portable code instead
    FILE *fp = fopen("copy.txt", "w");
    fwrite(text_snapshot, 1, strlen(text_snapshot), fp);
    fclose(fp);
  }
  free(text_snapshot);
}

// create a text snapshot and dump it to a file
  void
bx_gui_c::snapshot_handler(void)
{
  char *text_snapshot;
  Bit32u len;
  if (make_text_snapshot (&text_snapshot, &len) < 0) {
    BX_ERROR(( "snapshot button failed, mode not implemented"));
    return;
  }
  //FIXME
  char filename[BX_PATHNAME_LEN];
#if BX_WITH_WX
  int ret = SIM->ask_filename (filename, sizeof(filename),
    "Save snapshot as...", "snapshot.txt",
	bx_param_string_c::SAVE_FILE_DIALOG);
  if (ret < 0) { // cancelled
    free(text_snapshot);
    return;
  }
#else
  strcpy (filename, "snapshot.txt");
#endif
  FILE *fp = fopen(filename, "wb");
  fwrite(text_snapshot, 1, strlen(text_snapshot), fp);
  fclose(fp);
  free(text_snapshot);
}

// Read ASCII chars from the system clipboard and paste them into bochs.
// Note that paste cannot work with the key mapping tables loaded.
  void
bx_gui_c::paste_handler(void)
{
  Bit32s nbytes;
  Bit8u *bytes;
  if (!bx_keymap.isKeymapLoaded ()) {
    BX_ERROR (("keyboard_mapping disabled, so paste cannot work"));
    return;
  }
  if (!BX_GUI_THIS get_clipboard_text(&bytes, &nbytes)) {
    BX_ERROR (("paste not implemented on this platform"));
    return;
  }
  BX_INFO (("pasting %d bytes", nbytes));
  DEV_kbd_paste_bytes (bytes, nbytes);
}


  void
bx_gui_c::config_handler(void)
{
  SIM->configuration_interface (NULL, CI_RUNTIME_CONFIG);
}

  void
bx_gui_c::toggle_mouse_enable(void)
{
  int old = bx_options.Omouse_enabled->get ();
  BX_DEBUG (("toggle mouse_enabled, now %d", !old));
  bx_options.Omouse_enabled->set (!old);
}

  void
bx_gui_c::userbutton_handler(void)
{
  unsigned shortcut[4];
  unsigned p;
  char *user_shortcut;
  int i, len;

  len = 0;
#if BX_WITH_WX
  int ret = SIM->ask_param (BXP_USER_SHORTCUT);
  user_shortcut = bx_options.Ouser_shortcut->getptr();
  if ((ret > 0) && user_shortcut[0] && (strcmp(user_shortcut, "none"))) {
#else
  user_shortcut = bx_options.Ouser_shortcut->getptr();
  if (user_shortcut[0] && (strcmp(user_shortcut, "none"))) {
#endif
    len = 0;
    p = 0;
    while ((p < strlen(user_shortcut)) && (len < 3)) {
      if (!strncmp(user_shortcut+p, "alt", 3)) {
        shortcut[len] = BX_KEY_ALT_L;
        len++;
        p += 3;
      } else if (!strncmp(user_shortcut+p, "ctrl", 4)) {
        shortcut[len] = BX_KEY_CTRL_L;
        len++;
        p += 4;
      } else if (!strncmp(user_shortcut+p, "del", 3)) {
        shortcut[len] = BX_KEY_DELETE;
        len++;
        p += 3;
      } else if (!strncmp(user_shortcut+p, "esc", 3)) {
        shortcut[len] = BX_KEY_ESC;
        len++;
        p += 3;
      } else if (!strncmp(user_shortcut+p, "f1", 2)) {
        shortcut[len] = BX_KEY_F1;
        len++;
        p += 2;
      } else if (!strncmp(user_shortcut+p, "f4", 2)) {
        shortcut[len] = BX_KEY_F4;
        len++;
        p += 2;
      } else if (!strncmp(user_shortcut+p, "tab", 3)) {
        shortcut[len] = BX_KEY_TAB;
        len++;
        p += 3;
      } else if (!strncmp(user_shortcut+p, "win", 3)) {
        shortcut[len] = BX_KEY_WIN_L;
        len++;
        p += 3;
      } else {
        BX_ERROR(("Unknown shortcut %s ignored", user_shortcut));
      }
    }
    i = 0;
    while (i < len) {
      DEV_kbd_gen_scancode(shortcut[i++]);
    }
    i--;
    while (i >= 0) {
      DEV_kbd_gen_scancode(shortcut[i--] | BX_KEY_RELEASED);
    }
  }
}

  void
bx_gui_c::mouse_enabled_changed (Boolean val)
{
  // This is only called when SIM->get_init_done is 1.  Note that VAL
  // is the new value of mouse_enabled, which may not match the old
  // value which is still in bx_options.Omouse_enabled->get ().
  BX_DEBUG (("replacing the mouse bitmaps"));
  if (val)
    BX_GUI_THIS replace_bitmap(BX_GUI_THIS mouse_hbar_id, BX_GUI_THIS mouse_bmap_id);
  else
    BX_GUI_THIS replace_bitmap(BX_GUI_THIS mouse_hbar_id, BX_GUI_THIS nomouse_bmap_id);
  // give the GUI a chance to respond to the event.  Most guis will hide
  // the native mouse cursor and do something to trap the mouse inside the
  // bochs VGA display window.
  BX_GUI_THIS mouse_enabled_changed_specific (val);
}

void
bx_gui_c::init_signal_handlers ()
{
#if BX_GUI_SIGHANDLER
  Bit32u mask = bx_gui->get_sighandler_mask ();
  for (Bit32u sig=0; sig<32; sig++)
  {
    if (mask & (1<<sig))
      signal (sig, bx_signal_handler);
  }
#endif
}

  void
bx_gui_c::set_text_charmap(Bit8u *fbuffer)
{
  memcpy(& BX_GUI_THIS vga_charmap, fbuffer, 0x2000);
  for (unsigned i=0; i<256; i++) BX_GUI_THIS char_changed[i] = 1;
  BX_GUI_THIS charmap_updated = 1;
}

  void
bx_gui_c::set_text_charbyte(Bit16u address, Bit8u data)
{
  BX_GUI_THIS vga_charmap[address] = data;
  BX_GUI_THIS char_changed[address >> 5] = 1;
  BX_GUI_THIS charmap_updated = 1;
}
