/////////////////////////////////////////////////////////////////////////
// $Id: main.cc,v 1.186 2002-11-19 08:54:32 bdenney Exp $
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


#include "bochs.h"
#include <assert.h>
#include "state_file.h"

#if BX_WITH_SDL
// since SDL redefines main() to SDL_main(), we must include SDL.h so that the
// C language prototype is found.  Otherwise SDL_main() will get its name 
// mangled and not match what the SDL library is expecting.
#include <SDL/SDL.h>

#if defined(macintosh)
// Work around a bug in SDL 1.2.4 on MacOS X, which redefines getenv to
// SDL_getenv, but then neglects to provide SDL_getenv.  It happens
// because we are defining -Dmacintosh.
#undef getenv
#endif
#endif

#if BX_WITH_CARBON
#include <Carbon/Carbon.h>
#endif

int bochsrc_include_count = 0;

extern "C" {
#include <signal.h>
}

#ifdef __MINGW32__
void alarm(int);
#endif

#if BX_GUI_SIGHANDLER
bx_bool bx_gui_sighandler = 0;
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
static char *divider = "========================================================================";
static logfunctions thePluginLog;
logfunctions *pluginlog = &thePluginLog;

bx_startup_flags_t bx_startup_flags;

/* typedefs */

#define LOG_THIS genlog->

#if ( BX_PROVIDE_DEVICE_MODELS==1 )
bx_pc_system_c bx_pc_system;
class state_file state_stuff("state_file.out", "options");
#endif

bx_debug_t bx_dbg;

bx_options_t bx_options; // initialized in bx_init_options()
char *bochsrc_filename = NULL;

static Bit32s parse_line_unformatted(char *context, char *line);
static Bit32s parse_line_formatted(char *context, int num_params, char *params[]);
static int parse_bochsrc(char *rcfile);

static Bit64s
bx_param_handler (bx_param_c *param, int set, Bit64s val)
{
  bx_id id = param->get_id ();
  switch (id) {
    case BXP_VGA_UPDATE_INTERVAL:
      // if after init, notify the vga device to change its timer.
      if (set && SIM->get_init_done ())
        DEV_vga_set_update_interval (val);
      break;
    case BXP_MOUSE_ENABLED:
      // if after init, notify the GUI
      if (set && SIM->get_init_done ()) {
        bx_gui->mouse_enabled_changed (val!=0);
        DEV_mouse_enabled_changed (val!=0);
      }
      break;
    case BXP_NE2K_PRESENT:
      if (set) {
        int enable = (val != 0);
        SIM->get_param (BXP_NE2K_IOADDR)->set_enabled (enable);
        SIM->get_param (BXP_NE2K_IRQ)->set_enabled (enable);
        SIM->get_param (BXP_NE2K_MACADDR)->set_enabled (enable);
        SIM->get_param (BXP_NE2K_ETHMOD)->set_enabled (enable);
        SIM->get_param (BXP_NE2K_ETHDEV)->set_enabled (enable);
        SIM->get_param (BXP_NE2K_SCRIPT)->set_enabled (enable);
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
    case BXP_ATA0_MASTER_STATUS:
    case BXP_ATA0_SLAVE_STATUS:
    case BXP_ATA1_MASTER_STATUS:
    case BXP_ATA1_SLAVE_STATUS:
    case BXP_ATA2_MASTER_STATUS:
    case BXP_ATA2_SLAVE_STATUS:
    case BXP_ATA3_MASTER_STATUS:
    case BXP_ATA3_SLAVE_STATUS:
      if ((set) && (SIM->get_init_done ())) {
        Bit8u device = id - BXP_ATA0_MASTER_STATUS;
        Bit32u handle = DEV_hd_get_device_handle (device/2, device%2);
        DEV_hd_set_cd_media_status(handle, val == BX_INSERTED);
        bx_gui->update_drive_status_buttons ();
      }
      break;
    case BXP_FLOPPYA_TYPE:
      if ((set) && (!SIM->get_init_done ())) {
        bx_options.floppya.Odevtype->set (val);
      }
      break;
    case BXP_FLOPPYA_STATUS:
      if ((set) && (SIM->get_init_done ())) {
        DEV_floppy_set_media_status(0, val == BX_INSERTED);
        bx_gui->update_drive_status_buttons ();
      }
      break;
    case BXP_FLOPPYB_TYPE:
      if ((set) && (!SIM->get_init_done ())) {
        bx_options.floppyb.Odevtype->set (val);
      }
      break;
    case BXP_FLOPPYB_STATUS:
      if ((set) && (SIM->get_init_done ())) {
        DEV_floppy_set_media_status(1, val == BX_INSERTED);
        bx_gui->update_drive_status_buttons ();
      }
      break;
    case BXP_KBD_PASTE_DELAY:
      if ((set) && (SIM->get_init_done ())) {
        DEV_kbd_paste_delay_changed ();
        }
      break;
    case BXP_ATA0_MASTER_TYPE:
    case BXP_ATA0_SLAVE_TYPE:
    case BXP_ATA1_MASTER_TYPE:
    case BXP_ATA1_SLAVE_TYPE:
    case BXP_ATA2_MASTER_TYPE:
    case BXP_ATA2_SLAVE_TYPE:
    case BXP_ATA3_MASTER_TYPE:
    case BXP_ATA3_SLAVE_TYPE:
      if (set) {
        int device = id - BXP_ATA0_MASTER_TYPE;
        switch (val) {
          case BX_ATA_DEVICE_DISK:
            SIM->get_param_num ((bx_id)(BXP_ATA0_MASTER_PRESENT + device))->set (1);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_PATH + device))->set_enabled (1);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_CYLINDERS + device))->set_enabled (1);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_HEADS + device))->set_enabled (1);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_SPT + device))->set_enabled (1);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_STATUS + device))->set_enabled (0);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_MODEL + device))->set_enabled (1);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_BIOSDETECT + device))->set_enabled (1);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_TRANSLATION + device))->set_enabled (1);
            break;
          case BX_ATA_DEVICE_CDROM:
            SIM->get_param_num ((bx_id)(BXP_ATA0_MASTER_PRESENT + device))->set (1);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_PATH + device))->set_enabled (1);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_CYLINDERS + device))->set_enabled (0);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_HEADS + device))->set_enabled (0);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_SPT + device))->set_enabled (0);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_STATUS + device))->set_enabled (1);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_MODEL + device))->set_enabled (1);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_BIOSDETECT + device))->set_enabled (1);
            SIM->get_param ((bx_id)(BXP_ATA0_MASTER_TRANSLATION + device))->set_enabled (0);
            break;
          }
        }
      break;
    default:
      BX_PANIC (("bx_param_handler called with unknown id %d", id));
      return -1;
  }
  return val;
}

