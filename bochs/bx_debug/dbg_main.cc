/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2020  The Bochs Project
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
#include "cpu/cpu.h"
#include "cpu/decoder/ia_opcodes.h"
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
bx_list_c *dbg_cpu_list = 0;

extern const char* cpu_mode_string(unsigned cpu_mode);
extern void bx_sr_after_restore_state(void);

void bx_dbg_print_descriptor(Bit32u lo, Bit32u hi);
void bx_dbg_print_descriptor64(Bit32u lo1, Bit32u hi1, Bit32u lo2, Bit32u hi2);

static bx_param_bool_c *sim_running = NULL;
static bool bx_dbg_exit_called;

static char tmp_buf[512];
static char tmp_buf_prev[512];
static char *tmp_buf_ptr;
static char *argv0 = NULL;

static FILE *debugger_log = NULL;

static struct bx_debugger_state {
  // some fields used for single CPU debugger
  bool  auto_disassemble;
  unsigned disassemble_size;
  char     default_display_format;
  char     default_unit_size;
  bx_address default_addr;
} bx_debugger;

typedef struct _debug_info_t {
  const char *name;
  bx_devmodel_c *device;
  struct _debug_info_t *next;
} debug_info_t;

static debug_info_t *bx_debug_info_list = NULL;

typedef struct {
  FILE    *fp;
  char     fname[BX_MAX_PATH];
  unsigned lineno;
} bx_infile_stack_entry_t;

bx_infile_stack_entry_t bx_infile_stack[BX_INFILE_DEPTH];
int                     bx_infile_stack_index = 0;

static int  bx_nest_infile(char *path);

void CDECL bx_debug_ctrlc_handler(int signum);

static void bx_unnest_infile(void);
static void bx_get_command(void);
static void bx_dbg_print_guard_results();
extern void bx_dbg_breakpoint_changed(void);
static void bx_dbg_set_icount_guard(int which_cpu, Bit32u n);

bx_guard_t bx_guard;

// DMA stuff
void bx_dbg_post_dma_reports(void);
#define BX_BATCH_DMA_BUFSIZE 512

static struct {
  unsigned this_many; // batch this many max before posting events
  unsigned Qsize;     // this many have been batched
  struct {
    bx_phy_address addr; // address of DMA op
    unsigned len;        // number of bytes in op
    unsigned what;       // BX_READ or BX_WRITE
    Bit32u   val;        // value of DMA op
    Bit64u   time;       // system time at this dma op
  } Q[BX_BATCH_DMA_BUFSIZE];
} bx_dbg_batch_dma;

// some buffers for disassembly
static bool disasm_syntax_intel = 1;
static Bit8u bx_disasm_ibuf[32];
static char  bx_disasm_tbuf[512];

static bool watchpoint_continue = 0;
unsigned num_write_watchpoints = 0;
unsigned num_read_watchpoints = 0;
bx_watchpoint write_watchpoint[BX_DBG_MAX_WATCHPONTS];
bx_watchpoint read_watchpoint[BX_DBG_MAX_WATCHPONTS];

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

bool bx_dbg_register_debug_info(const char *devname, void *dev)
{
  debug_info_t *debug_info = new debug_info_t;
  debug_info->name = devname;
  debug_info->device = (bx_devmodel_c*)dev;
  debug_info->next = NULL;

  if (bx_debug_info_list == NULL) {
    bx_debug_info_list = debug_info;
  } else {
    debug_info_t *temp = bx_debug_info_list;

    while (temp->next) {
      if (!strcmp(temp->name, devname)) {
        delete debug_info;
        return 0;
      }
      temp = temp->next;
    }
    temp->next = debug_info;
  }
  return 1;
}

void bx_dbg_info_cleanup(void)
{
  debug_info_t *temp = bx_debug_info_list, *next;
  while (temp != NULL) {
    next = temp->next;
    delete temp;
    temp = next;
  }
  bx_debug_info_list = NULL;
}

bool bx_dbg_info_find_device(const char *devname, debug_info_t **found_debug_info)
{
  debug_info_t *debug_info;

  for (debug_info = bx_debug_info_list; debug_info; debug_info = debug_info->next) {
    if (!strcmp(debug_info->name, devname)) {
      *found_debug_info = debug_info;
      return 1;
    }
  }
  return 0;
}

int bx_dbg_main(void)
{
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  bx_dbg_exit_called = 0;
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

  dbg_cpu_list = (bx_list_c*) SIM->get_param("cpu0", SIM->get_bochs_root());
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
  for (int i=0; i<BX_SMP_PROCESSORS; i++) {
    BX_CPU(i)->guard_found.cs = BX_CPU(i)->sregs[BX_SEG_REG_CS].selector.value;
    BX_CPU(i)->guard_found.eip = BX_CPU(i)->get_instruction_pointer();
    BX_CPU(i)->guard_found.laddr =
      BX_CPU(i)->get_laddr(BX_SEG_REG_CS, BX_CPU(i)->guard_found.eip);
    BX_CPU(i)->guard_found.code_32_64 = 0;
    // 00 - 16 bit, 01 - 32 bit, 10 - 64-bit, 11 - illegal
    if (BX_CPU(i)->sregs[BX_SEG_REG_CS].cache.u.segment.d_b)
      BX_CPU(i)->guard_found.code_32_64 |= 0x1;
    if (BX_CPU(i)->get_cpu_mode() == BX_MODE_LONG_64)
      BX_CPU(i)->guard_found.code_32_64 |= 0x2;
  }
  // finally, call the usual function to print the disassembly
  dbg_printf("Next at t=" FMT_LL "d\n", bx_pc_system.time_ticks());
  bx_dbg_disassemble_current(-1, 0);  // all cpus, don't print time

  bx_dbg_user_input_loop();

  if(debugger_log != NULL) {
    fclose(debugger_log);
    debugger_log = NULL;
  }

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
      add_history(charptr_ret);
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
      bx_guard.interrupt_requested = 0;
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
  bx_guard.interrupt_requested = 1;
}

void bx_dbg_exception(unsigned cpu, Bit8u vector, Bit16u error_code)
{
  static const char *exception[] = {
     "(#DE) divide by zero",
     "(#DB) debug break",
     "(#NMI)",
     "(#BP) breakpoint match",
     "(#OF) overflow",
     "(#BR) boundary check",
     "(#UD) undefined opcode",
     "(#NM) device not available",
     "(#DF) double fault",
     "(#CO) coprocessor overrun",
     "(#TS) invalid TSS",
     "(#NP) segment not present",
     "(#SS) stack fault",
     "(#GP) general protection fault",
     "(#PF) page fault",
     "(#RESERVED)",
     "(#MF) floating point error",
     "(#AC) alignment check",
     "(#MC) machine check",
     "(#XF) SIMD floating point exception",
     "(#VE) Virtualization Exception",
     "(#CP) Shadow Stack Protection",
  };

  if (BX_CPU(dbg_cpu)->trace || bx_dbg.exceptions)
  {
    if (vector <= BX_CP_EXCEPTION) {
      dbg_printf("CPU %d: Exception 0x%02x - %s occurred (error_code=0x%04x)\n",
        cpu, vector, exception[vector], error_code);
    }
    else {
      dbg_printf("CPU %d: Exception 0x%02x occurred (error_code=0x%04x)\n",
        cpu, vector, error_code);
    }
  }
}

void bx_dbg_interrupt(unsigned cpu, Bit8u vector, Bit16u error_code)
{
  if (BX_CPU(dbg_cpu)->trace || bx_dbg.interrupts)
  {
    dbg_printf("CPU %d: Interrupt 0x%02x occurred (error_code=0x%04x)\n",
      cpu, vector, error_code);
  }
}

void bx_dbg_halt(unsigned cpu)
{
  if (BX_CPU(cpu)->trace)
  {
    dbg_printf("CPU %d: HALTED\n", cpu);
  }
}

void bx_dbg_watchpoint_continue(bool watch_continue)
{
  watchpoint_continue = watch_continue;
  if (watchpoint_continue) {
     dbg_printf("Will not stop on watch points (they will still be logged)\n");
  }
  else {
     dbg_printf("Will stop on watch points\n");
  }
}

void bx_dbg_check_memory_watchpoints(unsigned cpu, bx_phy_address phy, unsigned len, unsigned rw)
{
  bx_phy_address phy_end = phy + len - 1;

  if (rw & 1) {
    // Check for physical write watch points
    for (unsigned i = 0; i < num_write_watchpoints; i++) {
      bx_phy_address watch_end = write_watchpoint[i].addr + write_watchpoint[i].len - 1;
      if (watch_end < phy || phy_end < write_watchpoint[i].addr) continue;
      BX_CPU(cpu)->watchpoint  = phy;
      BX_CPU(cpu)->break_point = BREAK_POINT_WRITE;
      break;
    }
  }
  else {
    // Check for physical read watch points
    for (unsigned i = 0; i < num_read_watchpoints; i++) {
      bx_phy_address watch_end = read_watchpoint[i].addr + read_watchpoint[i].len - 1;
      if (watch_end < phy || phy_end < read_watchpoint[i].addr) continue;
      BX_CPU(cpu)->watchpoint  = phy;
      BX_CPU(cpu)->break_point = BREAK_POINT_READ;
      break;
    }
  }
}

extern const char *get_memtype_name(BxMemtype memtype);

void bx_dbg_lin_memory_access(unsigned cpu, bx_address lin, bx_phy_address phy, unsigned len, unsigned memtype, unsigned rw, Bit8u *data)
{
  bx_dbg_check_memory_watchpoints(cpu, phy, len, rw);

  if (! BX_CPU(cpu)->trace_mem)
    return;

  const char *access_type[] = {"RD","WR","EX","RW","SR","SW","--","SRW"};

  dbg_printf("[CPU%d %s]: LIN 0x" FMT_ADDRX " PHY 0x" FMT_PHY_ADDRX " (len=%d, %s)",
     cpu, access_type[rw],
     lin, phy, len, get_memtype_name(memtype));

  if (len == 1) {
     Bit8u val8 = *data;
     dbg_printf(": 0x%02X", (unsigned) val8);
  }
  else if (len == 2) {
     Bit16u val16 = *((Bit16u*) data);
     dbg_printf(": 0x%04X", (unsigned) val16);
  }
  else if (len == 4) {
     Bit32u val32 = *((Bit32u*) data);
     dbg_printf(": 0x%08X", (unsigned) val32);
  }
  else if (len == 8) {
     Bit64u data64 = * (Bit64u*)(data);
     dbg_printf(": 0x%08X 0x%08X", GET32H(data64), GET32L(data64));
  }
#if BX_CPU_LEVEL >= 6
  else if (len == 16) {
     const BxPackedXmmRegister *xmmdata = (const BxPackedXmmRegister*)(data);
     dbg_printf(": 0x%08X 0x%08X 0x%08X 0x%08X",
         xmmdata->xmm32u(3), xmmdata->xmm32u(2), xmmdata->xmm32u(1), xmmdata->xmm32u(0));
  }
#if BX_SUPPORT_AVX
  else if (len == 32) {
     const BxPackedYmmRegister *ymmdata = (const BxPackedYmmRegister*)(data);
     dbg_printf(": 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X",
        ymmdata->ymm32u(7), ymmdata->ymm32u(6), ymmdata->ymm32u(5), ymmdata->ymm32u(4),
        ymmdata->ymm32u(3), ymmdata->ymm32u(2), ymmdata->ymm32u(1), ymmdata->ymm32u(0));
  }
#endif
#if BX_SUPPORT_EVEX
  else if (len == 64) {
     const BxPackedZmmRegister *zmmdata = (const BxPackedZmmRegister*)(data);
     dbg_printf(": 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X",
        zmmdata->zmm32u(15), zmmdata->zmm32u(14), zmmdata->zmm32u(13), zmmdata->zmm32u(12),
        zmmdata->zmm32u(11), zmmdata->zmm32u(10), zmmdata->zmm32u(9),  zmmdata->zmm32u(8),
        zmmdata->zmm32u(7),  zmmdata->zmm32u(6),  zmmdata->zmm32u(5),  zmmdata->zmm32u(4),
        zmmdata->zmm32u(3),  zmmdata->zmm32u(2),  zmmdata->zmm32u(1),  zmmdata->zmm32u(0));
  }
#endif
#endif
  else {
     for (int i=len-1;i >= 0;i--) {
        dbg_printf(" %02x", data[i]);
     }
  }

  dbg_printf("\n");
}

void bx_dbg_phy_memory_access(unsigned cpu, bx_phy_address phy, unsigned len, unsigned memtype, unsigned rw, unsigned access, Bit8u *data)
{
  bx_dbg_check_memory_watchpoints(cpu, phy, len, rw);

  if (! BX_CPU(cpu)->trace_mem)
    return;

  static const char *access_string[] = {
    "",
    "PDPTR0",
    "PDPTR1",
    "PDPTR2",
    "PDPTR3",
    "PTE",
    "PDE",
    "PDPTE",
    "PML4E",
    "EPT PTE",
    "EPT PDE",
    "EPT PDPTE",
    "EPT PML4E",
    "EPT SPP PTE",
    "EPT SPP PDE",
    "EPT SPP PDPTE",
    "EPT SPP PML4E",
    "VMCS",
    "SHADOW_VMCS",
    "MSR BITMAP",
    "I/O BITMAP",
    "VMREAD BITMAP",
    "VMWRITE BITMAP",
    "VMX LDMSR",
    "VMX STMSR",
    "VAPIC",
    "PML",
    "SMRAM"
  };

  if (memtype > BX_MEMTYPE_INVALID) memtype = BX_MEMTYPE_INVALID;

  const char *access_type[] = {"RD","WR","EX","RW","SR","SW","--","SRW"};

  dbg_printf("[CPU%d %s]: PHY 0x" FMT_PHY_ADDRX " (len=%d, %s)",
     cpu, access_type[rw],
     phy, len, get_memtype_name(memtype));

  if (len == 1) {
     Bit8u val8 = *data;
     dbg_printf(": 0x%02X", (unsigned) val8);
  }
  else if (len == 2) {
     Bit16u val16 = *((Bit16u*) data);
     dbg_printf(": 0x%04X", (unsigned) val16);
  }
  else if (len == 4) {
     Bit32u val32 = *((Bit32u*) data);
     dbg_printf(": 0x%08X", (unsigned) val32);
  }
  else if (len == 8) {
     Bit64u data64 = * (Bit64u*)(data);
     dbg_printf(": 0x%08X 0x%08X", GET32H(data64), GET32L(data64));
  }
#if BX_CPU_LEVEL >= 6
  else if (len == 16) {
     const BxPackedXmmRegister *xmmdata = (const BxPackedXmmRegister*)(data);
     dbg_printf(": 0x%08X 0x%08X 0x%08X 0x%08X",
         xmmdata->xmm32u(3), xmmdata->xmm32u(2), xmmdata->xmm32u(1), xmmdata->xmm32u(0));
  }
#if BX_SUPPORT_AVX
  else if (len == 32) {
     const BxPackedYmmRegister *ymmdata = (const BxPackedYmmRegister*)(data);
     dbg_printf(": 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X",
        ymmdata->ymm32u(7), ymmdata->ymm32u(6), ymmdata->ymm32u(5), ymmdata->ymm32u(4),
        ymmdata->ymm32u(3), ymmdata->ymm32u(2), ymmdata->ymm32u(1), ymmdata->ymm32u(0));
  }
#endif
#if BX_SUPPORT_EVEX
  else if (len == 64) {
     const BxPackedZmmRegister *zmmdata = (const BxPackedZmmRegister*)(data);
     dbg_printf(": 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X",
        zmmdata->zmm32u(15), zmmdata->zmm32u(14), zmmdata->zmm32u(13), zmmdata->zmm32u(12),
        zmmdata->zmm32u(11), zmmdata->zmm32u(10), zmmdata->zmm32u(9),  zmmdata->zmm32u(8),
        zmmdata->zmm32u(7),  zmmdata->zmm32u(6),  zmmdata->zmm32u(5),  zmmdata->zmm32u(4),
        zmmdata->zmm32u(3),  zmmdata->zmm32u(2),  zmmdata->zmm32u(1),  zmmdata->zmm32u(0));
  }
#endif
#endif
  else {
     for (int i=len-1;i >= 0;i--) {
        dbg_printf(" %02x", data[i]);
     }
  }

  if (access != 0)
    dbg_printf("\t; %s\n", access_string[access]);
  else
    dbg_printf("\n");
}

