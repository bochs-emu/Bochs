/////////////////////////////////////////////////////////////////////////
// $Id: dbg_main.cc,v 1.93 2007-05-12 19:19:15 sshwarts Exp $
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
/////////////////////////////////////////////////////////////////////////

extern "C" {
#include <signal.h>
}

#include "bochs.h"
#include "cpu/cpu.h"
#include "iodev/iodev.h"
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

extern const char* cpu_mode_string(unsigned cpu_mode);

static bx_param_bool_c *sim_running = NULL;

static char tmp_buf[512];
static char tmp_buf_prev[512];
static char *tmp_buf_ptr;
static char *argv0 = NULL;

static FILE *debugger_log = NULL;

static struct {
  // some fields used for single CPU debugger
  bx_bool  auto_disassemble;
  unsigned disassemble_size;
  char     default_display_format;
  char     default_unit_size;
  Bit32u   default_addr;
  unsigned next_bpoint_id;
} bx_debugger;

#define BX_DBG_DEFAULT_ICOUNT_QUANTUM 5

typedef struct {
  FILE    *fp;
  char     fname[BX_MAX_PATH];
  unsigned lineno;
} bx_infile_stack_entry_t;

bx_infile_stack_entry_t bx_infile_stack[BX_INFILE_DEPTH];
int                     bx_infile_stack_index = 0;

static int  bx_nest_infile(char *path);

void bx_debug_ctrlc_handler(int signum);

static void bx_unnest_infile(void);
static void bx_get_command(void);
static void bx_dbg_print_guard_results();
static void bx_dbg_breakpoint_changed(void);

bx_guard_t        bx_guard;

// DMA stuff
void bx_dbg_post_dma_reports(void);
#define BX_BATCH_DMA_BUFSIZE 512

static struct {
  unsigned this_many; // batch this many max before posting events
  unsigned Qsize;     // this many have been batched
  struct {
    Bit32u   addr;    // address of DMA op
    unsigned len;     // number of bytes in op
    unsigned what;    // BX_READ or BX_WRITE
    Bit32u   val;     // value of DMA op
    Bit64u   time;    // system time at this dma op
  } Q[BX_BATCH_DMA_BUFSIZE];
} bx_dbg_batch_dma;

// some buffers for disassembly
static disassembler bx_disassemble;
static Bit8u bx_disasm_ibuf[32];
static char  bx_disasm_tbuf[512];

#define DBG_PRINTF_BUFFER_LEN 1024

void dbg_printf(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char *buf = new char[DBG_PRINTF_BUFFER_LEN+1];
  vsnprintf(buf, DBG_PRINTF_BUFFER_LEN, fmt, ap);
  va_end(ap);
  if (debugger_log != NULL) {
    fprintf(debugger_log,"%s", buf);
    fflush(debugger_log);
  }
  SIM->debug_puts(buf); // send to debugger, which will free buf when done.
}

void bx_dbg_init_infile(void)
{
  bx_infile_stack_index = 0;
  bx_infile_stack[0].fp = stdin;
  bx_infile_stack[0].lineno = 0;
}

int bx_dbg_set_rcfile(const char *rcfile)
{
  strncpy(bx_infile_stack[0].fname, rcfile, BX_MAX_PATH);
  bx_infile_stack[0].fname[BX_MAX_PATH-1] = 0;
  BX_INFO(("debugger using rc file '%s'.", rcfile));
  return bx_nest_infile((char*)rcfile);
}

int bx_dbg_main(void)
{
  int i;
  
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  bx_dbg_batch_dma.this_many = 1;
  bx_dbg_batch_dma.Qsize     = 0;

  memset(&bx_guard, 0, sizeof(bx_guard));
  bx_guard.async.irq = 1;
  bx_guard.async.dma = 1;

  memset(&bx_debugger, 0, sizeof(bx_debugger));
  bx_debugger.auto_disassemble = 1;
  bx_debugger.disassemble_size = 0;
  bx_debugger.default_display_format = 'x';
  bx_debugger.default_unit_size      = 'w';
  bx_debugger.default_addr = 0;
  bx_debugger.next_bpoint_id = 1;

  // Open debugger log file if needed
  if ((strlen(SIM->get_param_string(BXPN_DEBUGGER_LOG_FILENAME)->getptr()) > 0) 
   && (strcmp(SIM->get_param_string(BXPN_DEBUGGER_LOG_FILENAME)->getptr(), "-") != 0)) {
    debugger_log = fopen(SIM->get_param_string(BXPN_DEBUGGER_LOG_FILENAME)->getptr(), "w");
    if (!debugger_log) {
      BX_PANIC(("Can not open debugger log file '%s'",
        SIM->get_param_string(BXPN_DEBUGGER_LOG_FILENAME)->getptr()));
    }
    else {
      BX_INFO(("Using debugger log file %s",
        SIM->get_param_string(BXPN_DEBUGGER_LOG_FILENAME)->getptr()));
    }
  }

  memset(bx_disasm_ibuf, 0, sizeof(bx_disasm_ibuf));

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

  // Print disassembly of the first instruction...  you wouldn't think it
  // would have to be so hard.  First initialize guard_found, since it is used
  // in the disassembly code to decide what instruction to print.
  for (i=0; i<BX_SMP_PROCESSORS; i++) {
    BX_CPU(i)->guard_found.cs = BX_CPU(i)->sregs[BX_SEG_REG_CS].selector.value;
    BX_CPU(i)->guard_found.eip = BX_CPU(i)->prev_eip;
    BX_CPU(i)->guard_found.laddr =
      (BX_CPU(i)->get_segment_base(BX_SEG_REG_CS) + BX_CPU(i)->prev_eip);
    BX_CPU(i)->guard_found.is_32bit_code =
      (BX_CPU(i)->sregs[BX_SEG_REG_CS].cache.u.segment.d_b);
    BX_CPU(i)->guard_found.is_64bit_code =
      (BX_CPU(i)->get_cpu_mode() == BX_MODE_LONG_64);
  }
  // finally, call the usual function to print the disassembly
  dbg_printf("Next at t=" FMT_LL "d\n", bx_pc_system.time_ticks());
  bx_dbg_disassemble_current(-1, 0);  // all cpus, don't print time

  bx_dbg_user_input_loop();

  if(debugger_log != NULL)
    fclose(debugger_log);

  bx_dbg_exit(0);
  return(0); // keep compiler happy
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

  while(1) {
    SIM->refresh_ci();
    SIM->set_display_mode(DISP_MODE_CONFIG);
    bx_get_command();
reparse:
    if ((*tmp_buf_ptr == '\n') || (*tmp_buf_ptr == 0))
    {
        if ((*tmp_buf_prev != '\n') && (*tmp_buf_prev != 0)) {
          strncpy(tmp_buf, tmp_buf_prev, sizeof(tmp_buf_prev));
          goto reparse;
        }
    }
    else if ((strncmp(tmp_buf_ptr, BX_INCLUDE_CMD, include_cmd_len) == 0) &&
              (tmp_buf_ptr[include_cmd_len] == ' ' ||
               tmp_buf_ptr[include_cmd_len] == '\t'))
    {
      char *ptr = tmp_buf_ptr + include_cmd_len+1;
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
      if ((reti==0) && (bx_infile_stack_index > 0)) {
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
        bx_add_lex_input(tmp_buf_ptr);
        bxparse();
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
    sprintf(prompt, "<bochs:%d> ", bx_infile_stack[bx_infile_stack_index].lineno);
  }
  if (SIM->has_debug_gui() && bx_infile_stack_index == 0) {
    // wait for wxWidgets to send another debugger command
    charptr_ret = SIM->debug_get_next_command();
    if (charptr_ret) {
      strncpy(tmp_buf, charptr_ret, sizeof(tmp_buf));
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
      add_history(charptr_ret);
      strcpy(tmp_buf, charptr_ret);
      strcat(tmp_buf, "\n");
      free(charptr_ret);
      charptr_ret = &tmp_buf[0];
    }
  } else {
    charptr_ret = fgets(tmp_buf, sizeof(tmp_buf),
      bx_infile_stack[bx_infile_stack_index].fp);
  }
#else /* !HAVE_LIBREADLINE */
  else {
    if (bx_infile_stack_index == 0)
      dbg_printf("%s", prompt);
      strncpy(tmp_buf_prev, tmp_buf, sizeof(tmp_buf));
    charptr_ret = fgets(tmp_buf, sizeof(tmp_buf),
      bx_infile_stack[bx_infile_stack_index].fp);
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
      bx_guard.interrupt_requested = 0;
      return;
    }

    dbg_printf("fgets() returned ERROR.\n");
    dbg_printf("intr request was %u\n", bx_guard.interrupt_requested);
    bx_dbg_exit(1);
  }
  tmp_buf_ptr = &tmp_buf[0];

  if (debugger_log != NULL) {
    fprintf(debugger_log, "%s", tmp_buf);
    fflush(debugger_log);
  }

  // look for first non-whitespace character
  while (((*tmp_buf_ptr == ' ') || (*tmp_buf_ptr == '\t')) &&
          (*tmp_buf_ptr != '\n') && (*tmp_buf_ptr != 0) )
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
    dbg_printf("%s: ERROR: unnest_infile(): nesting level = 0.\n",
      argv0);
    bx_dbg_exit(1);
  }

  fclose(bx_infile_stack[bx_infile_stack_index].fp);
  bx_infile_stack_index--;
}

int bxwrap(void)
{
  dbg_printf("%s: ERROR: bxwrap() called.\n", argv0);
  bx_dbg_exit(1);
  return(0); // keep compiler quiet
}

void bxerror(char *s)
{
  dbg_printf("%s:%d: %s at '%s'\n",
    bx_infile_stack[bx_infile_stack_index].fname,
    bx_infile_stack[bx_infile_stack_index].lineno,
    s, bxtext);

  if (bx_infile_stack_index > 0) {
    dbg_printf("%s: ERROR in source file causes exit.\n", argv0);
    bx_dbg_exit(1);
  }
}

void bx_debug_ctrlc_handler(int signum)
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
  bx_guard.interrupt_requested = 1;
}

void bx_dbg_exception(unsigned cpu, Bit8u vector, Bit16u error_code)
{
  if (BX_CPU(dbg_cpu)->trace || bx_dbg.exceptions)
  {
    dbg_printf("CPU %d: Exception 0x%02x occured (error_code=0x%04x)\n", 
      cpu, vector, error_code);
  }
}

void bx_dbg_interrupt(unsigned cpu, Bit8u vector, Bit16u error_code)
{
  if (BX_CPU(dbg_cpu)->trace || bx_dbg.interrupts)
  {
    dbg_printf("CPU %d: Interrupt 0x%02x occured (error_code=0x%04x)\n", 
      cpu, vector, error_code);
  }
}

void bx_dbg_exit(int code)
{
  BX_DEBUG(("dbg: before exit" ));
  for (int cpu=0; cpu < BX_SMP_PROCESSORS; cpu++) {
    if (BX_CPU(cpu)) BX_CPU(cpu)->atexit();
  }

  bx_atexit();
  BX_EXIT(code);
}

//
// commands invoked from parser
//

void bx_dbg_quit_command(void)
{
  BX_INFO(("dbg: Quit"));
  bx_dbg_exit(0);
}

void bx_dbg_trace_command(bx_bool enable)
{
  BX_CPU(dbg_cpu)->trace = enable;
  dbg_printf("Tracing %s for %s\n", enable ? "enabled" : "disabled", 
     BX_CPU(dbg_cpu)->name);
}

void bx_dbg_trace_reg_command(bx_bool enable)
{
  BX_CPU(dbg_cpu)->trace_reg = enable;
  dbg_printf("Register-Tracing %s for %s\n", enable ? "enabled" : "disabled", 
     BX_CPU(dbg_cpu)->name);
}

void bx_dbg_ptime_command(void)
{
  dbg_printf("ptime: " FMT_LL "d\n", bx_pc_system.time_ticks());
}

int timebp_timer = -1;
Bit64u timebp_queue[MAX_CONCURRENT_BPS];
int timebp_queue_size = 0;

void bx_dbg_timebp_command(bx_bool absolute, Bit64u time)
{
  Bit64u abs_time = (absolute) ? time : time + bx_pc_system.time_ticks();

  if (abs_time < bx_pc_system.time_ticks()) {
    dbg_printf("Request for time break point in the past. I can't let you do that.\n");
    return;
  }

  if (timebp_queue_size == MAX_CONCURRENT_BPS) {
    dbg_printf("Too many time break points\n");
    return;
  }

  Bit64u diff = (absolute) ? time - bx_pc_system.time_ticks() : time;

  if (timebp_timer >= 0) {
    if (timebp_queue_size == 0 || abs_time < timebp_queue[0]) {
      /* first in queue */
      for (int i = timebp_queue_size; i >= 0; i--)
        timebp_queue[i+1] = timebp_queue[i];
      timebp_queue[0] = abs_time;
      timebp_queue_size++;
      bx_pc_system.activate_timer_ticks(timebp_timer, diff, 1);
    } else {
      /* not first, insert at suitable place */
      for (int i = 1; i < timebp_queue_size; i++) {
        if (timebp_queue[i] == abs_time) {
          dbg_printf("Time breakpoint not inserted (duplicate)\n");
          return;
        } else if (abs_time < timebp_queue[i]) {
          for (int j = timebp_queue_size; j >= i; j++)
            timebp_queue[j+1] = timebp_queue[j];
          timebp_queue[i] = abs_time;
          goto inserted;
        }
      }
      /* last */
      timebp_queue[timebp_queue_size] = abs_time;
inserted:
      timebp_queue_size++;
    }
  } else {
    timebp_queue_size = 1;
    timebp_queue[0] = abs_time;
    timebp_timer = bx_pc_system.register_timer_ticks(&bx_pc_system, bx_pc_system_c::timebp_handler, diff, 0, 1, "debug.timebp");
  }

  dbg_printf("Time breakpoint inserted. Delta = " FMT_LL "u\n", diff);
}

Bit32u conv_4xBit8u_to_Bit32u(const Bit8u* buf)
{
  Bit32u ret = 0;
  for (int i = 0; i < 4; i++) {
    ret |= (buf[i] << (8 * i));
  }
  return ret;
}

void bx_dbg_record_command(char* path_quoted)
{
  // skip beginning double quote
  if (path_quoted[0] == '"')
    path_quoted++;

  // null out ending quote
  int len = strlen(path_quoted);
  if (path_quoted[len - 1] == '"')
    path_quoted[len - 1] = '\0';

  bx_dbg.record_io = fopen(path_quoted, "w");
  if (bx_dbg.record_io)
    dbg_printf("IO record file '%s' opened\n", path_quoted);
  else
    dbg_printf("Error opening '%s' for writing\n", path_quoted);
}

static FILE* playback_file = 0;

struct playback_entry_t
{
  char command[100];
  Bit32u argument;

  void trigger();
};

static playback_entry_t playback_entry;
static Bit64u last_playback_time = 0;
static int playback_timer_index = -1;

void playback_function(void* this_ptr)
{
  ((playback_entry_t*)this_ptr)->trigger();
}

static void enter_playback_entry()
{
  static const int playback_buf_size = 100;
  char playback_buf[playback_buf_size];
  if (!fgets(playback_buf, playback_buf_size, playback_file))
    return;

  Bit64u time;
  if (sscanf(playback_buf, "%s " FMT_LL "d %x", playback_entry.command, &time, &playback_entry.argument) != 3) {
    dbg_printf("Parse error in playback string '%s'\n", playback_buf);
    return;
  }

  Bit64u diff = time - last_playback_time;
  last_playback_time = time;

  if (time < last_playback_time) {
    BX_PANIC(("Negative diff in playback"));
  } else if (diff == 0) {
    playback_entry.trigger();
  } else {
    if (playback_timer_index >= 0)
      bx_pc_system.activate_timer_ticks(playback_timer_index, diff, 0);
    else
      playback_timer_index = bx_pc_system.register_timer_ticks(&playback_entry, playback_function, diff, 0, 1, "debug.playback");
  }
}

void playback_entry_t::trigger()
{
  if (!strcmp("gen_scancode", command)) {
    DEV_kbd_gen_scancode(argument);
  } else {
    dbg_printf("Unknown playback command '%s'\n", command);
    return;
  }
  enter_playback_entry();
}

