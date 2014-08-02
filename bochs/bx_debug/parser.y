/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////

%{
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

#if BX_DEBUGGER
%}

%union {
  char    *sval;
  Bit64u   uval;
  bx_bool  bval;
}

// Common registers
%type <uval> BX_TOKEN_NONSEG_REG
%type <uval> BX_TOKEN_SEGREG
%type <bval> BX_TOKEN_TOGGLE_ON_OFF
%type <sval> BX_TOKEN_REGISTERS

%token <uval> BX_TOKEN_8BH_REG
%token <uval> BX_TOKEN_8BL_REG
%token <uval> BX_TOKEN_16B_REG
%token <uval> BX_TOKEN_32B_REG
%token <uval> BX_TOKEN_64B_REG
%token <uval> BX_TOKEN_CS
%token <uval> BX_TOKEN_ES
%token <uval> BX_TOKEN_SS
%token <uval> BX_TOKEN_DS
%token <uval> BX_TOKEN_FS
%token <uval> BX_TOKEN_GS
%token <uval> BX_TOKEN_OPMASK_REG
%token <uval> BX_TOKEN_FLAGS
%token <bval> BX_TOKEN_ON
%token <bval> BX_TOKEN_OFF
%token <sval> BX_TOKEN_CONTINUE
%token <sval> BX_TOKEN_STEPN
%token <sval> BX_TOKEN_STEP_OVER
%token <sval> BX_TOKEN_SET
%token <sval> BX_TOKEN_DEBUGGER
%token <sval> BX_TOKEN_LIST_BREAK
%token <sval> BX_TOKEN_VBREAKPOINT
%token <sval> BX_TOKEN_LBREAKPOINT
%token <sval> BX_TOKEN_PBREAKPOINT
%token <sval> BX_TOKEN_DEL_BREAKPOINT
%token <sval> BX_TOKEN_ENABLE_BREAKPOINT
%token <sval> BX_TOKEN_DISABLE_BREAKPOINT
%token <sval> BX_TOKEN_INFO
%token <sval> BX_TOKEN_QUIT
%token <sval> BX_TOKEN_R
%token <sval> BX_TOKEN_REGS
%token <sval> BX_TOKEN_CPU
%token <sval> BX_TOKEN_FPU
%token <sval> BX_TOKEN_MMX
%token <sval> BX_TOKEN_XMM
%token <sval> BX_TOKEN_YMM
%token <sval> BX_TOKEN_ZMM
%token <sval> BX_TOKEN_AVX
%token <sval> BX_TOKEN_IDT
%token <sval> BX_TOKEN_IVT
%token <sval> BX_TOKEN_GDT
%token <sval> BX_TOKEN_LDT
%token <sval> BX_TOKEN_TSS
%token <sval> BX_TOKEN_TAB
%token <sval> BX_TOKEN_ALL
%token <sval> BX_TOKEN_LINUX
%token <sval> BX_TOKEN_DEBUG_REGS
%token <sval> BX_TOKEN_CONTROL_REGS
%token <sval> BX_TOKEN_SEGMENT_REGS
%token <sval> BX_TOKEN_EXAMINE
%token <sval> BX_TOKEN_XFORMAT
%token <sval> BX_TOKEN_DISFORMAT
%token <sval> BX_TOKEN_RESTORE
%token <sval> BX_TOKEN_WRITEMEM
%token <sval> BX_TOKEN_SETPMEM
%token <sval> BX_TOKEN_SYMBOLNAME
%token <sval> BX_TOKEN_QUERY
%token <sval> BX_TOKEN_PENDING
%token <sval> BX_TOKEN_TAKE
%token <sval> BX_TOKEN_DMA
%token <sval> BX_TOKEN_IRQ
%token <sval> BX_TOKEN_TLB
%token <sval> BX_TOKEN_HEX
%token <sval> BX_TOKEN_DISASM
%token <sval> BX_TOKEN_INSTRUMENT
%token <sval> BX_TOKEN_STRING
%token <sval> BX_TOKEN_STOP
%token <sval> BX_TOKEN_DOIT
%token <sval> BX_TOKEN_CRC
%token <sval> BX_TOKEN_TRACE
%token <sval> BX_TOKEN_TRACEREG
%token <sval> BX_TOKEN_TRACEMEM
%token <sval> BX_TOKEN_SWITCH_MODE
%token <sval> BX_TOKEN_SIZE
%token <sval> BX_TOKEN_PTIME
%token <sval> BX_TOKEN_TIMEBP_ABSOLUTE
%token <sval> BX_TOKEN_TIMEBP
%token <sval> BX_TOKEN_MODEBP
%token <sval> BX_TOKEN_VMEXITBP
%token <sval> BX_TOKEN_PRINT_STACK
%token <sval> BX_TOKEN_WATCH
%token <sval> BX_TOKEN_UNWATCH
%token <sval> BX_TOKEN_READ
%token <sval> BX_TOKEN_WRITE
%token <sval> BX_TOKEN_SHOW
%token <sval> BX_TOKEN_LOAD_SYMBOLS
%token <sval> BX_TOKEN_SYMBOLS
%token <sval> BX_TOKEN_LIST_SYMBOLS
%token <sval> BX_TOKEN_GLOBAL
%token <sval> BX_TOKEN_WHERE
%token <sval> BX_TOKEN_PRINT_STRING
%token <uval> BX_TOKEN_NUMERIC
%token <sval> BX_TOKEN_PAGE
%token <sval> BX_TOKEN_HELP
%token <sval> BX_TOKEN_CALC
%token <sval> BX_TOKEN_DEVICE
%token <sval> BX_TOKEN_GENERIC
%token BX_TOKEN_RSHIFT
%token BX_TOKEN_LSHIFT
%token BX_TOKEN_REG_IP
%token BX_TOKEN_REG_EIP
%token BX_TOKEN_REG_RIP
%type <uval> optional_numeric
%type <uval> vexpression
%type <uval> expression

