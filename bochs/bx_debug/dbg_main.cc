/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2026  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

extern "C" {
#include <signal.h>
}

#include "bochs.h"
#include "param_names.h"
#include "debug.h"
#include "cpu/cpu.h"
#include "cpu/decoder/ia_opcodes.h"
#include "iodev/iodev.h"
#include "pc_system.h"
#if BX_DEBUGGER

#define LOG_THIS genlog->

#if HAVE_LIBREADLINE
extern "C" {
#include <stdio.h>
#include <readline/readline.h>
#if HAVE_READLINE_HISTORY_H
#include <readline/history.h>
#endif
}
#endif

// default CPU in the debugger.  For commands like "dump_cpu" it will
// use the default instead of always dumping all cpus.
unsigned dbg_cpu = 0;

// defined in dbg_commands.cc
extern bx_list_c **dbg_cpu_list;
extern unsigned *last_cpu_mode;
extern bx_address *last_cr3;

bx_param_bool_c *sim_running = NULL;
static bool bx_dbg_exit_called;

static char tmp_buf[512];
static char tmp_buf_prev[512];
static char *tmp_buf_ptr;
char *argv0 = NULL;

static FILE *debugger_log = NULL;

typedef struct {
  FILE    *fp;
  char     fname[BX_MAX_PATH];
  unsigned lineno;
} bx_infile_stack_entry_t;

static bx_infile_stack_entry_t bx_infile_stack[BX_INFILE_DEPTH];
static int                     bx_infile_stack_index = 0;

int bx_nest_infile(char *path);

void CDECL bx_debug_ctrlc_handler(int signum);

static void bx_unnest_infile(void);
static void bx_get_command(void);
static void bx_dbg_deactivate(void);

// leave the debugger input loop and detach the debugger
static bool bx_dbg_detach_requested = false;

bx_guard_t bx_guard;

#define DBG_PRINTF_BUFFER_LEN 1024

void dbg_printf(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char buf[DBG_PRINTF_BUFFER_LEN+1];
  vsnprintf(buf, DBG_PRINTF_BUFFER_LEN, fmt, ap);
  va_end(ap);
  if (debugger_log != NULL) {
    fprintf(debugger_log,"%s", buf);
    fflush(debugger_log);
  }
  SIM->debug_puts(buf); // send to debugger, which will free buf when done.
}

int bx_dbg_set_rcfile(const char *rcfile)
{
  strncpy(bx_infile_stack[0].fname, rcfile, BX_MAX_PATH);
  bx_infile_stack[0].fname[BX_MAX_PATH-1] = 0;
  BX_INFO(("debugger using rc file '%s'.", rcfile));
  return bx_nest_infile((char*)rcfile);
}

void switch_dbg_cpu(unsigned cpu)
{
  dbg_printf("Switching to CPU%d\n", cpu);
  dbg_cpu = cpu;
}

void bx_dbg_init(void)
{
  memset(&bx_guard, 0, sizeof(bx_guard));
  bx_guard.async.irq = 1;
  bx_guard.async.dma = 1;

  bx_infile_stack_index = 0;
  bx_infile_stack[0].fp = stdin;
  bx_infile_stack[0].lineno = 0;
}

