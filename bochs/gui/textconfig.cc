/////////////////////////////////////////////////////////////////////////
// $Id: textconfig.cc,v 1.56 2006-05-31 20:12:43 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
// This is code for a text-mode configuration interface.  Note that this file
// does NOT include bochs.h.  Instead, it does all of its contact with
// the simulator through an object called SIM, defined in siminterface.cc
// and siminterface.h.  This separation adds an extra layer of method
// calls before any work can be done, but the benefit is that the compiler
// enforces the rules.  I can guarantee that textconfig.cc doesn't call any
// I/O device objects directly, for example, because the bx_devices symbol
// isn't even defined in this context.
//

#include "config.h"

#if BX_USE_TEXTCONFIG

#ifndef __QNXNTO__
extern "C" {
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#ifndef __QNXNTO__
}
#endif

#include "osdep.h"
#include "textconfig.h"
#include "siminterface.h"
#include "extplugin.h"
#ifdef WIN32
#include "win32dialog.h"
#endif

#define CI_PATH_LENGTH 512

#define BX_INSERTED 11

/* functions for changing particular options */
void bx_config_interface_init();
int bx_read_rc(char *rc);
int bx_write_rc(char *rc);
void bx_log_options(int individual);

/******************************************************************/
/* lots of code stolen from bximage.c */
/* remove leading spaces, newline junk at end.  returns pointer to 
 cleaned string, which is between s0 and the null */
char *
clean_string(char *s0)
{
  char *s = s0;
  char *ptr;
  /* find first nonblank */
  while (isspace(*s))
    s++;
  /* truncate string at first non-alphanumeric */
  ptr = s;
  while (isprint(*ptr))
    ptr++;
  *ptr = 0;
  return s;
}

