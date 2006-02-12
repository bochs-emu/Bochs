/////////////////////////////////////////////////////////////////////////
// $Id: debug.h,v 1.20 2006-02-12 20:21:36 sshwarts Exp $
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


// if including from C parser, need basic types etc
#include "config.h"
#include "osdep.h"

#if BX_DBG_ICOUNT_SIZE == 32
  typedef Bit32u bx_dbg_icount_t;
#elif BX_DBG_ICOUNT_SIZE == 64
  typedef Bit64u bx_dbg_icount_t;
#else
#  error "BX_DBG_ICOUNT_SIZE incorrect."
#endif

#define BX_DBG_NO_HANDLE 1000

extern Bit32u dbg_cpu;

Bit32u crc32(const Bit8u *buf, int len);

#if BX_DEBUGGER

// some strict C declarations needed by the parser/lexer
#ifdef __cplusplus
extern "C" {
#endif

void dbg_printf (const char *fmt, ...);

typedef enum {
  BX_DBG_SREG_ES,
  BX_DBG_SREG_CS,
  BX_DBG_SREG_SS,
  BX_DBG_SREG_DS,
  BX_DBG_SREG_FS,
  BX_DBG_SREG_GS
} SRegs;

#if BX_SUPPORT_X86_64
# define BX_DBG_GEN_REGISTERS 16
#else
# define BX_DBG_GEN_REGISTERS 8
#endif

typedef enum {
  BX_DBG_REG8H_AH,
  BX_DBG_REG8H_CH,
  BX_DBG_REG8H_DH,
  BX_DBG_REG8H_BH,
} Regs8H;

#if BX_SUPPORT_X86_64

typedef enum {
  BX_DBG_REG8L_AL,
  BX_DBG_REG8L_CL,
  BX_DBG_REG8L_DL,
  BX_DBG_REG8L_BL,
  BX_DBG_REG8L_SPL,
  BX_DBG_REG8L_BPL,
  BX_DBG_REG8L_SIL,
  BX_DBG_REG8L_DIL,
  BX_DBG_REG8L_R8,
  BX_DBG_REG8L_R9,
  BX_DBG_REG8L_R10,
  BX_DBG_REG8L_R11,
  BX_DBG_REG8L_R12,
  BX_DBG_REG8L_R13,
  BX_DBG_REG8L_R14,
  BX_DBG_REG8L_R15
} Regs8L;

typedef enum {
  BX_DBG_REG16_AX,
  BX_DBG_REG16_CX,
  BX_DBG_REG16_DX,
  BX_DBG_REG16_BX,
  BX_DBG_REG16_SP,
  BX_DBG_REG16_BP,
  BX_DBG_REG16_SI,
  BX_DBG_REG16_DI,
  BX_DBG_REG16_R8,
  BX_DBG_REG16_R9,
  BX_DBG_REG16_R10,
  BX_DBG_REG16_R11,
  BX_DBG_REG16_R12,
  BX_DBG_REG16_R13,
  BX_DBG_REG16_R14,
  BX_DBG_REG16_R15
} Regs16;

typedef enum {
  BX_DBG_REG32_EAX,
  BX_DBG_REG32_ECX,
  BX_DBG_REG32_EDX,
  BX_DBG_REG32_EBX,
  BX_DBG_REG32_ESP,
  BX_DBG_REG32_EBP,
  BX_DBG_REG32_ESI,
  BX_DBG_REG32_EDI,
  BX_DBG_REG32_R8,
  BX_DBG_REG32_R9,
  BX_DBG_REG32_R10,
  BX_DBG_REG32_R11,
  BX_DBG_REG32_R12,
  BX_DBG_REG32_R13,
  BX_DBG_REG32_R14,
  BX_DBG_REG32_R15
} Regs32;

typedef enum {
  BX_DBG_REG64_RAX,
  BX_DBG_REG64_RCX,
  BX_DBG_REG64_RDX,
  BX_DBG_REG64_RBX,
  BX_DBG_REG64_RSP,
  BX_DBG_REG64_RBP,
  BX_DBG_REG64_RSI,
  BX_DBG_REG64_RDI,
  BX_DBG_REG64_R8,
  BX_DBG_REG64_R9,
  BX_DBG_REG64_R10,
  BX_DBG_REG64_R11,
  BX_DBG_REG64_R12,
  BX_DBG_REG64_R13,
  BX_DBG_REG64_R14,
  BX_DBG_REG64_R15
} Regs64;

#else

typedef enum {
  BX_DBG_REG8L_AL,
  BX_DBG_REG8L_CL,
  BX_DBG_REG8L_DL,
  BX_DBG_REG8L_BL
} Regs8L;

typedef enum {
  BX_DBG_REG16_AX,
  BX_DBG_REG16_CX,
  BX_DBG_REG16_DX,
  BX_DBG_REG16_BX,
  BX_DBG_REG16_SP,
  BX_DBG_REG16_BP,
  BX_DBG_REG16_SI,
  BX_DBG_REG16_DI
} Regs16;

typedef enum {
  BX_DBG_REG32_EAX,
  BX_DBG_REG32_ECX,
  BX_DBG_REG32_EDX,
  BX_DBG_REG32_EBX,
  BX_DBG_REG32_ESP,
  BX_DBG_REG32_EBP,
  BX_DBG_REG32_ESI,
  BX_DBG_REG32_EDI
} Regs32;

#endif

typedef enum
{
  bkRegular,
  bkAtIP,
  bkStepOver
} BreakpointKind;

typedef enum _show_flags {
  Flag_call    = 0x1,
  Flag_ret     = 0x2,
  Flag_softint = 0x4,
  Flag_iret    = 0x8,
  Flag_intsig  = 0x10,
  Flag_mode    = 0x20
} show_flags_t;

// Flex defs
extern int bxlex(void);
extern char *bxtext;  // Using the pointer option rather than array
extern int bxwrap(void);
void bx_add_lex_input(char *buf);

// Yacc defs
extern int bxparse(void);
extern void bxerror(char *s);

typedef struct {
  Bit64s from;
  Bit64s to;
} bx_num_range;
#define EMPTY_ARG (-1)

Bit16u bx_dbg_get_selector_value(unsigned int seg_no);
Bit16u bx_dbg_get_ip (void);
Bit32u bx_dbg_get_eip(void);
bx_address bx_dbg_get_intruction_pointer(void);
Bit8u bx_dbg_get_reg8l_value(unsigned reg);
Bit8u bx_dbg_get_reg8h_value(unsigned reg);
Bit16u bx_dbg_get_reg16_value(unsigned reg);
Bit32u bx_dbg_get_reg32_value(unsigned reg);
void bx_dbg_set_reg8l_value(unsigned reg, Bit8u value);
void bx_dbg_set_reg8h_value(unsigned reg, Bit8u value);
void bx_dbg_set_reg16_value(unsigned reg, Bit16u value);
void bx_dbg_set_reg32_value(unsigned reg, Bit32u value);
Bit32u bx_dbg_get_laddr(Bit16u sel, Bit32u ofs);
void bx_dbg_step_over_command(void);
bx_num_range make_num_range (Bit64s from, Bit64s to);
void bx_dbg_trace_command(bx_bool enable);
void bx_dbg_trace_reg_command(bx_bool enable);
void bx_dbg_ptime_command(void);
void bx_dbg_timebp_command(bx_bool absolute, Bit64u time);
#define MAX_CONCURRENT_BPS 5
extern int timebp_timer;
extern Bit64u timebp_queue[MAX_CONCURRENT_BPS];
extern int timebp_queue_size;
void bx_dbg_record_command(char*);
void bx_dbg_playback_command(char*);
void bx_dbg_modebp_command(void);
void bx_dbg_where_command(void);
void bx_dbg_print_string_command(bx_address addr);
void bx_dbg_show_command(const char*);
void bx_dbg_print_stack_command(unsigned nwords);
void bx_dbg_watch(int read, Bit32u address);
void bx_dbg_unwatch(int read, Bit32u address);
void bx_dbg_continue_command(void);
void bx_dbg_stepN_command(bx_dbg_icount_t count);
void bx_dbg_set_auto_disassemble(bx_bool enable);
void bx_dbg_disassemble_switch_mode(void);
void bx_dbg_set_disassemble_size(unsigned size);
void bx_dbg_del_breakpoint_command(unsigned handle);
void bx_dbg_en_dis_breakpoint_command(unsigned handle, bx_bool enable);
bx_bool bx_dbg_en_dis_pbreak (unsigned handle, bx_bool enable);
bx_bool bx_dbg_en_dis_lbreak (unsigned handle, bx_bool enable);
bx_bool bx_dbg_en_dis_vbreak (unsigned handle, bx_bool enable);
bx_bool bx_dbg_del_pbreak(unsigned handle);
bx_bool bx_dbg_del_lbreak (unsigned handle);
bx_bool bx_dbg_del_vbreak (unsigned handle);
int bx_dbg_vbreakpoint_command(BreakpointKind bk, Bit32u cs, bx_address eip);
int bx_dbg_lbreakpoint_command(BreakpointKind bk, bx_address laddress);
int bx_dbg_pbreakpoint_command(BreakpointKind bk, Bit32u paddress);
void bx_dbg_info_bpoints_command(void);
void bx_dbg_quit_command(void);
#define BX_INFO_CPU_REGS 1   /* bitmasks - choices for bx_dbg_info_registers_command */
#define BX_INFO_FPU_REGS 2
#define BX_INFO_SSE_REGS 4
void bx_dbg_info_registers_command(int); 
void bx_dbg_info_dirty_command(void);
void bx_dbg_info_idt_command(bx_num_range);
void bx_dbg_info_gdt_command(bx_num_range);
void bx_dbg_info_ldt_command(bx_num_range);
void bx_dbg_info_tss_command(bx_num_range);
void bx_dbg_info_control_regs_command(void);
void bx_dbg_info_flags(void);
void bx_dbg_info_linux_command(void);
void bx_dbg_examine_command(char *command, char *format, bx_bool format_passed,
                    Bit32u addr, bx_bool addr_passed);
void bx_dbg_setpmem_command(Bit32u addr, unsigned len, Bit32u val);
void bx_dbg_query_command(const char *);
void bx_dbg_take_command(const char *, unsigned n);
void bx_dbg_dump_cpu_command(void);
void bx_dbg_set_cpu_command(void);
void bx_dbg_disassemble_command(const char *,bx_num_range);
void bx_dbg_instrument_command(const char *);
void bx_dbg_doit_command(unsigned);
void bx_dbg_crc_command(Bit32u addr1, Bit32u addr2);
extern bx_bool watchpoint_continue;
void bx_dbg_linux_syscall(unsigned which_cpu);
void bx_dbg_info_ne2k(int page, int reg);
void bx_dbg_info_pic(void);
void bx_dbg_info_vga(void);
void bx_dbg_print_help(void);
void bx_dbg_calc_command(Bit64u value);
void bx_dbg_info_ivt_command(bx_num_range);
void bx_dbg_dump_table(void);
int bx_dbg_show_symbolic(void);
void bx_dbg_set_symbol_command(char *symbol, Bit32u val);
char* bx_dbg_symbolic_address(Bit32u context, Bit32u eip, Bit32u base);
char* bx_dbg_symbolic_address_16bit(Bit32u eip, Bit32u cs);
void bx_dbg_symbol_command(char* filename, bx_bool global, Bit32u offset);
void bx_dbg_info_symbols_command(char *Symbol);
int bx_dbg_lbreakpoint_symbol_command(char *Symbol);
Bit32u bx_dbg_get_symbol_value(char *Symbol);
char* bx_dbg_disasm_symbolic_address(Bit32u eip, Bit32u base);

#ifdef __cplusplus
}
#endif