void bx_dbg_playback_command(char* path_quoted)
{
  // skip beginning double quote
  if (path_quoted[0] == '"')
    path_quoted++;

  // null out ending quote
  int len = strlen(path_quoted);
  if (path_quoted[len - 1] == '"')
    path_quoted[len - 1] = '\0';

  playback_file = fopen(path_quoted, "r");
  if (playback_file) {
    dbg_printf("Playback from '%s'\n", path_quoted);
    last_playback_time = 0;
    dbg_printf("playback times relative from " FMT_LL "d\n",
        bx_pc_system.time_ticks());
    enter_playback_entry();
  } else {
    dbg_printf("Error opening '%s' for reading\n", path_quoted);
  }
}

// toggles mode switch breakpoint
void bx_dbg_modebp_command()
{
  BX_CPU(dbg_cpu)->dbg_cpu_mode = BX_CPU(dbg_cpu)->get_cpu_mode();
  BX_CPU(dbg_cpu)->mode_break = !BX_CPU(dbg_cpu)->mode_break;
  dbg_printf(" mode switch break %s\n", 
    BX_CPU(dbg_cpu)->mode_break ? "enabled" : "disabled");
}

static bx_bool bx_dbg_read_linear(unsigned which_cpu, bx_address laddr, unsigned len, Bit8u *buf)
{
  unsigned remainsInPage;
  bx_phy_address paddr;
  unsigned read_len;
  bx_bool paddr_valid;

next_page:
  remainsInPage = 0x1000 - (laddr & 0xfff);
  read_len = (remainsInPage < len) ? remainsInPage : len;
  paddr_valid = 0;

  paddr_valid = BX_CPU(which_cpu)->dbg_xlate_linear2phy(laddr, &paddr);
  if (paddr_valid) {
    if (! BX_MEM(0)->dbg_fetch_mem(BX_CPU(which_cpu), paddr, read_len, buf)) {
      dbg_printf("bx_dbg_read_linear: physical memory read error (phy=0x%08x, lin=0x" FMT_ADDRX "\n", paddr, laddr);
      return 0;
    }
  }
  else {
    dbg_printf("bx_dbg_read_linear: physical address not available for linear " FMT_ADDRX "\n", laddr);
    return 0;
  }

  /* check for access across multiple pages */
  if (remainsInPage < len)
  {
    laddr += read_len;
    len -= read_len;
    buf += read_len;
    goto next_page;
  }

  return 1;
}

// where
// stack trace: ebp -> old ebp
// return eip at ebp + 4
void bx_dbg_where_command()
{
  if (!BX_CPU(dbg_cpu)->protected_mode()) {
    dbg_printf("'where' only supported in protected mode\n");
    return;
  }
  if (BX_CPU(dbg_cpu)->get_segment_base(BX_SEG_REG_SS) != 0) {
    dbg_printf("non-zero stack base\n");
    return;
  }
  Bit32u bp = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_EBP);
  Bit32u ip = BX_CPU(dbg_cpu)->get_ip();
  dbg_printf("(%d) 0x%08x\n", 0, ip);
  for (int i = 1; i < 50; i++) {
    // Up
    Bit32u paddr;
    Bit8u buf[4];

    // bp = [bp];
    bx_bool paddr_valid = BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(bp, &paddr);
    if (paddr_valid) {
      if (BX_MEM(0)->dbg_fetch_mem(BX_CPU(dbg_cpu), paddr, 4, buf)) {
        bp = conv_4xBit8u_to_Bit32u(buf);
      } else {
        dbg_printf("(%d) Physical memory read error (BP)\n", i);
        break;
      }
    } else {
      dbg_printf("(%d) Could not translate linear address (BP)\n", i);
      break;
    }

    // ip = [bp + 4];
    paddr_valid = BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(bp + 4, &paddr);
    if (paddr_valid) {
      if (BX_MEM(0)->dbg_fetch_mem(BX_CPU(dbg_cpu), paddr, 4, buf)) {
        ip = conv_4xBit8u_to_Bit32u(buf);
      } else {
        dbg_printf("(%d) Physical memory read error (IP)\n", i);
        break;
      }
    } else {
      dbg_printf("(%d) Could not translate linear address (IP)\n", i);
      break;
    }

    // Print
    dbg_printf("(%d) 0x%08x\n", i, ip);
  }
}

void bx_dbg_print_string_command(bx_address start_addr)
{
  dbg_printf("0x%08x: ", start_addr);
  for (int i = 0; ; i++) {
    Bit8u buf = 0;
    if (! bx_dbg_read_linear(dbg_cpu, start_addr+i, 1, &buf)) break;
    if (buf == 0) break;
    if (isgraph(buf) || buf == 0x20)
      dbg_printf("%c", buf);
    else
      dbg_printf("\\%d", buf);
  }
  dbg_printf("\n");
}

static void dbg_print_guard_found(unsigned cpu_mode, Bit32u cs, bx_address eip, bx_address laddr)
{
#if BX_SUPPORT_X86_64
  if (cpu_mode == BX_MODE_LONG_64) {
    dbg_printf("0x%04x:" FMT_ADDRX " (0x" FMT_ADDRX ")", cs, eip, laddr);
    return;
  }
#endif

  if (cpu_mode >= BX_MODE_IA32_PROTECTED)
    dbg_printf("%04x:%08x (0x%08x)", cs, (unsigned) eip, (unsigned) laddr);
  else // real or v8086 mode
    dbg_printf("%04x:%04x (0x%08x)", cs, (unsigned) eip, (unsigned) laddr);
}

unsigned dbg_show_mask = 0;

#define BX_DBG_SHOW_CALLRET  (Flag_call|Flag_ret)
#define BX_DBG_SHOW_SOFTINT  (Flag_softint)
#define BX_DBG_SHOW_EXTINT   (Flag_intsig)
#define BX_DBG_SHOW_IRET     (Flag_iret)
#define BX_DBG_SHOW_INT      (Flag_softint|Flag_iret|Flag_intsig)
#define BX_DBG_SHOW_MODE     (Flag_mode)

void bx_dbg_show_command(const char* arg)
{
  if(arg) {
    if (!strcmp(arg, "mode")) {
      if (dbg_show_mask & BX_DBG_SHOW_MODE) {
        dbg_show_mask &= ~BX_DBG_SHOW_MODE;
        dbg_printf("show mode switch: OFF\n");
      } else {
        dbg_show_mask |= BX_DBG_SHOW_MODE;
        dbg_printf("show mode switch: ON\n");
      }
    } else if (!strcmp(arg, "int")) {
      if (dbg_show_mask & BX_DBG_SHOW_INT) {
        dbg_show_mask &= ~BX_DBG_SHOW_INT;
        dbg_printf("show interrupts tracing (extint/softint/iret): OFF\n");
      } else {
        dbg_show_mask |= BX_DBG_SHOW_INT;
        dbg_printf("show interrupts tracing (extint/softint/iret): ON\n");
      }
    } else if (!strcmp(arg, "extint")) {
      if (dbg_show_mask & BX_DBG_SHOW_EXTINT) {
        dbg_show_mask &= ~BX_DBG_SHOW_EXTINT;
        dbg_printf("show external interrupts: OFF\n");
      } else {
        dbg_show_mask |= BX_DBG_SHOW_EXTINT;
        dbg_printf("show external interrupts: ON\n");
      }
    } else if (!strcmp(arg, "softint")) {
      if (dbg_show_mask & BX_DBG_SHOW_SOFTINT) {
        dbg_show_mask &= ~BX_DBG_SHOW_SOFTINT;
        dbg_printf("show software interrupts: OFF\n");
      } else {
        dbg_show_mask |= BX_DBG_SHOW_SOFTINT;
        dbg_printf("show software interrupts: ON\n");
      }
    } else if (!strcmp(arg, "iret")) {
      if (dbg_show_mask & BX_DBG_SHOW_IRET) {
        dbg_show_mask &= ~BX_DBG_SHOW_IRET;
        dbg_printf("show iret: OFF\n");
      } else {
        dbg_show_mask |= BX_DBG_SHOW_IRET;
        dbg_printf("show iret: ON\n");
      }
    } else if(!strcmp(arg,"call")) {
      if (dbg_show_mask & BX_DBG_SHOW_CALLRET) {
        dbg_show_mask &= ~BX_DBG_SHOW_CALLRET;
        dbg_printf("show calls/returns: OFF\n");
      } else {
        dbg_show_mask |= BX_DBG_SHOW_CALLRET;
        dbg_printf("show calls/returns: ON\n");
      }
    } else if(!strcmp(arg,"off")) {
      dbg_show_mask = 0x0;
      dbg_printf("Disable all show flags\n");
    } else if(!strcmp(arg,"dbg-all")) {
      bx_dbg.floppy = 1;
      bx_dbg.keyboard = 1;
      bx_dbg.video = 1;
      bx_dbg.disk = 1;
      bx_dbg.pit = 1;
      bx_dbg.pic = 1;
      bx_dbg.bios = 1;
      bx_dbg.cmos = 1;
      bx_dbg.a20 = 1;
      bx_dbg.interrupts = 1;
      bx_dbg.exceptions = 1;
      bx_dbg.unsupported = 1;
      bx_dbg.temp = 1;
      bx_dbg.reset = 1;
      bx_dbg.mouse = 1;
      bx_dbg.io = 1;
      bx_dbg.debugger = 1;
      bx_dbg.xms = 1;
      bx_dbg.v8086 = 1;
      bx_dbg.paging = 1;
      bx_dbg.creg = 1;
      bx_dbg.dreg = 1;
      bx_dbg.dma = 1;
      bx_dbg.unsupported_io = 1;
      /* bx_dbg.record_io = 1; this is a pointer .. somewhere */
      dbg_printf("Turned ON all bx_dbg flags\n");
      return;
    } else if(!strcmp(arg,"dbg-none")) {
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
      /* bx_dbg.record_io = 0; this is a pointer .. somewhere */
      dbg_printf("Turned OFF all bx_dbg flags\n");
      return;
    } else if(!strcmp(arg,"vga")){
      DEV_vga_refresh();
      return;
    } else {
      dbg_printf("Unrecognized arg: %s (only 'mode', 'int', 'softint', 'extint', 'iret', 'call', 'off', 'dbg-all' and 'dbg-none' are valid)\n", arg);
      return;
    }
  }

  if (dbg_show_mask) {
    dbg_printf("show mask is:");
    if (dbg_show_mask & BX_DBG_SHOW_CALLRET)
      dbg_printf(" call");
    if (dbg_show_mask & BX_DBG_SHOW_SOFTINT)
      dbg_printf(" softint");
    if (dbg_show_mask & BX_DBG_SHOW_EXTINT)
      dbg_printf(" extint");
    if (dbg_show_mask & BX_DBG_SHOW_IRET)
      dbg_printf(" iret");
    if (dbg_show_mask & BX_DBG_SHOW_MODE)
      dbg_printf(" mode");
    dbg_printf("\n");
  }
  else {
    dbg_printf("show mask is: 0\n");
  }
}

void bx_dbg_show_param_command(char *param)
{
#if BX_SUPPORT_SAVE_RESTORE
  // remove leading and trailing quotas
  if (param[0]=='\"') param++;
  unsigned len = strlen(param);
  if (param[len - 1] == '\"') param[len - 1] = '\0';
  dbg_printf("show param name: <%s>\n", param);
  print_tree(SIM->get_param(param, SIM->get_sr_root()), 0);
#else
  dbg_printf("You must compile with save/restore to use this command !\n");
#endif
}

// return non zero to cause a stop
int bx_dbg_show_symbolic(void) 
{
  static unsigned   last_cpu_mode = 0;
  static bx_address last_cr3 = 0;

  /* modes & address spaces */
  if (dbg_show_mask & BX_DBG_SHOW_MODE) {
    if(BX_CPU(dbg_cpu)->get_cpu_mode() != last_cpu_mode) {
      dbg_printf (FMT_TICK ": switched from '%s' to '%s'\n", 
        bx_pc_system.time_ticks(),
        cpu_mode_string(last_cpu_mode),
        cpu_mode_string(BX_CPU(dbg_cpu)->get_cpu_mode()));
    }

    if(last_cr3 != BX_CPU(dbg_cpu)->cr3)
      dbg_printf(FMT_TICK ": address space switched. CR3: 0x" FMT_ADDRX "\n", 
        bx_pc_system.time_ticks(), BX_CPU(dbg_cpu)->cr3);
  }

  /* interrupts */
  if (dbg_show_mask & BX_DBG_SHOW_SOFTINT) {
    if(BX_CPU(dbg_cpu)->show_flag & Flag_softint) {
      dbg_printf(FMT_TICK ": softint ", bx_pc_system.time_ticks());
      dbg_print_guard_found(BX_CPU(dbg_cpu)->get_cpu_mode(),
        BX_CPU(dbg_cpu)->guard_found.cs, BX_CPU(dbg_cpu)->guard_found.eip,
        BX_CPU(dbg_cpu)->guard_found.laddr);
      dbg_printf("\n");
    }
  }
  
  if (dbg_show_mask & BX_DBG_SHOW_EXTINT) {
    if((BX_CPU(dbg_cpu)->show_flag & Flag_intsig) && !(BX_CPU(dbg_cpu)->show_flag & Flag_softint)) {
      dbg_printf(FMT_TICK ": exception (not softint) ", bx_pc_system.time_ticks());
      dbg_print_guard_found(BX_CPU(dbg_cpu)->get_cpu_mode(),
        BX_CPU(dbg_cpu)->guard_found.cs, BX_CPU(dbg_cpu)->guard_found.eip,
        BX_CPU(dbg_cpu)->guard_found.laddr);
      dbg_printf("\n");
    }
  }

  if (dbg_show_mask & BX_DBG_SHOW_IRET) {
    if(BX_CPU(dbg_cpu)->show_flag & Flag_iret) {
      dbg_printf(FMT_TICK ": iret ", bx_pc_system.time_ticks());
      dbg_print_guard_found(BX_CPU(dbg_cpu)->get_cpu_mode(),
        BX_CPU(dbg_cpu)->guard_found.cs, BX_CPU(dbg_cpu)->guard_found.eip,
        BX_CPU(dbg_cpu)->guard_found.laddr);
      dbg_printf("\n");
    }
  }

  /* calls */
  if (dbg_show_mask & BX_DBG_SHOW_CALLRET)
  {
    if(BX_CPU(dbg_cpu)->show_flag & Flag_call) {
      bx_phy_address phy = 0;
      bx_bool valid = BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(BX_CPU(dbg_cpu)->guard_found.laddr, &phy);
      dbg_printf(FMT_TICK ": call ", bx_pc_system.time_ticks());
      dbg_print_guard_found(BX_CPU(dbg_cpu)->get_cpu_mode(),
        BX_CPU(dbg_cpu)->guard_found.cs, BX_CPU(dbg_cpu)->guard_found.eip,
        BX_CPU(dbg_cpu)->guard_found.laddr);
      if (!valid) dbg_printf(" phys not valid");
      else {
        dbg_printf(" (phy: 0x%08x) %s", phy,
          bx_dbg_symbolic_address(BX_CPU(dbg_cpu)->cr3,
              BX_CPU(dbg_cpu)->guard_found.eip,
              BX_CPU(dbg_cpu)->guard_found.laddr - BX_CPU(dbg_cpu)->guard_found.eip));
      }
      dbg_printf("\n");
    }
  }

  last_cr3 = BX_CPU(dbg_cpu)->cr3;
  last_cpu_mode = BX_CPU(dbg_cpu)->get_cpu_mode();
  BX_CPU(dbg_cpu)->show_flag = 0;

  return 0;
}

void bx_dbg_print_stack_command(unsigned nwords)
{
  bx_address linear_sp;
  unsigned len;

#if BX_SUPPORT_X86_64
  if (BX_CPU(dbg_cpu)->get_cpu_mode() == BX_MODE_LONG_64) {
    linear_sp = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_RSP);
    len = 8;
  }
  else 