char *bx_param_string_handler (bx_param_string_c *param, int set, char *val, int maxlen)
{
  bx_id id = param->get_id ();

  int empty = 0;
  if ((strlen(val) < 1) || !strcmp ("none", val)) {
    empty = 1;
    val = "none";
  }
  switch (id) {
    case BXP_FLOPPYA_PATH:
      if (set==1) {
        if (SIM->get_init_done ()) {
          if (empty) {
            DEV_floppy_set_media_status(0, 0);
            bx_gui->update_drive_status_buttons ();
          } else {
            if (!SIM->get_param_num(BXP_FLOPPYA_TYPE)->get_enabled()) {
              BX_ERROR(("Cannot add a floppy drive at runtime"));
              bx_options.floppya.Opath->set ("none");
            }
          }
          if ((DEV_floppy_present()) &&
              (SIM->get_param_num(BXP_FLOPPYA_STATUS)->get () == BX_INSERTED)) {
            // tell the device model that we removed, then inserted the disk
            DEV_floppy_set_media_status(0, 0);
            DEV_floppy_set_media_status(0, 1);
          }
        } else {
          SIM->get_param_num(BXP_FLOPPYA_DEVTYPE)->set_enabled (!empty);
          SIM->get_param_num(BXP_FLOPPYA_TYPE)->set_enabled (!empty);
          SIM->get_param_num(BXP_FLOPPYA_STATUS)->set_enabled (!empty);
        }
      }
      break;
    case BXP_FLOPPYB_PATH:
      if (set==1) {
        if (SIM->get_init_done ()) {
          if (empty) {
            DEV_floppy_set_media_status(1, 0);
            bx_gui->update_drive_status_buttons ();
          } else {
            if (!SIM->get_param_num(BXP_FLOPPYB_TYPE)->get_enabled ()) {
              BX_ERROR(("Cannot add a floppy drive at runtime"));
              bx_options.floppyb.Opath->set ("none");
            }
          }
          if ((DEV_floppy_present()) &&
              (SIM->get_param_num(BXP_FLOPPYB_STATUS)->get () == BX_INSERTED)) {
            // tell the device model that we removed, then inserted the disk
            DEV_floppy_set_media_status(1, 0);
            DEV_floppy_set_media_status(1, 1);
          }
        } else {
          SIM->get_param_num(BXP_FLOPPYB_DEVTYPE)->set_enabled (!empty);
          SIM->get_param_num(BXP_FLOPPYB_TYPE)->set_enabled (!empty);
          SIM->get_param_num(BXP_FLOPPYB_STATUS)->set_enabled (!empty);
        }
      }
      break;

    case BXP_ATA0_MASTER_PATH:
    case BXP_ATA0_SLAVE_PATH:
    case BXP_ATA1_MASTER_PATH:
    case BXP_ATA1_SLAVE_PATH:
    case BXP_ATA2_MASTER_PATH:
    case BXP_ATA2_SLAVE_PATH:
    case BXP_ATA3_MASTER_PATH:
    case BXP_ATA3_SLAVE_PATH:
      if (set==1) {
        if (SIM->get_init_done ()) {

          Bit8u device = id - BXP_ATA0_MASTER_PATH;
          Bit32u handle = DEV_hd_get_device_handle(device/2, device%2);

          if (empty) {
            DEV_hd_set_cd_media_status(handle, 0);
            bx_gui->update_drive_status_buttons ();
          } else {
            if (!SIM->get_param_num((bx_id)(BXP_ATA0_MASTER_PRESENT + device))->get ()) {
              BX_ERROR(("Cannot add a cdrom drive at runtime"));
              bx_options.atadevice[device/2][device%2].Opresent->set (0);
            }
            if (SIM->get_param_num((bx_id)(BXP_ATA0_MASTER_TYPE + device))->get () != BX_ATA_DEVICE_CDROM) {
              BX_ERROR(("Device is not a cdrom drive"));
              bx_options.atadevice[device/2][device%2].Opresent->set (0);
            }
          }
          if (DEV_hd_present() &&
              (SIM->get_param_num((bx_id)(BXP_ATA0_MASTER_STATUS + device))->get () == BX_INSERTED) &&
              (SIM->get_param_num((bx_id)(BXP_ATA0_MASTER_TYPE + device))->get () == BX_ATA_DEVICE_CDROM)) {
            // tell the device model that we removed, then inserted the cd
            DEV_hd_set_cd_media_status(handle, 0);
            DEV_hd_set_cd_media_status(handle, 1);
          }
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
  int i;
  bx_list_c *menu;
  bx_list_c *deplist;
  char name[1024], descr[1024];

  memset (&bx_options, 0, sizeof(bx_options));

  // quick start option, set by command line arg
  new bx_param_enum_c (BXP_BOCHS_START,
      "Bochs start types",
      "Bochs start types",
      bochs_start_names,
      BX_RUN_START,
      BX_QUICK_START);

  // floppya
  bx_options.floppya.Opath = new bx_param_filename_c (BXP_FLOPPYA_PATH,
      "Floppy A image",
      "Pathname of first floppy image file or device.  If you're booting from floppy, this should be a bootable floppy.",
      "", BX_PATHNAME_LEN);
#if BX_WITH_WX
  bx_options.floppya.Opath->set_ask_format ("Filename of first floppy image");
#else
  bx_options.floppya.Opath->set_ask_format ("Enter new filename, or 'none' for no disk: [%s] ");
#endif
  bx_options.floppya.Odevtype = new bx_param_enum_c (BXP_FLOPPYA_DEVTYPE,
      "floppya:devtype",
      "Type of floppy drive",
      floppy_type_names,
      BX_FLOPPY_NONE,
      BX_FLOPPY_NONE);
  bx_options.floppya.Otype = new bx_param_enum_c (BXP_FLOPPYA_TYPE,
      "floppya:type",
      "Type of floppy disk",
      floppy_type_names,
      BX_FLOPPY_NONE,
      BX_FLOPPY_NONE);
  bx_options.floppya.Otype->set_ask_format ("What type of floppy disk? [%s] ");
  bx_options.floppya.Ostatus = new bx_param_enum_c (BXP_FLOPPYA_STATUS,
      "Is floppya inserted",
      "Inserted or ejected",
      floppy_status_names,
      BX_INSERTED,
      BX_EJECTED);
  bx_options.floppya.Ostatus->set_ask_format ("Is the floppy inserted or ejected? [%s] ");
  bx_options.floppya.Opath->set_format ("%s");
  bx_options.floppya.Otype->set_format (", size=%s, ");
  bx_options.floppya.Ostatus->set_format ("%s");
  bx_param_c *floppya_init_list[] = {
    // if the order "path,type,status" changes, corresponding changes must
    // be made in gui/wxmain.cc, MyFrame::editFloppyConfig.
    bx_options.floppya.Opath,
    bx_options.floppya.Otype,
    bx_options.floppya.Ostatus,
    NULL
  };
  menu = new bx_list_c (BXP_FLOPPYA, "Floppy Disk 0", "All options for first floppy disk", floppya_init_list);
  menu->get_options ()->set (menu->SERIES_ASK);
  bx_options.floppya.Opath->set_handler (bx_param_string_handler);
  bx_options.floppya.Opath->set ("none");
  bx_options.floppya.Otype->set_handler (bx_param_handler);
  bx_options.floppya.Ostatus->set_handler (bx_param_handler);

  bx_options.floppyb.Opath = new bx_param_filename_c (BXP_FLOPPYB_PATH,
      "floppyb:path",
      "Pathname of second floppy image file or device.",
      "", BX_PATHNAME_LEN);
#if BX_WITH_WX
  bx_options.floppyb.Opath->set_ask_format ("Filename of second floppy image");
#else
  bx_options.floppyb.Opath->set_ask_format ("Enter new filename, or 'none' for no disk: [%s] ");
#endif
  bx_options.floppyb.Odevtype = new bx_param_enum_c (BXP_FLOPPYB_DEVTYPE,
      "floppyb:devtype",
      "Type of floppy drive",
      floppy_type_names,
      BX_FLOPPY_NONE,
      BX_FLOPPY_NONE);
  bx_options.floppyb.Otype = new bx_param_enum_c (BXP_FLOPPYB_TYPE,
      "floppyb:type",
      "Type of floppy disk",
      floppy_type_names,
      BX_FLOPPY_NONE,
      BX_FLOPPY_NONE);
  bx_options.floppyb.Otype->set_ask_format ("What type of floppy disk? [%s] ");
  bx_options.floppyb.Ostatus = new bx_param_enum_c (BXP_FLOPPYB_STATUS,
      "Is floppyb inserted",
      "Inserted or ejected",
      floppy_status_names,
      BX_INSERTED,
      BX_EJECTED);
  bx_options.floppyb.Ostatus->set_ask_format ("Is the floppy inserted or ejected? [%s] ");
  bx_options.floppyb.Ostatus->set_format ("%s");
  bx_options.floppyb.Opath->set_format ("%s");
  bx_options.floppyb.Otype->set_format (", size=%s, ");
  bx_options.floppyb.Ostatus->set_format ("%s");
  bx_param_c *floppyb_init_list[] = {
    bx_options.floppyb.Opath,
    bx_options.floppyb.Otype,
    bx_options.floppyb.Ostatus,
    NULL
  };
  menu = new bx_list_c (BXP_FLOPPYB, "Floppy Disk 1", "All options for second floppy disk", floppyb_init_list);
  menu->get_options ()->set (menu->SERIES_ASK);
  bx_options.floppyb.Opath->set_handler (bx_param_string_handler);
  bx_options.floppyb.Opath->set ("none");
  bx_options.floppyb.Otype->set_handler (bx_param_handler);
  bx_options.floppyb.Ostatus->set_handler (bx_param_handler);

  // disk options

  // FIXME use descr and name
  char *s_atachannel[] = {
    "ATA channel 0",
    "ATA channel 1",
    "ATA channel 2",
    "ATA channel 3",
    };
  char *s_atadevice[4][2] = {
    { "First HD/CD on channel 0",
      "Second HD/CD on channel 0" },
    { "First HD/CD on channel 1",
    "Second HD/CD on channel 1" },
    { "First HD/CD on channel 2",
    "Second HD/CD on channel 2" },
    { "First HD/CD on channel 3",
    "Second HD/CD on channel 3" }
    };
  Bit16u ata_default_ioaddr1[BX_MAX_ATA_CHANNEL] = {
    0x1f0, 0x170, 0x1e8, 0x168 
  };
  Bit8u ata_default_irq[BX_MAX_ATA_CHANNEL] = { 
    14, 15, 11, 9 
  };

  bx_list_c *ata[BX_MAX_ATA_CHANNEL];
  bx_list_c *ata_menu[BX_MAX_ATA_CHANNEL];

  Bit8u channel;
  for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel ++) {

    ata[channel] = new bx_list_c ((bx_id)(BXP_ATA0+channel), s_atachannel[channel], s_atachannel[channel], 8);
    ata[channel]->get_options ()->set (bx_list_c::SERIES_ASK);

    ata[channel]->add (bx_options.ata[channel].Opresent = new bx_param_bool_c ((bx_id)(BXP_ATA0_PRESENT+channel),
      "ata:present",                                
      "Controls whether ata channel is installed or not",
      0));

    ata[channel]->add (bx_options.ata[channel].Oioaddr1 = new bx_param_num_c ((bx_id)(BXP_ATA0_IOADDR1+channel),
      "ata:ioaddr1",
      "IO adress of ata command block",
      0, 0xffff,
      ata_default_ioaddr1[channel]));

    ata[channel]->add (bx_options.ata[channel].Oioaddr2 = new bx_param_num_c ((bx_id)(BXP_ATA0_IOADDR2+channel),
      "ata:ioaddr2",
      "IO adress of ata control block",
      0, 0xffff,
      ata_default_ioaddr1[channel] + 0x200));

    ata[channel]->add (bx_options.ata[channel].Oirq = new bx_param_num_c ((bx_id)(BXP_ATA0_IRQ+channel),
      "ata:irq",
      "IRQ of ata ",
      0, 15,
      ata_default_irq[channel]));

    // all items in the ata[channel] menu depend on the present flag.
    // The menu list is complete, but a few dependent_list items will
    // be added later.  Use clone() to make a copy of the dependent_list
    // so that it can be changed without affecting the menu.
    bx_options.ata[channel].Opresent->set_dependent_list (
        ata[channel]->clone());

    for (Bit8u slave=0; slave<2; slave++) {

      menu = bx_options.atadevice[channel][slave].Omenu = new bx_list_c ((bx_id)(BXP_ATA0_MASTER+channel*2+slave),
          s_atadevice[channel][slave], 
          s_atadevice[channel][slave],
          12 /* list max size */);
      menu->get_options ()->set (menu->SERIES_ASK);

      menu->add (bx_options.atadevice[channel][slave].Opresent = new bx_param_bool_c ((bx_id)(BXP_ATA0_MASTER_PRESENT+channel*2+slave),
        "ata-device:present",                                
        "Controls whether ata device is installed or not",  
        0));

      menu->add (bx_options.atadevice[channel][slave].Otype = new bx_param_enum_c ((bx_id)(BXP_ATA0_MASTER_TYPE+channel*2+slave),
          "ata-device:type",
          "Type of ATA device",
          atadevice_type_names,
          BX_ATA_DEVICE_DISK,
          BX_ATA_DEVICE_DISK));

      menu->add (bx_options.atadevice[channel][slave].Opath = new bx_param_filename_c ((bx_id)(BXP_ATA0_MASTER_PATH+channel*2+slave),
          "ata-device:path",
          "Pathname of the image",
          "", BX_PATHNAME_LEN));

      menu->add (bx_options.atadevice[channel][slave].Ocylinders = new bx_param_num_c ((bx_id)(BXP_ATA0_MASTER_CYLINDERS+channel*2+slave),
          "ata-device:cylinders",
          "Number of cylinders",
          0, 65535,
          0));
      menu->add (bx_options.atadevice[channel][slave].Oheads = new bx_param_num_c ((bx_id)(BXP_ATA0_MASTER_HEADS+channel*2+slave),
          "ata-device:heads",
          "Number of heads",
          0, 65535,
          0));
      menu->add (bx_options.atadevice[channel][slave].Ospt = new bx_param_num_c ((bx_id)(BXP_ATA0_MASTER_SPT+channel*2+slave),
          "ata-device:spt",
          "Number of sectors per track",
          0, 65535,
          0));
      
      menu->add (bx_options.atadevice[channel][slave].Ostatus = new bx_param_enum_c ((bx_id)(BXP_ATA0_MASTER_STATUS+channel*2+slave),
       "ata-device:status",
       "Inserted or ejected",
       atadevice_status_names,
       BX_INSERTED,
       BX_EJECTED));

      menu->add (bx_options.atadevice[channel][slave].Omodel = new bx_param_string_c ((bx_id)(BXP_ATA0_MASTER_MODEL+channel*2+slave),
       "ata-device:model",
       "Model name",
       "Generic 1234", 40));

      menu->add (bx_options.atadevice[channel][slave].Obiosdetect = new bx_param_enum_c ((bx_id)(BXP_ATA0_MASTER_BIOSDETECT+channel*2+slave),
       "ata-device:biosdetect",
       "Type of bios detection",
       atadevice_biosdetect_names,
       BX_ATA_BIOSDETECT_AUTO,
       BX_ATA_BIOSDETECT_NONE));

      menu->add (bx_options.atadevice[channel][slave].Otranslation = new bx_param_enum_c ((bx_id)(BXP_ATA0_MASTER_TRANSLATION+channel*2+slave),
       "How the ata-disk translation is done by the bios",
       "Type of translation",
       atadevice_translation_names,
       BX_ATA_TRANSLATION_AUTO,
       BX_ATA_TRANSLATION_NONE));

      bx_options.atadevice[channel][slave].Opresent->set_dependent_list (
          menu->clone ());
      // the menu and all items on it depend on the Opresent flag
      bx_options.atadevice[channel][slave].Opresent->get_dependent_list()->add(menu);
      // the present flag depends on the ATA channel's present flag
      bx_options.ata[channel].Opresent->get_dependent_list()->add (
          bx_options.atadevice[channel][slave].Opresent);
      }

      // set up top level menu for ATA[i] controller configuration.  This list
      // controls what will appear on the ATA configure dialog.  It now
      // requests the USE_TAB_WINDOW display, which is implemented in wx.
      char buffer[32];
      sprintf (buffer, "Configure ATA%d", channel);
      ata_menu[channel] = new bx_list_c ((bx_id)(BXP_ATA0_MENU+channel), strdup(buffer), "", 4);
      ata_menu[channel]->add (ata[channel]);
      ata_menu[channel]->add (bx_options.atadevice[channel][0].Omenu);
      ata_menu[channel]->add (bx_options.atadevice[channel][1].Omenu);
      ata_menu[channel]->get_options()->set (bx_list_c::USE_TAB_WINDOW);
    }

  // Enable first ata interface by default, disable the others.
  bx_options.ata[0].Opresent->set_initial_val(1);

  // now that the dependence relationships are established, call set() on
  // the ata device present params to set all enables correctly.
  for (i=0; i<BX_MAX_ATA_CHANNEL; i++)
    bx_options.ata[i].Opresent->set (i==0);

  for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel ++) {

    bx_options.ata[channel].Opresent->set_ask_format (
        BX_WITH_WX? "Enable this channel?"
        : "Channel is enabled: [%s] ");
    bx_options.ata[channel].Oioaddr1->set_ask_format (
        BX_WITH_WX? "I/O Address 1:"
        : "Enter new ioaddr1: [0x%x] ");
    bx_options.ata[channel].Oioaddr2->set_ask_format (
        BX_WITH_WX? "I/O Address 2:"
        : "Enter new ioaddr2: [0x%x] ");
    bx_options.ata[channel].Oirq->set_ask_format (
        BX_WITH_WX? "IRQ:"
        : "Enter new IRQ: [%d] ");
#if !BX_WITH_WX
    bx_options.ata[channel].Opresent->set_format ("enabled: %s");
    bx_options.ata[channel].Oioaddr1->set_format (", ioaddr1: 0x%x");
    bx_options.ata[channel].Oioaddr2->set_format (", ioaddr2: 0x%x");
    bx_options.ata[channel].Oirq->set_format (", irq: %d");
#endif
    bx_options.ata[channel].Oioaddr1->set_base (16);
    bx_options.ata[channel].Oioaddr2->set_base (16);

    for (Bit8u slave=0; slave<2; slave++) {

      bx_options.atadevice[channel][slave].Opresent->set_ask_format (
          BX_WITH_WX? "Enable this device?"
          : "Device is enabled: [%s] ");
      bx_options.atadevice[channel][slave].Otype->set_ask_format (
          BX_WITH_WX? "Type of ATA device:"
          : "Enter type of ATA device, disk or cdrom: [%s] ");
      bx_options.atadevice[channel][slave].Opath->set_ask_format (
          BX_WITH_WX? "Path or physical device name:"
          : "Enter new filename: [%s] ");
      bx_options.atadevice[channel][slave].Ocylinders->set_ask_format (
          BX_WITH_WX? "Cylinders:"
          : "Enter number of cylinders: [%d] ");
      bx_options.atadevice[channel][slave].Oheads->set_ask_format (
          BX_WITH_WX? "Heads:"
          : "Enter number of heads: [%d] ");
      bx_options.atadevice[channel][slave].Ospt->set_ask_format (
          BX_WITH_WX? "Sectors per track:"
          : "Enter number of sectors per track: [%d] ");
      bx_options.atadevice[channel][slave].Ostatus->set_ask_format (
          BX_WITH_WX? "Inserted?"
          : "Is the device inserted or ejected? [%s] ");
      bx_options.atadevice[channel][slave].Omodel->set_ask_format (
          BX_WITH_WX? "Model name:"
          : "Enter new model name: [%s]");
      bx_options.atadevice[channel][slave].Otranslation->set_ask_format (
          BX_WITH_WX? "Translation type:"
          : "Enter translation type: [%s]");
      bx_options.atadevice[channel][slave].Obiosdetect->set_ask_format (
          BX_WITH_WX? "BIOS Detection:"
          : "Enter bios detection type: [%s]");

#if !BX_WITH_WX
      bx_options.atadevice[channel][slave].Opresent->set_format ("enabled: %s");
      bx_options.atadevice[channel][slave].Otype->set_format (", %s");
      bx_options.atadevice[channel][slave].Opath->set_format (" on '%s'");
      bx_options.atadevice[channel][slave].Ocylinders->set_format (", %d cylinders");
      bx_options.atadevice[channel][slave].Oheads->set_format (", %d heads");
      bx_options.atadevice[channel][slave].Ospt->set_format (", %d sectors/track");
      bx_options.atadevice[channel][slave].Ostatus->set_format (", %s");
      bx_options.atadevice[channel][slave].Omodel->set_format (", model '%s'");
      bx_options.atadevice[channel][slave].Otranslation->set_format (", translation '%s'");
      bx_options.atadevice[channel][slave].Obiosdetect->set_format (", biosdetect '%s'");
#endif

      bx_options.atadevice[channel][slave].Otype->set_handler (bx_param_handler);
      
      bx_options.atadevice[channel][slave].Ostatus->set_handler (bx_param_handler);
      bx_options.atadevice[channel][slave].Opath->set_handler (bx_param_string_handler);
      }
    }

  bx_options.OnewHardDriveSupport = new bx_param_bool_c (BXP_NEWHARDDRIVESUPPORT,
      "New hard drive support",
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

  bx_options.OfloppySigCheck = new bx_param_bool_c (BXP_FLOPPYSIGCHECK,
      "Skip Floppy Boot Signature Check",
      "Skips check for the 0xaa55 signature on floppy boot device.",
      0);

  // disk menu
  bx_param_c *disk_menu_init_list[] = {
    SIM->get_param (BXP_FLOPPYA),
    SIM->get_param (BXP_FLOPPYB),
    //SIM->get_param (BXP_DISKC),
    //SIM->get_param (BXP_DISKD),
    //SIM->get_param (BXP_CDROMD),
    SIM->get_param (BXP_ATA0),
    SIM->get_param (BXP_ATA0_MASTER),
    SIM->get_param (BXP_ATA0_SLAVE),
#if BX_MAX_ATA_CHANNEL>1
    SIM->get_param (BXP_ATA1),
    SIM->get_param (BXP_ATA1_MASTER),
    SIM->get_param (BXP_ATA1_SLAVE),
#endif
#if BX_MAX_ATA_CHANNEL>2
    SIM->get_param (BXP_ATA2),
    SIM->get_param (BXP_ATA2_MASTER),
    SIM->get_param (BXP_ATA2_SLAVE),
#endif
#if BX_MAX_ATA_CHANNEL>3
    SIM->get_param (BXP_ATA3),
    SIM->get_param (BXP_ATA3_MASTER),
    SIM->get_param (BXP_ATA3_SLAVE),
#endif
    SIM->get_param (BXP_NEWHARDDRIVESUPPORT),
    SIM->get_param (BXP_BOOTDRIVE),
    SIM->get_param (BXP_FLOPPYSIGCHECK),
    NULL
  };
  menu = new bx_list_c (BXP_MENU_DISK, "Bochs Disk Options", "diskmenu", disk_menu_init_list);
  menu->get_options ()->set (menu->SHOW_PARENT);

  // memory options menu
  bx_options.memory.Osize = new bx_param_num_c (BXP_MEM_SIZE,
      "megs",
      "Amount of RAM in megabytes",
      1, BX_MAX_BIT32U,
      BX_DEFAULT_MEM_MEGS);
  bx_options.memory.Osize->set_format ("Memory size in megabytes: %d");
  bx_options.memory.Osize->set_ask_format ("Enter memory size (MB): [%d] ");

  // initialize serial and parallel port options
#define PAR_SER_INIT_LIST_MAX \
  ((BXP_PARAMS_PER_PARALLEL_PORT * BX_N_PARALLEL_PORTS) \
  + (BXP_PARAMS_PER_SERIAL_PORT * BX_N_SERIAL_PORTS))
  bx_param_c *par_ser_init_list[1+PAR_SER_INIT_LIST_MAX];
  bx_param_c **par_ser_ptr = &par_ser_init_list[0];

  // parallel ports
  for (i=0; i<BX_N_PARALLEL_PORTS; i++) {
        sprintf (name, "Enable parallel port #%d", i+1);
        bx_options.par[i].Oenabled = new bx_param_bool_c (
                BXP_PARPORTx_ENABLED(i+1), 
                strdup(name), 
                "",
                (i==0)? 1 : 0);  // only enable #1 by default
        sprintf (name, "Parallel port #%d output file", i+1);
        sprintf (descr, "Data written to parport#%d by the guest OS is written to this file", i+1);
        bx_options.par[i].Ooutfile = new bx_param_filename_c (
                BXP_PARPORTx_OUTFILE(i+1), 
                strdup(name), 
                strdup(descr),
                "", BX_PATHNAME_LEN);
        deplist = new bx_list_c (BXP_NULL, 1);
        deplist->add (bx_options.par[i].Ooutfile);
        bx_options.par[i].Oenabled->set_dependent_list (deplist);
        // add to menu
        *par_ser_ptr++ = bx_options.par[i].Oenabled;
        *par_ser_ptr++ = bx_options.par[i].Ooutfile;
  }

  // serial ports
  for (i=0; i<BX_N_SERIAL_PORTS; i++) {
        // options for COM port
        sprintf (name, "Enable serial port #%d (COM%d)", i+1, i+1);
        sprintf (descr, "Controls whether COM%d is installed or not", i+1);
        bx_options.com[i].Oenabled = new bx_param_bool_c (
                BXP_COMx_ENABLED(i+1),
                strdup(name), 
                strdup(descr), 
                (i==0)?1 : 0);  // only enable the first by default
        sprintf (name, "Pathname of the serial device for COM%d", i+1);
        bx_options.com[i].Odev = new bx_param_filename_c (
                BXP_COMx_PATH(i+1),
                strdup(name), 
                "",
                "", BX_PATHNAME_LEN);
        deplist = new bx_list_c (BXP_NULL, 1);
        deplist->add (bx_options.com[i].Odev);
        bx_options.com[i].Oenabled->set_dependent_list (deplist);
        // add to menu
        *par_ser_ptr++ = bx_options.com[i].Oenabled;
        *par_ser_ptr++ = bx_options.com[i].Odev;
  }
  // add final NULL at the end, and build the menu
  *par_ser_ptr = NULL;
  menu = new bx_list_c (BXP_MENU_SERIAL_PARALLEL,
          "Serial and Parallel Port Options",
          "serial_parallel_menu",
          par_ser_init_list);
  menu->get_options ()->set (menu->SHOW_PARENT);

  bx_options.rom.Opath = new bx_param_filename_c (BXP_ROM_PATH,
      "romimage",
      "Pathname of ROM image to load",
      "", BX_PATHNAME_LEN);
  bx_options.rom.Opath->set_format ("Name of ROM BIOS image: %s");
  bx_options.rom.Oaddress = new bx_param_num_c (BXP_ROM_ADDRESS,
      "romaddr",
      "The address at which the ROM image should be loaded",
      0, BX_MAX_BIT32U, 
      0xf0000);
  bx_options.rom.Oaddress->set_format ("ROM BIOS address: 0x%05x");
  bx_options.rom.Oaddress->set_base (16);

  bx_options.optrom[0].Opath = new bx_param_filename_c (BXP_OPTROM1_PATH,
      "optional romimage #1",
      "Pathname of optional ROM image #1 to load",
      "", BX_PATHNAME_LEN);
  bx_options.optrom[0].Opath->set_format ("Name of optional ROM image #1 : %s");
  bx_options.optrom[0].Oaddress = new bx_param_num_c (BXP_OPTROM1_ADDRESS,
      "optional romaddr #1",
      "The address at which the optional ROM image #1 should be loaded",
      0, BX_MAX_BIT32U, 
      0);
  bx_options.optrom[0].Oaddress->set_format ("optional ROM #1 address: 0x%05x");
  bx_options.optrom[0].Oaddress->set_base (16);

  bx_options.optrom[1].Opath = new bx_param_filename_c (BXP_OPTROM2_PATH,
      "optional romimage #2",
      "Pathname of optional ROM image #2 to load",
      "", BX_PATHNAME_LEN);
  bx_options.optrom[1].Opath->set_format ("Name of optional ROM image #2 : %s");
  bx_options.optrom[1].Oaddress = new bx_param_num_c (BXP_OPTROM2_ADDRESS,
      "optional romaddr #2",
      "The address at which the optional ROM image #2 should be loaded",
      0, BX_MAX_BIT32U, 
      0);
  bx_options.optrom[1].Oaddress->set_format ("optional ROM #2 address: 0x%05x");
  bx_options.optrom[1].Oaddress->set_base (16);

  bx_options.optrom[2].Opath = new bx_param_filename_c (BXP_OPTROM3_PATH,
      "optional romimage #3",
      "Pathname of optional ROM image #3 to load",
      "", BX_PATHNAME_LEN);
  bx_options.optrom[2].Opath->set_format ("Name of optional ROM image #3 : %s");
  bx_options.optrom[2].Oaddress = new bx_param_num_c (BXP_OPTROM3_ADDRESS,
      "optional romaddr #3",
      "The address at which the optional ROM image #3 should be loaded",
      0, BX_MAX_BIT32U, 
      0);
  bx_options.optrom[2].Oaddress->set_format ("optional ROM #3 address: 0x%05x");
  bx_options.optrom[2].Oaddress->set_base (16);

  bx_options.optrom[3].Opath = new bx_param_filename_c (BXP_OPTROM4_PATH,
      "optional romimage #4",
      "Pathname of optional ROM image #4 to load",
      "", BX_PATHNAME_LEN);
  bx_options.optrom[3].Opath->set_format ("Name of optional ROM image #4 : %s");
  bx_options.optrom[3].Oaddress = new bx_param_num_c (BXP_OPTROM4_ADDRESS,
      "optional romaddr #4",
      "The address at which the optional ROM image #4 should be loaded",
      0, BX_MAX_BIT32U, 
      0);
  bx_options.optrom[3].Oaddress->set_format ("optional ROM #4 address: 0x%05x");
  bx_options.optrom[3].Oaddress->set_base (16);

  bx_options.vgarom.Opath = new bx_param_filename_c (BXP_VGA_ROM_PATH,
      "vgaromimage",
      "Pathname of VGA ROM image to load",
      "", BX_PATHNAME_LEN);
  bx_options.vgarom.Opath->set_format ("Name of VGA BIOS image: %s");
  bx_param_c *memory_init_list[] = {
    bx_options.memory.Osize,
    bx_options.vgarom.Opath,
    bx_options.rom.Opath,
    bx_options.rom.Oaddress,
    bx_options.optrom[0].Opath,
    bx_options.optrom[0].Oaddress,
    bx_options.optrom[1].Opath,
    bx_options.optrom[1].Oaddress,
    bx_options.optrom[2].Opath,
    bx_options.optrom[2].Oaddress,
    bx_options.optrom[3].Opath,
    bx_options.optrom[3].Oaddress,
    NULL
  };
  menu = new bx_list_c (BXP_MENU_MEMORY, "Bochs Memory Options", "memmenu", memory_init_list);
  menu->get_options ()->set (menu->SHOW_PARENT);

  // interface
  bx_options.Ovga_update_interval = new bx_param_num_c (BXP_VGA_UPDATE_INTERVAL,
      "VGA Update Interval",
      "Number of microseconds between VGA updates",
      1, BX_MAX_BIT32U,
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
      1, BX_MAX_BIT32U,
      500000);
  bx_options.Orealtime_pit = new bx_param_bool_c (BXP_REALTIME_PIT,
      "Enable the realtime PIT",
      "Keeps bochs in sync with real time, but sacrifices reproducibility",
      0);
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
  static char *config_interface_list[] = {
    "textconfig",
#if BX_WITH_WX
    "wx",
#endif
    NULL
  };
  bx_options.Osel_config = new bx_param_enum_c (
    BXP_SEL_CONFIG_INTERFACE,
    "Configuration interface",
    "Select configuration interface",
    config_interface_list,
    0,
    0);
  bx_options.Osel_config->set_by_name (BX_DEFAULT_CONFIG_INTERFACE);
  bx_options.Osel_config->set_format ("Configuration interface: %s");
  bx_options.Osel_config->set_ask_format ("Choose which configuration interface to use: [%s] ");
  // this is a list of gui libraries that are known to be available at
  // compile time.  The one that is listed first will be the default,
  // which is used unless the user overrides it on the command line or
  // in a configuration file.
  static char *display_library_list[] = {
#if BX_WITH_X11
    "x",
#endif
#if BX_WITH_WIN32
    "win32",
#endif
#if BX_WITH_CARBON
    "carbon",
#endif
#if BX_WITH_BEOS
    "beos",
#endif
#if BX_WITH_MACOS
    "macos",
#endif
#if BX_WITH_AMIGAOS
    "amigaos",
#endif
#if BX_WITH_SDL
    "sdl",
#endif
#if BX_WITH_TERM
    "term",
#endif
#if BX_WITH_RFB
    "rfb",
#endif
#if BX_WITH_WX
    "wx",
#endif
#if BX_WITH_NOGUI
    "nogui",
#endif
    NULL
  };
  bx_options.Osel_displaylib = new bx_param_enum_c (BXP_SEL_DISPLAY_LIBRARY,
    "VGA Display Library",
    "Select VGA Display Library",
    display_library_list,
    0,
    0);
  bx_options.Osel_displaylib->set_by_name (BX_DEFAULT_DISPLAY_LIBRARY);
  bx_options.Osel_displaylib->set_format ("Display library: %s");
  bx_options.Osel_displaylib->set_ask_format ("Choose which library to use for the Bochs display: [%s] ");
  bx_param_c *interface_init_list[] = {
    bx_options.Osel_config,
    bx_options.Osel_displaylib,
    bx_options.Ovga_update_interval,
    bx_options.Omouse_enabled,
    bx_options.Oips,
    bx_options.Orealtime_pit,
    bx_options.Oprivate_colormap,
#if BX_WITH_AMIGAOS
    bx_options.Ofullscreen,
    bx_options.Oscreenmode,
#endif
    NULL
  };
  menu = new bx_list_c (BXP_MENU_INTERFACE, "Bochs Interface Menu", "intfmenu", interface_init_list);
  menu->get_options ()->set (menu->SHOW_PARENT);

  // NE2K options
  bx_options.ne2k.Opresent = new bx_param_bool_c (BXP_NE2K_PRESENT,
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
  bx_options.ne2k.Omacaddr->get_options ()->set (bx_options.ne2k.Omacaddr->RAW_BYTES);
  bx_options.ne2k.Omacaddr->set_separator (':');
  bx_options.ne2k.Oethmod = new bx_param_string_c (BXP_NE2K_ETHMOD,
      "Ethernet module",
      "to be written",
      "null", 16);
  bx_options.ne2k.Oethdev = new bx_param_string_c (BXP_NE2K_ETHDEV,
      "Ethernet device",
      "to be written",
      "xl0", BX_PATHNAME_LEN);
  bx_options.ne2k.Oscript = new bx_param_string_c (BXP_NE2K_SCRIPT,
      "Device configuration script",
      "to be written",
      "none", BX_PATHNAME_LEN);
  bx_options.ne2k.Oscript->set_ask_format ("Enter new script name, or 'none': [%s] ");
  bx_param_c *ne2k_init_list[] = {
    bx_options.ne2k.Opresent,
    bx_options.ne2k.Oioaddr,
    bx_options.ne2k.Oirq,
    bx_options.ne2k.Omacaddr,
    bx_options.ne2k.Oethmod,
    bx_options.ne2k.Oethdev,
    bx_options.ne2k.Oscript,
    NULL
  };
  menu = new bx_list_c (BXP_NE2K, "NE2K Configuration", "", ne2k_init_list);
  menu->get_options ()->set (menu->SHOW_PARENT);
  bx_options.ne2k.Opresent->set_handler (bx_param_handler);
  bx_options.ne2k.Opresent->set (0);

  // SB16 options
  bx_options.sb16.Opresent = new bx_param_bool_c (BXP_SB16_PRESENT,
      "SB16 is present",
      "to be written",
      0);
  bx_options.sb16.Omidifile = new bx_param_filename_c (BXP_SB16_MIDIFILE,
      "Midi file",
      "to be written",
      "", BX_PATHNAME_LEN);
  bx_options.sb16.Owavefile = new bx_param_filename_c (BXP_SB16_WAVEFILE,
      "Wave file",
      "to be written",
      "", BX_PATHNAME_LEN);
  bx_options.sb16.Ologfile = new bx_param_filename_c (BXP_SB16_LOGFILE,
      "Log file",
      "to be written",
      "", BX_PATHNAME_LEN);
  bx_options.sb16.Omidimode = new bx_param_num_c (BXP_SB16_MIDIMODE,
      "Midi mode",
      "to be written",
      0, BX_MAX_BIT32U,
      0);
  bx_options.sb16.Owavemode = new bx_param_num_c (BXP_SB16_WAVEMODE,
      "Wave mode",
      "to be written",
      0, BX_MAX_BIT32U,
      0);
  bx_options.sb16.Ologlevel = new bx_param_num_c (BXP_SB16_LOGLEVEL,
      "Log mode",
      "to be written",
      0, BX_MAX_BIT32U,
      0);
  bx_options.sb16.Odmatimer = new bx_param_num_c (BXP_SB16_DMATIMER,
      "DMA timer",
      "to be written",
      0, BX_MAX_BIT32U,
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
  menu->get_options ()->set (menu->SHOW_PARENT);
  // sb16_dependent_list is a null-terminated list including all the
  // sb16 fields except for the "present" field.  These will all be enabled/
  // disabled according to the value of the present field.
  bx_param_c **sb16_dependent_list = &sb16_init_list[1];
  bx_options.sb16.Opresent->set_dependent_list (
      new bx_list_c (BXP_NULL, "", "", sb16_dependent_list));

  bx_options.log.Ofilename = new bx_param_filename_c (BXP_LOG_FILENAME,
      "Log filename",
      "Pathname of bochs log file",
      "-", BX_PATHNAME_LEN);
  bx_options.log.Ofilename->set_ask_format ("Enter log filename: [%s] ");

  bx_options.log.Oprefix = new bx_param_string_c (BXP_LOG_PREFIX,
      "Log output prefix",
      "Prefix prepended to log output",
      "%t%e%d", BX_PATHNAME_LEN);
  bx_options.log.Oprefix->set_ask_format ("Enter log prefix: [%s] ");

  // loader
  bx_options.load32bitOSImage.OwhichOS = new bx_param_enum_c (BXP_LOAD32BITOS_WHICH,
      "Which operating system?",
      "Which OS to boot",
      loader_os_names,
      Load32bitOSNone,
      Load32bitOSNone);
  bx_options.load32bitOSImage.Opath = new bx_param_filename_c (BXP_LOAD32BITOS_PATH,
      "Pathname of OS to load",
      NULL,
      "", BX_PATHNAME_LEN);
  bx_options.load32bitOSImage.Oiolog = new bx_param_filename_c (BXP_LOAD32BITOS_IOLOG,
      "Pathname of I/O log file",
      NULL,
      "", BX_PATHNAME_LEN);
  bx_options.load32bitOSImage.Oinitrd = new bx_param_filename_c (BXP_LOAD32BITOS_INITRD,
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
  menu->get_options ()->set (menu->SERIES_ASK);
  bx_options.load32bitOSImage.OwhichOS->set_handler (bx_param_handler);
  bx_options.load32bitOSImage.OwhichOS->set (Load32bitOSNone);

  // other
  bx_options.Okeyboard_serial_delay = new bx_param_num_c (BXP_KBD_SERIAL_DELAY,
      "Keyboard serial delay",
      "Approximate time in microseconds that it takes one character to be transfered from the keyboard to controller over the serial path.",
      1, BX_MAX_BIT32U,
      20000);
  bx_options.Okeyboard_paste_delay = new bx_param_num_c (BXP_KBD_PASTE_DELAY,
      "Keyboard paste delay",
      "Approximate time in microseconds between attemps to paste characters to the keyboard controller.",
      1000, BX_MAX_BIT32U,
      100000);
  bx_options.Okeyboard_paste_delay->set_handler (bx_param_handler);
  bx_options.Ofloppy_command_delay = new bx_param_num_c (BXP_FLOPPY_CMD_DELAY,
      "Floppy command delay",
      "Time in microseconds to wait before completing some floppy commands such as read/write/seek/etc, which normally have a delay associated.  This used to be hardwired to 50,000 before.",
      1, BX_MAX_BIT32U,
      50000);
  bx_options.Oi440FXSupport = new bx_param_bool_c (BXP_I440FX_SUPPORT,
      "PCI i440FX Support",
      "Controls whether to emulate PCI I440FX",
      0);
  bx_options.cmos.OcmosImage = new bx_param_bool_c (BXP_CMOS_IMAGE,
      "Use a CMOS image",
      NULL,
      0);
  bx_options.cmos.Opath = new bx_param_filename_c (BXP_CMOS_PATH,
      "Pathname of CMOS image",
      NULL,
      "", BX_PATHNAME_LEN);
  deplist = new bx_list_c (BXP_NULL, 1);
  deplist->add (bx_options.cmos.Opath);
  bx_options.cmos.OcmosImage->set_dependent_list (deplist);

  bx_options.cmos.Otime0 = new bx_param_num_c (BXP_CMOS_TIME0,
      "Initial CMOS time for Bochs",
      "Start time for Bochs CMOS clock, used if you really want two runs to be identical (cosimulation)",
      0, BX_MAX_BIT32U,
      0);

  // Keyboard mapping
  bx_options.keyboard.OuseMapping = new bx_param_bool_c(BXP_KEYBOARD_USEMAPPING,
      "Use keyboard mapping",
      NULL,
      0);
  bx_options.keyboard.Okeymap = new bx_param_filename_c (BXP_KEYBOARD_MAP,
      "Keymap filename",
      NULL,
      "", BX_PATHNAME_LEN);
  deplist = new bx_list_c (BXP_NULL, 1);
  deplist->add (bx_options.keyboard.Okeymap);
  bx_options.keyboard.OuseMapping->set_dependent_list (deplist);

 // Keyboard type
  bx_options.Okeyboard_type = new bx_param_enum_c (BXP_KBD_TYPE,
      "Keyboard type",
      "Keyboard type",
      keyboard_type_names,
      BX_KBD_MF_TYPE,
      BX_KBD_XT_TYPE);
  bx_options.Okeyboard_type->set_format ("Keyboard type: %s");
  bx_options.Okeyboard_type->set_ask_format ("Enter keyboard type: [%s] ");

  // Userbutton shortcut
  bx_options.Ouser_shortcut = new bx_param_string_c (BXP_USER_SHORTCUT,
      "Userbutton shortcut",
      "Userbutton shortcut",
      "none", 16);

  // GDB stub
  bx_options.gdbstub.port = 1234;
  bx_options.gdbstub.text_base = 0;
  bx_options.gdbstub.data_base = 0;
  bx_options.gdbstub.bss_base = 0;

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
      bx_options.Ouser_shortcut,
      NULL
  };
  menu = new bx_list_c (BXP_MENU_MISC, "Configure Everything Else", "", other_init_list);
  menu->get_options ()->set (menu->SHOW_PARENT);
}

void bx_reset_options ()
{
  // drives
  bx_options.floppya.Opath->reset();
  bx_options.floppya.Odevtype->reset();
  bx_options.floppya.Otype->reset();
  bx_options.floppya.Ostatus->reset();
  bx_options.floppyb.Opath->reset();
  bx_options.floppyb.Odevtype->reset();
  bx_options.floppyb.Otype->reset();
  bx_options.floppyb.Ostatus->reset();

  for (Bit8u channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    bx_options.ata[channel].Opresent->reset();
    bx_options.ata[channel].Oioaddr1->reset();
    bx_options.ata[channel].Oioaddr2->reset();
    bx_options.ata[channel].Oirq->reset();

    for (Bit8u slave=0; slave<2; slave++) {
      bx_options.atadevice[channel][slave].Opresent->reset();
      bx_options.atadevice[channel][slave].Otype->reset();
      bx_options.atadevice[channel][slave].Opath->reset();
      bx_options.atadevice[channel][slave].Ocylinders->reset();
      bx_options.atadevice[channel][slave].Oheads->reset();
      bx_options.atadevice[channel][slave].Ospt->reset();
      bx_options.atadevice[channel][slave].Ostatus->reset();
      bx_options.atadevice[channel][slave].Omodel->reset();
      bx_options.atadevice[channel][slave].Obiosdetect->reset();
      bx_options.atadevice[channel][slave].Otranslation->reset();
      }
    }
  bx_options.OnewHardDriveSupport->reset();

  // boot & memory
  bx_options.Obootdrive->reset();
  bx_options.OfloppySigCheck->reset();
  bx_options.memory.Osize->reset();

  // standard ports
  bx_options.com[0].Oenabled->reset();
  bx_options.com[0].Odev->reset();
  bx_options.par[0].Oenabled->reset();
  bx_options.par[0].Ooutfile->reset();

  // rom images
  bx_options.rom.Opath->reset();
  bx_options.rom.Oaddress->reset();
  bx_options.optrom[0].Opath->reset();
  bx_options.optrom[0].Oaddress->reset();
  bx_options.optrom[1].Opath->reset();
  bx_options.optrom[1].Oaddress->reset();
  bx_options.optrom[2].Opath->reset();
  bx_options.optrom[2].Oaddress->reset();
  bx_options.optrom[3].Opath->reset();
  bx_options.optrom[3].Oaddress->reset();
  bx_options.vgarom.Opath->reset();

  // interface
  bx_options.Ovga_update_interval->reset();
  bx_options.Omouse_enabled->reset();
  bx_options.Oips->reset();
  bx_options.Orealtime_pit->reset();
  bx_options.Oprivate_colormap->reset();
#if BX_WITH_AMIGAOS
  bx_options.Ofullscreen->reset();
  bx_options.Oscreenmode->reset();
#endif

  // ne2k
  bx_options.ne2k.Opresent->reset();
  bx_options.ne2k.Oioaddr->reset();
  bx_options.ne2k.Oirq->reset();
  bx_options.ne2k.Omacaddr->reset();
  bx_options.ne2k.Oethmod->reset();
  bx_options.ne2k.Oethdev->reset();
  bx_options.ne2k.Oscript->reset();

  // SB16
  bx_options.sb16.Opresent->reset();
  bx_options.sb16.Omidifile->reset();
  bx_options.sb16.Owavefile->reset();
  bx_options.sb16.Ologfile->reset();
  bx_options.sb16.Omidimode->reset();
  bx_options.sb16.Owavemode->reset();
  bx_options.sb16.Ologlevel->reset();
  bx_options.sb16.Odmatimer->reset();

  // logfile
  bx_options.log.Ofilename->reset();
  bx_options.log.Oprefix->reset();

  // loader
  bx_options.load32bitOSImage.OwhichOS->reset();
  bx_options.load32bitOSImage.Opath->reset();
  bx_options.load32bitOSImage.Oiolog->reset();
  bx_options.load32bitOSImage.Oinitrd->reset();

  // keyboard
  bx_options.Okeyboard_serial_delay->reset();
  bx_options.Okeyboard_paste_delay->reset();
  bx_options.keyboard.OuseMapping->reset();
  bx_options.keyboard.Okeymap->reset();
  bx_options.Okeyboard_type->reset();
  bx_options.Ouser_shortcut->reset();

  // other
  bx_options.Ofloppy_command_delay->reset();
  bx_options.Oi440FXSupport->reset();
  bx_options.cmos.OcmosImage->reset();
  bx_options.cmos.Opath->reset();
  bx_options.cmos.Otime0->reset();
}

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

int bxmain () {
  bx_init_siminterface ();   // create the SIM object
  static jmp_buf context;
  if (setjmp (context) == 0) {
    SIM->set_quit_context (&context);
    if (bx_init_main (bx_startup_flags.argc, bx_startup_flags.argv) < 0) 
      return 0;
    // read a param to decide which config interface to start.
    // If one exists, start it.  If not, just begin.
    bx_param_enum_c *ci_param = SIM->get_param_enum (BXP_SEL_CONFIG_INTERFACE);
    char *ci_name = ci_param->get_choice (ci_param->get ());
    if (!strcmp(ci_name, "textconfig")) {
      init_text_config_interface ();
    }
#if BX_WITH_WX
    else if (!strcmp(ci_name, "wx")) {
      PLUG_load_plugin(wx, PLUGTYPE_CORE);
    }
#endif
    else {
      BX_PANIC (("unsupported configuration interface '%s'", ci_name));
    }
    SIM->configuration_interface (ci_name, CI_START);
    // user quit the config interface, so just quit
  } else {
    // quit via longjmp
  }
  SIM->set_quit_context (NULL);
  return 0;
}

#if !defined(__WXMSW__)
// normal main function, presently in for all cases except for
// wxWindows under win32.
int main (int argc, char *argv[])
{
  bx_startup_flags.argc = argc;
  bx_startup_flags.argv = argv;
  return bxmain ();
}
#else

// special WinMain function for wxWindows under win32

#define MAX_ARGLEN 128

// win32 applications get the whole command line in one long string.
// This function is used to split up the string into argc and argv,
// so that the command line can be used on win32 just like on every
// other platform.
int split_string_into_argv (
  char *string,
  int *argc_out,
  char **argv,
  int max_argv) 
{
  char *buf0 = new char[strlen(string)+1];
  strcpy (buf0, string);
  char *buf = buf0;
  int in_double_quote = 0, in_single_quote = 0;
  for (int i=0; i<max_argv; i++) 
    argv[i] = NULL;
  argv[0] = new char[6];
  strcpy (argv[0], "bochs");
  int argc = 1;
  argv[argc] = new char[MAX_ARGLEN];
  char *outp = &argv[argc][0];
  // trim leading and trailing spaces
  while (*buf==' ') buf++;
  char *p;
  char *last_nonspace = buf;
  for (p=buf; *p; p++) {
    if (*p!=' ') last_nonspace = p;
  }
  if (last_nonspace != buf) *(last_nonspace+1) = 0;
  p = buf;
  bx_bool done = false;
  while (!done) {
    //fprintf (stderr, "parsing '%c' with singlequote=%d, dblquote=%d\n", *p, in_single_quote, in_double_quote);
    switch (*p) {
    case '\0':
      done = true;
      // fall through into behavior for space
    case ' ':
      if (in_double_quote || in_single_quote) 
        goto do_default;
      *outp = 0;
      //fprintf (stderr, "completed arg %d = '%s'\n", argc, argv[argc]);
      argc++;
      if (argc >= max_argv) {
        fprintf (stderr, "too many arguments. Increase MAX_ARGUMENTS\n");
        return -1;
      }
      argv[argc] = new char[MAX_ARGLEN];
      outp = &argv[argc][0];
      while (*p==' ') p++;
      break;
    case '"':
      if (in_single_quote) goto do_default;
      in_double_quote = !in_double_quote;
      p++;
      break;
    case '\'':
      if (in_double_quote) goto do_default;
      in_single_quote = !in_single_quote;
      p++;
      break;
    do_default:
    default:
      if (outp-&argv[argc][0] >= MAX_ARGLEN) {
        //fprintf (stderr, "command line arg %d exceeded max size %d\n", argc, MAX_ARGLEN);
        return -1;
      }
      *(outp++) = *(p++);
    }
  }
  if (in_single_quote) {
    fprintf (stderr, "end of string with mismatched single quote (')\n");
    return -1;
  }
  if (in_double_quote) {
    fprintf (stderr, "end of string with mismatched double quote (\")\n");
    return -1;
  }
  *argc_out = argc;
  return 0;
}

// The RedirectIOToConsole() function is copied from an article called "Adding
// Console I/O to a Win32 GUI App" in Windows Developer Journal, December 1997.
#define MAX_CONSOLE_LINES 30
void RedirectIOToConsole ()
{
  int hConHandle;
  long lStdHandle;
  CONSOLE_SCREEN_BUFFER_INFO coninfo;
  FILE *fp;
  // allocate a console for this app
  AllocConsole();
  // set the screen buffer to be big enough to let us scroll text
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
  &coninfo);
  coninfo.dwSize.Y = MAX_CONSOLE_LINES;
  SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
  // redirect unbuffered STDOUT to the console
  lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen( hConHandle, "w" );
  *stdout = *fp;
  setvbuf( stdout, NULL, _IONBF, 0 );
  // redirect unbuffered STDIN to the console
  lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen( hConHandle, "r" );
  *stdin = *fp;
  setvbuf( stdin, NULL, _IONBF, 0 );
  // redirect unbuffered STDERR to the console
  lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen( hConHandle, "w" );
  *stderr = *fp;
  setvbuf( stderr, NULL, _IONBF, 0 );
}

int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR m_lpCmdLine, int nCmdShow)
{
  bx_startup_flags.hInstance = hInstance;
  bx_startup_flags.hPrevInstance = hPrevInstance;
  bx_startup_flags.m_lpCmdLine = m_lpCmdLine;
  bx_startup_flags.nCmdShow = nCmdShow;
  RedirectIOToConsole ();
  int max_argv = 20;
  bx_startup_flags.argv = (char**) malloc (max_argv * sizeof (char*));
  split_string_into_argv (m_lpCmdLine, &bx_startup_flags.argc, bx_startup_flags.argv, max_argv);
  int status = bxmain ();
  fprintf (stderr, "\nBochs is exiting. Press ENTER when you're ready to close this window.\n");
  char buf[16];
  fgets (buf, sizeof(buf), stdin);
  return status;
}
#endif

void
print_usage ()
{
  fprintf(stderr, 
    "Usage: bochs [flags] [bochsrc options]\n\n"
    "  -f configfile    specify configuration file\n"
    "  -q               quick start (skip configuration interface)\n"
    "  --help           display this help and exit\n\n"
    "For information on Bochs configuration file arguments, see the\n"
#if (!defined(WIN32)) && !BX_WITH_MACOS
    "bochsrc section in the user documentation or the man page of bochsrc.\n");
#else
    "bochsrc section in the user documentation.\n");
#endif
}

int
bx_init_main (int argc, char *argv[])
{
  // To deal with initialization order problems inherent in C++, use the macros
  // SAFE_GET_IOFUNC and SAFE_GET_GENLOG to retrieve "io" and "genlog" in all
  // constructors or functions called by constructors.  The macros test for
  // NULL and create the object if necessary, then return it.  Ensure that io
  // and genlog get created, by making one reference to each macro right here.
  // All other code can reference io and genlog directly.  Because these
  // objects are required for logging, and logging is so fundamental to
  // knowing what the program is doing, they are never free()d.
  SAFE_GET_IOFUNC();  // never freed
  SAFE_GET_GENLOG();  // never freed

  // initalization must be done early because some destructors expect
  // the bx_options to exist by the time they are called.
  bx_init_bx_dbg ();
  bx_init_options ();

  bx_print_header ();

  SIM->get_param_enum(BXP_BOCHS_START)->set (BX_RUN_START);

  // interpret the args that start with -, like -q, -f, etc.
  int arg = 1;
  while (arg < argc) {
    // parse next arg
    if (!strcmp ("--help", argv[arg]) || !strncmp ("-h", argv[arg], 2)) {
      print_usage();
      SIM->quit_sim (0);
    }
    else if (!strcmp ("-q", argv[arg])) {
      SIM->get_param_enum(BXP_BOCHS_START)->set (BX_QUICK_START);
    }
    else if (!strcmp ("-f", argv[arg])) {
      if (++arg >= argc) BX_PANIC(("-f must be followed by a filename"));
      else bochsrc_filename = argv[arg];
    }
    else if (!strcmp ("-qf", argv[arg])) {
      SIM->get_param_enum(BXP_BOCHS_START)->set (BX_QUICK_START);
      if (++arg >= argc) BX_PANIC(("-qf must be followed by a filename"));
      else bochsrc_filename = argv[arg];
    }
#if BX_WITH_CARBON
    else if (!strncmp ("-psn", argv[arg], 4)) {
      // "-psn" is passed if we are launched by double-clicking
      // ugly hack.  I don't know how to open a window to print messages in,
      // so put them in /tmp/early-bochs-out.txt.  Sorry. -bbd
      io->init_log("/tmp/early-bochs-out.txt");
      BX_INFO (("I was launched by double clicking.  Fixing home directory."));
      arg = argc; // ignore all other args.
      setupWorkingDirectory (argv[0]);
      // there is no stdin/stdout so disable the text-based config interface.
      SIM->get_param_enum(BXP_BOCHS_START)->set (BX_QUICK_START);
      char cwd[MAXPATHLEN];
      getwd (cwd);
      BX_INFO (("Now my working directory is %s", cwd));
      // if it was started from command line, there could be some args still.
      for (int a=0; a<argc; a++) {
        BX_INFO (("argument %d is %s", a, argv[a]));
      }
    }
#endif
    else if (argv[arg][0] == '-') {
      print_usage();
      BX_PANIC (("command line arg '%s' was not understood", argv[arg]));
    }
    else {
      // the arg did not start with -, so stop interpreting flags
      break;
    }
    arg++;
  }
#if BX_PLUGINS
  // set a default plugin path, in case the user did not specify one
#if BX_WITH_CARBON
  // if there is no stdin, then we must create our own LTDL_LIBRARY_PATH.
  // also if there is no LTDL_LIBRARY_PATH, but we have a bundle since we're here
  // This is here so that it is available whenever --with-carbon is defined but
  // the above code might be skipped, as in --with-sdl --with-carbon
  if(!isatty(STDIN_FILENO) || !getenv("LTDL_LIBRARY_PATH"))
  {
    CFBundleRef mainBundle;
    CFURLRef libDir;
    char libDirPath[4096];
    if(!isatty(STDIN_FILENO))
    {
      // there is no stdin/stdout so disable the text-based config interface.
      SIM->get_param_enum(BXP_BOCHS_START)->set (BX_QUICK_START);
    }
    BX_INFO (("fixing default lib location ..."));
    // locate the lib directory within the application bundle.
    // our libs have been placed in bochs.app/Contents/(current platform aka MacOS)/lib
    // This isn't quite right, but they are platform specific and we haven't put
    // our plugins into true frameworks and bundles either
    mainBundle = CFBundleGetMainBundle();
    BX_ASSERT(mainBundle != NULL);
    libDir = CFBundleCopyAuxiliaryExecutableURL( mainBundle, CFSTR("lib"));
    BX_ASSERT(libDir != NULL);
    // translate this to a unix style full path
    if(!CFURLGetFileSystemRepresentation(libDir, true, libDirPath, 4096))
    {
      BX_PANIC(("Unable to work out ltdl library path within bochs bundle! (Most likely path too long!)"));
      return -1;
    }
    setenv("LTDL_LIBRARY_PATH", libDirPath, 1);
    BX_INFO (("now my LTDL_LIBRARY_PATH is %s", libDirPath));
    CFRelease(libDir);
  }
#elif BX_HAVE_GETENV && BX_HAVE_SETENV
  if (getenv("LTDL_LIBRARY_PATH") != NULL) {
    BX_INFO (("LTDL_LIBRARY_PATH is set to '%s'", getenv("LTDL_LIBRARY_PATH")));
  } else {
    BX_INFO (("LTDL_LIBRARY_PATH not set. using compile time default '%s'", 
	  BX_PLUGIN_PATH));
    setenv("LTDL_LIBRARY_PATH", BX_PLUGIN_PATH, 1);
  }
#else
  // we don't have getenv or setenv.  Do nothing.
#endif
#endif  /* if BX_PLUGINS */

#if !BX_USE_CONFIG_INTERFACE
  // this allows people to get quick start behavior by default
  SIM->get_param_enum(BXP_BOCHS_START)->set (BX_QUICK_START);
#endif

  int norcfile = 1;
  /* always parse configuration file and command line arguments */
  if (bochsrc_filename == NULL) bochsrc_filename = bx_find_bochsrc ();
  if (bochsrc_filename)
    norcfile = bx_read_configuration (bochsrc_filename);

  if (norcfile) {
    // No configuration was loaded, so the current settings are unusable.
    // Switch off quick start so that we will drop into the configuration
    // interface.
    if (SIM->get_param_enum(BXP_BOCHS_START)->get() == BX_QUICK_START) {
      BX_ERROR (("Switching off quick start, because no configuration file was found."));
    }
    SIM->get_param_enum(BXP_BOCHS_START)->set (BX_LOAD_START);
  }

  // parse the rest of the command line.  This is done after reading the
  // configuration file so that the command line arguments can override
  // the settings from the file.
  if (bx_parse_cmdline (arg, argc, argv)) {
    BX_PANIC(("There were errors while parsing the command line"));
    return -1;
  }
  // initialize plugin system. This must happen before we attempt to
  // load any modules.
  plugin_startup();
  return 0;
}

bx_bool load_and_init_display_lib () {
  if (bx_gui != NULL) {
    // bx_gui has already been filled in.  This happens when you start
    // the simulation for the second time.
    // Also, if you load wxWindows as the configuration interface.  Its
    // plugin_init will install wxWindows as the bx_gui.
    return true;
  }
  BX_ASSERT (bx_gui == NULL);
  bx_param_enum_c *gui_param = SIM->get_param_enum(BXP_SEL_DISPLAY_LIBRARY);
  char *gui_name = gui_param->get_choice (gui_param->get ());
  if (!strcmp (gui_name, "wx")) {
    // they must not have used wx as the configuration interface, or bx_gui
    // would already be initialized.  Sorry, it doesn't work that way.
    BX_ERROR (("wxWindows was not used as the configuration interface, so it cannot be used as the display library"));
    // choose another, hopefully different!
    gui_param->set (0);
    gui_name = gui_param->get_choice (gui_param->get ());
    if (!strcmp (gui_name, "wx")) {
      BX_PANIC (("no alternative display libraries are available"));
      return false;
    }
    BX_ERROR (("changing display library to '%s' instead", gui_name));
  }
#if BX_WITH_AMIGAOS
  if (!strcmp (gui_name, "amigaos")) 
    PLUG_load_plugin (amigaos, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_BEOS
  if (!strcmp (gui_name, "beos")) 
    PLUG_load_plugin (beos, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_CARBON
  if (!strcmp (gui_name, "carbon")) 
    PLUG_load_plugin (carbon, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_MACOS
  if (!strcmp (gui_name, "macintosh")) 
    PLUG_load_plugin (macintosh, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_NOGUI
  if (!strcmp (gui_name, "nogui")) 
    PLUG_load_plugin (nogui, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_RFB
  if (!strcmp (gui_name, "rfb")) 
    PLUG_load_plugin (rfb, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_SDL
  if (!strcmp (gui_name, "sdl")) 
    PLUG_load_plugin (sdl, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_TERM
  if (!strcmp (gui_name, "term")) 
    PLUG_load_plugin (term, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_WIN32
  if (!strcmp (gui_name, "win32")) 
    PLUG_load_plugin (win32, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_X11
  if (!strcmp (gui_name, "x")) 
    PLUG_load_plugin (x, PLUGTYPE_OPTIONAL);
#endif

#if BX_GUI_SIGHANDLER
  // set the flag for guis requiring a GUI sighandler.
  // useful when guis are compiled as plugins
  // only term for now
  if (!strcmp (gui_name, "term")) {
    bx_gui_sighandler = 1;
    }
#endif

  BX_ASSERT (bx_gui != NULL);
  return true;
}

int
bx_begin_simulation (int argc, char *argv[])
{
  // deal with gui selection
  if (!load_and_init_display_lib ()) {
    BX_PANIC (("no gui module was loaded"));
    return 0;
  }
#if BX_GDBSTUB
  // If using gdbstub, it will take control and call
  // bx_init_hardware() and cpu_loop()
  bx_gdbstub_init (argc, argv);
#elif BX_DEBUGGER
  // If using the debugger, it will take control and call
  // bx_init_hardware() and cpu_loop()
  bx_dbg_main(argc, argv);
#else
#if BX_PLUGINS
#ifdef __GNUC__
#warning bx_load_plugins doesnt do much anymore and should maybe be removed
#endif
  bx_load_plugins ();
#endif

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
        // for one processor, the only reason for cpu_loop to return is
        // that kill_bochs_request was set by the GUI interface.
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
          if (BX_CPU(0)->kill_bochs_request) 
            break;
      if (processor == 0) 
            BX_TICKN(quantum);
    }
  }
#endif
  BX_INFO (("cpu loop quit, shutting down simulator"));
  bx_atexit ();
  return(0);
}


int
bx_read_configuration (char *rcfile)
{
  // parse rcfile first, then parse arguments in order.
  BX_INFO (("reading configuration from %s", rcfile));
  if (parse_bochsrc(rcfile) < 0) {
    BX_PANIC (("reading from %s failed", rcfile));
    return -1;
  }
  // update log actions
  for (int level=0; level<N_LOGLEV; level++) {
    int action = SIM->get_default_log_action (level);
    io->set_log_action (level, action);
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
  // update log actions
  for (int level=0; level<N_LOGLEV; level++) {
    int action = SIM->get_default_log_action (level);
    io->set_log_action (level, action);
  }
  return 0;
}

  int
bx_init_hardware()
{
  // all configuration has been read, now initialize everything.

  if (SIM->get_param_enum(BXP_BOCHS_START)->get ()==BX_QUICK_START) {
    for (int level=0; level<N_LOGLEV; level++) {
      int action = SIM->get_default_log_action (level);
#if !BX_USE_CONFIG_INTERFACE
      if (action == ACT_ASK) action = ACT_FATAL;
#endif
      io->set_log_action (level, action);
    }
  }

  bx_pc_system.init_ips(bx_options.Oips->get ());

  if(bx_options.log.Ofilename->getptr()[0]!='-') {
    BX_INFO (("using log file %s", bx_options.log.Ofilename->getptr ()));
    io->init_log(bx_options.log.Ofilename->getptr ());
  }

  io->set_log_prefix(bx_options.log.Oprefix->getptr());

  // set up memory and CPU objects
#if BX_SUPPORT_APIC
  bx_generic_apic_c::reset_all_ids ();
#endif

  // Check if there is a romimage
  if (strcmp(bx_options.rom.Opath->getptr (),"") == 0) {
    BX_ERROR(("No romimage to load. Is your bochsrc file loaded/valid ?"));
  }

#if BX_SMP_PROCESSORS==1
  BX_MEM(0)->init_memory(bx_options.memory.Osize->get () * 1024*1024);

  // First load the optional ROM images
  if (bx_options.optrom[0].Opath->getptr () > 0)
    BX_MEM(0)->load_ROM(bx_options.optrom[0].Opath->getptr (), bx_options.optrom[0].Oaddress->get ());
  if (bx_options.optrom[1].Opath->getptr () > 0)
    BX_MEM(0)->load_ROM(bx_options.optrom[1].Opath->getptr (), bx_options.optrom[1].Oaddress->get ());
  if (bx_options.optrom[2].Opath->getptr () > 0)
    BX_MEM(0)->load_ROM(bx_options.optrom[2].Opath->getptr (), bx_options.optrom[2].Oaddress->get ());
  if (bx_options.optrom[3].Opath->getptr () > 0)
    BX_MEM(0)->load_ROM(bx_options.optrom[3].Opath->getptr (), bx_options.optrom[3].Oaddress->get ());

  // Then Load the BIOS and VGABIOS
  BX_MEM(0)->load_ROM(bx_options.rom.Opath->getptr (), bx_options.rom.Oaddress->get ());
  BX_MEM(0)->load_ROM(bx_options.vgarom.Opath->getptr (), 0xc0000);

  BX_CPU(0)->init (BX_MEM(0));
#if BX_SUPPORT_APIC
  BX_CPU(0)->local_apic.set_id (0);
  BX_INSTR_INIT(0);
#endif
  BX_CPU(0)->reset(BX_RESET_HARDWARE);
#else
  // SMP initialization
  bx_mem_array[0] = new BX_MEM_C ();
  bx_mem_array[0]->init_memory(bx_options.memory.Osize->get () * 1024*1024);

  // First load the optional ROM images
  if (bx_options.optrom[0].Opath->getptr () > 0)
    bx_mem_array[0]->load_ROM(bx_options.optrom[0].Opath->getptr (), bx_options.optrom[0].Oaddress->get ());
  if (bx_options.optrom[1].Opath->getptr () > 0)
    bx_mem_array[0]->load_ROM(bx_options.optrom[1].Opath->getptr (), bx_options.optrom[1].Oaddress->get ());
  if (bx_options.optrom[2].Opath->getptr () > 0)
    BX_MEM(0)->load_ROM(bx_options.optrom[2].Opath->getptr (), bx_options.optrom[2].Oaddress->get ());
    bx_mem_array[0]->load_ROM(bx_options.optrom[2].Opath->getptr (), bx_options.optrom[2].Oaddress->get ());
  if (bx_options.optrom[3].Opath->getptr () > 0)
    bx_mem_array[0]->load_ROM(bx_options.optrom[3].Opath->getptr (), bx_options.optrom[3].Oaddress->get ());

  // Then Load the BIOS and VGABIOS
  bx_mem_array[0]->load_ROM(bx_options.rom.Opath->getptr (), bx_options.rom.Oaddress->get ());
  bx_mem_array[0]->load_ROM(bx_options.vgarom.Opath->getptr (), 0xc0000);

  for (int i=0; i<BX_SMP_PROCESSORS; i++) {
    BX_CPU(i) = new BX_CPU_C ();
    BX_CPU(i)->init (bx_mem_array[0]);
    // assign apic ID from the index of this loop
    // if !BX_SUPPORT_APIC, this will not compile.
    BX_CPU(i)->local_apic.set_id (i);
    BX_INSTR_INIT(i);
    BX_CPU(i)->reset(BX_RESET_HARDWARE);
  }
#endif

#if BX_DEBUGGER == 0
  DEV_init_devices();
  DEV_reset_devices(BX_RESET_HARDWARE);
  bx_gui->init_signal_handlers ();
  bx_pc_system.start_timers();
#endif
  BX_DEBUG(("bx_init_hardware is setting signal handlers"));
// if not using debugger, then we can take control of SIGINT.
#if !BX_DEBUGGER
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


int
bx_atexit(void)
{
  static bx_bool been_here = 0;
  if (been_here) return 1;   // protect from reentry
  been_here = 1;


#if BX_PROVIDE_DEVICE_MODELS==1
  bx_pc_system.exit();
#endif

#if BX_DEBUGGER == 0
  if (SIM && SIM->get_init_done ()) {
    for (int cpu=0; cpu<BX_SMP_PROCESSORS; cpu++)
      if (BX_CPU(cpu)) BX_CPU(cpu)->atexit();
  }
#endif

#if BX_PCI_SUPPORT
  if (bx_options.Oi440FXSupport->get ()) {
    bx_devices.pluginPciBridge->print_i440fx_state();
    }
#endif

  // restore signal handling to defaults
#if !BX_DEBUGGER
  BX_INFO (("restoring default signal behavior"));
  signal(SIGINT, SIG_DFL);
#endif

#if BX_SHOW_IPS
#ifndef __MINGW32__
  signal(SIGALRM, SIG_DFL);
#endif
#endif
        return 0;
}

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
#if (!defined(WIN32)) && !BX_WITH_MACOS
      // only try this on unix
    case 3:
      {
      char *ptr = getenv("HOME");
      if (ptr) sprintf (rcfile, "%s/.bochsrc", ptr);
      }
      break;
     case 4: strcpy (rcfile, "/etc/bochsrc"); break;
#endif
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

  bochsrc_include_count++;

  fd = fopen (rcfile, "r");
  if (fd == NULL) return -1;

  int retval = 0;
  do {
    ret = fgets(line, sizeof(line)-1, fd);
    line[sizeof(line) - 1] = '\0';
    int len = strlen(line);
    if (len>0)
      line[len-1] = '\0';
    if ((ret != NULL) && strlen(line)) {
      if (parse_line_unformatted(rcfile, line) < 0) {
        retval = -1;
        break;  // quit parsing after first error
        }
      }
    } while (!feof(fd));
  fclose(fd);
  bochsrc_include_count--;
  return retval;
}

  static Bit32s
parse_line_unformatted(char *context, char *line)
{
#define MAX_PARAMS_LEN 40
  char *ptr;
  unsigned i, string_i;
  char string[512];
  char *params[MAX_PARAMS_LEN];
  int num_params;
  bx_bool inquotes = 0;

  memset(params, 0, sizeof(params));
  if (line == NULL) return 0;

  // if passed nothing but whitespace, just return
  for (i=0; i<strlen(line); i++) {
    if (!isspace(line[i])) break;
    }
  if (i>=strlen(line))
    return 0;

  num_params = 0;

  if (!strncmp(line, "#include", 8))
    ptr = strtok(line, " ");
  else
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
  Bit32s retval = parse_line_formatted(context, num_params, &params[0]);
  for (i=0; i < MAX_PARAMS_LEN; i++)
  {
    if ( params[i] != NULL )
    {
        free(params[i]);
        params[i] = NULL;
    }
  }
  return retval;
}

// These macros are called for all parse errors, so that we can easily
// change the behavior of all occurrences.
#define PARSE_ERR(x)  \
  do { BX_PANIC(x); return -1; } while (0)
#define PARSE_WARN(x)  \
  BX_ERROR(x)

  static Bit32s
parse_line_formatted(char *context, int num_params, char *params[])
{
  int i;

  if (num_params < 1) return 0;

  if (!strcmp(params[0], "#include")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: ignoring malformed #include directive.", context));
      }
    if (!strcmp(params[1], context)) {
      PARSE_ERR(("%s: cannot include this file again.", context));
      }
    if (bochsrc_include_count == 2) {
      PARSE_ERR(("%s: include directive in an included file not supported yet.", context));
      }
    bx_read_configuration(params[1]);
    }
  else if (params[0][0] == '#') return 0; /* comment */
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
      else if (!strncmp(params[i], "360k=", 5)) {
        bx_options.floppya.Opath->set (&params[i][5]);
        bx_options.floppya.Otype->set (BX_FLOPPY_360K);
        }
      else if (!strncmp(params[i], "status=ejected", 14)) {
        bx_options.floppya.Ostatus->set (BX_EJECTED);
        }
      else if (!strncmp(params[i], "status=inserted", 15)) {
        bx_options.floppya.Ostatus->set (BX_INSERTED);
        }
      else {
        PARSE_ERR(("%s: floppya attribute '%s' not understood.", context,
          params[i]));
        }
      }
    }
   else if (!strcmp(params[0], "gdbstub_port"))
     {
       if (num_params != 2)
         {
            fprintf(stderr, ".bochsrc: gdbstub_port directive: wrong # args.\n");
            exit(1);
         }
       bx_options.gdbstub.port = atoi(params[1]);
     }
   else if (!strcmp(params[0], "gdbstub_text_base"))
     {
       if (num_params != 2)
         {
            fprintf(stderr, ".bochsrc: gdbstub_text_base directive: wrong # args.\n");
            exit(1);
         }
       bx_options.gdbstub.text_base = atoi(params[1]);
     }
   else if (!strcmp(params[0], "gdbstub_data_base"))
     {
       if (num_params != 2)
         {
            fprintf(stderr, ".bochsrc: gdbstub_data_base directive: wrong # args.\n");
            exit(1);
         }
       bx_options.gdbstub.data_base = atoi(params[1]);
     }
   else if (!strcmp(params[0], "gdbstub_bss_base"))
     {
       if (num_params != 2)
         {
            fprintf(stderr, ".bochsrc: gdbstub_bss_base directive: wrong # args.\n");
            exit(1);
         }
       bx_options.gdbstub.bss_base = atoi(params[1]);
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
      else if (!strncmp(params[i], "360k=", 5)) {
        bx_options.floppyb.Opath->set (&params[i][5]);
        bx_options.floppyb.Otype->set (BX_FLOPPY_360K);
        }
      else if (!strncmp(params[i], "status=ejected", 14)) {
        bx_options.floppyb.Ostatus->set (BX_EJECTED);
        }
      else if (!strncmp(params[i], "status=inserted", 15)) {
        bx_options.floppyb.Ostatus->set (BX_INSERTED);
        }
      else {
        PARSE_ERR(("%s: floppyb attribute '%s' not understood.", context,
          params[i]));
        }
      }
    }

  else if ((!strncmp(params[0], "ata", 3)) && (strlen(params[0]) == 4)) {
    Bit8u channel = params[0][3];

    if ((channel < '0') || (channel > '9')) {
      PARSE_ERR(("%s: ataX directive malformed.", context));
      }
    channel-='0';
    if (channel >= BX_MAX_ATA_CHANNEL) {
      PARSE_ERR(("%s: ataX directive malformed.", context));
      }

    if ((num_params < 2) || (num_params > 5)) {
      PARSE_ERR(("%s: ataX directive malformed.", context));
      }

    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: ataX directive malformed.", context));
      }
    else {
      bx_options.ata[channel].Opresent->set (atol(&params[1][8]));
      }

    if (num_params > 2) {
      if (strncmp(params[2], "ioaddr1=", 8)) {
        PARSE_ERR(("%s: ataX directive malformed.", context));
        }
      else {
        if ( (params[2][8] == '0') && (params[2][9] == 'x') )
          bx_options.ata[channel].Oioaddr1->set (strtoul (&params[2][8], NULL, 16));
        else
          bx_options.ata[channel].Oioaddr1->set (strtoul (&params[2][8], NULL, 10));
        }
      }

    if (num_params > 3) {
      if (strncmp(params[3], "ioaddr2=", 8)) {
        PARSE_ERR(("%s: ataX directive malformed.", context));
        }
      else {
        if ( (params[3][8] == '0') && (params[3][9] == 'x') )
          bx_options.ata[channel].Oioaddr2->set (strtoul (&params[3][8], NULL, 16));
        else
          bx_options.ata[channel].Oioaddr2->set (strtoul (&params[3][8], NULL, 10));
        }
      }

    if (num_params > 4) {
      if (strncmp(params[4], "irq=", 4)) {
        PARSE_ERR(("%s: ataX directive malformed.", context));
        }
      else {
        bx_options.ata[channel].Oirq->set (atol(&params[4][4]));
        }
      }
    }

  // ataX-master, ataX-slave
  else if ((!strncmp(params[0], "ata", 3)) && (strlen(params[0]) > 4)) {
    Bit8u channel = params[0][3], slave = 0;

    if ((channel < '0') || (channel > '9')) {
      PARSE_ERR(("%s: ataX-master/slave directive malformed.", context));
      }
    channel-='0';
    if (channel >= BX_MAX_ATA_CHANNEL) {
      PARSE_ERR(("%s: ataX-master/slave directive malformed.", context));
      }

    if ((strcmp(&params[0][4], "-slave")) &&
        (strcmp(&params[0][4], "-master"))) {
      PARSE_ERR(("%s: ataX-master/slave directive malformed.", context));
      }

    if (!strcmp(&params[0][4], "-slave")) {
      slave = 1;
      }

    if (bx_options.atadevice[channel][slave].Opresent->get()) {
      PARSE_ERR(("%s: %s device of ata channel %d already defined.", context, slave?"slave":"master",channel));
      }

    for (i=1; i<num_params; i++) {
      if (!strcmp(params[i], "type=disk")) {
        bx_options.atadevice[channel][slave].Otype->set (BX_ATA_DEVICE_DISK);
        }
      else if (!strcmp(params[i], "type=cdrom")) {
        bx_options.atadevice[channel][slave].Otype->set (BX_ATA_DEVICE_CDROM);
        }
      else if (!strncmp(params[i], "path=", 5)) {
        bx_options.atadevice[channel][slave].Opath->set (&params[i][5]);
        }
      else if (!strncmp(params[i], "cylinders=", 10)) {
        bx_options.atadevice[channel][slave].Ocylinders->set (atol(&params[i][10]));
        }
      else if (!strncmp(params[i], "heads=", 6)) {
        bx_options.atadevice[channel][slave].Oheads->set (atol(&params[i][6]));
        }
      else if (!strncmp(params[i], "spt=", 4)) {
        bx_options.atadevice[channel][slave].Ospt->set (atol(&params[i][4]));
        }
      else if (!strncmp(params[i], "model=", 6)) {
        bx_options.atadevice[channel][slave].Omodel->set(&params[i][6]);
        }
      else if (!strcmp(params[i], "biosdetect=none")) {
        bx_options.atadevice[channel][slave].Obiosdetect->set(BX_ATA_BIOSDETECT_NONE);
        }
      else if (!strcmp(params[i], "biosdetect=cmos")) {
        bx_options.atadevice[channel][slave].Obiosdetect->set(BX_ATA_BIOSDETECT_CMOS);
        }
      else if (!strcmp(params[i], "biosdetect=auto")) {
        bx_options.atadevice[channel][slave].Obiosdetect->set(BX_ATA_BIOSDETECT_AUTO);
        }
      else if (!strcmp(params[i], "translation=none")) {
        bx_options.atadevice[channel][slave].Otranslation->set(BX_ATA_TRANSLATION_NONE);
        }
      else if (!strcmp(params[i], "translation=lba")) {
        bx_options.atadevice[channel][slave].Otranslation->set(BX_ATA_TRANSLATION_LBA);
        }
      else if (!strcmp(params[i], "translation=large")) { 
        bx_options.atadevice[channel][slave].Otranslation->set(BX_ATA_TRANSLATION_LARGE);
        }
      else if (!strcmp(params[i], "translation=echs")) { // synonym of large
        bx_options.atadevice[channel][slave].Otranslation->set(BX_ATA_TRANSLATION_LARGE);
        }
      else if (!strcmp(params[i], "translation=rechs")) {
        bx_options.atadevice[channel][slave].Otranslation->set(BX_ATA_TRANSLATION_RECHS);
        }
      else if (!strcmp(params[i], "translation=auto")) {
        bx_options.atadevice[channel][slave].Otranslation->set(BX_ATA_TRANSLATION_AUTO);
        }
      else if (!strcmp(params[i], "status=ejected")) {
        bx_options.atadevice[channel][slave].Ostatus->set(BX_EJECTED);
        }
      else if (!strcmp(params[i], "status=inserted")) {
        bx_options.atadevice[channel][slave].Ostatus->set(BX_INSERTED);
        }
      else {
        PARSE_ERR(("%s: ataX-master/slave directive malformed.", context));
        }
      }

    // Enables the ata device
    bx_options.atadevice[channel][slave].Opresent->set(1);

    // if enabled check if device ok
    if (bx_options.atadevice[channel][slave].Opresent->get() == 1) {
      if (bx_options.atadevice[channel][slave].Otype->get() == BX_ATA_DEVICE_DISK) {
        if (strlen(bx_options.atadevice[channel][slave].Opath->getptr()) ==0)
          PARSE_WARN(("%s: ataX-master/slave has empty path", context));
        if ((bx_options.atadevice[channel][slave].Ocylinders->get() == 0) ||
            (bx_options.atadevice[channel][slave].Oheads->get() ==0 ) ||
            (bx_options.atadevice[channel][slave].Ospt->get() == 0)) {
          PARSE_WARN(("%s: ataX-master/slave cannot have zero cylinders, heads, or sectors/track", context));
          }
        }
      else if (bx_options.atadevice[channel][slave].Otype->get() == BX_ATA_DEVICE_CDROM) {
        if (strlen(bx_options.atadevice[channel][slave].Opath->getptr()) == 0) {
          PARSE_WARN(("%s: ataX-master/slave has empty path", context));
          }
        }
      else {
        PARSE_WARN(("%s: ataX-master/slave: type should be specified", context));
        }
      }
    }

  // Legacy disk options emulation
  else if (!strcmp(params[0], "diskc")) {
    if (bx_options.atadevice[0][0].Opresent->get()) {
      PARSE_ERR(("%s: master device of ata channel 0 already defined.", context));
      }
    if (num_params != 5) {
      PARSE_ERR(("%s: diskc directive malformed.", context));
      }
    if (strncmp(params[1], "file=", 5) ||
        strncmp(params[2], "cyl=", 4) ||
        strncmp(params[3], "heads=", 6) ||
        strncmp(params[4], "spt=", 4)) {
      PARSE_ERR(("%s: diskc directive malformed.", context));
      }
    bx_options.ata[0].Opresent->set(1);
    bx_options.atadevice[0][0].Otype->set (BX_ATA_DEVICE_DISK);
    bx_options.atadevice[0][0].Opath->set (&params[1][5]);
    bx_options.atadevice[0][0].Ocylinders->set (atol(&params[2][4]));
    bx_options.atadevice[0][0].Oheads->set     (atol(&params[3][6]));
    bx_options.atadevice[0][0].Ospt->set       (atol(&params[4][4]));
    bx_options.atadevice[0][0].Opresent->set (1);
    }
  else if (!strcmp(params[0], "diskd")) {
    if (bx_options.atadevice[0][1].Opresent->get()) {
      PARSE_ERR(("%s: slave device of ata channel 0 already defined.", context));
      }
    if (num_params != 5) {
      PARSE_ERR(("%s: diskd directive malformed.", context));
      }
    if (strncmp(params[1], "file=", 5) ||
        strncmp(params[2], "cyl=", 4) ||
        strncmp(params[3], "heads=", 6) ||
        strncmp(params[4], "spt=", 4)) {
      PARSE_ERR(("%s: diskd directive malformed.", context));
      }
    bx_options.ata[0].Opresent->set(1);
    bx_options.atadevice[0][1].Otype->set (BX_ATA_DEVICE_DISK);
    bx_options.atadevice[0][1].Opath->set (&params[1][5]);
    bx_options.atadevice[0][1].Ocylinders->set (atol( &params[2][4]));
    bx_options.atadevice[0][1].Oheads->set     (atol( &params[3][6]));
    bx_options.atadevice[0][1].Ospt->set       (atol( &params[4][4]));
    bx_options.atadevice[0][1].Opresent->set (1);
    }
  else if (!strcmp(params[0], "cdromd")) {
    if (bx_options.atadevice[0][1].Opresent->get()) {
      PARSE_ERR(("%s: slave device of ata channel 0 already defined.", context));
      }
    if (num_params != 3) {
      PARSE_ERR(("%s: cdromd directive malformed.", context));
      }
    if (strncmp(params[1], "dev=", 4) || strncmp(params[2], "status=", 7)) {
      PARSE_ERR(("%s: cdromd directive malformed.", context));
      }
    bx_options.ata[0].Opresent->set(1);
    bx_options.atadevice[0][1].Otype->set (BX_ATA_DEVICE_CDROM);
    bx_options.atadevice[0][1].Opath->set (&params[1][4]);
    if (!strcmp(params[2], "status=inserted"))
      bx_options.atadevice[0][1].Ostatus->set (BX_INSERTED);
    else if (!strcmp(params[2], "status=ejected"))
      bx_options.atadevice[0][1].Ostatus->set (BX_EJECTED);
    else {
      PARSE_ERR(("%s: cdromd directive malformed.", context));
      }
    bx_options.atadevice[0][1].Opresent->set (1);
    }

  else if (!strcmp(params[0], "boot")) {
    if (!strcmp(params[1], "a")) {
      bx_options.Obootdrive->set (BX_BOOT_FLOPPYA);
    } else if (!strcmp(params[1], "floppy")) {
      bx_options.Obootdrive->set (BX_BOOT_FLOPPYA);
    } else if (!strcmp(params[1], "c")) {
      bx_options.Obootdrive->set (BX_BOOT_DISKC);
    } else if (!strcmp(params[1], "disk")) {
      bx_options.Obootdrive->set (BX_BOOT_DISKC);
    } else if (!strcmp(params[1], "cdrom")) {
      bx_options.Obootdrive->set (BX_BOOT_CDROM);
    } else {
      PARSE_ERR(("%s: boot directive with unknown boot device '%s'.  use 'a', 'c' or 'cdrom'.", context, params[1]));
      }
    }

  else if (!strcmp(params[0], "com1")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        bx_options.com[0].Oenabled->set (atol(&params[i][8]));
        }
      else if (!strncmp(params[i], "dev=", 4)) {
        bx_options.com[0].Odev->set (&params[i][4]);
        bx_options.com[0].Oenabled->set (1);
        }
      else {
        PARSE_ERR(("%s: unknown parameter for com1 ignored.", context));
        }
      }
    }
#if 0
  else if (!strcmp(params[0], "com2")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        bx_options.com[1].Oenabled->set (atol(&params[i][8]));
        }
      else if (!strncmp(params[i], "dev=", 4)) {
        bx_options.com[1].Odev->set (&params[i][4]);
        bx_options.com[1].Oenabled->set (1);
        }
      else {
        PARSE_ERR(("%s: unknown parameter for com2 ignored.", context));
        }
      }
    }
  else if (!strcmp(params[0], "com3")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        bx_options.com[2].Oenabled->set (atol(&params[i][8]));
        }
      else if (!strncmp(params[i], "dev=", 4)) {
        bx_options.com[2].Odev->set (&params[i][4]);
        bx_options.com[2].Oenabled->set (1);
        }
      else {
        PARSE_ERR(("%s: unknown parameter for com3 ignored.", context));
        }
      }
    }
  else if (!strcmp(params[0], "com4")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        bx_options.com[3].Oenabled->set (atol(&params[i][8]));
        }
      else if (!strncmp(params[i], "dev=", 4)) {
        bx_options.com[3].Odev->set (&params[i][4]);
        bx_options.com[3].Oenabled->set (1);
        }
      else {
        PARSE_ERR(("%s: unknown parameter for com4 ignored.", context));
        }
      }
    }
