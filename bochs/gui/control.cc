/*
 * gui/control.cc
 * $Id: control.cc,v 1.3 2001-06-09 20:01:12 bdenney Exp $
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

extern "C" {
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
}
#include "control.h"
#include "siminterface.h"

#define BX_PANIC(x) printf x

/* functions for changing particular options */
void bx_edit_floppy (int drive);
void bx_edit_hard_disk (int drive);
void bx_edit_cdrom ();
void bx_newhd_support ();
void bx_boot_from ();
void bx_ips_change ();
int bx_read_rc (char *rc);
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
"\n\
This version of Bochs has a prototype configuration interface.  Would\n\
you like to try it?\n\
\n\
If you choose yes, you can use a menu to choose configuration options.\n\
If you choose no, Bochs will read a bochsrc file and run as usual.\n\
Type yes or no: [yes] ";

static char *startup_menu_prompt =
"------------------\n\
Bochs Startup Menu\n\
------------------\n\
1. %s%s%s\n\
2. Read options from...\n\
3. Edit options\n\
4. Save options to...\n\
5. Begin simulation\n\
6. Quit now\n\
\n\
Please choose one: [%d] ";

static char *startup_options_prompt =
"------------------\n\
Bochs Options Menu\n\
------------------\n\
0. Return to previous menu\n\
1. Log file: %s\n\
2. Log options for all devices\n\
3. Log options for individual devices\n\
4. Boot options\n\
5. Interface options\n\
6. Disk options\n\
7. Sound options\n\
8. Other options\n\
\n\
Please choose one: [0] ";

static char *startup_boot_options_prompt =
"------------------\n\
Bochs Boot Options\n\
------------------\n\
0. Return to previous menu\n\
1. Memory in Megabytes: %d\n\
2. VGA ROM image: %s\n\
3. ROM image: %s\n\
4. ROM address: 0x%05x\n\
\n\
Please choose one: [0] ";

static char *startup_interface_options =
"------------------\n\
Bochs Interface Options\n\
------------------\n\
0. Return to previous menu\n\
1. VGA Update Interval: 300000\n\
2. Mouse: enabled\n\
3. Emulated instructions per second (IPS): 1000000\n\
4. Private Colormap: enabled=0\n\
\n\
Please choose one: [0] ";

static char *startup_disk_options_prompt =
"------------------\n\
Bochs Disk Options\n\
------------------\n\
0. Return to previous menu\n\
1. Floppy disk 0: %s\n\
2. Floppy disk 1: %s\n\
3. Hard disk 0: %s\n\
4. Hard disk 1: %s\n\
5. CDROM: %s\n\
6. New Hard Drive Support: %s\n\
7. Boot from: %s\n\
\n\
Please choose one: [0] ";

static char *startup_sound_options_prompt =
"------------------\n\
Bochs Sound Options\n\
------------------\n\
0. Return to previous menu\n\
1. Sound Blaster 16: disabled\n\
2. MIDI mode: 1, \n\
3. MIDI output file: /dev/midi00\n\
4. Wave mode: 1\n\
5. Wave output file: dev/dsp\n\
6. SB16 log level: 2\n\
7. SB16 log file: sb16.log\n\
8. DMA Timer: 600000\n\
\n\
Please choose one: [0] ";

static char *startup_misc_options_prompt =
"---------------------------\n\
Bochs Miscellaneous Options\n\
---------------------------\n\
1. Keyboard Serial Delay: 250\n\
2. Floppy command delay: 500\n\
\n\
Please choose one: [0] ";

static char *runtime_menu_prompt =
"---------------------\n\
Bochs Runtime Options\n\
---------------------\n\
1. Floppy disk 0 (example:a.img, 1.44MB, inserted)\n\
2. Floppy disk 1 (example:b.img, 1.44MB, inserted)\n\
3. Emulated instructions per second (IPS)\n\
4. Logging options\n\
5. VGA Update Interval\n\
6. Mouse: enabled\n\
7. Instruction tracing: off (doesn't exist yet)\n\
8. Continue simulation\n\
9. Quit now\n\
\n\
Please choose one:  [8] ";

