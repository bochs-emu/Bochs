/*
 * gui/control.cc
 * $Id: control.cc,v 1.21 2001-06-17 13:50:52 bdenney Exp $
 *
 * This is code for a text-mode control panel.  Note that this file
 * does NOT include bochs.h.  Instead, it does all of its contact with
 * the simulator through an object called SIM, defined in siminterface.cc
 * and siminterface.h.  This separation adds an extra layer of method
 * calls before any work can be done, but the benefit is that the compiler
 * enforces the rules.  I can guarantee that control.cc doesn't call any
 * I/O device objects directly, for example, because the bx_devices symbol
 * isn't even defined in this context.
 *

Musings by Bryce:

Now that there are some capabilities here, start moving toward the desired
model of separation of gui and simulator.  Design a method-call interface
between objects which could later be turned into a network protocol.  

Most messages go from the control panel to bochs, with bochs sending a response
back.  Examples: turn on debug event logging for the PIC, or set IPS to
1.5million.  Others kinds of messages go from bochs to the control panel.
Example: errors such as "disk image not found", status information such as the
current CPU state.

But because of the nature of a control panel, most information flows from
the control panel to Bochs.  I think we can take advantage of this to
simplify the protocol.  The GUI can ask bochs to do things and get a 
response back.  Bochs can tell the GUI something, but will not get a
response back.

The control panel should exist before the Bochs simulator starts, so that the
user can get their settings right before booting up a simulator.  Once
bochs has begun, some settings can no longer be changed, such as how much
memory there is.  Others can be changed at any time, like the value of IPS
or the name of the floppy disk images.  If you had the bochs simulator
as a shared library, the GUI could let you choose which version of the
simulator library to load (one with debugging or SMP support or not, 
for example).

When the control panel connects to Bochs, each needs to learn what the other
can do.  Some versions of Bochs will have cdrom support, others will not.
Design a simple and general way for the two parties to describe what they
can do, so that the GUI can grey-out some menu choices, and Bochs knows
what the GUI can handle.  I think Bochs should provide a list of 
capabilities in something like the form of:
  typedef struct { Bit16u id; String name; } bx_capability_t;
Only the negotiation of capabilities uses the string name, and for everything
else the 16-bit id is used (use htons() if sending over network).  Each
capability is like a function that can be called on the remote object, 
and each function needs an input list and a return value.

Hmm.... This sounds like reinventing function calls.  Next I'm going to
say we need to implement prototypes and type checking.  Why not just
write a compiler!


*/

#include "config.h"
#if BX_USE_CONTROL_PANEL

extern "C" {
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
}
#include "osdep.h"
#include "control.h"
#include "siminterface.h"

#define BX_PANIC(x) printf x
#define CPANEL_PATH_LEN 512

/* functions for changing particular options */
void bx_control_panel_init ();
void bx_edit_mem ();
void bx_edit_rom_addr ();
void bx_edit_floppy (int drive);
void bx_edit_hard_disk (int drive);
void bx_edit_cdrom ();
void bx_edit_rom_path (int vga);
void bx_edit_rom_addr ();
void bx_newhd_support ();
void bx_private_colormap ();
void bx_boot_from ();
void bx_ips_change ();
int bx_read_rc (char *rc);
int bx_write_rc (char *rc);
void bx_log_file ();
void bx_log_options (int individual);
void bx_vga_update_interval ();
void bx_mouse_enable ();

/******************************************************************/
/* lots of code stolen from bximage.c */
/* remove leading spaces, newline junk at end.  returns pointer to 
 cleaned string, which is between s0 and the null */
char *
clean_string (char *s0)
{
  char *s = s0;
  char *ptr;
  /* find first nonblank */
  while (isspace (*s))
    s++;
  /* truncate string at first non-alphanumeric */
  ptr = s;
  while (isprint (*ptr))
    ptr++;
  *ptr = 0;
  return s;
}