#endif

  else if (!strcmp(params[0], "floppy_bootsig_check")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: floppy_bootsig_check directive malformed.", context));
      }
    if (strncmp(params[1], "disabled=", 9)) {
      PARSE_ERR(("%s: floppy_bootsig_check directive malformed.", context));
      }
    if (params[1][9] == '0')
      bx_options.OfloppySigCheck->set (0);
    else if (params[1][9] == '1')
      bx_options.OfloppySigCheck->set (1);
    else {
      PARSE_ERR(("%s: floppy_bootsig_check directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "log")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: log directive has wrong # args.", context));
      }
    bx_options.log.Ofilename->set (params[1]);
    }
  else if (!strcmp(params[0], "logprefix")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: logprefix directive has wrong # args.", context));
      }
    bx_options.log.Oprefix->set (params[1]);
    }
  else if (!strcmp(params[0], "panic")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: panic directive malformed.", context));
      }
    if (strncmp(params[1], "action=", 7)) {
      PARSE_ERR(("%s: panic directive malformed.", context));
      }
    char *action = 7 + params[1];
    if (!strcmp(action, "fatal"))
      SIM->set_default_log_action (LOGLEV_PANIC, ACT_FATAL);
    else if (!strcmp (action, "report"))
      SIM->set_default_log_action (LOGLEV_PANIC, ACT_REPORT);
    else if (!strcmp (action, "ignore"))
      SIM->set_default_log_action (LOGLEV_PANIC, ACT_IGNORE);
    else if (!strcmp (action, "ask"))
      SIM->set_default_log_action (LOGLEV_PANIC, ACT_ASK);
    else {
      PARSE_ERR(("%s: panic directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "error")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: error directive malformed.", context));
      }
    if (strncmp(params[1], "action=", 7)) {
      PARSE_ERR(("%s: error directive malformed.", context));
      }
    char *action = 7 + params[1];
    if (!strcmp(action, "fatal"))
      SIM->set_default_log_action (LOGLEV_ERROR, ACT_FATAL);
    else if (!strcmp (action, "report"))
      SIM->set_default_log_action (LOGLEV_ERROR, ACT_REPORT);
    else if (!strcmp (action, "ignore"))
      SIM->set_default_log_action (LOGLEV_ERROR, ACT_IGNORE);
    else if (!strcmp (action, "ask"))
      SIM->set_default_log_action (LOGLEV_ERROR, ACT_ASK);
    else {
      PARSE_ERR(("%s: error directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "info")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: info directive malformed.", context));
      }
    if (strncmp(params[1], "action=", 7)) {
      PARSE_ERR(("%s: info directive malformed.", context));
      }
    char *action = 7 + params[1];
    if (!strcmp(action, "fatal"))
      SIM->set_default_log_action (LOGLEV_INFO, ACT_FATAL);
    else if (!strcmp (action, "report"))
      SIM->set_default_log_action (LOGLEV_INFO, ACT_REPORT);
    else if (!strcmp (action, "ignore"))
      SIM->set_default_log_action (LOGLEV_INFO, ACT_IGNORE);
    else if (!strcmp (action, "ask"))
      SIM->set_default_log_action (LOGLEV_INFO, ACT_ASK);
    else {
      PARSE_ERR(("%s: info directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "debug")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: debug directive malformed.", context));
      }
    if (strncmp(params[1], "action=", 7)) {
      PARSE_ERR(("%s: debug directive malformed.", context));
      }
    char *action = 7 + params[1];
    if (!strcmp(action, "fatal"))
      SIM->set_default_log_action (LOGLEV_DEBUG, ACT_FATAL);
    else if (!strcmp (action, "report"))
      SIM->set_default_log_action (LOGLEV_DEBUG, ACT_REPORT);
    else if (!strcmp (action, "ignore"))
      SIM->set_default_log_action (LOGLEV_DEBUG, ACT_IGNORE);
    else if (!strcmp (action, "ask"))
      SIM->set_default_log_action (LOGLEV_DEBUG, ACT_ASK);
    else {
      PARSE_ERR(("%s: debug directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "romimage")) {
    if (num_params != 3) {
      PARSE_ERR(("%s: romimage directive: wrong # args.", context));
      }
    if (strncmp(params[1], "file=", 5)) {
      PARSE_ERR(("%s: romimage directive malformed.", context));
      }
    if (strncmp(params[2], "address=", 8)) {
      PARSE_ERR(("%s: romimage directive malformed.", context));
      }
    bx_options.rom.Opath->set (&params[1][5]);
    if ( (params[2][8] == '0') && (params[2][9] == 'x') )
      bx_options.rom.Oaddress->set (strtoul (&params[2][8], NULL, 16));
    else
      bx_options.rom.Oaddress->set (strtoul (&params[2][8], NULL, 10));
    }
  else if (!strcmp(params[0], "optromimage1")) {
    if (num_params != 3) {
      PARSE_ERR(("%s: optromimage1 directive: wrong # args.", context));
      }
    if (strncmp(params[1], "file=", 5)) {
      PARSE_ERR(("%s: optromimage1 directive malformed.", context));
      }
    if (strncmp(params[2], "address=", 8)) {
      PARSE_ERR(("%s: optromimage2 directive malformed.", context));
      }
    bx_options.optrom[0].Opath->set (&params[1][5]);
    if ( (params[2][8] == '0') && (params[2][9] == 'x') )
      bx_options.optrom[0].Oaddress->set (strtoul (&params[2][8], NULL, 16));
    else
      bx_options.optrom[0].Oaddress->set (strtoul (&params[2][8], NULL, 10));
    }
  else if (!strcmp(params[0], "optromimage2")) {
    if (num_params != 3) {
      PARSE_ERR(("%s: optromimage2 directive: wrong # args.", context));
      }
    if (strncmp(params[1], "file=", 5)) {
      PARSE_ERR(("%s: optromimage2 directive malformed.", context));
      }
    if (strncmp(params[2], "address=", 8)) {
      PARSE_ERR(("%s: optromimage2 directive malformed.", context));
      }
    bx_options.optrom[1].Opath->set (&params[1][5]);
    if ( (params[2][8] == '0') && (params[2][9] == 'x') )
      bx_options.optrom[1].Oaddress->set (strtoul (&params[2][8], NULL, 16));
    else
      bx_options.optrom[1].Oaddress->set (strtoul (&params[2][8], NULL, 10));
    }
  else if (!strcmp(params[0], "optromimage3")) {
    if (num_params != 3) {
      PARSE_ERR(("%s: optromimage3 directive: wrong # args.", context));
      }
    if (strncmp(params[1], "file=", 5)) {
      PARSE_ERR(("%s: optromimage3 directive malformed.", context));
      }
    if (strncmp(params[2], "address=", 8)) {
      PARSE_ERR(("%s: optromimage2 directive malformed.", context));
      }
    bx_options.optrom[2].Opath->set (&params[1][5]);
    if ( (params[2][8] == '0') && (params[2][9] == 'x') )
      bx_options.optrom[2].Oaddress->set (strtoul (&params[2][8], NULL, 16));
    else
      bx_options.optrom[2].Oaddress->set (strtoul (&params[2][8], NULL, 10));
    }
  else if (!strcmp(params[0], "optromimage4")) {
    if (num_params != 3) {
      PARSE_ERR(("%s: optromimage4 directive: wrong # args.", context));
      }
    if (strncmp(params[1], "file=", 5)) {
      PARSE_ERR(("%s: optromimage4 directive malformed.", context));
      }
    if (strncmp(params[2], "address=", 8)) {
      PARSE_ERR(("%s: optromimage2 directive malformed.", context));
      }
    bx_options.optrom[3].Opath->set (&params[1][5]);
    if ( (params[2][8] == '0') && (params[2][9] == 'x') )
      bx_options.optrom[3].Oaddress->set (strtoul (&params[2][8], NULL, 16));
    else
      bx_options.optrom[3].Oaddress->set (strtoul (&params[2][8], NULL, 10));
    }
  else if (!strcmp(params[0], "vgaromimage")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: vgaromimage directive: wrong # args.", context));
      }
    bx_options.vgarom.Opath->set (params[1]);
    }
  else if (!strcmp(params[0], "vga_update_interval")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: vga_update_interval directive: wrong # args.", context));
      }
    bx_options.Ovga_update_interval->set (atol(params[1]));
    if (bx_options.Ovga_update_interval->get () < 50000) {
      BX_INFO(("%s: vga_update_interval seems awfully small!", context));
      }
    }
  else if (!strcmp(params[0], "keyboard_serial_delay")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: keyboard_serial_delay directive: wrong # args.", context));
      }
    bx_options.Okeyboard_serial_delay->set (atol(params[1]));
    if (bx_options.Okeyboard_serial_delay->get () < 5) {
      PARSE_ERR (("%s: keyboard_serial_delay not big enough!", context));
      }
    }
  else if (!strcmp(params[0], "keyboard_paste_delay")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: keyboard_paste_delay directive: wrong # args.", context));
      }
    bx_options.Okeyboard_paste_delay->set (atol(params[1]));
    if (bx_options.Okeyboard_paste_delay->get () < 1000) {
      PARSE_ERR (("%s: keyboard_paste_delay not big enough!", context));
      }
    }
  else if (!strcmp(params[0], "megs")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: megs directive: wrong # args.", context));
      }
    bx_options.memory.Osize->set (atol(params[1]));
    }
  else if (!strcmp(params[0], "floppy_command_delay")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: floppy_command_delay directive: wrong # args.", context));
      }
    bx_options.Ofloppy_command_delay->set (atol(params[1]));
    if (bx_options.Ofloppy_command_delay->get () < 100) {
      PARSE_ERR(("%s: floppy_command_delay not big enough!", context));
      }
    }
  else if (!strcmp(params[0], "ips")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: ips directive: wrong # args.", context));
      }
    bx_options.Oips->set (atol(params[1]));
    if (bx_options.Oips->get () < BX_MIN_IPS) {
      BX_ERROR(("%s: WARNING: ips is AWFULLY low!", context));
      }
    }
  else if (!strcmp(params[0], "realtime_pit")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: realtime_pit directive: wrong # args.", context));
      }
    if (!strncmp(params[1], "enable", 6)) {
      bx_options.Orealtime_pit->set (1);
      }
    else if (!strncmp(params[1], "disable", 7)) {
      bx_options.Orealtime_pit->set (0);
      }
    else if (!strncmp(params[1], "reproducible", 12)) {
      bx_options.Orealtime_pit->set (0);
      }
    else if (!strncmp(params[1], "realtime", 8)) {
      bx_options.Orealtime_pit->set (1);
      }
    else bx_options.Orealtime_pit->set (!!(atol(params[1])));
    }
  else if (!strcmp(params[0], "max_ips")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: max_ips directive: wrong # args.", context));
      }
    BX_INFO(("WARNING: max_ips not implemented"));
    }
  else if (!strcmp(params[0], "system_clock_sync")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: system_clock_sync directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: system_clock_sync directive malformed.", context));
      }
    if (params[1][8] == '0' || params[1][8] == '1')
      BX_INFO (("WARNING: system_clock_sync not implemented"));
    else
      PARSE_ERR(("%s: system_clock_sync directive malformed.", context));
    }
  else if (!strcmp(params[0], "mouse")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: mouse directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: mouse directive malformed.", context));
      }
    if (params[1][8] == '0' || params[1][8] == '1')
      bx_options.Omouse_enabled->set (params[1][8] - '0');
    else
      PARSE_ERR(("%s: mouse directive malformed.", context));
    }
  else if (!strcmp(params[0], "private_colormap")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: private_colormap directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: private_colormap directive malformed.", context));
      }
    if (params[1][8] == '0' || params[1][8] == '1')
      bx_options.Oprivate_colormap->set (params[1][8] - '0');
    else {
      PARSE_ERR(("%s: private_colormap directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "fullscreen")) {
#if BX_WITH_AMIGAOS
    if (num_params != 2) {
      PARSE_ERR(("%s: fullscreen directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: fullscreen directive malformed.", context));
      }
    if (params[1][8] == '0' || params[1][8] == '1') {
      bx_options.Ofullscreen->set (params[1][8] - '0');
    } else {
      PARSE_ERR(("%s: fullscreen directive malformed.", context));
      }
#endif
    }
  else if (!strcmp(params[0], "screenmode")) {
#if BX_WITH_AMIGAOS
    if (num_params != 2) {
      PARSE_ERR(("%s: screenmode directive malformed.", context));
      }
    if (strncmp(params[1], "name=", 5)) {
      PARSE_ERR(("%s: screenmode directive malformed.", context));
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
      if (!strncmp(params[i], "enabled=", 8)) {
        bx_options.par[0].Oenabled->set (atol(&params[i][8]));
        }
      else if (!strncmp(params[i], "file=", 5)) {
        bx_options.par[0].Ooutfile->set (strdup(&params[i][5]));
        bx_options.par[0].Oenabled->set (1);
        }
      else {
        BX_ERROR(("%s: unknown parameter for parport1 ignored.", context));
        }
    }
  }

#if 0
  else if (!strcmp(params[0], "parport2")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        bx_options.par[1].Oenabled->set (atol(&params[i][8]));
        }
      else if (!strncmp(params[i], "file=", 5)) {
        bx_options.par[1].Ooutfile->set (strdup(&params[i][5]));
        bx_options.par[1].Oenabled->set (1);
        }
      else {
        BX_ERROR(("%s: unknown parameter for parport2 ignored.", context));
        }
    }
  }
