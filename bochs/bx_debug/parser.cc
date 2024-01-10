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
  YYSYMBOL_set_command = 171,              /* set_command  */
  YYSYMBOL_breakpoint_command = 172,       /* breakpoint_command  */
  YYSYMBOL_blist_command = 173,            /* blist_command  */
  YYSYMBOL_slist_command = 174,            /* slist_command  */
  YYSYMBOL_info_command = 175,             /* info_command  */
  YYSYMBOL_optional_numeric = 176,         /* optional_numeric  */
  YYSYMBOL_regs_command = 177,             /* regs_command  */
  YYSYMBOL_fpu_regs_command = 178,         /* fpu_regs_command  */
  YYSYMBOL_mmx_regs_command = 179,         /* mmx_regs_command  */
  YYSYMBOL_xmm_regs_command = 180,         /* xmm_regs_command  */
  YYSYMBOL_ymm_regs_command = 181,         /* ymm_regs_command  */
  YYSYMBOL_zmm_regs_command = 182,         /* zmm_regs_command  */
  YYSYMBOL_amx_regs_command = 183,         /* amx_regs_command  */
  YYSYMBOL_print_tile_command = 184,       /* print_tile_command  */
  YYSYMBOL_segment_regs_command = 185,     /* segment_regs_command  */
  YYSYMBOL_control_regs_command = 186,     /* control_regs_command  */
  YYSYMBOL_debug_regs_command = 187,       /* debug_regs_command  */
  YYSYMBOL_delete_command = 188,           /* delete_command  */
  YYSYMBOL_bpe_command = 189,              /* bpe_command  */
  YYSYMBOL_bpd_command = 190,              /* bpd_command  */
  YYSYMBOL_quit_command = 191,             /* quit_command  */
  YYSYMBOL_examine_command = 192,          /* examine_command  */
  YYSYMBOL_restore_command = 193,          /* restore_command  */
  YYSYMBOL_writemem_command = 194,         /* writemem_command  */
  YYSYMBOL_loadmem_command = 195,          /* loadmem_command  */
  YYSYMBOL_setpmem_command = 196,          /* setpmem_command  */
  YYSYMBOL_deref_command = 197,            /* deref_command  */
  YYSYMBOL_query_command = 198,            /* query_command  */
  YYSYMBOL_take_command = 199,             /* take_command  */
  YYSYMBOL_disassemble_command = 200,      /* disassemble_command  */
  YYSYMBOL_instrument_command = 201,       /* instrument_command  */
  YYSYMBOL_doit_command = 202,             /* doit_command  */
  YYSYMBOL_crc_command = 203,              /* crc_command  */
  YYSYMBOL_help_command = 204,             /* help_command  */
  YYSYMBOL_calc_command = 205,             /* calc_command  */
  YYSYMBOL_addlyt_command = 206,           /* addlyt_command  */
  YYSYMBOL_remlyt_command = 207,           /* remlyt_command  */
  YYSYMBOL_lyt_command = 208,              /* lyt_command  */
  YYSYMBOL_if_command = 209,               /* if_command  */
  YYSYMBOL_vexpression = 210,              /* vexpression  */
  YYSYMBOL_expression = 211                /* expression  */
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
#define YYFINAL  350
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2569

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  144
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  68
/* YYNRULES -- Number of rules.  */
#define YYNRULES  335
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  652

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
     221,   222,   223,   224,   225,   226,   232,   233,   238,   239,
     244,   245,   246,   247,   248,   249,   254,   259,   267,   275,
     283,   288,   293,   298,   303,   308,   316,   324,   332,   340,
     348,   356,   364,   369,   377,   382,   390,   395,   400,   405,
     410,   415,   420,   425,   430,   435,   440,   448,   453,   458,
     463,   471,   476,   484,   489,   497,   505,   513,   518,   526,
     531,   536,   541,   549,   557,   562,   567,   571,   575,   579,
     583,   587,   591,   595,   602,   607,   612,   617,   622,   627,
     632,   637,   642,   647,   652,   657,   662,   670,   678,   683,
     691,   696,   701,   706,   711,   716,   721,   726,   731,   736,
     741,   746,   751,   756,   761,   766,   771,   779,   780,   783,
     791,   799,   807,   815,   823,   831,   839,   847,   855,   863,
     871,   879,   886,   894,   902,   907,   912,   917,   925,   933,
     941,   949,   957,   965,   973,   978,   983,   988,   993,  1001,
    1006,  1011,  1016,  1021,  1026,  1031,  1036,  1044,  1050,  1055,
    1063,  1071,  1079,  1084,  1090,  1097,  1102,  1108,  1114,  1120,
    1125,  1130,  1135,  1140,  1145,  1150,  1155,  1161,  1167,  1173,
    1181,  1186,  1191,  1196,  1201,  1206,  1211,  1216,  1221,  1226,
    1231,  1236,  1241,  1246,  1251,  1256,  1261,  1266,  1271,  1276,
    1281,  1286,  1291,  1296,  1306,  1317,  1323,  1336,  1341,  1352,
    1357,  1373,  1389,  1401,  1413,  1418,  1424,  1429,  1434,  1439,
    1447,  1456,  1465,  1473,  1481,  1491,  1492,  1493,  1494,  1495,
    1496,  1497,  1498,  1499,  1500,  1501,  1502,  1503,  1504,  1505,
    1506,  1507,  1508,  1509,  1510,  1511,  1512,  1513,  1514,  1515,
    1516,  1522,  1523,  1524,  1525,  1526,  1527,  1528,  1529,  1530,
    1531,  1532,  1533,  1534,  1535,  1536,  1537,  1538,  1539,  1540,
    1541,  1542,  1543,  1544,  1545,  1546,  1547,  1548,  1549,  1550,
    1551,  1552,  1553,  1554,  1555,  1556
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
  "stepN_command", "step_over_command", "set_command",
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