#endif
  {
    if (BX_CPU(dbg_cpu)->sregs[BX_SEG_REG_SS].cache.u.segment.d_b) {
      linear_sp = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_ESP);
      len = 4;
    }
    else {
      linear_sp = BX_CPU(dbg_cpu)->get_reg32(BX_16BIT_REG_SP);
      len = 2;
    }
  }

  Bit8u buf[8];

  dbg_printf("Stack address size %d\n", len);

  for (unsigned i = 0; i < nwords; i++) {
    if (! bx_dbg_read_linear(dbg_cpu, linear_sp, len, buf)) break;
#if BX_SUPPORT_X86_64
    if (len == 8) {
      dbg_printf(" | STACK 0x%08x%08x [0x%08x:0x%08x]\n", 
        GET32H(linear_sp), GET32L(linear_sp),
        (unsigned) conv_4xBit8u_to_Bit32u(buf+4), 
        (unsigned) conv_4xBit8u_to_Bit32u(buf));
    }
    else
#endif
    {
      if (len == 4) {
        dbg_printf(" | STACK 0x%08x [0x%08x]\n", 
          (unsigned) linear_sp, (unsigned) conv_4xBit8u_to_Bit32u(buf));
      }
      else {
        dbg_printf(" | STACK 0x%04x [0x%04x]\n", 
          (unsigned) linear_sp, (unsigned) conv_4xBit8u_to_Bit32u(buf));
      }
    }

    linear_sp += len;
  }
}

int num_write_watchpoints = 0;
int num_read_watchpoints = 0;
Bit32u write_watchpoint[MAX_WRITE_WATCHPOINTS];
Bit32u read_watchpoint[MAX_READ_WATCHPOINTS];
bx_bool watchpoint_continue = 0;

void bx_dbg_watch(int read, Bit32u address)
{
  if (read == -1) {
    // print watch point info
    int i;
    for (i = 0; i < num_read_watchpoints; i++) {
      Bit8u buf[2];
      if (BX_MEM(0)->dbg_fetch_mem(BX_CPU(dbg_cpu), read_watchpoint[i], 2, buf))
        dbg_printf("read   %08x   (%04x)\n",
            read_watchpoint[i], (int)buf[0] | ((int)buf[1] << 8));
      else
        dbg_printf("read   %08x   (read error)\n", read_watchpoint[i]);
    }
  for (i = 0; i < num_write_watchpoints; i++) {
    Bit8u buf[2];
    if (BX_MEM(0)->dbg_fetch_mem(BX_CPU(dbg_cpu), write_watchpoint[i], 2, buf))
      dbg_printf("write  %08x   (%04x)\n", write_watchpoint[i], (int)buf[0] | ((int)buf[1] << 8));
    else
      dbg_printf("write  %08x   (read error)\n", write_watchpoint[i]);
  }
  } else {
    if (read) {
        if (num_read_watchpoints == MAX_READ_WATCHPOINTS) {
        dbg_printf("Too many read watchpoints\n");
        return;
      }
      read_watchpoint[num_read_watchpoints++] = address;
      dbg_printf("Read watchpoint at %08x inserted\n", address);
    } else {
      if (num_write_watchpoints == MAX_WRITE_WATCHPOINTS) {
        dbg_printf("Too many write watchpoints\n");
        return;
      }
      write_watchpoint[num_write_watchpoints++] = address;
      dbg_printf("Write watchpoint at %08x inserted\n", address);
    }
  }
}

void bx_dbg_unwatch(int read, Bit32u address)
{
  if (read == -1) {
    // unwatch all
    num_read_watchpoints = num_write_watchpoints = 0;
    dbg_printf("All watchpoints removed\n");
  } else {
    if (read) {
      dbg_printf("Watchpoint remove not implemented\n");
    } else {
      dbg_printf("Watchpoint remove not implemented\n");
    }
  }
}

void bx_dbg_continue_command(void)
{
  // continue executing, until a guard found

one_more:

  // I must guard for ICOUNT or one CPU could run forever without giving
  // the others a chance.
  bx_guard.guard_for |= BX_DBG_GUARD_CTRL_C; // stop on Ctrl-C

  // update gui (disable continue command, enable stop command, etc.)
  sim_running->set(1);
  SIM->refresh_ci();

  // use simulation mode while executing instructions.  When the prompt
  // is printed, we will return to config mode.
  SIM->set_display_mode(DISP_MODE_SIM);

  bx_guard.interrupt_requested = 0;
  int stop = 0;
  int which = -1;
  while (!stop && !bx_guard.interrupt_requested) {
    // the quantum is an arbitrary number of cycles to run in each
    // processor.  In SMP mode, when this limit is reached, the
    // cpu_loop exits so that another processor can be simulated
    // for a few cycles.  With a single processor, the quantum
    // setting should have no effect, although a low setting does
    // lead to poor performance because cpu_loop is returning and 
    // getting called again, over and over.
    int quantum = BX_DBG_DEFAULT_ICOUNT_QUANTUM;
    int cpu;
    for (cpu=0; cpu < BX_SMP_PROCESSORS; cpu++) {
      BX_CPU(cpu)->guard_found.guard_found = 0;
      BX_CPU(cpu)->guard_found.icount = 0;
      BX_CPU(cpu)->guard_found.time_tick = bx_pc_system.time_ticks();
      BX_CPU(cpu)->cpu_loop(quantum);
      // set stop flag if a guard found other than icount or halted
      unsigned long found = BX_CPU(cpu)->guard_found.guard_found;
      stop_reason_t reason = (stop_reason_t) BX_CPU(cpu)->stop_reason;
      if (found) {
        stop = 1;
        which = cpu;
      } else if (reason != STOP_NO_REASON && reason != STOP_CPU_HALTED) {
        stop = 1;
        which = cpu;
      }
      // even if stop==1, finish cycling through all processors.
      // "which" remembers which cpu set the stop flag.  If multiple
      // cpus set stop, too bad.
    }

    // increment time tick only after all processors have had their chance.
    if (BX_SMP_PROCESSORS == 1) {
      // all ticks are handled inside the cpu loop
    }
    else {
      // We must tick by the number of instructions that were
      // ACTUALLY executed, not the number that we asked it to
      // execute.  Even this is tricky with SMP because one might
      // have hit a breakpoint, while others executed the whole
      // quantum.
      int max_executed = 0;
      for (cpu=0; cpu<BX_SMP_PROCESSORS; cpu++) {
        if (BX_CPU(cpu)->guard_found.icount > max_executed)
          max_executed = BX_CPU(cpu)->guard_found.icount;
      }
      // potential deadlock if all processors are halted.  Then 
      // max_executed will be 0, tick will be incremented by zero, and
      // there will never be a timed event to wake them up.  To avoid this,
      // always tick by a minimum of 1.
      if (max_executed < 1) max_executed=1;

      BX_TICKN(max_executed);
    }
  }

  sim_running->set(0);
  SIM->refresh_ci();

  // (mch) hack
  DEV_vga_refresh();

  BX_INSTR_DEBUG_PROMPT();
  bx_dbg_print_guard_results();

  if (watchpoint_continue && (BX_CPU(which)->stop_reason == STOP_READ_WATCH_POINT ||
            BX_CPU(which)->stop_reason == STOP_WRITE_WATCH_POINT))
  goto one_more;
}

void bx_dbg_stepN_command(Bit32u count)
{
  if (count == 0) {
    dbg_printf("Error: stepN: count=0\n");
    return;
  }

  // use simulation mode while executing instructions.  When the prompt
  // is printed, we will return to config mode.
  SIM->set_display_mode(DISP_MODE_SIM);

  // single CPU
  bx_guard.guard_for |= BX_DBG_GUARD_CTRL_C; // or Ctrl-C
  // for now, step each CPU one instruction at a time
  for (unsigned cycle=0; cycle < count; cycle++) {
    for (unsigned cpu=0; cpu < BX_SMP_PROCESSORS; cpu++) {
      bx_guard.interrupt_requested = 0;
      BX_CPU(cpu)->guard_found.guard_found = 0;
      BX_CPU(cpu)->guard_found.icount = 0;
      BX_CPU(cpu)->guard_found.time_tick = bx_pc_system.time_ticks();
      BX_CPU(cpu)->cpu_loop(1);
    }
#if BX_SUPPORT_SMP == 0
    // ticks are handled inside the cpu loop
#else
    BX_TICK1();
#endif
  }

  BX_INSTR_DEBUG_PROMPT();
  bx_dbg_print_guard_results();
}

void bx_dbg_disassemble_current(int which_cpu, int print_time)
{
  bx_phy_address phy;

  if (which_cpu < 0) {
    // iterate over all of them.
    for (int i=0; i<BX_SMP_PROCESSORS; i++)
      bx_dbg_disassemble_current(i, print_time);
    return;
  }

  bx_bool phy_valid = BX_CPU(which_cpu)->dbg_xlate_linear2phy(BX_CPU(which_cpu)->guard_found.laddr, &phy);
  if (! phy_valid) {
    dbg_printf("(%u).[" FMT_LL "d] ??? (physical address not available)\n", which_cpu, bx_pc_system.time_ticks());
    return;
  }

  if (bx_dbg_read_linear(which_cpu, BX_CPU(which_cpu)->guard_found.laddr, 16, bx_disasm_ibuf))
  {
    unsigned ilen = bx_disassemble.disasm(BX_CPU(which_cpu)->guard_found.is_32bit_code,
      BX_CPU(which_cpu)->guard_found.is_64bit_code,
      BX_CPU(which_cpu)->get_segment_base(BX_SEG_REG_CS), 
      BX_CPU(which_cpu)->guard_found.eip, bx_disasm_ibuf, bx_disasm_tbuf);

    // Note: it would be nice to display only the modified registers here, the easy
    // way out I have thought of would be to keep a prev_eax, prev_ebx, etc copies
    // in each cpu description (see cpu/cpu.h) and update/compare those "prev" values
    // from here. (eks)
    if(BX_CPU(dbg_cpu)->trace_reg)
      bx_dbg_info_registers_command(BX_INFO_CPU_REGS);

    if (print_time)
      dbg_printf("(%u).[" FMT_LL "d] ", which_cpu, bx_pc_system.time_ticks());
    else
      dbg_printf("(%u) ", which_cpu);

    if (BX_CPU(which_cpu)->protected_mode()) {
      dbg_printf("[0x%08x] %04x:" FMT_ADDRX " (%s): ", 
        phy, BX_CPU(which_cpu)->guard_found.cs,
        BX_CPU(which_cpu)->guard_found.eip,
        bx_dbg_symbolic_address((BX_CPU(which_cpu)->cr3) >> 12, 
           BX_CPU(which_cpu)->guard_found.eip, 
           BX_CPU(which_cpu)->get_segment_base(BX_SEG_REG_CS)));
    }
    else { // Real & V86 mode
      dbg_printf("[0x%08x] %04x:%04x (%s): ", 
        phy, BX_CPU(which_cpu)->guard_found.cs,
        (unsigned) BX_CPU(which_cpu)->guard_found.eip,
        bx_dbg_symbolic_address_16bit(BX_CPU(which_cpu)->guard_found.eip, 
          BX_CPU(which_cpu)->sregs[BX_SEG_REG_CS].selector.value));
    }
    dbg_printf("%-25s ; ", bx_disasm_tbuf);
    for (unsigned j=0; j<ilen; j++)
      dbg_printf("%02x", (unsigned) bx_disasm_ibuf[j]);
    dbg_printf("\n");
  }
}

void bx_dbg_print_guard_results(void)
{
  unsigned cpu, i;

  for (cpu=0; cpu<BX_SMP_PROCESSORS; cpu++) {
    unsigned long found = BX_CPU(cpu)->guard_found.guard_found;
    if (found & BX_DBG_GUARD_CTRL_C) { /* ... */ }
#if BX_DBG_SUPPORT_VIR_BPOINT
    else if (found & BX_DBG_GUARD_IADDR_VIR) {
      i = BX_CPU(cpu)->guard_found.iaddr_index;
      dbg_printf("(%u) Breakpoint %u, in ");
      dbg_print_guard_found(BX_CPU(dbg_cpu)->get_cpu_mode(),
            BX_CPU(cpu)->guard_found.cs, BX_CPU(cpu)->guard_found.eip,
            BX_CPU(cpu)->guard_found.laddr);
      dbg_printf("\n");
    }
#endif
#if BX_DBG_SUPPORT_LIN_BPOINT
    else if (found & BX_DBG_GUARD_IADDR_LIN) {
      i = BX_CPU(cpu)->guard_found.iaddr_index;
      if (bx_guard.iaddr.lin[i].bpoint_id != 0)
        dbg_printf("(%u) Breakpoint %u, 0x" FMT_ADDRX " in ?? ()\n",
            cpu,
            bx_guard.iaddr.lin[i].bpoint_id,
            BX_CPU(cpu)->guard_found.laddr);
    }
#endif
#if BX_DBG_SUPPORT_PHY_BPOINT
    else if (found & BX_DBG_GUARD_IADDR_PHY) {
      i = BX_CPU(cpu)->guard_found.iaddr_index;
      dbg_printf("(%u) Breakpoint %u, 0x" FMT_ADDRX " in ?? ()\n",
            cpu,
            bx_guard.iaddr.phy[i].bpoint_id,
            BX_CPU(cpu)->guard_found.laddr);
    }
#endif
    switch(BX_CPU(cpu)->stop_reason) {
    case STOP_NO_REASON:
    case STOP_CPU_HALTED:
        break;
    case STOP_TIME_BREAK_POINT:
        dbg_printf("(%u) Caught time breakpoint\n", cpu);
        break;
    case STOP_READ_WATCH_POINT:
        dbg_printf("(%u) Caught read watch point at %08X\n", cpu, BX_CPU(cpu)->watchpoint);
        break;
    case STOP_WRITE_WATCH_POINT:
        dbg_printf("(%u) Caught write watch point at %08X\n", cpu, BX_CPU(cpu)->watchpoint);
        break;
    case STOP_MAGIC_BREAK_POINT:
        dbg_printf("(%u) Magic breakpoint\n", cpu);
        break;
    case STOP_MODE_BREAK_POINT:
        dbg_printf("(%u) Caught mode switch breakpoint switching to '%s'\n",
          cpu, cpu_mode_string(BX_CPU(cpu)->get_cpu_mode()));
        break;
    default:
        dbg_printf("Error: (%u) print_guard_results: guard_found ? (stop reason %u)\n",
          cpu, BX_CPU(cpu)->stop_reason);
    }

    if (bx_debugger.auto_disassemble) {
      if (cpu==0) {
        // print this only once
        dbg_printf("Next at t=" FMT_LL "d\n", bx_pc_system.time_ticks ());
      }
      bx_dbg_disassemble_current(cpu, 0);  // one cpu, don't print time
    }
  }
#if 0
  // print the TSC value for every CPU
  for (cpu=0; cpu<BX_SMP_PROCESSORS; cpu++) {
    dbg_printf("TSC[%d] = " FMT_LL "d\n", cpu, BX_CPU(cpu)->tsc);
  }
#endif
}

void bx_dbg_breakpoint_changed(void)
{
#if BX_DBG_SUPPORT_VIR_BPOINT
  if (bx_guard.iaddr.num_virtual)
    bx_guard.guard_for |= BX_DBG_GUARD_IADDR_VIR;
  else
    bx_guard.guard_for &= ~BX_DBG_GUARD_IADDR_VIR;
#endif

#if BX_DBG_SUPPORT_LIN_BPOINT
  if (bx_guard.iaddr.num_linear)
    bx_guard.guard_for |= BX_DBG_GUARD_IADDR_LIN;
  else
    bx_guard.guard_for &= ~BX_DBG_GUARD_IADDR_LIN;
#endif

#if BX_DBG_SUPPORT_PHY_BPOINT
  if (bx_guard.iaddr.num_physical)
    bx_guard.guard_for |= BX_DBG_GUARD_IADDR_PHY;
  else
    bx_guard.guard_for &= ~BX_DBG_GUARD_IADDR_PHY;
#endif
}

void bx_dbg_en_dis_breakpoint_command(unsigned handle, bx_bool enable)
{
#if BX_DBG_SUPPORT_VIR_BPOINT
  if (bx_dbg_en_dis_vbreak(handle, enable))
   goto done;
#endif

#if BX_DBG_SUPPORT_LIN_BPOINT
  if (bx_dbg_en_dis_lbreak(handle, enable))
   goto done;
#endif

#if BX_DBG_SUPPORT_PHY_BPOINT
  if (bx_dbg_en_dis_pbreak(handle, enable))
      goto done;
#endif

  dbg_printf("Error: breakpoint %u not found.\n", handle);
  return;

done:
  bx_dbg_breakpoint_changed();
}

