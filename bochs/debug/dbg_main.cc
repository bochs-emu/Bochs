/////////////////////////////////////////////////////////////////////////
// $Id: dbg_main.cc,v 1.18.2.2 2002-03-17 08:57:02 bdenney Exp $
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

// define shortcuts to get register from the default CPU
#define EBP (BX_CPU(dbg_cpu)->gen_reg[5].erx)
#define EIP (BX_CPU(dbg_cpu)->eip)
#define ESP (BX_CPU(dbg_cpu)->gen_reg[4].erx)
#define SP  (BX_CPU(dbg_cpu)->gen_reg[4].word.rx)

extern "C" {
#include <signal.h>
}

#include "bochs.h"
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

static unsigned doit = 0;

#define SIM_NAME0 "bochs"
#ifndef SIM_NAME1_STR
#define SIM_NAME1_STR "sim1"
#endif
#define SIM_NAME(x) ((x == 0) ? SIM_NAME0 : SIM_NAME1_STR)

// default CPU in the debugger.  For commands like "dump_cpu" it will
// use the default instead of always dumping all cpus.
Bit32u dbg_cpu = 0;

static void bx_dbg_usage(void);
static char bx_debug_rc_fname[BX_MAX_PATH];
static char tmp_buf[512];
static char *tmp_buf_ptr;
static char *argv0 = NULL;


#if BX_NUM_SIMULATORS >= 2
#define BX_DBG_IO_JOURNAL_SIZE        1024
#define BX_DBG_UCMEM_JOURNAL_SIZE     1024
#define BX_DBG_ASYNC_JOURNAL_SIZE     1024
#define BX_DBG_MASTER_MODE              10
#define BX_DBG_SLAVE_MODE               11
// #define BX_DBG_DEFAULT_ICOUNT_QUANTUM   50
#define BX_DBG_DEFAULT_ICOUNT_QUANTUM   3 /* mch */

static unsigned bx_dbg_cosimulateN(bx_dbg_icount_t count);
static int      bx_dbg_compare_sim_iaddr(void);
static Boolean  bx_dbg_compare_sim_cpu(void);
static Boolean  bx_dbg_compare_sim_memory(void);
static void     bx_dbg_journal_a20_event(unsigned val);
#endif


static struct {
#if BX_NUM_SIMULATORS >= 2
  // some fields used only for cosimulation
  unsigned icount_quantum;
  unsigned master_slave_mode;
  unsigned master, slave;
  struct {
    struct {
      Bit8u    op;
      Bit8u    len;
      Bit16u   addr;
      Bit32u   value;
      } element[BX_DBG_IO_JOURNAL_SIZE];
    unsigned size;
    unsigned head, tail;
    } IO_journal;

  struct {
    struct {
      Bit8u    op;
      Bit8u    len;
      Bit32u   addr;
      Bit32u   value;
      } element[BX_DBG_UCMEM_JOURNAL_SIZE];
    unsigned size;
    unsigned head, tail;
    } UCmem_journal;

// need to handle DMA stuff in here...

#define BX_DBG_ASYNC_JOURNAL_NONE   0
#define BX_DBG_ASYNC_JOURNAL_A20    1
#define BX_DBG_ASYNC_JOURNAL_IAC    2
#define BX_DBG_ASYNC_JOURNAL_NMI    3
#define BX_DBG_ASYNC_JOURNAL_RESET  4

  // Asynchronous events at the boundaries they are *taken* by the master simulator.
  // These are replayed back to the slave at the same boundaries.
  struct {
    struct {
      unsigned        what; // A20, INTR, NMI, RESET, IAC, ...
      bx_dbg_icount_t icount;
      union {
        struct {
          unsigned val;
          } a20, nmi, reset, iac;
        // perhaps other more complex types here
        } u;
      } element[BX_DBG_ASYNC_JOURNAL_SIZE];
    unsigned size;
    unsigned head, tail;
    } async_journal;

  struct {
    Boolean iaddr;
    Boolean cpu;
    Boolean memory;
    } compare_at_sync;

  Boolean fast_forward_mode;

#endif  // #if BX_NUM_SIMULATORS >= 2

  // some fields used for single CPU debugger
  Boolean  auto_disassemble;
  unsigned disassemble_size;
  char     default_display_format;
  char     default_unit_size;
  Bit32u   default_addr;
  unsigned next_bpoint_id;

  // last icount known to be in sync
#if BX_DBG_ICOUNT_SIZE == 32
  Bit32u last_sync_icount;
#else  // BX_DBG_ICOUNT_SIZE == 64
  Bit64u last_sync_icount;
#endif
  } bx_debugger;



// cosim commands for handling of comparison of simulator
// environments when both simulators have reached a common
// point (synchronized).

// cosim compare_at_sync iaddr  (default is on)
// cosim compare_at_sync cpu    (default is off)
// cosim compare_at_sync memory (default is off)
// cosim compare iaddr
// cosim compare cpu
// cosim compare memory


typedef struct {
  FILE    *fp;
  char     fname[BX_MAX_PATH];
  unsigned lineno;
  } bx_infile_stack_entry_t;

bx_infile_stack_entry_t bx_infile_stack[BX_INFILE_DEPTH];
int                     bx_infile_stack_index = 0;

static int  bx_nest_infile(char *path);

static void bx_debug_ctrlc_handler(int signum);


static void bx_unnest_infile(void);
static void bx_get_command(void);
static void bx_dbg_print_guard_results();
static void bx_dbg_breakpoint_changed(void);

bx_dbg_callback_t bx_dbg_callback[BX_NUM_SIMULATORS];
bx_guard_t        bx_guard;


// DMA stuff
void bx_dbg_post_dma_reports(void);
#define BX_BATCH_DMA_BUFSIZE 512

static struct {
  unsigned this_many;  // batch this many max before posting events
  unsigned Qsize;      // this many have been batched
  struct {
    Bit32u   addr;   // address of DMA op
    unsigned len;    // number of bytes in op
    unsigned what;   // BX_READ or BX_WRITE
    Bit32u   val;    // value of DMA op
    bx_dbg_icount_t icount; // icount at this dma op
    } Q[BX_BATCH_DMA_BUFSIZE];
  } bx_dbg_batch_dma;


// some buffers for disassembly
#if BX_DISASM
static Bit8u bx_disasm_ibuf[32];
static char  bx_disasm_tbuf[512];
#endif


  int
bx_dbg_main(int argc, char *argv[])
{
  int i, bochs_argc=0, sim1_argc=0, sim2_argc=0;
  char **bochs_argv = NULL;
  char **sim1_argv = NULL;
  char **sim2_argv = NULL;
  argc = 1;
  
  bx_dbg_batch_dma.this_many = 1;
  bx_dbg_batch_dma.Qsize     = 0;

  // initialize callback functions, and guard environment
  memset(bx_dbg_callback, 0, sizeof(bx_dbg_callback));
  memset(&bx_guard, 0, sizeof(bx_guard));
  bx_guard.async.irq = 1;
  bx_guard.async.dma = 1;

  memset(&bx_debugger, 0, sizeof(bx_debugger));
#if BX_NUM_SIMULATORS >= 2
  bx_debugger.icount_quantum = BX_DBG_DEFAULT_ICOUNT_QUANTUM;
  bx_debugger.IO_journal.size = 0;
  bx_debugger.IO_journal.head = 0;
  bx_debugger.IO_journal.tail = 0;
  bx_debugger.UCmem_journal.size = 0;
  bx_debugger.UCmem_journal.head = 0;
  bx_debugger.UCmem_journal.tail = 0;
  bx_debugger.async_journal.size = 0;
  bx_debugger.async_journal.head = 0;
  bx_debugger.async_journal.tail = 0;
  bx_debugger.master = 0;
  bx_debugger.slave  = 1;
  bx_debugger.compare_at_sync.iaddr  = 1;
  bx_debugger.fast_forward_mode = 0;
#endif
  bx_debugger.auto_disassemble = 1;
  bx_debugger.disassemble_size = 32;
  bx_debugger.default_display_format = 'x';
  bx_debugger.default_unit_size      = 'w';
  bx_debugger.default_addr = 0;
  bx_debugger.next_bpoint_id = 1;
  bx_debugger.last_sync_icount = 0;


  argv0 = strdup(argv[0]);

  bx_debug_rc_fname[0] = '\0';

  bochs_argv = (char **) &argv[0];
  sim1_argv = bochs_argv;  // start out with something reasonable
  sim2_argv = bochs_argv;  // start out with something reasonable
  bochs_argc = 1;
  sim1_argc = 1;
  sim2_argc = 1;

  // process "-rc pathname" option, if it exists
  i = 1;
  if ( (argc >= 2) && !strcmp(argv[1], "-rc") ) {
    if ( argc == 2 ) {
      BX_ERROR(( "%s: -rc option used, but no path specified.",
        argv[0] ));
      bx_dbg_usage();
      BX_EXIT(1);
      }
    strncpy(bx_debug_rc_fname, argv[2], BX_MAX_PATH-1);
    i += 2; // skip past "-rc" and filename
    bochs_argv = (char **) &argv[2];
    }

  // process options to bochs framework
  for (; i<argc; i++) {
    if (strcmp(argv[i], "-sim1") == 0) {
      break;
      }
    else if (strcmp(argv[i], "-sim2") == 0) {
      break;
      }
    bochs_argc++;
    }

  if (i<argc) {  // more args to process
    // process options to each CPU simulator
    if (strcmp(argv[i], "-sim1") == 0) {
process_sim1:
      sim1_argv = (char **) &argv[i];
      i++;
      for (; i<argc; i++) {
        if (strcmp(argv[i], "-sim2") == 0)
          goto process_sim2;
        sim1_argc++;
        }
      }
    else if (strcmp(argv[i], "-sim2") == 0) {
process_sim2:
      sim2_argv = (char **) &argv[i];
      i++;
      for (; i<argc; i++) {
        if (strcmp(argv[i], "-sim1") == 0)
          goto process_sim1;
        sim2_argc++;
        }
      }
    }


  bx_infile_stack_index = 0;
  bx_infile_stack[0].fp = stdin;
  strncpy(bx_infile_stack[0].fname, argv[0], BX_MAX_PATH);
  bx_infile_stack[0].fname[BX_MAX_PATH-1] = 0;
  bx_infile_stack[0].lineno = 0;


  if (bx_debug_rc_fname[0] == '\0') {
    BX_INFO(("Warning: no rc file specified.", argv[0]));
    }
  else {
    BX_INFO (("%s: using rc file '%s'.", argv[0], bx_debug_rc_fname));
    // if there's an error, the user will know about it before proceeding
    (void) bx_nest_infile(bx_debug_rc_fname);
    }

#if BX_DISASM
  memset(bx_disasm_ibuf, 0, sizeof(bx_disasm_ibuf));
#endif

  BX_SIM1_INIT(&bx_dbg_callback[0], sim1_argc, sim1_argv);
#if BX_NUM_SIMULATORS > 1
  BX_SIM2_INIT(&bx_dbg_callback[1], sim2_argc, sim2_argv);
#endif

  // parse any remaining args in the usual way
  bx_parse_cmdline (1, bochs_argc, bochs_argv);

  // initialize hardware
  bx_init_hardware();   // doesn't this duplicate things?

  SIM->set_init_done (1);

#if BX_NUM_SIMULATORS >= 2
  bx_debugger.compare_at_sync.cpu    = 0;
  bx_debugger.compare_at_sync.memory = 0;
#endif

  // call init routines for each CPU+mem simulator
  // initialize for SMP. one memory, multiple processors.
#if BX_SUPPORT_APIC
  memset(apic_index, 0, sizeof(apic_index[0]) * APIC_MAX_ID);
#endif

#if BX_SMP_PROCESSORS==1
  BX_MEM(0)->init_memory(bx_options.memory.Osize->get () * 1024*1024);
  BX_MEM(0)->load_ROM(bx_options.rom.Opath->getptr (), bx_options.rom.Oaddress->get ());
  BX_MEM(0)->load_ROM(bx_options.vgarom.Opath->getptr (), 0xc0000);
  BX_CPU(0)->init (BX_MEM(0));
  BX_CPU(0)->reset(BX_RESET_HARDWARE);
#else
  // SMP initialization
  bx_mem_array[0] = new BX_MEM_C ();
  bx_mem_array[0]->init_memory(bx_options.memory.Osize->get () * 1024*1024);
  bx_mem_array[0]->load_ROM(bx_options.rom.Opath->getptr (), bx_options.rom.Oaddress->get ());
  bx_mem_array[0]->load_ROM(bx_options.vgarom.Opath->getptr (), 0xc0000);
  for (int i=0; i<BX_SMP_PROCESSORS; i++) {
    BX_CPU(i) = new BX_CPU_C ();
    BX_CPU(i)->init (BX_MEM(0));
    // assign apic ID from the index of this loop
    // if !BX_SUPPORT_APIC, this will not compile.
    BX_CPU(i)->local_apic.set_id (i);
    BX_CPU(i)->reset(BX_RESET_HARDWARE);
  }
#endif

#if BX_NUM_SIMULATORS > 1
#error cosimulation not supported until SMP stuff settles
  BX_MEM(1) = new BX_MEM_C ();
  BX_CPU(1) = new BX_CPU_C (BX_MEM(1));
  BX_CPU(1)->reset(BX_RESET_HARDWARE);
  BX_MEM(1)->init_memory(bx_options.memory.Osize->get () * 1024*1024);
  BX_MEM(1)->load_ROM(bx_options.rom.path->getptr (), bx_options.rom.address->get ());
  BX_MEM(1)->load_ROM(bx_options.vgarom.path->getptr (), 0xc0000);
#endif


  // (mch) Moved from main.cc
  bx_devices.init(BX_MEM(0));
  bx_gui.init_signal_handlers ();
  bx_pc_system.start_timers();

  // setup Ctrl-C handler
  signal(SIGINT, bx_debug_ctrlc_handler);

  // Print disassembly of the first instruction...  you wouldn't think it
  // would have to be so hard.  First initialize guard_found, since it is used
  // in the disassembly code to decide what instruction to print.
  BX_CPU_THIS_PTR guard_found.cs =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  BX_CPU_THIS_PTR guard_found.eip =
    BX_CPU_THIS_PTR prev_eip;
  BX_CPU_THIS_PTR guard_found.laddr =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base + BX_CPU_THIS_PTR prev_eip;
  BX_CPU_THIS_PTR guard_found.is_32bit_code =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b;
  // finally, call the usual function to print the disassembly
  fprintf (stderr, "Next at t=%lld\n", bx_pc_system.time_ticks ());
  bx_dbg_disassemble_current (-1, 0);  // all cpus, don't print time

  bx_dbg_user_input_loop();

  bx_dbg_exit(0);
  return(0); // keep compiler happy
}

  void
bx_dbg_usage(void)
{
  fprintf (stderr, "usage: %s [-rc path] [-sim1 ... ] [-sim2 ... ]\n", argv0);
}


  void
bx_dbg_user_input_loop(void)
{
  int reti;
  unsigned include_cmd_len = strlen(BX_INCLUDE_CMD);

  while ( 1 ) {
    bx_get_command();
    if ( (*tmp_buf_ptr == '\n') || (*tmp_buf_ptr == 0) ) {
      if (bx_infile_stack_index == 0)
        fprintf(stderr, "\n");
      }
    else if ( (strncmp(tmp_buf_ptr, BX_INCLUDE_CMD, include_cmd_len) == 0) &&
              (tmp_buf_ptr[include_cmd_len] == ' ' ||
               tmp_buf_ptr[include_cmd_len] == '\t') ) {
      char *ptr;
      int len;

      ptr = tmp_buf_ptr + include_cmd_len+1;
      while ( *ptr==' ' || *ptr=='\t' )
        ptr++;

      len = strlen(ptr);
      if (len == 0) {
        fprintf(stderr, "%s: no filename given to 'source' command.\n", argv0);
        if (bx_infile_stack_index > 0) {
          fprintf(stderr, "%s: ERROR in source file causes exit.\n", argv0);
          bx_dbg_exit(1);
          }
        continue;
        }
      ptr[len-1] = 0; // get rid of newline
      reti = bx_nest_infile(ptr);
      if ((reti==0) && (bx_infile_stack_index > 0)) {
        fprintf(stderr, "%s: ERROR in source file causes exit.\n", argv0);
        bx_dbg_exit(1);
        }
      }
    else {
      // Give a chance to the command line extensions, to
      // consume the command.  If they return 0, then
      // we need to process the command.  A return of 1
      // means, the extensions have handled the command
      if ( bx_dbg_extensions(tmp_buf_ptr)==0 ) {
        // process command here
        bx_add_lex_input(tmp_buf_ptr);
        bxparse();
        }
      }
    }
}

  void
bx_get_command(void)
{
  char *charptr_ret;

  bx_infile_stack[bx_infile_stack_index].lineno++;

  char prompt[256];
  if (bx_infile_stack_index == 0) {
    sprintf(prompt, "<bochs:%d> ", bx_infile_stack[bx_infile_stack_index].lineno);
    }
#if HAVE_LIBREADLINE
  if (bx_infile_stack_index == 0) {
    charptr_ret = readline (prompt);
    // beware, returns NULL on end of file
    if (charptr_ret && strlen(charptr_ret) > 0) {
      add_history (charptr_ret);
      strcpy (tmp_buf, charptr_ret);
      strcat (tmp_buf, "\n");
      free (charptr_ret);
      charptr_ret = &tmp_buf[0];
    }
  } else {
    charptr_ret = fgets(tmp_buf, 512,
      bx_infile_stack[bx_infile_stack_index].fp);
  }
#else
  if (bx_infile_stack_index == 0)
    fprintf (stderr, "%s", prompt);
  charptr_ret = fgets(tmp_buf, 512,
    bx_infile_stack[bx_infile_stack_index].fp);
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

    fprintf(stderr, "fgets() returned ERROR.\n");
fprintf(stderr, "intr request was %u\n", bx_guard.interrupt_requested);
    bx_dbg_exit(1);
    }
  tmp_buf_ptr = &tmp_buf[0];

  // look for first non-whitespace character
  while ( ((*tmp_buf_ptr == ' ') || (*tmp_buf_ptr == '\t')) &&
          (*tmp_buf_ptr != '\n') && (*tmp_buf_ptr != 0) ) {
    tmp_buf_ptr++;
    }

  return;
}

  int
bx_nest_infile(char *path)
{
  FILE *tmp_fp;

  tmp_fp = fopen(path, "r");
  if (!tmp_fp) {
    fprintf(stderr, "%s: can not open file '%s' for reading.\n",
            argv0, path);
    return(0);
    }

  if ( (bx_infile_stack_index+1) >= BX_INFILE_DEPTH ) {
    fprintf(stderr, "%s: source files nested too deeply\n", argv0);
    return(0);
    }

  bx_infile_stack_index++;
  bx_infile_stack[bx_infile_stack_index].fp = tmp_fp;
  strncpy(bx_infile_stack[bx_infile_stack_index].fname, path, BX_MAX_PATH);
  bx_infile_stack[bx_infile_stack_index].fname[BX_MAX_PATH-1] = 0;
  bx_infile_stack[bx_infile_stack_index].lineno = 0;
  return(1);
}

  void
