//  Copyright (C) 2000  MandrakeSoft S.A.
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


// if including from C parser, need basic types etc
#ifndef __cplusplus
#include "config.h"
#endif

#if BX_USE_LOADER
#include "loader_misc.h"
void bx_dbg_loader(char *path, bx_loader_misc_t *misc_ptr);
#endif

#if BX_DBG_ICOUNT_SIZE == 32
  typedef Bit32u bx_dbg_icount_t;
#elif BX_DBG_ICOUNT_SIZE == 64
  typedef Bit64u bx_dbg_icount_t;
#else
#  error "BX_DBG_ICOUNT_SIZE incorrect."
#endif


#define BX_DBG_NO_HANDLE 1000



unsigned long crc32(unsigned char *buf, int len);

#if BX_DEBUGGER

// some strict C declarations needed by the parser/lexer
#ifdef __cplusplus
extern "C" {
#endif

// Flex defs
extern int bxlex(void);
extern char *bxtext;  // Using the pointer option rather than array
extern int bxwrap(void);
void bx_add_lex_input(char *buf);

// Yacc defs
extern int bxparse(void);
extern void bxerror(char *s);

char* bx_dbg_symbolic_address(Bit32u context, Bit32u eip, Bit32u base);
void bx_dbg_symbol_command(char* filename, Boolean global, Bit32u offset);
void bx_dbg_trace_on_command(void);
void bx_dbg_trace_off_command(void);
void bx_dbg_ptime_command(void);
void bx_dbg_timebp_command(Boolean absolute, Bit64u time);
void bx_dbg_diff_memory(void);
void bx_dbg_always_check(Bit32u page_start, Boolean on);
void bx_dbg_sync_memory(Boolean set);
void bx_dbg_sync_cpu(Boolean set);
void bx_dbg_fast_forward(Bit32u num);
void bx_dbg_info_address(Bit32u seg_reg_num, Bit32u offset);
#define MAX_CONCURRENT_BPS 5
extern int timebp_timer;
extern Bit64u timebp_queue[MAX_CONCURRENT_BPS];
extern int timebp_queue_size;
void bx_dbg_record_command(char*);
void bx_dbg_playback_command(char*);
void bx_dbg_modebp_command(char*); /* BW */
void bx_dbg_where_command(void);
void bx_dbg_print_string_command(Bit32u addr);
void bx_dbg_show_command(char*); /* BW */
void enter_playback_entry();
void bx_dbg_print_stack_command(int nwords);
void bx_dbg_watch(Boolean read, Bit32u address);
void bx_dbg_unwatch(Boolean read, Bit32u address);
void bx_dbg_continue_command(void);
void bx_dbg_stepN_command(bx_dbg_icount_t count);
void bx_dbg_set_command(char *p1, char *p2, char *p3);
void bx_dbg_del_breakpoint_command(unsigned handle);
void bx_dbg_vbreakpoint_command(Boolean specific, Bit32u cs, Bit32u eip);
void bx_dbg_lbreakpoint_command(Boolean specific, Bit32u laddress);
void bx_dbg_pbreakpoint_command(Boolean specific, Bit32u paddress);
void bx_dbg_info_bpoints_command(void);
void bx_dbg_quit_command(void);
void bx_dbg_info_program_command(void);
void bx_dbg_info_registers_command(void);
void bx_dbg_info_dirty_command(void);
void bx_dbg_info_control_regs_command(void);
void bx_dbg_info_linux_command(void);
void bx_dbg_examine_command(char *command, char *format, Boolean format_passed,
                    Bit32u addr, Boolean addr_passed, int simulator);
void bx_dbg_setpmem_command(Bit32u addr, unsigned len, Bit32u val);
void bx_dbg_set_symbol_command(char *symbol, Bit32u val);
void bx_dbg_query_command(char *);
void bx_dbg_take_command(char *, unsigned n);
void bx_dbg_dump_cpu_command(void);
void bx_dbg_set_cpu_command(void);
void bx_dbg_disassemble_command(Bit32u addr1, Bit32u addr2);
void bx_dbg_instrument_command(char *);
void bx_dbg_loader_command(char *);
void bx_dbg_doit_command(unsigned);
void bx_dbg_crc_command(Bit32u addr1, Bit32u addr2);
void bx_dbg_maths_command(char *command, int data1, int data2);
void bx_dbg_maths_expression_command(char *expr);
void bx_dbg_v2l_command(unsigned seg_no, Bit32u offset);
extern Boolean watchpoint_continue;
void bx_dbg_linux_syscall ();

#ifdef __cplusplus
}
#endif

// the rest for C++
#ifdef __cplusplus

// (mch) Read/write watchpoint hack
#define MAX_WRITE_WATCHPOINTS 16
#define MAX_READ_WATCHPOINTS 16
extern int num_write_watchpoints;
extern Bit32u write_watchpoint[MAX_WRITE_WATCHPOINTS];
extern int num_read_watchpoints;
extern Bit32u read_watchpoint[MAX_READ_WATCHPOINTS];

