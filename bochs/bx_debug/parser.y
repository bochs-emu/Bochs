/////////////////////////////////////////////////////////////////////////
// $Id: parser.y,v 1.1 2003-11-28 15:07:25 danielg4 Exp $
/////////////////////////////////////////////////////////////////////////

%{
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

#if BX_DEBUGGER

// %left '-' '+'
// %left '*' '/'
// %right
// %nonassoc UMINUS
%}


%union {
  char    *sval;
  Bit32u   uval;
  Bit64u   ulval;
  bx_num_range   uval_range;
  Regs     reg;
  }

// Common registers
%type<reg> BX_TOKEN_COMMON_REG
%type<reg> BX_TOKEN_INDEX_REG
%type<reg> BX_TOKEN_PTR_REG
%type<reg> BX_TOKEN_NONSEG_REG
%token<reg> BX_TOKEN_REG_AL
%token<reg> BX_TOKEN_REG_BL
%token<reg> BX_TOKEN_REG_CL
%token<reg> BX_TOKEN_REG_DL
%token<reg> BX_TOKEN_REG_AH
%token<reg> BX_TOKEN_REG_BH
%token<reg> BX_TOKEN_REG_CH
%token<reg> BX_TOKEN_REG_DH
%token<reg> BX_TOKEN_REG_AX
%token<reg> BX_TOKEN_REG_BX
%token<reg> BX_TOKEN_REG_CX
%token<reg> BX_TOKEN_REG_DX
%token<reg> BX_TOKEN_REG_EAX
%token<reg> BX_TOKEN_REG_EBX
%token<reg> BX_TOKEN_REG_ECX
%token<reg> BX_TOKEN_REG_EDX
%token<reg> BX_TOKEN_REG_SI
%token<reg> BX_TOKEN_REG_DI
%token<reg> BX_TOKEN_REG_BP
%token<reg> BX_TOKEN_REG_SP
%token<reg> BX_TOKEN_REG_IP
%token<reg> BX_TOKEN_REG_ESI
%token<reg> BX_TOKEN_REG_EDI
%token<reg> BX_TOKEN_REG_EBP
%token<reg> BX_TOKEN_REG_ESP
%token<reg> BX_TOKEN_REG_EIP