bx_unnest_infile(void)
{
  if (bx_infile_stack_index <= 0) {
    fprintf(stderr, "%s: ERROR: unnest_infile(): nesting level = 0.\n",
      argv0);
    bx_dbg_exit(1);
    }

  fclose(bx_infile_stack[bx_infile_stack_index].fp);
  bx_infile_stack_index--;
}

  int
bxwrap(void)
{
  fprintf(stderr, "%s: ERROR: bxwrap() called.\n", argv0);
  bx_dbg_exit(1);
  return(0); // keep compiler quiet
}


  void
bxerror(char *s)
{
  fprintf(stderr, "%s:%d: %s at '%s'\n",
    bx_infile_stack[bx_infile_stack_index].fname,
    bx_infile_stack[bx_infile_stack_index].lineno,
    s, bxtext);

  if (bx_infile_stack_index > 0) {
    fprintf(stderr, "%s: ERROR in source file causes exit.\n", argv0);
    bx_dbg_exit(1);
    }
}

  void
bx_debug_ctrlc_handler(int signum)
{
  UNUSED(signum);
  BX_INFO(("Ctrl-C detected in signal handler."));

  signal(SIGINT, bx_debug_ctrlc_handler);
  bx_guard.interrupt_requested = 1;
}

  void
bx_dbg_exit(int code)
{
  BX_DEBUG(( "dbg: before sim1_exit" ));
  for (int cpu=0; cpu < BX_SMP_PROCESSORS; cpu++) {
    if (BX_CPU(cpu)) BX_CPU(cpu)->atexit();
  }

#if BX_NUM_SIMULATORS >= 2
  fprintf(stderr, "before sim2_exit\n");
  if (BX_CPU(1)) BX_CPU(1)->atexit();
#endif

  bx_atexit();

  exit(code);
}


//
// comands invoked from parser
//

  void
bx_dbg_quit_command(void)
{
  BX_INFO(("dbg: Quit"));
  bx_dbg_exit(0);
}

void
bx_dbg_trace_on_command(void)
{
  BX_CPU(dbg_cpu)->trace = 1;
  fprintf (stderr, "Tracing enabled for %s\n", BX_CPU(dbg_cpu)->name);
}

void
bx_dbg_trace_off_command(void)
{
  BX_CPU(dbg_cpu)->trace = 0;
  fprintf (stderr, "Tracing disabled for %s\n", BX_CPU(dbg_cpu)->name);
}

void
bx_dbg_trace_reg_on_command(void)
{
  BX_CPU(dbg_cpu)->trace_reg = 1;
  fprintf (stderr, "Register-Tracing enabled for %s\n", BX_CPU(dbg_cpu)->name);
}

void
bx_dbg_trace_reg_off_command(void)
{
  BX_CPU(dbg_cpu)->trace_reg = 0;
  fprintf (stderr, "Register-Tracing disabled for %s\n", BX_CPU(dbg_cpu)->name);
}

void
bx_dbg_ptime_command(void)
{
      fprintf(stderr, "ptime: %lld\n", bx_pc_system.time_ticks());
#if BX_NUM_SIMULATORS >= 2
    fprintf(stderr,
#if BX_DBG_ICOUNT_SIZE == 32
      "Last synchronized icount was %lu\n",
      (unsigned long) bx_debugger.last_sync_icount
#else  // BX_DBG_ICOUNT_SIZE == 64
      "Last synchronized icount was %Lu\n",
      (unsigned long long) bx_debugger.last_sync_icount
#endif /* BX_DBG_ICOUNT_SIZE == 32 */
      );
#endif /* BX_NUM_SIMULATORS >= 2 */
}

int timebp_timer = -1;
Bit64u timebp_queue[MAX_CONCURRENT_BPS];
int timebp_queue_size = 0;

void
bx_dbg_timebp_command(Boolean absolute, Bit64u time)
{
      Bit64u diff = (absolute) ? time - bx_pc_system.time_ticks() : time;
      Bit64u abs_time = (absolute) ? time : time + bx_pc_system.time_ticks();

      if (diff < 0) {
        fprintf(stderr, "Request for time break point in the past. I can't let you do that.\n");
        return;
      }

      if (timebp_queue_size == MAX_CONCURRENT_BPS) {
	    fprintf(stderr, "Too many time break points\n");
	    return;
      }
	    
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
			      fprintf(stderr, "Time breakpoint not inserted (duplicate)\n");
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
	    timebp_timer = bx_pc_system.register_timer_ticks(&bx_pc_system, bx_pc_system_c::timebp_handler, diff, 0, 1);
      }

      fprintf(stderr, "Time breakpoint inserted. Delta = %d\n", diff);
}

void
bx_dbg_diff_memory(void)
{
#if BX_NUM_SIMULATORS < 2
	printf("diff-memory supported only in cosimulation mode\n");
#else
	int num_pages = bx_options.memory.Osize->get () * 1024 / 4;
	for (int i = 0; i < num_pages; i++) {
		BX_CPU(dbg_cpu)->dbg_dirty_pages[i] = 1;
	}
	if (bx_dbg_compare_sim_memory())
		printf("[diff-memory] Diff detected\n");
	else
		printf("[diff-memory] No diff detected\n");
#endif /* NUM_SIMULATORS < 2 */
}

void
bx_dbg_sync_memory(Boolean set)
{
#if BX_NUM_SIMULATORS < 2
	printf("sync-memory supported only in cosimulation mode\n");
#else
	bx_debugger.compare_at_sync.memory = set;
	printf("Memory sync %s\n", (set) ? "enabled" : "disabled");
#endif
}

void
bx_dbg_sync_cpu(Boolean set)
{
#if BX_NUM_SIMULATORS < 2
	printf("sync-cpu supported only in cosimulation mode\n");
#else
	bx_debugger.compare_at_sync.cpu = set;
	printf("Register file sync %s\n", (set) ? "enabled" : "disabled");
#endif
}

void
bx_dbg_fast_forward(Bit32u num)
{
#if BX_NUM_SIMULATORS < 2
	printf("fast-forward supported only in cosimulation mode\n");
#else
	printf("Entering fast-forward mode\n");

	// Bit32u save_icount_quantum = bx_debugger.icount_quantum;
	// bx_debugger.icount_quantum = num;

	bx_guard.interrupt_requested = 0;

	bx_debugger.fast_forward_mode = 1;
	for (Bit32u e = 0; e < num; e += bx_debugger.icount_quantum)
		if (!bx_dbg_cosimulateN(bx_debugger.icount_quantum))
			break;
	bx_debugger.fast_forward_mode = 0;
	// bx_debugger.icount_quantum = save_icount_quantum;

	bx_vga.timer_handler(&bx_vga);

	printf("Copying CPU...\n");
	bx_dbg_cpu_t cpu;
	if (!BX_CPU(0)->dbg_get_cpu(&cpu) || !BX_CPU(1)->dbg_set_cpu(&cpu))
		printf("Error copying CPU data!\n");

	printf("Copying memory...\n");
	int num_pages = bx_options.memory.Osize->get () * 1024 / 4;
	for (int i = 0; i < num_pages; i++) {
		if (BX_CPU(0)->dbg_dirty_pages[i]) {
			Bit32u page_start = i * 1024 * 4;
			printf("Copying page %08x\n", page_start);
			extern Bit8u* SIM1_GET_PHYS_PTR(Bit32u page_start);
			Bit8u* sim0_page_vec = bx_mem0.vector + page_start;
			Bit8u* sim1_page_vec = SIM1_GET_PHYS_PTR(page_start);
			memcpy(sim1_page_vec, sim0_page_vec, 1024 * 4);
		}
	}

	printf("Taking async events...\n");

	printf("Exiting fast-forward mode\n");
#endif
}

static Bit32u
conv_4xBit8u_to_Bit32u (Bit8u* buf)
{
	Bit32u ret = 0;
	for (int i = 0; i < 4; i++) {
		ret |= (buf[i] << (8 * i));
	}
	return ret;
}

/*
  (mch) Print various info for logical address.
*/
void
bx_dbg_info_address(Bit32u seg_reg_num, Bit32u offset)
{
#if BX_NUM_SIMULATORS < 2
	printf("addr-info only supported in cosim configuration.\n");
#else
	for (int sim = 0; sim < 2; sim++) 
	{
		/* Find page table base address */
		bx_dbg_cpu_t regs;
		BX_CPU(sim)->dbg_get_cpu(&regs);
		Bit32u base = regs.cr3 & ~0xfff;

		Bit8u buf[4];
		Bit32u directory_addr = base + (offset >> 22) * 4;
		Bit32u directory;
		if (BX_CPU(sim)->mem->dbg_fetch_mem(directory_addr, 4, buf)) {
			directory = conv_4xBit8u_to_Bit32u(buf);
			Bit32u table_addr = (directory & ~0xfff) + ((offset >> 12) & 0x3ff) * 4;
			Bit32u table;

			printf("[%s] ", SIM_NAME(sim));
			printf("PDE: %08x (", directory);
			printf("%s,  %s, %s, %s, %s)",
			       (directory & 1) ? "Present" : "Not present",
			       (directory & 2) ? "Read/Write" : "Read-only",
			       (directory & 4) ? "User" : "Supervisor",
			       (directory & (1 << 5)) ? "Accessed" : "-",
			       (directory & (1 << 6)) ? "Dirty" : "-");

			if (directory & 1) {
				if (BX_CPU(sim)->mem->dbg_fetch_mem(table_addr, 4, buf)) {
					table = conv_4xBit8u_to_Bit32u(buf);

					printf(", PTE: %08x (", table);
					printf("%s,  %s, %s, %s, %s)\n",
					       (table & 1) ? "Present" : "Not present",
					       (table & 2) ? "Read/Write" : "Read-only",
					       (table & 4) ? "User" : "Supervisor",
					       (table & (1 << 5)) ? "Accessed" : "-",
					       (table & (1 << 6)) ? "Dirty" : "-");
				} else {
					printf("[%s] Could not read from physical address %08x\n",
					       SIM_NAME(sim), directory_addr);
					return;
				}
			} else {
				printf("\n");
			}
		} else {
			printf("[%s] Could not read from physical address %08x\n",
			       SIM_NAME(sim), directory_addr);
			return;
		}
	}
#endif
}

void
bx_dbg_record_command(char* path_quoted)
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
	    fprintf(stderr, "IO record file '%s' opened\n", path_quoted);
      else
	    fprintf(stderr, "Error opening '%s' for writing\n", path_quoted);
}

static FILE* playback_file = 0;

struct playback_entry_t
{
  char command[100];
  Bit32u argument;

  void trigger ();
};

static playback_entry_t playback_entry;
static Bit64u last_playback_time = 0;
static int playback_timer_index = -1;

void
bx_dbg_playback_command(char* path_quoted)
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
	    fprintf(stderr, "Playback from '%s'\n", path_quoted);
	    last_playback_time = 0;
	    fprintf(stderr, "playback times relative from %lld\n", bx_pc_system.time_ticks());
	    enter_playback_entry();
      } else
	    fprintf(stderr, "Error opening '%s' for reading\n", path_quoted);
}

// BW added. toggles vm86 mode switch breakpoint
//dummy not used and may be null
void
bx_dbg_modebp_command(char* dummy)
{
      BX_CPU(dbg_cpu)->debug_vm == BX_CPU(dbg_cpu)->eflags.vm;
      BX_CPU(dbg_cpu)->mode_break = !BX_CPU(dbg_cpu)->mode_break;
      fprintf(stderr," mode switch break %s\n", 
	      BX_CPU(dbg_cpu)->mode_break ? "enabled" : "disabled");
}

// where
// stack trace: ebp -> old ebp
// return eip at ebp + 4
void
bx_dbg_where_command()
{
      if (!BX_CPU(dbg_cpu)->protected_mode()) {
	    fprintf(stderr, "'where' only supported in protected mode\n");
	    return;
      }
      if (BX_CPU(dbg_cpu)->sregs[BX_SREG_SS].cache.u.segment.base != 0) {
	    fprintf(stderr, "non-zero stack base\n");
	    return;
      }
      Bit32u bp = EBP;
      Bit32u ip = EIP;
      fprintf(stderr, "(%d) 0x%08x\n", 0, ip);
      for (int i = 1; i < 50; i++) {
	    // Up
	    Boolean paddr_valid;
	    Bit32u paddr;
	    Bit8u buf[4];

	    // bp = [bp];
	    BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(bp, &paddr, &paddr_valid);
	    if (paddr_valid) {
		  if (BX_MEM(0)->dbg_fetch_mem(paddr, 4, buf)) {
			bp = conv_4xBit8u_to_Bit32u(buf);
		  } else {
			fprintf(stderr, "(%d) Physical memory read error (BP)\n", i);
			break;
		  }
	    } else {
		  fprintf(stderr, "(%d) Could not translate linear address (BP)\n", i);
		  break;
	    }

	    // ip = [bp + 4];
	    BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(bp + 4, &paddr, &paddr_valid);
	    if (paddr_valid) {
		  if (BX_MEM(0)->dbg_fetch_mem(paddr, 4, buf)) {
			ip = conv_4xBit8u_to_Bit32u(buf);
		  } else {
			fprintf(stderr, "(%d) Physical memory read error (IP)\n", i);
			break;
		  }
	    } else {
 		  fprintf(stderr, "(%d) Could not translate linear address (IP)\n", i);
		  break;
	    }

	    // Print
	    fprintf(stderr, "(%d) 0x%08x\n", i, ip);
      }
}

void
bx_dbg_print_string_command(Bit32u start_addr)
{
      fprintf(stderr, "0x%08x: ", start_addr);
      for (int i = 0; ; i++) {
	    Bit32u paddr;
	    Bit32u paddr_valid;
	    Bit8u buf[1];
	    BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(start_addr+i, &paddr, &paddr_valid);
	    if (paddr_valid) {
		  if (BX_MEM(0)->dbg_fetch_mem(paddr, 1, buf)) {
			if (buf[0] == 0)
			      break;
			if (isgraph(buf[0]) || buf[0] == 0x20)
			      fprintf(stderr, "%c", buf[0]);
			else
			      fprintf(stderr, "\\%d", buf[0]);
		  } else {
			fprintf(stderr, "<read error>");
			break;
		  }
	    } else {
		  fprintf(stderr, "<no translation>");
		  break;
	    }
      }
      fprintf(stderr, "\n");
}

static Bit32u last_cr3;
static int last_pe = 0;
static int last_vm = 0;

unsigned int dbg_show_mask = 0;
// 0x80 print mode
// 0x40 print interrupts
// 0x20 print calls

//BW added. toggles show symbolic info (calls to begin with)
// 0x1 call
// 0x2 return
// 0x4 int
// 0x8 iret
// 0x10 interrupts (includes iret)

static void dbg_dump_table(Boolean);

void bx_dbg_show_command(char* arg)
{
      if(arg) {
	    if (!strcmp(arg,"\"mode\"")){
		  dbg_show_mask = 0x80;
	    } else if (!strcmp(arg,"\"int\"")){
		  dbg_show_mask = 0xc0;
	    } else if(!strcmp(arg,"\"call\"")){
		  dbg_show_mask = 0xe0;
	    } else if(!strcmp(arg,"\"ret\"")){
		  dbg_show_mask = 0xe0;
	    } else if(!strcmp(arg,"\"off\"")){
		  dbg_show_mask = 0x0;
	    } else if(!strcmp(arg,"\"tab\"")){
		  dbg_dump_table(1);
		  return;
	    } else if(!strcmp(arg,"\"c\"")){
		  dbg_dump_table(0);
		  return;
	    } else if(!strcmp(arg,"\"dbg-all\"")){
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
		    printf("Turned on all bx_dbg flags\n");
		    return;
	    } else if(!strcmp(arg,"\"none\"")){
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
		    printf("Turned off all bx_dbg flags\n");
		    return;
	    } else if(!strcmp(arg,"\"vga\"")){
	      bx_vga.timer ();
	      return;
	    } else {
		  printf("Unrecognized arg: %s ('mode' 'int' 'call' 'ret' 'dbg-all' are valid)\n",arg);
		  return;
	    }
      } else {
	fprintf(stderr," show mask is 0x%x\n", dbg_show_mask);
	return;
      }

      // enable trace if any print is active
      if(dbg_show_mask & 0xe0)
	    dbg_show_mask |= 0x1f;
	    
      fprintf(stderr," show mask is 0x%x, cleared show_flag\n", dbg_show_mask);
      BX_CPU(dbg_cpu)->show_flag = 0;
      last_cr3 = BX_CPU(dbg_cpu)->cr3;
      last_pe = BX_CPU(dbg_cpu)->cr0.pe;
      last_vm = BX_CPU(dbg_cpu)->eflags.vm;

      fprintf(stderr,"%10lld: address %04x:%08x %08x\n\n", 
	      bx_pc_system.time_ticks(),
	      BX_CPU(dbg_cpu)->guard_found.cs,
	      BX_CPU(dbg_cpu)->guard_found.eip,
	      BX_CPU(dbg_cpu)->guard_found.laddr);

}

void
playback_function(void* this_ptr)
{
      ((playback_entry_t*)this_ptr)->trigger();
}

void
enter_playback_entry()
{
      static const int playback_buf_size = 100;
      char playback_buf[playback_buf_size];
      if (!fgets(playback_buf, playback_buf_size, playback_file))
	    return;

      Bit64u time;
      Bit32u argument;
      if (sscanf(playback_buf, "%s %lld %x", playback_entry.command, &time, &playback_entry.argument) != 3) {
	    fprintf(stderr, "Parse error in playback string '%s'\n", playback_buf);
	    return;
      }

      Bit64u diff = time - last_playback_time;
      last_playback_time = time;

      if (diff < 0) {
	    BX_PANIC(("Negative diff in playback"));
      } else if (diff == 0) {
	    playback_entry.trigger();
      } else {
	    if (playback_timer_index >= 0)
		  bx_pc_system.activate_timer_ticks(playback_timer_index, diff, 0);
	    else
		  playback_timer_index = bx_pc_system.register_timer_ticks(&playback_entry, playback_function, diff, 0, 1);
      }
}

void
playback_entry_t::trigger ()
{
      if (!strcmp("gen_scancode", command)) {
	    bx_devices.keyboard->gen_scancode(argument);
      } else {
	    fprintf(stderr, "Unknown playback command '%s'\n", command);
	    return;
      }
      enter_playback_entry();
}

void
bx_dbg_print_stack_command(int nwords)
{
	// Get linear address for stack top
	Bit32u sp = (BX_CPU(dbg_cpu)->sregs[BX_SREG_SS].cache.u.segment.d_b) ? ESP : SP;
	Bit32u linear_sp = sp + BX_CPU(dbg_cpu)->sregs[BX_SREG_SS].cache.u.segment.base;
	Bit8u buf[8];

	for (int i = 0; i < nwords; i++) {
		Bit32u paddr;
		Boolean paddr_valid;
		BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(sp, &paddr, &paddr_valid);
		if (paddr_valid) {
			if (BX_MEM(0)->dbg_fetch_mem(paddr, 2, buf))
				fprintf(stderr, "   %08x [%08x]  %04x\n", linear_sp, paddr, (int)buf[0] | ((int)buf[1] << 8));
			else
				fprintf(stderr, "   %08x [%08x]  <read error>\n", paddr, linear_sp);
		} else {
			fprintf(stderr, "   %08x   <could not translate>\n", linear_sp);
		}
		sp += 2;
		linear_sp += 2;
	}
}

#if !BX_HAVE_HASH_MAP