bx_bool bx_dbg_en_dis_pbreak(unsigned handle, bx_bool enable)
{
#if BX_DBG_SUPPORT_PHY_BPOINT
  // see if breakpoint is a physical breakpoint
  for (unsigned i=0; i<bx_guard.iaddr.num_physical; i++) {
    if (bx_guard.iaddr.phy[i].bpoint_id == handle) {
      bx_guard.iaddr.phy[i].enabled=enable;
      return (bx_bool)true;
    }
  }
#endif
  return (bx_bool)false;
}

bx_bool bx_dbg_en_dis_lbreak(unsigned handle, bx_bool enable)
{
#if BX_DBG_SUPPORT_LIN_BPOINT
  // see if breakpoint is a linear breakpoint
  for (unsigned i=0; i<bx_guard.iaddr.num_linear; i++) {
    if (bx_guard.iaddr.lin[i].bpoint_id == handle) {
      bx_guard.iaddr.lin[i].enabled=enable;
      return 1;
    }
  }
#endif
  return 0;
}

bx_bool bx_dbg_en_dis_vbreak(unsigned handle, bx_bool enable)
{
#if BX_DBG_SUPPORT_VIR_BPOINT
  // see if breakpoint is a virtual breakpoint
  for (unsigned i=0; i<bx_guard.iaddr.num_virtual; i++) {
    if (bx_guard.iaddr.vir[i].bpoint_id == handle) {
      bx_guard.iaddr.vir[i].enabled=enable;
      return 1;
    }
  }
#endif
  return 0;
}

void bx_dbg_del_breakpoint_command(unsigned handle)
{
#if BX_DBG_SUPPORT_VIR_BPOINT
  if (bx_dbg_del_vbreak(handle))
   goto done;
#endif

#if BX_DBG_SUPPORT_LIN_BPOINT
  if (bx_dbg_del_lbreak(handle))
   goto done;
#endif

#if BX_DBG_SUPPORT_PHY_BPOINT
  if (bx_dbg_del_pbreak(handle))
   goto done;
#endif

  dbg_printf("Error: breakpoint %u not found.\n", handle);
  return;

done:
  bx_dbg_breakpoint_changed();
}

bx_bool bx_dbg_del_pbreak(unsigned handle)
{
#if BX_DBG_SUPPORT_PHY_BPOINT
  // see if breakpoint is a physical breakpoint
  for (unsigned i=0; i<bx_guard.iaddr.num_physical; i++) {
    if (bx_guard.iaddr.phy[i].bpoint_id == handle) {
      // found breakpoint, delete it by shifting remaining entries left
      for (unsigned j=i; j<(bx_guard.iaddr.num_physical-1); j++) {
        bx_guard.iaddr.phy[j] = bx_guard.iaddr.phy[j+1];
      }
      bx_guard.iaddr.num_physical--;
      return 1;
    }
  }
#endif
  return 0;
}

bx_bool bx_dbg_del_lbreak(unsigned handle)
{
#if BX_DBG_SUPPORT_LIN_BPOINT
  // see if breakpoint is a linear breakpoint
  for (unsigned i=0; i<bx_guard.iaddr.num_linear; i++) {
    if (bx_guard.iaddr.lin[i].bpoint_id == handle) {
      // found breakpoint, delete it by shifting remaining entries left
      for (unsigned j=i; j<(bx_guard.iaddr.num_linear-1); j++) {
        bx_guard.iaddr.lin[j] = bx_guard.iaddr.lin[j+1];
      }
      bx_guard.iaddr.num_linear--;
      return 1;
    }
  }
#endif
  return 0;
}

bx_bool bx_dbg_del_vbreak(unsigned handle)
{
#if BX_DBG_SUPPORT_VIR_BPOINT
  // see if breakpoint is a virtual breakpoint
  for (unsigned i=0; i<bx_guard.iaddr.num_virtual; i++) {
    if (bx_guard.iaddr.vir[i].bpoint_id == handle) {
      // found breakpoint, delete it by shifting remaining entries left
      for (unsigned j=i; j<(bx_guard.iaddr.num_virtual-1); j++) {
        bx_guard.iaddr.vir[j] = bx_guard.iaddr.vir[j+1];
      }
      bx_guard.iaddr.num_virtual--;
      return 1;
    }
  }
#endif
  return 0;
}

int bx_dbg_vbreakpoint_command(BreakpointKind bk, Bit32u cs, bx_address eip)
{
#if BX_DBG_SUPPORT_VIR_BPOINT
  if (bk != bkRegular) {
    dbg_printf("Error: vbreak of this kind not implemented yet.\n");
    return -1;
  }

  if (bx_guard.iaddr.num_virtual >= BX_DBG_MAX_VIR_BPOINTS) {
    dbg_printf("Error: no more virtual breakpoint slots left.\n");
    dbg_printf("Error: see BX_DBG_MAX_VIR_BPOINTS.\n");
    return -1;
  }

  bx_guard.iaddr.vir[bx_guard.iaddr.num_virtual].cs  = cs;
  bx_guard.iaddr.vir[bx_guard.iaddr.num_virtual].eip = eip;
  bx_guard.iaddr.vir[bx_guard.iaddr.num_virtual].bpoint_id = bx_debugger.next_bpoint_id++;
  int BpId = (int)bx_guard.iaddr.vir[bx_guard.iaddr.num_virtual].bpoint_id;
  bx_guard.iaddr.vir[bx_guard.iaddr.num_virtual].enabled=1;
  bx_guard.iaddr.num_virtual++;
  bx_guard.guard_for |= BX_DBG_GUARD_IADDR_VIR;
  return BpId;

#else
  dbg_printf("Error: virtual breakpoint support not compiled in.\n");
  dbg_printf("Error: see BX_DBG_SUPPORT_VIR_BPOINT.\n");
  return -1;
#endif
}

int bx_dbg_lbreakpoint_command(BreakpointKind bk, bx_address laddress)
{
#if BX_DBG_SUPPORT_LIN_BPOINT
  if (bk == bkAtIP) {
    dbg_printf("Error: lbreak of this kind not implemented yet.\n");
    return -1;
  }

  if (bx_guard.iaddr.num_linear >= BX_DBG_MAX_LIN_BPOINTS) {
    dbg_printf("Error: no more linear breakpoint slots left.\n");
    dbg_printf("Error: see BX_DBG_MAX_LIN_BPOINTS.\n");
    return -1;
  }

  bx_guard.iaddr.lin[bx_guard.iaddr.num_linear].addr = laddress;
  int BpId = (bk == bkStepOver) ? 0 : bx_debugger.next_bpoint_id++;
  bx_guard.iaddr.lin[bx_guard.iaddr.num_linear].bpoint_id = BpId;
  bx_guard.iaddr.lin[bx_guard.iaddr.num_linear].enabled=1;
  bx_guard.iaddr.num_linear++;
  bx_guard.guard_for |= BX_DBG_GUARD_IADDR_LIN;
  return BpId;

#else
  dbg_printf("Error: linear breakpoint support not compiled in.\n");
  dbg_printf("Error: see BX_DBG_SUPPORT_LIN_BPOINT.\n");
  return -1;
#endif
}

int bx_dbg_pbreakpoint_command(BreakpointKind bk, Bit32u paddress)
{
#if BX_DBG_SUPPORT_PHY_BPOINT
  if (bk != bkRegular) {
    dbg_printf("Error: pbreak of this kind not implemented yet.\n");
    return -1;
  }

  if (bx_guard.iaddr.num_physical >= BX_DBG_MAX_PHY_BPOINTS) {
    dbg_printf("Error: no more physical breakpoint slots left.\n");
    dbg_printf("Error: see BX_DBG_MAX_PHY_BPOINTS.\n");
    return -1;
  }

  bx_guard.iaddr.phy[bx_guard.iaddr.num_physical].addr = paddress;
  bx_guard.iaddr.phy[bx_guard.iaddr.num_physical].bpoint_id = bx_debugger.next_bpoint_id++;
  int BpId = (int)bx_guard.iaddr.phy[bx_guard.iaddr.num_physical].bpoint_id;
  bx_guard.iaddr.phy[bx_guard.iaddr.num_physical].enabled=1;
  bx_guard.iaddr.num_physical++;
  bx_guard.guard_for |= BX_DBG_GUARD_IADDR_PHY;
  return BpId;
#else
  dbg_printf("Error: physical breakpoint support not compiled in.\n");
  dbg_printf("Error: see BX_DBG_SUPPORT_PHY_BPOINT.\n");
  return -1;
#endif
}

void bx_dbg_info_bpoints_command(void)
{
  unsigned i;
// Num Type           Disp Enb Address    What
// 1   breakpoint     keep y   0x00010664 in main at temp.c:7

  dbg_printf("Num Type           Disp Enb Address\n");
#if BX_DBG_SUPPORT_VIR_BPOINT
  for (i=0; i<bx_guard.iaddr.num_virtual; i++) {
    dbg_printf("%3u ", bx_guard.iaddr.vir[i].bpoint_id);
    dbg_printf("vbreakpoint    ");
    dbg_printf("keep ");
    dbg_printf(bx_guard.iaddr.vir[i].enabled?"y   ":"n   ");
    dbg_printf("0x%04x:" FMT_ADDRX "\n",
                  bx_guard.iaddr.vir[i].cs,
                  bx_guard.iaddr.vir[i].eip);
  }
#endif

#if BX_DBG_SUPPORT_LIN_BPOINT
  for (i=0; i<bx_guard.iaddr.num_linear; i++) {
    dbg_printf("%3u ", bx_guard.iaddr.lin[i].bpoint_id);
    dbg_printf("lbreakpoint    ");
    dbg_printf("keep ");
    dbg_printf(bx_guard.iaddr.lin[i].enabled?"y   ":"n   ");
    dbg_printf("0x" FMT_ADDRX "\n", bx_guard.iaddr.lin[i].addr);
  }
#endif

#if BX_DBG_SUPPORT_PHY_BPOINT
  for (i=0; i<bx_guard.iaddr.num_physical; i++) {
    dbg_printf("%3u ", bx_guard.iaddr.phy[i].bpoint_id);
    dbg_printf("pbreakpoint    ");
    dbg_printf("keep ");
    dbg_printf(bx_guard.iaddr.phy[i].enabled?"y   ":"n   ");
    dbg_printf("0x%08x\n", bx_guard.iaddr.phy[i].addr);
  }
#endif
}

void bx_dbg_set_auto_disassemble(bx_bool enable)
{
  bx_debugger.auto_disassemble = enable;
}

void bx_dbg_set_disassemble_size(unsigned size)
{
  if ((size!=16) && (size!=32) && (size!=64) && (size!=0))
  {
    dbg_printf("Error: disassemble size must be 16/32 or 64.\n");
    return;
  }
  bx_debugger.disassemble_size = size;
}

void bx_dbg_disassemble_switch_mode()
{
  bx_disassemble.toggle_syntax_mode();
}

void bx_dbg_take_command(const char *what, unsigned n)
{
  if (! strcmp(what, "dma")) {
    if (n == 0) {
      dbg_printf("Error: take what n=0.\n");
      return;
    }
    bx_dbg_post_dma_reports(); // in case there's some pending reports
    bx_dbg_batch_dma.this_many = n;

    for (unsigned i=0; i<n; i++) {
      BX_CPU(0)->dbg_take_dma();
    }

    bx_dbg_batch_dma.this_many = 1;  // reset to normal
    bx_dbg_post_dma_reports(); // print reports and flush
    if (bx_guard.report.dma)
      dbg_printf("done\n");
  }
  else if (! strcmp(what, "irq")) {
    BX_CPU(0)->dbg_take_irq();

    if (bx_guard.report.irq)
      dbg_printf("done\n");
  }
  else {
    dbg_printf("Error: Take '%s' not understood.\n", what);
  }
}

void bx_dbg_info_registers_command(int which_regs_mask)
{
  bx_address reg;

  if (which_regs_mask & BX_INFO_CPU_REGS) {
#if BX_SUPPORT_SMP
    dbg_printf("%s:\n", BX_CPU(dbg_cpu)->name);
#endif
#if BX_SUPPORT_X86_64 == 0
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_EAX);
    dbg_printf("eax: 0x%08x %d\n", (unsigned) reg, (int) reg);
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_ECX);
    dbg_printf("ecx: 0x%08x %d\n", (unsigned) reg, (int) reg);
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_EDX);
    dbg_printf("edx: 0x%08x %d\n", (unsigned) reg, (int) reg);
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_EBX);
    dbg_printf("ebx: 0x%08x %d\n", (unsigned) reg, (int) reg);
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_ESP);
    dbg_printf("esp: 0x%08x %d\n", (unsigned) reg, (int) reg);
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_EBP);
    dbg_printf("ebp: 0x%08x %d\n", (unsigned) reg, (int) reg);
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_ESI);
    dbg_printf("esi: 0x%08x %d\n", (unsigned) reg, (int) reg);
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_EDI);
    dbg_printf("edi: 0x%08x %d\n", (unsigned) reg, (int) reg);
    reg = bx_dbg_get_eip();
    dbg_printf("eip: 0x%08x\n", (unsigned) reg);