// the rest for C++
#ifdef __cplusplus

// Read/write watchpoint hack
#define MAX_WRITE_WATCHPOINTS 16
#define MAX_READ_WATCHPOINTS 16
extern int num_write_watchpoints;
extern Bit32u write_watchpoint[MAX_WRITE_WATCHPOINTS];
extern int num_read_watchpoints;
extern Bit32u read_watchpoint[MAX_READ_WATCHPOINTS];

typedef enum {
      STOP_NO_REASON = 0, STOP_TIME_BREAK_POINT, STOP_READ_WATCH_POINT,
      STOP_WRITE_WATCH_POINT, STOP_MAGIC_BREAK_POINT,
      STOP_MODE_BREAK_POINT, STOP_CPU_HALTED, STOP_CPU_PANIC
} stop_reason_t;

typedef enum {
      BREAK_POINT_MAGIC, BREAK_POINT_READ, BREAK_POINT_WRITE, BREAK_POINT_TIME
} break_point_t;

#define BX_DBG_REG_EIP          10
#define BX_DBG_REG_EFLAGS       11
#define BX_DBG_REG_CS           20
#define BX_DBG_REG_SS           21
#define BX_DBG_REG_DS           22
#define BX_DBG_REG_ES           23
#define BX_DBG_REG_FS           24
#define BX_DBG_REG_GS           25