static char *BX_HAVE_HASH_MAP_ERR = "context not implemented because BX_HAVE_HASH_MAP=0\n";
char*
bx_dbg_symbolic_address(Bit32u context, Bit32u eip, Bit32u base)
{
  static Boolean first = true;
  if (first) {
    fprintf (stderr, BX_HAVE_HASH_MAP_ERR);
    first = false;
  }
  return "unknown context";
}

void
bx_dbg_symbol_command(char* filename, Boolean global, Bit32u offset)
{
  fprintf (stderr, BX_HAVE_HASH_MAP_ERR);
}

#else   /* if BX_HAVE_HASH_MAP == 1 */

/* Haven't figured out how to port this code to OSF1 cxx compiler.
   Until a more portable solution is found, at least make it easy
   to disable the template code:  just set BX_HAVE_HASH_MAP=0
   in config.h */

#include <hash_map.h>
#include <set.h>

struct symbol_entry_t
{
  symbol_entry_t (Bit32u _start = 0, char* _name = 0)
  {
	start = _start;
	name = _name;
  }

  char* name;
  Bit32u start;
};

struct lt_symbol_entry_t
{
  bool operator()(const symbol_entry_t* s1, const symbol_entry_t* s2) const
  {
	return s1->start < s2->start;
  }
};

struct context_t
{
  context_t (Bit32u);
  static context_t* get_context(Bit32u);
  symbol_entry_t* get_symbol_entry(Bit32u);
  void add_symbol(symbol_entry_t*);

private:
  static hash_map<int,context_t*>* map;
  set<symbol_entry_t*,lt_symbol_entry_t>* syms;
  Bit32u id;
};

hash_map<int,context_t*>* context_t::map = new hash_map<int,context_t*>;

context_t::context_t (Bit32u _id)
{
      id = _id;
      syms = new set<symbol_entry_t*, lt_symbol_entry_t>;
      (*map)[id] = this;
}

context_t*
context_t::get_context(Bit32u i)
{
      return (*map)[i];
}

symbol_entry_t*
context_t::get_symbol_entry(Bit32u ip)
{
      symbol_entry_t probe;
      probe.start = ip;
      set<symbol_entry_t*>::iterator iter = syms->upper_bound(&probe);
      if (iter == syms->end())
	    return 0;
      else if (iter == syms->begin())
	    return 0;
      else {
	    iter--;
	    return *iter;
      }
}

void
context_t::add_symbol(symbol_entry_t* sym)
{
      syms->insert(sym);
}

char*
bx_dbg_symbolic_address(Bit32u context, Bit32u eip, Bit32u base)
{
      static char buf[80];
      if (base != 0) {
	    snprintf (buf, 80, "non-zero base");
	    return buf;
      }
      // Look up this context
      context_t* cntx = context_t::get_context(context);
      if (!cntx) {
	    // Try global context
	    cntx = context_t::get_context(0);
	    if (!cntx) {
		  snprintf (buf, 80, "unknown context");
		  return buf;
	    }
      }

      symbol_entry_t* entr = cntx->get_symbol_entry(eip);
      if (!entr) {
	    snprintf (buf, 80, "no symbol");
	    return buf;
      }
      snprintf (buf, 80, "%s+%x", entr->name, eip - entr->start);
      return buf;
}

void
bx_dbg_symbol_command(char* filename, Boolean global, Bit32u offset)
{
      if (filename[0] == '"')
	    filename++;
      int len = strlen(filename);
      if (filename[len - 1] == '"')
	    filename[len - 1] = '\0';

      // Install symbols in correct context (page table)
      // The file format should be
      // address symbol (example '00002afe _StartLoseNT')
      
      context_t* cntx = (global)
	    ? context_t::get_context(0)
	    : context_t::get_context((BX_CPU(dbg_cpu)->cr3) >> 12);

      if (!cntx)
	    cntx = (global)
		  ? new context_t(0)
		  : new context_t((BX_CPU(dbg_cpu)->cr3) >> 12);

      FILE* fp = fopen(filename, "r");
      if (!fp) {
	    fprintf(stderr, "Could not open symbol file '%s'\n", filename);
	    return;
      }
      char buf[200];
      while (fgets(buf, 200, fp)) {
	    // Parse
	    char* sym_name = buf;
	    for (int i = 0; i < 200 && buf[i]; i++) {
		  if (buf[i] == ' ') {
			buf[i] = '\0';
			sym_name = buf + i + 1;
			break;
		  }
	    }
	    if (sym_name == buf) {
		  fprintf(stderr, "Syntax error '%s'\n", buf);
		  break;
	    }
	    Bit32u addr = strtoul(buf, 0, 16);
	    if (sym_name[strlen(sym_name)-1] == '\n')
		  sym_name[strlen(sym_name)-1] = '\0';
	    symbol_entry_t* sym = new symbol_entry_t(addr + offset, strdup(sym_name));
	    cntx->add_symbol(sym);
      }
}
#endif

int num_write_watchpoints = 0;
int num_read_watchpoints = 0;
Bit32u write_watchpoint[MAX_WRITE_WATCHPOINTS];
Bit32u read_watchpoint[MAX_READ_WATCHPOINTS];
Boolean watchpoint_continue = 0;

void
bx_dbg_watch(int read, Bit32u address)
{
      if (read == -1) {
	    // print watch point info
	    int i;
	    for (i = 0; i < num_read_watchpoints; i++) {
		  Bit8u buf[2];
		  if (BX_MEM(0)->dbg_fetch_mem(read_watchpoint[i], 2, buf))
			fprintf(stderr, "read   %08x   (%04x)\n", read_watchpoint[i], (int)buf[0] | ((int)buf[1] << 8));
		  else
			fprintf(stderr, "read   %08x   (read error)\n", read_watchpoint[i]);
	    }
		for (i = 0; i < num_write_watchpoints; i++) {
			Bit8u buf[2];
			if (BX_MEM(0)->dbg_fetch_mem(write_watchpoint[i], 2, buf))
				fprintf(stderr, "write  %08x   (%04x)\n", write_watchpoint[i], (int)buf[0] | ((int)buf[1] << 8));
			else
				fprintf(stderr, "write  %08x   (read error)\n", write_watchpoint[i]);
		}
      } else {
	    if (read) {
		  if (num_read_watchpoints == MAX_READ_WATCHPOINTS) {
			fprintf(stderr, "Too many read watchpoints\n");
			return;
		  }
		  read_watchpoint[num_read_watchpoints++] = address;
		  fprintf(stderr, "Read watchpoint at %08x inserted\n", address);
	    } else {
		  if (num_write_watchpoints == MAX_WRITE_WATCHPOINTS) {
			fprintf(stderr, "Too many write watchpoints\n");
			return;
		  }
		  write_watchpoint[num_write_watchpoints++] = address;
		  fprintf(stderr, "Write watchpoint at %08x inserted\n", address);
	    }
      }
}

void
bx_dbg_unwatch(int read, Bit32u address)
{
      if (read == -1) {
	    // unwatch all
	    num_read_watchpoints = num_write_watchpoints = 0;
	    fprintf(stderr, "All watchpoints removed\n");
      } else {
	    if (read) {
		  fprintf(stderr, "Watchpoint remove not implemented\n");
	    } else {
		  fprintf(stderr, "Watchpoint remove not implemented\n");
	    }
      }
}

  void
bx_dbg_continue_command(void)
{
  // continue executing, until a guard found

  one_more:

#if BX_NUM_SIMULATORS >= 2
  bx_guard.interrupt_requested = 0;
  while (1) {
    if ( !bx_dbg_cosimulateN(bx_debugger.icount_quantum) )
      break;
    }
#else
  bx_guard.icount = 0;
  // I must guard for ICOUNT or one CPU could run forever without giving
  // the others a chance.
  bx_guard.guard_for |= BX_DBG_GUARD_ICOUNT;
  bx_guard.guard_for |=  BX_DBG_GUARD_CTRL_C; // stop on Ctrl-C


  bx_guard.interrupt_requested = 0;
	int stop = 0;
	int which = -1;
	while (!stop) {
		// the quantum is an arbitrary number of cycles to run in each
		// processor.  In SMP mode, when this limit is reached, the
		// cpu_loop exits so that another processor can be simulated
		// for a few cycles.  With a single processor, the quantum
		// setting should have no effect, although a low setting does
		// lead to poor performance because cpu_loop is returning and 
		// getting called again, over and over.
		int quantum = 25;
		int cpu;
		for (cpu=0; cpu < BX_SMP_PROCESSORS; cpu++) {
			BX_CPU(cpu)->guard_found.guard_found = 0;
			BX_CPU(cpu)->guard_found.icount = 0;
			bx_guard.icount = quantum;
			BX_CPU(cpu)->cpu_loop (-1);
			/// check out BX_CPU(cpu)->guard_found.icount
			//fprintf (stderr, "dbg_cont: after cpu_loop guard_found.icount=%d\n", BX_CPU(cpu)->guard_found.icount);
			// set stop flag if a guard found other than icount or halted
			unsigned long found = BX_CPU(cpu)->guard_found.guard_found;
			stop_reason_t reason = (stop_reason_t) BX_CPU(cpu)->stop_reason;
			if (found & BX_DBG_GUARD_ICOUNT) {
				// I expected this guard, don't stop
			} else if (found!=0) {
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
#if BX_SMP_PROCESSORS==1
		// all ticks are handled inside the cpu loop
#else
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
		BX_TICKN(max_executed);
#endif /* BX_SMP_PROCESSORS>1 */
	}
#endif /* BX_NUM_SIMULATORS */

  // (mch) hack
  bx_vga.timer_handler(&bx_vga);

  BX_INSTR_DEBUG_PROMPT();
  bx_dbg_print_guard_results();

  if (watchpoint_continue && (BX_CPU(which)->stop_reason == STOP_READ_WATCH_POINT ||
			      BX_CPU(which)->stop_reason == STOP_WRITE_WATCH_POINT))
	goto one_more;
}

  void
bx_dbg_stepN_command(bx_dbg_icount_t count)
{
  if (count == 0) {
    fprintf(stderr, "Error: stepN: count=0\n");
    return;
    }

#if BX_NUM_SIMULATORS >= 2
  bx_guard.interrupt_requested = 0;
  bx_dbg_cosimulateN(count);
#else
  // single CPU
  bx_guard.guard_for |= BX_DBG_GUARD_ICOUNT; // looking for icount
  bx_guard.guard_for |= BX_DBG_GUARD_CTRL_C; // or Ctrl-C
	// for now, step each CPU one BX_DBG_DEFAULT_ICOUNT_QUANTUM at a time
  //BX_INFO(("Stepping each CPU a total of %d cycles", count));
	for (unsigned cycle=0; cycle < count; cycle++) {
		for (unsigned cpu=0; cpu < BX_SMP_PROCESSORS; cpu++) {
		  //BX_INFO(("Stepping %s", BX_CPU(cpu)->name));
			bx_guard.icount = 1;
      bx_guard.interrupt_requested = 0;
			BX_CPU(cpu)->guard_found.guard_found = 0;
			BX_CPU(cpu)->guard_found.icount = 0;
			BX_CPU(cpu)->cpu_loop(-1);
		}
#if BX_SMP_PROCESSORS==1
		// ticks are handled inside the cpu loop
#else
		BX_TICK1 ();
#endif
	}
  //BX_INFO(("Stepped each CPU a total of %d cycles", count));
#endif

  BX_INSTR_DEBUG_PROMPT();
  bx_dbg_print_guard_results();
}


#if BX_NUM_SIMULATORS >= 2
  unsigned
bx_dbg_cosimulateN(bx_dbg_icount_t count)
{
  // execute both master & slave for count instructions,
  // handling asynchronous events, etc.
  // returns 0 = didn't get through all count instructions
  //             either a guard was hit, or a divergence occurred
  //         1 = got through all count instructions

  unsigned master, slave;
  bx_dbg_icount_t master_icount, slave_icount;
  Boolean bail_out = 0;
  unsigned ret = 0;
  Boolean save_INTR;
  Boolean pre_A20, post_A20;
  unsigned async_head;
  bx_dbg_icount_t async_icount, curr_icount;

  if (count == 0) {
    fprintf(stderr, "Error: cosimulateN: count=0\n");
    bx_dbg_exit(1);
    }

  bx_guard.guard_for |= BX_DBG_GUARD_ICOUNT;  // stop at icount
  bx_guard.guard_for &= ~BX_DBG_GUARD_CTRL_C; // ignore Ctrl-C

one_time_slice:
  // take minimum of requested count and maximum count quantum
  if (count > bx_debugger.icount_quantum)
    bx_guard.icount = bx_debugger.icount_quantum;
  else
    bx_guard.icount = count;

  // for now, assume...
  master = bx_debugger.master;
  slave  = bx_debugger.slave;

  // run master simulator
  bx_debugger.master_slave_mode = BX_DBG_MASTER_MODE;
  if (bx_guard.interrupt_requested) {
    bail_out = 1;
fprintf(stderr, "ctrlc typed\n");
    }
  bx_guard_found[master].guard_found = 0;
  bx_guard_found[master].icount = 0;
if (doit) fprintf(stderr, "requesting run of master for %u\n",
  (unsigned) bx_guard.icount);
  // save A20 value before master run
  pre_A20  = bx_pc_system.get_enable_a20();

  BX_MEM(master)->cpu_loop(-1);
  post_A20  = bx_pc_system.get_enable_a20(); // A20 after master run
  master_icount = bx_guard_found[master].icount;
  slave_icount = 0;
  if (master_icount)
    bx_pc_system.tickn(master_icount);
  save_INTR = bx_pc_system.INTR; // value after master run
  bx_pc_system.INTR = 0; // in case slave uses directly
  // Change A20 for slave run to model what it was at beginning of
  // master run, only if it needs to be changed.
  if (pre_A20 != post_A20) {
    bx_pc_system.set_enable_a20(pre_A20);
    if (BX_MEM(slave)->set_A20)
      BX_MEM(slave)->set_A20(pre_A20);
    }


  // if guard was anything except for icount, we should terminate
  // after synchronizing slave to master
  if (bx_guard_found[master].guard_found & ~BX_DBG_GUARD_ICOUNT)
    bail_out = 1;

  // Synchronize slave to master.  Account for Ctrl-C's typed during execution of
  // slave.
  bx_debugger.master_slave_mode = BX_DBG_SLAVE_MODE;
  do {
    // run slave for remaining instructions to catch up to master
    curr_icount = master_icount - slave_icount;
    if (bx_debugger.async_journal.size) {
      // If there were asynchronous events which occurred while the
      // master was running, have to run the slave up to each of these
      // points individually, and force it to take them on exactly the
      // same boundaries.
      async_head   = bx_debugger.async_journal.head;
      async_icount = bx_debugger.async_journal.element[async_head].icount;
      curr_icount  = async_icount;  // only run to next async event
      }
    else {
      async_head   = 0; // keep compiler happy
      async_icount = 0; // keep compiler happy
      }

    bx_guard_found[slave].guard_found = 0;
    bx_guard_found[slave].icount = 0;
    bx_guard.icount = curr_icount;

    if (curr_icount) {
      // Async event may be before completion of any instructions,
      // for example taking of interrupt.
if (doit) fprintf(stderr, "requesting run of slave for %u\n",
  (unsigned) bx_guard.icount);
      if (bx_debugger.fast_forward_mode) {
	      bx_guard_found[slave].icount = curr_icount;
	      bx_guard_found[slave].guard_found = BX_DBG_GUARD_ICOUNT;
      } else {
	      BX_MEM(slave)->cpu_loop(-1);
      }
      }
    slave_icount += bx_guard_found[slave].icount;
    if (bx_guard_found[slave].guard_found & ~BX_DBG_GUARD_ICOUNT) {
      bail_out = 1;
      // If user type Ctrl-C we're done after synchronizing.  If not,
      // then we have reached a true guard, and it's time to bail.
      if (bx_guard_found[slave].guard_found &
          ~(BX_DBG_GUARD_ICOUNT | BX_DBG_GUARD_CTRL_C))
        break;
      }
    if (bx_debugger.async_journal.size) {
      // sanity check: slave should be at async point
      if (bx_guard_found[slave].icount != async_icount) {
        fprintf(stderr, "Error: comsimulateN: async: slave not at sync point.\n");
        bx_dbg_exit(1);
        }
      switch (bx_debugger.async_journal.element[async_head].what) {
        case BX_DBG_ASYNC_JOURNAL_IAC:
		if (!bx_debugger.fast_forward_mode) {
if (doit)
  fprintf(stderr, "slave: forcing interrupt %u\n",
    bx_debugger.async_journal.element[async_head].u.iac.val);

          BX_MEM(slave)->dbg_force_interrupt(
            bx_debugger.async_journal.element[async_head].u.iac.val);
		}
          break;
        case BX_DBG_ASYNC_JOURNAL_A20:
          bx_pc_system.set_enable_a20(
            bx_debugger.async_journal.element[async_head].u.a20.val);
          if (BX_MEM(slave)->set_A20)
            BX_MEM(slave)->set_A20(
              bx_debugger.async_journal.element[async_head].u.a20.val);
          break;
        case BX_DBG_ASYNC_JOURNAL_NMI:
        case BX_DBG_ASYNC_JOURNAL_RESET:
        default:
          fprintf(stderr, "Error: cosimulateN: unimplemented async event.\n");
        }
      // async event processed, dequeue it
      bx_debugger.async_journal.size--;
      bx_debugger.async_journal.head++;
      }
    } while (slave_icount < master_icount);

  bx_pc_system.INTR = save_INTR; // restore INTR to value after master run

  // At this point, both simulators should be at the same point.  Either
  // they have finished executing for the desired count, or at least for
  // a time quantum.  Check to see if the environments are in sync.
  int iaddr_res;
  if (!bx_debugger.fast_forward_mode) {
	  if (bx_debugger.compare_at_sync.iaddr && (iaddr_res = bx_dbg_compare_sim_iaddr())) {
		  if (iaddr_res == 1)
			  bail_out = 1;
	  } else if (bx_debugger.compare_at_sync.cpu && bx_dbg_compare_sim_cpu())
		  bail_out = 1;
	  else if (bx_debugger.compare_at_sync.memory && bx_dbg_compare_sim_memory())
		  bail_out = 1;
  }

  if (bail_out) {
#ifdef DEBUGGER_ERROR
	  extern void DEBUGGER_ERROR(void);
	  DEBUGGER_ERROR();
#endif

    ret = 0; // didn't complete, stopped
    }
  else {
    count -= master_icount;
    // last icount known to be in sync
    bx_debugger.last_sync_icount += master_icount;
    if (count)
      goto one_time_slice;
    ret = 1; // completed OK
    }

  bx_guard.guard_for &= ~BX_DBG_GUARD_ICOUNT;
  return(ret);
}
#endif  // #if BX_NUM_SIMULATORS >= 2


#if BX_NUM_SIMULATORS >= 2
  int
bx_dbg_compare_sim_iaddr(void)
{
  // returns 0 = same, 1 = different, 2 = false diff
  if (BX_CPU(dbg_cpu)->guard_found.laddr != bx_guard_found[1].laddr) {

#ifdef FALSE_DIFF_DETECT
	  extern int FALSE_DIFF_DETECT();
	  if (FALSE_DIFF_DETECT())
		  return 2;
#endif

    fprintf(stderr,
#if BX_DBG_ICOUNT_SIZE == 32
      "*** Iaddr divergence ***: last know synchronized icount was %lu\n",
      (unsigned long) bx_debugger.last_sync_icount
#else  // BX_DBG_ICOUNT_SIZE == 64
      "*** Iaddr divergence ***: last know synchronized icount was %Lu\n",
      (unsigned long long) bx_debugger.last_sync_icount
#endif
      );

//    fprintf(stderr, "Divergence: sim[0].laddr=%x, sim[1].laddr=%x\n",
//      (unsigned) BX_CPU(dbg_cpu)->guard_found.laddr,
//      (unsigned) bx_guard_found[1].laddr);
    return(1); // different
    }
  return(0); // same
}

  Boolean
bx_dbg_compare_sim_cpu(void)
{
	// (mch) Get cpu structures from both simulators

	// Compare the structures (except the descriptor parts of the
	// segment registers
	bx_dbg_cpu_t regs[2];

	BX_MEM(0)->dbg_get_cpu(regs + 0);
	BX_MEM(1)->dbg_get_cpu(regs + 1);

	Boolean ret = 0;
	Boolean warn = 0;

	// (mch) Yes I know these are macros. The would have been
	// inner functions if g++ had supported it.
#define TEST_REG(reg, reg_name) do { \
        if (regs[0].reg != regs[1].reg) { \
        	printf("COSIM ERROR: [%s] %s: 0x%08x  %s: 0x%08x\n", reg_name, SIM_NAME0, regs[0].reg, SIM_NAME1_STR, regs[1].reg); \
                ret = 1; \
	} \
    } while(0)

#define TEST_REG_WARN(reg, reg_name, mask) do { \
        if ((regs[0].reg & mask) != (regs[1].reg & mask)) { \
        	printf("COSIM WARNING: [%s] %s: 0x%08x  %s: 0x%08x\n", reg_name, SIM_NAME0, (regs[0].reg & mask), SIM_NAME1_STR, (regs[1].reg & mask)); \
                warn = 1; \
	} \
    } while(0)

	TEST_REG(eax, "eax");
	TEST_REG(ebx, "ebx");
	TEST_REG(ecx, "ecx");
	TEST_REG(edx, "edx");
	TEST_REG(ebp, "ebp");
	TEST_REG(esi, "esi");
	TEST_REG(edi, "edi");
	TEST_REG(esp, "esp");
        TEST_REG_WARN(eflags, "eflags & CF", 0x1);
#define EFLAGS_MASK (~((1 << 11) | (1 << 7) | (1 << 6) | (1 << 4) | (1 << 2) | (1 << 0)))
	regs[0].eflags &= EFLAGS_MASK;
	regs[1].eflags &= EFLAGS_MASK;
	TEST_REG(eflags, "eflags");
	TEST_REG(eip, "eip");

#define TEST_SEG_REG(reg, reg_name) do { \
        if (regs[0].reg.sel != regs[1].reg.sel || regs[0].reg.valid != regs[1].reg.valid) { \
        	printf("COSIM ERROR: [%s] %s: 0x%04x (%d)  %s: 0x%04x (%d)\n", reg_name, SIM_NAME0, regs[0].reg.sel, regs[0].reg.valid, SIM_NAME1_STR, regs[1].reg.sel, regs[1].reg.valid); \
                ret = 1; \
	} \
    } while(0)

	TEST_SEG_REG(cs, "cs");
	TEST_SEG_REG(ss, "ss");
	TEST_SEG_REG(ds, "ds");
	TEST_SEG_REG(es, "es");
	TEST_SEG_REG(fs, "fs");
	TEST_SEG_REG(gs, "gs");
	TEST_SEG_REG(ldtr, "ldtr");
	TEST_SEG_REG(tr, "tr");

	if (regs[0].gdtr.base != regs[1].gdtr.base || regs[0].gdtr.limit != regs[1].gdtr.limit) {
		printf("COSIM ERROR: [gdtr] %s: 0x%08x:0x%04x  %s 0x%08x:0x%04x\n",
		       SIM_NAME0, regs[0].gdtr.base, regs[0].gdtr.limit, SIM_NAME1_STR, regs[1].gdtr.base, regs[1].gdtr.limit);
		ret = 1;
	}
	if (regs[0].idtr.base != regs[1].idtr.base || regs[0].idtr.limit != regs[1].idtr.limit) {
		printf("COSIM ERROR: [idtr] %s: 0x%08x:0x%04x  %s 0x%08x:0x%04x\n",
		       SIM_NAME0, regs[0].idtr.base, regs[0].idtr.limit, SIM_NAME1_STR, regs[1].idtr.base, regs[1].idtr.limit);
		ret = 1;
	}

	// drX ignored
	// trX ignored

	TEST_REG(cr0, "cr0");
	TEST_REG(cr1, "cr1");
	TEST_REG(cr2, "cr2");
	TEST_REG(cr3, "cr3");
	TEST_REG(cr4, "cr4");

	if (regs[0].inhibit_mask != regs[1].inhibit_mask) {
		printf("COSIM ERROR [inhibit_mask] %s: %d  %s: %d\n",
		       SIM_NAME0, regs[0].inhibit_mask, SIM_NAME1_STR, regs[1].inhibit_mask);
		ret = 1;
	}

	if (ret) {
		fprintf(stderr,
#if BX_DBG_ICOUNT_SIZE == 32
			"*** CPU divergence ***: last know synchronized icount was %lu\n",
			(unsigned long) bx_debugger.last_sync_icount
#else  // BX_DBG_ICOUNT_SIZE == 64
			"*** CPU divergence ***: last know synchronized icount was %Lu\n",
			(unsigned long long) bx_debugger.last_sync_icount
#endif
			);
	} else if (warn) {
		fprintf(stderr,
#if BX_DBG_ICOUNT_SIZE == 32
			"=== CPU divergence ===: last know synchronized icount was %lu\n",
			(unsigned long) bx_debugger.last_sync_icount
#else  // BX_DBG_ICOUNT_SIZE == 64
			"=== CPU divergence ===: last know synchronized icount was %Lu\n",
			(unsigned long long) bx_debugger.last_sync_icount
#endif
			);
#ifdef DEBUGGER_ERROR
		extern void DEBUGGER_ERROR(void);
		DEBUGGER_ERROR();
#endif
	}

	return ret;
}

  void