#else
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_RAX);
    dbg_printf("rax: 0x%08x:%08x ", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_RCX);
    dbg_printf("rcx: 0x%08x:%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_RDX);
    dbg_printf("rdx: 0x%08x:%08x ", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_RBX);
    dbg_printf("rbx: 0x%08x:%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_RSP);
    dbg_printf("rsp: 0x%08x:%08x ", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_RBP);
    dbg_printf("rbp: 0x%08x:%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_RSI);
    dbg_printf("rsi: 0x%08x:%08x ", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_RDI);
    dbg_printf("rdi: 0x%08x:%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_R8);
    dbg_printf("r8 : 0x%08x:%08x ", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_R9);
    dbg_printf("r9 : 0x%08x:%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_R10);
    dbg_printf("r10: 0x%08x:%08x ", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_R11);
    dbg_printf("r11: 0x%08x:%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_R12);
    dbg_printf("r12: 0x%08x:%08x ", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_R13);
    dbg_printf("r13: 0x%08x:%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_R14);
    dbg_printf("r14: 0x%08x:%08x ", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_64BIT_REG_R15);
    dbg_printf("r15: 0x%08x:%08x\n", GET32H(reg), GET32L(reg));
    reg = bx_dbg_get_instruction_pointer();
    dbg_printf("rip: 0x%08x:%08x\n", GET32H(reg), GET32L(reg));
#endif
    reg = BX_CPU(dbg_cpu)->read_eflags();
    dbg_printf("eflags 0x%08x\n", (unsigned) reg);
    bx_dbg_info_flags();
  }
#if BX_SUPPORT_FPU
  if (which_regs_mask & BX_INFO_FPU_REGS) {
    BX_CPU(i)->print_state_FPU();
  }
#endif
#if BX_SUPPORT_SSE
  if (which_regs_mask & BX_INFO_SSE_REGS) {
    BX_CPU(i)->print_state_SSE();
  }
#endif
}

void bx_dbg_dump_cpu_command(void)
{
  bx_dbg_cpu_t cpu;

  BX_CPU(dbg_cpu)->dbg_get_cpu(&cpu);

#if BX_SUPPORT_SMP
  dbg_printf("CPU#%u\n", i);
#endif
  dbg_printf("eax:0x%08x, ebx:0x%08x, ecx:0x%08x, edx:0x%08x\n", 
      (unsigned) cpu.eax, (unsigned) cpu.ebx,
      (unsigned) cpu.ecx, (unsigned) cpu.edx);
  dbg_printf("ebp:0x%08x, esp:0x%08x, esi:0x%08x, edi:0x%08x\n", 
      (unsigned) cpu.ebp, (unsigned) cpu.esp,
      (unsigned) cpu.esi, (unsigned) cpu.edi);
  dbg_printf("eip:0x%08x, eflags:0x%08x, inhibit_mask:%u\n",
      (unsigned) cpu.eip, (unsigned) cpu.eflags, cpu.inhibit_mask);

  dbg_printf("cs:s=0x%04x, dl=0x%08x, dh=0x%08x, valid=%u\n",
      (unsigned) cpu.cs.sel, (unsigned) cpu.cs.des_l,
      (unsigned) cpu.cs.des_h, (unsigned) cpu.cs.valid);
  dbg_printf("ss:s=0x%04x, dl=0x%08x, dh=0x%08x, valid=%u\n",
      (unsigned) cpu.ss.sel, (unsigned) cpu.ss.des_l,
      (unsigned) cpu.ss.des_h, (unsigned) cpu.ss.valid);
  dbg_printf("ds:s=0x%04x, dl=0x%08x, dh=0x%08x, valid=%u\n",
      (unsigned) cpu.ds.sel, (unsigned) cpu.ds.des_l,
      (unsigned) cpu.ds.des_h, (unsigned) cpu.ds.valid);
  dbg_printf("es:s=0x%04x, dl=0x%08x, dh=0x%08x, valid=%u\n",
      (unsigned) cpu.es.sel, (unsigned) cpu.es.des_l,
      (unsigned) cpu.es.des_h, (unsigned) cpu.es.valid);
  dbg_printf("fs:s=0x%04x, dl=0x%08x, dh=0x%08x, valid=%u\n",
      (unsigned) cpu.fs.sel, (unsigned) cpu.fs.des_l,
      (unsigned) cpu.fs.des_h, (unsigned) cpu.fs.valid);
  dbg_printf("gs:s=0x%04x, dl=0x%08x, dh=0x%08x, valid=%u\n",
      (unsigned) cpu.gs.sel, (unsigned) cpu.gs.des_l,
      (unsigned) cpu.gs.des_h, (unsigned) cpu.gs.valid);

  dbg_printf("ldtr:s=0x%04x, dl=0x%08x, dh=0x%08x, valid=%u\n",
      (unsigned) cpu.ldtr.sel, (unsigned) cpu.ldtr.des_l,
      (unsigned) cpu.ldtr.des_h, (unsigned) cpu.ldtr.valid);

  dbg_printf("tr:s=0x%04x, dl=0x%08x, dh=0x%08x, valid=%u\n",
      (unsigned) cpu.tr.sel, (unsigned) cpu.tr.des_l,
      (unsigned) cpu.tr.des_h, (unsigned) cpu.tr.valid);

  dbg_printf("gdtr:base=0x%08x, limit=0x%x\n",
      (unsigned) cpu.gdtr.base, (unsigned) cpu.gdtr.limit);
  dbg_printf("idtr:base=0x%08x, limit=0x%x\n",
      (unsigned) cpu.idtr.base, (unsigned) cpu.idtr.limit);

  dbg_printf("dr0:0x%08x, dr1:0x%08x, dr2:0x%08x\n", 
      (unsigned) cpu.dr0, (unsigned) cpu.dr1, (unsigned) cpu.dr2);
  dbg_printf("dr3:0x%08x, dr6:0x%08x, dr7:0x%08x\n", 
      (unsigned) cpu.dr3, (unsigned) cpu.dr6, (unsigned) cpu.dr7);

  dbg_printf("cr0:0x%08x, cr1:0x%08x, cr2:0x%08x\n", 
      (unsigned) cpu.cr0, (unsigned) cpu.cr1, (unsigned) cpu.cr2);
  dbg_printf("cr3:0x%08x, cr4:0x%08x\n", 
      (unsigned) cpu.cr3, (unsigned) cpu.cr4);

#if BX_SUPPORT_PCI
  if (SIM->get_param_bool(BXPN_I440FX_SUPPORT)->get()) {
    DEV_pci_print_i440fx_state();
  }
#endif

  dbg_printf("done\n");
}

static void bx_print_char(Bit8u ch)
{
  if (ch < 10)
    dbg_printf(" \\%d  ", ch);
  else if (isprint(ch))
    dbg_printf("  %c  ", ch);
  else
    dbg_printf(" \\x%02X", ch);
}

void dbg_printf_binary(char *format, Bit32u data, int bits)
{
  int b,len = 0;
  char num[33];

  for (b = 1 << (bits - 1); b; b >>= 1)
    num [len++] = (data & b) ? '1' : '0';
  num [len] = 0;
  dbg_printf (format, num);
}

void bx_dbg_examine_command(char *command, char *format, bx_bool format_passed,
               Bit32u addr, bx_bool addr_passed)
{
  unsigned repeat_count, i;
  char ch, display_format, unit_size;
  bx_bool iteration, memory_dump = false;
  unsigned data_size;
  Bit8u   data8;
  Bit16u  data16;
  Bit32u  data32;
  unsigned columns, per_line, offset;
  bx_bool is_linear;
  unsigned char databuf[8];

  dbg_printf("[bochs]:\n");

  // If command was the extended "xp" command, meaning eXamine Physical memory,
  // then flag memory address as physical, rather than linear.
  if (strcmp(command, "xp") == 0) {
    is_linear = 0;
  }
  else {
    is_linear = 1;
  }

  if (addr_passed==0)
    addr = bx_debugger.default_addr;

  if (format_passed==0) {
    display_format = bx_debugger.default_display_format;
    unit_size      = bx_debugger.default_unit_size;
    repeat_count   = 1;
  }
  else {
    if (format==NULL) {
      dbg_printf("dbg_examine: format NULL\n");
      bx_dbg_exit(1);
    }

    if (strlen(format) < 2) {
      dbg_printf("dbg_examine: invalid format passed.\n");
      bx_dbg_exit(1);
    }

    if (format[0] != '/') {
      dbg_printf("dbg_examine: '/' is not first char of format.\n");
      bx_dbg_exit(1);
    }

    format++;
    repeat_count = 0;
    ch = *format;
    iteration = 0;

    while ((ch>='0') && (ch<='9')) {
      iteration = 1;
      repeat_count = 10*repeat_count + (ch-'0');
      format++;
      ch = *format;
    }

    if (iteration==0) {
      // if no count given, use default
      repeat_count = 1;
    }
    else if (repeat_count==0) {
      // count give, but zero is an error
      dbg_printf("dbg_examine: repeat count given but is zero.\n");
      return;
    }

    // set up the default display format and unit size parameters
    display_format = bx_debugger.default_display_format;
    unit_size      = bx_debugger.default_unit_size;

    for (i = 0; format [i]; i++) {
      switch (ch = format [i]) {
        case 'x': // hex
        case 'd': // signed decimal
        case 'u': // unsigned decimal
        case 'o': // octal
        case 't': // binary
        case 'c': // chars
        case 's': // null terminated string
        case 'i': // machine instruction
          display_format = ch;
          break;

        case 'b': // bytes
        case 'h': // halfwords (two bytes)
        case 'w': // words (4 bytes)
        case 'g': // giant words (8 bytes)
          unit_size = ch;
          break;

	case 'm': // memory dump
	  memory_dump = true;
          break;

        default:
          dbg_printf("dbg_examine: invalid format passed. \'%c\'\n", ch);
          bx_dbg_exit(1);
          break;
      }
    }

    // store current options as default
    bx_debugger.default_display_format = display_format;
    bx_debugger.default_unit_size      = unit_size;
  }

  if ((display_format == 'i') || (display_format == 's')) {
    dbg_printf("error: dbg_examine: 'i' and 's' formats not supported.\n");
    return;
  }

  if (unit_size == 'g') {
    dbg_printf("error: dbg_examine: 'g' (8-byte) unit size not supported.\n");
    return;
  }

  data_size = 0;
  per_line  = 0;
  offset = 0;

  if (memory_dump) {
    if (display_format == 'c') {
      // Display character dump in lines of 64 characters
      unit_size = 'b';
      data_size = 1;
      per_line = 64;
    }
    else
      switch (unit_size) {
        case 'b': data_size = 1; per_line = 16; break;
        case 'h': data_size = 2; per_line = 8; break;
        case 'w': data_size = 4; per_line = 4; break;
        //case 'g': data_size = 8; per_line = 2; break;
      }
      // binary format is quite large
      if (display_format == 't')
        per_line /= 4;
  }
  else {
    switch (unit_size) {
      case 'b': data_size = 1; per_line = 8; break;
      case 'h': data_size = 2; per_line = 8; break;
      case 'w': data_size = 4; per_line = 4; break;
    //case 'g': data_size = 8; per_line = 2; break;
    }
  }

  columns = per_line + 1; // set current number columns past limit

  for (i=1; i<=repeat_count; i++) {
    if (columns > per_line) {
      // if not 1st run, need a newline from last line
      if (i!=1)
        dbg_printf("\n");
      if (memory_dump)
        dbg_printf("%08X  ", addr);
      else
        dbg_printf("0x%08x <bogus+%8u>:", addr, offset);
      columns = 1;
    }

    /* Put a space in the middle of dump, for readability */
    if ((columns - 1) == per_line / 2
     && memory_dump && display_format != 'c')
      dbg_printf(" ");

    if (is_linear) {
      if (! bx_dbg_read_linear(dbg_cpu, addr, data_size, databuf)) return;
    }
    else {
      // address is already physical address
      BX_MEM(0)->dbg_fetch_mem(BX_CPU(dbg_cpu), addr, data_size, databuf);
    }

    //FIXME HanishKVC The char display for data to be properly integrated
    //      so that repeat_count, columns, etc. can be set or used properly.
    //      Also for data_size of 2 and 4 how to display the individual
    //      characters i.e in which order to be decided.
    switch (data_size) {
      case 1:
        data8 = databuf[0];
        if (memory_dump)
          switch (display_format) {
	    case 'd': dbg_printf("%03d ", data8); break;
	    case 'u': dbg_printf("%03u ", data8); break;
	    case 'o': dbg_printf("%03o ", data8); break;
	    case 't': dbg_printf_binary ("%s ", data8, 8); break;
            case 'c': dbg_printf("%c", isprint(data8) ? data8 : '.'); break;
	    default : dbg_printf("%02X ", data8); break;
        }
	else
        switch (display_format) {
          case 'x': dbg_printf("\t0x%02x", (unsigned) data8); break;
          case 'd': dbg_printf("\t%d", (int) (Bit8s) data8); break;
          case 'u': dbg_printf("\t%u", (unsigned) data8); break;
          case 'o': dbg_printf("\t%o", (unsigned) data8); break;
            case 't': dbg_printf_binary("\t%s", data8, 8); break;
            case 'c': bx_print_char(data8); break;
        }
        break;

      case 2:
#ifdef BX_LITTLE_ENDIAN
        data16 = * (Bit16u *) databuf;
#else
        data16 = (databuf[1]<<8) | databuf[0];
#endif
        if (memory_dump)
          switch (display_format) {
	    case 'd': dbg_printf("%05d ", data16); break;
	    case 'u': dbg_printf("%05u ", data16); break;
	    case 'o': dbg_printf("%06o ", data16); break;
	    case 't': dbg_printf_binary ("%s ", data16, 16); break;
	    default : dbg_printf("%04X ", data16); break;
        }
	else
        switch (display_format) {
          case 'x': dbg_printf("\t0x%04x", (unsigned) data16); break;
          case 'd': dbg_printf("\t%d", (int) (Bit16s) data16); break;
          case 'u': dbg_printf("\t%u", (unsigned) data16); break;
          case 'o': dbg_printf("\t%o", (unsigned) data16); break;
            case 't': dbg_printf_binary("\t%s", data16, 16); break;
          case 'c': 
            bx_print_char(data16>>8);
            bx_print_char(data16 & 0xff);
            break;
        }
        break;

      case 4:
#ifdef BX_LITTLE_ENDIAN
        data32 = * (Bit32u *) databuf;
#else
        data32 = (databuf[3]<<24) | (databuf[2]<<16) |
                 (databuf[1]<<8)  |  databuf[0];
#endif
        if (memory_dump)
          switch (display_format) {
	    case 'd': dbg_printf("%10d ", data32); break;
	    case 'u': dbg_printf("%10u ", data32); break;
	    case 'o': dbg_printf("%12o ", data32); break;
	    case 't': dbg_printf_binary ("%s ", data32, 32); break;
	    default : dbg_printf("%08X ", data32); break;
        }
	else
        switch (display_format) {
          case 'x': dbg_printf("\t0x%08x", (unsigned) data32); break;
          case 'd': dbg_printf("\t%d", (int) (Bit32s) data32); break;
          case 'u': dbg_printf("\t%u", (unsigned) data32); break;
          case 'o': dbg_printf("\t%o", (unsigned) data32); break;
          case 't': dbg_printf_binary("\t%s", data32, 32); break;
          case 'c': 
            bx_print_char(0xff & (data32>>24));
            bx_print_char(0xff & (data32>>16));
            bx_print_char(0xff & (data32>> 8));
            bx_print_char(0xff & (data32>> 0));
            break;
        }
        break;
    }

    addr += data_size;
    bx_debugger.default_addr = addr;
    columns++;
    offset += data_size;
  }
  dbg_printf("\n");
}

void bx_dbg_setpmem_command(Bit32u paddr, unsigned len, Bit32u val)
{
  Bit8u buf[4];

  switch (len) {
    case 1:
      buf[0] = (Bit8u) val;
      break;
    case 2:
      buf[0] = val & 0xff; val >>= 8;
      buf[1] = val & 0xff;
      break;
    case 4:
      buf[0] = val & 0xff; val >>= 8;
      buf[1] = val & 0xff; val >>= 8;
      buf[2] = val & 0xff; val >>= 8;
      buf[3] = val & 0xff;
      break;
    default:
      dbg_printf("Error: setpmem: bad length value = %u\n", len);
      return;
    }

  if (! BX_MEM(0)->dbg_set_mem(paddr, len, buf)) {
    dbg_printf("Error: setpmem: could not set memory, out of physical bounds?\n");
  }
}

void bx_dbg_set_symbol_command(char *symbol, Bit32u val)
{
  bx_bool is_OK = false;
  symbol++; // get past '$'

  if ( !strcmp(symbol, "eip") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_EIP, val);
  }
  else if ( !strcmp(symbol, "eflags") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_EFLAGS, val);
  }
  else if ( !strcmp(symbol, "cs") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_CS, val);
  }
  else if ( !strcmp(symbol, "ss") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_SS, val);
  }
  else if ( !strcmp(symbol, "ds") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_DS, val);
  }
  else if ( !strcmp(symbol, "es") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_ES, val);
  }
  else if ( !strcmp(symbol, "fs") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_FS, val);
  }
  else if ( !strcmp(symbol, "gs") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_GS, val);
  }
  else if ( !strcmp(symbol, "cpu") ) {
#if BX_SUPPORT_SMP
    if (val > BX_SMP_PROCESSORS) {
      dbg_printf("invalid cpu id number %d\n", val);
      return;
    }
    dbg_cpu = val;
#endif
  }
  else if ( !strcmp(symbol, "synchronous_dma") ) {
    bx_guard.async.dma = !val;
    return;
  }
  else if ( !strcmp(symbol, "synchronous_irq") ) {
    bx_guard.async.irq = !val;
    return;
  }
  else if ( !strcmp(symbol, "event_reports") ) {
    bx_guard.report.irq   = val;
    bx_guard.report.a20   = val;
    bx_guard.report.io    = val;
    bx_guard.report.ucmem = val;
    bx_guard.report.dma   = val;
    return;
  }
  else if ( !strcmp(symbol, "auto_disassemble") ) {
    bx_dbg_set_auto_disassemble(val != 0);
    return;
  }
  else {
    dbg_printf("Error: set: unrecognized symbol.\n");
    return;
  }

  if (!is_OK) {
    dbg_printf("Error: could not set register '%s'.\n", symbol);
  }
}

void bx_dbg_query_command(const char *what)
{
  unsigned pending;

  if (! strcmp(what, "pending")) {
    pending = BX_CPU(0)->dbg_query_pending();

    if (pending & BX_DBG_PENDING_DMA)
      dbg_printf("pending DMA\n");

    if (pending & BX_DBG_PENDING_IRQ)
      dbg_printf("pending IRQ\n");

    if (!pending)
      dbg_printf("pending none\n");

    dbg_printf("done\n");
  }
  else {
    dbg_printf("Error: Query '%s' not understood.\n", what);
  }
}

