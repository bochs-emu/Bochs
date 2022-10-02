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
    BX_TOKEN_IDT = 297,            /* BX_TOKEN_IDT  */
    BX_TOKEN_IVT = 298,            /* BX_TOKEN_IVT  */
    BX_TOKEN_GDT = 299,            /* BX_TOKEN_GDT  */
    BX_TOKEN_LDT = 300,            /* BX_TOKEN_LDT  */
    BX_TOKEN_TSS = 301,            /* BX_TOKEN_TSS  */
    BX_TOKEN_TAB = 302,            /* BX_TOKEN_TAB  */
    BX_TOKEN_ALL = 303,            /* BX_TOKEN_ALL  */
    BX_TOKEN_LINUX = 304,          /* BX_TOKEN_LINUX  */
    BX_TOKEN_DEBUG_REGS = 305,     /* BX_TOKEN_DEBUG_REGS  */
    BX_TOKEN_CONTROL_REGS = 306,   /* BX_TOKEN_CONTROL_REGS  */
    BX_TOKEN_SEGMENT_REGS = 307,   /* BX_TOKEN_SEGMENT_REGS  */
    BX_TOKEN_EXAMINE = 308,        /* BX_TOKEN_EXAMINE  */
    BX_TOKEN_XFORMAT = 309,        /* BX_TOKEN_XFORMAT  */
    BX_TOKEN_DISFORMAT = 310,      /* BX_TOKEN_DISFORMAT  */
    BX_TOKEN_RESTORE = 311,        /* BX_TOKEN_RESTORE  */
    BX_TOKEN_WRITEMEM = 312,       /* BX_TOKEN_WRITEMEM  */
    BX_TOKEN_LOADMEM = 313,        /* BX_TOKEN_LOADMEM  */
    BX_TOKEN_SETPMEM = 314,        /* BX_TOKEN_SETPMEM  */
    BX_TOKEN_DEREF = 315,          /* BX_TOKEN_DEREF  */
    BX_TOKEN_SYMBOLNAME = 316,     /* BX_TOKEN_SYMBOLNAME  */
    BX_TOKEN_QUERY = 317,          /* BX_TOKEN_QUERY  */
    BX_TOKEN_PENDING = 318,        /* BX_TOKEN_PENDING  */
    BX_TOKEN_TAKE = 319,           /* BX_TOKEN_TAKE  */
    BX_TOKEN_DMA = 320,            /* BX_TOKEN_DMA  */
    BX_TOKEN_IRQ = 321,            /* BX_TOKEN_IRQ  */
    BX_TOKEN_SMI = 322,            /* BX_TOKEN_SMI  */
    BX_TOKEN_NMI = 323,            /* BX_TOKEN_NMI  */
    BX_TOKEN_TLB = 324,            /* BX_TOKEN_TLB  */
    BX_TOKEN_DISASM = 325,         /* BX_TOKEN_DISASM  */
    BX_TOKEN_INSTRUMENT = 326,     /* BX_TOKEN_INSTRUMENT  */
    BX_TOKEN_STRING = 327,         /* BX_TOKEN_STRING  */
    BX_TOKEN_STOP = 328,           /* BX_TOKEN_STOP  */
    BX_TOKEN_DOIT = 329,           /* BX_TOKEN_DOIT  */
    BX_TOKEN_CRC = 330,            /* BX_TOKEN_CRC  */
    BX_TOKEN_TRACE = 331,          /* BX_TOKEN_TRACE  */
    BX_TOKEN_TRACEREG = 332,       /* BX_TOKEN_TRACEREG  */
    BX_TOKEN_TRACEMEM = 333,       /* BX_TOKEN_TRACEMEM  */
    BX_TOKEN_SWITCH_MODE = 334,    /* BX_TOKEN_SWITCH_MODE  */
    BX_TOKEN_SIZE = 335,           /* BX_TOKEN_SIZE  */
    BX_TOKEN_PTIME = 336,          /* BX_TOKEN_PTIME  */
    BX_TOKEN_TIMEBP_ABSOLUTE = 337, /* BX_TOKEN_TIMEBP_ABSOLUTE  */
    BX_TOKEN_TIMEBP = 338,         /* BX_TOKEN_TIMEBP  */
    BX_TOKEN_MODEBP = 339,         /* BX_TOKEN_MODEBP  */
    BX_TOKEN_VMEXITBP = 340,       /* BX_TOKEN_VMEXITBP  */
    BX_TOKEN_PRINT_STACK = 341,    /* BX_TOKEN_PRINT_STACK  */
    BX_TOKEN_BT = 342,             /* BX_TOKEN_BT  */
    BX_TOKEN_WATCH = 343,          /* BX_TOKEN_WATCH  */
    BX_TOKEN_UNWATCH = 344,        /* BX_TOKEN_UNWATCH  */
    BX_TOKEN_READ = 345,           /* BX_TOKEN_READ  */
    BX_TOKEN_WRITE = 346,          /* BX_TOKEN_WRITE  */
    BX_TOKEN_SHOW = 347,           /* BX_TOKEN_SHOW  */
    BX_TOKEN_LOAD_SYMBOLS = 348,   /* BX_TOKEN_LOAD_SYMBOLS  */
    BX_TOKEN_SYMBOLS = 349,        /* BX_TOKEN_SYMBOLS  */
    BX_TOKEN_LIST_SYMBOLS = 350,   /* BX_TOKEN_LIST_SYMBOLS  */
    BX_TOKEN_GLOBAL = 351,         /* BX_TOKEN_GLOBAL  */
    BX_TOKEN_WHERE = 352,          /* BX_TOKEN_WHERE  */
    BX_TOKEN_PRINT_STRING = 353,   /* BX_TOKEN_PRINT_STRING  */
    BX_TOKEN_NUMERIC = 354,        /* BX_TOKEN_NUMERIC  */
    BX_TOKEN_PAGE = 355,           /* BX_TOKEN_PAGE  */
    BX_TOKEN_HELP = 356,           /* BX_TOKEN_HELP  */
    BX_TOKEN_XML = 357,            /* BX_TOKEN_XML  */
    BX_TOKEN_CALC = 358,           /* BX_TOKEN_CALC  */
    BX_TOKEN_ADDLYT = 359,         /* BX_TOKEN_ADDLYT  */
    BX_TOKEN_REMLYT = 360,         /* BX_TOKEN_REMLYT  */
    BX_TOKEN_LYT = 361,            /* BX_TOKEN_LYT  */
    BX_TOKEN_SOURCE = 362,         /* BX_TOKEN_SOURCE  */
    BX_TOKEN_DEVICE = 363,         /* BX_TOKEN_DEVICE  */
    BX_TOKEN_GENERIC = 364,        /* BX_TOKEN_GENERIC  */
    BX_TOKEN_DEREF_CHR = 365,      /* BX_TOKEN_DEREF_CHR  */
    BX_TOKEN_RSHIFT = 366,         /* BX_TOKEN_RSHIFT  */
    BX_TOKEN_LSHIFT = 367,         /* BX_TOKEN_LSHIFT  */
    BX_TOKEN_EQ = 368,             /* BX_TOKEN_EQ  */
    BX_TOKEN_NE = 369,             /* BX_TOKEN_NE  */
    BX_TOKEN_LE = 370,             /* BX_TOKEN_LE  */
    BX_TOKEN_GE = 371,             /* BX_TOKEN_GE  */
    BX_TOKEN_REG_IP = 372,         /* BX_TOKEN_REG_IP  */
    BX_TOKEN_REG_EIP = 373,        /* BX_TOKEN_REG_EIP  */
    BX_TOKEN_REG_RIP = 374,        /* BX_TOKEN_REG_RIP  */
    BX_TOKEN_REG_SSP = 375,        /* BX_TOKEN_REG_SSP  */
    NOT = 376,                     /* NOT  */
    NEG = 377,                     /* NEG  */
    INDIRECT = 378                 /* INDIRECT  */
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
#define BX_TOKEN_IDT 297
#define BX_TOKEN_IVT 298
#define BX_TOKEN_GDT 299
#define BX_TOKEN_LDT 300
#define BX_TOKEN_TSS 301
#define BX_TOKEN_TAB 302
#define BX_TOKEN_ALL 303
#define BX_TOKEN_LINUX 304
#define BX_TOKEN_DEBUG_REGS 305
#define BX_TOKEN_CONTROL_REGS 306
#define BX_TOKEN_SEGMENT_REGS 307
#define BX_TOKEN_EXAMINE 308
#define BX_TOKEN_XFORMAT 309
#define BX_TOKEN_DISFORMAT 310
#define BX_TOKEN_RESTORE 311
#define BX_TOKEN_WRITEMEM 312
#define BX_TOKEN_LOADMEM 313
#define BX_TOKEN_SETPMEM 314
#define BX_TOKEN_DEREF 315
#define BX_TOKEN_SYMBOLNAME 316
#define BX_TOKEN_QUERY 317
#define BX_TOKEN_PENDING 318
#define BX_TOKEN_TAKE 319
#define BX_TOKEN_DMA 320
#define BX_TOKEN_IRQ 321
#define BX_TOKEN_SMI 322
#define BX_TOKEN_NMI 323
#define BX_TOKEN_TLB 324
#define BX_TOKEN_DISASM 325
#define BX_TOKEN_INSTRUMENT 326
#define BX_TOKEN_STRING 327
#define BX_TOKEN_STOP 328
#define BX_TOKEN_DOIT 329
#define BX_TOKEN_CRC 330
#define BX_TOKEN_TRACE 331
#define BX_TOKEN_TRACEREG 332
#define BX_TOKEN_TRACEMEM 333
#define BX_TOKEN_SWITCH_MODE 334
#define BX_TOKEN_SIZE 335
#define BX_TOKEN_PTIME 336
#define BX_TOKEN_TIMEBP_ABSOLUTE 337
#define BX_TOKEN_TIMEBP 338
#define BX_TOKEN_MODEBP 339
#define BX_TOKEN_VMEXITBP 340
#define BX_TOKEN_PRINT_STACK 341
#define BX_TOKEN_BT 342
#define BX_TOKEN_WATCH 343
#define BX_TOKEN_UNWATCH 344
#define BX_TOKEN_READ 345
#define BX_TOKEN_WRITE 346
#define BX_TOKEN_SHOW 347
#define BX_TOKEN_LOAD_SYMBOLS 348
#define BX_TOKEN_SYMBOLS 349
#define BX_TOKEN_LIST_SYMBOLS 350
#define BX_TOKEN_GLOBAL 351
#define BX_TOKEN_WHERE 352
#define BX_TOKEN_PRINT_STRING 353
#define BX_TOKEN_NUMERIC 354
#define BX_TOKEN_PAGE 355
#define BX_TOKEN_HELP 356
#define BX_TOKEN_XML 357
#define BX_TOKEN_CALC 358
#define BX_TOKEN_ADDLYT 359
#define BX_TOKEN_REMLYT 360
#define BX_TOKEN_LYT 361
#define BX_TOKEN_SOURCE 362
#define BX_TOKEN_DEVICE 363
#define BX_TOKEN_GENERIC 364
#define BX_TOKEN_DEREF_CHR 365
#define BX_TOKEN_RSHIFT 366
#define BX_TOKEN_LSHIFT 367
#define BX_TOKEN_EQ 368
#define BX_TOKEN_NE 369
#define BX_TOKEN_LE 370
#define BX_TOKEN_GE 371
#define BX_TOKEN_REG_IP 372
#define BX_TOKEN_REG_EIP 373
#define BX_TOKEN_REG_RIP 374
#define BX_TOKEN_REG_SSP 375
#define NOT 376
#define NEG 377
#define INDIRECT 378

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 14 "parser.y"

  char    *sval;
  Bit64u   uval;
  unsigned bval;

