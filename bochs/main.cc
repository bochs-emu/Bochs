/////////////////////////////////////////////////////////////////////////
// $Id: main.cc,v 1.94 2002-03-26 14:46:01 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
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


#include "bochs.h"
#include <assert.h>
#include "state_file.h"

int enable_control_panel = 1;

extern "C" {
#include <signal.h>
}

#ifdef __MINGW32__
void alarm(int);
#endif


#if BX_PROVIDE_DEVICE_MODELS==1
// some prototypes from iodev/
// I want to stay away from including iodev/iodev.h here
Bit32u bx_unmapped_io_read_handler(Bit32u address, unsigned io_len);
void   bx_unmapped_io_write_handler(Bit32u address, Bit32u value,
                                    unsigned io_len);
void   bx_close_harddrive(void);
#endif



void bx_init_bx_dbg (void);
void bx_emulate_hga_dumps_timer(void);
static char *divider = "========================================================================";


/* typedefs */

#define LOG_THIS genlog->

#if ( BX_PROVIDE_DEVICE_MODELS==1 )
bx_pc_system_c bx_pc_system;
class state_file state_stuff("state_file.out", "options");
#endif

bx_debug_t bx_dbg;

bx_options_t bx_options = {
  { NULL, NULL, NULL },   // floppya
  { NULL, NULL, NULL },   // floppyb
  { 0, NULL, 0, 0, 0 },                   // diskc
  { 0, NULL, 0, 0, 0 },                   // diskd
  { 0, NULL},			// com1
  { 0, NULL},			// com2
  { 0, NULL},			// com3
  { 0, NULL},			// com4
  { 0, NULL, 0 },                         // cdromd
  { NULL, NULL },                          // rom
  { NULL },                             // vgarom
  { NULL },              // memory
  { 0, NULL },          // parallel port 1
  { 0, NULL },          // parallel port 2
  { 0, NULL, NULL, NULL, 0, 0, 0, 0 },  // SB16
  NULL,                                  // boot drive
  NULL,                               // vga update interval
  NULL,  // default keyboard serial path delay (usec)
  NULL,  // default keyboard paste delay (usec)
  NULL,  // default keyboard type
  NULL,  // default floppy command delay (usec)
  NULL,    // ips
  NULL,    // default mouse_enabled
  NULL,       // default private_colormap
#if BX_WITH_AMIGAOS
  NULL,       // full screen mode
  NULL,       // screen mode
#endif
  NULL,          // default i440FXSupport
  {NULL, NULL},  // cmos path, cmos image boolean
  { NULL, NULL, NULL, NULL, NULL, NULL }, // ne2k
  NULL,          // newHardDriveSupport
  { 0, NULL, NULL, NULL }, // load32bitOSImage hack stuff
  // log options: ignore debug, report info and error, crash on panic.
  { NULL, { ACT_IGNORE, ACT_REPORT, ACT_REPORT, ACT_ASK } },
  { NULL, NULL }, // KeyboardMapping
  };

static void parse_line_unformatted(char *context, char *line);
static void parse_line_formatted(char *context, int num_params, char *params[]);
static int parse_bochsrc(char *rcfile);

static Bit32s
bx_param_handler (bx_param_c *param, int set, Bit32s val)
{
  bx_id id = param->get_id ();
  switch (id) {
    case BXP_VGA_UPDATE_INTERVAL:
      // if after init, notify the vga device to change its timer.
      if (set && SIM->get_init_done ())
	bx_vga.set_update_interval (val);
      break;
    case BXP_MOUSE_ENABLED:
      // if after init, notify the GUI
      if (set && SIM->get_init_done ()) {
	bx_gui.mouse_enabled_changed (val!=0);
        bx_keyboard.mouse_enabled_changed (val!=0);
      }
      break;
    case BXP_PARPORT1_ENABLE:
      SIM->get_param (BXP_PARPORT1_OUTFILE)->set_enabled (val!=0);
      break;
    case BXP_PARPORT2_ENABLE:
      SIM->get_param (BXP_PARPORT2_OUTFILE)->set_enabled (val!=0);
      break;
    case BXP_COM1_PRESENT:
      SIM->get_param (BXP_COM1_PATH)->set_enabled (val!=0);
      break;
    case BXP_COM2_PRESENT:
      SIM->get_param (BXP_COM2_PATH)->set_enabled (val!=0);
      break;
    case BXP_COM3_PRESENT:
      SIM->get_param (BXP_COM3_PATH)->set_enabled (val!=0);
      break;
    case BXP_COM4_PRESENT:
      SIM->get_param (BXP_COM4_PATH)->set_enabled (val!=0);
      break;
    case BXP_SB16_PRESENT:
      if (set) {
	int enable = (val != 0);
	SIM->get_param (BXP_SB16_MIDIFILE)->set_enabled (enable);
	SIM->get_param (BXP_SB16_WAVEFILE)->set_enabled (enable);
	SIM->get_param (BXP_SB16_LOGFILE)->set_enabled (enable);
	SIM->get_param (BXP_SB16_MIDIMODE)->set_enabled (enable);
	SIM->get_param (BXP_SB16_WAVEMODE)->set_enabled (enable);
	SIM->get_param (BXP_SB16_LOGLEVEL)->set_enabled (enable);
	SIM->get_param (BXP_SB16_DMATIMER)->set_enabled (enable);
      }
      break;
    case BXP_NE2K_VALID:
      if (set) {
	int enable = (val != 0);
	SIM->get_param (BXP_NE2K_IOADDR)->set_enabled (enable);
	SIM->get_param (BXP_NE2K_IRQ)->set_enabled (enable);
	SIM->get_param (BXP_NE2K_MACADDR)->set_enabled (enable);
	SIM->get_param (BXP_NE2K_ETHMOD)->set_enabled (enable);
	SIM->get_param (BXP_NE2K_ETHDEV)->set_enabled (enable);
      }
      break;
    case BXP_LOAD32BITOS_WHICH:
      if (set) {
	int enable = (val != 0);
	SIM->get_param (BXP_LOAD32BITOS_PATH)->set_enabled (enable);
	SIM->get_param (BXP_LOAD32BITOS_IOLOG)->set_enabled (enable);
	SIM->get_param (BXP_LOAD32BITOS_INITRD)->set_enabled (enable);
      }
      break;
    case BXP_CMOS_IMAGE:
      SIM->get_param (BXP_CMOS_PATH)->set_enabled (val);
      break;
    case BXP_CDROM_INSERTED:
      if ((set) && (SIM->get_init_done ())) {
        bx_devices.hard_drive->set_cd_media_status(val == BX_INSERTED);
        bx_gui.update_drive_status_buttons ();
      }
      break;
    case BXP_FLOPPYA_STATUS:
      if ((set) && (SIM->get_init_done ())) {
        bx_devices.floppy->set_media_status(0, val == BX_INSERTED);
        bx_gui.update_drive_status_buttons ();
      }
      break;
    case BXP_FLOPPYB_STATUS:
      if ((set) && (SIM->get_init_done ())) {
        bx_devices.floppy->set_media_status(1, val == BX_INSERTED);
        bx_gui.update_drive_status_buttons ();
      }
      break;
    case BXP_KBD_PASTE_DELAY:
      if (set) bx_keyboard.paste_delay_changed ();
      break;
    default:
      BX_PANIC (("bx_param_handler called with unknown id %d", id));
      return -1;
  }
  return val;
}

char *bx_param_string_handler (bx_param_string_c *param, int set, char *val, int maxlen)
{
  int empty = 0;
  if ((strlen(val) < 1) || !strcmp ("none", val)) {
    empty = 1;
    val = "none";
  }
  switch (param->get_id ()) {
    case BXP_FLOPPYA_PATH:
      if (set==1) {
        if (SIM->get_init_done ()) {
          if (empty) {
            bx_devices.floppy->set_media_status(0, 0);
            bx_gui.update_drive_status_buttons ();
          } else {
            if (!SIM->get_param_num(BXP_FLOPPYA_TYPE)->get_enabled()) {
              BX_ERROR(("Cannot add a floppy drive at runtime"));
              bx_options.floppya.Opath->set ("none");
            }
          }
          if ((bx_devices.floppy) &&
              (SIM->get_param_num(BXP_FLOPPYA_STATUS)->get () == BX_INSERTED)) {
            // tell the device model that we removed, then inserted the disk
            bx_devices.floppy->set_media_status(0, 0);
            bx_devices.floppy->set_media_status(0, 1);
          }
        } else {
          SIM->get_param_num(BXP_FLOPPYA_TYPE)->set_enabled (!empty);
          SIM->get_param_num(BXP_FLOPPYA_STATUS)->set_enabled (!empty);
        }
      }
      break;
    case BXP_FLOPPYB_PATH:
      if (set==1) {
        if (SIM->get_init_done ()) {
          if (empty) {
            bx_devices.floppy->set_media_status(1, 0);
            bx_gui.update_drive_status_buttons ();
          } else {
            if (!SIM->get_param_num(BXP_FLOPPYB_TYPE)->get_enabled ()) {
              BX_ERROR(("Cannot add a floppy drive at runtime"));
              bx_options.floppyb.Opath->set ("none");
            }
          }
          if ((bx_devices.floppy) &&
              (SIM->get_param_num(BXP_FLOPPYB_STATUS)->get () == BX_INSERTED)) {
            // tell the device model that we removed, then inserted the disk
            bx_devices.floppy->set_media_status(1, 0);
            bx_devices.floppy->set_media_status(1, 1);
          }
        } else {
          SIM->get_param_num(BXP_FLOPPYB_TYPE)->set_enabled (!empty);
          SIM->get_param_num(BXP_FLOPPYB_STATUS)->set_enabled (!empty);
        }
      }
      break;
    case BXP_DISKC_PATH:
      if (set==1) {
	SIM->get_param_num(BXP_DISKC_PRESENT)->set (!empty);
	SIM->get_param_num(BXP_DISKC_CYLINDERS)->set_enabled (!empty);
	SIM->get_param_num(BXP_DISKC_HEADS)->set_enabled (!empty);
	SIM->get_param_num(BXP_DISKC_SPT)->set_enabled (!empty);
      }
      break;
    case BXP_DISKD_PATH:
      if (set==1) {
	SIM->get_param_num(BXP_DISKD_PRESENT)->set (!empty);
	SIM->get_param_num(BXP_DISKD_CYLINDERS)->set_enabled (!empty);
	SIM->get_param_num(BXP_DISKD_HEADS)->set_enabled (!empty);
	SIM->get_param_num(BXP_DISKD_SPT)->set_enabled (!empty);
      }
      break;
    case BXP_CDROM_PATH:
      if (set==1) {
        if (SIM->get_init_done ()) {
          if (empty) {
            bx_devices.hard_drive->set_cd_media_status(0);
            bx_gui.update_drive_status_buttons ();
          } else {
            if (!SIM->get_param_num(BXP_CDROM_PRESENT)->get ()) {
              BX_ERROR(("Cannot add a cdrom drive at runtime"));
              bx_options.cdromd.Opath->set ("none");
            }
          }
          if ((bx_devices.hard_drive) &&
              (SIM->get_param_num(BXP_CDROM_INSERTED)->get () == BX_INSERTED)) {
            // tell the device model that we removed, then inserted the cd
            bx_devices.hard_drive->set_cd_media_status(0);
            bx_devices.hard_drive->set_cd_media_status(1);
          }
        } else {
          SIM->get_param_num(BXP_CDROM_PRESENT)->set (!empty);
          SIM->get_param_num(BXP_CDROM_INSERTED)->set_enabled (!empty);
        }
      }
      break;
    case BXP_SCREENMODE:
      if (set==1) {
	BX_INFO (("Screen mode changed to %s", val));
      }
      break;
    default:
        BX_PANIC (("bx_string_handler called with unexpected parameter %d", param->get_id()));
  }
  return val;
}