%token <sval> BX_TOKEN_CONTINUE
%token <sval> BX_TOKEN_STEPN
%token <sval> BX_TOKEN_STEP_OVER
%token <sval> BX_TOKEN_NEXT_STEP
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
%token <sval> BX_TOKEN_PROGRAM
%token <sval> BX_TOKEN_REGISTERS
%token <sval> BX_TOKEN_CPU
%token <sval> BX_TOKEN_FPU
%token <sval> BX_TOKEN_ALL
%token <sval> BX_TOKEN_IDT
%token <sval> BX_TOKEN_GDT
%token <sval> BX_TOKEN_LDT
%token <sval> BX_TOKEN_TSS
%token <sval> BX_TOKEN_DIRTY
%token <sval> BX_TOKEN_LINUX
%token <sval> BX_TOKEN_CONTROL_REGS
%token <sval> BX_TOKEN_EXAMINE
%token <sval> BX_TOKEN_XFORMAT
%token <sval> BX_TOKEN_DISFORMAT
%token <sval> BX_TOKEN_SETPMEM
%token <sval> BX_TOKEN_SYMBOLNAME
%token <sval> BX_TOKEN_QUERY
%token <sval> BX_TOKEN_PENDING
%token <sval> BX_TOKEN_TAKE
%token <sval> BX_TOKEN_DMA
%token <sval> BX_TOKEN_IRQ
%token <sval> BX_TOKEN_DUMP_CPU
%token <sval> BX_TOKEN_SET_CPU
%token <sval> BX_TOKEN_DIS
%token <sval> BX_TOKEN_ON
%token <sval> BX_TOKEN_OFF
%token <sval> BX_TOKEN_DISASSEMBLE
%token <sval> BX_TOKEN_INSTRUMENT
%token <sval> BX_TOKEN_START
%token <sval> BX_TOKEN_STOP
%token <sval> BX_TOKEN_RESET
%token <sval> BX_TOKEN_PRINT
%token <sval> BX_TOKEN_LOADER
%token <sval> BX_TOKEN_STRING
%token <sval> BX_TOKEN_DOIT
%token <sval> BX_TOKEN_CRC
%token <sval> BX_TOKEN_TRACEON
%token <sval> BX_TOKEN_TRACEOFF
%token <sval> BX_TOKEN_PTIME
%token <sval> BX_TOKEN_TIMEBP_ABSOLUTE
%token <sval> BX_TOKEN_TIMEBP
%token <sval> BX_TOKEN_RECORD
%token <sval> BX_TOKEN_PLAYBACK
%token <sval> BX_TOKEN_MODEBP
%token <sval> BX_TOKEN_PRINT_STACK
%token <sval> BX_TOKEN_WATCH
%token <sval> BX_TOKEN_UNWATCH
%token <sval> BX_TOKEN_READ
%token <sval> BX_TOKEN_WRITE
%token <sval> BX_TOKEN_SHOW
%token <sval> BX_TOKEN_SYMBOL
%token <sval> BX_TOKEN_SYMBOLS
%token <sval> BX_TOKEN_LIST_SYMBOLS
%token <sval> BX_TOKEN_GLOBAL
%token <sval> BX_TOKEN_WHERE
%token <sval> BX_TOKEN_PRINT_STRING
%token <sval> BX_TOKEN_DIFF_MEMORY
%token <sval> BX_TOKEN_SYNC_MEMORY
%token <sval> BX_TOKEN_SYNC_CPU
%token <sval> BX_TOKEN_FAST_FORWARD
%token <sval> BX_TOKEN_PHY_2_LOG
%token <uval> BX_TOKEN_NUMERIC
%token <ulval> BX_TOKEN_LONG_NUMERIC
%token <sval> BX_TOKEN_INFO_ADDRESS
%token <sval> BX_TOKEN_NE2000
%token <sval> BX_TOKEN_PIC
%token <sval> BX_TOKEN_PAGE
%token <sval> BX_TOKEN_CS
%token <sval> BX_TOKEN_ES
%token <sval> BX_TOKEN_SS
%token <sval> BX_TOKEN_DS
%token <sval> BX_TOKEN_FS
%token <sval> BX_TOKEN_GS
%token <sval> BX_TOKEN_FLAGS
%token <sval> BX_TOKEN_ALWAYS_CHECK
%token <sval> BX_TOKEN_TRACEREGON
%token <sval> BX_TOKEN_TRACEREGOFF
%token <sval> BX_TOKEN_HELP
%token <sval> BX_TOKEN_CALC
%token BX_TOKEN_RSHIFT
%token BX_TOKEN_LSHIFT
%token <sval> BX_TOKEN_IVT
%type <uval> segment_register
%type <uval> optional_numeric
%type <uval_range> numeric_range optional_numeric_range
%type <ulval> vexpression
%type <ulval> expression

%left '+' '-' '|' '^'
%left '*' '/' '&' BX_TOKEN_LSHIFT BX_TOKEN_RSHIFT
%left NOT NEG

%start command

%%
command:
      continue_command
    | stepN_command
    | step_over_command
    | set_command
    | set_reg_command
    | breakpoint_command
    | info_command
    | blist_command
    | slist_command
    | dump_cpu_command
    | delete_command
    | bpe_command
    | bpd_command
    | quit_command
    | examine_command
    | setpmem_command
    | query_command
    | take_command
    | set_cpu_command
    | disassemble_command
    | instrument_command
    | loader_command
    | doit_command
    | crc_command
    | trace_on_command
    | trace_off_command
    | ptime_command
    | timebp_command
    | record_command
    | playback_command
    | modebp_command
    | print_stack_command
    | watch_point_command
    | show_command
    | symbol_command
    | where_command
    | print_string_command
    | cosim_commands
    | trace_reg_on_command
    | trace_reg_off_command
    | help_command
    | calc_command
    | 
    | '\n'
      {
      }
    ;