/* returns 0 on success, -1 on failure.  The value goes into out. */
int 
ask_int (char *prompt, int min, int max, int the_default, int *out)
{
  int n = max + 1;
  char buffer[1024];
  char *clean;
  int illegal;
  while (1) {
    printf (prompt, the_default);
    if (!fgets (buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string (buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    illegal = (1 != sscanf (buffer, "%d", &n));
    if (illegal || n<min || n>max) {
      printf ("Your choice (%s) was not an integer between %d and %d.\n\n",
	  clean, min, max);
    } else {
      // choice is okay
      *out = n;
      return 0;
    }
  }
}

int 
ask_menu (char *prompt, int n_choices, char *choice[], int the_default, int *out)
{
  char buffer[1024];
  char *clean;
  int i;
  *out = -1;
  while (1) {
    printf (prompt, choice[the_default]);
    if (!fgets (buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string (buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    for (i=0; i<n_choices; i++) {
      if (!strcmp (choice[i], clean)) {
	// matched, return the choice number
	*out = i;
	return 0;
      }
    }
    printf ("Your choice (%s) did not match any of the choices:\n", clean);
    for (i=0; i<n_choices; i++) {
      if (i>0) printf (", ");
      printf ("%s", choice[i]);
    }
    printf ("\n");
  }
}

int 
ask_yn (char *prompt, int the_default, int *out)
{
  char buffer[16];
  char *clean;
  *out = -1;
  while (1) {
    // if there's a %s field, substitute in the default yes/no.
    printf (prompt, the_default ? "yes" : "no");
    if (!fgets (buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string (buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    switch (tolower(clean[0])) {
      case 'y': *out=1; return 0;
      case 'n': *out=0; return 0;
    }
    printf ("Please type either yes or no.\n");
  }
}

int 
ask_string (char *prompt, char *the_default, char *out)
{
  char buffer[1024];
  char *clean;
  out[0] = 0;
  printf (prompt, the_default);
  if (!fgets (buffer, sizeof(buffer), stdin))
    return -1;
  clean = clean_string (buffer);
  if (strlen(clean) < 1) {
    // empty line, use the default
    strcpy (out, the_default);
    return 0;
  }
  strcpy (out, clean);
  return 0;
}

/******************************************************************/

static char *ask_about_control_panel =
"\n"
"This version of Bochs has a prototype configuration interface.  Would\n"
"you like to try it?\n"
"\n"
"If you choose yes, you can use a menu to choose configuration options.\n"
"If you choose no, Bochs will read a bochsrc file and run as usual.\n"
"Type yes or no: [yes] ";

static char *startup_menu_prompt =
"------------------\n"
"Bochs Startup Menu\n"
"------------------\n"
"1. Read options from...\n"
"2. Edit options\n"
"3. Save options to...\n"
"4. Begin simulation\n"
"5. Quit now\n"
"\n"
"Please choose one: [%d] ";

static char *startup_options_prompt =
"------------------\n"
"Bochs Options Menu\n"
"------------------\n"
"0. Return to previous menu\n"
"1. Log file: %s\n"
"2. Log options for all devices\n"
"3. Log options for individual devices\n"
"4. Memory options\n"
"5. Interface options\n"
"6. Disk options\n"
"7. Sound options\n"
"8. Other options\n"
"\n"
"Please choose one: [0] ";

static char *startup_mem_options_prompt =
"--------------------\n"
"Bochs Memory Options\n"
"--------------------\n"
"0. Return to previous menu\n"
"1. Memory in Megabytes: %d\n"
"2. VGA ROM image: %s\n"
"3. ROM image: %s\n"
"4. ROM address: 0x%05x\n"
"\n"
"Please choose one: [0] ";

static char *startup_interface_options =
"------------------\n"
"Bochs Interface Options\n"
"------------------\n"
"0. Return to previous menu\n"
"1. VGA Update Interval: %d\n"
"2. Mouse: %s\n"
"3. Emulated instructions per second (IPS): %u\n"
"4. Private Colormap: %s\n"
"\n"
"Please choose one: [0] ";

static char *startup_disk_options_prompt =
"------------------\n"
"Bochs Disk Options\n"
"------------------\n"
"0. Return to previous menu\n"
"1. Floppy disk 0: %s\n"
"2. Floppy disk 1: %s\n"
"3. Hard disk 0: %s\n"
"4. Hard disk 1: %s\n"
"5. CDROM: %s\n"
"6. New Hard Drive Support: %s\n"
"7. Boot from: %s\n"
"%s\n"
"Please choose one: [0] ";

static char *startup_sound_options_prompt =
"------------------\n"
"Bochs Sound Options\n"
"------------------\n"
"0. Return to previous menu\n"
"1. Sound Blaster 16: disabled\n"
"2. MIDI mode: 1, \n"
"3. MIDI output file: /dev/midi00\n"
"4. Wave mode: 1\n"
"5. Wave output file: dev/dsp\n"
"6. SB16 log level: 2\n"
"7. SB16 log file: sb16.log\n"
"8. DMA Timer: 600000\n"
"\n"
"Please choose one: [0] ";

static char *startup_misc_options_prompt =
"---------------------------\n"
"Bochs Miscellaneous Options\n"
"---------------------------\n"
"1. Keyboard Serial Delay: 250\n"
"2. Floppy command delay: 500\n"
"To be added someday: magic_break, ne2k, load32bitOSImage,i440fxsupport,time0"
"\n"
"Please choose one: [0] ";

static char *runtime_menu_prompt =
"---------------------\n"
"Bochs Runtime Options\n"
"---------------------\n"
"1. Floppy disk 0: %s\n"
"2. Floppy disk 1: %s\n"
"3. CDROM: %s\n"
"4. Emulated instructions per second (IPS): %u\n"
"5. Log options for all devices\n"
"6. Log options for individual devices\n"
"7. VGA Update Interval: %d\n"
"8. Mouse: %s\n"
"9. Instruction tracing: off (doesn't exist yet)\n"
"10. Continue simulation\n"
"11. Quit now\n"
"\n"
"Please choose one:  [10] ";

char *menu_prompt_list[BX_CPANEL_N_MENUS] = {
  ask_about_control_panel,
  startup_menu_prompt,
  startup_options_prompt,
  startup_mem_options_prompt,
  startup_interface_options,
  startup_disk_options_prompt,
  startup_sound_options_prompt,
  startup_misc_options_prompt,
  runtime_menu_prompt
};

#define NOT_IMPLEMENTED(choice) \
  fprintf (stderr, "ERROR: choice %d not implemented\n", choice);

#define BAD_OPTION(menu,choice) \
  do {fprintf (stderr, "ERROR: control panel menu %d has no choice %d\n", menu, choice); \
      assert (0); } while (0)

char *fdsize_choices[] = { "0.72","1.2","1.44","2.88" };
int fdsize_n_choices = 4;

void build_disk_options_prompt (char *format, char *buf, int size)
{
  bx_floppy_options floppyop;
  bx_disk_options diskop;
  bx_cdrom_options cdromop;
  char buffer[7][128];
  for (int i=0; i<2; i++) {
    SIM->get_floppy_options (i, &floppyop);
    sprintf (buffer[i], "%s, size=%s, %s", floppyop.path,
	  SIM->get_floppy_type_name (floppyop.type),
	  floppyop.initial_status ? "inserted" : "ejected");
    if (!floppyop.path[0]) strcpy (buffer[i], "none");
    SIM->get_disk_options (i, &diskop);
    sprintf (buffer[2+i], "%s, %d cylinders, %d heads, %d sectors/track", 
	diskop.path, diskop.cylinders, diskop.heads, diskop.spt);
    if (!diskop.present) strcpy (buffer[2+i], "none");
  }
  SIM->get_cdrom_options (0, &cdromop);
  sprintf (buffer[4], "%s, %spresent, %s",
     cdromop.dev, cdromop.present?"":"not ",
     cdromop.inserted?"inserted":"ejected");
  if (!cdromop.dev[0]) strcpy (buffer[4], "none");
  sprintf (buffer[5], "%s", SIM->get_newhd_support () ? "yes":"no");
  sprintf (buffer[6], "%s", SIM->get_boot_hard_disk () ? "hard drive":"floppy drive");
  // check if diskd and cdromd are on at once
  SIM->get_disk_options (1, &diskop);
  int conflict = (diskop.present && cdromop.present);
  char *diskd_cdromd_conflict_msg = "\nERROR:\nThis configuration has both a cdrom and a hard disk enabled.\nYou cannot have both!";
  snprintf (buf, size, format, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], conflict? diskd_cdromd_conflict_msg : "");
}

void build_runtime_options_prompt (char *format, char *buf, int size)
{
  bx_floppy_options floppyop;
  bx_cdrom_options cdromop;
  bx_param_num_c *ips = SIM->get_param_num (BXP_IPS);
  char buffer[3][128];
  for (int i=0; i<2; i++) {
    SIM->get_floppy_options (i, &floppyop);
    sprintf (buffer[i], "%s, size=%s, %s", floppyop.path,
	  SIM->get_floppy_type_name (floppyop.type),
	  floppyop.initial_status ? "inserted" : "ejected");
    if (!floppyop.path[0]) strcpy (buffer[i], "none");
  }
  SIM->get_cdrom_options (0, &cdromop);
  sprintf (buffer[2], "%s, %spresent, %s",
     cdromop.dev, cdromop.present?"":"not ",
     cdromop.inserted?"inserted":"ejected");
  snprintf (buf, size, format, buffer[0], buffer[1], buffer[2], 
      ips->get (),
      SIM->get_param_num (BXP_VGA_UPDATE_INTERVAL)->get (), 
      SIM->get_param_num (BXP_MOUSE_ENABLED)->get () ? "enabled" : "disabled");
}

// return value of bx_control_panel:
//   -1: error while reading, like if stdin closed
//    0: no error
//   BX_DISABLE_CONTROL_PANEL: returned from BX_CPANEL_START_MAIN if 
//       user chooses to revert to the normal way of running without the
//       control panel.
int bx_control_panel (int menu)
{
 int choice;
 while (1) {
  switch (menu)
  {
   case BX_CPANEL_START_MAIN:
     bx_control_panel_init ();
     {
     if (ask_yn (ask_about_control_panel, 1, &choice) < 0) return -1;
     if (choice == 0) return BX_DISABLE_CONTROL_PANEL;
     else return bx_control_panel (BX_CPANEL_START_MENU);
     }
   case BX_CPANEL_START_MENU:
     {
       static int read_rc = 0;
       int default_choice = 1;
       default_choice = read_rc ? 4 : 1;
       if (ask_int (startup_menu_prompt, 1, 5, default_choice, &choice) < 0) return -1;
       switch (choice) {
	 case 1: if (bx_read_rc (NULL) >= 0) read_rc=1; break;
	 case 2: bx_control_panel (BX_CPANEL_START_OPTS); break;
	 case 3: bx_write_rc (NULL); break;
	 case 4: return 0;   // return from menu
	 case 5: SIM->quit_sim (1); return -1;
	 default: BAD_OPTION(menu, choice);
       }
     }
     break;
   case BX_CPANEL_START_OPTS:
     {
       char prompt[CPANEL_PATH_LEN];
       char oldpath[CPANEL_PATH_LEN];
       assert (SIM->get_log_file (oldpath, CPANEL_PATH_LEN) >= 0);
       sprintf (prompt, startup_options_prompt, oldpath);
       if (ask_int (prompt, 0, 8, 0, &choice) < 0) return -1;
       switch (choice) {
	 case 0: return 0;
	 case 1: bx_log_file (); break;
	 case 2: bx_log_options (0); break;
	 case 3: bx_log_options (1); break;
	 case 4: bx_control_panel (BX_CPANEL_START_OPTS_MEM); break;
	 case 5: bx_control_panel (BX_CPANEL_START_OPTS_INTERFACE); break;
	 case 6: bx_control_panel (BX_CPANEL_START_OPTS_DISK); break;
	 case 7: bx_control_panel (BX_CPANEL_START_OPTS_SOUND); break;
	 case 8: bx_control_panel (BX_CPANEL_START_OPTS_MISC); break;
	 default: BAD_OPTION(menu, choice);
       }
     }
     break;
   case BX_CPANEL_START_OPTS_MEM:
     {
       char prompt[CPANEL_PATH_LEN], vgapath[CPANEL_PATH_LEN], rompath[CPANEL_PATH_LEN];
       bx_param_num_c *memsize = SIM->get_param_num (BXP_MEM_SIZE);
       bx_param_num_c *rom_addr = SIM->get_param_num (BXP_ROM_ADDRESS);
       bx_param_string_c *rom_path = SIM->get_param_string (BXP_ROM_PATH);
       bx_param_string_c *vga_rom_path = SIM->get_param_string (BXP_VGA_ROM_PATH);
       if (rom_path->get (rompath, CPANEL_PATH_LEN) < 0)
	 strcpy (rompath, "none");
       if (vga_rom_path->get (vgapath, CPANEL_PATH_LEN) < 0)
	 strcpy (vgapath, "none");
       sprintf (prompt, startup_mem_options_prompt, 
          memsize->get (),
	  vgapath, rompath,
	  rom_addr->get ());
       if (ask_int (prompt, 0, 4, 0, &choice) < 0) return -1;
       switch (choice) {
	 case 0: return 0;
	 case 1: bx_edit_mem (); break;
	 case 2: bx_edit_rom_path (1); break;
	 case 3: bx_edit_rom_path (0); break;
	 case 4: bx_edit_rom_addr (); break;
	 default: BAD_OPTION(menu, choice);
       }
     }
     break;
   case BX_CPANEL_START_OPTS_INTERFACE:
     {
       char prompt[1024];
       int interval = SIM->get_param_num (BXP_VGA_UPDATE_INTERVAL)->get ();
       bx_param_num_c *ips = SIM->get_param_num (BXP_IPS);
       sprintf (prompt, startup_interface_options, 
	 interval, 
	 SIM->get_param_num (BXP_MOUSE_ENABLED)->get () ? "enabled" : "disabled",
	 ips->get (),
	 SIM->get_private_colormap () ? "enabled" : "disabled");
       if (ask_int (prompt, 0, 4, 0, &choice) < 0) return -1;
       switch (choice) {
	 case 0: return 0;
	 case 1: bx_vga_update_interval (); break;
	 case 2: bx_mouse_enable (); break;
	 case 3: bx_ips_change (); break;
	 case 4: bx_private_colormap (); break;
	 default: BAD_OPTION(menu, choice);
       }
     }
     break;
   case BX_CPANEL_START_OPTS_DISK:
     {
       char prompt[1024];
       build_disk_options_prompt (startup_disk_options_prompt, prompt, 1024);
       if (ask_int (prompt, 0, 7, 0, &choice) < 0) return -1;
       switch (choice) {
	 case 0: return 0;
	 case 1: bx_edit_floppy (0); break;
	 case 2: bx_edit_floppy (1); break;
	 case 3: bx_edit_hard_disk (0); break;
	 case 4: bx_edit_hard_disk (1); break;
	 case 5: bx_edit_cdrom (); break;
	 case 6: bx_newhd_support (); break;
	 case 7: bx_boot_from (); break;
	 default: BAD_OPTION(menu, choice);
       }
     }
   break;
   case BX_CPANEL_RUNTIME:
     char prompt[1024];
     build_runtime_options_prompt (runtime_menu_prompt, prompt, 1024);
     if (ask_int (prompt, 1, 11, 10, &choice) < 0) return -1;
     switch (choice) {
       case 1: bx_edit_floppy (0); break;
       case 2: bx_edit_floppy (1); break;
       case 3: bx_edit_cdrom (); break;
       case 4: bx_ips_change (); break;
       case 5: bx_log_options (0); break;
       case 6: bx_log_options (1); break;
       case 7: bx_vga_update_interval (); break;
       case 8: bx_mouse_enable (); break;
       case 9: NOT_IMPLEMENTED (choice); break;
       case 10: fprintf (stderr, "Continuing simulation\n"); return 0;
       case 11:
	 fprintf (stderr, "You chose quit on the control panel.\n");
	 SIM->quit_sim (1);
	 return -1;
       default: fprintf (stderr, "Menu choice %d not implemented.\n", choice);
     }
     break;
   default:
     assert (menu >=0 && menu < BX_CPANEL_N_MENUS);
     fprintf (stderr, "--THIS IS A SAMPLE MENU, NO OPTIONS ARE IMPLEMENTED EXCEPT #0--\n");
     if (ask_int (menu_prompt_list[menu], 0, 99, 0, &choice) < 0) return -1;
     if (choice == 0) return 0;
     fprintf (stderr, "This is a sample menu.  Option %d is not implemented.\n", choice);
  }
 }
}

void bx_edit_floppy (int drive)
{
  bx_floppy_options opt, newopt;
  assert (SIM->get_floppy_options (drive, &opt) >= 0);
  newopt = opt;
  fprintf (stderr, "Changing options for floppy drive %d\n", drive);
  if (ask_string ("Enter new pathname: [%s] ", opt.path, newopt.path) < 0)
    return;
  int newtype, oldtype = opt.type - BX_FLOPPY_NONE;
  if (ask_menu ("What is the floppy disk size?\nChoices are 720K, 1.2M, 1.44M, 2.88M.  [%s] ", n_floppy_type_names, floppy_type_names, oldtype, &newtype) < 0) return;
  newopt.type = newtype + BX_FLOPPY_NONE;
  if (SIM->set_floppy_options (drive, &newopt) < 0) {
    fprintf (stderr, "The disk image %s could not be opened.\n", newopt.path);
  }
}

void bx_edit_hard_disk (int drive)
{
  bx_disk_options opt, newopt;
  assert (SIM->get_disk_options (drive, &opt) >= 0);
  newopt = opt;
  fprintf (stderr, "Changing options for hard drive %d\n", drive);
  if (ask_string ("Enter new pathname, or type 'none' for no disk: [%s] ", opt.path, newopt.path) < 0)
    return;
  newopt.present = (strcmp (newopt.path, "none") != 0);
  if (newopt.present) {  // skip if "none" is the path.
    // ask cyl, head, sec.
    int n;
    if (ask_int ("How many cylinders? [%d] ", 1, 65535, opt.cylinders, &n) < 0)
      return;
    newopt.cylinders = n;
    if (ask_int ("How many heads? [%d] ", 1, 256, opt.heads, &n) < 0)
      return;
    newopt.heads = n;
    if (ask_int ("How many sectors per track? [%d] ", 1, 255, opt.spt, &n) < 0)
      return;
    newopt.spt = n;
  }
  if (SIM->set_disk_options (drive, &newopt) < 0) {
    fprintf (stderr, "The disk image %s could not be opened.\n", newopt.path);
  }
}

void bx_edit_cdrom ()
{
  bx_cdrom_options opt, newopt;
  assert (SIM->get_cdrom_options (0, &opt) >= 0);
  newopt = opt;
  newopt.present = 1;
  if (ask_string ("Enter pathname of the cdrom device, or 'none' for no cdrom: [%s] ", opt.dev, newopt.dev) < 0)
    return;
  if (!strcmp (newopt.dev, "none")) {
    newopt.dev[0] = 0;
    newopt.present = 0;
  }
  if (SIM->set_cdrom_options (0, &newopt) < 0) {
    fprintf (stderr, "The device at %s could not be opened.\n", newopt.dev);
  }
}

void bx_edit_rom_path (int vga)
{
  char oldpath[CPANEL_PATH_LEN], newpath[CPANEL_PATH_LEN];
  if (vga) {
    bx_param_string_c *vga_path = SIM->get_param_string (BXP_VGA_ROM_PATH);
    if (vga_path->get (oldpath, CPANEL_PATH_LEN) < 0) return;
    if (ask_string ("Enter pathname of the VGA ROM image: [%s] ", oldpath, newpath) < 0)
      return;
    vga_path->set (newpath);
  } else {
    bx_param_string_c *rom_path = SIM->get_param_string (BXP_ROM_PATH);
    if (rom_path->get (oldpath, CPANEL_PATH_LEN) < 0) return;
    if (ask_string ("Enter pathname of the ROM image: [%s] ", oldpath, newpath) < 0)
      return;
    rom_path->set (newpath);
  }
}

void bx_newhd_support ()
{
  int newval, oldval = SIM->get_newhd_support ();
  if (ask_yn ("Use new hard disk support (recommended)? [%s] ", oldval, &newval) < 0) return;
  if (newval == oldval) return;
  SIM->set_newhd_support (newval);
}

void bx_private_colormap ()
{
  int newval, oldval = SIM->get_private_colormap ();
  if (ask_yn ("Use private colormap? [%s] ", oldval, &newval) < 0) return;
  if (newval == oldval) return;
  SIM->set_private_colormap (newval);
}


void bx_boot_from ()
{
  int newval, oldval = SIM->get_boot_hard_disk ();
  char *choices[] = {"fd","hd"};
  if (ask_menu ("Boot floppy disk or hard disk?  Type hd or fd. [%s] ",
	 2, choices, oldval, &newval) < 0) return;
  SIM->set_boot_hard_disk (newval);
}

void bx_edit_mem ()
{
  bx_param_num_c *memsize = SIM->get_param_num (BXP_MEM_SIZE);
  int newval, oldval = memsize->get ();
  if (ask_int ("How much memory (megabytes) in the simulated machine? [%d] ", 1, 1<<30, oldval, &newval) < 0)
    return;
  memsize->set (newval);
}

void bx_ips_change ()
{
  char prompt[1024];
  bx_param_num_c *ips = SIM->get_param_num (BXP_IPS);
  int oldips = ips->get ();
  sprintf (prompt, "Type a new value for ips: [%d] ", oldips);
  int newips;
  if (ask_int (prompt, 1, 1<<30, oldips, &newips) < 0)
    return;
  ips->set (newips);
}

void bx_vga_update_interval ()
{
  char prompt[1024];
  bx_param_num_c *pinterval = SIM->get_param_num (BXP_VGA_UPDATE_INTERVAL);
  int old = pinterval->get ();
  sprintf (prompt, "Type a new value for VGA update interval: [%d] ", old);
  int newinterval;
  if (ask_int (prompt, 1, 1<<30, old, &newinterval) < 0)
    return;
  pinterval->set (newinterval);
}

static void bx_print_log_action_table ()
{
  // just try to print all the prefixes first.
  fprintf (stderr, "Current log settings:\n");
  fprintf (stderr, "                 Debug      Info       Error       Panic\n");
  fprintf (stderr, "ID    Device     Action     Action     Action      Action\n");
  fprintf (stderr, "----  ---------  ---------  ---------  ----------  ----------\n");
  int i, j, imax=SIM->get_n_log_modules ();
  for (i=0; i<imax; i++) {
    fprintf (stderr, "%3d.  %s ", i, SIM->get_prefix (i));
    for (j=0; j<SIM->get_max_log_level (); j++) {
      fprintf (stderr, "%10s ", SIM->get_action_name (SIM->get_log_action (i, j)));
    }
    fprintf (stderr, "\n");
  }
}

static char *log_options_prompt1 = "Enter the ID of the device to edit, or -1 to return: [-1] ";
static char *log_level_choices[] = { "ignore", "report", "ask", "fatal", "no change" };
static int log_level_n_choices_normal = 4;

void bx_log_options (int individual)
{
  if (individual) {
    int done = 0;
    while (!done) {
      bx_print_log_action_table ();
      int id, level, action;
      int maxid = SIM->get_n_log_modules ();
      if (ask_int (log_options_prompt1, -1, maxid-1, -1, &id) < 0)
	return;
      if (id < 0) return;
      fprintf (stderr, "Editing log options for the device %s\n", SIM->get_prefix (id));
      for (level=0; level<SIM->get_max_log_level (); level++) {
	char prompt[1024];
	int default_action = SIM->get_log_action (id, level);
	sprintf (prompt, "Enter action for %s event: [%s] ", SIM->get_log_level_name (level), SIM->get_action_name(default_action));
	// don't show the no change choice (choices=3)
	if (ask_menu (prompt, log_level_n_choices_normal, log_level_choices, default_action, &action)<0)
	  return;
	SIM->set_log_action (id, level, action);
      }
    }
  } else {
    // provide an easy way to set log options for all devices at once
    bx_print_log_action_table ();
    for (int level=0; level<SIM->get_max_log_level (); level++) {
      char prompt[1024];
      int action, default_action = 3;  // default to no change
      sprintf (prompt, "Enter action for %s event on all devices: [no change] ", SIM->get_log_level_name (level));
	// do show the no change choice (choices=4)
      if (ask_menu (prompt, log_level_n_choices_normal+1, log_level_choices, default_action, &action)<0)
	return;
      if (action < 3) {
	for (int i=0; i<SIM->get_n_log_modules (); i++)
          SIM->set_log_action (i, level, action);
      }
    }
  }
}

void bx_mouse_enable ()
{
  bx_param_num_c *mouse_en = SIM->get_param_num (BXP_MOUSE_ENABLED);
  int newval, oldval = mouse_en->get ();
  if (ask_yn ("Enable the mouse? [%s] ", oldval, &newval) < 0) return;
  if (newval == oldval) return;
  mouse_en->set (newval);
}

int bx_read_rc (char *rc)
{
  if (rc && SIM->read_rc (rc) >= 0) return 0;
  char oldrc[CPANEL_PATH_LEN];
  if (SIM->get_default_rc (oldrc, CPANEL_PATH_LEN) < 0)
    strcpy (oldrc, "none");
  char newrc[CPANEL_PATH_LEN];
  while (1) {
    if (ask_string ("\nWhat is the configuration file name?\nTo cancel, type 'none'. [%s] ", oldrc, newrc) < 0) return -1;
    if (!strcmp (newrc, "none")) return 0;
    if (SIM->read_rc (newrc) >= 0) return 0;
    fprintf (stderr, "The file '%s' could not be found.\n", newrc);
  }
}

int bx_write_rc (char *rc)
{
  char oldrc[CPANEL_PATH_LEN], newrc[CPANEL_PATH_LEN];
  if (rc == NULL) {
    if (SIM->get_default_rc (oldrc, CPANEL_PATH_LEN) < 0)
      strcpy (oldrc, "none");
  } else {
    strncpy (oldrc, rc, CPANEL_PATH_LEN);
  }
  while (1) {
    if (ask_string ("Save configuration to what file?  To cancel, type 'none'.\n[%s] ", oldrc, newrc) < 0) return -1;
    if (!strcmp (newrc, "none")) return 0;
    // try with overwrite off first
    int status = SIM->write_rc (newrc, 0);
    if (status >= 0) {
      fprintf (stderr, "Wrote configuration to '%s'.\n", newrc);
      return 0;
    } else if (status == -2) {
      // return code -2 indicates the file already exists, and overwrite
      // confirmation is required.
      int overwrite = 0;
      char prompt[256];
      sprintf (prompt, "Configuration file '%s' already exists.  Overwrite it? [no] ", newrc);
      if (ask_yn (prompt, 0, &overwrite) < 0) return -1;
      if (!overwrite) continue;  // if "no", start loop over, asking for a different file
      // they confirmed, so try again with overwrite bit set
      if (SIM->write_rc (newrc, 1) >= 0) {
	fprintf (stderr, "Overwriting existing configuration '%s'.\n", newrc);
	return 0;
      } else {
	fprintf (stderr, "Write failed to '%s'.\n", newrc);
      }
    }
  }
}

void bx_log_file ()
{
  char oldpath[CPANEL_PATH_LEN], newpath[CPANEL_PATH_LEN];
  assert (SIM->get_log_file (oldpath, CPANEL_PATH_LEN) >= 0);
  if (ask_string ("Enter log file name: [%s] ", oldpath, newpath) < 0) return;
  SIM->set_log_file (newpath);
}

void bx_edit_rom_addr ()
{
  char oldval[256], newval[256];
  bx_param_num_c *rom_addr = SIM->get_param_num (BXP_ROM_ADDRESS);
  sprintf (oldval, "%05x", rom_addr->get ());
  while (1) {
    if (ask_string ("Enter the ROM BIOS address: [%s] ", oldval, newval) < 0)
      return;
    int val;
    if (1==sscanf (newval, "%x", &val)) {
      rom_addr->set (val);
      return;
    }
    fprintf (stderr, "The value should be in hex, as in 'e0000'.\n");
  }
}

/*
A panic has occurred.  Do you want to:
  cont       - continue execution
  alwayscont - continue execution, and do not ask again
  die        - stop execution now
*/

char *log_action_ask_choices[] = { "cont", "alwayscont", "die" };
int log_action_n_choices = 3;

int control_panel_notify_callback (int code)
{
  switch (code)
  {
  case NOTIFY_CODE_LOGMSG:
    {
      int level;
      char prefix[512], msg[512];
      assert (SIM->log_msg_2 (prefix, &level, msg, sizeof(msg)) >= 0);
      fprintf (stderr, "========================================================================\n");
      fprintf (stderr, "Event type: %s\n", SIM->get_log_level_name (level));
      fprintf (stderr, "Device: %s\n", prefix);
      fprintf (stderr, "Message: %s\n\n", msg);
      fprintf (stderr, "A %s has occurred.  Do you want to:\n", SIM->get_log_level_name (level));
      fprintf (stderr, "  cont       - continue execution\n");
      fprintf (stderr, "  alwayscont - continue execution, and don't ask again.\n");
      fprintf (stderr, "               This affects only %s events from device %s\n", SIM->get_log_level_name (level), prefix);
      fprintf (stderr, "  die        - stop execution now\n");
      int choice;
      if (ask_menu ("Choose cont, alwayscont, or die. [%s] ", 3, 
	    log_action_ask_choices, 2, &choice) < 0) 
	return SIM->notify_return(-1);
      // return 0 for continue, 1 for alwayscontinue, 2 for die.
      SIM->notify_return(choice);
    }
    break;
  default:
    fprintf (stderr, "Control panel: notify callback called with unknown code %04x\n", code);
  }
  // error if we fall through the case
  return -1;
}

void bx_control_panel_init () {
  //fprintf (stderr, "bx_control_panel_init()\n");
  SIM->set_notify_callback (control_panel_notify_callback);
}


#endif