void bx_init_options ()
{
  bx_list_c *menu;

  // floppya
  bx_options.floppya.Opath = new bx_param_string_c (BXP_FLOPPYA_PATH,
      "Floppy A image",
      "Pathname of first floppy image file or device.  If you're booting from floppy, this should be a bootable floppy.",
      "", BX_PATHNAME_LEN);
  bx_options.floppya.Opath->set_ask_format ("Enter new filename, or 'none' for no disk: [%s] ");
  bx_options.floppya.Otype = new bx_param_enum_c (BXP_FLOPPYA_TYPE,
      "floppya:type",
      "Type of floppy disk",
      floppy_type_names,
      BX_FLOPPY_NONE,
      BX_FLOPPY_NONE);
  bx_options.floppya.Otype->set_ask_format ("What type of floppy disk? [%s] ");
  bx_options.floppya.Oinitial_status = new bx_param_enum_c (BXP_FLOPPYA_STATUS,
      "Is floppya inserted",
      "Inserted or ejected",
      floppy_status_names,
      BX_INSERTED,
      BX_EJECTED);
  bx_options.floppya.Oinitial_status->set_ask_format ("Is the floppy inserted or ejected? [%s] ");
  bx_options.floppya.Opath->set_format ("%s");
  bx_options.floppya.Otype->set_format (", size=%s, ");
  bx_options.floppya.Oinitial_status->set_format ("%s");
  bx_param_c *floppya_init_list[] = {
    bx_options.floppya.Opath,
    bx_options.floppya.Otype,
    bx_options.floppya.Oinitial_status,
    NULL
  };
  menu = new bx_list_c (BXP_FLOPPYA, "Floppy Disk 0", "All options for first floppy disk", floppya_init_list);
  menu->get_options ()->set (menu->BX_SERIES_ASK);
  bx_options.floppya.Opath->set_handler (bx_param_string_handler);
  bx_options.floppya.Opath->set ("none");
  bx_options.floppya.Oinitial_status->set_handler (bx_param_handler);

  bx_options.floppyb.Opath = new bx_param_string_c (BXP_FLOPPYB_PATH,
      "floppyb:path",
      "Pathname of second floppy image file or device.",
      "", BX_PATHNAME_LEN);
  bx_options.floppyb.Opath->set_ask_format ("Enter new filename, or 'none' for no disk: [%s] ");
  bx_options.floppyb.Otype = new bx_param_enum_c (BXP_FLOPPYB_TYPE,
      "floppyb:type",
      "Type of floppy disk",
      floppy_type_names,
      BX_FLOPPY_NONE,
      BX_FLOPPY_NONE);
  bx_options.floppyb.Otype->set_ask_format ("What type of floppy disk? [%s] ");
  bx_options.floppyb.Oinitial_status = new bx_param_enum_c (BXP_FLOPPYB_STATUS,
      "Is floppyb inserted",
      "Inserted or ejected",
      floppy_status_names,
      BX_INSERTED,
      BX_EJECTED);
  bx_options.floppyb.Oinitial_status->set_ask_format ("Is the floppy inserted or ejected? [%s] ");
  bx_options.floppyb.Oinitial_status->set_format ("%s");
  bx_options.floppyb.Opath->set_format ("%s");
  bx_options.floppyb.Otype->set_format (", size=%s, ");
  bx_options.floppyb.Oinitial_status->set_format ("%s");
  bx_param_c *floppyb_init_list[] = {
    bx_options.floppyb.Opath,
    bx_options.floppyb.Otype,
    bx_options.floppyb.Oinitial_status,
    NULL
  };
  menu = new bx_list_c (BXP_FLOPPYB, "Floppy Disk 1", "All options for second floppy disk", floppyb_init_list);
  menu->get_options ()->set (menu->BX_SERIES_ASK);
  bx_options.floppyb.Opath->set_handler (bx_param_string_handler);
  bx_options.floppyb.Opath->set ("none");
  bx_options.floppyb.Oinitial_status->set_handler (bx_param_handler);

  // diskc options
  bx_options.diskc.Opresent = new bx_param_bool_c (BXP_DISKC_PRESENT,
      "diskc:present",
      "Controls whether diskc is installed or not",
      0);
  bx_options.diskc.Opath = new bx_param_string_c (BXP_DISKC_PATH,
      "",
      "Pathname of the hard drive image",
      "", BX_PATHNAME_LEN);
  bx_options.diskc.Ocylinders = new bx_param_num_c (BXP_DISKC_CYLINDERS,
      "diskc:cylinders",
      "Number of cylinders",
      0, 65535,
      0);
  bx_options.diskc.Oheads = new bx_param_num_c (BXP_DISKC_HEADS,
      "diskc:heads",
      "Number of heads",
      0, 65535,
      0);
  bx_options.diskc.Ospt = new bx_param_num_c (BXP_DISKC_SPT,
      "diskc:spt",
      "Number of sectors per track",
      0, 65535,
      0);
  bx_options.diskc.Opath->set_ask_format ("Enter new filename, or 'none' for no disk: [%s] ");
  bx_options.diskc.Ocylinders->set_ask_format ("Enter number of cylinders: [%d] ");
  bx_options.diskc.Oheads->set_ask_format ("Enter number of heads: [%d] ");
  bx_options.diskc.Ospt->set_ask_format ("Enter number of sectors per track: [%d] ");
  bx_options.diskc.Opath->set_format ("%s");
  bx_options.diskc.Ocylinders->set_format (", %d cylinders, ");
  bx_options.diskc.Oheads->set_format ("%d heads, ");
  bx_options.diskc.Ospt->set_format ("%d sectors/track");
  bx_param_c *diskc_init_list[] = {
    bx_options.diskc.Opath,
    bx_options.diskc.Ocylinders,
    bx_options.diskc.Oheads,
    bx_options.diskc.Ospt,
    NULL
  };
  menu = new bx_list_c (BXP_DISKC, "Hard disk 0", "All options for hard disk 0", diskc_init_list);
  menu->get_options ()->set (menu->BX_SERIES_ASK);
  // if path is the word "none", then do not ask the other options and
  // set present=0.
  bx_options.diskc.Opath->set_handler (bx_param_string_handler);
  bx_options.diskc.Opath->set ("none");

  // diskd options
  bx_options.diskd.Opresent = new bx_param_bool_c (BXP_DISKD_PRESENT,
      "diskd:present",
      "Controls whether diskd is installed or not",
      0);
  bx_options.diskd.Opath = new bx_param_string_c (BXP_DISKD_PATH,
      "diskd:path",
      "Pathname of the hard drive image",
      "", BX_PATHNAME_LEN);
  bx_options.diskd.Ocylinders = new bx_param_num_c (BXP_DISKD_CYLINDERS,
      "diskd:cylinders",
      "Number of cylinders",
      0, 65535,
      0);
  bx_options.diskd.Oheads = new bx_param_num_c (BXP_DISKD_HEADS,
      "diskd:heads",
      "Number of heads",
      0, 65535,
      0);
  bx_options.diskd.Ospt = new bx_param_num_c (BXP_DISKD_SPT,
      "diskd:spt",
      "Number of sectors per track",
      0, 65535,
      0);
  bx_options.diskd.Opath->set_ask_format ("Enter new filename, or none for no disk: [%s] ");
  bx_options.diskd.Ocylinders->set_ask_format ("Enter number of cylinders: [%d] ");
  bx_options.diskd.Oheads->set_ask_format ("Enter number of heads: [%d] ");
  bx_options.diskd.Ospt->set_ask_format ("Enter number of sectors per track: [%d] ");
  bx_options.diskd.Opath->set_format ("%s");
  bx_options.diskd.Ocylinders->set_format (", %d cylinders, ");
  bx_options.diskd.Oheads->set_format ("%d heads, ");
  bx_options.diskd.Ospt->set_format ("%d sectors/track");
  bx_param_c *diskd_init_list[] = {
    bx_options.diskd.Opath,
    bx_options.diskd.Ocylinders,
    bx_options.diskd.Oheads,
    bx_options.diskd.Ospt,
    NULL
  };
  menu = new bx_list_c (BXP_DISKD, "Hard disk 1", "All options for hard disk 1", diskd_init_list);
  menu->get_options ()->set (menu->BX_SERIES_ASK);
  bx_options.diskd.Opath->set_handler (bx_param_string_handler);
  bx_options.diskd.Opath->set ("none");

  // com1 options
  bx_options.com1.Opresent = new bx_param_bool_c (BXP_COM1_PRESENT,
						  "Enable serial port #1 (COM1)",
						  "",
						  0);

  bx_options.com1.Odev = new bx_param_string_c (BXP_COM1_PATH,
						 "Pathname of the serial device for COM1",
						 "",
						 "", BX_PATHNAME_LEN);
  bx_options.com1.Opresent->set_handler (bx_param_handler);
#if 0
  // com2 options
  bx_options.com2.Opresent = new bx_param_bool_c (BXP_COM2_PRESENT,
						  "Enable serial port #2 (COM1)",
						  "Controls whether com2 is installed or not",
						  0);

  bx_options.com2.Odev = new bx_param_string_c (BXP_COM2_PATH,
						 "",
						 "",
						 "", BX_PATHNAME_LEN);
  bx_options.com2.Opresent->set_handler (bx_param_handler);
  // com3 options
  bx_options.com3.Opresent = new bx_param_bool_c (BXP_COM3_PRESENT,
						  "Enable serial port #3 (COM1)",
						  "",
						  0);

  bx_options.com3.Odev = new bx_param_string_c (BXP_COM3_PATH,
						 "Pathname of the serial device for COM3",
						 "",
						 "", BX_PATHNAME_LEN);
  bx_options.com3.Opresent->set_handler (bx_param_handler);
  // com4 options
  bx_options.com4.Opresent = new bx_param_bool_c (BXP_COM4_PRESENT,
						  "Enable serial port #4 (COM1)",
						  "",
						  0);

  bx_options.com4.Odev = new bx_param_string_c (BXP_COM4_PATH,
						 "Pathname of the serial device for COM4",
						 "",
						 "", BX_PATHNAME_LEN);
  bx_options.com4.Opresent->set_handler (bx_param_handler);
#endif

  // cdrom options
  bx_options.cdromd.Opresent = new bx_param_bool_c (BXP_CDROM_PRESENT,
      "CDROM is present",
      "Controls whether cdromd is installed or not",
      0);
  bx_options.cdromd.Opath = new bx_param_string_c (BXP_CDROM_PATH,
      "CDROM image filename",
      "Pathname of the cdrom device or image",
      "", BX_PATHNAME_LEN);
  bx_options.cdromd.Opath->set_format ("%s");
  bx_options.cdromd.Opath->set_ask_format ("Enter new filename, or 'none' for no CDROM: [%s] ");
  bx_options.cdromd.Oinserted = new bx_param_enum_c (BXP_CDROM_INSERTED,
      "Is the CDROM inserted or ejected",
      "Inserted or ejected",
      floppy_status_names,
      BX_INSERTED,
      BX_EJECTED);
  bx_options.cdromd.Oinserted->set_format (", %s");
  bx_options.cdromd.Oinserted->set_ask_format ("Is the CDROM inserted or ejected? [%s] ");
  bx_param_c *cdromd_init_list[] = {
    bx_options.cdromd.Opath,
    bx_options.cdromd.Oinserted,
    NULL
  };
  menu = new bx_list_c (BXP_CDROMD, "CDROM", "Options for the CDROM", cdromd_init_list);
  menu->get_options ()->set (menu->BX_SERIES_ASK);
  bx_options.cdromd.Opath->set_handler (bx_param_string_handler);
  bx_options.cdromd.Opath->set ("none");
  bx_options.cdromd.Oinserted->set_handler (bx_param_handler);

  bx_options.OnewHardDriveSupport = new bx_param_bool_c (BXP_NEWHARDDRIVESUPPORT,
      "New Hard Drive Support",
      "Enables new features found on newer hard drives.",
      1);

  bx_options.Obootdrive = new bx_param_enum_c (BXP_BOOTDRIVE,
      "bootdrive",
      "Boot A, C or CD",
      floppy_bootdisk_names,
      BX_BOOT_FLOPPYA,
      BX_BOOT_FLOPPYA);
  bx_options.Obootdrive->set_format ("Boot from: %s drive");
  bx_options.Obootdrive->set_ask_format ("Boot from floppy drive, hard drive or cdrom ? [%s] ");
  // disk menu
  bx_param_c *disk_menu_init_list[] = {
    SIM->get_param (BXP_FLOPPYA),
    SIM->get_param (BXP_FLOPPYB),
    SIM->get_param (BXP_DISKC),
    SIM->get_param (BXP_DISKD),
    SIM->get_param (BXP_CDROMD),
    SIM->get_param (BXP_NEWHARDDRIVESUPPORT),
    SIM->get_param (BXP_BOOTDRIVE),
    NULL
  };
  menu = new bx_list_c (BXP_MENU_DISK, "Bochs Disk Options", "diskmenu", disk_menu_init_list);
  menu->get_options ()->set (menu->BX_SHOW_PARENT);

  // memory options menu
  bx_options.memory.Osize = new bx_param_num_c (BXP_MEM_SIZE,
      "megs",
      "Amount of RAM in megabytes",
      1, BX_MAX_INT,
      BX_DEFAULT_MEM_MEGS);
  bx_options.memory.Osize->set_format ("Memory size in megabytes: %d");
  bx_options.memory.Osize->set_ask_format ("Enter memory size (MB): [%d] ");

  bx_options.par1.Oenable = new bx_param_bool_c (BXP_PARPORT1_ENABLE,
      "Enable parallel port #1",
      "Enables the first parallel port",
      0);
  bx_options.par1.Oenable->set_handler (bx_param_handler);
  bx_options.par1.Ooutfile = new bx_param_string_c (BXP_PARPORT1_OUTFILE,
      "Parallel port #1 output file",
      "Data written to parport#1 by the guest OS is written to this file",
      "parport.out", BX_PATHNAME_LEN);
#if 0
  bx_options.par2.Oenable = new bx_param_bool_c (BXP_PARPORT2_ENABLE,
      "Enable parallel port #2",
      "Enables the second parallel port",
      0);
  bx_options.par2.Oenable->set_handler (bx_param_handler);
  bx_options.par2.Ooutfile = new bx_param_string_c (BXP_PARPORT2_OUTFILE,
      "Parallel port #2 output file",
      "Data written to parport#2 by the guest OS is written to this file",
      "parport2.out", BX_PATHNAME_LEN);
#endif
  bx_param_c *parport_init_list[] = {
    bx_options.par1.Oenable,
    bx_options.par1.Ooutfile,
    //bx_options.par2.Oenable,
    //bx_options.par2.Ooutfile,
    bx_options.com1.Opresent,
    bx_options.com1.Odev,
    //bx_options.com2.Opresent,
    //bx_options.com2.Odev,
    //bx_options.com3.Opresent,
    //bx_options.com3.Odev,
    //bx_options.com4.Opresent,
    //bx_options.com4.Odev,
    NULL
  };
  menu = new bx_list_c (BXP_MENU_SERIAL_PARALLEL, "Serial and Parallel Port Options", "serial_parallel_menu", parport_init_list);
  menu->get_options ()->set (menu->BX_SHOW_PARENT);

  bx_options.rom.Opath = new bx_param_string_c (BXP_ROM_PATH,
      "romimage",
      "Pathname of ROM image to load",
      "", BX_PATHNAME_LEN);
  bx_options.rom.Opath->set_format ("Name of ROM BIOS image: %s");
  bx_options.rom.Oaddress = new bx_param_num_c (BXP_ROM_ADDRESS,
      "romaddr",
      "The address at which the ROM image should be loaded",
      0, BX_MAX_INT, 
      0);
  bx_options.rom.Oaddress->set_format ("ROM BIOS address: 0x%05x");
  bx_options.rom.Oaddress->set_base (16);
  bx_options.vgarom.Opath = new bx_param_string_c (BXP_VGA_ROM_PATH,
      "vgaromimage",
      "Pathname of VGA ROM image to load",
      "", BX_PATHNAME_LEN);
  bx_options.vgarom.Opath->set_format ("Name of VGA BIOS image: %s");
  bx_param_c *memory_init_list[] = {
    bx_options.memory.Osize,
    bx_options.vgarom.Opath,
    bx_options.rom.Opath,
    bx_options.rom.Oaddress,
    NULL
  };
  menu = new bx_list_c (BXP_MENU_MEMORY, "Bochs Memory Options", "memmenu", memory_init_list);
  menu->get_options ()->set (menu->BX_SHOW_PARENT);

  // interface
  bx_options.Ovga_update_interval = new bx_param_num_c (BXP_VGA_UPDATE_INTERVAL,
      "VGA Update Interval",
      "Number of microseconds between VGA updates",
      1, BX_MAX_INT,
      30000);
  bx_options.Ovga_update_interval->set_handler (bx_param_handler);
  bx_options.Ovga_update_interval->set_ask_format ("Type a new value for VGA update interval: [%d] ");
  bx_options.Omouse_enabled = new bx_param_bool_c (BXP_MOUSE_ENABLED,
      "Enable the mouse",
      "Controls whether the mouse sends events to bochs",
      0);
  bx_options.Omouse_enabled->set_handler (bx_param_handler);
  bx_options.Oips = new bx_param_num_c (BXP_IPS, 
      "Emulated instructions per second (IPS)",
      "Emulated instructions per second, used to calibrate bochs emulated\ntime with wall clock time.",
      1, BX_MAX_INT,
      500000);
  bx_options.Oprivate_colormap = new bx_param_bool_c (BXP_PRIVATE_COLORMAP,
      "Use a private colormap",
      "Request that the GUI create and use it's own non-shared colormap.  This colormap will be used when in the bochs window.  If not enabled, a shared colormap scheme may be used.  Not implemented on all GUI's.",
      0);
#if BX_WITH_AMIGAOS
  bx_options.Ofullscreen = new bx_param_bool_c (BXP_FULLSCREEN,
      "Use full screen mode",
      "When enabled, bochs occupies the whole screen instead of just a window.",
      0);
  bx_options.Oscreenmode = new bx_param_string_c (BXP_SCREENMODE,
      "Screen mode name",
      "Screen mode name",
      "", BX_PATHNAME_LEN);
  bx_options.Oscreenmode->set_handler (bx_param_string_handler);
#endif
  bx_param_c *interface_init_list[] = {
    bx_options.Ovga_update_interval,
    bx_options.Omouse_enabled,
    bx_options.Oips,
    bx_options.Oprivate_colormap,
#if BX_WITH_AMIGAOS
    bx_options.Ofullscreen,
    bx_options.Oscreenmode,
#endif
    NULL
  };
  menu = new bx_list_c (BXP_MENU_INTERFACE, "Bochs Interface Menu", "intfmenu", interface_init_list);
  menu->get_options ()->set (menu->BX_SHOW_PARENT);

  // NE2K options
  bx_options.ne2k.Ovalid = new bx_param_bool_c (BXP_NE2K_VALID,
      "NE2K is present",
      "to be written",
      0);
  bx_options.ne2k.Oioaddr = new bx_param_num_c (BXP_NE2K_IOADDR,
      "NE2K I/O Address",
      "to be written",
      0, 0xffff,
      0);
  bx_options.ne2k.Oioaddr->set_base (16);
  bx_options.ne2k.Oirq = new bx_param_num_c (BXP_NE2K_IRQ,
      "NE2K Interrupt",
      "to be written",
      0, 15,
      0);
  bx_options.ne2k.Omacaddr = new bx_param_string_c (BXP_NE2K_MACADDR,
      "MAC Address",
      "to be written",
      "", 6);
  bx_options.ne2k.Omacaddr->get_options ()->set (bx_options.ne2k.Omacaddr->BX_RAW_BYTES);
  bx_options.ne2k.Omacaddr->set_separator (':');
  bx_options.ne2k.Oethmod = new bx_param_string_c (BXP_NE2K_ETHMOD,
      "Ethernet module",
      "to be written",
      "null", 16);
  bx_options.ne2k.Oethdev = new bx_param_string_c (BXP_NE2K_ETHDEV,
      "Ethernet device",
      "to be written",
      "xl0", BX_PATHNAME_LEN);
  bx_param_c *ne2k_init_list[] = {
    bx_options.ne2k.Ovalid,
    bx_options.ne2k.Oioaddr,
    bx_options.ne2k.Oirq,
    bx_options.ne2k.Omacaddr,
    bx_options.ne2k.Oethmod,
    bx_options.ne2k.Oethdev,
    NULL
  };
  menu = new bx_list_c (BXP_NE2K, "NE2K Configuration", "", ne2k_init_list);
  menu->get_options ()->set (menu->BX_SHOW_PARENT);
  bx_options.ne2k.Ovalid->set_handler (bx_param_handler);
  bx_options.ne2k.Ovalid->set (0);

  // SB16 options
  bx_options.sb16.Opresent = new bx_param_bool_c (BXP_SB16_PRESENT,
      "SB16 is present",
      "to be written",
      0);
  bx_options.sb16.Omidifile = new bx_param_string_c (BXP_SB16_MIDIFILE,
      "Midi file",
      "to be written",
      "", BX_PATHNAME_LEN);
  bx_options.sb16.Owavefile = new bx_param_string_c (BXP_SB16_WAVEFILE,
      "Wave file",
      "to be written",
      "", BX_PATHNAME_LEN);
  bx_options.sb16.Ologfile = new bx_param_string_c (BXP_SB16_LOGFILE,
      "Log file",
      "to be written",
      "", BX_PATHNAME_LEN);
  bx_options.sb16.Omidimode = new bx_param_num_c (BXP_SB16_MIDIMODE,
      "Midi mode",
      "to be written",
      0, BX_MAX_INT,
      0);
  bx_options.sb16.Owavemode = new bx_param_num_c (BXP_SB16_WAVEMODE,
      "Wave mode",
      "to be written",
      0, BX_MAX_INT,
      0);
  bx_options.sb16.Ologlevel = new bx_param_num_c (BXP_SB16_LOGLEVEL,
      "Log mode",
      "to be written",
      0, BX_MAX_INT,
      0);
  bx_options.sb16.Odmatimer = new bx_param_num_c (BXP_SB16_DMATIMER,
      "DMA timer",
      "to be written",
      0, BX_MAX_INT,
      0);
  bx_param_c *sb16_init_list[] = {
    bx_options.sb16.Opresent,
    bx_options.sb16.Omidifile,
    bx_options.sb16.Owavefile,
    bx_options.sb16.Ologfile,
    bx_options.sb16.Omidimode,
    bx_options.sb16.Owavemode,
    bx_options.sb16.Ologlevel,
    bx_options.sb16.Odmatimer,
    NULL
  };
  menu = new bx_list_c (BXP_SB16, "SB16 Configuration", "", sb16_init_list);
  menu->get_options ()->set (menu->BX_SHOW_PARENT);
  bx_options.sb16.Opresent->set_handler (bx_param_handler);
  bx_options.sb16.Opresent->set (0);

  bx_options.log.Ofilename = new bx_param_string_c (BXP_LOG_FILENAME,
      "log:filename",
      "Pathname of bochs log file",
      "-", BX_PATHNAME_LEN);
  bx_options.log.Ofilename->set_ask_format ("Enter log filename: [%s] ");

  // loader
  bx_options.load32bitOSImage.OwhichOS = new bx_param_enum_c (BXP_LOAD32BITOS_WHICH,
      "Which operating system?",
      "Which OS to boot",
      loader_os_names,
      Load32bitOSNone,
      Load32bitOSNone);
  bx_options.load32bitOSImage.Opath = new bx_param_string_c (BXP_LOAD32BITOS_PATH,
      "Pathname of OS to load",
      NULL,
      "", BX_PATHNAME_LEN);
  bx_options.load32bitOSImage.Oiolog = new bx_param_string_c (BXP_LOAD32BITOS_IOLOG,
      "Pathname of I/O log file",
      NULL,
      "", BX_PATHNAME_LEN);
  bx_options.load32bitOSImage.Oinitrd = new bx_param_string_c (BXP_LOAD32BITOS_INITRD,
      "Pathname of initrd",
      NULL,
      "", BX_PATHNAME_LEN);
  bx_param_c *loader_init_list[] = {
    bx_options.load32bitOSImage.OwhichOS,
    bx_options.load32bitOSImage.Opath,
    bx_options.load32bitOSImage.Oiolog,
    bx_options.load32bitOSImage.Oinitrd,
    NULL
  };
  bx_options.load32bitOSImage.OwhichOS->set_format ("os=%s");
  bx_options.load32bitOSImage.Opath->set_format (", path=%s");
  bx_options.load32bitOSImage.Oiolog->set_format (", iolog=%s");
  bx_options.load32bitOSImage.Oinitrd->set_format (", initrd=%s");
  bx_options.load32bitOSImage.OwhichOS->set_ask_format ("Enter OS to load: [%s] ");
  bx_options.load32bitOSImage.Opath->set_ask_format ("Enter pathname of OS: [%s]");
  bx_options.load32bitOSImage.Oiolog->set_ask_format ("Enter pathname of I/O log: [%s] ");
  bx_options.load32bitOSImage.Oinitrd->set_ask_format ("Enter pathname of initrd: [%s] ");
  menu = new bx_list_c (BXP_LOAD32BITOS, "32-bit OS Loader", "", loader_init_list);
  menu->get_options ()->set (menu->BX_SERIES_ASK);
  bx_options.load32bitOSImage.OwhichOS->set_handler (bx_param_handler);
  bx_options.load32bitOSImage.OwhichOS->set (Load32bitOSNone);

  // other
  bx_options.Okeyboard_serial_delay = new bx_param_num_c (BXP_KBD_SERIAL_DELAY,
      "keyboard_serial_delay",
      "Approximate time in microseconds that it takes one character to be transfered from the keyboard to controller over the serial path.",
      1, BX_MAX_INT,
      20000);
  bx_options.Okeyboard_paste_delay = new bx_param_num_c (BXP_KBD_PASTE_DELAY,
      "keyboard_paste_delay",
      "Approximate time in microseconds between attemps to paste characters to the keyboard controller.",
      1000, BX_MAX_INT,
      100000);
  bx_options.Okeyboard_paste_delay->set_handler (bx_param_handler);
  bx_options.Ofloppy_command_delay = new bx_param_num_c (BXP_FLOPPY_CMD_DELAY,
      "floppy_command_delay",
      "Time in microseconds to wait before completing some floppy commands such as read/write/seek/etc, which normally have a delay associated.  This used to be hardwired to 50,000 before.",
      1, BX_MAX_INT,
      50000);
  bx_options.Oi440FXSupport = new bx_param_bool_c (BXP_I440FX_SUPPORT,
      "i440FXSupport",
      "Controls whether to emulate PCI I440FX",
      0);
  bx_options.cmos.OcmosImage = new bx_param_bool_c (BXP_CMOS_IMAGE,
      "Use a CMOS image",
      NULL,
      0);
  bx_options.cmos.Opath = new bx_param_string_c (BXP_CMOS_PATH,
      "Pathname of CMOS image",
      NULL,
      "", BX_PATHNAME_LEN);
  bx_options.cmos.Otime0 = new bx_param_num_c (BXP_CMOS_TIME0,
      "Initial CMOS time for Bochs",
      "Start time for Bochs CMOS clock, used if you really want two runs to be identical (cosimulation)",
      0, BX_MAX_INT,
      0);

  // Keyboard mapping
  bx_options.keyboard.OuseMapping = new bx_param_bool_c(BXP_KEYBOARD_USEMAPPING,
      "Use keyboard mapping",
      NULL,
      0);
  bx_options.keyboard.Okeymap = new bx_param_string_c (BXP_KEYBOARD_MAP,
      "Keymap filename",
      NULL,
      "", BX_PATHNAME_LEN);

 // Keyboard type
  bx_options.Okeyboard_type = new bx_param_enum_c (BXP_KBD_TYPE,
      "Keyboard type",
      "Keyboard type",
      keyboard_type_names,
      BX_KBD_MF_TYPE,
      BX_KBD_XT_TYPE);
  bx_options.Okeyboard_type->set_format ("Keyboard type: %s");
  bx_options.Okeyboard_type->set_ask_format ("Enter keyboard type: [%s] ");

  bx_param_c *other_init_list[] = {
      bx_options.Okeyboard_serial_delay,
      bx_options.Okeyboard_paste_delay,
      bx_options.Ofloppy_command_delay,
      bx_options.Oi440FXSupport,
      bx_options.cmos.OcmosImage,
      bx_options.cmos.Opath,
      bx_options.cmos.Otime0,
      SIM->get_param (BXP_LOAD32BITOS),
      bx_options.keyboard.OuseMapping,
      bx_options.keyboard.Okeymap,
      bx_options.Okeyboard_type,
      NULL
  };
  menu = new bx_list_c (BXP_MENU_MISC, "Configure Everything Else", "", other_init_list);
  menu->get_options ()->set (menu->BX_SHOW_PARENT);
  bx_options.cmos.OcmosImage->set_handler (bx_param_handler);
  bx_options.cmos.OcmosImage->set (0);



}