clear_dirty_bits (void)
{
	int num_pages = bx_options.memory.Osize->get () * 1024 / 4;
	for (int i = 0; i < num_pages; i++) {
		BX_MEM(0)->dbg_dirty_pages[i] = 0;
		BX_MEM(1)->dbg_dirty_pages[i] = 0;
	}
}

Boolean always_check_page[128 * 1024 / 4];

void
bx_dbg_always_check(Bit32u page_start, Boolean on)
{
	always_check_page[page_start / (4 * 1024)] = on;
	printf("Forced check on page %08x %s\n",
	       page_start, on ? "enabled" : "disabled");
}

  Boolean
bx_dbg_compare_sim_memory(void)
{
	Boolean ret = 0;
	int num_pages = bx_options.memory.Osize->get () * 1024 / 4;

	for (int i = 0; i < num_pages; i++) {
		Boolean sim0_dirty = BX_MEM(0)->dbg_dirty_pages[i];
		Boolean sim1_dirty = BX_MEM(1)->dbg_dirty_pages[i];
		Bit32u page_start = i * 1024 * 4;

		if ((sim0_dirty != sim1_dirty) || sim0_dirty || always_check_page[i]) {
			// Page has been written, compare
			// (mch) I'm quite aware of how hackish this is. I don't care.
			extern Bit8u* SIM1_GET_PHYS_PTR(Bit32u page_start);
			Bit8u* sim0_page_vec = bx_mem0.vector + page_start;
			Bit8u* sim1_page_vec = SIM1_GET_PHYS_PTR(page_start);

			if (memcmp(sim0_page_vec, sim1_page_vec, 1024 * 4)) {
				printf("COSIM ERROR  Physical page %08x differs in content\n", page_start);
				for (int j = 0; j < 1024 * 4; j++) {
					if (sim0_page_vec[j] != sim1_page_vec[j]) {
						printf("%08x   %s: %02x  %s: %02x\n",
						       page_start+j, SIM_NAME0, sim0_page_vec[j], SIM_NAME1_STR, sim1_page_vec[j]);
					}
				}
				ret = 1;
			}
		}
	}

	if (ret) {
		fprintf(stderr,
#if BX_DBG_ICOUNT_SIZE == 32
			"*** Memory divergence ***: last know synchronized icount was %lu\n",
			(unsigned long) bx_debugger.last_sync_icount
#else  // BX_DBG_ICOUNT_SIZE == 64
			"*** Memory divergence ***: last know synchronized icount was %Lu\n",
			(unsigned long long) bx_debugger.last_sync_icount
#endif
			);
	}

	clear_dirty_bits();

	return ret;
}
#endif


void bx_dbg_disassemble_current (int which_cpu, int print_time)
{
  Bit32u phy;
  Boolean valid;

  if (which_cpu < 0) {
    // iterate over all of them.
    for (int i=0; i<BX_NUM_SIMULATORS; i++)
      bx_dbg_disassemble_current (i, print_time);
    return;
  }

  BX_CPU(which_cpu)->dbg_xlate_linear2phy(BX_CPU(which_cpu)->guard_found.laddr, &phy, &valid);

  if (valid) {
    unsigned ilen;

    BX_CPU(which_cpu)->mem->dbg_fetch_mem(phy, 16, bx_disasm_ibuf);
    ilen = bx_disassemble.disasm(BX_CPU(which_cpu)->guard_found.is_32bit_code,
				 bx_disasm_ibuf, bx_disasm_tbuf);

    // Note: it would be nice to display only the modified registers here, the easy
    // way out I have thought of would be to keep a prev_eax, prev_ebx, etc copies
    // in each cpu description (see cpu/cpu.h) and update/compare those "prev" values
    // from here. (eks)
    if( BX_CPU(dbg_cpu)->trace_reg )
	    fprintf( stderr,
		"eax: %08X\tecx: %08X\tedx: %08X\tebx: %08X\tesp: %08X\tebp: %08X\tesi: %08X\tedi: %08X\ncf=%u af=%u zf=%u sf=%u of=%u pf=%u tf=%u if=%u df=%u iopl=%u nt=%u rf=%u vm=%u\n",
		BX_CPU(which_cpu)->gen_reg[0],
		BX_CPU(which_cpu)->gen_reg[1],
		BX_CPU(which_cpu)->gen_reg[2],
		BX_CPU(which_cpu)->gen_reg[3],
		BX_CPU(which_cpu)->gen_reg[4],
		BX_CPU(which_cpu)->gen_reg[5],
		BX_CPU(which_cpu)->gen_reg[6],
		BX_CPU(which_cpu)->gen_reg[7],
		!!BX_CPU(which_cpu)->get_CF(),
		!!BX_CPU(which_cpu)->get_AF(),
		!!BX_CPU(which_cpu)->get_ZF(),
		!!BX_CPU(which_cpu)->get_SF(),
		!!BX_CPU(which_cpu)->get_OF(),
		!!BX_CPU(which_cpu)->get_PF(),
		BX_CPU(which_cpu)->eflags.tf,
		BX_CPU(which_cpu)->eflags.if_,
		BX_CPU(which_cpu)->eflags.df,
		BX_CPU(which_cpu)->eflags.iopl,
		BX_CPU(which_cpu)->eflags.nt,
		BX_CPU(which_cpu)->eflags.rf,
		BX_CPU(which_cpu)->eflags.vm
		);

    if (print_time)
      fprintf (stderr, "(%u).[%lld] ", which_cpu, bx_pc_system.time_ticks());
    else
      fprintf (stderr, "(%u) ", which_cpu);
    if (BX_CPU(which_cpu)->guard_found.is_32bit_code) {
      fprintf(stderr, "%04x:%08x (%s): ", 
	      (unsigned) BX_CPU(which_cpu)->guard_found.cs,
	      (unsigned) BX_CPU(which_cpu)->guard_found.eip,
	      bx_dbg_symbolic_address((BX_CPU(which_cpu)->cr3) >> 12, BX_CPU(which_cpu)->guard_found.eip, BX_CPU(which_cpu)->sregs[BX_SREG_CS].cache.u.segment.base));
      }
    else {
      fprintf(stderr, "%04x:%04x: ", 
	      (unsigned) BX_CPU(which_cpu)->guard_found.cs,
	      (unsigned) BX_CPU(which_cpu)->guard_found.eip);
      }
    for (unsigned j=0; j<ilen; j++)
      fprintf(stderr, "%02x", (unsigned) bx_disasm_ibuf[j]);
    fprintf(stderr, ": %s\n", bx_disasm_tbuf);

    
    }
  else {
    fprintf(stderr, "(%u).[%lld] ??? (physical address not available)\n", which_cpu, bx_pc_system.time_ticks());
  }
}

  void
bx_dbg_print_guard_results(void)
{
  unsigned i;
  unsigned sim;

for (sim=0; sim<BX_SMP_PROCESSORS; sim++) {
	unsigned long found = BX_CPU(sim)->guard_found.guard_found;
  if (found & BX_DBG_GUARD_ICOUNT) {
    }
  else if (found & BX_DBG_GUARD_CTRL_C) {
    }
#if BX_DBG_SUPPORT_VIR_BPOINT
  else if (found & BX_DBG_GUARD_IADDR_VIR) {
    i = BX_CPU(sim)->guard_found.iaddr_index;
    fprintf(stderr, "(%u) Breakpoint %u, 0x%x (0x%x:0x%x)\n",
            sim,
            bx_guard.iaddr.vir[i].bpoint_id,
            BX_CPU(sim)->guard_found.laddr,
            BX_CPU(sim)->guard_found.cs,
            BX_CPU(sim)->guard_found.eip);
    }
#endif
#if BX_DBG_SUPPORT_LIN_BPOINT
  else if (found & BX_DBG_GUARD_IADDR_LIN) {
    i = BX_CPU(sim)->guard_found.iaddr_index;
    fprintf(stderr, "(%u) Breakpoint %u, 0x%x in ?? ()\n",
            sim,
            bx_guard.iaddr.lin[i].bpoint_id,
            BX_CPU(sim)->guard_found.laddr);
    }
#endif
#if BX_DBG_SUPPORT_PHY_BPOINT
  else if (found & BX_DBG_GUARD_IADDR_PHY) {
    i = BX_CPU(sim)->guard_found.iaddr_index;
    fprintf(stderr, "(%u) Breakpoint %u, 0x%x in ?? ()\n",
            sim,
            bx_guard.iaddr.phy[i].bpoint_id,
            BX_CPU(sim)->guard_found.laddr);
    }
#endif
	else if (BX_CPU(sim)->stop_reason == STOP_CPU_HALTED) {
		/* returned early because processor is in halt state */
	}
  else if (BX_CPU(sim)->stop_reason == STOP_MAGIC_BREAK_POINT) {
	fprintf(stderr, "(%u) Magic breakpoint\n", sim);
  } else if (BX_CPU(sim)->stop_reason == STOP_TIME_BREAK_POINT) {
	fprintf(stderr, "(%u) Caught time breakpoint\n", sim);
  } else if (BX_CPU(sim)->stop_reason == STOP_MODE_BREAK_POINT) {
	fprintf(stderr, "(%u) Caught vm mode switch breakpoint to %s mode\n",
		sim, BX_CPU(sim)->eflags.vm ? "virtual 86" : "protected");
  } else if (BX_CPU(sim)->stop_reason == STOP_READ_WATCH_POINT) {
	fprintf(stderr, "(%u) Caught read watch point\n", sim);
  } else if (BX_CPU(sim)->stop_reason == STOP_WRITE_WATCH_POINT) {
	fprintf(stderr, "(%u) Caught write watch point\n", sim);
  }
  else {
    fprintf(stderr, "Error: (%u) print_guard_results: guard_found ? (stop reason %u)\n", 
	    sim, BX_CPU(sim)->stop_reason);
    }

#if BX_DISASM
  if (bx_debugger.auto_disassemble) {
    fprintf (stderr, "Next at t=%lld\n", bx_pc_system.time_ticks ());
    bx_dbg_disassemble_current (sim, 0);  // one cpu, don't print time
  }
#endif  // #if BX_DISASM
  }
}


  void
bx_dbg_breakpoint_changed(void)
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

  void
bx_dbg_del_breakpoint_command(unsigned handle)
{
  unsigned i;

#if BX_DBG_SUPPORT_VIR_BPOINT
  // see if breakpoint is a virtual breakpoint
  for (i=0; i<bx_guard.iaddr.num_virtual; i++) {
    if (bx_guard.iaddr.vir[i].bpoint_id == handle) {
      // found breakpoint, delete it by shifting remaining entries left
      for (unsigned j=i; j<(bx_guard.iaddr.num_virtual-1); j++) {
        bx_guard.iaddr.vir[j] = bx_guard.iaddr.vir[j+1];
        }
      bx_guard.iaddr.num_virtual--;
      goto done;
      }
    }
#endif

#if BX_DBG_SUPPORT_LIN_BPOINT
  // see if breakpoint is a linear breakpoint
  for (i=0; i<bx_guard.iaddr.num_linear; i++) {
    if (bx_guard.iaddr.lin[i].bpoint_id == handle) {
      // found breakpoint, delete it by shifting remaining entries left
      for (unsigned j=i; j<(bx_guard.iaddr.num_linear-1); j++) {
        bx_guard.iaddr.lin[j] = bx_guard.iaddr.lin[j+1];
        }
      bx_guard.iaddr.num_linear--;
      goto done;
      }
    }
#endif

#if BX_DBG_SUPPORT_PHY_BPOINT
  // see if breakpoint is a physical breakpoint
  for (i=0; i<bx_guard.iaddr.num_physical; i++) {
    if (bx_guard.iaddr.phy[i].bpoint_id == handle) {
      // found breakpoint, delete it by shifting remaining entries left
      for (unsigned j=i; j<(bx_guard.iaddr.num_physical-1); j++) {
        bx_guard.iaddr.phy[j] = bx_guard.iaddr.phy[j+1];
        }
      bx_guard.iaddr.num_physical--;
      goto done;
      }
    }
#endif

  fprintf(stderr, "Error: breakpoint %u not found.\n", handle);
  return;

done:
  bx_dbg_breakpoint_changed();
}

  void
bx_dbg_vbreakpoint_command(Boolean specific, Bit32u cs, Bit32u eip)
{
#if BX_DBG_SUPPORT_VIR_BPOINT
  if (specific == 0) {
    fprintf(stderr, "Error: vbreak without address not implemented yet.\n");
    return;
    }

  if (bx_guard.iaddr.num_virtual >= BX_DBG_MAX_VIR_BPOINTS) {
    fprintf(stderr, "Error: no more virtual breakpoint slots left.\n");
    fprintf(stderr, "Error: see BX_DBG_MAX_VIR_BPOINTS.\n");
    return;
    }

  bx_guard.iaddr.vir[bx_guard.iaddr.num_virtual].cs  = cs;
  bx_guard.iaddr.vir[bx_guard.iaddr.num_virtual].eip = eip;
  bx_guard.iaddr.vir[bx_guard.iaddr.num_virtual].bpoint_id = bx_debugger.next_bpoint_id++;
  bx_guard.iaddr.num_virtual++;
  bx_guard.guard_for |= BX_DBG_GUARD_IADDR_VIR;

#else
  fprintf(stderr, "Error: virtual breakpoint support not compiled in.\n");
  fprintf(stderr, "Error: see BX_DBG_SUPPORT_VIR_BPOINT.\n");
#endif
}

  void