#line 392 "y.tab.c"

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
  YYSYMBOL_BX_TOKEN_IDT = 42,              /* BX_TOKEN_IDT  */
  YYSYMBOL_BX_TOKEN_IVT = 43,              /* BX_TOKEN_IVT  */
  YYSYMBOL_BX_TOKEN_GDT = 44,              /* BX_TOKEN_GDT  */
  YYSYMBOL_BX_TOKEN_LDT = 45,              /* BX_TOKEN_LDT  */
  YYSYMBOL_BX_TOKEN_TSS = 46,              /* BX_TOKEN_TSS  */
  YYSYMBOL_BX_TOKEN_TAB = 47,              /* BX_TOKEN_TAB  */
  YYSYMBOL_BX_TOKEN_ALL = 48,              /* BX_TOKEN_ALL  */
  YYSYMBOL_BX_TOKEN_LINUX = 49,            /* BX_TOKEN_LINUX  */
  YYSYMBOL_BX_TOKEN_DEBUG_REGS = 50,       /* BX_TOKEN_DEBUG_REGS  */
  YYSYMBOL_BX_TOKEN_CONTROL_REGS = 51,     /* BX_TOKEN_CONTROL_REGS  */
  YYSYMBOL_BX_TOKEN_SEGMENT_REGS = 52,     /* BX_TOKEN_SEGMENT_REGS  */
  YYSYMBOL_BX_TOKEN_EXAMINE = 53,          /* BX_TOKEN_EXAMINE  */
  YYSYMBOL_BX_TOKEN_XFORMAT = 54,          /* BX_TOKEN_XFORMAT  */
  YYSYMBOL_BX_TOKEN_DISFORMAT = 55,        /* BX_TOKEN_DISFORMAT  */
  YYSYMBOL_BX_TOKEN_RESTORE = 56,          /* BX_TOKEN_RESTORE  */
  YYSYMBOL_BX_TOKEN_WRITEMEM = 57,         /* BX_TOKEN_WRITEMEM  */
  YYSYMBOL_BX_TOKEN_LOADMEM = 58,          /* BX_TOKEN_LOADMEM  */
  YYSYMBOL_BX_TOKEN_SETPMEM = 59,          /* BX_TOKEN_SETPMEM  */
  YYSYMBOL_BX_TOKEN_DEREF = 60,            /* BX_TOKEN_DEREF  */
  YYSYMBOL_BX_TOKEN_SYMBOLNAME = 61,       /* BX_TOKEN_SYMBOLNAME  */
  YYSYMBOL_BX_TOKEN_QUERY = 62,            /* BX_TOKEN_QUERY  */
  YYSYMBOL_BX_TOKEN_PENDING = 63,          /* BX_TOKEN_PENDING  */
  YYSYMBOL_BX_TOKEN_TAKE = 64,             /* BX_TOKEN_TAKE  */
  YYSYMBOL_BX_TOKEN_DMA = 65,              /* BX_TOKEN_DMA  */
  YYSYMBOL_BX_TOKEN_IRQ = 66,              /* BX_TOKEN_IRQ  */
  YYSYMBOL_BX_TOKEN_SMI = 67,              /* BX_TOKEN_SMI  */
  YYSYMBOL_BX_TOKEN_NMI = 68,              /* BX_TOKEN_NMI  */
  YYSYMBOL_BX_TOKEN_TLB = 69,              /* BX_TOKEN_TLB  */
  YYSYMBOL_BX_TOKEN_DISASM = 70,           /* BX_TOKEN_DISASM  */
  YYSYMBOL_BX_TOKEN_INSTRUMENT = 71,       /* BX_TOKEN_INSTRUMENT  */
  YYSYMBOL_BX_TOKEN_STRING = 72,           /* BX_TOKEN_STRING  */
  YYSYMBOL_BX_TOKEN_STOP = 73,             /* BX_TOKEN_STOP  */
  YYSYMBOL_BX_TOKEN_DOIT = 74,             /* BX_TOKEN_DOIT  */
  YYSYMBOL_BX_TOKEN_CRC = 75,              /* BX_TOKEN_CRC  */
  YYSYMBOL_BX_TOKEN_TRACE = 76,            /* BX_TOKEN_TRACE  */
  YYSYMBOL_BX_TOKEN_TRACEREG = 77,         /* BX_TOKEN_TRACEREG  */
  YYSYMBOL_BX_TOKEN_TRACEMEM = 78,         /* BX_TOKEN_TRACEMEM  */
  YYSYMBOL_BX_TOKEN_SWITCH_MODE = 79,      /* BX_TOKEN_SWITCH_MODE  */
  YYSYMBOL_BX_TOKEN_SIZE = 80,             /* BX_TOKEN_SIZE  */
  YYSYMBOL_BX_TOKEN_PTIME = 81,            /* BX_TOKEN_PTIME  */
  YYSYMBOL_BX_TOKEN_TIMEBP_ABSOLUTE = 82,  /* BX_TOKEN_TIMEBP_ABSOLUTE  */
  YYSYMBOL_BX_TOKEN_TIMEBP = 83,           /* BX_TOKEN_TIMEBP  */
  YYSYMBOL_BX_TOKEN_MODEBP = 84,           /* BX_TOKEN_MODEBP  */
  YYSYMBOL_BX_TOKEN_VMEXITBP = 85,         /* BX_TOKEN_VMEXITBP  */
  YYSYMBOL_BX_TOKEN_PRINT_STACK = 86,      /* BX_TOKEN_PRINT_STACK  */
  YYSYMBOL_BX_TOKEN_BT = 87,               /* BX_TOKEN_BT  */
  YYSYMBOL_BX_TOKEN_WATCH = 88,            /* BX_TOKEN_WATCH  */
  YYSYMBOL_BX_TOKEN_UNWATCH = 89,          /* BX_TOKEN_UNWATCH  */
  YYSYMBOL_BX_TOKEN_READ = 90,             /* BX_TOKEN_READ  */
  YYSYMBOL_BX_TOKEN_WRITE = 91,            /* BX_TOKEN_WRITE  */
  YYSYMBOL_BX_TOKEN_SHOW = 92,             /* BX_TOKEN_SHOW  */
  YYSYMBOL_BX_TOKEN_LOAD_SYMBOLS = 93,     /* BX_TOKEN_LOAD_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_SYMBOLS = 94,          /* BX_TOKEN_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_LIST_SYMBOLS = 95,     /* BX_TOKEN_LIST_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_GLOBAL = 96,           /* BX_TOKEN_GLOBAL  */
  YYSYMBOL_BX_TOKEN_WHERE = 97,            /* BX_TOKEN_WHERE  */
  YYSYMBOL_BX_TOKEN_PRINT_STRING = 98,     /* BX_TOKEN_PRINT_STRING  */
  YYSYMBOL_BX_TOKEN_NUMERIC = 99,          /* BX_TOKEN_NUMERIC  */
  YYSYMBOL_BX_TOKEN_PAGE = 100,            /* BX_TOKEN_PAGE  */
  YYSYMBOL_BX_TOKEN_HELP = 101,            /* BX_TOKEN_HELP  */
  YYSYMBOL_BX_TOKEN_XML = 102,             /* BX_TOKEN_XML  */
  YYSYMBOL_BX_TOKEN_CALC = 103,            /* BX_TOKEN_CALC  */
  YYSYMBOL_BX_TOKEN_ADDLYT = 104,          /* BX_TOKEN_ADDLYT  */
  YYSYMBOL_BX_TOKEN_REMLYT = 105,          /* BX_TOKEN_REMLYT  */
  YYSYMBOL_BX_TOKEN_LYT = 106,             /* BX_TOKEN_LYT  */
  YYSYMBOL_BX_TOKEN_SOURCE = 107,          /* BX_TOKEN_SOURCE  */
  YYSYMBOL_BX_TOKEN_DEVICE = 108,          /* BX_TOKEN_DEVICE  */
  YYSYMBOL_BX_TOKEN_GENERIC = 109,         /* BX_TOKEN_GENERIC  */
  YYSYMBOL_BX_TOKEN_DEREF_CHR = 110,       /* BX_TOKEN_DEREF_CHR  */
  YYSYMBOL_BX_TOKEN_RSHIFT = 111,          /* BX_TOKEN_RSHIFT  */
  YYSYMBOL_BX_TOKEN_LSHIFT = 112,          /* BX_TOKEN_LSHIFT  */
  YYSYMBOL_BX_TOKEN_EQ = 113,              /* BX_TOKEN_EQ  */
  YYSYMBOL_BX_TOKEN_NE = 114,              /* BX_TOKEN_NE  */
  YYSYMBOL_BX_TOKEN_LE = 115,              /* BX_TOKEN_LE  */
  YYSYMBOL_BX_TOKEN_GE = 116,              /* BX_TOKEN_GE  */
  YYSYMBOL_BX_TOKEN_REG_IP = 117,          /* BX_TOKEN_REG_IP  */
  YYSYMBOL_BX_TOKEN_REG_EIP = 118,         /* BX_TOKEN_REG_EIP  */
  YYSYMBOL_BX_TOKEN_REG_RIP = 119,         /* BX_TOKEN_REG_RIP  */
  YYSYMBOL_BX_TOKEN_REG_SSP = 120,         /* BX_TOKEN_REG_SSP  */
  YYSYMBOL_121_ = 121,                     /* '+'  */
  YYSYMBOL_122_ = 122,                     /* '-'  */
  YYSYMBOL_123_ = 123,                     /* '|'  */
  YYSYMBOL_124_ = 124,                     /* '^'  */
  YYSYMBOL_125_ = 125,                     /* '<'  */
  YYSYMBOL_126_ = 126,                     /* '>'  */
  YYSYMBOL_127_ = 127,                     /* '*'  */
  YYSYMBOL_128_ = 128,                     /* '/'  */
  YYSYMBOL_129_ = 129,                     /* '&'  */
  YYSYMBOL_NOT = 130,                      /* NOT  */
  YYSYMBOL_NEG = 131,                      /* NEG  */
  YYSYMBOL_INDIRECT = 132,                 /* INDIRECT  */
  YYSYMBOL_133_n_ = 133,                   /* '\n'  */
  YYSYMBOL_134_ = 134,                     /* '='  */
  YYSYMBOL_135_ = 135,                     /* ':'  */
  YYSYMBOL_136_ = 136,                     /* '!'  */
  YYSYMBOL_137_ = 137,                     /* '('  */
  YYSYMBOL_138_ = 138,                     /* ')'  */
  YYSYMBOL_139_ = 139,                     /* '@'  */
  YYSYMBOL_YYACCEPT = 140,                 /* $accept  */
  YYSYMBOL_commands = 141,                 /* commands  */
  YYSYMBOL_command = 142,                  /* command  */
  YYSYMBOL_BX_TOKEN_TOGGLE_ON_OFF = 143,   /* BX_TOKEN_TOGGLE_ON_OFF  */
  YYSYMBOL_BX_TOKEN_REGISTERS = 144,       /* BX_TOKEN_REGISTERS  */
  YYSYMBOL_BX_TOKEN_SEGREG = 145,          /* BX_TOKEN_SEGREG  */
  YYSYMBOL_timebp_command = 146,           /* timebp_command  */
  YYSYMBOL_modebp_command = 147,           /* modebp_command  */
  YYSYMBOL_vmexitbp_command = 148,         /* vmexitbp_command  */
  YYSYMBOL_show_command = 149,             /* show_command  */
  YYSYMBOL_page_command = 150,             /* page_command  */
  YYSYMBOL_tlb_command = 151,              /* tlb_command  */
  YYSYMBOL_ptime_command = 152,            /* ptime_command  */
  YYSYMBOL_trace_command = 153,            /* trace_command  */
  YYSYMBOL_trace_reg_command = 154,        /* trace_reg_command  */
  YYSYMBOL_trace_mem_command = 155,        /* trace_mem_command  */
  YYSYMBOL_print_stack_command = 156,      /* print_stack_command  */
  YYSYMBOL_backtrace_command = 157,        /* backtrace_command  */
  YYSYMBOL_watch_point_command = 158,      /* watch_point_command  */
  YYSYMBOL_symbol_command = 159,           /* symbol_command  */
  YYSYMBOL_where_command = 160,            /* where_command  */
  YYSYMBOL_print_string_command = 161,     /* print_string_command  */
  YYSYMBOL_continue_command = 162,         /* continue_command  */
  YYSYMBOL_stepN_command = 163,            /* stepN_command  */
  YYSYMBOL_step_over_command = 164,        /* step_over_command  */
  YYSYMBOL_set_command = 165,              /* set_command  */
  YYSYMBOL_breakpoint_command = 166,       /* breakpoint_command  */
  YYSYMBOL_blist_command = 167,            /* blist_command  */
  YYSYMBOL_slist_command = 168,            /* slist_command  */
  YYSYMBOL_info_command = 169,             /* info_command  */
  YYSYMBOL_optional_numeric = 170,         /* optional_numeric  */
  YYSYMBOL_regs_command = 171,             /* regs_command  */
  YYSYMBOL_fpu_regs_command = 172,         /* fpu_regs_command  */
  YYSYMBOL_mmx_regs_command = 173,         /* mmx_regs_command  */
  YYSYMBOL_xmm_regs_command = 174,         /* xmm_regs_command  */
  YYSYMBOL_ymm_regs_command = 175,         /* ymm_regs_command  */
  YYSYMBOL_zmm_regs_command = 176,         /* zmm_regs_command  */
  YYSYMBOL_segment_regs_command = 177,     /* segment_regs_command  */
  YYSYMBOL_control_regs_command = 178,     /* control_regs_command  */
  YYSYMBOL_debug_regs_command = 179,       /* debug_regs_command  */
  YYSYMBOL_delete_command = 180,           /* delete_command  */
  YYSYMBOL_bpe_command = 181,              /* bpe_command  */
  YYSYMBOL_bpd_command = 182,              /* bpd_command  */
  YYSYMBOL_quit_command = 183,             /* quit_command  */
  YYSYMBOL_examine_command = 184,          /* examine_command  */
  YYSYMBOL_restore_command = 185,          /* restore_command  */
  YYSYMBOL_writemem_command = 186,         /* writemem_command  */
  YYSYMBOL_loadmem_command = 187,          /* loadmem_command  */
  YYSYMBOL_setpmem_command = 188,          /* setpmem_command  */
  YYSYMBOL_deref_command = 189,            /* deref_command  */
  YYSYMBOL_query_command = 190,            /* query_command  */
  YYSYMBOL_take_command = 191,             /* take_command  */
  YYSYMBOL_disassemble_command = 192,      /* disassemble_command  */
  YYSYMBOL_instrument_command = 193,       /* instrument_command  */
  YYSYMBOL_doit_command = 194,             /* doit_command  */
  YYSYMBOL_crc_command = 195,              /* crc_command  */
  YYSYMBOL_help_command = 196,             /* help_command  */
  YYSYMBOL_calc_command = 197,             /* calc_command  */
  YYSYMBOL_addlyt_command = 198,           /* addlyt_command  */
  YYSYMBOL_remlyt_command = 199,           /* remlyt_command  */
  YYSYMBOL_lyt_command = 200,              /* lyt_command  */
  YYSYMBOL_if_command = 201,               /* if_command  */
  YYSYMBOL_vexpression = 202,              /* vexpression  */
  YYSYMBOL_expression = 203                /* expression  */
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
#define YYFINAL  333
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2573

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  140
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  64
/* YYNRULES -- Number of rules.  */
#define YYNRULES  322
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  629

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   378


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
     133,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   136,     2,     2,     2,     2,   129,     2,
     137,   138,   127,   121,     2,   122,     2,   128,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   135,     2,
     125,   134,   126,     2,   139,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   124,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   123,     2,     2,     2,     2,     2,
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
     115,   116,   117,   118,   119,   120,   130,   131,   132
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   156,   156,   157,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   224,   225,   230,   231,   236,   237,   238,   239,
     240,   241,   246,   251,   259,   267,   275,   280,   285,   290,
     295,   300,   308,   316,   324,   332,   340,   348,   356,   361,
     369,   374,   382,   387,   392,   397,   402,   407,   412,   417,
     422,   427,   432,   440,   445,   450,   455,   463,   471,   479,
     484,   492,   497,   502,   507,   515,   523,   528,   533,   537,
     541,   545,   549,   553,   557,   561,   568,   573,   578,   583,
     588,   593,   598,   603,   608,   613,   618,   623,   628,   636,
     644,   649,   657,   662,   667,   672,   677,   682,   687,   692,
     697,   702,   707,   712,   717,   722,   727,   732,   737,   745,
     746,   749,   757,   765,   773,   781,   789,   797,   805,   813,
     821,   829,   836,   844,   852,   857,   862,   867,   875,   883,
     891,   899,   907,   915,   923,   928,   933,   938,   943,   951,
     956,   961,   966,   971,   976,   981,   986,   994,  1000,  1005,
    1013,  1021,  1029,  1034,  1040,  1047,  1052,  1058,  1064,  1070,
    1075,  1080,  1085,  1090,  1095,  1100,  1105,  1111,  1117,  1123,
    1131,  1136,  1141,  1146,  1151,  1156,  1161,  1166,  1171,  1176,
    1181,  1186,  1191,  1196,  1201,  1206,  1211,  1216,  1221,  1226,
    1231,  1241,  1252,  1258,  1271,  1276,  1287,  1292,  1308,  1324,
    1336,  1348,  1353,  1359,  1364,  1369,  1374,  1382,  1391,  1400,
    1408,  1416,  1426,  1427,  1428,  1429,  1430,  1431,  1432,  1433,
    1434,  1435,  1436,  1437,  1438,  1439,  1440,  1441,  1442,  1443,
    1444,  1445,  1446,  1447,  1448,  1449,  1450,  1451,  1457,  1458,
    1459,  1460,  1461,  1462,  1463,  1464,  1465,  1466,  1467,  1468,
    1469,  1470,  1471,  1472,  1473,  1474,  1475,  1476,  1477,  1478,
    1479,  1480,  1481,  1482,  1483,  1484,  1485,  1486,  1487,  1488,
    1489,  1490,  1491
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
  "BX_TOKEN_AVX", "BX_TOKEN_IDT", "BX_TOKEN_IVT", "BX_TOKEN_GDT",
  "BX_TOKEN_LDT", "BX_TOKEN_TSS", "BX_TOKEN_TAB", "BX_TOKEN_ALL",
  "BX_TOKEN_LINUX", "BX_TOKEN_DEBUG_REGS", "BX_TOKEN_CONTROL_REGS",
  "BX_TOKEN_SEGMENT_REGS", "BX_TOKEN_EXAMINE", "BX_TOKEN_XFORMAT",
  "BX_TOKEN_DISFORMAT", "BX_TOKEN_RESTORE", "BX_TOKEN_WRITEMEM",
  "BX_TOKEN_LOADMEM", "BX_TOKEN_SETPMEM", "BX_TOKEN_DEREF",
  "BX_TOKEN_SYMBOLNAME", "BX_TOKEN_QUERY", "BX_TOKEN_PENDING",
  "BX_TOKEN_TAKE", "BX_TOKEN_DMA", "BX_TOKEN_IRQ", "BX_TOKEN_SMI",
  "BX_TOKEN_NMI", "BX_TOKEN_TLB", "BX_TOKEN_DISASM", "BX_TOKEN_INSTRUMENT",
  "BX_TOKEN_STRING", "BX_TOKEN_STOP", "BX_TOKEN_DOIT", "BX_TOKEN_CRC",
  "BX_TOKEN_TRACE", "BX_TOKEN_TRACEREG", "BX_TOKEN_TRACEMEM",
  "BX_TOKEN_SWITCH_MODE", "BX_TOKEN_SIZE", "BX_TOKEN_PTIME",
  "BX_TOKEN_TIMEBP_ABSOLUTE", "BX_TOKEN_TIMEBP", "BX_TOKEN_MODEBP",
  "BX_TOKEN_VMEXITBP", "BX_TOKEN_PRINT_STACK", "BX_TOKEN_BT",
  "BX_TOKEN_WATCH", "BX_TOKEN_UNWATCH", "BX_TOKEN_READ", "BX_TOKEN_WRITE",
  "BX_TOKEN_SHOW", "BX_TOKEN_LOAD_SYMBOLS", "BX_TOKEN_SYMBOLS",
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
  "where_command", "print_string_command", "continue_command",
  "stepN_command", "step_over_command", "set_command",
  "breakpoint_command", "blist_command", "slist_command", "info_command",
  "optional_numeric", "regs_command", "fpu_regs_command",
  "mmx_regs_command", "xmm_regs_command", "ymm_regs_command",
  "zmm_regs_command", "segment_regs_command", "control_regs_command",
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