void bx_print_header(void);

void bx_print_header ()
{
  fprintf (stderr, "%s\n", divider);
  char buffer[128];
  sprintf (buffer, "Bochs x86 Emulator %s\n", VER_STRING);
  bx_center_print (stderr, buffer, 72);
  if (REL_STRING[0]) {
    sprintf (buffer, "%s\n", REL_STRING);
    bx_center_print (stderr, buffer, 72);
  }
  fprintf (stderr, "%s\n", divider);
}

#if BX_WITH_CARBON
/* Original code by Darrell Walisser - dwaliss1@purdue.edu */

static void setupWorkingDirectory (char *path)
{
    char parentdir[MAXPATHLEN];
    char *c;
    
    strncpy ( parentdir, path, MAXPATHLEN );
    c = (char*) parentdir;
    
    while (*c != '\0')     /* go to end */
        c++;
    
    while (*c != '/')      /* back up to parent */
        c--;
    
    *c = '\0';             /* cut off last part (binary name) */
    
	/* chdir to the binary app's parent */
	int n;
        n = chdir (parentdir);
	if (n) BX_PANIC (("failed to change dir to parent"));
	/* chdir to the .app's parent */
	n = chdir ("../../../");
    if (n) BX_PANIC (("failed to change to ../../.."));
}
#endif