#endif

  else if (!strcmp(params[0], "i440fxsupport")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: i440FXSupport directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: i440FXSupport directive malformed.", context));
      }
    if (params[1][8] == '0')
      bx_options.Oi440FXSupport->set (0);
    else if (params[1][8] == '1')
      bx_options.Oi440FXSupport->set (1);
    else {
      PARSE_ERR(("%s: i440FXSupport directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "newharddrivesupport")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: newharddrivesupport directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: newharddrivesupport directive malformed.", context));
      }
    if (params[1][8] == '0')
      bx_options.OnewHardDriveSupport->set (0);
    else if (params[1][8] == '1')
      bx_options.OnewHardDriveSupport->set (1);
    else {
      PARSE_ERR(("%s: newharddrivesupport directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "cmosimage")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: cmosimage directive: wrong # args.", context));
      }
    bx_options.cmos.Opath->set (strdup(params[1]));
    bx_options.cmos.OcmosImage->set (1);                // CMOS Image is true
    }
  else if (!strcmp(params[0], "time0")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: time0 directive: wrong # args.", context));
      }
    bx_options.cmos.Otime0->set (atoi(params[1]));
    }
#ifdef MAGIC_BREAKPOINT
  else if (!strcmp(params[0], "magic_break")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: magic_break directive: wrong # args.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: magic_break directive malformed.", context));
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
      PARSE_ERR(("%s: magic_break directive malformed.", context));
      }
    }
