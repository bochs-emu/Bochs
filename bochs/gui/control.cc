#include "bochs.h"

#define LOG_THIS genlog->


void bx_control_panel_entry ()
{
  bx_control_panel_main ();
}


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
	BX_PANIC(("You chose quit on the control panel"));
	exit(1);
	break;
      default:
	fprintf (stderr, "Menu choice %d not implemented.\n", choice);
    }
  }
}


void bx_floppy_change ()
{
  int which;
  if (ask_int ("Change floppy 0 or 1?  [0] ", 0, 1, 0, &which) < 0) 
    return;
  char oldpath[1024], newpath[1024];
  strcpy (oldpath, (which)? bx_options.floppyb.path : bx_options.floppya.path);
  char prompt[1024];
  sprintf (prompt, "Enter new pathname:  [%s] ", oldpath);
  if (ask_string (prompt, oldpath, newpath) < 0)
    return;
  fprintf (stderr, "Changing floppy %d to new path %s\n", which, newpath);
  strcpy ((which)? bx_options.floppyb.path : bx_options.floppya.path, newpath);
  
  // notify
  bx_devices.floppy->set_media_status (which, 0);
  bx_devices.floppy->set_media_status (which, 1);
}

void bx_ips_change ()
{
  char prompt[1024];
  sprintf (prompt, "Type a new value for ips: [%d] ", bx_options.ips);
  int newips;
  if (ask_int (prompt, 1, 1<<30, bx_options.ips, &newips) < 0)
    return;
  bx_options.ips = newips;
}

static void bx_print_log_action_table ()
{
  // just try to print all the prefixes first.
  fprintf (stderr, "Current log settings:\n");
  fprintf (stderr, "                 Debug      Info       Error       Panic\n");
  fprintf (stderr, "ID    Device     Action     Action     Action      Action\n");
  for (int i=0; i<io->get_n_logfns (); i++) {
    logfunc_t *logfn = io->get_logfn (i);
    fprintf (stderr, "%3d.  %s ", i, logfn->getprefix ());
    for (int j=0; j<MAX_LOGLEV; j++) {
      fprintf (stderr, "%10s ", io->getaction (logfn->getonoff(j)));
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
    int maxid = io->get_n_logfns ();
    if (ask_int (log_options_prompt1, -1, maxid-1, -1, &id) < 0)
      return;
    if (id < 0) return;
    logfunc_t *logfn = io->get_logfn (id);
    fprintf (stderr, "Editing log options for the device %s\n", logfn->getprefix ());
    for (level=0; level<MAX_LOGLEV; level++) {
      char prompt[1024];
      int default_action = logfn->getonoff (level);
      sprintf (prompt, "Enter action for %s event: [%s] ", io->getlevel(level), io->getaction (default_action));
      if (ask_menu (prompt, 3, log_level_choices, default_action, &action)<0)
	return;
      logfn->setonoff (level, action);
    }
  }
}