typedef enum {
      STOP_NO_REASON = 0, STOP_TIME_BREAK_POINT, STOP_READ_WATCH_POINT, STOP_WRITE_WATCH_POINT, STOP_MAGIC_BREAK_POINT, STOP_TRACE, STOP_MODE_BREAK_POINT
} stop_reason_t;

typedef enum {
      BREAK_POINT_MAGIC, BREAK_POINT_READ, BREAK_POINT_WRITE, BREAK_POINT_TIME
} break_point_t;

#define BX_DBG_REG_EAX          10
#define BX_DBG_REG_ECX          11
#define BX_DBG_REG_EDX          12
#define BX_DBG_REG_EBX          13
#define BX_DBG_REG_ESP          14
#define BX_DBG_REG_EBP          15
#define BX_DBG_REG_ESI          16
#define BX_DBG_REG_EDI          17
#define BX_DBG_REG_EIP          18
#define BX_DBG_REG_EFLAGS       19
#define BX_DBG_REG_CS           20
#define BX_DBG_REG_SS           21
#define BX_DBG_REG_DS           22
#define BX_DBG_REG_ES           23
#define BX_DBG_REG_FS           24
#define BX_DBG_REG_GS           25


#define BX_DBG_PENDING_DMA 1
#define BX_DBG_PENDING_IRQ 2




void bx_dbg_exit(int code);
#if BX_DBG_EXTENSIONS
  int bx_dbg_extensions(char *command);
#else
#define bx_dbg_extensions(command) 0
#endif


//
// code for guards...
//

#define BX_DBG_GUARD_INSTR_BEGIN   0x0001
#define BX_DBG_GUARD_INSTR_END     0x0002
#define BX_DBG_GUARD_EXCEP_BEGIN   0x0004
#define BX_DBG_GUARD_EXCEP_END     0x0008
#define BX_DBG_GUARD_INTER_BEGIN   0x0010
#define BX_DBG_GUARD_INTER_END     0x0020
#define BX_DBG_GUARD_INSTR_MAP     0x0040

// following 3 go along with BX_DBG_GUARD_INSTR_BEGIN
// to provide breakpointing
#define BX_DBG_GUARD_IADDR_VIR     0x0080
#define BX_DBG_GUARD_IADDR_LIN     0x0100
#define BX_DBG_GUARD_IADDR_PHY     0x0200
#define BX_DBG_GUARD_IADDR_ALL (BX_DBG_GUARD_IADDR_VIR | \
                                BX_DBG_GUARD_IADDR_LIN | \
                                BX_DBG_GUARD_IADDR_PHY)

#define BX_DBG_GUARD_ICOUNT        0x0400
#define BX_DBG_GUARD_CTRL_C        0x0800


typedef struct {
  unsigned long guard_for;

  // instruction address breakpoints
  struct {
#if BX_DBG_SUPPORT_VIR_BPOINT
    unsigned num_virtual;
    struct {
      Bit32u cs;  // only use 16 bits
      Bit32u eip;
      unsigned bpoint_id;
      } vir[BX_DBG_MAX_VIR_BPOINTS];
#endif

#if BX_DBG_SUPPORT_LIN_BPOINT
    unsigned num_linear;
    struct {
      Bit32u addr;
      unsigned bpoint_id;
      } lin[BX_DBG_MAX_LIN_BPOINTS];
#endif

#if BX_DBG_SUPPORT_PHY_BPOINT
    unsigned num_physical;
    struct {
      Bit32u addr;
      unsigned bpoint_id;
      } phy[BX_DBG_MAX_PHY_BPOINTS];
#endif
    } iaddr;

  bx_dbg_icount_t icount; // stop after completing this many instructions

  // user typed Ctrl-C, requesting simulator stop at next convient spot
  volatile Boolean interrupt_requested;

  // booleans to control whether simulator should report events
  // to debug controller
  struct {
   Boolean irq;
   Boolean a20;
   Boolean io;
   Boolean ucmem;
   Boolean dma;
   } report;

  struct {
    Boolean irq;  // should process IRQs asynchronously
    Boolean dma;  // should process DMAs asynchronously
    } async;

#define BX_DBG_ASYNC_PENDING_A20   0x01
#define BX_DBG_ASYNC_PENDING_RESET 0x02
#define BX_DBG_ASYNC_PENDING_NMI   0x04

  // Asynchronous changes which are pending.  These are Q'd by
  // the debugger, as the master simulator is notified of a pending
  // async change.  At the simulator's next point, where it checks for
  // such events, it notifies the debugger with acknowlegement.  This
  // field contains a logically or'd list of all events which should
  // be checked, and ack'd.
  struct {
    unsigned which; // logical OR of above constants
    Boolean a20;
    Boolean reset;
    Boolean nmi;
    } async_changes_pending;
  } bx_guard_t;

// working information for each simulator to update when a guard
// is reached (found)
typedef struct {
  unsigned long guard_found;
  unsigned iaddr_index;
  bx_dbg_icount_t icount; // number of completed instructions
  Bit32u   cs;     // cs:eip and linear addr of instruction at guard point
  Bit32u   eip;
  Bit32u   laddr;
  Boolean  is_32bit_code; // CS seg size at guard point
  Boolean  ctrl_c; // simulator stopped due to Ctrl-C request
  } bx_guard_found_t;