#endif
  else if (!strcmp(params[0], "ne2k")) {
    int tmp[6];
    char tmpchar[6];
    bx_options.ne2k.Opresent->set (0);
    if ((num_params < 4) || (num_params > 7)) {
      PARSE_ERR(("%s: ne2k directive malformed.", context));
      }
    bx_options.ne2k.Oethmod->set ("null");
    if (strncmp(params[1], "ioaddr=", 7)) {
      PARSE_ERR(("%s: ne2k directive malformed.", context));
      }
    if (strncmp(params[2], "irq=", 4)) {
      PARSE_ERR(("%s: ne2k directive malformed.", context));
      }
    if (strncmp(params[3], "mac=", 4)) {
      PARSE_ERR(("%s: ne2k directive malformed.", context));
      }
    bx_options.ne2k.Oioaddr->set (strtoul(&params[1][7], NULL, 16));
    bx_options.ne2k.Oirq->set (atol(&params[2][4]));
    i = sscanf(&params[3][4], "%x:%x:%x:%x:%x:%x",
             &tmp[0],&tmp[1],&tmp[2],&tmp[3],&tmp[4],&tmp[5]);
    if (i != 6) {
      PARSE_ERR(("%s: ne2k mac address malformed.", context));
      }
    for (i=0;i<6;i++)
      tmpchar[i] = (unsigned char)tmp[i];
    bx_options.ne2k.Omacaddr->set (tmpchar);
    if (num_params > 4) {
      if (strncmp(params[4], "ethmod=", 7)) {
        PARSE_ERR(("%s: ne2k directive malformed.", context));
        }
      bx_options.ne2k.Oethmod->set (strdup(&params[4][7]));
      if (num_params > 5) {
        if (strncmp(params[5], "ethdev=", 7)) {
          PARSE_ERR(("%s: ne2k directive malformed.", context));
          }
        bx_options.ne2k.Oethdev->set (strdup(&params[5][7]));
        if (num_params > 6) {
          if (strncmp(params[6], "script=", 7)) {
            PARSE_ERR(("%s: ne2k directive malformed.", context));
            }
          bx_options.ne2k.Oscript->set (strdup(&params[6][7]));
          }
        }
      }
    bx_options.ne2k.Opresent->set (1);
    }

  else if (!strcmp(params[0], "load32bitOSImage")) {
    if ( (num_params!=4) && (num_params!=5) ) {
      PARSE_ERR(("%s: load32bitOSImage directive: wrong # args.", context));
      }
    if (strncmp(params[1], "os=", 3)) {
      PARSE_ERR(("%s: load32bitOSImage: directive malformed.", context));
      }
    if (!strcmp(&params[1][3], "nullkernel")) {
      bx_options.load32bitOSImage.OwhichOS->set (Load32bitOSNullKernel);
      }
    else if (!strcmp(&params[1][3], "linux")) {
      bx_options.load32bitOSImage.OwhichOS->set (Load32bitOSLinux);
      }
    else {
      PARSE_ERR(("%s: load32bitOSImage: unsupported OS.", context));
      }
    if (strncmp(params[2], "path=", 5)) {
      PARSE_ERR(("%s: load32bitOSImage: directive malformed.", context));
      }
    if (strncmp(params[3], "iolog=", 6)) {
      PARSE_ERR(("%s: load32bitOSImage: directive malformed.", context));
      }
    bx_options.load32bitOSImage.Opath->set (strdup(&params[2][5]));
    bx_options.load32bitOSImage.Oiolog->set (strdup(&params[3][6]));
    if (num_params == 5) {
      if (strncmp(params[4], "initrd=", 7)) {
        PARSE_ERR(("%s: load32bitOSImage: directive malformed.", context));
        }
      bx_options.load32bitOSImage.Oinitrd->set (strdup(&params[4][7]));
      }
    }
  else if (!strcmp(params[0], "keyboard_type")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: keyboard_type directive: wrong # args.", context));
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
      PARSE_ERR(("%s: keyboard_type directive: wrong arg %s.", context,params[1]));
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
  else if (!strcmp(params[0], "user_shortcut")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: user_shortcut directive: wrong # args.", context));
      }
    if(!strncmp(params[1], "keys=", 4)) {
      bx_options.Ouser_shortcut->set (strdup(&params[1][5]));
      }
    }
  else if (!strcmp(params[0], "config_interface")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: config_interface directive: wrong # args.", context));
      }
    if (!bx_options.Osel_config->set_by_name (params[1]))
      PARSE_ERR(("%s: config_interface '%s' not available", context, params[1]));
    }
  else if (!strcmp(params[0], "display_library")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: display_library directive: wrong # args.", context));
      }
    if (!bx_options.Osel_displaylib->set_by_name (params[1]))
      PARSE_ERR(("%s: display library '%s' not available", context, params[1]));
    }
  else {
    PARSE_ERR(( "%s: directive '%s' not understood", context, params[0]));
    }
  return 0;
}