int
main(int argc, char *argv[])
{
  int arg = 1;
  // To deal with initialization order problems inherent in C++, use the macros
  // SAFE_GET_IOFUNC and SAFE_GET_GENLOG to retrieve "io" and "genlog" in all
  // constructors or functions called by constructors.  The macros test for
  // NULL and create the object if necessary, then return it.  Ensure that io
  // and genlog get created, by making one reference to each macro right here.
  // All other code can reference io and genlog directly.
  SAFE_GET_IOFUNC();
  SAFE_GET_GENLOG();

  bx_print_header ();
  bx_init_bx_dbg ();

#if BX_WITH_CARBON
    /* "-psn" is passed if we are launched by double-clicking */
   if ( argc >= 2 && strncmp (argv[1], "-psn", 4) == 0 ) {
     // ugly hack.  I don't know how to open a window to print messages in,
     // so put them in /tmp/early-bochs-out.txt.  Sorry. -bbd
     io->init_log("/tmp/early-bochs-out.txt");
     BX_INFO (("I was launched by double clicking.  Fixing home directory."));
     argc = 1; // ignore all other args.
     setupWorkingDirectory (argv[0]);
     // there is no stdin/stdout so disable the text-based config interface.
     enable_control_panel = 0;
   }
   // if it was started from command line, there could be some args still.
   for (int a=0; a<argc; a++) {
     BX_INFO (("argument %d is %s", a, argv[a]));
   }
        
  char cwd[MAXPATHLEN];
  getwd (cwd);
  BX_INFO (("Now my working directory is %s", cwd));
#endif

  init_siminterface ();
  bx_control_panel (BX_CPANEL_INIT);
  bx_init_options ();

  // detect -nocontrolpanel or -nocp argument before anything else
  if (argc > 1 && 
       ((!strcmp ("-nocontrolpanel", argv[1]))
        || (!strcmp ("-nocp", argv[1])))) {
    // skip the control panel
    arg++;
    SIM->set_enabled (0);
    enable_control_panel = 0;
  }
#if !BX_USE_CONTROL_PANEL
  enable_control_panel = 0;
#endif

  if (!enable_control_panel) {
    // if we don't intend to run the control panel, parse the .bochsrc now.
    char *bochsrc = bx_find_bochsrc ();
    if (bochsrc)
      bx_read_configuration (bochsrc);

    if (bochsrc == NULL && arg>=argc) {
      // no bochsrc used.  This is legal since they may have everything on the
      // command line.  However if they have no arguments then give them some
      // friendly advice.
      fprintf (stderr, "%s\n", divider);
      fprintf (stderr, "Before running Bochs, you should cd to a directory which contains\n");
      fprintf (stderr, "a .bochsrc file and a disk image.  If you downloaded a binary package,\n");
      fprintf (stderr, "all the necessary files are already on your disk.\n");
#if defined(WIN32)
      fprintf (stderr, "\nFor Windows installations, go to the dlxlinux direectory and\n");
      fprintf (stderr, "double-click on the start.bat script.\n");
#elif !defined(macintosh)
      fprintf (stderr, "\nFor UNIX installations, try running \"bochs-dlx\" for a demo.  This script\n");
      fprintf (stderr, "is basically equivalent to typing:\n");
      fprintf (stderr, "   cd /usr/local/bochs/dlxlinux\n");
      fprintf (stderr, "   bochs\n");
#endif
      exit(1);
    }
  }

  // parse the rest of the command line.
  if (bx_parse_cmdline (arg, argc, argv)) {
    fprintf (stderr, "There were errors while parsing the command line.\n");
    fprintf (stderr, "Bochs is exiting.\n");
    exit (1);
  }

  if (enable_control_panel) {
    // update log actions before starting control panel
    for (int level=0; level<N_LOGLEV; level++) {
      int action = bx_options.log.actions[level];
      io->set_log_action (level, action);
    }
    // Display the pre-simulation control panel.
    SIM->set_enabled (1);
    bx_control_panel (BX_CPANEL_START_MENU);
  }

#if BX_DEBUGGER
  // If using the debugger, it will take control and call
  // bx_init_hardware() and cpu_loop()
  bx_dbg_main(argc, argv);
#else

  bx_init_hardware();

  if (bx_options.load32bitOSImage.OwhichOS->get ()) {
    void bx_load32bitOSimagehack(void);
    bx_load32bitOSimagehack();
    }

  SIM->set_init_done (1);

  // The set handler for mouse_enabled does not actually update the gui
  // until init_done is set.  This forces the set handler to be called,
  // which sets up the mouse enabled GUI-specific stuff correctly.
  // Not a great solution but it works. BBD
  bx_options.Omouse_enabled->set (bx_options.Omouse_enabled->get ());

  if (BX_SMP_PROCESSORS == 1) {
    // only one processor, run as fast as possible by not messing with
    // quantums and loops.
    BX_CPU(0)->cpu_loop(1);
    BX_PANIC (("cpu_loop should never return in a single-processor simulation"));
  } else {
    // SMP simulation: do a few instructions on each processor, then switch
    // to another.  Increasing quantum speeds up overall performance, but
    // reduces granularity of synchronization between processors.
    int processor = 0;
    int quantum = 5;
    while (1) {
      // do some instructions in each processor
      BX_CPU(processor)->cpu_loop(quantum);
      processor = (processor+1) % BX_SMP_PROCESSORS;
      if (processor == 0) 
	BX_TICKN(quantum);
    }
  }
#endif

  return(0);
}

