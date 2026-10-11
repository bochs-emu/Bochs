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

#line 87 "y.tab.c"

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
    BX_TOKEN_R = 288,              /* BX_TOKEN_R  */
    BX_TOKEN_REGS = 289,           /* BX_TOKEN_REGS  */
    BX_TOKEN_CPU = 290,            /* BX_TOKEN_CPU  */
    BX_TOKEN_FPU = 291,            /* BX_TOKEN_FPU  */
    BX_TOKEN_MMX = 292,            /* BX_TOKEN_MMX  */
    BX_TOKEN_XMM = 293,            /* BX_TOKEN_XMM  */
    BX_TOKEN_YMM = 294,            /* BX_TOKEN_YMM  */
    BX_TOKEN_ZMM = 295,            /* BX_TOKEN_ZMM  */
    BX_TOKEN_AVX = 296,            /* BX_TOKEN_AVX  */
    BX_TOKEN_AMX = 297,            /* BX_TOKEN_AMX  */
    BX_TOKEN_TILE = 298,           /* BX_TOKEN_TILE  */
    BX_TOKEN_IDT = 299,            /* BX_TOKEN_IDT  */
    BX_TOKEN_IVT = 300,            /* BX_TOKEN_IVT  */
    BX_TOKEN_GDT = 301,            /* BX_TOKEN_GDT  */
    BX_TOKEN_LDT = 302,            /* BX_TOKEN_LDT  */
    BX_TOKEN_TSS = 303,            /* BX_TOKEN_TSS  */
    BX_TOKEN_TAB = 304,            /* BX_TOKEN_TAB  */
    BX_TOKEN_ALL = 305,            /* BX_TOKEN_ALL  */
    BX_TOKEN_LINUX = 306,          /* BX_TOKEN_LINUX  */
    BX_TOKEN_DEBUG_REGS = 307,     /* BX_TOKEN_DEBUG_REGS  */
    BX_TOKEN_CONTROL_REGS = 308,   /* BX_TOKEN_CONTROL_REGS  */
    BX_TOKEN_SEGMENT_REGS = 309,   /* BX_TOKEN_SEGMENT_REGS  */
    BX_TOKEN_EXAMINE = 310,        /* BX_TOKEN_EXAMINE  */
    BX_TOKEN_XFORMAT = 311,        /* BX_TOKEN_XFORMAT  */
    BX_TOKEN_DISFORMAT = 312,      /* BX_TOKEN_DISFORMAT  */
    BX_TOKEN_RESTORE = 313,        /* BX_TOKEN_RESTORE  */
    BX_TOKEN_WRITEMEM = 314,       /* BX_TOKEN_WRITEMEM  */
    BX_TOKEN_LOADMEM = 315,        /* BX_TOKEN_LOADMEM  */
    BX_TOKEN_SETPMEM = 316,        /* BX_TOKEN_SETPMEM  */
    BX_TOKEN_DEREF = 317,          /* BX_TOKEN_DEREF  */
    BX_TOKEN_SYMBOLNAME = 318,     /* BX_TOKEN_SYMBOLNAME  */
    BX_TOKEN_QUERY = 319,          /* BX_TOKEN_QUERY  */
    BX_TOKEN_PENDING = 320,        /* BX_TOKEN_PENDING  */
    BX_TOKEN_TAKE = 321,           /* BX_TOKEN_TAKE  */
    BX_TOKEN_DMA = 322,            /* BX_TOKEN_DMA  */
    BX_TOKEN_IRQ = 323,            /* BX_TOKEN_IRQ  */
    BX_TOKEN_SMI = 324,            /* BX_TOKEN_SMI  */
    BX_TOKEN_NMI = 325,            /* BX_TOKEN_NMI  */
    BX_TOKEN_TLB = 326,            /* BX_TOKEN_TLB  */
    BX_TOKEN_DISASM = 327,         /* BX_TOKEN_DISASM  */
    BX_TOKEN_INSTRUMENT = 328,     /* BX_TOKEN_INSTRUMENT  */
    BX_TOKEN_STRING = 329,         /* BX_TOKEN_STRING  */
    BX_TOKEN_STOP = 330,           /* BX_TOKEN_STOP  */
    BX_TOKEN_DOIT = 331,           /* BX_TOKEN_DOIT  */
    BX_TOKEN_CRC = 332,            /* BX_TOKEN_CRC  */
    BX_TOKEN_TRACE = 333,          /* BX_TOKEN_TRACE  */
    BX_TOKEN_TRACEREG = 334,       /* BX_TOKEN_TRACEREG  */
    BX_TOKEN_TRACEMEM = 335,       /* BX_TOKEN_TRACEMEM  */
    BX_TOKEN_SWITCH_MODE = 336,    /* BX_TOKEN_SWITCH_MODE  */
    BX_TOKEN_SIZE = 337,           /* BX_TOKEN_SIZE  */
    BX_TOKEN_PTIME = 338,          /* BX_TOKEN_PTIME  */
    BX_TOKEN_TIMEBP_ABSOLUTE = 339, /* BX_TOKEN_TIMEBP_ABSOLUTE  */
    BX_TOKEN_TIMEBP = 340,         /* BX_TOKEN_TIMEBP  */
    BX_TOKEN_MODEBP = 341,         /* BX_TOKEN_MODEBP  */
    BX_TOKEN_VMEXITBP = 342,       /* BX_TOKEN_VMEXITBP  */
    BX_TOKEN_PRINT_STACK = 343,    /* BX_TOKEN_PRINT_STACK  */
    BX_TOKEN_BT = 344,             /* BX_TOKEN_BT  */
    BX_TOKEN_WATCH = 345,          /* BX_TOKEN_WATCH  */
    BX_TOKEN_UNWATCH = 346,        /* BX_TOKEN_UNWATCH  */
    BX_TOKEN_READ = 347,           /* BX_TOKEN_READ  */
    BX_TOKEN_WRITE = 348,          /* BX_TOKEN_WRITE  */
    BX_TOKEN_RUN_TO_LADDR = 349,   /* BX_TOKEN_RUN_TO_LADDR  */
    BX_TOKEN_SHOW = 350,           /* BX_TOKEN_SHOW  */
    BX_TOKEN_LOAD_SYMBOLS = 351,   /* BX_TOKEN_LOAD_SYMBOLS  */
    BX_TOKEN_SET_MAGIC_BREAK_POINTS = 352, /* BX_TOKEN_SET_MAGIC_BREAK_POINTS  */
    BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS = 353, /* BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS  */
    BX_TOKEN_SYMBOLS = 354,        /* BX_TOKEN_SYMBOLS  */
    BX_TOKEN_LIST_SYMBOLS = 355,   /* BX_TOKEN_LIST_SYMBOLS  */
    BX_TOKEN_GLOBAL = 356,         /* BX_TOKEN_GLOBAL  */
    BX_TOKEN_WHERE = 357,          /* BX_TOKEN_WHERE  */
    BX_TOKEN_PRINT_STRING = 358,   /* BX_TOKEN_PRINT_STRING  */
    BX_TOKEN_NUMERIC = 359,        /* BX_TOKEN_NUMERIC  */
    BX_TOKEN_PAGE = 360,           /* BX_TOKEN_PAGE  */
    BX_TOKEN_HELP = 361,           /* BX_TOKEN_HELP  */
    BX_TOKEN_XML = 362,            /* BX_TOKEN_XML  */
    BX_TOKEN_CALC = 363,           /* BX_TOKEN_CALC  */
    BX_TOKEN_ADDLYT = 364,         /* BX_TOKEN_ADDLYT  */
    BX_TOKEN_REMLYT = 365,         /* BX_TOKEN_REMLYT  */
    BX_TOKEN_LYT = 366,            /* BX_TOKEN_LYT  */
    BX_TOKEN_SOURCE = 367,         /* BX_TOKEN_SOURCE  */
    BX_TOKEN_DEVICE = 368,         /* BX_TOKEN_DEVICE  */
    BX_TOKEN_GENERIC = 369,        /* BX_TOKEN_GENERIC  */
    BX_TOKEN_DEREF_CHR = 370,      /* BX_TOKEN_DEREF_CHR  */
    BX_TOKEN_RSHIFT = 371,         /* BX_TOKEN_RSHIFT  */
    BX_TOKEN_LSHIFT = 372,         /* BX_TOKEN_LSHIFT  */
    BX_TOKEN_EQ = 373,             /* BX_TOKEN_EQ  */
    BX_TOKEN_NE = 374,             /* BX_TOKEN_NE  */
    BX_TOKEN_LE = 375,             /* BX_TOKEN_LE  */
    BX_TOKEN_GE = 376,             /* BX_TOKEN_GE  */
    BX_TOKEN_REG_IP = 377,         /* BX_TOKEN_REG_IP  */
    BX_TOKEN_REG_EIP = 378,        /* BX_TOKEN_REG_EIP  */
    BX_TOKEN_REG_RIP = 379,        /* BX_TOKEN_REG_RIP  */
    BX_TOKEN_REG_SSP = 380,        /* BX_TOKEN_REG_SSP  */
    NOT = 381,                     /* NOT  */
    NEG = 382,                     /* NEG  */
    INDIRECT = 383                 /* INDIRECT  */
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
#define BX_TOKEN_R 288
#define BX_TOKEN_REGS 289
#define BX_TOKEN_CPU 290
#define BX_TOKEN_FPU 291
#define BX_TOKEN_MMX 292
#define BX_TOKEN_XMM 293
#define BX_TOKEN_YMM 294
#define BX_TOKEN_ZMM 295
#define BX_TOKEN_AVX 296
#define BX_TOKEN_AMX 297
#define BX_TOKEN_TILE 298
#define BX_TOKEN_IDT 299
#define BX_TOKEN_IVT 300
#define BX_TOKEN_GDT 301
#define BX_TOKEN_LDT 302
#define BX_TOKEN_TSS 303
#define BX_TOKEN_TAB 304
#define BX_TOKEN_ALL 305
#define BX_TOKEN_LINUX 306
#define BX_TOKEN_DEBUG_REGS 307
#define BX_TOKEN_CONTROL_REGS 308
#define BX_TOKEN_SEGMENT_REGS 309
#define BX_TOKEN_EXAMINE 310
#define BX_TOKEN_XFORMAT 311
#define BX_TOKEN_DISFORMAT 312
#define BX_TOKEN_RESTORE 313
#define BX_TOKEN_WRITEMEM 314
#define BX_TOKEN_LOADMEM 315
#define BX_TOKEN_SETPMEM 316
#define BX_TOKEN_DEREF 317
#define BX_TOKEN_SYMBOLNAME 318
#define BX_TOKEN_QUERY 319
#define BX_TOKEN_PENDING 320
#define BX_TOKEN_TAKE 321
#define BX_TOKEN_DMA 322
#define BX_TOKEN_IRQ 323
#define BX_TOKEN_SMI 324
#define BX_TOKEN_NMI 325
#define BX_TOKEN_TLB 326
#define BX_TOKEN_DISASM 327
#define BX_TOKEN_INSTRUMENT 328
#define BX_TOKEN_STRING 329
#define BX_TOKEN_STOP 330
#define BX_TOKEN_DOIT 331
#define BX_TOKEN_CRC 332
#define BX_TOKEN_TRACE 333
#define BX_TOKEN_TRACEREG 334
#define BX_TOKEN_TRACEMEM 335
#define BX_TOKEN_SWITCH_MODE 336
#define BX_TOKEN_SIZE 337
#define BX_TOKEN_PTIME 338
#define BX_TOKEN_TIMEBP_ABSOLUTE 339
#define BX_TOKEN_TIMEBP 340
#define BX_TOKEN_MODEBP 341
#define BX_TOKEN_VMEXITBP 342
#define BX_TOKEN_PRINT_STACK 343
#define BX_TOKEN_BT 344
#define BX_TOKEN_WATCH 345
#define BX_TOKEN_UNWATCH 346
#define BX_TOKEN_READ 347
#define BX_TOKEN_WRITE 348
#define BX_TOKEN_RUN_TO_LADDR 349
#define BX_TOKEN_SHOW 350
#define BX_TOKEN_LOAD_SYMBOLS 351
#define BX_TOKEN_SET_MAGIC_BREAK_POINTS 352
#define BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS 353
#define BX_TOKEN_SYMBOLS 354
#define BX_TOKEN_LIST_SYMBOLS 355
#define BX_TOKEN_GLOBAL 356
#define BX_TOKEN_WHERE 357
#define BX_TOKEN_PRINT_STRING 358
#define BX_TOKEN_NUMERIC 359
#define BX_TOKEN_PAGE 360
#define BX_TOKEN_HELP 361
#define BX_TOKEN_XML 362
#define BX_TOKEN_CALC 363
#define BX_TOKEN_ADDLYT 364
#define BX_TOKEN_REMLYT 365
#define BX_TOKEN_LYT 366
#define BX_TOKEN_SOURCE 367
#define BX_TOKEN_DEVICE 368
#define BX_TOKEN_GENERIC 369
#define BX_TOKEN_DEREF_CHR 370
#define BX_TOKEN_RSHIFT 371
#define BX_TOKEN_LSHIFT 372
#define BX_TOKEN_EQ 373
#define BX_TOKEN_NE 374
#define BX_TOKEN_LE 375
#define BX_TOKEN_GE 376
#define BX_TOKEN_REG_IP 377
#define BX_TOKEN_REG_EIP 378
#define BX_TOKEN_REG_RIP 379
#define BX_TOKEN_REG_SSP 380
#define NOT 381
#define NEG 382
#define INDIRECT 383

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 14 "bx_parser.y"

  char    *sval;
  Bit64u   uval;
  unsigned bval;