static char *fdtypes[] = {
  "none", "1_2", "1_44", "2_88", "720k", "360k"
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
    opt->Ostatus->get ()==BX_EJECTED ? "ejected" : "inserted");
  return 0;
}

int 
bx_write_ata_options (FILE *fp, Bit8u channel, bx_ata_options *opt)
{
  fprintf (fp, "ata%d: enabled=%d", channel, opt->Opresent->get());

  if (opt->Opresent->get()) {
    fprintf (fp, ", ioaddr1=0x%x, ioaddr2=0x%x, irq=%d", opt->Oioaddr1->get(), 
      opt->Oioaddr2->get(), opt->Oirq->get());
    }

  fprintf (fp, "\n");
  return 0;
}

int 
bx_write_atadevice_options (FILE *fp, Bit8u channel, Bit8u drive, bx_atadevice_options *opt)
{
  if (opt->Opresent->get()) {
    fprintf (fp, "ata%d-%s: ", channel, drive==0?"master":"slave");

    if (opt->Otype->get() == BX_ATA_DEVICE_DISK) {
      fprintf (fp, "type=disk, path=\"%s\", cylinders=%d, heads=%d, spt=%d",
          opt->Opath->getptr(), opt->Ocylinders->get(), opt->Oheads->get(), opt->Ospt->get());

      switch(opt->Otranslation->get()) {
        case BX_ATA_TRANSLATION_NONE:
          fprintf (fp, ", translation=none");
          break;
        case BX_ATA_TRANSLATION_LBA:
          fprintf (fp, ", translation=lba");
          break;
        case BX_ATA_TRANSLATION_LARGE:
          fprintf (fp, ", translation=large");
          break;
        case BX_ATA_TRANSLATION_RECHS:
          fprintf (fp, ", translation=rechs");
          break;
        case BX_ATA_TRANSLATION_AUTO:
          fprintf (fp, ", translation=auto");
          break;
        }
      }
    else if (opt->Otype->get() == BX_ATA_DEVICE_CDROM) {
      fprintf (fp, "type=cdrom, path=\"%s\", status=%s", 
        opt->Opath->getptr(), 
        opt->Ostatus->get ()==BX_EJECTED ? "ejected" : "inserted");
      }

    switch(opt->Obiosdetect->get()) {
      case BX_ATA_BIOSDETECT_NONE:
        fprintf (fp, ", biosdetect=none");
        break;
      case BX_ATA_BIOSDETECT_CMOS:
        fprintf (fp, ", biosdetect=cmos");
        break;
      case BX_ATA_BIOSDETECT_AUTO:
        fprintf (fp, ", biosdetect=auto");
        break;
      }
    if (strlen(opt->Omodel->getptr())>0) {
        fprintf (fp, ", model=\"%s\"", opt->Omodel->getptr());
      }

    fprintf (fp, "\n");
  }
  return 0;
}