char *menu_prompt_list[BX_CPANEL_N_MENUS] = {
  ask_about_control_panel,
  startup_menu_prompt,
  startup_options_prompt,
  startup_boot_options_prompt,
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
  char buffer[6][128];
  for (int i=0; i<2; i++) {
    SIM->get_floppy_options (i, &floppyop);
    sprintf (buffer[i], "%s, size=%s, %s", floppyop.path,
	  SIM->get_floppy_type_name (floppyop.type),
	  floppyop.initial_status ? "inserted" : "ejected");
    if (!floppyop.path[0]) strcpy (buffer[i], "none");
    SIM->get_disk_options (i, &diskop);
    sprintf (buffer[2+i], "%s, %d cylinders, %d heads, %d sectors/track", 
	diskop.path, diskop.cylinders, diskop.heads, diskop.spt);
    if (!diskop.path[0]) strcpy (buffer[2+i], "none");
  }
  SIM->get_cdrom_options (0, &cdromop);
  sprintf (buffer[4], "%s, %spresent, %s",
     cdromop.dev, cdromop.present?"":"not ",
     cdromop.inserted?"inserted":"ejected");
  if (!cdromop.dev[0]) strcpy (buffer[4], "none");
  sprintf (buffer[5], "%s", SIM->get_newhd_support () ? "yes":"no");
  sprintf (buffer[6], "%s", SIM->get_boot_hard_disk () ? "hard drive":"floppy drive");
  snprintf (buf, size, format, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);
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
     {
     if (ask_yn (ask_about_control_panel, 1, &choice) < 0) return -1;
     if (choice == 0) return BX_DISABLE_CONTROL_PANEL;
     else return bx_control_panel (BX_CPANEL_START_MENU);
     }
   case BX_CPANEL_START_MENU:
     {
       char rc[512], prompt[512];
       char *choice_disabled = "Choice 1 not allowed because the default bochsrc file was not found.";
       static int read_rc = 0;
       int notfound = 0;
       int default_choice = 1;
       if (SIM->get_default_rc (rc, 512) >= 0) {
	 default_choice = read_rc ? 5 : 1;
         sprintf (prompt, startup_menu_prompt, "Read options from ", rc, "", default_choice);
       } else {
	 default_choice = read_rc ? 5 : 2;
         sprintf (prompt, startup_menu_prompt, "DISABLED because a bochsrc file was not found", "", "", default_choice);
	 notfound = 1;
       }
       if (ask_int (prompt, 1, 6, default_choice, &choice) < 0) return -1;
       switch (choice) {
	 case 1: if (notfound) fprintf (stderr, "%s\n", choice_disabled);
		 else if (bx_read_rc (rc) >= 0) read_rc=1;
		 break;
	 case 2: if (bx_read_rc (NULL) >= 0) read_rc=1; break;
	 case 3: bx_control_panel (BX_CPANEL_START_OPTS); break;
	 case 4: NOT_IMPLEMENTED (choice); break;
	 case 5: return 0;   // return from menu
	 case 6: SIM->quit_sim (1); return -1;
	 default: BAD_OPTION(menu, choice);
       }
     }
     break;
   case BX_CPANEL_START_OPTS:
     {
       char prompt[512];
       char oldpath[512];
       assert (SIM->get_log_file (oldpath, 512) >= 0);
       sprintf (prompt, startup_options_prompt, oldpath);
       if (ask_int (prompt, 0, 7, 0, &choice) < 0) return -1;
       switch (choice) {
	 case 0: return 0;
	 case 1: bx_log_file (); break;
	 case 2: bx_log_options (0); break;
	 case 3: bx_log_options (1); break;
	 case 4: bx_control_panel (BX_CPANEL_START_OPTS_BOOT); break;
	 case 5: bx_control_panel (BX_CPANEL_START_OPTS_INTERFACE); break;
	 case 6: bx_control_panel (BX_CPANEL_START_OPTS_DISK); break;
	 case 7: bx_control_panel (BX_CPANEL_START_OPTS_SOUND); break;
	 case 8: bx_control_panel (BX_CPANEL_START_OPTS_MISC); break;
	 default: BAD_OPTION(menu, choice);
       }
     }
     break;
   case BX_CPANEL_START_OPTS_BOOT:
     {
       char prompt[512];
       build_disk_options_prompt (startup_boot_options_prompt, prompt, 512);
       if (ask_int (prompt, 0, 4, 0, &choice) < 0) return -1;
       switch (choice) {
	 case 0: return 0;
	 case 1: NOT_IMPLEMENTED (choice); break;
	 case 2: NOT_IMPLEMENTED (choice); break;
	 case 3: NOT_IMPLEMENTED (choice); break;
	 case 4: NOT_IMPLEMENTED (choice); break;
	 default: BAD_OPTION(menu, choice);
       }
     }
     break;
   case BX_CPANEL_START_OPTS_DISK:
     {
       char prompt[512];
       build_disk_options_prompt (startup_disk_options_prompt, prompt, 512);
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
     if (ask_int (runtime_menu_prompt, 1, 9, 8, &choice) < 0) return -1;
     switch (choice) {
       case 1: bx_edit_floppy (0); break;
       case 2: bx_edit_floppy (1); break;
       case 3: bx_ips_change (); break;
       case 4: bx_log_options (1); break;
       case 5: bx_vga_update_interval (); break;
       case 6: bx_mouse_enable (); break;
       case 7: NOT_IMPLEMENTED (choice); break;
       case 8: fprintf (stderr, "Continuing simulation\n"); return 0;
       case 9:
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
  if (!strcmp (newopt.path, "none")) newopt.path[0] = 0;
  if (newopt.path[0]) {  // skip if "none" is the path.
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

void bx_newhd_support ()
{
  int newval, oldval = SIM->get_newhd_support ();
  if (ask_yn ("Use new hard disk support (recommended)? [%s] ", oldval, &newval) < 0) return;
  if (newval == oldval) return;
  SIM->set_newhd_support (newval);
}

void bx_boot_from ()
{
  int newval, oldval = SIM->get_boot_hard_disk ();
  char *choices[] = {"fd","hd"};
  if (ask_menu ("Boot floppy disk or hard disk?  Type hd or fd. [%s] ",
	 2, choices, oldval, &newval) < 0) return;
  SIM->set_boot_hard_disk (newval);
}

void bx_ips_change ()
{
  char prompt[1024];
  int oldips = SIM->getips ();
  sprintf (prompt, "Type a new value for ips: [%d] ", oldips);
  int newips;
  if (ask_int (prompt, 1, 1<<30, oldips, &newips) < 0)
    return;
  SIM->setips (newips);
}

void bx_vga_update_interval ()
{
  char prompt[1024];
  int old = SIM->get_vga_update_interval ();
  sprintf (prompt, "Type a new value for VGA update interval: [%d] ", old);
  int newinterval;
  if (ask_int (prompt, 1, 1<<30, old, &newinterval) < 0)
    return;
  SIM->set_vga_update_interval (newinterval);
}

static void bx_print_log_action_table ()
{
  // just try to print all the prefixes first.
  fprintf (stderr, "Current log settings:\n");
  fprintf (stderr, "                 Debug      Info       Error       Panic\n");
  fprintf (stderr, "ID    Device     Action     Action     Action      Action\n");
  fprintf (stderr, "----  ---------  ---------  ---------  ----------  ----------\n");
  int i, imax=SIM->get_n_log_modules ();
  for (int i=0; i<imax; i++) {
    fprintf (stderr, "%3d.  %s ", i, SIM->get_prefix (i));
    for (int j=0; j<SIM->get_max_log_level (); j++) {
      fprintf (stderr, "%10s ", SIM->get_action_name (SIM->get_log_action (i, j)));
    }
    fprintf (stderr, "\n");
  }
}

static char *log_options_prompt1 = "Enter the ID of the device to edit, or -1 to return: [-1] ";
static char *log_level_choices[] = { "ignore", "report", "fatal", "no change" };

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
	if (ask_menu (prompt, 3, log_level_choices, default_action, &action)<0)
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
      if (ask_menu (prompt, 4, log_level_choices, default_action, &action)<0)
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
  int newval, oldval = SIM->get_mouse_enabled ();
  if (ask_yn ("Enable the mouse? [%s] ", oldval, &newval) < 0) return;
  if (newval == oldval) return;
  SIM->set_mouse_enabled (newval);
}

int bx_read_rc (char *rc)
{
  if (rc && SIM->read_rc (rc) >= 0) return 0;
  char oldrc[512];
  if (SIM->get_default_rc (oldrc, 512) < 0)
    oldrc[0] = 0;
  char newrc[512];
  while (1) {
    if (ask_string ("\nWhat is the configuration file name?  If there is no config file\ntype none.  [%s] ", oldrc, newrc) < 0) return -1;
    if (!strcmp (newrc, "none")) return 0;
    if (SIM->read_rc (newrc) >= 0) return 0;
    fprintf (stderr, "The file '%s' could not be found.\n", newrc);
  }
}

void bx_log_file ()
{
  char oldpath[512], newpath[512];
  assert (SIM->get_log_file (oldpath, 512) >= 0);
  if (ask_string ("Enter log file name: [%s] ", oldpath, newpath) < 0) return;
  SIM->set_log_file (newpath);
}