bx_dbg_lbreakpoint_command(Boolean specific, Bit32u laddress)
{
#if BX_DBG_SUPPORT_LIN_BPOINT
  if (specific == 0) {
    fprintf(stderr, "Error: lbreak without address not implemented yet.\n");
    return;
    }

  if (bx_guard.iaddr.num_linear >= BX_DBG_MAX_LIN_BPOINTS) {
    fprintf(stderr, "Error: no more linear breakpoint slots left.\n");
    fprintf(stderr, "Error: see BX_DBG_MAX_LIN_BPOINTS.\n");
    return;
    }

  bx_guard.iaddr.lin[bx_guard.iaddr.num_linear].addr = laddress;
  bx_guard.iaddr.lin[bx_guard.iaddr.num_linear].bpoint_id = bx_debugger.next_bpoint_id++;
  bx_guard.iaddr.num_linear++;
  bx_guard.guard_for |= BX_DBG_GUARD_IADDR_LIN;

#else
  fprintf(stderr, "Error: linear breakpoint support not compiled in.\n");
  fprintf(stderr, "Error: see BX_DBG_SUPPORT_LIN_BPOINT.\n");
#endif
}

  void
bx_dbg_pbreakpoint_command(Boolean specific, Bit32u paddress)
{
#if BX_DBG_SUPPORT_PHY_BPOINT
  if (specific == 0) {
    fprintf(stderr, "Error: pbreak without address not implemented yet.\n");
    return;
    }

  if (bx_guard.iaddr.num_physical >= BX_DBG_MAX_PHY_BPOINTS) {
    fprintf(stderr, "Error: no more physical breakpoint slots left.\n");
    fprintf(stderr, "Error: see BX_DBG_MAX_PHY_BPOINTS.\n");
    return;
    }

  bx_guard.iaddr.phy[bx_guard.iaddr.num_physical].addr = paddress;
  bx_guard.iaddr.phy[bx_guard.iaddr.num_physical].bpoint_id = bx_debugger.next_bpoint_id++;
  bx_guard.iaddr.num_physical++;
  bx_guard.guard_for |= BX_DBG_GUARD_IADDR_PHY;

#else
  fprintf(stderr, "Error: physical breakpoint support not compiled in.\n");
  fprintf(stderr, "Error: see BX_DBG_SUPPORT_PHY_BPOINT.\n");
#endif
}

  void
bx_dbg_info_bpoints_command(void)
{
  unsigned i;
// Num Type           Disp Enb Address    What
// 1   breakpoint     keep y   0x00010664 in main at temp.c:7

  fprintf(stderr, "Num Type           Disp Enb Address\n");
#if BX_DBG_SUPPORT_VIR_BPOINT
  for (i=0; i<bx_guard.iaddr.num_virtual; i++) {
    fprintf(stderr, "%3u ", bx_guard.iaddr.vir[i].bpoint_id);
    fprintf(stderr, "vbreakpoint    ");
    fprintf(stderr, "keep ");
    fprintf(stderr, "y   ");
    fprintf(stderr, "0x%04x:0x%08x\n",
                  bx_guard.iaddr.vir[i].cs,
                  bx_guard.iaddr.vir[i].eip);
    }
#endif

#if BX_DBG_SUPPORT_LIN_BPOINT
  for (i=0; i<bx_guard.iaddr.num_linear; i++) {
    fprintf(stderr, "%3u ", bx_guard.iaddr.lin[i].bpoint_id);
    fprintf(stderr, "lbreakpoint    ");
    fprintf(stderr, "keep ");
    fprintf(stderr, "y   ");
    fprintf(stderr, "0x%08x\n",
                  bx_guard.iaddr.lin[i].addr);
    }
#endif

#if BX_DBG_SUPPORT_PHY_BPOINT
  for (i=0; i<bx_guard.iaddr.num_physical; i++) {
    fprintf(stderr, "%3u ", bx_guard.iaddr.phy[i].bpoint_id);
    fprintf(stderr, "pbreakpoint    ");
    fprintf(stderr, "keep ");
    fprintf(stderr, "y   ");
    fprintf(stderr, "0x%08x\n",
                  bx_guard.iaddr.phy[i].addr);
    }
#endif
}


  void
bx_dbg_set_command(char *p1, char *p2, char *p3)
{
  fprintf(stderr, "Error: %s %s %s: command 'set' not implemented yet.\n",
    p1, p2, p3);
}

  void
bx_dbg_take_command(char *what, unsigned n)
{
  if ( !strcmp(what, "dma") ) {
    unsigned i;
    if (n == 0) {
      fprintf(stderr, "Error: take what n=0.\n");
      return;
      }
    bx_dbg_post_dma_reports(); // in case there's some pending reports
    bx_dbg_batch_dma.this_many = n;

    for (i=0; i<n; i++) {
      BX_CPU(0)->dbg_take_dma();
      }

    bx_dbg_batch_dma.this_many = 1;  // reset to normal
    bx_dbg_post_dma_reports(); // print reports and flush
    if (bx_guard.report.dma)
      fprintf(stderr, "done\n");
    }
  else if ( !strcmp(what, "irq") ) {
    BX_CPU(0)->dbg_take_irq();

    if (bx_guard.report.irq)
      fprintf(stderr, "done\n");
    }
  else {
    fprintf(stderr, "Error: Take '%s' not understood.\n", what);
    }
}


  void
bx_dbg_info_registers_command(int which_regs_mask)
{
  Bit32u reg;
  bx_dbg_cpu_t cpu;

  for (unsigned i=0; i<BX_SMP_PROCESSORS; i++) {
    if (which_regs_mask & BX_INFO_CPU_REGS) {
      memset(&cpu, 0, sizeof(cpu));
      BX_CPU(i)->dbg_get_cpu(&cpu);

#if (BX_SMP_PROCESSORS > 1)
      fprintf(stderr, "%s:\n", BX_CPU(i)->name, i);
#endif
      reg = cpu.eax;
      fprintf(stderr, "eax            0x%-8x\t%d\n", (unsigned) reg, (int) reg);
      reg = cpu.ecx;
      fprintf(stderr, "ecx            0x%-8x\t%d\n", (unsigned) reg, (int) reg);
      reg = cpu.edx;
      fprintf(stderr, "edx            0x%-8x\t%d\n", (unsigned) reg, (int) reg);
      reg = cpu.ebx;
      fprintf(stderr, "ebx            0x%-8x\t%d\n", (unsigned) reg, (int) reg);

      reg = cpu.esp;
      fprintf(stderr, "esp            0x%-8x\t0x%-8x\n", (unsigned) reg, (int) reg);
      reg = cpu.ebp;
      fprintf(stderr, "ebp            0x%-8x\t0x%-8x\n", (unsigned) reg, (int) reg);
      reg = cpu.esi;
      fprintf(stderr, "esi            0x%-8x\t%d\n", (unsigned) reg, (int) reg);
      reg = cpu.edi;
      fprintf(stderr, "edi            0x%-8x\t%d\n", (unsigned) reg, (int) reg);

      reg = cpu.eip;
      fprintf(stderr, "eip            0x%-8x\t0x%-8x\n", (unsigned) reg, (int) reg);

      reg = cpu.eflags;
      fprintf(stderr, "eflags         0x%-8x\t%d\n", (unsigned) reg, (int) reg);

      reg = cpu.cs.sel;
      fprintf(stderr, "cs             0x%-8x\t%d\n", (unsigned) reg, (int) reg);
      reg = cpu.ss.sel;
      fprintf(stderr, "ss             0x%-8x\t%d\n", (unsigned) reg, (int) reg);
      reg = cpu.ds.sel;
      fprintf(stderr, "ds             0x%-8x\t%d\n", (unsigned) reg, (int) reg);
      reg = cpu.es.sel;
      fprintf(stderr, "es             0x%-8x\t%d\n", (unsigned) reg, (int) reg);
      reg = cpu.fs.sel;
      fprintf(stderr, "fs             0x%-8x\t%d\n", (unsigned) reg, (int) reg);
      reg = cpu.gs.sel;
      fprintf(stderr, "gs             0x%-8x\t%d\n", (unsigned) reg, (int) reg);
    }
    if (which_regs_mask & BX_INFO_FPU_REGS) {
      BX_CPU(i)->fpu_print_regs ();
    }
  }
}

  void
bx_dbg_info_program_command(void)
{
  fprintf(stderr, "        Using the running image of child process -1.\n");
  fprintf(stderr, "Program stopped at 0x0.\n");
  fprintf(stderr, "It stopped at breakpoint 0.\n");
}


  void
bx_dbg_dump_cpu_command(void)
{
  bx_dbg_cpu_t cpu;

  for (unsigned i=0; i<BX_SMP_PROCESSORS; i++ ) {
    BX_CPU(i)->dbg_get_cpu(&cpu);

#if (BX_SMP_PROCESSORS >= 2)
    fprintf(stderr, "CPU#%u\n", i);
#endif
    fprintf(stderr, "eax:0x%x\n", (unsigned) cpu.eax);
    fprintf(stderr, "ebx:0x%x\n", (unsigned) cpu.ebx);
    fprintf(stderr, "ecx:0x%x\n", (unsigned) cpu.ecx);
    fprintf(stderr, "edx:0x%x\n", (unsigned) cpu.edx);

    fprintf(stderr, "ebp:0x%x\n", (unsigned) cpu.ebp);
    fprintf(stderr, "esi:0x%x\n", (unsigned) cpu.esi);
    fprintf(stderr, "edi:0x%x\n", (unsigned) cpu.edi);
    fprintf(stderr, "esp:0x%x\n", (unsigned) cpu.esp);

    fprintf(stderr, "eflags:0x%x\n", (unsigned) cpu.eflags);
    fprintf(stderr, "eip:0x%x\n", (unsigned) cpu.eip);

    fprintf(stderr, "cs:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
      (unsigned) cpu.cs.sel, (unsigned) cpu.cs.des_l,
      (unsigned) cpu.cs.des_h, (unsigned) cpu.cs.valid);

    fprintf(stderr, "ss:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
      (unsigned) cpu.ss.sel, (unsigned) cpu.ss.des_l,
      (unsigned) cpu.ss.des_h, (unsigned) cpu.ss.valid);

    fprintf(stderr, "ds:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
      (unsigned) cpu.ds.sel, (unsigned) cpu.ds.des_l,
      (unsigned) cpu.ds.des_h, (unsigned) cpu.ds.valid);

    fprintf(stderr, "es:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
      (unsigned) cpu.es.sel, (unsigned) cpu.es.des_l,
      (unsigned) cpu.es.des_h, (unsigned) cpu.es.valid);

    fprintf(stderr, "fs:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
      (unsigned) cpu.fs.sel, (unsigned) cpu.fs.des_l,
      (unsigned) cpu.fs.des_h, (unsigned) cpu.fs.valid);

    fprintf(stderr, "gs:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
      (unsigned) cpu.gs.sel, (unsigned) cpu.gs.des_l,
      (unsigned) cpu.gs.des_h, (unsigned) cpu.gs.valid);

    fprintf(stderr, "ldtr:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
      (unsigned) cpu.ldtr.sel, (unsigned) cpu.ldtr.des_l,
      (unsigned) cpu.ldtr.des_h, (unsigned) cpu.ldtr.valid);

    fprintf(stderr, "tr:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
      (unsigned) cpu.tr.sel, (unsigned) cpu.tr.des_l,
      (unsigned) cpu.tr.des_h, (unsigned) cpu.tr.valid);

    fprintf(stderr, "gdtr:base=0x%x, limit=0x%x\n",
      (unsigned) cpu.gdtr.base, (unsigned) cpu.gdtr.limit);

    fprintf(stderr, "idtr:base=0x%x, limit=0x%x\n",
      (unsigned) cpu.idtr.base, (unsigned) cpu.idtr.limit);

    fprintf(stderr, "dr0:0x%x\n", (unsigned) cpu.dr0);
    fprintf(stderr, "dr1:0x%x\n", (unsigned) cpu.dr1);
    fprintf(stderr, "dr2:0x%x\n", (unsigned) cpu.dr2);
    fprintf(stderr, "dr3:0x%x\n", (unsigned) cpu.dr3);
    fprintf(stderr, "dr6:0x%x\n", (unsigned) cpu.dr6);
    fprintf(stderr, "dr7:0x%x\n", (unsigned) cpu.dr7);

    fprintf(stderr, "tr3:0x%x\n", (unsigned) cpu.tr3);
    fprintf(stderr, "tr4:0x%x\n", (unsigned) cpu.tr4);
    fprintf(stderr, "tr5:0x%x\n", (unsigned) cpu.tr5);
    fprintf(stderr, "tr6:0x%x\n", (unsigned) cpu.tr6);
    fprintf(stderr, "tr7:0x%x\n", (unsigned) cpu.tr7);

    fprintf(stderr, "cr0:0x%x\n", (unsigned) cpu.cr0);
    fprintf(stderr, "cr1:0x%x\n", (unsigned) cpu.cr1);
    fprintf(stderr, "cr2:0x%x\n", (unsigned) cpu.cr2);
    fprintf(stderr, "cr3:0x%x\n", (unsigned) cpu.cr3);
    fprintf(stderr, "cr4:0x%x\n", (unsigned) cpu.cr4);

    fprintf(stderr, "inhibit_mask:%u\n", cpu.inhibit_mask);
    }

#if BX_PCI_SUPPORT
  if (bx_options.Oi440FXSupport->get ()) {
    bx_devices.pci->print_i440fx_state();
    }
#endif

  fprintf(stderr, "done\n");
}


  void
bx_dbg_examine_command(char *command, char *format, Boolean format_passed,
               Bit32u addr, Boolean addr_passed, int simulator)
{
  unsigned repeat_count, i;
  char ch, display_format, unit_size;
  Boolean iteration;
  unsigned data_size;
  Boolean paddr_valid;
  Bit32u  paddr;
  Bit8u   data8;
  Bit16u  data16;
  Bit32u  data32;
  unsigned columns, per_line, offset;
  unsigned char digit;
  unsigned biti;
  Boolean is_linear;
  unsigned char databuf[8];

  if (simulator == 0)
	  printf("[%s]:\n", SIM_NAME0);
  else
	  printf("[%s]:\n", SIM_NAME1_STR);

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
      fprintf(stderr, "dbg_examine: format NULL\n");
      bx_dbg_exit(1);
      }

    if (strlen(format) < 2) {
      fprintf(stderr, "dbg_examine: invalid format passed.\n");
      bx_dbg_exit(1);
      }

    if (format[0] != '/') {
      fprintf(stderr, "dbg_examine: '/' is not first char of format.\n");
      bx_dbg_exit(1);
      }

    format++;
    repeat_count = 0;
    ch = *format;
    iteration = 0;

    while ( (ch>='0') && (ch<='9') ) {
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
      fprintf(stderr, "dbg_examine: repeat count given but is zero.\n");
      return;
      }


    // set up the default display format and unit size parameters
    display_format = bx_debugger.default_display_format;
    unit_size      = bx_debugger.default_unit_size;

    for (i=0; i<=1; i++) {
      if (ch==0) break; // bail on null character
      switch (ch) {
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
        default:
          fprintf(stderr, "dbg_examine: invalid format passed.\n");
          bx_dbg_exit(1);
          break;
        }
      format++;
      ch = *format;
      }

    // store current options as default
    bx_debugger.default_display_format = display_format;
    bx_debugger.default_unit_size      = unit_size;
    }

  //fprintf(stderr, "  repeat count was %u\n", repeat_count);
  //fprintf(stderr, "  display_format = '%c'\n", display_format);
  //fprintf(stderr, "  unit_size      = '%c'\n", unit_size);

  if ( (display_format == 'i') || (display_format == 's') ) {
    fprintf(stderr, "error: dbg_examine: 'i' and 's' formats not supported.\n");
    return;
    }

  if (unit_size == 'g') {
    fprintf(stderr, "error: dbg_examine: 'g' (8-byte) unit size not supported.\n");
    return;
    }

  data_size = 0;
  per_line  = 0;
  offset = 0;

  switch (unit_size) {
    case 'b': data_size = 1; per_line = 8; break;
    case 'h': data_size = 2; per_line = 8; break;
    case 'w': data_size = 4; per_line = 4; break;
    //case 'g': data_size = 8; per_line = 2; break;
    }

  columns = per_line + 1; // set current number columns past limit

  for (i=1; i<=repeat_count; i++) {

    if (columns > per_line) {
      // if not 1st run, need a newline from last line
      if (i!=1)
        fprintf(stderr, "\n");
      fprintf(stderr, "0x%x <bogus+%u>:", addr, offset);
      columns = 1;
      }

    if (is_linear) {
      BX_CPU(simulator)->dbg_xlate_linear2phy(addr, &paddr, &paddr_valid);
      if (!paddr_valid) {
        fprintf(stderr, "error: examine memory: no tranlation for linear-to-phy mem available.\n");
        return;
        }
      }
    else {
      paddr = addr;  // address is already physical address
      }

    BX_MEM(simulator)->dbg_fetch_mem(paddr, data_size, databuf);
    //FIXME HanishKVC The char display for data to be properly integrated
    //      so that repeat_count, columns, etc. can be set or used properly.
    //      Also for data_size of 2 and 4 how to display the individual
    //      characters i.e in which order to be decided.
    switch (data_size) {
      case 1:
        data8 = databuf[0];
        switch (display_format) {
          case 'x': fprintf(stderr, "\t0x%02x", (unsigned) data8); break;
          case 'd': fprintf(stderr, "\t%d", (int) (Bit8s) data8); break;
          case 'u': fprintf(stderr, "\t%u", (unsigned) data8); break;
          case 'o': fprintf(stderr, "\t%o", (unsigned) data8); break;
          case 't':
            fputc('\t', stderr);
            for (biti=7; ; biti--) {
              digit = (data8 >> biti) & 0x01;
              fputc(digit + '0', stderr);
              if (biti==0) break;
              }
            break;
	  case 'c': fprintf(stderr, "  %c",data8); break;
          }
        break;

      case 2:
#ifdef BX_LITTLE_ENDIAN
        data16 = * (Bit16u *) databuf;
#else
        data16 = (databuf[1]<<8)  |  databuf[0];
#endif
        switch (display_format) {
          case 'x': fprintf(stderr, "\t0x%04x", (unsigned) data16); break;
          case 'd': fprintf(stderr, "\t%d", (int) (Bit16s) data16); break;
          case 'u': fprintf(stderr, "\t%u", (unsigned) data16); break;
          case 'o': fprintf(stderr, "\t%o", (unsigned) data16); break;
          case 't':
            fputc('\t', stderr);
            for (biti=15; ; biti--) {
              digit = (data16 >> biti) & 0x01;
              fputc(digit + '0', stderr);
              if (biti==0) break;
              }
            break;
	  case 'c': fprintf(stderr, "  %c  %c",data16>>8,data16&0xff); break;
          }
        break;

      case 4:
#ifdef BX_LITTLE_ENDIAN
        data32 = * (Bit32u *) databuf;
#else
        data32 = (databuf[3]<<24) | (databuf[2]<<16) |
                 (databuf[1]<<8)  |  databuf[0];
#endif
        switch (display_format) {
          case 'x': fprintf(stderr, "\t0x%08x", (unsigned) data32); break;
          case 'd': fprintf(stderr, "\t%d", (int) (Bit32s) data32); break;
          case 'u': fprintf(stderr, "\t%u", (unsigned) data32); break;
          case 'o': fprintf(stderr, "\t%o", (unsigned) data32); break;
          case 't':
            fputc('\t', stderr);
            for (biti=31; ; biti--) {
              digit = (data32 >> biti) & 0x01;
              fputc(digit + '0', stderr);
              if (biti==0) break;
              }
            break;
	  case 'c': 
	    fprintf(stderr, "  %c  %c",data32>>24,(data32>>16)&0xff); 
	    fprintf(stderr, "  %c  %c",(data32>>8)&0xff,data32&0xff); 
	    break;
          }
        break;
      }

    addr += data_size;
    bx_debugger.default_addr = addr;
    columns++;
    offset += data_size;
    }
  fprintf(stderr, "\n");
}

  void