%left '+' '-' '|' '^'
%left '*' '/' '&' BX_TOKEN_LSHIFT BX_TOKEN_RSHIFT
%left NOT NEG INDIRECT

%start commands

%%
commands:
      commands command
    | command
;

command:
      continue_command
    | stepN_command
    | step_over_command
    | set_command
    | breakpoint_command
    | info_command
    | regs_command
    | fpu_regs_command
    | mmx_regs_command
    | xmm_regs_command
    | ymm_regs_command
    | zmm_regs_command
    | segment_regs_command
    | debug_regs_command
    | control_regs_command
    | blist_command
    | slist_command
    | delete_command
    | bpe_command
    | bpd_command
    | quit_command
    | examine_command
    | restore_command
    | writemem_command
    | setpmem_command
    | query_command
    | take_command
    | disassemble_command
    | instrument_command
    | doit_command
    | crc_command
    | trace_command
    | trace_reg_command
    | trace_mem_command
    | ptime_command
    | timebp_command
    | modebp_command
    | vmexitbp_command
    | print_stack_command
    | watch_point_command
    | page_command
    | tlb_command
    | show_command
    | symbol_command
    | where_command
    | print_string_command
    | help_command
    | calc_command
    | 
    | '\n'
      {
      }
    ;

BX_TOKEN_TOGGLE_ON_OFF:
      BX_TOKEN_ON
    | BX_TOKEN_OFF
    { $$=$1; }
;

BX_TOKEN_REGISTERS:
      BX_TOKEN_R
    | BX_TOKEN_REGS
    { $$=$1; }
;

BX_TOKEN_SEGREG:
      BX_TOKEN_CS
    | BX_TOKEN_ES
    | BX_TOKEN_SS
    | BX_TOKEN_DS
    | BX_TOKEN_FS
    | BX_TOKEN_GS
    { $$=$1; }
;

timebp_command:
      BX_TOKEN_TIMEBP expression '\n'
      {
          bx_dbg_timebp_command(0, $2);
          free($1);
      }
    | BX_TOKEN_TIMEBP_ABSOLUTE expression '\n'
      {
          bx_dbg_timebp_command(1, $2);
          free($1);
      }
    ;

modebp_command:
      BX_TOKEN_MODEBP '\n'
      {
          bx_dbg_modebp_command();
          free($1);
      }
    ;
    
vmexitbp_command:
      BX_TOKEN_VMEXITBP '\n'
      {
          bx_dbg_vmexitbp_command();
          free($1);
      }
    ;
    
show_command:
      BX_TOKEN_SHOW BX_TOKEN_GENERIC '\n'
      {
          bx_dbg_show_command($2);
          free($1); free($2);
      }
    | BX_TOKEN_SHOW BX_TOKEN_ALL '\n'
      {
          bx_dbg_show_command("all");
          free($1);
      }
    | BX_TOKEN_SHOW BX_TOKEN_OFF '\n'
      {
          bx_dbg_show_command("off");
          free($1);
      }
    | BX_TOKEN_SHOW BX_TOKEN_STRING '\n'
      {
          bx_dbg_show_param_command($2);
          free($1); free($2);
      }
    | BX_TOKEN_SHOW '\n'
      {
          bx_dbg_show_command(0);
          free($1);
      }
    ;

page_command:
      BX_TOKEN_PAGE expression '\n'
      {
          bx_dbg_xlate_address($2);
          free($1);
      }
    ;

