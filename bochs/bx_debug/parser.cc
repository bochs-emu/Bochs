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
#line 5 "parser.y"

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
    BX_TOKEN_SHOW = 349,           /* BX_TOKEN_SHOW  */
    BX_TOKEN_LOAD_SYMBOLS = 350,   /* BX_TOKEN_LOAD_SYMBOLS  */
    BX_TOKEN_SET_MAGIC_BREAK_POINTS = 351, /* BX_TOKEN_SET_MAGIC_BREAK_POINTS  */
    BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS = 352, /* BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS  */
    BX_TOKEN_SYMBOLS = 353,        /* BX_TOKEN_SYMBOLS  */
    BX_TOKEN_LIST_SYMBOLS = 354,   /* BX_TOKEN_LIST_SYMBOLS  */
    BX_TOKEN_GLOBAL = 355,         /* BX_TOKEN_GLOBAL  */
    BX_TOKEN_WHERE = 356,          /* BX_TOKEN_WHERE  */
    BX_TOKEN_PRINT_STRING = 357,   /* BX_TOKEN_PRINT_STRING  */
    BX_TOKEN_NUMERIC = 358,        /* BX_TOKEN_NUMERIC  */
    BX_TOKEN_PAGE = 359,           /* BX_TOKEN_PAGE  */
    BX_TOKEN_HELP = 360,           /* BX_TOKEN_HELP  */
    BX_TOKEN_XML = 361,            /* BX_TOKEN_XML  */
    BX_TOKEN_CALC = 362,           /* BX_TOKEN_CALC  */
    BX_TOKEN_ADDLYT = 363,         /* BX_TOKEN_ADDLYT  */
    BX_TOKEN_REMLYT = 364,         /* BX_TOKEN_REMLYT  */
    BX_TOKEN_LYT = 365,            /* BX_TOKEN_LYT  */
    BX_TOKEN_SOURCE = 366,         /* BX_TOKEN_SOURCE  */
    BX_TOKEN_DEVICE = 367,         /* BX_TOKEN_DEVICE  */
    BX_TOKEN_GENERIC = 368,        /* BX_TOKEN_GENERIC  */
    BX_TOKEN_DEREF_CHR = 369,      /* BX_TOKEN_DEREF_CHR  */
    BX_TOKEN_RSHIFT = 370,         /* BX_TOKEN_RSHIFT  */
    BX_TOKEN_LSHIFT = 371,         /* BX_TOKEN_LSHIFT  */
    BX_TOKEN_EQ = 372,             /* BX_TOKEN_EQ  */
    BX_TOKEN_NE = 373,             /* BX_TOKEN_NE  */
    BX_TOKEN_LE = 374,             /* BX_TOKEN_LE  */
    BX_TOKEN_GE = 375,             /* BX_TOKEN_GE  */
    BX_TOKEN_REG_IP = 376,         /* BX_TOKEN_REG_IP  */
    BX_TOKEN_REG_EIP = 377,        /* BX_TOKEN_REG_EIP  */
    BX_TOKEN_REG_RIP = 378,        /* BX_TOKEN_REG_RIP  */
    BX_TOKEN_REG_SSP = 379,        /* BX_TOKEN_REG_SSP  */
    NOT = 380,                     /* NOT  */
    NEG = 381,                     /* NEG  */
    INDIRECT = 382                 /* INDIRECT  */
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
#define BX_TOKEN_SHOW 349
#define BX_TOKEN_LOAD_SYMBOLS 350
#define BX_TOKEN_SET_MAGIC_BREAK_POINTS 351
#define BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS 352
#define BX_TOKEN_SYMBOLS 353
#define BX_TOKEN_LIST_SYMBOLS 354
#define BX_TOKEN_GLOBAL 355
#define BX_TOKEN_WHERE 356
#define BX_TOKEN_PRINT_STRING 357
#define BX_TOKEN_NUMERIC 358
#define BX_TOKEN_PAGE 359
#define BX_TOKEN_HELP 360
#define BX_TOKEN_XML 361
#define BX_TOKEN_CALC 362
#define BX_TOKEN_ADDLYT 363
#define BX_TOKEN_REMLYT 364
#define BX_TOKEN_LYT 365
#define BX_TOKEN_SOURCE 366
#define BX_TOKEN_DEVICE 367
#define BX_TOKEN_GENERIC 368
#define BX_TOKEN_DEREF_CHR 369
#define BX_TOKEN_RSHIFT 370
#define BX_TOKEN_LSHIFT 371
#define BX_TOKEN_EQ 372
#define BX_TOKEN_NE 373
#define BX_TOKEN_LE 374
#define BX_TOKEN_GE 375
#define BX_TOKEN_REG_IP 376
#define BX_TOKEN_REG_EIP 377
#define BX_TOKEN_REG_RIP 378
#define BX_TOKEN_REG_SSP 379
#define NOT 380
#define NEG 381
#define INDIRECT 382

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 14 "parser.y"

  char    *sval;
  Bit64u   uval;
  unsigned bval;

#line 400 "y.tab.c"

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
  YYSYMBOL_BX_TOKEN_SHOW = 94,             /* BX_TOKEN_SHOW  */
  YYSYMBOL_BX_TOKEN_LOAD_SYMBOLS = 95,     /* BX_TOKEN_LOAD_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_SET_MAGIC_BREAK_POINTS = 96, /* BX_TOKEN_SET_MAGIC_BREAK_POINTS  */
  YYSYMBOL_BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS = 97, /* BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS  */
  YYSYMBOL_BX_TOKEN_SYMBOLS = 98,          /* BX_TOKEN_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_LIST_SYMBOLS = 99,     /* BX_TOKEN_LIST_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_GLOBAL = 100,          /* BX_TOKEN_GLOBAL  */
  YYSYMBOL_BX_TOKEN_WHERE = 101,           /* BX_TOKEN_WHERE  */
  YYSYMBOL_BX_TOKEN_PRINT_STRING = 102,    /* BX_TOKEN_PRINT_STRING  */
  YYSYMBOL_BX_TOKEN_NUMERIC = 103,         /* BX_TOKEN_NUMERIC  */
  YYSYMBOL_BX_TOKEN_PAGE = 104,            /* BX_TOKEN_PAGE  */
  YYSYMBOL_BX_TOKEN_HELP = 105,            /* BX_TOKEN_HELP  */
  YYSYMBOL_BX_TOKEN_XML = 106,             /* BX_TOKEN_XML  */
  YYSYMBOL_BX_TOKEN_CALC = 107,            /* BX_TOKEN_CALC  */
  YYSYMBOL_BX_TOKEN_ADDLYT = 108,          /* BX_TOKEN_ADDLYT  */
  YYSYMBOL_BX_TOKEN_REMLYT = 109,          /* BX_TOKEN_REMLYT  */
  YYSYMBOL_BX_TOKEN_LYT = 110,             /* BX_TOKEN_LYT  */
  YYSYMBOL_BX_TOKEN_SOURCE = 111,          /* BX_TOKEN_SOURCE  */
  YYSYMBOL_BX_TOKEN_DEVICE = 112,          /* BX_TOKEN_DEVICE  */
  YYSYMBOL_BX_TOKEN_GENERIC = 113,         /* BX_TOKEN_GENERIC  */
  YYSYMBOL_BX_TOKEN_DEREF_CHR = 114,       /* BX_TOKEN_DEREF_CHR  */
  YYSYMBOL_BX_TOKEN_RSHIFT = 115,          /* BX_TOKEN_RSHIFT  */
  YYSYMBOL_BX_TOKEN_LSHIFT = 116,          /* BX_TOKEN_LSHIFT  */
  YYSYMBOL_BX_TOKEN_EQ = 117,              /* BX_TOKEN_EQ  */
  YYSYMBOL_BX_TOKEN_NE = 118,              /* BX_TOKEN_NE  */
  YYSYMBOL_BX_TOKEN_LE = 119,              /* BX_TOKEN_LE  */
  YYSYMBOL_BX_TOKEN_GE = 120,              /* BX_TOKEN_GE  */
  YYSYMBOL_BX_TOKEN_REG_IP = 121,          /* BX_TOKEN_REG_IP  */
  YYSYMBOL_BX_TOKEN_REG_EIP = 122,         /* BX_TOKEN_REG_EIP  */
  YYSYMBOL_BX_TOKEN_REG_RIP = 123,         /* BX_TOKEN_REG_RIP  */
  YYSYMBOL_BX_TOKEN_REG_SSP = 124,         /* BX_TOKEN_REG_SSP  */
  YYSYMBOL_125_ = 125,                     /* '+'  */
  YYSYMBOL_126_ = 126,                     /* '-'  */
  YYSYMBOL_127_ = 127,                     /* '|'  */
  YYSYMBOL_128_ = 128,                     /* '^'  */
  YYSYMBOL_129_ = 129,                     /* '<'  */
  YYSYMBOL_130_ = 130,                     /* '>'  */
  YYSYMBOL_131_ = 131,                     /* '*'  */
  YYSYMBOL_132_ = 132,                     /* '/'  */
  YYSYMBOL_133_ = 133,                     /* '&'  */
  YYSYMBOL_NOT = 134,                      /* NOT  */
  YYSYMBOL_NEG = 135,                      /* NEG  */
  YYSYMBOL_INDIRECT = 136,                 /* INDIRECT  */
  YYSYMBOL_137_n_ = 137,                   /* '\n'  */
  YYSYMBOL_138_ = 138,                     /* '='  */
  YYSYMBOL_139_ = 139,                     /* ':'  */
  YYSYMBOL_140_ = 140,                     /* '!'  */
  YYSYMBOL_141_ = 141,                     /* '('  */
  YYSYMBOL_142_ = 142,                     /* ')'  */
  YYSYMBOL_143_ = 143,                     /* '@'  */
  YYSYMBOL_YYACCEPT = 144,                 /* $accept  */
  YYSYMBOL_commands = 145,                 /* commands  */
  YYSYMBOL_command = 146,                  /* command  */
  YYSYMBOL_BX_TOKEN_TOGGLE_ON_OFF = 147,   /* BX_TOKEN_TOGGLE_ON_OFF  */
  YYSYMBOL_BX_TOKEN_REGISTERS = 148,       /* BX_TOKEN_REGISTERS  */
  YYSYMBOL_BX_TOKEN_SEGREG = 149,          /* BX_TOKEN_SEGREG  */
  YYSYMBOL_timebp_command = 150,           /* timebp_command  */
  YYSYMBOL_modebp_command = 151,           /* modebp_command  */
  YYSYMBOL_vmexitbp_command = 152,         /* vmexitbp_command  */
  YYSYMBOL_show_command = 153,             /* show_command  */
  YYSYMBOL_page_command = 154,             /* page_command  */
  YYSYMBOL_tlb_command = 155,              /* tlb_command  */
  YYSYMBOL_ptime_command = 156,            /* ptime_command  */
  YYSYMBOL_trace_command = 157,            /* trace_command  */
  YYSYMBOL_trace_reg_command = 158,        /* trace_reg_command  */
  YYSYMBOL_trace_mem_command = 159,        /* trace_mem_command  */
  YYSYMBOL_print_stack_command = 160,      /* print_stack_command  */
  YYSYMBOL_backtrace_command = 161,        /* backtrace_command  */
  YYSYMBOL_watch_point_command = 162,      /* watch_point_command  */
  YYSYMBOL_symbol_command = 163,           /* symbol_command  */
  YYSYMBOL_set_magic_break_points_command = 164, /* set_magic_break_points_command  */
  YYSYMBOL_clr_magic_break_points_command = 165, /* clr_magic_break_points_command  */
  YYSYMBOL_where_command = 166,            /* where_command  */
  YYSYMBOL_print_string_command = 167,     /* print_string_command  */
  YYSYMBOL_continue_command = 168,         /* continue_command  */
  YYSYMBOL_stepN_command = 169,            /* stepN_command  */
  YYSYMBOL_step_over_command = 170,        /* step_over_command  */
  YYSYMBOL_cpu_command = 171,              /* cpu_command  */
  YYSYMBOL_set_command = 172,              /* set_command  */
  YYSYMBOL_breakpoint_command = 173,       /* breakpoint_command  */
  YYSYMBOL_blist_command = 174,            /* blist_command  */
  YYSYMBOL_slist_command = 175,            /* slist_command  */
  YYSYMBOL_info_command = 176,             /* info_command  */
  YYSYMBOL_optional_numeric = 177,         /* optional_numeric  */
  YYSYMBOL_regs_command = 178,             /* regs_command  */
  YYSYMBOL_fpu_regs_command = 179,         /* fpu_regs_command  */
  YYSYMBOL_mmx_regs_command = 180,         /* mmx_regs_command  */
  YYSYMBOL_xmm_regs_command = 181,         /* xmm_regs_command  */
  YYSYMBOL_ymm_regs_command = 182,         /* ymm_regs_command  */
  YYSYMBOL_zmm_regs_command = 183,         /* zmm_regs_command  */
  YYSYMBOL_amx_regs_command = 184,         /* amx_regs_command  */
  YYSYMBOL_print_tile_command = 185,       /* print_tile_command  */
  YYSYMBOL_segment_regs_command = 186,     /* segment_regs_command  */
  YYSYMBOL_control_regs_command = 187,     /* control_regs_command  */
  YYSYMBOL_debug_regs_command = 188,       /* debug_regs_command  */
  YYSYMBOL_delete_command = 189,           /* delete_command  */
  YYSYMBOL_bpe_command = 190,              /* bpe_command  */
  YYSYMBOL_bpd_command = 191,              /* bpd_command  */
  YYSYMBOL_quit_command = 192,             /* quit_command  */
  YYSYMBOL_examine_command = 193,          /* examine_command  */
  YYSYMBOL_restore_command = 194,          /* restore_command  */
  YYSYMBOL_writemem_command = 195,         /* writemem_command  */
  YYSYMBOL_loadmem_command = 196,          /* loadmem_command  */
  YYSYMBOL_setpmem_command = 197,          /* setpmem_command  */
  YYSYMBOL_deref_command = 198,            /* deref_command  */
  YYSYMBOL_query_command = 199,            /* query_command  */
  YYSYMBOL_take_command = 200,             /* take_command  */
  YYSYMBOL_disassemble_command = 201,      /* disassemble_command  */
  YYSYMBOL_instrument_command = 202,       /* instrument_command  */
  YYSYMBOL_doit_command = 203,             /* doit_command  */
  YYSYMBOL_crc_command = 204,              /* crc_command  */
  YYSYMBOL_help_command = 205,             /* help_command  */
  YYSYMBOL_calc_command = 206,             /* calc_command  */
  YYSYMBOL_addlyt_command = 207,           /* addlyt_command  */
  YYSYMBOL_remlyt_command = 208,           /* remlyt_command  */
  YYSYMBOL_lyt_command = 209,              /* lyt_command  */
  YYSYMBOL_if_command = 210,               /* if_command  */
  YYSYMBOL_vexpression = 211,              /* vexpression  */
  YYSYMBOL_expression = 212                /* expression  */
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
#define YYFINAL  353
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2573

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  144
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  69
/* YYNRULES -- Number of rules.  */
#define YYNRULES  337
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  656

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   382


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
     137,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   140,     2,     2,     2,     2,   133,     2,
     141,   142,   131,   125,     2,   126,     2,   132,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   139,     2,
     129,   138,   130,     2,   143,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   128,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   127,     2,     2,     2,     2,     2,
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
     134,   135,   136
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   160,   160,   161,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   233,   234,   239,
     240,   245,   246,   247,   248,   249,   250,   255,   260,   268,
     276,   284,   289,   294,   299,   304,   309,   317,   325,   333,
     341,   349,   357,   365,   370,   378,   383,   391,   396,   401,
     406,   411,   416,   421,   426,   431,   436,   441,   449,   454,
     459,   464,   472,   477,   485,   490,   498,   506,   514,   519,
     527,   532,   537,   542,   550,   558,   565,   570,   575,   579,
     583,   587,   591,   595,   599,   603,   610,   615,   620,   625,
     630,   635,   640,   645,   650,   655,   660,   665,   670,   678,
     686,   691,   699,   704,   709,   714,   719,   724,   729,   734,
     739,   744,   749,   754,   759,   764,   769,   774,   779,   787,
     788,   791,   799,   807,   815,   823,   831,   839,   847,   855,
     863,   871,   879,   887,   894,   902,   910,   915,   920,   925,
     933,   941,   949,   957,   965,   973,   981,   986,   991,   996,
    1001,  1009,  1014,  1019,  1024,  1029,  1034,  1039,  1044,  1052,
    1058,  1063,  1071,  1079,  1087,  1092,  1098,  1105,  1110,  1116,
    1122,  1128,  1133,  1138,  1143,  1148,  1153,  1158,  1163,  1169,
    1175,  1181,  1189,  1194,  1199,  1204,  1209,  1214,  1219,  1224,
    1229,  1234,  1239,  1244,  1249,  1254,  1259,  1264,  1269,  1274,
    1279,  1284,  1289,  1294,  1299,  1304,  1314,  1325,  1331,  1344,
    1349,  1360,  1365,  1381,  1397,  1409,  1421,  1426,  1432,  1437,
    1442,  1447,  1455,  1464,  1473,  1481,  1489,  1499,  1500,  1501,
    1502,  1503,  1504,  1505,  1506,  1507,  1508,  1509,  1510,  1511,
    1512,  1513,  1514,  1515,  1516,  1517,  1518,  1519,  1520,  1521,
    1522,  1523,  1524,  1530,  1531,  1532,  1533,  1534,  1535,  1536,
    1537,  1538,  1539,  1540,  1541,  1542,  1543,  1544,  1545,  1546,
    1547,  1548,  1549,  1550,  1551,  1552,  1553,  1554,  1555,  1556,
    1557,  1558,  1559,  1560,  1561,  1562,  1563,  1564
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
  "BX_TOKEN_WRITE", "BX_TOKEN_SHOW", "BX_TOKEN_LOAD_SYMBOLS",
  "BX_TOKEN_SET_MAGIC_BREAK_POINTS", "BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS",
  "BX_TOKEN_SYMBOLS", "BX_TOKEN_LIST_SYMBOLS", "BX_TOKEN_GLOBAL",
  "BX_TOKEN_WHERE", "BX_TOKEN_PRINT_STRING", "BX_TOKEN_NUMERIC",
  "BX_TOKEN_PAGE", "BX_TOKEN_HELP", "BX_TOKEN_XML", "BX_TOKEN_CALC",
  "BX_TOKEN_ADDLYT", "BX_TOKEN_REMLYT", "BX_TOKEN_LYT", "BX_TOKEN_SOURCE",
  "BX_TOKEN_DEVICE", "BX_TOKEN_GENERIC", "BX_TOKEN_DEREF_CHR",
  "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT", "BX_TOKEN_EQ", "BX_TOKEN_NE",
  "BX_TOKEN_LE", "BX_TOKEN_GE", "BX_TOKEN_REG_IP", "BX_TOKEN_REG_EIP",
  "BX_TOKEN_REG_RIP", "BX_TOKEN_REG_SSP", "'+'", "'-'", "'|'", "'^'",
  "'<'", "'>'", "'*'", "'/'", "'&'", "NOT", "NEG", "INDIRECT", "'\\n'",
  "'='", "':'", "'!'", "'('", "')'", "'@'", "$accept", "commands",
  "command", "BX_TOKEN_TOGGLE_ON_OFF", "BX_TOKEN_REGISTERS",
  "BX_TOKEN_SEGREG", "timebp_command", "modebp_command",
  "vmexitbp_command", "show_command", "page_command", "tlb_command",
  "ptime_command", "trace_command", "trace_reg_command",
  "trace_mem_command", "print_stack_command", "backtrace_command",
  "watch_point_command", "symbol_command",
  "set_magic_break_points_command", "clr_magic_break_points_command",
  "where_command", "print_string_command", "continue_command",
  "stepN_command", "step_over_command", "cpu_command", "set_command",
  "breakpoint_command", "blist_command", "slist_command", "info_command",
  "optional_numeric", "regs_command", "fpu_regs_command",
  "mmx_regs_command", "xmm_regs_command", "ymm_regs_command",
  "zmm_regs_command", "amx_regs_command", "print_tile_command",
  "segment_regs_command", "control_regs_command", "debug_regs_command",
  "delete_command", "bpe_command", "bpd_command", "quit_command",
  "examine_command", "restore_command", "writemem_command",
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