bx_dbg_setpmem_command(Bit32u addr, unsigned len, Bit32u val)
{
  Boolean is_OK;
  Bit8u   buf[4];

  switch ( len ) {
    case 1:
      buf[0] = (Bit8u) val;
      break;
    case 2:
      buf[0] = val & 0xff;
      buf[1] = (val>>8) & 0xff;
      break;
    case 4:
      buf[0] = val & 0xff; val >>= 8;
      buf[1] = val & 0xff; val >>= 8;
      buf[2] = val & 0xff; val >>= 8;
      buf[3] = val & 0xff;
      break;
    default:
      fprintf(stderr, "Error: setpmem: bad length value = %u\n", len);
      return;
    }

  is_OK = BX_MEM(0)->dbg_set_mem(addr, len, buf);
  if (!is_OK) {
    fprintf(stderr, "Error: setpmem: could not set memory, out of physical bounds?\n");
    }
}

  void
bx_dbg_set_symbol_command(char *symbol, Bit32u val)
{
  Boolean is_OK;
  symbol++; // get past '$'

  if ( !strcmp(symbol, "eax") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_EAX, val);
    }
  else if ( !strcmp(symbol, "ecx") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_ECX, val);
    }
  else if ( !strcmp(symbol, "edx") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_EDX, val);
    }
  else if ( !strcmp(symbol, "ebx") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_EBX, val);
    }
  else if ( !strcmp(symbol, "esp") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_ESP, val);
    }
  else if ( !strcmp(symbol, "ebp") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_EBP, val);
    }
  else if ( !strcmp(symbol, "esi") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_ESI, val);
    }
  else if ( !strcmp(symbol, "edi") ) {
    is_OK = BX_CPU(dbg_cpu)->dbg_set_reg(BX_DBG_REG_EDI, val);
    }
  else if ( !strcmp(symbol, "eip") ) {
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
#if ((BX_SMP_PROCESSORS>1) && (BX_SUPPORT_APIC))
      if ((val > BX_SMP_PROCESSORS) 
	  || (val >= APIC_MAX_ID) 
	  || (apic_index[val] == NULL)) {
        fprintf (stderr, "invalid cpu id number %d\n", val);
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
    bx_debugger.auto_disassemble = (val > 0);
    return;
    }
  else if ( !strcmp(symbol, "disassemble_size") ) {
    if ( (val!=16) && (val!=32) ) {
      fprintf(stderr, "Error: disassemble_size must be 16 or 32.\n");
      return;
      }
    bx_debugger.disassemble_size = val;
    return;
    }
  else {
    fprintf(stderr, "Error: set: unrecognized symbol.\n");
    return;
    }

  if (!is_OK) {
    fprintf(stderr, "Error: could not set register '%s'.\n", symbol);
    }
}

  void
bx_dbg_query_command(char *what)
{
  unsigned pending;

  if ( !strcmp(what, "pending") ) {
    pending = BX_CPU(0)->dbg_query_pending();

    if ( pending & BX_DBG_PENDING_DMA )
      fprintf(stderr, "pending DMA\n");

    if ( pending & BX_DBG_PENDING_IRQ )
      fprintf(stderr, "pending IRQ\n");

    if (!pending)
      fprintf(stderr, "pending none\n");

    fprintf(stderr, "done\n");
    }
  else {
    fprintf(stderr, "Error: Query '%s' not understood.\n", what);
    }
}

  void
bx_dbg_set_cpu_command(void)
{
  FILE *fp;
  int   reti;
  char *rets;
  Boolean retb;
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
  reti = sscanf(tmp_buf, "tr3:0x%lx", &ul1); cpu.tr3 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "tr4:0x%lx", &ul1); cpu.tr4 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "tr5:0x%lx", &ul1); cpu.tr5 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "tr6:0x%lx", &ul1); cpu.tr6 = ul1;
  if (reti != 1) goto scanf_error;

  rets = fgets(tmp_buf, 512, fp); if (!rets) goto eof_error;
  reti = sscanf(tmp_buf, "tr7:0x%lx", &ul1); cpu.tr7 = ul1;
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

  retb = BX_CPU(0)->dbg_set_cpu(&cpu);
  if (retb == 0)
    fprintf(stderr, "Error: dbg_set_cpu encountered error\n");
  else
    fprintf(stderr, "OK\n");
  return;

eof_error:
  fprintf(stderr, "Error: EOF encountered in dbg_set_cpu input stream\n");
  return;

scanf_error:
  fprintf(stderr, "Error: scanf returned error in dbg_set_cpu input stream\n");
  return;
}

  void
bx_dbg_disassemble_command(bx_num_range range)
{
#if BX_DISASM
  Boolean paddr_valid;
  Bit32u  paddr;
  unsigned ilen;

  if (range.to == EMPTY_ARG) {
    // should set to cs:eip. FIXME
    BX_INFO(("Error: type 'disassemble ADDR' or 'disassemble ADDR:ADDR'"));
    return;
  }

  do {
    BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(range.from, &paddr, &paddr_valid);

    if (paddr_valid) {
      BX_MEM(0)->dbg_fetch_mem(paddr, 16, bx_disasm_ibuf);
      ilen = bx_disassemble.disasm(bx_debugger.disassemble_size==32,
                                   bx_disasm_ibuf, bx_disasm_tbuf);

      fprintf(stderr, "%08x: ", (unsigned) range.from);
      for (unsigned j=0; j<ilen; j++)
        fprintf(stderr, "%02x", (unsigned) bx_disasm_ibuf[j]);
      fprintf(stderr, ": %s\n", bx_disasm_tbuf);
      }
    else {
      fprintf(stderr, "??? (physical address not available)\n");
      ilen = 0; // keep compiler happy
      range.from = range.to; // bail out
      }
      range.from += ilen;
    } while (range.from < range.to);
#else
  UNUSED(range);
#endif  // #if BX_DISASM
}

//NOTE simple minded maths logic
  void
bx_dbg_maths_command(char *command, int data1, int data2)
{
  if(strcmp(command,"add") == 0)
  {
    fprintf(stderr," %x + %x = %x ", data1, data2, data1+data2);
  }
  else if(strcmp(command,"sub") == 0)
  {
    fprintf(stderr," %x - %x = %x ", data1, data2, data1-data2); 
  }
  else if(strcmp(command,"mul") == 0)
  {
    fprintf(stderr," %x * %x = %x ", data1, data2, data1*data2); 
  }
  else if(strcmp(command,"div") == 0)
  {
    fprintf(stderr," %x / %x = %x ", data1, data2, data1/data2); 
  }
  fprintf(stderr,"\n");
}

//FIXME HanishKVC requires better error checking in POST FIX expression
//NOTE Uses POST FIX EXPRESSION handling for better maths
  void
bx_dbg_maths_expression_command(char *expr)
{
  int data1, data2, res;
  int biti,digit;
  char *next_token;

  fprintf(stderr,"%s\n",expr);

  expr++; // skip " in the string token passed
  while(expr[0] == ' ')expr++; // skip any spaces following the " 

  next_token = strtok(expr," ");
  if(next_token == NULL) return;
  data1 = res = strtol(next_token,NULL,0);
  do
  {
    switch(next_token[0])
    {
      case '+':  
        res = data1+data2;
        fprintf(stderr," %x + %x = %x ",data1,data2,res);
        data1 = res;
  break;
      case '-':  
        res = data1-data2;
        fprintf(stderr," %x - %x = %x ",data1,data2,res);
        data1 = res;
	break;
      case '*':  
        res = data1*data2;
        fprintf(stderr," %x * %x = %x ",data1,data2,res);
        data1 = res;
	break;
      case '/':  
        res = data1/data2;
        fprintf(stderr," %x / %x = %x ",data1,data2,res);
        data1 = res;
	break;
      case '&':
        res = data1 & data2;
        fprintf(stderr," %x & %x = %x ",data1,data2,res);
        data1 = res;
        break;
      case '|':
        res = data1 | data2;
        fprintf(stderr," %x | %x = %x ",data1,data2,res);
        data1 = res;
        break;
      case '~':
        res = ~data1;
        fprintf(stderr," ~ %x = %x ",data1,res);
        data1 = res;
        break;
     default:
       data2 = strtol(next_token,NULL,0);
       break;
    }
    next_token = strtok(NULL," ");
    if(next_token == NULL) break;
  }while(1);
  fprintf(stderr,"\n");
  //FIXME HanishKVC If sizeof changes from a Byte addressed to 
  //      Word addressed machine & so on then the logic 
  //      below requires to be updated
  fprintf(stderr," Binary of %x : ",res);
  for(biti=(sizeof(int)*8)-1; ; biti--) 
  {
    digit = (res >> biti) & 0x01;
    fputc(digit + '0', stderr);
    if(biti==0) break;
    if((biti%4) == 0) fputc(' ',stderr);
  }
  fprintf(stderr,"\n");
}

  void