int
bx_read_configuration (char *rcfile)
{
  // parse rcfile first, then parse arguments in order.
  BX_INFO (("reading configuration from %s", rcfile));
  if (parse_bochsrc(rcfile) < 0) {
    BX_ERROR (("reading from %s failed", rcfile));
    return -1;
  }
  // update log actions if control panel is enabled
  if (SIM->get_enabled ()) {
    for (int level=0; level<N_LOGLEV; level++) {
      int action = bx_options.log.actions[level];
      io->set_log_action (level, action);
    }
  }
  return 0;
}

int bx_parse_cmdline (int arg, int argc, char *argv[])
{
  //if (arg < argc) BX_INFO (("parsing command line arguments"));

  while (arg < argc) {
    BX_INFO (("parsing arg %d, %s", arg, argv[arg]));
    parse_line_unformatted("cmdline args", argv[arg]);
    arg++;
  }
  return 0;
}

  int
bx_init_hardware()
{
  // all configuration has been read, now initialize everything.

  if (!SIM->get_enabled ()) {
    for (int level=0; level<N_LOGLEV; level++) {
      int action = bx_options.log.actions[level];
      if (action == ACT_ASK) action = ACT_FATAL;
      io->set_log_action (level, action);
    }
  }

  bx_pc_system.init_ips(bx_options.Oips->get ());

  if(bx_options.log.Ofilename->getptr()[0]!='-') {
    BX_INFO (("using log file %s", bx_options.log.Ofilename->getptr ()));
    io->init_log(bx_options.log.Ofilename->getptr ());
  }

  // set up memory and CPU objects
#if BX_SUPPORT_APIC
  memset(apic_index, 0, sizeof(apic_index[0]) * APIC_MAX_ID);
#endif

#if BX_SMP_PROCESSORS==1
  BX_MEM(0)->init_memory(bx_options.memory.Osize->get () * 1024*1024);
  BX_MEM(0)->load_ROM(bx_options.rom.Opath->getptr (), bx_options.rom.Oaddress->get ());
  BX_MEM(0)->load_ROM(bx_options.vgarom.Opath->getptr (), 0xc0000);
  BX_CPU(0)->init (BX_MEM(0));
  BX_CPU(0)->reset(BX_RESET_HARDWARE);
#else
  // SMP initialization
  bx_mem_array[0] = new BX_MEM_C ();
  bx_mem_array[0]->init_memory(bx_options.memory.Osize->get () * 1024*1024);
  bx_mem_array[0]->load_ROM(bx_options.rom.Opath->getptr (), bx_options.rom.Oaddress->get ());
  bx_mem_array[0]->load_ROM(bx_options.vgarom.Opath->getptr (), 0xc0000);
  for (int i=0; i<BX_SMP_PROCESSORS; i++) {
    BX_CPU(i) = new BX_CPU_C ();
    BX_CPU(i)->init (bx_mem_array[0]);
    // assign apic ID from the index of this loop
    // if !BX_SUPPORT_APIC, this will not compile.
    BX_CPU(i)->local_apic.set_id (i);
    BX_CPU(i)->reset(BX_RESET_HARDWARE);
  }
#endif

#if BX_DEBUGGER == 0
  bx_devices.init(BX_MEM(0));
  bx_gui.init_signal_handlers ();
  bx_pc_system.start_timers();
#endif
  BX_DEBUG(("bx_init_hardware is setting signal handlers"));
// if not using debugger, then we can take control of SIGINT.
// If using debugger, it needs control of this.
#if BX_DEBUGGER==0
  signal(SIGINT, bx_signal_handler);
#endif

#if BX_SHOW_IPS
#ifndef __MINGW32__
  signal(SIGALRM, bx_signal_handler);
#endif
  alarm( 1 );
#endif

  return(0);
}



  void
bx_init_bx_dbg (void)
{
  bx_dbg.floppy = 0;
  bx_dbg.keyboard = 0;
  bx_dbg.video = 0;
  bx_dbg.disk = 0;
  bx_dbg.pit = 0;
  bx_dbg.pic = 0;
  bx_dbg.bios = 0;
  bx_dbg.cmos = 0;
  bx_dbg.a20 = 0;
  bx_dbg.interrupts = 0;
  bx_dbg.exceptions = 0;
  bx_dbg.unsupported = 0;
  bx_dbg.temp = 0;
  bx_dbg.reset = 0;
  bx_dbg.mouse = 0;
  bx_dbg.io = 0;
  bx_dbg.debugger = 0;
  bx_dbg.xms = 0;
  bx_dbg.v8086 = 0;
  bx_dbg.paging = 0;
  bx_dbg.creg = 0;
  bx_dbg.dreg = 0;
  bx_dbg.dma = 0;
  bx_dbg.unsupported_io = 0;
  bx_dbg.record_io = 0;
  bx_dbg.serial = 0;
  bx_dbg.cdrom = 0;
#ifdef MAGIC_BREAKPOINT
  bx_dbg.magic_break_enabled = 0;
#endif

}


  void
bx_atexit(void)
{
  static Boolean been_here = 0;


#if BX_PROVIDE_DEVICE_MODELS==1
  if (been_here == 0) {
    bx_pc_system.exit();
    }
#endif

#if BX_DEBUGGER == 0
  for (int cpu=0; cpu<BX_SMP_PROCESSORS; cpu++)
    if (BX_CPU(cpu)) BX_CPU(cpu)->atexit();
#endif

#if BX_PCI_SUPPORT
    if (bx_options.Oi440FXSupport->get ()) {
      bx_devices.pci->print_i440fx_state();
      }
#endif
}

#if (BX_PROVIDE_CPU_MEMORY==1) && (BX_EMULATE_HGA_DUMPS>0)
  void
bx_emulate_hga_dumps_timer(void)
{
  void bx_hga_set_video_memory(Bit8u *ptr);

  bx_hga_set_video_memory(&bx_phy_memory[0xb0000]);
}
#endif

#if BX_PROVIDE_MAIN

char *
bx_find_bochsrc ()
{
  FILE *fd = NULL;
  char rcfile[512];
  Bit32u retry = 0, found = 0;
  // try several possibilities for the bochsrc before giving up
  while (!found) {
    rcfile[0] = 0;
    switch (retry++) {
    case 0: strcpy (rcfile, ".bochsrc"); break;
    case 1: strcpy (rcfile, "bochsrc"); break;
    case 2: strcpy (rcfile, "bochsrc.txt"); break;
    case 3:
#if (!defined(WIN32)) && !BX_WITH_MACOS
      // only try this on unix
      {
      char *ptr = getenv("HOME");
      if (ptr) sprintf (rcfile, "%s/.bochsrc", ptr);
      }
#endif
      break;
    default:
      return NULL;
    }
    if (rcfile[0]) {
      BX_DEBUG (("looking for configuration in %s", rcfile));
      fd = fopen(rcfile, "r");
      if (fd) found = 1;
    }
  }
  assert (fd != NULL && rcfile[0] != 0);
  fclose (fd);
  return strdup (rcfile);
}

  static int
parse_bochsrc(char *rcfile)
{
  FILE *fd = NULL;
  char *ret;
  char line[512];

  // try several possibilities for the bochsrc before giving up

  fd = fopen (rcfile, "r");
  if (!fd) return -1;

  do {
    ret = fgets(line, sizeof(line)-1, fd);
    line[sizeof(line) - 1] = '\0';
    int len = strlen(line);
    if (len>0)
      line[len-1] = '\0';
    if ((ret != NULL) && strlen(line)) {
      parse_line_unformatted(rcfile, line);
      }
    } while (!feof(fd));
  return 0;
}

  static void
parse_line_unformatted(char *context, char *line)
{
#define MAX_PARAMS_LEN 40
  char *ptr;
  unsigned i, string_i;
  char string[512];
  char *params[MAX_PARAMS_LEN];
  int num_params;
  Boolean inquotes = 0;

  memset(params, 0, sizeof(params));
  if (line == NULL) return;

  // if passed nothing but whitespace, just return
  for (i=0; i<strlen(line); i++) {
    if (!isspace(line[i])) break;
    }
  if (i>=strlen(line))
    return;

  num_params = 0;

  ptr = strtok(line, ":");
  while (ptr) {
    string_i = 0;
    for (i=0; i<strlen(ptr); i++) {
      if (ptr[i] == '"')
        inquotes = !inquotes;
      else {
#if BX_HAVE_GETENV
	// substitute environment variables.
	if (ptr[i] == '$') {
	  char varname[512];
	  char *pv = varname;
	  char *value;
	  *pv = 0;
	  i++;
	  while (isalpha(ptr[i]) || ptr[i]=='_') {
	    *pv = ptr[i]; pv++; i++;
	  }
	  *pv = 0;
	  if (strlen(varname)<1 || !(value = getenv(varname))) {
	    BX_PANIC (("could not look up environment variable '%s'\n", varname));
	  } else {
	    // append value to the string
	    for (pv=value; *pv; pv++)
		string[string_i++] = *pv;
	  }
	}
#endif
        if (!isspace(ptr[i]) || inquotes) {
	  string[string_i++] = ptr[i];
	}
      }
    }
    string[string_i] = '\0';
    if ( params[num_params] != NULL )
    {
        free(params[num_params]);
        params[num_params] = NULL;
    }
    if ( num_params < MAX_PARAMS_LEN )
    {
    params[num_params++] = strdup (string);
    ptr = strtok(NULL, ",");
  }
    else
    {
        BX_PANIC (("too many parameters, max is %d\n", MAX_PARAMS_LEN));
    }
  }
  parse_line_formatted(context, num_params, &params[0]);
  for (i=0; i < MAX_PARAMS_LEN; i++)
  {
    if ( params[i] != NULL )
    {
        free(params[i]);
        params[i] = NULL;
    }
  }
}

  static void