/* returns 0 on success, -1 on failure.  The value goes into out. */
int 
ask_uint(char *prompt, Bit32u min, Bit32u max, Bit32u the_default, Bit32u *out, int base)
{
  Bit32u n = max + 1;
  char buffer[1024];
  char *clean;
  int illegal;
  assert(base==10 || base==16);
  while (1) {
    printf(prompt, the_default);
    if (!fgets(buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string(buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    const char *format = (base==10) ? "%d" : "%x";
    illegal = (1 != sscanf(buffer, format, &n));
    if (illegal || n<min || n>max) {
      printf("Your choice (%s) was not an integer between %u and %u.\n\n",
	  clean, min, max);
    } else {
      // choice is okay
      *out = n;
      return 0;
    }
  }
}

// identical to ask_uint, but uses signed comparisons
int 
ask_int(char *prompt, Bit32s min, Bit32s max, Bit32s the_default, Bit32s *out)
{
  int n = max + 1;
  char buffer[1024];
  char *clean;
  int illegal;
  while (1) {
    printf(prompt, the_default);
    if (!fgets(buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string(buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    illegal = (1 != sscanf(buffer, "%d", &n));
    if (illegal || n<min || n>max) {
      printf("Your choice (%s) was not an integer between %d and %d.\n\n",
	  clean, min, max);
    } else {
      // choice is okay
      *out = n;
      return 0;
    }
  }
}

int 
ask_menu(char *prompt, int n_choices, char *choice[], int the_default, int *out)
{
  char buffer[1024];
  char *clean;
  int i;
  *out = -1;
  while (1) {
    printf(prompt, choice[the_default]);
    if (!fgets(buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string(buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    for (i=0; i<n_choices; i++) {
      if (!strcmp(choice[i], clean)) {
	// matched, return the choice number
	*out = i;
	return 0;
      }
    }
    if (clean[0] != '?')
      printf("Your choice (%s) did not match any of the choices:\n", clean);
    for (i=0; i<n_choices; i++) {
      if (i>0) printf (", ");
      printf("%s", choice[i]);
    }
    printf("\n");
  }
}

int 
ask_yn(char *prompt, Bit32u the_default, Bit32u *out)
{
  char buffer[16];
  char *clean;
  *out = 1<<31;
  while (1) {
    // if there's a %s field, substitute in the default yes/no.
    printf(prompt, the_default ? "yes" : "no");
    if (!fgets(buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string(buffer);
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

// returns -1 on error (stream closed or  something)
// returns 0 if default was taken
// returns 1 if value changed
int ask_string(char *prompt, const char *the_default, char *out)
{
  char buffer[1024];
  char *clean;
  assert(the_default != out);
  out[0] = 0;
  printf(prompt, the_default);
  if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    return -1;
  clean = clean_string(buffer);
  if (strlen(clean) < 1) {
    // empty line, use the default
    strcpy(out, the_default);
    return 0;
  }
  strcpy(out, clean);
  return 1;
}

/******************************************************************/

static char *startup_menu_prompt =
"------------------------------\n"
"Bochs Configuration: Main Menu\n"
"------------------------------\n"
"\n"
"This is the Bochs Configuration Interface, where you can describe the\n"
"machine that you want to simulate.  Bochs has already searched for a\n"
"configuration file (typically called bochsrc.txt) and loaded it if it\n"
"could be found.  When you are satisfied with the configuration, go\n"
"ahead and start the simulation.\n"
"\n"
"You can also start bochs with the -q option to skip these menus.\n"
"\n"
"1. Restore factory default configuration\n"
"2. Read options from...\n"
"3. Edit options\n"
"4. Save options to...\n"
#if BX_SUPPORT_SAVE_RESTORE
"5. Restore the Bochs state from...\n"
"6. Begin simulation\n"
"7. Quit now\n"
#else
"5. Begin simulation\n"
"6. Quit now\n"
#endif
"\n"
"Please choose one: [%d] ";

static char *startup_options_prompt =
"------------------\n"
"Bochs Options Menu\n"
"------------------\n"
"0. Return to previous menu\n"
"1. Logfile options\n"
"2. Log options for all devices\n"
"3. Log options for individual devices\n"
"4. CPU options\n"
"5. Memory options\n"
"6. Clock & CMOS options\n"
"7. PCI options\n"
"8. Bochs Display & Interface options\n"
"9. Keyboard & Mouse options\n"
"10. Disk options\n"
"11. Serial & Parallel port options\n"
"12. Network card options\n"
"13. Sound Blaster 16 options\n"
"14. Other options\n"
"\n"
"Please choose one: [0] ";

#ifndef WIN32
static char *runtime_menu_prompt =
"---------------------\n"
"Bochs Runtime Options\n"
"---------------------\n"
"1. Floppy disk 0: %s\n"
"2. Floppy disk 1: %s\n"
"3. 1st CDROM: %s\n"
"4. 2nd CDROM: %s\n"
"5. 3rd CDROM: %s\n"
"6. 4th CDROM: %s\n"
"7. (not implemented)\n"
"8. Log options for all devices\n"
"9. Log options for individual devices\n"
"10. Instruction tracing: off (doesn't exist yet)\n"
"11. Misc runtime options\n"
#if BX_SUPPORT_SAVE_RESTORE
"12. Save the Bochs state to directory and quit...\n"
"13. Continue simulation\n"
"14. Quit now\n"
"\n"
"Please choose one:  [13] ";
#else
"12. Continue simulation\n"
"13. Quit now\n"
"\n"
"Please choose one:  [12] ";
#endif
#endif

#define NOT_IMPLEMENTED(choice) \
  fprintf (stderr, "ERROR: choice %d not implemented\n", choice);

#define BAD_OPTION(menu,choice) \
  do {fprintf (stderr, "ERROR: menu %d has no choice %d\n", menu, choice); \
      assert (0); } while (0)

#ifndef WIN32
void build_runtime_options_prompt(char *format, char *buf, int size)
{
  bx_list_c *floppyop;
  bx_list_c *cdromop = NULL;
  char pname[80];
  char buffer[6][128];

  for (int i=0; i<2; i++) {
    sprintf(pname, "floppy.%d", i);
    floppyop = (bx_list_c*) SIM->get_param(pname);
    if (SIM->get_param_enum("devtype", floppyop)->get() == BX_FLOPPY_NONE)
      strcpy(buffer[i], "(not present)");
    else {
      sprintf(buffer[i], "%s, size=%s, %s", SIM->get_param_string("path", floppyop)->getptr(),
        SIM->get_param_enum("type", floppyop)->get_selected(),
        SIM->get_param_enum("status", floppyop)->get_selected());
      if (!SIM->get_param_string("path", floppyop)->getptr()[0]) strcpy(buffer[i], "none");
    }
  }

  // 4 cdroms supported at run time
  int device;
  for (Bit8u cdrom=0; cdrom<4; cdrom++) {
    if (!SIM->get_cdrom_options(cdrom, &cdromop, &device) || !SIM->get_param_bool("present", cdromop)->get())
      sprintf (buffer[2+cdrom], "(not present)");
    else
      sprintf (buffer[2+cdrom], "(%s on ata%d) %s, %s",
        device&1?"slave":"master", device/2, SIM->get_param_string("path", cdromop)->getptr(),
        (SIM->get_param_enum("status", cdromop)->get () == BX_INSERTED)? "inserted" : "ejected");
    }

  snprintf(buf, size, format, buffer[0], buffer[1], buffer[2],
           buffer[3], buffer[4], buffer[5]);
}
#endif

int do_menu(const char *pname)
{
  bx_list_c *menu = (bx_list_c *)SIM->get_param(pname, NULL);
  while (1) {
    menu->get_choice()->set(0);
    int status = menu->text_ask(stdin, stderr);
    if (status < 0) return status;
    bx_param_num_c *choice = menu->get_choice();
    if (choice->get() < 1)
      return choice->get();
    else {
      int index = choice->get() - 1;  // choosing 1 means list[0]
      bx_param_c *chosen = menu->get(index);
      assert(chosen != NULL);
      if (chosen->get_enabled()) {
        if (chosen->get_type() == BXT_LIST) {
          char chosen_pname[80];
          chosen->get_param_path(chosen_pname, 80);
          do_menu(chosen_pname);
        } else {
          chosen->text_ask(stdin, stderr);
        }
      }
    }
  }
}

void askparam(char *pname)
{
  bx_param_c *param = SIM->get_param(pname);
  param->text_ask(stdin, stderr);
}

int bx_config_interface(int menu)
{
  Bit32u choice;
#if BX_SUPPORT_SAVE_RESTORE
  char sr_path[CI_PATH_LENGTH];
#endif
  while (1) {
    switch (menu) {
      case BX_CI_INIT:
        bx_config_interface_init();
        return 0;
      case BX_CI_START_SIMULATION:
        SIM->begin_simulation(bx_startup_flags.argc, bx_startup_flags.argv);
        // we don't expect it to return, but if it does, quit
        SIM->quit_sim(1);
        break;
      case BX_CI_START_MENU:
        {
          Bit32u default_choice;
          switch (SIM->get_param_enum(BXPN_BOCHS_START)->get()) {
            case BX_LOAD_START: 
              default_choice = 2; break;
            case BX_EDIT_START: 
              default_choice = 3; break;
            default: 
#if BX_SUPPORT_SAVE_RESTORE
              default_choice = 6; break;
#else
              default_choice = 5; break;
#endif
          }
#if BX_SUPPORT_SAVE_RESTORE
          if (ask_uint(startup_menu_prompt, 1, 7, default_choice, &choice, 10) < 0) return -1;
#else
          if (ask_uint(startup_menu_prompt, 1, 6, default_choice, &choice, 10) < 0) return -1;
#endif
          switch (choice) {
            case 1:
              fprintf(stderr, "I reset all options back to their factory defaults.\n\n");
              SIM->reset_all_param();
              SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_EDIT_START);
              break;
            case 2: 
              // Before reading a new configuration, reset every option to its
              // original state.
              SIM->reset_all_param();
              if (bx_read_rc(NULL) >= 0)
                SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_RUN_START);
              break;
            case 3: 
              bx_config_interface(BX_CI_START_OPTS); 
              SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_RUN_START);
              break;
            case 4: bx_write_rc(NULL); break;
#if BX_SUPPORT_SAVE_RESTORE
            case 5:
              if (ask_string("\nWhat is the path to restore the Bochs state from?\nTo cancel, type 'none'. [%s] ", "none", sr_path) >= 0) {
                if (strcmp (sr_path, "none")) {
                  SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(1);
                  SIM->get_param_string(BXPN_RESTORE_PATH)->set(sr_path);
                  bx_config_interface(BX_CI_START_SIMULATION);
                }
              }
              break;
            case 6: bx_config_interface(BX_CI_START_SIMULATION); break;
            case 7: SIM->quit_sim(1); return -1;
#else
            case 5: bx_config_interface(BX_CI_START_SIMULATION); break;
            case 6: SIM->quit_sim(1); return -1;
#endif
            default: BAD_OPTION(menu, choice);
          }
        }
        break;
      case BX_CI_START_OPTS:
        if (ask_uint(startup_options_prompt, 0, 14, 0, &choice, 10) < 0) return -1;
        switch (choice) {
          case 0: return 0;
          case 1: do_menu("log"); break;
          case 2: bx_log_options(0); break;
          case 3: bx_log_options(1); break;
          case 4: do_menu("cpu"); break;
          case 5: do_menu(BXPN_MENU_MEMORY); break;
          case 6: do_menu("clock_cmos"); break;
          case 7: do_menu("pci"); break;
          case 8: do_menu("display"); break;
          case 9: do_menu("keyboard_mouse"); break;
          case 10: do_menu(BXPN_MENU_DISK); break;
          case 11: do_menu("ports"); break;
          case 12: do_menu("network"); break;
          case 13: do_menu(BXPN_SB16); break;
          case 14: do_menu("misc"); break;
          default: BAD_OPTION(menu, choice);
        }
        break;
      case BX_CI_RUNTIME:
        {
          bx_list_c *cdromop = NULL;
          char pname[80];
#ifdef WIN32
          choice = RuntimeOptionsDialog();
#else
          char prompt[1024];
          build_runtime_options_prompt(runtime_menu_prompt, prompt, 1024);
          if (ask_uint(prompt, 1, BX_CI_RT_QUIT, BX_CI_RT_CONT, &choice, 10) < 0) return -1;
#endif
          switch (choice) {
            case BX_CI_RT_FLOPPYA: 
              if (SIM->get_param_enum(BXPN_FLOPPYA_DEVTYPE)->get() != BX_FLOPPY_NONE) do_menu(BXPN_FLOPPYA);
              break;
            case BX_CI_RT_FLOPPYB:
              if (SIM->get_param_enum(BXPN_FLOPPYB_DEVTYPE)->get() != BX_FLOPPY_NONE) do_menu(BXPN_FLOPPYB);
              break;
            case BX_CI_RT_CDROM1:
            case BX_CI_RT_CDROM2:
            case BX_CI_RT_CDROM3:
            case BX_CI_RT_CDROM4:
              int device;
              if (SIM->get_cdrom_options(choice - BX_CI_RT_CDROM1, &cdromop, &device) && SIM->get_param_bool("present", cdromop)->get()) {
                // disable type selection
                SIM->get_param("type", cdromop)->set_enabled(0);
                SIM->get_param("model", cdromop)->set_enabled(0);
                SIM->get_param("biosdetect", cdromop)->set_enabled(0);
                cdromop->get_param_path(pname, 80);
                do_menu(pname);
              }
              break;
            case BX_CI_RT_IPS:
              // not implemented yet because I would have to mess with
              // resetting timers and pits and everything on the fly.
              // askparam(BXPN_IPS);
              break;
            case BX_CI_RT_LOGOPTS1: bx_log_options(0); break;
            case BX_CI_RT_LOGOPTS2: bx_log_options(1); break;
            case BX_CI_RT_INST_TR: NOT_IMPLEMENTED(choice); break;
            case BX_CI_RT_MISC: do_menu(BXPN_MENU_RUNTIME); break;
            case BX_CI_RT_CONT: fprintf(stderr, "Continuing simulation\n"); return 0;
            case BX_CI_RT_QUIT:
              fprintf(stderr, "You chose quit on the configuration interface.\n");
              bx_user_quit = 1;
              SIM->quit_sim(1);
              return -1;
#if BX_SUPPORT_SAVE_RESTORE
            case BX_CI_RT_SAVE:
              if (ask_string("\nWhat is the path to save the Bochs state to?\nTo cancel, type 'none'. [%s] ", "none", sr_path) >= 0) {
                if (strcmp (sr_path, "none")) {
                  if (SIM->save_state(sr_path)) {
                    Bit32u cont = 0;
                    ask_yn("\nThe save function currently doesn't handle the state of hard drive images,\n"
                           "so we don't recommend to continue, unless you are running a read-only\n"
                           "guest system (e.g. Live-CD).\n\n"
                           "Do you want to continue? [no]", 0, &cont);
                    if (!cont) {
                      bx_user_quit = 1;
                      SIM->quit_sim(1);
                      return -1;
                    }
                  }
                }
              }
              break;
#endif
            default: fprintf(stderr, "Menu choice %d not implemented.\n", choice);
          }
        }
        break;
      default:
        fprintf (stderr, "Unknown config interface menu type.\n");
        assert (menu >=0 && menu < BX_CI_N_MENUS);
    }
  }
}

static void bx_print_log_action_table()
{
  // just try to print all the prefixes first.
  fprintf (stderr, "Current log settings:\n");
  fprintf (stderr, "                 Debug      Info       Error       Panic       Pass\n");
  fprintf (stderr, "ID    Device     Action     Action     Action      Action      Action\n");
  fprintf (stderr, "----  ---------  ---------  ---------  ----------  ----------  ----------\n");
  int i, j, imax=SIM->get_n_log_modules ();
  for (i=0; i<imax; i++) {
    if (strcmp(SIM->get_prefix(i), "[     ]")) {
      fprintf (stderr, "%3d.  %s ", i, SIM->get_prefix (i));
      for (j=0; j<SIM->get_max_log_level (); j++) {
        fprintf (stderr, "%10s ", SIM->get_action_name (SIM->get_log_action (i, j)));
      }
      fprintf (stderr, "\n");
    }
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
      Bit32s id, level, action;
      Bit32s maxid = SIM->get_n_log_modules ();
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
	SIM->set_default_log_action (level, action);
	SIM->set_log_action (-1, level, action);
      }
    }
  }
}

int bx_read_rc (char *rc)
{
  if (rc && SIM->read_rc (rc) >= 0) return 0;
  char oldrc[CI_PATH_LENGTH];
  if (SIM->get_default_rc (oldrc, CI_PATH_LENGTH) < 0)
    strcpy (oldrc, "none");
  char newrc[CI_PATH_LENGTH];
  while (1) {
    if (ask_string ("\nWhat is the configuration file name?\nTo cancel, type 'none'. [%s] ", oldrc, newrc) < 0) return -1;
    if (!strcmp (newrc, "none")) return -1;
    if (SIM->read_rc (newrc) >= 0) return 0;
    fprintf (stderr, "The file '%s' could not be found.\n", newrc);
  }
}

int bx_write_rc (char *rc)
{
  char oldrc[CI_PATH_LENGTH], newrc[CI_PATH_LENGTH];
  if (rc == NULL) {
    if (SIM->get_default_rc (oldrc, CI_PATH_LENGTH) < 0)
      strcpy (oldrc, "none");
  } else {
    strncpy (oldrc, rc, CI_PATH_LENGTH);
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
      Bit32u overwrite = 0;
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

char *log_action_ask_choices[] = { "cont", "alwayscont", "die", "abort", "debug" };
int log_action_n_choices = 4 + (BX_DEBUGGER||BX_GDBSTUB?1:0);

BxEvent *
config_interface_notify_callback (void *unused, BxEvent *event)
{
  event->retcode = -1;
  switch (event->type)
  {
    case BX_SYNC_EVT_TICK:
      event->retcode = 0;
      return event;
    case BX_SYNC_EVT_ASK_PARAM:
      event->retcode = event->u.param.param->text_ask(stdin, stderr);
      return event;
    case BX_SYNC_EVT_LOG_ASK:
    {
      int level = event->u.logmsg.level;
      fprintf (stderr, "========================================================================\n");
      fprintf (stderr, "Event type: %s\n", SIM->get_log_level_name (level));
      fprintf (stderr, "Device: %s\n", event->u.logmsg.prefix);
      fprintf (stderr, "Message: %s\n\n", event->u.logmsg.msg);
      fprintf (stderr, "A %s has occurred.  Do you want to:\n", SIM->get_log_level_name (level));
      fprintf (stderr, "  cont       - continue execution\n");
      fprintf (stderr, "  alwayscont - continue execution, and don't ask again.\n");
      fprintf (stderr, "               This affects only %s events from device %s\n", SIM->get_log_level_name (level), event->u.logmsg.prefix);
      fprintf (stderr, "  die        - stop execution now\n");
      fprintf (stderr, "  abort      - dump core %s\n", 
	  BX_HAVE_ABORT ? "" : "(Disabled)");
#if BX_DEBUGGER
      fprintf (stderr, "  debug      - continue and return to bochs debugger\n");
#endif
#if BX_GDBSTUB
      fprintf (stderr, "  debug      - hand control to gdb\n");
#endif

      int choice;
ask:
      if (ask_menu ("Choose one of the actions above: [%s] ", 
	    log_action_n_choices, log_action_ask_choices, 2, &choice) < 0) 
	event->retcode = -1;
      // return 0 for continue, 1 for alwayscontinue, 2 for die, 3 for debug.
      if (!BX_HAVE_ABORT && choice==BX_LOG_ASK_CHOICE_DUMP_CORE) goto ask;
      fflush(stdout);
      fflush(stderr);
      event->retcode = choice;
    }
    return event;
  case BX_ASYNC_EVT_REFRESH:
  case BX_ASYNC_EVT_DBG_MSG:
    // The text mode interface does not use these events, so just ignore
    // them.
    return event;
  default:
    fprintf (stderr, "Control panel: notify callback called with event type %04x\n", event->type);
    return event;
  }
  assert (0); // switch statement should return
}

void bx_config_interface_init () {
  SIM->set_notify_callback (config_interface_notify_callback, NULL);
}

/////////////////////////////////////////////////////////////////////
// implement the text_* methods for bx_param types.

void
bx_param_num_c::text_print (FILE *fp)
{
  if (get_long_format()) {
    fprintf(fp, get_long_format(), get());
  } else {
    char *format = "%s: %d"; 
    assert (base==10 || base==16);
    if (base==16) format = "%s: 0x%x";
    if (get_label()) {
      fprintf(fp, format, get_label(), get());
    } else {
      fprintf(fp, format, get_name(), get());
    }
  }
}

void
bx_param_bool_c::text_print (FILE *fp)
{
  if (get_format()) {
    fprintf(fp, get_format(), get() ? "yes" : "no");
  } else {
    char *format = "%s: %s"; 
    if (get_label()) {
      fprintf(fp, format, get_label(), get() ? "yes" : "no");
    } else {
      fprintf(fp, format, get_name(), get() ? "yes" : "no");
    }
  }
}

void
bx_param_enum_c::text_print (FILE *fp)
{
  int n = get();
  assert (n >= min && n <= max);
  char *choice = choices[n - min];
  if (get_format()) {
    fprintf(fp, get_format(), choice);
  } else {
    char *format = "%s: %s"; 
    if (get_label()) {
      fprintf(fp, format, get_label(), choice);
    } else {
      fprintf(fp, format, get_name(), choice);
    }
  }
}

void
bx_param_string_c::text_print (FILE *fp)
{
  char *value = getptr ();
  int opts = options->get ();
  if (opts & RAW_BYTES) {
    char buffer[1024];
    buffer[0] = 0;
    char sep_string[2];
    sep_string[0] = separator;
    sep_string[1] = 0;
    for (int i=0; i<maxsize; i++) {
      char eachbyte[16];
      sprintf (eachbyte, "%s%02x", (i>0)?sep_string : "", (unsigned int)0xff&val[i]);
      strncat (buffer, eachbyte, sizeof(buffer));
    }
    if (strlen (buffer) > sizeof(buffer)-4) {
      assert (0); // raw byte print buffer is probably overflowing. increase the max or make it dynamic
    }
    value = buffer;
  }
  if (get_format()) {
    fprintf(fp, get_format(), value);
  } else {
    if (get_label()) {
      fprintf(fp, "%s: %s", get_label(), value);
    } else {
      fprintf(fp, "%s: %s", get_name(), value);
    }
  }
}

void
bx_list_c::text_print (FILE *fp)
{
  fprintf (fp, "%s: ", get_name ());
  for (int i=0; i<size; i++) {
    assert (list[i] != NULL);
    if (list[i]->get_enabled ()) {
      if ((i>0) && (options->get () & SERIES_ASK))
        fprintf (fp, ", ");
      list[i]->text_print (fp);
      if (!(options->get () & SERIES_ASK))
        fprintf (fp, "\n");
    }
  }
}

int 
bx_param_num_c::text_ask (FILE *fpin, FILE *fpout)
{
  fprintf (fpout, "\n");
  int status;
  char *prompt = get_ask_format ();
  if (prompt == NULL) {
    // default prompt, if they didn't set an ask format string
    text_print (fpout);
    fprintf (fpout, "\n");
    prompt = "Enter new value: [%d] ";
    if (base==16)
      prompt = "Enter new value in hex: [%x] ";
  }
  Bit32u n = get ();
  status = ask_uint (prompt, (Bit32u)min, (Bit32u)max, n, &n, base);
  if (status < 0) return status;
  set (n);
  return 0;
}

int 
bx_param_bool_c::text_ask (FILE *fpin, FILE *fpout)
{
  fprintf (fpout, "\n");
  int status;
  char *prompt = get_ask_format();
  char buffer[512];
  if (prompt == NULL) {
    if (get_label() != NULL) {
      sprintf(buffer, "%s? [%%s] ", get_label());
      prompt = buffer;
    } else {
      // default prompt, if they didn't set an ask format or label string
      sprintf(buffer, "%s? [%%s] ", get_name());
      prompt = buffer;
    }
  }
  Bit32u n = get();
  status = ask_yn(prompt, n, &n);
  if (status < 0) return status;
  set(n);
  return 0;
}

int 
bx_param_enum_c::text_ask (FILE *fpin, FILE *fpout)
{
  fprintf (fpout, "\n");
  char *prompt = get_ask_format ();
  if (prompt == NULL) {
    // default prompt, if they didn't set an ask format string
    fprintf (fpout, "%s = ", get_name ());
    text_print (fpout);
    fprintf (fpout, "\n");
    prompt = "Enter new value: [%s] ";
  }
  Bit32s n = (Bit32s)(get () - min);
  int status = ask_menu (prompt, (Bit32u)(max-min+1), choices, n, &n);
  if (status < 0) return status;
  n += (Bit32s)min;
  set (n);
  return 0;
}

int parse_raw_bytes (char *dest, char *src, int destsize, char separator)
{
  //printf ("parsing src='%s'\n", src);
  int i;
  unsigned int n;
  for (i=0; i<destsize; i++) 
    dest[i] = 0;
  for (i=0; i<destsize; i++) {
    while (*src == separator)
      src++;
    if (*src == 0) break;
    // try to read a byte of hex
    if (sscanf (src, "%02x", &n) == 1) {
      dest[i] = n;
      src+=2;
    } else {
      return -1;
    }
  }
  return 0;
}

int 
bx_param_string_c::text_ask (FILE *fpin, FILE *fpout)
{
  fprintf (fpout, "\n");
  int status;
  char *prompt = get_ask_format ();
  if (prompt == NULL) {
    // default prompt, if they didn't set an ask format string
    text_print (fpout);
    fprintf (fpout, "\n");
    prompt = "Enter a new value, or press return for no change.\n";
  }
  while (1) {
    char buffer[1024];
    status = ask_string (prompt, getptr(), buffer);
    if (status < 0) return status;
    int opts = options->get ();
    char buffer2[1024];
    strcpy (buffer2, buffer);
    if (status == 1 && opts & RAW_BYTES) {
      // copy raw hex into buffer
      status = parse_raw_bytes (buffer, buffer2, maxsize, separator);
      if (status < 0) {
	fprintf (fpout, "Illegal raw byte format.  I expected something like 3A%c03%c12%c...\n", separator, separator, separator);
	continue;
      }
    }
    if (!equals (buffer)) 
      set (buffer);
    return 0;
  }
}

int bx_list_c::text_ask(FILE *fpin, FILE *fpout)
{
  bx_list_c *child;

  char *my_title = title->getptr();
  fprintf (fpout, "\n");
  int i, imax = strlen (my_title);
  for (i=0; i<imax; i++) fprintf (fpout, "-");
  fprintf (fpout, "\n%s\n", my_title);
  for (i=0; i<imax; i++) fprintf (fpout, "-");
  fprintf(fpout, "\n");
  if (options->get () & SERIES_ASK) {
    for (int i=0; i<size; i++) {
      if (list[i]->get_enabled()) {
        if (!SIM->get_init_done() || list[i]->get_runtime_param()) {
          list[i]->text_ask (fpin, fpout);
        }
      }
    }
  } else {
    if (options->get () & SHOW_PARENT)
      fprintf (fpout, "0. Return to previous menu\n");
    for (int i=0; i<size; i++) {
      assert (list[i] != NULL);
      fprintf(fpout, "%d. ", i+1);
      if (list[i]->get_enabled()) {
        if (list[i]->get_type() == BXT_LIST) {
          child = (bx_list_c*)list[i];
          fprintf(fpout, "%s\n", child->get_title()->getptr());
        } else {
          if ((options->get() & SHOW_GROUP_NAME) && (list[i]->get_group() != NULL))
            fprintf(fpout, "%s ", list[i]->get_group());
          list[i]->text_print(fpout);
          fprintf(fpout, "\n");
        }
      } else {
        if (list[i]->get_type() == BXT_LIST) {
          child = (bx_list_c*)list[i];
          fprintf(fpout, "%s (disabled)\n", child->get_title()->getptr());
        } else {
          fprintf(fpout, "(disabled)\n");
        }
      }
    }
    fprintf (fpout, "\n");
    Bit32u n = choice->get();
    int min = (options->get() & SHOW_PARENT) ? 0 : 1;
    int max = size;
    int status = ask_uint("Please choose one: [%d] ", min, max, n, &n, 10);
    if (status < 0) return status;
    choice->set(n);
  }
  return 0;
}

static int ci_callback(void *userdata, ci_command_t command)
{
  switch (command)
  {
    case CI_START:
      bx_config_interface_init();
      if (SIM->get_param_enum(BXPN_BOCHS_START)->get() == BX_QUICK_START)
	bx_config_interface(BX_CI_START_SIMULATION);
      else {
        if (!SIM->test_for_text_console())
	  return CI_ERR_NO_TEXT_CONSOLE;
        bx_config_interface(BX_CI_START_MENU);
      }
      break;
    case CI_RUNTIME_CONFIG:
      bx_config_interface(BX_CI_RUNTIME);
      break;
    case CI_SHUTDOWN:
      break;
  }
  return 0;
}

// if I can make things compile without this module linked in, then
// this file can become a plugin too.
int init_text_config_interface ()
{
  //fprintf (stderr, "plugin_init for textconfig.cc\n");
  SIM->register_configuration_interface ("textconfig", ci_callback, NULL);
  return 0;  // success
}

#endif