cosim_commands:
      BX_TOKEN_DIFF_MEMORY '\n'
        {
		bx_dbg_diff_memory();
		free($1);
	}
    | BX_TOKEN_SYNC_MEMORY BX_TOKEN_ON '\n'
        {
		bx_dbg_sync_memory(1);
		free($1); free($2);
	}
    | BX_TOKEN_SYNC_MEMORY BX_TOKEN_OFF '\n'
        {
		bx_dbg_sync_memory(0);
		free($1); free($2);
	}
    | BX_TOKEN_SYNC_CPU BX_TOKEN_ON '\n'
        {
		bx_dbg_sync_cpu(1);
		free($1); free($2);
	}
    | BX_TOKEN_SYNC_CPU BX_TOKEN_OFF '\n'
        {
		bx_dbg_sync_cpu(0);
		free($1); free($2);
	}
    | BX_TOKEN_FAST_FORWARD BX_TOKEN_NUMERIC '\n'
        {
		free($1);
		bx_dbg_fast_forward($2);
	}
    | BX_TOKEN_PHY_2_LOG BX_TOKEN_NUMERIC '\n'
        {
	}
    | BX_TOKEN_INFO_ADDRESS segment_register ':' BX_TOKEN_NUMERIC '\n'
        {
		free($1);
		bx_dbg_info_address($2, $4);
        }
    | BX_TOKEN_ALWAYS_CHECK BX_TOKEN_NUMERIC BX_TOKEN_ON '\n'
        {
        }
    | BX_TOKEN_ALWAYS_CHECK BX_TOKEN_NUMERIC BX_TOKEN_OFF '\n'
        {
        }
    ;

segment_register:
      BX_TOKEN_CS { $$ = 1; }
    | BX_TOKEN_ES { $$ = 0; }
    | BX_TOKEN_SS { $$ = 2; }
    | BX_TOKEN_DS { $$ = 3; }
    | BX_TOKEN_FS { $$ = 4; }
    | BX_TOKEN_GS { $$ = 5; }
    ;

timebp_command:
      BX_TOKEN_TIMEBP BX_TOKEN_LONG_NUMERIC '\n'
        {
        bx_dbg_timebp_command(0, $2);
	free($1);
	}
    | BX_TOKEN_TIMEBP_ABSOLUTE BX_TOKEN_LONG_NUMERIC '\n'
        {
        bx_dbg_timebp_command(1, $2);
	free($1);
	}
    ;

record_command:
	BX_TOKEN_RECORD BX_TOKEN_STRING '\n'
          {
          bx_dbg_record_command($2);
          free($1); free($2);
          }
    ;

playback_command:
	BX_TOKEN_PLAYBACK BX_TOKEN_STRING '\n'
          {
          bx_dbg_playback_command($2);
          free($1); free($2);
          }
    ;

modebp_command:
	BX_TOKEN_MODEBP BX_TOKEN_STRING '\n'
          {
          bx_dbg_modebp_command($2);
          free($1); free($2);
          }
	| BX_TOKEN_MODEBP '\n'
          {
          bx_dbg_modebp_command(0);
          free($1);
          }
    ;