// Enter the internal debugger, returns when the debugger is detached
int bx_dbg_main(void)
{
  bx_dbg_exit_called = 0;

  // one-time initialization on the first debugger entry
  if (dbg_cpu_list == NULL) {
    const char *debugger_log_filename = SIM->get_param_string(BXPN_DEBUGGER_LOG_FILENAME)->getptr();

    // Open debugger log file if needed
    if (strlen(debugger_log_filename) > 0 && (strcmp(debugger_log_filename, "-") != 0))
    {
      debugger_log = fopen(debugger_log_filename, "w");
      if (!debugger_log) {
        BX_PANIC(("Can not open debugger log file '%s'", debugger_log_filename));
      }
      else {
        BX_INFO(("Using debugger log file %s", debugger_log_filename));
      }
    }

    dbg_printf("Bochs internal debugger, type 'help' for help or 'c' to continue\n");

    last_cr3 = new bx_address[BX_SMP_PROCESSORS];
    last_cpu_mode = new unsigned[BX_SMP_PROCESSORS];

    dbg_cpu_list = new bx_list_c *[BX_SMP_PROCESSORS];
    for (int cpu=0; cpu<BX_SMP_PROCESSORS; cpu++) {
      char cpu_param_name[10];
      sprintf(cpu_param_name, "cpu%d", (int)cpu);
      dbg_cpu_list[cpu] = (bx_list_c*) SIM->get_param(cpu_param_name, SIM->get_bochs_root());

      last_cr3[cpu] = 0;
      last_cpu_mode[cpu] = 0;
    }
  }

  // create a boolean parameter that will tell if the simulation is
  // running (continue command) or waiting for user response.  This affects
  // some parts of the GUI.
  if (sim_running == NULL) {
    bx_list_c *base = (bx_list_c*) SIM->get_param("general");
    sim_running = new bx_param_bool_c(base,
        "debug_running",
        "Simulation is running", "", 0);
  } else {
    sim_running->set(0);
  }
  // setup Ctrl-C handler
  if (!SIM->has_debug_gui()) {
    signal(SIGINT, bx_debug_ctrlc_handler);
    BX_INFO(("set SIGINT handler to bx_debug_ctrlc_handler"));
  }

  // select the CPU which requested the debugger activation (CPU0 by default)
  switch_dbg_cpu(bx_dbg.activation_cpu);

  if (bx_dbg.activation_reason != NULL) {
    dbg_printf("(%u) Debugger activated: %s\n", bx_dbg.activation_cpu, bx_dbg.activation_reason);
    bx_dbg.activation_reason = NULL;
  }

  // finally, call the usual function to print the disassembly
  dbg_printf("Next at t=" FMT_LL "d\n", bx_pc_system.time_ticks());
  bx_dbg_disassemble_current(-1, 0);  // all cpus, don't print time

  bx_dbg_user_input_loop();

  // the debugger was detached
  bx_dbg_deactivate();

  return(0);
}

void bx_dbg_interpret_line(char *cmd)
{
  bx_add_lex_input(cmd);
  bxparse();
}

void bx_dbg_user_input_loop(void)
{
  int reti;
  unsigned include_cmd_len = strlen(BX_INCLUDE_CMD);

  bx_dbg_detach_requested = false;

  while(! bx_dbg_detach_requested) {
    SIM->refresh_ci();
    SIM->set_display_mode(DISP_MODE_CONFIG);
    SIM->get_param_bool(BXPN_MOUSE_ENABLED)->set(0);
    bx_get_command();
reparse:
    if ((*tmp_buf_ptr == '\n') || (*tmp_buf_ptr == 0))
    {
        if ((*tmp_buf_prev != '\n') && (*tmp_buf_prev != 0)) {
          strncpy(tmp_buf, tmp_buf_prev, sizeof(tmp_buf));
          tmp_buf[sizeof(tmp_buf) - 1] = '\0';
          goto reparse;
        }
    }
    else if ((strncmp(tmp_buf_ptr, BX_INCLUDE_CMD, include_cmd_len) == 0) &&
              (tmp_buf_ptr[include_cmd_len] == ' ' ||
               tmp_buf_ptr[include_cmd_len] == '\t'))
    {
      char *ptr = tmp_buf_ptr + include_cmd_len + 1;
      while(*ptr==' ' || *ptr=='\t')
        ptr++;

      int len = strlen(ptr);
      if (len == 0) {
        dbg_printf("%s: no filename given to 'source' command.\n", argv0);
        if (bx_infile_stack_index > 0) {
          dbg_printf("%s: ERROR in source file causes exit.\n", argv0);
          bx_dbg_exit(1);
        }
        continue;
      }
      ptr[len-1] = 0; // get rid of newline
      reti = bx_nest_infile(ptr);
      if (reti==0 && bx_infile_stack_index > 0) {
        dbg_printf("%s: ERROR in source file causes exit.\n", argv0);
        bx_dbg_exit(1);
      }
    }
    else {
      // Give a chance to the command line extensions, to
      // consume the command.  If they return 0, then
      // we need to process the command.  A return of 1
      // means, the extensions have handled the command
      if (bx_dbg_extensions(tmp_buf_ptr)==0) {
        // process command here
        bx_dbg_interpret_line(tmp_buf_ptr);
      }
    }
  }
}