#define BX_DBG_PENDING_DMA 1
#define BX_DBG_PENDING_IRQ 2

void bx_debug_break ();

void bx_dbg_exit(int code);
#if BX_DBG_EXTENSIONS
  int bx_dbg_extensions(char *command);
#else
#define bx_dbg_extensions(command) 0
#endif

//
// code for guards...
//

#define BX_DBG_GUARD_IADDR_VIR     0x0001
#define BX_DBG_GUARD_IADDR_LIN     0x0002
#define BX_DBG_GUARD_IADDR_PHY     0x0004
#define BX_DBG_GUARD_IADDR_ALL (BX_DBG_GUARD_IADDR_VIR | \
                                BX_DBG_GUARD_IADDR_LIN | \
                                BX_DBG_GUARD_IADDR_PHY)

#define BX_DBG_GUARD_ICOUNT        0x0100
#define BX_DBG_GUARD_CTRL_C        0x0200

typedef struct {
  unsigned long guard_for;

  // instruction address breakpoints
  struct {
#if BX_DBG_SUPPORT_VIR_BPOINT
    unsigned num_virtual;
    struct {
      Bit32u cs;  // only use 16 bits
      bx_address eip;
      unsigned bpoint_id;
      bx_bool enabled;
    } vir[BX_DBG_MAX_VIR_BPOINTS];
#endif

#if BX_DBG_SUPPORT_LIN_BPOINT
    unsigned num_linear;
    struct {
      bx_address addr;
      unsigned bpoint_id;
      bx_bool enabled;
    } lin[BX_DBG_MAX_LIN_BPOINTS];
#endif

#if BX_DBG_SUPPORT_PHY_BPOINT
    unsigned num_physical;
    struct {
      Bit32u addr; // physical address in 32 bits only
      unsigned bpoint_id;
      bx_bool enabled;
    } phy[BX_DBG_MAX_PHY_BPOINTS];
#endif
  } iaddr;

  bx_dbg_icount_t icount; // stop after completing this many instructions

  // user typed Ctrl-C, requesting simulator stop at next convient spot
  volatile bx_bool interrupt_requested;

  // when a triple fault occurs, Bochs panics.  If you continue through
  // the panic, it will generally produce another exception and panic
  // again at an even deeper stack level.  To recover from this potentially
  // infinite recursion, I set special_unwind_stack to true.  This causes
  // the interrupt() and exception() functions to return immediately instead
  // of creating more exception conditions, and allows the user to reenter the
  // debugger after the triple fault.  Note that special_unwind_stack causes
  // bochs to NOT emulate the hardware behavior correctly.  The correct
  // behavior would be to reboot.  (Rebooting, if it is ever implemented,
  // will need some kind of unwinding too.)
  bx_bool special_unwind_stack;

  // booleans to control whether simulator should report events
  // to debug controller
  struct {
    bx_bool irq;
    bx_bool a20;
    bx_bool io;
    bx_bool ucmem;
    bx_bool dma;
  } report;

  struct {
    bx_bool irq;  // should process IRQs asynchronously
    bx_bool dma;  // should process DMAs asynchronously
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
    bx_bool a20;
    bx_bool reset;
    bx_bool nmi;
  } async_changes_pending;
} bx_guard_t;