extern bx_guard_t        bx_guard;
extern bx_guard_found_t  bx_guard_found[];





int  bx_dbg_main(int argc, char *argv[]);
void bx_dbg_user_input_loop(void);


typedef struct {
  Bit16u sel;
  Bit32u des_l, des_h, valid;
  } bx_dbg_sreg_t;

typedef struct {
    Bit32u eax;
    Bit32u ebx;
    Bit32u ecx;
    Bit32u edx;
    Bit32u ebp;
    Bit32u esi;
    Bit32u edi;
    Bit32u esp;
    Bit32u eflags;
    Bit32u eip;
    bx_dbg_sreg_t cs;
    bx_dbg_sreg_t ss;
    bx_dbg_sreg_t ds;
    bx_dbg_sreg_t es;
    bx_dbg_sreg_t fs;
    bx_dbg_sreg_t gs;
    bx_dbg_sreg_t ldtr;
    bx_dbg_sreg_t tr;
    struct { Bit32u base, limit; } gdtr;
    struct { Bit32u base, limit; } idtr;
    Bit32u dr0, dr1, dr2, dr3, dr6, dr7;
    Bit32u tr3, tr4, tr5, tr6, tr7;
    Bit32u cr0, cr1, cr2, cr3, cr4;
    unsigned inhibit_mask;
    } bx_dbg_cpu_t;




typedef struct {
  // call back functions specific to each simulator
  Boolean (*setphymem)(Bit32u addr, unsigned len, Bit8u *buf);
  Boolean (*getphymem)(Bit32u addr, unsigned len, Bit8u *buf);
  void    (*xlate_linear2phy)(Bit32u linear, Bit32u *phy, Boolean *valid);
  Boolean (*set_reg)(unsigned reg, Bit32u val);
  Bit32u  (*get_reg)(unsigned reg);
  Boolean (*get_sreg)(bx_dbg_sreg_t *sreg, unsigned sreg_no);
  Boolean (*set_cpu)(bx_dbg_cpu_t *cpu);
  Boolean (*get_cpu)(bx_dbg_cpu_t *cpu);
  unsigned       dirty_page_tbl_size;
  unsigned char *dirty_page_tbl;
  void    (*atexit)(void);
  unsigned (*query_pending)(void);
  void     (*execute)(void);
  void     (*take_irq)(void);
  void     (*take_dma)(void);
  void     (*reset_cpu)(unsigned source);
  void     (*init_mem)(int size_in_bytes);
  void     (*load_ROM)(const char *path, Bit32u romaddress);

  // for asynchronous environment handling
  void     (*set_A20)(unsigned val);
  void     (*set_NMI)(unsigned val);
  void     (*set_RESET)(unsigned val);
  void     (*set_INTR)(unsigned val);
  void     (*force_interrupt)(unsigned vector);

#if BX_INSTRUMENTATION
  void    (*instr_start)(void);
  void    (*instr_stop)(void);
  void    (*instr_reset)(void);
  void    (*instr_print)(void);
#endif
#if BX_USE_LOADER
  void    (*loader)(char *path, bx_loader_misc_t *misc_ptr);
#endif
  Boolean (*crc32)(unsigned long (*f)(unsigned char *buf, int len),
                   Bit32u addr1, Bit32u addr2, Bit32u *crc);
  } bx_dbg_callback_t;

extern bx_dbg_callback_t bx_dbg_callback[BX_NUM_SIMULATORS];

void BX_SIM1_INIT(bx_dbg_callback_t *, int argc, char *argv[]);
#ifdef BX_SIM2_INIT
void BX_SIM2_INIT(bx_dbg_callback_t *, int argc, char *argv[]);
#endif


void bx_dbg_dma_report(Bit32u addr, unsigned len, unsigned what, Bit32u val);
void bx_dbg_iac_report(unsigned vector, unsigned irq);
void bx_dbg_a20_report(unsigned val);
void bx_dbg_io_report(Bit32u addr, unsigned size, unsigned op, Bit32u val);
void bx_dbg_ucmem_report(Bit32u addr, unsigned size, unsigned op, Bit32u val);

Bit8u   bx_dbg_ucmem_read(Bit32u addr);
void    bx_dbg_ucmem_write(Bit32u addr, Bit8u value);
void    bx_dbg_async_pin_request(unsigned what, Boolean val);
void    bx_dbg_async_pin_ack(unsigned what, Boolean val);
Bit32u  bx_dbg_inp(Bit16u addr, unsigned len);
void    bx_dbg_outp(Bit16u addr, Bit32u value, unsigned len);
void    bx_dbg_raise_HLDA(void);
Bit8u   bx_dbg_IAC(void);
void    bx_dbg_set_INTR(Boolean b);

int bx_dbg_symbolic_output(void); /* BW */
#endif // #ifdef __cplusplus
#endif // #if BX_DEBUGGER
