%{
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
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
  }

%token <sval> BX_TOKEN_CONTINUE
%token <sval> BX_TOKEN_STEPN
%token <sval> BX_TOKEN_NEXT_STEP
%token <sval> BX_TOKEN_SET
%token <sval> BX_TOKEN_DEBUGGER
%token <sval> BX_TOKEN_VBREAKPOINT
%token <sval> BX_TOKEN_LBREAKPOINT
%token <sval> BX_TOKEN_PBREAKPOINT
%token <sval> BX_TOKEN_DEL_BREAKPOINT
%token <sval> BX_TOKEN_INFO
%token <sval> BX_TOKEN_QUIT
%token <sval> BX_TOKEN_PROGRAM
%token <sval> BX_TOKEN_REGISTERS
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
%token <sval> BX_TOKEN_CS
%token <sval> BX_TOKEN_ES
%token <sval> BX_TOKEN_SS
%token <sval> BX_TOKEN_DS
%token <sval> BX_TOKEN_FS
%token <sval> BX_TOKEN_GS
%token <sval> BX_TOKEN_ALWAYS_CHECK
%token <sval> BX_TOKEN_MATHS
%token <sval> BX_TOKEN_ADD
%token <sval> BX_TOKEN_SUB
%token <sval> BX_TOKEN_MUL
%token <sval> BX_TOKEN_DIV
%token <sval> BX_TOKEN_V2L
%type <uval> segment_register
%type <uval> optional_numeric
%type <uval_range> numeric_range optional_numeric_range

%start command

%%
command:
      continue_command
    | stepN_command
    | set_command
    | breakpoint_command
    | info_command
    | dump_cpu_command
    | delete_command
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
    | maths_command
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
    | v2l_command
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
        bx_dbg_vbreakpoint_command(0, 0, 0);
        free($1);
        }
    | BX_TOKEN_VBREAKPOINT BX_TOKEN_NUMERIC ':' BX_TOKEN_NUMERIC '\n'
        {
        bx_dbg_vbreakpoint_command(1, $2, $4);
        free($1);
        }
    | BX_TOKEN_LBREAKPOINT '\n'
        {
        bx_dbg_lbreakpoint_command(0, 0);
        free($1);
        }
    | BX_TOKEN_LBREAKPOINT BX_TOKEN_NUMERIC '\n'
        {
        bx_dbg_lbreakpoint_command(1, $2);
        free($1);
        }
    | BX_TOKEN_PBREAKPOINT '\n'
        {
        bx_dbg_pbreakpoint_command(0, 0);
        free($1);
        }
    | BX_TOKEN_PBREAKPOINT BX_TOKEN_NUMERIC '\n'
        {
        bx_dbg_pbreakpoint_command(1, $2);
        free($1);
        }
    | BX_TOKEN_PBREAKPOINT '*' BX_TOKEN_NUMERIC '\n'
        {
        bx_dbg_pbreakpoint_command(1, $3);
        free($1);
        }
    ;

info_command:
      BX_TOKEN_INFO BX_TOKEN_PBREAKPOINT '\n'
        {
        bx_dbg_info_bpoints_command();
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
    | BX_TOKEN_INFO BX_TOKEN_LINUX '\n'
        {
        bx_dbg_info_linux_command();
        free($1); free($2);
        }
    | BX_TOKEN_INFO BX_TOKEN_CONTROL_REGS '\n'
        {
        bx_dbg_info_control_regs_command();
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
  BX_TOKEN_NUMERIC
  {
    $$ = make_num_range ($1, $1);
  }
  | 
  BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC
  {
    $$ = make_num_range ($1, $2);
  }
  | 
  BX_TOKEN_NUMERIC ':' BX_TOKEN_NUMERIC
  {
    $$ = make_num_range ($1, $3);
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

quit_command:
      BX_TOKEN_QUIT '\n'
        {
	  bx_dbg_quit_command();
	  free($1);
        }
    ;


examine_command:
      BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT BX_TOKEN_NUMERIC '\n'
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
    | BX_TOKEN_EXAMINE BX_TOKEN_NUMERIC '\n'
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
        bx_dbg_disassemble_command($2);
        free($1);
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

maths_command:
      BX_TOKEN_MATHS BX_TOKEN_ADD BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'
        {
        bx_dbg_maths_command($2, $3, $4);
        free($1); free($2);
        }
    | BX_TOKEN_MATHS BX_TOKEN_SUB BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'
        {
        bx_dbg_maths_command($2, $3, $4);
        free($1); free($2);
        }
    | BX_TOKEN_MATHS BX_TOKEN_MUL BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'
        {
        bx_dbg_maths_command($2, $3, $4);
        free($1); free($2);
        }
    | BX_TOKEN_MATHS BX_TOKEN_DIV BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'
        {
        bx_dbg_maths_command($2, $3, $4);
        free($1); free($2);
        }
    | BX_TOKEN_MATHS BX_TOKEN_STRING '\n'
        {
        bx_dbg_maths_expression_command($2);
        free($1); free($2);
        }

v2l_command:
      BX_TOKEN_V2L segment_register ':' BX_TOKEN_NUMERIC '\n'
        {
        bx_dbg_v2l_command($2, $4);
        free($1);
        }
    ;

%%