bx_dbg_v2l_command(unsigned seg_no, Bit32u offset)
{
#if BX_NUM_SIMULATORS > 1
  fprintf(stderr, "Error: v2l not supported for nsim > 1\n"
#else
  bx_dbg_sreg_t sreg;
  Bit32u laddr;

  if (seg_no > 5) {
    fprintf(stderr, "Error: seg_no out of bounds\n");
    return;
    }
  BX_CPU(dbg_cpu)->dbg_get_sreg(&sreg, seg_no);
  if (!sreg.valid) {
    fprintf(stderr, "Error: segment valid bit cleared\n");
    return;
    }
  laddr = (sreg.des_l>>16) |
          ((sreg.des_h<<16)&0x00ff0000) |
          (sreg.des_h & 0xff000000);
  laddr += offset;
  
  fprintf(stderr, "laddr: 0x%x (%u)\n",
    (unsigned) laddr, (unsigned) laddr);
#endif
}

  void
bx_dbg_instrument_command(char *comm)
{
#if BX_INSTRUMENTATION
  if ( !strcmp(comm, "start") ) {
    BX_INSTR_START ();
    }
  else if ( !strcmp(comm, "stop") ) {
    BX_INSTR_STOP ();
    }
  else if ( !strcmp(comm, "reset") ) {
    BX_INSTR_RESET ();
    }
  else if ( !strcmp(comm, "print") ) {
    BX_INSTR_PRINT ();
    }
  else {
    fprintf(stderr, "Error: command instrument %s not implemented.\n", comm);
    bx_dbg_exit(1);
    }
#else
    UNUSED(comm);

    fprintf(stderr, "Error: instrumentation not enabled.\n");
#endif
}

  void
bx_dbg_loader_command(char *path_quoted)
{
  size_t len;

  // skip beginning double quote
  if (path_quoted[0] == '"')
    path_quoted++;

  // null out ending quote
  len = strlen(path_quoted);
  if (path_quoted[len - 1] == '"')
    path_quoted[len - 1] = '\0';

#if BX_USE_LOADER
  {
  bx_loader_misc_t loader_misc;
  BX_CPU(0)->loader(path_quoted, &loader_misc);
#if 0
fprintf(stderr, "dr0: 0x%08x\n", loader_misc.dr0);
fprintf(stderr, "dr1: 0x%08x\n", loader_misc.dr1);
fprintf(stderr, "dr2: 0x%08x\n", loader_misc.dr2);
fprintf(stderr, "dr3: 0x%08x\n", loader_misc.dr3);
fprintf(stderr, "dr6: 0x%08x\n", loader_misc.dr6);
fprintf(stderr, "dr7: 0x%08x\n", loader_misc.dr7);
#endif
bx_cpu.dr0 = loader_misc.dr0;
bx_cpu.dr1 = loader_misc.dr1;
bx_cpu.dr2 = loader_misc.dr2;
bx_cpu.dr3 = loader_misc.dr3;
bx_cpu.dr7 = loader_misc.dr7;
  }
#else
  fprintf(stderr, "Error: loader not implemented.\n");
#endif
}

  void
bx_dbg_doit_command(unsigned n)
{
  // generic command to add temporary hacks to
  // for debugging purposes
  UNUSED(n);



  bx_dbg.interrupts = n;
  bx_dbg.exceptions = n;
}

  void
bx_dbg_crc_command(Bit32u addr1, Bit32u addr2)
{
  Bit32u crc1, crc2;

  if (addr1 >= addr2) {
    fprintf(stderr, "Error: crc: invalid range.\n");
    return;
    }

  if (!BX_MEM(0)->dbg_crc32(crc32, addr1, addr2, &crc1)) {
    fprintf(stderr, "sim0: could not CRC memory\n");
    return;
    }
#if BX_NUM_SIMULATORS == 1
  fprintf(stderr, "0x%lx\n", crc1);
#else
  if (!BX_MEM(1)->dbg_crc32(crc32, addr1, addr2, &crc2)) {
    fprintf(stderr, "sim1: could not CRC memory\n");
    return;
    }
  if (crc1 == crc2) {
    fprintf(stderr, "CRC same: 0x%x\n", (unsigned) crc1);
    }
  else {
    fprintf(stderr, "CRC different: sim0=0x%x, sim1=0x%x\n",
            (unsigned) crc1, (unsigned) crc2);
    }
#endif
}

  void
bx_dbg_info_dirty_command(void)
{
  unsigned char *page_tbl = BX_MEM(0)->dbg_dirty_pages;
  unsigned page_tbl_size  = BX_MEM(0)->dbg_count_dirty_pages ();

  for (unsigned i=0; i<page_tbl_size; i++) {
    if (page_tbl[i]) {
      fprintf(stderr, "0x%x\n", i);
      page_tbl[i] = 0; // reset to clean
      }
    }
}

void bx_dbg_print_descriptor (FILE *fp, unsigned char desc[8], int verbose)
{
  int lo = (desc[3] << 24) | (desc[2] << 16) | (desc[1] << 8) | (desc[0]);
  int hi = (desc[7] << 24) | (desc[6] << 16) | (desc[5] << 8) | (desc[4]);
  //fprintf (fp, "descriptor hi,lo = %08x,%08x\n", hi, lo);
  int base = ((lo >> 16) & 0xffff)
             | ((hi << 16) & 0xff0000)
             | (hi & 0xff000000);
  int limit = (lo & 0xffff);
  int segment = (lo >> 16) & 0xffff;
  int offset = (lo & 0xffff) | (hi & 0xffff0000);
  int type = (hi >> 8) & 0x0f;
  int dpl = (hi >> 13) & 0x03;
  int s = (hi >> 12) & 0x01;
  int present = (hi >> 15) & 0x01;
  int avl = (hi >> 20) & 0x01;
  int d_b = (hi >> 22) & 0x01;
  int g = (hi >> 23) & 0x01;
  int base_is_jump_addr;
#if 0
  if (s) {
    // either a code or a data segment. bit 11 (type file MSB) then says 
    // 0=data segment, 1=code seg
    if (type&8) {
      fprintf (fp, "Segment type: Code, %s%s%s\n",
	(type&2)? "Execute/Read" : "Execute-Only",
	(type&4)? ", Conforming" : "",
	(type&1)? ", Accessed" : "");
      fprintf (fp, "D flag=%d (use %d-bit addresses, %d-bit or 8-bit operands)\n", d_b, d_b? 32 : 16);
    } else {
      fprintf (fp, "Segment type: Data, %s%s%s\n",
	(type&2)? "Read/Write" : "Read-Only",
	(type&4)? ", Expand-down" : "",
	(type&1)? ", Accessed" : "");
    }
  } else {
    // types from IA32-devel-guide-3, page 3-15.
    static char *type_names[16] = { "Reserved", "16-Bit TSS (available)", "LDT", "16-Bit TSS (Busy)", "16-Bit Call Gate", "Task Gate", "16-Bit Interrupt Gate", "16-Bit Trap Gate", "Reserved", "32-Bit TSS (Available)", "Reserved", "32-Bit TSS (Busy)", "32-Bit Call Gate", "Reserved", "32-Bit Interrupt Gate", "32-Bit Trap Gate" };
    // some kind of gate?
    fprintf (fp, "System segment, type=0x%x=%s\n", type, type_names[type]);
    base_is_jump_addr = 1;
    // for call gates, print segment:offset and parameter count p.40-15
    // for task gate, only present,dpl,TSS segment selector exist. p.5-13
    // for interrupt gate, segment:offset,p,dpl
    // for trap gate, segment:offset,p,dpl
  }
  fprintf (fp, "DPL=descriptor privilege level=%d\n", dpl);
  if (base_is_jump_addr) {
    fprintf (fp, "target address=%04x:%08x\n", segment, offset);
  } else {
    fprintf (fp, "base address=%p\n", base);
    fprintf (fp, "G=granularity=%d\n", g);
    fprintf (fp, "limit=0x%x %s (see G)\n", limit, g?"4K-byte units" : "bytes");
    fprintf (fp, "AVL=available to OS=%d\n", avl);
  }
  fprintf (fp, "P=present=%d\n", present);
#endif
  /* brief output */
// 32-bit trap gate, target=0010:c0108ec4, DPL=0, present=1
// code segment, base=0000:00cfffff, length=0xffff
  if (s) {
    // either a code or a data segment. bit 11 (type file MSB) then says 
    // 0=data segment, 1=code seg
    if (type&8) {
      fprintf (fp, "Code segment, linearaddr=%08x, len=%04x %s, %s%s%s, %d-bit addrs\n", 
	base, limit, g ? "* 4Kbytes" : "bytes",
	(type&2)? "Execute/Read" : "Execute-Only",
	(type&4)? ", Conforming" : "",
	(type&1)? ", Accessed" : "",
	d_b? 32 : 16);
    } else {
      fprintf (fp, "Data segment, linearaddr=%08x, len=%04x %s, %s%s%s\n",
	base, limit, g ? "* 4Kbytes" : "bytes",
	(type&2)? "Read/Write" : "Read-Only",
	(type&4)? ", Expand-down" : "",
	(type&1)? ", Accessed" : "");
    }
  } else {
    // types from IA32-devel-guide-3, page 3-15.
    static char *undef = "???";
    static char *type_names[16] = { undef, "16-Bit TSS (available)", "LDT", "16-Bit TSS (Busy)", "16-Bit Call Gate", "Task Gate", "16-Bit Interrupt Gate", "16-Bit Trap Gate", undef, "32-Bit TSS (Available)", undef, "32-Bit TSS (Busy)", "32-Bit Call Gate", undef, "32-Bit Interrupt Gate", "32-Bit Trap Gate" };
    fprintf (fp, "%s ", type_names[type]);
    // only print more if type is valid
    if (type_names[type] == undef)  {
      fprintf (fp, "descriptor hi=%08x, lo=%08x", hi, lo);
    } else {
      // for call gates, print segment:offset and parameter count p.4-15
      // for task gate, only present,dpl,TSS segment selector exist. p.5-13
      // for interrupt gate, segment:offset,p,dpl
      // for trap gate, segment:offset,p,dpl
      // for TSS, base address and segment limit
      switch (type) {
	case 1: case 3:  // 16-bit TSS
	case 9: case 11: // 32-bit TSS
	  limit = (hi&0x000f0000) | (lo&0xffff);
	  fprintf (fp, "at %08x, length 0x%x", base, limit);
	  break;
	case 2:
	  // it's an LDT. not much to print.
	  break;
	default:
	  // task, int, trap, or call gate.
	  fprintf (fp, "target=%04x:%08x, DPL=%d", segment, offset, dpl);
      }
    }
    fprintf (fp, "\n");
  }
}

void
bx_dbg_info_idt_command(bx_num_range range) {
  bx_dbg_cpu_t cpu;
  BX_CPU(0)->dbg_get_cpu(&cpu);
  int n, print_table = 0;
  if (range.to == EMPTY_ARG) {
    // show all entries
    range.from = 0;
    range.to = (cpu.idtr.limit) / 8;
    print_table = 1;
  }
  if (print_table)
    fprintf (stderr, "Interrupt Descriptor Table (0x%08x):\n", cpu.idtr.base);
  for (n = range.from; n<=range.to; n++) {
    Bit32u paddr, paddr_valid;
    BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(cpu.idtr.base + 8*n, &paddr, &paddr_valid);
    if (!paddr_valid) {
      fprintf (stderr, "error: IDTR+8*%d points to invalid linear address %p\n",
	 n, cpu.idtr.base);
      return;
    }
    // read 8-byte entry from IDT
    unsigned char entry[8];
    BX_MEM(0)->dbg_fetch_mem (paddr, 8, entry);
    fprintf (stderr, "IDT[0x%02x]=", n);
    bx_dbg_print_descriptor (stderr, entry, 0);
  }
  if (print_table) fprintf (stderr, "You can list individual entries with 'info idt NUM' or groups with 'info idt NUM:NUM'\n");
}

void
bx_dbg_info_gdt_command(bx_num_range range) {
  bx_dbg_cpu_t cpu;
  BX_CPU(0)->dbg_get_cpu(&cpu);
  int n, print_table = 0;
  if (range.to == EMPTY_ARG) {
    // show all entries
    range.from = 0;
    range.to = (cpu.gdtr.limit) / 8;
    print_table = 1;
  }
  if (print_table)
    fprintf (stderr, "Global Descriptor Table (0x%08x):\n", cpu.gdtr.base);
  for (n = range.from; n<=range.to; n++) {
    Bit32u paddr, paddr_valid;
    BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(cpu.gdtr.base + 8*n, &paddr, &paddr_valid);
    if (!paddr_valid) {
      fprintf (stderr, "error: GDTR+8*%d points to invalid linear address %p\n",
	  n, cpu.gdtr.base);
      return;
    }
    unsigned char entry[8];
    // read 8-byte entry from GDT
    BX_MEM(0)->dbg_fetch_mem (paddr, 8, entry);
    fprintf (stderr, "GDT[0x%02x]=", n);
    bx_dbg_print_descriptor (stderr, entry, 0);
  }
  if (print_table) fprintf (stderr, "You can list individual interrupts with 'info gdt NUM'.\n");
}

void
bx_dbg_info_ldt_command(bx_num_range n) {
  bx_dbg_cpu_t cpu;
  BX_CPU(0)->dbg_get_cpu(&cpu);
  fprintf (stderr, "Local Descriptor Table output not implemented\n");
}

void
bx_dbg_info_tss_command(bx_num_range n) {
  bx_dbg_cpu_t cpu;
  BX_CPU(0)->dbg_get_cpu(&cpu);
  fprintf (stderr, "TSS output not implemented\n");
}

bx_num_range 
make_num_range (Bit64s from, Bit64s to)
{
  bx_num_range x;
  x.from = from;
  x.to = to;
  return x;
}

  void
bx_dbg_info_control_regs_command(void)
{
  bx_dbg_cpu_t cpu;
  BX_CPU(0)->dbg_get_cpu(&cpu);
  int cr0 = cpu.cr0;
  int cr2 = cpu.cr2;
  int cr3 = cpu.cr3;
  int cr4 = cpu.cr4;
  fprintf (stderr, "CR0=0x%08x\n", cr0);
  fprintf (stderr, "    PG=paging=%d\n", (cr0>>31) & 1);
  fprintf (stderr, "    CD=cache disable=%d\n", (cr0>>30) & 1);
  fprintf (stderr, "    NW=not write through=%d\n", (cr0>>29) & 1);
  fprintf (stderr, "    AM=alignment mask=%d\n", (cr0>>18) & 1);
  fprintf (stderr, "    WP=write protect=%d\n", (cr0>>16) & 1);
  fprintf (stderr, "    NE=numeric error=%d\n", (cr0>>5) & 1);
  fprintf (stderr, "    ET=extension type=%d\n", (cr0>>4) & 1);
  fprintf (stderr, "    TS=task switched=%d\n", (cr0>>3) & 1);
  fprintf (stderr, "    EM=FPU emulation=%d\n", (cr0>>2) & 1);
  fprintf (stderr, "    MP=monitor coprocessor=%d\n", (cr0>>1) & 1);
  fprintf (stderr, "    PE=protection enable=%d\n", (cr0>>0) & 1);
  fprintf (stderr, "CR2=page fault linear address=0x%08x\n", cr2);
  fprintf (stderr, "CR3=0x%08x\n", cr3);
  fprintf (stderr, "    PCD=page-level cache disable=%d\n", (cr3>>4) & 1);
  fprintf (stderr, "    PWT=page-level writes transparent=%d\n", (cr3>>3) & 1);
  fprintf (stderr, "CR4=0x%08x\n", cr4);
  fprintf (stderr, "    VME=virtual-8086 mode extensions=%d\n", (cr4>>0) & 1);
  fprintf (stderr, "    PVI=protected-mode virtual interrupts=%d\n", (cr4>>1) & 1);
  fprintf (stderr, "    TSD=time stamp disable=%d\n", (cr4>>2) & 1);
  fprintf (stderr, "    DE=debugging extensions=%d\n", (cr4>>3) & 1);
  fprintf (stderr, "    PSE=page size extensions=%d\n", (cr4>>4) & 1);
  fprintf (stderr, "    PAE=physical address extension=%d\n", (cr4>>5) & 1);
  fprintf (stderr, "    MCE=machine check enable=%d\n", (cr4>>6) & 1);
  fprintf (stderr, "    PGE=page global enable=%d\n", (cr4>>7) & 1);
  fprintf (stderr, "    PCE=performance-monitor counter enable=%d\n", (cr4>>8) & 1);
  fprintf (stderr, "    OXFXSR=OS support for FXSAVE/FXRSTOR=%d\n", (cr4>>9) & 1);
  fprintf (stderr, "    OSXMMEXCPT=OS support for unmasked SIMD FP exceptions=%d\n", (cr4>>10) & 1);
}

/*
 * this implements the info ne2k commands in the debugger.
 * info ne2k - shows all registers
 * info ne2k page N - shows all registers in a page
 * info ne2k page N reg M - shows just one register
 */
void
bx_dbg_info_ne2k(int page, int reg)
{
#if BX_NE2K_SUPPORT
  bx_ne2k.print_info (stderr, page, reg, 0);
#else
  fprintf (stderr, "NE2000 support is not compiled in.\n");
#endif
}

//
// Reports from various events
//

  void
bx_dbg_iac_report(unsigned vector, unsigned irq)
{
#if BX_NUM_SIMULATORS > 1
  unsigned tail, master;
#endif

if (doit) fprintf(stderr, "iac report: vector=%u\n", vector);

  if (bx_guard.report.irq) {
    fprintf(stderr, "event icount=%u IRQ irq=%u vec=%x\n",
      (unsigned) BX_CPU(dbg_cpu)->guard_found.icount, irq, vector);
    }

#if BX_NUM_SIMULATORS > 1
  if (bx_debugger.master_slave_mode == BX_DBG_SLAVE_MODE ) {
    fprintf(stderr, "Error: iac_report: in slave mode.\n");
    bx_dbg_exit(1);
    }

  // Master simulator mode
  if (bx_debugger.async_journal.size >= BX_DBG_ASYNC_JOURNAL_SIZE) {
    fprintf(stderr, "Error: iac: async journal full.\n");
    bx_dbg_exit(1);
    }

  if (bx_debugger.async_journal.size == 0) {
    // start off point head & tail at same element
    bx_debugger.async_journal.head = 0;
    tail = bx_debugger.async_journal.tail = 0;
    }
  else {
    tail = bx_debugger.async_journal.tail + 1;
    }
  if (tail >= BX_DBG_ASYNC_JOURNAL_SIZE) {
    fprintf(stderr, "Error: iac_report: journal wrapped.\n");
    bx_dbg_exit(0);
    }

  master = bx_debugger.master;
  bx_debugger.async_journal.element[tail].what = BX_DBG_ASYNC_JOURNAL_IAC;
  bx_debugger.async_journal.element[tail].icount = bx_guard_found[master].icount;
  bx_debugger.async_journal.element[tail].u.iac.val = vector;

  if (bx_debugger.async_journal.size)
    bx_debugger.async_journal.tail++;
  bx_debugger.async_journal.size++;
#endif
}

  void
bx_dbg_a20_report(unsigned val)
{
  if (bx_guard.report.a20) {
    fprintf(stderr, "event icount=%u A20 val=%u\n",
      (unsigned) BX_CPU(dbg_cpu)->guard_found.icount, val);
    }
}

#if BX_NUM_SIMULATORS > 1
  void
bx_dbg_journal_a20_event(unsigned val)
{
  unsigned tail, master;

  if (bx_debugger.master_slave_mode == BX_DBG_SLAVE_MODE ) {
    fprintf(stderr, "Error: a20_report: in slave mode.\n");
    bx_dbg_exit(1);
    }

  // Master simulator mode
  if (bx_debugger.async_journal.size >= BX_DBG_ASYNC_JOURNAL_SIZE) {
    fprintf(stderr, "Error: async journal full.\n");
    bx_dbg_exit(1);
    }

  if (bx_debugger.async_journal.size == 0) {
    // start off point head & tail at same element
    bx_debugger.async_journal.head = 0;
    tail = bx_debugger.async_journal.tail = 0;
    }
  else {
    tail = bx_debugger.async_journal.tail + 1;
    }
  if (tail >= BX_DBG_ASYNC_JOURNAL_SIZE) {
    fprintf(stderr, "Error: a20_report: journal wrapped.\n");
    bx_dbg_exit(0);
    }

  master = bx_debugger.master;
  bx_debugger.async_journal.element[tail].what = BX_DBG_ASYNC_JOURNAL_A20;
  bx_debugger.async_journal.element[tail].icount = bx_guard_found[master].icount;
  bx_debugger.async_journal.element[tail].u.a20.val = val;

  if (bx_debugger.async_journal.size)
    bx_debugger.async_journal.tail++;
  bx_debugger.async_journal.size++;
}
#endif

  void
bx_dbg_io_report(Bit32u addr, unsigned size, unsigned op, Bit32u val)
{
  if (bx_guard.report.io) {
    fprintf(stderr, "event icount=%u IO addr=0x%x size=%u op=%s val=0x%x\n",
      (unsigned) BX_CPU(dbg_cpu)->guard_found.icount,
      (unsigned) addr,
      size,
      (op==BX_READ) ? "read" : "write",
      (unsigned) val);
    }

  // nothing else to do.  bx_dbg_inp() and bx_dbg_outp() do the journaling.
}

  void
bx_dbg_ucmem_report(Bit32u addr, unsigned size, unsigned op, Bit32u val)
{
  if (bx_guard.report.ucmem) {
    fprintf(stderr, "event icount=%u UCmem addr=0x%x size=%u op=%s val=0x%x\n",
      (unsigned) BX_CPU(dbg_cpu)->guard_found.icount,
      (unsigned) addr,
      size,
      (op==BX_READ) ? "read" : "write",
      (unsigned) val);
    }
  // nothing else to do.  bx_dbg_ucmem_read() and bx_dbg_ucmem_write()
  // do the journaling.
}

  void
bx_dbg_dma_report(Bit32u addr, unsigned len, unsigned what, Bit32u val)
{
  if (bx_dbg_batch_dma.this_many == 0) {
    fprintf(stderr, "%s: DMA batch this_many=0.\n", argv0);
    bx_dbg_exit(1);
    }

  // if Q is full, post events (and flush)
  if (bx_dbg_batch_dma.Qsize >= bx_dbg_batch_dma.this_many) {
    fprintf(stderr, "%s: DMA batch Q was not flushed.\n", argv0);
    bx_dbg_exit(1);
    }

  // if Q already has MAX elements in it
  if (bx_dbg_batch_dma.Qsize >= BX_BATCH_DMA_BUFSIZE) {
    fprintf(stderr, "%s: DMA batch buffer overrun.\n", argv0);
    bx_dbg_exit(1);
    }

  bx_dbg_batch_dma.Qsize++;
  bx_dbg_batch_dma.Q[bx_dbg_batch_dma.Qsize-1].addr   = addr;
  bx_dbg_batch_dma.Q[bx_dbg_batch_dma.Qsize-1].len    = len;
  bx_dbg_batch_dma.Q[bx_dbg_batch_dma.Qsize-1].what   = what;
  bx_dbg_batch_dma.Q[bx_dbg_batch_dma.Qsize-1].val    = val;
  bx_dbg_batch_dma.Q[bx_dbg_batch_dma.Qsize-1].icount = BX_CPU(dbg_cpu)->guard_found.icount;

  // if Q is full, post events (and flush)
  if (bx_dbg_batch_dma.Qsize >= bx_dbg_batch_dma.this_many)
    bx_dbg_post_dma_reports();
}

  void
bx_dbg_post_dma_reports(void)
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
          fprintf(stderr, "\n");
        else
          first_iteration = 0;
        // need to output the event header
        fprintf(stderr, "event icount=%u DMA addr=0x%x size=%u op=%s val=0x%x",
                         (unsigned) bx_dbg_batch_dma.Q[i].icount,
                         addr, len, (what==BX_READ) ? "read" : "write",
                         val );
        print_header = 0;
        }
      else {
        // *no* need to output the event header
        fprintf(stderr, " 0x%x", val);
        }
      }
    if (bx_dbg_batch_dma.Qsize)
      fprintf(stderr, "\n");
    }

  // empty Q, regardless of whether reports are printed
  bx_dbg_batch_dma.Qsize = 0;
}




//
// Cosimulation routines
//

#if (BX_NUM_SIMULATORS >= 2)

  Bit8u
bx_dbg_ucmem_read(Bit32u addr)
{
  Bit8u value;
  unsigned head, tail;

  if ( bx_debugger.master_slave_mode == BX_DBG_MASTER_MODE ) {
	  if (!bx_debugger.fast_forward_mode) {
		  if (bx_debugger.UCmem_journal.size >= BX_DBG_UCMEM_JOURNAL_SIZE) {
			  fprintf(stderr, "dbg_ucmem_read: journal full.\n");
			  bx_dbg_exit(0);
		  }

		  if (bx_debugger.UCmem_journal.size == 0) {
			  // start off point head & tail at same element
			  bx_debugger.UCmem_journal.head = 0;
			  tail = bx_debugger.UCmem_journal.tail = 0;
		  }
		  else {
			  tail = bx_debugger.UCmem_journal.tail + 1;
		  }
		  if (tail >= BX_DBG_UCMEM_JOURNAL_SIZE) {
			  fprintf(stderr, "dbg_ucmem_read: journal wrapped.\n");
			  bx_dbg_exit(0);
		  }

		  value = bx_devices.vga->mem_read(addr);
		  bx_dbg_ucmem_report(addr, 1, BX_READ, value);
		  bx_debugger.UCmem_journal.element[tail].op    = BX_READ;
		  bx_debugger.UCmem_journal.element[tail].len   = 1;
		  bx_debugger.UCmem_journal.element[tail].addr  = addr;
		  bx_debugger.UCmem_journal.element[tail].value = value;
		  if (bx_debugger.UCmem_journal.size)
			  bx_debugger.UCmem_journal.tail++;
		  bx_debugger.UCmem_journal.size++;

		  if (doit)
			  fprintf(stderr, "MASTER UCR: head:%u tail%u size:%u\n",
				  bx_debugger.UCmem_journal.head,
				  bx_debugger.UCmem_journal.tail,
				  bx_debugger.UCmem_journal.size);

		  return(value);
	  } else {
		  value = bx_devices.vga->mem_read(addr);
		  return(value);
	  }
  }
  else {
    if (bx_debugger.UCmem_journal.size == 0) {
      fprintf(stderr, "Error: ucmem_read: journal empty.\n");
      return(0xff);
      }
    head = bx_debugger.UCmem_journal.head;
    value = bx_debugger.UCmem_journal.element[head].value;

    if ((bx_debugger.UCmem_journal.element[head].op != BX_READ) ||
        (bx_debugger.UCmem_journal.element[head].len != 1) ||
        (bx_debugger.UCmem_journal.element[head].addr != addr)) {

      fprintf(stderr, "Error: ucmem_read: out of sync with journal.\n");
      fprintf(stderr, "Error: master: op=%1s len=%u addr=0x%x val=0x%x\n",
        (bx_debugger.UCmem_journal.element[head].op==BX_READ) ? "W" : "R",
        (unsigned) bx_debugger.UCmem_journal.element[head].len,
        (unsigned) bx_debugger.UCmem_journal.element[head].addr,
        (unsigned) bx_debugger.UCmem_journal.element[head].value);
      fprintf(stderr, "Error: slave:  op=W len=%u addr=0x%x val=0x%x\n",
        (unsigned) 1, (unsigned) addr, (unsigned) value);
      return(0xff);
      }
    // slave UCmem op in sync with journaled master op, delete this entry
    bx_debugger.UCmem_journal.head++;
    bx_debugger.UCmem_journal.size--;
    return(value);
    }
}

  void