tlb_command:
      BX_TOKEN_TLB expression '\n'
      {
          bx_dbg_tlb_lookup($2);
          free($1);
      }
    ;

ptime_command:
      BX_TOKEN_PTIME '\n'
      {
          bx_dbg_ptime_command();
          free($1);
      }
    ;

trace_command:
      BX_TOKEN_TRACE BX_TOKEN_TOGGLE_ON_OFF '\n'
      {
          bx_dbg_trace_command($2);
          free($1);
      }
    ;

trace_reg_command:
      BX_TOKEN_TRACEREG BX_TOKEN_TOGGLE_ON_OFF '\n'
      {
          bx_dbg_trace_reg_command($2);
          free($1);
      }
    ;

trace_mem_command:
      BX_TOKEN_TRACEMEM BX_TOKEN_TOGGLE_ON_OFF '\n'
      {
          bx_dbg_trace_mem_command($2);
          free($1);
      }
    ;

print_stack_command:
      BX_TOKEN_PRINT_STACK '\n'
      {
          bx_dbg_print_stack_command(16);
          free($1);
      }
    | BX_TOKEN_PRINT_STACK BX_TOKEN_NUMERIC '\n'
      {
          bx_dbg_print_stack_command($2);
          free($1);
      }
    ;

watch_point_command:
      BX_TOKEN_WATCH BX_TOKEN_STOP '\n'
      {
          bx_dbg_watchpoint_continue(0);
          free($1); free($2);
      }
    | BX_TOKEN_WATCH BX_TOKEN_CONTINUE '\n'
      {
          bx_dbg_watchpoint_continue(1);
          free($1); free($2);
      }
    | BX_TOKEN_WATCH '\n'
      {
          bx_dbg_print_watchpoints();
          free($1);
      }
    | BX_TOKEN_WATCH BX_TOKEN_R expression '\n'
      {
          bx_dbg_watch(0, $3, 1); /* BX_READ */
          free($1); free($2);
      }
    | BX_TOKEN_WATCH BX_TOKEN_READ expression '\n'
      {
          bx_dbg_watch(0, $3, 1); /* BX_READ */
          free($1); free($2);
      }
    | BX_TOKEN_WATCH BX_TOKEN_WRITE expression '\n'
      {
          bx_dbg_watch(1, $3, 1); /* BX_WRITE */
          free($1); free($2);
      }
    | BX_TOKEN_WATCH BX_TOKEN_R expression expression '\n'
      {
          bx_dbg_watch(0, $3, $4); /* BX_READ */
          free($1); free($2);
      }
    | BX_TOKEN_WATCH BX_TOKEN_READ expression expression '\n'
      {
          bx_dbg_watch(0, $3, $4); /* BX_READ */
          free($1); free($2);
      }
    | BX_TOKEN_WATCH BX_TOKEN_WRITE expression expression '\n'
      {
          bx_dbg_watch(1, $3, $4); /* BX_WRITE */
          free($1); free($2);
      }
    | BX_TOKEN_UNWATCH '\n'
      {
          bx_dbg_unwatch_all();
          free($1);
      }
    | BX_TOKEN_UNWATCH expression '\n'
      {
          bx_dbg_unwatch($2);
          free($1);
      }
    ;

symbol_command:
      BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING '\n'
      {
        bx_dbg_symbol_command($2, 0, 0);
        free($1); free($2);
      }
    | BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING expression '\n'
      {
        bx_dbg_symbol_command($2, 0, $3);
        free($1); free($2);
      }
    | BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING '\n'
      {
        bx_dbg_symbol_command($3, 1, 0);
        free($1); free($2); free($3);
      }
    | BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING expression '\n'
      {
        bx_dbg_symbol_command($3, 1, $4);
        free($1); free($2); free($3);
      }
    ;

where_command:
      BX_TOKEN_WHERE '\n'
      {
        bx_dbg_where_command();
        free($1);
      }
    ;

print_string_command:
      BX_TOKEN_PRINT_STRING expression '\n'
      {
        bx_dbg_print_string_command($2);
        free($1);
      }
    ;

continue_command:
      BX_TOKEN_CONTINUE '\n'
      {
        bx_dbg_continue_command();
        free($1);
      }
    ;

stepN_command:
      BX_TOKEN_STEPN '\n'
      {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free($1);
      }
    | BX_TOKEN_STEPN BX_TOKEN_NUMERIC '\n'
      {
        bx_dbg_stepN_command(dbg_cpu, $2);
        free($1);
      }
    | BX_TOKEN_STEPN BX_TOKEN_ALL BX_TOKEN_NUMERIC '\n'
      {
        bx_dbg_stepN_command(-1, $3);
        free($1); free($2);
      }
    | BX_TOKEN_STEPN BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'
      {
        bx_dbg_stepN_command($2, $3);
        free($1);
      }
    ;