void bx_dbg_exit(int code)
{
  if (bx_dbg_exit_called) return;
  bx_dbg_exit_called = 1;
  BX_DEBUG(("dbg: before exit"));
  for (int cpu=0; cpu < BX_SMP_PROCESSORS; cpu++) {
    if (BX_CPU(cpu)) BX_CPU(cpu)->atexit();
  }

  bx_atexit();
  BX_EXIT(code);
}

//
// functions for browsing of cpu state
//

void bx_dbg_print_mxcsr_state(void)
{
#if BX_CPU_LEVEL >= 6
  Bit32u mxcsr = SIM->get_param_num("SSE.mxcsr", dbg_cpu_list)->get();

  static const char* round_control[] = {
    "Nearest", "Down", "Up", "Chop"
  };

  dbg_printf("MXCSR: 0x%05x: %s %s RC:%s %s %s %s %s %s %s %s %s %s %s %s %s %s\n", mxcsr,
     (mxcsr & (1<<17)) ? "ULE" : "ule",
     (mxcsr & (1<<15)) ? "FUZ" : "fuz",
     round_control[(mxcsr >> 13) & 3],
     (mxcsr & (1<<12)) ? "PM" : "pm",
     (mxcsr & (1<<11)) ? "UM" : "um",
     (mxcsr & (1<<10)) ? "OM" : "om",
     (mxcsr & (1<<9)) ? "ZM" : "zm",
     (mxcsr & (1<<8)) ? "DM" : "dm",
     (mxcsr & (1<<7)) ? "IM" : "im",
     (mxcsr & (1<<6)) ? "DAZ" : "daz",
     (mxcsr & (1<<5)) ? "PE" : "pe",
     (mxcsr & (1<<4)) ? "UE" : "ue",
     (mxcsr & (1<<3)) ? "OE" : "oe",
     (mxcsr & (1<<2)) ? "ZE" : "ze",
     (mxcsr & (1<<1)) ? "DE" : "de",
     (mxcsr & (1<<0)) ? "IE" : "ie");
#endif
}

void bx_dbg_print_sse_state(void)
{
#if BX_CPU_LEVEL >= 6
  if (BX_CPU(dbg_cpu)->is_cpu_extension_supported(BX_ISA_SSE)) {
    bx_dbg_print_mxcsr_state();

    char param_name[20];
    unsigned registers = BX_SUPPORT_X86_64 ? 16 : 8; // don't want to print XMM16..31 in EVEX mode
    for(unsigned i=0;i<registers;i++) {
      sprintf(param_name, "SSE.xmm%02d_1", i);
      Bit64u hi = SIM->get_param_num(param_name, dbg_cpu_list)->get64();
      sprintf(param_name, "SSE.xmm%02d_0", i);
      Bit64u lo = SIM->get_param_num(param_name, dbg_cpu_list)->get64();
      dbg_printf("XMM[%02u]: %08x_%08x_%08x_%08x\n", i,
         GET32H(hi), GET32L(hi), GET32H(lo), GET32L(lo));
    }
  }
  else
#endif
  {
    dbg_printf("The CPU doesn't support SSE state !\n");
  }
}

void bx_dbg_print_avx_state(unsigned vlen)
{
#if BX_SUPPORT_AVX
  char param_name[20];

  if (BX_CPU(dbg_cpu)->is_cpu_extension_supported(BX_ISA_AVX)) {
    bx_dbg_print_mxcsr_state();

    unsigned num_regs = 16;
#if BX_SUPPORT_EVEX
    if (BX_CPU(dbg_cpu)->is_cpu_extension_supported(BX_ISA_AVX512))
      num_regs = BX_XMM_REGISTERS;
    else
      vlen = BX_VL256;
#endif

    for(unsigned i=0;i<num_regs;i++) {
      dbg_printf("VMM[%02u]: ", i);
      for (int j=vlen-1;j >= 0; j--) {
        sprintf(param_name, "SSE.xmm%02u_%d", i, j*2+1);
        Bit64u hi = SIM->get_param_num(param_name, dbg_cpu_list)->get64();
        sprintf(param_name, "SSE.xmm%02u_%d", i, j*2);
        Bit64u lo = SIM->get_param_num(param_name, dbg_cpu_list)->get64();
        dbg_printf("%08x_%08x_%08x_%08x", GET32H(hi), GET32L(hi), GET32H(lo), GET32L(lo));
        if (j!=0) dbg_printf("_");
      }
      dbg_printf("\n");
    }
  }
  else
#endif
  {
    dbg_printf("The CPU doesn't support AVX state !\n");
  }

#if BX_SUPPORT_EVEX
  if (BX_CPU(dbg_cpu)->is_cpu_extension_supported(BX_ISA_AVX512)) {
    for(unsigned i=0;i<8;i++) {
      sprintf(param_name, "OPMASK.k%u", i);
      Bit64u opmask = SIM->get_param_num(param_name, dbg_cpu_list)->get64();
      dbg_printf("K%d: %08x_%08x\n", i, GET32H(opmask), GET32L(opmask));
    }
  }
#endif
}

void bx_dbg_print_mmx_state(void)
{
#if BX_CPU_LEVEL >= 5
  if (BX_CPU(dbg_cpu)->is_cpu_extension_supported(BX_ISA_MMX)) {
    char param_name[20];
    for(unsigned i=0;i<8;i++) {
      sprintf(param_name, "FPU.st%d.fraction", i);
      Bit64u mmreg = SIM->get_param_num(param_name, dbg_cpu_list)->get64();
      dbg_printf("MM[%d]: %08x_%08x\n", i, GET32H(mmreg), GET32L(mmreg));
    }
  }
  else
#endif
  {
    dbg_printf("The CPU doesn't support MMX state !\n");
  }
}

void bx_dbg_print_fpu_state(void)
{
#if BX_SUPPORT_FPU
  BX_CPU(dbg_cpu)->print_state_FPU();
#else
  dbg_printf("The CPU doesn't support FPU state !\n");
#endif
}