#define YYPACT_NINF (-186)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-321)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     591,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,   -13,  1486,   -41,  -109,   382,  -105,  1527,
     312,  1217,   -61,   -55,   -52,  1065,   -93,  -186,  -186,   -85,
     -82,   -79,   -76,   -72,   -65,   -63,   -60,   124,     3,     4,
       6,  1486,  1486,     9,    75,  1486,  1139,   -50,  -186,  1486,
    1486,    19,    19,    19,   -44,  1486,  1486,   -43,   -42,   -81,
     -69,   222,  1241,    -9,   -53,   -59,   -32,  1486,  -186,  1486,
    1659,  1486,    30,   -30,   -20,  -186,  -186,  -186,  -186,  1486,
    1486,  -186,  1486,  1486,  1486,   454,  -186,   -18,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  2438,  1486,  -186,   832,    15,   -68,
    -186,  -186,   -12,   -11,    10,    12,    13,    22,    19,    23,
      37,    38,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  1539,  -186,  1539,  1539,  -186,
    1506,    -7,  -186,   -17,  1486,  -186,   666,   -16,    -8,    44,
      46,    47,    48,  1486,  1486,  1486,  1486,    49,    50,    52,
     -47,   -56,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  1290,  -186,   859,   114,  1486,  1486,  1064,  1064,    54,
     -67,    55,    56,    57,  1005,  1314,    58,    59,  -186,   155,
      62,    76,    89,  1658,  1064,  -186,  -186,    91,    93,    95,
    -186,  1684,  1710,  -186,  -186,   102,  -186,   106,  -186,   112,
    1486,   115,  1486,  1486,  -186,  -186,  1736,   116,   123,   -73,
     125,  -186,  1387,   120,   126,  -186,  -186,  1762,  1788,   131,
     156,   160,   167,   168,   172,   173,   174,   176,   177,   178,
     181,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     220,   221,   223,   224,   231,   235,   250,   263,   274,   275,
     276,   277,   280,   287,   288,   289,   291,   294,   295,   300,
     302,   303,   304,   305,   307,   308,   309,   317,   320,   322,
     336,   337,  -186,   338,  1814,   344,  -186,  -186,   -54,   -54,
     -54,   694,   -54,  -186,  -186,  -186,  1486,  1486,  1486,  1486,
    1486,  1486,  1486,  1486,  1486,  1486,  1486,  1486,  1486,  1486,
    1486,  1486,  1486,  1840,  -186,   356,   362,  -186,  1486,  1486,
    1486,  1486,  1486,  1486,   363,  1486,  1486,  1486,  -186,  -186,
     109,  1539,  1539,  1539,  1539,  1539,  1539,  1539,  1539,  1539,
    1539,  1539,   136,  -186,   351,  -186,   -14,   425,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  1486,  2438,  1486,  1486,  1486,
    -186,  -186,  -186,   365,  -186,   -46,   -45,  -186,  -186,  1866,
    -186,   366,  1064,  1892,  1486,  1486,  1064,  1918,  -186,   369,
    -186,  -186,  -186,  -186,  -186,  -186,   728,  -186,   404,  -186,
    1944,  -186,  -186,  -186,  -186,  1970,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,   759,  -186,   896,   927,  -186,  -186,
    -186,   375,  -186,  -186,  -186,  1996,  1457,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,    35,    35,    35,
     -54,   -54,   -54,   -54,   290,   290,   290,   290,   290,   290,
      35,    35,    35,  2438,  -186,  -186,  -186,  2022,  2048,  2074,
    2100,  2126,  2152,  -186,  2178,  2204,  2230,  -186,  -186,  -186,
    -186,   270,   270,   270,   270,  -186,  -186,  -186,   175,   376,
     384,   443,  -186,   386,   387,   388,   389,   394,  -186,   400,
    -186,   401,  -186,  -186,  -186,  2256,  -186,   451,    39,  2282,
    -186,  -186,  -186,  2308,   411,  -186,  -186,  -186,  2334,  -186,
    2360,  -186,  2386,  -186,  -186,  -186,  2412,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,   473,  -186,  -186,  -186,
     415,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,   417,  -186,  -186
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
      60,   291,   290,   292,   293,   294,    66,    67,    68,    69,
      70,    71,   295,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    64,    65,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   289,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   288,     0,
       0,     0,     0,     0,     0,   297,   298,   299,   300,     0,
       0,    61,     0,     0,     0,     0,     3,     0,   296,    41,
      42,    43,    49,    47,    48,    40,    37,    38,    39,    44,
      45,    46,    50,    51,    52,     4,     5,     6,     7,     8,
      19,    20,     9,    10,    11,    12,    13,    14,    15,    16,
      18,    17,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    53,    54,
      55,    56,    57,    58,    59,     0,   109,     0,     0,     0,
     111,   115,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   139,   265,   264,   266,   267,   268,   269,   263,
     262,   271,   272,   273,   274,     0,   126,     0,     0,   270,
       0,   289,   129,     0,     0,   134,     0,     0,     0,     0,
       0,     0,     0,   159,   159,   159,   159,     0,     0,     0,
       0,     0,   173,   162,   163,   164,   165,   166,   169,   168,
     167,     0,   177,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   189,     0,
       0,     0,     0,     0,     0,    62,    63,     0,     0,     0,
      84,     0,     0,    74,    75,     0,    88,     0,    90,     0,
       0,     0,     0,     0,    94,   101,     0,     0,     0,     0,
       0,    81,     0,     0,     0,   140,   107,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   256,     0,     0,     0,   259,   260,   319,   320,
     318,     0,   321,     1,     2,   161,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   261,     0,     0,   112,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   286,   285,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   132,     0,   130,   320,     0,   135,   170,
     171,   172,   150,   142,   143,   159,   160,   159,   159,   159,
     149,   148,   151,     0,   152,     0,     0,   154,   175,     0,
     176,     0,     0,     0,     0,     0,     0,     0,   183,     0,
     184,   186,   187,   188,    83,   192,     0,   195,     0,   190,
       0,   198,   197,   199,   200,     0,    85,    86,    87,    73,
      72,    89,    91,    93,     0,    92,     0,     0,   102,    78,
      77,     0,    79,    76,   103,     0,     0,   141,   108,    82,
     203,   204,   205,   245,   212,   206,   207,   208,   209,   210,
     211,   247,   202,   228,   229,   230,   231,   232,   235,   234,
     233,   243,   219,   236,   237,   238,   239,   240,   244,   215,
     216,   217,   218,   220,   222,   221,   213,   214,   223,   224,
     241,   242,   248,   225,   226,   253,   246,   255,   249,   250,
     251,   252,   254,   227,   257,   258,   322,   306,   307,   308,
     314,   315,   316,   317,   302,   303,   309,   310,   313,   312,
     304,   305,   311,   301,   110,   113,   114,     0,     0,     0,
       0,     0,     0,   116,     0,     0,     0,   287,   279,   280,
     281,   275,   276,   282,   283,   277,   278,   284,     0,     0,
       0,     0,   137,     0,     0,     0,     0,     0,   153,     0,
     157,     0,   155,   174,   178,     0,   180,   303,   304,     0,
     182,   185,   193,     0,     0,   191,   201,    95,     0,    96,
       0,    97,     0,    80,   104,   105,     0,   119,   118,   120,
     121,   122,   117,   123,   124,   125,     0,   127,   133,   131,
       0,   136,   144,   145,   146,   147,   158,   156,   179,   181,
     194,   196,    98,    99,   100,   106,     0,   138,   128
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -186,  -186,   471,   -19,   498,    -2,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -185,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -174,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    85,    86,   237,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     395,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   180,   396
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     144,   368,   384,   369,   370,   561,   145,   148,   257,   397,
     398,   399,   382,   264,   147,   161,   405,   179,   245,   262,
     183,   186,   230,   231,   151,   403,   569,   571,   162,   451,
     247,   356,   419,   238,   239,   235,   236,   213,   187,   258,
     202,   217,   218,   263,   188,   224,   229,   189,   203,   233,
     234,   204,   246,   406,   205,   241,   242,   206,   149,   232,
     452,   207,   256,   259,   248,   357,   420,   267,   208,   268,
     209,   324,   219,   210,   265,   214,   215,   407,   216,   328,
     329,   352,   330,   331,   332,   144,   404,   570,   572,   240,
     243,   244,   150,   336,   337,   338,   339,   340,   341,   342,
     260,   266,   325,   326,   343,   344,   345,   346,   347,   348,
     349,   350,   351,   327,   355,   335,   385,   389,   352,   562,
     146,   352,   358,   359,   261,   390,   383,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,   364,
     220,   221,   222,   223,   360,   353,   361,   362,   339,   340,
     341,   342,  -320,  -320,  -320,  -320,   363,   365,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
     352,   366,   367,   179,   352,   179,   179,   391,   211,   392,
     393,   394,   400,   401,   386,   402,   411,   418,   421,   422,
     423,   427,   456,   428,   606,   431,    48,   548,   549,   550,
     551,   552,   553,   554,   555,   556,   557,   558,   559,   432,
     564,   409,   565,   566,   567,   412,   413,   416,   417,   371,
     372,   373,   433,    68,   436,   426,   437,    48,   438,   430,
     374,   375,   376,   377,   435,   441,   378,   379,   380,   442,
     249,    75,    76,    77,    78,   443,    79,   547,   445,   449,
     444,    80,   446,   447,    68,   250,   450,   212,   453,   457,
      82,    83,   455,    84,   460,   336,   337,   338,   339,   340,
     341,   342,    75,    76,    77,    78,   343,   414,   345,   346,
     347,   348,   415,   350,   351,   371,   372,   373,   429,   461,
     352,    82,    83,   462,    84,   251,   374,   375,   376,   377,
     463,   464,   378,   379,   380,   465,   466,   467,   607,   468,
     469,   470,   252,   253,   471,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   517,   518,   519,   520,
     521,   522,   523,   524,   525,   526,   527,   528,   529,   530,
     531,   532,   533,   481,   482,   254,   483,   484,   537,   538,
     539,   540,   541,   542,   485,   544,   545,   546,   486,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     371,   372,   373,   487,   181,   152,   153,   154,   155,   156,
       6,     7,     8,     9,    10,    11,   488,   378,   379,   380,
     336,   337,   338,   339,   340,   341,   342,   489,   490,   491,
     492,    68,   575,   493,   577,   578,   579,   349,   350,   351,
     494,   495,   496,   560,   497,   352,   583,   498,   499,    75,
      76,    77,    78,   500,    79,   501,   502,   503,   504,    80,
     505,   506,   507,   157,   588,   182,   590,   592,    82,    83,
     508,    84,   158,   509,   333,   510,   596,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,   511,
     512,   513,    13,    14,    15,    16,    17,   515,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,   535,
      29,    30,    31,    32,    33,   536,   543,   563,   568,   574,
     159,   160,   581,   584,    34,    35,    36,    37,   593,   608,
      38,    39,    40,    41,    42,   610,    43,   609,    44,   611,
     612,   613,   614,    45,    46,    47,    48,   615,    49,    50,
      51,    52,    53,   616,   617,    54,    55,    56,    57,    58,
      59,    60,    61,    62,   621,   626,    63,    64,   627,    65,
     628,    66,    67,    68,    69,    70,   334,    71,    72,    73,
      74,  -319,  -319,  -319,  -319,  -319,  -319,  -319,   323,     0,
       0,    75,    76,    77,    78,     0,    79,     0,  -319,  -319,
    -319,    80,     0,     0,     0,     0,   352,    81,     0,     0,
      82,    83,     0,    84,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,     0,     0,     0,    13,
      14,    15,    16,    17,     0,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,     0,    29,    30,    31,
      32,    33,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    34,    35,    36,    37,     0,     0,    38,    39,    40,
      41,    42,     0,    43,     0,    44,     0,     0,     0,     0,
      45,    46,    47,    48,     0,    49,    50,    51,    52,    53,
       0,     0,    54,    55,    56,    57,    58,    59,    60,    61,
      62,     0,     0,    63,    64,   387,    65,     0,    66,    67,
      68,    69,    70,     0,    71,    72,    73,    74,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    75,    76,
      77,    78,     0,    79,     0,     0,     0,     0,    80,     0,
       0,     0,     0,     0,    81,     0,     0,    82,    83,     0,
      84,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,     0,   336,   337,   338,   339,
     340,   341,   342,     0,     0,     0,     0,   343,   344,   345,
     346,   347,   348,   349,   350,   351,     0,     0,     0,   388,
      48,   352,     0,     0,   336,   337,   338,   339,   340,   341,
     342,     0,     0,     0,     0,   343,   344,   345,   346,   347,
     348,   349,   350,   351,     0,     0,     0,    68,     0,   352,
       0,    48,   516,     0,     0,     0,     0,     0,   336,   337,
     338,   339,   340,   341,   342,    75,    76,    77,    78,   343,
     414,   345,   346,   347,   348,   415,   350,   351,    68,     0,
       0,   582,     0,   352,    82,    83,     0,    84,     0,   336,
     337,   338,   339,   340,   341,   342,    75,    76,    77,    78,
     343,   414,   345,   346,   347,   348,   415,   350,   351,     0,
       0,     0,   587,     0,   352,    82,    83,     0,    84,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,   336,   337,   338,   339,   340,   341,   342,     0,
       0,     0,     0,   343,   344,   345,   346,   347,   348,   349,
     350,   351,     0,     0,     0,   354,     0,   352,    48,   336,
     337,   338,   339,   340,   341,   342,     0,     0,     0,     0,
     343,   344,   345,   346,   347,   348,   349,   350,   351,     0,
       0,     0,   410,     0,   352,    68,     0,     0,     0,    48,
       0,     0,     0,     0,     0,     0,   336,   337,   338,   339,
     340,   341,   342,    75,    76,    77,    78,   343,   414,   345,
     346,   347,   348,   415,   350,   351,    68,     0,     0,   589,
       0,   352,    82,    83,     0,    84,     0,   336,   337,   338,
     339,   340,   341,   342,    75,    76,    77,    78,   343,   414,
     345,   346,   347,   348,   415,   350,   351,     0,     0,     0,
     591,     0,   352,    82,    83,     0,    84,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
     190,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   191,     0,     0,     0,     0,     0,     0,     0,
     192,     0,     0,     0,     0,     0,     0,   193,   194,   195,
     196,   197,   198,     0,   199,   336,   337,   338,   339,   340,
     341,   342,     0,     0,     0,     0,   343,   344,   345,   346,
     347,   348,   349,   350,   351,     0,    48,     0,   424,     0,
     352,     0,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,     0,     0,     0,     0,   200,
       0,     0,     0,    68,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   201,   336,   337,   338,   339,   340,   341,
     342,    75,    76,    77,    78,   343,   414,   345,   346,   347,
     348,   415,   350,   351,   225,     0,     0,     0,     0,   352,
      82,    83,     0,    84,     0,     0,     0,     0,     0,     0,
       0,    48,     0,     0,     0,     0,     0,     0,   226,   227,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,     0,     0,     0,     0,     0,     0,    68,     0,
       0,     0,     0,     0,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    75,    76,    77,    78,
       0,    79,     0,     0,     0,     0,    80,     0,     0,     0,
       0,     0,   228,     0,     0,    82,    83,     0,    84,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,     0,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,    68,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,     0,     0,     0,    75,    76,    77,    78,     0,    79,
      68,     0,     0,     0,   184,     0,     0,     0,     0,     0,
     185,     0,     0,    82,    83,     0,    84,     0,    75,    76,
      77,    78,    48,    79,     0,     0,     0,     0,    80,     0,
       0,     0,     0,     0,   255,     0,     0,    82,    83,     0,
      84,     0,     0,     0,     0,     0,    48,     0,     0,    68,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,     0,     0,     0,     0,     0,    75,    76,    77,
      78,     0,    79,    68,     0,     0,     0,    80,     0,     0,
       0,     0,     0,   408,     0,     0,    82,    83,     0,    84,
       0,    75,    76,    77,    78,     0,    79,     0,     0,     0,
       0,    80,     0,     0,     0,     0,     0,   425,     0,     0,
      82,    83,     0,    84,     0,     0,     0,     0,     0,    48,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    68,     0,     0,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,     0,    75,    76,    77,    78,     0,    79,
       0,     0,     0,     0,    80,     0,     0,     0,     0,     0,
     454,     0,     0,    82,    83,     0,    84,     0,     0,    48,
     163,   164,   165,   166,   167,     6,     7,     8,     9,    10,
      11,   168,   163,   164,   165,   166,   167,     6,     7,     8,
       9,    10,    11,   168,     0,     0,    68,     0,    48,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    75,    76,    77,    78,     0,    79,
       0,     0,     0,     0,    80,    68,     0,     0,     0,     0,
     595,     0,     0,    82,    83,     0,    84,     0,     0,   169,
       0,     0,     0,    75,    76,    77,    78,     0,    79,     0,
       0,   169,     0,    80,     0,     0,   371,   372,   373,     0,
       0,     0,    82,    83,     0,    84,   170,   374,   375,   376,
     377,     0,     0,   378,   379,   380,     0,     0,   170,     0,
       0,   381,     0,     0,   171,   172,   173,   174,     0,   175,
       0,     0,     0,     0,     0,     0,   171,   172,   173,   174,
     176,   175,     0,   177,   178,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   177,   178,   269,     0,   270,
     271,   272,     0,   273,   274,   275,   276,   277,   278,   279,
     280,   281,    27,    28,     0,   282,   283,   284,   285,   286,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   287,
     288,   289,   290,     0,     0,   291,   292,   293,   294,   295,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   296,
     297,     0,     0,     0,   298,   299,   300,   301,     0,     0,
     302,   303,   304,   305,   306,   307,   308,   309,   310,     0,
       0,   311,   312,     0,   313,     0,     0,   314,     0,   315,
     316,     0,   317,   318,   319,   320,   321,     0,   336,   337,
     338,   339,   340,   341,   342,     0,     0,     0,     0,   343,
     344,   345,   346,   347,   348,   349,   350,   351,     0,     0,
       0,   434,   322,   352,   336,   337,   338,   339,   340,   341,
     342,     0,     0,     0,     0,   343,   344,   345,   346,   347,
     348,   349,   350,   351,     0,     0,     0,   439,     0,   352,
     336,   337,   338,   339,   340,   341,   342,     0,     0,     0,
       0,   343,   344,   345,   346,   347,   348,   349,   350,   351,
       0,     0,     0,   440,     0,   352,   336,   337,   338,   339,
     340,   341,   342,     0,     0,     0,     0,   343,   344,   345,
     346,   347,   348,   349,   350,   351,     0,     0,     0,   448,
       0,   352,   336,   337,   338,   339,   340,   341,   342,     0,
       0,     0,     0,   343,   344,   345,   346,   347,   348,   349,
     350,   351,     0,     0,     0,   458,     0,   352,   336,   337,
     338,   339,   340,   341,   342,     0,     0,     0,     0,   343,
     344,   345,   346,   347,   348,   349,   350,   351,     0,     0,
       0,   459,     0,   352,   336,   337,   338,   339,   340,   341,
     342,     0,     0,     0,     0,   343,   344,   345,   346,   347,
     348,   349,   350,   351,     0,     0,     0,   514,     0,   352,
     336,   337,   338,   339,   340,   341,   342,     0,     0,     0,
       0,   343,   344,   345,   346,   347,   348,   349,   350,   351,
       0,     0,     0,   534,     0,   352,   336,   337,   338,   339,
     340,   341,   342,     0,     0,     0,     0,   343,   344,   345,
     346,   347,   348,   349,   350,   351,     0,     0,     0,   573,
       0,   352,   336,   337,   338,   339,   340,   341,   342,     0,
       0,     0,     0,   343,   344,   345,   346,   347,   348,   349,
     350,   351,     0,     0,     0,   576,     0,   352,   336,   337,
     338,   339,   340,   341,   342,     0,     0,     0,     0,   343,
     344,   345,   346,   347,   348,   349,   350,   351,     0,     0,
       0,   580,     0,   352,   336,   337,   338,   339,   340,   341,
     342,     0,     0,     0,     0,   343,   344,   345,   346,   347,
     348,   349,   350,   351,     0,     0,     0,   585,     0,   352,
     336,   337,   338,   339,   340,   341,   342,     0,     0,     0,
       0,   343,   344,   345,   346,   347,   348,   349,   350,   351,
       0,     0,     0,   586,     0,   352,   336,   337,   338,   339,
     340,   341,   342,     0,     0,     0,     0,   343,   344,   345,
     346,   347,   348,   349,   350,   351,     0,     0,     0,   594,
       0,   352,   336,   337,   338,   339,   340,   341,   342,     0,
       0,     0,     0,   343,   344,   345,   346,   347,   348,   349,
     350,   351,     0,     0,     0,   597,     0,   352,   336,   337,
     338,   339,   340,   341,   342,     0,     0,     0,     0,   343,
     344,   345,   346,   347,   348,   349,   350,   351,     0,     0,
       0,   598,     0,   352,   336,   337,   338,   339,   340,   341,
     342,     0,     0,     0,     0,   343,   344,   345,   346,   347,
     348,   349,   350,   351,     0,     0,     0,   599,     0,   352,
     336,   337,   338,   339,   340,   341,   342,     0,     0,     0,
       0,   343,   344,   345,   346,   347,   348,   349,   350,   351,
       0,     0,     0,   600,     0,   352,   336,   337,   338,   339,
     340,   341,   342,     0,     0,     0,     0,   343,   344,   345,
     346,   347,   348,   349,   350,   351,     0,     0,     0,   601,
       0,   352,   336,   337,   338,   339,   340,   341,   342,     0,
       0,     0,     0,   343,   344,   345,   346,   347,   348,   349,
     350,   351,     0,     0,     0,   602,     0,   352,   336,   337,
     338,   339,   340,   341,   342,     0,     0,     0,     0,   343,
     344,   345,   346,   347,   348,   349,   350,   351,     0,     0,
       0,   603,     0,   352,   336,   337,   338,   339,   340,   341,
     342,     0,     0,     0,     0,   343,   344,   345,   346,   347,
     348,   349,   350,   351,     0,     0,     0,   604,     0,   352,
     336,   337,   338,   339,   340,   341,   342,     0,     0,     0,
       0,   343,   344,   345,   346,   347,   348,   349,   350,   351,
       0,     0,     0,   605,     0,   352,   336,   337,   338,   339,
     340,   341,   342,     0,     0,     0,     0,   343,   344,   345,
     346,   347,   348,   349,   350,   351,     0,     0,     0,   618,
       0,   352,   336,   337,   338,   339,   340,   341,   342,     0,
       0,     0,     0,   343,   344,   345,   346,   347,   348,   349,
     350,   351,     0,     0,     0,   619,     0,   352,   336,   337,
     338,   339,   340,   341,   342,     0,     0,     0,     0,   343,
     344,   345,   346,   347,   348,   349,   350,   351,     0,     0,
       0,   620,     0,   352,   336,   337,   338,   339,   340,   341,
     342,     0,     0,     0,     0,   343,   344,   345,   346,   347,
     348,   349,   350,   351,     0,     0,     0,   622,     0,   352,
     336,   337,   338,   339,   340,   341,   342,     0,     0,     0,
       0,   343,   344,   345,   346,   347,   348,   349,   350,   351,
       0,     0,     0,   623,     0,   352,   336,   337,   338,   339,
     340,   341,   342,     0,     0,     0,     0,   343,   344,   345,
     346,   347,   348,   349,   350,   351,     0,     0,     0,   624,
       0,   352,   336,   337,   338,   339,   340,   341,   342,     0,
       0,     0,     0,   343,   344,   345,   346,   347,   348,   349,
     350,   351,     0,     0,     0,   625,     0,   352,   336,   337,
     338,   339,   340,   341,   342,     0,     0,     0,     0,   343,
     344,   345,   346,   347,   348,   349,   350,   351,     0,     0,
       0,     0,     0,   352
};