void bx_get_command(void)
{
  char *charptr_ret;

  bx_infile_stack[bx_infile_stack_index].lineno++;

  char prompt[256];
  if (bx_infile_stack_index == 0) {
    sprintf(prompt, "<bochs:%u> ", bx_infile_stack[bx_infile_stack_index].lineno);
  }
  if (SIM->has_debug_gui() && bx_infile_stack_index == 0) {
    // wait for gui debugger to send another debugger command
    charptr_ret = SIM->debug_get_next_command();
    if (charptr_ret) {
      strncpy(tmp_buf, charptr_ret, sizeof(tmp_buf));
      tmp_buf[sizeof(tmp_buf) - 2] = '\0';
      strcat(tmp_buf, "\n");
      // The returned string was allocated in wxmain.cc by "new char[]".
      // Free it with delete[].
      delete [] charptr_ret;
      charptr_ret = &tmp_buf[0];
    } else {
      // if debug_get_next_command returned NULL, probably the GUI is
      // shutting down
    }
  }
#if HAVE_LIBREADLINE
  else if (bx_infile_stack_index == 0) {
    charptr_ret = readline(prompt);
    // beware, returns NULL on end of file
    if (charptr_ret && strlen(charptr_ret) > 0) {
#if HAVE_READLINE_HISTORY_H
      add_history(charptr_ret);
#endif
      strncpy(tmp_buf, charptr_ret, sizeof(tmp_buf));
      tmp_buf[sizeof(tmp_buf) - 2] = '\0';
      strcat(tmp_buf, "\n");
      free(charptr_ret);
      charptr_ret = &tmp_buf[0];
    }
  } else {
    charptr_ret = fgets(tmp_buf, sizeof(tmp_buf), bx_infile_stack[bx_infile_stack_index].fp);
  }
#else /* !HAVE_LIBREADLINE */
  else {
    if (bx_infile_stack_index == 0)
      dbg_printf("%s", prompt);
    strncpy(tmp_buf_prev, tmp_buf, sizeof(tmp_buf_prev));
    charptr_ret = fgets(tmp_buf, sizeof(tmp_buf), bx_infile_stack[bx_infile_stack_index].fp);
  }
#endif
  if (charptr_ret == NULL) {
    // see if error was due to EOF condition
    if (feof(bx_infile_stack[bx_infile_stack_index].fp)) {
      if (bx_infile_stack_index > 0) {
        // nested level of include files, pop back to previous one
        bx_unnest_infile();
      }
      else {
        // not nested, sitting at stdin prompt, user wants out
        bx_dbg_quit_command();
        BX_PANIC(("bx_dbg_quit_command should not return, but it did"));
      }

      // call recursively
      bx_get_command();
      return;
    }

    // error was not EOF, see if it was from a Ctrl-C
    if (bx_guard.interrupt_requested) {
      tmp_buf[0] = '\n';
      tmp_buf[1] = 0;
      tmp_buf_ptr = &tmp_buf[0];
      bx_guard.interrupt_requested = false;
      return;
    }

    dbg_printf("fgets() returned ERROR.\n");
    dbg_printf("debugger interrupt request was %u\n", bx_guard.interrupt_requested);
    bx_dbg_exit(1);
  }
  tmp_buf_ptr = &tmp_buf[0];

  if (debugger_log != NULL) {
    fprintf(debugger_log, "%s", tmp_buf);
    fflush(debugger_log);
  }

  // look for first non-whitespace character
  while (((*tmp_buf_ptr == ' ')  || (*tmp_buf_ptr == '\t')) &&
          (*tmp_buf_ptr != '\n') && (*tmp_buf_ptr != 0))
  {
    tmp_buf_ptr++;
  }
}