void bx_dbg_info_flags(void)
{
  dbg_printf("%s %s %s %s %s %s %s IOPL=%1u %s %s %s %s %s %s %s %s %s\n",
    BX_CPU(dbg_cpu)->get_ID() ? "ID" : "id",
    BX_CPU(dbg_cpu)->get_VIP() ? "VIP" : "vip",
    BX_CPU(dbg_cpu)->get_VIF() ? "VIF" : "vif",
    BX_CPU(dbg_cpu)->get_AC() ? "AC" : "ac",
    BX_CPU(dbg_cpu)->get_VM() ? "VM" : "vm",
    BX_CPU(dbg_cpu)->get_RF() ? "RF" : "rf",
    BX_CPU(dbg_cpu)->get_NT() ? "NT" : "nt",
    BX_CPU(dbg_cpu)->get_IOPL(),
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

void bx_dbg_info_debug_regs_command(void)
{
  bx_address dr0 = SIM->get_param_num("DR0", dbg_cpu_list)->get64();
  bx_address dr1 = SIM->get_param_num("DR1", dbg_cpu_list)->get64();
  bx_address dr2 = SIM->get_param_num("DR2", dbg_cpu_list)->get64();
  bx_address dr3 = SIM->get_param_num("DR3", dbg_cpu_list)->get64();
  Bit32u dr6 = SIM->get_param_num("DR6", dbg_cpu_list)->get();
  Bit32u dr7 = SIM->get_param_num("DR7", dbg_cpu_list)->get();

  dbg_printf("DR0=0x" FMT_ADDRX "\n", dr0);
  dbg_printf("DR1=0x" FMT_ADDRX "\n", dr1);
  dbg_printf("DR2=0x" FMT_ADDRX "\n", dr2);
  dbg_printf("DR3=0x" FMT_ADDRX "\n", dr3);

  static const char *dr_ln[4] = { "Byte", "Word", "QWord", "Dword" };
  static const char *dr_rw[4] = { "Code", "DataW", "I/O", "DataRW" };

  dbg_printf("DR6=0x%08x: %s %s %s %s %s %s %s\n", dr6,
      (dr6 & (1<<15)) ? "BT" : "bt",
      (dr6 & (1<<14)) ? "BS" : "bs",
      (dr6 & (1<<13)) ? "BD" : "bd",
      (dr6 & (1<<3)) ? "B3" : "b3",
      (dr6 & (1<<2)) ? "B2" : "b2",
      (dr6 & (1<<1)) ? "B1" : "b1",
      (dr6 & (1<<0)) ? "B0" : "b0");

  dbg_printf("DR7=0x%08x: DR3=%s-%s DR2=%s-%s DR1=%s-%s DR0=%s-%s %s | %s %s | %s %s %s %s %s %s %s %s\n", dr7,
      dr_rw[(dr7 >> 28) & 3], dr_ln[(dr7 >> 30) & 3],
      dr_rw[(dr7 >> 24) & 3], dr_ln[(dr7 >> 26) & 3],
      dr_rw[(dr7 >> 20) & 3], dr_ln[(dr7 >> 22) & 3],
      dr_rw[(dr7 >> 16) & 3], dr_ln[(dr7 >> 18) & 3],
      (dr7 & (1<<13)) ? "GD" : "gd",
      (dr7 & (1<<9)) ? "GE" : "ge",
      (dr7 & (1<<8)) ? "LE" : "le",
      (dr7 & (1<<7)) ? "G3" : "g3",
      (dr7 & (1<<6)) ? "L3" : "l3",
      (dr7 & (1<<5)) ? "G2" : "g2",
      (dr7 & (1<<4)) ? "L2" : "l2",
      (dr7 & (1<<3)) ? "G1" : "g1",
      (dr7 & (1<<2)) ? "L1" : "l1",
      (dr7 & (1<<1)) ? "G0" : "g0",
      (dr7 & (1<<0)) ? "L0" : "l0");
}

void bx_dbg_info_control_regs_command(void)
{
  Bit32u cr0 = SIM->get_param_num("CR0", dbg_cpu_list)->get();
  bx_address cr2 = (bx_address) SIM->get_param_num("CR2", dbg_cpu_list)->get64();
  bx_phy_address cr3 = (bx_phy_address) SIM->get_param_num("CR3", dbg_cpu_list)->get64();
  dbg_printf("CR0=0x%08x: %s %s %s %s %s %s %s %s %s %s %s\n", cr0,
    (cr0 & (1<<31)) ? "PG" : "pg",
    (cr0 & (1<<30)) ? "CD" : "cd",
    (cr0 & (1<<29)) ? "NW" : "nw",
    (cr0 & (1<<18)) ? "AC" : "ac",
    (cr0 & (1<<16)) ? "WP" : "wp",
    (cr0 & (1<<5))  ? "NE" : "ne",
    (cr0 & (1<<4))  ? "ET" : "et",
    (cr0 & (1<<3))  ? "TS" : "ts",
    (cr0 & (1<<2))  ? "EM" : "em",
    (cr0 & (1<<1))  ? "MP" : "mp",
    (cr0 & (1<<0))  ? "PE" : "pe");
  dbg_printf("CR2=page fault laddr=0x" FMT_ADDRX "\n", cr2);
  dbg_printf("CR3=0x" FMT_PHY_ADDRX "\n", cr3);
  dbg_printf("    PCD=page-level cache disable=%d\n", (cr3>>4) & 1);
  dbg_printf("    PWT=page-level write-through=%d\n", (cr3>>3) & 1);
#if BX_CPU_LEVEL >= 5
  Bit32u cr4 = SIM->get_param_num("CR4", dbg_cpu_list)->get();
  dbg_printf("CR4=0x%08x: %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n", cr4,
    (cr4 & (1<<25)) ? "UINTR" : "uintr",
    (cr4 & (1<<24)) ? "PKS" : "pks",
    (cr4 & (1<<23)) ? "CET" : "cet",
    (cr4 & (1<<22)) ? "PKE" : "pke",
    (cr4 & (1<<21)) ? "SMAP" : "smap",
    (cr4 & (1<<20)) ? "SMEP" : "smep",
    (cr4 & (1<<19)) ? "KEYLOCK" : "keylock",
    (cr4 & (1<<18)) ? "OSXSAVE" : "osxsave",
    (cr4 & (1<<17)) ? "PCID" : "pcid",
    (cr4 & (1<<16)) ? "FSGSBASE" : "fsgsbase",
    (cr4 & (1<<14)) ? "SMX" : "smx",
    (cr4 & (1<<13)) ? "VMX" : "vmx",
    (cr4 & (1<<12)) ? "LA57" : "la57",
    (cr4 & (1<<11)) ? "UMIP" : "umip",
    (cr4 & (1<<10)) ? "OSXMMEXCPT" : "osxmmexcpt",
    (cr4 & (1<<9))  ? "OSFXSR" : "osfxsr",
    (cr4 & (1<<8))  ? "PCE" : "pce",
    (cr4 & (1<<7))  ? "PGE" : "pge",
    (cr4 & (1<<6))  ? "MCE" : "mce",
    (cr4 & (1<<5))  ? "PAE" : "pae",
    (cr4 & (1<<4))  ? "PSE" : "pse",
    (cr4 & (1<<3))  ? "DE" : "de",
    (cr4 & (1<<2))  ? "TSD" : "tsd",
    (cr4 & (1<<1))  ? "PVI" : "pvi",
    (cr4 & (1<<0))  ? "VME" : "vme");
#if BX_SUPPORT_X86_64
  if (BX_CPU(dbg_cpu)->is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
    dbg_printf("CR8: 0x%x\n", BX_CPU(dbg_cpu)->get_cr8());
  }
#endif
  Bit32u efer = SIM->get_param_num("MSR.EFER", dbg_cpu_list)->get();
  dbg_printf("EFER=0x%08x: %s %s %s %s %s\n", efer,
    (efer & (1<<14)) ? "FFXSR" : "ffxsr",
    (efer & (1<<11)) ? "NXE" : "nxe",
    (efer & (1<<10)) ? "LMA" : "lma",
    (efer & (1<<8))  ? "LME" : "lme",
    (efer & (1<<0))  ? "SCE" : "sce");
#endif
#if BX_CPU_LEVEL >= 6
  if (BX_CPU(dbg_cpu)->is_cpu_extension_supported(BX_ISA_XSAVE)) {
    Bit32u xcr0 = SIM->get_param_num("XCR0", dbg_cpu_list)->get();
    dbg_printf("XCR0=0x%08x: %s %s %s %s %s %s %s %s %s %s %s %s\n", xcr0,
      (xcr0 & (1<<14)) ? "UINTR" : "uintr",
      (xcr0 & (1<<12)) ? "CET_S" : "cet_s",
      (xcr0 & (1<<11)) ? "CET_U" : "cet_u",
      (xcr0 & (1<<9))  ? "PKRU" : "pkru",
      (xcr0 & (1<<7))  ? "HI_ZMM" : "hi_zmm",
      (xcr0 & (1<<6))  ? "ZMM_HI256" : "zmm_hi256",
      (xcr0 & (1<<5))  ? "OPMASK" : "opmask",
      (xcr0 & (1<<4))  ? "BNDCFG" : "bndcfg",
      (xcr0 & (1<<3))  ? "BNDREGS" : "bndregs",
      (xcr0 & (1<<2))  ? "YMM" : "ymm",
      (xcr0 & (1<<1))  ? "SSE" : "sse",
      (xcr0 & (1<<0))  ? "FPU" : "fpu");
  }
#endif
}

void bx_dbg_info_segment_regs_command(void)
{
  static const char *segname[] = { "es", "cs", "ss", "ds", "fs", "gs" };

  bx_dbg_sreg_t sreg;
  bx_dbg_global_sreg_t global_sreg;

  for(int s=0;s<6;s++) {
    BX_CPU(dbg_cpu)->dbg_get_sreg(&sreg, s);
    dbg_printf("%s:0x%04x, dh=0x%08x, dl=0x%08x, valid=%u\n", segname[s],
       (unsigned) sreg.sel, (unsigned) sreg.des_h,
       (unsigned) sreg.des_l, (unsigned) sreg.valid);
    if (sreg.valid) {
       dbg_printf("\t");
       bx_dbg_print_descriptor(sreg.des_l, sreg.des_h);
    }
  }

  BX_CPU(dbg_cpu)->dbg_get_ldtr(&sreg);
  dbg_printf("ldtr:0x%04x, dh=0x%08x, dl=0x%08x, valid=%u\n",
      (unsigned) sreg.sel, (unsigned) sreg.des_h,
      (unsigned) sreg.des_l, (unsigned) sreg.valid);

  BX_CPU(dbg_cpu)->dbg_get_tr(&sreg);
  dbg_printf("tr:0x%04x, dh=0x%08x, dl=0x%08x, valid=%u\n",
      (unsigned) sreg.sel, (unsigned) sreg.des_h,
      (unsigned) sreg.des_l, (unsigned) sreg.valid);

  BX_CPU(dbg_cpu)->dbg_get_gdtr(&global_sreg);
  dbg_printf("gdtr:base=0x" FMT_ADDRX ", limit=0x%x\n",
      global_sreg.base, (unsigned) global_sreg.limit);

  BX_CPU(dbg_cpu)->dbg_get_idtr(&global_sreg);
  dbg_printf("idtr:base=0x" FMT_ADDRX ", limit=0x%x\n",
      global_sreg.base, (unsigned) global_sreg.limit);
}

void bx_dbg_info_registers_command(int which_regs_mask)
{
  bx_address reg;

  if (which_regs_mask & BX_INFO_GENERAL_PURPOSE_REGS) {
#if BX_SUPPORT_SMP
    dbg_printf("CPU%d:\n", BX_CPU(dbg_cpu)->bx_cpuid);
#endif
#if BX_SUPPORT_X86_64 == 0
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_EAX);
    dbg_printf("eax: 0x%08x %d\n", (unsigned) reg, (int) reg);
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_EBX);
    dbg_printf("ebx: 0x%08x %d\n", (unsigned) reg, (int) reg);
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_ECX);
    dbg_printf("ecx: 0x%08x %d\n", (unsigned) reg, (int) reg);
    reg = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_EDX);
    dbg_printf("edx: 0x%08x %d\n", (unsigned) reg, (int) reg);
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
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_RAX);
    dbg_printf("rax: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_RBX);
    dbg_printf("rbx: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_RCX);
    dbg_printf("rcx: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_RDX);
    dbg_printf("rdx: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_RSP);
    dbg_printf("rsp: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_RBP);
    dbg_printf("rbp: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_RSI);
    dbg_printf("rsi: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_RDI);
    dbg_printf("rdi: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_R8);
    dbg_printf("r8 : %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_R9);
    dbg_printf("r9 : %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_R10);
    dbg_printf("r10: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_R11);
    dbg_printf("r11: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_R12);
    dbg_printf("r12: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_R13);
    dbg_printf("r13: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_R14);
    dbg_printf("r14: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_R15);
    dbg_printf("r15: %08x_%08x\n", GET32H(reg), GET32L(reg));
    reg = bx_dbg_get_rip();
    dbg_printf("rip: %08x_%08x\n", GET32H(reg), GET32L(reg));
#if BX_SUPPORT_CET
    if (BX_CPU(dbg_cpu)->is_cpu_extension_supported(BX_ISA_CET)) {
      reg = BX_CPU(dbg_cpu)->get_ssp();
      dbg_printf("ssp: %08x_%08x\n", GET32H(reg), GET32L(reg));
    }
#endif
#endif
    reg = BX_CPU(dbg_cpu)->read_eflags();
    dbg_printf("eflags 0x%08x: ", (unsigned) reg);
    bx_dbg_info_flags();
  }

#if BX_SUPPORT_FPU
  if (which_regs_mask & BX_INFO_FPU_REGS) {
    bx_dbg_print_fpu_state();
  }
#endif

  if (which_regs_mask & BX_INFO_MMX_REGS) {
    bx_dbg_print_mmx_state();
  }

#if BX_SUPPORT_EVEX
  if (which_regs_mask & BX_INFO_ZMM_REGS) {
    bx_dbg_print_avx_state(BX_VL512);
  }
  else
#endif
  {
#if BX_SUPPORT_AVX
    if (which_regs_mask & BX_INFO_YMM_REGS) {
      bx_dbg_print_avx_state(BX_VL256);
    }
    else
#endif
    {
      if (which_regs_mask & BX_INFO_SSE_REGS) {
        bx_dbg_print_sse_state();
      }
    }
  }
}

//
// commands invoked from parser
//

void bx_dbg_quit_command(void)
{
  BX_INFO(("dbg: Quit"));
  bx_dbg_exit(0);
}

void bx_dbg_trace_command(bool enable)
{
  BX_CPU(dbg_cpu)->trace = enable;
  dbg_printf("Tracing %s for CPU%d\n", enable ? "enabled" : "disabled",
     BX_CPU(dbg_cpu)->which_cpu());
}

void bx_dbg_trace_reg_command(bool enable)
{
  BX_CPU(dbg_cpu)->trace_reg = enable;
  dbg_printf("Register-Tracing %s for CPU%d\n", enable ? "enabled" : "disabled",
     BX_CPU(dbg_cpu)->which_cpu());
}

void bx_dbg_trace_mem_command(bool enable)
{
  BX_CPU(dbg_cpu)->trace_mem = enable;
  dbg_printf("Memory-Tracing %s for CPU%d\n", enable ? "enabled" : "disabled",
     BX_CPU(dbg_cpu)->which_cpu());
}

void bx_dbg_ptime_command(void)
{
  dbg_printf("ptime: " FMT_LL "d\n", bx_pc_system.time_ticks());
}

int timebp_timer = -1;
Bit64u timebp_queue[MAX_CONCURRENT_BPS];
int timebp_queue_size = 0;

void bx_dbg_timebp_command(bool absolute, Bit64u time)
{
  Bit64u abs_time = (absolute) ? time : time + bx_pc_system.time_ticks();

  if (abs_time <= bx_pc_system.time_ticks()) {
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
          for (int j = timebp_queue_size; j >= i; j--)
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

Bit64u conv_8xBit8u_to_Bit64u(const Bit8u* buf)
{
  Bit64u ret = 0;
  for (int i = 0; i < 8; i++) {
    ret |= ((Bit64u)(buf[i]) << (8 * i));
  }
  return ret;
}

Bit32u conv_4xBit8u_to_Bit32u(const Bit8u* buf)
{
  Bit32u ret = 0;
  for (int i = 0; i < 4; i++) {
    ret |= ((Bit32u)(buf[i]) << (8 * i));
  }
  return ret;
}

Bit16u conv_2xBit8u_to_Bit16u(const Bit8u* buf)
{
  Bit16u ret = 0;
  for (int i = 0; i < 2; i++) {
    ret |= ((Bit16u)(buf[i]) << (8 * i));
  }
  return ret;
}

// toggles mode switch breakpoint
void bx_dbg_modebp_command()
{
  BX_CPU(dbg_cpu)->mode_break = !BX_CPU(dbg_cpu)->mode_break;
  dbg_printf("mode switch break %s\n",
    BX_CPU(dbg_cpu)->mode_break ? "enabled" : "disabled");
}

// toggles vmexit switch breakpoint
void bx_dbg_vmexitbp_command()
{
#if BX_SUPPORT_VMX
  BX_CPU(dbg_cpu)->vmexit_break = !BX_CPU(dbg_cpu)->vmexit_break;
  dbg_printf("vmexit switch break %s\n",
    BX_CPU(dbg_cpu)->vmexit_break ? "enabled" : "disabled");
#else
  dbg_printf("VMX is not compiled in, cannot set vmexit breakpoint !");
#endif
}

bool bx_dbg_read_linear(unsigned which_cpu, bx_address laddr, unsigned len, Bit8u *buf)
{
  unsigned remainsInPage;
  bx_phy_address paddr;
  unsigned read_len;
  bool paddr_valid;

next_page:
  remainsInPage = 0x1000 - PAGE_OFFSET(laddr);
  read_len = (remainsInPage < len) ? remainsInPage : len;

  paddr_valid = BX_CPU(which_cpu)->dbg_xlate_linear2phy(laddr, &paddr);
  if (paddr_valid) {
    if (! BX_MEM(0)->dbg_fetch_mem(BX_CPU(which_cpu), paddr, read_len, buf)) {
      dbg_printf("bx_dbg_read_linear: physical memory read error (phy=0x" FMT_PHY_ADDRX ", lin=0x" FMT_ADDRX ")\n", paddr, laddr);
      return 0;
    }
  }
  else {
    dbg_printf("bx_dbg_read_linear: physical address not available for linear 0x" FMT_ADDRX "\n", laddr);
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
  bx_address ip = BX_CPU(dbg_cpu)->get_instruction_pointer();
  dbg_printf("(%d) 0x%08x\n", dbg_cpu, ip);
  for (int i = 1; i < 50; i++) {
    // Up
    bx_phy_address paddr;
    Bit8u buf[4];

    // bp = [bp];
    bool paddr_valid = BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(bp, &paddr);
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

void bx_dbg_xlate_address(bx_lin_address laddr)
{
  bx_phy_address paddr;
  bx_address lpf_mask;

  laddr &= BX_CONST64(0xfffffffffffff000);

  bool paddr_valid = BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(laddr, &paddr, &lpf_mask, 1);
  if (paddr_valid) {
    dbg_printf("linear page 0x" FMT_ADDRX " maps to physical page 0x" FMT_PHY_ADDRX "\n", laddr, paddr);
  }
  else {
    dbg_printf("physical address not available for linear 0x" FMT_ADDRX "\n", laddr);
  }
}

void bx_dbg_tlb_lookup(bx_lin_address laddr)
{
  char cpu_param_name[16];

  Bit32u index = BX_CPU(dbg_cpu)->ITLB.get_index_of(laddr);
  sprintf(cpu_param_name, "ITLB.entry%d", index);
  bx_dbg_show_param_command(cpu_param_name, 0);

  index = BX_CPU(dbg_cpu)->DTLB.get_index_of(laddr);
  sprintf(cpu_param_name, "DTLB.entry%d", index);
  bx_dbg_show_param_command(cpu_param_name, 0);
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
    } else if(!strcmp(arg,"all")) {
      dbg_show_mask = ~0x0;
      dbg_printf("Enable all show flags\n");
    } else if(!strcmp(arg,"off")) {
      dbg_show_mask = 0x0;
      dbg_printf("Disable all show flags\n");
    } else if(!strcmp(arg,"dbg_all")) {
      bx_dbg.interrupts = 1;
      bx_dbg.exceptions = 1;
      dbg_printf("Turned ON all bx_dbg flags\n");
      return;
    } else if(!strcmp(arg,"dbg_none")) {
      bx_dbg.interrupts = 0;
      bx_dbg.exceptions = 0;
      dbg_printf("Turned OFF all bx_dbg flags\n");
      return;
    } else if(!strcmp(arg,"vga")){
      SIM->refresh_vga();
      return;
    } else {
      dbg_printf("Unrecognized arg: %s (only 'mode', 'int', 'softint', 'extint', 'iret', 'call', 'all', 'off', 'dbg_all' and 'dbg_none' are valid)\n", arg);
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

void bx_dbg_show_param_command(const char *param, bool xml)
{
  dbg_printf("show param name: <%s>\n", param);
  bx_param_c *node = SIM->get_param(param, SIM->get_bochs_root());
  if (node) {
    print_tree(node, 0, xml);
  }
  else {
    node = SIM->get_param(param, dbg_cpu_list);
    if (node)
      print_tree(node, 0, xml);
    else
      dbg_printf("can't find param <%s> in global or default CPU tree\n", param);
  }
}

// return non zero to cause a stop
int bx_dbg_show_symbolic(void)
{
  static unsigned last_cpu_mode = 0;
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
      dbg_printf(FMT_TICK ": address space switched. CR3: 0x" FMT_PHY_ADDRX "\n",
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
      bool valid = BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(BX_CPU(dbg_cpu)->guard_found.laddr, &phy);
      dbg_printf(FMT_TICK ": call ", bx_pc_system.time_ticks());
      dbg_print_guard_found(BX_CPU(dbg_cpu)->get_cpu_mode(),
        BX_CPU(dbg_cpu)->guard_found.cs, BX_CPU(dbg_cpu)->guard_found.eip,
        BX_CPU(dbg_cpu)->guard_found.laddr);
      if (!valid) dbg_printf(" phys not valid");
      else {
        dbg_printf(" (phy: 0x" FMT_PHY_ADDRX ") %s", phy,
          bx_dbg_symbolic_address(BX_CPU(dbg_cpu)->cr3 >> 12,
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
    linear_sp = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_RSP);
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
      linear_sp = BX_CPU(dbg_cpu)->get_reg16(BX_16BIT_REG_SP);
      len = 2;
    }

    linear_sp = BX_CPU(dbg_cpu)->get_laddr(BX_SEG_REG_SS, linear_sp);
  }

  Bit8u buf[8];

  dbg_printf("Stack address size %d\n", len);

  for (unsigned i = 0; i < nwords; i++) {
    if (! bx_dbg_read_linear(dbg_cpu, linear_sp, len, buf)) break;
#if BX_SUPPORT_X86_64
    if (len == 8) {
      Bit64u addr_on_stack = conv_8xBit8u_to_Bit64u(buf);
      const char *Sym=bx_dbg_disasm_symbolic_address(addr_on_stack, 0);
      dbg_printf(" | STACK 0x%08x%08x [0x%08x:0x%08x] (%s)\n",
        GET32H(linear_sp), GET32L(linear_sp),
        (unsigned) conv_4xBit8u_to_Bit32u(buf+4),
        (unsigned) conv_4xBit8u_to_Bit32u(buf),
        Sym ? Sym : "<unknown>");
    }
    else
#endif
    {
      if (len == 4) {
        Bit32u addr_on_stack = conv_4xBit8u_to_Bit32u(buf);
        const char *Sym=bx_dbg_disasm_symbolic_address(addr_on_stack, 0);
        dbg_printf(" | STACK 0x%08x [0x%08x] (%s)\n",
          (unsigned) linear_sp, (unsigned) conv_4xBit8u_to_Bit32u(buf),
          Sym ? Sym : "<unknown>");
      }
      else {
        Bit32u addr_on_stack = conv_2xBit8u_to_Bit16u(buf);
        const char *Sym=bx_dbg_disasm_symbolic_address(addr_on_stack, 0);
        dbg_printf(" | STACK 0x%04x [0x%04x] (%s)\n",
          (unsigned) linear_sp, (unsigned) conv_2xBit8u_to_Bit16u(buf),
          Sym ? Sym : "<unknown>");
      }
    }

    linear_sp += len;
  }
}

void bx_dbg_bt_command(unsigned dist)
{
  bx_address linear_bp;
  bx_address linear_sp;
  unsigned len;

#if BX_SUPPORT_X86_64
  if (BX_CPU(dbg_cpu)->get_cpu_mode() == BX_MODE_LONG_64) {
    linear_bp = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_RBP);
    linear_sp = BX_CPU(dbg_cpu)->get_reg64(BX_64BIT_REG_RSP);
    bx_address rip = BX_CPU(dbg_cpu)->get_instruction_pointer();
    const char *Sym=bx_dbg_disasm_symbolic_address(rip, 0);
    dbg_printf("%012" FMT_64 "x -> %012" FMT_64 "x (%s)\n",
      (Bit64u)linear_sp, (Bit64u)rip, Sym);
    len = 8;
  } else
#endif
  {
    if (BX_CPU(dbg_cpu)->sregs[BX_SEG_REG_SS].cache.u.segment.d_b) {
      linear_bp = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_EBP);
      linear_sp = BX_CPU(dbg_cpu)->get_reg32(BX_32BIT_REG_ESP);
      bx_address eip = BX_CPU(dbg_cpu)->get_instruction_pointer();
      const char *Sym=bx_dbg_disasm_symbolic_address(eip, 0);
      dbg_printf("%08x -> %08x (%s)\n",
        (Bit32u)linear_sp, (Bit32u)eip, Sym);
      len = 4;
    } else {
      linear_bp = BX_CPU(dbg_cpu)->get_reg16(BX_16BIT_REG_BP);
      linear_sp = BX_CPU(dbg_cpu)->get_reg16(BX_16BIT_REG_BP);
      bx_address ip = BX_CPU(dbg_cpu)->get_instruction_pointer();
      const char *Sym=bx_dbg_disasm_symbolic_address(ip, 0);
      dbg_printf("%04x -> %04x (%s)\n",
        (Bit16u)linear_sp, (Bit16u)ip, Sym);
      len = 2;
    }
  }

  Bit8u buf[8];
  Bit64u addr_on_stack;
  bx_address addr;

  for (unsigned i = 0; i < dist; ++i) {
    // Read return address right above frame pointer
    addr = BX_CPU(dbg_cpu)->get_laddr(BX_SEG_REG_SS, linear_bp);
    if (!bx_dbg_read_linear(dbg_cpu, addr + len, len, buf))
      break;
#if BX_SUPPORT_X86_64
    if (len == 8) {
      addr_on_stack = conv_8xBit8u_to_Bit64u(buf);
      const char *Sym=bx_dbg_disasm_symbolic_address(addr_on_stack, 0);
      dbg_printf("%012" FMT_64 "x -> %012" FMT_64 "x (%s)\n",
                 (Bit64u)addr,
                 (Bit64u)addr_on_stack,
                 Sym ? Sym : "<unknown>");

      // Get next frame pointer
      if (!bx_dbg_read_linear(dbg_cpu, addr, len, buf))
        break;
      linear_bp = conv_8xBit8u_to_Bit64u(buf);
    }
    else
#endif
    {
      if (len == 4) {
        addr_on_stack = conv_4xBit8u_to_Bit32u(buf);
        const char *Sym=bx_dbg_disasm_symbolic_address(addr_on_stack, 0);
        dbg_printf(FMT_ADDRX32 " -> " FMT_ADDRX32 " (%s)\n",
                   (Bit32u)addr,
                   (Bit32u)addr_on_stack,
                   Sym ? Sym : "<unknown>");

        // Get next frame pointer
        if (!bx_dbg_read_linear(dbg_cpu, addr, len, buf))
          break;
        linear_bp = conv_4xBit8u_to_Bit32u(buf);
      } else {
        addr_on_stack = conv_2xBit8u_to_Bit16u(buf);
        const char *Sym=bx_dbg_disasm_symbolic_address(addr_on_stack, 0);
        dbg_printf(FMT_ADDRX16 " -> " FMT_ADDRX16  " (%s)\n",
                   (Bit16u)addr,
                   (Bit16u)addr_on_stack,
                   Sym ? Sym : "<unknown>");

        // Get next frame pointer
        if (!bx_dbg_read_linear(dbg_cpu, addr, len, buf))
          break;
        linear_bp = conv_2xBit8u_to_Bit16u(buf);
      }
    }
  }
}

void bx_dbg_print_watchpoints(void)
{
  Bit8u buf[2];
  unsigned i;

  // print watch point info
  for (i = 0; i < num_read_watchpoints; i++) {
    if (BX_MEM(0)->dbg_fetch_mem(BX_CPU(dbg_cpu), read_watchpoint[i].addr, 2, buf))
      dbg_printf("rd 0x" FMT_PHY_ADDRX " len=%d\t\t(%04x)\n",
          read_watchpoint[i].addr, read_watchpoint[i].len, (int)buf[0] | ((int)buf[1] << 8));
    else
      dbg_printf("rd 0x" FMT_PHY_ADDRX " len=%d\t\t(read error)\n",
          read_watchpoint[i].addr, read_watchpoint[i].len);
  }
  for (i = 0; i < num_write_watchpoints; i++) {
    if (BX_MEM(0)->dbg_fetch_mem(BX_CPU(dbg_cpu), write_watchpoint[i].addr, 2, buf))
      dbg_printf("wr 0x" FMT_PHY_ADDRX " len=%d\t\t(%04x)\n",
          write_watchpoint[i].addr, write_watchpoint[i].len, (int)buf[0] | ((int)buf[1] << 8));
    else
      dbg_printf("rd 0x" FMT_PHY_ADDRX " len=%d\t\t(read error)\n",
          write_watchpoint[i].addr, write_watchpoint[i].len);
  }
}

void bx_dbg_watch(int type, bx_phy_address address, Bit32u len)
{
  if (type == BX_READ) {
    if (num_read_watchpoints == BX_DBG_MAX_WATCHPONTS) {
      dbg_printf("Too many read watchpoints (%d)\n", BX_DBG_MAX_WATCHPONTS);
      return;
    }
    read_watchpoint[num_read_watchpoints].addr = address;
    read_watchpoint[num_read_watchpoints].len = len;
    num_read_watchpoints++;
    dbg_printf("read watchpoint at 0x" FMT_PHY_ADDRX " len=%d inserted\n", address, len);
  }
  else if (type == BX_WRITE) {
    if (num_write_watchpoints == BX_DBG_MAX_WATCHPONTS) {
      dbg_printf("Too many write watchpoints (%d)\n", BX_DBG_MAX_WATCHPONTS);
      return;
    }
    write_watchpoint[num_write_watchpoints].addr = address;
    write_watchpoint[num_write_watchpoints].len = len;
    num_write_watchpoints++;
    dbg_printf("write watchpoint at 0x" FMT_PHY_ADDRX " len=%d inserted\n", address, len);
  }
  else {
    dbg_printf("bx_dbg_watch: broken watchpoint type");
  }
}

void bx_dbg_unwatch_all()
{
  num_read_watchpoints = num_write_watchpoints = 0;
  dbg_printf("All watchpoints removed\n");
}

void bx_dbg_unwatch(bx_phy_address address)
{
  unsigned i;
  for (i=0; i<num_read_watchpoints; i++) {
    if (read_watchpoint[i].addr == address) {
      dbg_printf("read watchpoint at 0x" FMT_PHY_ADDRX " removed\n", address);
      // found watchpoint, delete it by shifting remaining entries left
      for (int j=i; j<(int)(num_read_watchpoints-1); j++) {
        read_watchpoint[j] = read_watchpoint[j+1];
      }
      num_read_watchpoints--;
      break;
    }
  }

  for (i=0; i<num_write_watchpoints; i++) {
    if (write_watchpoint[i].addr == address) {
      dbg_printf("write watchpoint at 0x" FMT_PHY_ADDRX " removed\n", address);
      // found watchpoint, delete it by shifting remaining entries left
      for (int j=i; j<(int)(num_write_watchpoints-1); j++) {
        write_watchpoint[j] = write_watchpoint[j+1];
      }
      num_write_watchpoints--;
      break;
    }
  }
}

void bx_dbg_continue_command(bool expression)
{
  if (! expression) {
    dbg_printf("continue condition is FALSE\n");
    return;
  }

  // continue executing, until a guard found

one_more:

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

#define BX_DBG_DEFAULT_ICOUNT_QUANTUM 5

    if (BX_SMP_PROCESSORS == 1) {
      bx_dbg_set_icount_guard(0, 0); // run to next breakpoint
      BX_CPU(0)->cpu_loop();
      // set stop flag if a guard found other than icount or halted
      unsigned found = BX_CPU(0)->guard_found.guard_found;
      stop_reason_t reason = (stop_reason_t) BX_CPU(0)->stop_reason;
      if (found || (reason != STOP_NO_REASON && reason != STOP_CPU_HALTED)) {
        stop = 1;
        which = 0;
      }
    }
#if BX_SUPPORT_SMP
    else {
      Bit32u max_executed = 0;
      for (int cpu=0; cpu < BX_SMP_PROCESSORS; cpu++) {
        Bit64u cpu_icount = BX_CPU(cpu)->get_icount();
        bx_dbg_set_icount_guard(cpu, BX_DBG_DEFAULT_ICOUNT_QUANTUM);
        BX_CPU(cpu)->cpu_loop();
        Bit32u executed = BX_CPU(cpu)->get_icount() - cpu_icount;
        if (executed > max_executed) max_executed = executed;
        // set stop flag if a guard found other than icount or halted
        unsigned found = BX_CPU(cpu)->guard_found.guard_found;
        stop_reason_t reason = (stop_reason_t) BX_CPU(cpu)->stop_reason;
        if (found || (reason != STOP_NO_REASON && reason != STOP_CPU_HALTED)) {
          stop = 1;
          which = cpu;
        }
        // even if stop==1, finish cycling through all processors.
        // "which" remembers which cpu set the stop flag.  If multiple
        // cpus set stop, too bad.
      }

      // Potential deadlock if all processors are halted.  Then
      // max_executed will be 0, tick will be incremented by zero, and
      // there will never be a timed event to wake them up.
      // To avoid this, always tick by a minimum of 1.
      if (max_executed < 1) max_executed=1;

      // increment time tick only after all processors have had their chance.
      BX_TICKN(max_executed);
    }
#endif
  }

  sim_running->set(0);
  SIM->refresh_ci();

  // (mch) hack
  if (!bx_user_quit) {
    SIM->refresh_vga();
  }

  BX_INSTR_DEBUG_PROMPT();
  bx_dbg_print_guard_results();

  if (watchpoint_continue && (BX_CPU(which)->stop_reason == STOP_READ_WATCH_POINT ||
            BX_CPU(which)->stop_reason == STOP_WRITE_WATCH_POINT))
  goto one_more;
}

void bx_dbg_stepN_command(int cpu, Bit32u count)
{
  if (cpu != -1 && cpu >= BX_SMP_PROCESSORS) {
    dbg_printf("Error: stepN: unknown cpu=%d\n", cpu);
    return;
  }

  if (count == 0) {
    dbg_printf("Error: stepN: count=0\n");
    return;
  }

  // use simulation mode while executing instructions.  When the prompt
  // is printed, we will return to config mode.
  SIM->set_display_mode(DISP_MODE_SIM);

  if (cpu >= 0 || BX_SMP_PROCESSORS == 1) {
    bx_guard.interrupt_requested = 0;
    bx_dbg_set_icount_guard(cpu, count);
    BX_CPU(cpu)->cpu_loop();
  }
#if BX_SUPPORT_SMP
  else {
    int stop = 0;
    // for now, step each CPU one instruction at a time
    for (unsigned cycle=0; !stop && cycle < count; cycle++) {
      for (unsigned ncpu=0; ncpu < BX_SMP_PROCESSORS; ncpu++) {
        bx_guard.interrupt_requested = 0;
        bx_dbg_set_icount_guard(ncpu, 1);
        BX_CPU(ncpu)->cpu_loop();
        // set stop flag if a guard found other than icount or halted
        unsigned found = BX_CPU(ncpu)->guard_found.guard_found;
        stop_reason_t reason = (stop_reason_t) BX_CPU(ncpu)->stop_reason;
        if (found || (reason != STOP_NO_REASON && reason != STOP_CPU_HALTED))
          stop = 1;
      }

      // when (BX_SMP_PROCESSORS == 1) ticks are handled inside the cpu loop
      BX_TICK1();
    }
  }
#endif

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

  bool phy_valid = BX_CPU(which_cpu)->dbg_xlate_linear2phy(BX_CPU(which_cpu)->guard_found.laddr, &phy);
  if (! phy_valid) {
    dbg_printf("(%u).[" FMT_LL "d] ??? (physical address not available)\n", which_cpu, bx_pc_system.time_ticks());
    return;
  }

  if (bx_dbg_read_linear(which_cpu, BX_CPU(which_cpu)->guard_found.laddr, 16, bx_disasm_ibuf))
  {
    unsigned ilen = bx_dbg_disasm_wrapper(IS_CODE_32(BX_CPU(which_cpu)->guard_found.code_32_64),
      IS_CODE_64(BX_CPU(which_cpu)->guard_found.code_32_64),
      BX_CPU(which_cpu)->get_segment_base(BX_SEG_REG_CS),
      BX_CPU(which_cpu)->guard_found.eip, bx_disasm_ibuf, bx_disasm_tbuf);

    // Note: it would be nice to display only the modified registers here, the easy
    // way out I have thought of would be to keep a prev_eax, prev_ebx, etc copies
    // in each cpu description (see cpu/cpu.h) and update/compare those "prev" values
    // from here. (eks)
    if(BX_CPU(which_cpu)->trace_reg)
      bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);

    if (print_time)
      dbg_printf("(%u).[" FMT_LL "d] ", which_cpu, bx_pc_system.time_ticks());
    else
      dbg_printf("(%u) ", which_cpu);

    if (BX_CPU(which_cpu)->protected_mode()) {
      dbg_printf("[0x" FMT_PHY_ADDRX "] %04x:" FMT_ADDRX " (%s): ",
        phy, BX_CPU(which_cpu)->guard_found.cs,
        BX_CPU(which_cpu)->guard_found.eip,
        bx_dbg_symbolic_address(BX_CPU(which_cpu)->cr3 >> 12,
           BX_CPU(which_cpu)->guard_found.eip,
           BX_CPU(which_cpu)->get_segment_base(BX_SEG_REG_CS)));
    }
    else { // Real & V86 mode
      dbg_printf("[0x" FMT_PHY_ADDRX "] %04x:%04x (%s): ",
        phy, BX_CPU(which_cpu)->guard_found.cs,
        (unsigned) BX_CPU(which_cpu)->guard_found.eip,
        bx_dbg_symbolic_address(0,
           BX_CPU(which_cpu)->guard_found.eip & 0xffff,
           BX_CPU(which_cpu)->get_segment_base(BX_SEG_REG_CS)));
    }
    dbg_printf("%-25s ; ", bx_disasm_tbuf);
    for (unsigned j=0; j<ilen; j++) {
      dbg_printf("%02x", (unsigned) bx_disasm_ibuf[j]);
    }
    dbg_printf("\n");
  }
}

void bx_dbg_print_guard_results(void)
{
  unsigned cpu, i;

  for (cpu=0; cpu<BX_SMP_PROCESSORS; cpu++) {
    unsigned found = BX_CPU(cpu)->guard_found.guard_found;
    if (! found) { /* ... */ }
#if (BX_DBG_MAX_VIR_BPOINTS > 0)
    else if (found & BX_DBG_GUARD_IADDR_VIR) {
      i = BX_CPU(cpu)->guard_found.iaddr_index;
      dbg_printf("(%u) Breakpoint %u, in ", cpu,
            bx_guard.iaddr.vir[i].bpoint_id);
      dbg_print_guard_found(BX_CPU(dbg_cpu)->get_cpu_mode(),
            BX_CPU(cpu)->guard_found.cs, BX_CPU(cpu)->guard_found.eip,
            BX_CPU(cpu)->guard_found.laddr);
      dbg_printf("\n");
    }
#endif
#if (BX_DBG_MAX_LIN_BPOINTS > 0)
    else if (found & BX_DBG_GUARD_IADDR_LIN) {
      i = BX_CPU(cpu)->guard_found.iaddr_index;
      if (bx_guard.iaddr.lin[i].bpoint_id != 0)
        dbg_printf("(%u) Breakpoint %u, 0x" FMT_ADDRX " in ?? ()\n",
            cpu,
            bx_guard.iaddr.lin[i].bpoint_id,
            BX_CPU(cpu)->guard_found.laddr);
    }
#endif
#if (BX_DBG_MAX_PHY_BPOINTS > 0)
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
        dbg_printf("(%u) Caught read watch point at 0x" FMT_PHY_ADDRX "\n", cpu, BX_CPU(cpu)->watchpoint);
        break;
    case STOP_WRITE_WATCH_POINT:
        dbg_printf("(%u) Caught write watch point at 0x" FMT_PHY_ADDRX "\n", cpu, BX_CPU(cpu)->watchpoint);
        break;
    case STOP_MAGIC_BREAK_POINT:
        dbg_printf("(%u) Magic breakpoint\n", cpu);
        break;
    case STOP_MODE_BREAK_POINT:
        dbg_printf("(%u) Caught mode switch breakpoint switching to '%s'\n",
          cpu, cpu_mode_string(BX_CPU(cpu)->get_cpu_mode()));
        break;
    case STOP_VMEXIT_BREAK_POINT:
        dbg_printf("(%u) Caught VMEXIT breakpoint\n", cpu);
        break;
    default:
        dbg_printf("Error: (%u) print_guard_results: guard_found ? (stop reason %u)\n",
          cpu, BX_CPU(cpu)->stop_reason);
    }

    if (bx_debugger.auto_disassemble) {
      if (cpu==0) {
        // print this only once
        dbg_printf("Next at t=" FMT_LL "d\n", bx_pc_system.time_ticks());
      }
      bx_dbg_disassemble_current(cpu, 0);  // one cpu, don't print time
    }
  }
}

void bx_dbg_set_icount_guard(int which_cpu, Bit32u n)
{
  if (n == 0) {
    bx_guard.guard_for &= ~BX_DBG_GUARD_ICOUNT;
  }
  else {
    bx_guard.guard_for |= BX_DBG_GUARD_ICOUNT;
    BX_CPU(which_cpu)->guard_found.icount_max = BX_CPU(which_cpu)->get_icount() + n;
  }

  BX_CPU(which_cpu)->guard_found.guard_found = 0;
}

void bx_dbg_set_auto_disassemble(bool enable)
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
  disasm_syntax_intel = !disasm_syntax_intel;
}

void bx_dbg_take_command(const char *what, unsigned n)
{
  if (! strcmp(what, "smi")) {
    dbg_printf("Delivering SMI to cpu0\n");
    BX_CPU(0)->deliver_SMI();
  }
  else if (! strcmp(what, "nmi")) {
    dbg_printf("Delivering NMI to cpu0\n");
    BX_CPU(0)->deliver_NMI();
  }
  else if (! strcmp(what, "dma")) {
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
  else {
    dbg_printf("Error: Take '%s' not understood.\n", what);
  }
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

void dbg_printf_binary(const char *format, Bit32u data, int bits)
{
  int len = 0;
  char num[33];

  for (unsigned b = 1 << (bits - 1); b; b >>= 1)
    num [len++] = (data & b) ? '1' : '0';
  num[len] = 0;
  dbg_printf(format, num);
}

void bx_dbg_examine_command(const char *command, const char *format, bool format_passed,
               bx_address addr, bool addr_passed)
{
  unsigned repeat_count, i;
  char ch, display_format, unit_size;
  bool iteration, memory_dump = false;
  unsigned data_size;
  Bit8u   data8;
  Bit16u  data16;
  Bit32u  data32;
  Bit64u  data64;
  unsigned columns, per_line, offset;
  bool is_linear;
  Bit8u databuf[8];

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

    while (ch>='0' && ch<='9') {
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

    for (i = 0; format[i]; i++) {
      switch (ch = format[i]) {
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
  }

  if ((display_format == 'i') || (display_format == 's')) {
    dbg_printf("error: dbg_examine: 'i' and 's' formats not supported.\n");
    return;
  }

  if (format_passed) {
    // store current options as default
    bx_debugger.default_display_format = display_format;
    bx_debugger.default_unit_size      = unit_size;
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
    else {
      switch (unit_size) {
        case 'b': data_size = 1; per_line = 16; break;
        case 'h': data_size = 2; per_line = 8; break;
        case 'w': data_size = 4; per_line = 4; break;
        case 'g': data_size = 8; per_line = 2; break;
      }
      // binary format is quite large
      if (display_format == 't')
        per_line /= 4;
    }
  }
  else {
    switch (unit_size) {
      case 'b': data_size = 1; per_line = 8; break;
      case 'h': data_size = 2; per_line = 8; break;
      case 'w': data_size = 4; per_line = 4; break;
      case 'g': data_size = 8; per_line = 2; break;
    }
  }

  columns = per_line + 1; // set current number columns past limit

  for (i=1; i<=repeat_count; i++) {
    if (columns > per_line) {
      // if not 1st run, need a newline from last line
      if (i!=1)
        dbg_printf("\n");
      if (memory_dump)
        dbg_printf("0x" FMT_ADDRX ":", addr);
      else {
        const char *Sym=bx_dbg_disasm_symbolic_address(addr + offset, 0);
        if (Sym)
          dbg_printf("0x" FMT_ADDRX " <%s>:", addr, Sym);
      else
        dbg_printf("0x" FMT_ADDRX " <bogus+%8u>:", addr, offset);
      }
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
      BX_MEM(0)->dbg_fetch_mem(BX_CPU(dbg_cpu), (bx_phy_address) addr, data_size, databuf);
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
	    case 't': dbg_printf_binary("%s ", data8, 8); break;
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
        data16 = ReadHostWordFromLittleEndian((Bit16u*)databuf);

        if (memory_dump)
          switch (display_format) {
	    case 'd': dbg_printf("%05d ", data16); break;
	    case 'u': dbg_printf("%05u ", data16); break;
	    case 'o': dbg_printf("%06o ", data16); break;
	    case 't': dbg_printf_binary("%s ", data16, 16); break;
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
        data32 = ReadHostDWordFromLittleEndian((Bit32u*)databuf);

        if (memory_dump)
          switch (display_format) {
	    case 'd': dbg_printf("%10d ", data32); break;
	    case 'u': dbg_printf("%10u ", data32); break;
	    case 'o': dbg_printf("%12o ", data32); break;
	    case 't': dbg_printf_binary("%s ", data32, 32); break;
	    default : dbg_printf("%08X ", data32); break;
        }
	else
        switch (display_format) {
          case 'x': dbg_printf("\t0x%08x", data32); break;
          case 'd': dbg_printf("\t%d", (Bit32s) data32); break;
          case 'u': dbg_printf("\t%u", data32); break;
          case 'o': dbg_printf("\t%o", data32); break;
          case 't': dbg_printf_binary("\t%s", data32, 32); break;
          case 'c':
            bx_print_char(0xff & (data32>>24));
            bx_print_char(0xff & (data32>>16));
            bx_print_char(0xff & (data32>> 8));
            bx_print_char(0xff & (data32>> 0));
            break;
        }
        break;

      case 8:
        data64 = ReadHostQWordFromLittleEndian((Bit64u*)databuf);

        if (memory_dump)
          switch (display_format) {
            case 'd': dbg_printf("%10" FMT_64 "d ", data64); break;
            case 'u': dbg_printf("%10" FMT_64 "u ", data64); break;
            case 'o': dbg_printf("%12" FMT_64 "o ", data64); break;
            case 't': dbg_printf_binary("%s ", data64, 64); break;
            default : dbg_printf("%08" FMT_64 "X ", data64); break;
          }
        else
          switch (display_format) {
            case 'x': dbg_printf("\t0x%08" FMT_64 "x", data64); break;
            case 'd': dbg_printf("\t%" FMT_64 "d", (Bit64s) data64); break;
            case 'u': dbg_printf("\t%" FMT_64 "u", data64); break;
            case 'o': dbg_printf("\t%" FMT_64 "o", data64); break;
            case 't': dbg_printf_binary("\t%s", data64, 64); break;
            case 'c':
              bx_print_char(0xff & (data64>>56));
              bx_print_char(0xff & (data64>>48));
              bx_print_char(0xff & (data64>>40));
              bx_print_char(0xff & (data64>>32));
              bx_print_char(0xff & (data64>>24));
              bx_print_char(0xff & (data64>>16));
              bx_print_char(0xff & (data64>> 8));
              bx_print_char(0xff & (data64>> 0));
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

Bit32u bx_dbg_lin_indirect(bx_address addr)
{
  Bit8u  databuf[4];

  if (! bx_dbg_read_linear(dbg_cpu, addr, 4, databuf)) {
    /* bx_dbg_read_linear already printed an error message if failed */
    return 0;
  }

  Bit32u result = ReadHostDWordFromLittleEndian((Bit32u*) databuf);
  return result;
}

Bit32u bx_dbg_phy_indirect(bx_phy_address paddr)
{
  Bit8u  databuf[4];

  if (! BX_MEM(0)->dbg_fetch_mem(BX_CPU(dbg_cpu), paddr, 4, databuf)) {
    /* dbg_fetch_mem already printed an error message if failed */
    return 0;
  }

  Bit32u result = ReadHostDWordFromLittleEndian((Bit32u*) databuf);
  return result;
}

void bx_dbg_writemem_command(const char *filename, bx_address laddr, unsigned len)
{
  if (len == 0) {
    dbg_printf("writemem: required length in bytes\n");
    return;
  }

  FILE *f = fopen(filename, "wb");
  if (!f) {
    dbg_printf("Can not open file '%s' for writemem log!\n", filename);
    return;
  }

  Bit8u databuf[4096];

  while(len > 0) {
    unsigned bytes = len;
    if (len > 4096) bytes = 4096;

    // I hope laddr is 4KB aligned so read_linear will be done efficiently
    if (! bx_dbg_read_linear(dbg_cpu, laddr, bytes, databuf)) {
      /* bx_dbg_read_linear already printed an error message if failed */
      len = 0;
      break;
    }

    if (fwrite(databuf, 1, bytes, f) < bytes) {
      dbg_printf("Write error to file '%s'\n", filename);
      len = 0;
      break;
    }

    len -= bytes;
    laddr += bytes;
  }

  fclose(f);
}

void bx_dbg_setpmem_command(bx_phy_address paddr, unsigned len, Bit64u val)
{
  Bit8u buf[8];

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
    case 8:
      buf[0] = val & 0xff; val >>= 8;
      buf[1] = val & 0xff; val >>= 8;
      buf[2] = val & 0xff; val >>= 8;
      buf[3] = val & 0xff; val >>= 8;
      buf[4] = val & 0xff; val >>= 8;
      buf[5] = val & 0xff; val >>= 8;
      buf[6] = val & 0xff; val >>= 8;
      buf[7] = val & 0xff;
      break;
    default:
      dbg_printf("Error: setpmem: bad length value = %u\n", len);
      return;
    }

  if (! BX_MEM(0)->dbg_set_mem(BX_CPU(dbg_cpu), paddr, len, buf)) {
    dbg_printf("Error: setpmem: could not set memory, out of physical bounds?\n");
  }
}

void bx_dbg_set_symbol_command(const char *symbol, bx_address val)
{
  bool is_OK = false;
  symbol++; // get past '$'

  if (!strcmp(symbol, "eip") || !strcmp(symbol, "rip")) {
    bx_dbg_set_rip_value(val);
    return;
  }
  else if (!strcmp(symbol, "eflags")) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_eflags(val);
  }
  else if (!strcmp(symbol, "cpu")) {
    if (val >= BX_SMP_PROCESSORS) {
      dbg_printf("invalid cpu id number %d\n", val);
      return;
    }
    char cpu_param_name[10];
    sprintf(cpu_param_name, "cpu%d", (int)val);
    dbg_cpu_list = (bx_list_c*) SIM->get_param(cpu_param_name, SIM->get_bochs_root());
    dbg_cpu = val;
    return;
  }
  else if (!strcmp(symbol, "synchronous_dma")) {
    bx_guard.async.dma = !val;
    return;
  }
  else if (!strcmp(symbol, "synchronous_irq")) {
    bx_guard.async.irq = !val;
    return;
  }
  else if (!strcmp(symbol, "event_reports")) {
    bx_guard.report.irq   = val;
    bx_guard.report.a20   = val;
    bx_guard.report.io    = val;
    bx_guard.report.dma   = val;
    return;
  }
  else if (!strcmp(symbol, "auto_disassemble")) {
    bx_dbg_set_auto_disassemble(val != 0);
    return;
  }
  else {
    dbg_printf("Error: set: unrecognized symbol.\n");
    return;
  }

  if (!is_OK) {
    dbg_printf("Error: could not set value for '%s'\n", symbol);
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

void bx_dbg_restore_command(const char *param_name, const char *restore_path)
{
  const char *path = (restore_path == NULL) ? "." : restore_path;
  dbg_printf("restoring param (%s) state from file (%s/%s)\n",
      param_name, path, param_name);
  if (! SIM->restore_bochs_param(SIM->get_bochs_root(), path, param_name)) {
    dbg_printf("Error: error occurred during restore\n");
  }
  else {
    bx_sr_after_restore_state();
  }
}

void bx_dbg_disassemble_current(const char *format)
{
  Bit64u addr = BX_CPU(dbg_cpu)->get_laddr(BX_SEG_REG_CS, BX_CPU(dbg_cpu)->get_instruction_pointer());
  bx_dbg_disassemble_command(format, addr, addr);
}

void bx_dbg_disassemble_command(const char *format, Bit64u from, Bit64u to)
{
  int numlines = INT_MAX;

  if (from > to) {
     Bit64u temp = from;
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

    unsigned ilen = bx_dbg_disasm_wrapper(dis_size==32, dis_size==64,
       0/*(bx_address)(-1)*/, from/*(bx_address)(-1)*/, bx_disasm_ibuf, bx_disasm_tbuf);

    const char *Sym=bx_dbg_disasm_symbolic_address(from, 0);

    dbg_printf(FMT_ADDRX ": ", from);
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
  dbg_printf("Command '%s' passed to instrumentation module\n", comm);
  BX_INSTR_DEBUG_CMD(comm);
#else
  UNUSED(comm);
  dbg_printf("Error: instrumentation not enabled.\n");
#endif
}

void bx_dbg_doit_command(unsigned n)
{
  // generic command to add temporary hacks to
  // for debugging purposes
  bx_dbg.interrupts = n;
  bx_dbg.exceptions = n;
}

void bx_dbg_crc_command(bx_phy_address addr1, bx_phy_address addr2)
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

void bx_dbg_print_descriptor(Bit32u lo, Bit32u hi)
{
  Bit32u base = ((lo >> 16) & 0xffff)
             | ((hi << 16) & 0xff0000)
             | (hi & 0xff000000);
  Bit32u limit = (hi & 0x000f0000) | (lo & 0xffff);
  Bit32u segment = (lo >> 16) & 0xffff;
  Bit32u offset = (lo & 0xffff) | (hi & 0xffff0000);
  unsigned type = (hi >> 8) & 0xf;
  unsigned dpl = (hi >> 13) & 0x3;
  unsigned s = (hi >> 12) & 0x1;
  unsigned d_b = (hi >> 22) & 0x1;
  unsigned g = (hi >> 23) & 0x1;
#if BX_SUPPORT_X86_64
  unsigned l = (hi >> 21) & 0x1;
#endif

  // 32-bit trap gate, target=0010:c0108ec4, DPL=0, present=1
  // code segment, base=0000:00cfffff, length=0xffff
  if (s) {
    // either a code or a data segment. bit 11 (type file MSB) then says
    // 0=data segment, 1=code seg
    if (IS_CODE_SEGMENT(type)) {
      dbg_printf("Code segment, base=0x%08x, limit=0x%08x, %s, %s%s",
        base, g ? (limit * 4096 + 4095) : limit,
        IS_CODE_SEGMENT_READABLE(type) ? "Execute/Read" : "Execute-Only",
        IS_CODE_SEGMENT_CONFORMING(type)? "Conforming" : "Non-Conforming",
        IS_SEGMENT_ACCESSED(type)? ", Accessed" : "");
#if BX_SUPPORT_X86_64
      if (l && !d_b)
        dbg_printf(", 64-bit\n");
      else
#endif
        dbg_printf(", %d-bit\n", d_b ? 32 : 16);
    } else {
      dbg_printf("Data segment, base=0x%08x, limit=0x%08x, %s%s%s\n",
        base, g ? (limit * 4096 + 4095) : limit,
        IS_DATA_SEGMENT_WRITEABLE(type)? "Read/Write" : "Read-Only",
        IS_DATA_SEGMENT_EXPAND_DOWN(type)? ", Expand-down" : "",
        IS_SEGMENT_ACCESSED(type)? ", Accessed" : "");
    }
  } else {
    // types from IA32-devel-guide-3, page 3-15.
    static const char *undef = "???";
    static const char *type_names[16] = { 
        undef,
        "16-Bit TSS (available)",
        "LDT",
        "16-Bit TSS (Busy)",
        "16-Bit Call Gate",
        "Task Gate",
        "16-Bit Interrupt Gate",
        "16-Bit Trap Gate",
        undef,
        "32-Bit TSS (Available)",
        undef,
        "32-Bit TSS (Busy)",
        "32-Bit Call Gate",
        undef,
        "32-Bit Interrupt Gate",
        "32-Bit Trap Gate"
    };
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
        break;
      }
    }
    dbg_printf("\n");
  }
}

#if BX_SUPPORT_X86_64
void bx_dbg_print_descriptor64(Bit32u lo1, Bit32u hi1, Bit32u lo2, Bit32u hi2)
{
  Bit32u segment = (lo1 >> 16) & 0xffff;
  Bit64u offset = (lo1 & 0xffff) | (hi1 & 0xffff0000) | ((Bit64u)(lo2) << 32);
  unsigned type = (hi1 >> 8) & 0xf;
  unsigned dpl = (hi1 >> 13) & 0x3;
  unsigned s = (hi1 >> 12) & 0x1;

  if (s) {
    dbg_printf("bx_dbg_print_descriptor64: only system entries displayed in 64bit mode\n");
  }
  else {
    static const char *undef = "???";
    static const char *type_names[16] = { 
        undef,
        undef,
        "LDT",
        undef,
        undef,
        undef,
        undef,
        undef,
        undef,
        "64-Bit TSS (Available)",
        undef,
        "64-Bit TSS (Busy)",
        "64-Bit Call Gate",
        undef,
        "64-Bit Interrupt Gate",
        "64-Bit Trap Gate"
    };
    dbg_printf("%s ", type_names[type]);
    // only print more if type is valid
    if (type_names[type] == undef)  {
      dbg_printf("\ndescriptor dword2 hi=0x%08x, lo=0x%08x", hi2, lo2);
      dbg_printf("\n           dword1 hi=0x%08x, lo=0x%08x", hi1, lo1);
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
        // don't print nothing about 64-bit TSS
        break;
      case BX_SYS_SEGMENT_LDT:
        // it's an LDT. not much to print.
        break;
      default:
        // task, int, trap, or call gate.
        dbg_printf("target=0x%04x:" FMT_ADDRX ", DPL=%d", segment, offset, dpl);

        const char *Sym = bx_dbg_disasm_symbolic_address(offset, 0);
        if (Sym)
          dbg_printf(" (%s)", Sym);

        break;
      }
    }
    dbg_printf("\n");
  }
}
#endif

void bx_dbg_info_idt_command(unsigned from, unsigned to)
{
  bx_dbg_global_sreg_t idtr;
  BX_CPU(dbg_cpu)->dbg_get_idtr(&idtr);
  bool all = 0;

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

#if BX_SUPPORT_X86_64
  if (BX_CPU(dbg_cpu)->long_mode()) {
    dbg_printf("Interrupt Descriptor Table (base=0x" FMT_ADDRX ", limit=%d):\n", idtr.base, idtr.limit);
    for (unsigned n = from; n<=to; n++) {
      Bit8u entry[16];
      if (16*n + 15 > idtr.limit) break;
      if (bx_dbg_read_linear(dbg_cpu, idtr.base + 16*n, 16, entry)) {
        dbg_printf("IDT[0x%02x]=", n);

        Bit32u lo1 = (entry[3]  << 24) | (entry[2]  << 16) | (entry[1]  << 8) | (entry[0]);
        Bit32u hi1 = (entry[7]  << 24) | (entry[6]  << 16) | (entry[5]  << 8) | (entry[4]);
        Bit32u lo2 = (entry[11] << 24) | (entry[10] << 16) | (entry[9]  << 8) | (entry[8]);
        Bit32u hi2 = (entry[15] << 24) | (entry[14] << 16) | (entry[13] << 8) | (entry[12]);

        bx_dbg_print_descriptor64(lo1, hi1, lo2, hi2);
      }
      else {
        dbg_printf("error: IDTR+16*%d points to invalid linear address 0x" FMT_ADDRX "\n", n, idtr.base);
      }
    }
  }
  else
#endif
  {
    dbg_printf("Interrupt Descriptor Table (base=0x" FMT_ADDRX ", limit=%d):\n", idtr.base, idtr.limit);
    for (unsigned n = from; n<=to; n++) {
      Bit8u entry[8];
      if (8*n + 7 > idtr.limit) break;
      if (bx_dbg_read_linear(dbg_cpu, idtr.base + 8*n, 8, entry)) {
        dbg_printf("IDT[0x%02x]=", n);

        Bit32u lo = (entry[3]  << 24) | (entry[2]  << 16) | (entry[1]  << 8) | (entry[0]);
        Bit32u hi = (entry[7]  << 24) | (entry[6]  << 16) | (entry[5]  << 8) | (entry[4]);

        bx_dbg_print_descriptor(lo, hi);
      }
      else {
        dbg_printf("error: IDTR+8*%d points to invalid linear address 0x" FMT_ADDRX "\n", n, idtr.base);
      }
    }
  }

  if (all)
    dbg_printf("You can list individual entries with 'info idt [NUM]' or groups with 'info idt [NUM] [NUM]'\n");
}

void bx_dbg_info_lgdt_command(unsigned from, unsigned to, bool gdt)
{
  bool all = 0;
  const char *name = gdt ? "gdt" : "ldt";
  bx_address base = 0;
  Bit32u limit = 0;

  if (gdt) {
      bx_dbg_global_sreg_t gdtr;
      BX_CPU(dbg_cpu)->dbg_get_gdtr(&gdtr);
      base = gdtr.base;
      limit = gdtr.limit;
  }
  else { // LDT
      base = SIM->get_param_num("LDTR.base", dbg_cpu_list)->get64();
      limit = SIM->get_param_num("LDTR.limit_scaled", dbg_cpu_list)->get();
  }

  if (to == (unsigned) EMPTY_ARG) {
    to = from;
    if(from == (unsigned) EMPTY_ARG) { from = 0; to = 0xffff; all = 1; }
  }
  if (from > 0xffff || to > 0xffff) {
    dbg_printf("%s entry should be [0-65535], 'info %s' command malformed\n", name, name);
    return;
  }
  if (from > to) {
    unsigned temp = from;
    from = to;
    to = temp;
  }

  dbg_printf("%s (base=0x" FMT_ADDRX ", limit=%d):\n", name, base, limit);
  for (unsigned n = from; n<=to; n++) {
    Bit8u entry[16];
    if (8*n + 7 > limit) break;
    if (bx_dbg_read_linear(dbg_cpu, base + 8*n, 8, entry)) {
      if (gdt) {
        dbg_printf("%s[0x%04x]=", name, n << 3);
      }
      else { // LDT
        dbg_printf("%s[0x%02x]=", name, n);
      }

      Bit32u lo = (entry[3]  << 24) | (entry[2]  << 16) | (entry[1]  << 8) | (entry[0]);
      Bit32u hi = (entry[7]  << 24) | (entry[6]  << 16) | (entry[5]  << 8) | (entry[4]);

      if (0 == lo && 0 == hi) {
        dbg_printf("<null entry>\n");
        continue;
      }

#if BX_SUPPORT_X86_64
      if (BX_CPU(dbg_cpu)->long_mode()) {
        const Bit32u sys_descriptor = (1 << 12);
        if ((hi & sys_descriptor) == 0) {
          n++;
          if (8*n + 7 > limit) break;
          if (bx_dbg_read_linear(dbg_cpu, base + 8*n, 8, entry + 8)) {
            Bit32u lo2 = (entry[11] << 24) | (entry[10] << 16) | (entry[9] << 8) | (entry[8]);
            Bit32u hi2 = (entry[15] << 24) | (entry[14] << 16) | (entry[13] << 8) | (entry[12]);

            bx_dbg_print_descriptor64(lo, hi, lo2, hi2);
          }
          else {
            dbg_printf("error: %sr+8*%d points to incomplete (size must be 16) descriptor, linear address 0x" FMT_ADDRX "\n", name, n, base);
          }
          continue;
        }
        else {
          bx_dbg_print_descriptor(lo, hi);
          continue;
        }
      }
      else
#endif
      {
        bx_dbg_print_descriptor(lo, hi);
      }
    }
    else {
      dbg_printf("error: %sr+8*%d points to invalid linear address 0x" FMT_ADDRX "\n", name, n, base);
    }
  }

  if (all)
    dbg_printf("You can list individual entries with 'info %s [NUM]' or groups with 'info %s [NUM] [NUM]'\n", name, name);
}

void bx_dbg_info_gdt_command(unsigned from, unsigned to)
{
  bx_dbg_info_lgdt_command(from, to, true);
}

void bx_dbg_info_ldt_command(unsigned from, unsigned to)
{

  bx_dbg_info_lgdt_command(from, to, false);
}

/*form RB list*/
static const char* bx_dbg_ivt_desc(int intnum)
{
  const char* ret;
  switch (intnum) {
    case 0x00: ret = "DIVIDE ERROR"                        ; break;
    case 0x01: ret = "SINGLE STEP"                         ; break;
    case 0x02: ret = "NON-MASKABLE INTERRUPT"              ; break;
    case 0x03: ret = "BREAKPOINT"                          ; break;
    case 0x04: ret = "INT0 DETECTED OVERFLOW"              ; break;
    case 0x05: ret = "BOUND RANGE EXCEED"                  ; break;
    case 0x06: ret = "INVALID OPCODE"                      ; break;
    case 0x07: ret = "PROCESSOR EXTENSION NOT AVAILABLE"   ; break;
    case 0x08: ret = "IRQ0 - SYSTEM TIMER"                 ; break;
    case 0x09: ret = "IRQ1 - KEYBOARD DATA READY"          ; break;
    case 0x0a: ret = "IRQ2 - LPT2"                         ; break;
    case 0x0b: ret = "IRQ3 - COM2"                         ; break;
    case 0x0c: ret = "IRQ4 - COM1"                         ; break;
    case 0x0d: ret = "IRQ5 - FIXED DISK"                   ; break;
    case 0x0e: ret = "IRQ6 - DISKETTE CONTROLLER"          ; break;
    case 0x0f: ret = "IRQ7 - PARALLEL PRINTER"             ; break;
    case 0x10: ret = "VIDEO"                               ; break;
    case 0x11: ret = "GET EQUIPMENT LIST"                  ; break;
    case 0x12: ret = "GET MEMORY SIZE"                     ; break;
    case 0x13: ret = "DISK"                                ; break;
    case 0x14: ret = "SERIAL"                              ; break;
    case 0x15: ret = "SYSTEM"                              ; break;
    case 0x16: ret = "KEYBOARD"                            ; break;
    case 0x17: ret = "PRINTER"                             ; break;
    case 0x18: ret = "CASETTE BASIC"                       ; break;
    case 0x19: ret = "BOOTSTRAP LOADER"                    ; break;
    case 0x1a: ret = "TIME"                                ; break;
    case 0x1b: ret = "KEYBOARD - CONTROL-BREAK HANDLER"    ; break;
    case 0x1c: ret = "TIME - SYSTEM TIMER TICK"            ; break;
    case 0x1d: ret = "SYSTEM DATA - VIDEO PARAMETER TABLES"; break;
    case 0x1e: ret = "SYSTEM DATA - DISKETTE PARAMETERS"   ; break;
    case 0x1f: ret = "SYSTEM DATA - 8x8 GRAPHICS FONT"     ; break;
    case 0x70: ret = "IRQ8 - CMOS REAL-TIME CLOCK"         ; break;
    case 0x71: ret = "IRQ9 - REDIRECTED TO INT 0A BY BIOS" ; break;
    case 0x72: ret = "IRQ10 - RESERVED"                    ; break;
    case 0x73: ret = "IRQ11 - RESERVED"                    ; break;
    case 0x74: ret = "IRQ12 - POINTING DEVICE"             ; break;
    case 0x75: ret = "IRQ13 - MATH COPROCESSOR EXCEPTION"  ; break;
    case 0x76: ret = "IRQ14 - HARD DISK CONTROLLER OPERATION COMPLETE"; break;
    case 0x77: ret = "IRQ15 - SECONDARY IDE CONTROLLER OPERATION COMPLETE"; break;
    default  : ret = ""                                    ; break;
  }
  return ret;
}

void bx_dbg_info_ivt_command(unsigned from, unsigned to)
{
  unsigned char buff[4];
  unsigned seg, off;
  bool all = 0;
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
      bx_dbg_read_linear(dbg_cpu, idtr.base + i*4, 4, buff);
      seg = ((Bit32u) buff[3] << 8) | buff[2];
      off = ((Bit32u) buff[1] << 8) | buff[0];
      bx_dbg_read_linear(dbg_cpu, (seg << 4) + off, 1, buff);
      dbg_printf("INT# %02x > %04X:%04X (0x%08x) %s%s\n", i, seg, off,
         (unsigned) ((seg << 4) + off), bx_dbg_ivt_desc(i),
         (buff[0] == 0xcf) ? " ; dummy iret" : "");
    }
    if (all) dbg_printf("You can list individual entries with 'info ivt [NUM]' or groups with 'info ivt [NUM] [NUM]'\n");
  }
  else
    dbg_printf("cpu in protected mode, use info idt\n");
}

static void bx_dbg_print_tss(Bit8u *tss, int len)
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

  bx_address base = (tr.des_l>>16) |
                   ((tr.des_h<<16)&0x00ff0000) | (tr.des_h & 0xff000000);
#if BX_SUPPORT_X86_64
  base |= (Bit64u)(tr.dword3) << 32;
#endif
  Bit32u len = (tr.des_l & 0xffff) + 1;

  dbg_printf("tr:s=0x%x, base=0x" FMT_ADDRX ", valid=%u\n",
      (unsigned) tr.sel, base, (unsigned) tr.valid);

  bx_phy_address paddr = 0;
  if (BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(base, &paddr)) {
    bx_dbg_print_tss(BX_MEM(0)->get_vector(paddr), len);
  }
  else {
    dbg_printf("bx_dbg_info_tss_command: failed to get physical address for TSS.BASE !");
  }
}

/*
 * this implements the info device command in the debugger.
 * info device - list devices supported by this command
 * info device [string] - shows the state of device specified in string
 * info device [string] [string] - shows the state of device with options
 */
void bx_dbg_info_device(const char *dev, const char *args)
{
  debug_info_t *temp = NULL;
  unsigned i, string_i;
  int argc = 0;
  char *argv[16];
  char *ptr;
  char string[512];
  size_t len;

  if (strlen(dev) == 0) {
    if (bx_debug_info_list == NULL) {
      dbg_printf("info device list: no device registered\n");
    } else {
      dbg_printf("devices supported by 'info device':\n\n");
      temp = bx_debug_info_list;
      while (temp) {
        dbg_printf("%s\n", temp->name);
        temp = temp->next;
      }
      dbg_printf("\n");
    }
  } else {
    if (bx_dbg_info_find_device(dev, &temp)) {
      if (temp->device != NULL) {
        len = strlen(args);
        memset(argv, 0, sizeof(argv));
        if (len > 0) {
          char *options = new char[len + 1];
          strcpy(options, args);
          ptr = strtok(options, ",");
          while (ptr) {
            string_i = 0;
            for (i=0; i<strlen(ptr); i++) {
              if (!isspace(ptr[i])) string[string_i++] = ptr[i];
            }
            string[string_i] = '\0';
            if (argc < 16) {
              argv[argc++] = strdup(string);
            } else {
              BX_PANIC (("too many parameters, max is 16\n"));
              break;
            }
            ptr = strtok(NULL, ",");
          }
          delete [] options;
        }
        temp->device->debug_dump(argc, argv);
        for (i = 0; i < (unsigned)argc; i++) {
          if (argv[i] != NULL) {
            free(argv[i]);
            argv[i] = NULL;
          }
        }
      } else {
        BX_PANIC(("info device: device pointer is NULL"));
      }
    } else {
      dbg_printf("info device: '%s' not found\n", dev);
    }
  }
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

void bx_dbg_io_report(Bit32u port, unsigned size, unsigned op, Bit32u val)
{
  if (bx_guard.report.io) {
    dbg_printf("event at t=" FMT_LL "d IO addr=0x%x size=%u op=%s val=0x%x\n",
      bx_pc_system.time_ticks(),
      port,
      size,
      (op==BX_READ) ? "read" : "write",
      (unsigned) val);
  }
}

void bx_dbg_dma_report(bx_phy_address addr, unsigned len, unsigned what, Bit32u val)
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
  Bit64u lin, start_lin; // show only low 32 bit
  bx_phy_address phy, start_phy; // start of a valid translation interval
  bx_address lpf_mask = 0;
  bool valid;

  if (! BX_CPU(dbg_cpu)->cr0.get_PG()) {
    dbg_printf("paging off\n");
    return;
  }

  dbg_printf("cr3: 0x" FMT_PHY_ADDRX "\n", (bx_phy_address)BX_CPU(dbg_cpu)->cr3);

  lin = 0;
  phy = 0;

  start_lin = 1;
  start_phy = 2;
  while(1) {
    valid = BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(lin, &phy, &lpf_mask);
    if(valid) {
      if((lin - start_lin) != (phy - start_phy)) {
        if(start_lin != 1)
          dbg_printf("0x" FMT_ADDRX "-0x" FMT_ADDRX " -> 0x" FMT_PHY_ADDRX "-0x" FMT_PHY_ADDRX "\n",
            start_lin, lin - 1, start_phy, start_phy + (lin-1-start_lin));
        start_lin = lin;
        start_phy = phy;
      }
    } else {
      if(start_lin != 1)
        dbg_printf("0x" FMT_ADDRX "-0x" FMT_ADDRX " -> 0x" FMT_PHY_ADDRX "-0x" FMT_PHY_ADDRX "\n",
          start_lin, lin - 1, start_phy, start_phy + (lin-1-start_lin));
      start_lin = 1;
      start_phy = 2;
    }

    lin += lpf_mask;
    if (!BX_CPU(dbg_cpu)->long64_mode() && lin >= BX_CONST64(0xfffff000)) break;
    if (lin >= BX_CONST64(0x00007ffffffff000)) {
      if (lin < BX_CONST64(0xfffff00000000000))
        lin = BX_CONST64(0xfffff00000000000) - 1;
      if (lin >= BX_CONST64(0xfffffffffffff000))
        break;
    }
    lin++;
  }
  if(start_lin != 1)
    dbg_printf("0x" FMT_ADDRX "-0x" FMT_ADDRX " -> 0x" FMT_PHY_ADDRX "-0x" FMT_PHY_ADDRX "\n",
         start_lin, lin, start_phy, start_phy + (lin-start_lin));
}

void bx_dbg_print_help(void)
{
  dbg_printf("h|help - show list of debugger commands\n");
  dbg_printf("h|help command - show short command description\n");
  dbg_printf("-*- Debugger control -*-\n");
  dbg_printf("    help, q|quit|exit, set, instrument, show, trace, trace-reg,\n");
  dbg_printf("    trace-mem, u|disasm, ldsym, slist\n");
  dbg_printf("-*- Execution control -*-\n");
  dbg_printf("    c|cont|continue, s|step, p|n|next, modebp, vmexitbp\n");
  dbg_printf("-*- Breakpoint management -*-\n");
  dbg_printf("    vb|vbreak, lb|lbreak, pb|pbreak|b|break, sb, sba, blist,\n");
  dbg_printf("    bpe, bpd, d|del|delete, watch, unwatch\n");
  dbg_printf("-*- CPU and memory contents -*-\n");
  dbg_printf("    x, xp, setpmem, writemem, crc, info,\n");
  dbg_printf("    r|reg|regs|registers, fp|fpu, mmx, sse, sreg, dreg, creg,\n");
  dbg_printf("    page, set, ptime, print-stack, bt, ?|calc\n");
  dbg_printf("-*- Working with bochs param tree -*-\n");
  dbg_printf("    show \"param\", restore\n");
}

extern Bit64u eval_value;

void bx_dbg_calc_command(Bit64u value)
{
  assert(value == eval_value);
  dbg_printf("0x" FMT_LL "x " FMT_LL "d\n", eval_value, eval_value);
}

bool bx_dbg_eval_condition(char *condition)
{
  extern Bit64u eval_value;
  bx_dbg_interpret_line(condition);
  return eval_value != 0;
}

bx_address bx_dbg_get_ssp(void)
{
#if BX_SUPPORT_CET
  return BX_CPU(dbg_cpu)->get_ssp();
#else
  return 0;
#endif
}

Bit8u bx_dbg_get_reg8l_value(unsigned reg)
{
  if (reg < 8 || (BX_CPU(dbg_cpu)->long64_mode() && reg < BX_GENERAL_REGISTERS))
    return BX_CPU(dbg_cpu)->get_reg8l(reg);

  dbg_printf("Unknown 8BL register [%d] !!!\n", reg);
  return 0;
}

Bit8u bx_dbg_get_reg8h_value(unsigned reg)
{
  if (reg < 8 || (BX_CPU(dbg_cpu)->long64_mode() && reg < BX_GENERAL_REGISTERS))
    return BX_CPU(dbg_cpu)->get_reg8h(reg);

  dbg_printf("Unknown 8BH register [%d] !!!\n", reg);
  return 0;
}

Bit16u bx_dbg_get_reg16_value(unsigned reg)
{
  if (reg < 8 || (BX_CPU(dbg_cpu)->long64_mode() && reg < BX_GENERAL_REGISTERS))
    return BX_CPU(dbg_cpu)->get_reg16(reg);

  dbg_printf("Unknown 16B register [%d] !!!\n", reg);
  return 0;
}

Bit32u bx_dbg_get_reg32_value(unsigned reg)
{
  if (reg < 8 || (BX_CPU(dbg_cpu)->long64_mode() && reg < BX_GENERAL_REGISTERS))
    return BX_CPU(dbg_cpu)->get_reg32(reg);

  dbg_printf("Unknown 32B register [%d] !!!\n", reg);
  return 0;
}

Bit64u bx_dbg_get_reg64_value(unsigned reg)
{
#if BX_SUPPORT_X86_64
  if (BX_CPU(dbg_cpu)->long64_mode() && reg < BX_GENERAL_REGISTERS)
    return BX_CPU(dbg_cpu)->get_reg64(reg);
#endif

  dbg_printf("Unknown 64B register [%d] !!!\n", reg);
  return 0;
}

void bx_dbg_set_reg8l_value(unsigned reg, Bit8u value)
{
  if (reg < 8 || (BX_CPU(dbg_cpu)->long64_mode() && reg < BX_GENERAL_REGISTERS))
    BX_CPU(dbg_cpu)->set_reg8l(reg, value);
  else
    dbg_printf("Unknown 8BL register [%d] !!!\n", reg);
}

void bx_dbg_set_reg8h_value(unsigned reg, Bit8u value)
{
  if (reg < 8 || (BX_CPU(dbg_cpu)->long64_mode() && reg < BX_GENERAL_REGISTERS))
    BX_CPU(dbg_cpu)->set_reg8h(reg, value);
  else
    dbg_printf("Unknown 8BH register [%d] !!!\n", reg);
}

void bx_dbg_set_reg16_value(unsigned reg, Bit16u value)
{
  if (reg < 8 || (BX_CPU(dbg_cpu)->long64_mode() && reg < BX_GENERAL_REGISTERS))
    BX_CPU(dbg_cpu)->set_reg16(reg, value);
  else
    dbg_printf("Unknown 16B register [%d] !!!\n", reg);
}

void bx_dbg_set_reg32_value(unsigned reg, Bit32u value)
{
  if (reg < 8 || (BX_CPU(dbg_cpu)->long64_mode() && reg < BX_GENERAL_REGISTERS))
    BX_CPU(dbg_cpu)->set_reg32(reg, value);
  else
    dbg_printf("Unknown 32B register [%d] !!!\n", reg);
}

void bx_dbg_set_reg64_value(unsigned reg, Bit64u value)
{
#if BX_SUPPORT_X86_64
  if (BX_CPU(dbg_cpu)->long64_mode() && reg < BX_GENERAL_REGISTERS)
    BX_CPU(dbg_cpu)->set_reg64(reg, value);
  else
#endif
    dbg_printf("Unknown 64B register [%d] !!!\n", reg);
}

void bx_dbg_set_rip_value(bx_address value)
{
#if BX_SUPPORT_X86_64
  if ((value >> 32) != 0 && ! BX_CPU(dbg_cpu)->long64_mode()) {
    dbg_printf("Cannot set EIP to 64-bit value when not in long64 mode !\n");
  }
  else
#endif
    BX_CPU(dbg_cpu)->dbg_set_eip(value);
}

Bit64u bx_dbg_get_opmask_value(unsigned reg)
{
#if BX_SUPPORT_EVEX
  if (reg < 8)
    return BX_CPU(dbg_cpu)->get_opmask(reg);
#endif

  dbg_printf("Unknown OPMASK register k%d !!!\n", reg);
  return 0;
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
  return BX_CPU(dbg_cpu)->get_ip();
}

Bit32u bx_dbg_get_eip(void)
{
  return BX_CPU(dbg_cpu)->get_eip();
}

bx_address bx_dbg_get_rip(void)
{
  return BX_CPU(dbg_cpu)->get_instruction_pointer();
}

bool bx_dbg_read_pmode_descriptor(Bit16u sel, bx_descriptor_t *descriptor)
{
  bx_selector_t selector;
  Bit32u dword1, dword2;
  bx_address desc_base;

  /* if selector is NULL, error */
  if ((sel & 0xfffc) == 0) {
    dbg_printf("bx_dbg_read_pmode_descriptor: Dereferencing a NULL selector!\n");
    return 0;
  }

  /* parse fields in selector */
  parse_selector(sel, &selector);

  if (selector.ti) {
    // LDT
    if (((Bit32u)selector.index*8 + 7) > BX_CPU(dbg_cpu)->ldtr.cache.u.segment.limit_scaled) {
      dbg_printf("bx_dbg_read_pmode_descriptor: selector (0x%04x) > LDT size limit\n", selector.index*8);
      return 0;
    }
    desc_base = BX_CPU(dbg_cpu)->ldtr.cache.u.segment.base;
  }
  else {
    // GDT
    if (((Bit32u)selector.index*8 + 7) > BX_CPU(dbg_cpu)->gdtr.limit) {
      dbg_printf("bx_dbg_read_pmode_descriptor: selector (0x%04x) > GDT size limit\n", selector.index*8);
      return 0;
    }
    desc_base = BX_CPU(dbg_cpu)->gdtr.base;
  }

  if (! bx_dbg_read_linear(dbg_cpu, desc_base + selector.index * 8,     4, (Bit8u*) &dword1)) {
    dbg_printf("bx_dbg_read_pmode_descriptor: cannot read selector 0x%04x (index=0x%04x)\n", sel, selector.index);
    return 0;
  }
  if (! bx_dbg_read_linear(dbg_cpu, desc_base + selector.index * 8 + 4, 4, (Bit8u*) &dword2)) {
    dbg_printf("bx_dbg_read_pmode_descriptor: cannot read selector 0x%04x (index=0x%04x)\n", sel, selector.index);
    return 0;
  }

  memset (descriptor, 0, sizeof (bx_descriptor_t));
  parse_descriptor(dword1, dword2, descriptor);

  if (!descriptor->segment) {
    dbg_printf("bx_dbg_read_pmode_descriptor: selector 0x%04x points to a system descriptor and is not supported!\n", sel);
    return 0;
  }

  /* #NP(selector) if descriptor is not present */
  if (descriptor->p==0) {
    dbg_printf("bx_dbg_read_pmode_descriptor: descriptor 0x%04x not present!\n", sel);
    return 0;
  }

  return 1;
}

void bx_dbg_load_segreg(unsigned seg_no, unsigned value)
{
  bx_segment_reg_t sreg;

  if (seg_no > 6) {
    dbg_printf("bx_dbg_load_segreg: unknown segment register !");
    return;
  }

  if (value > 0xffff) {
    dbg_printf("bx_dbg_load_segreg: segment selector out of limits !");
    return;
  }

  unsigned cpu_mode = BX_CPU(dbg_cpu)->get_cpu_mode();
  if (cpu_mode == BX_MODE_LONG_64) {
    dbg_printf("bx_dbg_load_segreg: not supported in long64 mode !");
    return;
  }

  if (! BX_CPU(dbg_cpu)->protected_mode()) {
    parse_selector(value, &sreg.selector);

    sreg.cache.valid   = SegValidCache;
    sreg.cache.p       = 1;
    sreg.cache.dpl     = (cpu_mode == BX_MODE_IA32_V8086);
    sreg.cache.segment = 1;
    sreg.cache.type    = BX_DATA_READ_WRITE_ACCESSED;

    sreg.cache.u.segment.base = sreg.selector.value << 4;
    sreg.cache.u.segment.limit_scaled = 0xffff;
    sreg.cache.u.segment.g            = 0;
    sreg.cache.u.segment.d_b          = 0;
    sreg.cache.u.segment.avl          = 0;
    sreg.selector.rpl                 = (cpu_mode == BX_MODE_IA32_V8086);

    BX_CPU(dbg_cpu)->dbg_set_sreg(seg_no, &sreg);
  }
  else {
    parse_selector(value, &sreg.selector);
    if (bx_dbg_read_pmode_descriptor(value, &sreg.cache)) {
      BX_CPU(dbg_cpu)->dbg_set_sreg(seg_no, &sreg);
    }
  }
}

bx_address bx_dbg_get_laddr(Bit16u sel, bx_address ofs)
{
  bx_address laddr;

  if (BX_CPU(dbg_cpu)->protected_mode()) {

    bx_descriptor_t descriptor;
    if (! bx_dbg_read_pmode_descriptor(sel, &descriptor))
      return 0;

    if (BX_CPU(dbg_cpu)->get_cpu_mode() != BX_MODE_LONG_64) {
      Bit32u lowaddr, highaddr;

      // expand-down
      if (IS_DATA_SEGMENT(descriptor.type) && IS_DATA_SEGMENT_EXPAND_DOWN(descriptor.type)) {
        lowaddr = descriptor.u.segment.limit_scaled;
        highaddr = descriptor.u.segment.d_b ? 0xffffffff : 0xffff;
      }
       else {
        lowaddr = 0;
        highaddr = descriptor.u.segment.limit_scaled;
      }

      if (ofs < lowaddr || ofs > highaddr) {
        dbg_printf("WARNING: Offset %08X is out of selector %04x limit (%08x...%08x)!\n", ofs, sel, lowaddr, highaddr);
      }
    }

    laddr = descriptor.u.segment.base + ofs;
  }
  else {
    laddr = sel * 16 + ofs;
  }

  return laddr;
}

extern int fetchDecode32(const Bit8u *fetchPtr, bool is_32, bxInstruction_c *i, unsigned remainingInPage);
#if BX_SUPPORT_X86_64
extern int fetchDecode64(const Bit8u *fetchPtr, bxInstruction_c *i, unsigned remainingInPage);
#endif

void bx_dbg_step_over_command()
{
  bx_address laddr = BX_CPU(dbg_cpu)->guard_found.laddr;
  Bit8u opcode_bytes[32];

  if (! bx_dbg_read_linear(dbg_cpu, laddr, 16, opcode_bytes))
  {
    return;
  }

  bxInstruction_c i;
  int ret = -1;
#if BX_SUPPORT_X86_64
  if (IS_CODE_64(BX_CPU(dbg_cpu)->guard_found.code_32_64))
    ret = fetchDecode64(opcode_bytes, &i, 16);
  else
#endif
    ret = fetchDecode32(opcode_bytes, IS_CODE_32(BX_CPU(dbg_cpu)->guard_found.code_32_64), &i, 16);

  if (ret < 0) {
    dbg_printf("bx_dbg_step_over_command:: Failed to fetch instructions !\n");
    return;
  }    

  switch(i.getIaOpcode()) {
    // Jcc short
    case BX_IA_JO_Jbw:      // opcode 0x70
    case BX_IA_JO_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JO_Jbq:
#endif
    case BX_IA_JNO_Jbw:     // opcode 0x71
    case BX_IA_JNO_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNO_Jbq:
#endif
    case BX_IA_JB_Jbw:      // opcode 0x72
    case BX_IA_JB_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JB_Jbq:
#endif
    case BX_IA_JNB_Jbw:     // opcode 0x73
    case BX_IA_JNB_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNB_Jbq:
#endif
    case BX_IA_JZ_Jbw:      // opcode 0x74
    case BX_IA_JZ_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JZ_Jbq:
#endif
    case BX_IA_JNZ_Jbw:     // opcode 0x75
    case BX_IA_JNZ_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNZ_Jbq:
#endif
    case BX_IA_JBE_Jbw:     // opcode 0x76
    case BX_IA_JBE_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JBE_Jbq:
#endif
    case BX_IA_JNBE_Jbw:    // opcode 0x77
    case BX_IA_JNBE_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNBE_Jbq:
#endif
    case BX_IA_JS_Jbw:      // opcode 0x78
    case BX_IA_JS_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JS_Jbq:
#endif
    case BX_IA_JNS_Jbw:     // opcode 0x79
    case BX_IA_JNS_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNS_Jbq:
#endif
    case BX_IA_JP_Jbw:      // opcode 0x7A
    case BX_IA_JP_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JP_Jbq:
#endif
    case BX_IA_JNP_Jbw:     // opcode 0x7B
    case BX_IA_JNP_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNP_Jbq:
#endif
    case BX_IA_JL_Jbw:      // opcode 0x7C
    case BX_IA_JL_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JL_Jbq:
#endif
    case BX_IA_JNL_Jbw:     // opcode 0x7D
    case BX_IA_JNL_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNL_Jbq:
#endif
    case BX_IA_JLE_Jbw:     // opcode 0x7E
    case BX_IA_JLE_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JLE_Jbq:
#endif
    case BX_IA_JNLE_Jbw:    // opcode 0x7F
    case BX_IA_JNLE_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNLE_Jbq:
#endif

    // Jcc near
    case BX_IA_JO_Jw:      // opcode 0x0F 0x80
    case BX_IA_JO_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JO_Jq:
#endif
    case BX_IA_JNO_Jw:     // opcode 0x0F 0x81
    case BX_IA_JNO_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNO_Jq:
#endif
    case BX_IA_JB_Jw:      // opcode 0x0F 0x82
    case BX_IA_JB_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JB_Jq:
#endif
    case BX_IA_JNB_Jw:     // opcode 0x0F 0x83
    case BX_IA_JNB_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNB_Jq:
#endif
    case BX_IA_JZ_Jw:      // opcode 0x0F 0x84
    case BX_IA_JZ_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JZ_Jq:
#endif
    case BX_IA_JNZ_Jw:     // opcode 0x0F 0x85
    case BX_IA_JNZ_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNZ_Jq:
#endif
    case BX_IA_JBE_Jw:     // opcode 0x0F 0x86
    case BX_IA_JBE_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JBE_Jq:
#endif
    case BX_IA_JNBE_Jw:    // opcode 0x0F 0x87
    case BX_IA_JNBE_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNBE_Jq:
#endif
    case BX_IA_JS_Jw:      // opcode 0x0F 0x88
    case BX_IA_JS_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JS_Jq:
#endif
    case BX_IA_JNS_Jw:     // opcode 0x0F 0x89
    case BX_IA_JNS_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNS_Jq:
#endif
    case BX_IA_JP_Jw:      // opcode 0x0F 0x8A
    case BX_IA_JP_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JP_Jq:
#endif
    case BX_IA_JNP_Jw:     // opcode 0x0F 0x8B
    case BX_IA_JNP_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNP_Jq:
#endif
    case BX_IA_JL_Jw:      // opcode 0x0F 0x8C
    case BX_IA_JL_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JL_Jq:
#endif
    case BX_IA_JNL_Jw:     // opcode 0x0F 0x8D
    case BX_IA_JNL_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNL_Jq:
#endif
    case BX_IA_JLE_Jw:     // opcode 0x0F 0x8E
    case BX_IA_JLE_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JLE_Jq:
#endif
    case BX_IA_JNLE_Jw:    // opcode 0x0F 0x8F
    case BX_IA_JNLE_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JNLE_Jq:
#endif

    // jcxz
    case BX_IA_JCXZ_Jbw:   // opcode 0xE3
    case BX_IA_JECXZ_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JRCXZ_Jbq:
#endif

    // retn n 0xC2
    case BX_IA_RET_Op16_Iw:
    case BX_IA_RET_Op32_Iw:
#if BX_SUPPORT_X86_64
    case BX_IA_RET_Op64_Iw:
#endif
    // retn   0xC3
    case BX_IA_RET_Op16:
    case BX_IA_RET_Op32:
#if BX_SUPPORT_X86_64
    case BX_IA_RET_Op64:
#endif
    // retf n 0xCA
    case BX_IA_RETF_Op16_Iw:
    case BX_IA_RETF_Op32_Iw:
#if BX_SUPPORT_X86_64
    case BX_IA_RETF_Op64_Iw:
#endif
    // retf  0xCB
    case BX_IA_RETF_Op16:
    case BX_IA_RETF_Op32:
#if BX_SUPPORT_X86_64
    case BX_IA_RETF_Op64:
#endif
    // iret  0xCF
    case BX_IA_IRET_Op16:
    case BX_IA_IRET_Op32:
#if BX_SUPPORT_X86_64
    case BX_IA_IRET_Op64:
#endif

    // jmp near  0xE9
    case BX_IA_JMP_Jw:
    case BX_IA_JMP_Jd:
#if BX_SUPPORT_X86_64
    case BX_IA_JMP_Jq:
#endif
    // jmp far   0xEA
    case BX_IA_JMPF_Ap:
    // jmp short 0xEB
    case BX_IA_JMP_Jbw:
    case BX_IA_JMP_Jbd:
#if BX_SUPPORT_X86_64
    case BX_IA_JMP_Jbq:
#endif
    // jmp absolute indirect
    case BX_IA_JMP_Ew:        // opcode 0xFF /4
    case BX_IA_JMP_Ed:
#if BX_SUPPORT_X86_64
    case BX_IA_JMP_Eq:
#endif

    case BX_IA_JMPF_Op16_Ep:  // opcode 0xFF /5
    case BX_IA_JMPF_Op32_Ep:
#if BX_SUPPORT_X86_64
    case BX_IA_JMPF_Op64_Ep:
#endif
      bx_dbg_stepN_command(dbg_cpu, 1);
      return;
  }

  // calls, ints, loops and so on
  int BpId = bx_dbg_lbreakpoint_command(bkStepOver, laddr + i.ilen(), NULL);
  if (BpId == -1) {
    dbg_printf("bx_dbg_step_over_command:: Failed to set lbreakpoint !\n");
    return;
  }

  bx_dbg_continue_command(1);

  if (bx_dbg_del_lbreak(BpId))
    bx_dbg_breakpoint_changed();
}

unsigned bx_dbg_disasm_wrapper(bool is_32, bool is_64, bx_address cs_base, bx_address ip, const Bit8u *instr, char *disbuf, int disasm_style)
{
  BxDisasmStyle new_disasm_style;

  if (disasm_style == BX_DISASM_INTEL || disasm_style == BX_DISASM_GAS)
    new_disasm_style = BxDisasmStyle(disasm_style);
  else if (disasm_syntax_intel == BX_DISASM_INTEL)
    new_disasm_style = BxDisasmStyle(BX_DISASM_INTEL);
  else
    new_disasm_style = BxDisasmStyle(BX_DISASM_GAS);

  bxInstruction_c i;
  disasm(instr, is_32, is_64, disbuf, &i, cs_base, ip, new_disasm_style ? BX_DISASM_INTEL : BX_DISASM_GAS);
  unsigned ilen = i.ilen();

  return ilen;
}

#endif /* if BX_DEBUGGER */
