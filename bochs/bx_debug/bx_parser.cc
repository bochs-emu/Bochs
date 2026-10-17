/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         bxparse
#define yylex           bxlex
#define yyerror         bxerror
#define yydebug         bxdebug
#define yynerrs         bxnerrs
#define yylval          bxlval
#define yychar          bxchar

/* First part of user prologue.  */
#line 5 "bx_parser.y"

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

#if BX_DEBUGGER
Bit64u eval_value;
bool eval_value_valid; // eval_value was set by the last parsed line

#line 88 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_BX_Y_TAB_H_INCLUDED
# define YY_BX_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int bxdebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    BX_TOKEN_8BH_REG = 258,        /* BX_TOKEN_8BH_REG  */
    BX_TOKEN_8BL_REG = 259,        /* BX_TOKEN_8BL_REG  */
    BX_TOKEN_16B_REG = 260,        /* BX_TOKEN_16B_REG  */
    BX_TOKEN_32B_REG = 261,        /* BX_TOKEN_32B_REG  */
    BX_TOKEN_64B_REG = 262,        /* BX_TOKEN_64B_REG  */
    BX_TOKEN_CS = 263,             /* BX_TOKEN_CS  */
    BX_TOKEN_ES = 264,             /* BX_TOKEN_ES  */
    BX_TOKEN_SS = 265,             /* BX_TOKEN_SS  */
    BX_TOKEN_DS = 266,             /* BX_TOKEN_DS  */
    BX_TOKEN_FS = 267,             /* BX_TOKEN_FS  */
    BX_TOKEN_GS = 268,             /* BX_TOKEN_GS  */
    BX_TOKEN_OPMASK_REG = 269,     /* BX_TOKEN_OPMASK_REG  */
    BX_TOKEN_FLAGS = 270,          /* BX_TOKEN_FLAGS  */
    BX_TOKEN_ON = 271,             /* BX_TOKEN_ON  */
    BX_TOKEN_OFF = 272,            /* BX_TOKEN_OFF  */
    BX_TOKEN_CONTINUE = 273,       /* BX_TOKEN_CONTINUE  */
    BX_TOKEN_IF = 274,             /* BX_TOKEN_IF  */
    BX_TOKEN_STEPN = 275,          /* BX_TOKEN_STEPN  */
    BX_TOKEN_STEP_OVER = 276,      /* BX_TOKEN_STEP_OVER  */
    BX_TOKEN_SET = 277,            /* BX_TOKEN_SET  */
    BX_TOKEN_DEBUGGER = 278,       /* BX_TOKEN_DEBUGGER  */
    BX_TOKEN_LIST_BREAK = 279,     /* BX_TOKEN_LIST_BREAK  */
    BX_TOKEN_VBREAKPOINT = 280,    /* BX_TOKEN_VBREAKPOINT  */
    BX_TOKEN_LBREAKPOINT = 281,    /* BX_TOKEN_LBREAKPOINT  */
    BX_TOKEN_PBREAKPOINT = 282,    /* BX_TOKEN_PBREAKPOINT  */
    BX_TOKEN_DEL_BREAKPOINT = 283, /* BX_TOKEN_DEL_BREAKPOINT  */
    BX_TOKEN_ENABLE_BREAKPOINT = 284, /* BX_TOKEN_ENABLE_BREAKPOINT  */
    BX_TOKEN_DISABLE_BREAKPOINT = 285, /* BX_TOKEN_DISABLE_BREAKPOINT  */
    BX_TOKEN_INFO = 286,           /* BX_TOKEN_INFO  */
    BX_TOKEN_QUIT = 287,           /* BX_TOKEN_QUIT  */
    BX_TOKEN_DETACH = 288,         /* BX_TOKEN_DETACH  */
    BX_TOKEN_R = 289,              /* BX_TOKEN_R  */
    BX_TOKEN_REGS = 290,           /* BX_TOKEN_REGS  */
    BX_TOKEN_CPU = 291,            /* BX_TOKEN_CPU  */
    BX_TOKEN_FPU = 292,            /* BX_TOKEN_FPU  */
    BX_TOKEN_MMX = 293,            /* BX_TOKEN_MMX  */
    BX_TOKEN_XMM = 294,            /* BX_TOKEN_XMM  */
    BX_TOKEN_YMM = 295,            /* BX_TOKEN_YMM  */
    BX_TOKEN_ZMM = 296,            /* BX_TOKEN_ZMM  */
    BX_TOKEN_AVX = 297,            /* BX_TOKEN_AVX  */
    BX_TOKEN_AMX = 298,            /* BX_TOKEN_AMX  */
    BX_TOKEN_TILE = 299,           /* BX_TOKEN_TILE  */
    BX_TOKEN_IDT = 300,            /* BX_TOKEN_IDT  */
    BX_TOKEN_IVT = 301,            /* BX_TOKEN_IVT  */
    BX_TOKEN_GDT = 302,            /* BX_TOKEN_GDT  */
    BX_TOKEN_LDT = 303,            /* BX_TOKEN_LDT  */
    BX_TOKEN_TSS = 304,            /* BX_TOKEN_TSS  */
    BX_TOKEN_TAB = 305,            /* BX_TOKEN_TAB  */
    BX_TOKEN_ALL = 306,            /* BX_TOKEN_ALL  */
    BX_TOKEN_LINUX = 307,          /* BX_TOKEN_LINUX  */
    BX_TOKEN_DEBUG_REGS = 308,     /* BX_TOKEN_DEBUG_REGS  */
    BX_TOKEN_CONTROL_REGS = 309,   /* BX_TOKEN_CONTROL_REGS  */
    BX_TOKEN_SEGMENT_REGS = 310,   /* BX_TOKEN_SEGMENT_REGS  */
    BX_TOKEN_EXAMINE = 311,        /* BX_TOKEN_EXAMINE  */
    BX_TOKEN_XFORMAT = 312,        /* BX_TOKEN_XFORMAT  */
    BX_TOKEN_DISFORMAT = 313,      /* BX_TOKEN_DISFORMAT  */
    BX_TOKEN_RESTORE = 314,        /* BX_TOKEN_RESTORE  */
    BX_TOKEN_WRITEMEM = 315,       /* BX_TOKEN_WRITEMEM  */
    BX_TOKEN_LOADMEM = 316,        /* BX_TOKEN_LOADMEM  */
    BX_TOKEN_SETPMEM = 317,        /* BX_TOKEN_SETPMEM  */
    BX_TOKEN_DEREF = 318,          /* BX_TOKEN_DEREF  */
    BX_TOKEN_SYMBOLNAME = 319,     /* BX_TOKEN_SYMBOLNAME  */
    BX_TOKEN_QUERY = 320,          /* BX_TOKEN_QUERY  */
    BX_TOKEN_PENDING = 321,        /* BX_TOKEN_PENDING  */
    BX_TOKEN_TAKE = 322,           /* BX_TOKEN_TAKE  */
    BX_TOKEN_DMA = 323,            /* BX_TOKEN_DMA  */
    BX_TOKEN_IRQ = 324,            /* BX_TOKEN_IRQ  */
    BX_TOKEN_SMI = 325,            /* BX_TOKEN_SMI  */
    BX_TOKEN_NMI = 326,            /* BX_TOKEN_NMI  */
    BX_TOKEN_TLB = 327,            /* BX_TOKEN_TLB  */
    BX_TOKEN_DISASM = 328,         /* BX_TOKEN_DISASM  */
    BX_TOKEN_INSTRUMENT = 329,     /* BX_TOKEN_INSTRUMENT  */
    BX_TOKEN_STRING = 330,         /* BX_TOKEN_STRING  */
    BX_TOKEN_STOP = 331,           /* BX_TOKEN_STOP  */
    BX_TOKEN_DOIT = 332,           /* BX_TOKEN_DOIT  */
    BX_TOKEN_CRC = 333,            /* BX_TOKEN_CRC  */
    BX_TOKEN_TRACE = 334,          /* BX_TOKEN_TRACE  */
    BX_TOKEN_TRACEREG = 335,       /* BX_TOKEN_TRACEREG  */
    BX_TOKEN_TRACEMEM = 336,       /* BX_TOKEN_TRACEMEM  */
    BX_TOKEN_SWITCH_MODE = 337,    /* BX_TOKEN_SWITCH_MODE  */
    BX_TOKEN_SIZE = 338,           /* BX_TOKEN_SIZE  */
    BX_TOKEN_PTIME = 339,          /* BX_TOKEN_PTIME  */
    BX_TOKEN_TIMEBP_ABSOLUTE = 340, /* BX_TOKEN_TIMEBP_ABSOLUTE  */
    BX_TOKEN_TIMEBP = 341,         /* BX_TOKEN_TIMEBP  */
    BX_TOKEN_MODEBP = 342,         /* BX_TOKEN_MODEBP  */
    BX_TOKEN_VMEXITBP = 343,       /* BX_TOKEN_VMEXITBP  */
    BX_TOKEN_PRINT_STACK = 344,    /* BX_TOKEN_PRINT_STACK  */
    BX_TOKEN_BT = 345,             /* BX_TOKEN_BT  */
    BX_TOKEN_WATCH = 346,          /* BX_TOKEN_WATCH  */
    BX_TOKEN_UNWATCH = 347,        /* BX_TOKEN_UNWATCH  */
    BX_TOKEN_READ = 348,           /* BX_TOKEN_READ  */
    BX_TOKEN_WRITE = 349,          /* BX_TOKEN_WRITE  */
    BX_TOKEN_RUN_TO_LADDR = 350,   /* BX_TOKEN_RUN_TO_LADDR  */
    BX_TOKEN_SHOW = 351,           /* BX_TOKEN_SHOW  */
    BX_TOKEN_LOAD_SYMBOLS = 352,   /* BX_TOKEN_LOAD_SYMBOLS  */
    BX_TOKEN_SET_MAGIC_BREAK_POINTS = 353, /* BX_TOKEN_SET_MAGIC_BREAK_POINTS  */
    BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS = 354, /* BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS  */
    BX_TOKEN_SYMBOLS = 355,        /* BX_TOKEN_SYMBOLS  */
    BX_TOKEN_LIST_SYMBOLS = 356,   /* BX_TOKEN_LIST_SYMBOLS  */
    BX_TOKEN_GLOBAL = 357,         /* BX_TOKEN_GLOBAL  */
    BX_TOKEN_WHERE = 358,          /* BX_TOKEN_WHERE  */
    BX_TOKEN_PRINT_STRING = 359,   /* BX_TOKEN_PRINT_STRING  */
    BX_TOKEN_NUMERIC = 360,        /* BX_TOKEN_NUMERIC  */
    BX_TOKEN_PAGE = 361,           /* BX_TOKEN_PAGE  */
    BX_TOKEN_HELP = 362,           /* BX_TOKEN_HELP  */
    BX_TOKEN_XML = 363,            /* BX_TOKEN_XML  */
    BX_TOKEN_CALC = 364,           /* BX_TOKEN_CALC  */
    BX_TOKEN_ADDLYT = 365,         /* BX_TOKEN_ADDLYT  */
    BX_TOKEN_REMLYT = 366,         /* BX_TOKEN_REMLYT  */
    BX_TOKEN_LYT = 367,            /* BX_TOKEN_LYT  */
    BX_TOKEN_SOURCE = 368,         /* BX_TOKEN_SOURCE  */
    BX_TOKEN_DEVICE = 369,         /* BX_TOKEN_DEVICE  */
    BX_TOKEN_GENERIC = 370,        /* BX_TOKEN_GENERIC  */
    BX_TOKEN_DEREF_CHR = 371,      /* BX_TOKEN_DEREF_CHR  */
    BX_TOKEN_RSHIFT = 372,         /* BX_TOKEN_RSHIFT  */
    BX_TOKEN_LSHIFT = 373,         /* BX_TOKEN_LSHIFT  */
    BX_TOKEN_EQ = 374,             /* BX_TOKEN_EQ  */
    BX_TOKEN_NE = 375,             /* BX_TOKEN_NE  */
    BX_TOKEN_LE = 376,             /* BX_TOKEN_LE  */
    BX_TOKEN_GE = 377,             /* BX_TOKEN_GE  */
    BX_TOKEN_REG_IP = 378,         /* BX_TOKEN_REG_IP  */
    BX_TOKEN_REG_EIP = 379,        /* BX_TOKEN_REG_EIP  */
    BX_TOKEN_REG_RIP = 380,        /* BX_TOKEN_REG_RIP  */
    BX_TOKEN_REG_SSP = 381,        /* BX_TOKEN_REG_SSP  */
    SEGOFF = 382,                  /* SEGOFF  */
    NOT = 383,                     /* NOT  */
    NEG = 384,                     /* NEG  */
    INDIRECT = 385                 /* INDIRECT  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define BX_TOKEN_8BH_REG 258
#define BX_TOKEN_8BL_REG 259
#define BX_TOKEN_16B_REG 260
#define BX_TOKEN_32B_REG 261
#define BX_TOKEN_64B_REG 262
#define BX_TOKEN_CS 263
#define BX_TOKEN_ES 264
#define BX_TOKEN_SS 265
#define BX_TOKEN_DS 266
#define BX_TOKEN_FS 267
#define BX_TOKEN_GS 268
#define BX_TOKEN_OPMASK_REG 269
#define BX_TOKEN_FLAGS 270
#define BX_TOKEN_ON 271
#define BX_TOKEN_OFF 272
#define BX_TOKEN_CONTINUE 273
#define BX_TOKEN_IF 274
#define BX_TOKEN_STEPN 275
#define BX_TOKEN_STEP_OVER 276
#define BX_TOKEN_SET 277
#define BX_TOKEN_DEBUGGER 278
#define BX_TOKEN_LIST_BREAK 279
#define BX_TOKEN_VBREAKPOINT 280
#define BX_TOKEN_LBREAKPOINT 281
#define BX_TOKEN_PBREAKPOINT 282
#define BX_TOKEN_DEL_BREAKPOINT 283
#define BX_TOKEN_ENABLE_BREAKPOINT 284
#define BX_TOKEN_DISABLE_BREAKPOINT 285
#define BX_TOKEN_INFO 286
#define BX_TOKEN_QUIT 287
#define BX_TOKEN_DETACH 288
#define BX_TOKEN_R 289
#define BX_TOKEN_REGS 290
#define BX_TOKEN_CPU 291
#define BX_TOKEN_FPU 292
#define BX_TOKEN_MMX 293
#define BX_TOKEN_XMM 294
#define BX_TOKEN_YMM 295
#define BX_TOKEN_ZMM 296
#define BX_TOKEN_AVX 297
#define BX_TOKEN_AMX 298
#define BX_TOKEN_TILE 299
#define BX_TOKEN_IDT 300
#define BX_TOKEN_IVT 301
#define BX_TOKEN_GDT 302
#define BX_TOKEN_LDT 303
#define BX_TOKEN_TSS 304
#define BX_TOKEN_TAB 305
#define BX_TOKEN_ALL 306
#define BX_TOKEN_LINUX 307
#define BX_TOKEN_DEBUG_REGS 308
#define BX_TOKEN_CONTROL_REGS 309
#define BX_TOKEN_SEGMENT_REGS 310
#define BX_TOKEN_EXAMINE 311
#define BX_TOKEN_XFORMAT 312
#define BX_TOKEN_DISFORMAT 313
#define BX_TOKEN_RESTORE 314
#define BX_TOKEN_WRITEMEM 315
#define BX_TOKEN_LOADMEM 316
#define BX_TOKEN_SETPMEM 317
#define BX_TOKEN_DEREF 318
#define BX_TOKEN_SYMBOLNAME 319
#define BX_TOKEN_QUERY 320
#define BX_TOKEN_PENDING 321
#define BX_TOKEN_TAKE 322
#define BX_TOKEN_DMA 323
#define BX_TOKEN_IRQ 324
#define BX_TOKEN_SMI 325
#define BX_TOKEN_NMI 326
#define BX_TOKEN_TLB 327
#define BX_TOKEN_DISASM 328
#define BX_TOKEN_INSTRUMENT 329
#define BX_TOKEN_STRING 330
#define BX_TOKEN_STOP 331
#define BX_TOKEN_DOIT 332
#define BX_TOKEN_CRC 333
#define BX_TOKEN_TRACE 334
#define BX_TOKEN_TRACEREG 335
#define BX_TOKEN_TRACEMEM 336
#define BX_TOKEN_SWITCH_MODE 337
#define BX_TOKEN_SIZE 338
#define BX_TOKEN_PTIME 339
#define BX_TOKEN_TIMEBP_ABSOLUTE 340
#define BX_TOKEN_TIMEBP 341
#define BX_TOKEN_MODEBP 342
#define BX_TOKEN_VMEXITBP 343
#define BX_TOKEN_PRINT_STACK 344
#define BX_TOKEN_BT 345
#define BX_TOKEN_WATCH 346
#define BX_TOKEN_UNWATCH 347
#define BX_TOKEN_READ 348
#define BX_TOKEN_WRITE 349
#define BX_TOKEN_RUN_TO_LADDR 350
#define BX_TOKEN_SHOW 351
#define BX_TOKEN_LOAD_SYMBOLS 352
#define BX_TOKEN_SET_MAGIC_BREAK_POINTS 353
#define BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS 354
#define BX_TOKEN_SYMBOLS 355
#define BX_TOKEN_LIST_SYMBOLS 356
#define BX_TOKEN_GLOBAL 357
#define BX_TOKEN_WHERE 358
#define BX_TOKEN_PRINT_STRING 359
#define BX_TOKEN_NUMERIC 360
#define BX_TOKEN_PAGE 361
#define BX_TOKEN_HELP 362
#define BX_TOKEN_XML 363
#define BX_TOKEN_CALC 364
#define BX_TOKEN_ADDLYT 365
#define BX_TOKEN_REMLYT 366
#define BX_TOKEN_LYT 367
#define BX_TOKEN_SOURCE 368
#define BX_TOKEN_DEVICE 369
#define BX_TOKEN_GENERIC 370
#define BX_TOKEN_DEREF_CHR 371
#define BX_TOKEN_RSHIFT 372
#define BX_TOKEN_LSHIFT 373
#define BX_TOKEN_EQ 374
#define BX_TOKEN_NE 375
#define BX_TOKEN_LE 376
#define BX_TOKEN_GE 377
#define BX_TOKEN_REG_IP 378
#define BX_TOKEN_REG_EIP 379
#define BX_TOKEN_REG_RIP 380
#define BX_TOKEN_REG_SSP 381
#define SEGOFF 382
#define NOT 383
#define NEG 384
#define INDIRECT 385

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 15 "bx_parser.y"

  char    *sval;
  Bit64u   uval;
  unsigned bval;

#line 407 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern "C" YYSTYPE bxlval;


int bxparse (void);


#endif /* !YY_BX_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_BX_TOKEN_8BH_REG = 3,           /* BX_TOKEN_8BH_REG  */
  YYSYMBOL_BX_TOKEN_8BL_REG = 4,           /* BX_TOKEN_8BL_REG  */
  YYSYMBOL_BX_TOKEN_16B_REG = 5,           /* BX_TOKEN_16B_REG  */
  YYSYMBOL_BX_TOKEN_32B_REG = 6,           /* BX_TOKEN_32B_REG  */
  YYSYMBOL_BX_TOKEN_64B_REG = 7,           /* BX_TOKEN_64B_REG  */
  YYSYMBOL_BX_TOKEN_CS = 8,                /* BX_TOKEN_CS  */
  YYSYMBOL_BX_TOKEN_ES = 9,                /* BX_TOKEN_ES  */
  YYSYMBOL_BX_TOKEN_SS = 10,               /* BX_TOKEN_SS  */
  YYSYMBOL_BX_TOKEN_DS = 11,               /* BX_TOKEN_DS  */
  YYSYMBOL_BX_TOKEN_FS = 12,               /* BX_TOKEN_FS  */
  YYSYMBOL_BX_TOKEN_GS = 13,               /* BX_TOKEN_GS  */
  YYSYMBOL_BX_TOKEN_OPMASK_REG = 14,       /* BX_TOKEN_OPMASK_REG  */
  YYSYMBOL_BX_TOKEN_FLAGS = 15,            /* BX_TOKEN_FLAGS  */
  YYSYMBOL_BX_TOKEN_ON = 16,               /* BX_TOKEN_ON  */
  YYSYMBOL_BX_TOKEN_OFF = 17,              /* BX_TOKEN_OFF  */
  YYSYMBOL_BX_TOKEN_CONTINUE = 18,         /* BX_TOKEN_CONTINUE  */
  YYSYMBOL_BX_TOKEN_IF = 19,               /* BX_TOKEN_IF  */
  YYSYMBOL_BX_TOKEN_STEPN = 20,            /* BX_TOKEN_STEPN  */
  YYSYMBOL_BX_TOKEN_STEP_OVER = 21,        /* BX_TOKEN_STEP_OVER  */
  YYSYMBOL_BX_TOKEN_SET = 22,              /* BX_TOKEN_SET  */
  YYSYMBOL_BX_TOKEN_DEBUGGER = 23,         /* BX_TOKEN_DEBUGGER  */
  YYSYMBOL_BX_TOKEN_LIST_BREAK = 24,       /* BX_TOKEN_LIST_BREAK  */
  YYSYMBOL_BX_TOKEN_VBREAKPOINT = 25,      /* BX_TOKEN_VBREAKPOINT  */
  YYSYMBOL_BX_TOKEN_LBREAKPOINT = 26,      /* BX_TOKEN_LBREAKPOINT  */
  YYSYMBOL_BX_TOKEN_PBREAKPOINT = 27,      /* BX_TOKEN_PBREAKPOINT  */
  YYSYMBOL_BX_TOKEN_DEL_BREAKPOINT = 28,   /* BX_TOKEN_DEL_BREAKPOINT  */
  YYSYMBOL_BX_TOKEN_ENABLE_BREAKPOINT = 29, /* BX_TOKEN_ENABLE_BREAKPOINT  */
  YYSYMBOL_BX_TOKEN_DISABLE_BREAKPOINT = 30, /* BX_TOKEN_DISABLE_BREAKPOINT  */
  YYSYMBOL_BX_TOKEN_INFO = 31,             /* BX_TOKEN_INFO  */
  YYSYMBOL_BX_TOKEN_QUIT = 32,             /* BX_TOKEN_QUIT  */
  YYSYMBOL_BX_TOKEN_DETACH = 33,           /* BX_TOKEN_DETACH  */
  YYSYMBOL_BX_TOKEN_R = 34,                /* BX_TOKEN_R  */
  YYSYMBOL_BX_TOKEN_REGS = 35,             /* BX_TOKEN_REGS  */
  YYSYMBOL_BX_TOKEN_CPU = 36,              /* BX_TOKEN_CPU  */
  YYSYMBOL_BX_TOKEN_FPU = 37,              /* BX_TOKEN_FPU  */
  YYSYMBOL_BX_TOKEN_MMX = 38,              /* BX_TOKEN_MMX  */
  YYSYMBOL_BX_TOKEN_XMM = 39,              /* BX_TOKEN_XMM  */
  YYSYMBOL_BX_TOKEN_YMM = 40,              /* BX_TOKEN_YMM  */
  YYSYMBOL_BX_TOKEN_ZMM = 41,              /* BX_TOKEN_ZMM  */
  YYSYMBOL_BX_TOKEN_AVX = 42,              /* BX_TOKEN_AVX  */
  YYSYMBOL_BX_TOKEN_AMX = 43,              /* BX_TOKEN_AMX  */
  YYSYMBOL_BX_TOKEN_TILE = 44,             /* BX_TOKEN_TILE  */
  YYSYMBOL_BX_TOKEN_IDT = 45,              /* BX_TOKEN_IDT  */
  YYSYMBOL_BX_TOKEN_IVT = 46,              /* BX_TOKEN_IVT  */
  YYSYMBOL_BX_TOKEN_GDT = 47,              /* BX_TOKEN_GDT  */
  YYSYMBOL_BX_TOKEN_LDT = 48,              /* BX_TOKEN_LDT  */
  YYSYMBOL_BX_TOKEN_TSS = 49,              /* BX_TOKEN_TSS  */
  YYSYMBOL_BX_TOKEN_TAB = 50,              /* BX_TOKEN_TAB  */
  YYSYMBOL_BX_TOKEN_ALL = 51,              /* BX_TOKEN_ALL  */
  YYSYMBOL_BX_TOKEN_LINUX = 52,            /* BX_TOKEN_LINUX  */
  YYSYMBOL_BX_TOKEN_DEBUG_REGS = 53,       /* BX_TOKEN_DEBUG_REGS  */
  YYSYMBOL_BX_TOKEN_CONTROL_REGS = 54,     /* BX_TOKEN_CONTROL_REGS  */
  YYSYMBOL_BX_TOKEN_SEGMENT_REGS = 55,     /* BX_TOKEN_SEGMENT_REGS  */
  YYSYMBOL_BX_TOKEN_EXAMINE = 56,          /* BX_TOKEN_EXAMINE  */
  YYSYMBOL_BX_TOKEN_XFORMAT = 57,          /* BX_TOKEN_XFORMAT  */
  YYSYMBOL_BX_TOKEN_DISFORMAT = 58,        /* BX_TOKEN_DISFORMAT  */
  YYSYMBOL_BX_TOKEN_RESTORE = 59,          /* BX_TOKEN_RESTORE  */
  YYSYMBOL_BX_TOKEN_WRITEMEM = 60,         /* BX_TOKEN_WRITEMEM  */
  YYSYMBOL_BX_TOKEN_LOADMEM = 61,          /* BX_TOKEN_LOADMEM  */
  YYSYMBOL_BX_TOKEN_SETPMEM = 62,          /* BX_TOKEN_SETPMEM  */
  YYSYMBOL_BX_TOKEN_DEREF = 63,            /* BX_TOKEN_DEREF  */
  YYSYMBOL_BX_TOKEN_SYMBOLNAME = 64,       /* BX_TOKEN_SYMBOLNAME  */
  YYSYMBOL_BX_TOKEN_QUERY = 65,            /* BX_TOKEN_QUERY  */
  YYSYMBOL_BX_TOKEN_PENDING = 66,          /* BX_TOKEN_PENDING  */
  YYSYMBOL_BX_TOKEN_TAKE = 67,             /* BX_TOKEN_TAKE  */
  YYSYMBOL_BX_TOKEN_DMA = 68,              /* BX_TOKEN_DMA  */
  YYSYMBOL_BX_TOKEN_IRQ = 69,              /* BX_TOKEN_IRQ  */
  YYSYMBOL_BX_TOKEN_SMI = 70,              /* BX_TOKEN_SMI  */
  YYSYMBOL_BX_TOKEN_NMI = 71,              /* BX_TOKEN_NMI  */
  YYSYMBOL_BX_TOKEN_TLB = 72,              /* BX_TOKEN_TLB  */
  YYSYMBOL_BX_TOKEN_DISASM = 73,           /* BX_TOKEN_DISASM  */
  YYSYMBOL_BX_TOKEN_INSTRUMENT = 74,       /* BX_TOKEN_INSTRUMENT  */
  YYSYMBOL_BX_TOKEN_STRING = 75,           /* BX_TOKEN_STRING  */
  YYSYMBOL_BX_TOKEN_STOP = 76,             /* BX_TOKEN_STOP  */
  YYSYMBOL_BX_TOKEN_DOIT = 77,             /* BX_TOKEN_DOIT  */
  YYSYMBOL_BX_TOKEN_CRC = 78,              /* BX_TOKEN_CRC  */
  YYSYMBOL_BX_TOKEN_TRACE = 79,            /* BX_TOKEN_TRACE  */
  YYSYMBOL_BX_TOKEN_TRACEREG = 80,         /* BX_TOKEN_TRACEREG  */
  YYSYMBOL_BX_TOKEN_TRACEMEM = 81,         /* BX_TOKEN_TRACEMEM  */
  YYSYMBOL_BX_TOKEN_SWITCH_MODE = 82,      /* BX_TOKEN_SWITCH_MODE  */
  YYSYMBOL_BX_TOKEN_SIZE = 83,             /* BX_TOKEN_SIZE  */
  YYSYMBOL_BX_TOKEN_PTIME = 84,            /* BX_TOKEN_PTIME  */
  YYSYMBOL_BX_TOKEN_TIMEBP_ABSOLUTE = 85,  /* BX_TOKEN_TIMEBP_ABSOLUTE  */
  YYSYMBOL_BX_TOKEN_TIMEBP = 86,           /* BX_TOKEN_TIMEBP  */
  YYSYMBOL_BX_TOKEN_MODEBP = 87,           /* BX_TOKEN_MODEBP  */
  YYSYMBOL_BX_TOKEN_VMEXITBP = 88,         /* BX_TOKEN_VMEXITBP  */
  YYSYMBOL_BX_TOKEN_PRINT_STACK = 89,      /* BX_TOKEN_PRINT_STACK  */
  YYSYMBOL_BX_TOKEN_BT = 90,               /* BX_TOKEN_BT  */
  YYSYMBOL_BX_TOKEN_WATCH = 91,            /* BX_TOKEN_WATCH  */
  YYSYMBOL_BX_TOKEN_UNWATCH = 92,          /* BX_TOKEN_UNWATCH  */
  YYSYMBOL_BX_TOKEN_READ = 93,             /* BX_TOKEN_READ  */
  YYSYMBOL_BX_TOKEN_WRITE = 94,            /* BX_TOKEN_WRITE  */
  YYSYMBOL_BX_TOKEN_RUN_TO_LADDR = 95,     /* BX_TOKEN_RUN_TO_LADDR  */
  YYSYMBOL_BX_TOKEN_SHOW = 96,             /* BX_TOKEN_SHOW  */
  YYSYMBOL_BX_TOKEN_LOAD_SYMBOLS = 97,     /* BX_TOKEN_LOAD_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_SET_MAGIC_BREAK_POINTS = 98, /* BX_TOKEN_SET_MAGIC_BREAK_POINTS  */
  YYSYMBOL_BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS = 99, /* BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS  */
  YYSYMBOL_BX_TOKEN_SYMBOLS = 100,         /* BX_TOKEN_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_LIST_SYMBOLS = 101,    /* BX_TOKEN_LIST_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_GLOBAL = 102,          /* BX_TOKEN_GLOBAL  */
  YYSYMBOL_BX_TOKEN_WHERE = 103,           /* BX_TOKEN_WHERE  */
  YYSYMBOL_BX_TOKEN_PRINT_STRING = 104,    /* BX_TOKEN_PRINT_STRING  */
  YYSYMBOL_BX_TOKEN_NUMERIC = 105,         /* BX_TOKEN_NUMERIC  */
  YYSYMBOL_BX_TOKEN_PAGE = 106,            /* BX_TOKEN_PAGE  */
  YYSYMBOL_BX_TOKEN_HELP = 107,            /* BX_TOKEN_HELP  */
  YYSYMBOL_BX_TOKEN_XML = 108,             /* BX_TOKEN_XML  */
  YYSYMBOL_BX_TOKEN_CALC = 109,            /* BX_TOKEN_CALC  */
  YYSYMBOL_BX_TOKEN_ADDLYT = 110,          /* BX_TOKEN_ADDLYT  */
  YYSYMBOL_BX_TOKEN_REMLYT = 111,          /* BX_TOKEN_REMLYT  */
  YYSYMBOL_BX_TOKEN_LYT = 112,             /* BX_TOKEN_LYT  */
  YYSYMBOL_BX_TOKEN_SOURCE = 113,          /* BX_TOKEN_SOURCE  */
  YYSYMBOL_BX_TOKEN_DEVICE = 114,          /* BX_TOKEN_DEVICE  */
  YYSYMBOL_BX_TOKEN_GENERIC = 115,         /* BX_TOKEN_GENERIC  */
  YYSYMBOL_BX_TOKEN_DEREF_CHR = 116,       /* BX_TOKEN_DEREF_CHR  */
  YYSYMBOL_BX_TOKEN_RSHIFT = 117,          /* BX_TOKEN_RSHIFT  */
  YYSYMBOL_BX_TOKEN_LSHIFT = 118,          /* BX_TOKEN_LSHIFT  */
  YYSYMBOL_BX_TOKEN_EQ = 119,              /* BX_TOKEN_EQ  */
  YYSYMBOL_BX_TOKEN_NE = 120,              /* BX_TOKEN_NE  */
  YYSYMBOL_BX_TOKEN_LE = 121,              /* BX_TOKEN_LE  */
  YYSYMBOL_BX_TOKEN_GE = 122,              /* BX_TOKEN_GE  */
  YYSYMBOL_BX_TOKEN_REG_IP = 123,          /* BX_TOKEN_REG_IP  */
  YYSYMBOL_BX_TOKEN_REG_EIP = 124,         /* BX_TOKEN_REG_EIP  */
  YYSYMBOL_BX_TOKEN_REG_RIP = 125,         /* BX_TOKEN_REG_RIP  */
  YYSYMBOL_BX_TOKEN_REG_SSP = 126,         /* BX_TOKEN_REG_SSP  */
  YYSYMBOL_SEGOFF = 127,                   /* SEGOFF  */
  YYSYMBOL_128_ = 128,                     /* '|'  */
  YYSYMBOL_129_ = 129,                     /* '^'  */
  YYSYMBOL_130_ = 130,                     /* '&'  */
  YYSYMBOL_131_ = 131,                     /* '<'  */
  YYSYMBOL_132_ = 132,                     /* '>'  */
  YYSYMBOL_133_ = 133,                     /* '+'  */
  YYSYMBOL_134_ = 134,                     /* '-'  */
  YYSYMBOL_135_ = 135,                     /* '*'  */
  YYSYMBOL_136_ = 136,                     /* '/'  */
  YYSYMBOL_NOT = 137,                      /* NOT  */
  YYSYMBOL_NEG = 138,                      /* NEG  */
  YYSYMBOL_INDIRECT = 139,                 /* INDIRECT  */
  YYSYMBOL_140_ = 140,                     /* ':'  */
  YYSYMBOL_141_n_ = 141,                   /* '\n'  */
  YYSYMBOL_142_ = 142,                     /* '='  */
  YYSYMBOL_143_ = 143,                     /* '!'  */
  YYSYMBOL_144_ = 144,                     /* '('  */
  YYSYMBOL_145_ = 145,                     /* ')'  */
  YYSYMBOL_146_ = 146,                     /* '@'  */
  YYSYMBOL_YYACCEPT = 147,                 /* $accept  */
  YYSYMBOL_commands = 148,                 /* commands  */
  YYSYMBOL_command = 149,                  /* command  */
  YYSYMBOL_BX_TOKEN_TOGGLE_ON_OFF = 150,   /* BX_TOKEN_TOGGLE_ON_OFF  */
  YYSYMBOL_BX_TOKEN_REGISTERS = 151,       /* BX_TOKEN_REGISTERS  */
  YYSYMBOL_BX_TOKEN_SEGREG = 152,          /* BX_TOKEN_SEGREG  */
  YYSYMBOL_timebp_command = 153,           /* timebp_command  */
  YYSYMBOL_modebp_command = 154,           /* modebp_command  */
  YYSYMBOL_vmexitbp_command = 155,         /* vmexitbp_command  */
  YYSYMBOL_show_command = 156,             /* show_command  */
  YYSYMBOL_page_command = 157,             /* page_command  */
  YYSYMBOL_tlb_command = 158,              /* tlb_command  */
  YYSYMBOL_ptime_command = 159,            /* ptime_command  */
  YYSYMBOL_trace_command = 160,            /* trace_command  */
  YYSYMBOL_trace_reg_command = 161,        /* trace_reg_command  */
  YYSYMBOL_trace_mem_command = 162,        /* trace_mem_command  */
  YYSYMBOL_print_stack_command = 163,      /* print_stack_command  */
  YYSYMBOL_backtrace_command = 164,        /* backtrace_command  */
  YYSYMBOL_watch_point_command = 165,      /* watch_point_command  */
  YYSYMBOL_symbol_command = 166,           /* symbol_command  */
  YYSYMBOL_set_magic_break_points_command = 167, /* set_magic_break_points_command  */
  YYSYMBOL_clr_magic_break_points_command = 168, /* clr_magic_break_points_command  */
  YYSYMBOL_where_command = 169,            /* where_command  */
  YYSYMBOL_print_string_command = 170,     /* print_string_command  */
  YYSYMBOL_continue_command = 171,         /* continue_command  */
  YYSYMBOL_stepN_command = 172,            /* stepN_command  */
  YYSYMBOL_step_over_command = 173,        /* step_over_command  */
  YYSYMBOL_run_to_laddr_command = 174,     /* run_to_laddr_command  */
  YYSYMBOL_cpu_command = 175,              /* cpu_command  */
  YYSYMBOL_set_command = 176,              /* set_command  */
  YYSYMBOL_breakpoint_command = 177,       /* breakpoint_command  */
  YYSYMBOL_blist_command = 178,            /* blist_command  */
  YYSYMBOL_slist_command = 179,            /* slist_command  */
  YYSYMBOL_info_command = 180,             /* info_command  */
  YYSYMBOL_optional_numeric = 181,         /* optional_numeric  */
  YYSYMBOL_regs_command = 182,             /* regs_command  */
  YYSYMBOL_fpu_regs_command = 183,         /* fpu_regs_command  */
  YYSYMBOL_mmx_regs_command = 184,         /* mmx_regs_command  */
  YYSYMBOL_xmm_regs_command = 185,         /* xmm_regs_command  */
  YYSYMBOL_ymm_regs_command = 186,         /* ymm_regs_command  */
  YYSYMBOL_zmm_regs_command = 187,         /* zmm_regs_command  */
  YYSYMBOL_amx_regs_command = 188,         /* amx_regs_command  */
  YYSYMBOL_print_tile_command = 189,       /* print_tile_command  */
  YYSYMBOL_segment_regs_command = 190,     /* segment_regs_command  */
  YYSYMBOL_control_regs_command = 191,     /* control_regs_command  */
  YYSYMBOL_debug_regs_command = 192,       /* debug_regs_command  */
  YYSYMBOL_delete_command = 193,           /* delete_command  */
  YYSYMBOL_bpe_command = 194,              /* bpe_command  */
  YYSYMBOL_bpd_command = 195,              /* bpd_command  */
  YYSYMBOL_quit_command = 196,             /* quit_command  */
  YYSYMBOL_detach_command = 197,           /* detach_command  */
  YYSYMBOL_examine_command = 198,          /* examine_command  */
  YYSYMBOL_restore_command = 199,          /* restore_command  */
  YYSYMBOL_writemem_command = 200,         /* writemem_command  */
  YYSYMBOL_loadmem_command = 201,          /* loadmem_command  */
  YYSYMBOL_setpmem_command = 202,          /* setpmem_command  */
  YYSYMBOL_deref_command = 203,            /* deref_command  */
  YYSYMBOL_query_command = 204,            /* query_command  */
  YYSYMBOL_take_command = 205,             /* take_command  */
  YYSYMBOL_disassemble_command = 206,      /* disassemble_command  */
  YYSYMBOL_instrument_command = 207,       /* instrument_command  */
  YYSYMBOL_doit_command = 208,             /* doit_command  */
  YYSYMBOL_crc_command = 209,              /* crc_command  */
  YYSYMBOL_help_command = 210,             /* help_command  */
  YYSYMBOL_calc_command = 211,             /* calc_command  */
  YYSYMBOL_addlyt_command = 212,           /* addlyt_command  */
  YYSYMBOL_remlyt_command = 213,           /* remlyt_command  */
  YYSYMBOL_lyt_command = 214,              /* lyt_command  */
  YYSYMBOL_if_command = 215,               /* if_command  */
  YYSYMBOL_vexpression = 216,              /* vexpression  */
  YYSYMBOL_expression = 217                /* expression  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  361
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2643

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  147
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  71
/* YYNRULES -- Number of rules.  */
#define YYNRULES  343
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  667

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   385


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     141,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   143,     2,     2,     2,     2,   130,     2,
     144,   145,   135,   133,     2,   134,     2,   136,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   140,     2,
     131,   142,   132,     2,   146,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   129,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   128,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   137,   138,   139
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   182,   182,   183,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   257,
     258,   263,   264,   269,   270,   271,   272,   273,   274,   279,
     284,   292,   300,   308,   313,   318,   323,   328,   333,   341,
     349,   357,   365,   373,   381,   389,   394,   402,   407,   415,
     420,   425,   430,   435,   440,   445,   450,   455,   460,   465,
     473,   478,   483,   488,   496,   501,   509,   514,   522,   530,
     538,   543,   551,   556,   561,   566,   574,   582,   590,   597,
     602,   607,   612,   617,   622,   627,   632,   637,   642,   650,
     655,   660,   665,   670,   675,   680,   685,   690,   695,   700,
     705,   710,   718,   726,   731,   739,   744,   749,   754,   759,
     764,   769,   774,   779,   784,   789,   794,   799,   804,   809,
     814,   819,   827,   828,   831,   839,   847,   855,   863,   871,
     879,   887,   895,   903,   911,   919,   927,   934,   942,   950,
     958,   963,   968,   973,   981,   989,   997,  1005,  1013,  1021,
    1029,  1034,  1039,  1044,  1049,  1057,  1062,  1067,  1072,  1077,
    1082,  1087,  1092,  1100,  1106,  1111,  1119,  1127,  1135,  1140,
    1146,  1152,  1159,  1164,  1169,  1175,  1181,  1187,  1192,  1197,
    1202,  1207,  1212,  1217,  1222,  1228,  1234,  1240,  1248,  1253,
    1258,  1263,  1268,  1273,  1278,  1283,  1288,  1293,  1298,  1303,
    1308,  1313,  1318,  1323,  1328,  1333,  1338,  1343,  1348,  1353,
    1358,  1363,  1372,  1383,  1389,  1402,  1407,  1418,  1423,  1439,
    1455,  1467,  1479,  1484,  1490,  1495,  1500,  1505,  1513,  1523,
    1532,  1540,  1548,  1559,  1560,  1561,  1562,  1563,  1564,  1565,
    1566,  1567,  1568,  1569,  1570,  1571,  1572,  1573,  1574,  1575,
    1576,  1577,  1578,  1579,  1580,  1581,  1582,  1583,  1584,  1590,
    1591,  1592,  1593,  1594,  1595,  1596,  1597,  1598,  1599,  1600,
    1601,  1602,  1603,  1604,  1605,  1606,  1607,  1608,  1609,  1610,
    1611,  1612,  1613,  1614,  1615,  1616,  1617,  1618,  1619,  1620,
    1621,  1622,  1623,  1624
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "BX_TOKEN_8BH_REG",
  "BX_TOKEN_8BL_REG", "BX_TOKEN_16B_REG", "BX_TOKEN_32B_REG",
  "BX_TOKEN_64B_REG", "BX_TOKEN_CS", "BX_TOKEN_ES", "BX_TOKEN_SS",
  "BX_TOKEN_DS", "BX_TOKEN_FS", "BX_TOKEN_GS", "BX_TOKEN_OPMASK_REG",
  "BX_TOKEN_FLAGS", "BX_TOKEN_ON", "BX_TOKEN_OFF", "BX_TOKEN_CONTINUE",
  "BX_TOKEN_IF", "BX_TOKEN_STEPN", "BX_TOKEN_STEP_OVER", "BX_TOKEN_SET",
  "BX_TOKEN_DEBUGGER", "BX_TOKEN_LIST_BREAK", "BX_TOKEN_VBREAKPOINT",
  "BX_TOKEN_LBREAKPOINT", "BX_TOKEN_PBREAKPOINT",
  "BX_TOKEN_DEL_BREAKPOINT", "BX_TOKEN_ENABLE_BREAKPOINT",
  "BX_TOKEN_DISABLE_BREAKPOINT", "BX_TOKEN_INFO", "BX_TOKEN_QUIT",
  "BX_TOKEN_DETACH", "BX_TOKEN_R", "BX_TOKEN_REGS", "BX_TOKEN_CPU",
  "BX_TOKEN_FPU", "BX_TOKEN_MMX", "BX_TOKEN_XMM", "BX_TOKEN_YMM",
  "BX_TOKEN_ZMM", "BX_TOKEN_AVX", "BX_TOKEN_AMX", "BX_TOKEN_TILE",
  "BX_TOKEN_IDT", "BX_TOKEN_IVT", "BX_TOKEN_GDT", "BX_TOKEN_LDT",
  "BX_TOKEN_TSS", "BX_TOKEN_TAB", "BX_TOKEN_ALL", "BX_TOKEN_LINUX",
  "BX_TOKEN_DEBUG_REGS", "BX_TOKEN_CONTROL_REGS", "BX_TOKEN_SEGMENT_REGS",
  "BX_TOKEN_EXAMINE", "BX_TOKEN_XFORMAT", "BX_TOKEN_DISFORMAT",
  "BX_TOKEN_RESTORE", "BX_TOKEN_WRITEMEM", "BX_TOKEN_LOADMEM",
  "BX_TOKEN_SETPMEM", "BX_TOKEN_DEREF", "BX_TOKEN_SYMBOLNAME",
  "BX_TOKEN_QUERY", "BX_TOKEN_PENDING", "BX_TOKEN_TAKE", "BX_TOKEN_DMA",
  "BX_TOKEN_IRQ", "BX_TOKEN_SMI", "BX_TOKEN_NMI", "BX_TOKEN_TLB",
  "BX_TOKEN_DISASM", "BX_TOKEN_INSTRUMENT", "BX_TOKEN_STRING",
  "BX_TOKEN_STOP", "BX_TOKEN_DOIT", "BX_TOKEN_CRC", "BX_TOKEN_TRACE",
  "BX_TOKEN_TRACEREG", "BX_TOKEN_TRACEMEM", "BX_TOKEN_SWITCH_MODE",
  "BX_TOKEN_SIZE", "BX_TOKEN_PTIME", "BX_TOKEN_TIMEBP_ABSOLUTE",
  "BX_TOKEN_TIMEBP", "BX_TOKEN_MODEBP", "BX_TOKEN_VMEXITBP",
  "BX_TOKEN_PRINT_STACK", "BX_TOKEN_BT", "BX_TOKEN_WATCH",
  "BX_TOKEN_UNWATCH", "BX_TOKEN_READ", "BX_TOKEN_WRITE",
  "BX_TOKEN_RUN_TO_LADDR", "BX_TOKEN_SHOW", "BX_TOKEN_LOAD_SYMBOLS",
  "BX_TOKEN_SET_MAGIC_BREAK_POINTS", "BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS",
  "BX_TOKEN_SYMBOLS", "BX_TOKEN_LIST_SYMBOLS", "BX_TOKEN_GLOBAL",
  "BX_TOKEN_WHERE", "BX_TOKEN_PRINT_STRING", "BX_TOKEN_NUMERIC",
  "BX_TOKEN_PAGE", "BX_TOKEN_HELP", "BX_TOKEN_XML", "BX_TOKEN_CALC",
  "BX_TOKEN_ADDLYT", "BX_TOKEN_REMLYT", "BX_TOKEN_LYT", "BX_TOKEN_SOURCE",
  "BX_TOKEN_DEVICE", "BX_TOKEN_GENERIC", "BX_TOKEN_DEREF_CHR",
  "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT", "BX_TOKEN_EQ", "BX_TOKEN_NE",
  "BX_TOKEN_LE", "BX_TOKEN_GE", "BX_TOKEN_REG_IP", "BX_TOKEN_REG_EIP",
  "BX_TOKEN_REG_RIP", "BX_TOKEN_REG_SSP", "SEGOFF", "'|'", "'^'", "'&'",
  "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "NOT", "NEG", "INDIRECT",
  "':'", "'\\n'", "'='", "'!'", "'('", "')'", "'@'", "$accept", "commands",
  "command", "BX_TOKEN_TOGGLE_ON_OFF", "BX_TOKEN_REGISTERS",
  "BX_TOKEN_SEGREG", "timebp_command", "modebp_command",
  "vmexitbp_command", "show_command", "page_command", "tlb_command",
  "ptime_command", "trace_command", "trace_reg_command",
  "trace_mem_command", "print_stack_command", "backtrace_command",
  "watch_point_command", "symbol_command",
  "set_magic_break_points_command", "clr_magic_break_points_command",
  "where_command", "print_string_command", "continue_command",
  "stepN_command", "step_over_command", "run_to_laddr_command",
  "cpu_command", "set_command", "breakpoint_command", "blist_command",
  "slist_command", "info_command", "optional_numeric", "regs_command",
  "fpu_regs_command", "mmx_regs_command", "xmm_regs_command",
  "ymm_regs_command", "zmm_regs_command", "amx_regs_command",
  "print_tile_command", "segment_regs_command", "control_regs_command",
  "debug_regs_command", "delete_command", "bpe_command", "bpd_command",
  "quit_command", "detach_command", "examine_command", "restore_command",
  "writemem_command", "loadmem_command", "setpmem_command",
  "deref_command", "query_command", "take_command", "disassemble_command",
  "instrument_command", "doit_command", "crc_command", "help_command",
  "calc_command", "addlyt_command", "remlyt_command", "lyt_command",
  "if_command", "vexpression", "expression", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-202)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-342)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     629,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,   -13,  1439,   -35,  -123,   196,  -101,  1481,
    1075,  1088,   -49,   -44,   -40,    69,   -66,   -47,  -202,  -202,
     -28,   -43,   -42,   -34,   -29,   -21,   -19,    -4,   -15,   -11,
     -10,  1033,    38,    58,    59,  1439,  1439,    81,   124,  1439,
     338,   -18,  -202,  1439,  1439,    52,    52,    52,    -6,  1439,
    1439,    16,    24,   -68,   -61,    17,  1178,  1439,   -12,   -20,
     -62,   -56,   -39,    27,  1439,  -202,  1439,  1620,  1439,   107,
      43,    44,  -202,  -202,  -202,  -202,  1439,  1439,  -202,  1439,
    1439,  1439,   485,  -202,    47,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  2478,  1439,
    -202,  1459,    91,   -58,  -202,  -202,    55,    68,    85,    86,
      88,    89,    52,    90,    98,    99,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  1493,
    -202,  1493,  1493,  -202,   199,    -9,  -202,   319,  1439,  -202,
     704,    82,   112,   118,   126,   132,   133,  1439,  1439,  1439,
    1439,   138,   139,   153,   -37,   -53,  -202,  -202,   163,  -202,
    -202,  -202,  -202,  -202,  -202,   164,  -202,  -202,  -202,  1232,
    -202,  1618,   173,  1439,  1439,  1001,  1001,   168,   -41,   171,
     177,   178,  1646,  1256,   181,   123,  -202,    20,   182,   183,
     212,  1672,  1001,  -202,  -202,   213,   216,   217,  -202,  1698,
    1724,  -202,  -202,   218,  -202,   221,  -202,   222,  1439,   240,
    1439,  1439,  -202,  -202,  1750,  1776,   241,   242,   -60,   243,
    -202,  1284,   239,   244,  -202,   251,  -202,   267,  -202,  -202,
    1802,  1828,   271,   275,   276,   277,   278,   281,   282,   289,
     290,   291,   316,   317,   324,   325,   326,   327,   328,   329,
     330,   334,   337,   339,   342,   345,   359,   360,   361,   367,
     386,   389,   390,   391,   392,   393,   394,   396,   401,   402,
     408,   410,   426,   427,   437,   438,   444,   446,   452,   464,
     465,   466,   480,   483,   484,   486,   489,   503,   504,   505,
    -202,   511,  1854,   530,  -202,  -202,   191,   191,   191,  1400,
     191,  -202,  -202,  -202,  1439,  1439,  1439,  1439,  1439,  1439,
    1439,  1439,  1439,  1439,  1439,  1439,  1439,  1439,  1439,  1439,
    1439,  1880,  -202,   533,   534,  -202,  1439,  1439,  1439,  1439,
    1439,  1439,   535,  1439,  1439,  1439,  -202,  -202,   121,  1493,
    1493,  1493,  1493,  1493,  1493,  1493,  1493,  1493,  1493,  1493,
     262,  -202,   265,  -202,   -17,   286,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  1439,  2478,  1439,  1439,  1439,  -202,  -202,
    -202,   536,  -202,   -33,   -32,  -202,  -202,  -202,  -202,  1906,
    -202,   537,  1001,  1932,  1439,  1439,  1001,  1958,  -202,   538,
    -202,  -202,  -202,  -202,  -202,  -202,   167,  -202,   456,  -202,
    1984,  -202,  -202,  -202,  -202,  2010,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,   773,  -202,   805,   857,  -202,  -202,
    -202,  -202,   539,  -202,  -202,  -202,  2036,  1336,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,   191,   142,   142,   293,   293,
     739,   739,  2503,   482,   626,   739,   739,   190,   190,    46,
      46,  2478,  -202,  -202,  -202,  2062,  2088,  2114,  2140,  2166,
    2192,  -202,  2218,  2244,  2270,  -202,  -202,   420,   420,  1036,
    1193,   128,   -64,   -64,   318,   318,   933,   540,   545,   548,
    -202,   546,   552,   554,   556,   557,  -202,   558,  -202,   559,
    -202,  -202,  -202,  2296,  -202,   220,    51,  2322,  -202,  -202,
    -202,  2348,   564,  -202,  -202,  -202,  2374,  -202,  2400,  -202,
    2426,  -202,  -202,  -202,  2452,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,   636,  -202,  -202,  -202,   571,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,   581,  -202,  -202
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
      67,   312,   311,   313,   314,   315,    73,    74,    75,    76,
      77,    78,   316,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    71,    72,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   310,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   309,     0,     0,     0,     0,
       0,     0,   318,   319,   320,   321,     0,     0,    68,     0,
       0,     0,     0,     3,     0,   317,    46,    47,    48,    56,
      54,    55,    45,    42,    43,    44,    49,    50,    53,    57,
      51,    52,    58,    59,     4,     5,     6,     7,     9,     8,
      10,    23,    24,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    22,    21,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    60,    61,    62,    63,    64,    65,    66,     0,
     120,     0,     0,     0,   122,   126,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   152,   286,   285,   287,
     288,   289,   290,   284,   283,   292,   293,   294,   295,     0,
     139,     0,     0,   291,     0,   310,   142,     0,     0,   147,
       0,     0,     0,     0,     0,     0,     0,   172,   172,   172,
     172,     0,     0,     0,     0,     0,   188,   189,     0,   175,
     176,   177,   178,   179,   180,     0,   184,   183,   182,     0,
     193,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   205,     0,     0,     0,
       0,     0,     0,    69,    70,     0,     0,     0,    91,     0,
       0,    81,    82,     0,    95,     0,    97,     0,     0,     0,
       0,     0,   101,   108,     0,     0,     0,     0,     0,     0,
      88,     0,     0,     0,   114,     0,   116,     0,   153,   118,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     277,     0,     0,     0,   280,   281,   340,   341,   339,     0,
     342,     1,     2,   174,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   282,     0,     0,   123,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   307,   306,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   145,     0,   143,   341,     0,   148,   185,   186,   187,
     163,   155,   156,   172,   173,   172,   172,   172,   162,   161,
     164,     0,   165,     0,     0,   167,   128,   181,   191,     0,
     192,     0,     0,     0,     0,     0,     0,     0,   199,     0,
     200,   202,   203,   204,    90,   208,     0,   211,     0,   206,
       0,   214,   213,   215,   216,     0,    92,    93,    94,    80,
      79,    96,    98,   100,     0,    99,     0,     0,   109,   127,
      85,    84,     0,    86,    83,   110,     0,     0,   115,   117,
     154,   119,    89,   220,   221,   222,   266,   230,   224,   225,
     226,   227,   228,   229,   268,   218,   219,   248,   249,   250,
     251,   252,   253,   256,   255,   254,   264,   237,   257,   258,
     259,   260,   261,   265,   233,   234,   235,   236,   238,   240,
     239,   231,   232,   241,   242,   262,   263,   223,   269,   243,
     244,   245,   246,   274,   267,   276,   270,   271,   272,   273,
     275,   247,   278,   279,   343,   327,   328,   329,   335,   336,
     337,   338,   330,   331,   332,   334,   333,   323,   324,   325,
     326,   322,   121,   124,   125,     0,     0,     0,     0,     0,
       0,   129,     0,     0,     0,   308,   300,   301,   302,   303,
     304,   305,   296,   297,   298,   299,     0,     0,     0,     0,
     150,     0,     0,     0,     0,     0,   166,     0,   170,     0,
     168,   190,   194,     0,   196,   324,   325,     0,   198,   201,
     209,     0,     0,   207,   217,   102,     0,   103,     0,   104,
       0,    87,   111,   112,     0,   132,   131,   133,   134,   135,
     130,   136,   137,   138,     0,   140,   146,   144,     0,   149,
     157,   158,   159,   160,   171,   169,   195,   197,   210,   212,
     105,   106,   107,   113,     0,   151,   141
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -202,  -202,   637,   -45,   654,    -2,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -201,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,
    -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -202,  -188,
       0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    92,    93,   255,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   423,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   194,
     424
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     158,   396,   599,   397,   398,   276,   159,   425,   426,   427,
     410,   256,   257,   283,   161,   175,   162,   193,   165,   285,
     197,   200,   433,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,   267,   287,   263,   431,   277,
     176,   231,   607,   609,   265,   235,   236,   384,   482,   242,
     247,   268,   399,   251,   252,   281,   201,   248,   249,   259,
     260,   202,   434,   278,   449,   203,   274,   275,   253,   254,
     163,   407,   408,   264,   290,   216,   291,   218,   352,   284,
     266,   483,   282,   385,   204,   286,   356,   357,   435,   358,
     359,   360,   158,   269,   217,    52,   205,   250,   219,   220,
     450,   225,   288,   279,   432,   206,   164,   221,   608,   610,
     270,   271,   222,   232,   207,   208,   209,   210,   211,   212,
     223,   213,   224,   380,   600,    75,   226,   392,   160,   280,
     227,   228,   411,   233,   234,   258,   364,   365,   366,   367,
     368,   369,   370,    82,    83,    84,    85,   237,   371,   372,
     373,   374,   375,   376,   444,   445,   379,   261,   272,   381,
     380,   459,   364,    89,    90,   262,    91,  -341,   289,   214,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,   353,   215,   354,   355,   380,   193,   363,   193,
     193,   380,   238,   239,   240,   241,   383,   386,   414,   166,
     167,   168,   169,   170,     6,     7,     8,     9,    10,    11,
     387,   586,   587,   588,   589,   590,   591,   592,   593,   594,
     595,   596,   602,   417,   603,   604,   605,   388,   389,   439,
     390,   391,   393,   442,   443,   446,   447,   399,   400,   401,
     394,   395,    52,   456,   399,   400,   401,   460,   441,   402,
     403,   404,   465,   418,   405,   406,   407,   408,   364,   419,
     171,   405,   406,   407,   408,   458,   585,   420,   474,   172,
     476,   477,    75,   421,   422,   376,   377,   378,   379,   428,
     429,   486,   380,   364,   365,   366,   367,   368,   369,   370,
      82,    83,    84,    85,   430,   371,   372,   373,   374,   375,
     376,   444,   445,   379,   436,   437,   364,   380,   620,   448,
      89,    90,   451,    91,   487,   399,   400,   401,   452,   453,
     173,   174,   457,   461,   462,   378,   379,   402,   403,   404,
     380,   380,   405,   406,   407,   408,  -340,   597,   412,   409,
     598,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,   463,   466,  -340,  -340,   467,   468,   471,
     380,   601,   472,   473,   555,   556,   557,   558,   559,   560,
     561,   562,   563,   564,   565,   566,   567,   568,   569,   570,
     571,   475,   480,   481,   484,   488,   575,   576,   577,   578,
     579,   580,   489,   582,   583,   584,   243,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   490,   364,
     365,   366,   493,    52,   369,   370,   494,   495,   496,   497,
     244,   245,   498,   499,   374,   375,   376,   377,   378,   379,
     500,   501,   502,   380,   399,   364,   365,   366,   367,   368,
     369,   370,   613,    75,   615,   616,   617,   371,   372,   373,
     374,   375,   376,   377,   378,   379,   621,   503,   504,   380,
     413,    82,    83,    84,    85,   505,   506,   507,   508,   509,
     510,   511,    86,    87,   626,   512,   628,   630,   513,   246,
     514,    89,    90,   515,    91,   361,   516,   634,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
     517,   518,   519,    13,    14,    15,    16,    17,   520,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,   521,    36,    37,
     522,   523,   524,   525,   526,   527,   399,   528,    38,    39,
      40,    41,   529,   530,    42,    43,    44,    45,    46,   531,
      47,   532,    48,   405,   406,   407,   408,    49,    50,    51,
      52,   622,    53,    54,    55,    56,    57,   533,   534,    58,
      59,    60,    61,    62,    63,    64,    65,    66,   535,   536,
      67,    68,    69,    70,    71,   537,    72,   538,    73,    74,
      75,    76,    77,   539,    78,    79,    80,    81,   364,   365,
     366,   367,   368,   369,   370,   540,   541,   542,    82,    83,
      84,    85,   373,   374,   375,   376,   377,   378,   379,    86,
      87,   543,   380,   648,   544,   545,    88,   546,    89,    90,
     547,    91,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,   548,   549,   550,    13,    14,    15,
      16,    17,   551,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,   553,    36,    37,   573,   574,   581,   606,   612,   619,
     631,   646,    38,    39,    40,    41,   647,   649,    42,    43,
      44,    45,    46,   650,    47,   651,    48,   652,   653,   654,
     655,    49,    50,    51,    52,   659,    53,    54,    55,    56,
      57,   664,   665,    58,    59,    60,    61,    62,    63,    64,
      65,    66,   666,   415,    67,    68,    69,    70,    71,   362,
      72,   351,    73,    74,    75,    76,    77,     0,    78,    79,
      80,    81,   364,   365,   366,   367,   368,   369,   370,     0,
       0,     0,    82,    83,    84,    85,     0,   374,   375,   376,
     377,   378,   379,    86,    87,     0,   380,     0,     0,     0,
      88,     0,    89,    90,     0,    91,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
     364,   365,   366,   367,   368,   369,   370,     0,     0,     0,
       0,     0,   371,   372,   373,   374,   375,   376,   377,   378,
     379,     0,     0,     0,   380,   416,     0,     0,    52,     0,
       0,     0,     0,     0,     0,   364,   365,   366,     0,     0,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,   376,   377,   378,   379,     0,     0,    75,   380,
      52,     0,     0,     0,     0,     0,     0,     0,     0,   364,
     365,   366,   367,   368,   369,   370,    82,    83,    84,    85,
       0,   371,   372,   373,   374,   375,   376,   444,   445,   379,
      75,     0,     0,   380,   625,     0,    89,    90,     0,    91,
       0,   364,   365,   366,   367,   368,   369,   370,    82,    83,
      84,    85,    52,   371,   372,   373,   374,   375,   376,   444,
     445,   379,     0,     0,     0,   380,   627,     0,    89,    90,
       0,    91,   644,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    75,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   364,   365,   366,   367,   368,   369,   370,
      82,    83,    84,    85,     0,   371,   372,   373,   374,   375,
     376,   444,   445,   379,     0,     0,     0,   380,   629,     0,
      89,    90,     0,    91,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     0,   399,
     400,   401,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   402,   403,   404,     0,     0,   405,   406,   407,   408,
       0,     0,     0,     0,   645,     0,    52,     0,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
     229,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,    75,     0,    52,     0,
       0,     0,     0,     0,     0,     0,     0,   364,   365,   366,
     367,   368,   369,   370,    82,    83,    84,    85,     0,   371,
     372,   373,   374,   375,   376,   444,   445,   379,    75,     0,
       0,   380,     0,     0,    89,    90,     0,    91,     0,     0,
     195,     0,   399,   400,   401,     0,    82,    83,    84,    85,
       0,     0,     0,    52,     0,   403,   404,    86,    87,   405,
     406,   407,   408,     0,   230,     0,    89,    90,     0,    91,
      75,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    75,     0,     0,     0,     0,    82,    83,
      84,    85,     0,     0,     0,     0,     0,     0,     0,    86,
      87,    82,    83,    84,    85,     0,   196,     0,    89,    90,
       0,    91,    86,   198,     0,     0,     0,     0,     0,   199,
       0,    89,    90,     0,    91,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,     0,     0,     0,
       0,     0,     0,    52,     0,     0,     0,     0,     0,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    75,     0,     0,     0,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,    82,    83,    84,    85,     0,     0,    52,     0,   399,
     400,   401,    86,    87,     0,     0,     0,     0,     0,   273,
       0,    89,    90,   404,    91,     0,   405,   406,   407,   408,
       0,    52,     0,     0,     0,     0,     0,    75,     0,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,     0,     0,    82,    83,    84,    85,    52,
       0,    75,     0,     0,     0,     0,    86,    87,     0,     0,
       0,     0,     0,   438,     0,    89,    90,     0,    91,    82,
      83,    84,    85,     0,     0,     0,     0,     0,     0,    75,
      86,    87,     0,     0,     0,     0,     0,   455,     0,    89,
      90,     0,    91,     0,     0,     0,     0,    82,    83,    84,
      85,    52,     0,     0,     0,     0,     0,     0,    86,    87,
       0,     0,     0,     0,     0,   485,     0,    89,    90,     0,
      91,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    75,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,     0,     0,     0,     0,    82,
      83,    84,    85,     0,     0,     0,     0,     0,     0,     0,
      86,    87,     0,     0,     0,     0,     0,   633,     0,    89,
      90,     0,    91,     0,   177,   178,   179,   180,   181,     6,
       7,     8,     9,    10,    11,   182,   177,   178,   179,   180,
     181,     6,     7,     8,     9,    10,    11,   182,     0,     0,
       0,     0,     0,     0,    52,     0,   364,   365,   366,   367,
     368,   369,   370,     0,     0,     0,     0,     0,   371,   372,
     373,   374,   375,   376,   377,   378,   379,     0,     0,     0,
     380,     0,     0,     0,    75,   554,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   183,     0,     0,     0,
       0,     0,    82,    83,    84,    85,     0,     0,   183,     0,
       0,     0,     0,    86,    87,   364,   365,   366,   367,   368,
     369,   370,    89,    90,     0,    91,   184,   371,   372,   373,
     374,   375,   376,   377,   378,   379,     0,     0,   184,   380,
     382,     0,     0,     0,   185,   186,   187,   188,     0,     0,
       0,     0,     0,     0,     0,   189,   185,   186,   187,   188,
       0,     0,   190,     0,   191,   192,     0,   189,     0,     0,
       0,     0,     0,     0,     0,     0,   191,   192,   292,     0,
     293,   294,   295,     0,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,    28,    29,     0,   306,   307,   308,
     309,   310,     0,   311,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   312,   313,   314,   315,     0,     0,   316,
     317,   318,   319,   320,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   321,   322,     0,     0,     0,   323,   324,
     325,   326,     0,     0,   327,   328,   329,   330,   331,   332,
     333,   334,   335,     0,     0,   336,   337,   338,   339,   340,
       0,   341,     0,     0,   342,     0,   343,   344,     0,   345,
     346,   347,   348,   349,   364,   365,   366,   367,   368,   369,
     370,     0,     0,     0,     0,     0,   371,   372,   373,   374,
     375,   376,   377,   378,   379,     0,     0,     0,   380,   440,
       0,   350,   364,   365,   366,   367,   368,   369,   370,     0,
       0,     0,     0,     0,   371,   372,   373,   374,   375,   376,
     377,   378,   379,     0,     0,     0,   380,   454,   364,   365,
     366,   367,   368,   369,   370,     0,     0,     0,     0,     0,
     371,   372,   373,   374,   375,   376,   377,   378,   379,     0,
       0,     0,   380,   464,   364,   365,   366,   367,   368,   369,
     370,     0,     0,     0,     0,     0,   371,   372,   373,   374,
     375,   376,   377,   378,   379,     0,     0,     0,   380,   469,
     364,   365,   366,   367,   368,   369,   370,     0,     0,     0,
       0,     0,   371,   372,   373,   374,   375,   376,   377,   378,
     379,     0,     0,     0,   380,   470,   364,   365,   366,   367,
     368,   369,   370,     0,     0,     0,     0,     0,   371,   372,
     373,   374,   375,   376,   377,   378,   379,     0,     0,     0,
     380,   478,   364,   365,   366,   367,   368,   369,   370,     0,
       0,     0,     0,     0,   371,   372,   373,   374,   375,   376,
     377,   378,   379,     0,     0,     0,   380,   479,   364,   365,
     366,   367,   368,   369,   370,     0,     0,     0,     0,     0,
     371,   372,   373,   374,   375,   376,   377,   378,   379,     0,
       0,     0,   380,   491,   364,   365,   366,   367,   368,   369,
     370,     0,     0,     0,     0,     0,   371,   372,   373,   374,
     375,   376,   377,   378,   379,     0,     0,     0,   380,   492,
     364,   365,   366,   367,   368,   369,   370,     0,     0,     0,
       0,     0,   371,   372,   373,   374,   375,   376,   377,   378,
     379,     0,     0,     0,   380,   552,   364,   365,   366,   367,
     368,   369,   370,     0,     0,     0,     0,     0,   371,   372,
     373,   374,   375,   376,   377,   378,   379,     0,     0,     0,
     380,   572,   364,   365,   366,   367,   368,   369,   370,     0,
       0,     0,     0,     0,   371,   372,   373,   374,   375,   376,
     377,   378,   379,     0,     0,     0,   380,   611,   364,   365,
     366,   367,   368,   369,   370,     0,     0,     0,     0,     0,
     371,   372,   373,   374,   375,   376,   377,   378,   379,     0,
       0,     0,   380,   614,   364,   365,   366,   367,   368,   369,
     370,     0,     0,     0,     0,     0,   371,   372,   373,   374,
     375,   376,   377,   378,   379,     0,     0,     0,   380,   618,
     364,   365,   366,   367,   368,   369,   370,     0,     0,     0,
       0,     0,   371,   372,   373,   374,   375,   376,   377,   378,
     379,     0,     0,     0,   380,   623,   364,   365,   366,   367,
     368,   369,   370,     0,     0,     0,     0,     0,   371,   372,
     373,   374,   375,   376,   377,   378,   379,     0,     0,     0,
     380,   624,   364,   365,   366,   367,   368,   369,   370,     0,
       0,     0,     0,     0,   371,   372,   373,   374,   375,   376,
     377,   378,   379,     0,     0,     0,   380,   632,   364,   365,
     366,   367,   368,   369,   370,     0,     0,     0,     0,     0,
     371,   372,   373,   374,   375,   376,   377,   378,   379,     0,
       0,     0,   380,   635,   364,   365,   366,   367,   368,   369,
     370,     0,     0,     0,     0,     0,   371,   372,   373,   374,
     375,   376,   377,   378,   379,     0,     0,     0,   380,   636,
     364,   365,   366,   367,   368,   369,   370,     0,     0,     0,
       0,     0,   371,   372,   373,   374,   375,   376,   377,   378,
     379,     0,     0,     0,   380,   637,   364,   365,   366,   367,
     368,   369,   370,     0,     0,     0,     0,     0,   371,   372,
     373,   374,   375,   376,   377,   378,   379,     0,     0,     0,
     380,   638,   364,   365,   366,   367,   368,   369,   370,     0,
       0,     0,     0,     0,   371,   372,   373,   374,   375,   376,
     377,   378,   379,     0,     0,     0,   380,   639,   364,   365,
     366,   367,   368,   369,   370,     0,     0,     0,     0,     0,
     371,   372,   373,   374,   375,   376,   377,   378,   379,     0,
       0,     0,   380,   640,   364,   365,   366,   367,   368,   369,
     370,     0,     0,     0,     0,     0,   371,   372,   373,   374,
     375,   376,   377,   378,   379,     0,     0,     0,   380,   641,
     364,   365,   366,   367,   368,   369,   370,     0,     0,     0,
       0,     0,   371,   372,   373,   374,   375,   376,   377,   378,
     379,     0,     0,     0,   380,   642,   364,   365,   366,   367,
     368,   369,   370,     0,     0,     0,     0,     0,   371,   372,
     373,   374,   375,   376,   377,   378,   379,     0,     0,     0,
     380,   643,   364,   365,   366,   367,   368,   369,   370,     0,
       0,     0,     0,     0,   371,   372,   373,   374,   375,   376,
     377,   378,   379,     0,     0,     0,   380,   656,   364,   365,
     366,   367,   368,   369,   370,     0,     0,     0,     0,     0,
     371,   372,   373,   374,   375,   376,   377,   378,   379,     0,
       0,     0,   380,   657,   364,   365,   366,   367,   368,   369,
     370,     0,     0,     0,     0,     0,   371,   372,   373,   374,
     375,   376,   377,   378,   379,     0,     0,     0,   380,   658,
     364,   365,   366,   367,   368,   369,   370,     0,     0,     0,
       0,     0,   371,   372,   373,   374,   375,   376,   377,   378,
     379,     0,     0,     0,   380,   660,   364,   365,   366,   367,
     368,   369,   370,     0,     0,     0,     0,     0,   371,   372,
     373,   374,   375,   376,   377,   378,   379,     0,     0,     0,
     380,   661,   364,   365,   366,   367,   368,   369,   370,     0,
       0,     0,     0,     0,   371,   372,   373,   374,   375,   376,
     377,   378,   379,     0,     0,     0,   380,   662,   364,   365,
     366,   367,   368,   369,   370,     0,     0,     0,     0,     0,
     371,   372,   373,   374,   375,   376,   377,   378,   379,     0,
       0,     0,   380,   663,   364,   365,   366,   367,   368,   369,
     370,     0,     0,     0,     0,     0,   371,   372,   373,   374,
     375,   376,   377,   378,   379,     0,     0,     0,   380,   364,
     365,   366,   367,   368,   369,   370,     0,     0,     0,     0,
       0,     0,   372,   373,   374,   375,   376,   377,   378,   379,
       0,     0,     0,   380
};