int bx_nest_infile(char *path)
{
  FILE *tmp_fp;

  tmp_fp = fopen(path, "r");
  if (!tmp_fp) {
    dbg_printf("%s: can not open file '%s' for reading.\n", argv0, path);
    return(0);
  }

  if ((bx_infile_stack_index+1) >= BX_INFILE_DEPTH) {
    fclose(tmp_fp);
    dbg_printf("%s: source files nested too deeply\n", argv0);
    return(0);
  }

  bx_infile_stack_index++;
  bx_infile_stack[bx_infile_stack_index].fp = tmp_fp;
  strncpy(bx_infile_stack[bx_infile_stack_index].fname, path, BX_MAX_PATH);
  bx_infile_stack[bx_infile_stack_index].fname[BX_MAX_PATH-1] = 0;
  bx_infile_stack[bx_infile_stack_index].lineno = 0;
  return(1);
}

void bx_unnest_infile(void)
{
  if (bx_infile_stack_index <= 0) {
    dbg_printf("%s: ERROR: unnest_infile(): nesting level = 0\n", argv0);
    bx_dbg_exit(1);
  }

  fclose(bx_infile_stack[bx_infile_stack_index].fp);
  bx_infile_stack_index--;
}

int bxwrap(void)
{
  dbg_printf("%s: ERROR: bxwrap() called\n", argv0);
  bx_dbg_exit(1);
  return(0); // keep compiler quiet
}

#ifdef WIN32
extern "C" char* bxtext;
#endif

void bxerror(const char *s)
{
  dbg_printf("%s:%d: %s at '%s'\n", bx_infile_stack[bx_infile_stack_index].fname,
    bx_infile_stack[bx_infile_stack_index].lineno, s, bxtext);

  if (bx_infile_stack_index > 0) {
    dbg_printf("%s: ERROR in source file causes exit\n", argv0);
    bx_dbg_exit(1);
  }
}

void CDECL bx_debug_ctrlc_handler(int signum)
{
  UNUSED(signum);
  if (SIM->has_debug_gui()) {
    // in a multithreaded environment, a signal such as SIGINT can be sent to all
    // threads.  This function is only intended to handle signals in the
    // simulator thread.  It will simply return if called from any other thread.
    // Otherwise the BX_PANIC() below can be called in multiple threads at
    // once, leading to multiple threads trying to display a dialog box,
    // leading to GUI deadlock.
    if (!SIM->is_sim_thread()) {
      BX_INFO(("bx_signal_handler: ignored sig %d because it wasn't called from the simulator thread", signum));
      return;
    }
  }
  BX_INFO(("Ctrl-C detected in signal handler."));

  signal(SIGINT, bx_debug_ctrlc_handler);
  bx_debug_break();
}

void bx_debug_break()
{
  bx_guard.interrupt_requested = true;
}

// Request to activate the internal debugger when Bochs is running without
// it. The CPU loop returns on the next instruction boundary and the debugger
// is activated by bx_dbg_activate() called from the simulation main loop.
void bx_dbg_request_activation(unsigned cpu, const char *reason)
{
  if (bx_dbg.debugger_active) {
    bx_debug_break();
    return;
  }

  // the debugger cannot be activated at runtime with these display libraries:
  // wx forces the gui debugger and term sets up the debugger terminal only
  // when the debugger is active on startup
  const char *display = SIM->get_param_enum(BXPN_SEL_DISPLAY_LIBRARY)->get_selected();
  if (SIM->is_wx_selected() || !strcmp(display, "term")) {
    static bool warned = false;
    if (! warned) {
      BX_ERROR(("debugger activation (%s) ignored: not supported with '%s' display library", reason, display));
      warned = true;
    }
    return;
  }

  // keep the first request if several CPUs requested activation
  if (bx_dbg.activation_request) return;

  bx_dbg.activation_cpu = cpu;
  bx_dbg.activation_reason = reason;
  bx_dbg.activation_request = true;

  for (int n=0; n<BX_SMP_PROCESSORS; n++) {
    BX_CPU(n)->async_event |= BX_ASYNC_EVENT_DEBUGGER_REQUEST;
  }
}