parse_line_formatted(char *context, int num_params, char *params[])
{
  int i;

  if (num_params < 1) return;

  if (params[0][0] == '#') return; /* comment */
  else if (!strcmp(params[0], "floppya")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "2_88=", 5)) {
	bx_options.floppya.Opath->set (&params[i][5]);
        bx_options.floppya.Otype->set (BX_FLOPPY_2_88);
        }
      else if (!strncmp(params[i], "1_44=", 5)) {
	bx_options.floppya.Opath->set (&params[i][5]);
        bx_options.floppya.Otype->set (BX_FLOPPY_1_44);
        }
      else if (!strncmp(params[i], "1_2=", 4)) {
	bx_options.floppya.Opath->set (&params[i][4]);
        bx_options.floppya.Otype->set (BX_FLOPPY_1_2);
        }
      else if (!strncmp(params[i], "720k=", 5)) {
	bx_options.floppya.Opath->set (&params[i][5]);
        bx_options.floppya.Otype->set (BX_FLOPPY_720K);
        }
      else if (!strncmp(params[i], "status=ejected", 14)) {
        bx_options.floppya.Oinitial_status->set (BX_EJECTED);
        }
      else if (!strncmp(params[i], "status=inserted", 15)) {
        bx_options.floppya.Oinitial_status->set (BX_INSERTED);
        }
      else {
        BX_PANIC(("%s: floppya attribute '%s' not understood.", context,
          params[i]));
        }
      }
    }

  else if (!strcmp(params[0], "floppyb")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "2_88=", 5)) {
	bx_options.floppyb.Opath->set (&params[i][5]);
        bx_options.floppyb.Otype->set (BX_FLOPPY_2_88);
        }
      else if (!strncmp(params[i], "1_44=", 5)) {
	bx_options.floppyb.Opath->set (&params[i][5]);
        bx_options.floppyb.Otype->set (BX_FLOPPY_1_44);
        }
      else if (!strncmp(params[i], "1_2=", 4)) {
	bx_options.floppyb.Opath->set (&params[i][4]);
        bx_options.floppyb.Otype->set (BX_FLOPPY_1_2);
        }
      else if (!strncmp(params[i], "720k=", 5)) {
	bx_options.floppyb.Opath->set (&params[i][5]);
        bx_options.floppyb.Otype->set (BX_FLOPPY_720K);
        }
      else if (!strncmp(params[i], "status=ejected", 14)) {
        bx_options.floppyb.Oinitial_status->set (BX_EJECTED);
        }
      else if (!strncmp(params[i], "status=inserted", 15)) {
        bx_options.floppyb.Oinitial_status->set (BX_INSERTED);
        }
      else {
        BX_PANIC(("%s: floppyb attribute '%s' not understood.", context,
          params[i]));
        }
      }
    }

  else if (!strcmp(params[0], "diskc")) {
    if (num_params != 5) {
      BX_PANIC(("%s: diskc directive malformed.", context));
      }
    if (strncmp(params[1], "file=", 5) ||
        strncmp(params[2], "cyl=", 4) ||
        strncmp(params[3], "heads=", 6) ||
        strncmp(params[4], "spt=", 4)) {
      BX_PANIC(("%s: diskc directive malformed.", context));
      }
    bx_options.diskc.Opath->set (&params[1][5]);
    bx_options.diskc.Ocylinders->set (atol(&params[2][4]));
    bx_options.diskc.Oheads->set     (atol(&params[3][6]));
    bx_options.diskc.Ospt->set       (atol(&params[4][4]));
    bx_options.diskc.Opresent->set (1);
    }
  else if (!strcmp(params[0], "diskd")) {
    if (num_params != 5) {
      BX_PANIC(("%s: diskd directive malformed.", context));
      }
    if (strncmp(params[1], "file=", 5) ||
        strncmp(params[2], "cyl=", 4) ||
        strncmp(params[3], "heads=", 6) ||
        strncmp(params[4], "spt=", 4)) {
      BX_PANIC(("%s: diskd directive malformed.", context));
      }
    bx_options.diskd.Opath->set (&params[1][5]);
    bx_options.diskd.Ocylinders->set (atol( &params[2][4]));
    bx_options.diskd.Oheads->set     (atol( &params[3][6] ));
    bx_options.diskd.Ospt->set       (atol( &params[4][4] ));
    bx_options.diskd.Opresent->set (1);
    }

  else if (!strcmp(params[0], "com1")) {
    if (num_params != 2) {
      BX_PANIC(("%s: com1 directive malformed.", context));
      }
    if (strncmp(params[1], "dev=", 4)) {
      BX_PANIC(("%s: com1 directive malformed.", context));
      }
    bx_options.com1.Odev->set (&params[1][4]);
    bx_options.com1.Opresent->set (1);
    }
  else if (!strcmp(params[0], "com2")) {
    if (num_params != 2) {
      BX_PANIC(("%s: com2 directive malformed.", context));
      }
    if (strncmp(params[1], "dev=", 4)) {
      BX_PANIC(("%s: com2 directive malformed.", context));
      }
    bx_options.com2.Odev->set (&params[1][4]);
    bx_options.com2.Opresent->set (1);
    }
  else if (!strcmp(params[0], "com3")) {
    if (num_params != 2) {
      BX_PANIC(("%s: com3 directive malformed.", context));
      }
    if (strncmp(params[1], "dev=", 4)) {
      BX_PANIC(("%s: com3 directive malformed.", context));
      }
    bx_options.com3.Odev->set (&params[1][4]);
    bx_options.com3.Opresent->set (1);
    }
  else if (!strcmp(params[0], "com4")) {
    if (num_params != 2) {
      BX_PANIC(("%s: com4 directive malformed.", context));
      }
    if (strncmp(params[1], "dev=", 4)) {
      BX_PANIC(("%s: com4 directive malformed.", context));
      }
    bx_options.com4.Odev->set (&params[1][4]);
    bx_options.com4.Opresent->set (1);
    }

  else if (!strcmp(params[0], "cdromd")) {
    if (num_params != 3) {
      BX_PANIC(("%s: cdromd directive malformed.", context));
      }
    if (strncmp(params[1], "dev=", 4) || strncmp(params[2], "status=", 7)) {
      BX_PANIC(("%s: cdromd directive malformed.", context));
      }
    bx_options.cdromd.Opath->set (&params[1][4]);
    if (!strcmp(params[2], "status=inserted"))
      bx_options.cdromd.Oinserted->set (BX_INSERTED);
    else if (!strcmp(params[2], "status=ejected"))
      bx_options.cdromd.Oinserted->set (BX_EJECTED);
    else {
      BX_PANIC(("%s: cdromd directive malformed.", context));
      }
    bx_options.cdromd.Opresent->set (1);
    }

  else if (!strcmp(params[0], "boot")) {
    if (!strcmp(params[1], "a")) {
      bx_options.Obootdrive->set (BX_BOOT_FLOPPYA);
    } else if (!strcmp(params[1], "c")) {
      bx_options.Obootdrive->set (BX_BOOT_DISKC);
    } else if (!strcmp(params[1], "cdrom")) {
      bx_options.Obootdrive->set (BX_BOOT_CDROM);
    } else {
      BX_PANIC(("%s: boot directive with unknown boot device '%s'.  use 'a', 'c' or 'cdrom'.", context, params[1]));
      }
    }
  else if (!strcmp(params[0], "log")) {
    if (num_params != 2) {
      BX_PANIC(("%s: log directive has wrong # args.", context));
      }
    bx_options.log.Ofilename->set (params[1]);
    }
  else if (!strcmp(params[0], "panic")) {
    if (num_params != 2) {
      BX_PANIC(("%s: panic directive malformed.", context));
      }
    if (strncmp(params[1], "action=", 7)) {
      BX_PANIC(("%s: panic directive malformed.", context));
      }
    char *action = 7 + params[1];
    if (!strcmp(action, "fatal"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_FATAL;
    else if (!strcmp (action, "report"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_REPORT;
    else if (!strcmp (action, "ignore"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_IGNORE;
    else if (!strcmp (action, "ask"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_ASK;
    else {
      BX_PANIC(("%s: panic directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "error")) {
    if (num_params != 2) {
      BX_PANIC(("%s: error directive malformed.", context));
      }
    if (strncmp(params[1], "action=", 7)) {
      BX_PANIC(("%s: error directive malformed.", context));
      }
    char *action = 7 + params[1];
    if (!strcmp(action, "fatal"))
      bx_options.log.actions[LOGLEV_ERROR] = ACT_FATAL;
    else if (!strcmp (action, "report"))
      bx_options.log.actions[LOGLEV_ERROR] = ACT_REPORT;
    else if (!strcmp (action, "ignore"))
      bx_options.log.actions[LOGLEV_ERROR] = ACT_IGNORE;
    else if (!strcmp (action, "ask"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_ASK;
    else {
      BX_PANIC(("%s: error directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "info")) {
    if (num_params != 2) {
      BX_PANIC(("%s: info directive malformed.", context));
      }
    if (strncmp(params[1], "action=", 7)) {
      BX_PANIC(("%s: info directive malformed.", context));
      }
    char *action = 7 + params[1];
    if (!strcmp(action, "fatal"))
      bx_options.log.actions[LOGLEV_INFO] = ACT_FATAL;
    else if (!strcmp (action, "report"))
      bx_options.log.actions[LOGLEV_INFO] = ACT_REPORT;
    else if (!strcmp (action, "ignore"))
      bx_options.log.actions[LOGLEV_INFO] = ACT_IGNORE;
    else if (!strcmp (action, "ask"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_ASK;
    else {
      BX_PANIC(("%s: info directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "debug")) {
    if (num_params != 2) {
      BX_PANIC(("%s: debug directive malformed.", context));
      }
    if (strncmp(params[1], "action=", 7)) {
      BX_PANIC(("%s: debug directive malformed.", context));
      }
    char *action = 7 + params[1];
    if (!strcmp(action, "fatal"))
      bx_options.log.actions[LOGLEV_DEBUG] = ACT_FATAL;
    else if (!strcmp (action, "report"))
      bx_options.log.actions[LOGLEV_DEBUG] = ACT_REPORT;
    else if (!strcmp (action, "ignore"))
      bx_options.log.actions[LOGLEV_DEBUG] = ACT_IGNORE;
    else if (!strcmp (action, "ask"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_ASK;
    else {
      BX_PANIC(("%s: debug directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "romimage")) {
    if (num_params != 3) {
      BX_PANIC(("%s: romimage directive: wrong # args.", context));
      }
    if (strncmp(params[1], "file=", 5)) {
      BX_PANIC(("%s: romimage directive malformed.", context));
      }
    if (strncmp(params[2], "address=", 8)) {
      BX_PANIC(("%s: romimage directive malformed.", context));
      }
    bx_options.rom.Opath->set (&params[1][5]);
    if ( (params[2][8] == '0') && (params[2][9] == 'x') )
      bx_options.rom.Oaddress->set (strtoul (&params[2][8], NULL, 16));
    else
      bx_options.rom.Oaddress->set (strtoul (&params[2][8], NULL, 10));
    }
  else if (!strcmp(params[0], "vgaromimage")) {
    if (num_params != 2) {
      BX_PANIC(("%s: vgaromimage directive: wrong # args.", context));
      }
    bx_options.vgarom.Opath->set (params[1]);
    }
  else if (!strcmp(params[0], "vga_update_interval")) {
    if (num_params != 2) {
      BX_PANIC(("%s: vga_update_interval directive: wrong # args.", context));
      }
    bx_options.Ovga_update_interval->set (atol(params[1]));
    if (bx_options.Ovga_update_interval->get () < 50000) {
      BX_INFO(("%s: vga_update_interval seems awfully small!", context));
      }
    }
  else if (!strcmp(params[0], "keyboard_serial_delay")) {
    if (num_params != 2) {
      BX_PANIC(("%s: keyboard_serial_delay directive: wrong # args.", context));
      }
    bx_options.Okeyboard_serial_delay->set (atol(params[1]));
    if (bx_options.Okeyboard_serial_delay->get () < 5) {
      BX_ERROR (("%s: keyboard_serial_delay not big enough!", context));
      }
    }
  else if (!strcmp(params[0], "keyboard_paste_delay")) {
    if (num_params != 2) {
      BX_PANIC(("%s: keyboard_paste_delay directive: wrong # args.", context));
      }
    bx_options.Okeyboard_paste_delay->set (atol(params[1]));
    if (bx_options.Okeyboard_paste_delay->get () < 1000) {
      BX_ERROR (("%s: keyboard_paste_delay not big enough!", context));
      }
    }
  else if (!strcmp(params[0], "megs")) {
    if (num_params != 2) {
      BX_PANIC(("%s: megs directive: wrong # args.", context));
      }
    bx_options.memory.Osize->set (atol(params[1]));
    }
  else if (!strcmp(params[0], "floppy_command_delay")) {
    if (num_params != 2) {
      BX_PANIC(("%s: floppy_command_delay directive: wrong # args.", context));
      }
    bx_options.Ofloppy_command_delay->set (atol(params[1]));
    if (bx_options.Ofloppy_command_delay->get () < 100) {
      BX_PANIC(("%s: floppy_command_delay not big enough!", context));
      }
    }
  else if (!strcmp(params[0], "ips")) {
    if (num_params != 2) {
      BX_PANIC(("%s: ips directive: wrong # args.", context));
      }
    bx_options.Oips->set (atol(params[1]));
    if (bx_options.Oips->get () < 200000) {
      BX_ERROR(("%s: WARNING: ips is AWFULLY low!", context));
      }
    }
  else if (!strcmp(params[0], "max_ips")) {
    if (num_params != 2) {
      BX_PANIC(("%s: max_ips directive: wrong # args.", context));
      }
    BX_INFO(("WARNING: max_ips not implemented"));
    }
  else if (!strcmp(params[0], "system_clock_sync")) {
    if (num_params != 2) {
      BX_PANIC(("%s: system_clock_sync directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      BX_PANIC(("%s: system_clock_sync directive malformed.", context));
      }
    if (params[1][8] == '0' || params[1][8] == '1')
      BX_INFO (("WARNING: system_clock_sync not implemented"));
    else
      BX_PANIC(("%s: system_clock_sync directive malformed.", context));
    }
  else if (!strcmp(params[0], "mouse")) {
    if (num_params != 2) {
      BX_PANIC(("%s: mouse directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      BX_PANIC(("%s: mouse directive malformed.", context));
      }
    if (params[1][8] == '0' || params[1][8] == '1')
      bx_options.Omouse_enabled->set (params[1][8] - '0');
    else
      BX_PANIC(("%s: mouse directive malformed.", context));
    }
  else if (!strcmp(params[0], "private_colormap")) {
    if (num_params != 2) {
      BX_PANIC(("%s: private_colormap directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      BX_PANIC(("%s: private_colormap directive malformed.", context));
      }
    if (params[1][8] == '0' || params[1][8] == '1')
      bx_options.Oprivate_colormap->set (params[1][8] - '0');
    else {
      BX_PANIC(("%s: private_colormap directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "fullscreen")) {
#if BX_WITH_AMIGAOS
    if (num_params != 2) {
      BX_PANIC(("%s: fullscreen directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      BX_PANIC(("%s: fullscreen directive malformed.", context));
      }
    if (params[1][8] == '0' || params[1][8] == '1') {
      bx_options.Ofullscreen->set (params[1][8] - '0');
    } else {
      BX_PANIC(("%s: fullscreen directive malformed.", context));
      }
#endif
    }
  else if (!strcmp(params[0], "screenmode")) {
#if BX_WITH_AMIGAOS
    if (num_params != 2) {
      BX_PANIC(("%s: screenmode directive malformed.", context));
      }
    if (strncmp(params[1], "name=", 5)) {
      BX_PANIC(("%s: screenmode directive malformed.", context));
      }
    bx_options.Oscreenmode->set (strdup(&params[1][5]));
#endif
    }

  else if (!strcmp(params[0], "sb16")) {
    for (i=1; i<num_params; i++) {
        bx_options.sb16.Opresent->set (1);
      if (!strncmp(params[i], "midi=", 5)) {
	bx_options.sb16.Omidifile->set (strdup(&params[i][5]));
        }
      else if (!strncmp(params[i], "midimode=", 9)) {
	bx_options.sb16.Omidimode->set (atol(&params[i][9]));
        }
      else if (!strncmp(params[i], "wave=", 5)) {
	bx_options.sb16.Owavefile->set (strdup(&params[i][5]));
        }
      else if (!strncmp(params[i], "wavemode=", 9)) {
	bx_options.sb16.Owavemode->set (atol(&params[i][9]));
        }
      else if (!strncmp(params[i], "log=", 4)) {
	bx_options.sb16.Ologfile->set (strdup(&params[i][4]));
        }
      else if (!strncmp(params[i], "loglevel=", 9)) {
	bx_options.sb16.Ologlevel->set (atol(&params[i][9]));
        }
      else if (!strncmp(params[i], "dmatimer=", 9)) {
	bx_options.sb16.Odmatimer->set (atol(&params[i][9]));
        }
      }
    }

  else if (!strcmp(params[0], "parport1")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enable=", 7)) {
	bx_options.par1.Oenable->set (atol(&params[i][7]));
        }
      else if (!strncmp(params[i], "file=", 5)) {
	bx_options.par1.Ooutfile->set (strdup(&params[i][5]));
        }
    }
  }

#if 0
  else if (!strcmp(params[0], "parport2")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enable=", 7)) {
	bx_options.par1.Oenable->set (atol(&params[i][7]));
        }
      else if (!strncmp(params[i], "file=", 5)) {
	bx_options.par1.Ooutfile->set (strdup(&params[i][5]));
        }
    }
  }
#endif

  else if (!strcmp(params[0], "i440fxsupport")) {
    if (num_params != 2) {
      BX_PANIC(("%s: i440FXSupport directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      BX_PANIC(("%s: i440FXSupport directive malformed.", context));
      }
    if (params[1][8] == '0')
      bx_options.Oi440FXSupport->set (0);
    else if (params[1][8] == '1')
      bx_options.Oi440FXSupport->set (1);
    else {
      BX_PANIC(("%s: i440FXSupport directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "newharddrivesupport")) {
    if (num_params != 2) {
      BX_PANIC(("%s: newharddrivesupport directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      BX_PANIC(("%s: newharddrivesupport directive malformed.", context));
      }
    if (params[1][8] == '0')
      bx_options.OnewHardDriveSupport->set (0);
    else if (params[1][8] == '1')
      bx_options.OnewHardDriveSupport->set (1);
    else {
      BX_PANIC(("%s: newharddrivesupport directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "cmosimage")) {
    if (num_params != 2) {
      BX_PANIC(("%s: cmosimage directive: wrong # args.", context));
      }
    bx_options.cmos.Opath->set (strdup(params[1]));
    bx_options.cmos.OcmosImage->set (1);                // CMOS Image is true
    }
  else if (!strcmp(params[0], "time0")) {
    if (num_params != 2) {
      BX_PANIC(("%s: time0 directive: wrong # args.", context));
      }
    bx_options.cmos.Otime0->set (atoi(params[1]));
    }
#ifdef MAGIC_BREAKPOINT
  else if (!strcmp(params[0], "magic_break")) {
    if (num_params != 2) {
      BX_PANIC(("%s: magic_break directive: wrong # args.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      BX_PANIC(("%s: magic_break directive malformed.", context));
      }
    if (params[1][8] == '0') {
      BX_INFO(("Ignoring magic break points"));
      bx_dbg.magic_break_enabled = 0;
      }
    else if (params[1][8] == '1') {
      BX_INFO(("Stopping on magic break points"));
      bx_dbg.magic_break_enabled = 1;
      }
    else {
      BX_PANIC(("%s: magic_break directive malformed.", context));
      }
    }
#endif
  else if (!strcmp(params[0], "ne2k")) {
    int tmp[6];
    char tmpchar[6];
    bx_options.ne2k.Ovalid->set (0);
    if ((num_params < 4) || (num_params > 6)) {
      BX_PANIC(("%s: ne2k directive malformed.", context));
      return;
      }
    bx_options.ne2k.Oethmod->set ("null");
    if (strncmp(params[1], "ioaddr=", 7)) {
      BX_PANIC(("%s: ne2k directive malformed.", context));
      return;
      }
    if (strncmp(params[2], "irq=", 4)) {
      BX_PANIC(("%s: ne2k directive malformed.", context));
      return;
      }
    if (strncmp(params[3], "mac=", 4)) {
      BX_PANIC(("%s: ne2k directive malformed.", context));
      return;
      }
    bx_options.ne2k.Oioaddr->set (strtoul(&params[1][7], NULL, 16));
    bx_options.ne2k.Oirq->set (atol(&params[2][4]));
    i = sscanf(&params[3][4], "%x:%x:%x:%x:%x:%x",
             &tmp[0],&tmp[1],&tmp[2],&tmp[3],&tmp[4],&tmp[5]);
    if (i != 6) {
      BX_PANIC(("%s: ne2k mac address malformed.", context));
      return;
      }
    for (i=0;i<6;i++)
      tmpchar[i] = (unsigned char)tmp[i];
    bx_options.ne2k.Omacaddr->set (tmpchar);
    if (num_params > 4) {
      if (strncmp(params[4], "ethmod=", 7)) {
      BX_PANIC(("%s: ne2k directive malformed.", context));
      return;
        }
      bx_options.ne2k.Oethmod->set (strdup(&params[4][7]));
      if (num_params == 6) {
      if (strncmp(params[5], "ethdev=", 7)) {
      BX_PANIC(("%s: ne2k directive malformed.", context));
      return;
          }
      bx_options.ne2k.Oethdev->set (strdup(&params[5][7]));
        }
      }
    bx_options.ne2k.Ovalid->set (1);
    }

  else if (!strcmp(params[0], "load32bitOSImage")) {
    if ( (num_params!=4) && (num_params!=5) ) {
      BX_PANIC(("%s: load32bitOSImage directive: wrong # args.", context));
      }
    if (strncmp(params[1], "os=", 3)) {
      BX_PANIC(("%s: load32bitOSImage: directive malformed.", context));
      }
    if (!strcmp(&params[1][3], "nullkernel")) {
      bx_options.load32bitOSImage.OwhichOS->set (Load32bitOSNullKernel);
      }
    else if (!strcmp(&params[1][3], "linux")) {
      bx_options.load32bitOSImage.OwhichOS->set (Load32bitOSLinux);
      }
    else {
      BX_PANIC(("%s: load32bitOSImage: unsupported OS.", context));
      }
    if (strncmp(params[2], "path=", 5)) {
      BX_PANIC(("%s: load32bitOSImage: directive malformed.", context));
      }
    if (strncmp(params[3], "iolog=", 6)) {
      BX_PANIC(("%s: load32bitOSImage: directive malformed.", context));
      }
    bx_options.load32bitOSImage.Opath->set (strdup(&params[2][5]));
    bx_options.load32bitOSImage.Oiolog->set (strdup(&params[3][6]));
    if (num_params == 5) {
      if (strncmp(params[4], "initrd=", 7)) {
        BX_PANIC(("%s: load32bitOSImage: directive malformed.", context));
        }
      bx_options.load32bitOSImage.Oinitrd->set (strdup(&params[4][7]));
      }
    }
  else if (!strcmp(params[0], "keyboard_type")) {
    if (num_params != 2) {
      BX_PANIC(("%s: keyboard_type directive: wrong # args.", context));
      }
    if(strcmp(params[1],"xt")==0){
      bx_options.Okeyboard_type->set (BX_KBD_XT_TYPE);
      }
    else if(strcmp(params[1],"at")==0){
      bx_options.Okeyboard_type->set (BX_KBD_AT_TYPE);
      }
    else if(strcmp(params[1],"mf")==0){
      bx_options.Okeyboard_type->set (BX_KBD_MF_TYPE);
      }
    else{
      BX_PANIC(("%s: keyboard_type directive: wrong arg %s.", context,params[1]));
      }
    }

  else if (!strcmp(params[0], "keyboard_mapping")
         ||!strcmp(params[0], "keyboardmapping")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        bx_options.keyboard.OuseMapping->set (atol(&params[i][8]));
        }
      else if (!strncmp(params[i], "map=", 4)) {
        bx_options.keyboard.Okeymap->set (strdup(&params[i][4]));
        }
    }
  }


  else {
    BX_PANIC(( "%s: directive '%s' not understood", context, params[0]));
    }

  if (bx_options.diskd.Opresent->get () && bx_options.cdromd.Opresent->get ())
    BX_PANIC(("At present, using both diskd and cdromd at once is not supported."));
}

static char *fdtypes[] = {
  "none", "1_2", "1_44", "2_88", "720k"
};

int 
bx_write_floppy_options (FILE *fp, int drive, bx_floppy_options *opt)
{
  BX_ASSERT (drive==0 || drive==1);
  if (opt->Otype->get () == BX_FLOPPY_NONE) {
    fprintf (fp, "# no floppy%c\n", (char)'a'+drive);
    return 0;
  }
  BX_ASSERT (opt->Otype->get () > BX_FLOPPY_NONE && opt->Otype->get () <= BX_FLOPPY_LAST);
  fprintf (fp, "floppy%c: %s=\"%s\", status=%s\n", 
    (char)'a'+drive, 
    fdtypes[opt->Otype->get () - BX_FLOPPY_NONE],
    opt->Opath->getptr (),
    opt->Oinitial_status->get ()==BX_EJECTED ? "ejected" : "inserted");
  return 0;
}

int 
bx_write_disk_options (FILE *fp, int drive, bx_disk_options *opt)
{
  if (!opt->Opresent->get ()) {
    fprintf (fp, "# no disk%c\n", (char)'c'+drive);
    return 0;
  }
  fprintf (fp, "disk%c: file=\"%s\", cyl=%d, heads=%d, spt=%d\n",
     (char)'c'+drive,
     opt->Opath->getptr (),
     opt->Ocylinders->get (),
     opt->Oheads->get (),
     opt->Ospt->get ());
  return 0;
}

int 
bx_write_cdrom_options (FILE *fp, int drive, bx_cdrom_options *opt)
{
  BX_ASSERT (drive == 0);
  if (!opt->Opresent->get ()) {
    fprintf (fp, "# no cdromd\n");
    return 0;
  }
  fprintf (fp, "cdromd: dev=%s, status=%s\n",
    opt->Opath->getptr (),
    opt->Oinserted->get ()==BX_INSERTED ? "inserted" : "ejected");
  return 0;
}

int
bx_write_parport_options (FILE *fp, bx_parport_options *opt, int n)
{
  if (!opt->Oenable->get ()) {
    fprintf (fp, "# no parport #%d\n", n);
    return 0;
  }
  fprintf (fp, "parport%d: enable=%d", n, opt->Oenable->get ());
  if (opt->Oenable->get ()) {
  fprintf (fp, ", file=\"%s\"", opt->Ooutfile->getptr ());
  }
  fprintf (fp, "\n");
  return 0;
}

int
bx_write_sb16_options (FILE *fp, bx_sb16_options *opt)
{
  if (!opt->Opresent->get ()) {
    fprintf (fp, "# no sb16\n");
    return 0;
  }
  fprintf (fp, "sb16: midimode=%d, midi=%s, wavemode=%d, wave=%s, loglevel=%d, log=%s, dmatimer=%d\n", opt->Omidimode->get (), opt->Omidifile->getptr (), opt->Owavemode->get (), opt->Owavefile->getptr (), opt->Ologlevel->get (), opt->Ologfile->getptr (), opt->Odmatimer->get ());
  return 0;
}

int
bx_write_ne2k_options (FILE *fp, bx_ne2k_options *opt)
{
  if (!opt->Ovalid->get ()) {
    fprintf (fp, "# no ne2k\n");
    return 0;
  }
  char *ptr = opt->Omacaddr->getptr ();
  fprintf (fp, "ne2k: ioaddr=0x%x, irq=%d, mac=%02x:%02x:%02x:%02x:%02x:%02x, ethmod=%s, ethdev=%s\n",
      opt->Oioaddr->get (), 
      opt->Oirq->get (),
      (unsigned int)(0xff & ptr[0]),
      (unsigned int)(0xff & ptr[1]),
      (unsigned int)(0xff & ptr[2]),
      (unsigned int)(0xff & ptr[3]),
      (unsigned int)(0xff & ptr[4]),
      (unsigned int)(0xff & ptr[5]),
      opt->Oethmod->getptr (),
      opt->Oethdev->getptr ());
  return 0;
}

int
bx_write_loader_options (FILE *fp, bx_load32bitOSImage_t *opt)
{
  if (opt->OwhichOS->get () == 0) {
    fprintf (fp, "# no loader\n");
    return 0;
  }
  BX_ASSERT(opt->OwhichOS->get () == Load32bitOSLinux || opt->OwhichOS->get () == Load32bitOSNullKernel);
  fprintf (fp, "load32bitOSImage: os=%s, path=%s, iolog=%s, initrd=%s\n",
      (opt->OwhichOS->get () == Load32bitOSLinux) ? "linux" : "nullkernel",
      opt->Opath->getptr (),
      opt->Oiolog->getptr (),
      opt->Oinitrd->getptr ());
  return 0;
}

int
bx_write_log_options (FILE *fp, bx_log_options *opt)
{
  fprintf (fp, "log: %s\n", opt->Ofilename->getptr ());
  // no syntax to describe all the possible action settings for every 
  // device.  Instead, take a vote and record the most popular action
  // for each level of event.
  int action_tally[N_ACT];
  int most_popular_action[N_LOGLEV];
  int i,lev;
  for (lev = 0; lev < N_LOGLEV; lev++) {
    // clear tally
    for (i=0; i<N_ACT; i++)
      action_tally[i] = 0;
    // count how many devices use each action
    for (i=0; i<io->get_n_logfns (); i++) {
      logfunc_t *fn = io->get_logfn (i);
      int action = fn->getonoff(lev);
      BX_ASSERT (action >= 0 && action < N_ACT);
      action_tally[action]++;
    }
    // count the votes
    int best = 0, best_votes = action_tally[0];
    for (i=1; i<N_ACT; i++) {
      if (action_tally[i] > best_votes) {
	best = i;
	best_votes = action_tally[i];
      }
    }
    most_popular_action[lev] = best;
  }
  fprintf (fp, "panic: action=%s\n",
      io->getaction(most_popular_action[LOGLEV_PANIC]));
  fprintf (fp, "error: action=%s\n",
      io->getaction(most_popular_action[LOGLEV_ERROR]));
  fprintf (fp, "info: action=%s\n",
      io->getaction(most_popular_action[LOGLEV_INFO]));
  fprintf (fp, "debug: action=%s\n",
      io->getaction(most_popular_action[LOGLEV_DEBUG]));
  return 0;
}

int
bx_write_keyboard_options (FILE *fp, bx_keyboard_options *opt)
{
  fprintf (fp, "keyboard_mapping: enabled=%d, map=%s\n", opt->OuseMapping->get(), opt->Okeymap->getptr());
  return 0;
}

// return values:
//   0: written ok
//  -1: failed
//  -2: already exists, and overwrite was off
int
bx_write_configuration (char *rc, int overwrite)
{
  BX_INFO (("write configuration to %s\n", rc));
  // check if it exists.  If so, only proceed if overwrite is set.
  FILE *fp = fopen (rc, "r");
  if (fp != NULL) {
    fclose (fp);
    if (!overwrite) return -2;
  }
  fp = fopen (rc, "w");
  if (fp == NULL) return -1;
  // finally it's open and we can start writing.
  fprintf (fp, "# configuration file generated by Bochs\n");
  // it would be nice to put this type of function as methods on
  // the structs like bx_floppy_options::print or something.
  bx_write_floppy_options (fp, 0, &bx_options.floppya);
  bx_write_floppy_options (fp, 1, &bx_options.floppyb);
  bx_write_disk_options (fp, 0, &bx_options.diskc);
  bx_write_disk_options (fp, 1, &bx_options.diskd);
  bx_write_cdrom_options (fp, 0, &bx_options.cdromd);
  if (strlen (bx_options.rom.Opath->getptr ()) > 0)
    fprintf (fp, "romimage: file=%s, address=0x%05x\n", bx_options.rom.Opath->getptr(), (unsigned int)bx_options.rom.Oaddress->get ());
  else
    fprintf (fp, "# no romimage\n");
  if (strlen (bx_options.vgarom.Opath->getptr ()) > 0)
    fprintf (fp, "vgaromimage: %s\n", bx_options.vgarom.Opath->getptr ());
  else
    fprintf (fp, "# no vgaromimage\n");
  fprintf (fp, "megs: %d\n", bx_options.memory.Osize->get ());
  bx_write_parport_options (fp, &bx_options.par1, 1);
  //bx_write_parport_options (fp, &bx_options.par2);
  bx_write_sb16_options (fp, &bx_options.sb16);
  int bootdrive = bx_options.Obootdrive->get ();
  fprintf (fp, "boot: %s\n", (bootdrive==BX_BOOT_FLOPPYA) ? "a" : (bootdrive==BX_BOOT_DISKC) ? "c" : "cdrom");
  fprintf (fp, "vga_update_interval: %u\n", bx_options.Ovga_update_interval->get ());
  fprintf (fp, "keyboard_serial_delay: %u\n", bx_options.Okeyboard_serial_delay->get ());
  fprintf (fp, "keyboard_paste_delay: %u\n", bx_options.Okeyboard_paste_delay->get ());
  fprintf (fp, "floppy_command_delay: %u\n", bx_options.Ofloppy_command_delay->get ());
  fprintf (fp, "ips: %u\n", bx_options.Oips->get ());
  fprintf (fp, "mouse: enabled=%d\n", bx_options.Omouse_enabled->get ());
  fprintf (fp, "private_colormap: enabled=%d\n", bx_options.Oprivate_colormap->get ());
#if BX_WITH_AMIGAOS
  fprintf (fp, "fullscreen: enabled=%d\n", bx_options.Ofullscreen->get ());
  fprintf (fp, "screenmode: name=\"%s\"\n", bx_options.Oscreenmode->getptr ());
#endif
  fprintf (fp, "i440fxsupport: enabled=%d\n", bx_options.Oi440FXSupport->get ());
  fprintf (fp, "time0: %u\n", bx_options.cmos.Otime0->get ());
  bx_write_ne2k_options (fp, &bx_options.ne2k);
  fprintf (fp, "newharddrivesupport: enabled=%d\n", bx_options.OnewHardDriveSupport->get ());
  bx_write_loader_options (fp, &bx_options.load32bitOSImage);
  bx_write_log_options (fp, &bx_options.log);
  bx_write_keyboard_options (fp, &bx_options.keyboard);
  fprintf (fp, "keyboard_type: %s\n", bx_options.Okeyboard_type->get ()==BX_KBD_XT_TYPE?"xt":
                                       bx_options.Okeyboard_type->get ()==BX_KBD_AT_TYPE?"at":"mf");
  fclose (fp);
  return 0;
}
#endif // #if BX_PROVIDE_MAIN

  void
bx_signal_handler( int signum)
{
#if BX_GUI_SIGHANDLER
  // GUI signal handler gets first priority, if the mask says it's wanted
  if ((1<<signum) & bx_gui.get_sighandler_mask ()) {
    bx_gui.sighandler (signum);
    return;
  }
#endif

#if BX_SHOW_IPS
  extern unsigned long ips_count;

  if (signum == SIGALRM ) {
    BX_INFO(("ips = %lu", ips_count));
    ips_count = 0;
#ifndef __MINGW32__
    signal(SIGALRM, bx_signal_handler);
    alarm( 1 );
#endif
    return;
    }
#endif

#if BX_GUI_SIGHANDLER
  if ((1<<signum) & bx_gui.get_sighandler_mask ()) {
    bx_gui.sighandler (signum);
    return;
  }
#endif
  BX_PANIC(("SIGNAL %u caught", signum));
}