void bx_dbg_set_cpu_command(void)
{
  FILE *fp;
  int   reti;
  char *rets;
  bx_bool retb;
  unsigned long ul1, ul2, ul3, ul4;

  bx_dbg_cpu_t cpu;

  fp = bx_infile_stack[bx_infile_stack_index].fp;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "eax:0x%lx", &ul1); cpu.eax = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "ebx:0x%lx", &ul1); cpu.ebx = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "ecx:0x%lx", &ul1); cpu.ecx = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "edx:0x%lx", &ul1); cpu.edx = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "ebp:0x%lx", &ul1); cpu.ebp = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "esi:0x%lx", &ul1); cpu.esi = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "edi:0x%lx", &ul1); cpu.edi = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "esp:0x%lx", &ul1); cpu.esp = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "eflags:0x%lx", &ul1); cpu.eflags = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "eip:0x%lx", &ul1); cpu.eip = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "cs:s=0x%lx, dl=0x%lx, dh=0x%lx, valid=%lu",
                &ul1, &ul2, &ul3, &ul4);
  cpu.cs.sel   = (Bit16u) ul1;
  cpu.cs.des_l = ul2;
  cpu.cs.des_h = ul3;
  cpu.cs.valid = ul4;
  if (reti != 4) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "ss:s=0x%lx, dl=0x%lx, dh=0x%lx, valid=%lu",
                &ul1, &ul2, &ul3, &ul4);
  cpu.ss.sel   = (Bit16u) ul1;
  cpu.ss.des_l = ul2;
  cpu.ss.des_h = ul3;
  cpu.ss.valid = ul4;
  if (reti != 4) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "ds:s=0x%lx, dl=0x%lx, dh=0x%lx, valid=%lu",
                &ul1, &ul2, &ul3, &ul4);
  cpu.ds.sel   = (Bit16u) ul1;
  cpu.ds.des_l = ul2;
  cpu.ds.des_h = ul3;
  cpu.ds.valid = ul4;
  if (reti != 4) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "es:s=0x%lx, dl=0x%lx, dh=0x%lx, valid=%lu",
                &ul1, &ul2, &ul3, &ul4);
  cpu.es.sel   = (Bit16u) ul1;
  cpu.es.des_l = ul2;
  cpu.es.des_h = ul3;
  cpu.es.valid = ul4;
  if (reti != 4) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "fs:s=0x%lx, dl=0x%lx, dh=0x%lx, valid=%lu",
                &ul1, &ul2, &ul3, &ul4);
  cpu.fs.sel   = (Bit16u) ul1;
  cpu.fs.des_l = ul2;
  cpu.fs.des_h = ul3;
  cpu.fs.valid = ul4;
  if (reti != 4) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "gs:s=0x%lx, dl=0x%lx, dh=0x%lx, valid=%lu",
                &ul1, &ul2, &ul3, &ul4);
  cpu.gs.sel   = (Bit16u) ul1;
  cpu.gs.des_l = ul2;
  cpu.gs.des_h = ul3;
  cpu.gs.valid = ul4;
  if (reti != 4) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "ldtr:s=0x%lx, dl=0x%lx, dh=0x%lx, valid=%lu",
                &ul1, &ul2, &ul3, &ul4);
  cpu.ldtr.sel   = (Bit16u) ul1;
  cpu.ldtr.des_l = ul2;
  cpu.ldtr.des_h = ul3;
  cpu.ldtr.valid = ul4;
  if (reti != 4) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "tr:s=0x%lx, dl=0x%lx, dh=0x%lx, valid=%lu",
                &ul1, &ul2, &ul3, &ul4);
  cpu.tr.sel   = (Bit16u) ul1;
  cpu.tr.des_l = ul2;
  cpu.tr.des_h = ul3;
  cpu.tr.valid = ul4;
  if (reti != 4) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "gdtr:base=0x%lx, limit=0x%lx",
                &ul1, &ul2);
  cpu.gdtr.base  = ul1;
  cpu.gdtr.limit = ul2;
  if (reti != 2) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "idtr:base=0x%lx, limit=0x%lx",
                &ul1, &ul2);
  cpu.idtr.base  = ul1;
  cpu.idtr.limit = ul2;
  if (reti != 2) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "dr0:0x%lx", &ul1); cpu.dr0 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "dr1:0x%lx", &ul1); cpu.dr1 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "dr2:0x%lx", &ul1); cpu.dr2 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "dr3:0x%lx", &ul1); cpu.dr3 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "dr6:0x%lx", &ul1); cpu.dr6 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "dr7:0x%lx", &ul1); cpu.dr7 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "cr0:0x%lx", &ul1); cpu.cr0 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "cr1:0x%lx", &ul1); cpu.cr1 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "cr2:0x%lx", &ul1); cpu.cr2 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "cr3:0x%lx", &ul1); cpu.cr3 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "cr4:0x%lx", &ul1); cpu.cr4 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "inhibit_mask:%u", &cpu.inhibit_mask);
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "done");
  if (reti != 0) goto scanf_error;

  retb = BX_CPU(dbg_cpu)->dbg_set_cpu(&cpu);
  if (retb == 0)
    dbg_printf("Error: dbg_set_cpu encountered error\n");
  else
    dbg_printf("OK\n");
  return;

eof_error:
  dbg_printf("Error: EOF encountered in dbg_set_cpu input stream\n");
  return;

scanf_error:
  dbg_printf("Error: scanf returned error in dbg_set_cpu input stream\n");
  return;
}

void bx_dbg_disassemble_current(const char *format)
{
  Bit64u addr = bx_dbg_get_laddr(bx_dbg_get_selector_value(BX_DBG_SREG_CS), BX_CPU(dbg_cpu)->get_ip());
  bx_dbg_disassemble_command(format, addr, addr);
}

void bx_dbg_disassemble_command(const char *format, Bit64u from, Bit64u to)
{
  int numlines = INT_MAX;

  if (from > to) {
     int temp = from;
     from = to;
     to = temp;
  }

  if (format) {
    // format always begins with '/' (checked in lexer)
    // so we won't bother checking it here second time.
    numlines = atoi(format + 1);
    if (to == from)
      to = BX_MAX_BIT64U; // Disassemble just X lines
  }

  unsigned dis_size = bx_debugger.disassemble_size;
  if (dis_size == 0) {
    dis_size = 16; 		// until otherwise proven
    if (BX_CPU(dbg_cpu)->sregs[BX_SEG_REG_CS].cache.u.segment.d_b)
      dis_size = 32;
    if (BX_CPU(dbg_cpu)->get_cpu_mode() == BX_MODE_LONG_64)
      dis_size = 64;
  }

  do {
    numlines--;

    if (! bx_dbg_read_linear(dbg_cpu, from, 16, bx_disasm_ibuf)) break;

    unsigned ilen = bx_disassemble.disasm(dis_size==32, dis_size==64,
       (bx_address)(-1), (bx_address)(-1), bx_disasm_ibuf, bx_disasm_tbuf);

    char *Sym=bx_dbg_disasm_symbolic_address((Bit32u)from, 0);

    dbg_printf("%08x: ", (unsigned) from);
    dbg_printf("(%20s): ", Sym?Sym:"");
    dbg_printf("%-25s ; ", bx_disasm_tbuf);

    for (unsigned j=0; j<ilen; j++)
      dbg_printf("%02x", (unsigned) bx_disasm_ibuf[j]);
    dbg_printf("\n");

    from += ilen;
  } while ((from < to) && numlines > 0);
}

void bx_dbg_instrument_command(const char *comm)
{
#if BX_INSTRUMENTATION
  if ( !strcmp(comm, "start") ) {
    BX_INSTR_START();
  }
  else if ( !strcmp(comm, "stop") ) {
    BX_INSTR_STOP();
  }
  else if ( !strcmp(comm, "reset") ) {
    BX_INSTR_RESET(dbg_cpu);
  }
  else if ( !strcmp(comm, "print") ) {
    BX_INSTR_PRINT();
  }
  else {
    dbg_printf("Error: command 'instrument %s' not recognized\n", comm);
    bx_dbg_exit(1);
  }
#else
  UNUSED(comm);

  dbg_printf("Error: instrumentation not enabled.\n");
#endif
}

void bx_dbg_doit_command(unsigned n)
{
  // generic command to add temporary hacks to
  // for debugging purposes
  UNUSED(n);

  bx_dbg.interrupts = n;
  bx_dbg.exceptions = n;
}

void bx_dbg_crc_command(Bit32u addr1, Bit32u addr2)
{
  Bit32u crc1;

  if (addr1 >= addr2) {
    dbg_printf("Error: crc32: invalid range\n");
    return;
  }

  if (!BX_MEM(0)->dbg_crc32(addr1, addr2, &crc1)) {
    dbg_printf("Error: could not crc32 memory\n");
    return;
  }
  dbg_printf("0x%lx\n", crc1);
}

void bx_dbg_info_dirty_command(void)
{
  unsigned char *page_tbl = BX_MEM(0)->dbg_dirty_pages;
  Bit32u page_tbl_size = BX_MEM(0)->get_num_allocated_pages();

  for (unsigned i=0; i<page_tbl_size; i++) {
    if (page_tbl[i]) {
      dbg_printf("0x%x\n", i);
      page_tbl[i] = 0; // reset to clean
    }
  }
}

void bx_dbg_print_descriptor(unsigned char desc[8], int verbose)
{
  Bit32u lo = (desc[3] << 24) | (desc[2] << 16) | (desc[1] << 8) | (desc[0]);
  Bit32u hi = (desc[7] << 24) | (desc[6] << 16) | (desc[5] << 8) | (desc[4]);
//dbg_printf("Descriptor hi,lo = %08x,%08x\n", hi, lo);
  Bit32u base = ((lo >> 16) & 0xffff)
             | ((hi << 16) & 0xff0000)
             | (hi & 0xff000000);
  Bit32u limit = (hi & 0x000f0000) | (lo & 0xffff);
  Bit32u segment = (lo >> 16) & 0xffff;
  Bit32u offset = (lo & 0xffff) | (hi & 0xffff0000);
  unsigned type = (hi >> 8) & 0x0f;
  unsigned dpl = (hi >> 13) & 0x03;
  unsigned s = (hi >> 12) & 0x01;
  unsigned d_b = (hi >> 22) & 0x01;
  unsigned g = (hi >> 23) & 0x01;
#if 0
  unsigned present = (hi >> 15) & 0x01;
  unsigned avl = (hi >> 20) & 0x01;
  unsigned base_is_jump_addr;
  if (s) {
    // either a code or a data segment. bit 11 (type file MSB) then says 
    // 0=data segment, 1=code seg
    if (type&8) {
      dbg_printf("Segment type: Code, %s%s%s\n",
        (type&2)? "Execute/Read" : "Execute-Only",
        (type&4)? ", Conforming" : "",
        (type&1)? ", Accessed" : "");
      dbg_printf("D flag=%d (use %d-bit addresses, %d-bit or 8-bit operands)\n", d_b, d_b? 32 : 16);
    } else {
      dbg_printf("Segment type: Data, %s%s%s\n",
        (type&2)? "Read/Write" : "Read-Only",
        (type&4)? ", Expand-down" : "",
        (type&1)? ", Accessed" : "");
    }
  } else {
    // types from IA32-devel-guide-3, page 3-15.
    static char *type_names[16] = { "Reserved", "16-Bit TSS (available)", "LDT", "16-Bit TSS (Busy)", "16-Bit Call Gate", "Task Gate", "16-Bit Interrupt Gate", "16-Bit Trap Gate", "Reserved", "32-Bit TSS (Available)", "Reserved", "32-Bit TSS (Busy)", "32-Bit Call Gate", "Reserved", "32-Bit Interrupt Gate", "32-Bit Trap Gate" };
    // some kind of gate?
    dbg_printf("System segment, type=0x%x=%s\n", type, type_names[type]);
    base_is_jump_addr = 1;
    // for call gates, print segment:offset and parameter count p.40-15
    // for task gate, only present,dpl,TSS segment selector exist. p.5-13
    // for interrupt gate, segment:offset,p,dpl
    // for trap gate, segment:offset,p,dpl
  }
  dbg_printf("DPL=descriptor privilege level=%d\n", dpl);
  if (base_is_jump_addr) {
    dbg_printf("target address=%04x:%08x\n", segment, offset);
  } else {
    dbg_printf("base address=%08x\n", base);
    dbg_printf("G=granularity=%d\n", g);
    dbg_printf("limit=0x%05x %s (see G)\n", limit, g?"4K-byte units" : "bytes");
    dbg_printf("AVL=available to OS=%d\n", avl);
  }
  dbg_printf("P=present=%d\n", present);
#endif
  /* brief output */
  // 32-bit trap gate, target=0010:c0108ec4, DPL=0, present=1
  // code segment, base=0000:00cfffff, length=0xffff
  if (s) {
    // either a code or a data segment. bit 11 (type file MSB) then says 
    // 0=data segment, 1=code seg
    if (type&8) {
      dbg_printf("Code segment, linearaddr=%08x, limit=%05x %s, %s%s%s, %d-bit\n", 
        base, limit, g ? "* 4Kbytes" : "bytes",
        (type&2)? "Execute/Read" : "Execute-Only",
        (type&4)? ", Conforming" : "",
        (type&1)? ", Accessed" : "",
        d_b ? 32 : 16);
    } else {
      dbg_printf("Data segment, linearaddr=%08x, limit=%05x %s, %s%s%s\n",
        base, limit, g ? "* 4Kbytes" : "bytes",
        (type&2)? "Read/Write" : "Read-Only",
        (type&4)? ", Expand-down" : "",
        (type&1)? ", Accessed" : "");
    }
  } else {
    // types from IA32-devel-guide-3, page 3-15.
    static char *undef = "???";
    static char *type_names[16] = { undef, "16-Bit TSS (available)", "LDT", "16-Bit TSS (Busy)", "16-Bit Call Gate", "Task Gate", "16-Bit Interrupt Gate", "16-Bit Trap Gate", undef, "32-Bit TSS (Available)", undef, "32-Bit TSS (Busy)", "32-Bit Call Gate", undef, "32-Bit Interrupt Gate", "32-Bit Trap Gate" };
    dbg_printf("%s ", type_names[type]);
    // only print more if type is valid
    if (type_names[type] == undef)  {
      dbg_printf("descriptor hi=0x%08x, lo=0x%08x", hi, lo);
    } else {
      // for call gates, print segment:offset and parameter count p.4-15
      // for task gate, only present,dpl,TSS segment selector exist. p.5-13
      // for interrupt gate, segment:offset,p,dpl
      // for trap gate, segment:offset,p,dpl
      // for TSS, base address and segment limit
      switch (type) {
      case BX_SYS_SEGMENT_AVAIL_286_TSS:
      case BX_SYS_SEGMENT_BUSY_286_TSS:
      case BX_SYS_SEGMENT_AVAIL_386_TSS:
      case BX_SYS_SEGMENT_BUSY_386_TSS:
       dbg_printf("at 0x%08x, length 0x%05x", base, limit);
        break;
      case BX_SYS_SEGMENT_LDT:
        // it's an LDT. not much to print.
        break;
      default:
        // task, int, trap, or call gate.
        dbg_printf("target=0x%04x:0x%08x, DPL=%d", segment, offset, dpl);
      }
    }
    dbg_printf("\n");
  }
}

void bx_dbg_info_idt_command(unsigned from, unsigned to)
{
  bx_dbg_global_sreg_t idtr;
  BX_CPU(dbg_cpu)->dbg_get_idtr(&idtr);
  bx_bool all = 0;

  if (to == (unsigned) EMPTY_ARG) {
    to = from;
    if(from == (unsigned) EMPTY_ARG) { from = 0; to = 255; all = 1; }
  }
  if (from > 255 || to > 255) {
    dbg_printf("IDT entry should be [0-255], 'info idt' command malformed\n");
    return;
  }
  if (from > to) {
    unsigned temp = from;
    from = to;
    to = temp;
  }

  dbg_printf("Interrupt Descriptor Table (base=0x" FMT_ADDRX ", limit=%d):\n", idtr.base, idtr.limit);
  for (unsigned n = from; n<=to; n++) {
    Bit8u entry[8];
    if (8*n + 7 > idtr.limit) break;
    if (bx_dbg_read_linear(dbg_cpu, idtr.base + 8*n, 8, entry)) {
      dbg_printf("IDT[0x%02x]=", n);
      bx_dbg_print_descriptor(entry, 0);
    }
    else {
      dbg_printf("error: IDTR+8*%d points to invalid linear address 0x" FMT_ADDRX "\n",
        n, idtr.base);
    }
  }
  if (all)
    dbg_printf("You can list individual entries with 'info idt [NUM]' or groups with 'info idt [NUM] [NUM]'\n");
}