show_command:
	BX_TOKEN_SHOW BX_TOKEN_STRING '\n'
          {
          bx_dbg_show_command($2);
          free($1); free($2);
          }
	| BX_TOKEN_SHOW '\n'
          {
          bx_dbg_show_command(0);
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

trace_on_command:
      BX_TOKEN_TRACEON '\n'
        {
        bx_dbg_trace_on_command();
        free($1);
	}
    ;

trace_off_command:
      BX_TOKEN_TRACEOFF '\n'
        {
        bx_dbg_trace_off_command();
        free($1);
	}
    ;

print_stack_command:
	BX_TOKEN_PRINT_STACK '\n'
          {
          bx_dbg_print_stack_command(16);
          free($1);
	  }
    |   BX_TOKEN_PRINT_STACK BX_TOKEN_NUMERIC '\n'
          {
          bx_dbg_print_stack_command($2);
          free($1);
	  }
    ;

watch_point_command:
	BX_TOKEN_WATCH BX_TOKEN_STOP '\n'
          {
          watchpoint_continue = 0;
	  fprintf(stderr, "Will stop on watch points\n");
          free($1); free($2);
          }
    |   BX_TOKEN_WATCH BX_TOKEN_CONTINUE '\n'
          {
          watchpoint_continue = 1;
          fprintf(stderr, "Will not stop on watch points (they will still be logged)\n");
          free($1); free($2);
          }
    |   BX_TOKEN_WATCH '\n'
          {
          bx_dbg_watch(-1, 0);
          free($1);
          }
    |   BX_TOKEN_UNWATCH '\n'
          {
          bx_dbg_unwatch(-1, 0);
          free($1);
          }
    |   BX_TOKEN_WATCH BX_TOKEN_READ BX_TOKEN_NUMERIC '\n'
          {
          bx_dbg_watch(1, $3);
          free($1); free($2);
          }
    |   BX_TOKEN_UNWATCH BX_TOKEN_READ BX_TOKEN_NUMERIC '\n'
          {
          bx_dbg_unwatch(1, $3);
          free($1); free($2);
          }
    |   BX_TOKEN_WATCH BX_TOKEN_WRITE BX_TOKEN_NUMERIC '\n'
          {
          bx_dbg_watch(0, $3);
          free($1); free($2);
          }
    |   BX_TOKEN_UNWATCH BX_TOKEN_WRITE BX_TOKEN_NUMERIC '\n'
          {
          bx_dbg_unwatch(0, $3);
          free($1); free($2);
          }
    ;

symbol_command:
      BX_TOKEN_SYMBOL BX_TOKEN_STRING '\n' 
        {
	bx_dbg_symbol_command($2, 0, 0);
        free($1); free($2);
        }
    | BX_TOKEN_SYMBOL BX_TOKEN_STRING BX_TOKEN_NUMERIC '\n' 
        {
	bx_dbg_symbol_command($2, 0, $3);
        free($1); free($2);
        }
    | BX_TOKEN_SYMBOL BX_TOKEN_GLOBAL BX_TOKEN_STRING BX_TOKEN_NUMERIC '\n' 
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
      BX_TOKEN_PRINT_STRING BX_TOKEN_NUMERIC '\n'
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
        bx_dbg_stepN_command(1);
        free($1);
        }
    | BX_TOKEN_STEPN BX_TOKEN_NUMERIC '\n'
        {
        bx_dbg_stepN_command($2);
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
      BX_TOKEN_SET BX_TOKEN_DIS BX_TOKEN_ON '\n'
        {
        bx_dbg_set_command($1, $2, $3);
        free($1); free($2); free($3);
        }
    | BX_TOKEN_SET BX_TOKEN_DIS BX_TOKEN_OFF '\n'
        {
        bx_dbg_set_command($1, $2, $3);
        free($1); free($2); free($3);
        }
    | BX_TOKEN_SET BX_TOKEN_SYMBOLNAME '=' BX_TOKEN_NUMERIC '\n'
        {
        bx_dbg_set_symbol_command($2, $4);
        free($1); free($2);
        }
    ;

breakpoint_command:
      BX_TOKEN_VBREAKPOINT '\n'
        {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free($1);
        }
    | BX_TOKEN_VBREAKPOINT vexpression ':' expression '\n'
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
        bx_dbg_dump_cpu_command();
        free($1); free($2);
        }
    | BX_TOKEN_INFO BX_TOKEN_PROGRAM '\n'
        {
        bx_dbg_info_program_command();
        free($1); free($2);
        }
    | BX_TOKEN_INFO BX_TOKEN_REGISTERS '\n'
        {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free($1); free($2);
        }
    | BX_TOKEN_INFO BX_TOKEN_FPU '\n'
        {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free($1); free($2);
        }
    | BX_TOKEN_INFO BX_TOKEN_ALL '\n'
        {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS | BX_INFO_FPU_REGS);
        free($1); free($2);
        }
    | BX_TOKEN_INFO BX_TOKEN_DIRTY '\n'
        {
        bx_dbg_info_dirty_command();
        free($1); free($2);
	}
    | BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric_range '\n'
        {
        bx_dbg_info_idt_command($3);
        free($1); free($2);
        }
    | BX_TOKEN_INFO BX_TOKEN_IVT optional_numeric_range '\n'
       {
       bx_dbg_info_ivt_command($3);
       free($1); free($2);
       }
    | BX_TOKEN_INFO BX_TOKEN_GDT optional_numeric_range '\n'
        {
        bx_dbg_info_gdt_command($3);
        free($1); free($2);
        }
    | BX_TOKEN_INFO BX_TOKEN_LDT optional_numeric_range '\n'
        {
        bx_dbg_info_ldt_command($3);
        free($1); free($2);
        }
    | BX_TOKEN_INFO BX_TOKEN_TSS optional_numeric_range '\n'
        {
        bx_dbg_info_tss_command($3);
        free($1); free($2);
        }
    | BX_TOKEN_INFO BX_TOKEN_FLAGS '\n'
        {
        bx_dbg_info_flags();
        free($1); free($2);
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
    | BX_TOKEN_INFO BX_TOKEN_CONTROL_REGS '\n'
        {
        bx_dbg_info_control_regs_command();
        free($1); free($2);
        }
    | BX_TOKEN_INFO BX_TOKEN_NE2000 '\n'
        {
        bx_dbg_info_ne2k(-1, -1);
        free($1); free($2);
        }
    | BX_TOKEN_INFO BX_TOKEN_NE2000 BX_TOKEN_PAGE BX_TOKEN_NUMERIC '\n'
        {
        free($1); free($2); free($3);
        bx_dbg_info_ne2k($4, -1);
        }
    | BX_TOKEN_INFO BX_TOKEN_NE2000 BX_TOKEN_PAGE BX_TOKEN_NUMERIC BX_TOKEN_REGISTERS BX_TOKEN_NUMERIC '\n'
        {
        free($1); free($2); free($3); free($5);
        bx_dbg_info_ne2k($4, $6);
        }
    | BX_TOKEN_INFO BX_TOKEN_PIC '\n'
        {
        bx_dbg_info_pic();
        free($1); free($2);
        }
    ;

optional_numeric :
   /* empty */ { $$ = EMPTY_ARG; }
   | BX_TOKEN_NUMERIC;

optional_numeric_range:
  /* empty */ { $$ = make_num_range (EMPTY_ARG, EMPTY_ARG); }
  | numeric_range;

numeric_range :
  expression
  {
    $$ = make_num_range ($1, $1);
  }
  | 
  expression expression
  {
    $$ = make_num_range ($1, $2);
  };
   

dump_cpu_command:
      BX_TOKEN_DUMP_CPU '\n'
        {
        bx_dbg_dump_cpu_command();
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
        bx_dbg_examine_command($1, $2,1, $3,1, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command($1, $2,1, $3,1, 1);
#endif
        free($1); free($2);
        }
    | BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT '\n'
        {
        bx_dbg_examine_command($1, $2,1, 0,0, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command($1, $2,1, 0,0, 1);
#endif
        free($1); free($2);
        }
    | BX_TOKEN_EXAMINE expression '\n'
        {
        //FIXME HanishKVC This method of hunting thro all the 
        //      simulators may be better than using 2 calls if 
        //      BX_NUM_SIMULATORS greater than or equal to 2 as 
        //      done for other cases of BX_TOKEN_EXAMINE
        int iCurSim; 
        for(iCurSim = 0; iCurSim < BX_NUM_SIMULATORS; iCurSim++)
        {
          bx_dbg_examine_command($1, NULL,0, $2,1, iCurSim);
        }
        free($1);
        }
    | BX_TOKEN_EXAMINE '\n'
        {
        //FIXME HanishKVC This method of hunting thro all the 
        //      simulators may be better than using 2 calls if 
        //      BX_NUM_SIMULATORS greater than or equal to 2 as 
        //      done for other cases of BX_TOKEN_EXAMINE
        int iCurSim; 
        for(iCurSim = 0; iCurSim < BX_NUM_SIMULATORS; iCurSim++)
        {
          bx_dbg_examine_command($1, NULL,0, 0,0, iCurSim);
        }
        free($1);
        }
    ;

setpmem_command:
      BX_TOKEN_SETPMEM BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'
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

set_cpu_command:
      BX_TOKEN_SET_CPU '\n'
        {
        bx_dbg_set_cpu_command();
        free($1);
        }
    ;

disassemble_command:
      BX_TOKEN_DISASSEMBLE optional_numeric_range '\n'
        {
        bx_dbg_disassemble_command(NULL, $2);
        free($1);
        }
    | BX_TOKEN_DISASSEMBLE BX_TOKEN_DISFORMAT optional_numeric_range '\n'
        {
        bx_dbg_disassemble_command($2, $3);
	free($1); free($2);
        }
    ;

instrument_command:
      BX_TOKEN_INSTRUMENT BX_TOKEN_START '\n'
        {
        bx_dbg_instrument_command($2);
        free($1); free($2);
        }
    | BX_TOKEN_INSTRUMENT BX_TOKEN_STOP '\n'
        {
        bx_dbg_instrument_command($2);
        free($1); free($2);
        }
    | BX_TOKEN_INSTRUMENT BX_TOKEN_RESET '\n'
        {
        bx_dbg_instrument_command($2);
        free($1); free($2);
        }
    | BX_TOKEN_INSTRUMENT BX_TOKEN_PRINT '\n'
        {
        bx_dbg_instrument_command($2);
        free($1); free($2);
        }
    ;

loader_command:
      BX_TOKEN_LOADER BX_TOKEN_STRING '\n'
        {
        bx_dbg_loader_command($2);
        free($1); free($2);
        }
    ;

doit_command:
      BX_TOKEN_DOIT BX_TOKEN_NUMERIC '\n'
        {
        bx_dbg_doit_command($2);
        free($1);
        }
    ;

crc_command:
      BX_TOKEN_CRC BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'
        {
        bx_dbg_crc_command($2, $3);
        free($1);
        }
    ;

trace_reg_on_command:
      BX_TOKEN_TRACEREGON '\n'
        {
	bx_dbg_trace_reg_on_command();
	free($1);
	}
    ;

trace_reg_off_command:
      BX_TOKEN_TRACEREGOFF '\n'
        {
	bx_dbg_trace_reg_off_command();
	free($1);
	}
    ;

help_command:
       BX_TOKEN_HELP BX_TOKEN_STRING '\n'
         {
         bx_dbg_help_command($2);
         free($1);free($2);
         }
       | BX_TOKEN_HELP '\n'
         {
         bx_dbg_help_command(0);
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

BX_TOKEN_COMMON_REG:
     BX_TOKEN_REG_AL
   | BX_TOKEN_REG_BL
   | BX_TOKEN_REG_CL
   | BX_TOKEN_REG_DL
   | BX_TOKEN_REG_AH
   | BX_TOKEN_REG_BH
   | BX_TOKEN_REG_CH
   | BX_TOKEN_REG_DH
   | BX_TOKEN_REG_AX
   | BX_TOKEN_REG_BX
   | BX_TOKEN_REG_CX
   | BX_TOKEN_REG_DX
   | BX_TOKEN_REG_EAX
   | BX_TOKEN_REG_EBX
   | BX_TOKEN_REG_ECX
   | BX_TOKEN_REG_EDX
   { $$=$1; }
;

BX_TOKEN_INDEX_REG:
     BX_TOKEN_REG_SI
   | BX_TOKEN_REG_DI
   | BX_TOKEN_REG_ESI
   | BX_TOKEN_REG_EDI
   { $$=$1; }
;

BX_TOKEN_PTR_REG:
     BX_TOKEN_REG_BP
   | BX_TOKEN_REG_SP
   | BX_TOKEN_REG_IP
   | BX_TOKEN_REG_EBP
   | BX_TOKEN_REG_ESP
   | BX_TOKEN_REG_EIP
   { $$=$1; }
;

BX_TOKEN_NONSEG_REG:
     BX_TOKEN_COMMON_REG
   | BX_TOKEN_INDEX_REG
   | BX_TOKEN_PTR_REG
   { $$=$1; }
;

/* Arithmetic expression for vbreak command */
vexpression:
     BX_TOKEN_NUMERIC                { $$ = $1; }
   | BX_TOKEN_LONG_NUMERIC           { $$ = $1; }
   | BX_TOKEN_NONSEG_REG             { $$ = bx_dbg_get_reg_value($1); }
   | segment_register                { $$ = bx_dbg_get_selector_value($1); }
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

/* Same as vexpression but includes the ':' operator - used in most commands */
expression:
     BX_TOKEN_NUMERIC                { $$ = $1; }
   | BX_TOKEN_LONG_NUMERIC           { $$ = $1; }
   | BX_TOKEN_STRING                 { $$ = bx_dbg_get_symbol_value($1); free($1);}
   | BX_TOKEN_NONSEG_REG             { $$ = bx_dbg_get_reg_value($1);}
   | segment_register                { $$ = bx_dbg_get_selector_value($1);}
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
   | '(' expression ')'              { $$ = $2; }
;

set_reg_command: 
     BX_TOKEN_REGISTERS BX_TOKEN_NONSEG_REG '=' expression '\n'
     { bx_dbg_set_reg_value($2, $4); }
;
%%