#line 402 "y.tab.c"

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
  YYSYMBOL_BX_TOKEN_R = 33,                /* BX_TOKEN_R  */
  YYSYMBOL_BX_TOKEN_REGS = 34,             /* BX_TOKEN_REGS  */
  YYSYMBOL_BX_TOKEN_CPU = 35,              /* BX_TOKEN_CPU  */
  YYSYMBOL_BX_TOKEN_FPU = 36,              /* BX_TOKEN_FPU  */
  YYSYMBOL_BX_TOKEN_MMX = 37,              /* BX_TOKEN_MMX  */
  YYSYMBOL_BX_TOKEN_XMM = 38,              /* BX_TOKEN_XMM  */
  YYSYMBOL_BX_TOKEN_YMM = 39,              /* BX_TOKEN_YMM  */
  YYSYMBOL_BX_TOKEN_ZMM = 40,              /* BX_TOKEN_ZMM  */
  YYSYMBOL_BX_TOKEN_AVX = 41,              /* BX_TOKEN_AVX  */
  YYSYMBOL_BX_TOKEN_AMX = 42,              /* BX_TOKEN_AMX  */
  YYSYMBOL_BX_TOKEN_TILE = 43,             /* BX_TOKEN_TILE  */
  YYSYMBOL_BX_TOKEN_IDT = 44,              /* BX_TOKEN_IDT  */
  YYSYMBOL_BX_TOKEN_IVT = 45,              /* BX_TOKEN_IVT  */
  YYSYMBOL_BX_TOKEN_GDT = 46,              /* BX_TOKEN_GDT  */
  YYSYMBOL_BX_TOKEN_LDT = 47,              /* BX_TOKEN_LDT  */
  YYSYMBOL_BX_TOKEN_TSS = 48,              /* BX_TOKEN_TSS  */
  YYSYMBOL_BX_TOKEN_TAB = 49,              /* BX_TOKEN_TAB  */
  YYSYMBOL_BX_TOKEN_ALL = 50,              /* BX_TOKEN_ALL  */
  YYSYMBOL_BX_TOKEN_LINUX = 51,            /* BX_TOKEN_LINUX  */
  YYSYMBOL_BX_TOKEN_DEBUG_REGS = 52,       /* BX_TOKEN_DEBUG_REGS  */
  YYSYMBOL_BX_TOKEN_CONTROL_REGS = 53,     /* BX_TOKEN_CONTROL_REGS  */
  YYSYMBOL_BX_TOKEN_SEGMENT_REGS = 54,     /* BX_TOKEN_SEGMENT_REGS  */
  YYSYMBOL_BX_TOKEN_EXAMINE = 55,          /* BX_TOKEN_EXAMINE  */
  YYSYMBOL_BX_TOKEN_XFORMAT = 56,          /* BX_TOKEN_XFORMAT  */
  YYSYMBOL_BX_TOKEN_DISFORMAT = 57,        /* BX_TOKEN_DISFORMAT  */
  YYSYMBOL_BX_TOKEN_RESTORE = 58,          /* BX_TOKEN_RESTORE  */
  YYSYMBOL_BX_TOKEN_WRITEMEM = 59,         /* BX_TOKEN_WRITEMEM  */
  YYSYMBOL_BX_TOKEN_LOADMEM = 60,          /* BX_TOKEN_LOADMEM  */
  YYSYMBOL_BX_TOKEN_SETPMEM = 61,          /* BX_TOKEN_SETPMEM  */
  YYSYMBOL_BX_TOKEN_DEREF = 62,            /* BX_TOKEN_DEREF  */
  YYSYMBOL_BX_TOKEN_SYMBOLNAME = 63,       /* BX_TOKEN_SYMBOLNAME  */
  YYSYMBOL_BX_TOKEN_QUERY = 64,            /* BX_TOKEN_QUERY  */
  YYSYMBOL_BX_TOKEN_PENDING = 65,          /* BX_TOKEN_PENDING  */
  YYSYMBOL_BX_TOKEN_TAKE = 66,             /* BX_TOKEN_TAKE  */
  YYSYMBOL_BX_TOKEN_DMA = 67,              /* BX_TOKEN_DMA  */
  YYSYMBOL_BX_TOKEN_IRQ = 68,              /* BX_TOKEN_IRQ  */
  YYSYMBOL_BX_TOKEN_SMI = 69,              /* BX_TOKEN_SMI  */
  YYSYMBOL_BX_TOKEN_NMI = 70,              /* BX_TOKEN_NMI  */
  YYSYMBOL_BX_TOKEN_TLB = 71,              /* BX_TOKEN_TLB  */
  YYSYMBOL_BX_TOKEN_DISASM = 72,           /* BX_TOKEN_DISASM  */
  YYSYMBOL_BX_TOKEN_INSTRUMENT = 73,       /* BX_TOKEN_INSTRUMENT  */
  YYSYMBOL_BX_TOKEN_STRING = 74,           /* BX_TOKEN_STRING  */
  YYSYMBOL_BX_TOKEN_STOP = 75,             /* BX_TOKEN_STOP  */
  YYSYMBOL_BX_TOKEN_DOIT = 76,             /* BX_TOKEN_DOIT  */
  YYSYMBOL_BX_TOKEN_CRC = 77,              /* BX_TOKEN_CRC  */
  YYSYMBOL_BX_TOKEN_TRACE = 78,            /* BX_TOKEN_TRACE  */
  YYSYMBOL_BX_TOKEN_TRACEREG = 79,         /* BX_TOKEN_TRACEREG  */
  YYSYMBOL_BX_TOKEN_TRACEMEM = 80,         /* BX_TOKEN_TRACEMEM  */
  YYSYMBOL_BX_TOKEN_SWITCH_MODE = 81,      /* BX_TOKEN_SWITCH_MODE  */
  YYSYMBOL_BX_TOKEN_SIZE = 82,             /* BX_TOKEN_SIZE  */
  YYSYMBOL_BX_TOKEN_PTIME = 83,            /* BX_TOKEN_PTIME  */
  YYSYMBOL_BX_TOKEN_TIMEBP_ABSOLUTE = 84,  /* BX_TOKEN_TIMEBP_ABSOLUTE  */
  YYSYMBOL_BX_TOKEN_TIMEBP = 85,           /* BX_TOKEN_TIMEBP  */
  YYSYMBOL_BX_TOKEN_MODEBP = 86,           /* BX_TOKEN_MODEBP  */
  YYSYMBOL_BX_TOKEN_VMEXITBP = 87,         /* BX_TOKEN_VMEXITBP  */
  YYSYMBOL_BX_TOKEN_PRINT_STACK = 88,      /* BX_TOKEN_PRINT_STACK  */
  YYSYMBOL_BX_TOKEN_BT = 89,               /* BX_TOKEN_BT  */
  YYSYMBOL_BX_TOKEN_WATCH = 90,            /* BX_TOKEN_WATCH  */
  YYSYMBOL_BX_TOKEN_UNWATCH = 91,          /* BX_TOKEN_UNWATCH  */
  YYSYMBOL_BX_TOKEN_READ = 92,             /* BX_TOKEN_READ  */
  YYSYMBOL_BX_TOKEN_WRITE = 93,            /* BX_TOKEN_WRITE  */
  YYSYMBOL_BX_TOKEN_RUN_TO_LADDR = 94,     /* BX_TOKEN_RUN_TO_LADDR  */
  YYSYMBOL_BX_TOKEN_SHOW = 95,             /* BX_TOKEN_SHOW  */
  YYSYMBOL_BX_TOKEN_LOAD_SYMBOLS = 96,     /* BX_TOKEN_LOAD_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_SET_MAGIC_BREAK_POINTS = 97, /* BX_TOKEN_SET_MAGIC_BREAK_POINTS  */
  YYSYMBOL_BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS = 98, /* BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS  */
  YYSYMBOL_BX_TOKEN_SYMBOLS = 99,          /* BX_TOKEN_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_LIST_SYMBOLS = 100,    /* BX_TOKEN_LIST_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_GLOBAL = 101,          /* BX_TOKEN_GLOBAL  */
  YYSYMBOL_BX_TOKEN_WHERE = 102,           /* BX_TOKEN_WHERE  */
  YYSYMBOL_BX_TOKEN_PRINT_STRING = 103,    /* BX_TOKEN_PRINT_STRING  */
  YYSYMBOL_BX_TOKEN_NUMERIC = 104,         /* BX_TOKEN_NUMERIC  */
  YYSYMBOL_BX_TOKEN_PAGE = 105,            /* BX_TOKEN_PAGE  */
  YYSYMBOL_BX_TOKEN_HELP = 106,            /* BX_TOKEN_HELP  */
  YYSYMBOL_BX_TOKEN_XML = 107,             /* BX_TOKEN_XML  */
  YYSYMBOL_BX_TOKEN_CALC = 108,            /* BX_TOKEN_CALC  */
  YYSYMBOL_BX_TOKEN_ADDLYT = 109,          /* BX_TOKEN_ADDLYT  */
  YYSYMBOL_BX_TOKEN_REMLYT = 110,          /* BX_TOKEN_REMLYT  */
  YYSYMBOL_BX_TOKEN_LYT = 111,             /* BX_TOKEN_LYT  */
  YYSYMBOL_BX_TOKEN_SOURCE = 112,          /* BX_TOKEN_SOURCE  */
  YYSYMBOL_BX_TOKEN_DEVICE = 113,          /* BX_TOKEN_DEVICE  */
  YYSYMBOL_BX_TOKEN_GENERIC = 114,         /* BX_TOKEN_GENERIC  */
  YYSYMBOL_BX_TOKEN_DEREF_CHR = 115,       /* BX_TOKEN_DEREF_CHR  */
  YYSYMBOL_BX_TOKEN_RSHIFT = 116,          /* BX_TOKEN_RSHIFT  */
  YYSYMBOL_BX_TOKEN_LSHIFT = 117,          /* BX_TOKEN_LSHIFT  */
  YYSYMBOL_BX_TOKEN_EQ = 118,              /* BX_TOKEN_EQ  */
  YYSYMBOL_BX_TOKEN_NE = 119,              /* BX_TOKEN_NE  */
  YYSYMBOL_BX_TOKEN_LE = 120,              /* BX_TOKEN_LE  */
  YYSYMBOL_BX_TOKEN_GE = 121,              /* BX_TOKEN_GE  */
  YYSYMBOL_BX_TOKEN_REG_IP = 122,          /* BX_TOKEN_REG_IP  */
  YYSYMBOL_BX_TOKEN_REG_EIP = 123,         /* BX_TOKEN_REG_EIP  */
  YYSYMBOL_BX_TOKEN_REG_RIP = 124,         /* BX_TOKEN_REG_RIP  */
  YYSYMBOL_BX_TOKEN_REG_SSP = 125,         /* BX_TOKEN_REG_SSP  */
  YYSYMBOL_126_ = 126,                     /* '+'  */
  YYSYMBOL_127_ = 127,                     /* '-'  */
  YYSYMBOL_128_ = 128,                     /* '|'  */
  YYSYMBOL_129_ = 129,                     /* '^'  */
  YYSYMBOL_130_ = 130,                     /* '<'  */
  YYSYMBOL_131_ = 131,                     /* '>'  */
  YYSYMBOL_132_ = 132,                     /* '*'  */
  YYSYMBOL_133_ = 133,                     /* '/'  */
  YYSYMBOL_134_ = 134,                     /* '&'  */
  YYSYMBOL_NOT = 135,                      /* NOT  */
  YYSYMBOL_NEG = 136,                      /* NEG  */
  YYSYMBOL_INDIRECT = 137,                 /* INDIRECT  */
  YYSYMBOL_138_n_ = 138,                   /* '\n'  */
  YYSYMBOL_139_ = 139,                     /* '='  */
  YYSYMBOL_140_ = 140,                     /* ':'  */
  YYSYMBOL_141_ = 141,                     /* '!'  */
  YYSYMBOL_142_ = 142,                     /* '('  */
  YYSYMBOL_143_ = 143,                     /* ')'  */
  YYSYMBOL_144_ = 144,                     /* '@'  */
  YYSYMBOL_YYACCEPT = 145,                 /* $accept  */
  YYSYMBOL_commands = 146,                 /* commands  */
  YYSYMBOL_command = 147,                  /* command  */
  YYSYMBOL_BX_TOKEN_TOGGLE_ON_OFF = 148,   /* BX_TOKEN_TOGGLE_ON_OFF  */
  YYSYMBOL_BX_TOKEN_REGISTERS = 149,       /* BX_TOKEN_REGISTERS  */
  YYSYMBOL_BX_TOKEN_SEGREG = 150,          /* BX_TOKEN_SEGREG  */
  YYSYMBOL_timebp_command = 151,           /* timebp_command  */
  YYSYMBOL_modebp_command = 152,           /* modebp_command  */
  YYSYMBOL_vmexitbp_command = 153,         /* vmexitbp_command  */
  YYSYMBOL_show_command = 154,             /* show_command  */
  YYSYMBOL_page_command = 155,             /* page_command  */
  YYSYMBOL_tlb_command = 156,              /* tlb_command  */
  YYSYMBOL_ptime_command = 157,            /* ptime_command  */
  YYSYMBOL_trace_command = 158,            /* trace_command  */
  YYSYMBOL_trace_reg_command = 159,        /* trace_reg_command  */
  YYSYMBOL_trace_mem_command = 160,        /* trace_mem_command  */
  YYSYMBOL_print_stack_command = 161,      /* print_stack_command  */
  YYSYMBOL_backtrace_command = 162,        /* backtrace_command  */
  YYSYMBOL_watch_point_command = 163,      /* watch_point_command  */
  YYSYMBOL_symbol_command = 164,           /* symbol_command  */
  YYSYMBOL_set_magic_break_points_command = 165, /* set_magic_break_points_command  */
  YYSYMBOL_clr_magic_break_points_command = 166, /* clr_magic_break_points_command  */
  YYSYMBOL_where_command = 167,            /* where_command  */
  YYSYMBOL_print_string_command = 168,     /* print_string_command  */
  YYSYMBOL_continue_command = 169,         /* continue_command  */
  YYSYMBOL_stepN_command = 170,            /* stepN_command  */
  YYSYMBOL_step_over_command = 171,        /* step_over_command  */
  YYSYMBOL_run_to_laddr_command = 172,     /* run_to_laddr_command  */
  YYSYMBOL_cpu_command = 173,              /* cpu_command  */
  YYSYMBOL_set_command = 174,              /* set_command  */
  YYSYMBOL_breakpoint_command = 175,       /* breakpoint_command  */
  YYSYMBOL_blist_command = 176,            /* blist_command  */
  YYSYMBOL_slist_command = 177,            /* slist_command  */
  YYSYMBOL_info_command = 178,             /* info_command  */
  YYSYMBOL_optional_numeric = 179,         /* optional_numeric  */
  YYSYMBOL_regs_command = 180,             /* regs_command  */
  YYSYMBOL_fpu_regs_command = 181,         /* fpu_regs_command  */
  YYSYMBOL_mmx_regs_command = 182,         /* mmx_regs_command  */
  YYSYMBOL_xmm_regs_command = 183,         /* xmm_regs_command  */
  YYSYMBOL_ymm_regs_command = 184,         /* ymm_regs_command  */
  YYSYMBOL_zmm_regs_command = 185,         /* zmm_regs_command  */
  YYSYMBOL_amx_regs_command = 186,         /* amx_regs_command  */
  YYSYMBOL_print_tile_command = 187,       /* print_tile_command  */
  YYSYMBOL_segment_regs_command = 188,     /* segment_regs_command  */
  YYSYMBOL_control_regs_command = 189,     /* control_regs_command  */
  YYSYMBOL_debug_regs_command = 190,       /* debug_regs_command  */
  YYSYMBOL_delete_command = 191,           /* delete_command  */
  YYSYMBOL_bpe_command = 192,              /* bpe_command  */
  YYSYMBOL_bpd_command = 193,              /* bpd_command  */
  YYSYMBOL_quit_command = 194,             /* quit_command  */
  YYSYMBOL_examine_command = 195,          /* examine_command  */
  YYSYMBOL_restore_command = 196,          /* restore_command  */
  YYSYMBOL_writemem_command = 197,         /* writemem_command  */
  YYSYMBOL_loadmem_command = 198,          /* loadmem_command  */
  YYSYMBOL_setpmem_command = 199,          /* setpmem_command  */
  YYSYMBOL_deref_command = 200,            /* deref_command  */
  YYSYMBOL_query_command = 201,            /* query_command  */
  YYSYMBOL_take_command = 202,             /* take_command  */
  YYSYMBOL_disassemble_command = 203,      /* disassemble_command  */
  YYSYMBOL_instrument_command = 204,       /* instrument_command  */
  YYSYMBOL_doit_command = 205,             /* doit_command  */
  YYSYMBOL_crc_command = 206,              /* crc_command  */
  YYSYMBOL_help_command = 207,             /* help_command  */
  YYSYMBOL_calc_command = 208,             /* calc_command  */
  YYSYMBOL_addlyt_command = 209,           /* addlyt_command  */
  YYSYMBOL_remlyt_command = 210,           /* remlyt_command  */
  YYSYMBOL_lyt_command = 211,              /* lyt_command  */
  YYSYMBOL_if_command = 212,               /* if_command  */
  YYSYMBOL_vexpression = 213,              /* vexpression  */
  YYSYMBOL_expression = 214                /* expression  */
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
#define YYFINAL  357
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2619

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  145
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  70
/* YYNRULES -- Number of rules.  */
#define YYNRULES  340
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  662

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   383


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
     138,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   141,     2,     2,     2,     2,   134,     2,
     142,   143,   132,   126,     2,   127,     2,   133,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   140,     2,
     130,   139,   131,     2,   144,     2,     2,     2,     2,     2,
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
     125,   135,   136,   137
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   161,   161,   162,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   235,   236,
     241,   242,   247,   248,   249,   250,   251,   252,   257,   262,
     270,   278,   286,   291,   296,   301,   306,   311,   319,   327,
     335,   343,   351,   359,   367,   372,   380,   385,   393,   398,
     403,   408,   413,   418,   423,   428,   433,   438,   443,   451,
     456,   461,   466,   474,   479,   487,   492,   500,   508,   516,
     521,   529,   534,   539,   544,   552,   560,   568,   575,   580,
     585,   589,   593,   597,   601,   605,   609,   613,   620,   625,
     630,   635,   640,   645,   650,   655,   660,   665,   670,   675,
     680,   688,   696,   701,   709,   714,   719,   724,   729,   734,
     739,   744,   749,   754,   759,   764,   769,   774,   779,   784,
     789,   797,   798,   801,   809,   817,   825,   833,   841,   849,
     857,   865,   873,   881,   889,   897,   904,   912,   920,   925,
     930,   935,   943,   951,   959,   967,   975,   983,   991,   996,
    1001,  1006,  1011,  1019,  1024,  1029,  1034,  1039,  1044,  1049,
    1054,  1062,  1068,  1073,  1081,  1089,  1097,  1102,  1108,  1115,
    1120,  1125,  1131,  1137,  1143,  1148,  1153,  1158,  1163,  1168,
    1173,  1178,  1184,  1190,  1196,  1204,  1209,  1214,  1219,  1224,
    1229,  1234,  1239,  1244,  1249,  1254,  1259,  1264,  1269,  1274,
    1279,  1284,  1289,  1294,  1299,  1304,  1309,  1314,  1319,  1329,
    1340,  1346,  1359,  1364,  1375,  1380,  1396,  1412,  1424,  1436,
    1441,  1447,  1452,  1457,  1462,  1470,  1479,  1488,  1496,  1504,
    1514,  1515,  1516,  1517,  1518,  1519,  1520,  1521,  1522,  1523,
    1524,  1525,  1526,  1527,  1528,  1529,  1530,  1531,  1532,  1533,
    1534,  1535,  1536,  1537,  1538,  1539,  1545,  1546,  1547,  1548,
    1549,  1550,  1551,  1552,  1553,  1554,  1555,  1556,  1557,  1558,
    1559,  1560,  1561,  1562,  1563,  1564,  1565,  1566,  1567,  1568,
    1569,  1570,  1571,  1572,  1573,  1574,  1575,  1576,  1577,  1578,
    1579
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
  "BX_TOKEN_R", "BX_TOKEN_REGS", "BX_TOKEN_CPU", "BX_TOKEN_FPU",
  "BX_TOKEN_MMX", "BX_TOKEN_XMM", "BX_TOKEN_YMM", "BX_TOKEN_ZMM",
  "BX_TOKEN_AVX", "BX_TOKEN_AMX", "BX_TOKEN_TILE", "BX_TOKEN_IDT",
  "BX_TOKEN_IVT", "BX_TOKEN_GDT", "BX_TOKEN_LDT", "BX_TOKEN_TSS",
  "BX_TOKEN_TAB", "BX_TOKEN_ALL", "BX_TOKEN_LINUX", "BX_TOKEN_DEBUG_REGS",
  "BX_TOKEN_CONTROL_REGS", "BX_TOKEN_SEGMENT_REGS", "BX_TOKEN_EXAMINE",
  "BX_TOKEN_XFORMAT", "BX_TOKEN_DISFORMAT", "BX_TOKEN_RESTORE",
  "BX_TOKEN_WRITEMEM", "BX_TOKEN_LOADMEM", "BX_TOKEN_SETPMEM",
  "BX_TOKEN_DEREF", "BX_TOKEN_SYMBOLNAME", "BX_TOKEN_QUERY",
  "BX_TOKEN_PENDING", "BX_TOKEN_TAKE", "BX_TOKEN_DMA", "BX_TOKEN_IRQ",
  "BX_TOKEN_SMI", "BX_TOKEN_NMI", "BX_TOKEN_TLB", "BX_TOKEN_DISASM",
  "BX_TOKEN_INSTRUMENT", "BX_TOKEN_STRING", "BX_TOKEN_STOP",
  "BX_TOKEN_DOIT", "BX_TOKEN_CRC", "BX_TOKEN_TRACE", "BX_TOKEN_TRACEREG",
  "BX_TOKEN_TRACEMEM", "BX_TOKEN_SWITCH_MODE", "BX_TOKEN_SIZE",
  "BX_TOKEN_PTIME", "BX_TOKEN_TIMEBP_ABSOLUTE", "BX_TOKEN_TIMEBP",
  "BX_TOKEN_MODEBP", "BX_TOKEN_VMEXITBP", "BX_TOKEN_PRINT_STACK",
  "BX_TOKEN_BT", "BX_TOKEN_WATCH", "BX_TOKEN_UNWATCH", "BX_TOKEN_READ",
  "BX_TOKEN_WRITE", "BX_TOKEN_RUN_TO_LADDR", "BX_TOKEN_SHOW",
  "BX_TOKEN_LOAD_SYMBOLS", "BX_TOKEN_SET_MAGIC_BREAK_POINTS",
  "BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS", "BX_TOKEN_SYMBOLS",
  "BX_TOKEN_LIST_SYMBOLS", "BX_TOKEN_GLOBAL", "BX_TOKEN_WHERE",
  "BX_TOKEN_PRINT_STRING", "BX_TOKEN_NUMERIC", "BX_TOKEN_PAGE",
  "BX_TOKEN_HELP", "BX_TOKEN_XML", "BX_TOKEN_CALC", "BX_TOKEN_ADDLYT",
  "BX_TOKEN_REMLYT", "BX_TOKEN_LYT", "BX_TOKEN_SOURCE", "BX_TOKEN_DEVICE",
  "BX_TOKEN_GENERIC", "BX_TOKEN_DEREF_CHR", "BX_TOKEN_RSHIFT",
  "BX_TOKEN_LSHIFT", "BX_TOKEN_EQ", "BX_TOKEN_NE", "BX_TOKEN_LE",
  "BX_TOKEN_GE", "BX_TOKEN_REG_IP", "BX_TOKEN_REG_EIP", "BX_TOKEN_REG_RIP",
  "BX_TOKEN_REG_SSP", "'+'", "'-'", "'|'", "'^'", "'<'", "'>'", "'*'",
  "'/'", "'&'", "NOT", "NEG", "INDIRECT", "'\\n'", "'='", "':'", "'!'",
  "'('", "')'", "'@'", "$accept", "commands", "command",
  "BX_TOKEN_TOGGLE_ON_OFF", "BX_TOKEN_REGISTERS", "BX_TOKEN_SEGREG",
  "timebp_command", "modebp_command", "vmexitbp_command", "show_command",
  "page_command", "tlb_command", "ptime_command", "trace_command",
  "trace_reg_command", "trace_mem_command", "print_stack_command",
  "backtrace_command", "watch_point_command", "symbol_command",
  "set_magic_break_points_command", "clr_magic_break_points_command",
  "where_command", "print_string_command", "continue_command",
  "stepN_command", "step_over_command", "run_to_laddr_command",
  "cpu_command", "set_command", "breakpoint_command", "blist_command",
  "slist_command", "info_command", "optional_numeric", "regs_command",
  "fpu_regs_command", "mmx_regs_command", "xmm_regs_command",
  "ymm_regs_command", "zmm_regs_command", "amx_regs_command",
  "print_tile_command", "segment_regs_command", "control_regs_command",
  "debug_regs_command", "delete_command", "bpe_command", "bpd_command",
  "quit_command", "examine_command", "restore_command", "writemem_command",
  "loadmem_command", "setpmem_command", "deref_command", "query_command",
  "take_command", "disassemble_command", "instrument_command",
  "doit_command", "crc_command", "help_command", "calc_command",
  "addlyt_command", "remlyt_command", "lyt_command", "if_command",
  "vexpression", "expression", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-200)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-339)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     623,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,    -8,  1415,   -12,  -116,   345,  -103,  1372,
     307,   333,   -65,   -47,   -41,  1421,   -92,  -200,  -200,   -40,
     -70,   -66,   -64,   -59,   -51,   -43,     4,   -42,   -18,   -10,
    1135,    23,    55,    60,  1415,  1415,    90,   189,  1415,  1111,
       8,  -200,  1415,  1415,    44,    44,    44,    18,  1415,  1415,
      21,    25,   -68,   -67,   162,  1166,  1415,    -7,   -23,   -69,
     -62,   -33,    27,  1415,  -200,  1415,  1562,  1415,    92,    29,
      48,  -200,  -200,  -200,  -200,  1415,  1415,  -200,  1415,  1415,
    1415,   481,  -200,    54,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  2433,  1415,  -200,   899,
      89,   -57,  -200,  -200,    58,    59,    65,    66,    86,    88,
      44,    99,   102,   103,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  1557,  -200,  1557,
    1557,  -200,  2453,    -6,  -200,   -17,  1415,  -200,   696,    56,
      91,   105,   112,   122,   123,  1415,  1415,  1415,  1415,   125,
     126,   128,   -32,   -58,  -200,   132,  -200,  -200,  -200,  -200,
    -200,  -200,   133,  -200,  -200,  -200,  1191,  -200,  1575,   154,
    1415,  1415,   969,   969,   134,   -54,   135,   136,   137,  1601,
    1218,   138,   140,  -200,    20,   139,   163,   164,  1627,   969,
    -200,  -200,   166,   170,   184,  -200,  1653,  1679,  -200,  -200,
     185,  -200,   186,  -200,   187,  1415,   188,  1415,  1415,  -200,
    -200,  1705,  1731,   190,   191,   -45,   192,  -200,  1360,   253,
     193,  -200,   194,  -200,   195,  -200,  -200,  1757,  1783,   196,
     197,   221,   239,   240,   241,   242,   250,   254,   266,   267,
     268,   271,   272,   274,   275,   276,   277,   278,   280,   281,
     282,   283,   284,   285,   286,   287,   288,   289,   290,   295,
     297,   298,   305,   306,   308,   309,   312,   315,   316,   321,
     323,   324,   325,   326,   328,   329,   338,   340,   341,   342,
     344,   358,   359,   360,   366,   384,  -200,   387,  1809,   388,
    -200,  -200,   389,   389,   389,   870,   389,  -200,  -200,  -200,
    1415,  1415,  1415,  1415,  1415,  1415,  1415,  1415,  1415,  1415,
    1415,  1415,  1415,  1415,  1415,  1415,  1415,  1835,  -200,   390,
     392,  -200,  1415,  1415,  1415,  1415,  1415,  1415,   393,  1415,
    1415,  1415,  -200,  -200,   119,  1557,  1557,  1557,  1557,  1557,
    1557,  1557,  1557,  1557,  1557,  1557,   453,  -200,   458,  -200,
     -13,   463,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  1415,
    2433,  1415,  1415,  1415,  -200,  -200,  -200,   400,  -200,   -20,
     -19,  -200,  -200,  -200,  -200,  1861,  -200,   406,   969,  1887,
    1415,  1415,   969,  1913,  -200,   408,  -200,  -200,  -200,  -200,
    -200,  -200,   165,  -200,   444,  -200,  1939,  -200,  -200,  -200,
    -200,  1965,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
     765,  -200,   796,   938,  -200,  -200,  -200,  -200,   411,  -200,
    -200,  -200,  1991,  1384,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
      63,    63,    63,   389,   389,   389,   389,   478,   478,   478,
     478,   478,   478,    63,    63,    63,  2433,  -200,  -200,  -200,
    2017,  2043,  2069,  2095,  2121,  2147,  -200,  2173,  2199,  2225,
    -200,  -200,  -200,  -200,    74,    74,    74,    74,  -200,  -200,
    -200,   716,   412,   413,   482,  -200,   424,   425,   435,   436,
     442,  -200,   450,  -200,   462,  -200,  -200,  -200,  2251,  -200,
    2479,    81,  2277,  -200,  -200,  -200,  2303,   464,  -200,  -200,
    -200,  2329,  -200,  2355,  -200,  2381,  -200,  -200,  -200,  2407,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,   508,
    -200,  -200,  -200,   469,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,   471,
    -200,  -200
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
      66,   309,   308,   310,   311,   312,    72,    73,    74,    75,
      76,    77,   313,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    70,    71,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   307,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   306,     0,     0,     0,     0,     0,
       0,   315,   316,   317,   318,     0,     0,    67,     0,     0,
       0,     0,     3,     0,   314,    45,    46,    47,    55,    53,
      54,    44,    41,    42,    43,    48,    49,    52,    56,    50,
      51,    57,    58,     4,     5,     6,     7,     9,     8,    10,
      23,    24,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    22,    21,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      59,    60,    61,    62,    63,    64,    65,     0,   119,     0,
       0,     0,   121,   125,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   151,   283,   282,   284,   285,   286,
     287,   281,   280,   289,   290,   291,   292,     0,   138,     0,
       0,   288,     0,   307,   141,     0,     0,   146,     0,     0,
       0,     0,     0,     0,     0,   171,   171,   171,   171,     0,
       0,     0,     0,     0,   187,     0,   174,   175,   176,   177,
     178,   179,     0,   183,   182,   181,     0,   191,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   203,     0,     0,     0,     0,     0,     0,
      68,    69,     0,     0,     0,    90,     0,     0,    80,    81,
       0,    94,     0,    96,     0,     0,     0,     0,     0,   100,
     107,     0,     0,     0,     0,     0,     0,    87,     0,     0,
       0,   113,     0,   115,     0,   152,   117,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   274,     0,     0,     0,
     277,   278,   337,   338,   336,     0,   339,     1,     2,   173,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   279,     0,
       0,   122,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   304,   303,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   144,     0,   142,
     338,     0,   147,   184,   185,   186,   162,   154,   155,   171,
     172,   171,   171,   171,   161,   160,   163,     0,   164,     0,
       0,   166,   127,   180,   189,     0,   190,     0,     0,     0,
       0,     0,     0,     0,   197,     0,   198,   200,   201,   202,
      89,   206,     0,   209,     0,   204,     0,   212,   211,   213,
     214,     0,    91,    92,    93,    79,    78,    95,    97,    99,
       0,    98,     0,     0,   108,   126,    84,    83,     0,    85,
      82,   109,     0,     0,   114,   116,   153,   118,    88,   217,
     218,   219,   263,   227,   221,   222,   223,   224,   225,   226,
     265,   216,   245,   246,   247,   248,   249,   250,   253,   252,
     251,   261,   234,   254,   255,   256,   257,   258,   262,   230,
     231,   232,   233,   235,   237,   236,   228,   229,   238,   239,
     259,   260,   220,   266,   240,   241,   242,   243,   271,   264,
     273,   267,   268,   269,   270,   272,   244,   275,   276,   340,
     324,   325,   326,   332,   333,   334,   335,   320,   321,   327,
     328,   331,   330,   322,   323,   329,   319,   120,   123,   124,
       0,     0,     0,     0,     0,     0,   128,     0,     0,     0,
     305,   297,   298,   299,   293,   294,   300,   301,   295,   296,
     302,     0,     0,     0,     0,   149,     0,     0,     0,     0,
       0,   165,     0,   169,     0,   167,   188,   192,     0,   194,
     321,   322,     0,   196,   199,   207,     0,     0,   205,   215,
     101,     0,   102,     0,   103,     0,    86,   110,   111,     0,
     131,   130,   132,   133,   134,   129,   135,   136,   137,     0,
     139,   145,   143,     0,   148,   156,   157,   158,   159,   170,
     168,   193,   195,   208,   210,   104,   105,   106,   112,     0,
     150,   140
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -200,  -200,   510,   -37,   538,    -2,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -199,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -186,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    91,    92,   252,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   419,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   192,   420
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     156,   392,   408,   393,   394,   280,   594,   421,   422,   423,
     273,   157,   282,   406,   159,   173,   429,   191,   253,   254,
     195,   198,   163,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,   174,   260,   262,   160,   199,
     228,   284,   427,   274,   232,   233,   214,   380,   239,   244,
     445,   278,   248,   249,   602,   604,   430,   200,   256,   257,
     250,   251,   478,   201,   215,   271,   272,   275,   216,   281,
     261,   263,   217,   287,   218,   288,   283,   348,   279,   219,
     431,   381,   245,   246,   446,   352,   353,   220,   354,   355,
     356,   156,   161,   479,    51,   221,   223,   229,   360,   361,
     362,   363,   364,   365,   366,   285,   428,   276,   222,   367,
     368,   369,   370,   371,   372,   373,   374,   375,   603,   605,
     224,   409,   247,   376,    74,   595,   162,   376,   225,   230,
     158,   277,   407,   388,   231,   360,   361,   362,   363,   364,
     365,   366,    81,    82,    83,    84,   367,   440,   369,   370,
     371,   372,   441,   374,   375,   234,   255,   377,   455,   258,
     376,    88,    89,   259,    90,   286,   349,   350,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
     264,   363,   364,   365,   366,   191,   351,   191,   191,   395,
     396,   397,   359,   379,   413,   265,   410,   382,   383,  -338,
    -338,  -338,  -338,   376,   384,   385,   402,   403,   404,   581,
     582,   583,   584,   585,   586,   587,   588,   589,   590,   591,
     597,   376,   598,   599,   600,   386,   435,   387,   437,   414,
     438,   439,   442,   443,   395,   396,   397,   266,   389,    51,
     452,   390,   391,   415,   456,   398,   399,   400,   401,   461,
     416,   402,   403,   404,   267,   268,   235,   236,   237,   238,
     417,   418,   580,   424,   425,   470,   426,   472,   473,    74,
     432,   433,   444,   447,   448,   449,   453,   457,   482,   454,
     360,   361,   362,   363,   364,   365,   366,    81,    82,    83,
      84,   367,   440,   369,   370,   371,   372,   441,   374,   375,
     269,   458,   459,   615,   462,   376,    88,    89,   463,    90,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,   464,   467,   468,   469,   471,   483,   476,   477,
     480,   484,   485,   486,   489,   490,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,   164,   165,
     166,   167,   168,     6,     7,     8,     9,    10,    11,   491,
     550,   551,   552,   553,   554,   555,   556,   557,   558,   559,
     560,   561,   562,   563,   564,   565,   566,   492,   493,   494,
     495,   193,   570,   571,   572,   573,   574,   575,   496,   577,
     578,   579,   497,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   498,   499,   500,    51,   169,   501,
     502,    74,   503,   504,   505,   506,   507,   170,   508,   509,
     510,   511,   512,   513,   514,   515,   516,   517,   518,    81,
      82,    83,    84,   519,    85,   520,   521,    74,   608,    86,
     610,   611,   612,   522,   523,   194,   524,   525,    88,    89,
     526,    90,   616,   527,   528,    81,    82,    83,    84,   529,
      85,   530,   531,   532,   533,   196,   534,   535,   171,   172,
     621,   197,   623,   625,    88,    89,   536,    90,   537,   538,
     539,   357,   540,   629,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,   541,   542,   543,    13,
      14,    15,    16,    17,   544,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,   545,    35,    36,   546,   548,   592,   568,   376,
     569,   576,   593,    37,    38,    39,    40,   596,   601,    41,
      42,    43,    44,    45,   607,    46,   614,    47,   617,   626,
     641,   642,    48,    49,    50,    51,   643,    52,    53,    54,
      55,    56,   644,   645,    57,    58,    59,    60,    61,    62,
      63,    64,    65,   646,   647,    66,    67,    68,    69,    70,
     648,    71,   659,    72,    73,    74,    75,    76,   649,    77,
      78,    79,    80,   360,   361,   362,   363,   364,   365,   366,
     650,   358,   654,    81,    82,    83,    84,   660,    85,   661,
     373,   374,   375,    86,   347,     0,     0,     0,   376,    87,
       0,     0,    88,    89,     0,    90,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     0,     0,
       0,    13,    14,    15,    16,    17,     0,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,    35,    36,     0,     0,     0,
       0,     0,     0,     0,     0,    37,    38,    39,    40,     0,
       0,    41,    42,    43,    44,    45,     0,    46,     0,    47,
       0,     0,     0,     0,    48,    49,    50,    51,     0,    52,
      53,    54,    55,    56,     0,     0,    57,    58,    59,    60,
      61,    62,    63,    64,    65,   411,     0,    66,    67,    68,
      69,    70,     0,    71,     0,    72,    73,    74,    75,    76,
       0,    77,    78,    79,    80,   639,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    81,    82,    83,    84,     0,
      85,     0,     0,     0,     0,    86,     0,     0,     0,     0,
       0,    87,     0,     0,    88,    89,     0,    90,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,   360,   361,   362,   363,   364,   365,   366,     0,     0,
       0,     0,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   395,   396,   397,   412,     0,   376,     0,     0,    51,
       0,     0,   398,   399,   400,   401,     0,     0,   402,   403,
     404,     0,     0,     0,   640,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    74,
      51,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     360,   361,   362,   363,   364,   365,   366,    81,    82,    83,
      84,   367,   440,   369,   370,   371,   372,   441,   374,   375,
      74,     0,     0,   620,     0,   376,    88,    89,     0,    90,
       0,   360,   361,   362,   363,   364,   365,   366,    81,    82,
      83,    84,   367,   440,   369,   370,   371,   372,   441,   374,
     375,     0,     0,     0,   622,     0,   376,    88,    89,     0,
      90,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,   360,   361,   362,   363,   364,
     365,   366,     0,     0,     0,     0,   367,   368,   369,   370,
     371,   372,   373,   374,   375,     0,     0,     0,     0,     0,
     376,     0,    51,   549,   360,   361,   362,   363,   364,   365,
     366,     0,     0,     0,     0,   367,   368,   369,   370,   371,
     372,   373,   374,   375,     0,     0,     0,   378,     0,   376,
       0,     0,    74,    51,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   360,   361,   362,   363,   364,   365,   366,
      81,    82,    83,    84,   367,   440,   369,   370,   371,   372,
     441,   374,   375,    74,     0,     0,   624,     0,   376,    88,
      89,     0,    90,     0,   360,   361,   362,   363,   364,   365,
     366,    81,    82,    83,    84,   367,   440,   369,   370,   371,
     372,   441,   374,   375,     0,     0,     0,     0,     0,   376,
      88,    89,     0,    90,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   240,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,     0,     0,    51,     0,     0,     0,     0,
       0,   226,   241,   242,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,     0,     0,     0,    51,
       0,     0,     0,     0,     0,    74,     0,     0,     0,     0,
       0,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    81,    82,    83,    84,     0,    85,    74,
      51,     0,     0,    86,     0,     0,     0,     0,     0,   243,
       0,     0,    88,    89,     0,    90,     0,    81,    82,    83,
      84,     0,    85,     0,     0,    51,     0,    86,     0,     0,
      74,     0,     0,   227,     0,     0,    88,    89,     0,    90,
       0,     0,     0,     0,     0,     0,     0,     0,    81,    82,
      83,    84,    51,    85,     0,    74,     0,     0,    86,     0,
       0,     0,     0,     0,   270,     0,     0,    88,    89,     0,
      90,     0,     0,    81,    82,    83,    84,     0,    85,     0,
       0,     0,    74,    86,     0,     0,     0,     0,     0,   434,
       0,     0,    88,    89,     0,    90,     0,     0,     0,     0,
      81,    82,    83,    84,     0,    85,     0,     0,     0,     0,
      86,     0,     0,     0,     0,     0,   451,     0,     0,    88,
      89,     0,    90,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,   175,   176,   177,   178,   179,
       6,     7,     8,     9,    10,    11,   180,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
       0,     0,     0,     0,    51,     0,   202,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   181,     0,   203,     0,
       0,     0,     0,     0,     0,     0,   204,     0,    51,     0,
       0,     0,     0,     0,    74,   205,   206,   207,   208,   209,
     210,     0,   211,     0,     0,     0,   182,     0,     0,     0,
       0,     0,    81,    82,    83,    84,     0,    85,    74,    51,
       0,     0,    86,     0,   183,   184,   185,   186,   481,   187,
       0,    88,    89,     0,    90,     0,    81,    82,    83,    84,
     188,    85,     0,   189,   190,     0,    86,     0,     0,    74,
     212,     0,   628,     0,     0,    88,    89,     0,    90,     0,
       0,     0,     0,     0,   213,     0,     0,    81,    82,    83,
      84,     0,    85,     0,     0,     0,     0,    86,     0,     0,
       0,     0,     0,     0,     0,     0,    88,    89,     0,    90,
     175,   176,   177,   178,   179,     6,     7,     8,     9,    10,
      11,   180,     0,     0,     0,     0,     0,     0,     0,     0,
     289,     0,   290,   291,   292,     0,   293,   294,   295,   296,
     297,   298,   299,   300,   301,    27,    28,     0,   302,   303,
     304,   305,   306,     0,   307,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   308,   309,   310,   311,     0,     0,
     312,   313,   314,   315,   316,     0,     0,     0,     0,     0,
       0,   181,     0,     0,   317,   318,     0,     0,     0,   319,
     320,   321,   322,     0,     0,   323,   324,   325,   326,   327,
     328,   329,   330,   331,     0,     0,   332,   333,   334,   335,
     336,   182,   337,     0,     0,   338,     0,   339,   340,     0,
     341,   342,   343,   344,   345,     0,     0,     0,     0,   183,
     184,   185,   186,     0,   187,     0,     0,     0,     0,     0,
     360,   361,   362,   363,   364,   365,   366,     0,   189,   190,
     346,   367,   368,   369,   370,   371,   372,   373,   374,   375,
       0,     0,     0,   436,     0,   376,   360,   361,   362,   363,
     364,   365,   366,     0,     0,     0,     0,   367,   368,   369,
     370,   371,   372,   373,   374,   375,     0,     0,     0,   450,
       0,   376,   360,   361,   362,   363,   364,   365,   366,     0,
       0,     0,     0,   367,   368,   369,   370,   371,   372,   373,
     374,   375,     0,     0,     0,   460,     0,   376,   360,   361,
     362,   363,   364,   365,   366,     0,     0,     0,     0,   367,
     368,   369,   370,   371,   372,   373,   374,   375,     0,     0,
       0,   465,     0,   376,   360,   361,   362,   363,   364,   365,
     366,     0,     0,     0,     0,   367,   368,   369,   370,   371,
     372,   373,   374,   375,     0,     0,     0,   466,     0,   376,
     360,   361,   362,   363,   364,   365,   366,     0,     0,     0,
       0,   367,   368,   369,   370,   371,   372,   373,   374,   375,
       0,     0,     0,   474,     0,   376,   360,   361,   362,   363,
     364,   365,   366,     0,     0,     0,     0,   367,   368,   369,
     370,   371,   372,   373,   374,   375,     0,     0,     0,   475,
       0,   376,   360,   361,   362,   363,   364,   365,   366,     0,
       0,     0,     0,   367,   368,   369,   370,   371,   372,   373,
     374,   375,     0,     0,     0,   487,     0,   376,   360,   361,
     362,   363,   364,   365,   366,     0,     0,     0,     0,   367,
     368,   369,   370,   371,   372,   373,   374,   375,     0,     0,
       0,   488,     0,   376,   360,   361,   362,   363,   364,   365,
     366,     0,     0,     0,     0,   367,   368,   369,   370,   371,
     372,   373,   374,   375,     0,     0,     0,   547,     0,   376,
     360,   361,   362,   363,   364,   365,   366,     0,     0,     0,
       0,   367,   368,   369,   370,   371,   372,   373,   374,   375,
       0,     0,     0,   567,     0,   376,   360,   361,   362,   363,
     364,   365,   366,     0,     0,     0,     0,   367,   368,   369,
     370,   371,   372,   373,   374,   375,     0,     0,     0,   606,
       0,   376,   360,   361,   362,   363,   364,   365,   366,     0,
       0,     0,     0,   367,   368,   369,   370,   371,   372,   373,
     374,   375,     0,     0,     0,   609,     0,   376,   360,   361,
     362,   363,   364,   365,   366,     0,     0,     0,     0,   367,
     368,   369,   370,   371,   372,   373,   374,   375,     0,     0,
       0,   613,     0,   376,   360,   361,   362,   363,   364,   365,
     366,     0,     0,     0,     0,   367,   368,   369,   370,   371,
     372,   373,   374,   375,     0,     0,     0,   618,     0,   376,
     360,   361,   362,   363,   364,   365,   366,     0,     0,     0,
       0,   367,   368,   369,   370,   371,   372,   373,   374,   375,
       0,     0,     0,   619,     0,   376,   360,   361,   362,   363,
     364,   365,   366,     0,     0,     0,     0,   367,   368,   369,
     370,   371,   372,   373,   374,   375,     0,     0,     0,   627,
       0,   376,   360,   361,   362,   363,   364,   365,   366,     0,
       0,     0,     0,   367,   368,   369,   370,   371,   372,   373,
     374,   375,     0,     0,     0,   630,     0,   376,   360,   361,
     362,   363,   364,   365,   366,     0,     0,     0,     0,   367,
     368,   369,   370,   371,   372,   373,   374,   375,     0,     0,
       0,   631,     0,   376,   360,   361,   362,   363,   364,   365,
     366,     0,     0,     0,     0,   367,   368,   369,   370,   371,
     372,   373,   374,   375,     0,     0,     0,   632,     0,   376,
     360,   361,   362,   363,   364,   365,   366,     0,     0,     0,
       0,   367,   368,   369,   370,   371,   372,   373,   374,   375,
       0,     0,     0,   633,     0,   376,   360,   361,   362,   363,
     364,   365,   366,     0,     0,     0,     0,   367,   368,   369,
     370,   371,   372,   373,   374,   375,     0,     0,     0,   634,
       0,   376,   360,   361,   362,   363,   364,   365,   366,     0,
       0,     0,     0,   367,   368,   369,   370,   371,   372,   373,
     374,   375,     0,     0,     0,   635,     0,   376,   360,   361,
     362,   363,   364,   365,   366,     0,     0,     0,     0,   367,
     368,   369,   370,   371,   372,   373,   374,   375,     0,     0,
       0,   636,     0,   376,   360,   361,   362,   363,   364,   365,
     366,     0,     0,     0,     0,   367,   368,   369,   370,   371,
     372,   373,   374,   375,     0,     0,     0,   637,     0,   376,
     360,   361,   362,   363,   364,   365,   366,     0,     0,     0,
       0,   367,   368,   369,   370,   371,   372,   373,   374,   375,
       0,     0,     0,   638,     0,   376,   360,   361,   362,   363,
     364,   365,   366,     0,     0,     0,     0,   367,   368,   369,
     370,   371,   372,   373,   374,   375,     0,     0,     0,   651,
       0,   376,   360,   361,   362,   363,   364,   365,   366,     0,
       0,     0,     0,   367,   368,   369,   370,   371,   372,   373,
     374,   375,     0,     0,     0,   652,     0,   376,   360,   361,
     362,   363,   364,   365,   366,     0,     0,     0,     0,   367,
     368,   369,   370,   371,   372,   373,   374,   375,     0,     0,
       0,   653,     0,   376,   360,   361,   362,   363,   364,   365,
     366,     0,     0,     0,     0,   367,   368,   369,   370,   371,
     372,   373,   374,   375,     0,     0,     0,   655,     0,   376,
     360,   361,   362,   363,   364,   365,   366,     0,     0,     0,
       0,   367,   368,   369,   370,   371,   372,   373,   374,   375,
       0,     0,     0,   656,     0,   376,   360,   361,   362,   363,
     364,   365,   366,     0,     0,     0,     0,   367,   368,   369,
     370,   371,   372,   373,   374,   375,     0,     0,     0,   657,
       0,   376,   360,   361,   362,   363,   364,   365,   366,     0,
       0,     0,     0,   367,   368,   369,   370,   371,   372,   373,
     374,   375,     0,     0,     0,   658,     0,   376,   360,   361,
     362,   363,   364,   365,   366,     0,     0,     0,     0,   367,
     368,   369,   370,   371,   372,   373,   374,   375,   395,   396,
     397,     0,     0,   376,     0,     0,     0,     0,     0,   398,
     399,   400,   401,     0,     0,   402,   403,   404,     0,     0,
       0,     0,     0,   405,  -337,  -337,  -337,  -337,  -337,  -337,
    -337,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -337,  -337,  -337,     0,     0,     0,     0,     0,   376
};