void bx_dbg_info_gdt_command(unsigned from, unsigned to)
{
  bx_dbg_global_sreg_t gdtr;
  BX_CPU(dbg_cpu)->dbg_get_gdtr(&gdtr);
  bx_bool all = 0;

  if (to == (unsigned) EMPTY_ARG) {
    to = from;
    if(from == (unsigned) EMPTY_ARG) { from = 0; to = 0xffff; all = 1; }
  }
  if (from > 0xffff || to > 0xffff) {
    dbg_printf("IDT entry should be [0-65535], 'info gdt' command malformed\n");
    return;
  }
  if (from > to) {
    unsigned temp = from;
    from = to;
    to = temp;
  }

  dbg_printf("Global Descriptor Table (base=0x" FMT_ADDRX ", limit=%d):\n", gdtr.base, gdtr.limit);
  for (unsigned n = from; n<=to; n++) {
    Bit8u entry[8];
    if (8*n + 7 > gdtr.limit) break;
    if (bx_dbg_read_linear(dbg_cpu, gdtr.base + 8*n, 8, entry)) {
      dbg_printf("GDT[0x%02x]=", n);
      bx_dbg_print_descriptor(entry, 0);
    }
    else {
      dbg_printf("error: GDTR+8*%d points to invalid linear address 0x " FMT_ADDRX "\n",
        n, gdtr.base);
    }
  }
  if (all)
    dbg_printf("You can list individual entries with 'info gdt [NUM]' or groups with 'info gdt [NUM] [NUM]'\n");
}

void bx_dbg_info_ldt_command(unsigned from, unsigned to)
{
  dbg_printf("Local Descriptor Table output not implemented\n");
}

/*form RB list*/
static char* bx_dbg_ivt_desc(int intnum)
{ 
  char* ret = "";
  switch (intnum)
  { case 0x00 : ret = "DIVIDE ERROR"                        ; break;
    case 0x01 : ret = "SINGLE STEP"                         ; break;
    case 0x02 : ret = "NON-MASKABLE INTERRUPT"              ; break;
    case 0x03 : ret = "BREAKPOINT"                          ; break;
    case 0x04 : ret = "INT0 DETECTED OVERFLOW"              ; break;
    case 0x05 : ret = "BOUND RANGE EXCEED"                  ; break;
    case 0x06 : ret = "INVALID OPCODE"                      ; break;
    case 0x07 : ret = "PROCESSOR EXTENSION NOT AVAILABLE"   ; break;
    case 0x08 : ret = "IRQ0 - SYSTEM TIMER"                 ; break;
    case 0x09 : ret = "IRQ1 - KEYBOARD DATA READY"          ; break;
    case 0x0a : ret = "IRQ2 - LPT2"                         ; break;
    case 0x0b : ret = "IRQ3 - COM2"                         ; break;
    case 0x0c : ret = "IRQ4 - COM1"                         ; break;
    case 0x0d : ret = "IRQ5 - FIXED DISK"                   ; break;
    case 0x0e : ret = "IRQ6 - DISKETTE CONTROLLER"          ; break;
    case 0x0f : ret = "IRQ7 - PARALLEL PRINTER"             ; break;
    case 0x10 : ret = "VIDEO"                               ; break;
    case 0x11 : ret = "GET EQUIPMENT LIST"                  ; break;
    case 0x12 : ret = "GET MEMORY SIZE"                     ; break;
    case 0x13 : ret = "DISK"                                ; break;
    case 0x14 : ret = "SERIAL"                              ; break;
    case 0x15 : ret = "SYSTEM"                              ; break;
    case 0x16 : ret = "KEYBOARD"                            ; break;
    case 0x17 : ret = "PRINTER"                             ; break;
    case 0x18 : ret = "CASETTE BASIC"                       ; break;
    case 0x19 : ret = "BOOTSTRAP LOADER"                    ; break;
    case 0x1a : ret = "TIME"                                ; break;
    case 0x1b : ret = "KEYBOARD - CONTROL-BREAK HANDLER"    ; break;
    case 0x1c : ret = "TIME - SYSTEM TIMER TICK"            ; break;
    case 0x1d : ret = "SYSTEM DATA - VIDEO PARAMETER TABLES"; break;
    case 0x1e : ret = "SYSTEM DATA - DISKETTE PARAMETERS"   ; break;
    case 0x1f : ret = "SYSTEM DATA - 8x8 GRAPHICS FONT"     ; break;
    case 0x70 : ret = "IRQ8 - CMOS REAL-TIME CLOCK"         ; break;
    case 0x71 : ret = "IRQ9 - REDIRECTED TO INT 0A BY BIOS" ; break;
    case 0x72 : ret = "IRQ10 - RESERVED"                    ; break;
    case 0x73 : ret = "IRQ11 - RESERVED"                    ; break;
    case 0x74 : ret = "IRQ12 - POINTING DEVICE"             ; break;
    case 0x75 : ret = "IRQ13 - MATH COPROCESSOR EXCEPTION"  ; break;
    case 0x76 : ret = "IRQ14 - HARD DISK CONTROLLER OPERATION COMPLETE"; break;
    case 0x77 : ret = "IRQ15 - SECONDARY IDE CONTROLLER OPERATION COMPLETE"; break;
    default   : ret = ""                                    ; break;
  }
  return ret;
}

void bx_dbg_info_ivt_command(unsigned from, unsigned to)
{ 
  unsigned char buff[4];
  Bit16u seg;
  Bit16u off;
  bx_bool all = 0;
  bx_dbg_global_sreg_t idtr;

  BX_CPU(dbg_cpu)->dbg_get_idtr(&idtr);
  
  if (! BX_CPU(dbg_cpu)->protected_mode())
  { 
    if (to == (unsigned) EMPTY_ARG) {
      to = from;
      if(from == (unsigned) EMPTY_ARG) { from = 0; to = 255; all = 1; }
    }
    if (from > 255 || to > 255) {
      dbg_printf("IVT entry should be [0-255], 'info ivt' command malformed\n");
      return;
    }
    if (from > to) {
      unsigned temp = from;
      from = to;
      to = temp;
    }

    for (unsigned i = from; i <= to; i++)
    { 
      BX_MEM(0)->dbg_fetch_mem(BX_CPU(dbg_cpu), idtr.base + i * 4, sizeof(buff), buff);
#ifdef BX_LITTLE_ENDIAN
      seg = *(Bit16u*)(&buff[2]);
      off = *(Bit16u*)(&buff[0]);
#else
      seg = (buff[3] << 8) | buff[2];
      off = (buff[1] << 8) | buff[0];
#endif
      BX_MEM(0)->dbg_fetch_mem(BX_CPU(dbg_cpu), ((seg << 4) + off), sizeof(buff), buff);
      dbg_printf("INT# %02x > %04X:%04X (" FMT_ADDRX ") %s%s\n", i, seg, off,
         ((seg << 4) + off), bx_dbg_ivt_desc(i),
         (buff[0] == 0xcf) ? " ; dummy iret" : "");
    }
    if (all) dbg_printf("You can list individual entries with 'info ivt [NUM]' or groups with 'info ivt [NUM] [NUM]'\n");
  }
  else
    dbg_printf("cpu in protected mode, use info idt\n");
}

static void bx_dbg_print_tss(unsigned char *tss, int len)
{
  if (len<104) {
    dbg_printf("Invalid tss length (limit must be greater then 103)\n");
    return;
  }

  dbg_printf("ss:esp(0): 0x%04x:0x%08x\n",
    *(Bit16u*)(tss+8), *(Bit32u*)(tss+4));
  dbg_printf("ss:esp(1): 0x%04x:0x%08x\n",
    *(Bit16u*)(tss+0x10), *(Bit32u*)(tss+0xc));
  dbg_printf("ss:esp(2): 0x%04x:0x%08x\n",
    *(Bit16u*)(tss+0x18), *(Bit32u*)(tss+0x14));
  dbg_printf("cr3: 0x%08x\n", *(Bit32u*)(tss+0x1c));
  dbg_printf("eip: 0x%08x\n", *(Bit32u*)(tss+0x20));
  dbg_printf("eflags: 0x%08x\n", *(Bit32u*)(tss+0x24));

  dbg_printf("cs: 0x%04x ds: 0x%04x ss: 0x%04x\n",
    *(Bit16u*)(tss+76), *(Bit16u*)(tss+84), *(Bit16u*)(tss+80));
  dbg_printf("es: 0x%04x fs: 0x%04x gs: 0x%04x\n",
    *(Bit16u*)(tss+72), *(Bit16u*)(tss+88), *(Bit16u*)(tss+92));

  dbg_printf("eax: 0x%08x  ebx: 0x%08x  ecx: 0x%08x  edx: 0x%08x\n",
    *(Bit32u*)(tss+0x28), *(Bit32u*)(tss+0x34), *(Bit32u*)(tss+0x2c), *(Bit32u*)(tss+0x30));
  dbg_printf("esi: 0x%08x  edi: 0x%08x  ebp: 0x%08x  esp: 0x%08x\n",
    *(Bit32u*)(tss+0x40), *(Bit32u*)(tss+0x44), *(Bit32u*)(tss+0x3c), *(Bit32u*)(tss+0x38));

  dbg_printf("ldt: 0x%04x\n", *(Bit16u*)(tss+0x60));
  dbg_printf("i/o map: 0x%04x\n", *(Bit16u*)(tss+0x66));
}

void bx_dbg_info_tss_command(void)
{
  bx_dbg_sreg_t tr;
  BX_CPU(dbg_cpu)->dbg_get_tr(&tr);

  Bit32u laddr = (tr.des_l>>16) |
                ((tr.des_h<<16)&0x00ff0000) | (tr.des_h & 0xff000000);
  Bit32u len = (tr.des_l & 0xffff) + 1;

  dbg_printf("tr:s=0x%x, base=0x%x, valid=%u\n",
      (unsigned) tr.sel, laddr, (unsigned) tr.valid);

  bx_phy_address paddr = 0;
  BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(laddr, &paddr);
  bx_dbg_print_tss(BX_MEM(0)->vector+paddr, len);
}

void bx_dbg_info_control_regs_command(void)
{
  Bit32u cr0 = BX_CPU(dbg_cpu)->dbg_get_reg(BX_DBG_REG_CR0);
  Bit32u cr2 = BX_CPU(dbg_cpu)->dbg_get_reg(BX_DBG_REG_CR2);
  Bit32u cr3 = BX_CPU(dbg_cpu)->dbg_get_reg(BX_DBG_REG_CR3);
  dbg_printf("CR0=0x%08x\n", cr0);
  dbg_printf("    PG=paging=%d\n", (cr0>>31) & 1);
  dbg_printf("    CD=cache disable=%d\n", (cr0>>30) & 1);
  dbg_printf("    NW=not write through=%d\n", (cr0>>29) & 1);
  dbg_printf("    AM=alignment mask=%d\n", (cr0>>18) & 1);
  dbg_printf("    WP=write protect=%d\n", (cr0>>16) & 1);
  dbg_printf("    NE=numeric error=%d\n", (cr0>>5) & 1);
  dbg_printf("    ET=extension type=%d\n", (cr0>>4) & 1);
  dbg_printf("    TS=task switched=%d\n", (cr0>>3) & 1);
  dbg_printf("    EM=FPU emulation=%d\n", (cr0>>2) & 1);
  dbg_printf("    MP=monitor coprocessor=%d\n", (cr0>>1) & 1);
  dbg_printf("    PE=protection enable=%d\n", (cr0>>0) & 1);
  dbg_printf("CR2=page fault linear address=0x%08x\n", cr2);
  dbg_printf("CR3=0x%08x\n", cr3);
  dbg_printf("    PCD=page-level cache disable=%d\n", (cr3>>4) & 1);
  dbg_printf("    PWT=page-level writes transparent=%d\n", (cr3>>3) & 1);
#if BX_CPU_LEVEL >= 4
  Bit32u cr4 = BX_CPU(dbg_cpu)->dbg_get_reg(BX_DBG_REG_CR4);
  dbg_printf("CR4=0x%08x\n", cr4);
  dbg_printf("    VME=virtual-8086 mode extensions=%d\n", (cr4>>0) & 1);
  dbg_printf("    PVI=protected-mode virtual interrupts=%d\n", (cr4>>1) & 1);
  dbg_printf("    TSD=time stamp disable=%d\n", (cr4>>2) & 1);
  dbg_printf("    DE=debugging extensions=%d\n", (cr4>>3) & 1);
  dbg_printf("    PSE=page size extensions=%d\n", (cr4>>4) & 1);
  dbg_printf("    PAE=physical address extension=%d\n", (cr4>>5) & 1);
  dbg_printf("    MCE=machine check enable=%d\n", (cr4>>6) & 1);
  dbg_printf("    PGE=page global enable=%d\n", (cr4>>7) & 1);
  dbg_printf("    PCE=performance-monitor counter enable=%d\n", (cr4>>8) & 1);
  dbg_printf("    OXFXSR=OS support for FXSAVE/FXRSTOR=%d\n", (cr4>>9) & 1);
  dbg_printf("    OSXMMEXCPT=OS support for unmasked SIMD FP exceptions=%d\n", (cr4>>10) & 1);
#endif   /* BX_CPU_LEVEL >= 4 */
#if BX_SUPPORT_X86_64
  Bit32u efer = BX_CPU(dbg_cpu)->get_EFER();
  dbg_printf("EFER=0x%08x\n", efer);
  dbg_printf("    SCE=SYSCALL/SYSRET support=%d\n", (efer>>0) & 1);
  dbg_printf("    LME=long mode enabled=%d\n", (efer>>8) & 1);
  dbg_printf("    LMA=long mode activated=%d\n", (efer>>10) & 1);
  dbg_printf("    NXE=non-execuable page protection=%d\n", (efer>>11) & 1);
  dbg_printf("    FFXSR=OS support for fast FXSAVE/FXRSTOR=%d\n", (efer>>14) & 1);
#endif
}

/*
 * this function implements the info ne2k commands in the debugger
 * info ne2k - shows all registers
 * info ne2k page N - shows all registers in a page
 * info ne2k page N reg M - shows just one register
 */
void bx_dbg_info_ne2k(int page, int reg)
{
#if BX_SUPPORT_NE2K
  DEV_ne2k_print_info(stderr, page, reg, 0);
#else
  dbg_printf("NE2000 support is not compiled in.\n");
#endif
}

/*
 * this implements the info pic command in the debugger.
 * info pic - shows pic registers
 */
void bx_dbg_info_pic()
{
  DEV_pic_show_pic_state();
}

/*
 * this implements the info vga command in the debugger.
 * info vga - shows vga registers
 */
void bx_dbg_info_vga()
{
  DEV_vga_dump_status();
}

//
// Reports from various events
//

void bx_dbg_iac_report(unsigned vector, unsigned irq)
{
  if (bx_guard.report.irq) {
    dbg_printf("event at t=" FMT_LL "d IRQ irq=%u vec=%x\n",
      bx_pc_system.time_ticks(), irq, vector);
  }
}

void bx_dbg_a20_report(unsigned val)
{
  if (bx_guard.report.a20) {
    dbg_printf("event at t=" FMT_LL "d A20 val=%u\n",
      bx_pc_system.time_ticks(), val);
  }
}

void bx_dbg_io_report(Bit32u addr, unsigned size, unsigned op, Bit32u val)
{
  if (bx_guard.report.io) {
    dbg_printf("event at t=" FMT_LL "d IO addr=0x%x size=%u op=%s val=0x%x\n",
      bx_pc_system.time_ticks(),
      (unsigned) addr,
      size,
      (op==BX_READ) ? "read" : "write",
      (unsigned) val);
  }
}

void bx_dbg_ucmem_report(Bit32u addr, unsigned size, unsigned op, Bit32u val)
{
  if (bx_guard.report.ucmem) {
    dbg_printf("event at t=" FMT_LL "d UCmem addr=0x%x size=%u op=%s val=0x%x\n",
      bx_pc_system.time_ticks(),
      (unsigned) addr,
      size,
      (op==BX_READ) ? "read" : "write",
      (unsigned) val);
  }
}

