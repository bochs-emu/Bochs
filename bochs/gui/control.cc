/*
 * gui/control.cc
 * $Id: control.cc,v 1.2 2001-06-08 07:20:07 bdenney Exp $
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
say we need to implement prototypes and type checking.
*/

extern "C" {
#include <stdio.h>
#include <ctype.h>
#include <string.h>
}
#include "siminterface.h"

#define BX_PANIC(x) printf x

void bx_floppy_change ();
void bx_ips_change ();
void bx_log_options ();

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
    printf ("%s", prompt);
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
    printf ("%s", prompt);
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
    printf ("%s", prompt);
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
  printf ("%s", prompt);
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

/*
What should I be able to change?
- floppy disk image name
- ips value
- debug for each device.
- vga update interval
- mouse enabled
- keyboard serial delay
- floppy command delay
- start tracing instructions?
- simulate a particular keystroke
- continue simulation
*/

  static char *main_menu_prompt =
"\n\
-------------------------------\n\
Bochs Control Panel (text mode)\n\
-------------------------------\n\
 1. floppy disk image name \n\
 2. ips value \n\
 3. set logging options \n\
 4. vga update interval \n\
 5. mouse enabled \n\
 6. keyboard serial delay \n\
 7. floppy command delay \n\
 8. start tracing instructions? \n\
 9. simulate a particular keystroke \n\
10. Continue simulation \n\
11. Quit simulation\n\
\n\
Please choose one:  [10] ";

void bx_control_panel_main ()
{
  int choice;
  while (1) {
    if (ask_int (main_menu_prompt, 1, 11, 10, &choice) < 0) return;
    switch (choice)
    {
      case 1:
	bx_floppy_change ();
	break;
      case 2:
	bx_ips_change ();
	break;
      case 3:
	bx_log_options ();
	break;
      case 10:
	fprintf (stderr, "Continuing simulation\n");
	return;
      case 11:
	fprintf (stderr, "You chose quit on the control panel.\n");
	SIM->quit_sim ();
	break;
      default:
	fprintf (stderr, "Menu choice %d not implemented.\n", choice);
    }
  }
}


void bx_floppy_change ()
{
  int drive;
  if (ask_int ("Change floppy 0 or 1?  [0] ", 0, 1, 0, &drive) < 0) 
    return;
  char oldpath[1024], newpath[1024];
  SIM->get_floppy_path (drive, oldpath, 1024);
  char prompt[1024];
  sprintf (prompt, "Enter new pathname:  [%s] ", oldpath);
  if (ask_string (prompt, oldpath, newpath) < 0)
    return;
  fprintf (stderr, "Changing floppy %d to new path %s\n", drive, newpath);
  SIM->set_floppy_options (drive, 0, newpath);
  SIM->set_floppy_options (drive, 1, newpath);
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

static void bx_print_log_action_table ()
{
  // just try to print all the prefixes first.
  fprintf (stderr, "Current log settings:\n");
  fprintf (stderr, "                 Debug      Info       Error       Panic\n");
  fprintf (stderr, "ID    Device     Action     Action     Action      Action\n");
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
static char *log_level_choices[] = { "ignore", "report", "fatal" };

void bx_log_options ()
{
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
      if (ask_menu (prompt, 3, log_level_choices, default_action, &action)<0)
	return;
      SIM->set_log_action (id, level, action);
    }
  }
}