#define YYPACT_NINF (-198)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-336)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     617,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,   -17,  1424,   -39,  -124,   192,  -102,  1451,
     330,  1157,   -60,   -53,   -46,  1226,   -96,  -198,  -198,   -40,
     -69,   -67,   -59,   -56,   -55,   -54,   -11,   -51,   -30,   -29,
    1126,    23,    47,    53,  1424,  1424,    88,   120,  1424,  1102,
     -36,  -198,  1424,  1424,    39,    39,    39,    20,  1424,  1424,
      24,    29,   -66,   -43,   236,  1182,    17,   -20,   -58,   -38,
     -28,    30,  1424,  -198,  1424,  1595,  1424,   110,    54,    55,
    -198,  -198,  -198,  -198,  1424,  1424,  -198,  1424,  1424,  1424,
     476,  -198,    56,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  2414,  1424,  -198,   862,   103,   -42,
    -198,  -198,    81,    85,    87,    89,    97,    98,    39,    99,
     102,   114,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  1590,  -198,  1590,  1590,  -198,
    2434,    -9,  -198,   -14,  1424,  -198,   291,   104,   111,   116,
     124,   125,   130,  1424,  1424,  1424,  1424,   133,   134,   135,
     -27,   -62,  -198,   136,  -198,  -198,  -198,  -198,  -198,  -198,
     137,  -198,  -198,  -198,  1208,  -198,   891,   152,  1424,  1424,
     961,   961,   140,   -41,   162,   163,   164,  1608,  1349,   166,
     138,  -198,    19,   170,   172,   175,  1634,   961,  -198,  -198,
     176,   179,   180,  -198,  1660,  1686,  -198,  -198,   181,  -198,
     182,  -198,   183,  1424,   184,  1424,  1424,  -198,  -198,  1712,
     185,   190,   -64,   193,  -198,  1373,   257,   195,  -198,   209,
    -198,   210,  -198,  -198,  1738,  1764,   211,   212,   213,   214,
     215,   216,   217,   218,   237,   238,   239,   240,   247,   251,
     263,   264,   265,   266,   275,   276,   277,   278,   288,   289,
     290,   292,   294,   295,   298,   302,   310,   312,   313,   318,
     323,   325,   327,   335,   337,   338,   340,   354,   355,   356,
     362,   380,   383,   384,   385,   386,   387,   388,   389,   390,
     395,   396,  -198,   402,  1790,   404,  -198,  -198,   324,   324,
     324,   720,   324,  -198,  -198,  -198,  1424,  1424,  1424,  1424,
    1424,  1424,  1424,  1424,  1424,  1424,  1424,  1424,  1424,  1424,
    1424,  1424,  1424,  1816,  -198,   406,   407,  -198,  1424,  1424,
    1424,  1424,  1424,  1424,   408,  1424,  1424,  1424,  -198,  -198,
     118,  1590,  1590,  1590,  1590,  1590,  1590,  1590,  1590,  1590,
    1590,  1590,   472,  -198,   477,  -198,   -13,   483,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  1424,  2414,  1424,  1424,  1424,
    -198,  -198,  -198,   421,  -198,    -8,    -5,  -198,  -198,  -198,
    -198,  1842,  -198,   431,   961,  1868,  1424,  1424,   961,  1894,
    -198,   432,  -198,  -198,  -198,  -198,  -198,  -198,   165,  -198,
     471,  -198,  1920,  -198,  -198,  -198,  -198,  1946,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,   758,  -198,   789,   930,
    -198,  -198,  -198,   439,  -198,  -198,  -198,  1972,  1400,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,   139,   139,   139,   324,   324,   324,
     324,   326,   326,   326,   326,   326,   326,   139,   139,   139,
    2414,  -198,  -198,  -198,  1998,  2024,  2050,  2076,  2102,  2128,
    -198,  2154,  2180,  2206,  -198,  -198,  -198,  -198,    49,    49,
      49,    49,  -198,  -198,  -198,   690,   445,   457,   521,  -198,
     459,   464,   466,   474,   478,  -198,   481,  -198,   495,  -198,
    -198,  -198,  2232,  -198,   473,   206,  2258,  -198,  -198,  -198,
    2284,   496,  -198,  -198,  -198,  2310,  -198,  2336,  -198,  2362,
    -198,  -198,  -198,  2388,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,   534,  -198,  -198,  -198,   497,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,   503,  -198,  -198
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
      65,   306,   305,   307,   308,   309,    71,    72,    73,    74,
      75,    76,   310,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    69,    70,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   304,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   303,     0,     0,     0,     0,     0,     0,
     312,   313,   314,   315,     0,     0,    66,     0,     0,     0,
       0,     3,     0,   311,    44,    45,    46,    54,    52,    53,
      43,    40,    41,    42,    47,    48,    51,    55,    49,    50,
      56,    57,     4,     5,     6,     8,     7,     9,    22,    23,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      21,    20,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    58,    59,
      60,    61,    62,    63,    64,     0,   118,     0,     0,     0,
     120,   124,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   149,   280,   279,   281,   282,   283,   284,   278,
     277,   286,   287,   288,   289,     0,   136,     0,     0,   285,
       0,   304,   139,     0,     0,   144,     0,     0,     0,     0,
       0,     0,     0,   169,   169,   169,   169,     0,     0,     0,
       0,     0,   185,     0,   172,   173,   174,   175,   176,   177,
       0,   181,   180,   179,     0,   189,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   201,     0,     0,     0,     0,     0,     0,    67,    68,
       0,     0,     0,    89,     0,     0,    79,    80,     0,    93,
       0,    95,     0,     0,     0,     0,     0,    99,   106,     0,
       0,     0,     0,     0,    86,     0,     0,     0,   112,     0,
     114,     0,   150,   116,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   271,     0,     0,     0,   274,   275,   334,   335,
     333,     0,   336,     1,     2,   171,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   276,     0,     0,   121,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   301,   300,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   142,     0,   140,   335,     0,   145,   182,
     183,   184,   160,   152,   153,   169,   170,   169,   169,   169,
     159,   158,   161,     0,   162,     0,     0,   164,   125,   178,
     187,     0,   188,     0,     0,     0,     0,     0,     0,     0,
     195,     0,   196,   198,   199,   200,    88,   204,     0,   207,
       0,   202,     0,   210,   209,   211,   212,     0,    90,    91,
      92,    78,    77,    94,    96,    98,     0,    97,     0,     0,
     107,    83,    82,     0,    84,    81,   108,     0,     0,   113,
     115,   151,   117,    87,   215,   216,   217,   260,   224,   218,
     219,   220,   221,   222,   223,   262,   214,   242,   243,   244,
     245,   246,   247,   250,   249,   248,   258,   231,   251,   252,
     253,   254,   255,   259,   227,   228,   229,   230,   232,   234,
     233,   225,   226,   235,   236,   256,   257,   263,   237,   238,
     239,   240,   268,   261,   270,   264,   265,   266,   267,   269,
     241,   272,   273,   337,   321,   322,   323,   329,   330,   331,
     332,   317,   318,   324,   325,   328,   327,   319,   320,   326,
     316,   119,   122,   123,     0,     0,     0,     0,     0,     0,
     126,     0,     0,     0,   302,   294,   295,   296,   290,   291,
     297,   298,   292,   293,   299,     0,     0,     0,     0,   147,
       0,     0,     0,     0,     0,   163,     0,   167,     0,   165,
     186,   190,     0,   192,   318,   319,     0,   194,   197,   205,
       0,     0,   203,   213,   100,     0,   101,     0,   102,     0,
      85,   109,   110,     0,   129,   128,   130,   131,   132,   127,
     133,   134,   135,     0,   137,   143,   141,     0,   146,   154,
     155,   156,   157,   168,   166,   191,   193,   206,   208,   103,
     104,   105,   111,     0,   148,   138
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -198,  -198,   519,   -37,   535,    -2,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -197,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -184,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    90,    91,   250,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   415,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   190,   416
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     154,   388,   155,   389,   390,   404,   588,   417,   418,   419,
     402,   158,   425,   161,   157,   171,   277,   189,   251,   252,
     193,   196,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,   270,   172,   279,   258,   243,   244,
     226,   212,   473,   197,   230,   231,   281,   423,   237,   242,
     198,   426,   246,   247,   275,   248,   249,   199,   254,   255,
     260,   376,   441,   213,   159,   269,   596,   271,   214,   598,
     215,   259,   284,   474,   285,   427,   344,   245,   216,   278,
     276,   217,   218,   219,   348,   349,   221,   350,   351,   352,
     154,   272,   220,    51,   261,   377,   442,   227,   160,   280,
     356,   357,   358,   359,   360,   361,   362,   222,   223,   282,
     424,   363,   364,   365,   366,   367,   368,   369,   370,   371,
     156,   228,    73,   405,   589,   372,   372,   229,   403,   597,
     273,   384,   599,   356,   357,   358,   359,   360,   361,   362,
      80,    81,    82,    83,   363,   436,   365,   366,   367,   368,
     437,   370,   371,   232,   274,   373,   451,   253,   372,    87,
      88,   256,    89,   391,   392,   393,   257,   283,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
     398,   399,   400,   189,   345,   189,   189,   233,   234,   235,
     236,   346,   347,   355,   406,   162,   163,   164,   165,   166,
       6,     7,     8,     9,    10,    11,   375,   575,   576,   577,
     578,   579,   580,   581,   582,   583,   584,   585,   591,   378,
     592,   593,   594,   379,   431,   380,   433,   381,   434,   435,
     438,   439,   391,   392,   393,   382,   383,   385,   448,    51,
     386,   409,   452,   394,   395,   396,   397,   457,   410,   398,
     399,   400,   387,   411,   262,   167,   359,   360,   361,   362,
     574,   412,   413,   466,   168,   468,   469,   414,    73,   263,
     420,   421,   422,   428,   429,   477,   450,   440,   372,   356,
     357,   358,   359,   360,   361,   362,    80,    81,    82,    83,
     363,   436,   365,   366,   367,   368,   437,   370,   371,   443,
     444,   445,   609,   449,   372,    87,    88,   453,    89,   454,
     407,   264,   455,   458,   169,   170,   459,   460,   463,   464,
     465,   467,   471,  -335,  -335,  -335,  -335,   472,   265,   266,
     475,   478,   479,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,   372,   480,   481,   484,   485,
     486,   487,   488,   489,   490,   491,   544,   545,   546,   547,
     548,   549,   550,   551,   552,   553,   554,   555,   556,   557,
     558,   559,   560,   267,   492,   493,   494,   495,   564,   565,
     566,   567,   568,   569,   496,   571,   572,   573,   497,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     498,   499,   500,   501,   191,   356,   357,   358,   359,   360,
     361,   362,   502,   503,   504,   505,   363,   364,   365,   366,
     367,   368,   369,   370,   371,   506,   507,   508,   408,   509,
     372,   510,   511,    73,   602,   512,   604,   605,   606,   513,
     356,   357,   358,   359,   360,   361,   362,   514,   610,   515,
     516,    80,    81,    82,    83,   517,    84,   369,   370,   371,
     518,    85,   519,   372,   520,   372,   615,   192,   617,   619,
      87,    88,   521,    89,   522,   523,   353,   524,   623,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,   525,   526,   527,    13,    14,    15,    16,    17,   528,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,   529,    35,    36,
     530,   531,   532,   533,   534,   535,   536,   537,    37,    38,
      39,    40,   538,   539,    41,    42,    43,    44,    45,   540,
      46,   542,    47,   562,   563,   570,   586,    48,    49,    50,
      51,   587,    52,    53,    54,    55,    56,   590,   595,    57,
      58,    59,    60,    61,    62,    63,    64,    65,   601,   608,
      66,    67,    68,    69,   611,    70,   620,    71,    72,    73,
      74,    75,   635,    76,    77,    78,    79,  -334,  -334,  -334,
    -334,  -334,  -334,  -334,   636,   637,   638,    80,    81,    82,
      83,   639,    84,   640,  -334,  -334,  -334,    85,   653,   354,
     343,   641,   372,    86,     0,   642,    87,    88,   643,    89,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,   644,   648,   654,    13,    14,    15,    16,    17,
     655,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,     0,    35,
      36,     0,     0,     0,     0,     0,     0,     0,     0,    37,
      38,    39,    40,     0,     0,    41,    42,    43,    44,    45,
       0,    46,     0,    47,     0,     0,     0,     0,    48,    49,
      50,    51,     0,    52,    53,    54,    55,    56,     0,     0,
      57,    58,    59,    60,    61,    62,    63,    64,    65,   633,
       0,    66,    67,    68,    69,     0,    70,     0,    71,    72,
      73,    74,    75,     0,    76,    77,    78,    79,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    80,    81,
      82,    83,     0,    84,     0,     0,     0,     0,    85,     0,
       0,     0,     0,     0,    86,     0,     0,    87,    88,     0,
      89,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,   391,   392,   393,     0,     0,     0,
       0,     0,     0,     0,     0,   394,   395,   396,   397,     0,
       0,   398,   399,   400,     0,     0,     0,   634,     0,     0,
       0,     0,    51,     0,   356,   357,   358,   359,   360,   361,
     362,     0,     0,     0,     0,   363,   364,   365,   366,   367,
     368,   369,   370,   371,     0,     0,     0,     0,     0,   372,
       0,    73,   543,    51,     0,     0,     0,     0,     0,     0,
       0,     0,   356,   357,   358,   359,   360,   361,   362,    80,
      81,    82,    83,   363,   436,   365,   366,   367,   368,   437,
     370,   371,    73,     0,     0,   614,     0,   372,    87,    88,
       0,    89,     0,   356,   357,   358,   359,   360,   361,   362,
      80,    81,    82,    83,   363,   436,   365,   366,   367,   368,
     437,   370,   371,     0,     0,     0,   616,     0,   372,    87,
      88,     0,    89,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,   356,   357,   358,   359,
     360,   361,   362,     0,     0,     0,     0,   363,   364,   365,
     366,   367,   368,   369,   370,   371,     0,     0,     0,   374,
       0,   372,     0,     0,    51,   356,   357,   358,   359,   360,
     361,   362,     0,     0,     0,     0,   363,   364,   365,   366,
     367,   368,   369,   370,   371,     0,     0,     0,   432,     0,
     372,     0,     0,    73,     0,    51,     0,     0,     0,     0,
       0,     0,     0,     0,   356,   357,   358,   359,   360,   361,
     362,    80,    81,    82,    83,   363,   436,   365,   366,   367,
     368,   437,   370,   371,    73,     0,     0,   618,     0,   372,
      87,    88,     0,    89,     0,   356,   357,   358,   359,   360,
     361,   362,    80,    81,    82,    83,   363,   436,   365,   366,
     367,   368,   437,   370,   371,     0,     0,     0,     0,     0,
     372,    87,    88,     0,    89,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   238,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,     0,     0,     0,     0,    51,     0,     0,     0,
       0,     0,   224,   239,   240,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,     0,     0,     0,
      51,     0,     0,     0,     0,    73,     0,     0,     0,     0,
       0,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    80,    81,    82,    83,     0,    84,    73,
       0,    51,     0,    85,     0,     0,     0,     0,     0,   241,
       0,   200,    87,    88,     0,    89,     0,    80,    81,    82,
      83,     0,    84,   201,     0,     0,    51,    85,     0,     0,
      73,   202,     0,   225,     0,     0,    87,    88,     0,    89,
     203,   204,   205,   206,   207,   208,     0,   209,    80,    81,
      82,    83,    51,    84,     0,    73,     0,     0,   194,     0,
       0,     0,     0,     0,   195,     0,     0,    87,    88,     0,
      89,     0,     0,    80,    81,    82,    83,     0,    84,     0,
       0,    73,     0,    85,     0,     0,     0,     0,     0,   268,
       0,     0,    87,    88,   210,    89,     0,     0,     0,    80,
      81,    82,    83,     0,    84,     0,     0,     0,   211,    85,
       0,     0,     0,     0,     0,   430,     0,     0,    87,    88,
       0,    89,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,     0,     0,     0,     0,     0,
       0,     0,     0,    51,     0,     0,     0,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,     0,     0,     0,     0,     0,     0,    51,     0,     0,
       0,     0,    73,     0,   173,   174,   175,   176,   177,     6,
       7,     8,     9,    10,    11,   178,     0,     0,     0,     0,
      80,    81,    82,    83,    51,    84,    73,     0,     0,     0,
      85,     0,     0,     0,     0,     0,   447,     0,     0,    87,
      88,     0,    89,     0,    80,    81,    82,    83,    51,    84,
       0,     0,     0,    73,    85,     0,     0,     0,     0,     0,
     476,     0,     0,    87,    88,     0,    89,     0,     0,     0,
       0,    80,    81,    82,    83,   179,    84,    73,     0,     0,
       0,    85,     0,     0,     0,     0,     0,   622,     0,     0,
      87,    88,     0,    89,     0,    80,    81,    82,    83,     0,
      84,     0,     0,     0,   180,    85,     0,     0,     0,     0,
       0,     0,     0,     0,    87,    88,     0,    89,     0,     0,
       0,     0,   181,   182,   183,   184,     0,   185,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   186,     0,
       0,   187,   188,   173,   174,   175,   176,   177,     6,     7,
       8,     9,    10,    11,   178,     0,     0,     0,     0,     0,
       0,     0,     0,   286,     0,   287,   288,   289,     0,   290,
     291,   292,   293,   294,   295,   296,   297,   298,    27,    28,
       0,   299,   300,   301,   302,   303,     0,   304,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   305,   306,   307,
     308,     0,     0,   309,   310,   311,   312,   313,     0,     0,
       0,     0,     0,     0,   179,     0,     0,   314,   315,     0,
       0,     0,   316,   317,   318,   319,     0,     0,   320,   321,
     322,   323,   324,   325,   326,   327,   328,     0,     0,   329,
     330,   331,   332,   180,   333,     0,     0,   334,     0,   335,
     336,     0,   337,   338,   339,   340,   341,     0,     0,     0,
       0,   181,   182,   183,   184,     0,   185,     0,     0,     0,
       0,     0,   356,   357,   358,   359,   360,   361,   362,     0,
     187,   188,   342,   363,   364,   365,   366,   367,   368,   369,
     370,   371,     0,     0,     0,   446,     0,   372,   356,   357,
     358,   359,   360,   361,   362,     0,     0,     0,     0,   363,
     364,   365,   366,   367,   368,   369,   370,   371,     0,     0,
       0,   456,     0,   372,   356,   357,   358,   359,   360,   361,
     362,     0,     0,     0,     0,   363,   364,   365,   366,   367,
     368,   369,   370,   371,     0,     0,     0,   461,     0,   372,
     356,   357,   358,   359,   360,   361,   362,     0,     0,     0,
       0,   363,   364,   365,   366,   367,   368,   369,   370,   371,
       0,     0,     0,   462,     0,   372,   356,   357,   358,   359,
     360,   361,   362,     0,     0,     0,     0,   363,   364,   365,
     366,   367,   368,   369,   370,   371,     0,     0,     0,   470,
       0,   372,   356,   357,   358,   359,   360,   361,   362,     0,
       0,     0,     0,   363,   364,   365,   366,   367,   368,   369,
     370,   371,     0,     0,     0,   482,     0,   372,   356,   357,
     358,   359,   360,   361,   362,     0,     0,     0,     0,   363,
     364,   365,   366,   367,   368,   369,   370,   371,     0,     0,
       0,   483,     0,   372,   356,   357,   358,   359,   360,   361,
     362,     0,     0,     0,     0,   363,   364,   365,   366,   367,
     368,   369,   370,   371,     0,     0,     0,   541,     0,   372,
     356,   357,   358,   359,   360,   361,   362,     0,     0,     0,
       0,   363,   364,   365,   366,   367,   368,   369,   370,   371,
       0,     0,     0,   561,     0,   372,   356,   357,   358,   359,
     360,   361,   362,     0,     0,     0,     0,   363,   364,   365,
     366,   367,   368,   369,   370,   371,     0,     0,     0,   600,
       0,   372,   356,   357,   358,   359,   360,   361,   362,     0,
       0,     0,     0,   363,   364,   365,   366,   367,   368,   369,
     370,   371,     0,     0,     0,   603,     0,   372,   356,   357,
     358,   359,   360,   361,   362,     0,     0,     0,     0,   363,
     364,   365,   366,   367,   368,   369,   370,   371,     0,     0,
       0,   607,     0,   372,   356,   357,   358,   359,   360,   361,
     362,     0,     0,     0,     0,   363,   364,   365,   366,   367,
     368,   369,   370,   371,     0,     0,     0,   612,     0,   372,
     356,   357,   358,   359,   360,   361,   362,     0,     0,     0,
       0,   363,   364,   365,   366,   367,   368,   369,   370,   371,
       0,     0,     0,   613,     0,   372,   356,   357,   358,   359,
     360,   361,   362,     0,     0,     0,     0,   363,   364,   365,
     366,   367,   368,   369,   370,   371,     0,     0,     0,   621,
       0,   372,   356,   357,   358,   359,   360,   361,   362,     0,
       0,     0,     0,   363,   364,   365,   366,   367,   368,   369,
     370,   371,     0,     0,     0,   624,     0,   372,   356,   357,
     358,   359,   360,   361,   362,     0,     0,     0,     0,   363,
     364,   365,   366,   367,   368,   369,   370,   371,     0,     0,
       0,   625,     0,   372,   356,   357,   358,   359,   360,   361,
     362,     0,     0,     0,     0,   363,   364,   365,   366,   367,
     368,   369,   370,   371,     0,     0,     0,   626,     0,   372,
     356,   357,   358,   359,   360,   361,   362,     0,     0,     0,
       0,   363,   364,   365,   366,   367,   368,   369,   370,   371,
       0,     0,     0,   627,     0,   372,   356,   357,   358,   359,
     360,   361,   362,     0,     0,     0,     0,   363,   364,   365,
     366,   367,   368,   369,   370,   371,     0,     0,     0,   628,
       0,   372,   356,   357,   358,   359,   360,   361,   362,     0,
       0,     0,     0,   363,   364,   365,   366,   367,   368,   369,
     370,   371,     0,     0,     0,   629,     0,   372,   356,   357,
     358,   359,   360,   361,   362,     0,     0,     0,     0,   363,
     364,   365,   366,   367,   368,   369,   370,   371,     0,     0,
       0,   630,     0,   372,   356,   357,   358,   359,   360,   361,
     362,     0,     0,     0,     0,   363,   364,   365,   366,   367,
     368,   369,   370,   371,     0,     0,     0,   631,     0,   372,
     356,   357,   358,   359,   360,   361,   362,     0,     0,     0,
       0,   363,   364,   365,   366,   367,   368,   369,   370,   371,
       0,     0,     0,   632,     0,   372,   356,   357,   358,   359,
     360,   361,   362,     0,     0,     0,     0,   363,   364,   365,
     366,   367,   368,   369,   370,   371,     0,     0,     0,   645,
       0,   372,   356,   357,   358,   359,   360,   361,   362,     0,
       0,     0,     0,   363,   364,   365,   366,   367,   368,   369,
     370,   371,     0,     0,     0,   646,     0,   372,   356,   357,
     358,   359,   360,   361,   362,     0,     0,     0,     0,   363,
     364,   365,   366,   367,   368,   369,   370,   371,     0,     0,
       0,   647,     0,   372,   356,   357,   358,   359,   360,   361,
     362,     0,     0,     0,     0,   363,   364,   365,   366,   367,
     368,   369,   370,   371,     0,     0,     0,   649,     0,   372,
     356,   357,   358,   359,   360,   361,   362,     0,     0,     0,
       0,   363,   364,   365,   366,   367,   368,   369,   370,   371,
       0,     0,     0,   650,     0,   372,   356,   357,   358,   359,
     360,   361,   362,     0,     0,     0,     0,   363,   364,   365,
     366,   367,   368,   369,   370,   371,     0,     0,     0,   651,
       0,   372,   356,   357,   358,   359,   360,   361,   362,     0,
       0,     0,     0,   363,   364,   365,   366,   367,   368,   369,
     370,   371,     0,     0,     0,   652,     0,   372,   356,   357,
     358,   359,   360,   361,   362,     0,     0,     0,     0,   363,
     364,   365,   366,   367,   368,   369,   370,   371,   391,   392,
     393,     0,     0,   372,     0,     0,     0,     0,     0,   394,
     395,   396,   397,     0,     0,   398,   399,   400,     0,     0,
       0,     0,     0,   401
};