// working information for each simulator to update when a guard
// is reached (found)
typedef struct bx_guard_found_t {
  unsigned long guard_found;
  unsigned iaddr_index;
  bx_dbg_icount_t icount; // number of completed instructions
  Bit32u  cs; // cs:eip and linear addr of instruction at guard point
  bx_address eip;
  bx_address laddr;
  bx_bool is_32bit_code; // CS seg size at guard point
  bx_bool is_64bit_code;
  bx_bool ctrl_c; // simulator stopped due to Ctrl-C request
  Bit64u  time_tick; // time tick when guard reached
} bx_guard_found_t;

extern bx_guard_t bx_guard;

int  bx_dbg_main(int argc, char *argv[]);
void bx_dbg_user_input_loop(void);
void bx_dbg_interpret_line (char *cmd);

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
    Bit32u cr0, cr1, cr2, cr3, cr4;
    unsigned inhibit_mask;
} bx_dbg_cpu_t;

void bx_dbg_dma_report(Bit32u addr, unsigned len, unsigned what, Bit32u val);
void bx_dbg_iac_report(unsigned vector, unsigned irq);
void bx_dbg_a20_report(unsigned val);
void bx_dbg_io_report(Bit32u addr, unsigned size, unsigned op, Bit32u val);
void bx_dbg_ucmem_report(Bit32u addr, unsigned size, unsigned op, Bit32u val);
void bx_dbg_disassemble_current(int which_cpu, int print_time);

#endif // #ifdef __cplusplus

#endif // #if BX_DEBUGGER