static const yytype_int16 yycheck[] =
{
       0,   189,    19,   191,   192,    17,    19,   208,   209,   210,
      19,    56,    57,    75,    14,    17,    51,    19,   141,    75,
      20,    21,    75,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    18,    75,   105,    75,    51,
     141,    41,    75,    75,   105,    45,    46,   105,   108,    49,
      50,    34,   116,    53,    54,    75,   105,    75,    76,    59,
      60,   105,   115,    75,   105,   105,    66,    67,    16,    17,
     105,   135,   136,   141,    74,   141,    76,   105,    78,   141,
     141,   141,   102,   141,    15,   141,    86,    87,   141,    89,
      90,    91,    92,    76,   141,    75,    27,   115,   141,   141,
     141,   105,   141,   115,   141,    36,   141,   141,   141,   141,
      93,    94,   141,    75,    45,    46,    47,    48,    49,    50,
     141,    52,   141,   140,   141,   105,   141,   172,   141,   141,
     141,   141,   141,    75,    75,   141,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,    66,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   141,   141,   159,
     140,   141,   116,   143,   144,   141,   146,   116,   141,   100,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    75,   114,   141,   141,   140,   189,   141,   191,
     192,   140,    68,    69,    70,    71,   105,   142,   198,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
     142,   399,   400,   401,   402,   403,   404,   405,   406,   407,
     408,   409,   423,   141,   425,   426,   427,   142,   142,   229,
     142,   142,   142,   233,   234,   235,   236,   116,   117,   118,
     142,   142,    75,   243,   116,   117,   118,   247,    75,   128,
     129,   130,   252,   141,   133,   134,   135,   136,   116,   141,
      64,   133,   134,   135,   136,   142,   145,   141,   268,    73,
     270,   271,   105,   141,   141,   133,   134,   135,   136,   141,
     141,   281,   140,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   141,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   141,   141,   116,   140,   141,   141,
     143,   144,   141,   146,    75,   116,   117,   118,   141,   141,
     124,   125,   141,   141,   141,   135,   136,   128,   129,   130,
     140,   140,   133,   134,   135,   136,   116,    75,    19,   140,
      75,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,   141,   141,   135,   136,   141,   141,   141,
     140,    75,   141,   141,   364,   365,   366,   367,   368,   369,
     370,   371,   372,   373,   374,   375,   376,   377,   378,   379,
     380,   141,   141,   141,   141,   141,   386,   387,   388,   389,
     390,   391,   141,   393,   394,   395,    58,   399,   400,   401,
     402,   403,   404,   405,   406,   407,   408,   409,   141,   116,
     117,   118,   141,    75,   121,   122,   141,   141,   141,   141,
      82,    83,   141,   141,   131,   132,   133,   134,   135,   136,
     141,   141,   141,   140,   116,   116,   117,   118,   119,   120,
     121,   122,   442,   105,   444,   445,   446,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   456,   141,   141,   140,
     141,   123,   124,   125,   126,   141,   141,   141,   141,   141,
     141,   141,   134,   135,   474,   141,   476,   477,   141,   141,
     141,   143,   144,   141,   146,     0,   141,   487,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     141,   141,   141,    18,    19,    20,    21,    22,   141,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,   141,    43,    44,
     141,   141,   141,   141,   141,   141,   116,   141,    53,    54,
      55,    56,   141,   141,    59,    60,    61,    62,    63,   141,
      65,   141,    67,   133,   134,   135,   136,    72,    73,    74,
      75,   105,    77,    78,    79,    80,    81,   141,   141,    84,
      85,    86,    87,    88,    89,    90,    91,    92,   141,   141,
      95,    96,    97,    98,    99,   141,   101,   141,   103,   104,
     105,   106,   107,   141,   109,   110,   111,   112,   116,   117,
     118,   119,   120,   121,   122,   141,   141,   141,   123,   124,
     125,   126,   130,   131,   132,   133,   134,   135,   136,   134,
     135,   141,   140,    75,   141,   141,   141,   141,   143,   144,
     141,   146,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,   141,   141,   141,    18,    19,    20,
      21,    22,   141,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,   141,    43,    44,   141,   141,   141,   141,   141,   141,
     141,   141,    53,    54,    55,    56,   141,   141,    59,    60,
      61,    62,    63,   141,    65,   141,    67,   141,   141,   141,
     141,    72,    73,    74,    75,   141,    77,    78,    79,    80,
      81,    75,   141,    84,    85,    86,    87,    88,    89,    90,
      91,    92,   141,    19,    95,    96,    97,    98,    99,    92,
     101,    77,   103,   104,   105,   106,   107,    -1,   109,   110,
     111,   112,   116,   117,   118,   119,   120,   121,   122,    -1,
      -1,    -1,   123,   124,   125,   126,    -1,   131,   132,   133,
     134,   135,   136,   134,   135,    -1,   140,    -1,    -1,    -1,
     141,    -1,   143,   144,    -1,   146,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     116,   117,   118,   119,   120,   121,   122,    -1,    -1,    -1,
      -1,    -1,   128,   129,   130,   131,   132,   133,   134,   135,
     136,    -1,    -1,    -1,   140,   141,    -1,    -1,    75,    -1,
      -1,    -1,    -1,    -1,    -1,   116,   117,   118,    -1,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,   133,   134,   135,   136,    -1,    -1,   105,   140,
      75,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
      -1,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     105,    -1,    -1,   140,   141,    -1,   143,   144,    -1,   146,
      -1,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,    75,   128,   129,   130,   131,   132,   133,   134,
     135,   136,    -1,    -1,    -1,   140,   141,    -1,   143,   144,
      -1,   146,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   105,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,    -1,   128,   129,   130,   131,   132,
     133,   134,   135,   136,    -1,    -1,    -1,   140,   141,    -1,
     143,   144,    -1,   146,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,   116,
     117,   118,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   128,   129,   130,    -1,    -1,   133,   134,   135,   136,
      -1,    -1,    -1,    -1,   141,    -1,    75,    -1,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      57,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,   105,    -1,    75,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,    -1,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   105,    -1,
      -1,   140,    -1,    -1,   143,   144,    -1,   146,    -1,    -1,
      75,    -1,   116,   117,   118,    -1,   123,   124,   125,   126,
      -1,    -1,    -1,    75,    -1,   129,   130,   134,   135,   133,
     134,   135,   136,    -1,   141,    -1,   143,   144,    -1,   146,
     105,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,   105,    -1,    -1,    -1,    -1,   123,   124,
     125,   126,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   134,
     135,   123,   124,   125,   126,    -1,   141,    -1,   143,   144,
      -1,   146,   134,   135,    -1,    -1,    -1,    -1,    -1,   141,
      -1,   143,   144,    -1,   146,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    -1,    -1,    75,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   105,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,   123,   124,   125,   126,    -1,    -1,    75,    -1,   116,
     117,   118,   134,   135,    -1,    -1,    -1,    -1,    -1,   141,
      -1,   143,   144,   130,   146,    -1,   133,   134,   135,   136,
      -1,    75,    -1,    -1,    -1,    -1,    -1,   105,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,   123,   124,   125,   126,    75,
      -1,   105,    -1,    -1,    -1,    -1,   134,   135,    -1,    -1,
      -1,    -1,    -1,   141,    -1,   143,   144,    -1,   146,   123,
     124,   125,   126,    -1,    -1,    -1,    -1,    -1,    -1,   105,
     134,   135,    -1,    -1,    -1,    -1,    -1,   141,    -1,   143,
     144,    -1,   146,    -1,    -1,    -1,    -1,   123,   124,   125,
     126,    75,    -1,    -1,    -1,    -1,    -1,    -1,   134,   135,
      -1,    -1,    -1,    -1,    -1,   141,    -1,   143,   144,    -1,
     146,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   105,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,   123,
     124,   125,   126,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     134,   135,    -1,    -1,    -1,    -1,    -1,   141,    -1,   143,
     144,    -1,   146,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    -1,   116,   117,   118,   119,
     120,   121,   122,    -1,    -1,    -1,    -1,    -1,   128,   129,
     130,   131,   132,   133,   134,   135,   136,    -1,    -1,    -1,
     140,    -1,    -1,    -1,   105,   145,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    75,    -1,    -1,    -1,
      -1,    -1,   123,   124,   125,   126,    -1,    -1,    75,    -1,
      -1,    -1,    -1,   134,   135,   116,   117,   118,   119,   120,
     121,   122,   143,   144,    -1,   146,   105,   128,   129,   130,
     131,   132,   133,   134,   135,   136,    -1,    -1,   105,   140,
     141,    -1,    -1,    -1,   123,   124,   125,   126,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   134,   123,   124,   125,   126,
      -1,    -1,   141,    -1,   143,   144,    -1,   134,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   143,   144,    18,    -1,
      20,    21,    22,    -1,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    38,    39,
      40,    41,    -1,    43,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    53,    54,    55,    56,    -1,    -1,    59,
      60,    61,    62,    63,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    73,    74,    -1,    -1,    -1,    78,    79,
      80,    81,    -1,    -1,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    -1,    -1,    95,    96,    97,    98,    99,
      -1,   101,    -1,    -1,   104,    -1,   106,   107,    -1,   109,
     110,   111,   112,   113,   116,   117,   118,   119,   120,   121,
     122,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,   131,
     132,   133,   134,   135,   136,    -1,    -1,    -1,   140,   141,
      -1,   141,   116,   117,   118,   119,   120,   121,   122,    -1,
      -1,    -1,    -1,    -1,   128,   129,   130,   131,   132,   133,
     134,   135,   136,    -1,    -1,    -1,   140,   141,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,    -1,    -1,
     128,   129,   130,   131,   132,   133,   134,   135,   136,    -1,
      -1,    -1,   140,   141,   116,   117,   118,   119,   120,   121,
     122,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,   131,
     132,   133,   134,   135,   136,    -1,    -1,    -1,   140,   141,
     116,   117,   118,   119,   120,   121,   122,    -1,    -1,    -1,
      -1,    -1,   128,   129,   130,   131,   132,   133,   134,   135,
     136,    -1,    -1,    -1,   140,   141,   116,   117,   118,   119,
     120,   121,   122,    -1,    -1,    -1,    -1,    -1,   128,   129,
     130,   131,   132,   133,   134,   135,   136,    -1,    -1,    -1,
     140,   141,   116,   117,   118,   119,   120,   121,   122,    -1,
      -1,    -1,    -1,    -1,   128,   129,   130,   131,   132,   133,
     134,   135,   136,    -1,    -1,    -1,   140,   141,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,    -1,    -1,
     128,   129,   130,   131,   132,   133,   134,   135,   136,    -1,
      -1,    -1,   140,   141,   116,   117,   118,   119,   120,   121,
     122,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,   131,
     132,   133,   134,   135,   136,    -1,    -1,    -1,   140,   141,
     116,   117,   118,   119,   120,   121,   122,    -1,    -1,    -1,
      -1,    -1,   128,   129,   130,   131,   132,   133,   134,   135,
     136,    -1,    -1,    -1,   140,   141,   116,   117,   118,   119,
     120,   121,   122,    -1,    -1,    -1,    -1,    -1,   128,   129,
     130,   131,   132,   133,   134,   135,   136,    -1,    -1,    -1,
     140,   141,   116,   117,   118,   119,   120,   121,   122,    -1,
      -1,    -1,    -1,    -1,   128,   129,   130,   131,   132,   133,
     134,   135,   136,    -1,    -1,    -1,   140,   141,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,    -1,    -1,
     128,   129,   130,   131,   132,   133,   134,   135,   136,    -1,
      -1,    -1,   140,   141,   116,   117,   118,   119,   120,   121,
     122,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,   131,
     132,   133,   134,   135,   136,    -1,    -1,    -1,   140,   141,
     116,   117,   118,   119,   120,   121,   122,    -1,    -1,    -1,
      -1,    -1,   128,   129,   130,   131,   132,   133,   134,   135,
     136,    -1,    -1,    -1,   140,   141,   116,   117,   118,   119,
     120,   121,   122,    -1,    -1,    -1,    -1,    -1,   128,   129,
     130,   131,   132,   133,   134,   135,   136,    -1,    -1,    -1,
     140,   141,   116,   117,   118,   119,   120,   121,   122,    -1,
      -1,    -1,    -1,    -1,   128,   129,   130,   131,   132,   133,
     134,   135,   136,    -1,    -1,    -1,   140,   141,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,    -1,    -1,
     128,   129,   130,   131,   132,   133,   134,   135,   136,    -1,
      -1,    -1,   140,   141,   116,   117,   118,   119,   120,   121,
     122,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,   131,
     132,   133,   134,   135,   136,    -1,    -1,    -1,   140,   141,
     116,   117,   118,   119,   120,   121,   122,    -1,    -1,    -1,
      -1,    -1,   128,   129,   130,   131,   132,   133,   134,   135,
     136,    -1,    -1,    -1,   140,   141,   116,   117,   118,   119,
     120,   121,   122,    -1,    -1,    -1,    -1,    -1,   128,   129,
     130,   131,   132,   133,   134,   135,   136,    -1,    -1,    -1,
     140,   141,   116,   117,   118,   119,   120,   121,   122,    -1,
      -1,    -1,    -1,    -1,   128,   129,   130,   131,   132,   133,
     134,   135,   136,    -1,    -1,    -1,   140,   141,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,    -1,    -1,
     128,   129,   130,   131,   132,   133,   134,   135,   136,    -1,
      -1,    -1,   140,   141,   116,   117,   118,   119,   120,   121,
     122,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,   131,
     132,   133,   134,   135,   136,    -1,    -1,    -1,   140,   141,
     116,   117,   118,   119,   120,   121,   122,    -1,    -1,    -1,
      -1,    -1,   128,   129,   130,   131,   132,   133,   134,   135,
     136,    -1,    -1,    -1,   140,   141,   116,   117,   118,   119,
     120,   121,   122,    -1,    -1,    -1,    -1,    -1,   128,   129,
     130,   131,   132,   133,   134,   135,   136,    -1,    -1,    -1,
     140,   141,   116,   117,   118,   119,   120,   121,   122,    -1,
      -1,    -1,    -1,    -1,   128,   129,   130,   131,   132,   133,
     134,   135,   136,    -1,    -1,    -1,   140,   141,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,    -1,    -1,
     128,   129,   130,   131,   132,   133,   134,   135,   136,    -1,
      -1,    -1,   140,   141,   116,   117,   118,   119,   120,   121,
     122,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,   131,
     132,   133,   134,   135,   136,    -1,    -1,    -1,   140,   141,
     116,   117,   118,   119,   120,   121,   122,    -1,    -1,    -1,
      -1,    -1,   128,   129,   130,   131,   132,   133,   134,   135,
     136,    -1,    -1,    -1,   140,   141,   116,   117,   118,   119,
     120,   121,   122,    -1,    -1,    -1,    -1,    -1,   128,   129,
     130,   131,   132,   133,   134,   135,   136,    -1,    -1,    -1,
     140,   141,   116,   117,   118,   119,   120,   121,   122,    -1,
      -1,    -1,    -1,    -1,   128,   129,   130,   131,   132,   133,
     134,   135,   136,    -1,    -1,    -1,   140,   141,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,    -1,    -1,
     128,   129,   130,   131,   132,   133,   134,   135,   136,    -1,
      -1,    -1,   140,   141,   116,   117,   118,   119,   120,   121,
     122,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,   131,
     132,   133,   134,   135,   136,    -1,    -1,    -1,   140,   116,
     117,   118,   119,   120,   121,   122,    -1,    -1,    -1,    -1,
      -1,    -1,   129,   130,   131,   132,   133,   134,   135,   136,
      -1,    -1,    -1,   140
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    18,    19,    20,    21,    22,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    43,    44,    53,    54,
      55,    56,    59,    60,    61,    62,    63,    65,    67,    72,
      73,    74,    75,    77,    78,    79,    80,    81,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    95,    96,    97,
      98,    99,   101,   103,   104,   105,   106,   107,   109,   110,
     111,   112,   123,   124,   125,   126,   134,   135,   141,   143,
     144,   146,   148,   149,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   217,    19,
     141,   217,    51,   105,   141,   141,     3,     4,     5,     6,
       7,    64,    73,   124,   125,   152,   141,     3,     4,     5,
       6,     7,    14,    75,   105,   123,   124,   125,   126,   134,
     141,   143,   144,   152,   216,    75,   141,   217,   135,   141,
     217,   105,   105,   105,    15,    27,    36,    45,    46,    47,
      48,    49,    50,    52,   100,   114,   141,   141,   105,   141,
     141,   141,   141,   141,   141,   105,   141,   141,   141,    57,
     141,   217,    75,    75,    75,   217,   217,    66,    68,    69,
      70,    71,   217,    58,    82,    83,   141,   217,    75,    76,
     115,   217,   217,    16,    17,   150,   150,   150,   141,   217,
     217,   141,   141,   105,   141,   105,   141,    18,    34,    76,
      93,    94,   141,   141,   217,   217,    17,    51,    75,   115,
     141,    75,   102,    75,   141,    75,   141,    75,   141,   141,
     217,   217,    18,    20,    21,    22,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    37,    38,    39,    40,
      41,    43,    53,    54,    55,    56,    59,    60,    61,    62,
      63,    73,    74,    78,    79,    80,    81,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    95,    96,    97,    98,
      99,   101,   104,   106,   107,   109,   110,   111,   112,   113,
     141,   151,   217,    75,   141,   141,   217,   217,   217,   217,
     217,     0,   149,   141,   116,   117,   118,   119,   120,   121,
     122,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     140,   217,   141,   105,   105,   141,   142,   142,   142,   142,
     142,   142,   150,   142,   142,   142,   216,   216,   216,   116,
     117,   118,   128,   129,   130,   133,   134,   135,   136,   140,
      19,   141,    19,   141,   217,    19,   141,   141,   141,   141,
     141,   141,   141,   181,   217,   181,   181,   181,   141,   141,
     141,    75,   141,    75,   115,   141,   141,   141,   141,   217,
     141,    75,   217,   217,   134,   135,   217,   217,   141,   105,
     141,   141,   141,   141,   141,   141,   217,   141,   142,   141,
     217,   141,   141,   141,   141,   217,   141,   141,   141,   141,
     141,   141,   141,   141,   217,   141,   217,   217,   141,   141,
     141,   141,   108,   141,   141,   141,   217,    75,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   145,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   141,   141,   141,   217,   217,   217,   217,   217,
     217,   141,   217,   217,   217,   145,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,    75,    75,    19,
     141,    75,   181,   181,   181,   181,   141,    75,   141,    75,
     141,   141,   141,   217,   141,   217,   217,   217,   141,   141,
     141,   217,   105,   141,   141,   141,   217,   141,   217,   141,
     217,   141,   141,   141,   217,   141,   141,   141,   141,   141,
     141,   141,   141,   141,    19,   141,   141,   141,    75,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,    75,   141,   141
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   147,   148,   148,   149,   149,   149,   149,   149,   149,
     149,   149,   149,   149,   149,   149,   149,   149,   149,   149,
     149,   149,   149,   149,   149,   149,   149,   149,   149,   149,
     149,   149,   149,   149,   149,   149,   149,   149,   149,   149,
     149,   149,   149,   149,   149,   149,   149,   149,   149,   149,
     149,   149,   149,   149,   149,   149,   149,   149,   149,   149,
     149,   149,   149,   149,   149,   149,   149,   149,   149,   150,
     150,   151,   151,   152,   152,   152,   152,   152,   152,   153,
     153,   154,   155,   156,   156,   156,   156,   156,   156,   157,
     158,   159,   160,   161,   162,   163,   163,   164,   164,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     166,   166,   166,   166,   167,   167,   168,   168,   169,   170,
     171,   171,   172,   172,   172,   172,   173,   174,   175,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   177,
     177,   177,   177,   177,   177,   177,   177,   177,   177,   177,
     177,   177,   178,   179,   179,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   181,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   198,   198,   198,   199,   200,   201,   202,   203,   204,
     205,   205,   205,   205,   205,   206,   206,   206,   206,   206,
     206,   206,   206,   207,   207,   207,   208,   209,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   211,   212,
     213,   214,   215,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     2,     2,     3,     3,     3,     3,     4,     2,     3,
       3,     2,     3,     3,     3,     2,     3,     2,     3,     3,
       3,     2,     4,     4,     4,     5,     5,     5,     2,     3,
       3,     4,     4,     5,     2,     3,     2,     3,     2,     3,
       2,     4,     2,     3,     4,     4,     2,     3,     3,     4,
       5,     5,     5,     5,     5,     5,     5,     5,     5,     2,
       5,     7,     2,     3,     5,     3,     5,     2,     3,     5,
       4,     6,     2,     2,     3,     3,     3,     5,     5,     5,
       5,     3,     3,     3,     3,     3,     4,     3,     4,     5,
       4,     5,     0,     1,     2,     2,     2,     2,     2,     2,
       2,     3,     2,     2,     2,     3,     3,     3,     2,     2,
       4,     3,     3,     2,     4,     5,     4,     5,     4,     3,
       3,     4,     3,     3,     3,     2,     3,     4,     3,     4,
       5,     3,     5,     3,     3,     3,     3,     4,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     3,     3,
       2,     2,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_BX_TOKEN_CONTINUE: /* BX_TOKEN_CONTINUE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2243 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_IF: /* BX_TOKEN_IF  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2249 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_STEPN: /* BX_TOKEN_STEPN  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2255 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_STEP_OVER: /* BX_TOKEN_STEP_OVER  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2261 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_SET: /* BX_TOKEN_SET  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2267 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_DEBUGGER: /* BX_TOKEN_DEBUGGER  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2273 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_LIST_BREAK: /* BX_TOKEN_LIST_BREAK  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2279 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_VBREAKPOINT: /* BX_TOKEN_VBREAKPOINT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2285 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_LBREAKPOINT: /* BX_TOKEN_LBREAKPOINT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2291 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_PBREAKPOINT: /* BX_TOKEN_PBREAKPOINT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2297 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_DEL_BREAKPOINT: /* BX_TOKEN_DEL_BREAKPOINT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2303 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_ENABLE_BREAKPOINT: /* BX_TOKEN_ENABLE_BREAKPOINT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2309 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_DISABLE_BREAKPOINT: /* BX_TOKEN_DISABLE_BREAKPOINT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2315 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_INFO: /* BX_TOKEN_INFO  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2321 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_QUIT: /* BX_TOKEN_QUIT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2327 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_DETACH: /* BX_TOKEN_DETACH  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2333 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_R: /* BX_TOKEN_R  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2339 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_REGS: /* BX_TOKEN_REGS  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2345 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_CPU: /* BX_TOKEN_CPU  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2351 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_FPU: /* BX_TOKEN_FPU  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2357 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_MMX: /* BX_TOKEN_MMX  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2363 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_XMM: /* BX_TOKEN_XMM  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2369 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_YMM: /* BX_TOKEN_YMM  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2375 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_ZMM: /* BX_TOKEN_ZMM  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2381 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_AVX: /* BX_TOKEN_AVX  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2387 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_AMX: /* BX_TOKEN_AMX  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2393 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_TILE: /* BX_TOKEN_TILE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2399 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_IDT: /* BX_TOKEN_IDT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2405 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_IVT: /* BX_TOKEN_IVT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2411 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_GDT: /* BX_TOKEN_GDT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2417 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_LDT: /* BX_TOKEN_LDT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2423 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_TSS: /* BX_TOKEN_TSS  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2429 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_TAB: /* BX_TOKEN_TAB  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2435 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_ALL: /* BX_TOKEN_ALL  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2441 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_LINUX: /* BX_TOKEN_LINUX  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2447 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_DEBUG_REGS: /* BX_TOKEN_DEBUG_REGS  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2453 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_CONTROL_REGS: /* BX_TOKEN_CONTROL_REGS  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2459 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_SEGMENT_REGS: /* BX_TOKEN_SEGMENT_REGS  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2465 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_EXAMINE: /* BX_TOKEN_EXAMINE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2471 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_XFORMAT: /* BX_TOKEN_XFORMAT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2477 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_DISFORMAT: /* BX_TOKEN_DISFORMAT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2483 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_RESTORE: /* BX_TOKEN_RESTORE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2489 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_WRITEMEM: /* BX_TOKEN_WRITEMEM  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2495 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_LOADMEM: /* BX_TOKEN_LOADMEM  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2501 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_SETPMEM: /* BX_TOKEN_SETPMEM  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2507 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_DEREF: /* BX_TOKEN_DEREF  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2513 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_SYMBOLNAME: /* BX_TOKEN_SYMBOLNAME  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2519 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_QUERY: /* BX_TOKEN_QUERY  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2525 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_PENDING: /* BX_TOKEN_PENDING  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2531 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_TAKE: /* BX_TOKEN_TAKE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2537 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_DMA: /* BX_TOKEN_DMA  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2543 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_IRQ: /* BX_TOKEN_IRQ  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2549 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_SMI: /* BX_TOKEN_SMI  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2555 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_NMI: /* BX_TOKEN_NMI  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2561 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_TLB: /* BX_TOKEN_TLB  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2567 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_DISASM: /* BX_TOKEN_DISASM  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2573 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_INSTRUMENT: /* BX_TOKEN_INSTRUMENT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2579 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_STRING: /* BX_TOKEN_STRING  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2585 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_STOP: /* BX_TOKEN_STOP  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2591 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_DOIT: /* BX_TOKEN_DOIT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2597 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_CRC: /* BX_TOKEN_CRC  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2603 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_TRACE: /* BX_TOKEN_TRACE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2609 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_TRACEREG: /* BX_TOKEN_TRACEREG  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2615 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_TRACEMEM: /* BX_TOKEN_TRACEMEM  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2621 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_SWITCH_MODE: /* BX_TOKEN_SWITCH_MODE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2627 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_SIZE: /* BX_TOKEN_SIZE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2633 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_PTIME: /* BX_TOKEN_PTIME  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2639 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_TIMEBP_ABSOLUTE: /* BX_TOKEN_TIMEBP_ABSOLUTE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2645 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_TIMEBP: /* BX_TOKEN_TIMEBP  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2651 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_MODEBP: /* BX_TOKEN_MODEBP  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2657 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_VMEXITBP: /* BX_TOKEN_VMEXITBP  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2663 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_PRINT_STACK: /* BX_TOKEN_PRINT_STACK  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2669 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_BT: /* BX_TOKEN_BT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2675 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_WATCH: /* BX_TOKEN_WATCH  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2681 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_UNWATCH: /* BX_TOKEN_UNWATCH  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2687 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_READ: /* BX_TOKEN_READ  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2693 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_WRITE: /* BX_TOKEN_WRITE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2699 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_RUN_TO_LADDR: /* BX_TOKEN_RUN_TO_LADDR  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2705 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_SHOW: /* BX_TOKEN_SHOW  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2711 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_LOAD_SYMBOLS: /* BX_TOKEN_LOAD_SYMBOLS  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2717 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_SET_MAGIC_BREAK_POINTS: /* BX_TOKEN_SET_MAGIC_BREAK_POINTS  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2723 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS: /* BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2729 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_SYMBOLS: /* BX_TOKEN_SYMBOLS  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2735 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_LIST_SYMBOLS: /* BX_TOKEN_LIST_SYMBOLS  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2741 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_GLOBAL: /* BX_TOKEN_GLOBAL  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2747 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_WHERE: /* BX_TOKEN_WHERE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2753 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_PRINT_STRING: /* BX_TOKEN_PRINT_STRING  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2759 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_PAGE: /* BX_TOKEN_PAGE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2765 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_HELP: /* BX_TOKEN_HELP  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2771 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_XML: /* BX_TOKEN_XML  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2777 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_CALC: /* BX_TOKEN_CALC  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2783 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_ADDLYT: /* BX_TOKEN_ADDLYT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2789 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_REMLYT: /* BX_TOKEN_REMLYT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2795 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_LYT: /* BX_TOKEN_LYT  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2801 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_SOURCE: /* BX_TOKEN_SOURCE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2807 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_DEVICE: /* BX_TOKEN_DEVICE  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2813 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_GENERIC: /* BX_TOKEN_GENERIC  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2819 "y.tab.c"
        break;

    case YYSYMBOL_BX_TOKEN_REGISTERS: /* BX_TOKEN_REGISTERS  */
#line 22 "bx_parser.y"
            { free(((*yyvaluep).sval)); }
#line 2825 "y.tab.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 66: /* command: expression  */
#line 249 "bx_parser.y"
                 { eval_value = (yyvsp[0].uval); eval_value_valid = true; }
#line 3095 "y.tab.c"
    break;

  case 68: /* command: '\n'  */
#line 252 "bx_parser.y"
      {
      }
#line 3102 "y.tab.c"
    break;

  case 70: /* BX_TOKEN_TOGGLE_ON_OFF: BX_TOKEN_OFF  */
#line 259 "bx_parser.y"
    { (yyval.bval)=(yyvsp[0].bval); }
#line 3108 "y.tab.c"
    break;

  case 72: /* BX_TOKEN_REGISTERS: BX_TOKEN_REGS  */
#line 265 "bx_parser.y"
    { (yyval.sval)=(yyvsp[0].sval); }
#line 3114 "y.tab.c"
    break;

  case 78: /* BX_TOKEN_SEGREG: BX_TOKEN_GS  */
#line 275 "bx_parser.y"
    { (yyval.uval)=(yyvsp[0].uval); }
#line 3120 "y.tab.c"
    break;

  case 79: /* timebp_command: BX_TOKEN_TIMEBP expression '\n'  */
#line 280 "bx_parser.y"
      {
          bx_dbg_timebp_command(0, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 3129 "y.tab.c"
    break;

  case 80: /* timebp_command: BX_TOKEN_TIMEBP_ABSOLUTE expression '\n'  */
#line 285 "bx_parser.y"
      {
          bx_dbg_timebp_command(1, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 3138 "y.tab.c"
    break;

  case 81: /* modebp_command: BX_TOKEN_MODEBP '\n'  */
#line 293 "bx_parser.y"
      {
          bx_dbg_modebp_command();
          free((yyvsp[-1].sval));
      }
#line 3147 "y.tab.c"
    break;

  case 82: /* vmexitbp_command: BX_TOKEN_VMEXITBP '\n'  */
#line 301 "bx_parser.y"
      {
          bx_dbg_vmexitbp_command();
          free((yyvsp[-1].sval));
      }
#line 3156 "y.tab.c"
    break;

  case 83: /* show_command: BX_TOKEN_SHOW BX_TOKEN_GENERIC '\n'  */
#line 309 "bx_parser.y"
      {
          bx_dbg_show_command((yyvsp[-1].sval));
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3165 "y.tab.c"
    break;

  case 84: /* show_command: BX_TOKEN_SHOW BX_TOKEN_ALL '\n'  */
#line 314 "bx_parser.y"
      {
          bx_dbg_show_command("all");
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3174 "y.tab.c"
    break;

  case 85: /* show_command: BX_TOKEN_SHOW BX_TOKEN_OFF '\n'  */
#line 319 "bx_parser.y"
      {
          bx_dbg_show_command("off");
          free((yyvsp[-2].sval));
      }
#line 3183 "y.tab.c"
    break;

  case 86: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING '\n'  */
#line 324 "bx_parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-1].sval), 0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3192 "y.tab.c"
    break;

  case 87: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING BX_TOKEN_XML '\n'  */
#line 329 "bx_parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-2].sval), 1);
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3201 "y.tab.c"
    break;

  case 88: /* show_command: BX_TOKEN_SHOW '\n'  */
#line 334 "bx_parser.y"
      {
          bx_dbg_show_command(0);
          free((yyvsp[-1].sval));
      }
#line 3210 "y.tab.c"
    break;

  case 89: /* page_command: BX_TOKEN_PAGE expression '\n'  */
#line 342 "bx_parser.y"
      {
          bx_dbg_xlate_address((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 3219 "y.tab.c"
    break;

  case 90: /* tlb_command: BX_TOKEN_TLB expression '\n'  */
#line 350 "bx_parser.y"
      {
          bx_dbg_tlb_lookup((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 3228 "y.tab.c"
    break;

  case 91: /* ptime_command: BX_TOKEN_PTIME '\n'  */
#line 358 "bx_parser.y"
      {
          bx_dbg_ptime_command();
          free((yyvsp[-1].sval));
      }
#line 3237 "y.tab.c"
    break;

  case 92: /* trace_command: BX_TOKEN_TRACE BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 366 "bx_parser.y"
      {
          bx_dbg_trace_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 3246 "y.tab.c"
    break;

  case 93: /* trace_reg_command: BX_TOKEN_TRACEREG BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 374 "bx_parser.y"
      {
          bx_dbg_trace_reg_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 3255 "y.tab.c"
    break;

  case 94: /* trace_mem_command: BX_TOKEN_TRACEMEM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 382 "bx_parser.y"
      {
          bx_dbg_trace_mem_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 3264 "y.tab.c"
    break;

  case 95: /* print_stack_command: BX_TOKEN_PRINT_STACK '\n'  */
#line 390 "bx_parser.y"
      {
          bx_dbg_print_stack_command(16);
          free((yyvsp[-1].sval));
      }
#line 3273 "y.tab.c"
    break;

  case 96: /* print_stack_command: BX_TOKEN_PRINT_STACK BX_TOKEN_NUMERIC '\n'  */
#line 395 "bx_parser.y"
      {
          bx_dbg_print_stack_command((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 3282 "y.tab.c"
    break;

  case 97: /* backtrace_command: BX_TOKEN_BT '\n'  */
#line 403 "bx_parser.y"
      {
        bx_dbg_bt_command(16);
        free((yyvsp[-1].sval));
      }
#line 3291 "y.tab.c"
    break;

  case 98: /* backtrace_command: BX_TOKEN_BT BX_TOKEN_NUMERIC '\n'  */
#line 408 "bx_parser.y"
      {
        bx_dbg_bt_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3300 "y.tab.c"
    break;

  case 99: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_STOP '\n'  */
#line 416 "bx_parser.y"
      {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3309 "y.tab.c"
    break;

  case 100: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_CONTINUE '\n'  */
#line 421 "bx_parser.y"
      {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3318 "y.tab.c"
    break;

  case 101: /* watch_point_command: BX_TOKEN_WATCH '\n'  */
#line 426 "bx_parser.y"
      {
          bx_dbg_print_watchpoints();
          free((yyvsp[-1].sval));
      }
#line 3327 "y.tab.c"
    break;

  case 102: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression '\n'  */
#line 431 "bx_parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3336 "y.tab.c"
    break;

  case 103: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression '\n'  */
#line 436 "bx_parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3345 "y.tab.c"
    break;

  case 104: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression '\n'  */
#line 441 "bx_parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-1].uval), 1); /* BX_WRITE */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3354 "y.tab.c"
    break;

  case 105: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression expression '\n'  */
#line 446 "bx_parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3363 "y.tab.c"
    break;

  case 106: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression expression '\n'  */
#line 451 "bx_parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3372 "y.tab.c"
    break;

  case 107: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression expression '\n'  */
#line 456 "bx_parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_WRITE */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3381 "y.tab.c"
    break;

  case 108: /* watch_point_command: BX_TOKEN_UNWATCH '\n'  */
#line 461 "bx_parser.y"
      {
          bx_dbg_unwatch_all();
          free((yyvsp[-1].sval));
      }
#line 3390 "y.tab.c"
    break;

  case 109: /* watch_point_command: BX_TOKEN_UNWATCH expression '\n'  */
#line 466 "bx_parser.y"
      {
          bx_dbg_unwatch((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 3399 "y.tab.c"
    break;

  case 110: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 474 "bx_parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3408 "y.tab.c"
    break;

  case 111: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING expression '\n'  */
#line 479 "bx_parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 0, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3417 "y.tab.c"
    break;

  case 112: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING '\n'  */
#line 484 "bx_parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 1, 0);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3426 "y.tab.c"
    break;

  case 113: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING expression '\n'  */
#line 489 "bx_parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 1, (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3435 "y.tab.c"
    break;

  case 114: /* set_magic_break_points_command: BX_TOKEN_SET_MAGIC_BREAK_POINTS '\n'  */
#line 497 "bx_parser.y"
      {
        bx_dbg_set_magic_bp_mask(0);
        free((yyvsp[-1].sval));
      }
#line 3444 "y.tab.c"
    break;

  case 115: /* set_magic_break_points_command: BX_TOKEN_SET_MAGIC_BREAK_POINTS BX_TOKEN_STRING '\n'  */
#line 502 "bx_parser.y"
      {
        bx_dbg_set_magic_bp_mask(bx_dbg_get_magic_bp_mask_from_str((yyvsp[-1].sval)));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3453 "y.tab.c"
    break;

  case 116: /* clr_magic_break_points_command: BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS '\n'  */
#line 510 "bx_parser.y"
      {
        bx_dbg_set_magic_bp_mask(0);
        free((yyvsp[-1].sval));
      }
#line 3462 "y.tab.c"
    break;

  case 117: /* clr_magic_break_points_command: BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS BX_TOKEN_STRING '\n'  */
#line 515 "bx_parser.y"
      {
        bx_dbg_clr_magic_bp_mask(bx_dbg_get_magic_bp_mask_from_str((yyvsp[-1].sval)));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3471 "y.tab.c"
    break;

  case 118: /* where_command: BX_TOKEN_WHERE '\n'  */
#line 523 "bx_parser.y"
      {
        bx_dbg_where_command();
        free((yyvsp[-1].sval));
      }
#line 3480 "y.tab.c"
    break;

  case 119: /* print_string_command: BX_TOKEN_PRINT_STRING expression '\n'  */
#line 531 "bx_parser.y"
      {
        bx_dbg_print_string_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3489 "y.tab.c"
    break;

  case 120: /* continue_command: BX_TOKEN_CONTINUE '\n'  */
#line 539 "bx_parser.y"
      {
        bx_dbg_continue_command(1);
        free((yyvsp[-1].sval));
      }
#line 3498 "y.tab.c"
    break;

  case 121: /* continue_command: BX_TOKEN_CONTINUE BX_TOKEN_IF expression '\n'  */
#line 544 "bx_parser.y"
      {
        bx_dbg_continue_command((yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3507 "y.tab.c"
    break;

  case 122: /* stepN_command: BX_TOKEN_STEPN '\n'  */
#line 552 "bx_parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[-1].sval));
      }
#line 3516 "y.tab.c"
    break;

  case 123: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC '\n'  */
#line 557 "bx_parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3525 "y.tab.c"
    break;

  case 124: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_ALL BX_TOKEN_NUMERIC '\n'  */
#line 562 "bx_parser.y"
      {
        bx_dbg_stepN_command(-1, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3534 "y.tab.c"
    break;

  case 125: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'  */
#line 567 "bx_parser.y"
      {
        bx_dbg_stepN_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3543 "y.tab.c"
    break;

  case 126: /* step_over_command: BX_TOKEN_STEP_OVER '\n'  */
#line 575 "bx_parser.y"
      {
        bx_dbg_step_over_command();
        free((yyvsp[-1].sval));
      }
#line 3552 "y.tab.c"
    break;

  case 127: /* run_to_laddr_command: BX_TOKEN_RUN_TO_LADDR expression '\n'  */
#line 583 "bx_parser.y"
      {
        bx_dbg_run_to_laddr((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3561 "y.tab.c"
    break;

  case 128: /* cpu_command: BX_TOKEN_CPU BX_TOKEN_NUMERIC '\n'  */
#line 591 "bx_parser.y"
      {
        bx_dbg_set_symbol_command("$cpu", (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3570 "y.tab.c"
    break;

  case 129: /* set_command: BX_TOKEN_SET BX_TOKEN_DISASM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 598 "bx_parser.y"
      {
        bx_dbg_set_auto_disassemble((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3579 "y.tab.c"
    break;

  case 130: /* set_command: BX_TOKEN_SET BX_TOKEN_SYMBOLNAME '=' expression '\n'  */
#line 603 "bx_parser.y"
      {
        bx_dbg_set_symbol_command((yyvsp[-3].sval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3588 "y.tab.c"
    break;

  case 131: /* set_command: BX_TOKEN_SET BX_TOKEN_8BL_REG '=' expression '\n'  */
#line 608 "bx_parser.y"
      {
        bx_dbg_set_reg8l_value((yyvsp[-3].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3597 "y.tab.c"
    break;

  case 132: /* set_command: BX_TOKEN_SET BX_TOKEN_8BH_REG '=' expression '\n'  */
#line 613 "bx_parser.y"
      {
        bx_dbg_set_reg8h_value((yyvsp[-3].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3606 "y.tab.c"
    break;

  case 133: /* set_command: BX_TOKEN_SET BX_TOKEN_16B_REG '=' expression '\n'  */
#line 618 "bx_parser.y"
      {
        bx_dbg_set_reg16_value((yyvsp[-3].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3615 "y.tab.c"
    break;

  case 134: /* set_command: BX_TOKEN_SET BX_TOKEN_32B_REG '=' expression '\n'  */
#line 623 "bx_parser.y"
      {
        bx_dbg_set_reg32_value((yyvsp[-3].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3624 "y.tab.c"
    break;

  case 135: /* set_command: BX_TOKEN_SET BX_TOKEN_64B_REG '=' expression '\n'  */
#line 628 "bx_parser.y"
      {
        bx_dbg_set_reg64_value((yyvsp[-3].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3633 "y.tab.c"
    break;

  case 136: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_EIP '=' expression '\n'  */
#line 633 "bx_parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3642 "y.tab.c"
    break;

  case 137: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_RIP '=' expression '\n'  */
#line 638 "bx_parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3651 "y.tab.c"
    break;

  case 138: /* set_command: BX_TOKEN_SET BX_TOKEN_SEGREG '=' expression '\n'  */
#line 643 "bx_parser.y"
      {
        bx_dbg_load_segreg((yyvsp[-3].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3660 "y.tab.c"
    break;

  case 139: /* breakpoint_command: BX_TOKEN_VBREAKPOINT '\n'  */
#line 651 "bx_parser.y"
      {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3669 "y.tab.c"
    break;

  case 140: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression '\n'  */
#line 656 "bx_parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].uval), NULL);
        free((yyvsp[-4].sval));
      }
#line 3678 "y.tab.c"
    break;

  case 141: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 661 "bx_parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-5].uval), (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-6].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3687 "y.tab.c"
    break;

  case 142: /* breakpoint_command: BX_TOKEN_LBREAKPOINT '\n'  */
#line 666 "bx_parser.y"
      {
        bx_dbg_lbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3696 "y.tab.c"
    break;

  case 143: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression '\n'  */
#line 671 "bx_parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 3705 "y.tab.c"
    break;

  case 144: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 676 "bx_parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3714 "y.tab.c"
    break;

  case 145: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING '\n'  */
#line 681 "bx_parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-1].sval), NULL);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3723 "y.tab.c"
    break;

  case 146: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 686 "bx_parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-3].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3732 "y.tab.c"
    break;

  case 147: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '\n'  */
#line 691 "bx_parser.y"
      {
        bx_dbg_pbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3741 "y.tab.c"
    break;

  case 148: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression '\n'  */
#line 696 "bx_parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 3750 "y.tab.c"
    break;

  case 149: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 701 "bx_parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3759 "y.tab.c"
    break;

  case 150: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression '\n'  */
#line 706 "bx_parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-3].sval));
      }
#line 3768 "y.tab.c"
    break;

  case 151: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 711 "bx_parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-5].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3777 "y.tab.c"
    break;

  case 152: /* blist_command: BX_TOKEN_LIST_BREAK '\n'  */
#line 719 "bx_parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-1].sval));
      }
#line 3786 "y.tab.c"
    break;

  case 153: /* slist_command: BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 727 "bx_parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-1].sval));
      }
#line 3795 "y.tab.c"
    break;

  case 154: /* slist_command: BX_TOKEN_LIST_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 732 "bx_parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 3804 "y.tab.c"
    break;

  case 155: /* info_command: BX_TOKEN_INFO BX_TOKEN_PBREAKPOINT '\n'  */
#line 740 "bx_parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3813 "y.tab.c"
    break;

  case 156: /* info_command: BX_TOKEN_INFO BX_TOKEN_CPU '\n'  */
#line 745 "bx_parser.y"
      {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3822 "y.tab.c"
    break;

  case 157: /* info_command: BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric optional_numeric '\n'  */
#line 750 "bx_parser.y"
      {
        bx_dbg_info_idt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3831 "y.tab.c"
    break;

  case 158: /* info_command: BX_TOKEN_INFO BX_TOKEN_IVT optional_numeric optional_numeric '\n'  */
#line 755 "bx_parser.y"
      {
        bx_dbg_info_ivt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3840 "y.tab.c"
    break;

  case 159: /* info_command: BX_TOKEN_INFO BX_TOKEN_GDT optional_numeric optional_numeric '\n'  */
#line 760 "bx_parser.y"
      {
        bx_dbg_info_gdt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3849 "y.tab.c"
    break;

  case 160: /* info_command: BX_TOKEN_INFO BX_TOKEN_LDT optional_numeric optional_numeric '\n'  */
#line 765 "bx_parser.y"
      {
        bx_dbg_info_ldt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3858 "y.tab.c"
    break;

  case 161: /* info_command: BX_TOKEN_INFO BX_TOKEN_TAB '\n'  */
#line 770 "bx_parser.y"
      {
        bx_dbg_dump_table();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3867 "y.tab.c"
    break;

  case 162: /* info_command: BX_TOKEN_INFO BX_TOKEN_TSS '\n'  */
#line 775 "bx_parser.y"
      {
        bx_dbg_info_tss_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3876 "y.tab.c"
    break;

  case 163: /* info_command: BX_TOKEN_INFO BX_TOKEN_FLAGS '\n'  */
#line 780 "bx_parser.y"
      {
        bx_dbg_info_flags();
        free((yyvsp[-2].sval));
      }
#line 3885 "y.tab.c"
    break;

  case 164: /* info_command: BX_TOKEN_INFO BX_TOKEN_LINUX '\n'  */
#line 785 "bx_parser.y"
      {
        bx_dbg_info_linux_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3894 "y.tab.c"
    break;

  case 165: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS '\n'  */
#line 790 "bx_parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3903 "y.tab.c"
    break;

  case 166: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 795 "bx_parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3912 "y.tab.c"
    break;

  case 167: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE '\n'  */
#line 800 "bx_parser.y"
      {
        bx_dbg_info_device("", "");
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3921 "y.tab.c"
    break;

  case 168: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC '\n'  */
#line 805 "bx_parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3930 "y.tab.c"
    break;

  case 169: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC BX_TOKEN_STRING '\n'  */
#line 810 "bx_parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3939 "y.tab.c"
    break;

  case 170: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING '\n'  */
#line 815 "bx_parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3948 "y.tab.c"
    break;

  case 171: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 820 "bx_parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3957 "y.tab.c"
    break;

  case 172: /* optional_numeric: %empty  */
#line 827 "bx_parser.y"
               { (yyval.uval) = EMPTY_ARG; }
#line 3963 "y.tab.c"
    break;

  case 174: /* regs_command: BX_TOKEN_REGISTERS '\n'  */
#line 832 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3972 "y.tab.c"
    break;

  case 175: /* fpu_regs_command: BX_TOKEN_FPU '\n'  */
#line 840 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[-1].sval));
      }
#line 3981 "y.tab.c"
    break;

  case 176: /* mmx_regs_command: BX_TOKEN_MMX '\n'  */
#line 848 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3990 "y.tab.c"
    break;

  case 177: /* xmm_regs_command: BX_TOKEN_XMM '\n'  */
#line 856 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3999 "y.tab.c"
    break;

  case 178: /* ymm_regs_command: BX_TOKEN_YMM '\n'  */
#line 864 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 4008 "y.tab.c"
    break;

  case 179: /* zmm_regs_command: BX_TOKEN_ZMM '\n'  */
#line 872 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 4017 "y.tab.c"
    break;

  case 180: /* amx_regs_command: BX_TOKEN_AMX '\n'  */
#line 880 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_AMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 4026 "y.tab.c"
    break;

  case 181: /* print_tile_command: BX_TOKEN_TILE BX_TOKEN_NUMERIC '\n'  */
#line 888 "bx_parser.y"
      {
        bx_dbg_print_amx_tile_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 4035 "y.tab.c"
    break;

  case 182: /* segment_regs_command: BX_TOKEN_SEGMENT_REGS '\n'  */
#line 896 "bx_parser.y"
      {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[-1].sval));
      }
#line 4044 "y.tab.c"
    break;

  case 183: /* control_regs_command: BX_TOKEN_CONTROL_REGS '\n'  */
#line 904 "bx_parser.y"
      {
        bx_dbg_info_control_regs_command();
        free((yyvsp[-1].sval));
      }
#line 4053 "y.tab.c"
    break;

  case 184: /* debug_regs_command: BX_TOKEN_DEBUG_REGS '\n'  */
#line 912 "bx_parser.y"
      {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[-1].sval));
      }
#line 4062 "y.tab.c"
    break;

  case 185: /* delete_command: BX_TOKEN_DEL_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 920 "bx_parser.y"
      {
        bx_dbg_del_breakpoint_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 4071 "y.tab.c"
    break;

  case 186: /* bpe_command: BX_TOKEN_ENABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 928 "bx_parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 4080 "y.tab.c"
    break;

  case 187: /* bpd_command: BX_TOKEN_DISABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 935 "bx_parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 0);
        free((yyvsp[-2].sval));
      }
#line 4089 "y.tab.c"
    break;

  case 188: /* quit_command: BX_TOKEN_QUIT '\n'  */
#line 943 "bx_parser.y"
      {
        bx_dbg_quit_command();
        free((yyvsp[-1].sval));
      }
#line 4098 "y.tab.c"
    break;

  case 189: /* detach_command: BX_TOKEN_DETACH '\n'  */
#line 951 "bx_parser.y"
      {
        bx_dbg_detach_command();
        free((yyvsp[-1].sval));
      }
#line 4107 "y.tab.c"
    break;

  case 190: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT expression '\n'  */
#line 959 "bx_parser.y"
      {
        bx_dbg_examine_command((yyvsp[-3].sval), (yyvsp[-2].sval),1, (yyvsp[-1].uval), 1);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 4116 "y.tab.c"
    break;

  case 191: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT '\n'  */
#line 964 "bx_parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), (yyvsp[-1].sval),1, 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 4125 "y.tab.c"
    break;

  case 192: /* examine_command: BX_TOKEN_EXAMINE expression '\n'  */
#line 969 "bx_parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), NULL,0, (yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 4134 "y.tab.c"
    break;

  case 193: /* examine_command: BX_TOKEN_EXAMINE '\n'  */
#line 974 "bx_parser.y"
      {
        bx_dbg_examine_command((yyvsp[-1].sval), NULL,0, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 4143 "y.tab.c"
    break;

  case 194: /* restore_command: BX_TOKEN_RESTORE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 982 "bx_parser.y"
      {
        bx_dbg_restore_command((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 4152 "y.tab.c"
    break;

  case 195: /* writemem_command: BX_TOKEN_WRITEMEM BX_TOKEN_STRING expression expression '\n'  */
#line 990 "bx_parser.y"
      {
        bx_dbg_writemem_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 4161 "y.tab.c"
    break;

  case 196: /* loadmem_command: BX_TOKEN_LOADMEM BX_TOKEN_STRING expression '\n'  */
#line 998 "bx_parser.y"
      {
        bx_dbg_loadmem_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 4170 "y.tab.c"
    break;

  case 197: /* setpmem_command: BX_TOKEN_SETPMEM expression expression expression '\n'  */
#line 1006 "bx_parser.y"
      {
        bx_dbg_setpmem_command((yyvsp[-3].uval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 4179 "y.tab.c"
    break;

  case 198: /* deref_command: BX_TOKEN_DEREF expression expression '\n'  */
#line 1014 "bx_parser.y"
      {
        bx_dbg_deref_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 4188 "y.tab.c"
    break;

  case 199: /* query_command: BX_TOKEN_QUERY BX_TOKEN_PENDING '\n'  */
#line 1022 "bx_parser.y"
      {
        bx_dbg_query_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 4197 "y.tab.c"
    break;

  case 200: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA '\n'  */
#line 1030 "bx_parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 4206 "y.tab.c"
    break;

  case 201: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA BX_TOKEN_NUMERIC '\n'  */
#line 1035 "bx_parser.y"
      {
        bx_dbg_take_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 4215 "y.tab.c"
    break;

  case 202: /* take_command: BX_TOKEN_TAKE BX_TOKEN_IRQ '\n'  */
#line 1040 "bx_parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 4224 "y.tab.c"
    break;

  case 203: /* take_command: BX_TOKEN_TAKE BX_TOKEN_SMI '\n'  */
#line 1045 "bx_parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 4233 "y.tab.c"
    break;

  case 204: /* take_command: BX_TOKEN_TAKE BX_TOKEN_NMI '\n'  */
#line 1050 "bx_parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 4242 "y.tab.c"
    break;

  case 205: /* disassemble_command: BX_TOKEN_DISASM '\n'  */
#line 1058 "bx_parser.y"
      {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[-1].sval));
      }
#line 4251 "y.tab.c"
    break;

  case 206: /* disassemble_command: BX_TOKEN_DISASM expression '\n'  */
#line 1063 "bx_parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 4260 "y.tab.c"
    break;

  case 207: /* disassemble_command: BX_TOKEN_DISASM expression expression '\n'  */
#line 1068 "bx_parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 4269 "y.tab.c"
    break;

  case 208: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT '\n'  */
#line 1073 "bx_parser.y"
      {
        bx_dbg_disassemble_current((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 4278 "y.tab.c"
    break;

  case 209: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression '\n'  */
#line 1078 "bx_parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-2].sval), (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 4287 "y.tab.c"
    break;

  case 210: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression expression '\n'  */
#line 1083 "bx_parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 4296 "y.tab.c"
    break;

  case 211: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SWITCH_MODE '\n'  */
#line 1088 "bx_parser.y"
      {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 4305 "y.tab.c"
    break;

  case 212: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SIZE '=' BX_TOKEN_NUMERIC '\n'  */
#line 1093 "bx_parser.y"
      {
        bx_dbg_set_disassemble_size((yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 4314 "y.tab.c"
    break;

  case 213: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STOP '\n'  */
#line 1101 "bx_parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 4323 "y.tab.c"
    break;

  case 214: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STRING '\n'  */
#line 1107 "bx_parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 4332 "y.tab.c"
    break;

  case 215: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_GENERIC '\n'  */
#line 1112 "bx_parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 4341 "y.tab.c"
    break;

  case 216: /* doit_command: BX_TOKEN_DOIT expression '\n'  */
#line 1120 "bx_parser.y"
      {
        bx_dbg_doit_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 4350 "y.tab.c"
    break;

  case 217: /* crc_command: BX_TOKEN_CRC expression expression '\n'  */
#line 1128 "bx_parser.y"
      {
        bx_dbg_crc_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 4359 "y.tab.c"
    break;

  case 218: /* help_command: BX_TOKEN_HELP BX_TOKEN_QUIT '\n'  */
#line 1136 "bx_parser.y"
       {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4368 "y.tab.c"
    break;

  case 219: /* help_command: BX_TOKEN_HELP BX_TOKEN_DETACH '\n'  */
#line 1141 "bx_parser.y"
       {
         dbg_printf("detach - leave debugger and continue emulator execution without it,\n");
         dbg_printf("         the debugger is activated again by magic or time breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4378 "y.tab.c"
    break;

  case 220: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTINUE '\n'  */
#line 1147 "bx_parser.y"
       {
         dbg_printf("c|cont|continue - continue executing\n");
         dbg_printf("c|cont|continue if \"expression\" - continue executing only if expression is true\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4388 "y.tab.c"
    break;

  case 221: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEPN '\n'  */
#line 1153 "bx_parser.y"
       {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4399 "y.tab.c"
    break;

  case 222: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEP_OVER '\n'  */
#line 1160 "bx_parser.y"
       {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4408 "y.tab.c"
    break;

  case 223: /* help_command: BX_TOKEN_HELP BX_TOKEN_RUN_TO_LADDR '\n'  */
#line 1165 "bx_parser.y"
       {
         dbg_printf("rla <addr> - continue until reaching the linear address\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4417 "y.tab.c"
    break;

  case 224: /* help_command: BX_TOKEN_HELP BX_TOKEN_VBREAKPOINT '\n'  */
#line 1170 "bx_parser.y"
       {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         dbg_printf("vb|vbreak <seg:offset> if \"expression\" - set a conditional virtual address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4427 "y.tab.c"
    break;

  case 225: /* help_command: BX_TOKEN_HELP BX_TOKEN_LBREAKPOINT '\n'  */
#line 1176 "bx_parser.y"
       {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         dbg_printf("lb|lbreak <addr> if \"expression\" - set a conditional linear address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4437 "y.tab.c"
    break;

  case 226: /* help_command: BX_TOKEN_HELP BX_TOKEN_PBREAKPOINT '\n'  */
#line 1182 "bx_parser.y"
       {
         dbg_printf("b|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         dbg_printf("b|pb|break|pbreak <addr> if \"expression\" - set a conditional physical address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4447 "y.tab.c"
    break;

  case 227: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEL_BREAKPOINT '\n'  */
#line 1188 "bx_parser.y"
       {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4456 "y.tab.c"
    break;

  case 228: /* help_command: BX_TOKEN_HELP BX_TOKEN_ENABLE_BREAKPOINT '\n'  */
#line 1193 "bx_parser.y"
       {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4465 "y.tab.c"
    break;

  case 229: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISABLE_BREAKPOINT '\n'  */
#line 1198 "bx_parser.y"
       {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4474 "y.tab.c"
    break;

  case 230: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_BREAK '\n'  */
#line 1203 "bx_parser.y"
       {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4483 "y.tab.c"
    break;

  case 231: /* help_command: BX_TOKEN_HELP BX_TOKEN_MODEBP '\n'  */
#line 1208 "bx_parser.y"
       {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4492 "y.tab.c"
    break;

  case 232: /* help_command: BX_TOKEN_HELP BX_TOKEN_VMEXITBP '\n'  */
#line 1213 "bx_parser.y"
       {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4501 "y.tab.c"
    break;

  case 233: /* help_command: BX_TOKEN_HELP BX_TOKEN_CRC '\n'  */
#line 1218 "bx_parser.y"
       {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4510 "y.tab.c"
    break;

  case 234: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACE '\n'  */
#line 1223 "bx_parser.y"
       {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4520 "y.tab.c"
    break;

  case 235: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEREG '\n'  */
#line 1229 "bx_parser.y"
       {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4530 "y.tab.c"
    break;

  case 236: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEMEM '\n'  */
#line 1235 "bx_parser.y"
       {
         dbg_printf("trace-mem on  - print all memory accesses occurred during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4540 "y.tab.c"
    break;

  case 237: /* help_command: BX_TOKEN_HELP BX_TOKEN_RESTORE '\n'  */
#line 1241 "bx_parser.y"
       {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4552 "y.tab.c"
    break;

  case 238: /* help_command: BX_TOKEN_HELP BX_TOKEN_PTIME '\n'  */
#line 1249 "bx_parser.y"
       {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4561 "y.tab.c"
    break;

  case 239: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP '\n'  */
#line 1254 "bx_parser.y"
       {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4570 "y.tab.c"
    break;

  case 240: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP_ABSOLUTE '\n'  */
#line 1259 "bx_parser.y"
       {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4579 "y.tab.c"
    break;

  case 241: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STACK '\n'  */
#line 1264 "bx_parser.y"
       {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4588 "y.tab.c"
    break;

  case 242: /* help_command: BX_TOKEN_HELP BX_TOKEN_BT '\n'  */
#line 1269 "bx_parser.y"
       {
         dbg_printf("bt [num_entries] - prints backtrace\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4597 "y.tab.c"
    break;

  case 243: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOAD_SYMBOLS '\n'  */
#line 1274 "bx_parser.y"
       {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4606 "y.tab.c"
    break;

  case 244: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET_MAGIC_BREAK_POINTS '\n'  */
#line 1279 "bx_parser.y"
       {
         dbg_printf("setmagicbps \"cx dx bx sp bp si di\" - set new magic breakpoints. You can specify multiple at once. Using the setmagicbps command without any arguments will disable all of them\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4615 "y.tab.c"
    break;

  case 245: /* help_command: BX_TOKEN_HELP BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS '\n'  */
#line 1284 "bx_parser.y"
       {
         dbg_printf("clrmagicbps \"cx dx bx sp bp si di\" - clear magic breakpoints. You can specify multiple at once. Using the clrmagicbps command without any arguments will disable all of them\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4624 "y.tab.c"
    break;

  case 246: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 1289 "bx_parser.y"
       {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4633 "y.tab.c"
    break;

  case 247: /* help_command: BX_TOKEN_HELP BX_TOKEN_REGISTERS '\n'  */
#line 1294 "bx_parser.y"
       {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4642 "y.tab.c"
    break;

  case 248: /* help_command: BX_TOKEN_HELP BX_TOKEN_FPU '\n'  */
#line 1299 "bx_parser.y"
       {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4651 "y.tab.c"
    break;

  case 249: /* help_command: BX_TOKEN_HELP BX_TOKEN_MMX '\n'  */
#line 1304 "bx_parser.y"
       {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4660 "y.tab.c"
    break;

  case 250: /* help_command: BX_TOKEN_HELP BX_TOKEN_XMM '\n'  */
#line 1309 "bx_parser.y"
       {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4669 "y.tab.c"
    break;

  case 251: /* help_command: BX_TOKEN_HELP BX_TOKEN_YMM '\n'  */
#line 1314 "bx_parser.y"
       {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4678 "y.tab.c"
    break;

  case 252: /* help_command: BX_TOKEN_HELP BX_TOKEN_ZMM '\n'  */
#line 1319 "bx_parser.y"
       {
         dbg_printf("zmm - print AVX-512 state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4687 "y.tab.c"
    break;

  case 253: /* help_command: BX_TOKEN_HELP BX_TOKEN_AMX '\n'  */
#line 1324 "bx_parser.y"
       {
         dbg_printf("amx - print AMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4696 "y.tab.c"
    break;

  case 254: /* help_command: BX_TOKEN_HELP BX_TOKEN_SEGMENT_REGS '\n'  */
#line 1329 "bx_parser.y"
       {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4705 "y.tab.c"
    break;

  case 255: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTROL_REGS '\n'  */
#line 1334 "bx_parser.y"
       {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4714 "y.tab.c"
    break;

  case 256: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEBUG_REGS '\n'  */
#line 1339 "bx_parser.y"
       {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4723 "y.tab.c"
    break;

  case 257: /* help_command: BX_TOKEN_HELP BX_TOKEN_WRITEMEM '\n'  */
#line 1344 "bx_parser.y"
       {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4732 "y.tab.c"
    break;

  case 258: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOADMEM '\n'  */
#line 1349 "bx_parser.y"
       {
         dbg_printf("loadmem <filename> <laddr> - load file bytes to virtual memory starting from the linear address 'laddr'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4741 "y.tab.c"
    break;

  case 259: /* help_command: BX_TOKEN_HELP BX_TOKEN_SETPMEM '\n'  */
#line 1354 "bx_parser.y"
       {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4750 "y.tab.c"
    break;

  case 260: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEREF '\n'  */
#line 1359 "bx_parser.y"
       {
         dbg_printf("deref <addr> <deep> - pointer dereference. For example: get value of [[[rax]]] or ***rax: deref rax 3\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4759 "y.tab.c"
    break;

  case 261: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISASM '\n'  */
#line 1364 "bx_parser.y"
       {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4772 "y.tab.c"
    break;

  case 262: /* help_command: BX_TOKEN_HELP BX_TOKEN_WATCH '\n'  */
#line 1373 "bx_parser.y"
       {
         dbg_printf("watch - print current watch point status\n");
         dbg_printf("watch stop - stop simulation when a watchpoint is encountred\n");
         dbg_printf("watch continue - do not stop the simulation when watch point is encountred\n");
         dbg_printf("watch r|read addr - insert a read watch point at physical address addr\n");
         dbg_printf("watch w|write addr - insert a write watch point at physical address addr\n");
         dbg_printf("watch r|read addr <len> - insert a read watch point at physical address addr with range <len>\n");
         dbg_printf("watch w|write addr <len> - insert a write watch point at physical address addr with range <len>\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4787 "y.tab.c"
    break;

  case 263: /* help_command: BX_TOKEN_HELP BX_TOKEN_UNWATCH '\n'  */
#line 1384 "bx_parser.y"
       {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4797 "y.tab.c"
    break;

  case 264: /* help_command: BX_TOKEN_HELP BX_TOKEN_EXAMINE '\n'  */
#line 1390 "bx_parser.y"
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
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4814 "y.tab.c"
    break;

  case 265: /* help_command: BX_TOKEN_HELP BX_TOKEN_INSTRUMENT '\n'  */
#line 1403 "bx_parser.y"
       {
         dbg_printf("instrument <command|\"string command\"> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command|\"string command\">\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4823 "y.tab.c"
    break;

  case 266: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET '\n'  */
#line 1408 "bx_parser.y"
       {
         dbg_printf("set <regname> = <expr> - set register value to expression\n");
         dbg_printf("set eflags = <expr> - set eflags value to expression, not all flags can be modified\n");
         dbg_printf("set $cpu = <N> or just cpu <N> - move debugger control to cpu <N> in SMP simulation\n");
         dbg_printf("set $auto_disassemble = 1 -> cause debugger to disassemble current instruction\n");
         dbg_printf("       every time execution stops\n");
         dbg_printf("set u|disasm on  - same as 'set $auto_disassemble = 1'\n");
         dbg_printf("set u|disasm off - same as 'set $auto_disassemble = 0'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4838 "y.tab.c"
    break;

  case 267: /* help_command: BX_TOKEN_HELP BX_TOKEN_PAGE '\n'  */
#line 1419 "bx_parser.y"
       {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4847 "y.tab.c"
    break;

  case 268: /* help_command: BX_TOKEN_HELP BX_TOKEN_INFO '\n'  */
#line 1424 "bx_parser.y"
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
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4867 "y.tab.c"
    break;

  case 269: /* help_command: BX_TOKEN_HELP BX_TOKEN_SHOW '\n'  */
#line 1440 "bx_parser.y"
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
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4887 "y.tab.c"
    break;

  case 270: /* help_command: BX_TOKEN_HELP BX_TOKEN_CALC '\n'  */
#line 1456 "bx_parser.y"
       {
         dbg_printf("calc|? <expr> - calculate a expression and display the result.\n");
         dbg_printf("    'expr' can reference any general-purpose, opmask and segment\n");
         dbg_printf("    registers, use any arithmetic and logic operations, and\n");
         dbg_printf("    also the special ':' operator which computes the linear\n");
         dbg_printf("    address of a segment:offset (in real and v86 mode) or of\n");
         dbg_printf("    a selector:offset (in protected mode) pair. Use $ operator\n");
         dbg_printf("    for dereference, for example get value of [[[rax]]] or\n");
         dbg_printf("    ***rax: rax$3\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4903 "y.tab.c"
    break;

  case 271: /* help_command: BX_TOKEN_HELP BX_TOKEN_ADDLYT '\n'  */
#line 1468 "bx_parser.y"
       {
         dbg_printf("addlyt <file> - cause debugger to execute a script file every time execution stops.\n");
         dbg_printf("    Example of use: 1. Create a script file (script.txt) with the following content:\n");
         dbg_printf("             regs\n");
         dbg_printf("             print-stack 7\n");
         dbg_printf("             u /10\n");
         dbg_printf("             <EMPTY NEW LINE>\n");
         dbg_printf("    2. Execute: addlyt \"script.txt\"\n");
         dbg_printf("    Then, when you execute a step/DebugBreak... you will see: registers, stack and disasm.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4919 "y.tab.c"
    break;

  case 272: /* help_command: BX_TOKEN_HELP BX_TOKEN_REMLYT '\n'  */
#line 1480 "bx_parser.y"
       {
         dbg_printf("remlyt - stops debugger to execute the script file added previously with addlyt command.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4928 "y.tab.c"
    break;

  case 273: /* help_command: BX_TOKEN_HELP BX_TOKEN_LYT '\n'  */
#line 1485 "bx_parser.y"
       {
         dbg_printf("lyt - cause debugger to execute script file added previously with addlyt command.\n");
         dbg_printf("    Use it as a refresh/context.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4938 "y.tab.c"
    break;

  case 274: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STRING '\n'  */
#line 1491 "bx_parser.y"
       {
         dbg_printf("print-string <addr> - prints a null-ended string from a linear address.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4947 "y.tab.c"
    break;

  case 275: /* help_command: BX_TOKEN_HELP BX_TOKEN_SOURCE '\n'  */
#line 1496 "bx_parser.y"
       {
         dbg_printf("source <file> - cause debugger to execute a script file.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4956 "y.tab.c"
    break;

  case 276: /* help_command: BX_TOKEN_HELP BX_TOKEN_HELP '\n'  */
#line 1501 "bx_parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4965 "y.tab.c"
    break;

  case 277: /* help_command: BX_TOKEN_HELP '\n'  */
#line 1506 "bx_parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-1].sval));
       }
#line 4974 "y.tab.c"
    break;

  case 278: /* calc_command: BX_TOKEN_CALC expression '\n'  */
#line 1514 "bx_parser.y"
   {
     eval_value = (yyvsp[-1].uval);
     eval_value_valid = true;
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4985 "y.tab.c"
    break;

  case 279: /* addlyt_command: BX_TOKEN_ADDLYT BX_TOKEN_STRING '\n'  */
#line 1524 "bx_parser.y"
   {
     bx_dbg_addlyt((yyvsp[-1].sval));
     free((yyvsp[-2].sval));
     free((yyvsp[-1].sval));
   }
#line 4995 "y.tab.c"
    break;

  case 280: /* remlyt_command: BX_TOKEN_REMLYT '\n'  */
#line 1533 "bx_parser.y"
   {
     bx_dbg_remlyt();
     free((yyvsp[-1].sval));
   }
#line 5004 "y.tab.c"
    break;

  case 281: /* lyt_command: BX_TOKEN_LYT '\n'  */
#line 1541 "bx_parser.y"
   {
     bx_dbg_lyt();
     free((yyvsp[-1].sval));
   }
#line 5013 "y.tab.c"
    break;

  case 282: /* if_command: BX_TOKEN_IF expression '\n'  */
#line 1549 "bx_parser.y"
   {
     eval_value = (yyvsp[-1].uval) != 0;
     eval_value_valid = true;
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 5024 "y.tab.c"
    break;

  case 283: /* vexpression: BX_TOKEN_NUMERIC  */
#line 1559 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 5030 "y.tab.c"
    break;

  case 284: /* vexpression: BX_TOKEN_STRING  */
#line 1560 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 5036 "y.tab.c"
    break;

  case 285: /* vexpression: BX_TOKEN_8BL_REG  */
#line 1561 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 5042 "y.tab.c"
    break;

  case 286: /* vexpression: BX_TOKEN_8BH_REG  */
#line 1562 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 5048 "y.tab.c"
    break;

  case 287: /* vexpression: BX_TOKEN_16B_REG  */
#line 1563 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 5054 "y.tab.c"
    break;

  case 288: /* vexpression: BX_TOKEN_32B_REG  */
#line 1564 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 5060 "y.tab.c"
    break;

  case 289: /* vexpression: BX_TOKEN_64B_REG  */
#line 1565 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 5066 "y.tab.c"
    break;

  case 290: /* vexpression: BX_TOKEN_OPMASK_REG  */
#line 1566 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 5072 "y.tab.c"
    break;

  case 291: /* vexpression: BX_TOKEN_SEGREG  */
#line 1567 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 5078 "y.tab.c"
    break;

  case 292: /* vexpression: BX_TOKEN_REG_IP  */
#line 1568 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 5084 "y.tab.c"
    break;

  case 293: /* vexpression: BX_TOKEN_REG_EIP  */
#line 1569 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 5090 "y.tab.c"
    break;

  case 294: /* vexpression: BX_TOKEN_REG_RIP  */
#line 1570 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 5096 "y.tab.c"
    break;

  case 295: /* vexpression: BX_TOKEN_REG_SSP  */
#line 1571 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 5102 "y.tab.c"
    break;

  case 296: /* vexpression: vexpression '+' vexpression  */
#line 1572 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 5108 "y.tab.c"
    break;

  case 297: /* vexpression: vexpression '-' vexpression  */
#line 1573 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 5114 "y.tab.c"
    break;

  case 298: /* vexpression: vexpression '*' vexpression  */
#line 1574 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 5120 "y.tab.c"
    break;

  case 299: /* vexpression: vexpression '/' vexpression  */
#line 1575 "bx_parser.y"
                                     { (yyval.uval) = ((yyvsp[0].uval) != 0) ? (yyvsp[-2].uval) / (yyvsp[0].uval) : 0; }
#line 5126 "y.tab.c"
    break;

  case 300: /* vexpression: vexpression BX_TOKEN_DEREF_CHR vexpression  */
#line 1576 "bx_parser.y"
                                                { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 5132 "y.tab.c"
    break;

  case 301: /* vexpression: vexpression BX_TOKEN_RSHIFT vexpression  */
#line 1577 "bx_parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 5138 "y.tab.c"
    break;

  case 302: /* vexpression: vexpression BX_TOKEN_LSHIFT vexpression  */
#line 1578 "bx_parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 5144 "y.tab.c"
    break;

  case 303: /* vexpression: vexpression '|' vexpression  */
#line 1579 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 5150 "y.tab.c"
    break;

  case 304: /* vexpression: vexpression '^' vexpression  */
#line 1580 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 5156 "y.tab.c"
    break;

  case 305: /* vexpression: vexpression '&' vexpression  */
#line 1581 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 5162 "y.tab.c"
    break;

  case 306: /* vexpression: '!' vexpression  */
#line 1582 "bx_parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 5168 "y.tab.c"
    break;

  case 307: /* vexpression: '-' vexpression  */
#line 1583 "bx_parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 5174 "y.tab.c"
    break;

  case 308: /* vexpression: '(' vexpression ')'  */
#line 1584 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 5180 "y.tab.c"
    break;

  case 309: /* expression: BX_TOKEN_NUMERIC  */
#line 1590 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 5186 "y.tab.c"
    break;

  case 310: /* expression: BX_TOKEN_STRING  */
#line 1591 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 5192 "y.tab.c"
    break;

  case 311: /* expression: BX_TOKEN_8BL_REG  */
#line 1592 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 5198 "y.tab.c"
    break;

  case 312: /* expression: BX_TOKEN_8BH_REG  */
#line 1593 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 5204 "y.tab.c"
    break;

  case 313: /* expression: BX_TOKEN_16B_REG  */
#line 1594 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 5210 "y.tab.c"
    break;

  case 314: /* expression: BX_TOKEN_32B_REG  */
#line 1595 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 5216 "y.tab.c"
    break;

  case 315: /* expression: BX_TOKEN_64B_REG  */
#line 1596 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 5222 "y.tab.c"
    break;

  case 316: /* expression: BX_TOKEN_OPMASK_REG  */
#line 1597 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 5228 "y.tab.c"
    break;

  case 317: /* expression: BX_TOKEN_SEGREG  */
#line 1598 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 5234 "y.tab.c"
    break;

  case 318: /* expression: BX_TOKEN_REG_IP  */
#line 1599 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 5240 "y.tab.c"
    break;

  case 319: /* expression: BX_TOKEN_REG_EIP  */
#line 1600 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 5246 "y.tab.c"
    break;

  case 320: /* expression: BX_TOKEN_REG_RIP  */
#line 1601 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 5252 "y.tab.c"
    break;

  case 321: /* expression: BX_TOKEN_REG_SSP  */
#line 1602 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 5258 "y.tab.c"
    break;

  case 322: /* expression: expression ':' expression  */
#line 1603 "bx_parser.y"
                                            { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[-2].uval), (yyvsp[0].uval)); }
#line 5264 "y.tab.c"
    break;

  case 323: /* expression: expression '+' expression  */
#line 1604 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 5270 "y.tab.c"
    break;

  case 324: /* expression: expression '-' expression  */
#line 1605 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 5276 "y.tab.c"
    break;

  case 325: /* expression: expression '*' expression  */
#line 1606 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 5282 "y.tab.c"
    break;

  case 326: /* expression: expression '/' expression  */
#line 1607 "bx_parser.y"
                                     { (yyval.uval) = ((yyvsp[0].uval) != 0) ? (yyvsp[-2].uval) / (yyvsp[0].uval) : 0; }
#line 5288 "y.tab.c"
    break;

  case 327: /* expression: expression BX_TOKEN_DEREF_CHR expression  */
#line 1608 "bx_parser.y"
                                              { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 5294 "y.tab.c"
    break;

  case 328: /* expression: expression BX_TOKEN_RSHIFT expression  */
#line 1609 "bx_parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 5300 "y.tab.c"
    break;

  case 329: /* expression: expression BX_TOKEN_LSHIFT expression  */
#line 1610 "bx_parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 5306 "y.tab.c"
    break;

  case 330: /* expression: expression '|' expression  */
#line 1611 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 5312 "y.tab.c"
    break;

  case 331: /* expression: expression '^' expression  */
#line 1612 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 5318 "y.tab.c"
    break;

  case 332: /* expression: expression '&' expression  */
#line 1613 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 5324 "y.tab.c"
    break;

  case 333: /* expression: expression '>' expression  */
#line 1614 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) > (yyvsp[0].uval); }
#line 5330 "y.tab.c"
    break;

  case 334: /* expression: expression '<' expression  */
#line 1615 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) < (yyvsp[0].uval); }
#line 5336 "y.tab.c"
    break;

  case 335: /* expression: expression BX_TOKEN_EQ expression  */
#line 1616 "bx_parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) == (yyvsp[0].uval); }
#line 5342 "y.tab.c"
    break;

  case 336: /* expression: expression BX_TOKEN_NE expression  */
#line 1617 "bx_parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) != (yyvsp[0].uval); }
#line 5348 "y.tab.c"
    break;

  case 337: /* expression: expression BX_TOKEN_LE expression  */
#line 1618 "bx_parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) <= (yyvsp[0].uval); }
#line 5354 "y.tab.c"
    break;

  case 338: /* expression: expression BX_TOKEN_GE expression  */
#line 1619 "bx_parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) >= (yyvsp[0].uval); }
#line 5360 "y.tab.c"
    break;

  case 339: /* expression: '!' expression  */
#line 1620 "bx_parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 5366 "y.tab.c"
    break;

  case 340: /* expression: '-' expression  */
#line 1621 "bx_parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 5372 "y.tab.c"
    break;

  case 341: /* expression: '*' expression  */
#line 1622 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[0].uval)); }
#line 5378 "y.tab.c"
    break;

  case 342: /* expression: '@' expression  */
#line 1623 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[0].uval)); }
#line 5384 "y.tab.c"
    break;

  case 343: /* expression: '(' expression ')'  */
#line 1624 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 5390 "y.tab.c"
    break;


#line 5394 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1627 "bx_parser.y"

#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
