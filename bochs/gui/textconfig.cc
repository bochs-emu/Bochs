/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2023  The Bochs Project
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
//
/////////////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

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
#include "bx_debug/debug.h"
#include "param_names.h"
#include "logio.h"
#include "paramtree.h"
#include "siminterface.h"
#include "plugin.h"

#define CI_PATH_LENGTH 512

#if BX_USE_GUI_CONSOLE
#define bx_printf SIM->bx_printf
#define bx_fgets  SIM->bx_gets
#else
#define bx_printf printf
#define bx_fgets  fgets
#endif

enum {
  BX_CI_START_MENU,
  BX_CI_START_OPTS,
  BX_CI_START_SIMULATION,
  BX_CI_RUNTIME,
  BX_CI_N_MENUS
};

enum {
  BX_CI_RT_FLOPPYA = 1,
  BX_CI_RT_FLOPPYB,
  BX_CI_RT_CDROM,
  BX_CI_RT_LOGOPTS1,
  BX_CI_RT_LOGOPTS2,
  BX_CI_RT_USB,
  BX_CI_RT_MISC,
  BX_CI_RT_SAVE_CFG,
  BX_CI_RT_CONT,
  BX_CI_RT_QUIT
};

static int text_ci_callback(void *userdata, ci_command_t command);
static BxEvent* textconfig_notify_callback(void *unused, BxEvent *event);

PLUGIN_ENTRY_FOR_MODULE(textconfig)
{
  if (mode == PLUGIN_INIT) {
    SIM->register_configuration_interface("textconfig", text_ci_callback, NULL);
    SIM->set_notify_callback(textconfig_notify_callback, NULL);
  } else if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_CI;
  }
  return 0; // Success
}

/* functions for changing particular options */
int bx_read_rc(char *rc);
int bx_write_rc(char *rc);
void bx_plugin_ctrl();
void bx_log_options(int individual);
int bx_atexit();
#if BX_DEBUGGER
void bx_dbg_exit(int code);
#endif
int text_ask(bx_param_c *param);

/******************************************************************/
/* lots of code stolen from bximage.c */
/* remove leading spaces, newline junk at end.  returns pointer to
 cleaned string, which is between s0 and the null */