int
bx_write_parport_options (FILE *fp, bx_parport_options *opt, int n)
{
  fprintf (fp, "parport%d: enabled=%d", n, opt->Oenabled->get ());
  if (opt->Oenabled->get ()) {
    fprintf (fp, ", file=\"%s\"", opt->Ooutfile->getptr ());
  }
  fprintf (fp, "\n");
  return 0;
}

int
bx_write_serial_options (FILE *fp, bx_serial_options *opt, int n)
{
  fprintf (fp, "com%d: enabled=%d", n, opt->Oenabled->get ());
  if (opt->Oenabled->get ()) {
    fprintf (fp, ", dev=\"%s\"", opt->Odev->getptr ());
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
  if (!opt->Opresent->get ()) {
    fprintf (fp, "# no ne2k\n");
    return 0;
  }
  char *ptr = opt->Omacaddr->getptr ();
  fprintf (fp, "ne2k: ioaddr=0x%x, irq=%d, mac=%02x:%02x:%02x:%02x:%02x:%02x, ethmod=%s, ethdev=%s, script=%s\n",
      opt->Oioaddr->get (), 
      opt->Oirq->get (),
      (unsigned int)(0xff & ptr[0]),
      (unsigned int)(0xff & ptr[1]),
      (unsigned int)(0xff & ptr[2]),
      (unsigned int)(0xff & ptr[3]),
      (unsigned int)(0xff & ptr[4]),
      (unsigned int)(0xff & ptr[5]),
      opt->Oethmod->getptr (),
      opt->Oethdev->getptr (),
      opt->Oscript->getptr ());
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
  fprintf (fp, "logprefix: %s\n", opt->Oprefix->getptr ());
  fprintf (fp, "panic: action=%s\n",
      io->getaction(logfunctions::get_default_action (LOGLEV_PANIC)));
  fprintf (fp, "error: action=%s\n",
      io->getaction(logfunctions::get_default_action (LOGLEV_ERROR)));
  fprintf (fp, "info: action=%s\n",
      io->getaction(logfunctions::get_default_action (LOGLEV_INFO)));
  fprintf (fp, "debug: action=%s\n",
      io->getaction(logfunctions::get_default_action (LOGLEV_DEBUG)));
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
  for (Bit8u channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    bx_write_ata_options (fp, channel, &bx_options.ata[channel]);
    bx_write_atadevice_options (fp, channel, 0, &bx_options.atadevice[channel][0]);
    bx_write_atadevice_options (fp, channel, 1, &bx_options.atadevice[channel][1]);
    }
  if (strlen (bx_options.rom.Opath->getptr ()) > 0)
    fprintf (fp, "romimage: file=%s, address=0x%05x\n", bx_options.rom.Opath->getptr(), (unsigned int)bx_options.rom.Oaddress->get ());
  else
    fprintf (fp, "# no romimage\n");
  if (strlen (bx_options.vgarom.Opath->getptr ()) > 0)
    fprintf (fp, "vgaromimage: %s\n", bx_options.vgarom.Opath->getptr ());
  else
    fprintf (fp, "# no vgaromimage\n");
  if (strlen (bx_options.optrom[0].Opath->getptr ()) > 0)
    fprintf (fp, "optromimage1: file=%s, address=0x%05x\n", bx_options.optrom[0].Opath->getptr(), (unsigned int)bx_options.optrom[0].Oaddress->get ());
  if (strlen (bx_options.optrom[1].Opath->getptr ()) > 0)
    fprintf (fp, "optromimage2: file=%s, address=0x%05x\n", bx_options.optrom[1].Opath->getptr(), (unsigned int)bx_options.optrom[1].Oaddress->get ());
  if (strlen (bx_options.optrom[2].Opath->getptr ()) > 0)
    fprintf (fp, "optromimage3: file=%s, address=0x%05x\n", bx_options.optrom[2].Opath->getptr(), (unsigned int)bx_options.optrom[2].Oaddress->get ());
  if (strlen (bx_options.optrom[3].Opath->getptr ()) > 0)
    fprintf (fp, "optromimage4: file=%s, address=0x%05x\n", bx_options.optrom[3].Opath->getptr(), (unsigned int)bx_options.optrom[3].Oaddress->get ());
  fprintf (fp, "megs: %d\n", bx_options.memory.Osize->get ());
  bx_write_parport_options (fp, &bx_options.par[0], 1);
  //bx_write_parport_options (fp, &bx_options.par[1], 2);
  bx_write_serial_options (fp, &bx_options.com[0], 1);
  //bx_write_serial_options (fp, &bx_options.com[1], 2);
  //bx_write_serial_options (fp, &bx_options.com[2], 3);
  //bx_write_serial_options (fp, &bx_options.com[3], 4);
  bx_write_sb16_options (fp, &bx_options.sb16);
  int bootdrive = bx_options.Obootdrive->get ();
  fprintf (fp, "boot: %s\n", (bootdrive==BX_BOOT_FLOPPYA) ? "floppy" : (bootdrive==BX_BOOT_DISKC) ? "disk" : "cdrom");
  fprintf (fp, "floppy_bootsig_check: disabled=%d\n", bx_options.OfloppySigCheck->get ());
  fprintf (fp, "vga_update_interval: %u\n", bx_options.Ovga_update_interval->get ());
  fprintf (fp, "keyboard_serial_delay: %u\n", bx_options.Okeyboard_serial_delay->get ());
  fprintf (fp, "keyboard_paste_delay: %u\n", bx_options.Okeyboard_paste_delay->get ());
  fprintf (fp, "floppy_command_delay: %u\n", bx_options.Ofloppy_command_delay->get ());
  fprintf (fp, "ips: %u\n", bx_options.Oips->get ());
  fprintf (fp, "realtime_pit: %d\n", bx_options.Orealtime_pit->get ());
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
  // in a multithreaded environment, a signal such as SIGINT can be sent to all
  // threads.  This function is only intended to handle signals in the
  // simulator thread.  It will simply return if called from any other thread.
  // Otherwise the BX_PANIC() below can be called in multiple threads at
  // once, leading to multiple threads trying to display a dialog box,
  // leading to GUI deadlock.
  if (!SIM->is_sim_thread ()) {
    BX_INFO (("bx_signal_handler: ignored sig %d because it wasn't called from the simulator thread", signum));
    return;
  }
#if BX_GUI_SIGHANDLER
  if (bx_gui_sighandler) {
    // GUI signal handler gets first priority, if the mask says it's wanted
    if ((1<<signum) & bx_gui->get_sighandler_mask ()) {
      bx_gui->sighandler (signum);
      return;
    }
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
  if (bx_gui_sighandler) {
    if ((1<<signum) & bx_gui->get_sighandler_mask ()) {
      bx_gui->sighandler (signum);
      return;
    }
  }
#endif
  BX_PANIC(("SIGNAL %u caught", signum));
}