static const yytype_int16 yycheck[] =
{
       0,   185,    19,   187,   188,    19,    19,   204,   205,   206,
      19,    50,    74,   137,    14,    17,    74,    19,    55,    56,
      20,    21,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    17,   137,    74,   103,    74,    75,
      40,   137,   106,   103,    44,    45,    74,    74,    48,    49,
     103,   113,    52,    53,    74,    16,    17,   103,    58,    59,
     103,   103,   103,   103,   103,    65,    74,    50,   137,    74,
     137,   137,    72,   137,    74,   137,    76,   113,   137,   137,
     100,   137,   137,   137,    84,    85,   137,    87,    88,    89,
      90,    74,   103,    74,   137,   137,   137,    74,   137,   137,
     114,   115,   116,   117,   118,   119,   120,   137,   137,   137,
     137,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     137,    74,   103,   137,   137,   139,   139,    74,   137,   137,
     113,   168,   137,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,    65,   137,   155,   137,   137,   139,   140,
     141,   137,   143,   114,   115,   116,   137,   137,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     131,   132,   133,   185,    74,   187,   188,    67,    68,    69,
      70,   137,   137,   137,   194,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,   103,   391,   392,   393,
     394,   395,   396,   397,   398,   399,   400,   401,   415,   138,
     417,   418,   419,   138,   224,   138,    74,   138,   228,   229,
     230,   231,   114,   115,   116,   138,   138,   138,   238,    74,
     138,   137,   242,   125,   126,   127,   128,   247,   137,   131,
     132,   133,   138,   137,    18,    63,   117,   118,   119,   120,
     142,   137,   137,   263,    72,   265,   266,   137,   103,    33,
     137,   137,   137,   137,   137,   275,   138,   137,   139,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   137,
     137,   137,   137,   137,   139,   140,   141,   137,   143,   137,
      19,    75,   137,   137,   122,   123,   137,   137,   137,   137,
     137,   137,   137,   117,   118,   119,   120,   137,    92,    93,
     137,    74,   137,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,   139,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   356,   357,   358,   359,
     360,   361,   362,   363,   364,   365,   366,   367,   368,   369,
     370,   371,   372,   137,   137,   137,   137,   137,   378,   379,
     380,   381,   382,   383,   137,   385,   386,   387,   137,   391,
     392,   393,   394,   395,   396,   397,   398,   399,   400,   401,
     137,   137,   137,   137,    74,   114,   115,   116,   117,   118,
     119,   120,   137,   137,   137,   137,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   137,   137,   137,   137,   137,
     139,   137,   137,   103,   434,   137,   436,   437,   438,   137,
     114,   115,   116,   117,   118,   119,   120,   137,   448,   137,
     137,   121,   122,   123,   124,   137,   126,   131,   132,   133,
     137,   131,   137,   139,   137,   139,   466,   137,   468,   469,
     140,   141,   137,   143,   137,   137,     0,   137,   478,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,   137,   137,   137,    18,    19,    20,    21,    22,   137,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,   137,    42,    43,
     137,   137,   137,   137,   137,   137,   137,   137,    52,    53,
      54,    55,   137,   137,    58,    59,    60,    61,    62,   137,
      64,   137,    66,   137,   137,   137,    74,    71,    72,    73,
      74,    74,    76,    77,    78,    79,    80,    74,   137,    83,
      84,    85,    86,    87,    88,    89,    90,    91,   137,   137,
      94,    95,    96,    97,   103,    99,   137,   101,   102,   103,
     104,   105,   137,   107,   108,   109,   110,   114,   115,   116,
     117,   118,   119,   120,   137,    74,   137,   121,   122,   123,
     124,   137,   126,   137,   131,   132,   133,   131,    74,    90,
      75,   137,   139,   137,    -1,   137,   140,   141,   137,   143,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,   137,   137,   137,    18,    19,    20,    21,    22,
     137,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    42,
      43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,
      53,    54,    55,    -1,    -1,    58,    59,    60,    61,    62,
      -1,    64,    -1,    66,    -1,    -1,    -1,    -1,    71,    72,
      73,    74,    -1,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    19,
      -1,    94,    95,    96,    97,    -1,    99,    -1,   101,   102,
     103,   104,   105,    -1,   107,   108,   109,   110,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   121,   122,
     123,   124,    -1,   126,    -1,    -1,    -1,    -1,   131,    -1,
      -1,    -1,    -1,    -1,   137,    -1,    -1,   140,   141,    -1,
     143,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,   114,   115,   116,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   125,   126,   127,   128,    -1,
      -1,   131,   132,   133,    -1,    -1,    -1,   137,    -1,    -1,
      -1,    -1,    74,    -1,   114,   115,   116,   117,   118,   119,
     120,    -1,    -1,    -1,    -1,   125,   126,   127,   128,   129,
     130,   131,   132,   133,    -1,    -1,    -1,    -1,    -1,   139,
      -1,   103,   142,    74,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   103,    -1,    -1,   137,    -1,   139,   140,   141,
      -1,   143,    -1,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,    -1,    -1,    -1,   137,    -1,   139,   140,
     141,    -1,   143,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,   114,   115,   116,   117,
     118,   119,   120,    -1,    -1,    -1,    -1,   125,   126,   127,
     128,   129,   130,   131,   132,   133,    -1,    -1,    -1,   137,
      -1,   139,    -1,    -1,    74,   114,   115,   116,   117,   118,
     119,   120,    -1,    -1,    -1,    -1,   125,   126,   127,   128,
     129,   130,   131,   132,   133,    -1,    -1,    -1,   137,    -1,
     139,    -1,    -1,   103,    -1,    74,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   103,    -1,    -1,   137,    -1,   139,
     140,   141,    -1,   143,    -1,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,    -1,    -1,    -1,    -1,    -1,
     139,   140,   141,    -1,   143,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    57,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    74,    -1,    -1,    -1,
      -1,    -1,    56,    81,    82,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      74,    -1,    -1,    -1,    -1,   103,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,   121,   122,   123,   124,    -1,   126,   103,
      -1,    74,    -1,   131,    -1,    -1,    -1,    -1,    -1,   137,
      -1,    15,   140,   141,    -1,   143,    -1,   121,   122,   123,
     124,    -1,   126,    27,    -1,    -1,    74,   131,    -1,    -1,
     103,    35,    -1,   137,    -1,    -1,   140,   141,    -1,   143,
      44,    45,    46,    47,    48,    49,    -1,    51,   121,   122,
     123,   124,    74,   126,    -1,   103,    -1,    -1,   131,    -1,
      -1,    -1,    -1,    -1,   137,    -1,    -1,   140,   141,    -1,
     143,    -1,    -1,   121,   122,   123,   124,    -1,   126,    -1,
      -1,   103,    -1,   131,    -1,    -1,    -1,    -1,    -1,   137,
      -1,    -1,   140,   141,    98,   143,    -1,    -1,    -1,   121,
     122,   123,   124,    -1,   126,    -1,    -1,    -1,   112,   131,
      -1,    -1,    -1,    -1,    -1,   137,    -1,    -1,   140,   141,
      -1,   143,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    74,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    74,    -1,    -1,
      -1,    -1,   103,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
     121,   122,   123,   124,    74,   126,   103,    -1,    -1,    -1,
     131,    -1,    -1,    -1,    -1,    -1,   137,    -1,    -1,   140,
     141,    -1,   143,    -1,   121,   122,   123,   124,    74,   126,
      -1,    -1,    -1,   103,   131,    -1,    -1,    -1,    -1,    -1,
     137,    -1,    -1,   140,   141,    -1,   143,    -1,    -1,    -1,
      -1,   121,   122,   123,   124,    74,   126,   103,    -1,    -1,
      -1,   131,    -1,    -1,    -1,    -1,    -1,   137,    -1,    -1,
     140,   141,    -1,   143,    -1,   121,   122,   123,   124,    -1,
     126,    -1,    -1,    -1,   103,   131,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   140,   141,    -1,   143,    -1,    -1,
      -1,    -1,   121,   122,   123,   124,    -1,   126,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   137,    -1,
      -1,   140,   141,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    18,    -1,    20,    21,    22,    -1,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      -1,    36,    37,    38,    39,    40,    -1,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    53,    54,
      55,    -1,    -1,    58,    59,    60,    61,    62,    -1,    -1,
      -1,    -1,    -1,    -1,    74,    -1,    -1,    72,    73,    -1,
      -1,    -1,    77,    78,    79,    80,    -1,    -1,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    -1,    -1,    94,
      95,    96,    97,   103,    99,    -1,    -1,   102,    -1,   104,
     105,    -1,   107,   108,   109,   110,   111,    -1,    -1,    -1,
      -1,   121,   122,   123,   124,    -1,   126,    -1,    -1,    -1,
      -1,    -1,   114,   115,   116,   117,   118,   119,   120,    -1,
     140,   141,   137,   125,   126,   127,   128,   129,   130,   131,
     132,   133,    -1,    -1,    -1,   137,    -1,   139,   114,   115,
     116,   117,   118,   119,   120,    -1,    -1,    -1,    -1,   125,
     126,   127,   128,   129,   130,   131,   132,   133,    -1,    -1,
      -1,   137,    -1,   139,   114,   115,   116,   117,   118,   119,
     120,    -1,    -1,    -1,    -1,   125,   126,   127,   128,   129,
     130,   131,   132,   133,    -1,    -1,    -1,   137,    -1,   139,
     114,   115,   116,   117,   118,   119,   120,    -1,    -1,    -1,
      -1,   125,   126,   127,   128,   129,   130,   131,   132,   133,
      -1,    -1,    -1,   137,    -1,   139,   114,   115,   116,   117,
     118,   119,   120,    -1,    -1,    -1,    -1,   125,   126,   127,
     128,   129,   130,   131,   132,   133,    -1,    -1,    -1,   137,
      -1,   139,   114,   115,   116,   117,   118,   119,   120,    -1,
      -1,    -1,    -1,   125,   126,   127,   128,   129,   130,   131,
     132,   133,    -1,    -1,    -1,   137,    -1,   139,   114,   115,
     116,   117,   118,   119,   120,    -1,    -1,    -1,    -1,   125,
     126,   127,   128,   129,   130,   131,   132,   133,    -1,    -1,
      -1,   137,    -1,   139,   114,   115,   116,   117,   118,   119,
     120,    -1,    -1,    -1,    -1,   125,   126,   127,   128,   129,
     130,   131,   132,   133,    -1,    -1,    -1,   137,    -1,   139,
     114,   115,   116,   117,   118,   119,   120,    -1,    -1,    -1,
      -1,   125,   126,   127,   128,   129,   130,   131,   132,   133,
      -1,    -1,    -1,   137,    -1,   139,   114,   115,   116,   117,
     118,   119,   120,    -1,    -1,    -1,    -1,   125,   126,   127,
     128,   129,   130,   131,   132,   133,    -1,    -1,    -1,   137,
      -1,   139,   114,   115,   116,   117,   118,   119,   120,    -1,
      -1,    -1,    -1,   125,   126,   127,   128,   129,   130,   131,
     132,   133,    -1,    -1,    -1,   137,    -1,   139,   114,   115,
     116,   117,   118,   119,   120,    -1,    -1,    -1,    -1,   125,
     126,   127,   128,   129,   130,   131,   132,   133,    -1,    -1,
      -1,   137,    -1,   139,   114,   115,   116,   117,   118,   119,
     120,    -1,    -1,    -1,    -1,   125,   126,   127,   128,   129,
     130,   131,   132,   133,    -1,    -1,    -1,   137,    -1,   139,
     114,   115,   116,   117,   118,   119,   120,    -1,    -1,    -1,
      -1,   125,   126,   127,   128,   129,   130,   131,   132,   133,
      -1,    -1,    -1,   137,    -1,   139,   114,   115,   116,   117,
     118,   119,   120,    -1,    -1,    -1,    -1,   125,   126,   127,
     128,   129,   130,   131,   132,   133,    -1,    -1,    -1,   137,
      -1,   139,   114,   115,   116,   117,   118,   119,   120,    -1,
      -1,    -1,    -1,   125,   126,   127,   128,   129,   130,   131,
     132,   133,    -1,    -1,    -1,   137,    -1,   139,   114,   115,
     116,   117,   118,   119,   120,    -1,    -1,    -1,    -1,   125,
     126,   127,   128,   129,   130,   131,   132,   133,    -1,    -1,
      -1,   137,    -1,   139,   114,   115,   116,   117,   118,   119,
     120,    -1,    -1,    -1,    -1,   125,   126,   127,   128,   129,
     130,   131,   132,   133,    -1,    -1,    -1,   137,    -1,   139,
     114,   115,   116,   117,   118,   119,   120,    -1,    -1,    -1,
      -1,   125,   126,   127,   128,   129,   130,   131,   132,   133,
      -1,    -1,    -1,   137,    -1,   139,   114,   115,   116,   117,
     118,   119,   120,    -1,    -1,    -1,    -1,   125,   126,   127,
     128,   129,   130,   131,   132,   133,    -1,    -1,    -1,   137,
      -1,   139,   114,   115,   116,   117,   118,   119,   120,    -1,
      -1,    -1,    -1,   125,   126,   127,   128,   129,   130,   131,
     132,   133,    -1,    -1,    -1,   137,    -1,   139,   114,   115,
     116,   117,   118,   119,   120,    -1,    -1,    -1,    -1,   125,
     126,   127,   128,   129,   130,   131,   132,   133,    -1,    -1,
      -1,   137,    -1,   139,   114,   115,   116,   117,   118,   119,
     120,    -1,    -1,    -1,    -1,   125,   126,   127,   128,   129,
     130,   131,   132,   133,    -1,    -1,    -1,   137,    -1,   139,
     114,   115,   116,   117,   118,   119,   120,    -1,    -1,    -1,
      -1,   125,   126,   127,   128,   129,   130,   131,   132,   133,
      -1,    -1,    -1,   137,    -1,   139,   114,   115,   116,   117,
     118,   119,   120,    -1,    -1,    -1,    -1,   125,   126,   127,
     128,   129,   130,   131,   132,   133,    -1,    -1,    -1,   137,
      -1,   139,   114,   115,   116,   117,   118,   119,   120,    -1,
      -1,    -1,    -1,   125,   126,   127,   128,   129,   130,   131,
     132,   133,    -1,    -1,    -1,   137,    -1,   139,   114,   115,
     116,   117,   118,   119,   120,    -1,    -1,    -1,    -1,   125,
     126,   127,   128,   129,   130,   131,   132,   133,    -1,    -1,
      -1,   137,    -1,   139,   114,   115,   116,   117,   118,   119,
     120,    -1,    -1,    -1,    -1,   125,   126,   127,   128,   129,
     130,   131,   132,   133,    -1,    -1,    -1,   137,    -1,   139,
     114,   115,   116,   117,   118,   119,   120,    -1,    -1,    -1,
      -1,   125,   126,   127,   128,   129,   130,   131,   132,   133,
      -1,    -1,    -1,   137,    -1,   139,   114,   115,   116,   117,
     118,   119,   120,    -1,    -1,    -1,    -1,   125,   126,   127,
     128,   129,   130,   131,   132,   133,    -1,    -1,    -1,   137,
      -1,   139,   114,   115,   116,   117,   118,   119,   120,    -1,
      -1,    -1,    -1,   125,   126,   127,   128,   129,   130,   131,
     132,   133,    -1,    -1,    -1,   137,    -1,   139,   114,   115,
     116,   117,   118,   119,   120,    -1,    -1,    -1,    -1,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   114,   115,
     116,    -1,    -1,   139,    -1,    -1,    -1,    -1,    -1,   125,
     126,   127,   128,    -1,    -1,   131,   132,   133,    -1,    -1,
      -1,    -1,    -1,   139
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
      99,   101,   102,   103,   104,   105,   107,   108,   109,   110,
     121,   122,   123,   124,   126,   131,   137,   140,   141,   143,
     145,   146,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   212,    19,   137,   212,    50,   103,
     137,   137,     3,     4,     5,     6,     7,    63,    72,   122,
     123,   149,   137,     3,     4,     5,     6,     7,    14,    74,
     103,   121,   122,   123,   124,   126,   137,   140,   141,   149,
     211,    74,   137,   212,   131,   137,   212,   103,   103,   103,
      15,    27,    35,    44,    45,    46,    47,    48,    49,    51,
      98,   112,   137,   103,   137,   137,   137,   137,   137,   137,
     103,   137,   137,   137,    56,   137,   212,    74,    74,    74,
     212,   212,    65,    67,    68,    69,    70,   212,    57,    81,
      82,   137,   212,    74,    75,   113,   212,   212,    16,    17,
     147,   147,   147,   137,   212,   212,   137,   137,   103,   137,
     103,   137,    18,    33,    75,    92,    93,   137,   137,   212,
      17,    50,    74,   113,   137,    74,   100,    74,   137,    74,
     137,    74,   137,   137,   212,   212,    18,    20,    21,    22,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    36,
      37,    38,    39,    40,    42,    52,    53,    54,    55,    58,
      59,    60,    61,    62,    72,    73,    77,    78,    79,    80,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    94,
      95,    96,    97,    99,   102,   104,   105,   107,   108,   109,
     110,   111,   137,   148,   212,    74,   137,   137,   212,   212,
     212,   212,   212,     0,   146,   137,   114,   115,   116,   117,
     118,   119,   120,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   139,   212,   137,   103,   103,   137,   138,   138,
     138,   138,   138,   138,   147,   138,   138,   138,   211,   211,
     211,   114,   115,   116,   125,   126,   127,   128,   131,   132,
     133,   139,    19,   137,    19,   137,   212,    19,   137,   137,
     137,   137,   137,   137,   137,   177,   212,   177,   177,   177,
     137,   137,   137,    74,   137,    74,   113,   137,   137,   137,
     137,   212,   137,    74,   212,   212,   126,   131,   212,   212,
     137,   103,   137,   137,   137,   137,   137,   137,   212,   137,
     138,   137,   212,   137,   137,   137,   137,   212,   137,   137,
     137,   137,   137,   137,   137,   137,   212,   137,   212,   212,
     137,   137,   137,   106,   137,   137,   137,   212,    74,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   142,   212,   212,   212,   212,   212,   212,
     212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   137,   137,   137,   212,   212,   212,   212,   212,   212,
     137,   212,   212,   212,   142,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,    74,    74,    19,   137,
      74,   177,   177,   177,   177,   137,    74,   137,    74,   137,
     137,   137,   212,   137,   212,   212,   212,   137,   137,   137,
     212,   103,   137,   137,   137,   212,   137,   212,   137,   212,
     137,   137,   137,   212,   137,   137,   137,   137,   137,   137,
     137,   137,   137,    19,   137,   137,   137,    74,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,    74,   137,   137
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   144,   145,   145,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   147,   147,   148,
     148,   149,   149,   149,   149,   149,   149,   150,   150,   151,
     152,   153,   153,   153,   153,   153,   153,   154,   155,   156,
     157,   158,   159,   160,   160,   161,   161,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   163,   163,
     163,   163,   164,   164,   165,   165,   166,   167,   168,   168,
     169,   169,   169,   169,   170,   171,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   174,
     175,   175,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   177,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   193,   193,   193,
     194,   195,   196,   197,   198,   199,   200,   200,   200,   200,
     200,   201,   201,   201,   201,   201,   201,   201,   201,   202,
     202,   202,   203,   204,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   206,   207,   208,   209,   210,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   212,   212,   212,   212,   212,   212,   212,
     212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   212,   212,   212,   212,   212,   212,   212
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
       1,     1,     1,     1,     1,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     2,
       2,     3,     3,     3,     3,     4,     2,     3,     3,     2,
       3,     3,     3,     2,     3,     2,     3,     3,     3,     2,
       4,     4,     4,     5,     5,     5,     2,     3,     3,     4,
       4,     5,     2,     3,     2,     3,     2,     3,     2,     4,
       2,     3,     4,     4,     2,     3,     4,     5,     5,     5,
       5,     5,     5,     5,     5,     5,     2,     5,     7,     2,
       3,     5,     3,     5,     2,     3,     5,     4,     6,     2,
       2,     3,     3,     3,     5,     5,     5,     5,     3,     3,
       3,     3,     3,     4,     3,     4,     5,     4,     5,     0,
       1,     2,     2,     2,     2,     2,     2,     2,     3,     2,
       2,     2,     3,     3,     3,     2,     4,     3,     3,     2,
       4,     5,     4,     5,     4,     3,     3,     4,     3,     3,
       3,     2,     3,     4,     3,     4,     5,     3,     5,     3,
       3,     3,     3,     4,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     3,     3,     2,     2,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     2,     2,     3
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
  case 64: /* command: expression  */
#line 225 "parser.y"
                 { eval_value = (yyvsp[0].uval); }
#line 2468 "y.tab.c"
    break;

  case 66: /* command: '\n'  */
#line 228 "parser.y"
      {
      }
#line 2475 "y.tab.c"
    break;

  case 68: /* BX_TOKEN_TOGGLE_ON_OFF: BX_TOKEN_OFF  */
#line 235 "parser.y"
    { (yyval.bval)=(yyvsp[0].bval); }
#line 2481 "y.tab.c"
    break;

  case 70: /* BX_TOKEN_REGISTERS: BX_TOKEN_REGS  */
#line 241 "parser.y"
    { (yyval.sval)=(yyvsp[0].sval); }
#line 2487 "y.tab.c"
    break;

  case 76: /* BX_TOKEN_SEGREG: BX_TOKEN_GS  */
#line 251 "parser.y"
    { (yyval.uval)=(yyvsp[0].uval); }
#line 2493 "y.tab.c"
    break;

  case 77: /* timebp_command: BX_TOKEN_TIMEBP expression '\n'  */
#line 256 "parser.y"
      {
          bx_dbg_timebp_command(0, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2502 "y.tab.c"
    break;

  case 78: /* timebp_command: BX_TOKEN_TIMEBP_ABSOLUTE expression '\n'  */
#line 261 "parser.y"
      {
          bx_dbg_timebp_command(1, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2511 "y.tab.c"
    break;

  case 79: /* modebp_command: BX_TOKEN_MODEBP '\n'  */
#line 269 "parser.y"
      {
          bx_dbg_modebp_command();
          free((yyvsp[-1].sval));
      }
#line 2520 "y.tab.c"
    break;

  case 80: /* vmexitbp_command: BX_TOKEN_VMEXITBP '\n'  */
#line 277 "parser.y"
      {
          bx_dbg_vmexitbp_command();
          free((yyvsp[-1].sval));
      }
#line 2529 "y.tab.c"
    break;

  case 81: /* show_command: BX_TOKEN_SHOW BX_TOKEN_GENERIC '\n'  */
#line 285 "parser.y"
      {
          bx_dbg_show_command((yyvsp[-1].sval));
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2538 "y.tab.c"
    break;

  case 82: /* show_command: BX_TOKEN_SHOW BX_TOKEN_ALL '\n'  */
#line 290 "parser.y"
      {
          bx_dbg_show_command("all");
          free((yyvsp[-2].sval));
      }
#line 2547 "y.tab.c"
    break;

  case 83: /* show_command: BX_TOKEN_SHOW BX_TOKEN_OFF '\n'  */
#line 295 "parser.y"
      {
          bx_dbg_show_command("off");
          free((yyvsp[-2].sval));
      }
#line 2556 "y.tab.c"
    break;

  case 84: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING '\n'  */
#line 300 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-1].sval), 0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2565 "y.tab.c"
    break;

  case 85: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING BX_TOKEN_XML '\n'  */
#line 305 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-2].sval), 1);
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2574 "y.tab.c"
    break;

  case 86: /* show_command: BX_TOKEN_SHOW '\n'  */
#line 310 "parser.y"
      {
          bx_dbg_show_command(0);
          free((yyvsp[-1].sval));
      }
#line 2583 "y.tab.c"
    break;

  case 87: /* page_command: BX_TOKEN_PAGE expression '\n'  */
#line 318 "parser.y"
      {
          bx_dbg_xlate_address((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2592 "y.tab.c"
    break;

  case 88: /* tlb_command: BX_TOKEN_TLB expression '\n'  */
#line 326 "parser.y"
      {
          bx_dbg_tlb_lookup((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2601 "y.tab.c"
    break;

  case 89: /* ptime_command: BX_TOKEN_PTIME '\n'  */
#line 334 "parser.y"
      {
          bx_dbg_ptime_command();
          free((yyvsp[-1].sval));
      }
#line 2610 "y.tab.c"
    break;

  case 90: /* trace_command: BX_TOKEN_TRACE BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 342 "parser.y"
      {
          bx_dbg_trace_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2619 "y.tab.c"
    break;

  case 91: /* trace_reg_command: BX_TOKEN_TRACEREG BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 350 "parser.y"
      {
          bx_dbg_trace_reg_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2628 "y.tab.c"
    break;

  case 92: /* trace_mem_command: BX_TOKEN_TRACEMEM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 358 "parser.y"
      {
          bx_dbg_trace_mem_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2637 "y.tab.c"
    break;

  case 93: /* print_stack_command: BX_TOKEN_PRINT_STACK '\n'  */
#line 366 "parser.y"
      {
          bx_dbg_print_stack_command(16);
          free((yyvsp[-1].sval));
      }
#line 2646 "y.tab.c"
    break;

  case 94: /* print_stack_command: BX_TOKEN_PRINT_STACK BX_TOKEN_NUMERIC '\n'  */
#line 371 "parser.y"
      {
          bx_dbg_print_stack_command((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2655 "y.tab.c"
    break;

  case 95: /* backtrace_command: BX_TOKEN_BT '\n'  */
#line 379 "parser.y"
      {
        bx_dbg_bt_command(16);
        free((yyvsp[-1].sval));
      }
#line 2664 "y.tab.c"
    break;

  case 96: /* backtrace_command: BX_TOKEN_BT BX_TOKEN_NUMERIC '\n'  */
#line 384 "parser.y"
      {
        bx_dbg_bt_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2673 "y.tab.c"
    break;

  case 97: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_STOP '\n'  */
#line 392 "parser.y"
      {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2682 "y.tab.c"
    break;

  case 98: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_CONTINUE '\n'  */
#line 397 "parser.y"
      {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2691 "y.tab.c"
    break;

  case 99: /* watch_point_command: BX_TOKEN_WATCH '\n'  */
#line 402 "parser.y"
      {
          bx_dbg_print_watchpoints();
          free((yyvsp[-1].sval));
      }
#line 2700 "y.tab.c"
    break;

  case 100: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression '\n'  */
#line 407 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2709 "y.tab.c"
    break;

  case 101: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression '\n'  */
#line 412 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2718 "y.tab.c"
    break;

  case 102: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression '\n'  */
#line 417 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-1].uval), 1); /* BX_WRITE */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2727 "y.tab.c"
    break;

  case 103: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression expression '\n'  */
#line 422 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2736 "y.tab.c"
    break;

  case 104: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression expression '\n'  */
#line 427 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2745 "y.tab.c"
    break;

  case 105: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression expression '\n'  */
#line 432 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_WRITE */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2754 "y.tab.c"
    break;

  case 106: /* watch_point_command: BX_TOKEN_UNWATCH '\n'  */
#line 437 "parser.y"
      {
          bx_dbg_unwatch_all();
          free((yyvsp[-1].sval));
      }
#line 2763 "y.tab.c"
    break;

  case 107: /* watch_point_command: BX_TOKEN_UNWATCH expression '\n'  */
#line 442 "parser.y"
      {
          bx_dbg_unwatch((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2772 "y.tab.c"
    break;

  case 108: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 450 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2781 "y.tab.c"
    break;

  case 109: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING expression '\n'  */
#line 455 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 0, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2790 "y.tab.c"
    break;

  case 110: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING '\n'  */
#line 460 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 1, 0);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2799 "y.tab.c"
    break;

  case 111: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING expression '\n'  */
#line 465 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 1, (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2808 "y.tab.c"
    break;

  case 112: /* set_magic_break_points_command: BX_TOKEN_SET_MAGIC_BREAK_POINTS '\n'  */
#line 473 "parser.y"
      {
        bx_dbg_set_magic_bp_mask(0);
        free((yyvsp[-1].sval));
      }
#line 2817 "y.tab.c"
    break;

  case 113: /* set_magic_break_points_command: BX_TOKEN_SET_MAGIC_BREAK_POINTS BX_TOKEN_STRING '\n'  */
#line 478 "parser.y"
      {
        bx_dbg_set_magic_bp_mask(bx_dbg_get_magic_bp_mask_from_str((yyvsp[-1].sval)));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2826 "y.tab.c"
    break;

  case 114: /* clr_magic_break_points_command: BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS '\n'  */
#line 486 "parser.y"
      {
        bx_dbg_set_magic_bp_mask(0);
        free((yyvsp[-1].sval));
      }
#line 2835 "y.tab.c"
    break;

  case 115: /* clr_magic_break_points_command: BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS BX_TOKEN_STRING '\n'  */
#line 491 "parser.y"
      {
        bx_dbg_clr_magic_bp_mask(bx_dbg_get_magic_bp_mask_from_str((yyvsp[-1].sval)));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2844 "y.tab.c"
    break;

  case 116: /* where_command: BX_TOKEN_WHERE '\n'  */
#line 499 "parser.y"
      {
        bx_dbg_where_command();
        free((yyvsp[-1].sval));
      }
#line 2853 "y.tab.c"
    break;

  case 117: /* print_string_command: BX_TOKEN_PRINT_STRING expression '\n'  */
#line 507 "parser.y"
      {
        bx_dbg_print_string_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2862 "y.tab.c"
    break;

  case 118: /* continue_command: BX_TOKEN_CONTINUE '\n'  */
#line 515 "parser.y"
      {
        bx_dbg_continue_command(1);
        free((yyvsp[-1].sval));
      }
#line 2871 "y.tab.c"
    break;

  case 119: /* continue_command: BX_TOKEN_CONTINUE BX_TOKEN_IF expression '\n'  */
#line 520 "parser.y"
      {
        bx_dbg_continue_command((yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2880 "y.tab.c"
    break;

  case 120: /* stepN_command: BX_TOKEN_STEPN '\n'  */
#line 528 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[-1].sval));
      }
#line 2889 "y.tab.c"
    break;

  case 121: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC '\n'  */
#line 533 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2898 "y.tab.c"
    break;

  case 122: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_ALL BX_TOKEN_NUMERIC '\n'  */
#line 538 "parser.y"
      {
        bx_dbg_stepN_command(-1, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2907 "y.tab.c"
    break;

  case 123: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'  */
#line 543 "parser.y"
      {
        bx_dbg_stepN_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2916 "y.tab.c"
    break;

  case 124: /* step_over_command: BX_TOKEN_STEP_OVER '\n'  */
#line 551 "parser.y"
      {
        bx_dbg_step_over_command();
        free((yyvsp[-1].sval));
      }
#line 2925 "y.tab.c"
    break;

  case 125: /* cpu_command: BX_TOKEN_CPU BX_TOKEN_NUMERIC '\n'  */
#line 559 "parser.y"
      {
        bx_dbg_set_symbol_command("$cpu", (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2934 "y.tab.c"
    break;

  case 126: /* set_command: BX_TOKEN_SET BX_TOKEN_DISASM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 566 "parser.y"
      {
        bx_dbg_set_auto_disassemble((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2943 "y.tab.c"
    break;

  case 127: /* set_command: BX_TOKEN_SET BX_TOKEN_SYMBOLNAME '=' expression '\n'  */
#line 571 "parser.y"
      {
        bx_dbg_set_symbol_command((yyvsp[-3].sval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2952 "y.tab.c"
    break;

  case 128: /* set_command: BX_TOKEN_SET BX_TOKEN_8BL_REG '=' expression '\n'  */
#line 576 "parser.y"
      {
        bx_dbg_set_reg8l_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2960 "y.tab.c"
    break;

  case 129: /* set_command: BX_TOKEN_SET BX_TOKEN_8BH_REG '=' expression '\n'  */
#line 580 "parser.y"
      {
        bx_dbg_set_reg8h_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2968 "y.tab.c"
    break;

  case 130: /* set_command: BX_TOKEN_SET BX_TOKEN_16B_REG '=' expression '\n'  */
#line 584 "parser.y"
      {
        bx_dbg_set_reg16_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2976 "y.tab.c"
    break;

  case 131: /* set_command: BX_TOKEN_SET BX_TOKEN_32B_REG '=' expression '\n'  */
#line 588 "parser.y"
      {
        bx_dbg_set_reg32_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2984 "y.tab.c"
    break;

  case 132: /* set_command: BX_TOKEN_SET BX_TOKEN_64B_REG '=' expression '\n'  */
#line 592 "parser.y"
      {
        bx_dbg_set_reg64_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2992 "y.tab.c"
    break;

  case 133: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_EIP '=' expression '\n'  */
#line 596 "parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 3000 "y.tab.c"
    break;

  case 134: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_RIP '=' expression '\n'  */
#line 600 "parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 3008 "y.tab.c"
    break;

  case 135: /* set_command: BX_TOKEN_SET BX_TOKEN_SEGREG '=' expression '\n'  */
#line 604 "parser.y"
      {
        bx_dbg_load_segreg((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 3016 "y.tab.c"
    break;

  case 136: /* breakpoint_command: BX_TOKEN_VBREAKPOINT '\n'  */
#line 611 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3025 "y.tab.c"
    break;

  case 137: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression '\n'  */
#line 616 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].uval), NULL);
        free((yyvsp[-4].sval));
      }
#line 3034 "y.tab.c"
    break;

  case 138: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 621 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-5].uval), (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-6].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3043 "y.tab.c"
    break;

  case 139: /* breakpoint_command: BX_TOKEN_LBREAKPOINT '\n'  */
#line 626 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3052 "y.tab.c"
    break;

  case 140: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression '\n'  */
#line 631 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 3061 "y.tab.c"
    break;

  case 141: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 636 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3070 "y.tab.c"
    break;

  case 142: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING '\n'  */
#line 641 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-1].sval), NULL);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3079 "y.tab.c"
    break;

  case 143: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 646 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-3].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3088 "y.tab.c"
    break;

  case 144: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '\n'  */
#line 651 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3097 "y.tab.c"
    break;

  case 145: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression '\n'  */
#line 656 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 3106 "y.tab.c"
    break;

  case 146: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 661 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3115 "y.tab.c"
    break;

  case 147: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression '\n'  */
#line 666 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-3].sval));
      }
#line 3124 "y.tab.c"
    break;

  case 148: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 671 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-5].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3133 "y.tab.c"
    break;

  case 149: /* blist_command: BX_TOKEN_LIST_BREAK '\n'  */
#line 679 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-1].sval));
      }
#line 3142 "y.tab.c"
    break;

  case 150: /* slist_command: BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 687 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-1].sval));
      }
#line 3151 "y.tab.c"
    break;

  case 151: /* slist_command: BX_TOKEN_LIST_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 692 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 3160 "y.tab.c"
    break;

  case 152: /* info_command: BX_TOKEN_INFO BX_TOKEN_PBREAKPOINT '\n'  */
#line 700 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3169 "y.tab.c"
    break;

  case 153: /* info_command: BX_TOKEN_INFO BX_TOKEN_CPU '\n'  */
#line 705 "parser.y"
      {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3178 "y.tab.c"
    break;

  case 154: /* info_command: BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric optional_numeric '\n'  */
#line 710 "parser.y"
      {
        bx_dbg_info_idt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3187 "y.tab.c"
    break;

  case 155: /* info_command: BX_TOKEN_INFO BX_TOKEN_IVT optional_numeric optional_numeric '\n'  */
#line 715 "parser.y"
      {
        bx_dbg_info_ivt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3196 "y.tab.c"
    break;

  case 156: /* info_command: BX_TOKEN_INFO BX_TOKEN_GDT optional_numeric optional_numeric '\n'  */
#line 720 "parser.y"
      {
        bx_dbg_info_gdt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3205 "y.tab.c"
    break;

  case 157: /* info_command: BX_TOKEN_INFO BX_TOKEN_LDT optional_numeric optional_numeric '\n'  */
#line 725 "parser.y"
      {
        bx_dbg_info_ldt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3214 "y.tab.c"
    break;

  case 158: /* info_command: BX_TOKEN_INFO BX_TOKEN_TAB '\n'  */
#line 730 "parser.y"
      {
        bx_dbg_dump_table();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3223 "y.tab.c"
    break;

  case 159: /* info_command: BX_TOKEN_INFO BX_TOKEN_TSS '\n'  */
#line 735 "parser.y"
      {
        bx_dbg_info_tss_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3232 "y.tab.c"
    break;

  case 160: /* info_command: BX_TOKEN_INFO BX_TOKEN_FLAGS '\n'  */
#line 740 "parser.y"
      {
        bx_dbg_info_flags();
        free((yyvsp[-2].sval));
      }
#line 3241 "y.tab.c"
    break;

  case 161: /* info_command: BX_TOKEN_INFO BX_TOKEN_LINUX '\n'  */
#line 745 "parser.y"
      {
        bx_dbg_info_linux_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3250 "y.tab.c"
    break;

  case 162: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS '\n'  */
#line 750 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3259 "y.tab.c"
    break;

  case 163: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 755 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3268 "y.tab.c"
    break;

  case 164: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE '\n'  */
#line 760 "parser.y"
      {
        bx_dbg_info_device("", "");
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3277 "y.tab.c"
    break;

  case 165: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC '\n'  */
#line 765 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3286 "y.tab.c"
    break;

  case 166: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC BX_TOKEN_STRING '\n'  */
#line 770 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3295 "y.tab.c"
    break;

  case 167: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING '\n'  */
#line 775 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3304 "y.tab.c"
    break;

  case 168: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 780 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3313 "y.tab.c"
    break;

  case 169: /* optional_numeric: %empty  */
#line 787 "parser.y"
               { (yyval.uval) = EMPTY_ARG; }
#line 3319 "y.tab.c"
    break;

  case 171: /* regs_command: BX_TOKEN_REGISTERS '\n'  */
#line 792 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3328 "y.tab.c"
    break;

  case 172: /* fpu_regs_command: BX_TOKEN_FPU '\n'  */
#line 800 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[-1].sval));
      }
#line 3337 "y.tab.c"
    break;

  case 173: /* mmx_regs_command: BX_TOKEN_MMX '\n'  */
#line 808 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3346 "y.tab.c"
    break;

  case 174: /* xmm_regs_command: BX_TOKEN_XMM '\n'  */
#line 816 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3355 "y.tab.c"
    break;

  case 175: /* ymm_regs_command: BX_TOKEN_YMM '\n'  */
#line 824 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3364 "y.tab.c"
    break;

  case 176: /* zmm_regs_command: BX_TOKEN_ZMM '\n'  */
#line 832 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3373 "y.tab.c"
    break;

  case 177: /* amx_regs_command: BX_TOKEN_AMX '\n'  */
#line 840 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_AMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3382 "y.tab.c"
    break;

  case 178: /* print_tile_command: BX_TOKEN_TILE BX_TOKEN_NUMERIC '\n'  */
#line 848 "parser.y"
      {
        bx_dbg_print_amx_tile_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3391 "y.tab.c"
    break;

  case 179: /* segment_regs_command: BX_TOKEN_SEGMENT_REGS '\n'  */
#line 856 "parser.y"
      {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3400 "y.tab.c"
    break;

  case 180: /* control_regs_command: BX_TOKEN_CONTROL_REGS '\n'  */
#line 864 "parser.y"
      {
        bx_dbg_info_control_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3409 "y.tab.c"
    break;

  case 181: /* debug_regs_command: BX_TOKEN_DEBUG_REGS '\n'  */
#line 872 "parser.y"
      {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3418 "y.tab.c"
    break;

  case 182: /* delete_command: BX_TOKEN_DEL_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 880 "parser.y"
      {
        bx_dbg_del_breakpoint_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3427 "y.tab.c"
    break;

  case 183: /* bpe_command: BX_TOKEN_ENABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 888 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3436 "y.tab.c"
    break;

  case 184: /* bpd_command: BX_TOKEN_DISABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 895 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 0);
        free((yyvsp[-2].sval));
      }
#line 3445 "y.tab.c"
    break;

  case 185: /* quit_command: BX_TOKEN_QUIT '\n'  */
#line 903 "parser.y"
      {
        bx_dbg_quit_command();
        free((yyvsp[-1].sval));
      }
#line 3454 "y.tab.c"
    break;

  case 186: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT expression '\n'  */
#line 911 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-3].sval), (yyvsp[-2].sval),1, (yyvsp[-1].uval), 1);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3463 "y.tab.c"
    break;

  case 187: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT '\n'  */
#line 916 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), (yyvsp[-1].sval),1, 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3472 "y.tab.c"
    break;

  case 188: /* examine_command: BX_TOKEN_EXAMINE expression '\n'  */
#line 921 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), NULL,0, (yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3481 "y.tab.c"
    break;

  case 189: /* examine_command: BX_TOKEN_EXAMINE '\n'  */
#line 926 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-1].sval), NULL,0, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 3490 "y.tab.c"
    break;

  case 190: /* restore_command: BX_TOKEN_RESTORE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 934 "parser.y"
      {
        bx_dbg_restore_command((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3499 "y.tab.c"
    break;

  case 191: /* writemem_command: BX_TOKEN_WRITEMEM BX_TOKEN_STRING expression expression '\n'  */
#line 942 "parser.y"
      {
        bx_dbg_writemem_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3508 "y.tab.c"
    break;

  case 192: /* loadmem_command: BX_TOKEN_LOADMEM BX_TOKEN_STRING expression '\n'  */
#line 950 "parser.y"
      {
        bx_dbg_loadmem_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3517 "y.tab.c"
    break;

  case 193: /* setpmem_command: BX_TOKEN_SETPMEM expression expression expression '\n'  */
#line 958 "parser.y"
      {
        bx_dbg_setpmem_command((yyvsp[-3].uval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3526 "y.tab.c"
    break;

  case 194: /* deref_command: BX_TOKEN_DEREF expression expression '\n'  */
#line 966 "parser.y"
      {
        bx_dbg_deref_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3535 "y.tab.c"
    break;

  case 195: /* query_command: BX_TOKEN_QUERY BX_TOKEN_PENDING '\n'  */
#line 974 "parser.y"
      {
        bx_dbg_query_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3544 "y.tab.c"
    break;

  case 196: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA '\n'  */
#line 982 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3553 "y.tab.c"
    break;

  case 197: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA BX_TOKEN_NUMERIC '\n'  */
#line 987 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3562 "y.tab.c"
    break;

  case 198: /* take_command: BX_TOKEN_TAKE BX_TOKEN_IRQ '\n'  */
#line 992 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3571 "y.tab.c"
    break;

  case 199: /* take_command: BX_TOKEN_TAKE BX_TOKEN_SMI '\n'  */
#line 997 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3580 "y.tab.c"
    break;

  case 200: /* take_command: BX_TOKEN_TAKE BX_TOKEN_NMI '\n'  */
#line 1002 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3589 "y.tab.c"
    break;

  case 201: /* disassemble_command: BX_TOKEN_DISASM '\n'  */
#line 1010 "parser.y"
      {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[-1].sval));
      }
#line 3598 "y.tab.c"
    break;

  case 202: /* disassemble_command: BX_TOKEN_DISASM expression '\n'  */
#line 1015 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3607 "y.tab.c"
    break;

  case 203: /* disassemble_command: BX_TOKEN_DISASM expression expression '\n'  */
#line 1020 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3616 "y.tab.c"
    break;

  case 204: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT '\n'  */
#line 1025 "parser.y"
      {
        bx_dbg_disassemble_current((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3625 "y.tab.c"
    break;

  case 205: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression '\n'  */
#line 1030 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-2].sval), (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3634 "y.tab.c"
    break;

  case 206: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression expression '\n'  */
#line 1035 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3643 "y.tab.c"
    break;

  case 207: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SWITCH_MODE '\n'  */
#line 1040 "parser.y"
      {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3652 "y.tab.c"
    break;

  case 208: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SIZE '=' BX_TOKEN_NUMERIC '\n'  */
#line 1045 "parser.y"
      {
        bx_dbg_set_disassemble_size((yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3661 "y.tab.c"
    break;

  case 209: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STOP '\n'  */
#line 1053 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3670 "y.tab.c"
    break;

  case 210: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STRING '\n'  */
#line 1059 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3679 "y.tab.c"
    break;

  case 211: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_GENERIC '\n'  */
#line 1064 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3688 "y.tab.c"
    break;

  case 212: /* doit_command: BX_TOKEN_DOIT expression '\n'  */
#line 1072 "parser.y"
      {
        bx_dbg_doit_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3697 "y.tab.c"
    break;

  case 213: /* crc_command: BX_TOKEN_CRC expression expression '\n'  */
#line 1080 "parser.y"
      {
        bx_dbg_crc_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3706 "y.tab.c"
    break;

  case 214: /* help_command: BX_TOKEN_HELP BX_TOKEN_QUIT '\n'  */
#line 1088 "parser.y"
       {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3715 "y.tab.c"
    break;

  case 215: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTINUE '\n'  */
#line 1093 "parser.y"
       {
         dbg_printf("c|cont|continue - continue executing\n");
         dbg_printf("c|cont|continue if \"expression\" - continue executing only if expression is true\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3725 "y.tab.c"
    break;

  case 216: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEPN '\n'  */
#line 1099 "parser.y"
       {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3736 "y.tab.c"
    break;

  case 217: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEP_OVER '\n'  */
#line 1106 "parser.y"
       {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3745 "y.tab.c"
    break;

  case 218: /* help_command: BX_TOKEN_HELP BX_TOKEN_VBREAKPOINT '\n'  */
#line 1111 "parser.y"
       {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         dbg_printf("vb|vbreak <seg:offset> if \"expression\" - set a conditional virtual address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3755 "y.tab.c"
    break;

  case 219: /* help_command: BX_TOKEN_HELP BX_TOKEN_LBREAKPOINT '\n'  */
#line 1117 "parser.y"
       {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         dbg_printf("lb|lbreak <addr> if \"expression\" - set a conditional linear address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3765 "y.tab.c"
    break;

  case 220: /* help_command: BX_TOKEN_HELP BX_TOKEN_PBREAKPOINT '\n'  */
#line 1123 "parser.y"
       {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         dbg_printf("p|pb|break|pbreak <addr> if \"expression\" - set a conditional physical address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3775 "y.tab.c"
    break;

  case 221: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEL_BREAKPOINT '\n'  */
#line 1129 "parser.y"
       {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3784 "y.tab.c"
    break;

  case 222: /* help_command: BX_TOKEN_HELP BX_TOKEN_ENABLE_BREAKPOINT '\n'  */
#line 1134 "parser.y"
       {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3793 "y.tab.c"
    break;

  case 223: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISABLE_BREAKPOINT '\n'  */
#line 1139 "parser.y"
       {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3802 "y.tab.c"
    break;

  case 224: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_BREAK '\n'  */
#line 1144 "parser.y"
       {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3811 "y.tab.c"
    break;

  case 225: /* help_command: BX_TOKEN_HELP BX_TOKEN_MODEBP '\n'  */
#line 1149 "parser.y"
       {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3820 "y.tab.c"
    break;

  case 226: /* help_command: BX_TOKEN_HELP BX_TOKEN_VMEXITBP '\n'  */
#line 1154 "parser.y"
       {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3829 "y.tab.c"
    break;

  case 227: /* help_command: BX_TOKEN_HELP BX_TOKEN_CRC '\n'  */
#line 1159 "parser.y"
       {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3838 "y.tab.c"
    break;

  case 228: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACE '\n'  */
#line 1164 "parser.y"
       {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3848 "y.tab.c"
    break;

  case 229: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEREG '\n'  */
#line 1170 "parser.y"
       {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3858 "y.tab.c"
    break;

  case 230: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEMEM '\n'  */
#line 1176 "parser.y"
       {
         dbg_printf("trace-mem on  - print all memory accesses occurred during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3868 "y.tab.c"
    break;

  case 231: /* help_command: BX_TOKEN_HELP BX_TOKEN_RESTORE '\n'  */
#line 1182 "parser.y"
       {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3880 "y.tab.c"
    break;

  case 232: /* help_command: BX_TOKEN_HELP BX_TOKEN_PTIME '\n'  */
#line 1190 "parser.y"
       {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3889 "y.tab.c"
    break;

  case 233: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP '\n'  */
#line 1195 "parser.y"
       {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3898 "y.tab.c"
    break;

  case 234: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP_ABSOLUTE '\n'  */
#line 1200 "parser.y"
       {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3907 "y.tab.c"
    break;

  case 235: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STACK '\n'  */
#line 1205 "parser.y"
       {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3916 "y.tab.c"
    break;

  case 236: /* help_command: BX_TOKEN_HELP BX_TOKEN_BT '\n'  */
#line 1210 "parser.y"
       {
         dbg_printf("bt [num_entries] - prints backtrace\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3925 "y.tab.c"
    break;

  case 237: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOAD_SYMBOLS '\n'  */
#line 1215 "parser.y"
       {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3934 "y.tab.c"
    break;

  case 238: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET_MAGIC_BREAK_POINTS '\n'  */
#line 1220 "parser.y"
       {
         dbg_printf("setmagicbps \"cx dx bx sp bp si di\" - set new magic breakpoints. You can specify multiple at once. Using the setmagicbps command without any arguments will disable all of them\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3943 "y.tab.c"
    break;

  case 239: /* help_command: BX_TOKEN_HELP BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS '\n'  */
#line 1225 "parser.y"
       {
         dbg_printf("clrmagicbps \"cx dx bx sp bp si di\" - clear magic breakpoints. You can specify multiple at once. Using the clrmagicbps command without any arguments will disable all of them\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3952 "y.tab.c"
    break;

  case 240: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 1230 "parser.y"
       {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3961 "y.tab.c"
    break;

  case 241: /* help_command: BX_TOKEN_HELP BX_TOKEN_REGISTERS '\n'  */
#line 1235 "parser.y"
       {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3970 "y.tab.c"
    break;

  case 242: /* help_command: BX_TOKEN_HELP BX_TOKEN_FPU '\n'  */
#line 1240 "parser.y"
       {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3979 "y.tab.c"
    break;

  case 243: /* help_command: BX_TOKEN_HELP BX_TOKEN_MMX '\n'  */
#line 1245 "parser.y"
       {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3988 "y.tab.c"
    break;

  case 244: /* help_command: BX_TOKEN_HELP BX_TOKEN_XMM '\n'  */
#line 1250 "parser.y"
       {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3997 "y.tab.c"
    break;

  case 245: /* help_command: BX_TOKEN_HELP BX_TOKEN_YMM '\n'  */
#line 1255 "parser.y"
       {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4006 "y.tab.c"
    break;

  case 246: /* help_command: BX_TOKEN_HELP BX_TOKEN_ZMM '\n'  */
#line 1260 "parser.y"
       {
         dbg_printf("zmm - print AVX-512 state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4015 "y.tab.c"
    break;

  case 247: /* help_command: BX_TOKEN_HELP BX_TOKEN_AMX '\n'  */
#line 1265 "parser.y"
       {
         dbg_printf("amx - print AMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4024 "y.tab.c"
    break;

  case 248: /* help_command: BX_TOKEN_HELP BX_TOKEN_SEGMENT_REGS '\n'  */
#line 1270 "parser.y"
       {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4033 "y.tab.c"
    break;

  case 249: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTROL_REGS '\n'  */
#line 1275 "parser.y"
       {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4042 "y.tab.c"
    break;

  case 250: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEBUG_REGS '\n'  */
#line 1280 "parser.y"
       {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4051 "y.tab.c"
    break;

  case 251: /* help_command: BX_TOKEN_HELP BX_TOKEN_WRITEMEM '\n'  */
#line 1285 "parser.y"
       {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4060 "y.tab.c"
    break;

  case 252: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOADMEM '\n'  */
#line 1290 "parser.y"
       {
         dbg_printf("loadmem <filename> <laddr> - load file bytes to virtual memory starting from the linear address 'laddr'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4069 "y.tab.c"
    break;

  case 253: /* help_command: BX_TOKEN_HELP BX_TOKEN_SETPMEM '\n'  */
#line 1295 "parser.y"
       {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4078 "y.tab.c"
    break;

  case 254: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEREF '\n'  */
#line 1300 "parser.y"
       {
         dbg_printf("deref <addr> <deep> - pointer dereference. For example: get value of [[[rax]]] or ***rax: deref rax 3\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4087 "y.tab.c"
    break;

  case 255: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISASM '\n'  */
#line 1305 "parser.y"
       {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm hex on/off - control disasm offsets and displacements format\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4101 "y.tab.c"
    break;

  case 256: /* help_command: BX_TOKEN_HELP BX_TOKEN_WATCH '\n'  */
#line 1315 "parser.y"
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
#line 4116 "y.tab.c"
    break;

  case 257: /* help_command: BX_TOKEN_HELP BX_TOKEN_UNWATCH '\n'  */
#line 1326 "parser.y"
       {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4126 "y.tab.c"
    break;

  case 258: /* help_command: BX_TOKEN_HELP BX_TOKEN_EXAMINE '\n'  */
#line 1332 "parser.y"
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
#line 4143 "y.tab.c"
    break;

  case 259: /* help_command: BX_TOKEN_HELP BX_TOKEN_INSTRUMENT '\n'  */
#line 1345 "parser.y"
       {
         dbg_printf("instrument <command|\"string command\"> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command|\"string command\">\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4152 "y.tab.c"
    break;

  case 260: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET '\n'  */
#line 1350 "parser.y"
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
#line 4167 "y.tab.c"
    break;

  case 261: /* help_command: BX_TOKEN_HELP BX_TOKEN_PAGE '\n'  */
#line 1361 "parser.y"
       {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4176 "y.tab.c"
    break;

  case 262: /* help_command: BX_TOKEN_HELP BX_TOKEN_INFO '\n'  */
#line 1366 "parser.y"
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
#line 4196 "y.tab.c"
    break;

  case 263: /* help_command: BX_TOKEN_HELP BX_TOKEN_SHOW '\n'  */
#line 1382 "parser.y"
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
#line 4216 "y.tab.c"
    break;

  case 264: /* help_command: BX_TOKEN_HELP BX_TOKEN_CALC '\n'  */
#line 1398 "parser.y"
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
#line 4232 "y.tab.c"
    break;

  case 265: /* help_command: BX_TOKEN_HELP BX_TOKEN_ADDLYT '\n'  */
#line 1410 "parser.y"
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
#line 4248 "y.tab.c"
    break;

  case 266: /* help_command: BX_TOKEN_HELP BX_TOKEN_REMLYT '\n'  */
#line 1422 "parser.y"
       {
         dbg_printf("remlyt - stops debugger to execute the script file added previously with addlyt command.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4257 "y.tab.c"
    break;

  case 267: /* help_command: BX_TOKEN_HELP BX_TOKEN_LYT '\n'  */
#line 1427 "parser.y"
       {
         dbg_printf("lyt - cause debugger to execute script file added previously with addlyt command.\n");
         dbg_printf("    Use it as a refresh/context.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4267 "y.tab.c"
    break;

  case 268: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STRING '\n'  */
#line 1433 "parser.y"
       {
         dbg_printf("print-string <addr> - prints a null-ended string from a linear address.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4276 "y.tab.c"
    break;

  case 269: /* help_command: BX_TOKEN_HELP BX_TOKEN_SOURCE '\n'  */
#line 1438 "parser.y"
       {
         dbg_printf("source <file> - cause debugger to execute a script file.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4285 "y.tab.c"
    break;

  case 270: /* help_command: BX_TOKEN_HELP BX_TOKEN_HELP '\n'  */
#line 1443 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4294 "y.tab.c"
    break;

  case 271: /* help_command: BX_TOKEN_HELP '\n'  */
#line 1448 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-1].sval));
       }
#line 4303 "y.tab.c"
    break;

  case 272: /* calc_command: BX_TOKEN_CALC expression '\n'  */
#line 1456 "parser.y"
   {
     eval_value = (yyvsp[-1].uval);
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4313 "y.tab.c"
    break;

  case 273: /* addlyt_command: BX_TOKEN_ADDLYT BX_TOKEN_STRING '\n'  */
#line 1465 "parser.y"
   {
     bx_dbg_addlyt((yyvsp[-1].sval));
     free((yyvsp[-2].sval));
     free((yyvsp[-1].sval));
   }
#line 4323 "y.tab.c"
    break;

  case 274: /* remlyt_command: BX_TOKEN_REMLYT '\n'  */
#line 1474 "parser.y"
   {
     bx_dbg_remlyt();
     free((yyvsp[-1].sval));
   }
#line 4332 "y.tab.c"
    break;

  case 275: /* lyt_command: BX_TOKEN_LYT '\n'  */
#line 1482 "parser.y"
   {
     bx_dbg_lyt();
     free((yyvsp[-1].sval));
   }
#line 4341 "y.tab.c"
    break;

  case 276: /* if_command: BX_TOKEN_IF expression '\n'  */
#line 1490 "parser.y"
   {
     eval_value = (yyvsp[-1].uval) != 0;
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4351 "y.tab.c"
    break;

  case 277: /* vexpression: BX_TOKEN_NUMERIC  */
#line 1499 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4357 "y.tab.c"
    break;

  case 278: /* vexpression: BX_TOKEN_STRING  */
#line 1500 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4363 "y.tab.c"
    break;

  case 279: /* vexpression: BX_TOKEN_8BL_REG  */
#line 1501 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4369 "y.tab.c"
    break;

  case 280: /* vexpression: BX_TOKEN_8BH_REG  */
#line 1502 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4375 "y.tab.c"
    break;

  case 281: /* vexpression: BX_TOKEN_16B_REG  */
#line 1503 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4381 "y.tab.c"
    break;

  case 282: /* vexpression: BX_TOKEN_32B_REG  */
#line 1504 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4387 "y.tab.c"
    break;

  case 283: /* vexpression: BX_TOKEN_64B_REG  */
#line 1505 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4393 "y.tab.c"
    break;

  case 284: /* vexpression: BX_TOKEN_OPMASK_REG  */
#line 1506 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4399 "y.tab.c"
    break;

  case 285: /* vexpression: BX_TOKEN_SEGREG  */
#line 1507 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4405 "y.tab.c"
    break;

  case 286: /* vexpression: BX_TOKEN_REG_IP  */
#line 1508 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4411 "y.tab.c"
    break;

  case 287: /* vexpression: BX_TOKEN_REG_EIP  */
#line 1509 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4417 "y.tab.c"
    break;

  case 288: /* vexpression: BX_TOKEN_REG_RIP  */
#line 1510 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4423 "y.tab.c"
    break;

  case 289: /* vexpression: BX_TOKEN_REG_SSP  */
#line 1511 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4429 "y.tab.c"
    break;

  case 290: /* vexpression: vexpression '+' vexpression  */
#line 1512 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4435 "y.tab.c"
    break;

  case 291: /* vexpression: vexpression '-' vexpression  */
#line 1513 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4441 "y.tab.c"
    break;

  case 292: /* vexpression: vexpression '*' vexpression  */
#line 1514 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4447 "y.tab.c"
    break;

  case 293: /* vexpression: vexpression '/' vexpression  */
#line 1515 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) / (yyvsp[0].uval); }
#line 4453 "y.tab.c"
    break;

  case 294: /* vexpression: vexpression BX_TOKEN_DEREF_CHR vexpression  */
#line 1516 "parser.y"
                                                { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 4459 "y.tab.c"
    break;

  case 295: /* vexpression: vexpression BX_TOKEN_RSHIFT vexpression  */
#line 1517 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4465 "y.tab.c"
    break;

  case 296: /* vexpression: vexpression BX_TOKEN_LSHIFT vexpression  */
#line 1518 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4471 "y.tab.c"
    break;

  case 297: /* vexpression: vexpression '|' vexpression  */
#line 1519 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4477 "y.tab.c"
    break;

  case 298: /* vexpression: vexpression '^' vexpression  */
#line 1520 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4483 "y.tab.c"
    break;

  case 299: /* vexpression: vexpression '&' vexpression  */
#line 1521 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4489 "y.tab.c"
    break;

  case 300: /* vexpression: '!' vexpression  */
#line 1522 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4495 "y.tab.c"
    break;

  case 301: /* vexpression: '-' vexpression  */
#line 1523 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4501 "y.tab.c"
    break;

  case 302: /* vexpression: '(' vexpression ')'  */
#line 1524 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4507 "y.tab.c"
    break;

  case 303: /* expression: BX_TOKEN_NUMERIC  */
#line 1530 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4513 "y.tab.c"
    break;

  case 304: /* expression: BX_TOKEN_STRING  */
#line 1531 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4519 "y.tab.c"
    break;

  case 305: /* expression: BX_TOKEN_8BL_REG  */
#line 1532 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4525 "y.tab.c"
    break;

  case 306: /* expression: BX_TOKEN_8BH_REG  */
#line 1533 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4531 "y.tab.c"
    break;

  case 307: /* expression: BX_TOKEN_16B_REG  */
#line 1534 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4537 "y.tab.c"
    break;

  case 308: /* expression: BX_TOKEN_32B_REG  */
#line 1535 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4543 "y.tab.c"
    break;

  case 309: /* expression: BX_TOKEN_64B_REG  */
#line 1536 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4549 "y.tab.c"
    break;

  case 310: /* expression: BX_TOKEN_OPMASK_REG  */
#line 1537 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4555 "y.tab.c"
    break;

  case 311: /* expression: BX_TOKEN_SEGREG  */
#line 1538 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4561 "y.tab.c"
    break;

  case 312: /* expression: BX_TOKEN_REG_IP  */
#line 1539 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4567 "y.tab.c"
    break;

  case 313: /* expression: BX_TOKEN_REG_EIP  */
#line 1540 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4573 "y.tab.c"
    break;

  case 314: /* expression: BX_TOKEN_REG_RIP  */
#line 1541 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4579 "y.tab.c"
    break;

  case 315: /* expression: BX_TOKEN_REG_SSP  */
#line 1542 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4585 "y.tab.c"
    break;

  case 316: /* expression: expression ':' expression  */
#line 1543 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[-2].uval), (yyvsp[0].uval)); }
#line 4591 "y.tab.c"
    break;

  case 317: /* expression: expression '+' expression  */
#line 1544 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4597 "y.tab.c"
    break;

  case 318: /* expression: expression '-' expression  */
#line 1545 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4603 "y.tab.c"
    break;

  case 319: /* expression: expression '*' expression  */
#line 1546 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4609 "y.tab.c"
    break;

  case 320: /* expression: expression '/' expression  */
#line 1547 "parser.y"
                                     { (yyval.uval) = ((yyvsp[0].uval) != 0) ? (yyvsp[-2].uval) / (yyvsp[0].uval) : 0; }
#line 4615 "y.tab.c"
    break;

  case 321: /* expression: expression BX_TOKEN_DEREF_CHR expression  */
#line 1548 "parser.y"
                                              { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 4621 "y.tab.c"
    break;

  case 322: /* expression: expression BX_TOKEN_RSHIFT expression  */
#line 1549 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4627 "y.tab.c"
    break;

  case 323: /* expression: expression BX_TOKEN_LSHIFT expression  */
#line 1550 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4633 "y.tab.c"
    break;

  case 324: /* expression: expression '|' expression  */
#line 1551 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4639 "y.tab.c"
    break;

  case 325: /* expression: expression '^' expression  */
#line 1552 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4645 "y.tab.c"
    break;

  case 326: /* expression: expression '&' expression  */
#line 1553 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4651 "y.tab.c"
    break;

  case 327: /* expression: expression '>' expression  */
#line 1554 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) > (yyvsp[0].uval); }
#line 4657 "y.tab.c"
    break;

  case 328: /* expression: expression '<' expression  */
#line 1555 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) < (yyvsp[0].uval); }
#line 4663 "y.tab.c"
    break;

  case 329: /* expression: expression BX_TOKEN_EQ expression  */
#line 1556 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) == (yyvsp[0].uval); }
#line 4669 "y.tab.c"
    break;

  case 330: /* expression: expression BX_TOKEN_NE expression  */
#line 1557 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) != (yyvsp[0].uval); }
#line 4675 "y.tab.c"
    break;

  case 331: /* expression: expression BX_TOKEN_LE expression  */
#line 1558 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) <= (yyvsp[0].uval); }
#line 4681 "y.tab.c"
    break;

  case 332: /* expression: expression BX_TOKEN_GE expression  */
#line 1559 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) >= (yyvsp[0].uval); }
#line 4687 "y.tab.c"
    break;

  case 333: /* expression: '!' expression  */
#line 1560 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4693 "y.tab.c"
    break;

  case 334: /* expression: '-' expression  */
#line 1561 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4699 "y.tab.c"
    break;

  case 335: /* expression: '*' expression  */
#line 1562 "parser.y"
                                     { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[0].uval)); }
#line 4705 "y.tab.c"
    break;

  case 336: /* expression: '@' expression  */
#line 1563 "parser.y"
                                     { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[0].uval)); }
#line 4711 "y.tab.c"
    break;

  case 337: /* expression: '(' expression ')'  */
#line 1564 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4717 "y.tab.c"
    break;


#line 4721 "y.tab.c"

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

#line 1567 "parser.y"

#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