bx_dbg_ucmem_write(Bit32u addr, Bit8u value)
{
  unsigned tail, head;

  if ( bx_debugger.master_slave_mode == BX_DBG_MASTER_MODE ) {
	  if (!bx_debugger.fast_forward_mode) {
		  if (bx_debugger.UCmem_journal.size >= BX_DBG_UCMEM_JOURNAL_SIZE) {
			  fprintf(stderr, "dbg_ucmem_write: journal full.\n");
			  bx_dbg_exit(0);
		  }

		  if (bx_debugger.UCmem_journal.size == 0) {
			  // start off point head & tail at same element
			  bx_debugger.UCmem_journal.head = 0;
			  tail = bx_debugger.UCmem_journal.tail = 0;
		  }
		  else {
			  tail = bx_debugger.UCmem_journal.tail + 1;
		  }
		  if (tail >= BX_DBG_UCMEM_JOURNAL_SIZE) {
			  fprintf(stderr, "dbg_ucmem_write: journal wrapped.\n");
			  bx_dbg_exit(0);
		  }

		  bx_debugger.UCmem_journal.element[tail].op    = BX_WRITE;
		  bx_debugger.UCmem_journal.element[tail].len   = 1;
		  bx_debugger.UCmem_journal.element[tail].addr  = addr;
		  bx_debugger.UCmem_journal.element[tail].value = value;

		  if (bx_debugger.UCmem_journal.size)
			  bx_debugger.UCmem_journal.tail++;
		  bx_debugger.UCmem_journal.size++;
		  bx_devices.vga->mem_write(addr, value);
		  bx_dbg_ucmem_report(addr, 1, BX_WRITE, value);
	  } else {
		  bx_devices.vga->mem_write(addr, value);
	  }
  }
  else {
    if (bx_debugger.UCmem_journal.size == 0) {
      fprintf(stderr, "Error: ucmem_write: journal empty.\n");
      return;
      }
    head = bx_debugger.UCmem_journal.head;

    if ((bx_debugger.UCmem_journal.element[head].op != BX_WRITE) ||
        (bx_debugger.UCmem_journal.element[head].len != 1) ||
        (bx_debugger.UCmem_journal.element[head].addr != addr) ||
        (bx_debugger.UCmem_journal.element[head].value != value) ) {
      fprintf(stderr, "Error: ucmem_write: out of sync with journal.\n");
      fprintf(stderr, "Error: master: op=%1s len=%u addr=0x%x val=0x%x\n",
        (bx_debugger.UCmem_journal.element[head].op==BX_WRITE) ? "W" : "R",
        (unsigned) bx_debugger.UCmem_journal.element[head].len,
        (unsigned) bx_debugger.UCmem_journal.element[head].addr,
        (unsigned) bx_debugger.UCmem_journal.element[head].value);
      fprintf(stderr, "Error: slave:  op=W len=%u addr=0x%x val=0x%x\n",
        (unsigned) 1, (unsigned) addr, (unsigned) value);
      return;
      }
    // slave UCmem op in sync with journaled master op, delete this entry
    bx_debugger.UCmem_journal.head++;
    bx_debugger.UCmem_journal.size--;
    }
}

  void
bx_dbg_async_pin_request(unsigned what, Boolean val)
{
  // Request from IO devices for change in pin external to CPU.
  // This is pended until CPU ack's with bx_dbg_async_pin_ack().

  if (bx_debugger.master_slave_mode != BX_DBG_MASTER_MODE) {
    fprintf(stderr, "Error: dbg_async_pin_request not in master mode.\n");
    bx_dbg_exit(1);
    }

  switch (what) {
    case BX_DBG_ASYNC_PENDING_A20:
      // Q pending status
      bx_guard.async_changes_pending.which |= BX_DBG_ASYNC_PENDING_A20;
      bx_guard.async_changes_pending.a20    = val;
      return;
      break;

    case BX_DBG_ASYNC_PENDING_RESET:
    case BX_DBG_ASYNC_PENDING_NMI:
    default:
      fprintf(stderr, "Error: set_async_pin: unhandled case.\n");
      bx_dbg_exit(1);
    }
}


  void
bx_dbg_async_pin_ack(unsigned what, Boolean val)
{
  // Acknowledgement from master simulator for pending change in pin
  // external to CPU.

  if (bx_debugger.master_slave_mode != BX_DBG_MASTER_MODE) {
    fprintf(stderr, "Error: dbg_async_pin_ack: not master mode.\n");
    bx_dbg_exit(1);
    }

  switch (what) {
    case BX_DBG_ASYNC_PENDING_A20:
      // get rid of pending status
      bx_guard.async_changes_pending.which &= ~BX_DBG_ASYNC_PENDING_A20;
      // notify pc_system of change
      bx_pc_system.set_enable_a20(val);
      if (BX_CPU(bx_debugger.master)->set_A20)
        BX_CPU(bx_debugger.master)->set_A20(val);
      bx_dbg_journal_a20_event(val);
      return;
      break;

    case BX_DBG_ASYNC_PENDING_RESET:
    case BX_DBG_ASYNC_PENDING_NMI:
    default:
      fprintf(stderr, "Error: set_async_pin: unhandled case.\n");
      bx_dbg_exit(1);
    }
}

  Bit32u
bx_dbg_inp(Bit16u addr, unsigned len)
{
  Bit32u value;
  unsigned tail, head;

  if ( bx_debugger.master_slave_mode == BX_DBG_MASTER_MODE ) {
	  if (!bx_debugger.fast_forward_mode) {
		  if (bx_debugger.IO_journal.size >= BX_DBG_IO_JOURNAL_SIZE) {
			  fprintf(stderr, "dbg_inp: journal full.\n");
			  bx_dbg_exit(0);
		  }

		  if (bx_debugger.IO_journal.size == 0) {
			  // start off point head & tail at same element
			  bx_debugger.IO_journal.head = 0;
			  tail = bx_debugger.IO_journal.tail = 0;
		  }
		  else {
			  tail = bx_debugger.IO_journal.tail + 1;
		  }
		  if (tail >= BX_DBG_IO_JOURNAL_SIZE) {
			  fprintf(stderr, "dbg_inp: journal wrapped.\n");
			  bx_dbg_exit(0);
		  }

		  value = bx_pc_system.inp(addr, len);
		  bx_debugger.IO_journal.element[tail].op    = BX_READ;
		  bx_debugger.IO_journal.element[tail].len   = (Bit8u) len;
		  bx_debugger.IO_journal.element[tail].addr  = addr;
		  bx_debugger.IO_journal.element[tail].value = value;
		  if (bx_debugger.IO_journal.size)
			  bx_debugger.IO_journal.tail++;
		  bx_debugger.IO_journal.size++;
//fprintf(stderr, "MASTER IN: head:%u tail%u size:%u\n",
//  bx_debugger.IO_journal.head,
//  bx_debugger.IO_journal.tail,
//  bx_debugger.IO_journal.size);
		  return(value);
	  } else {
		  value = bx_pc_system.inp(addr, len);
		  return(value);
	  }
  }
  else {
    if (bx_debugger.IO_journal.size == 0) {
      fprintf(stderr, "Error: dbg_inp: journal empty.\n");
      return(0xffffffff);
      }
    head = bx_debugger.IO_journal.head;
    value = bx_debugger.IO_journal.element[head].value;

    if ((bx_debugger.IO_journal.element[head].op != BX_READ) ||
        (bx_debugger.IO_journal.element[head].len != len) ||
        (bx_debugger.IO_journal.element[head].addr != addr) ) {
      fprintf(stderr, "Error: dbg_inp: out of sync with journal.\n");
      fprintf(stderr, "Error: master: op=%3s len=%u addr=0x%x\n",
        (bx_debugger.IO_journal.element[head].op==BX_WRITE) ? "OUT" : "IN",
        (unsigned) bx_debugger.IO_journal.element[head].len,
        (unsigned) bx_debugger.IO_journal.element[head].addr);
      fprintf(stderr, "Error: slave:  op=OUT len=%u addr=0x%x\n",
        (unsigned) len, (unsigned) addr);
      return(0xffffffff);
      }
    // slave IO op in sync with journaled master op, delete this entry
    bx_debugger.IO_journal.head++;
    bx_debugger.IO_journal.size--;
// fprintf(stderr, "SLAVE   IN: head:%u tail%u size:%u\n",
//   bx_debugger.IO_journal.head,
//   bx_debugger.IO_journal.tail,
//   bx_debugger.IO_journal.size);
    return(value);
    }
}

  void
bx_dbg_outp(Bit16u addr, Bit32u value, unsigned len)
{
  unsigned tail, head;

  if ( bx_debugger.master_slave_mode == BX_DBG_MASTER_MODE ) {
	  if (!bx_debugger.fast_forward_mode) {
		  if (bx_debugger.IO_journal.size >= BX_DBG_IO_JOURNAL_SIZE) {
			  fprintf(stderr, "dbg_outp: IO journal full.\n");
			  bx_dbg_exit(0);
		  }

		  if (bx_debugger.IO_journal.size == 0) {
			  // start off point head & tail at same element
			  bx_debugger.IO_journal.head = 0;
			  tail = bx_debugger.IO_journal.tail = 0;
		  }
		  else {
			  tail = bx_debugger.IO_journal.tail + 1;
		  }
		  if (tail >= BX_DBG_IO_JOURNAL_SIZE) {
			  fprintf(stderr, "dbg_outp: IO journal wrapped.\n");
			  bx_dbg_exit(0);
		  }

		  bx_debugger.IO_journal.element[tail].op    = BX_WRITE;
		  bx_debugger.IO_journal.element[tail].len   = (Bit8u) len;
		  bx_debugger.IO_journal.element[tail].addr  = addr;
		  bx_debugger.IO_journal.element[tail].value = value;
		  if (bx_debugger.IO_journal.size)
			  bx_debugger.IO_journal.tail++;
		  bx_debugger.IO_journal.size++;
		  bx_pc_system.outp(addr, value, len);
		  if (doit)
			  fprintf(stderr, "master: IO journal size now %u\n", bx_debugger.IO_journal.size);
	  } else {
		  bx_pc_system.outp(addr, value, len);
	  }
  }
  else {
    if (bx_debugger.IO_journal.size == 0) {
      fprintf(stderr, "Error: dbg_outp: journal empty.\n");
      return;
      }
    head = bx_debugger.IO_journal.head;

    if ((bx_debugger.IO_journal.element[head].op != BX_WRITE) ||
        (bx_debugger.IO_journal.element[head].len != len) ||
        (bx_debugger.IO_journal.element[head].addr != addr) ||
        (bx_debugger.IO_journal.element[head].value != value) ) {
      fprintf(stderr, "Error: dbg_outp: out of sync with journal.\n");
      fprintf(stderr, "Error: master: op=%3s len=%u addr=0x%x val=0x%x\n",
        (bx_debugger.IO_journal.element[head].op==BX_WRITE) ? "OUT" : "IN",
        (unsigned) bx_debugger.IO_journal.element[head].len,
        (unsigned) bx_debugger.IO_journal.element[head].addr,
        (unsigned) bx_debugger.IO_journal.element[head].value);
      fprintf(stderr, "Error: slave:  op=OUT len=%u addr=0x%x val=0x%x\n",
        (unsigned) len, (unsigned) addr, (unsigned) value);
      return;
      }
    // slave IO op in sync with journaled master op, delete this entry
    bx_debugger.IO_journal.head++;
    bx_debugger.IO_journal.size--;
if (doit)
fprintf(stderr, "slave: IO journal size now %u\n", bx_debugger.IO_journal.size);
    }
}

  void
bx_dbg_raise_HLDA(void)
{
  fprintf(stderr, "dbg_HLDA called\n");
  bx_dbg_exit(0);
}

  Bit8u
bx_dbg_IAC(void)
{
  // Convience routine.  bochs skips this, and calls the PIC code
  // directly.  This is for other simulators to interface to the
  // the PIC code.
  unsigned iac;

  iac = bx_devices.pic->IAC();
  return(iac);
}

  void
bx_dbg_set_INTR(Boolean b)
{
  if ( bx_debugger.master_slave_mode == BX_DBG_SLAVE_MODE ) {
    fprintf(stderr, "Error: set_INTR in slave mode.\n");
    bx_dbg_exit(1);
    }

  bx_pc_system.INTR = b;
  BX_CPU(bx_debugger.master)->set_INTR(b);
}

#endif  // #if (BX_NUM_SIMULATORS >= 2)

// BW added. return non zero to cause a stop
#if BX_DEBUGGER
static int symbol_level;

int 
bx_dbg_symbolic_output(void) 
{

      /* modes & address spaces */
      if(BX_CPU(dbg_cpu)->cr0.pe != last_pe) {
	    fprintf(stderr,"%10lld: Switched %s protected mode\n", 
		    bx_pc_system.time_ticks(),
		    last_pe ? "from" : "to");
	    last_pe = !last_pe;
      }

      if(last_vm != BX_CPU(dbg_cpu)->eflags.vm) {
	    fprintf(stderr,"%10lld: %s V86 mode\n", 
		    bx_pc_system.time_ticks(), 
		    last_vm ? "Exited" : "Entered");
	    last_vm = !last_vm;
      }

      if(last_cr3 != BX_CPU(dbg_cpu)->cr3)
	    fprintf(stderr,"\n%10lld: Address space switched since last trigger. CR3: 0x%08x\n", 
		    bx_pc_system.time_ticks(), BX_CPU(dbg_cpu)->cr3);

      /* interrupts */
      if (dbg_show_mask & 0x40) {
	    if(BX_CPU(dbg_cpu)->show_flag & 0x4) {
		  fprintf(stderr,"%10lld:  softint %04x:%08x %08x\n", 
			  bx_pc_system.time_ticks(),
			  BX_CPU(dbg_cpu)->guard_found.cs,
			  BX_CPU(dbg_cpu)->guard_found.eip,
			  BX_CPU(dbg_cpu)->guard_found.laddr);
	    }
	    if((BX_CPU(dbg_cpu)->show_flag & 0x10) && !(BX_CPU(dbg_cpu)->show_flag & 0x4)) {
		  fprintf(stderr,"\n%10lld:  exception (not softint) %04x:%08x %08x\n", 
			  bx_pc_system.time_ticks(),
			  BX_CPU(dbg_cpu)->guard_found.cs,
			  BX_CPU(dbg_cpu)->guard_found.eip,
			  BX_CPU(dbg_cpu)->guard_found.laddr);
	    }
	    if(BX_CPU(dbg_cpu)->show_flag & 0x8) {
		  fprintf(stderr,"%10lld:  iret %04x:%08x %08x (from %08x)\n\n", 
			  bx_pc_system.time_ticks(),
			  BX_CPU(dbg_cpu)->guard_found.cs,
			  BX_CPU(dbg_cpu)->guard_found.eip,
			  BX_CPU(dbg_cpu)->guard_found.laddr,
			  BX_CPU(dbg_cpu)->show_eip);
	    }
      }
	
      /* calls */
      if(BX_CPU(dbg_cpu)->show_flag & 0x1) {
	    Bit32u phy = 0;
	    Boolean valid;

	    if (dbg_show_mask & 0x20) {
		  BX_CPU(dbg_cpu)->dbg_xlate_linear2phy(BX_CPU(dbg_cpu)->guard_found.laddr,
				       &phy, &valid);

		  fprintf(stderr,"%10lld:%*s call %04x:%08x 0x%08x (%08x) %s",
			  bx_pc_system.time_ticks(),
			  symbol_level+1," ",
			  BX_CPU(dbg_cpu)->guard_found.cs,
			  BX_CPU(dbg_cpu)->guard_found.eip,
			  BX_CPU(dbg_cpu)->guard_found.laddr,
			  phy,
			  bx_dbg_symbolic_address(BX_CPU(dbg_cpu)->cr3,
						  BX_CPU(dbg_cpu)->guard_found.eip,
						  BX_CPU(dbg_cpu)->guard_found.laddr - BX_CPU(dbg_cpu)->guard_found.eip) );
		  if(!valid)
			fprintf(stderr," phys not valid");
		  fprintf(stderr,"\n");
	    }

	    symbol_level++;
	    if(symbol_level > 40)
		  symbol_level = 10;
      }

      if (BX_CPU(dbg_cpu)->show_flag & 0x2) {
	    symbol_level--;
	    if(symbol_level < 0)
		  symbol_level = 0;
      }

      BX_CPU(dbg_cpu)->show_flag = 0;
      last_cr3 = BX_CPU(dbg_cpu)->cr3;
      return 0;
}
#endif

// BW added to dump page table

static void 
dbg_lin2phys(BX_CPU_C *cpu, Bit32u laddress, Bit32u *phy, Boolean *valid, Bit32u *tlb_phy, Boolean *tlb_valid) {
  Bit32u   lpf, ppf, poffset, TLB_index, paddress;
  Bit32u   pde, pde_addr;
  Bit32u   pte, pte_addr;
  
  *tlb_valid = 0;

  if (cpu->cr0.pg == 0) {
    *phy = laddress;
    *valid = 1;
    return;
    }

  lpf       = laddress & 0xfffff000; // linear page frame
  poffset   = laddress & 0x00000fff; // physical offset
  TLB_index = BX_TLB_INDEX_OF(lpf);

  // see if page is in the TLB first
  if (cpu->TLB.entry[TLB_index].lpf == lpf) {
	*tlb_phy        = cpu->TLB.entry[TLB_index].ppf | poffset;
	*tlb_valid = 1;
  }

  // Get page dir entry
  pde_addr = (cpu->cr3 & 0xfffff000) |
             ((laddress & 0xffc00000) >> 20);
  BX_MEM(0)->read_physical(cpu, pde_addr, 4, &pde);
  if ( !(pde & 0x01) ) {
    // Page Directory Entry NOT present
    goto page_fault;
    }

  // Get page table entry
  pte_addr = (pde & 0xfffff000) |
             ((laddress & 0x003ff000) >> 10);
  BX_MEM(0)->read_physical(cpu, pte_addr, 4, &pte);
  if ( !(pte & 0x01) ) {
    // Page Table Entry NOT present
    goto page_fault;
    }

  ppf = pte & 0xfffff000;
  paddress = ppf | poffset;

  *phy = paddress;
  *valid = 1;
  return;

page_fault:
  *phy = 0;
  *valid = 0;
  return;
}

static void dbg_dump_table(Boolean all) 
{
  Bit32u   lina;
  Bit32u phy, tlb_phy;
  Boolean valid, tlb_valid;

  Bit32u start_lina, start_phy;	// start of a valid translation interval

  if (BX_CPU(dbg_cpu)->cr0.pg == 0) {
	printf("paging off\n");
	return;
  }

  printf("cr3: %08x \n", BX_CPU(dbg_cpu)->cr3);

  lina = 0; 
  start_lina = 1;
  start_phy = 2;
  while(1) {
	dbg_lin2phys(BX_CPU(dbg_cpu), lina, &phy, &valid, &tlb_phy, &tlb_valid);
	if(valid) {
	      if( (lina - start_lina != phy - start_phy) || tlb_valid) {
		    if(all && (start_lina != 1))
			  printf("%08x - %08x: %8x - %8x\n",
				 start_lina, lina - 0x1000, start_phy, start_phy + (lina-0x1000-start_lina));
		    start_lina = lina;
		    start_phy = phy;
	      }
	      if(tlb_valid) {
		    if(all && tlb_phy == phy)
			  printf("%08x           : %8x (%8x) in TLB\n",
				 lina, phy, tlb_phy);
		    if(tlb_phy != phy)
			  printf("%08x           : %8x (%8x) in TLB Phys differs!!!\n",
				 lina, phy, tlb_phy);
		    start_lina = 1;
		    start_phy = 2;
	      }		    
	} else {
	      if(all && start_lina != 1)
		    printf("%08x - %08x: %8x - %8x\n",
			   start_lina, lina - 0x1000, start_phy, start_phy + (lina-0x1000-start_lina));
	      if(tlb_valid) {
		    printf("%08x           :          (%8x) in TLB  Table not valid!!!\n",
			   lina, tlb_phy);
	      }
	      start_lina = 1;
	      start_phy = 2;
	}

	if(lina == 0xfffff000)
	    break;
      lina += 0x1000;
  }
  if(all & start_lina != 1)
	printf("%08x - %08x: %8x - %8x\n",
	       start_lina, 0xfffff000, start_phy, start_phy + (0xfffff000-start_lina));
}

      