char *clean_string(char *s0)
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
int ask_uint(const char *prompt, const char *help, Bit32u min, Bit32u max, Bit32u the_default, Bit32u *out, int base)
{
  Bit32u n = max + 1;
  char buffer[1024];
  char *clean;
  int illegal;
  assert(base==10 || base==16);
  while (1) {
    bx_printf(prompt, the_default);
    fflush(stdout);
    if (!bx_fgets(buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string(buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    if ((clean[0] == '?') && (strlen(help) > 0)) {
      bx_printf("\n%s\n", help);
      if (base == 10) {
        bx_printf("Your choice must be an integer between %u and %u.\n\n", min, max);
      } else {
        bx_printf("Your choice must be an integer between 0x%x and 0x%x.\n\n", min, max);
      }
      continue;
    }
    const char *format = (base==10) ? "%d" : "%x";
    illegal = (1 != sscanf(buffer, format, &n));
    if (illegal || n<min || n>max) {
      if (base == 10) {
        bx_printf("Your choice (%s) was not an integer between %u and %u.\n\n",
               clean, min, max);
      } else {
        bx_printf("Your choice (%s) was not an integer between 0x%x and 0x%x.\n\n",
               clean, min, max);
      }
    } else {
      // choice is okay
      *out = n;
      return 0;
    }
  }
}

// Identical to ask_uint but uses 64 bits signed integers
int ask_int(const char *prompt, const char *help, Bit64s min, Bit64s max, Bit64s the_default, Bit64s *out)
{
  Bit64s n = max + 1;
  char buffer[1024];
  char *clean;
  int illegal;
  while (1) {
    bx_printf(prompt, the_default);
    fflush(stdout);
    if (!bx_fgets(buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string(buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    if ((clean[0] == '?') && (strlen(help) > 0)) {
      bx_printf("\n%s\n", help);
      bx_printf("Your choice must be an integer between " FMT_LL "d and " FMT_LL "d.\n\n", min, max);
      continue;
    }
    illegal = (1 != sscanf(buffer, "%ld", &n));
    if (illegal || n<min || n>max) {
      bx_printf("Your choice (%s) was not an integer between " FMT_LL "d and " FMT_LL "d.\n\n",
             clean, min, max);
    } else {
      // choice is okay
      *out = n;
      return 0;
    }
  }
}

int ask_menu(const char *prompt, const char *help, int n_choices, const char *choice[], int the_default, int *out)
{
  char buffer[1024];
  char *clean;
  int i;
  *out = -1;
  while (1) {
    bx_printf(prompt, choice[the_default]);
    fflush(stdout);
    if (!bx_fgets(buffer, sizeof(buffer), stdin))
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
    if (clean[0] != '?') {
      bx_printf("Your choice (%s) did not match any of the choices:\n", clean);
    } else if (strlen(help) > 0) {
      bx_printf("\n%s\nValid values are: ", help);
    }
    for (i=0; i<n_choices; i++) {
      if (i>0) bx_printf(", ");
      bx_printf("%s", choice[i]);
    }
    bx_printf("\n");
  }
}

int ask_yn(const char *prompt, const char *help, Bit32u the_default, Bit32u *out)
{
  char buffer[16];
  char *clean;
  *out = 1<<31;
  while (1) {
    // if there's a %s field, substitute in the default yes/no.
    bx_printf(prompt, the_default ? "yes" : "no");
    fflush(stdout);
    if (!bx_fgets(buffer, sizeof(buffer), stdin))
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
      case '?':
        if (strlen(help) > 0) {
          bx_printf("\n%s\n", help);
        }
        break;
    }
    bx_printf("Please type either yes or no.\n");
  }
}

// returns -1 on error (stream closed or  something)
// returns 0 if default was taken
// returns 1 if value changed
// returns -2 if help requested
int ask_string(const char *prompt, const char *the_default, char *out)
{
  char buffer[1024];
  char *clean;
  assert(the_default != out);
  out[0] = 0;
  bx_printf(prompt, the_default);
  fflush(stdout);
  if (bx_fgets(buffer, sizeof(buffer), stdin) == NULL)
    return -1;
  clean = clean_string(buffer);
  if (clean[0] == '?')
    return -2;
  if (strlen(clean) < 1) {
    // empty line, use the default
    strcpy(out, the_default);
    return 0;
  }
  strcpy(out, clean);
  return 1;
}

/******************************************************************/

static const char *startup_menu_prompt =
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
"5. Restore the Bochs state from...\n"
"6. Begin simulation\n"
"7. Quit now\n"
"\n"
"Please choose one: [%d] ";

static const char *startup_options_prompt =
"------------------\n"
"Bochs Options Menu\n"
"------------------\n"
"0. Return to previous menu\n"
"1. Optional plugin control\n"
"2. Logfile options\n"
"3. Log options for all devices\n"
"4. Log options for individual devices\n"
"5. CPU options\n"
"6. CPUID options\n"
"7. Memory options\n"
"8. Clock & CMOS options\n"
"9. PCI options\n"
"10. Bochs Display & Interface options\n"
"11. Keyboard & Mouse options\n"
"12. Disk & Boot options\n"
"13. Serial / Parallel / USB options\n"
"14. Network card options\n"
"15. Sound card options\n"
"16. Other options\n"
#if BX_PLUGINS
"17. User-defined options\n"
#endif
"\n"
"Please choose one: [0] ";

static const char *runtime_menu_prompt =
"---------------------\n"
"Bochs Runtime Options\n"
"---------------------\n"
"1. Floppy disk 0: %s\n"
"2. Floppy disk 1: %s\n"
"3. CDROM runtime options\n"
"4. Log options for all devices\n"
"5. Log options for individual devices\n"
"6. USB runtime options\n"
"7. Misc runtime options\n"
"8. Save configuration\n"
"9. Continue simulation\n"
"10. Quit now\n"
"\n"
"Please choose one:  [9] ";

static const char *plugin_ctrl_prompt =
"\n-----------------------\n"
"Optional plugin control\n"
"-----------------------\n"
"0. Return to previous menu\n"
"1. Load optional plugin\n"
"2. Unload optional plugin\n"
"\n"
"Please choose one:  [0] ";

#define NOT_IMPLEMENTED(choice) \
  bx_printf("ERROR: choice %d not implemented\n", choice);

#define BAD_OPTION(menu,choice) \
  do {bx_printf("ERROR: menu %d has no choice %d\n", menu, choice); \
      assert(0); } while (0)

void build_runtime_options_prompt(const char *format, char *buf, int size)
{
  bx_list_c *floppyop;
  char pname[80];
  char buffer[6][128];

  for (int i=0; i<2; i++) {
    sprintf(pname, "floppy.%d", i);
    floppyop = (bx_list_c*) SIM->get_param(pname);
    if (SIM->get_param_enum("devtype", floppyop)->get() == BX_FDD_NONE)
      strcpy(buffer[i], "(not present)");
    else {
      sprintf(buffer[i], "%s, size=%s, %s", SIM->get_param_string("path", floppyop)->getptr(),
        SIM->get_param_enum("type", floppyop)->get_selected(),
        SIM->get_param_enum("status", floppyop)->get_selected());
      if (!SIM->get_param_string("path", floppyop)->getptr()[0]) strcpy(buffer[i], "none");
    }
  }

  snprintf(buf, size, format, buffer[0], buffer[1]);
}

int do_menu(const char *pname)
{
  bx_list_c *menu = (bx_list_c *)SIM->get_param(pname, NULL);
  if (menu != NULL) {
    if (menu->get_size() > 0) {
      while (1) {
        menu->set_choice(0);
        int status = text_ask(menu);
        if (status < 0) return status;
        if (menu->get_choice() < 1)
          return menu->get_choice();
        else {
          int index = menu->get_choice() - 1;  // choosing 1 means list[0]
          bx_param_c *chosen = menu->get(index);
          assert(chosen != NULL);
          if (chosen->get_enabled()) {
            if (SIM->get_init_done() && !chosen->get_runtime_param()) {
              bx_printf("\nWARNING: parameter not available at runtime!\n");
            } else if (chosen->get_type() == BXT_LIST) {
              char chosen_pname[80];
              chosen->get_param_path(chosen_pname, 80);
              do_menu(chosen_pname);
            } else {
              text_ask(chosen);
            }
          }
        }
      }
    } else {
      bx_printf("\nERROR: nothing to configure in this section!\n");
    }
  } else {
     bx_printf("\nERROR: nothing to configure in this section!\n");
  }
  return -1;
}

int bx_text_config_interface(int menu)
{
  Bit32u choice;
  char sr_path[CI_PATH_LENGTH];
  while (1) {
    switch (menu) {
      case BX_CI_START_SIMULATION:
        SIM->begin_simulation(bx_startup_flags.argc, bx_startup_flags.argv);
        // we don't expect it to return, but if it does, quit
        SIM->quit_sim(1);
        break;
      case BX_CI_START_MENU:
        {
          Bit32u n_choices = 7;
          Bit32u default_choice;
          switch (SIM->get_param_enum(BXPN_BOCHS_START)->get()) {
            case BX_LOAD_START:
              default_choice = 2; break;
            case BX_EDIT_START:
              default_choice = 3; break;
            default:
              default_choice = 6; break;
          }
          if (ask_uint(startup_menu_prompt, "", 1, n_choices, default_choice, &choice, 10) < 0) return -1;
          switch (choice) {
            case 1:
              bx_printf("I reset all options back to their factory defaults.\n\n");
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
              bx_text_config_interface(BX_CI_START_OPTS);
              SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_RUN_START);
              break;
            case 4: bx_write_rc(NULL); break;
            case 5:
              if (ask_string("\nWhat is the path to restore the Bochs state from?\nTo cancel, type 'none'. [%s] ", "none", sr_path) >= 0) {
                if (strcmp(sr_path, "none")) {
                  SIM->reset_all_param();
                  SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(1);
                  SIM->get_param_string(BXPN_RESTORE_PATH)->set(sr_path);
                  if (!SIM->restore_config()) {
                    bx_printf("\nERROR: Cannot restore configuration!\n");
                  } else {
                    bx_text_config_interface(BX_CI_START_SIMULATION);
                  }
                }
              }
              break;
            case 6: bx_text_config_interface(BX_CI_START_SIMULATION); break;
            case 7: SIM->quit_sim(1); return -1;
            default: BAD_OPTION(menu, choice);
          }
        }
        break;
      case BX_CI_START_OPTS:
        if (ask_uint(startup_options_prompt, "", 0, 16+BX_PLUGINS, 0, &choice, 10) < 0) return -1;
        switch (choice) {
          case 0: return 0;
          case 1: bx_plugin_ctrl(); break;
          case 3: bx_log_options(0); break;
          case 4: bx_log_options(1); break;
          case 2: do_menu("log"); break;
          case 5: do_menu("cpu"); break;
          case 6: do_menu("cpuid"); break;
          case 7: do_menu("memory"); break;
          case 8: do_menu("clock_cmos"); break;
          case 9: do_menu("pci"); break;
          case 10: do_menu("display"); break;
          case 11: do_menu("keyboard_mouse"); break;
          case 12: do_menu(BXPN_MENU_DISK); break;
          case 13: do_menu("ports"); break;
          case 14: do_menu("network"); break;
          case 15: do_menu("sound"); break;
          case 16: do_menu("misc"); break;
#if BX_PLUGINS
          case 17: do_menu("user"); break;
#endif
          default: BAD_OPTION(menu, choice);
        }
        break;
      case BX_CI_RUNTIME:
        {
          char prompt[1024];
          build_runtime_options_prompt(runtime_menu_prompt, prompt, 1024);
          if (ask_uint(prompt, "", 1, BX_CI_RT_QUIT, BX_CI_RT_CONT, &choice, 10) < 0) return -1;
          switch (choice) {
            case BX_CI_RT_FLOPPYA:
              if (SIM->get_param_enum(BXPN_FLOPPYA_DEVTYPE)->get() != BX_FDD_NONE) do_menu(BXPN_FLOPPYA);
              break;
            case BX_CI_RT_FLOPPYB:
              if (SIM->get_param_enum(BXPN_FLOPPYB_DEVTYPE)->get() != BX_FDD_NONE) do_menu(BXPN_FLOPPYB);
              break;
            case BX_CI_RT_CDROM: do_menu(BXPN_MENU_RUNTIME_CDROM); break;
            case BX_CI_RT_LOGOPTS1: bx_log_options(0); break;
            case BX_CI_RT_LOGOPTS2: bx_log_options(1); break;
            case BX_CI_RT_USB: do_menu(BXPN_MENU_RUNTIME_USB); break;
            case BX_CI_RT_MISC: do_menu(BXPN_MENU_RUNTIME_MISC); break;
            case BX_CI_RT_SAVE_CFG:
              bx_write_rc(NULL);
              break;
            case BX_CI_RT_CONT:
              SIM->update_runtime_options();
              bx_printf("Continuing simulation\n");
              return 0;
            case BX_CI_RT_QUIT:
              bx_printf("You chose quit on the configuration interface.\n");
              bx_user_quit = 1;
#if !BX_DEBUGGER
              bx_atexit();
              SIM->quit_sim(1);
#else
              bx_dbg_exit(1);
#endif
              return -1;
            default: bx_printf("Menu choice %d not implemented.\n", choice);
          }
        }
        break;
      default:
        bx_printf("Unknown config interface menu type.\n");
        assert(menu >=0 && menu < BX_CI_N_MENUS);
    }
  }
}

static void bx_print_log_action_table()
{
  // just try to print all the prefixes first.
  bx_printf("Current log settings:\n");
  bx_printf("                 Debug      Info       Error       Panic\n");
  bx_printf("ID    Device     Action     Action     Action      Action\n");
  bx_printf("----  ---------  ---------  ---------  ----------  ----------\n");
  int i, j, imax=SIM->get_n_log_modules();
  for (i=0; i<imax; i++) {
    if (strcmp(SIM->get_prefix(i), BX_NULL_PREFIX)) {
      bx_printf("%3d.  %s ", i, SIM->get_prefix(i));
      for (j=0; j<SIM->get_max_log_level(); j++) {
        bx_printf("%10s ", SIM->get_action_name(SIM->get_log_action(i, j)));
      }
      bx_printf("\n");
    }
  }
}

static const char *log_options_prompt1 = "Enter the ID of the device to edit, or -1 to return: [-1] ";
static const char *log_level_choices[N_ACT+1] = { "ignore", "report", "warn", "ask", "fatal", "no change" };
static int log_level_n_choices_normal = N_ACT;

void bx_log_options(int individual)
{
  if (individual) {
    int done = 0;
    while (!done) {
      bx_print_log_action_table();
      Bit64s id;
      Bit32s level, action;
      Bit32s maxid = SIM->get_n_log_modules();
      if (ask_int(log_options_prompt1, "", -1, maxid-1, -1, &id) < 0)
        return;
      if (id < 0) return;
      bx_printf("Editing log options for the device %s\n", SIM->get_prefix(id));
      for (level=0; level<SIM->get_max_log_level(); level++) {
        char prompt[1024];
        int default_action = SIM->get_log_action(id, level);
        sprintf(prompt, "Enter action for %s event: [%s] ", SIM->get_log_level_name(level), SIM->get_action_name(default_action));
        // don't show the no change choice (choices=3)
        if (ask_menu(prompt, "", log_level_n_choices_normal, log_level_choices, default_action, &action)<0)
          return;
        // the exclude expression allows some choices not being available if they
        // don't make any sense.  For example, it would be stupid to ignore a panic.
        if (!BX_LOG_OPTS_EXCLUDE(level, action)) {
          SIM->set_log_action(id, level, action);
        } else {
          bx_printf("Event type '%s' does not support log action '%s'.\n",
                  SIM->get_log_level_name(level), log_level_choices[action]);
        }
      }
    }
  } else {
    // provide an easy way to set log options for all devices at once
    bx_print_log_action_table();
    for (int level=0; level<SIM->get_max_log_level(); level++) {
      char prompt[1024];
      int action, default_action = N_ACT;  // default to no change
      sprintf(prompt, "Enter action for %s event on all devices: [no change] ", SIM->get_log_level_name(level));
      // do show the no change choice (choices=4)
      if (ask_menu(prompt, "", log_level_n_choices_normal+1, log_level_choices, default_action, &action)<0)
        return;
      if (action < log_level_n_choices_normal) {
        if  (!BX_LOG_OPTS_EXCLUDE(level, action)) {
          SIM->set_default_log_action(level, action);
          SIM->set_log_action(-1, level, action);
        } else {
          bx_printf("Event type '%s' does not support log action '%s'.\n",
                  SIM->get_log_level_name(level), log_level_choices[action]);
        }
      }
    }
  }
}

int bx_read_rc(char *rc)
{
  if (rc && SIM->read_rc(rc) >= 0) return 0;
  char oldrc[CI_PATH_LENGTH];
  if (SIM->get_default_rc(oldrc, CI_PATH_LENGTH) < 0)
    strcpy(oldrc, "none");
  char newrc[CI_PATH_LENGTH];
  while (1) {
    if (ask_string("\nWhat is the configuration file name?\nTo cancel, type 'none'. [%s] ", oldrc, newrc) < 0) return -1;
    if (!strcmp(newrc, "none")) return -1;
    if (SIM->read_rc(newrc) >= 0) return 0;
    bx_printf("The file '%s' could not be found.\n", newrc);
  }
}

int bx_write_rc(char *rc)
{
  char oldrc[CI_PATH_LENGTH], newrc[CI_PATH_LENGTH];
  if (rc == NULL) {
    if (SIM->get_default_rc(oldrc, CI_PATH_LENGTH) < 0)
      strcpy(oldrc, "none");
  } else {
    strncpy(oldrc, rc, CI_PATH_LENGTH);
    oldrc[sizeof(oldrc) - 1] = '\0';
  }
  while (1) {
    if (ask_string("Save configuration to what file?  To cancel, type 'none'.\n[%s] ", oldrc, newrc) < 0) return -1;
    if (!strcmp(newrc, "none")) return 0;
    // try with overwrite off first
    int status = SIM->write_rc(newrc, 0);
    if (status >= 0) {
      bx_printf("Wrote configuration to '%s'.\n", newrc);
      return 0;
    } else if (status == -2) {
      // return code -2 indicates the file already exists, and overwrite
      // confirmation is required.
      Bit32u overwrite = 0;
      char prompt[570];
      sprintf(prompt, "Configuration file '%s' already exists.  Overwrite it? [no] ", newrc);
      if (ask_yn(prompt, "", 0, &overwrite) < 0) return -1;
      if (!overwrite) continue;  // if "no", start loop over, asking for a different file
      // they confirmed, so try again with overwrite bit set
      if (SIM->write_rc(newrc, 1) >= 0) {
        bx_printf("Overwriting existing configuration '%s'.\n", newrc);
        return 0;
      } else {
        bx_printf("Write failed to '%s'.\n", newrc);
      }
    }
  }
}

void bx_plugin_ctrl()
{
  Bit32u choice;
  bx_list_c *plugin_ctrl;
  bx_param_bool_c *plugin;
  int i, j, count;
  char plugname[512];

  while (1) {
    if (ask_uint(plugin_ctrl_prompt, "", 0, 2, 0, &choice, 10) < 0) return;
    if (choice == 0) {
      return;
    } else {
      plugin_ctrl = (bx_list_c*) SIM->get_param(BXPN_PLUGIN_CTRL);
      count = plugin_ctrl->get_size();
      if (count == 0) {
        bx_printf("\nNo optional plugins available\n");
      } else {
        bx_printf("\nCurrently loaded plugins:");
        j = 0;
        for (i = 0; i < count; i++) {
          plugin = (bx_param_bool_c*)plugin_ctrl->get(i);
          if (plugin->get()) {
            if (j > 0) bx_printf(",");
            bx_printf(" %s", plugin->get_name());
            j++;
          }
        }
        bx_printf("\n");
        if (choice == 1) {
          bx_printf("\nAdditionally available plugins:");
          j = 0;
          for (i = 0; i < count; i++) {
            plugin = (bx_param_bool_c*)plugin_ctrl->get(i);
            if (!plugin->get()) {
              if (j > 0) bx_printf(",");
              bx_printf(" %s", plugin->get_name());
              j++;
            }
          }
          bx_printf("\n");
        }
      }
      if (choice == 1) {
        ask_string("\nEnter the name of the plugin to load.\nTo cancel, type 'none'. [%s] ", "none", plugname);
        if (strcmp(plugname, "none")) {
          if (SIM->opt_plugin_ctrl(plugname, 1)) {
            bx_printf("\nLoaded plugin '%s'.\n", plugname);
          }
        }
      } else {
        ask_string("\nEnter the name of the plugin to unload.\nTo cancel, type 'none'. [%s] ", "none", plugname);
        if (strcmp(plugname, "none")) {
          if (SIM->opt_plugin_ctrl(plugname, 0)) {
            bx_printf("\nUnloaded plugin '%s'.\n", plugname);
          }
        }
      }
    }
  }
}

const char *log_action_ask_choices[] = { "cont", "alwayscont", "die", "abort", "debug" };
int log_action_n_choices = 4 + (BX_DEBUGGER||BX_GDBSTUB?1:0);

BxEvent *
textconfig_notify_callback(void *unused, BxEvent *event)
{
  event->retcode = -1;
  switch (event->type)
  {
    case BX_SYNC_EVT_TICK:
      event->retcode = 0;
      return event;
    case BX_SYNC_EVT_ASK_PARAM:
      event->retcode = text_ask(event->u.param.param);
      return event;
    case BX_SYNC_EVT_LOG_DLG:
      if (event->u.logmsg.mode == BX_LOG_DLG_ASK) {
        int level = event->u.logmsg.level;
        fprintf(stderr, "========================================================================\n");
        fprintf(stderr, "Event type: %s\n", SIM->get_log_level_name (level));
        fprintf(stderr, "Device: %s\n", event->u.logmsg.prefix);
        fprintf(stderr, "Message: %s\n\n", event->u.logmsg.msg);
        fprintf(stderr, "A %s has occurred.  Do you want to:\n", SIM->get_log_level_name (level));
        fprintf(stderr, "  cont       - continue execution\n");
        fprintf(stderr, "  alwayscont - continue execution, and don't ask again.\n");
        fprintf(stderr, "               This affects only %s events from device %s\n", SIM->get_log_level_name (level), event->u.logmsg.prefix);
        fprintf(stderr, "  die        - stop execution now\n");
        fprintf(stderr, "  abort      - dump core %s\n",
                BX_HAVE_ABORT ? "" : "(Disabled)");
#if BX_DEBUGGER
        fprintf(stderr, "  debug      - continue and return to bochs debugger\n");
#endif
#if BX_GDBSTUB
        fprintf(stderr, "  debug      - hand control to gdb\n");
#endif

        int choice;
ask:
        if (ask_menu("Choose one of the actions above: [%s] ", "",
                     log_action_n_choices, log_action_ask_choices, 2, &choice) < 0)
        event->retcode = -1;
        // return 0 for continue, 1 for alwayscontinue, 2 for die, 3 for debug.
        if (!BX_HAVE_ABORT && choice==BX_LOG_ASK_CHOICE_DUMP_CORE) goto ask;
        fflush(stdout);
        fflush(stderr);
        event->retcode = choice;
      } else {
        // warning prompt not implemented
        event->retcode = 0;
      }
      return event;
    case BX_SYNC_EVT_ML_MSG_BOX:
      fprintf(stderr, "%s\n%s\n", event->u.logmsg.prefix, event->u.logmsg.msg);
      return event;
    case BX_SYNC_EVT_ML_MSG_BOX_KILL:
      // Nothing to do
      return event;
    case BX_ASYNC_EVT_REFRESH:
    case BX_ASYNC_EVT_DBG_MSG:
    case BX_ASYNC_EVT_LOG_MSG:
      // The text mode interface does not use these events, so just ignore
      // them.
      return event;
    default:
      fprintf(stderr, "textconfig: notify callback called with event type %04x\n", event->type);
      return event;
  }
  assert(0); // switch statement should return
}

/////////////////////////////////////////////////////////////////////
// implement the text_* methods for bx_param types.

void text_print(bx_param_c *param)
{
  switch (param->get_type()) {
    case BXT_PARAM_NUM:
      {
        bx_param_num_c *nparam = (bx_param_num_c*)param;
        Bit64s nval = nparam->get64();
        if (nparam->get_long_format()) {
          bx_printf(nparam->get_long_format(), nval);
        } else {
          const char *format;
          int base = nparam->get_base();
          if (base == 16)
            format = "%s: 0x%x";
          else
            format = "%s: %ld";
          if (nparam->get_label()) {
            bx_printf(format, nparam->get_label(), (Bit64s)nval);
          } else {
            bx_printf(format, nparam->get_name(), (Bit64s)nval);
          }
        }
      }
      break;
    case BXT_PARAM_BOOL:
      {
        bx_param_bool_c *bparam = (bx_param_bool_c*)param;
        bool bval = (bool)bparam->get();
        if (bparam->get_format()) {
          bx_printf(bparam->get_format(), bval ? "yes" : "no");
        } else {
          const char *format = "%s: %s";
          if (bparam->get_label()) {
            bx_printf(format, bparam->get_label(), bval ? "yes" : "no");
          } else {
            bx_printf(format, bparam->get_name(), bval ? "yes" : "no");
          }
        }
      }
      break;
    case BXT_PARAM_ENUM:
      {
        bx_param_enum_c *eparam = (bx_param_enum_c*)param;
        const char *choice = eparam->get_selected();
        if (eparam->get_format()) {
          bx_printf(eparam->get_format(), choice);
        } else {
          const char *format = "%s: %s";
          if (eparam->get_label()) {
            bx_printf(format, eparam->get_label(), choice);
          } else {
            bx_printf(format, eparam->get_name(), choice);
          }
        }
      }
      break;
    case BXT_PARAM_STRING:
    case BXT_PARAM_BYTESTRING:
      {
        bx_param_string_c *sparam = (bx_param_string_c*)param;
        char value[1024];

        sparam->dump_param(value, 1024);
        if (sparam->get_format()) {
          bx_printf(sparam->get_format(), value);
        } else {
          if (sparam->get_label()) {
            bx_printf("%s: %s", sparam->get_label(), value);
          } else {
            bx_printf("%s: %s", sparam->get_name(), value);
          }
        }
      }
      break;
    default:
      bx_printf("ERROR: unsupported parameter type\n");
  }
}

int text_ask(bx_param_c *param)
{
  const char *prompt = param->get_ask_format();
  const char *help = param->get_description();
  Bit32u options = param->get_options();

  bx_printf("\n");
  switch (param->get_type()) {
    case BXT_PARAM_NUM:
      {
        bx_param_num_c *nparam = (bx_param_num_c*)param;
        int status;
        if (prompt == NULL) {
          // default prompt, if they didn't set an ask format string
          text_print(param);
          bx_printf("\n");
          if (nparam->get_base() == 16)
            prompt = "Enter new value in hex or '?' for help: [%x] ";
          else
            prompt = "Enter new value or '?' for help: [%d] ";
        }
        Bit32u n = nparam->get();
        Bit64s m = nparam->get64();

        if(nparam->get_base() != 10){
          status = ask_uint(prompt, help, (Bit32u)nparam->get_min(),
                          (Bit32u)nparam->get_max(), n, &n, nparam->get_base());
        }else{
          status = ask_int(prompt, help, (Bit64s)nparam->get_min(),
                          (Bit64s)nparam->get_max(), m, &m);
        }
        if (status < 0) return status;
        if(nparam->get_base() != 10) nparam->set(n);
        else nparam->set(m);
      }
      break;
    case BXT_PARAM_BOOL:
      {
        bx_param_bool_c *bparam = (bx_param_bool_c*)param;
        int status;
        char buffer[512];
        if (prompt == NULL) {
          if (bparam->get_label() != NULL) {
            sprintf(buffer, "%s? [%%s] ", bparam->get_label());
            prompt = buffer;
          } else {
            // default prompt, if they didn't set an ask format or label string
            sprintf(buffer, "%s? [%%s] ", bparam->get_name());
            prompt = buffer;
          }
        }
        Bit32u n = bparam->get();
        status = ask_yn(prompt, help, n, &n);
        if (status < 0) return status;
        bparam->set(n);
      }
      break;
    case BXT_PARAM_ENUM:
      {
        bx_param_enum_c *eparam = (bx_param_enum_c*)param;
        if (prompt == NULL) {
          // default prompt, if they didn't set an ask format string
          text_print(param);
          bx_printf("\n");
          prompt = "Enter new value or '?' for help: [%s] ";
        }
        Bit32s min = (Bit32s)eparam->get_min();
        Bit32s max = (Bit32s)eparam->get_max();
        Bit32s n = eparam->get() - min;
        int status = ask_menu(prompt, help, (Bit32u)(max-min+1),
                              eparam->get_choices(), n, &n);
        if (status < 0) return status;
        n += min;
        eparam->set(n);
      }
      break;
    case BXT_PARAM_STRING:
      {
        bx_param_string_c *sparam = (bx_param_string_c*)param;
        if (prompt == NULL) {
          if (options & sparam->SELECT_FOLDER_DLG) {
            bx_printf("%s\n\n", sparam->get_label());
            prompt = "Enter a path to an existing folder or press enter to cancel\n";
          } else {
            // default prompt, if they didn't set an ask format string
            text_print(param);
            bx_printf("\n");
            prompt = "Enter a new value, '?' for help, or press return for no change.\n";
          }
        }
        while (1) {
          char buffer[1024];
          int status = ask_string(prompt, sparam->getptr(), buffer);
          if (status == -2) {
            bx_printf("\n%s\n", help);
            continue;
          }
          if (status < 0) return status;
          if (!sparam->equals(buffer))
            sparam->set(buffer);
          return 0;
        }
      }
      break;
    case BXT_PARAM_BYTESTRING:
      {
        bx_param_bytestring_c *hparam = (bx_param_bytestring_c*)param;
        if (prompt == NULL) {
          // default prompt, if they didn't set an ask format string
          text_print(param);
          bx_printf("\n");
          prompt = "Enter a new value, '?' for help, or press return for no change.\n";
        }
        while (1) {
          char buffer[1024];
          int status = ask_string(prompt, hparam->getptr(), buffer);
          if (status == -2) {
            bx_printf("\n%s\n", help);
            continue;
          }
          if (status < 0) return status;
          if (status == 0) return 0;
          // copy raw hex into buffer
          status = hparam->parse_param(buffer);
          if (status == 0) {
            char separator = hparam->get_separator();
            bx_printf("Illegal raw byte format.  I expected something like 3A%c03%c12%c...\n", separator, separator, separator);
            continue;
          }
          return 0;
        }
      }
      break;
    case BXT_LIST:
      {
        bx_list_c *list = (bx_list_c*)param;
        bx_param_c *child;
        const char *my_title = list->get_title();
        int i, imax = strlen(my_title);
        for (i=0; i<imax; i++) bx_printf("-");
        bx_printf("\n%s\n", my_title);
        for (i=0; i<imax; i++) bx_printf("-");
        bx_printf("\n");
        if (options & list->SERIES_ASK) {
          for (i = 0; i < list->get_size(); i++) {
            child = list->get(i);
            if (child->get_enabled()) {
              if (!SIM->get_init_done() || child->get_runtime_param()) {
                text_ask(child);
              }
            }
          }
        } else {
          if (options & list->SHOW_PARENT)
            bx_printf("0. Return to previous menu\n");
          for (i = 0; i < list->get_size(); i++) {
            bx_printf("%d. ", i+1);
            child = list->get(i);
            if ((child->get_enabled()) &&
                (!SIM->get_init_done() || child->get_runtime_param())) {
              if (child->get_type() == BXT_LIST) {
                bx_printf("%s\n", ((bx_list_c*)child)->get_title());
              } else {
                if ((options & list->SHOW_GROUP_NAME) &&
                    (child->get_group() != NULL))
                  bx_printf("%s ", child->get_group());
                text_print(child);
                bx_printf("\n");
              }
            } else {
              if (child->get_type() == BXT_LIST) {
                bx_printf("%s (disabled)\n", ((bx_list_c*)child)->get_title());
              } else {
                bx_printf("(disabled)\n");
              }
            }
          }
          bx_printf("\n");
          int min = (options & list->SHOW_PARENT) ? 0 : 1;
          int max = list->get_size();
          Bit32u choice = list->get_choice();
          int status = ask_uint("Please choose one: [%d] ", "", min, max, choice, &choice, 10);
          if (status < 0) return status;
          list->set_choice(choice);
        }
      }
      break;
    default:
      bx_printf("ERROR: unsupported parameter type\n");
  }
  return 0;
}

static int text_ci_callback(void *userdata, ci_command_t command)
{
  switch (command)
  {
    case CI_START:
      if (SIM->get_param_enum(BXPN_BOCHS_START)->get() == BX_QUICK_START)
        bx_text_config_interface(BX_CI_START_SIMULATION);
      else {
        if (!SIM->test_for_text_console())
          return CI_ERR_NO_TEXT_CONSOLE;
        bx_text_config_interface(BX_CI_START_MENU);
      }
      break;
    case CI_RUNTIME_CONFIG:
      bx_text_config_interface(BX_CI_RUNTIME);
      break;
    case CI_SHUTDOWN:
      break;
  }
  return 0;
}

#endif