static const yytype_int16 yycheck[] =
{
       0,   175,    19,   177,   178,    19,    19,    48,    17,   194,
     195,   196,    19,    72,    14,    17,    72,    19,    99,    72,
      20,    21,    72,    73,   133,    72,    72,    72,   133,   102,
      99,    99,    99,    52,    53,    16,    17,    37,    99,    48,
     133,    41,    42,    96,    99,    45,    46,    99,   133,    49,
      50,   133,   133,   109,   133,    55,    56,   133,    99,   109,
     133,   133,    62,    72,   133,   133,   133,    67,   133,    69,
     133,    71,    63,   133,   133,    72,    72,   133,    72,    79,
      80,   135,    82,    83,    84,    85,   133,   133,   133,   133,
     133,   133,   133,   110,   111,   112,   113,   114,   115,   116,
     109,   133,    72,   133,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   133,    99,   133,   133,   133,   135,   133,
     133,   135,   134,   134,   133,   133,   133,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,   158,
      65,    66,    67,    68,   134,   145,   134,   134,   113,   114,
     115,   116,   113,   114,   115,   116,   134,   134,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     135,   134,   134,   175,   135,   177,   178,   133,    54,   133,
     133,   133,   133,   133,   184,   133,    72,   133,   133,   133,
     133,   133,    72,   134,    19,   133,    72,   371,   372,   373,
     374,   375,   376,   377,   378,   379,   380,   381,    72,   133,
     395,   211,   397,   398,   399,   215,   216,   217,   218,   110,
     111,   112,   133,    99,   133,   225,   133,    72,   133,   229,
     121,   122,   123,   124,   234,   133,   127,   128,   129,   133,
      18,   117,   118,   119,   120,   133,   122,   138,   133,   133,
     250,   127,   252,   253,    99,    33,   133,   133,   133,   133,
     136,   137,   262,   139,   133,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   110,   111,   112,   133,   133,
     135,   136,   137,   133,   139,    73,   121,   122,   123,   124,
     133,   133,   127,   128,   129,   133,   133,   133,   133,   133,
     133,   133,    90,    91,   133,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   336,   337,   338,   339,
     340,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     350,   351,   352,   133,   133,   133,   133,   133,   358,   359,
     360,   361,   362,   363,   133,   365,   366,   367,   133,   371,
     372,   373,   374,   375,   376,   377,   378,   379,   380,   381,
     110,   111,   112,   133,    72,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,   133,   127,   128,   129,
     110,   111,   112,   113,   114,   115,   116,   133,   133,   133,
     133,    99,   412,   133,   414,   415,   416,   127,   128,   129,
     133,   133,   133,    72,   133,   135,   426,   133,   133,   117,
     118,   119,   120,   133,   122,   133,   133,   133,   133,   127,
     133,   133,   133,    61,   444,   133,   446,   447,   136,   137,
     133,   139,    70,   133,     0,   133,   456,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,   133,
     133,   133,    18,    19,    20,    21,    22,   133,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,   133,
      36,    37,    38,    39,    40,   133,   133,    72,   133,   133,
     118,   119,   133,    99,    50,    51,    52,    53,   133,   133,
      56,    57,    58,    59,    60,    72,    62,   133,    64,   133,
     133,   133,   133,    69,    70,    71,    72,   133,    74,    75,
      76,    77,    78,   133,   133,    81,    82,    83,    84,    85,
      86,    87,    88,    89,   133,    72,    92,    93,   133,    95,
     133,    97,    98,    99,   100,   101,    85,   103,   104,   105,
     106,   110,   111,   112,   113,   114,   115,   116,    70,    -1,
      -1,   117,   118,   119,   120,    -1,   122,    -1,   127,   128,
     129,   127,    -1,    -1,    -1,    -1,   135,   133,    -1,    -1,
     136,   137,    -1,   139,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    18,
      19,    20,    21,    22,    -1,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    -1,    36,    37,    38,
      39,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    50,    51,    52,    53,    -1,    -1,    56,    57,    58,
      59,    60,    -1,    62,    -1,    64,    -1,    -1,    -1,    -1,
      69,    70,    71,    72,    -1,    74,    75,    76,    77,    78,
      -1,    -1,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    -1,    -1,    92,    93,    19,    95,    -1,    97,    98,
      99,   100,   101,    -1,   103,   104,   105,   106,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,
     119,   120,    -1,   122,    -1,    -1,    -1,    -1,   127,    -1,
      -1,    -1,    -1,    -1,   133,    -1,    -1,   136,   137,    -1,
     139,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,   110,   111,   112,   113,
     114,   115,   116,    -1,    -1,    -1,    -1,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    -1,    -1,    -1,   133,
      72,   135,    -1,    -1,   110,   111,   112,   113,   114,   115,
     116,    -1,    -1,    -1,    -1,   121,   122,   123,   124,   125,
     126,   127,   128,   129,    -1,    -1,    -1,    99,    -1,   135,
      -1,    72,   138,    -1,    -1,    -1,    -1,    -1,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,    99,    -1,
      -1,   133,    -1,   135,   136,   137,    -1,   139,    -1,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,    -1,
      -1,    -1,   133,    -1,   135,   136,   137,    -1,   139,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,   110,   111,   112,   113,   114,   115,   116,    -1,
      -1,    -1,    -1,   121,   122,   123,   124,   125,   126,   127,
     128,   129,    -1,    -1,    -1,   133,    -1,   135,    72,   110,
     111,   112,   113,   114,   115,   116,    -1,    -1,    -1,    -1,
     121,   122,   123,   124,   125,   126,   127,   128,   129,    -1,
      -1,    -1,   133,    -1,   135,    99,    -1,    -1,    -1,    72,
      -1,    -1,    -1,    -1,    -1,    -1,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    99,    -1,    -1,   133,
      -1,   135,   136,   137,    -1,   139,    -1,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,    -1,    -1,    -1,
     133,    -1,   135,   136,   137,    -1,   139,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    42,    43,    44,
      45,    46,    47,    -1,    49,   110,   111,   112,   113,   114,
     115,   116,    -1,    -1,    -1,    -1,   121,   122,   123,   124,
     125,   126,   127,   128,   129,    -1,    72,    -1,   133,    -1,
     135,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    94,
      -1,    -1,    -1,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,    55,    -1,    -1,    -1,    -1,   135,
     136,   137,    -1,   139,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    72,    -1,    -1,    -1,    -1,    -1,    -1,    79,    80,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    99,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,   117,   118,   119,   120,
      -1,   122,    -1,    -1,    -1,    -1,   127,    -1,    -1,    -1,
      -1,    -1,   133,    -1,    -1,   136,   137,    -1,   139,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,
      -1,    -1,    -1,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    72,    -1,    -1,    99,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,   117,   118,   119,   120,    -1,   122,
      99,    -1,    -1,    -1,   127,    -1,    -1,    -1,    -1,    -1,
     133,    -1,    -1,   136,   137,    -1,   139,    -1,   117,   118,
     119,   120,    72,   122,    -1,    -1,    -1,    -1,   127,    -1,
      -1,    -1,    -1,    -1,   133,    -1,    -1,   136,   137,    -1,
     139,    -1,    -1,    -1,    -1,    -1,    72,    -1,    -1,    99,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,
     120,    -1,   122,    99,    -1,    -1,    -1,   127,    -1,    -1,
      -1,    -1,    -1,   133,    -1,    -1,   136,   137,    -1,   139,
      -1,   117,   118,   119,   120,    -1,   122,    -1,    -1,    -1,
      -1,   127,    -1,    -1,    -1,    -1,    -1,   133,    -1,    -1,
     136,   137,    -1,   139,    -1,    -1,    -1,    -1,    -1,    72,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    99,    -1,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,   117,   118,   119,   120,    -1,   122,
      -1,    -1,    -1,    -1,   127,    -1,    -1,    -1,    -1,    -1,
     133,    -1,    -1,   136,   137,    -1,   139,    -1,    -1,    72,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    99,    -1,    72,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   117,   118,   119,   120,    -1,   122,
      -1,    -1,    -1,    -1,   127,    99,    -1,    -1,    -1,    -1,
     133,    -1,    -1,   136,   137,    -1,   139,    -1,    -1,    72,
      -1,    -1,    -1,   117,   118,   119,   120,    -1,   122,    -1,
      -1,    72,    -1,   127,    -1,    -1,   110,   111,   112,    -1,
      -1,    -1,   136,   137,    -1,   139,    99,   121,   122,   123,
     124,    -1,    -1,   127,   128,   129,    -1,    -1,    99,    -1,
      -1,   135,    -1,    -1,   117,   118,   119,   120,    -1,   122,
      -1,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,   120,
     133,   122,    -1,   136,   137,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   136,   137,    18,    -1,    20,
      21,    22,    -1,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    -1,    36,    37,    38,    39,    40,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,
      51,    52,    53,    -1,    -1,    56,    57,    58,    59,    60,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,
      71,    -1,    -1,    -1,    75,    76,    77,    78,    -1,    -1,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    -1,
      -1,    92,    93,    -1,    95,    -1,    -1,    98,    -1,   100,
     101,    -1,   103,   104,   105,   106,   107,    -1,   110,   111,
     112,   113,   114,   115,   116,    -1,    -1,    -1,    -1,   121,
     122,   123,   124,   125,   126,   127,   128,   129,    -1,    -1,
      -1,   133,   133,   135,   110,   111,   112,   113,   114,   115,
     116,    -1,    -1,    -1,    -1,   121,   122,   123,   124,   125,
     126,   127,   128,   129,    -1,    -1,    -1,   133,    -1,   135,
     110,   111,   112,   113,   114,   115,   116,    -1,    -1,    -1,
      -1,   121,   122,   123,   124,   125,   126,   127,   128,   129,
      -1,    -1,    -1,   133,    -1,   135,   110,   111,   112,   113,
     114,   115,   116,    -1,    -1,    -1,    -1,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    -1,    -1,    -1,   133,
      -1,   135,   110,   111,   112,   113,   114,   115,   116,    -1,
      -1,    -1,    -1,   121,   122,   123,   124,   125,   126,   127,
     128,   129,    -1,    -1,    -1,   133,    -1,   135,   110,   111,
     112,   113,   114,   115,   116,    -1,    -1,    -1,    -1,   121,
     122,   123,   124,   125,   126,   127,   128,   129,    -1,    -1,
      -1,   133,    -1,   135,   110,   111,   112,   113,   114,   115,
     116,    -1,    -1,    -1,    -1,   121,   122,   123,   124,   125,
     126,   127,   128,   129,    -1,    -1,    -1,   133,    -1,   135,
     110,   111,   112,   113,   114,   115,   116,    -1,    -1,    -1,
      -1,   121,   122,   123,   124,   125,   126,   127,   128,   129,
      -1,    -1,    -1,   133,    -1,   135,   110,   111,   112,   113,
     114,   115,   116,    -1,    -1,    -1,    -1,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    -1,    -1,    -1,   133,
      -1,   135,   110,   111,   112,   113,   114,   115,   116,    -1,
      -1,    -1,    -1,   121,   122,   123,   124,   125,   126,   127,
     128,   129,    -1,    -1,    -1,   133,    -1,   135,   110,   111,
     112,   113,   114,   115,   116,    -1,    -1,    -1,    -1,   121,
     122,   123,   124,   125,   126,   127,   128,   129,    -1,    -1,
      -1,   133,    -1,   135,   110,   111,   112,   113,   114,   115,
     116,    -1,    -1,    -1,    -1,   121,   122,   123,   124,   125,
     126,   127,   128,   129,    -1,    -1,    -1,   133,    -1,   135,
     110,   111,   112,   113,   114,   115,   116,    -1,    -1,    -1,
      -1,   121,   122,   123,   124,   125,   126,   127,   128,   129,
      -1,    -1,    -1,   133,    -1,   135,   110,   111,   112,   113,
     114,   115,   116,    -1,    -1,    -1,    -1,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    -1,    -1,    -1,   133,
      -1,   135,   110,   111,   112,   113,   114,   115,   116,    -1,
      -1,    -1,    -1,   121,   122,   123,   124,   125,   126,   127,
     128,   129,    -1,    -1,    -1,   133,    -1,   135,   110,   111,
     112,   113,   114,   115,   116,    -1,    -1,    -1,    -1,   121,
     122,   123,   124,   125,   126,   127,   128,   129,    -1,    -1,
      -1,   133,    -1,   135,   110,   111,   112,   113,   114,   115,
     116,    -1,    -1,    -1,    -1,   121,   122,   123,   124,   125,
     126,   127,   128,   129,    -1,    -1,    -1,   133,    -1,   135,
     110,   111,   112,   113,   114,   115,   116,    -1,    -1,    -1,
      -1,   121,   122,   123,   124,   125,   126,   127,   128,   129,
      -1,    -1,    -1,   133,    -1,   135,   110,   111,   112,   113,
     114,   115,   116,    -1,    -1,    -1,    -1,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    -1,    -1,    -1,   133,
      -1,   135,   110,   111,   112,   113,   114,   115,   116,    -1,
      -1,    -1,    -1,   121,   122,   123,   124,   125,   126,   127,
     128,   129,    -1,    -1,    -1,   133,    -1,   135,   110,   111,
     112,   113,   114,   115,   116,    -1,    -1,    -1,    -1,   121,
     122,   123,   124,   125,   126,   127,   128,   129,    -1,    -1,
      -1,   133,    -1,   135,   110,   111,   112,   113,   114,   115,
     116,    -1,    -1,    -1,    -1,   121,   122,   123,   124,   125,
     126,   127,   128,   129,    -1,    -1,    -1,   133,    -1,   135,
     110,   111,   112,   113,   114,   115,   116,    -1,    -1,    -1,
      -1,   121,   122,   123,   124,   125,   126,   127,   128,   129,
      -1,    -1,    -1,   133,    -1,   135,   110,   111,   112,   113,
     114,   115,   116,    -1,    -1,    -1,    -1,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    -1,    -1,    -1,   133,
      -1,   135,   110,   111,   112,   113,   114,   115,   116,    -1,
      -1,    -1,    -1,   121,   122,   123,   124,   125,   126,   127,
     128,   129,    -1,    -1,    -1,   133,    -1,   135,   110,   111,
     112,   113,   114,   115,   116,    -1,    -1,    -1,    -1,   121,
     122,   123,   124,   125,   126,   127,   128,   129,    -1,    -1,
      -1,   133,    -1,   135,   110,   111,   112,   113,   114,   115,
     116,    -1,    -1,    -1,    -1,   121,   122,   123,   124,   125,
     126,   127,   128,   129,    -1,    -1,    -1,   133,    -1,   135,
     110,   111,   112,   113,   114,   115,   116,    -1,    -1,    -1,
      -1,   121,   122,   123,   124,   125,   126,   127,   128,   129,
      -1,    -1,    -1,   133,    -1,   135,   110,   111,   112,   113,
     114,   115,   116,    -1,    -1,    -1,    -1,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    -1,    -1,    -1,   133,
      -1,   135,   110,   111,   112,   113,   114,   115,   116,    -1,
      -1,    -1,    -1,   121,   122,   123,   124,   125,   126,   127,
     128,   129,    -1,    -1,    -1,   133,    -1,   135,   110,   111,
     112,   113,   114,   115,   116,    -1,    -1,    -1,    -1,   121,
     122,   123,   124,   125,   126,   127,   128,   129,    -1,    -1,
      -1,    -1,    -1,   135
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    18,    19,    20,    21,    22,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    36,
      37,    38,    39,    40,    50,    51,    52,    53,    56,    57,
      58,    59,    60,    62,    64,    69,    70,    71,    72,    74,
      75,    76,    77,    78,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    92,    93,    95,    97,    98,    99,   100,
     101,   103,   104,   105,   106,   117,   118,   119,   120,   122,
     127,   133,   136,   137,   139,   141,   142,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   203,    19,   133,   203,    48,    99,
     133,   133,     3,     4,     5,     6,     7,    61,    70,   118,
     119,   145,   133,     3,     4,     5,     6,     7,    14,    72,
      99,   117,   118,   119,   120,   122,   133,   136,   137,   145,
     202,    72,   133,   203,   127,   133,   203,    99,    99,    99,
      15,    27,    35,    42,    43,    44,    45,    46,    47,    49,
      94,   108,   133,   133,   133,   133,   133,   133,   133,   133,
     133,    54,   133,   203,    72,    72,    72,   203,   203,    63,
      65,    66,    67,    68,   203,    55,    79,    80,   133,   203,
      72,    73,   109,   203,   203,    16,    17,   143,   143,   143,
     133,   203,   203,   133,   133,    99,   133,    99,   133,    18,
      33,    73,    90,    91,   133,   133,   203,    17,    48,    72,
     109,   133,    72,    96,    72,   133,   133,   203,   203,    18,
      20,    21,    22,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    36,    37,    38,    39,    40,    50,    51,    52,
      53,    56,    57,    58,    59,    60,    70,    71,    75,    76,
      77,    78,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    92,    93,    95,    98,   100,   101,   103,   104,   105,
     106,   107,   133,   144,   203,    72,   133,   133,   203,   203,
     203,   203,   203,     0,   142,   133,   110,   111,   112,   113,
     114,   115,   116,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   135,   203,   133,    99,    99,   133,   134,   134,
     134,   134,   134,   134,   143,   134,   134,   134,   202,   202,
     202,   110,   111,   112,   121,   122,   123,   124,   127,   128,
     129,   135,    19,   133,    19,   133,   203,    19,   133,   133,
     133,   133,   133,   133,   133,   170,   203,   170,   170,   170,
     133,   133,   133,    72,   133,    72,   109,   133,   133,   203,
     133,    72,   203,   203,   122,   127,   203,   203,   133,    99,
     133,   133,   133,   133,   133,   133,   203,   133,   134,   133,
     203,   133,   133,   133,   133,   203,   133,   133,   133,   133,
     133,   133,   133,   133,   203,   133,   203,   203,   133,   133,
     133,   102,   133,   133,   133,   203,    72,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   138,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   133,   133,   133,   203,   203,   203,
     203,   203,   203,   133,   203,   203,   203,   138,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,    72,
      72,    19,   133,    72,   170,   170,   170,   170,   133,    72,
     133,    72,   133,   133,   133,   203,   133,   203,   203,   203,
     133,   133,   133,   203,    99,   133,   133,   133,   203,   133,
     203,   133,   203,   133,   133,   133,   203,   133,   133,   133,
     133,   133,   133,   133,   133,   133,    19,   133,   133,   133,
      72,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,    72,   133,   133
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   140,   141,   141,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
     142,   142,   143,   143,   144,   144,   145,   145,   145,   145,
     145,   145,   146,   146,   147,   148,   149,   149,   149,   149,
     149,   149,   150,   151,   152,   153,   154,   155,   156,   156,
     157,   157,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   159,   159,   159,   159,   160,   161,   162,
     162,   163,   163,   163,   163,   164,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   167,
     168,   168,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   170,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   184,   184,   184,   185,   186,
     187,   188,   189,   190,   191,   191,   191,   191,   191,   192,
     192,   192,   192,   192,   192,   192,   192,   193,   193,   193,
     194,   195,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   197,   198,   199,
     200,   201,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203
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
       0,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     2,     2,     3,     3,     3,     3,
       4,     2,     3,     3,     2,     3,     3,     3,     2,     3,
       2,     3,     3,     3,     2,     4,     4,     4,     5,     5,
       5,     2,     3,     3,     4,     4,     5,     2,     3,     2,
       4,     2,     3,     4,     4,     2,     4,     5,     5,     5,
       5,     5,     5,     5,     5,     5,     2,     5,     7,     2,
       3,     5,     3,     5,     2,     3,     5,     4,     6,     2,
       2,     3,     3,     3,     5,     5,     5,     5,     3,     3,
       3,     3,     3,     4,     3,     4,     5,     4,     5,     0,
       1,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       3,     3,     3,     2,     4,     3,     3,     2,     4,     5,
       4,     5,     4,     3,     3,     4,     3,     3,     3,     2,
       3,     4,     3,     4,     5,     3,     5,     3,     3,     3,
       3,     4,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     3,     3,     2,
       2,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       2,     2,     3
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
  case 59: /* command: expression  */
#line 216 "parser.y"
                 { eval_value = (yyvsp[0].uval); }
#line 2434 "y.tab.c"
    break;

  case 61: /* command: '\n'  */
#line 219 "parser.y"
      {
      }
#line 2441 "y.tab.c"
    break;

  case 63: /* BX_TOKEN_TOGGLE_ON_OFF: BX_TOKEN_OFF  */
#line 226 "parser.y"
    { (yyval.bval)=(yyvsp[0].bval); }
#line 2447 "y.tab.c"
    break;

  case 65: /* BX_TOKEN_REGISTERS: BX_TOKEN_REGS  */
#line 232 "parser.y"
    { (yyval.sval)=(yyvsp[0].sval); }
#line 2453 "y.tab.c"
    break;

  case 71: /* BX_TOKEN_SEGREG: BX_TOKEN_GS  */
#line 242 "parser.y"
    { (yyval.uval)=(yyvsp[0].uval); }
#line 2459 "y.tab.c"
    break;

  case 72: /* timebp_command: BX_TOKEN_TIMEBP expression '\n'  */
#line 247 "parser.y"
      {
          bx_dbg_timebp_command(0, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2468 "y.tab.c"
    break;

  case 73: /* timebp_command: BX_TOKEN_TIMEBP_ABSOLUTE expression '\n'  */
#line 252 "parser.y"
      {
          bx_dbg_timebp_command(1, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2477 "y.tab.c"
    break;

  case 74: /* modebp_command: BX_TOKEN_MODEBP '\n'  */
#line 260 "parser.y"
      {
          bx_dbg_modebp_command();
          free((yyvsp[-1].sval));
      }
#line 2486 "y.tab.c"
    break;

  case 75: /* vmexitbp_command: BX_TOKEN_VMEXITBP '\n'  */
#line 268 "parser.y"
      {
          bx_dbg_vmexitbp_command();
          free((yyvsp[-1].sval));
      }
#line 2495 "y.tab.c"
    break;

  case 76: /* show_command: BX_TOKEN_SHOW BX_TOKEN_GENERIC '\n'  */
#line 276 "parser.y"
      {
          bx_dbg_show_command((yyvsp[-1].sval));
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2504 "y.tab.c"
    break;

  case 77: /* show_command: BX_TOKEN_SHOW BX_TOKEN_ALL '\n'  */
#line 281 "parser.y"
      {
          bx_dbg_show_command("all");
          free((yyvsp[-2].sval));
      }
#line 2513 "y.tab.c"
    break;

  case 78: /* show_command: BX_TOKEN_SHOW BX_TOKEN_OFF '\n'  */
#line 286 "parser.y"
      {
          bx_dbg_show_command("off");
          free((yyvsp[-2].sval));
      }
#line 2522 "y.tab.c"
    break;

  case 79: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING '\n'  */
#line 291 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-1].sval), 0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2531 "y.tab.c"
    break;

  case 80: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING BX_TOKEN_XML '\n'  */
#line 296 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-2].sval), 1);
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2540 "y.tab.c"
    break;

  case 81: /* show_command: BX_TOKEN_SHOW '\n'  */
#line 301 "parser.y"
      {
          bx_dbg_show_command(0);
          free((yyvsp[-1].sval));
      }
#line 2549 "y.tab.c"
    break;

  case 82: /* page_command: BX_TOKEN_PAGE expression '\n'  */
#line 309 "parser.y"
      {
          bx_dbg_xlate_address((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2558 "y.tab.c"
    break;

  case 83: /* tlb_command: BX_TOKEN_TLB expression '\n'  */
#line 317 "parser.y"
      {
          bx_dbg_tlb_lookup((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2567 "y.tab.c"
    break;

  case 84: /* ptime_command: BX_TOKEN_PTIME '\n'  */
#line 325 "parser.y"
      {
          bx_dbg_ptime_command();
          free((yyvsp[-1].sval));
      }
#line 2576 "y.tab.c"
    break;

  case 85: /* trace_command: BX_TOKEN_TRACE BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 333 "parser.y"
      {
          bx_dbg_trace_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2585 "y.tab.c"
    break;

  case 86: /* trace_reg_command: BX_TOKEN_TRACEREG BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 341 "parser.y"
      {
          bx_dbg_trace_reg_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2594 "y.tab.c"
    break;

  case 87: /* trace_mem_command: BX_TOKEN_TRACEMEM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 349 "parser.y"
      {
          bx_dbg_trace_mem_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2603 "y.tab.c"
    break;

  case 88: /* print_stack_command: BX_TOKEN_PRINT_STACK '\n'  */
#line 357 "parser.y"
      {
          bx_dbg_print_stack_command(16);
          free((yyvsp[-1].sval));
      }
#line 2612 "y.tab.c"
    break;

  case 89: /* print_stack_command: BX_TOKEN_PRINT_STACK BX_TOKEN_NUMERIC '\n'  */
#line 362 "parser.y"
      {
          bx_dbg_print_stack_command((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2621 "y.tab.c"
    break;

  case 90: /* backtrace_command: BX_TOKEN_BT '\n'  */
#line 370 "parser.y"
      {
        bx_dbg_bt_command(16);
        free((yyvsp[-1].sval));
      }
#line 2630 "y.tab.c"
    break;

  case 91: /* backtrace_command: BX_TOKEN_BT BX_TOKEN_NUMERIC '\n'  */
#line 375 "parser.y"
      {
        bx_dbg_bt_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2639 "y.tab.c"
    break;

  case 92: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_STOP '\n'  */
#line 383 "parser.y"
      {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2648 "y.tab.c"
    break;

  case 93: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_CONTINUE '\n'  */
#line 388 "parser.y"
      {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2657 "y.tab.c"
    break;

  case 94: /* watch_point_command: BX_TOKEN_WATCH '\n'  */
#line 393 "parser.y"
      {
          bx_dbg_print_watchpoints();
          free((yyvsp[-1].sval));
      }
#line 2666 "y.tab.c"
    break;

  case 95: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression '\n'  */
#line 398 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2675 "y.tab.c"
    break;

  case 96: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression '\n'  */
#line 403 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2684 "y.tab.c"
    break;

  case 97: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression '\n'  */
#line 408 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-1].uval), 1); /* BX_WRITE */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2693 "y.tab.c"
    break;

  case 98: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression expression '\n'  */
#line 413 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2702 "y.tab.c"
    break;

  case 99: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression expression '\n'  */
#line 418 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2711 "y.tab.c"
    break;

  case 100: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression expression '\n'  */
#line 423 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_WRITE */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2720 "y.tab.c"
    break;

  case 101: /* watch_point_command: BX_TOKEN_UNWATCH '\n'  */
#line 428 "parser.y"
      {
          bx_dbg_unwatch_all();
          free((yyvsp[-1].sval));
      }
#line 2729 "y.tab.c"
    break;

  case 102: /* watch_point_command: BX_TOKEN_UNWATCH expression '\n'  */
#line 433 "parser.y"
      {
          bx_dbg_unwatch((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2738 "y.tab.c"
    break;

  case 103: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 441 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2747 "y.tab.c"
    break;

  case 104: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING expression '\n'  */
#line 446 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 0, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2756 "y.tab.c"
    break;

  case 105: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING '\n'  */
#line 451 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 1, 0);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2765 "y.tab.c"
    break;

  case 106: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING expression '\n'  */
#line 456 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 1, (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2774 "y.tab.c"
    break;

  case 107: /* where_command: BX_TOKEN_WHERE '\n'  */
#line 464 "parser.y"
      {
        bx_dbg_where_command();
        free((yyvsp[-1].sval));
      }
#line 2783 "y.tab.c"
    break;

  case 108: /* print_string_command: BX_TOKEN_PRINT_STRING expression '\n'  */
#line 472 "parser.y"
      {
        bx_dbg_print_string_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2792 "y.tab.c"
    break;

  case 109: /* continue_command: BX_TOKEN_CONTINUE '\n'  */
#line 480 "parser.y"
      {
        bx_dbg_continue_command(1);
        free((yyvsp[-1].sval));
      }
#line 2801 "y.tab.c"
    break;

  case 110: /* continue_command: BX_TOKEN_CONTINUE BX_TOKEN_IF expression '\n'  */
#line 485 "parser.y"
      {
        bx_dbg_continue_command((yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2810 "y.tab.c"
    break;

  case 111: /* stepN_command: BX_TOKEN_STEPN '\n'  */
#line 493 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[-1].sval));
      }
#line 2819 "y.tab.c"
    break;

  case 112: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC '\n'  */
#line 498 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2828 "y.tab.c"
    break;

  case 113: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_ALL BX_TOKEN_NUMERIC '\n'  */
#line 503 "parser.y"
      {
        bx_dbg_stepN_command(-1, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2837 "y.tab.c"
    break;

  case 114: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'  */
#line 508 "parser.y"
      {
        bx_dbg_stepN_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2846 "y.tab.c"
    break;

  case 115: /* step_over_command: BX_TOKEN_STEP_OVER '\n'  */
#line 516 "parser.y"
      {
        bx_dbg_step_over_command();
        free((yyvsp[-1].sval));
      }
#line 2855 "y.tab.c"
    break;

  case 116: /* set_command: BX_TOKEN_SET BX_TOKEN_DISASM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 524 "parser.y"
      {
        bx_dbg_set_auto_disassemble((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2864 "y.tab.c"
    break;

  case 117: /* set_command: BX_TOKEN_SET BX_TOKEN_SYMBOLNAME '=' expression '\n'  */
#line 529 "parser.y"
      {
        bx_dbg_set_symbol_command((yyvsp[-3].sval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2873 "y.tab.c"
    break;

  case 118: /* set_command: BX_TOKEN_SET BX_TOKEN_8BL_REG '=' expression '\n'  */
#line 534 "parser.y"
      {
        bx_dbg_set_reg8l_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2881 "y.tab.c"
    break;

  case 119: /* set_command: BX_TOKEN_SET BX_TOKEN_8BH_REG '=' expression '\n'  */
#line 538 "parser.y"
      {
        bx_dbg_set_reg8h_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2889 "y.tab.c"
    break;

  case 120: /* set_command: BX_TOKEN_SET BX_TOKEN_16B_REG '=' expression '\n'  */
#line 542 "parser.y"
      {
        bx_dbg_set_reg16_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2897 "y.tab.c"
    break;

  case 121: /* set_command: BX_TOKEN_SET BX_TOKEN_32B_REG '=' expression '\n'  */
#line 546 "parser.y"
      {
        bx_dbg_set_reg32_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2905 "y.tab.c"
    break;

  case 122: /* set_command: BX_TOKEN_SET BX_TOKEN_64B_REG '=' expression '\n'  */
#line 550 "parser.y"
      {
        bx_dbg_set_reg64_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2913 "y.tab.c"
    break;

  case 123: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_EIP '=' expression '\n'  */
#line 554 "parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2921 "y.tab.c"
    break;

  case 124: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_RIP '=' expression '\n'  */
#line 558 "parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2929 "y.tab.c"
    break;

  case 125: /* set_command: BX_TOKEN_SET BX_TOKEN_SEGREG '=' expression '\n'  */
#line 562 "parser.y"
      {
        bx_dbg_load_segreg((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2937 "y.tab.c"
    break;

  case 126: /* breakpoint_command: BX_TOKEN_VBREAKPOINT '\n'  */
#line 569 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2946 "y.tab.c"
    break;

  case 127: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression '\n'  */
#line 574 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].uval), NULL);
        free((yyvsp[-4].sval));
      }
#line 2955 "y.tab.c"
    break;

  case 128: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 579 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-5].uval), (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-6].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2964 "y.tab.c"
    break;

  case 129: /* breakpoint_command: BX_TOKEN_LBREAKPOINT '\n'  */
#line 584 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2973 "y.tab.c"
    break;

  case 130: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression '\n'  */
#line 589 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 2982 "y.tab.c"
    break;

  case 131: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 594 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2991 "y.tab.c"
    break;

  case 132: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING '\n'  */
#line 599 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-1].sval), NULL);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3000 "y.tab.c"
    break;

  case 133: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 604 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-3].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3009 "y.tab.c"
    break;

  case 134: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '\n'  */
#line 609 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3018 "y.tab.c"
    break;

  case 135: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression '\n'  */
#line 614 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 3027 "y.tab.c"
    break;

  case 136: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 619 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3036 "y.tab.c"
    break;

  case 137: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression '\n'  */
#line 624 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-3].sval));
      }
#line 3045 "y.tab.c"
    break;

  case 138: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 629 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-5].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3054 "y.tab.c"
    break;

  case 139: /* blist_command: BX_TOKEN_LIST_BREAK '\n'  */
#line 637 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-1].sval));
      }
#line 3063 "y.tab.c"
    break;

  case 140: /* slist_command: BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 645 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-1].sval));
      }
#line 3072 "y.tab.c"
    break;

  case 141: /* slist_command: BX_TOKEN_LIST_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 650 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 3081 "y.tab.c"
    break;

  case 142: /* info_command: BX_TOKEN_INFO BX_TOKEN_PBREAKPOINT '\n'  */
#line 658 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3090 "y.tab.c"
    break;

  case 143: /* info_command: BX_TOKEN_INFO BX_TOKEN_CPU '\n'  */
#line 663 "parser.y"
      {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3099 "y.tab.c"
    break;

  case 144: /* info_command: BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric optional_numeric '\n'  */
#line 668 "parser.y"
      {
        bx_dbg_info_idt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3108 "y.tab.c"
    break;

  case 145: /* info_command: BX_TOKEN_INFO BX_TOKEN_IVT optional_numeric optional_numeric '\n'  */
#line 673 "parser.y"
      {
        bx_dbg_info_ivt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3117 "y.tab.c"
    break;

  case 146: /* info_command: BX_TOKEN_INFO BX_TOKEN_GDT optional_numeric optional_numeric '\n'  */
#line 678 "parser.y"
      {
        bx_dbg_info_gdt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3126 "y.tab.c"
    break;

  case 147: /* info_command: BX_TOKEN_INFO BX_TOKEN_LDT optional_numeric optional_numeric '\n'  */
#line 683 "parser.y"
      {
        bx_dbg_info_ldt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3135 "y.tab.c"
    break;

  case 148: /* info_command: BX_TOKEN_INFO BX_TOKEN_TAB '\n'  */
#line 688 "parser.y"
      {
        bx_dbg_dump_table();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3144 "y.tab.c"
    break;

  case 149: /* info_command: BX_TOKEN_INFO BX_TOKEN_TSS '\n'  */
#line 693 "parser.y"
      {
        bx_dbg_info_tss_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3153 "y.tab.c"
    break;

  case 150: /* info_command: BX_TOKEN_INFO BX_TOKEN_FLAGS '\n'  */
#line 698 "parser.y"
      {
        bx_dbg_info_flags();
        free((yyvsp[-2].sval));
      }
#line 3162 "y.tab.c"
    break;

  case 151: /* info_command: BX_TOKEN_INFO BX_TOKEN_LINUX '\n'  */
#line 703 "parser.y"
      {
        bx_dbg_info_linux_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3171 "y.tab.c"
    break;

  case 152: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS '\n'  */
#line 708 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3180 "y.tab.c"
    break;

  case 153: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 713 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3189 "y.tab.c"
    break;

  case 154: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE '\n'  */
#line 718 "parser.y"
      {
        bx_dbg_info_device("", "");
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3198 "y.tab.c"
    break;

  case 155: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC '\n'  */
#line 723 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3207 "y.tab.c"
    break;

  case 156: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC BX_TOKEN_STRING '\n'  */
#line 728 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3216 "y.tab.c"
    break;

  case 157: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING '\n'  */
#line 733 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3225 "y.tab.c"
    break;

  case 158: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 738 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3234 "y.tab.c"
    break;

  case 159: /* optional_numeric: %empty  */
#line 745 "parser.y"
               { (yyval.uval) = EMPTY_ARG; }
#line 3240 "y.tab.c"
    break;

  case 161: /* regs_command: BX_TOKEN_REGISTERS '\n'  */
#line 750 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3249 "y.tab.c"
    break;

  case 162: /* fpu_regs_command: BX_TOKEN_FPU '\n'  */
#line 758 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[-1].sval));
      }
#line 3258 "y.tab.c"
    break;

  case 163: /* mmx_regs_command: BX_TOKEN_MMX '\n'  */
#line 766 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3267 "y.tab.c"
    break;

  case 164: /* xmm_regs_command: BX_TOKEN_XMM '\n'  */
#line 774 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3276 "y.tab.c"
    break;

  case 165: /* ymm_regs_command: BX_TOKEN_YMM '\n'  */
#line 782 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3285 "y.tab.c"
    break;

  case 166: /* zmm_regs_command: BX_TOKEN_ZMM '\n'  */
#line 790 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3294 "y.tab.c"
    break;

  case 167: /* segment_regs_command: BX_TOKEN_SEGMENT_REGS '\n'  */
#line 798 "parser.y"
      {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3303 "y.tab.c"
    break;

  case 168: /* control_regs_command: BX_TOKEN_CONTROL_REGS '\n'  */
#line 806 "parser.y"
      {
        bx_dbg_info_control_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3312 "y.tab.c"
    break;

  case 169: /* debug_regs_command: BX_TOKEN_DEBUG_REGS '\n'  */
#line 814 "parser.y"
      {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3321 "y.tab.c"
    break;

  case 170: /* delete_command: BX_TOKEN_DEL_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 822 "parser.y"
      {
        bx_dbg_del_breakpoint_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3330 "y.tab.c"
    break;

  case 171: /* bpe_command: BX_TOKEN_ENABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 830 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3339 "y.tab.c"
    break;

  case 172: /* bpd_command: BX_TOKEN_DISABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 837 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 0);
        free((yyvsp[-2].sval));
      }
#line 3348 "y.tab.c"
    break;

  case 173: /* quit_command: BX_TOKEN_QUIT '\n'  */
#line 845 "parser.y"
      {
        bx_dbg_quit_command();
        free((yyvsp[-1].sval));
      }
#line 3357 "y.tab.c"
    break;

  case 174: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT expression '\n'  */
#line 853 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-3].sval), (yyvsp[-2].sval),1, (yyvsp[-1].uval), 1);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3366 "y.tab.c"
    break;

  case 175: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT '\n'  */
#line 858 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), (yyvsp[-1].sval),1, 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3375 "y.tab.c"
    break;

  case 176: /* examine_command: BX_TOKEN_EXAMINE expression '\n'  */
#line 863 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), NULL,0, (yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3384 "y.tab.c"
    break;

  case 177: /* examine_command: BX_TOKEN_EXAMINE '\n'  */
#line 868 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-1].sval), NULL,0, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 3393 "y.tab.c"
    break;

  case 178: /* restore_command: BX_TOKEN_RESTORE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 876 "parser.y"
      {
        bx_dbg_restore_command((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3402 "y.tab.c"
    break;

  case 179: /* writemem_command: BX_TOKEN_WRITEMEM BX_TOKEN_STRING expression expression '\n'  */
#line 884 "parser.y"
      {
        bx_dbg_writemem_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3411 "y.tab.c"
    break;

  case 180: /* loadmem_command: BX_TOKEN_LOADMEM BX_TOKEN_STRING expression '\n'  */
#line 892 "parser.y"
      {
        bx_dbg_loadmem_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3420 "y.tab.c"
    break;

  case 181: /* setpmem_command: BX_TOKEN_SETPMEM expression expression expression '\n'  */
#line 900 "parser.y"
      {
        bx_dbg_setpmem_command((yyvsp[-3].uval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3429 "y.tab.c"
    break;

  case 182: /* deref_command: BX_TOKEN_DEREF expression expression '\n'  */
#line 908 "parser.y"
      {
        bx_dbg_deref_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3438 "y.tab.c"
    break;

  case 183: /* query_command: BX_TOKEN_QUERY BX_TOKEN_PENDING '\n'  */
#line 916 "parser.y"
      {
        bx_dbg_query_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3447 "y.tab.c"
    break;

  case 184: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA '\n'  */
#line 924 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3456 "y.tab.c"
    break;

  case 185: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA BX_TOKEN_NUMERIC '\n'  */
#line 929 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3465 "y.tab.c"
    break;

  case 186: /* take_command: BX_TOKEN_TAKE BX_TOKEN_IRQ '\n'  */
#line 934 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3474 "y.tab.c"
    break;

  case 187: /* take_command: BX_TOKEN_TAKE BX_TOKEN_SMI '\n'  */
#line 939 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3483 "y.tab.c"
    break;

  case 188: /* take_command: BX_TOKEN_TAKE BX_TOKEN_NMI '\n'  */
#line 944 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3492 "y.tab.c"
    break;

  case 189: /* disassemble_command: BX_TOKEN_DISASM '\n'  */
#line 952 "parser.y"
      {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[-1].sval));
      }
#line 3501 "y.tab.c"
    break;

  case 190: /* disassemble_command: BX_TOKEN_DISASM expression '\n'  */
#line 957 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3510 "y.tab.c"
    break;

  case 191: /* disassemble_command: BX_TOKEN_DISASM expression expression '\n'  */
#line 962 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3519 "y.tab.c"
    break;

  case 192: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT '\n'  */
#line 967 "parser.y"
      {
        bx_dbg_disassemble_current((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3528 "y.tab.c"
    break;

  case 193: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression '\n'  */
#line 972 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-2].sval), (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3537 "y.tab.c"
    break;

  case 194: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression expression '\n'  */
#line 977 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3546 "y.tab.c"
    break;

  case 195: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SWITCH_MODE '\n'  */
#line 982 "parser.y"
      {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3555 "y.tab.c"
    break;

  case 196: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SIZE '=' BX_TOKEN_NUMERIC '\n'  */
#line 987 "parser.y"
      {
        bx_dbg_set_disassemble_size((yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3564 "y.tab.c"
    break;

  case 197: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STOP '\n'  */
#line 995 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3573 "y.tab.c"
    break;

  case 198: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STRING '\n'  */
#line 1001 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3582 "y.tab.c"
    break;

  case 199: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_GENERIC '\n'  */
#line 1006 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3591 "y.tab.c"
    break;

  case 200: /* doit_command: BX_TOKEN_DOIT expression '\n'  */
#line 1014 "parser.y"
      {
        bx_dbg_doit_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3600 "y.tab.c"
    break;

  case 201: /* crc_command: BX_TOKEN_CRC expression expression '\n'  */
#line 1022 "parser.y"
      {
        bx_dbg_crc_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3609 "y.tab.c"
    break;

  case 202: /* help_command: BX_TOKEN_HELP BX_TOKEN_QUIT '\n'  */
#line 1030 "parser.y"
       {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3618 "y.tab.c"
    break;

  case 203: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTINUE '\n'  */
#line 1035 "parser.y"
       {
         dbg_printf("c|cont|continue - continue executing\n");
         dbg_printf("c|cont|continue if \"expression\" - continue executing only if expression is true\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3628 "y.tab.c"
    break;

  case 204: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEPN '\n'  */
#line 1041 "parser.y"
       {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3639 "y.tab.c"
    break;

  case 205: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEP_OVER '\n'  */
#line 1048 "parser.y"
       {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3648 "y.tab.c"
    break;

  case 206: /* help_command: BX_TOKEN_HELP BX_TOKEN_VBREAKPOINT '\n'  */
#line 1053 "parser.y"
       {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         dbg_printf("vb|vbreak <seg:offset> if \"expression\" - set a conditional virtual address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3658 "y.tab.c"
    break;

  case 207: /* help_command: BX_TOKEN_HELP BX_TOKEN_LBREAKPOINT '\n'  */
#line 1059 "parser.y"
       {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         dbg_printf("lb|lbreak <addr> if \"expression\" - set a conditional linear address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3668 "y.tab.c"
    break;

  case 208: /* help_command: BX_TOKEN_HELP BX_TOKEN_PBREAKPOINT '\n'  */
#line 1065 "parser.y"
       {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         dbg_printf("p|pb|break|pbreak <addr> if \"expression\" - set a conditional physical address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3678 "y.tab.c"
    break;

  case 209: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEL_BREAKPOINT '\n'  */
#line 1071 "parser.y"
       {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3687 "y.tab.c"
    break;

  case 210: /* help_command: BX_TOKEN_HELP BX_TOKEN_ENABLE_BREAKPOINT '\n'  */
#line 1076 "parser.y"
       {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3696 "y.tab.c"
    break;

  case 211: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISABLE_BREAKPOINT '\n'  */
#line 1081 "parser.y"
       {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3705 "y.tab.c"
    break;

  case 212: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_BREAK '\n'  */
#line 1086 "parser.y"
       {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3714 "y.tab.c"
    break;

  case 213: /* help_command: BX_TOKEN_HELP BX_TOKEN_MODEBP '\n'  */
#line 1091 "parser.y"
       {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3723 "y.tab.c"
    break;

  case 214: /* help_command: BX_TOKEN_HELP BX_TOKEN_VMEXITBP '\n'  */
#line 1096 "parser.y"
       {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3732 "y.tab.c"
    break;

  case 215: /* help_command: BX_TOKEN_HELP BX_TOKEN_CRC '\n'  */
#line 1101 "parser.y"
       {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3741 "y.tab.c"
    break;

  case 216: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACE '\n'  */
#line 1106 "parser.y"
       {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3751 "y.tab.c"
    break;

  case 217: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEREG '\n'  */
#line 1112 "parser.y"
       {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3761 "y.tab.c"
    break;

  case 218: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEMEM '\n'  */
#line 1118 "parser.y"
       {
         dbg_printf("trace-mem on  - print all memory accesses occurred during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3771 "y.tab.c"
    break;

  case 219: /* help_command: BX_TOKEN_HELP BX_TOKEN_RESTORE '\n'  */
#line 1124 "parser.y"
       {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3783 "y.tab.c"
    break;

  case 220: /* help_command: BX_TOKEN_HELP BX_TOKEN_PTIME '\n'  */
#line 1132 "parser.y"
       {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3792 "y.tab.c"
    break;

  case 221: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP '\n'  */
#line 1137 "parser.y"
       {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3801 "y.tab.c"
    break;

  case 222: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP_ABSOLUTE '\n'  */
#line 1142 "parser.y"
       {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3810 "y.tab.c"
    break;

  case 223: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STACK '\n'  */
#line 1147 "parser.y"
       {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3819 "y.tab.c"
    break;

  case 224: /* help_command: BX_TOKEN_HELP BX_TOKEN_BT '\n'  */
#line 1152 "parser.y"
       {
         dbg_printf("bt [num_entries] - prints backtrace\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3828 "y.tab.c"
    break;

  case 225: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOAD_SYMBOLS '\n'  */
#line 1157 "parser.y"
       {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3837 "y.tab.c"
    break;

  case 226: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 1162 "parser.y"
       {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3846 "y.tab.c"
    break;

  case 227: /* help_command: BX_TOKEN_HELP BX_TOKEN_REGISTERS '\n'  */
#line 1167 "parser.y"
       {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3855 "y.tab.c"
    break;

  case 228: /* help_command: BX_TOKEN_HELP BX_TOKEN_FPU '\n'  */
#line 1172 "parser.y"
       {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3864 "y.tab.c"
    break;

  case 229: /* help_command: BX_TOKEN_HELP BX_TOKEN_MMX '\n'  */
#line 1177 "parser.y"
       {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3873 "y.tab.c"
    break;

  case 230: /* help_command: BX_TOKEN_HELP BX_TOKEN_XMM '\n'  */
#line 1182 "parser.y"
       {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3882 "y.tab.c"
    break;

  case 231: /* help_command: BX_TOKEN_HELP BX_TOKEN_YMM '\n'  */
#line 1187 "parser.y"
       {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3891 "y.tab.c"
    break;

  case 232: /* help_command: BX_TOKEN_HELP BX_TOKEN_ZMM '\n'  */
#line 1192 "parser.y"
       {
         dbg_printf("zmm - print AVX-512 state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3900 "y.tab.c"
    break;

  case 233: /* help_command: BX_TOKEN_HELP BX_TOKEN_SEGMENT_REGS '\n'  */
#line 1197 "parser.y"
       {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3909 "y.tab.c"
    break;

  case 234: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTROL_REGS '\n'  */
#line 1202 "parser.y"
       {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3918 "y.tab.c"
    break;

  case 235: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEBUG_REGS '\n'  */
#line 1207 "parser.y"
       {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3927 "y.tab.c"
    break;

  case 236: /* help_command: BX_TOKEN_HELP BX_TOKEN_WRITEMEM '\n'  */
#line 1212 "parser.y"
       {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3936 "y.tab.c"
    break;

  case 237: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOADMEM '\n'  */
#line 1217 "parser.y"
       {
         dbg_printf("loadmem <filename> <laddr> - load file bytes to virtual memory starting from the linear address 'laddr'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3945 "y.tab.c"
    break;

  case 238: /* help_command: BX_TOKEN_HELP BX_TOKEN_SETPMEM '\n'  */
#line 1222 "parser.y"
       {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3954 "y.tab.c"
    break;

  case 239: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEREF '\n'  */
#line 1227 "parser.y"
       {
         dbg_printf("deref <addr> <deep> - pointer dereference. For example: get value of [[[rax]]] or ***rax: deref rax 3\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3963 "y.tab.c"
    break;

  case 240: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISASM '\n'  */
#line 1232 "parser.y"
       {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm hex on/off - control disasm offsets and displacements format\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3977 "y.tab.c"
    break;

  case 241: /* help_command: BX_TOKEN_HELP BX_TOKEN_WATCH '\n'  */
#line 1242 "parser.y"
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
#line 3992 "y.tab.c"
    break;

  case 242: /* help_command: BX_TOKEN_HELP BX_TOKEN_UNWATCH '\n'  */
#line 1253 "parser.y"
       {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4002 "y.tab.c"
    break;

  case 243: /* help_command: BX_TOKEN_HELP BX_TOKEN_EXAMINE '\n'  */
#line 1259 "parser.y"
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
#line 4019 "y.tab.c"
    break;

  case 244: /* help_command: BX_TOKEN_HELP BX_TOKEN_INSTRUMENT '\n'  */
#line 1272 "parser.y"
       {
         dbg_printf("instrument <command|\"string command\"> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command|\"string command\">\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4028 "y.tab.c"
    break;

  case 245: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET '\n'  */
#line 1277 "parser.y"
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
#line 4043 "y.tab.c"
    break;

  case 246: /* help_command: BX_TOKEN_HELP BX_TOKEN_PAGE '\n'  */
#line 1288 "parser.y"
       {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4052 "y.tab.c"
    break;

  case 247: /* help_command: BX_TOKEN_HELP BX_TOKEN_INFO '\n'  */
#line 1293 "parser.y"
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
#line 4072 "y.tab.c"
    break;

  case 248: /* help_command: BX_TOKEN_HELP BX_TOKEN_SHOW '\n'  */
#line 1309 "parser.y"
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
#line 4092 "y.tab.c"
    break;

  case 249: /* help_command: BX_TOKEN_HELP BX_TOKEN_CALC '\n'  */
#line 1325 "parser.y"
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
#line 4108 "y.tab.c"
    break;

  case 250: /* help_command: BX_TOKEN_HELP BX_TOKEN_ADDLYT '\n'  */
#line 1337 "parser.y"
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
#line 4124 "y.tab.c"
    break;

  case 251: /* help_command: BX_TOKEN_HELP BX_TOKEN_REMLYT '\n'  */
#line 1349 "parser.y"
       {
         dbg_printf("remlyt - stops debugger to execute the script file added previously with addlyt command.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4133 "y.tab.c"
    break;

  case 252: /* help_command: BX_TOKEN_HELP BX_TOKEN_LYT '\n'  */
#line 1354 "parser.y"
       {
         dbg_printf("lyt - cause debugger to execute script file added previously with addlyt command.\n");
         dbg_printf("    Use it as a refresh/context.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4143 "y.tab.c"
    break;

  case 253: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STRING '\n'  */
#line 1360 "parser.y"
       {
         dbg_printf("print-string <addr> - prints a null-ended string from a linear address.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4152 "y.tab.c"
    break;

  case 254: /* help_command: BX_TOKEN_HELP BX_TOKEN_SOURCE '\n'  */
#line 1365 "parser.y"
       {
         dbg_printf("source <file> - cause debugger to execute a script file.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4161 "y.tab.c"
    break;

  case 255: /* help_command: BX_TOKEN_HELP BX_TOKEN_HELP '\n'  */
#line 1370 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4170 "y.tab.c"
    break;

  case 256: /* help_command: BX_TOKEN_HELP '\n'  */
#line 1375 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-1].sval));
       }
#line 4179 "y.tab.c"
    break;

  case 257: /* calc_command: BX_TOKEN_CALC expression '\n'  */
#line 1383 "parser.y"
   {
     eval_value = (yyvsp[-1].uval);
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4189 "y.tab.c"
    break;

  case 258: /* addlyt_command: BX_TOKEN_ADDLYT BX_TOKEN_STRING '\n'  */
#line 1392 "parser.y"
   {
     bx_dbg_addlyt((yyvsp[-1].sval));
     free((yyvsp[-2].sval));
     free((yyvsp[-1].sval));
   }
#line 4199 "y.tab.c"
    break;

  case 259: /* remlyt_command: BX_TOKEN_REMLYT '\n'  */
#line 1401 "parser.y"
   {
     bx_dbg_remlyt();
     free((yyvsp[-1].sval));
   }
#line 4208 "y.tab.c"
    break;

  case 260: /* lyt_command: BX_TOKEN_LYT '\n'  */
#line 1409 "parser.y"
   {
     bx_dbg_lyt();
     free((yyvsp[-1].sval));
   }
#line 4217 "y.tab.c"
    break;

  case 261: /* if_command: BX_TOKEN_IF expression '\n'  */
#line 1417 "parser.y"
   {
     eval_value = (yyvsp[-1].uval) != 0;
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4227 "y.tab.c"
    break;

  case 262: /* vexpression: BX_TOKEN_NUMERIC  */
#line 1426 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4233 "y.tab.c"
    break;

  case 263: /* vexpression: BX_TOKEN_STRING  */
#line 1427 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4239 "y.tab.c"
    break;

  case 264: /* vexpression: BX_TOKEN_8BL_REG  */
#line 1428 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4245 "y.tab.c"
    break;

  case 265: /* vexpression: BX_TOKEN_8BH_REG  */
#line 1429 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4251 "y.tab.c"
    break;

  case 266: /* vexpression: BX_TOKEN_16B_REG  */
#line 1430 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4257 "y.tab.c"
    break;

  case 267: /* vexpression: BX_TOKEN_32B_REG  */
#line 1431 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4263 "y.tab.c"
    break;

  case 268: /* vexpression: BX_TOKEN_64B_REG  */
#line 1432 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4269 "y.tab.c"
    break;

  case 269: /* vexpression: BX_TOKEN_OPMASK_REG  */
#line 1433 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4275 "y.tab.c"
    break;

  case 270: /* vexpression: BX_TOKEN_SEGREG  */
#line 1434 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4281 "y.tab.c"
    break;

  case 271: /* vexpression: BX_TOKEN_REG_IP  */
#line 1435 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4287 "y.tab.c"
    break;

  case 272: /* vexpression: BX_TOKEN_REG_EIP  */
#line 1436 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4293 "y.tab.c"
    break;

  case 273: /* vexpression: BX_TOKEN_REG_RIP  */
#line 1437 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4299 "y.tab.c"
    break;

  case 274: /* vexpression: BX_TOKEN_REG_SSP  */
#line 1438 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4305 "y.tab.c"
    break;

  case 275: /* vexpression: vexpression '+' vexpression  */
#line 1439 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4311 "y.tab.c"
    break;

  case 276: /* vexpression: vexpression '-' vexpression  */
#line 1440 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4317 "y.tab.c"
    break;

  case 277: /* vexpression: vexpression '*' vexpression  */
#line 1441 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4323 "y.tab.c"
    break;

  case 278: /* vexpression: vexpression '/' vexpression  */
#line 1442 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) / (yyvsp[0].uval); }
#line 4329 "y.tab.c"
    break;

  case 279: /* vexpression: vexpression BX_TOKEN_DEREF_CHR vexpression  */
#line 1443 "parser.y"
                                                { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 4335 "y.tab.c"
    break;

  case 280: /* vexpression: vexpression BX_TOKEN_RSHIFT vexpression  */
#line 1444 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4341 "y.tab.c"
    break;

  case 281: /* vexpression: vexpression BX_TOKEN_LSHIFT vexpression  */
#line 1445 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4347 "y.tab.c"
    break;

  case 282: /* vexpression: vexpression '|' vexpression  */
#line 1446 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4353 "y.tab.c"
    break;

  case 283: /* vexpression: vexpression '^' vexpression  */
#line 1447 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4359 "y.tab.c"
    break;

  case 284: /* vexpression: vexpression '&' vexpression  */
#line 1448 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4365 "y.tab.c"
    break;

  case 285: /* vexpression: '!' vexpression  */
#line 1449 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4371 "y.tab.c"
    break;

  case 286: /* vexpression: '-' vexpression  */
#line 1450 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4377 "y.tab.c"
    break;

  case 287: /* vexpression: '(' vexpression ')'  */
#line 1451 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4383 "y.tab.c"
    break;

  case 288: /* expression: BX_TOKEN_NUMERIC  */
#line 1457 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4389 "y.tab.c"
    break;

  case 289: /* expression: BX_TOKEN_STRING  */
#line 1458 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4395 "y.tab.c"
    break;

  case 290: /* expression: BX_TOKEN_8BL_REG  */
#line 1459 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4401 "y.tab.c"
    break;

  case 291: /* expression: BX_TOKEN_8BH_REG  */
#line 1460 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4407 "y.tab.c"
    break;

  case 292: /* expression: BX_TOKEN_16B_REG  */
#line 1461 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4413 "y.tab.c"
    break;

  case 293: /* expression: BX_TOKEN_32B_REG  */
#line 1462 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4419 "y.tab.c"
    break;

  case 294: /* expression: BX_TOKEN_64B_REG  */
#line 1463 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4425 "y.tab.c"
    break;

  case 295: /* expression: BX_TOKEN_OPMASK_REG  */
#line 1464 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4431 "y.tab.c"
    break;

  case 296: /* expression: BX_TOKEN_SEGREG  */
#line 1465 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4437 "y.tab.c"
    break;

  case 297: /* expression: BX_TOKEN_REG_IP  */
#line 1466 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4443 "y.tab.c"
    break;

  case 298: /* expression: BX_TOKEN_REG_EIP  */
#line 1467 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4449 "y.tab.c"
    break;

  case 299: /* expression: BX_TOKEN_REG_RIP  */
#line 1468 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4455 "y.tab.c"
    break;

  case 300: /* expression: BX_TOKEN_REG_SSP  */
#line 1469 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4461 "y.tab.c"
    break;

  case 301: /* expression: expression ':' expression  */
#line 1470 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[-2].uval), (yyvsp[0].uval)); }
#line 4467 "y.tab.c"
    break;

  case 302: /* expression: expression '+' expression  */
#line 1471 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4473 "y.tab.c"
    break;

  case 303: /* expression: expression '-' expression  */
#line 1472 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4479 "y.tab.c"
    break;

  case 304: /* expression: expression '*' expression  */
#line 1473 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4485 "y.tab.c"
    break;

  case 305: /* expression: expression '/' expression  */
#line 1474 "parser.y"
                                     { (yyval.uval) = ((yyvsp[0].uval) != 0) ? (yyvsp[-2].uval) / (yyvsp[0].uval) : 0; }
#line 4491 "y.tab.c"
    break;

  case 306: /* expression: expression BX_TOKEN_DEREF_CHR expression  */
#line 1475 "parser.y"
                                              { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 4497 "y.tab.c"
    break;

  case 307: /* expression: expression BX_TOKEN_RSHIFT expression  */
#line 1476 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4503 "y.tab.c"
    break;

  case 308: /* expression: expression BX_TOKEN_LSHIFT expression  */
#line 1477 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4509 "y.tab.c"
    break;

  case 309: /* expression: expression '|' expression  */
#line 1478 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4515 "y.tab.c"
    break;

  case 310: /* expression: expression '^' expression  */
#line 1479 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4521 "y.tab.c"
    break;

  case 311: /* expression: expression '&' expression  */
#line 1480 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4527 "y.tab.c"
    break;

  case 312: /* expression: expression '>' expression  */
#line 1481 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) > (yyvsp[0].uval); }
#line 4533 "y.tab.c"
    break;

  case 313: /* expression: expression '<' expression  */
#line 1482 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) < (yyvsp[0].uval); }
#line 4539 "y.tab.c"
    break;

  case 314: /* expression: expression BX_TOKEN_EQ expression  */
#line 1483 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) == (yyvsp[0].uval); }
#line 4545 "y.tab.c"
    break;

  case 315: /* expression: expression BX_TOKEN_NE expression  */
#line 1484 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) != (yyvsp[0].uval); }
#line 4551 "y.tab.c"
    break;

  case 316: /* expression: expression BX_TOKEN_LE expression  */
#line 1485 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) <= (yyvsp[0].uval); }
#line 4557 "y.tab.c"
    break;

  case 317: /* expression: expression BX_TOKEN_GE expression  */
#line 1486 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) >= (yyvsp[0].uval); }
#line 4563 "y.tab.c"
    break;

  case 318: /* expression: '!' expression  */
#line 1487 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4569 "y.tab.c"
    break;

  case 319: /* expression: '-' expression  */
#line 1488 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4575 "y.tab.c"
    break;

  case 320: /* expression: '*' expression  */
#line 1489 "parser.y"
                                     { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[0].uval)); }
#line 4581 "y.tab.c"
    break;

  case 321: /* expression: '@' expression  */
#line 1490 "parser.y"
                                     { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[0].uval)); }
#line 4587 "y.tab.c"
    break;

  case 322: /* expression: '(' expression ')'  */
#line 1491 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4593 "y.tab.c"
    break;


#line 4597 "y.tab.c"

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

#line 1494 "parser.y"

#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