#define YYPACT_NINF (-193)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-334)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     613,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,   -17,  1420,   -43,  -129,   192,   -84,  1447,
     326,  1153,   -49,   -41,   -40,  1222,   -72,  -193,  -193,   -68,
     -67,   -60,   -59,   -45,   -16,    -7,    -6,    -5,    18,  1122,
      23,    54,    87,  1420,  1420,    98,    40,  1420,  1098,   -33,
    -193,  1420,  1420,    33,    33,    33,    27,  1420,  1420,    41,
      42,   -65,   -63,   233,  1178,    17,   -15,   -61,   -56,   -55,
      43,  1420,  -193,  1420,  1591,  1420,   103,    45,    48,  -193,
    -193,  -193,  -193,  1420,  1420,  -193,  1420,  1420,  1420,   472,
    -193,    49,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  2410,  1420,  -193,   858,    84,   -47,  -193,  -193,
      50,    51,    52,    53,    55,    56,    33,    79,    80,    81,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  1586,  -193,  1586,  1586,  -193,  2430,   -10,
    -193,   -14,  1420,  -193,   287,    95,    96,    97,   100,   101,
     108,  1420,  1420,  1420,  1420,   112,   113,   119,   -39,   -58,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,   121,  -193,  -193,
    -193,  1204,  -193,   887,   185,  1420,  1420,   957,   957,   124,
     -42,   130,   131,   136,  1604,  1345,   137,   158,  -193,    19,
     160,   161,   163,  1630,   957,  -193,  -193,   167,   170,   172,
    -193,  1656,  1682,  -193,  -193,   173,  -193,   174,  -193,   175,
    1420,   176,  1420,  1420,  -193,  -193,  1708,   179,   180,   -69,
     181,  -193,  1369,   201,   186,  -193,   187,  -193,   190,  -193,
    -193,  1734,  1760,   191,   205,   206,   207,   208,   209,   210,
     211,   235,   236,   237,   244,   248,   260,   261,   262,   271,
     272,   273,   274,   284,   285,   286,   288,   290,   291,   294,
     298,   306,   308,   309,   314,   319,   321,   323,   331,   333,
     334,   336,   350,   351,   352,   358,   370,   376,   379,   380,
     381,   382,   383,   384,   385,   386,   391,   392,   398,  -193,
     400,  1786,   402,  -193,  -193,   320,   320,   320,   716,   320,
    -193,  -193,  -193,  1420,  1420,  1420,  1420,  1420,  1420,  1420,
    1420,  1420,  1420,  1420,  1420,  1420,  1420,  1420,  1420,  1420,
    1812,  -193,   403,   404,  -193,  1420,  1420,  1420,  1420,  1420,
    1420,   405,  1420,  1420,  1420,  -193,  -193,   115,  1586,  1586,
    1586,  1586,  1586,  1586,  1586,  1586,  1586,  1586,  1586,   473,
    -193,   479,  -193,   -13,   480,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  1420,  2410,  1420,  1420,  1420,  -193,  -193,  -193,
     427,  -193,   -38,    -8,  -193,  -193,  -193,  1838,  -193,   428,
     957,  1864,  1420,  1420,   957,  1890,  -193,   433,  -193,  -193,
    -193,  -193,  -193,  -193,   162,  -193,   475,  -193,  1916,  -193,
    -193,  -193,  -193,  1942,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,   754,  -193,   785,   926,  -193,  -193,  -193,   435,
    -193,  -193,  -193,  1968,  1396,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
     202,   202,   202,   320,   320,   320,   320,   322,   322,   322,
     322,   322,   322,   202,   202,   202,  2410,  -193,  -193,  -193,
    1994,  2020,  2046,  2072,  2098,  2124,  -193,  2150,  2176,  2202,
    -193,  -193,  -193,  -193,   138,   138,   138,   138,  -193,  -193,
    -193,   686,   453,   454,   518,  -193,   460,   462,   467,   468,
     470,  -193,   474,  -193,   477,  -193,  -193,  -193,  2228,  -193,
     469,   232,  2254,  -193,  -193,  -193,  2280,   491,  -193,  -193,
    -193,  2306,  -193,  2332,  -193,  2358,  -193,  -193,  -193,  2384,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,   532,
    -193,  -193,  -193,   492,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,   493,
    -193,  -193
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
      64,   304,   303,   305,   306,   307,    70,    71,    72,    73,
      74,    75,   308,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    68,    69,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     302,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   301,     0,     0,     0,     0,     0,     0,   310,
     311,   312,   313,     0,     0,    65,     0,     0,     0,     0,
       3,     0,   309,    43,    44,    45,    53,    51,    52,    42,
      39,    40,    41,    46,    47,    50,    54,    48,    49,    55,
      56,     4,     5,     6,     7,     8,    21,    22,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    20,    19,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    57,    58,    59,    60,
      61,    62,    63,     0,   117,     0,     0,     0,   119,   123,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     147,   278,   277,   279,   280,   281,   282,   276,   275,   284,
     285,   286,   287,     0,   134,     0,     0,   283,     0,   302,
     137,     0,     0,   142,     0,     0,     0,     0,     0,     0,
       0,   167,   167,   167,   167,     0,     0,     0,     0,     0,
     183,   170,   171,   172,   173,   174,   175,     0,   179,   178,
     177,     0,   187,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   199,     0,
       0,     0,     0,     0,     0,    66,    67,     0,     0,     0,
      88,     0,     0,    78,    79,     0,    92,     0,    94,     0,
       0,     0,     0,     0,    98,   105,     0,     0,     0,     0,
       0,    85,     0,     0,     0,   111,     0,   113,     0,   148,
     115,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   269,
       0,     0,     0,   272,   273,   332,   333,   331,     0,   334,
       1,     2,   169,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   274,     0,     0,   120,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   299,   298,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     140,     0,   138,   333,     0,   143,   180,   181,   182,   158,
     150,   151,   167,   168,   167,   167,   167,   157,   156,   159,
       0,   160,     0,     0,   162,   176,   185,     0,   186,     0,
       0,     0,     0,     0,     0,     0,   193,     0,   194,   196,
     197,   198,    87,   202,     0,   205,     0,   200,     0,   208,
     207,   209,   210,     0,    89,    90,    91,    77,    76,    93,
      95,    97,     0,    96,     0,     0,   106,    82,    81,     0,
      83,    80,   107,     0,     0,   112,   114,   149,   116,    86,
     213,   214,   215,   258,   222,   216,   217,   218,   219,   220,
     221,   260,   212,   240,   241,   242,   243,   244,   245,   248,
     247,   246,   256,   229,   249,   250,   251,   252,   253,   257,
     225,   226,   227,   228,   230,   232,   231,   223,   224,   233,
     234,   254,   255,   261,   235,   236,   237,   238,   266,   259,
     268,   262,   263,   264,   265,   267,   239,   270,   271,   335,
     319,   320,   321,   327,   328,   329,   330,   315,   316,   322,
     323,   326,   325,   317,   318,   324,   314,   118,   121,   122,
       0,     0,     0,     0,     0,     0,   124,     0,     0,     0,
     300,   292,   293,   294,   288,   289,   295,   296,   290,   291,
     297,     0,     0,     0,     0,   145,     0,     0,     0,     0,
       0,   161,     0,   165,     0,   163,   184,   188,     0,   190,
     316,   317,     0,   192,   195,   203,     0,     0,   201,   211,
      99,     0,   100,     0,   101,     0,    84,   108,   109,     0,
     127,   126,   128,   129,   130,   125,   131,   132,   133,     0,
     135,   141,   139,     0,   144,   152,   153,   154,   155,   166,
     164,   189,   191,   204,   206,   102,   103,   104,   110,     0,
     146,   136
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -193,  -193,   521,    -9,   562,    -2,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -192,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -182,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    89,    90,   247,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   412,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   188,   413
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     152,   385,   153,   386,   387,   401,   584,   156,   159,   399,
     414,   415,   416,   274,   155,   169,   422,   187,   276,   278,
     191,   194,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,   267,   420,   592,   469,   255,   223,
     257,   240,   241,   227,   228,   248,   249,   234,   239,   245,
     246,   243,   244,   170,   195,   423,   373,   251,   252,   272,
     157,   437,   196,   197,   266,   210,   594,   268,   470,   211,
     212,   281,   256,   282,   258,   341,   275,   213,   214,   424,
     242,   277,   279,   345,   346,   273,   347,   348,   349,   152,
     374,   269,   215,    50,   158,   438,   217,   224,   421,   593,
     353,   354,   355,   356,   357,   358,   359,   230,   231,   232,
     233,   360,   361,   362,   363,   364,   365,   366,   367,   368,
     154,   216,    72,   402,   585,   369,   369,   400,   225,   595,
     270,   218,   219,   353,   354,   355,   356,   357,   358,   359,
      79,    80,    81,    82,   360,   432,   362,   363,   364,   365,
     433,   367,   368,   370,   271,   220,   447,   381,   369,    86,
      87,   226,    88,   229,   250,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,   342,   253,   254,
     280,   187,   343,   187,   187,   344,   352,   372,   375,   376,
     377,   378,   403,   379,   380,   160,   161,   162,   163,   164,
       6,     7,     8,     9,    10,    11,   571,   572,   573,   574,
     575,   576,   577,   578,   579,   580,   581,   382,   383,   384,
     587,   427,   588,   589,   590,   430,   431,   434,   435,   388,
     389,   390,   406,   407,   408,   444,    50,   409,   410,   448,
     391,   392,   393,   394,   453,   411,   395,   396,   397,   417,
     418,   259,   388,   389,   390,   165,   419,   570,   425,   429,
     462,   436,   464,   465,   166,    72,   260,   439,   440,   395,
     396,   397,   473,   441,   445,   474,   353,   354,   355,   356,
     357,   358,   359,    79,    80,    81,    82,   360,   432,   362,
     363,   364,   365,   433,   367,   368,   446,   449,   450,   605,
     451,   369,    86,    87,   454,    88,   404,   455,   261,   456,
     459,   460,   461,   463,   167,   168,   467,   468,   471,   356,
     357,   358,   359,   475,   476,   262,   263,   477,   480,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,   369,   481,   482,   483,   484,   485,   486,   487,  -333,
    -333,  -333,  -333,   540,   541,   542,   543,   544,   545,   546,
     547,   548,   549,   550,   551,   552,   553,   554,   555,   556,
     264,   369,   488,   489,   490,   560,   561,   562,   563,   564,
     565,   491,   567,   568,   569,   492,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   493,   494,   495,
     189,   353,   354,   355,   356,   357,   358,   359,   496,   497,
     498,   499,   360,   361,   362,   363,   364,   365,   366,   367,
     368,   500,   501,   502,   405,   503,   369,   504,   505,    72,
     598,   506,   600,   601,   602,   507,   353,   354,   355,   356,
     357,   358,   359,   508,   606,   509,   510,    79,    80,    81,
      82,   511,    83,   366,   367,   368,   512,    84,   513,   369,
     514,   369,   611,   190,   613,   615,    86,    87,   515,    88,
     516,   517,   350,   518,   619,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,   519,   520,   521,
      13,    14,    15,    16,    17,   522,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,   523,    29,    30,
      31,    32,    33,   524,    34,    35,   525,   526,   527,   528,
     529,   530,   531,   532,    36,    37,    38,    39,   533,   534,
      40,    41,    42,    43,    44,   535,    45,   536,    46,   538,
     558,   559,   566,    47,    48,    49,    50,   582,    51,    52,
      53,    54,    55,   583,   586,    56,    57,    58,    59,    60,
      61,    62,    63,    64,   591,   597,    65,    66,    67,    68,
     604,    69,   616,    70,    71,    72,    73,    74,   607,    75,
      76,    77,    78,  -332,  -332,  -332,  -332,  -332,  -332,  -332,
     631,   632,   633,    79,    80,    81,    82,   634,    83,   635,
    -332,  -332,  -332,    84,   636,   637,   649,   638,   369,    85,
     351,   639,    86,    87,   640,    88,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,   644,   650,
     651,    13,    14,    15,    16,    17,   340,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,     0,    29,
      30,    31,    32,    33,     0,    34,    35,     0,     0,     0,
       0,     0,     0,     0,     0,    36,    37,    38,    39,     0,
       0,    40,    41,    42,    43,    44,     0,    45,     0,    46,
       0,     0,     0,     0,    47,    48,    49,    50,     0,    51,
      52,    53,    54,    55,     0,     0,    56,    57,    58,    59,
      60,    61,    62,    63,    64,   629,     0,    65,    66,    67,
      68,     0,    69,     0,    70,    71,    72,    73,    74,     0,
      75,    76,    77,    78,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    79,    80,    81,    82,     0,    83,
       0,     0,     0,     0,    84,     0,     0,     0,     0,     0,
      85,     0,     0,    86,    87,     0,    88,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
     388,   389,   390,     0,     0,     0,     0,     0,     0,     0,
       0,   391,   392,   393,   394,     0,     0,   395,   396,   397,
       0,     0,     0,   630,     0,     0,     0,     0,    50,     0,
     353,   354,   355,   356,   357,   358,   359,     0,     0,     0,
       0,   360,   361,   362,   363,   364,   365,   366,   367,   368,
       0,     0,     0,     0,     0,   369,     0,    72,   539,    50,
       0,     0,     0,     0,     0,     0,     0,     0,   353,   354,
     355,   356,   357,   358,   359,    79,    80,    81,    82,   360,
     432,   362,   363,   364,   365,   433,   367,   368,    72,     0,
       0,   610,     0,   369,    86,    87,     0,    88,     0,   353,
     354,   355,   356,   357,   358,   359,    79,    80,    81,    82,
     360,   432,   362,   363,   364,   365,   433,   367,   368,     0,
       0,     0,   612,     0,   369,    86,    87,     0,    88,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,   353,   354,   355,   356,   357,   358,   359,     0,
       0,     0,     0,   360,   361,   362,   363,   364,   365,   366,
     367,   368,     0,     0,     0,   371,     0,   369,     0,     0,
      50,   353,   354,   355,   356,   357,   358,   359,     0,     0,
       0,     0,   360,   361,   362,   363,   364,   365,   366,   367,
     368,     0,     0,     0,   428,     0,   369,     0,     0,    72,
       0,    50,     0,     0,     0,     0,     0,     0,     0,     0,
     353,   354,   355,   356,   357,   358,   359,    79,    80,    81,
      82,   360,   432,   362,   363,   364,   365,   433,   367,   368,
      72,     0,     0,   614,     0,   369,    86,    87,     0,    88,
       0,   353,   354,   355,   356,   357,   358,   359,    79,    80,
      81,    82,   360,   432,   362,   363,   364,   365,   433,   367,
     368,     0,     0,     0,     0,     0,   369,    86,    87,     0,
      88,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   235,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     0,     0,
       0,     0,    50,     0,     0,     0,     0,     0,   221,   236,
     237,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,    50,     0,     0,     0,
       0,    72,     0,     0,     0,     0,     0,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    79,
      80,    81,    82,     0,    83,    72,     0,    50,     0,    84,
       0,     0,     0,     0,     0,   238,     0,   198,    86,    87,
       0,    88,     0,    79,    80,    81,    82,     0,    83,   199,
       0,     0,    50,    84,     0,     0,    72,   200,     0,   222,
       0,     0,    86,    87,     0,    88,   201,   202,   203,   204,
     205,   206,     0,   207,    79,    80,    81,    82,    50,    83,
       0,    72,     0,     0,   192,     0,     0,     0,     0,     0,
     193,     0,     0,    86,    87,     0,    88,     0,     0,    79,
      80,    81,    82,     0,    83,     0,     0,    72,     0,    84,
       0,     0,     0,     0,     0,   265,     0,     0,    86,    87,
     208,    88,     0,     0,     0,    79,    80,    81,    82,     0,
      83,     0,     0,     0,   209,    84,     0,     0,     0,     0,
       0,   426,     0,     0,    86,    87,     0,    88,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,     0,     0,     0,     0,     0,     0,    50,
       0,     0,     0,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,     0,     0,     0,     0,     0,
       0,     0,     0,    50,     0,     0,     0,     0,    72,     0,
     171,   172,   173,   174,   175,     6,     7,     8,     9,    10,
      11,   176,     0,     0,     0,     0,    79,    80,    81,    82,
      50,    83,    72,     0,     0,     0,    84,     0,     0,     0,
       0,     0,   443,     0,     0,    86,    87,     0,    88,     0,
      79,    80,    81,    82,    50,    83,     0,     0,     0,    72,
      84,     0,     0,     0,     0,     0,   472,     0,     0,    86,
      87,     0,    88,     0,     0,     0,     0,    79,    80,    81,
      82,   177,    83,    72,     0,     0,     0,    84,     0,     0,
       0,     0,     0,   618,     0,     0,    86,    87,     0,    88,
       0,    79,    80,    81,    82,     0,    83,     0,     0,     0,
     178,    84,     0,     0,     0,     0,     0,     0,     0,     0,
      86,    87,     0,    88,     0,     0,     0,     0,   179,   180,
     181,   182,     0,   183,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   184,     0,     0,   185,   186,   171,
     172,   173,   174,   175,     6,     7,     8,     9,    10,    11,
     176,     0,     0,     0,     0,     0,     0,     0,     0,   283,
       0,   284,   285,   286,     0,   287,   288,   289,   290,   291,
     292,   293,   294,   295,    27,    28,     0,   296,   297,   298,
     299,   300,     0,   301,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   302,   303,   304,   305,     0,     0,   306,
     307,   308,   309,   310,     0,     0,     0,     0,     0,     0,
     177,     0,     0,   311,   312,     0,     0,     0,   313,   314,
     315,   316,     0,     0,   317,   318,   319,   320,   321,   322,
     323,   324,   325,     0,     0,   326,   327,   328,   329,   178,
     330,     0,     0,   331,     0,   332,   333,     0,   334,   335,
     336,   337,   338,     0,     0,     0,     0,   179,   180,   181,
     182,     0,   183,     0,     0,     0,     0,     0,   353,   354,
     355,   356,   357,   358,   359,     0,   185,   186,   339,   360,
     361,   362,   363,   364,   365,   366,   367,   368,     0,     0,
       0,   442,     0,   369,   353,   354,   355,   356,   357,   358,
     359,     0,     0,     0,     0,   360,   361,   362,   363,   364,
     365,   366,   367,   368,     0,     0,     0,   452,     0,   369,
     353,   354,   355,   356,   357,   358,   359,     0,     0,     0,
       0,   360,   361,   362,   363,   364,   365,   366,   367,   368,
       0,     0,     0,   457,     0,   369,   353,   354,   355,   356,
     357,   358,   359,     0,     0,     0,     0,   360,   361,   362,
     363,   364,   365,   366,   367,   368,     0,     0,     0,   458,
       0,   369,   353,   354,   355,   356,   357,   358,   359,     0,
       0,     0,     0,   360,   361,   362,   363,   364,   365,   366,
     367,   368,     0,     0,     0,   466,     0,   369,   353,   354,
     355,   356,   357,   358,   359,     0,     0,     0,     0,   360,
     361,   362,   363,   364,   365,   366,   367,   368,     0,     0,
       0,   478,     0,   369,   353,   354,   355,   356,   357,   358,
     359,     0,     0,     0,     0,   360,   361,   362,   363,   364,
     365,   366,   367,   368,     0,     0,     0,   479,     0,   369,
     353,   354,   355,   356,   357,   358,   359,     0,     0,     0,
       0,   360,   361,   362,   363,   364,   365,   366,   367,   368,
       0,     0,     0,   537,     0,   369,   353,   354,   355,   356,
     357,   358,   359,     0,     0,     0,     0,   360,   361,   362,
     363,   364,   365,   366,   367,   368,     0,     0,     0,   557,
       0,   369,   353,   354,   355,   356,   357,   358,   359,     0,
       0,     0,     0,   360,   361,   362,   363,   364,   365,   366,
     367,   368,     0,     0,     0,   596,     0,   369,   353,   354,
     355,   356,   357,   358,   359,     0,     0,     0,     0,   360,
     361,   362,   363,   364,   365,   366,   367,   368,     0,     0,
       0,   599,     0,   369,   353,   354,   355,   356,   357,   358,
     359,     0,     0,     0,     0,   360,   361,   362,   363,   364,
     365,   366,   367,   368,     0,     0,     0,   603,     0,   369,
     353,   354,   355,   356,   357,   358,   359,     0,     0,     0,
       0,   360,   361,   362,   363,   364,   365,   366,   367,   368,
       0,     0,     0,   608,     0,   369,   353,   354,   355,   356,
     357,   358,   359,     0,     0,     0,     0,   360,   361,   362,
     363,   364,   365,   366,   367,   368,     0,     0,     0,   609,
       0,   369,   353,   354,   355,   356,   357,   358,   359,     0,
       0,     0,     0,   360,   361,   362,   363,   364,   365,   366,
     367,   368,     0,     0,     0,   617,     0,   369,   353,   354,
     355,   356,   357,   358,   359,     0,     0,     0,     0,   360,
     361,   362,   363,   364,   365,   366,   367,   368,     0,     0,
       0,   620,     0,   369,   353,   354,   355,   356,   357,   358,
     359,     0,     0,     0,     0,   360,   361,   362,   363,   364,
     365,   366,   367,   368,     0,     0,     0,   621,     0,   369,
     353,   354,   355,   356,   357,   358,   359,     0,     0,     0,
       0,   360,   361,   362,   363,   364,   365,   366,   367,   368,
       0,     0,     0,   622,     0,   369,   353,   354,   355,   356,
     357,   358,   359,     0,     0,     0,     0,   360,   361,   362,
     363,   364,   365,   366,   367,   368,     0,     0,     0,   623,
       0,   369,   353,   354,   355,   356,   357,   358,   359,     0,
       0,     0,     0,   360,   361,   362,   363,   364,   365,   366,
     367,   368,     0,     0,     0,   624,     0,   369,   353,   354,
     355,   356,   357,   358,   359,     0,     0,     0,     0,   360,
     361,   362,   363,   364,   365,   366,   367,   368,     0,     0,
       0,   625,     0,   369,   353,   354,   355,   356,   357,   358,
     359,     0,     0,     0,     0,   360,   361,   362,   363,   364,
     365,   366,   367,   368,     0,     0,     0,   626,     0,   369,
     353,   354,   355,   356,   357,   358,   359,     0,     0,     0,
       0,   360,   361,   362,   363,   364,   365,   366,   367,   368,
       0,     0,     0,   627,     0,   369,   353,   354,   355,   356,
     357,   358,   359,     0,     0,     0,     0,   360,   361,   362,
     363,   364,   365,   366,   367,   368,     0,     0,     0,   628,
       0,   369,   353,   354,   355,   356,   357,   358,   359,     0,
       0,     0,     0,   360,   361,   362,   363,   364,   365,   366,
     367,   368,     0,     0,     0,   641,     0,   369,   353,   354,
     355,   356,   357,   358,   359,     0,     0,     0,     0,   360,
     361,   362,   363,   364,   365,   366,   367,   368,     0,     0,
       0,   642,     0,   369,   353,   354,   355,   356,   357,   358,
     359,     0,     0,     0,     0,   360,   361,   362,   363,   364,
     365,   366,   367,   368,     0,     0,     0,   643,     0,   369,
     353,   354,   355,   356,   357,   358,   359,     0,     0,     0,
       0,   360,   361,   362,   363,   364,   365,   366,   367,   368,
       0,     0,     0,   645,     0,   369,   353,   354,   355,   356,
     357,   358,   359,     0,     0,     0,     0,   360,   361,   362,
     363,   364,   365,   366,   367,   368,     0,     0,     0,   646,
       0,   369,   353,   354,   355,   356,   357,   358,   359,     0,
       0,     0,     0,   360,   361,   362,   363,   364,   365,   366,
     367,   368,     0,     0,     0,   647,     0,   369,   353,   354,
     355,   356,   357,   358,   359,     0,     0,     0,     0,   360,
     361,   362,   363,   364,   365,   366,   367,   368,     0,     0,
       0,   648,     0,   369,   353,   354,   355,   356,   357,   358,
     359,     0,     0,     0,     0,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   388,   389,   390,     0,     0,   369,
       0,     0,     0,     0,     0,   391,   392,   393,   394,     0,
       0,   395,   396,   397,     0,     0,     0,     0,     0,   398
};