// Activate the internal debugger requested by bx_dbg_request_activation(),
// called when the CPU loop is not running. The simulation main loop enters
// the debugger by bx_dbg_main() after that.
void bx_dbg_activate(void)
{
  BX_ASSERT(! bx_dbg.debugger_active);

  bx_dbg.activation_request = false;
  bx_dbg.debugger_active = true;
  bx_guard.interrupt_requested = false;

  SIM->get_param_string(BXPN_DEBUGGER_LOG_FILENAME)->set_enabled(1);

  for (int n=0; n<BX_SMP_PROCESSORS; n++) {
    BX_CPU(n)->async_event &= ~BX_ASYNC_EVENT_DEBUGGER_REQUEST;
    // stop reason could be left stale by the CPU loop running without the debugger
    BX_CPU(n)->stop_reason = (BX_CPU(n)->activity_state != BX_CPU_C::BX_ACTIVITY_STATE_ACTIVE) ? STOP_CPU_HALTED : STOP_NO_REASON;
    // the fetch window was established without the debugger, force prefetch()
    // to recompute the code breakpoints page filter for it
    BX_CPU(n)->invalidate_prefetch_q();
  }

  BX_INFO(("[" FMT_LL "d] Debugger activated on CPU%u: %s",
    bx_pc_system.time_ticks(), bx_dbg.activation_cpu, bx_dbg.activation_reason));
}

// Leave the debugger and continue the simulation without it
void bx_dbg_detach_command(void)
{
  if (SIM->has_debug_gui()) {
    dbg_printf("detach is not supported with the gui debugger yet\n");
    return;
  }

  unsigned num_bpoints = 0;
#if (BX_DBG_MAX_VIR_BPOINTS > 0)
  num_bpoints += bx_guard.iaddr.num_virtual;
#endif
#if (BX_DBG_MAX_LIN_BPOINTS > 0)
  num_bpoints += bx_guard.iaddr.num_linear;
#endif
#if (BX_DBG_MAX_PHY_BPOINTS > 0)
  num_bpoints += bx_guard.iaddr.num_physical;
#endif

  dbg_printf("Detaching debugger, simulation continues without it\n");
  if (num_bpoints > 0 || num_read_watchpoints > 0 || num_write_watchpoints > 0) {
    dbg_printf("%u breakpoints and %u watchpoints are inactive until the debugger is activated again\n",
      num_bpoints, num_read_watchpoints + num_write_watchpoints);
  }

  char magic_str[64];
  bx_dbg_get_magic_bp_str_from_mask(bx_dbg.magic_break, magic_str);
  dbg_printf("Debugger activation: magic breakpoint%s, %d time breakpoints pending\n",
    bx_dbg.magic_break ? magic_str : " disabled", timebp_queue_size);

  bx_dbg_detach_requested = true;
}

// Deactivate the debugger after the detach command, the simulation main loop
// continues to run without the debugger after that
static void bx_dbg_deactivate(void)
{
  bx_dbg.debugger_active = false;
  bx_guard.interrupt_requested = false;

  // The debugger loop advances the time by single instructions without
  // updating the time sync point, sync it so the instructions executed in
  // the debugger are not accounted again by the CPU loop
  for (int n=0; n<BX_SMP_PROCESSORS; n++) {
    BX_CPU(n)->sync_icount();
  }

  // restore Ctrl-C handler used without the debugger
  if (!SIM->has_debug_gui()) {
    signal(SIGINT, bx_signal_handler);
  }

  if (debugger_log != NULL)
    fflush(debugger_log);

  sim_running->set(1);
  SIM->refresh_ci();
  SIM->set_display_mode(DISP_MODE_SIM);

  BX_INFO(("[" FMT_LL "d] Debugger detached", bx_pc_system.time_ticks()));
}

void bx_dbg_exit(int code)
{
  if (bx_dbg_exit_called) return;
  bx_dbg_exit_called = 1;
  BX_DEBUG(("dbg: before exit"));

  if(debugger_log != NULL)
    fclose(debugger_log);
  debugger_log = NULL;

  delete [] dbg_cpu_list;
  dbg_cpu_list = NULL;

  delete [] last_cr3;
  last_cr3 = NULL;

  delete [] last_cpu_mode;
  last_cpu_mode = NULL;

  bx_atexit();

  BX_EXIT(code);
}

void bx_dbg_quit_command(void)
{
  BX_INFO(("dbg: Quit"));
  bx_dbg_exit(0);
}

#endif /* if BX_DEBUGGER */