static const yytype_int16 yycheck[] =
{
       0,   187,    19,   189,   190,    74,    19,   206,   207,   208,
      17,    19,    74,    19,    14,    17,    74,    19,    55,    56,
      20,    21,   138,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,   138,   104,   104,    50,   104,
      40,    74,    74,    50,    44,    45,   138,   104,    48,    49,
     104,    74,    52,    53,    74,    74,   114,   104,    58,    59,
      16,    17,   107,   104,   104,    65,    66,    74,   138,   138,
     138,   138,   138,    73,   138,    75,   138,    77,   101,   138,
     138,   138,    74,    75,   138,    85,    86,   138,    88,    89,
      90,    91,   104,   138,    74,   138,   138,    74,   115,   116,
     117,   118,   119,   120,   121,   138,   138,   114,   104,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   138,   138,
     138,   138,   114,   140,   104,   138,   138,   140,   138,    74,
     138,   138,   138,   170,    74,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,    65,   138,   157,   138,   138,
     140,   141,   142,   138,   144,   138,    74,   138,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      18,   118,   119,   120,   121,   187,   138,   189,   190,   115,
     116,   117,   138,   104,   138,    33,   196,   139,   139,   118,
     119,   120,   121,   140,   139,   139,   132,   133,   134,   395,
     396,   397,   398,   399,   400,   401,   402,   403,   404,   405,
     419,   140,   421,   422,   423,   139,   226,   139,    74,   138,
     230,   231,   232,   233,   115,   116,   117,    75,   139,    74,
     240,   139,   139,   138,   244,   126,   127,   128,   129,   249,
     138,   132,   133,   134,    92,    93,    67,    68,    69,    70,
     138,   138,   143,   138,   138,   265,   138,   267,   268,   104,
     138,   138,   138,   138,   138,   138,   138,   138,   278,   139,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     138,   138,   138,   138,   138,   140,   141,   142,   138,   144,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,   138,   138,   138,   138,   138,    74,   138,   138,
     138,   138,   138,   138,   138,   138,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,   138,
     360,   361,   362,   363,   364,   365,   366,   367,   368,   369,
     370,   371,   372,   373,   374,   375,   376,   138,   138,   138,
     138,    74,   382,   383,   384,   385,   386,   387,   138,   389,
     390,   391,   138,   395,   396,   397,   398,   399,   400,   401,
     402,   403,   404,   405,   138,   138,   138,    74,    63,   138,
     138,   104,   138,   138,   138,   138,   138,    72,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   122,
     123,   124,   125,   138,   127,   138,   138,   104,   438,   132,
     440,   441,   442,   138,   138,   138,   138,   138,   141,   142,
     138,   144,   452,   138,   138,   122,   123,   124,   125,   138,
     127,   138,   138,   138,   138,   132,   138,   138,   123,   124,
     470,   138,   472,   473,   141,   142,   138,   144,   138,   138,
     138,     0,   138,   483,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,   138,   138,   138,    18,
      19,    20,    21,    22,   138,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,   138,    42,    43,   138,   138,    74,   138,   140,
     138,   138,    74,    52,    53,    54,    55,    74,   138,    58,
      59,    60,    61,    62,   138,    64,   138,    66,   104,   138,
     138,   138,    71,    72,    73,    74,    74,    76,    77,    78,
      79,    80,   138,   138,    83,    84,    85,    86,    87,    88,
      89,    90,    91,   138,   138,    94,    95,    96,    97,    98,
     138,   100,    74,   102,   103,   104,   105,   106,   138,   108,
     109,   110,   111,   115,   116,   117,   118,   119,   120,   121,
     138,    91,   138,   122,   123,   124,   125,   138,   127,   138,
     132,   133,   134,   132,    76,    -1,    -1,    -1,   140,   138,
      -1,    -1,   141,   142,    -1,   144,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    18,    19,    20,    21,    22,    -1,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    -1,    42,    43,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    52,    53,    54,    55,    -1,
      -1,    58,    59,    60,    61,    62,    -1,    64,    -1,    66,
      -1,    -1,    -1,    -1,    71,    72,    73,    74,    -1,    76,
      77,    78,    79,    80,    -1,    -1,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    19,    -1,    94,    95,    96,
      97,    98,    -1,   100,    -1,   102,   103,   104,   105,   106,
      -1,   108,   109,   110,   111,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   122,   123,   124,   125,    -1,
     127,    -1,    -1,    -1,    -1,   132,    -1,    -1,    -1,    -1,
      -1,   138,    -1,    -1,   141,   142,    -1,   144,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,   115,   116,   117,   118,   119,   120,   121,    -1,    -1,
      -1,    -1,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   115,   116,   117,   138,    -1,   140,    -1,    -1,    74,
      -1,    -1,   126,   127,   128,   129,    -1,    -1,   132,   133,
     134,    -1,    -1,    -1,   138,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   104,
      74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     104,    -1,    -1,   138,    -1,   140,   141,   142,    -1,   144,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,    -1,    -1,    -1,   138,    -1,   140,   141,   142,    -1,
     144,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,   115,   116,   117,   118,   119,
     120,   121,    -1,    -1,    -1,    -1,   126,   127,   128,   129,
     130,   131,   132,   133,   134,    -1,    -1,    -1,    -1,    -1,
     140,    -1,    74,   143,   115,   116,   117,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,   126,   127,   128,   129,   130,
     131,   132,   133,   134,    -1,    -1,    -1,   138,    -1,   140,
      -1,    -1,   104,    74,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   104,    -1,    -1,   138,    -1,   140,   141,
     142,    -1,   144,    -1,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,    -1,    -1,    -1,    -1,    -1,   140,
     141,   142,    -1,   144,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    57,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    74,    -1,    -1,    -1,    -1,
      -1,    56,    81,    82,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    74,
      -1,    -1,    -1,    -1,    -1,   104,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,   122,   123,   124,   125,    -1,   127,   104,
      74,    -1,    -1,   132,    -1,    -1,    -1,    -1,    -1,   138,
      -1,    -1,   141,   142,    -1,   144,    -1,   122,   123,   124,
     125,    -1,   127,    -1,    -1,    74,    -1,   132,    -1,    -1,
     104,    -1,    -1,   138,    -1,    -1,   141,   142,    -1,   144,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,   123,
     124,   125,    74,   127,    -1,   104,    -1,    -1,   132,    -1,
      -1,    -1,    -1,    -1,   138,    -1,    -1,   141,   142,    -1,
     144,    -1,    -1,   122,   123,   124,   125,    -1,   127,    -1,
      -1,    -1,   104,   132,    -1,    -1,    -1,    -1,    -1,   138,
      -1,    -1,   141,   142,    -1,   144,    -1,    -1,    -1,    -1,
     122,   123,   124,   125,    -1,   127,    -1,    -1,    -1,    -1,
     132,    -1,    -1,    -1,    -1,    -1,   138,    -1,    -1,   141,
     142,    -1,   144,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    74,    -1,    15,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    74,    -1,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    35,    -1,    74,    -1,
      -1,    -1,    -1,    -1,   104,    44,    45,    46,    47,    48,
      49,    -1,    51,    -1,    -1,    -1,   104,    -1,    -1,    -1,
      -1,    -1,   122,   123,   124,   125,    -1,   127,   104,    74,
      -1,    -1,   132,    -1,   122,   123,   124,   125,   138,   127,
      -1,   141,   142,    -1,   144,    -1,   122,   123,   124,   125,
     138,   127,    -1,   141,   142,    -1,   132,    -1,    -1,   104,
      99,    -1,   138,    -1,    -1,   141,   142,    -1,   144,    -1,
      -1,    -1,    -1,    -1,   113,    -1,    -1,   122,   123,   124,
     125,    -1,   127,    -1,    -1,    -1,    -1,   132,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   141,   142,    -1,   144,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      18,    -1,    20,    21,    22,    -1,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    -1,    36,    37,
      38,    39,    40,    -1,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    52,    53,    54,    55,    -1,    -1,
      58,    59,    60,    61,    62,    -1,    -1,    -1,    -1,    -1,
      -1,    74,    -1,    -1,    72,    73,    -1,    -1,    -1,    77,
      78,    79,    80,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    -1,    -1,    94,    95,    96,    97,
      98,   104,   100,    -1,    -1,   103,    -1,   105,   106,    -1,
     108,   109,   110,   111,   112,    -1,    -1,    -1,    -1,   122,
     123,   124,   125,    -1,   127,    -1,    -1,    -1,    -1,    -1,
     115,   116,   117,   118,   119,   120,   121,    -1,   141,   142,
     138,   126,   127,   128,   129,   130,   131,   132,   133,   134,
      -1,    -1,    -1,   138,    -1,   140,   115,   116,   117,   118,
     119,   120,   121,    -1,    -1,    -1,    -1,   126,   127,   128,
     129,   130,   131,   132,   133,   134,    -1,    -1,    -1,   138,
      -1,   140,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,   126,   127,   128,   129,   130,   131,   132,
     133,   134,    -1,    -1,    -1,   138,    -1,   140,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,   126,
     127,   128,   129,   130,   131,   132,   133,   134,    -1,    -1,
      -1,   138,    -1,   140,   115,   116,   117,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,   126,   127,   128,   129,   130,
     131,   132,   133,   134,    -1,    -1,    -1,   138,    -1,   140,
     115,   116,   117,   118,   119,   120,   121,    -1,    -1,    -1,
      -1,   126,   127,   128,   129,   130,   131,   132,   133,   134,
      -1,    -1,    -1,   138,    -1,   140,   115,   116,   117,   118,
     119,   120,   121,    -1,    -1,    -1,    -1,   126,   127,   128,
     129,   130,   131,   132,   133,   134,    -1,    -1,    -1,   138,
      -1,   140,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,   126,   127,   128,   129,   130,   131,   132,
     133,   134,    -1,    -1,    -1,   138,    -1,   140,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,   126,
     127,   128,   129,   130,   131,   132,   133,   134,    -1,    -1,
      -1,   138,    -1,   140,   115,   116,   117,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,   126,   127,   128,   129,   130,
     131,   132,   133,   134,    -1,    -1,    -1,   138,    -1,   140,
     115,   116,   117,   118,   119,   120,   121,    -1,    -1,    -1,
      -1,   126,   127,   128,   129,   130,   131,   132,   133,   134,
      -1,    -1,    -1,   138,    -1,   140,   115,   116,   117,   118,
     119,   120,   121,    -1,    -1,    -1,    -1,   126,   127,   128,
     129,   130,   131,   132,   133,   134,    -1,    -1,    -1,   138,
      -1,   140,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,   126,   127,   128,   129,   130,   131,   132,
     133,   134,    -1,    -1,    -1,   138,    -1,   140,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,   126,
     127,   128,   129,   130,   131,   132,   133,   134,    -1,    -1,
      -1,   138,    -1,   140,   115,   116,   117,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,   126,   127,   128,   129,   130,
     131,   132,   133,   134,    -1,    -1,    -1,   138,    -1,   140,
     115,   116,   117,   118,   119,   120,   121,    -1,    -1,    -1,
      -1,   126,   127,   128,   129,   130,   131,   132,   133,   134,
      -1,    -1,    -1,   138,    -1,   140,   115,   116,   117,   118,
     119,   120,   121,    -1,    -1,    -1,    -1,   126,   127,   128,
     129,   130,   131,   132,   133,   134,    -1,    -1,    -1,   138,
      -1,   140,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,   126,   127,   128,   129,   130,   131,   132,
     133,   134,    -1,    -1,    -1,   138,    -1,   140,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,   126,
     127,   128,   129,   130,   131,   132,   133,   134,    -1,    -1,
      -1,   138,    -1,   140,   115,   116,   117,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,   126,   127,   128,   129,   130,
     131,   132,   133,   134,    -1,    -1,    -1,   138,    -1,   140,
     115,   116,   117,   118,   119,   120,   121,    -1,    -1,    -1,
      -1,   126,   127,   128,   129,   130,   131,   132,   133,   134,
      -1,    -1,    -1,   138,    -1,   140,   115,   116,   117,   118,
     119,   120,   121,    -1,    -1,    -1,    -1,   126,   127,   128,
     129,   130,   131,   132,   133,   134,    -1,    -1,    -1,   138,
      -1,   140,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,   126,   127,   128,   129,   130,   131,   132,
     133,   134,    -1,    -1,    -1,   138,    -1,   140,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,   126,
     127,   128,   129,   130,   131,   132,   133,   134,    -1,    -1,
      -1,   138,    -1,   140,   115,   116,   117,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,   126,   127,   128,   129,   130,
     131,   132,   133,   134,    -1,    -1,    -1,   138,    -1,   140,
     115,   116,   117,   118,   119,   120,   121,    -1,    -1,    -1,
      -1,   126,   127,   128,   129,   130,   131,   132,   133,   134,
      -1,    -1,    -1,   138,    -1,   140,   115,   116,   117,   118,
     119,   120,   121,    -1,    -1,    -1,    -1,   126,   127,   128,
     129,   130,   131,   132,   133,   134,    -1,    -1,    -1,   138,
      -1,   140,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,   126,   127,   128,   129,   130,   131,   132,
     133,   134,    -1,    -1,    -1,   138,    -1,   140,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,   126,
     127,   128,   129,   130,   131,   132,   133,   134,    -1,    -1,
      -1,   138,    -1,   140,   115,   116,   117,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,   126,   127,   128,   129,   130,
     131,   132,   133,   134,    -1,    -1,    -1,   138,    -1,   140,
     115,   116,   117,   118,   119,   120,   121,    -1,    -1,    -1,
      -1,   126,   127,   128,   129,   130,   131,   132,   133,   134,
      -1,    -1,    -1,   138,    -1,   140,   115,   116,   117,   118,
     119,   120,   121,    -1,    -1,    -1,    -1,   126,   127,   128,
     129,   130,   131,   132,   133,   134,    -1,    -1,    -1,   138,
      -1,   140,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,   126,   127,   128,   129,   130,   131,   132,
     133,   134,    -1,    -1,    -1,   138,    -1,   140,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   115,   116,
     117,    -1,    -1,   140,    -1,    -1,    -1,    -1,    -1,   126,
     127,   128,   129,    -1,    -1,   132,   133,   134,    -1,    -1,
      -1,    -1,    -1,   140,   115,   116,   117,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   132,   133,   134,    -1,    -1,    -1,    -1,    -1,   140
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    18,    19,    20,    21,    22,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    42,    43,    52,    53,    54,
      55,    58,    59,    60,    61,    62,    64,    66,    71,    72,
      73,    74,    76,    77,    78,    79,    80,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    94,    95,    96,    97,
      98,   100,   102,   103,   104,   105,   106,   108,   109,   110,
     111,   122,   123,   124,   125,   127,   132,   138,   141,   142,
     144,   146,   147,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   214,    19,   138,   214,
      50,   104,   138,   138,     3,     4,     5,     6,     7,    63,
      72,   123,   124,   150,   138,     3,     4,     5,     6,     7,
      14,    74,   104,   122,   123,   124,   125,   127,   138,   141,
     142,   150,   213,    74,   138,   214,   132,   138,   214,   104,
     104,   104,    15,    27,    35,    44,    45,    46,    47,    48,
      49,    51,    99,   113,   138,   104,   138,   138,   138,   138,
     138,   138,   104,   138,   138,   138,    56,   138,   214,    74,
      74,    74,   214,   214,    65,    67,    68,    69,    70,   214,
      57,    81,    82,   138,   214,    74,    75,   114,   214,   214,
      16,    17,   148,   148,   148,   138,   214,   214,   138,   138,
     104,   138,   104,   138,    18,    33,    75,    92,    93,   138,
     138,   214,   214,    17,    50,    74,   114,   138,    74,   101,
      74,   138,    74,   138,    74,   138,   138,   214,   214,    18,
      20,    21,    22,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    36,    37,    38,    39,    40,    42,    52,    53,
      54,    55,    58,    59,    60,    61,    62,    72,    73,    77,
      78,    79,    80,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    94,    95,    96,    97,    98,   100,   103,   105,
     106,   108,   109,   110,   111,   112,   138,   149,   214,    74,
     138,   138,   214,   214,   214,   214,   214,     0,   147,   138,
     115,   116,   117,   118,   119,   120,   121,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   140,   214,   138,   104,
     104,   138,   139,   139,   139,   139,   139,   139,   148,   139,
     139,   139,   213,   213,   213,   115,   116,   117,   126,   127,
     128,   129,   132,   133,   134,   140,    19,   138,    19,   138,
     214,    19,   138,   138,   138,   138,   138,   138,   138,   179,
     214,   179,   179,   179,   138,   138,   138,    74,   138,    74,
     114,   138,   138,   138,   138,   214,   138,    74,   214,   214,
     127,   132,   214,   214,   138,   104,   138,   138,   138,   138,
     138,   138,   214,   138,   139,   138,   214,   138,   138,   138,
     138,   214,   138,   138,   138,   138,   138,   138,   138,   138,
     214,   138,   214,   214,   138,   138,   138,   138,   107,   138,
     138,   138,   214,    74,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   143,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   138,   138,   138,
     214,   214,   214,   214,   214,   214,   138,   214,   214,   214,
     143,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,    74,    74,    19,   138,    74,   179,   179,   179,
     179,   138,    74,   138,    74,   138,   138,   138,   214,   138,
     214,   214,   214,   138,   138,   138,   214,   104,   138,   138,
     138,   214,   138,   214,   138,   214,   138,   138,   138,   214,
     138,   138,   138,   138,   138,   138,   138,   138,   138,    19,
     138,   138,   138,    74,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,    74,
     138,   138
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   145,   146,   146,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   148,   148,
     149,   149,   150,   150,   150,   150,   150,   150,   151,   151,
     152,   153,   154,   154,   154,   154,   154,   154,   155,   156,
     157,   158,   159,   160,   161,   161,   162,   162,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   164,
     164,   164,   164,   165,   165,   166,   166,   167,   168,   169,
     169,   170,   170,   170,   170,   171,   172,   173,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   176,   177,   177,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   179,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   195,
     195,   195,   196,   197,   198,   199,   200,   201,   202,   202,
     202,   202,   202,   203,   203,   203,   203,   203,   203,   203,
     203,   204,   204,   204,   205,   206,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   208,   209,   210,   211,   212,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214
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
       1,     1,     1,     1,     1,     1,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       2,     2,     3,     3,     3,     3,     4,     2,     3,     3,
       2,     3,     3,     3,     2,     3,     2,     3,     3,     3,
       2,     4,     4,     4,     5,     5,     5,     2,     3,     3,
       4,     4,     5,     2,     3,     2,     3,     2,     3,     2,
       4,     2,     3,     4,     4,     2,     3,     3,     4,     5,
       5,     5,     5,     5,     5,     5,     5,     5,     2,     5,
       7,     2,     3,     5,     3,     5,     2,     3,     5,     4,
       6,     2,     2,     3,     3,     3,     5,     5,     5,     5,
       3,     3,     3,     3,     3,     4,     3,     4,     5,     4,
       5,     0,     1,     2,     2,     2,     2,     2,     2,     2,
       3,     2,     2,     2,     3,     3,     3,     2,     4,     3,
       3,     2,     4,     5,     4,     5,     4,     3,     3,     4,
       3,     3,     3,     2,     3,     4,     3,     4,     5,     3,
       5,     3,     3,     3,     3,     4,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     3,     2,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     2,     2,
       3
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
  YY_USE (yykind);
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
  case 65: /* command: expression  */
#line 227 "bx_parser.y"
                 { eval_value = (yyvsp[0].uval); }
#line 2486 "y.tab.c"
    break;

  case 67: /* command: '\n'  */
#line 230 "bx_parser.y"
      {
      }
#line 2493 "y.tab.c"
    break;

  case 69: /* BX_TOKEN_TOGGLE_ON_OFF: BX_TOKEN_OFF  */
#line 237 "bx_parser.y"
    { (yyval.bval)=(yyvsp[0].bval); }
#line 2499 "y.tab.c"
    break;

  case 71: /* BX_TOKEN_REGISTERS: BX_TOKEN_REGS  */
#line 243 "bx_parser.y"
    { (yyval.sval)=(yyvsp[0].sval); }
#line 2505 "y.tab.c"
    break;

  case 77: /* BX_TOKEN_SEGREG: BX_TOKEN_GS  */
#line 253 "bx_parser.y"
    { (yyval.uval)=(yyvsp[0].uval); }
#line 2511 "y.tab.c"
    break;

  case 78: /* timebp_command: BX_TOKEN_TIMEBP expression '\n'  */
#line 258 "bx_parser.y"
      {
          bx_dbg_timebp_command(0, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2520 "y.tab.c"
    break;

  case 79: /* timebp_command: BX_TOKEN_TIMEBP_ABSOLUTE expression '\n'  */
#line 263 "bx_parser.y"
      {
          bx_dbg_timebp_command(1, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2529 "y.tab.c"
    break;

  case 80: /* modebp_command: BX_TOKEN_MODEBP '\n'  */
#line 271 "bx_parser.y"
      {
          bx_dbg_modebp_command();
          free((yyvsp[-1].sval));
      }
#line 2538 "y.tab.c"
    break;

  case 81: /* vmexitbp_command: BX_TOKEN_VMEXITBP '\n'  */
#line 279 "bx_parser.y"
      {
          bx_dbg_vmexitbp_command();
          free((yyvsp[-1].sval));
      }
#line 2547 "y.tab.c"
    break;

  case 82: /* show_command: BX_TOKEN_SHOW BX_TOKEN_GENERIC '\n'  */
#line 287 "bx_parser.y"
      {
          bx_dbg_show_command((yyvsp[-1].sval));
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2556 "y.tab.c"
    break;

  case 83: /* show_command: BX_TOKEN_SHOW BX_TOKEN_ALL '\n'  */
#line 292 "bx_parser.y"
      {
          bx_dbg_show_command("all");
          free((yyvsp[-2].sval));
      }
#line 2565 "y.tab.c"
    break;

  case 84: /* show_command: BX_TOKEN_SHOW BX_TOKEN_OFF '\n'  */
#line 297 "bx_parser.y"
      {
          bx_dbg_show_command("off");
          free((yyvsp[-2].sval));
      }
#line 2574 "y.tab.c"
    break;

  case 85: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING '\n'  */
#line 302 "bx_parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-1].sval), 0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2583 "y.tab.c"
    break;

  case 86: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING BX_TOKEN_XML '\n'  */
#line 307 "bx_parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-2].sval), 1);
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2592 "y.tab.c"
    break;

  case 87: /* show_command: BX_TOKEN_SHOW '\n'  */
#line 312 "bx_parser.y"
      {
          bx_dbg_show_command(0);
          free((yyvsp[-1].sval));
      }
#line 2601 "y.tab.c"
    break;

  case 88: /* page_command: BX_TOKEN_PAGE expression '\n'  */
#line 320 "bx_parser.y"
      {
          bx_dbg_xlate_address((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2610 "y.tab.c"
    break;

  case 89: /* tlb_command: BX_TOKEN_TLB expression '\n'  */
#line 328 "bx_parser.y"
      {
          bx_dbg_tlb_lookup((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2619 "y.tab.c"
    break;

  case 90: /* ptime_command: BX_TOKEN_PTIME '\n'  */
#line 336 "bx_parser.y"
      {
          bx_dbg_ptime_command();
          free((yyvsp[-1].sval));
      }
#line 2628 "y.tab.c"
    break;

  case 91: /* trace_command: BX_TOKEN_TRACE BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 344 "bx_parser.y"
      {
          bx_dbg_trace_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2637 "y.tab.c"
    break;

  case 92: /* trace_reg_command: BX_TOKEN_TRACEREG BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 352 "bx_parser.y"
      {
          bx_dbg_trace_reg_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2646 "y.tab.c"
    break;

  case 93: /* trace_mem_command: BX_TOKEN_TRACEMEM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 360 "bx_parser.y"
      {
          bx_dbg_trace_mem_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2655 "y.tab.c"
    break;

  case 94: /* print_stack_command: BX_TOKEN_PRINT_STACK '\n'  */
#line 368 "bx_parser.y"
      {
          bx_dbg_print_stack_command(16);
          free((yyvsp[-1].sval));
      }
#line 2664 "y.tab.c"
    break;

  case 95: /* print_stack_command: BX_TOKEN_PRINT_STACK BX_TOKEN_NUMERIC '\n'  */
#line 373 "bx_parser.y"
      {
          bx_dbg_print_stack_command((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2673 "y.tab.c"
    break;

  case 96: /* backtrace_command: BX_TOKEN_BT '\n'  */
#line 381 "bx_parser.y"
      {
        bx_dbg_bt_command(16);
        free((yyvsp[-1].sval));
      }
#line 2682 "y.tab.c"
    break;

  case 97: /* backtrace_command: BX_TOKEN_BT BX_TOKEN_NUMERIC '\n'  */
#line 386 "bx_parser.y"
      {
        bx_dbg_bt_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2691 "y.tab.c"
    break;

  case 98: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_STOP '\n'  */
#line 394 "bx_parser.y"
      {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2700 "y.tab.c"
    break;

  case 99: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_CONTINUE '\n'  */
#line 399 "bx_parser.y"
      {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2709 "y.tab.c"
    break;

  case 100: /* watch_point_command: BX_TOKEN_WATCH '\n'  */
#line 404 "bx_parser.y"
      {
          bx_dbg_print_watchpoints();
          free((yyvsp[-1].sval));
      }
#line 2718 "y.tab.c"
    break;

  case 101: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression '\n'  */
#line 409 "bx_parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2727 "y.tab.c"
    break;

  case 102: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression '\n'  */
#line 414 "bx_parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2736 "y.tab.c"
    break;

  case 103: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression '\n'  */
#line 419 "bx_parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-1].uval), 1); /* BX_WRITE */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2745 "y.tab.c"
    break;

  case 104: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression expression '\n'  */
#line 424 "bx_parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2754 "y.tab.c"
    break;

  case 105: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression expression '\n'  */
#line 429 "bx_parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2763 "y.tab.c"
    break;

  case 106: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression expression '\n'  */
#line 434 "bx_parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_WRITE */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2772 "y.tab.c"
    break;

  case 107: /* watch_point_command: BX_TOKEN_UNWATCH '\n'  */
#line 439 "bx_parser.y"
      {
          bx_dbg_unwatch_all();
          free((yyvsp[-1].sval));
      }
#line 2781 "y.tab.c"
    break;

  case 108: /* watch_point_command: BX_TOKEN_UNWATCH expression '\n'  */
#line 444 "bx_parser.y"
      {
          bx_dbg_unwatch((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2790 "y.tab.c"
    break;

  case 109: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 452 "bx_parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2799 "y.tab.c"
    break;

  case 110: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING expression '\n'  */
#line 457 "bx_parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 0, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2808 "y.tab.c"
    break;

  case 111: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING '\n'  */
#line 462 "bx_parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 1, 0);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2817 "y.tab.c"
    break;

  case 112: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING expression '\n'  */
#line 467 "bx_parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 1, (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2826 "y.tab.c"
    break;

  case 113: /* set_magic_break_points_command: BX_TOKEN_SET_MAGIC_BREAK_POINTS '\n'  */
#line 475 "bx_parser.y"
      {
        bx_dbg_set_magic_bp_mask(0);
        free((yyvsp[-1].sval));
      }
#line 2835 "y.tab.c"
    break;

  case 114: /* set_magic_break_points_command: BX_TOKEN_SET_MAGIC_BREAK_POINTS BX_TOKEN_STRING '\n'  */
#line 480 "bx_parser.y"
      {
        bx_dbg_set_magic_bp_mask(bx_dbg_get_magic_bp_mask_from_str((yyvsp[-1].sval)));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2844 "y.tab.c"
    break;

  case 115: /* clr_magic_break_points_command: BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS '\n'  */
#line 488 "bx_parser.y"
      {
        bx_dbg_set_magic_bp_mask(0);
        free((yyvsp[-1].sval));
      }
#line 2853 "y.tab.c"
    break;

  case 116: /* clr_magic_break_points_command: BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS BX_TOKEN_STRING '\n'  */
#line 493 "bx_parser.y"
      {
        bx_dbg_clr_magic_bp_mask(bx_dbg_get_magic_bp_mask_from_str((yyvsp[-1].sval)));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2862 "y.tab.c"
    break;

  case 117: /* where_command: BX_TOKEN_WHERE '\n'  */
#line 501 "bx_parser.y"
      {
        bx_dbg_where_command();
        free((yyvsp[-1].sval));
      }
#line 2871 "y.tab.c"
    break;

  case 118: /* print_string_command: BX_TOKEN_PRINT_STRING expression '\n'  */
#line 509 "bx_parser.y"
      {
        bx_dbg_print_string_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2880 "y.tab.c"
    break;

  case 119: /* continue_command: BX_TOKEN_CONTINUE '\n'  */
#line 517 "bx_parser.y"
      {
        bx_dbg_continue_command(1);
        free((yyvsp[-1].sval));
      }
#line 2889 "y.tab.c"
    break;

  case 120: /* continue_command: BX_TOKEN_CONTINUE BX_TOKEN_IF expression '\n'  */
#line 522 "bx_parser.y"
      {
        bx_dbg_continue_command((yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2898 "y.tab.c"
    break;

  case 121: /* stepN_command: BX_TOKEN_STEPN '\n'  */
#line 530 "bx_parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[-1].sval));
      }
#line 2907 "y.tab.c"
    break;

  case 122: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC '\n'  */
#line 535 "bx_parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2916 "y.tab.c"
    break;

  case 123: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_ALL BX_TOKEN_NUMERIC '\n'  */
#line 540 "bx_parser.y"
      {
        bx_dbg_stepN_command(-1, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2925 "y.tab.c"
    break;

  case 124: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'  */
#line 545 "bx_parser.y"
      {
        bx_dbg_stepN_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2934 "y.tab.c"
    break;

  case 125: /* step_over_command: BX_TOKEN_STEP_OVER '\n'  */
#line 553 "bx_parser.y"
      {
        bx_dbg_step_over_command();
        free((yyvsp[-1].sval));
      }
#line 2943 "y.tab.c"
    break;

  case 126: /* run_to_laddr_command: BX_TOKEN_RUN_TO_LADDR expression '\n'  */
#line 561 "bx_parser.y"
      {
        bx_dbg_run_to_laddr((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2952 "y.tab.c"
    break;

  case 127: /* cpu_command: BX_TOKEN_CPU BX_TOKEN_NUMERIC '\n'  */
#line 569 "bx_parser.y"
      {
        bx_dbg_set_symbol_command("$cpu", (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2961 "y.tab.c"
    break;

  case 128: /* set_command: BX_TOKEN_SET BX_TOKEN_DISASM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 576 "bx_parser.y"
      {
        bx_dbg_set_auto_disassemble((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2970 "y.tab.c"
    break;

  case 129: /* set_command: BX_TOKEN_SET BX_TOKEN_SYMBOLNAME '=' expression '\n'  */
#line 581 "bx_parser.y"
      {
        bx_dbg_set_symbol_command((yyvsp[-3].sval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2979 "y.tab.c"
    break;

  case 130: /* set_command: BX_TOKEN_SET BX_TOKEN_8BL_REG '=' expression '\n'  */
#line 586 "bx_parser.y"
      {
        bx_dbg_set_reg8l_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2987 "y.tab.c"
    break;

  case 131: /* set_command: BX_TOKEN_SET BX_TOKEN_8BH_REG '=' expression '\n'  */
#line 590 "bx_parser.y"
      {
        bx_dbg_set_reg8h_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2995 "y.tab.c"
    break;

  case 132: /* set_command: BX_TOKEN_SET BX_TOKEN_16B_REG '=' expression '\n'  */
#line 594 "bx_parser.y"
      {
        bx_dbg_set_reg16_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 3003 "y.tab.c"
    break;

  case 133: /* set_command: BX_TOKEN_SET BX_TOKEN_32B_REG '=' expression '\n'  */
#line 598 "bx_parser.y"
      {
        bx_dbg_set_reg32_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 3011 "y.tab.c"
    break;

  case 134: /* set_command: BX_TOKEN_SET BX_TOKEN_64B_REG '=' expression '\n'  */
#line 602 "bx_parser.y"
      {
        bx_dbg_set_reg64_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 3019 "y.tab.c"
    break;

  case 135: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_EIP '=' expression '\n'  */
#line 606 "bx_parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 3027 "y.tab.c"
    break;

  case 136: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_RIP '=' expression '\n'  */
#line 610 "bx_parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 3035 "y.tab.c"
    break;

  case 137: /* set_command: BX_TOKEN_SET BX_TOKEN_SEGREG '=' expression '\n'  */
#line 614 "bx_parser.y"
      {
        bx_dbg_load_segreg((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 3043 "y.tab.c"
    break;

  case 138: /* breakpoint_command: BX_TOKEN_VBREAKPOINT '\n'  */
#line 621 "bx_parser.y"
      {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3052 "y.tab.c"
    break;

  case 139: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression '\n'  */
#line 626 "bx_parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].uval), NULL);
        free((yyvsp[-4].sval));
      }
#line 3061 "y.tab.c"
    break;

  case 140: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 631 "bx_parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-5].uval), (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-6].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3070 "y.tab.c"
    break;

  case 141: /* breakpoint_command: BX_TOKEN_LBREAKPOINT '\n'  */
#line 636 "bx_parser.y"
      {
        bx_dbg_lbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3079 "y.tab.c"
    break;

  case 142: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression '\n'  */
#line 641 "bx_parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 3088 "y.tab.c"
    break;

  case 143: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 646 "bx_parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3097 "y.tab.c"
    break;

  case 144: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING '\n'  */
#line 651 "bx_parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-1].sval), NULL);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3106 "y.tab.c"
    break;

  case 145: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 656 "bx_parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-3].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3115 "y.tab.c"
    break;

  case 146: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '\n'  */
#line 661 "bx_parser.y"
      {
        bx_dbg_pbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3124 "y.tab.c"
    break;

  case 147: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression '\n'  */
#line 666 "bx_parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 3133 "y.tab.c"
    break;

  case 148: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 671 "bx_parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3142 "y.tab.c"
    break;

  case 149: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression '\n'  */
#line 676 "bx_parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-3].sval));
      }
#line 3151 "y.tab.c"
    break;

  case 150: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 681 "bx_parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-5].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3160 "y.tab.c"
    break;

  case 151: /* blist_command: BX_TOKEN_LIST_BREAK '\n'  */
#line 689 "bx_parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-1].sval));
      }
#line 3169 "y.tab.c"
    break;

  case 152: /* slist_command: BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 697 "bx_parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-1].sval));
      }
#line 3178 "y.tab.c"
    break;

  case 153: /* slist_command: BX_TOKEN_LIST_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 702 "bx_parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 3187 "y.tab.c"
    break;

  case 154: /* info_command: BX_TOKEN_INFO BX_TOKEN_PBREAKPOINT '\n'  */
#line 710 "bx_parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3196 "y.tab.c"
    break;

  case 155: /* info_command: BX_TOKEN_INFO BX_TOKEN_CPU '\n'  */
#line 715 "bx_parser.y"
      {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3205 "y.tab.c"
    break;

  case 156: /* info_command: BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric optional_numeric '\n'  */
#line 720 "bx_parser.y"
      {
        bx_dbg_info_idt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3214 "y.tab.c"
    break;

  case 157: /* info_command: BX_TOKEN_INFO BX_TOKEN_IVT optional_numeric optional_numeric '\n'  */
#line 725 "bx_parser.y"
      {
        bx_dbg_info_ivt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3223 "y.tab.c"
    break;

  case 158: /* info_command: BX_TOKEN_INFO BX_TOKEN_GDT optional_numeric optional_numeric '\n'  */
#line 730 "bx_parser.y"
      {
        bx_dbg_info_gdt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3232 "y.tab.c"
    break;

  case 159: /* info_command: BX_TOKEN_INFO BX_TOKEN_LDT optional_numeric optional_numeric '\n'  */
#line 735 "bx_parser.y"
      {
        bx_dbg_info_ldt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3241 "y.tab.c"
    break;

  case 160: /* info_command: BX_TOKEN_INFO BX_TOKEN_TAB '\n'  */
#line 740 "bx_parser.y"
      {
        bx_dbg_dump_table();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3250 "y.tab.c"
    break;

  case 161: /* info_command: BX_TOKEN_INFO BX_TOKEN_TSS '\n'  */
#line 745 "bx_parser.y"
      {
        bx_dbg_info_tss_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3259 "y.tab.c"
    break;

  case 162: /* info_command: BX_TOKEN_INFO BX_TOKEN_FLAGS '\n'  */
#line 750 "bx_parser.y"
      {
        bx_dbg_info_flags();
        free((yyvsp[-2].sval));
      }
#line 3268 "y.tab.c"
    break;

  case 163: /* info_command: BX_TOKEN_INFO BX_TOKEN_LINUX '\n'  */
#line 755 "bx_parser.y"
      {
        bx_dbg_info_linux_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3277 "y.tab.c"
    break;

  case 164: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS '\n'  */
#line 760 "bx_parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3286 "y.tab.c"
    break;

  case 165: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 765 "bx_parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3295 "y.tab.c"
    break;

  case 166: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE '\n'  */
#line 770 "bx_parser.y"
      {
        bx_dbg_info_device("", "");
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3304 "y.tab.c"
    break;

  case 167: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC '\n'  */
#line 775 "bx_parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3313 "y.tab.c"
    break;

  case 168: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC BX_TOKEN_STRING '\n'  */
#line 780 "bx_parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3322 "y.tab.c"
    break;

  case 169: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING '\n'  */
#line 785 "bx_parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3331 "y.tab.c"
    break;

  case 170: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 790 "bx_parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3340 "y.tab.c"
    break;

  case 171: /* optional_numeric: %empty  */
#line 797 "bx_parser.y"
               { (yyval.uval) = EMPTY_ARG; }
#line 3346 "y.tab.c"
    break;

  case 173: /* regs_command: BX_TOKEN_REGISTERS '\n'  */
#line 802 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3355 "y.tab.c"
    break;

  case 174: /* fpu_regs_command: BX_TOKEN_FPU '\n'  */
#line 810 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[-1].sval));
      }
#line 3364 "y.tab.c"
    break;

  case 175: /* mmx_regs_command: BX_TOKEN_MMX '\n'  */
#line 818 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3373 "y.tab.c"
    break;

  case 176: /* xmm_regs_command: BX_TOKEN_XMM '\n'  */
#line 826 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3382 "y.tab.c"
    break;

  case 177: /* ymm_regs_command: BX_TOKEN_YMM '\n'  */
#line 834 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3391 "y.tab.c"
    break;

  case 178: /* zmm_regs_command: BX_TOKEN_ZMM '\n'  */
#line 842 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3400 "y.tab.c"
    break;

  case 179: /* amx_regs_command: BX_TOKEN_AMX '\n'  */
#line 850 "bx_parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_AMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3409 "y.tab.c"
    break;

  case 180: /* print_tile_command: BX_TOKEN_TILE BX_TOKEN_NUMERIC '\n'  */
#line 858 "bx_parser.y"
      {
        bx_dbg_print_amx_tile_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3418 "y.tab.c"
    break;

  case 181: /* segment_regs_command: BX_TOKEN_SEGMENT_REGS '\n'  */
#line 866 "bx_parser.y"
      {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3427 "y.tab.c"
    break;

  case 182: /* control_regs_command: BX_TOKEN_CONTROL_REGS '\n'  */
#line 874 "bx_parser.y"
      {
        bx_dbg_info_control_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3436 "y.tab.c"
    break;

  case 183: /* debug_regs_command: BX_TOKEN_DEBUG_REGS '\n'  */
#line 882 "bx_parser.y"
      {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3445 "y.tab.c"
    break;

  case 184: /* delete_command: BX_TOKEN_DEL_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 890 "bx_parser.y"
      {
        bx_dbg_del_breakpoint_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3454 "y.tab.c"
    break;

  case 185: /* bpe_command: BX_TOKEN_ENABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 898 "bx_parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3463 "y.tab.c"
    break;

  case 186: /* bpd_command: BX_TOKEN_DISABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 905 "bx_parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 0);
        free((yyvsp[-2].sval));
      }
#line 3472 "y.tab.c"
    break;

  case 187: /* quit_command: BX_TOKEN_QUIT '\n'  */
#line 913 "bx_parser.y"
      {
        bx_dbg_quit_command();
        free((yyvsp[-1].sval));
      }
#line 3481 "y.tab.c"
    break;

  case 188: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT expression '\n'  */
#line 921 "bx_parser.y"
      {
        bx_dbg_examine_command((yyvsp[-3].sval), (yyvsp[-2].sval),1, (yyvsp[-1].uval), 1);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3490 "y.tab.c"
    break;

  case 189: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT '\n'  */
#line 926 "bx_parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), (yyvsp[-1].sval),1, 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3499 "y.tab.c"
    break;

  case 190: /* examine_command: BX_TOKEN_EXAMINE expression '\n'  */
#line 931 "bx_parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), NULL,0, (yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3508 "y.tab.c"
    break;

  case 191: /* examine_command: BX_TOKEN_EXAMINE '\n'  */
#line 936 "bx_parser.y"
      {
        bx_dbg_examine_command((yyvsp[-1].sval), NULL,0, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 3517 "y.tab.c"
    break;

  case 192: /* restore_command: BX_TOKEN_RESTORE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 944 "bx_parser.y"
      {
        bx_dbg_restore_command((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3526 "y.tab.c"
    break;

  case 193: /* writemem_command: BX_TOKEN_WRITEMEM BX_TOKEN_STRING expression expression '\n'  */
#line 952 "bx_parser.y"
      {
        bx_dbg_writemem_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3535 "y.tab.c"
    break;

  case 194: /* loadmem_command: BX_TOKEN_LOADMEM BX_TOKEN_STRING expression '\n'  */
#line 960 "bx_parser.y"
      {
        bx_dbg_loadmem_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3544 "y.tab.c"
    break;

  case 195: /* setpmem_command: BX_TOKEN_SETPMEM expression expression expression '\n'  */
#line 968 "bx_parser.y"
      {
        bx_dbg_setpmem_command((yyvsp[-3].uval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3553 "y.tab.c"
    break;

  case 196: /* deref_command: BX_TOKEN_DEREF expression expression '\n'  */
#line 976 "bx_parser.y"
      {
        bx_dbg_deref_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3562 "y.tab.c"
    break;

  case 197: /* query_command: BX_TOKEN_QUERY BX_TOKEN_PENDING '\n'  */
#line 984 "bx_parser.y"
      {
        bx_dbg_query_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3571 "y.tab.c"
    break;

  case 198: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA '\n'  */
#line 992 "bx_parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3580 "y.tab.c"
    break;

  case 199: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA BX_TOKEN_NUMERIC '\n'  */
#line 997 "bx_parser.y"
      {
        bx_dbg_take_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3589 "y.tab.c"
    break;

  case 200: /* take_command: BX_TOKEN_TAKE BX_TOKEN_IRQ '\n'  */
#line 1002 "bx_parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3598 "y.tab.c"
    break;

  case 201: /* take_command: BX_TOKEN_TAKE BX_TOKEN_SMI '\n'  */
#line 1007 "bx_parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3607 "y.tab.c"
    break;

  case 202: /* take_command: BX_TOKEN_TAKE BX_TOKEN_NMI '\n'  */
#line 1012 "bx_parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3616 "y.tab.c"
    break;

  case 203: /* disassemble_command: BX_TOKEN_DISASM '\n'  */
#line 1020 "bx_parser.y"
      {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[-1].sval));
      }
#line 3625 "y.tab.c"
    break;

  case 204: /* disassemble_command: BX_TOKEN_DISASM expression '\n'  */
#line 1025 "bx_parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3634 "y.tab.c"
    break;

  case 205: /* disassemble_command: BX_TOKEN_DISASM expression expression '\n'  */
#line 1030 "bx_parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3643 "y.tab.c"
    break;

  case 206: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT '\n'  */
#line 1035 "bx_parser.y"
      {
        bx_dbg_disassemble_current((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3652 "y.tab.c"
    break;

  case 207: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression '\n'  */
#line 1040 "bx_parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-2].sval), (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3661 "y.tab.c"
    break;

  case 208: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression expression '\n'  */
#line 1045 "bx_parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3670 "y.tab.c"
    break;

  case 209: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SWITCH_MODE '\n'  */
#line 1050 "bx_parser.y"
      {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3679 "y.tab.c"
    break;

  case 210: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SIZE '=' BX_TOKEN_NUMERIC '\n'  */
#line 1055 "bx_parser.y"
      {
        bx_dbg_set_disassemble_size((yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3688 "y.tab.c"
    break;

  case 211: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STOP '\n'  */
#line 1063 "bx_parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3697 "y.tab.c"
    break;

  case 212: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STRING '\n'  */
#line 1069 "bx_parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3706 "y.tab.c"
    break;

  case 213: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_GENERIC '\n'  */
#line 1074 "bx_parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3715 "y.tab.c"
    break;

  case 214: /* doit_command: BX_TOKEN_DOIT expression '\n'  */
#line 1082 "bx_parser.y"
      {
        bx_dbg_doit_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3724 "y.tab.c"
    break;

  case 215: /* crc_command: BX_TOKEN_CRC expression expression '\n'  */
#line 1090 "bx_parser.y"
      {
        bx_dbg_crc_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3733 "y.tab.c"
    break;

  case 216: /* help_command: BX_TOKEN_HELP BX_TOKEN_QUIT '\n'  */
#line 1098 "bx_parser.y"
       {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3742 "y.tab.c"
    break;

  case 217: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTINUE '\n'  */
#line 1103 "bx_parser.y"
       {
         dbg_printf("c|cont|continue - continue executing\n");
         dbg_printf("c|cont|continue if \"expression\" - continue executing only if expression is true\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3752 "y.tab.c"
    break;

  case 218: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEPN '\n'  */
#line 1109 "bx_parser.y"
       {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3763 "y.tab.c"
    break;

  case 219: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEP_OVER '\n'  */
#line 1116 "bx_parser.y"
       {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3772 "y.tab.c"
    break;

  case 220: /* help_command: BX_TOKEN_HELP BX_TOKEN_RUN_TO_LADDR '\n'  */
#line 1121 "bx_parser.y"
       {
         dbg_printf("rla <addr> - continue until reaching the linear address\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3781 "y.tab.c"
    break;

  case 221: /* help_command: BX_TOKEN_HELP BX_TOKEN_VBREAKPOINT '\n'  */
#line 1126 "bx_parser.y"
       {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         dbg_printf("vb|vbreak <seg:offset> if \"expression\" - set a conditional virtual address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3791 "y.tab.c"
    break;

  case 222: /* help_command: BX_TOKEN_HELP BX_TOKEN_LBREAKPOINT '\n'  */
#line 1132 "bx_parser.y"
       {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         dbg_printf("lb|lbreak <addr> if \"expression\" - set a conditional linear address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3801 "y.tab.c"
    break;

  case 223: /* help_command: BX_TOKEN_HELP BX_TOKEN_PBREAKPOINT '\n'  */
#line 1138 "bx_parser.y"
       {
         dbg_printf("b|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         dbg_printf("b|pb|break|pbreak <addr> if \"expression\" - set a conditional physical address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3811 "y.tab.c"
    break;

  case 224: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEL_BREAKPOINT '\n'  */
#line 1144 "bx_parser.y"
       {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3820 "y.tab.c"
    break;

  case 225: /* help_command: BX_TOKEN_HELP BX_TOKEN_ENABLE_BREAKPOINT '\n'  */
#line 1149 "bx_parser.y"
       {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3829 "y.tab.c"
    break;

  case 226: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISABLE_BREAKPOINT '\n'  */
#line 1154 "bx_parser.y"
       {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3838 "y.tab.c"
    break;

  case 227: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_BREAK '\n'  */
#line 1159 "bx_parser.y"
       {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3847 "y.tab.c"
    break;

  case 228: /* help_command: BX_TOKEN_HELP BX_TOKEN_MODEBP '\n'  */
#line 1164 "bx_parser.y"
       {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3856 "y.tab.c"
    break;

  case 229: /* help_command: BX_TOKEN_HELP BX_TOKEN_VMEXITBP '\n'  */
#line 1169 "bx_parser.y"
       {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3865 "y.tab.c"
    break;

  case 230: /* help_command: BX_TOKEN_HELP BX_TOKEN_CRC '\n'  */
#line 1174 "bx_parser.y"
       {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3874 "y.tab.c"
    break;

  case 231: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACE '\n'  */
#line 1179 "bx_parser.y"
       {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3884 "y.tab.c"
    break;

  case 232: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEREG '\n'  */
#line 1185 "bx_parser.y"
       {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3894 "y.tab.c"
    break;

  case 233: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEMEM '\n'  */
#line 1191 "bx_parser.y"
       {
         dbg_printf("trace-mem on  - print all memory accesses occurred during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3904 "y.tab.c"
    break;

  case 234: /* help_command: BX_TOKEN_HELP BX_TOKEN_RESTORE '\n'  */
#line 1197 "bx_parser.y"
       {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3916 "y.tab.c"
    break;

  case 235: /* help_command: BX_TOKEN_HELP BX_TOKEN_PTIME '\n'  */
#line 1205 "bx_parser.y"
       {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3925 "y.tab.c"
    break;

  case 236: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP '\n'  */
#line 1210 "bx_parser.y"
       {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3934 "y.tab.c"
    break;

  case 237: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP_ABSOLUTE '\n'  */
#line 1215 "bx_parser.y"
       {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3943 "y.tab.c"
    break;

  case 238: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STACK '\n'  */
#line 1220 "bx_parser.y"
       {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3952 "y.tab.c"
    break;

  case 239: /* help_command: BX_TOKEN_HELP BX_TOKEN_BT '\n'  */
#line 1225 "bx_parser.y"
       {
         dbg_printf("bt [num_entries] - prints backtrace\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3961 "y.tab.c"
    break;

  case 240: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOAD_SYMBOLS '\n'  */
#line 1230 "bx_parser.y"
       {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3970 "y.tab.c"
    break;

  case 241: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET_MAGIC_BREAK_POINTS '\n'  */
#line 1235 "bx_parser.y"
       {
         dbg_printf("setmagicbps \"cx dx bx sp bp si di\" - set new magic breakpoints. You can specify multiple at once. Using the setmagicbps command without any arguments will disable all of them\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3979 "y.tab.c"
    break;

  case 242: /* help_command: BX_TOKEN_HELP BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS '\n'  */
#line 1240 "bx_parser.y"
       {
         dbg_printf("clrmagicbps \"cx dx bx sp bp si di\" - clear magic breakpoints. You can specify multiple at once. Using the clrmagicbps command without any arguments will disable all of them\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3988 "y.tab.c"
    break;

  case 243: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 1245 "bx_parser.y"
       {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3997 "y.tab.c"
    break;

  case 244: /* help_command: BX_TOKEN_HELP BX_TOKEN_REGISTERS '\n'  */
#line 1250 "bx_parser.y"
       {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4006 "y.tab.c"
    break;

  case 245: /* help_command: BX_TOKEN_HELP BX_TOKEN_FPU '\n'  */
#line 1255 "bx_parser.y"
       {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4015 "y.tab.c"
    break;

  case 246: /* help_command: BX_TOKEN_HELP BX_TOKEN_MMX '\n'  */
#line 1260 "bx_parser.y"
       {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4024 "y.tab.c"
    break;

  case 247: /* help_command: BX_TOKEN_HELP BX_TOKEN_XMM '\n'  */
#line 1265 "bx_parser.y"
       {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4033 "y.tab.c"
    break;

  case 248: /* help_command: BX_TOKEN_HELP BX_TOKEN_YMM '\n'  */
#line 1270 "bx_parser.y"
       {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4042 "y.tab.c"
    break;

  case 249: /* help_command: BX_TOKEN_HELP BX_TOKEN_ZMM '\n'  */
#line 1275 "bx_parser.y"
       {
         dbg_printf("zmm - print AVX-512 state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4051 "y.tab.c"
    break;

  case 250: /* help_command: BX_TOKEN_HELP BX_TOKEN_AMX '\n'  */
#line 1280 "bx_parser.y"
       {
         dbg_printf("amx - print AMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4060 "y.tab.c"
    break;

  case 251: /* help_command: BX_TOKEN_HELP BX_TOKEN_SEGMENT_REGS '\n'  */
#line 1285 "bx_parser.y"
       {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4069 "y.tab.c"
    break;

  case 252: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTROL_REGS '\n'  */
#line 1290 "bx_parser.y"
       {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4078 "y.tab.c"
    break;

  case 253: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEBUG_REGS '\n'  */
#line 1295 "bx_parser.y"
       {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4087 "y.tab.c"
    break;

  case 254: /* help_command: BX_TOKEN_HELP BX_TOKEN_WRITEMEM '\n'  */
#line 1300 "bx_parser.y"
       {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4096 "y.tab.c"
    break;

  case 255: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOADMEM '\n'  */
#line 1305 "bx_parser.y"
       {
         dbg_printf("loadmem <filename> <laddr> - load file bytes to virtual memory starting from the linear address 'laddr'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4105 "y.tab.c"
    break;

  case 256: /* help_command: BX_TOKEN_HELP BX_TOKEN_SETPMEM '\n'  */
#line 1310 "bx_parser.y"
       {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4114 "y.tab.c"
    break;

  case 257: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEREF '\n'  */
#line 1315 "bx_parser.y"
       {
         dbg_printf("deref <addr> <deep> - pointer dereference. For example: get value of [[[rax]]] or ***rax: deref rax 3\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4123 "y.tab.c"
    break;

  case 258: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISASM '\n'  */
#line 1320 "bx_parser.y"
       {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm hex on/off - control disasm offsets and displacements format\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4137 "y.tab.c"
    break;

  case 259: /* help_command: BX_TOKEN_HELP BX_TOKEN_WATCH '\n'  */
#line 1330 "bx_parser.y"
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
#line 4152 "y.tab.c"
    break;

  case 260: /* help_command: BX_TOKEN_HELP BX_TOKEN_UNWATCH '\n'  */
#line 1341 "bx_parser.y"
       {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4162 "y.tab.c"
    break;

  case 261: /* help_command: BX_TOKEN_HELP BX_TOKEN_EXAMINE '\n'  */
#line 1347 "bx_parser.y"
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
#line 4179 "y.tab.c"
    break;

  case 262: /* help_command: BX_TOKEN_HELP BX_TOKEN_INSTRUMENT '\n'  */
#line 1360 "bx_parser.y"
       {
         dbg_printf("instrument <command|\"string command\"> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command|\"string command\">\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4188 "y.tab.c"
    break;

  case 263: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET '\n'  */
#line 1365 "bx_parser.y"
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
#line 4203 "y.tab.c"
    break;

  case 264: /* help_command: BX_TOKEN_HELP BX_TOKEN_PAGE '\n'  */
#line 1376 "bx_parser.y"
       {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4212 "y.tab.c"
    break;

  case 265: /* help_command: BX_TOKEN_HELP BX_TOKEN_INFO '\n'  */
#line 1381 "bx_parser.y"
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
#line 4232 "y.tab.c"
    break;

  case 266: /* help_command: BX_TOKEN_HELP BX_TOKEN_SHOW '\n'  */
#line 1397 "bx_parser.y"
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
#line 4252 "y.tab.c"
    break;

  case 267: /* help_command: BX_TOKEN_HELP BX_TOKEN_CALC '\n'  */
#line 1413 "bx_parser.y"
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
#line 4268 "y.tab.c"
    break;

  case 268: /* help_command: BX_TOKEN_HELP BX_TOKEN_ADDLYT '\n'  */
#line 1425 "bx_parser.y"
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
#line 4284 "y.tab.c"
    break;

  case 269: /* help_command: BX_TOKEN_HELP BX_TOKEN_REMLYT '\n'  */
#line 1437 "bx_parser.y"
       {
         dbg_printf("remlyt - stops debugger to execute the script file added previously with addlyt command.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4293 "y.tab.c"
    break;

  case 270: /* help_command: BX_TOKEN_HELP BX_TOKEN_LYT '\n'  */
#line 1442 "bx_parser.y"
       {
         dbg_printf("lyt - cause debugger to execute script file added previously with addlyt command.\n");
         dbg_printf("    Use it as a refresh/context.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4303 "y.tab.c"
    break;

  case 271: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STRING '\n'  */
#line 1448 "bx_parser.y"
       {
         dbg_printf("print-string <addr> - prints a null-ended string from a linear address.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4312 "y.tab.c"
    break;

  case 272: /* help_command: BX_TOKEN_HELP BX_TOKEN_SOURCE '\n'  */
#line 1453 "bx_parser.y"
       {
         dbg_printf("source <file> - cause debugger to execute a script file.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4321 "y.tab.c"
    break;

  case 273: /* help_command: BX_TOKEN_HELP BX_TOKEN_HELP '\n'  */
#line 1458 "bx_parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4330 "y.tab.c"
    break;

  case 274: /* help_command: BX_TOKEN_HELP '\n'  */
#line 1463 "bx_parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-1].sval));
       }
#line 4339 "y.tab.c"
    break;

  case 275: /* calc_command: BX_TOKEN_CALC expression '\n'  */
#line 1471 "bx_parser.y"
   {
     eval_value = (yyvsp[-1].uval);
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4349 "y.tab.c"
    break;

  case 276: /* addlyt_command: BX_TOKEN_ADDLYT BX_TOKEN_STRING '\n'  */
#line 1480 "bx_parser.y"
   {
     bx_dbg_addlyt((yyvsp[-1].sval));
     free((yyvsp[-2].sval));
     free((yyvsp[-1].sval));
   }
#line 4359 "y.tab.c"
    break;

  case 277: /* remlyt_command: BX_TOKEN_REMLYT '\n'  */
#line 1489 "bx_parser.y"
   {
     bx_dbg_remlyt();
     free((yyvsp[-1].sval));
   }
#line 4368 "y.tab.c"
    break;

  case 278: /* lyt_command: BX_TOKEN_LYT '\n'  */
#line 1497 "bx_parser.y"
   {
     bx_dbg_lyt();
     free((yyvsp[-1].sval));
   }
#line 4377 "y.tab.c"
    break;

  case 279: /* if_command: BX_TOKEN_IF expression '\n'  */
#line 1505 "bx_parser.y"
   {
     eval_value = (yyvsp[-1].uval) != 0;
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4387 "y.tab.c"
    break;

  case 280: /* vexpression: BX_TOKEN_NUMERIC  */
#line 1514 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4393 "y.tab.c"
    break;

  case 281: /* vexpression: BX_TOKEN_STRING  */
#line 1515 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4399 "y.tab.c"
    break;

  case 282: /* vexpression: BX_TOKEN_8BL_REG  */
#line 1516 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4405 "y.tab.c"
    break;

  case 283: /* vexpression: BX_TOKEN_8BH_REG  */
#line 1517 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4411 "y.tab.c"
    break;

  case 284: /* vexpression: BX_TOKEN_16B_REG  */
#line 1518 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4417 "y.tab.c"
    break;

  case 285: /* vexpression: BX_TOKEN_32B_REG  */
#line 1519 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4423 "y.tab.c"
    break;

  case 286: /* vexpression: BX_TOKEN_64B_REG  */
#line 1520 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4429 "y.tab.c"
    break;

  case 287: /* vexpression: BX_TOKEN_OPMASK_REG  */
#line 1521 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4435 "y.tab.c"
    break;

  case 288: /* vexpression: BX_TOKEN_SEGREG  */
#line 1522 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4441 "y.tab.c"
    break;

  case 289: /* vexpression: BX_TOKEN_REG_IP  */
#line 1523 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4447 "y.tab.c"
    break;

  case 290: /* vexpression: BX_TOKEN_REG_EIP  */
#line 1524 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4453 "y.tab.c"
    break;

  case 291: /* vexpression: BX_TOKEN_REG_RIP  */
#line 1525 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4459 "y.tab.c"
    break;

  case 292: /* vexpression: BX_TOKEN_REG_SSP  */
#line 1526 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4465 "y.tab.c"
    break;

  case 293: /* vexpression: vexpression '+' vexpression  */
#line 1527 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4471 "y.tab.c"
    break;

  case 294: /* vexpression: vexpression '-' vexpression  */
#line 1528 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4477 "y.tab.c"
    break;

  case 295: /* vexpression: vexpression '*' vexpression  */
#line 1529 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4483 "y.tab.c"
    break;

  case 296: /* vexpression: vexpression '/' vexpression  */
#line 1530 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) / (yyvsp[0].uval); }
#line 4489 "y.tab.c"
    break;

  case 297: /* vexpression: vexpression BX_TOKEN_DEREF_CHR vexpression  */
#line 1531 "bx_parser.y"
                                                { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 4495 "y.tab.c"
    break;

  case 298: /* vexpression: vexpression BX_TOKEN_RSHIFT vexpression  */
#line 1532 "bx_parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4501 "y.tab.c"
    break;

  case 299: /* vexpression: vexpression BX_TOKEN_LSHIFT vexpression  */
#line 1533 "bx_parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4507 "y.tab.c"
    break;

  case 300: /* vexpression: vexpression '|' vexpression  */
#line 1534 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4513 "y.tab.c"
    break;

  case 301: /* vexpression: vexpression '^' vexpression  */
#line 1535 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4519 "y.tab.c"
    break;

  case 302: /* vexpression: vexpression '&' vexpression  */
#line 1536 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4525 "y.tab.c"
    break;

  case 303: /* vexpression: '!' vexpression  */
#line 1537 "bx_parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4531 "y.tab.c"
    break;

  case 304: /* vexpression: '-' vexpression  */
#line 1538 "bx_parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4537 "y.tab.c"
    break;

  case 305: /* vexpression: '(' vexpression ')'  */
#line 1539 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4543 "y.tab.c"
    break;

  case 306: /* expression: BX_TOKEN_NUMERIC  */
#line 1545 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4549 "y.tab.c"
    break;

  case 307: /* expression: BX_TOKEN_STRING  */
#line 1546 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4555 "y.tab.c"
    break;

  case 308: /* expression: BX_TOKEN_8BL_REG  */
#line 1547 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4561 "y.tab.c"
    break;

  case 309: /* expression: BX_TOKEN_8BH_REG  */
#line 1548 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4567 "y.tab.c"
    break;

  case 310: /* expression: BX_TOKEN_16B_REG  */
#line 1549 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4573 "y.tab.c"
    break;

  case 311: /* expression: BX_TOKEN_32B_REG  */
#line 1550 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4579 "y.tab.c"
    break;

  case 312: /* expression: BX_TOKEN_64B_REG  */
#line 1551 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4585 "y.tab.c"
    break;

  case 313: /* expression: BX_TOKEN_OPMASK_REG  */
#line 1552 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4591 "y.tab.c"
    break;

  case 314: /* expression: BX_TOKEN_SEGREG  */
#line 1553 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4597 "y.tab.c"
    break;

  case 315: /* expression: BX_TOKEN_REG_IP  */
#line 1554 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4603 "y.tab.c"
    break;

  case 316: /* expression: BX_TOKEN_REG_EIP  */
#line 1555 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4609 "y.tab.c"
    break;

  case 317: /* expression: BX_TOKEN_REG_RIP  */
#line 1556 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4615 "y.tab.c"
    break;

  case 318: /* expression: BX_TOKEN_REG_SSP  */
#line 1557 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4621 "y.tab.c"
    break;

  case 319: /* expression: expression ':' expression  */
#line 1558 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[-2].uval), (yyvsp[0].uval)); }
#line 4627 "y.tab.c"
    break;

  case 320: /* expression: expression '+' expression  */
#line 1559 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4633 "y.tab.c"
    break;

  case 321: /* expression: expression '-' expression  */
#line 1560 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4639 "y.tab.c"
    break;

  case 322: /* expression: expression '*' expression  */
#line 1561 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4645 "y.tab.c"
    break;

  case 323: /* expression: expression '/' expression  */
#line 1562 "bx_parser.y"
                                     { (yyval.uval) = ((yyvsp[0].uval) != 0) ? (yyvsp[-2].uval) / (yyvsp[0].uval) : 0; }
#line 4651 "y.tab.c"
    break;

  case 324: /* expression: expression BX_TOKEN_DEREF_CHR expression  */
#line 1563 "bx_parser.y"
                                              { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 4657 "y.tab.c"
    break;

  case 325: /* expression: expression BX_TOKEN_RSHIFT expression  */
#line 1564 "bx_parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4663 "y.tab.c"
    break;

  case 326: /* expression: expression BX_TOKEN_LSHIFT expression  */
#line 1565 "bx_parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4669 "y.tab.c"
    break;

  case 327: /* expression: expression '|' expression  */
#line 1566 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4675 "y.tab.c"
    break;

  case 328: /* expression: expression '^' expression  */
#line 1567 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4681 "y.tab.c"
    break;

  case 329: /* expression: expression '&' expression  */
#line 1568 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4687 "y.tab.c"
    break;

  case 330: /* expression: expression '>' expression  */
#line 1569 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) > (yyvsp[0].uval); }
#line 4693 "y.tab.c"
    break;

  case 331: /* expression: expression '<' expression  */
#line 1570 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) < (yyvsp[0].uval); }
#line 4699 "y.tab.c"
    break;

  case 332: /* expression: expression BX_TOKEN_EQ expression  */
#line 1571 "bx_parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) == (yyvsp[0].uval); }
#line 4705 "y.tab.c"
    break;

  case 333: /* expression: expression BX_TOKEN_NE expression  */
#line 1572 "bx_parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) != (yyvsp[0].uval); }
#line 4711 "y.tab.c"
    break;

  case 334: /* expression: expression BX_TOKEN_LE expression  */
#line 1573 "bx_parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) <= (yyvsp[0].uval); }
#line 4717 "y.tab.c"
    break;

  case 335: /* expression: expression BX_TOKEN_GE expression  */
#line 1574 "bx_parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) >= (yyvsp[0].uval); }
#line 4723 "y.tab.c"
    break;

  case 336: /* expression: '!' expression  */
#line 1575 "bx_parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4729 "y.tab.c"
    break;

  case 337: /* expression: '-' expression  */
#line 1576 "bx_parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4735 "y.tab.c"
    break;

  case 338: /* expression: '*' expression  */
#line 1577 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[0].uval)); }
#line 4741 "y.tab.c"
    break;

  case 339: /* expression: '@' expression  */
#line 1578 "bx_parser.y"
                                     { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[0].uval)); }
#line 4747 "y.tab.c"
    break;

  case 340: /* expression: '(' expression ')'  */
#line 1579 "bx_parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4753 "y.tab.c"
    break;


#line 4757 "y.tab.c"

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

#line 1582 "bx_parser.y"

#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