static const yytype_int16 yycheck[] =
{
       0,   183,    19,   185,   186,    19,    19,    50,   137,    19,
     202,   203,   204,    74,    14,    17,    74,    19,    74,    74,
      20,    21,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    17,    74,    74,   106,   103,    39,
     103,    74,    75,    43,    44,    54,    55,    47,    48,    16,
      17,    51,    52,   137,   103,   113,   103,    57,    58,    74,
     103,   103,   103,   103,    64,   137,    74,    50,   137,   137,
     137,    71,   137,    73,   137,    75,   137,   137,   137,   137,
     113,   137,   137,    83,    84,   100,    86,    87,    88,    89,
     137,    74,   137,    74,   137,   137,   103,    74,   137,   137,
     114,   115,   116,   117,   118,   119,   120,    67,    68,    69,
      70,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     137,   137,   103,   137,   137,   139,   139,   137,    74,   137,
     113,   137,   137,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   153,   137,   137,   137,   166,   139,   140,
     141,    74,   143,    65,   137,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    74,   137,   137,
     137,   183,   137,   185,   186,   137,   137,   103,   138,   138,
     138,   138,   192,   138,   138,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,   388,   389,   390,   391,
     392,   393,   394,   395,   396,   397,   398,   138,   138,   138,
     412,   221,   414,   415,   416,   225,   226,   227,   228,   114,
     115,   116,   137,   137,   137,   235,    74,   137,   137,   239,
     125,   126,   127,   128,   244,   137,   131,   132,   133,   137,
     137,    18,   114,   115,   116,    63,   137,   142,   137,    74,
     260,   137,   262,   263,    72,   103,    33,   137,   137,   131,
     132,   133,   272,   137,   137,    74,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   138,   137,   137,   137,
     137,   139,   140,   141,   137,   143,    19,   137,    75,   137,
     137,   137,   137,   137,   122,   123,   137,   137,   137,   117,
     118,   119,   120,   137,   137,    92,    93,   137,   137,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,   139,   137,   137,   137,   137,   137,   137,   137,   117,
     118,   119,   120,   353,   354,   355,   356,   357,   358,   359,
     360,   361,   362,   363,   364,   365,   366,   367,   368,   369,
     137,   139,   137,   137,   137,   375,   376,   377,   378,   379,
     380,   137,   382,   383,   384,   137,   388,   389,   390,   391,
     392,   393,   394,   395,   396,   397,   398,   137,   137,   137,
      74,   114,   115,   116,   117,   118,   119,   120,   137,   137,
     137,   137,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   137,   137,   137,   137,   137,   139,   137,   137,   103,
     430,   137,   432,   433,   434,   137,   114,   115,   116,   117,
     118,   119,   120,   137,   444,   137,   137,   121,   122,   123,
     124,   137,   126,   131,   132,   133,   137,   131,   137,   139,
     137,   139,   462,   137,   464,   465,   140,   141,   137,   143,
     137,   137,     0,   137,   474,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,   137,   137,   137,
      18,    19,    20,    21,    22,   137,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,   137,    36,    37,
      38,    39,    40,   137,    42,    43,   137,   137,   137,   137,
     137,   137,   137,   137,    52,    53,    54,    55,   137,   137,
      58,    59,    60,    61,    62,   137,    64,   137,    66,   137,
     137,   137,   137,    71,    72,    73,    74,    74,    76,    77,
      78,    79,    80,    74,    74,    83,    84,    85,    86,    87,
      88,    89,    90,    91,   137,   137,    94,    95,    96,    97,
     137,    99,   137,   101,   102,   103,   104,   105,   103,   107,
     108,   109,   110,   114,   115,   116,   117,   118,   119,   120,
     137,   137,    74,   121,   122,   123,   124,   137,   126,   137,
     131,   132,   133,   131,   137,   137,    74,   137,   139,   137,
      89,   137,   140,   141,   137,   143,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,   137,   137,
     137,    18,    19,    20,    21,    22,    74,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    36,
      37,    38,    39,    40,    -1,    42,    43,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    52,    53,    54,    55,    -1,
      -1,    58,    59,    60,    61,    62,    -1,    64,    -1,    66,
      -1,    -1,    -1,    -1,    71,    72,    73,    74,    -1,    76,
      77,    78,    79,    80,    -1,    -1,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    19,    -1,    94,    95,    96,
      97,    -1,    99,    -1,   101,   102,   103,   104,   105,    -1,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   121,   122,   123,   124,    -1,   126,
      -1,    -1,    -1,    -1,   131,    -1,    -1,    -1,    -1,    -1,
     137,    -1,    -1,   140,   141,    -1,   143,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     114,   115,   116,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   125,   126,   127,   128,    -1,    -1,   131,   132,   133,
      -1,    -1,    -1,   137,    -1,    -1,    -1,    -1,    74,    -1,
     114,   115,   116,   117,   118,   119,   120,    -1,    -1,    -1,
      -1,   125,   126,   127,   128,   129,   130,   131,   132,   133,
      -1,    -1,    -1,    -1,    -1,   139,    -1,   103,   142,    74,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   103,    -1,
      -1,   137,    -1,   139,   140,   141,    -1,   143,    -1,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,    -1,
      -1,    -1,   137,    -1,   139,   140,   141,    -1,   143,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,   114,   115,   116,   117,   118,   119,   120,    -1,
      -1,    -1,    -1,   125,   126,   127,   128,   129,   130,   131,
     132,   133,    -1,    -1,    -1,   137,    -1,   139,    -1,    -1,
      74,   114,   115,   116,   117,   118,   119,   120,    -1,    -1,
      -1,    -1,   125,   126,   127,   128,   129,   130,   131,   132,
     133,    -1,    -1,    -1,   137,    -1,   139,    -1,    -1,   103,
      -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     103,    -1,    -1,   137,    -1,   139,   140,   141,    -1,   143,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,    -1,    -1,    -1,    -1,    -1,   139,   140,   141,    -1,
     143,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    57,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    74,    -1,    -1,    -1,    -1,    -1,    56,    81,
      82,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    74,    -1,    -1,    -1,
      -1,   103,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,   121,
     122,   123,   124,    -1,   126,   103,    -1,    74,    -1,   131,
      -1,    -1,    -1,    -1,    -1,   137,    -1,    15,   140,   141,
      -1,   143,    -1,   121,   122,   123,   124,    -1,   126,    27,
      -1,    -1,    74,   131,    -1,    -1,   103,    35,    -1,   137,
      -1,    -1,   140,   141,    -1,   143,    44,    45,    46,    47,
      48,    49,    -1,    51,   121,   122,   123,   124,    74,   126,
      -1,   103,    -1,    -1,   131,    -1,    -1,    -1,    -1,    -1,
     137,    -1,    -1,   140,   141,    -1,   143,    -1,    -1,   121,
     122,   123,   124,    -1,   126,    -1,    -1,   103,    -1,   131,
      -1,    -1,    -1,    -1,    -1,   137,    -1,    -1,   140,   141,
      98,   143,    -1,    -1,    -1,   121,   122,   123,   124,    -1,
     126,    -1,    -1,    -1,   112,   131,    -1,    -1,    -1,    -1,
      -1,   137,    -1,    -1,   140,   141,    -1,   143,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    74,
      -1,    -1,    -1,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    74,    -1,    -1,    -1,    -1,   103,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,   121,   122,   123,   124,
      74,   126,   103,    -1,    -1,    -1,   131,    -1,    -1,    -1,
      -1,    -1,   137,    -1,    -1,   140,   141,    -1,   143,    -1,
     121,   122,   123,   124,    74,   126,    -1,    -1,    -1,   103,
     131,    -1,    -1,    -1,    -1,    -1,   137,    -1,    -1,   140,
     141,    -1,   143,    -1,    -1,    -1,    -1,   121,   122,   123,
     124,    74,   126,   103,    -1,    -1,    -1,   131,    -1,    -1,
      -1,    -1,    -1,   137,    -1,    -1,   140,   141,    -1,   143,
      -1,   121,   122,   123,   124,    -1,   126,    -1,    -1,    -1,
     103,   131,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     140,   141,    -1,   143,    -1,    -1,    -1,    -1,   121,   122,
     123,   124,    -1,   126,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   137,    -1,    -1,   140,   141,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,
      -1,    20,    21,    22,    -1,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    -1,    36,    37,    38,
      39,    40,    -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    53,    54,    55,    -1,    -1,    58,
      59,    60,    61,    62,    -1,    -1,    -1,    -1,    -1,    -1,
      74,    -1,    -1,    72,    73,    -1,    -1,    -1,    77,    78,
      79,    80,    -1,    -1,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    -1,    -1,    94,    95,    96,    97,   103,
      99,    -1,    -1,   102,    -1,   104,   105,    -1,   107,   108,
     109,   110,   111,    -1,    -1,    -1,    -1,   121,   122,   123,
     124,    -1,   126,    -1,    -1,    -1,    -1,    -1,   114,   115,
     116,   117,   118,   119,   120,    -1,   140,   141,   137,   125,
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
     126,   127,   128,   129,   130,   131,   132,   133,    -1,    -1,
      -1,   137,    -1,   139,   114,   115,   116,   117,   118,   119,
     120,    -1,    -1,    -1,    -1,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   114,   115,   116,    -1,    -1,   139,
      -1,    -1,    -1,    -1,    -1,   125,   126,   127,   128,    -1,
      -1,   131,   132,   133,    -1,    -1,    -1,    -1,    -1,   139
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    18,    19,    20,    21,    22,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    36,
      37,    38,    39,    40,    42,    43,    52,    53,    54,    55,
      58,    59,    60,    61,    62,    64,    66,    71,    72,    73,
      74,    76,    77,    78,    79,    80,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    94,    95,    96,    97,    99,
     101,   102,   103,   104,   105,   107,   108,   109,   110,   121,
     122,   123,   124,   126,   131,   137,   140,   141,   143,   145,
     146,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   211,    19,   137,   211,    50,   103,   137,   137,
       3,     4,     5,     6,     7,    63,    72,   122,   123,   149,
     137,     3,     4,     5,     6,     7,    14,    74,   103,   121,
     122,   123,   124,   126,   137,   140,   141,   149,   210,    74,
     137,   211,   131,   137,   211,   103,   103,   103,    15,    27,
      35,    44,    45,    46,    47,    48,    49,    51,    98,   112,
     137,   137,   137,   137,   137,   137,   137,   103,   137,   137,
     137,    56,   137,   211,    74,    74,    74,   211,   211,    65,
      67,    68,    69,    70,   211,    57,    81,    82,   137,   211,
      74,    75,   113,   211,   211,    16,    17,   147,   147,   147,
     137,   211,   211,   137,   137,   103,   137,   103,   137,    18,
      33,    75,    92,    93,   137,   137,   211,    17,    50,    74,
     113,   137,    74,   100,    74,   137,    74,   137,    74,   137,
     137,   211,   211,    18,    20,    21,    22,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    36,    37,    38,    39,
      40,    42,    52,    53,    54,    55,    58,    59,    60,    61,
      62,    72,    73,    77,    78,    79,    80,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    94,    95,    96,    97,
      99,   102,   104,   105,   107,   108,   109,   110,   111,   137,
     148,   211,    74,   137,   137,   211,   211,   211,   211,   211,
       0,   146,   137,   114,   115,   116,   117,   118,   119,   120,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   139,
     211,   137,   103,   103,   137,   138,   138,   138,   138,   138,
     138,   147,   138,   138,   138,   210,   210,   210,   114,   115,
     116,   125,   126,   127,   128,   131,   132,   133,   139,    19,
     137,    19,   137,   211,    19,   137,   137,   137,   137,   137,
     137,   137,   176,   211,   176,   176,   176,   137,   137,   137,
      74,   137,    74,   113,   137,   137,   137,   211,   137,    74,
     211,   211,   126,   131,   211,   211,   137,   103,   137,   137,
     137,   137,   137,   137,   211,   137,   138,   137,   211,   137,
     137,   137,   137,   211,   137,   137,   137,   137,   137,   137,
     137,   137,   211,   137,   211,   211,   137,   137,   137,   106,
     137,   137,   137,   211,    74,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   142,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   137,   137,   137,
     211,   211,   211,   211,   211,   211,   137,   211,   211,   211,
     142,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,    74,    74,    19,   137,    74,   176,   176,   176,
     176,   137,    74,   137,    74,   137,   137,   137,   211,   137,
     211,   211,   211,   137,   137,   137,   211,   103,   137,   137,
     137,   211,   137,   211,   137,   211,   137,   137,   137,   211,
     137,   137,   137,   137,   137,   137,   137,   137,   137,    19,
     137,   137,   137,    74,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,    74,
     137,   137
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
     146,   146,   146,   146,   146,   146,   147,   147,   148,   148,
     149,   149,   149,   149,   149,   149,   150,   150,   151,   152,
     153,   153,   153,   153,   153,   153,   154,   155,   156,   157,
     158,   159,   160,   160,   161,   161,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   163,   163,   163,
     163,   164,   164,   165,   165,   166,   167,   168,   168,   169,
     169,   169,   169,   170,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   171,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   173,   174,   174,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   176,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   192,   192,   192,   193,   194,
     195,   196,   197,   198,   199,   199,   199,   199,   199,   200,
     200,   200,   200,   200,   200,   200,   200,   201,   201,   201,
     202,   203,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     205,   206,   207,   208,   209,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211
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
       1,     1,     1,     1,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     2,     2,
       3,     3,     3,     3,     4,     2,     3,     3,     2,     3,
       3,     3,     2,     3,     2,     3,     3,     3,     2,     4,
       4,     4,     5,     5,     5,     2,     3,     3,     4,     4,
       5,     2,     3,     2,     3,     2,     3,     2,     4,     2,
       3,     4,     4,     2,     4,     5,     5,     5,     5,     5,
       5,     5,     5,     5,     2,     5,     7,     2,     3,     5,
       3,     5,     2,     3,     5,     4,     6,     2,     2,     3,
       3,     3,     5,     5,     5,     5,     3,     3,     3,     3,
       3,     4,     3,     4,     5,     4,     5,     0,     1,     2,
       2,     2,     2,     2,     2,     2,     3,     2,     2,     2,
       3,     3,     3,     2,     4,     3,     3,     2,     4,     5,
       4,     5,     4,     3,     3,     4,     3,     3,     3,     2,
       3,     4,     3,     4,     5,     3,     5,     3,     3,     3,
       3,     4,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       3,     3,     2,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     2,     2,     3
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
  case 63: /* command: expression  */
#line 224 "parser.y"
                 { eval_value = (yyvsp[0].uval); }
#line 2465 "y.tab.c"
    break;

  case 65: /* command: '\n'  */
#line 227 "parser.y"
      {
      }
#line 2472 "y.tab.c"
    break;

  case 67: /* BX_TOKEN_TOGGLE_ON_OFF: BX_TOKEN_OFF  */
#line 234 "parser.y"
    { (yyval.bval)=(yyvsp[0].bval); }
#line 2478 "y.tab.c"
    break;

  case 69: /* BX_TOKEN_REGISTERS: BX_TOKEN_REGS  */
#line 240 "parser.y"
    { (yyval.sval)=(yyvsp[0].sval); }
#line 2484 "y.tab.c"
    break;

  case 75: /* BX_TOKEN_SEGREG: BX_TOKEN_GS  */
#line 250 "parser.y"
    { (yyval.uval)=(yyvsp[0].uval); }
#line 2490 "y.tab.c"
    break;

  case 76: /* timebp_command: BX_TOKEN_TIMEBP expression '\n'  */
#line 255 "parser.y"
      {
          bx_dbg_timebp_command(0, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2499 "y.tab.c"
    break;

  case 77: /* timebp_command: BX_TOKEN_TIMEBP_ABSOLUTE expression '\n'  */
#line 260 "parser.y"
      {
          bx_dbg_timebp_command(1, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2508 "y.tab.c"
    break;

  case 78: /* modebp_command: BX_TOKEN_MODEBP '\n'  */
#line 268 "parser.y"
      {
          bx_dbg_modebp_command();
          free((yyvsp[-1].sval));
      }
#line 2517 "y.tab.c"
    break;

  case 79: /* vmexitbp_command: BX_TOKEN_VMEXITBP '\n'  */
#line 276 "parser.y"
      {
          bx_dbg_vmexitbp_command();
          free((yyvsp[-1].sval));
      }
#line 2526 "y.tab.c"
    break;

  case 80: /* show_command: BX_TOKEN_SHOW BX_TOKEN_GENERIC '\n'  */
#line 284 "parser.y"
      {
          bx_dbg_show_command((yyvsp[-1].sval));
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2535 "y.tab.c"
    break;

  case 81: /* show_command: BX_TOKEN_SHOW BX_TOKEN_ALL '\n'  */
#line 289 "parser.y"
      {
          bx_dbg_show_command("all");
          free((yyvsp[-2].sval));
      }
#line 2544 "y.tab.c"
    break;

  case 82: /* show_command: BX_TOKEN_SHOW BX_TOKEN_OFF '\n'  */
#line 294 "parser.y"
      {
          bx_dbg_show_command("off");
          free((yyvsp[-2].sval));
      }
#line 2553 "y.tab.c"
    break;

  case 83: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING '\n'  */
#line 299 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-1].sval), 0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2562 "y.tab.c"
    break;

  case 84: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING BX_TOKEN_XML '\n'  */
#line 304 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-2].sval), 1);
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2571 "y.tab.c"
    break;

  case 85: /* show_command: BX_TOKEN_SHOW '\n'  */
#line 309 "parser.y"
      {
          bx_dbg_show_command(0);
          free((yyvsp[-1].sval));
      }
#line 2580 "y.tab.c"
    break;

  case 86: /* page_command: BX_TOKEN_PAGE expression '\n'  */
#line 317 "parser.y"
      {
          bx_dbg_xlate_address((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2589 "y.tab.c"
    break;

  case 87: /* tlb_command: BX_TOKEN_TLB expression '\n'  */
#line 325 "parser.y"
      {
          bx_dbg_tlb_lookup((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2598 "y.tab.c"
    break;

  case 88: /* ptime_command: BX_TOKEN_PTIME '\n'  */
#line 333 "parser.y"
      {
          bx_dbg_ptime_command();
          free((yyvsp[-1].sval));
      }
#line 2607 "y.tab.c"
    break;

  case 89: /* trace_command: BX_TOKEN_TRACE BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 341 "parser.y"
      {
          bx_dbg_trace_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2616 "y.tab.c"
    break;

  case 90: /* trace_reg_command: BX_TOKEN_TRACEREG BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 349 "parser.y"
      {
          bx_dbg_trace_reg_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2625 "y.tab.c"
    break;

  case 91: /* trace_mem_command: BX_TOKEN_TRACEMEM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 357 "parser.y"
      {
          bx_dbg_trace_mem_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2634 "y.tab.c"
    break;

  case 92: /* print_stack_command: BX_TOKEN_PRINT_STACK '\n'  */
#line 365 "parser.y"
      {
          bx_dbg_print_stack_command(16);
          free((yyvsp[-1].sval));
      }
#line 2643 "y.tab.c"
    break;

  case 93: /* print_stack_command: BX_TOKEN_PRINT_STACK BX_TOKEN_NUMERIC '\n'  */
#line 370 "parser.y"
      {
          bx_dbg_print_stack_command((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2652 "y.tab.c"
    break;

  case 94: /* backtrace_command: BX_TOKEN_BT '\n'  */
#line 378 "parser.y"
      {
        bx_dbg_bt_command(16);
        free((yyvsp[-1].sval));
      }
#line 2661 "y.tab.c"
    break;

  case 95: /* backtrace_command: BX_TOKEN_BT BX_TOKEN_NUMERIC '\n'  */
#line 383 "parser.y"
      {
        bx_dbg_bt_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2670 "y.tab.c"
    break;

  case 96: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_STOP '\n'  */
#line 391 "parser.y"
      {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2679 "y.tab.c"
    break;

  case 97: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_CONTINUE '\n'  */
#line 396 "parser.y"
      {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2688 "y.tab.c"
    break;

  case 98: /* watch_point_command: BX_TOKEN_WATCH '\n'  */
#line 401 "parser.y"
      {
          bx_dbg_print_watchpoints();
          free((yyvsp[-1].sval));
      }
#line 2697 "y.tab.c"
    break;

  case 99: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression '\n'  */
#line 406 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2706 "y.tab.c"
    break;

  case 100: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression '\n'  */
#line 411 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2715 "y.tab.c"
    break;

  case 101: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression '\n'  */
#line 416 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-1].uval), 1); /* BX_WRITE */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2724 "y.tab.c"
    break;

  case 102: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression expression '\n'  */
#line 421 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2733 "y.tab.c"
    break;

  case 103: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression expression '\n'  */
#line 426 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2742 "y.tab.c"
    break;

  case 104: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression expression '\n'  */
#line 431 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_WRITE */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2751 "y.tab.c"
    break;

  case 105: /* watch_point_command: BX_TOKEN_UNWATCH '\n'  */
#line 436 "parser.y"
      {
          bx_dbg_unwatch_all();
          free((yyvsp[-1].sval));
      }
#line 2760 "y.tab.c"
    break;

  case 106: /* watch_point_command: BX_TOKEN_UNWATCH expression '\n'  */
#line 441 "parser.y"
      {
          bx_dbg_unwatch((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2769 "y.tab.c"
    break;

  case 107: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 449 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2778 "y.tab.c"
    break;

  case 108: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING expression '\n'  */
#line 454 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 0, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2787 "y.tab.c"
    break;

  case 109: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING '\n'  */
#line 459 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 1, 0);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2796 "y.tab.c"
    break;

  case 110: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING expression '\n'  */
#line 464 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 1, (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2805 "y.tab.c"
    break;

  case 111: /* set_magic_break_points_command: BX_TOKEN_SET_MAGIC_BREAK_POINTS '\n'  */
#line 472 "parser.y"
      {
        bx_dbg_set_magic_bp_mask(0);
        free((yyvsp[-1].sval));
      }
#line 2814 "y.tab.c"
    break;

  case 112: /* set_magic_break_points_command: BX_TOKEN_SET_MAGIC_BREAK_POINTS BX_TOKEN_STRING '\n'  */
#line 477 "parser.y"
      {
        bx_dbg_set_magic_bp_mask(bx_dbg_get_magic_bp_mask_from_str((yyvsp[-1].sval)));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2823 "y.tab.c"
    break;

  case 113: /* clr_magic_break_points_command: BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS '\n'  */
#line 485 "parser.y"
      {
        bx_dbg_set_magic_bp_mask(0);
        free((yyvsp[-1].sval));
      }
#line 2832 "y.tab.c"
    break;

  case 114: /* clr_magic_break_points_command: BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS BX_TOKEN_STRING '\n'  */
#line 490 "parser.y"
      {
        bx_dbg_clr_magic_bp_mask(bx_dbg_get_magic_bp_mask_from_str((yyvsp[-1].sval)));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2841 "y.tab.c"
    break;

  case 115: /* where_command: BX_TOKEN_WHERE '\n'  */
#line 498 "parser.y"
      {
        bx_dbg_where_command();
        free((yyvsp[-1].sval));
      }
#line 2850 "y.tab.c"
    break;

  case 116: /* print_string_command: BX_TOKEN_PRINT_STRING expression '\n'  */
#line 506 "parser.y"
      {
        bx_dbg_print_string_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2859 "y.tab.c"
    break;

  case 117: /* continue_command: BX_TOKEN_CONTINUE '\n'  */
#line 514 "parser.y"
      {
        bx_dbg_continue_command(1);
        free((yyvsp[-1].sval));
      }
#line 2868 "y.tab.c"
    break;

  case 118: /* continue_command: BX_TOKEN_CONTINUE BX_TOKEN_IF expression '\n'  */
#line 519 "parser.y"
      {
        bx_dbg_continue_command((yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2877 "y.tab.c"
    break;

  case 119: /* stepN_command: BX_TOKEN_STEPN '\n'  */
#line 527 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[-1].sval));
      }
#line 2886 "y.tab.c"
    break;

  case 120: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC '\n'  */
#line 532 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2895 "y.tab.c"
    break;

  case 121: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_ALL BX_TOKEN_NUMERIC '\n'  */
#line 537 "parser.y"
      {
        bx_dbg_stepN_command(-1, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2904 "y.tab.c"
    break;

  case 122: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'  */
#line 542 "parser.y"
      {
        bx_dbg_stepN_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2913 "y.tab.c"
    break;

  case 123: /* step_over_command: BX_TOKEN_STEP_OVER '\n'  */
#line 550 "parser.y"
      {
        bx_dbg_step_over_command();
        free((yyvsp[-1].sval));
      }
#line 2922 "y.tab.c"
    break;

  case 124: /* set_command: BX_TOKEN_SET BX_TOKEN_DISASM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 558 "parser.y"
      {
        bx_dbg_set_auto_disassemble((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2931 "y.tab.c"
    break;

  case 125: /* set_command: BX_TOKEN_SET BX_TOKEN_SYMBOLNAME '=' expression '\n'  */
#line 563 "parser.y"
      {
        bx_dbg_set_symbol_command((yyvsp[-3].sval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2940 "y.tab.c"
    break;

  case 126: /* set_command: BX_TOKEN_SET BX_TOKEN_8BL_REG '=' expression '\n'  */
#line 568 "parser.y"
      {
        bx_dbg_set_reg8l_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2948 "y.tab.c"
    break;

  case 127: /* set_command: BX_TOKEN_SET BX_TOKEN_8BH_REG '=' expression '\n'  */
#line 572 "parser.y"
      {
        bx_dbg_set_reg8h_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2956 "y.tab.c"
    break;

  case 128: /* set_command: BX_TOKEN_SET BX_TOKEN_16B_REG '=' expression '\n'  */
#line 576 "parser.y"
      {
        bx_dbg_set_reg16_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2964 "y.tab.c"
    break;

  case 129: /* set_command: BX_TOKEN_SET BX_TOKEN_32B_REG '=' expression '\n'  */
#line 580 "parser.y"
      {
        bx_dbg_set_reg32_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2972 "y.tab.c"
    break;

  case 130: /* set_command: BX_TOKEN_SET BX_TOKEN_64B_REG '=' expression '\n'  */
#line 584 "parser.y"
      {
        bx_dbg_set_reg64_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2980 "y.tab.c"
    break;

  case 131: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_EIP '=' expression '\n'  */
#line 588 "parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2988 "y.tab.c"
    break;

  case 132: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_RIP '=' expression '\n'  */
#line 592 "parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2996 "y.tab.c"
    break;

  case 133: /* set_command: BX_TOKEN_SET BX_TOKEN_SEGREG '=' expression '\n'  */
#line 596 "parser.y"
      {
        bx_dbg_load_segreg((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 3004 "y.tab.c"
    break;

  case 134: /* breakpoint_command: BX_TOKEN_VBREAKPOINT '\n'  */
#line 603 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3013 "y.tab.c"
    break;

  case 135: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression '\n'  */
#line 608 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].uval), NULL);
        free((yyvsp[-4].sval));
      }
#line 3022 "y.tab.c"
    break;

  case 136: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 613 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-5].uval), (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-6].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3031 "y.tab.c"
    break;

  case 137: /* breakpoint_command: BX_TOKEN_LBREAKPOINT '\n'  */
#line 618 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3040 "y.tab.c"
    break;

  case 138: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression '\n'  */
#line 623 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 3049 "y.tab.c"
    break;

  case 139: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 628 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3058 "y.tab.c"
    break;

  case 140: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING '\n'  */
#line 633 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-1].sval), NULL);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3067 "y.tab.c"
    break;

  case 141: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 638 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-3].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3076 "y.tab.c"
    break;

  case 142: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '\n'  */
#line 643 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3085 "y.tab.c"
    break;

  case 143: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression '\n'  */
#line 648 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 3094 "y.tab.c"
    break;

  case 144: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 653 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3103 "y.tab.c"
    break;

  case 145: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression '\n'  */
#line 658 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-3].sval));
      }
#line 3112 "y.tab.c"
    break;

  case 146: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 663 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-5].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3121 "y.tab.c"
    break;

  case 147: /* blist_command: BX_TOKEN_LIST_BREAK '\n'  */
#line 671 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-1].sval));
      }
#line 3130 "y.tab.c"
    break;

  case 148: /* slist_command: BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 679 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-1].sval));
      }
#line 3139 "y.tab.c"
    break;

  case 149: /* slist_command: BX_TOKEN_LIST_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 684 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 3148 "y.tab.c"
    break;

  case 150: /* info_command: BX_TOKEN_INFO BX_TOKEN_PBREAKPOINT '\n'  */
#line 692 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3157 "y.tab.c"
    break;

  case 151: /* info_command: BX_TOKEN_INFO BX_TOKEN_CPU '\n'  */
#line 697 "parser.y"
      {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3166 "y.tab.c"
    break;

  case 152: /* info_command: BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric optional_numeric '\n'  */
#line 702 "parser.y"
      {
        bx_dbg_info_idt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3175 "y.tab.c"
    break;

  case 153: /* info_command: BX_TOKEN_INFO BX_TOKEN_IVT optional_numeric optional_numeric '\n'  */
#line 707 "parser.y"
      {
        bx_dbg_info_ivt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3184 "y.tab.c"
    break;

  case 154: /* info_command: BX_TOKEN_INFO BX_TOKEN_GDT optional_numeric optional_numeric '\n'  */
#line 712 "parser.y"
      {
        bx_dbg_info_gdt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3193 "y.tab.c"
    break;

  case 155: /* info_command: BX_TOKEN_INFO BX_TOKEN_LDT optional_numeric optional_numeric '\n'  */
#line 717 "parser.y"
      {
        bx_dbg_info_ldt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3202 "y.tab.c"
    break;

  case 156: /* info_command: BX_TOKEN_INFO BX_TOKEN_TAB '\n'  */
#line 722 "parser.y"
      {
        bx_dbg_dump_table();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3211 "y.tab.c"
    break;

  case 157: /* info_command: BX_TOKEN_INFO BX_TOKEN_TSS '\n'  */
#line 727 "parser.y"
      {
        bx_dbg_info_tss_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3220 "y.tab.c"
    break;

  case 158: /* info_command: BX_TOKEN_INFO BX_TOKEN_FLAGS '\n'  */
#line 732 "parser.y"
      {
        bx_dbg_info_flags();
        free((yyvsp[-2].sval));
      }
#line 3229 "y.tab.c"
    break;

  case 159: /* info_command: BX_TOKEN_INFO BX_TOKEN_LINUX '\n'  */
#line 737 "parser.y"
      {
        bx_dbg_info_linux_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3238 "y.tab.c"
    break;

  case 160: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS '\n'  */
#line 742 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3247 "y.tab.c"
    break;

  case 161: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 747 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3256 "y.tab.c"
    break;

  case 162: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE '\n'  */
#line 752 "parser.y"
      {
        bx_dbg_info_device("", "");
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3265 "y.tab.c"
    break;

  case 163: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC '\n'  */
#line 757 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3274 "y.tab.c"
    break;

  case 164: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC BX_TOKEN_STRING '\n'  */
#line 762 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3283 "y.tab.c"
    break;

  case 165: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING '\n'  */
#line 767 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3292 "y.tab.c"
    break;

  case 166: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 772 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3301 "y.tab.c"
    break;

  case 167: /* optional_numeric: %empty  */
#line 779 "parser.y"
               { (yyval.uval) = EMPTY_ARG; }
#line 3307 "y.tab.c"
    break;

  case 169: /* regs_command: BX_TOKEN_REGISTERS '\n'  */
#line 784 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3316 "y.tab.c"
    break;

  case 170: /* fpu_regs_command: BX_TOKEN_FPU '\n'  */
#line 792 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[-1].sval));
      }
#line 3325 "y.tab.c"
    break;

  case 171: /* mmx_regs_command: BX_TOKEN_MMX '\n'  */
#line 800 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3334 "y.tab.c"
    break;

  case 172: /* xmm_regs_command: BX_TOKEN_XMM '\n'  */
#line 808 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3343 "y.tab.c"
    break;

  case 173: /* ymm_regs_command: BX_TOKEN_YMM '\n'  */
#line 816 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3352 "y.tab.c"
    break;

  case 174: /* zmm_regs_command: BX_TOKEN_ZMM '\n'  */
#line 824 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3361 "y.tab.c"
    break;

  case 175: /* amx_regs_command: BX_TOKEN_AMX '\n'  */
#line 832 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_AMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3370 "y.tab.c"
    break;

  case 176: /* print_tile_command: BX_TOKEN_TILE BX_TOKEN_NUMERIC '\n'  */
#line 840 "parser.y"
      {
        bx_dbg_print_amx_tile_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3379 "y.tab.c"
    break;

  case 177: /* segment_regs_command: BX_TOKEN_SEGMENT_REGS '\n'  */
#line 848 "parser.y"
      {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3388 "y.tab.c"
    break;

  case 178: /* control_regs_command: BX_TOKEN_CONTROL_REGS '\n'  */
#line 856 "parser.y"
      {
        bx_dbg_info_control_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3397 "y.tab.c"
    break;

  case 179: /* debug_regs_command: BX_TOKEN_DEBUG_REGS '\n'  */
#line 864 "parser.y"
      {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3406 "y.tab.c"
    break;

  case 180: /* delete_command: BX_TOKEN_DEL_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 872 "parser.y"
      {
        bx_dbg_del_breakpoint_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3415 "y.tab.c"
    break;

  case 181: /* bpe_command: BX_TOKEN_ENABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 880 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3424 "y.tab.c"
    break;

  case 182: /* bpd_command: BX_TOKEN_DISABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 887 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 0);
        free((yyvsp[-2].sval));
      }
#line 3433 "y.tab.c"
    break;

  case 183: /* quit_command: BX_TOKEN_QUIT '\n'  */
#line 895 "parser.y"
      {
        bx_dbg_quit_command();
        free((yyvsp[-1].sval));
      }
#line 3442 "y.tab.c"
    break;

  case 184: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT expression '\n'  */
#line 903 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-3].sval), (yyvsp[-2].sval),1, (yyvsp[-1].uval), 1);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3451 "y.tab.c"
    break;

  case 185: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT '\n'  */
#line 908 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), (yyvsp[-1].sval),1, 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3460 "y.tab.c"
    break;

  case 186: /* examine_command: BX_TOKEN_EXAMINE expression '\n'  */
#line 913 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), NULL,0, (yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3469 "y.tab.c"
    break;

  case 187: /* examine_command: BX_TOKEN_EXAMINE '\n'  */
#line 918 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-1].sval), NULL,0, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 3478 "y.tab.c"
    break;

  case 188: /* restore_command: BX_TOKEN_RESTORE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 926 "parser.y"
      {
        bx_dbg_restore_command((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3487 "y.tab.c"
    break;

  case 189: /* writemem_command: BX_TOKEN_WRITEMEM BX_TOKEN_STRING expression expression '\n'  */
#line 934 "parser.y"
      {
        bx_dbg_writemem_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3496 "y.tab.c"
    break;

  case 190: /* loadmem_command: BX_TOKEN_LOADMEM BX_TOKEN_STRING expression '\n'  */
#line 942 "parser.y"
      {
        bx_dbg_loadmem_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3505 "y.tab.c"
    break;

  case 191: /* setpmem_command: BX_TOKEN_SETPMEM expression expression expression '\n'  */
#line 950 "parser.y"
      {
        bx_dbg_setpmem_command((yyvsp[-3].uval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3514 "y.tab.c"
    break;

  case 192: /* deref_command: BX_TOKEN_DEREF expression expression '\n'  */
#line 958 "parser.y"
      {
        bx_dbg_deref_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3523 "y.tab.c"
    break;

  case 193: /* query_command: BX_TOKEN_QUERY BX_TOKEN_PENDING '\n'  */
#line 966 "parser.y"
      {
        bx_dbg_query_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3532 "y.tab.c"
    break;

  case 194: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA '\n'  */
#line 974 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3541 "y.tab.c"
    break;

  case 195: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA BX_TOKEN_NUMERIC '\n'  */
#line 979 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3550 "y.tab.c"
    break;

  case 196: /* take_command: BX_TOKEN_TAKE BX_TOKEN_IRQ '\n'  */
#line 984 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3559 "y.tab.c"
    break;

  case 197: /* take_command: BX_TOKEN_TAKE BX_TOKEN_SMI '\n'  */
#line 989 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3568 "y.tab.c"
    break;

  case 198: /* take_command: BX_TOKEN_TAKE BX_TOKEN_NMI '\n'  */
#line 994 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3577 "y.tab.c"
    break;

  case 199: /* disassemble_command: BX_TOKEN_DISASM '\n'  */
#line 1002 "parser.y"
      {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[-1].sval));
      }
#line 3586 "y.tab.c"
    break;

  case 200: /* disassemble_command: BX_TOKEN_DISASM expression '\n'  */
#line 1007 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3595 "y.tab.c"
    break;

  case 201: /* disassemble_command: BX_TOKEN_DISASM expression expression '\n'  */
#line 1012 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3604 "y.tab.c"
    break;

  case 202: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT '\n'  */
#line 1017 "parser.y"
      {
        bx_dbg_disassemble_current((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3613 "y.tab.c"
    break;

  case 203: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression '\n'  */
#line 1022 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-2].sval), (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3622 "y.tab.c"
    break;

  case 204: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression expression '\n'  */
#line 1027 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3631 "y.tab.c"
    break;

  case 205: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SWITCH_MODE '\n'  */
#line 1032 "parser.y"
      {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3640 "y.tab.c"
    break;

  case 206: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SIZE '=' BX_TOKEN_NUMERIC '\n'  */
#line 1037 "parser.y"
      {
        bx_dbg_set_disassemble_size((yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3649 "y.tab.c"
    break;

  case 207: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STOP '\n'  */
#line 1045 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3658 "y.tab.c"
    break;

  case 208: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STRING '\n'  */
#line 1051 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3667 "y.tab.c"
    break;

  case 209: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_GENERIC '\n'  */
#line 1056 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3676 "y.tab.c"
    break;

  case 210: /* doit_command: BX_TOKEN_DOIT expression '\n'  */
#line 1064 "parser.y"
      {
        bx_dbg_doit_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3685 "y.tab.c"
    break;

  case 211: /* crc_command: BX_TOKEN_CRC expression expression '\n'  */
#line 1072 "parser.y"
      {
        bx_dbg_crc_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3694 "y.tab.c"
    break;

  case 212: /* help_command: BX_TOKEN_HELP BX_TOKEN_QUIT '\n'  */
#line 1080 "parser.y"
       {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3703 "y.tab.c"
    break;

  case 213: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTINUE '\n'  */
#line 1085 "parser.y"
       {
         dbg_printf("c|cont|continue - continue executing\n");
         dbg_printf("c|cont|continue if \"expression\" - continue executing only if expression is true\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3713 "y.tab.c"
    break;

  case 214: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEPN '\n'  */
#line 1091 "parser.y"
       {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3724 "y.tab.c"
    break;

  case 215: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEP_OVER '\n'  */
#line 1098 "parser.y"
       {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3733 "y.tab.c"
    break;

  case 216: /* help_command: BX_TOKEN_HELP BX_TOKEN_VBREAKPOINT '\n'  */
#line 1103 "parser.y"
       {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         dbg_printf("vb|vbreak <seg:offset> if \"expression\" - set a conditional virtual address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3743 "y.tab.c"
    break;

  case 217: /* help_command: BX_TOKEN_HELP BX_TOKEN_LBREAKPOINT '\n'  */
#line 1109 "parser.y"
       {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         dbg_printf("lb|lbreak <addr> if \"expression\" - set a conditional linear address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3753 "y.tab.c"
    break;

  case 218: /* help_command: BX_TOKEN_HELP BX_TOKEN_PBREAKPOINT '\n'  */
#line 1115 "parser.y"
       {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         dbg_printf("p|pb|break|pbreak <addr> if \"expression\" - set a conditional physical address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3763 "y.tab.c"
    break;

  case 219: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEL_BREAKPOINT '\n'  */
#line 1121 "parser.y"
       {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3772 "y.tab.c"
    break;

  case 220: /* help_command: BX_TOKEN_HELP BX_TOKEN_ENABLE_BREAKPOINT '\n'  */
#line 1126 "parser.y"
       {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3781 "y.tab.c"
    break;

  case 221: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISABLE_BREAKPOINT '\n'  */
#line 1131 "parser.y"
       {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3790 "y.tab.c"
    break;

  case 222: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_BREAK '\n'  */
#line 1136 "parser.y"
       {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3799 "y.tab.c"
    break;

  case 223: /* help_command: BX_TOKEN_HELP BX_TOKEN_MODEBP '\n'  */
#line 1141 "parser.y"
       {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3808 "y.tab.c"
    break;

  case 224: /* help_command: BX_TOKEN_HELP BX_TOKEN_VMEXITBP '\n'  */
#line 1146 "parser.y"
       {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3817 "y.tab.c"
    break;

  case 225: /* help_command: BX_TOKEN_HELP BX_TOKEN_CRC '\n'  */
#line 1151 "parser.y"
       {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3826 "y.tab.c"
    break;

  case 226: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACE '\n'  */
#line 1156 "parser.y"
       {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3836 "y.tab.c"
    break;

  case 227: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEREG '\n'  */
#line 1162 "parser.y"
       {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3846 "y.tab.c"
    break;

  case 228: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEMEM '\n'  */
#line 1168 "parser.y"
       {
         dbg_printf("trace-mem on  - print all memory accesses occurred during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3856 "y.tab.c"
    break;

  case 229: /* help_command: BX_TOKEN_HELP BX_TOKEN_RESTORE '\n'  */
#line 1174 "parser.y"
       {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3868 "y.tab.c"
    break;

  case 230: /* help_command: BX_TOKEN_HELP BX_TOKEN_PTIME '\n'  */
#line 1182 "parser.y"
       {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3877 "y.tab.c"
    break;

  case 231: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP '\n'  */
#line 1187 "parser.y"
       {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3886 "y.tab.c"
    break;

  case 232: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP_ABSOLUTE '\n'  */
#line 1192 "parser.y"
       {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3895 "y.tab.c"
    break;

  case 233: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STACK '\n'  */
#line 1197 "parser.y"
       {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3904 "y.tab.c"
    break;

  case 234: /* help_command: BX_TOKEN_HELP BX_TOKEN_BT '\n'  */
#line 1202 "parser.y"
       {
         dbg_printf("bt [num_entries] - prints backtrace\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3913 "y.tab.c"
    break;

  case 235: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOAD_SYMBOLS '\n'  */
#line 1207 "parser.y"
       {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3922 "y.tab.c"
    break;

  case 236: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET_MAGIC_BREAK_POINTS '\n'  */
#line 1212 "parser.y"
       {
         dbg_printf("setmagicbps \"cx dx bx sp bp si di\" - set new magic breakpoints. You can specify multiple at once. Using the setmagicbps command without any arguments will disable all of them\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3931 "y.tab.c"
    break;

  case 237: /* help_command: BX_TOKEN_HELP BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS '\n'  */
#line 1217 "parser.y"
       {
         dbg_printf("clrmagicbps \"cx dx bx sp bp si di\" - clear magic breakpoints. You can specify multiple at once. Using the clrmagicbps command without any arguments will disable all of them\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3940 "y.tab.c"
    break;

  case 238: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 1222 "parser.y"
       {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3949 "y.tab.c"
    break;

  case 239: /* help_command: BX_TOKEN_HELP BX_TOKEN_REGISTERS '\n'  */
#line 1227 "parser.y"
       {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3958 "y.tab.c"
    break;

  case 240: /* help_command: BX_TOKEN_HELP BX_TOKEN_FPU '\n'  */
#line 1232 "parser.y"
       {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3967 "y.tab.c"
    break;

  case 241: /* help_command: BX_TOKEN_HELP BX_TOKEN_MMX '\n'  */
#line 1237 "parser.y"
       {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3976 "y.tab.c"
    break;

  case 242: /* help_command: BX_TOKEN_HELP BX_TOKEN_XMM '\n'  */
#line 1242 "parser.y"
       {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3985 "y.tab.c"
    break;

  case 243: /* help_command: BX_TOKEN_HELP BX_TOKEN_YMM '\n'  */
#line 1247 "parser.y"
       {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3994 "y.tab.c"
    break;

  case 244: /* help_command: BX_TOKEN_HELP BX_TOKEN_ZMM '\n'  */
#line 1252 "parser.y"
       {
         dbg_printf("zmm - print AVX-512 state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4003 "y.tab.c"
    break;

  case 245: /* help_command: BX_TOKEN_HELP BX_TOKEN_AMX '\n'  */
#line 1257 "parser.y"
       {
         dbg_printf("amx - print AMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4012 "y.tab.c"
    break;

  case 246: /* help_command: BX_TOKEN_HELP BX_TOKEN_SEGMENT_REGS '\n'  */
#line 1262 "parser.y"
       {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4021 "y.tab.c"
    break;

  case 247: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTROL_REGS '\n'  */
#line 1267 "parser.y"
       {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4030 "y.tab.c"
    break;

  case 248: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEBUG_REGS '\n'  */
#line 1272 "parser.y"
       {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4039 "y.tab.c"
    break;

  case 249: /* help_command: BX_TOKEN_HELP BX_TOKEN_WRITEMEM '\n'  */
#line 1277 "parser.y"
       {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4048 "y.tab.c"
    break;

  case 250: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOADMEM '\n'  */
#line 1282 "parser.y"
       {
         dbg_printf("loadmem <filename> <laddr> - load file bytes to virtual memory starting from the linear address 'laddr'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4057 "y.tab.c"
    break;

  case 251: /* help_command: BX_TOKEN_HELP BX_TOKEN_SETPMEM '\n'  */
#line 1287 "parser.y"
       {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4066 "y.tab.c"
    break;

  case 252: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEREF '\n'  */
#line 1292 "parser.y"
       {
         dbg_printf("deref <addr> <deep> - pointer dereference. For example: get value of [[[rax]]] or ***rax: deref rax 3\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4075 "y.tab.c"
    break;

  case 253: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISASM '\n'  */
#line 1297 "parser.y"
       {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm hex on/off - control disasm offsets and displacements format\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4089 "y.tab.c"
    break;

  case 254: /* help_command: BX_TOKEN_HELP BX_TOKEN_WATCH '\n'  */
#line 1307 "parser.y"
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
#line 4104 "y.tab.c"
    break;

  case 255: /* help_command: BX_TOKEN_HELP BX_TOKEN_UNWATCH '\n'  */
#line 1318 "parser.y"
       {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4114 "y.tab.c"
    break;

  case 256: /* help_command: BX_TOKEN_HELP BX_TOKEN_EXAMINE '\n'  */
#line 1324 "parser.y"
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
#line 4131 "y.tab.c"
    break;

  case 257: /* help_command: BX_TOKEN_HELP BX_TOKEN_INSTRUMENT '\n'  */
#line 1337 "parser.y"
       {
         dbg_printf("instrument <command|\"string command\"> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command|\"string command\">\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4140 "y.tab.c"
    break;

  case 258: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET '\n'  */
#line 1342 "parser.y"
       {
         dbg_printf("set <regname> = <expr> - set register value to expression\n");
         dbg_printf("set eflags = <expr> - set eflags value to expression, not all flags can be modified\n");
         dbg_printf("set $cpu = <N> - move debugger control to cpu <N> in SMP simulation\n");
         dbg_printf("set $auto_disassemble = 1 - cause debugger to disassemble current instruction\n");
         dbg_printf("       every time execution stops\n");
         dbg_printf("set u|disasm|disassemble on  - same as 'set $auto_disassemble = 1'\n");
         dbg_printf("set u|disasm|disassemble off - same as 'set $auto_disassemble = 0'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4155 "y.tab.c"
    break;

  case 259: /* help_command: BX_TOKEN_HELP BX_TOKEN_PAGE '\n'  */
#line 1353 "parser.y"
       {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4164 "y.tab.c"
    break;

  case 260: /* help_command: BX_TOKEN_HELP BX_TOKEN_INFO '\n'  */
#line 1358 "parser.y"
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
#line 4184 "y.tab.c"
    break;

  case 261: /* help_command: BX_TOKEN_HELP BX_TOKEN_SHOW '\n'  */
#line 1374 "parser.y"
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
#line 4204 "y.tab.c"
    break;

  case 262: /* help_command: BX_TOKEN_HELP BX_TOKEN_CALC '\n'  */
#line 1390 "parser.y"
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
#line 4220 "y.tab.c"
    break;

  case 263: /* help_command: BX_TOKEN_HELP BX_TOKEN_ADDLYT '\n'  */
#line 1402 "parser.y"
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
#line 4236 "y.tab.c"
    break;

  case 264: /* help_command: BX_TOKEN_HELP BX_TOKEN_REMLYT '\n'  */
#line 1414 "parser.y"
       {
         dbg_printf("remlyt - stops debugger to execute the script file added previously with addlyt command.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4245 "y.tab.c"
    break;

  case 265: /* help_command: BX_TOKEN_HELP BX_TOKEN_LYT '\n'  */
#line 1419 "parser.y"
       {
         dbg_printf("lyt - cause debugger to execute script file added previously with addlyt command.\n");
         dbg_printf("    Use it as a refresh/context.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4255 "y.tab.c"
    break;

  case 266: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STRING '\n'  */
#line 1425 "parser.y"
       {
         dbg_printf("print-string <addr> - prints a null-ended string from a linear address.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4264 "y.tab.c"
    break;

  case 267: /* help_command: BX_TOKEN_HELP BX_TOKEN_SOURCE '\n'  */
#line 1430 "parser.y"
       {
         dbg_printf("source <file> - cause debugger to execute a script file.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4273 "y.tab.c"
    break;

  case 268: /* help_command: BX_TOKEN_HELP BX_TOKEN_HELP '\n'  */
#line 1435 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4282 "y.tab.c"
    break;

  case 269: /* help_command: BX_TOKEN_HELP '\n'  */
#line 1440 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-1].sval));
       }
#line 4291 "y.tab.c"
    break;

  case 270: /* calc_command: BX_TOKEN_CALC expression '\n'  */
#line 1448 "parser.y"
   {
     eval_value = (yyvsp[-1].uval);
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4301 "y.tab.c"
    break;

  case 271: /* addlyt_command: BX_TOKEN_ADDLYT BX_TOKEN_STRING '\n'  */
#line 1457 "parser.y"
   {
     bx_dbg_addlyt((yyvsp[-1].sval));
     free((yyvsp[-2].sval));
     free((yyvsp[-1].sval));
   }
#line 4311 "y.tab.c"
    break;

  case 272: /* remlyt_command: BX_TOKEN_REMLYT '\n'  */
#line 1466 "parser.y"
   {
     bx_dbg_remlyt();
     free((yyvsp[-1].sval));
   }
#line 4320 "y.tab.c"
    break;

  case 273: /* lyt_command: BX_TOKEN_LYT '\n'  */
#line 1474 "parser.y"
   {
     bx_dbg_lyt();
     free((yyvsp[-1].sval));
   }
#line 4329 "y.tab.c"
    break;

  case 274: /* if_command: BX_TOKEN_IF expression '\n'  */
#line 1482 "parser.y"
   {
     eval_value = (yyvsp[-1].uval) != 0;
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4339 "y.tab.c"
    break;

  case 275: /* vexpression: BX_TOKEN_NUMERIC  */
#line 1491 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4345 "y.tab.c"
    break;

  case 276: /* vexpression: BX_TOKEN_STRING  */
#line 1492 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4351 "y.tab.c"
    break;

  case 277: /* vexpression: BX_TOKEN_8BL_REG  */
#line 1493 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4357 "y.tab.c"
    break;

  case 278: /* vexpression: BX_TOKEN_8BH_REG  */
#line 1494 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4363 "y.tab.c"
    break;

  case 279: /* vexpression: BX_TOKEN_16B_REG  */
#line 1495 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4369 "y.tab.c"
    break;

  case 280: /* vexpression: BX_TOKEN_32B_REG  */
#line 1496 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4375 "y.tab.c"
    break;

  case 281: /* vexpression: BX_TOKEN_64B_REG  */
#line 1497 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4381 "y.tab.c"
    break;

  case 282: /* vexpression: BX_TOKEN_OPMASK_REG  */
#line 1498 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4387 "y.tab.c"
    break;

  case 283: /* vexpression: BX_TOKEN_SEGREG  */
#line 1499 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4393 "y.tab.c"
    break;

  case 284: /* vexpression: BX_TOKEN_REG_IP  */
#line 1500 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4399 "y.tab.c"
    break;

  case 285: /* vexpression: BX_TOKEN_REG_EIP  */
#line 1501 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4405 "y.tab.c"
    break;

  case 286: /* vexpression: BX_TOKEN_REG_RIP  */
#line 1502 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4411 "y.tab.c"
    break;

  case 287: /* vexpression: BX_TOKEN_REG_SSP  */
#line 1503 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4417 "y.tab.c"
    break;

  case 288: /* vexpression: vexpression '+' vexpression  */
#line 1504 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4423 "y.tab.c"
    break;

  case 289: /* vexpression: vexpression '-' vexpression  */
#line 1505 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4429 "y.tab.c"
    break;

  case 290: /* vexpression: vexpression '*' vexpression  */
#line 1506 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4435 "y.tab.c"
    break;

  case 291: /* vexpression: vexpression '/' vexpression  */
#line 1507 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) / (yyvsp[0].uval); }
#line 4441 "y.tab.c"
    break;

  case 292: /* vexpression: vexpression BX_TOKEN_DEREF_CHR vexpression  */
#line 1508 "parser.y"
                                                { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 4447 "y.tab.c"
    break;

  case 293: /* vexpression: vexpression BX_TOKEN_RSHIFT vexpression  */
#line 1509 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4453 "y.tab.c"
    break;

  case 294: /* vexpression: vexpression BX_TOKEN_LSHIFT vexpression  */
#line 1510 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4459 "y.tab.c"
    break;

  case 295: /* vexpression: vexpression '|' vexpression  */
#line 1511 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4465 "y.tab.c"
    break;

  case 296: /* vexpression: vexpression '^' vexpression  */
#line 1512 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4471 "y.tab.c"
    break;

  case 297: /* vexpression: vexpression '&' vexpression  */
#line 1513 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4477 "y.tab.c"
    break;

  case 298: /* vexpression: '!' vexpression  */
#line 1514 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4483 "y.tab.c"
    break;

  case 299: /* vexpression: '-' vexpression  */
#line 1515 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4489 "y.tab.c"
    break;

  case 300: /* vexpression: '(' vexpression ')'  */
#line 1516 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4495 "y.tab.c"
    break;

  case 301: /* expression: BX_TOKEN_NUMERIC  */
#line 1522 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4501 "y.tab.c"
    break;

  case 302: /* expression: BX_TOKEN_STRING  */
#line 1523 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4507 "y.tab.c"
    break;

  case 303: /* expression: BX_TOKEN_8BL_REG  */
#line 1524 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4513 "y.tab.c"
    break;

  case 304: /* expression: BX_TOKEN_8BH_REG  */
#line 1525 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4519 "y.tab.c"
    break;

  case 305: /* expression: BX_TOKEN_16B_REG  */
#line 1526 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4525 "y.tab.c"
    break;

  case 306: /* expression: BX_TOKEN_32B_REG  */
#line 1527 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4531 "y.tab.c"
    break;

  case 307: /* expression: BX_TOKEN_64B_REG  */
#line 1528 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4537 "y.tab.c"
    break;

  case 308: /* expression: BX_TOKEN_OPMASK_REG  */
#line 1529 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4543 "y.tab.c"
    break;

  case 309: /* expression: BX_TOKEN_SEGREG  */
#line 1530 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4549 "y.tab.c"
    break;

  case 310: /* expression: BX_TOKEN_REG_IP  */
#line 1531 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4555 "y.tab.c"
    break;

  case 311: /* expression: BX_TOKEN_REG_EIP  */
#line 1532 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4561 "y.tab.c"
    break;

  case 312: /* expression: BX_TOKEN_REG_RIP  */
#line 1533 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4567 "y.tab.c"
    break;

  case 313: /* expression: BX_TOKEN_REG_SSP  */
#line 1534 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4573 "y.tab.c"
    break;

  case 314: /* expression: expression ':' expression  */
#line 1535 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[-2].uval), (yyvsp[0].uval)); }
#line 4579 "y.tab.c"
    break;

  case 315: /* expression: expression '+' expression  */
#line 1536 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4585 "y.tab.c"
    break;

  case 316: /* expression: expression '-' expression  */
#line 1537 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4591 "y.tab.c"
    break;

  case 317: /* expression: expression '*' expression  */
#line 1538 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4597 "y.tab.c"
    break;

  case 318: /* expression: expression '/' expression  */
#line 1539 "parser.y"
                                     { (yyval.uval) = ((yyvsp[0].uval) != 0) ? (yyvsp[-2].uval) / (yyvsp[0].uval) : 0; }
#line 4603 "y.tab.c"
    break;

  case 319: /* expression: expression BX_TOKEN_DEREF_CHR expression  */
#line 1540 "parser.y"
                                              { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 4609 "y.tab.c"
    break;

  case 320: /* expression: expression BX_TOKEN_RSHIFT expression  */
#line 1541 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4615 "y.tab.c"
    break;

  case 321: /* expression: expression BX_TOKEN_LSHIFT expression  */
#line 1542 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4621 "y.tab.c"
    break;

  case 322: /* expression: expression '|' expression  */
#line 1543 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4627 "y.tab.c"
    break;

  case 323: /* expression: expression '^' expression  */
#line 1544 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4633 "y.tab.c"
    break;

  case 324: /* expression: expression '&' expression  */
#line 1545 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4639 "y.tab.c"
    break;

  case 325: /* expression: expression '>' expression  */
#line 1546 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) > (yyvsp[0].uval); }
#line 4645 "y.tab.c"
    break;

  case 326: /* expression: expression '<' expression  */
#line 1547 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) < (yyvsp[0].uval); }
#line 4651 "y.tab.c"
    break;

  case 327: /* expression: expression BX_TOKEN_EQ expression  */
#line 1548 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) == (yyvsp[0].uval); }
#line 4657 "y.tab.c"
    break;

  case 328: /* expression: expression BX_TOKEN_NE expression  */
#line 1549 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) != (yyvsp[0].uval); }
#line 4663 "y.tab.c"
    break;

  case 329: /* expression: expression BX_TOKEN_LE expression  */
#line 1550 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) <= (yyvsp[0].uval); }
#line 4669 "y.tab.c"
    break;

  case 330: /* expression: expression BX_TOKEN_GE expression  */
#line 1551 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) >= (yyvsp[0].uval); }
#line 4675 "y.tab.c"
    break;

  case 331: /* expression: '!' expression  */
#line 1552 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4681 "y.tab.c"
    break;

  case 332: /* expression: '-' expression  */
#line 1553 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4687 "y.tab.c"
    break;

  case 333: /* expression: '*' expression  */
#line 1554 "parser.y"
                                     { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[0].uval)); }
#line 4693 "y.tab.c"
    break;

  case 334: /* expression: '@' expression  */
#line 1555 "parser.y"
                                     { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[0].uval)); }
#line 4699 "y.tab.c"
    break;

  case 335: /* expression: '(' expression ')'  */
#line 1556 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4705 "y.tab.c"
    break;


#line 4709 "y.tab.c"

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

#line 1559 "parser.y"

#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