void bx_dbg_dma_report(Bit32u addr, unsigned len, unsigned what, Bit32u val)
{
  if (bx_dbg_batch_dma.this_many == 0) {
    dbg_printf("%s: DMA batch this_many=0.\n", argv0);
    bx_dbg_exit(1);
  }

  // if Q is full, post events (and flush)
  if (bx_dbg_batch_dma.Qsize >= bx_dbg_batch_dma.this_many) {
    dbg_printf("%s: DMA batch Q was not flushed.\n", argv0);
    bx_dbg_exit(1);
  }

  // if Q already has MAX elements in it
  if (bx_dbg_batch_dma.Qsize >= BX_BATCH_DMA_BUFSIZE) {
    dbg_printf("%s: DMA batch buffer overrun.\n", argv0);
    bx_dbg_exit(1);
  }

  bx_dbg_batch_dma.Qsize++;
  bx_dbg_batch_dma.Q[bx_dbg_batch_dma.Qsize-1].addr = addr;
  bx_dbg_batch_dma.Q[bx_dbg_batch_dma.Qsize-1].len  = len;
  bx_dbg_batch_dma.Q[bx_dbg_batch_dma.Qsize-1].what = what;
  bx_dbg_batch_dma.Q[bx_dbg_batch_dma.Qsize-1].val  = val;
  bx_dbg_batch_dma.Q[bx_dbg_batch_dma.Qsize-1].time = bx_pc_system.time_ticks();

  // if Q is full, post events (and flush)
  if (bx_dbg_batch_dma.Qsize >= bx_dbg_batch_dma.this_many)
    bx_dbg_post_dma_reports();
}

void bx_dbg_post_dma_reports(void)
{
  unsigned i;
  unsigned addr, len, what, val;
  unsigned last_addr, last_len, last_what;
  unsigned print_header;
  unsigned first_iteration;

  if (bx_guard.report.dma) {
    if (bx_dbg_batch_dma.Qsize == 0) return; // nothing batched to print

    // compress output so all contiguous DMA ops of the same type and size
    // are printed on the same line
    last_addr = bx_dbg_batch_dma.Q[0].addr;
    last_len  = bx_dbg_batch_dma.Q[0].len;
    last_what = bx_dbg_batch_dma.Q[0].what;
    first_iteration = 1;

    for (i=0; i<bx_dbg_batch_dma.Qsize; i++) {
      addr = bx_dbg_batch_dma.Q[i].addr;
      len  = bx_dbg_batch_dma.Q[i].len;
      what = bx_dbg_batch_dma.Q[i].what;
      val  = bx_dbg_batch_dma.Q[i].val;

      if (len != last_len)
        print_header = 1;
      else if (what != last_what)
        print_header = 1;
      else if (addr != (last_addr + last_len))
        print_header = 1;
      else
        print_header = 0;

      // now store current values for next iteration
      last_addr = addr;
      last_len  = len;
      last_what = what;

      if (print_header) {
        if (!first_iteration) // need return from previous line
          dbg_printf("\n");
        else
          first_iteration = 0;
        // need to output the event header
        dbg_printf("event at t=" FMT_LL "d DMA addr=0x%x size=%u op=%s val=0x%x",
                         bx_pc_system.time_ticks(),
                         addr, len, (what==BX_READ) ? "read" : "write", val);
        print_header = 0;
      }
      else {
        // *no* need to output the event header
        dbg_printf(" 0x%x", val);
      }
    }
    if (bx_dbg_batch_dma.Qsize)
      dbg_printf("\n");
  }

  // empty Q, regardless of whether reports are printed
  bx_dbg_batch_dma.Qsize = 0;
}

void bx_dbg_dump_table(void)
{
  Bit32u lin; // show only low 32 bit
  Bit32u phy;
  Bit32u start_lin, start_phy;  // start of a valid translation interval
  bx_bool valid;

  if (BX_CPU(dbg_cpu)->cr0.pg == 0) {
    printf("paging off\n");
    return;
  }

  printf("cr3: " FMT_ADDRX "\n", BX_CPU(dbg_cpu)->cr3);

  lin = 0; 
  phy = 0; 

  start_lin = 1;
  start_phy = 2;
  while(1) {
    valid = BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(lin, &phy);
    if(valid) {
      if((lin - start_lin) != (phy - start_phy)) {
        if(start_lin != 1)
          dbg_printf("0x%08x-0x%08x -> 0x%08x-0x%08x\n",
            start_lin, lin - 1, start_phy, start_phy + (lin-1-start_lin));
        start_lin = lin;
        start_phy = phy;
      }
    } else {
      if(start_lin != 1)
        dbg_printf("0x%08x-0x%08x -> 0x%08x-0x%08x\n",
          start_lin, lin - 1, start_phy, start_phy + (lin-1-start_lin));
      start_lin = 1;
      start_phy = 2;
    }

    if(lin == 0xfffff000) break;
    lin += 0x1000;
  }
  if(start_lin != 1)
    dbg_printf("0x%08x-0x%08x -> 0x%08x-0x%08x\n",
         start_lin, 0xffffffff, start_phy, start_phy + (0xffffffff-start_lin));
}

void bx_dbg_print_help(void)
{ 
  dbg_printf("h|help - show list of debugger commands\n");
  dbg_printf("h|help command - show short command description\n");
  dbg_printf("-*- Debugger control -*-\n");
  dbg_printf("    help, q|quit|exit, set, instrument, show, trace, trace-reg,\n");
  dbg_printf("    record, playback, load-symbols, slist\n");
  dbg_printf("-*- Execution control -*-\n");
  dbg_printf("    c|cont|continue, s|step|stepi, p|n|next, modebp\n");
  dbg_printf("-*- Breakpoint management -*-\n");
  dbg_printf("    vb|vbreak, lb|lbreak, pb|pbreak|b|break, sb, sba, blist,\n");
  dbg_printf("    bpe, bpd, d|del|delete\n");
  dbg_printf("-*- CPU and memory contents -*-\n");
  dbg_printf("    x, xp, u|disasm|disassemble, r|reg|regs|registers, setpmem, crc, info,\n");
  dbg_printf("    set, dump_cpu, set_cpu, ptime, print-stack, watch, unwatch, ?|calc\n");
}

void bx_dbg_calc_command(Bit64u value)
{
  dbg_printf("0x" FMT_LL "x " FMT_LL "d\n", value, value);
}

Bit8u bx_dbg_get_reg8l_value(unsigned reg)
{
  if (reg < BX_GENERAL_REGISTERS)
    return BX_CPU(dbg_cpu)->get_reg8l(reg);

  dbg_printf("Unknown 8BL register [%d] !!!\n", reg);
  return 0;
}

Bit8u bx_dbg_get_reg8h_value(unsigned reg)
{
  if (reg < BX_GENERAL_REGISTERS)
    return BX_CPU(dbg_cpu)->get_reg8h(reg);

  dbg_printf("Unknown 8BH register [%d] !!!\n", reg);
  return 0;
}

Bit16u bx_dbg_get_reg16_value(unsigned reg)
{
  if (reg < BX_GENERAL_REGISTERS)
    return BX_CPU(dbg_cpu)->get_reg16(reg);

  dbg_printf("Unknown 16B register [%d] !!!\n", reg);
  return 0;
}

Bit32u bx_dbg_get_reg32_value(unsigned reg)
{
  if (reg < BX_GENERAL_REGISTERS)
    return BX_CPU(dbg_cpu)->get_reg32(reg);

  dbg_printf("Unknown 32B register [%d] !!!\n", reg);
  return 0;
}

Bit64u bx_dbg_get_reg64_value(unsigned reg)
{
#if BX_SUPPORT_X86_64
  if (reg < BX_GENERAL_REGISTERS)
    return BX_CPU(dbg_cpu)->get_reg64(reg);
#endif

  dbg_printf("Unknown 64B register [%d] !!!\n", reg);
  return 0;
}

void bx_dbg_set_reg8l_value(unsigned reg, Bit8u value)
{
  if (reg < BX_GENERAL_REGISTERS)
    BX_CPU(dbg_cpu)->set_reg8l(reg, value);
  else
    dbg_printf("Unknown 8BL register [%d] !!!\n", reg);
}

void bx_dbg_set_reg8h_value(unsigned reg, Bit8u value)
{
  if (reg < BX_GENERAL_REGISTERS)
    BX_CPU(dbg_cpu)->set_reg8h(reg, value);
  else
    dbg_printf("Unknown 8BH register [%d] !!!\n", reg);
}

void bx_dbg_set_reg16_value(unsigned reg, Bit16u value)
{
  if (reg < BX_GENERAL_REGISTERS)
    BX_CPU(dbg_cpu)->set_reg16(reg, value);
  else
    dbg_printf("Unknown 16B register [%d] !!!\n", reg);
}

void bx_dbg_set_reg32_value(unsigned reg, Bit32u value)
{
  if (reg < BX_GENERAL_REGISTERS)
    BX_CPU(dbg_cpu)->set_reg32(reg, value);
  else
    dbg_printf("Unknown 32B register [%d] !!!\n", reg);
}

void bx_dbg_set_reg64_value(unsigned reg, Bit64u value)
{
#if BX_SUPPORT_X86_64
  if (reg < BX_GENERAL_REGISTERS)
    BX_CPU(dbg_cpu)->set_reg64(reg, value);
  else
#endif
    dbg_printf("Unknown 64B register [%d] !!!\n", reg);
}

Bit16u bx_dbg_get_selector_value(unsigned int seg_no)
{
  bx_dbg_sreg_t sreg;

  if (seg_no > 5) {
    dbg_printf("Error: seg_no out of bounds\n");
    return 0;
  }
  BX_CPU(dbg_cpu)->dbg_get_sreg(&sreg, seg_no);
  if (!sreg.valid) {
    dbg_printf("Error: segment valid bit cleared\n");
    return 0;
  }
  return sreg.sel;
}

Bit16u bx_dbg_get_ip(void)
{
  return (BX_CPU(dbg_cpu)->get_ip() & 0xffff);
}

Bit32u bx_dbg_get_eip(void)
{
  return (BX_CPU(dbg_cpu)->get_ip() & 0xffffffff);
}

bx_address bx_dbg_get_instruction_pointer(void)
{
  return BX_CPU(dbg_cpu)->get_ip();
}

Bit32u bx_dbg_get_laddr(Bit16u sel, Bit32u ofs)
{
  if (BX_CPU(dbg_cpu)->protected_mode()) {
    bx_descriptor_t descriptor;
    bx_selector_t selector;
    Bit32u dword1, dword2;

    /* if selector is NULL, error */
    if ((sel & 0xfffc) == 0) {
      dbg_printf("ERROR: Dereferencing a NULL selector!\n");
      return 0;
    }

    /* parse fields in selector */
    BX_CPU(dbg_cpu)->parse_selector(sel, &selector);

    Bit32u desc_base;
    if (selector.ti) {
      // LDT
      if (((Bit32u)selector.index*8 + 7) > BX_CPU(dbg_cpu)->ldtr.cache.u.system.limit) {
        dbg_printf("ERROR: selector (%04x) > GDT size limit\n", selector.index*8);
        return 0;
      }
      desc_base = BX_CPU(dbg_cpu)->ldtr.cache.u.system.base;
    }
    else {
      // GDT
      if (((Bit32u)selector.index*8 + 7) > BX_CPU(dbg_cpu)->gdtr.limit) {
        dbg_printf("ERROR: selector (%04x) > GDT size limit\n", selector.index*8);
        return 0;
      }
      desc_base = BX_CPU(dbg_cpu)->gdtr.base;
    }

    BX_CPU(dbg_cpu)->access_linear(desc_base + selector.index * 8,     4, 0, BX_READ, &dword1);
    BX_CPU(dbg_cpu)->access_linear(desc_base + selector.index * 8 + 4, 4, 0, BX_READ, &dword2);

    memset (&descriptor, 0, sizeof (descriptor));
    BX_CPU(dbg_cpu)->parse_descriptor(dword1, dword2, &descriptor);

    if (!descriptor.segment) {
      dbg_printf("ERROR: selector %04x points to a system descriptor and is not supported!\n", sel);
      return 0;
    }

    /* #NP(selector) if descriptor is not present */
    if (descriptor.p==0) {
      dbg_printf("ERROR: descriptor %04x not present!\n", sel);
      return 0;
    }

    Bit32u lowaddr, highaddr;

    // expand-down
    if (IS_DATA_SEGMENT(descriptor.type) && IS_DATA_SEGMENT_EXPAND_DOWN(descriptor.type)) {
      lowaddr = descriptor.u.segment.limit_scaled;
      highaddr = descriptor.u.segment.g ? 0xffffffff : 0xffff;
    }
    else {
      lowaddr = 0;
      highaddr = descriptor.u.segment.limit_scaled;
    }

    if ((ofs < lowaddr) || (ofs > highaddr)) {
      dbg_printf("WARNING: Offset %08X is out of selector %04x limit (%08x...%08x)!\n",
        ofs, sel, lowaddr, highaddr);
    }

    return descriptor.u.segment.base + ofs;
  }
  else {
    return sel * 16 + ofs;
  }
}

void bx_dbg_step_over_command()
{
  bx_address Laddr = BX_CPU(dbg_cpu)->guard_found.laddr;

  if (! bx_dbg_read_linear(dbg_cpu, Laddr, 16, bx_disasm_ibuf))
  {
    return;
  }

  x86_insn insn = bx_disassemble.decode(BX_CPU(dbg_cpu)->guard_found.is_32bit_code,
      BX_CPU(dbg_cpu)->guard_found.is_64bit_code,
      BX_CPU(dbg_cpu)->get_segment_base(BX_SEG_REG_CS), 
      BX_CPU(dbg_cpu)->guard_found.eip, bx_disasm_ibuf, bx_disasm_tbuf);

  unsigned b1 = insn.b1;

  switch(b1) {
    // Jcc short
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7A:
    case 0x7B:
    case 0x7C:
    case 0x7D:
    case 0x7E:
    case 0x7F:
    
    // Jcc near
    case 0x180:
    case 0x181:
    case 0x182:
    case 0x183:
    case 0x184:
    case 0x185:
    case 0x186:
    case 0x187:
    case 0x188:
    case 0x189:
    case 0x18A:
    case 0x18B:
    case 0x18C:
    case 0x18D:
    case 0x18E:
    case 0x18F:

    // jcxz
    case 0xE3:

    // retn n
    case 0xC2:
    // retn
    case 0xC3:
    // retf n
    case 0xCA:
    // retf
    case 0xCB:
    // iret
    case 0xCF:

    // jmp near
    case 0xE9:
    // jmp far
    case 0xEA:
    // jmp short
    case 0xEB:
      bx_dbg_stepN_command(1);
      return;
    // jmp absolute indirect
    case 0xFF:
      switch (insn.nnn) {
        // near
        case 4:
        // far
        case 5:
         bx_dbg_stepN_command (1);
         return;
      }
  }

  // calls, ints, loops and so on
  int BpId = bx_dbg_lbreakpoint_command(bkStepOver, Laddr + insn.ilen);
  if (BpId == -1) {
    dbg_printf("bx_dbg_step_over_command:: Failed to set lbreakpoint !\n");
    return;
  }
  
  bx_dbg_continue_command();
  
  if (bx_dbg_del_lbreak (BpId))
    bx_dbg_breakpoint_changed();
}

void bx_dbg_info_flags(void)
{
  dbg_printf("IOPL=%1u %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",
    BX_CPU(dbg_cpu)->get_IOPL(),
    BX_CPU(dbg_cpu)->get_ID() ? "ID" : "id",
    BX_CPU(dbg_cpu)->get_VIP() ? "VIP" : "vip",
    BX_CPU(dbg_cpu)->get_VIF() ? "VIF" : "vif",
    BX_CPU(dbg_cpu)->get_AC() ? "AC" : "ac",
    BX_CPU(dbg_cpu)->get_VM() ? "VM" : "vm",
    BX_CPU(dbg_cpu)->get_RF() ? "RF" : "rf",
    BX_CPU(dbg_cpu)->get_NT() ? "NT" : "nt",
    BX_CPU(dbg_cpu)->get_OF() ? "OF" : "of",
    BX_CPU(dbg_cpu)->get_DF() ? "DF" : "df",
    BX_CPU(dbg_cpu)->get_IF() ? "IF" : "if",
    BX_CPU(dbg_cpu)->get_TF() ? "TF" : "tf",
    BX_CPU(dbg_cpu)->get_SF() ? "SF" : "sf",
    BX_CPU(dbg_cpu)->get_ZF() ? "ZF" : "zf",
    BX_CPU(dbg_cpu)->get_AF() ? "AF" : "af",
    BX_CPU(dbg_cpu)->get_PF() ? "PF" : "pf",
    BX_CPU(dbg_cpu)->get_CF() ? "CF" : "cf");
}

#endif /* if BX_DEBUGGER */