step_over_command:
      BX_TOKEN_STEP_OVER '\n'
      {
        bx_dbg_step_over_command();
        free($1);
      }
    ;

set_command:
      BX_TOKEN_SET BX_TOKEN_DISASM BX_TOKEN_TOGGLE_ON_OFF '\n'
      {
        bx_dbg_set_auto_disassemble($3);
        free($1); free($2);
      }
    | BX_TOKEN_SET BX_TOKEN_SYMBOLNAME '=' expression '\n'
      {
        bx_dbg_set_symbol_command($2, $4);
        free($1); free($2);
      }
    | BX_TOKEN_SET BX_TOKEN_8BL_REG '=' expression '\n'
      { 
        bx_dbg_set_reg8l_value($2, $4);
      }
    | BX_TOKEN_SET BX_TOKEN_8BH_REG '=' expression '\n'
      { 
        bx_dbg_set_reg8h_value($2, $4);
      }
    | BX_TOKEN_SET BX_TOKEN_16B_REG '=' expression '\n'
      { 
        bx_dbg_set_reg16_value($2, $4);
      }
    | BX_TOKEN_SET BX_TOKEN_32B_REG '=' expression '\n'
      { 
        bx_dbg_set_reg32_value($2, $4);
      }
    | BX_TOKEN_SET BX_TOKEN_64B_REG '=' expression '\n'
      { 
        bx_dbg_set_reg64_value($2, $4);
      }
    | BX_TOKEN_SET BX_TOKEN_REG_EIP '=' expression '\n'
      { 
        bx_dbg_set_rip_value($4);
      }
    | BX_TOKEN_SET BX_TOKEN_REG_RIP '=' expression '\n'
      { 
        bx_dbg_set_rip_value($4);
      }
    | BX_TOKEN_SET BX_TOKEN_SEGREG '=' expression '\n'
      { 
        bx_dbg_load_segreg($2, $4);
      }
    ;

breakpoint_command:
      BX_TOKEN_VBREAKPOINT '\n'
      {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free($1);
      }
    | BX_TOKEN_VBREAKPOINT vexpression ':' vexpression '\n'
      {
        bx_dbg_vbreakpoint_command(bkRegular, $2, $4);
        free($1);
      }
    | BX_TOKEN_LBREAKPOINT '\n'
      {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free($1);
      }
    | BX_TOKEN_LBREAKPOINT expression '\n'
      {
        bx_dbg_lbreakpoint_command(bkRegular, $2);
        free($1);
      }
    | BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING '\n'
      {
        bx_dbg_lbreakpoint_symbol_command($2);
        free($1);free($2);
      }
    | BX_TOKEN_PBREAKPOINT '\n'
      {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free($1);
      }
    | BX_TOKEN_PBREAKPOINT expression '\n'
      {
        bx_dbg_pbreakpoint_command(bkRegular, $2);
        free($1);
      }
    | BX_TOKEN_PBREAKPOINT '*' expression '\n'
      {
        bx_dbg_pbreakpoint_command(bkRegular, $3);
        free($1);
      }
    ;

blist_command:
      BX_TOKEN_LIST_BREAK '\n'
      {
        bx_dbg_info_bpoints_command();
        free($1);
      }
    ;

slist_command:
      BX_TOKEN_LIST_SYMBOLS '\n'
      {
        bx_dbg_info_symbols_command(0);
        free($1);
      }
    | BX_TOKEN_LIST_SYMBOLS BX_TOKEN_STRING '\n'
      {
        bx_dbg_info_symbols_command($2);
        free($1);free($2);
      }
    ;

info_command:
      BX_TOKEN_INFO BX_TOKEN_PBREAKPOINT '\n'
      {
        bx_dbg_info_bpoints_command();
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_CPU '\n'
      {
        bx_dbg_info_registers_command(-1);
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric optional_numeric '\n'
      {
        bx_dbg_info_idt_command($3, $4);
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_IVT optional_numeric optional_numeric '\n'
      {
        bx_dbg_info_ivt_command($3, $4);
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_GDT optional_numeric optional_numeric '\n'
      {
        bx_dbg_info_gdt_command($3, $4);
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_LDT optional_numeric optional_numeric '\n'
      {
        bx_dbg_info_ldt_command($3, $4);
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_TAB '\n'
      {
        bx_dbg_dump_table();
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_TSS '\n'
      {
        bx_dbg_info_tss_command();
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_FLAGS '\n'
      {
        bx_dbg_info_flags();
        free($1);
      }
    | BX_TOKEN_INFO BX_TOKEN_LINUX '\n'
      {
        bx_dbg_info_linux_command();
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_SYMBOLS '\n'
      {
        bx_dbg_info_symbols_command(0);
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_SYMBOLS BX_TOKEN_STRING '\n'
      {
        bx_dbg_info_symbols_command($3);
        free($1); free($2); free($3);
      }
    | BX_TOKEN_INFO BX_TOKEN_DEVICE '\n'
      {
        bx_dbg_info_device("", "");
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC '\n'
      {
        bx_dbg_info_device($3, "");
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC BX_TOKEN_STRING '\n'
      {
        bx_dbg_info_device($3, $4);
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING '\n'
      {
        bx_dbg_info_device($3, "");
        free($1); free($2);
      }
    | BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING BX_TOKEN_STRING '\n'
      {
        bx_dbg_info_device($3, $4);
        free($1); free($2);
      }
    ;

optional_numeric :
   /* empty */ { $$ = EMPTY_ARG; }
   | expression;
   
regs_command:
      BX_TOKEN_REGISTERS '\n'
      {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free($1);
      }
    ;

fpu_regs_command:
      BX_TOKEN_FPU '\n'
      {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free($1);
      }
    ;

mmx_regs_command:
      BX_TOKEN_MMX '\n'
      {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free($1);
      }
    ;

xmm_regs_command:
      BX_TOKEN_XMM '\n'
      {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free($1);
      }
    ;

ymm_regs_command:
      BX_TOKEN_YMM '\n'
      {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free($1);
      }
    ;

zmm_regs_command:
      BX_TOKEN_ZMM '\n'
      {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free($1);
      }
    ;

segment_regs_command:
      BX_TOKEN_SEGMENT_REGS '\n'
      {
        bx_dbg_info_segment_regs_command();
        free($1);
      }
    ;

control_regs_command:
      BX_TOKEN_CONTROL_REGS '\n'
      {
        bx_dbg_info_control_regs_command();
        free($1);
      }
    ;

debug_regs_command:
      BX_TOKEN_DEBUG_REGS '\n'
      {
        bx_dbg_info_debug_regs_command();
        free($1);
      }
    ;

delete_command:
      BX_TOKEN_DEL_BREAKPOINT BX_TOKEN_NUMERIC '\n'
      {
        bx_dbg_del_breakpoint_command($2);
        free($1);
      }
    ;

bpe_command:
      BX_TOKEN_ENABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'
      {
        bx_dbg_en_dis_breakpoint_command($2, 1);
        free($1);
      }
    ;
bpd_command:
      BX_TOKEN_DISABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'
      {
        bx_dbg_en_dis_breakpoint_command($2, 0);
        free($1);
      }
    ;

quit_command:
      BX_TOKEN_QUIT '\n'
      {
        bx_dbg_quit_command();
        free($1);
      }
    ;

examine_command:
      BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT expression '\n'
      {
        bx_dbg_examine_command($1, $2,1, $3, 1);
        free($1); free($2);
      }
    | BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT '\n'
      {
        bx_dbg_examine_command($1, $2,1, 0, 0);
        free($1); free($2);
      }
    | BX_TOKEN_EXAMINE expression '\n'
      {
        bx_dbg_examine_command($1, NULL,0, $2, 1);
        free($1);
      }
    | BX_TOKEN_EXAMINE '\n'
      {
        bx_dbg_examine_command($1, NULL,0, 0, 0);
        free($1);
      }
    ;

restore_command:
      BX_TOKEN_RESTORE BX_TOKEN_STRING BX_TOKEN_STRING '\n'
      {
        bx_dbg_restore_command($2, $3);
        free($1); free($2); free($3);
      }
    ;

writemem_command:
      BX_TOKEN_WRITEMEM BX_TOKEN_STRING expression expression '\n'
      {
        bx_dbg_writemem_command($2, $3, $4);
        free($1); free($2);
      }
    ;

setpmem_command:
      BX_TOKEN_SETPMEM expression expression expression '\n'
      {
        bx_dbg_setpmem_command($2, $3, $4);
        free($1);
      }
    ;

query_command:
      BX_TOKEN_QUERY BX_TOKEN_PENDING '\n'
      {
        bx_dbg_query_command($2);
        free($1); free($2);
      }
    ;

take_command:
      BX_TOKEN_TAKE BX_TOKEN_DMA '\n'
      {
        bx_dbg_take_command($2, 1);
        free($1); free($2);
      }
    | BX_TOKEN_TAKE BX_TOKEN_DMA BX_TOKEN_NUMERIC '\n'
      {
        bx_dbg_take_command($2, $3);
        free($1); free($2);
      }
    | BX_TOKEN_TAKE BX_TOKEN_IRQ '\n'
      {
        bx_dbg_take_command($2, 1);
        free($1); free($2);
      }
    ;

disassemble_command:
      BX_TOKEN_DISASM '\n'
      {
        bx_dbg_disassemble_current(NULL);
        free($1);
      }
    | BX_TOKEN_DISASM expression '\n'
      {
        bx_dbg_disassemble_command(NULL, $2, $2);
        free($1);
      }
    | BX_TOKEN_DISASM expression expression '\n'
      {
        bx_dbg_disassemble_command(NULL, $2, $3);
        free($1);
      }
    | BX_TOKEN_DISASM BX_TOKEN_DISFORMAT '\n'
      {
        bx_dbg_disassemble_current($2);
        free($1); free($2);
      }
    | BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression '\n'
      {
        bx_dbg_disassemble_command($2, $3, $3);
        free($1); free($2);
      }
    | BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression expression '\n'
      {
        bx_dbg_disassemble_command($2, $3, $4);
        free($1); free($2);
      }
    | BX_TOKEN_DISASM BX_TOKEN_SWITCH_MODE '\n'
      {
        bx_dbg_disassemble_switch_mode();
        free($1); free($2);
      }
    | BX_TOKEN_DISASM BX_TOKEN_HEX BX_TOKEN_TOGGLE_ON_OFF '\n'
      {
        bx_dbg_disassemble_hex_mode_switch($3);
        free($1); free($2);
      }
    | BX_TOKEN_DISASM BX_TOKEN_SIZE '=' BX_TOKEN_NUMERIC '\n'
      {
        bx_dbg_set_disassemble_size($4);
        free($1); free($2);
      }
    ;

instrument_command:
      BX_TOKEN_INSTRUMENT BX_TOKEN_STOP '\n'
      {
        bx_dbg_instrument_command($2);
        free($1); free($2);
      }
    | BX_TOKEN_INSTRUMENT BX_TOKEN_GENERIC '\n'
      {
        bx_dbg_instrument_command($2);
        free($1); free($2);
      }
    ;

doit_command:
      BX_TOKEN_DOIT expression '\n'
      {
        bx_dbg_doit_command($2);
        free($1);
      }
    ;

crc_command:
      BX_TOKEN_CRC expression expression '\n'
      {
        bx_dbg_crc_command($2, $3);
        free($1);
      }
    ;

help_command:
       BX_TOKEN_HELP BX_TOKEN_QUIT '\n'
       {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_CONTINUE '\n'
       {
         dbg_printf("c|cont|continue - continue executing\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_STEPN '\n'
       {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_STEP_OVER '\n'
       {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_VBREAKPOINT '\n'
       {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_LBREAKPOINT '\n'
       {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_PBREAKPOINT '\n'
       {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_DEL_BREAKPOINT '\n'
       {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_ENABLE_BREAKPOINT '\n'
       {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_DISABLE_BREAKPOINT '\n'
       {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_LIST_BREAK '\n'
       {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_MODEBP '\n'
       {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_VMEXITBP '\n'
       {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_CRC '\n'
       {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_TRACE '\n'
       {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_TRACEREG '\n'
       {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_TRACEMEM '\n'
       {
         dbg_printf("trace-mem on  - print all memory accesses occured during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_RESTORE '\n'
       {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_PTIME '\n'
       {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_TIMEBP '\n'
       {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_TIMEBP_ABSOLUTE '\n'
       {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_PRINT_STACK '\n'
       {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_LOAD_SYMBOLS '\n'
       {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_LIST_SYMBOLS '\n'
       {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_REGISTERS '\n'
       {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_FPU '\n'
       {
         dbg_printf("fp|fpu - print FPU state\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_MMX '\n'
       {
         dbg_printf("mmx - print MMX state\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_XMM '\n'
       {
         dbg_printf("xmm|sse - print SSE state\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_YMM '\n'
       {
         dbg_printf("ymm - print AVX state\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_ZMM '\n'
       {
         dbg_printf("zmm - print AVX-512 state\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_SEGMENT_REGS '\n'
       {
         dbg_printf("sreg - show segment registers\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_CONTROL_REGS '\n'
       {
         dbg_printf("creg - show control registers\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_DEBUG_REGS '\n'
       {
         dbg_printf("dreg - show debug registers\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_WRITEMEM '\n'
       {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_SETPMEM '\n'
       {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_DISASM '\n'
       {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm hex on/off - control disasm offsets and displacements format\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_WATCH '\n'
       {
         dbg_printf("watch - print current watch point status\n");
         dbg_printf("watch stop - stop simulation when a watchpoint is encountred\n");
         dbg_printf("watch continue - do not stop the simulation when watch point is encountred\n");
         dbg_printf("watch r|read addr - insert a read watch point at physical address addr\n");
         dbg_printf("watch w|write addr - insert a write watch point at physical address addr\n");
         dbg_printf("watch r|read addr <len> - insert a read watch point at physical address addr with range <len>\n");
         dbg_printf("watch w|write addr <len> - insert a write watch point at physical address addr with range <len>\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_UNWATCH '\n'
       {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_EXAMINE '\n'
       {
         dbg_printf("x  /nuf <addr> - examine memory at linear address\n");
         dbg_printf("xp /nuf <addr> - examine memory at physical address\n");
         dbg_printf("    nuf is a sequence of numbers (how much values to display)\n");
         dbg_printf("    and one or more of the [mxduotcsibhwg] format specificators:\n");
         dbg_printf("    x,d,u,o,t,c,s,i select the format of the output (they stand for\n");
         dbg_printf("        hex, decimal, unsigned, octal, binary, char, asciiz, instr)\n");
         dbg_printf("    b,h,w,g select the size of a data element (for byte, half-word,\n");
         dbg_printf("        word and giant word)\n");
         dbg_printf("    m selects an alternative output format (memory dump)\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_INSTRUMENT '\n'
       {
         dbg_printf("instrument <command> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command>\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_SET '\n'
       {
         dbg_printf("set <regname> = <expr> - set register value to expression\n");
         dbg_printf("set eflags = <expr> - set eflags value to expression, not all flags can be modified\n");
         dbg_printf("set $cpu = <N> - move debugger control to cpu <N> in SMP simulation\n");
         dbg_printf("set $auto_disassemble = 1 - cause debugger to disassemble current instruction\n");
         dbg_printf("       every time execution stops\n");
         dbg_printf("set u|disasm|disassemble on  - same as 'set $auto_disassemble = 1'\n");
         dbg_printf("set u|disasm|disassemble off - same as 'set $auto_disassemble = 0'\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_PAGE '\n'
       {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_INFO '\n'
       {
         dbg_printf("info break - show information about current breakpoint status\n");
         dbg_printf("info cpu - show dump of all cpu registers\n");
         dbg_printf("info idt - show interrupt descriptor table\n");
         dbg_printf("info ivt - show interrupt vector table\n");
         dbg_printf("info gdt - show global descriptor table\n");
         dbg_printf("info tss - show current task state segment\n");
         dbg_printf("info tab - show page tables\n");
         dbg_printf("info eflags - show decoded EFLAGS register\n");
         dbg_printf("info symbols [string] - list symbols whose prefix is string\n");
         dbg_printf("info device - show list of devices supported by this command\n");
         dbg_printf("info device [string] - show state of device specified in string\n");
         dbg_printf("info device [string] [string] - show state of device with options\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_SHOW '\n'
       {
         dbg_printf("show <command> - toggles show symbolic info (calls to begin with)\n");
         dbg_printf("show - shows current show mode\n");
         dbg_printf("show mode - show, when processor switch mode\n");
         dbg_printf("show int - show, when an interrupt happens\n");
         dbg_printf("show softint - show, when software interrupt happens\n");
         dbg_printf("show extint - show, when external interrupt happens\n");
         dbg_printf("show call - show, when call is happens\n");
         dbg_printf("show iret - show, when iret is happens\n");
         dbg_printf("show all - turns on all symbolic info\n");
         dbg_printf("show off - turns off symbolic info\n");
         dbg_printf("show dbg_all - turn on all bx_dbg flags\n");
         dbg_printf("show dbg_none - turn off all bx_dbg flags\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_CALC '\n'
       {
         dbg_printf("calc|? <expr> - calculate a expression and display the result.\n");
         dbg_printf("    'expr' can reference any general-purpose, opmask and segment\n");
         dbg_printf("    registers, use any arithmetic and logic operations, and\n");
         dbg_printf("    also the special ':' operator which computes the linear\n");
         dbg_printf("    address of a segment:offset (in real and v86 mode) or of\n");
         dbg_printf("    a selector:offset (in protected mode) pair.\n");
         free($1);free($2);
       }
     | BX_TOKEN_HELP BX_TOKEN_HELP '\n'
       {
         bx_dbg_print_help();
         free($1);free($2);
       }
     | BX_TOKEN_HELP '\n'
       {
         bx_dbg_print_help();
         free($1);
       }
    ;

calc_command:
   BX_TOKEN_CALC expression '\n'
   {
     bx_dbg_calc_command($2);
     free($1);
   }
;

BX_TOKEN_NONSEG_REG:
     BX_TOKEN_8BL_REG
   | BX_TOKEN_8BH_REG
   | BX_TOKEN_16B_REG
   | BX_TOKEN_32B_REG
   | BX_TOKEN_64B_REG
   | BX_TOKEN_OPMASK_REG
   { $$=$1; }
;

/* Arithmetic expression for vbreak command */
vexpression:
     BX_TOKEN_NUMERIC                { $$ = $1; }
   | BX_TOKEN_STRING                 { $$ = bx_dbg_get_symbol_value($1); free($1);}
   | BX_TOKEN_8BL_REG                { $$ = bx_dbg_get_reg8l_value($1); }
   | BX_TOKEN_8BH_REG                { $$ = bx_dbg_get_reg8h_value($1); }
   | BX_TOKEN_16B_REG                { $$ = bx_dbg_get_reg16_value($1); }
   | BX_TOKEN_32B_REG                { $$ = bx_dbg_get_reg32_value($1); }
   | BX_TOKEN_64B_REG                { $$ = bx_dbg_get_reg64_value($1); }
   | BX_TOKEN_OPMASK_REG             { $$ = bx_dbg_get_opmask_value($1); }
   | BX_TOKEN_SEGREG                 { $$ = bx_dbg_get_selector_value($1); }
   | BX_TOKEN_REG_IP                 { $$ = bx_dbg_get_ip (); }
   | BX_TOKEN_REG_EIP                { $$ = bx_dbg_get_eip(); }
   | BX_TOKEN_REG_RIP                { $$ = bx_dbg_get_instruction_pointer(); }
   | vexpression '+' vexpression     { $$ = $1 + $3; }
   | vexpression '-' vexpression     { $$ = $1 - $3; }
   | vexpression '*' vexpression     { $$ = $1 * $3; }
   | vexpression '/' vexpression     { $$ = $1 / $3; }
   | vexpression BX_TOKEN_RSHIFT vexpression { $$ = $1 >> $3; }
   | vexpression BX_TOKEN_LSHIFT vexpression { $$ = $1 << $3; }
   | vexpression '|' vexpression     { $$ = $1 | $3; }
   | vexpression '^' vexpression     { $$ = $1 ^ $3; }
   | vexpression '&' vexpression     { $$ = $1 & $3; }
   | '!' vexpression %prec NOT       { $$ = !$2; }
   | '-' vexpression %prec NEG       { $$ = -$2; }
   | '(' vexpression ')'             { $$ = $2; }
;

/* Same as vexpression but includes the ':' operator and unary '*' and '@'
+   operators - used in most commands */
expression:
     BX_TOKEN_NUMERIC                { $$ = $1; }
   | BX_TOKEN_STRING                 { $$ = bx_dbg_get_symbol_value($1); free($1);}
   | BX_TOKEN_8BL_REG                { $$ = bx_dbg_get_reg8l_value($1); }
   | BX_TOKEN_8BH_REG                { $$ = bx_dbg_get_reg8h_value($1); }
   | BX_TOKEN_16B_REG                { $$ = bx_dbg_get_reg16_value($1); }
   | BX_TOKEN_32B_REG                { $$ = bx_dbg_get_reg32_value($1); }
   | BX_TOKEN_64B_REG                { $$ = bx_dbg_get_reg64_value($1); }
   | BX_TOKEN_OPMASK_REG             { $$ = bx_dbg_get_opmask_value($1); }
   | BX_TOKEN_SEGREG                 { $$ = bx_dbg_get_selector_value($1); }
   | BX_TOKEN_REG_IP                 { $$ = bx_dbg_get_ip (); }
   | BX_TOKEN_REG_EIP                { $$ = bx_dbg_get_eip(); }
   | BX_TOKEN_REG_RIP                { $$ = bx_dbg_get_instruction_pointer(); }
   | expression ':' expression       { $$ = bx_dbg_get_laddr ($1, $3); }
   | expression '+' expression       { $$ = $1 + $3; }
   | expression '-' expression       { $$ = $1 - $3; }
   | expression '*' expression       { $$ = $1 * $3; }
   | expression '/' expression       { $$ = ($3 != 0) ? $1 / $3 : 0; }
   | expression BX_TOKEN_RSHIFT expression { $$ = $1 >> $3; }
   | expression BX_TOKEN_LSHIFT expression { $$ = $1 << $3; }
   | expression '|' expression       { $$ = $1 | $3; }
   | expression '^' expression       { $$ = $1 ^ $3; }
   | expression '&' expression       { $$ = $1 & $3; }
   | '!' expression %prec NOT        { $$ = !$2; }
   | '-' expression %prec NEG        { $$ = -$2; }
   | '*' expression %prec INDIRECT   { $$ = bx_dbg_lin_indirect($2); }
   | '@' expression %prec INDIRECT   { $$ = bx_dbg_phy_indirect($2); }
   | '(' expression ')'              { $$ = $2; }
;

%%
