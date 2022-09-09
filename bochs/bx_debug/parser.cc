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
    BX_TOKEN_DEVICE = 362,         /* BX_TOKEN_DEVICE  */
    BX_TOKEN_GENERIC = 363,        /* BX_TOKEN_GENERIC  */
    BX_TOKEN_DEREF_CHR = 364,      /* BX_TOKEN_DEREF_CHR  */
    BX_TOKEN_RSHIFT = 365,         /* BX_TOKEN_RSHIFT  */
    BX_TOKEN_LSHIFT = 366,         /* BX_TOKEN_LSHIFT  */
    BX_TOKEN_EQ = 367,             /* BX_TOKEN_EQ  */
    BX_TOKEN_NE = 368,             /* BX_TOKEN_NE  */
    BX_TOKEN_LE = 369,             /* BX_TOKEN_LE  */
    BX_TOKEN_GE = 370,             /* BX_TOKEN_GE  */
    BX_TOKEN_REG_IP = 371,         /* BX_TOKEN_REG_IP  */
    BX_TOKEN_REG_EIP = 372,        /* BX_TOKEN_REG_EIP  */
    BX_TOKEN_REG_RIP = 373,        /* BX_TOKEN_REG_RIP  */
    BX_TOKEN_REG_SSP = 374,        /* BX_TOKEN_REG_SSP  */
    NOT = 375,                     /* NOT  */
    NEG = 376,                     /* NEG  */
    INDIRECT = 377                 /* INDIRECT  */
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
#define BX_TOKEN_DEVICE 362
#define BX_TOKEN_GENERIC 363
#define BX_TOKEN_DEREF_CHR 364
#define BX_TOKEN_RSHIFT 365
#define BX_TOKEN_LSHIFT 366
#define BX_TOKEN_EQ 367
#define BX_TOKEN_NE 368
#define BX_TOKEN_LE 369
#define BX_TOKEN_GE 370
#define BX_TOKEN_REG_IP 371
#define BX_TOKEN_REG_EIP 372
#define BX_TOKEN_REG_RIP 373
#define BX_TOKEN_REG_SSP 374
#define NOT 375
#define NEG 376
#define INDIRECT 377

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 14 "parser.y"

  char    *sval;
  Bit64u   uval;
  unsigned bval;

#line 390 "y.tab.c"

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
  YYSYMBOL_BX_TOKEN_DEVICE = 107,          /* BX_TOKEN_DEVICE  */
  YYSYMBOL_BX_TOKEN_GENERIC = 108,         /* BX_TOKEN_GENERIC  */
  YYSYMBOL_BX_TOKEN_DEREF_CHR = 109,       /* BX_TOKEN_DEREF_CHR  */
  YYSYMBOL_BX_TOKEN_RSHIFT = 110,          /* BX_TOKEN_RSHIFT  */
  YYSYMBOL_BX_TOKEN_LSHIFT = 111,          /* BX_TOKEN_LSHIFT  */
  YYSYMBOL_BX_TOKEN_EQ = 112,              /* BX_TOKEN_EQ  */
  YYSYMBOL_BX_TOKEN_NE = 113,              /* BX_TOKEN_NE  */
  YYSYMBOL_BX_TOKEN_LE = 114,              /* BX_TOKEN_LE  */
  YYSYMBOL_BX_TOKEN_GE = 115,              /* BX_TOKEN_GE  */
  YYSYMBOL_BX_TOKEN_REG_IP = 116,          /* BX_TOKEN_REG_IP  */
  YYSYMBOL_BX_TOKEN_REG_EIP = 117,         /* BX_TOKEN_REG_EIP  */
  YYSYMBOL_BX_TOKEN_REG_RIP = 118,         /* BX_TOKEN_REG_RIP  */
  YYSYMBOL_BX_TOKEN_REG_SSP = 119,         /* BX_TOKEN_REG_SSP  */
  YYSYMBOL_120_ = 120,                     /* '+'  */
  YYSYMBOL_121_ = 121,                     /* '-'  */
  YYSYMBOL_122_ = 122,                     /* '|'  */
  YYSYMBOL_123_ = 123,                     /* '^'  */
  YYSYMBOL_124_ = 124,                     /* '<'  */
  YYSYMBOL_125_ = 125,                     /* '>'  */
  YYSYMBOL_126_ = 126,                     /* '*'  */
  YYSYMBOL_127_ = 127,                     /* '/'  */
  YYSYMBOL_128_ = 128,                     /* '&'  */
  YYSYMBOL_NOT = 129,                      /* NOT  */
  YYSYMBOL_NEG = 130,                      /* NEG  */
  YYSYMBOL_INDIRECT = 131,                 /* INDIRECT  */
  YYSYMBOL_132_n_ = 132,                   /* '\n'  */
  YYSYMBOL_133_ = 133,                     /* '='  */
  YYSYMBOL_134_ = 134,                     /* ':'  */
  YYSYMBOL_135_ = 135,                     /* '!'  */
  YYSYMBOL_136_ = 136,                     /* '('  */
  YYSYMBOL_137_ = 137,                     /* ')'  */
  YYSYMBOL_138_ = 138,                     /* '@'  */
  YYSYMBOL_YYACCEPT = 139,                 /* $accept  */
  YYSYMBOL_commands = 140,                 /* commands  */
  YYSYMBOL_command = 141,                  /* command  */
  YYSYMBOL_BX_TOKEN_TOGGLE_ON_OFF = 142,   /* BX_TOKEN_TOGGLE_ON_OFF  */
  YYSYMBOL_BX_TOKEN_REGISTERS = 143,       /* BX_TOKEN_REGISTERS  */
  YYSYMBOL_BX_TOKEN_SEGREG = 144,          /* BX_TOKEN_SEGREG  */
  YYSYMBOL_timebp_command = 145,           /* timebp_command  */
  YYSYMBOL_modebp_command = 146,           /* modebp_command  */
  YYSYMBOL_vmexitbp_command = 147,         /* vmexitbp_command  */
  YYSYMBOL_show_command = 148,             /* show_command  */
  YYSYMBOL_page_command = 149,             /* page_command  */
  YYSYMBOL_tlb_command = 150,              /* tlb_command  */
  YYSYMBOL_ptime_command = 151,            /* ptime_command  */
  YYSYMBOL_trace_command = 152,            /* trace_command  */
  YYSYMBOL_trace_reg_command = 153,        /* trace_reg_command  */
  YYSYMBOL_trace_mem_command = 154,        /* trace_mem_command  */
  YYSYMBOL_print_stack_command = 155,      /* print_stack_command  */
  YYSYMBOL_backtrace_command = 156,        /* backtrace_command  */
  YYSYMBOL_watch_point_command = 157,      /* watch_point_command  */
  YYSYMBOL_symbol_command = 158,           /* symbol_command  */
  YYSYMBOL_where_command = 159,            /* where_command  */
  YYSYMBOL_print_string_command = 160,     /* print_string_command  */
  YYSYMBOL_continue_command = 161,         /* continue_command  */
  YYSYMBOL_stepN_command = 162,            /* stepN_command  */
  YYSYMBOL_step_over_command = 163,        /* step_over_command  */
  YYSYMBOL_set_command = 164,              /* set_command  */
  YYSYMBOL_breakpoint_command = 165,       /* breakpoint_command  */
  YYSYMBOL_blist_command = 166,            /* blist_command  */
  YYSYMBOL_slist_command = 167,            /* slist_command  */
  YYSYMBOL_info_command = 168,             /* info_command  */
  YYSYMBOL_optional_numeric = 169,         /* optional_numeric  */
  YYSYMBOL_regs_command = 170,             /* regs_command  */
  YYSYMBOL_fpu_regs_command = 171,         /* fpu_regs_command  */
  YYSYMBOL_mmx_regs_command = 172,         /* mmx_regs_command  */
  YYSYMBOL_xmm_regs_command = 173,         /* xmm_regs_command  */
  YYSYMBOL_ymm_regs_command = 174,         /* ymm_regs_command  */
  YYSYMBOL_zmm_regs_command = 175,         /* zmm_regs_command  */
  YYSYMBOL_segment_regs_command = 176,     /* segment_regs_command  */
  YYSYMBOL_control_regs_command = 177,     /* control_regs_command  */
  YYSYMBOL_debug_regs_command = 178,       /* debug_regs_command  */
  YYSYMBOL_delete_command = 179,           /* delete_command  */
  YYSYMBOL_bpe_command = 180,              /* bpe_command  */
  YYSYMBOL_bpd_command = 181,              /* bpd_command  */
  YYSYMBOL_quit_command = 182,             /* quit_command  */
  YYSYMBOL_examine_command = 183,          /* examine_command  */
  YYSYMBOL_restore_command = 184,          /* restore_command  */
  YYSYMBOL_writemem_command = 185,         /* writemem_command  */
  YYSYMBOL_loadmem_command = 186,          /* loadmem_command  */
  YYSYMBOL_setpmem_command = 187,          /* setpmem_command  */
  YYSYMBOL_deref_command = 188,            /* deref_command  */
  YYSYMBOL_query_command = 189,            /* query_command  */
  YYSYMBOL_take_command = 190,             /* take_command  */
  YYSYMBOL_disassemble_command = 191,      /* disassemble_command  */
  YYSYMBOL_instrument_command = 192,       /* instrument_command  */
  YYSYMBOL_doit_command = 193,             /* doit_command  */
  YYSYMBOL_crc_command = 194,              /* crc_command  */
  YYSYMBOL_help_command = 195,             /* help_command  */
  YYSYMBOL_calc_command = 196,             /* calc_command  */
  YYSYMBOL_addlyt_command = 197,           /* addlyt_command  */
  YYSYMBOL_remlyt_command = 198,           /* remlyt_command  */
  YYSYMBOL_lyt_command = 199,              /* lyt_command  */
  YYSYMBOL_if_command = 200,               /* if_command  */
  YYSYMBOL_vexpression = 201,              /* vexpression  */
  YYSYMBOL_expression = 202                /* expression  */
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
#define YYFINAL  330
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2566

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  139
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  64
/* YYNRULES -- Number of rules.  */
#define YYNRULES  319
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  623

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   377


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
     132,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   135,     2,     2,     2,     2,   128,     2,
     136,   137,   126,   120,     2,   121,     2,   127,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   134,     2,
     124,   133,   125,     2,   138,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   123,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   122,     2,     2,     2,     2,     2,
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
     115,   116,   117,   118,   119,   129,   130,   131
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   155,   155,   156,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   223,   224,   229,   230,   235,   236,   237,   238,
     239,   240,   245,   250,   258,   266,   274,   279,   284,   289,
     294,   299,   307,   315,   323,   331,   339,   347,   355,   360,
     368,   373,   381,   386,   391,   396,   401,   406,   411,   416,
     421,   426,   431,   439,   444,   449,   454,   462,   470,   478,
     483,   491,   496,   501,   506,   514,   522,   527,   532,   536,
     540,   544,   548,   552,   556,   560,   567,   572,   577,   582,
     587,   592,   597,   602,   607,   612,   617,   622,   627,   635,
     643,   648,   656,   661,   666,   671,   676,   681,   686,   691,
     696,   701,   706,   711,   716,   721,   726,   731,   736,   744,
     745,   748,   756,   764,   772,   780,   788,   796,   804,   812,
     820,   828,   835,   843,   851,   856,   861,   866,   874,   882,
     890,   898,   906,   914,   922,   927,   932,   937,   942,   950,
     955,   960,   965,   970,   975,   980,   985,   993,   999,  1004,
    1012,  1020,  1028,  1033,  1039,  1046,  1051,  1057,  1063,  1069,
    1074,  1079,  1084,  1089,  1094,  1099,  1104,  1110,  1116,  1122,
    1130,  1135,  1140,  1145,  1150,  1155,  1160,  1165,  1170,  1175,
    1180,  1185,  1190,  1195,  1200,  1205,  1210,  1215,  1220,  1225,
    1235,  1246,  1252,  1265,  1270,  1281,  1286,  1302,  1318,  1330,
    1342,  1347,  1353,  1358,  1366,  1375,  1384,  1392,  1400,  1410,
    1411,  1412,  1413,  1414,  1415,  1416,  1417,  1418,  1419,  1420,
    1421,  1422,  1423,  1424,  1425,  1426,  1427,  1428,  1429,  1430,
    1431,  1432,  1433,  1434,  1435,  1441,  1442,  1443,  1444,  1445,
    1446,  1447,  1448,  1449,  1450,  1451,  1452,  1453,  1454,  1455,
    1456,  1457,  1458,  1459,  1460,  1461,  1462,  1463,  1464,  1465,
    1466,  1467,  1468,  1469,  1470,  1471,  1472,  1473,  1474,  1475
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
  "BX_TOKEN_REMLYT", "BX_TOKEN_LYT", "BX_TOKEN_DEVICE", "BX_TOKEN_GENERIC",
  "BX_TOKEN_DEREF_CHR", "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT",
  "BX_TOKEN_EQ", "BX_TOKEN_NE", "BX_TOKEN_LE", "BX_TOKEN_GE",
  "BX_TOKEN_REG_IP", "BX_TOKEN_REG_EIP", "BX_TOKEN_REG_RIP",
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

#define YYPACT_NINF (-188)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-318)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     587,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,    15,   120,   -10,   -89,   736,   -85,    19,
    1265,  1288,   -45,   -35,   -29,  1056,   -66,  -188,  -188,   -58,
     -31,   -30,   -19,   -18,   -12,     7,     9,  1208,    18,    70,
      71,   120,   120,    81,    -7,   120,  1129,   -33,  -188,   120,
     120,    83,    83,    83,    14,   120,   120,    20,    21,   -80,
     -64,   153,  1344,    40,    -9,   -67,    24,   120,  -188,   120,
    1679,   120,    78,    25,    38,  -188,  -188,  -188,  -188,   120,
     120,  -188,   120,   120,   120,   451,  -188,    42,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  2432,   120,  -188,   825,    79,   -55,
    -188,  -188,    44,    46,    47,    48,    49,    50,    83,    52,
      54,    60,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,   311,  -188,   311,   311,  -188,
    1076,    17,  -188,   -17,   120,  -188,   659,    74,    80,    82,
      92,    96,   101,   120,   120,   120,   120,   103,   113,   119,
     -59,   -60,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  1424,  -188,   853,   141,   120,   120,  1055,  1055,   125,
     -46,   127,   128,   129,   997,  1480,   131,   107,  -188,   155,
     152,   154,   160,  1540,  1055,  -188,  -188,   162,   163,   164,
    -188,  1678,  1704,  -188,  -188,   165,  -188,   166,  -188,   167,
     120,   168,   120,   120,  -188,  -188,  1730,   169,   170,   -51,
     171,  -188,  1503,   232,   173,  -188,  -188,  1756,  1782,   174,
     175,   176,   177,   178,   179,   180,   181,   194,   195,   196,
     197,   198,   199,   200,   218,   219,   220,   221,   222,   229,
     233,   245,   249,   250,   252,   253,   254,   255,   256,   261,
     262,   266,   267,   268,   270,   274,   275,   276,   286,   287,
     292,   293,   301,   302,   303,   304,   305,   306,   307,  -188,
     308,  1808,   310,  -188,  -188,   314,   314,   314,   689,   314,
    -188,  -188,  -188,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
    1834,  -188,   313,   317,  -188,   120,   120,   120,   120,   120,
     120,   318,   120,   120,   120,  -188,  -188,   294,   311,   311,
     311,   311,   311,   311,   311,   311,   311,   311,   311,   354,
    -188,   380,  -188,   -13,   394,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,   120,  2432,   120,   120,   120,  -188,  -188,  -188,
     335,  -188,   -56,   -54,  -188,  -188,  1860,  -188,   336,  1055,
    1886,   120,   120,  1055,  1912,  -188,   342,  -188,  -188,  -188,
    -188,  -188,  -188,   723,  -188,   387,  -188,  1938,  -188,  -188,
    -188,  -188,  1964,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,   753,  -188,   889,   919,  -188,  -188,  -188,   360,  -188,
    -188,  -188,  1990,  1560,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,    76,    76,    76,   314,   314,   314,   314,  1125,  1125,
    1125,  1125,  1125,  1125,    76,    76,    76,  2432,  -188,  -188,
    -188,  2016,  2042,  2068,  2094,  2120,  2146,  -188,  2172,  2198,
    2224,  -188,  -188,  -188,  -188,   121,   121,   121,   121,  -188,
    -188,  -188,   269,   361,   362,   423,  -188,   364,   365,   366,
     367,   368,  -188,   373,  -188,   374,  -188,  -188,  -188,  2250,
    -188,  1204,   108,  2276,  -188,  -188,  -188,  2302,   382,  -188,
    -188,  -188,  2328,  -188,  2354,  -188,  2380,  -188,  -188,  -188,
    2406,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
     440,  -188,  -188,  -188,   384,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
     385,  -188,  -188
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
      60,   288,   287,   289,   290,   291,    66,    67,    68,    69,
      70,    71,   292,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    64,    65,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   286,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   285,     0,
       0,     0,     0,     0,     0,   294,   295,   296,   297,     0,
       0,    61,     0,     0,     0,     0,     3,     0,   293,    41,
      42,    43,    49,    47,    48,    40,    37,    38,    39,    44,
      45,    46,    50,    51,    52,     4,     5,     6,     7,     8,
      19,    20,     9,    10,    11,    12,    13,    14,    15,    16,
      18,    17,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    53,    54,
      55,    56,    57,    58,    59,     0,   109,     0,     0,     0,
     111,   115,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   139,   262,   261,   263,   264,   265,   266,   260,
     259,   268,   269,   270,   271,     0,   126,     0,     0,   267,
       0,   286,   129,     0,     0,   134,     0,     0,     0,     0,
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,   253,
       0,     0,     0,   256,   257,   316,   317,   315,     0,   318,
       1,     2,   161,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   258,     0,     0,   112,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   283,   282,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     132,     0,   130,   317,     0,   135,   170,   171,   172,   150,
     142,   143,   159,   160,   159,   159,   159,   149,   148,   151,
       0,   152,     0,     0,   154,   175,     0,   176,     0,     0,
       0,     0,     0,     0,     0,   183,     0,   184,   186,   187,
     188,    83,   192,     0,   195,     0,   190,     0,   198,   197,
     199,   200,     0,    85,    86,    87,    73,    72,    89,    91,
      93,     0,    92,     0,     0,   102,    78,    77,     0,    79,
      76,   103,     0,     0,   141,   108,    82,   203,   204,   205,
     244,   212,   206,   207,   208,   209,   210,   211,   246,   202,
     227,   228,   229,   230,   231,   234,   233,   232,   242,   219,
     235,   236,   237,   238,   239,   243,   215,   216,   217,   218,
     220,   222,   221,   213,   214,   223,   240,   241,   247,   224,
     225,   245,   252,   248,   249,   250,   251,   226,   254,   255,
     319,   303,   304,   305,   311,   312,   313,   314,   299,   300,
     306,   307,   310,   309,   301,   302,   308,   298,   110,   113,
     114,     0,     0,     0,     0,     0,     0,   116,     0,     0,
       0,   284,   276,   277,   278,   272,   273,   279,   280,   274,
     275,   281,     0,     0,     0,     0,   137,     0,     0,     0,
       0,     0,   153,     0,   157,     0,   155,   174,   178,     0,
     180,   300,   301,     0,   182,   185,   193,     0,     0,   191,
     201,    95,     0,    96,     0,    97,     0,    80,   104,   105,
       0,   119,   118,   120,   121,   122,   117,   123,   124,   125,
       0,   127,   133,   131,     0,   136,   144,   145,   146,   147,
     158,   156,   179,   181,   194,   196,    98,    99,   100,   106,
       0,   138,   128
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -188,  -188,   433,   -42,   449,    -2,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -187,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -174,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    85,    86,   237,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     392,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   180,   393
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     144,   365,   381,   366,   367,   264,   555,   394,   395,   396,
     238,   239,   402,   400,   147,   161,   563,   179,   565,   245,
     183,   186,   163,   164,   165,   166,   167,     6,     7,     8,
       9,    10,    11,   168,   145,   247,   379,   213,   148,   230,
     231,   217,   218,   151,   353,   224,   229,   162,   403,   233,
     234,   448,   246,   416,   187,   241,   242,   257,   220,   221,
     222,   223,   256,   262,   188,   265,   202,   267,   248,   268,
     189,   321,   404,   401,   203,   232,   564,   354,   566,   325,
     326,   449,   327,   328,   329,   144,   417,   263,   258,   149,
     214,   169,   333,   334,   335,   336,   337,   338,   339,   235,
     236,   204,   205,   340,   341,   342,   343,   344,   345,   346,
     347,   348,   259,   206,   207,   382,   361,   349,   170,   556,
     208,   349,   150,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,   171,   172,   173,   174,   209,
     175,   210,   215,   216,   219,   350,   240,   146,   260,   380,
     322,   176,   243,   244,   177,   178,   266,   323,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
     324,   249,   261,   179,   332,   179,   179,   355,   352,   356,
     357,   358,   359,   360,   383,   362,   250,   363,   336,   337,
     338,   339,    48,   364,   542,   543,   544,   545,   546,   547,
     548,   549,   550,   551,   552,   558,   386,   559,   560,   561,
     349,   406,   387,   408,   388,   409,   410,   413,   414,    68,
    -317,  -317,  -317,  -317,   389,   423,   251,    48,   390,   427,
     368,   369,   370,   391,   432,   397,    75,    76,    77,    78,
     425,    79,   349,   252,   253,   398,    80,   375,   376,   377,
     441,   399,   443,   444,    68,    82,    83,   415,    84,   418,
     419,   420,   452,   424,   333,   334,   335,   336,   337,   338,
     339,    75,    76,    77,    78,   340,   411,   342,   343,   344,
     345,   412,   347,   348,   428,   254,   429,   426,   600,   349,
      82,    83,   430,    84,   433,   434,   435,   438,   439,   440,
     442,   446,   447,   450,   453,   454,   457,   458,   459,   460,
     461,   462,   463,   464,   163,   164,   165,   166,   167,     6,
       7,     8,     9,    10,    11,   168,   465,   466,   467,   468,
     469,   470,   471,   511,   512,   513,   514,   515,   516,   517,
     518,   519,   520,   521,   522,   523,   524,   525,   526,   527,
     472,   473,   474,   475,   476,   531,   532,   533,   534,   535,
     536,   477,   538,   539,   540,   478,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   479,   368,   369,
     370,   480,   481,   169,   482,   483,   484,   485,   486,   371,
     372,   373,   374,   487,   488,   375,   376,   377,   489,   490,
     491,   601,   492,   368,   369,   370,   493,   494,   495,   569,
     170,   571,   572,   573,   371,   372,   373,   374,   496,   497,
     375,   376,   377,   577,   498,   499,   553,   171,   172,   173,
     174,   541,   175,   500,   501,   502,   503,   504,   505,   506,
     507,   582,   509,   584,   586,   529,   177,   178,   349,   530,
     537,   330,   554,   590,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,   557,   562,   568,    13,
      14,    15,    16,    17,   575,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,   578,    29,    30,    31,
      32,    33,   587,   602,   603,   604,   605,   606,   607,   608,
     609,    34,    35,    36,    37,   610,   611,    38,    39,    40,
      41,    42,   620,    43,   615,    44,   621,   622,   331,   320,
      45,    46,    47,    48,     0,    49,    50,    51,    52,    53,
       0,     0,    54,    55,    56,    57,    58,    59,    60,    61,
      62,     0,     0,    63,    64,     0,    65,     0,    66,    67,
      68,    69,    70,     0,    71,    72,    73,    74,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    75,    76,    77,
      78,     0,    79,     0,     0,     0,     0,    80,     0,     0,
       0,     0,     0,    81,     0,     0,    82,    83,     0,    84,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,     0,     0,     0,    13,    14,    15,    16,    17,
       0,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,     0,    29,    30,    31,    32,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    34,    35,    36,
      37,     0,     0,    38,    39,    40,    41,    42,     0,    43,
       0,    44,     0,     0,     0,     0,    45,    46,    47,    48,
       0,    49,    50,    51,    52,    53,     0,     0,    54,    55,
      56,    57,    58,    59,    60,    61,    62,     0,   384,    63,
      64,     0,    65,     0,    66,    67,    68,    69,    70,     0,
      71,    72,    73,    74,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    75,    76,    77,    78,     0,    79,     0,
       0,     0,     0,    80,     0,     0,     0,     0,     0,    81,
       0,     0,    82,    83,     0,    84,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     0,   152,
     153,   154,   155,   156,     6,     7,     8,     9,    10,    11,
       0,     0,     0,     0,     0,     0,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,   333,   334,
     335,   336,   337,   338,   339,     0,     0,     0,     0,   340,
     341,   342,   343,   344,   345,   346,   347,   348,     0,     0,
       0,   385,     0,   349,     0,    48,     0,   157,   333,   334,
     335,   336,   337,   338,   339,     0,   158,     0,     0,   340,
     341,   342,   343,   344,   345,   346,   347,   348,     0,     0,
       0,     0,    68,   349,     0,    48,   510,     0,     0,     0,
       0,     0,   333,   334,   335,   336,   337,   338,   339,    75,
      76,    77,    78,   340,   411,   342,   343,   344,   345,   412,
     347,   348,    68,   159,   160,   576,     0,   349,    82,    83,
       0,    84,   333,   334,   335,   336,   337,   338,   339,    75,
      76,    77,    78,   340,   411,   342,   343,   344,   345,   412,
     347,   348,     0,     0,     0,   581,     0,   349,    82,    83,
       0,    84,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,   333,   334,   335,   336,   337,   338,
     339,     0,     0,     0,     0,   340,   341,   342,   343,   344,
     345,   346,   347,   348,     0,     0,     0,   351,     0,   349,
       0,    48,   333,   334,   335,   336,   337,   338,   339,     0,
       0,     0,     0,   340,   341,   342,   343,   344,   345,   346,
     347,   348,     0,     0,     0,   407,     0,   349,    68,     0,
       0,    48,     0,     0,     0,     0,     0,     0,   333,   334,
     335,   336,   337,   338,   339,    75,    76,    77,    78,   340,
     411,   342,   343,   344,   345,   412,   347,   348,    68,     0,
       0,   583,     0,   349,    82,    83,     0,    84,   333,   334,
     335,   336,   337,   338,   339,    75,    76,    77,    78,   340,
     411,   342,   343,   344,   345,   412,   347,   348,     0,     0,
       0,   585,     0,   349,    82,    83,     0,    84,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
       0,   190,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   191,     0,     0,     0,     0,     0,     0,
       0,   192,     0,     0,     0,     0,     0,     0,   193,   194,
     195,   196,   197,   198,     0,   199,   333,   334,   335,   336,
     337,   338,   339,     0,     0,     0,     0,   340,   341,   342,
     343,   344,   345,   346,   347,   348,     0,    48,     0,   421,
       0,   349,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,     0,     0,     0,     0,     0,
     200,     0,     0,     0,    68,     0,     0,     0,     0,     0,
       0,     0,     0,   201,   333,   334,   335,   336,   337,   338,
     339,    75,    76,    77,    78,   340,   411,   342,   343,   344,
     345,   412,   347,   348,   225,   368,   369,   370,     0,   349,
      82,    83,     0,    84,     0,     0,   371,   372,   373,   374,
       0,    48,   375,   376,   377,     0,     0,     0,   226,   227,
     378,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,     0,     0,    68,     0,
       0,     0,     0,     0,   333,   334,   335,   336,   337,   338,
     339,     0,     0,     0,     0,    75,    76,    77,    78,     0,
      79,   346,   347,   348,     0,    80,     0,     0,     0,   349,
       0,   228,   211,     0,    82,    83,     0,    84,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      48,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,     0,    68,     0,     0,
       0,     0,     0,  -316,  -316,  -316,  -316,  -316,  -316,  -316,
       0,     0,     0,     0,    75,    76,    77,    78,     0,    79,
    -316,  -316,  -316,     0,    80,     0,     0,   181,   349,     0,
     212,     0,     0,    82,    83,     0,    84,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
      48,     0,     0,     0,    68,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    75,    76,    77,    78,     0,    79,    68,     0,     0,
       0,    80,     0,     0,     0,     0,     0,   182,     0,     0,
      82,    83,     0,    84,    75,    76,    77,    78,     0,    79,
       0,     0,     0,     0,   184,     0,    48,     0,     0,     0,
     185,     0,     0,    82,    83,     0,    84,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,     0,     0,    68,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      75,    76,    77,    78,     0,    79,     0,     0,     0,     0,
      80,     0,     0,     0,     0,     0,   255,     0,     0,    82,
      83,     0,    84,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,     0,    48,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     0,     0,
       0,     0,     0,    68,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      75,    76,    77,    78,     0,    79,     0,     0,     0,     0,
      80,     0,    48,     0,     0,     0,   405,     0,     0,    82,
      83,     0,    84,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    48,     0,     0,     0,    68,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    75,    76,    77,    78,
       0,    79,    68,     0,     0,     0,    80,     0,     0,     0,
       0,     0,   422,     0,     0,    82,    83,     0,    84,    75,
      76,    77,    78,     0,    79,     0,     0,     0,     0,    80,
       0,     0,    48,     0,     0,   451,     0,     0,    82,    83,
       0,    84,     0,     0,     0,     0,     0,     0,     0,   333,
     334,   335,   336,   337,   338,   339,     0,     0,     0,    68,
     340,   341,   342,   343,   344,   345,   346,   347,   348,     0,
       0,     0,   431,     0,   349,     0,    75,    76,    77,    78,
       0,    79,     0,     0,     0,     0,    80,     0,     0,     0,
       0,     0,   589,     0,     0,    82,    83,   269,    84,   270,
     271,   272,     0,   273,   274,   275,   276,   277,   278,   279,
     280,   281,    27,    28,     0,   282,   283,   284,   285,   286,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   287,
     288,   289,   290,     0,     0,   291,   292,   293,   294,   295,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   296,
     297,     0,     0,     0,   298,   299,   300,   301,     0,     0,
     302,   303,   304,   305,   306,   307,     0,   308,   309,     0,
       0,   310,   311,     0,   312,     0,     0,     0,     0,   313,
     314,     0,   315,   316,   317,   318,     0,   333,   334,   335,
     336,   337,   338,   339,     0,     0,     0,     0,   340,   341,
     342,   343,   344,   345,   346,   347,   348,     0,     0,     0,
     436,   319,   349,   333,   334,   335,   336,   337,   338,   339,
       0,     0,     0,     0,   340,   341,   342,   343,   344,   345,
     346,   347,   348,     0,     0,     0,   437,     0,   349,   333,
     334,   335,   336,   337,   338,   339,     0,     0,     0,     0,
     340,   341,   342,   343,   344,   345,   346,   347,   348,     0,
       0,     0,   445,     0,   349,   333,   334,   335,   336,   337,
     338,   339,     0,     0,     0,     0,   340,   341,   342,   343,
     344,   345,   346,   347,   348,     0,     0,     0,   455,     0,
     349,   333,   334,   335,   336,   337,   338,   339,     0,     0,
       0,     0,   340,   341,   342,   343,   344,   345,   346,   347,
     348,     0,     0,     0,   456,     0,   349,   333,   334,   335,
     336,   337,   338,   339,     0,     0,     0,     0,   340,   341,
     342,   343,   344,   345,   346,   347,   348,     0,     0,     0,
     508,     0,   349,   333,   334,   335,   336,   337,   338,   339,
       0,     0,     0,     0,   340,   341,   342,   343,   344,   345,
     346,   347,   348,     0,     0,     0,   528,     0,   349,   333,
     334,   335,   336,   337,   338,   339,     0,     0,     0,     0,
     340,   341,   342,   343,   344,   345,   346,   347,   348,     0,
       0,     0,   567,     0,   349,   333,   334,   335,   336,   337,
     338,   339,     0,     0,     0,     0,   340,   341,   342,   343,
     344,   345,   346,   347,   348,     0,     0,     0,   570,     0,
     349,   333,   334,   335,   336,   337,   338,   339,     0,     0,
       0,     0,   340,   341,   342,   343,   344,   345,   346,   347,
     348,     0,     0,     0,   574,     0,   349,   333,   334,   335,
     336,   337,   338,   339,     0,     0,     0,     0,   340,   341,
     342,   343,   344,   345,   346,   347,   348,     0,     0,     0,
     579,     0,   349,   333,   334,   335,   336,   337,   338,   339,
       0,     0,     0,     0,   340,   341,   342,   343,   344,   345,
     346,   347,   348,     0,     0,     0,   580,     0,   349,   333,
     334,   335,   336,   337,   338,   339,     0,     0,     0,     0,
     340,   341,   342,   343,   344,   345,   346,   347,   348,     0,
       0,     0,   588,     0,   349,   333,   334,   335,   336,   337,
     338,   339,     0,     0,     0,     0,   340,   341,   342,   343,
     344,   345,   346,   347,   348,     0,     0,     0,   591,     0,
     349,   333,   334,   335,   336,   337,   338,   339,     0,     0,
       0,     0,   340,   341,   342,   343,   344,   345,   346,   347,
     348,     0,     0,     0,   592,     0,   349,   333,   334,   335,
     336,   337,   338,   339,     0,     0,     0,     0,   340,   341,
     342,   343,   344,   345,   346,   347,   348,     0,     0,     0,
     593,     0,   349,   333,   334,   335,   336,   337,   338,   339,
       0,     0,     0,     0,   340,   341,   342,   343,   344,   345,
     346,   347,   348,     0,     0,     0,   594,     0,   349,   333,
     334,   335,   336,   337,   338,   339,     0,     0,     0,     0,
     340,   341,   342,   343,   344,   345,   346,   347,   348,     0,
       0,     0,   595,     0,   349,   333,   334,   335,   336,   337,
     338,   339,     0,     0,     0,     0,   340,   341,   342,   343,
     344,   345,   346,   347,   348,     0,     0,     0,   596,     0,
     349,   333,   334,   335,   336,   337,   338,   339,     0,     0,
       0,     0,   340,   341,   342,   343,   344,   345,   346,   347,
     348,     0,     0,     0,   597,     0,   349,   333,   334,   335,
     336,   337,   338,   339,     0,     0,     0,     0,   340,   341,
     342,   343,   344,   345,   346,   347,   348,     0,     0,     0,
     598,     0,   349,   333,   334,   335,   336,   337,   338,   339,
       0,     0,     0,     0,   340,   341,   342,   343,   344,   345,
     346,   347,   348,     0,     0,     0,   599,     0,   349,   333,
     334,   335,   336,   337,   338,   339,     0,     0,     0,     0,
     340,   341,   342,   343,   344,   345,   346,   347,   348,     0,
       0,     0,   612,     0,   349,   333,   334,   335,   336,   337,
     338,   339,     0,     0,     0,     0,   340,   341,   342,   343,
     344,   345,   346,   347,   348,     0,     0,     0,   613,     0,
     349,   333,   334,   335,   336,   337,   338,   339,     0,     0,
       0,     0,   340,   341,   342,   343,   344,   345,   346,   347,
     348,     0,     0,     0,   614,     0,   349,   333,   334,   335,
     336,   337,   338,   339,     0,     0,     0,     0,   340,   341,
     342,   343,   344,   345,   346,   347,   348,     0,     0,     0,
     616,     0,   349,   333,   334,   335,   336,   337,   338,   339,
       0,     0,     0,     0,   340,   341,   342,   343,   344,   345,
     346,   347,   348,     0,     0,     0,   617,     0,   349,   333,
     334,   335,   336,   337,   338,   339,     0,     0,     0,     0,
     340,   341,   342,   343,   344,   345,   346,   347,   348,     0,
       0,     0,   618,     0,   349,   333,   334,   335,   336,   337,
     338,   339,     0,     0,     0,     0,   340,   341,   342,   343,
     344,   345,   346,   347,   348,     0,     0,     0,   619,     0,
     349,   333,   334,   335,   336,   337,   338,   339,     0,     0,
       0,     0,   340,   341,   342,   343,   344,   345,   346,   347,
     348,     0,     0,     0,     0,     0,   349
};

static const yytype_int16 yycheck[] =
{
       0,   175,    19,   177,   178,    72,    19,   194,   195,   196,
      52,    53,    72,    72,    14,    17,    72,    19,    72,    99,
      20,    21,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    19,    99,    19,    37,    48,    72,
      73,    41,    42,   132,    99,    45,    46,   132,   108,    49,
      50,   102,   132,    99,    99,    55,    56,    17,    65,    66,
      67,    68,    62,    72,    99,   132,   132,    67,   132,    69,
      99,    71,   132,   132,   132,   108,   132,   132,   132,    79,
      80,   132,    82,    83,    84,    85,   132,    96,    48,    99,
      72,    72,   109,   110,   111,   112,   113,   114,   115,    16,
      17,   132,   132,   120,   121,   122,   123,   124,   125,   126,
     127,   128,    72,   132,   132,   132,   158,   134,    99,   132,
     132,   134,   132,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,   116,   117,   118,   119,   132,
     121,   132,    72,    72,    63,   145,   132,   132,   108,   132,
      72,   132,   132,   132,   135,   136,   132,   132,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     132,    18,   132,   175,   132,   177,   178,   133,    99,   133,
     133,   133,   133,   133,   184,   133,    33,   133,   112,   113,
     114,   115,    72,   133,   368,   369,   370,   371,   372,   373,
     374,   375,   376,   377,   378,   392,   132,   394,   395,   396,
     134,   211,   132,    72,   132,   215,   216,   217,   218,    99,
     112,   113,   114,   115,   132,   225,    73,    72,   132,   229,
     109,   110,   111,   132,   234,   132,   116,   117,   118,   119,
     133,   121,   134,    90,    91,   132,   126,   126,   127,   128,
     250,   132,   252,   253,    99,   135,   136,   132,   138,   132,
     132,   132,   262,   132,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   132,   132,   132,   132,    19,   134,
     135,   136,   132,   138,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,    72,   132,   132,   132,   132,   132,
     132,   132,   132,   132,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,   132,   132,   132,   132,
     132,   132,   132,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     132,   132,   132,   132,   132,   355,   356,   357,   358,   359,
     360,   132,   362,   363,   364,   132,   368,   369,   370,   371,
     372,   373,   374,   375,   376,   377,   378,   132,   109,   110,
     111,   132,   132,    72,   132,   132,   132,   132,   132,   120,
     121,   122,   123,   132,   132,   126,   127,   128,   132,   132,
     132,   132,   132,   109,   110,   111,   132,   132,   132,   409,
      99,   411,   412,   413,   120,   121,   122,   123,   132,   132,
     126,   127,   128,   423,   132,   132,    72,   116,   117,   118,
     119,   137,   121,   132,   132,   132,   132,   132,   132,   132,
     132,   441,   132,   443,   444,   132,   135,   136,   134,   132,
     132,     0,    72,   453,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    72,   132,   132,    18,
      19,    20,    21,    22,   132,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    99,    36,    37,    38,
      39,    40,   132,   132,   132,    72,   132,   132,   132,   132,
     132,    50,    51,    52,    53,   132,   132,    56,    57,    58,
      59,    60,    72,    62,   132,    64,   132,   132,    85,    70,
      69,    70,    71,    72,    -1,    74,    75,    76,    77,    78,
      -1,    -1,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    -1,    -1,    92,    93,    -1,    95,    -1,    97,    98,
      99,   100,   101,    -1,   103,   104,   105,   106,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,   117,   118,
     119,    -1,   121,    -1,    -1,    -1,    -1,   126,    -1,    -1,
      -1,    -1,    -1,   132,    -1,    -1,   135,   136,    -1,   138,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    18,    19,    20,    21,    22,
      -1,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    -1,    36,    37,    38,    39,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    51,    52,
      53,    -1,    -1,    56,    57,    58,    59,    60,    -1,    62,
      -1,    64,    -1,    -1,    -1,    -1,    69,    70,    71,    72,
      -1,    74,    75,    76,    77,    78,    -1,    -1,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    -1,    19,    92,
      93,    -1,    95,    -1,    97,    98,    99,   100,   101,    -1,
     103,   104,   105,   106,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   116,   117,   118,   119,    -1,   121,    -1,
      -1,    -1,    -1,   126,    -1,    -1,    -1,    -1,    -1,   132,
      -1,    -1,   135,   136,    -1,   138,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,   109,   110,
     111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,   120,
     121,   122,   123,   124,   125,   126,   127,   128,    -1,    -1,
      -1,   132,    -1,   134,    -1,    72,    -1,    61,   109,   110,
     111,   112,   113,   114,   115,    -1,    70,    -1,    -1,   120,
     121,   122,   123,   124,   125,   126,   127,   128,    -1,    -1,
      -1,    -1,    99,   134,    -1,    72,   137,    -1,    -1,    -1,
      -1,    -1,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,    99,   117,   118,   132,    -1,   134,   135,   136,
      -1,   138,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,    -1,    -1,    -1,   132,    -1,   134,   135,   136,
      -1,   138,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,   109,   110,   111,   112,   113,   114,
     115,    -1,    -1,    -1,    -1,   120,   121,   122,   123,   124,
     125,   126,   127,   128,    -1,    -1,    -1,   132,    -1,   134,
      -1,    72,   109,   110,   111,   112,   113,   114,   115,    -1,
      -1,    -1,    -1,   120,   121,   122,   123,   124,   125,   126,
     127,   128,    -1,    -1,    -1,   132,    -1,   134,    99,    -1,
      -1,    72,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,    99,    -1,
      -1,   132,    -1,   134,   135,   136,    -1,   138,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,    -1,    -1,
      -1,   132,    -1,   134,   135,   136,    -1,   138,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    42,    43,
      44,    45,    46,    47,    -1,    49,   109,   110,   111,   112,
     113,   114,   115,    -1,    -1,    -1,    -1,   120,   121,   122,
     123,   124,   125,   126,   127,   128,    -1,    72,    -1,   132,
      -1,   134,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    -1,    -1,    -1,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   107,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,    55,   109,   110,   111,    -1,   134,
     135,   136,    -1,   138,    -1,    -1,   120,   121,   122,   123,
      -1,    72,   126,   127,   128,    -1,    -1,    -1,    79,    80,
     134,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    -1,    99,    -1,
      -1,    -1,    -1,    -1,   109,   110,   111,   112,   113,   114,
     115,    -1,    -1,    -1,    -1,   116,   117,   118,   119,    -1,
     121,   126,   127,   128,    -1,   126,    -1,    -1,    -1,   134,
      -1,   132,    54,    -1,   135,   136,    -1,   138,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    99,    -1,    -1,
      -1,    -1,    -1,   109,   110,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,   116,   117,   118,   119,    -1,   121,
     126,   127,   128,    -1,   126,    -1,    -1,    72,   134,    -1,
     132,    -1,    -1,   135,   136,    -1,   138,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      72,    -1,    -1,    -1,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   116,   117,   118,   119,    -1,   121,    99,    -1,    -1,
      -1,   126,    -1,    -1,    -1,    -1,    -1,   132,    -1,    -1,
     135,   136,    -1,   138,   116,   117,   118,   119,    -1,   121,
      -1,    -1,    -1,    -1,   126,    -1,    72,    -1,    -1,    -1,
     132,    -1,    -1,   135,   136,    -1,   138,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     116,   117,   118,   119,    -1,   121,    -1,    -1,    -1,    -1,
     126,    -1,    -1,    -1,    -1,    -1,   132,    -1,    -1,   135,
     136,    -1,   138,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    72,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    -1,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     116,   117,   118,   119,    -1,   121,    -1,    -1,    -1,    -1,
     126,    -1,    72,    -1,    -1,    -1,   132,    -1,    -1,   135,
     136,    -1,   138,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    72,    -1,    -1,    -1,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   116,   117,   118,   119,
      -1,   121,    99,    -1,    -1,    -1,   126,    -1,    -1,    -1,
      -1,    -1,   132,    -1,    -1,   135,   136,    -1,   138,   116,
     117,   118,   119,    -1,   121,    -1,    -1,    -1,    -1,   126,
      -1,    -1,    72,    -1,    -1,   132,    -1,    -1,   135,   136,
      -1,   138,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    99,
     120,   121,   122,   123,   124,   125,   126,   127,   128,    -1,
      -1,    -1,   132,    -1,   134,    -1,   116,   117,   118,   119,
      -1,   121,    -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,
      -1,    -1,   132,    -1,    -1,   135,   136,    18,   138,    20,
      21,    22,    -1,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    -1,    36,    37,    38,    39,    40,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,
      51,    52,    53,    -1,    -1,    56,    57,    58,    59,    60,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,
      71,    -1,    -1,    -1,    75,    76,    77,    78,    -1,    -1,
      81,    82,    83,    84,    85,    86,    -1,    88,    89,    -1,
      -1,    92,    93,    -1,    95,    -1,    -1,    -1,    -1,   100,
     101,    -1,   103,   104,   105,   106,    -1,   109,   110,   111,
     112,   113,   114,   115,    -1,    -1,    -1,    -1,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    -1,    -1,    -1,
     132,   132,   134,   109,   110,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,   120,   121,   122,   123,   124,   125,
     126,   127,   128,    -1,    -1,    -1,   132,    -1,   134,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
     120,   121,   122,   123,   124,   125,   126,   127,   128,    -1,
      -1,    -1,   132,    -1,   134,   109,   110,   111,   112,   113,
     114,   115,    -1,    -1,    -1,    -1,   120,   121,   122,   123,
     124,   125,   126,   127,   128,    -1,    -1,    -1,   132,    -1,
     134,   109,   110,   111,   112,   113,   114,   115,    -1,    -1,
      -1,    -1,   120,   121,   122,   123,   124,   125,   126,   127,
     128,    -1,    -1,    -1,   132,    -1,   134,   109,   110,   111,
     112,   113,   114,   115,    -1,    -1,    -1,    -1,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    -1,    -1,    -1,
     132,    -1,   134,   109,   110,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,   120,   121,   122,   123,   124,   125,
     126,   127,   128,    -1,    -1,    -1,   132,    -1,   134,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
     120,   121,   122,   123,   124,   125,   126,   127,   128,    -1,
      -1,    -1,   132,    -1,   134,   109,   110,   111,   112,   113,
     114,   115,    -1,    -1,    -1,    -1,   120,   121,   122,   123,
     124,   125,   126,   127,   128,    -1,    -1,    -1,   132,    -1,
     134,   109,   110,   111,   112,   113,   114,   115,    -1,    -1,
      -1,    -1,   120,   121,   122,   123,   124,   125,   126,   127,
     128,    -1,    -1,    -1,   132,    -1,   134,   109,   110,   111,
     112,   113,   114,   115,    -1,    -1,    -1,    -1,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    -1,    -1,    -1,
     132,    -1,   134,   109,   110,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,   120,   121,   122,   123,   124,   125,
     126,   127,   128,    -1,    -1,    -1,   132,    -1,   134,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
     120,   121,   122,   123,   124,   125,   126,   127,   128,    -1,
      -1,    -1,   132,    -1,   134,   109,   110,   111,   112,   113,
     114,   115,    -1,    -1,    -1,    -1,   120,   121,   122,   123,
     124,   125,   126,   127,   128,    -1,    -1,    -1,   132,    -1,
     134,   109,   110,   111,   112,   113,   114,   115,    -1,    -1,
      -1,    -1,   120,   121,   122,   123,   124,   125,   126,   127,
     128,    -1,    -1,    -1,   132,    -1,   134,   109,   110,   111,
     112,   113,   114,   115,    -1,    -1,    -1,    -1,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    -1,    -1,    -1,
     132,    -1,   134,   109,   110,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,   120,   121,   122,   123,   124,   125,
     126,   127,   128,    -1,    -1,    -1,   132,    -1,   134,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
     120,   121,   122,   123,   124,   125,   126,   127,   128,    -1,
      -1,    -1,   132,    -1,   134,   109,   110,   111,   112,   113,
     114,   115,    -1,    -1,    -1,    -1,   120,   121,   122,   123,
     124,   125,   126,   127,   128,    -1,    -1,    -1,   132,    -1,
     134,   109,   110,   111,   112,   113,   114,   115,    -1,    -1,
      -1,    -1,   120,   121,   122,   123,   124,   125,   126,   127,
     128,    -1,    -1,    -1,   132,    -1,   134,   109,   110,   111,
     112,   113,   114,   115,    -1,    -1,    -1,    -1,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    -1,    -1,    -1,
     132,    -1,   134,   109,   110,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,   120,   121,   122,   123,   124,   125,
     126,   127,   128,    -1,    -1,    -1,   132,    -1,   134,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
     120,   121,   122,   123,   124,   125,   126,   127,   128,    -1,
      -1,    -1,   132,    -1,   134,   109,   110,   111,   112,   113,
     114,   115,    -1,    -1,    -1,    -1,   120,   121,   122,   123,
     124,   125,   126,   127,   128,    -1,    -1,    -1,   132,    -1,
     134,   109,   110,   111,   112,   113,   114,   115,    -1,    -1,
      -1,    -1,   120,   121,   122,   123,   124,   125,   126,   127,
     128,    -1,    -1,    -1,   132,    -1,   134,   109,   110,   111,
     112,   113,   114,   115,    -1,    -1,    -1,    -1,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    -1,    -1,    -1,
     132,    -1,   134,   109,   110,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,   120,   121,   122,   123,   124,   125,
     126,   127,   128,    -1,    -1,    -1,   132,    -1,   134,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
     120,   121,   122,   123,   124,   125,   126,   127,   128,    -1,
      -1,    -1,   132,    -1,   134,   109,   110,   111,   112,   113,
     114,   115,    -1,    -1,    -1,    -1,   120,   121,   122,   123,
     124,   125,   126,   127,   128,    -1,    -1,    -1,   132,    -1,
     134,   109,   110,   111,   112,   113,   114,   115,    -1,    -1,
      -1,    -1,   120,   121,   122,   123,   124,   125,   126,   127,
     128,    -1,    -1,    -1,    -1,    -1,   134
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
     101,   103,   104,   105,   106,   116,   117,   118,   119,   121,
     126,   132,   135,   136,   138,   140,   141,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   202,    19,   132,   202,    48,    99,
     132,   132,     3,     4,     5,     6,     7,    61,    70,   117,
     118,   144,   132,     3,     4,     5,     6,     7,    14,    72,
      99,   116,   117,   118,   119,   121,   132,   135,   136,   144,
     201,    72,   132,   202,   126,   132,   202,    99,    99,    99,
      15,    27,    35,    42,    43,    44,    45,    46,    47,    49,
      94,   107,   132,   132,   132,   132,   132,   132,   132,   132,
     132,    54,   132,   202,    72,    72,    72,   202,   202,    63,
      65,    66,    67,    68,   202,    55,    79,    80,   132,   202,
      72,    73,   108,   202,   202,    16,    17,   142,   142,   142,
     132,   202,   202,   132,   132,    99,   132,    99,   132,    18,
      33,    73,    90,    91,   132,   132,   202,    17,    48,    72,
     108,   132,    72,    96,    72,   132,   132,   202,   202,    18,
      20,    21,    22,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    36,    37,    38,    39,    40,    50,    51,    52,
      53,    56,    57,    58,    59,    60,    70,    71,    75,    76,
      77,    78,    81,    82,    83,    84,    85,    86,    88,    89,
      92,    93,    95,   100,   101,   103,   104,   105,   106,   132,
     143,   202,    72,   132,   132,   202,   202,   202,   202,   202,
       0,   141,   132,   109,   110,   111,   112,   113,   114,   115,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   134,
     202,   132,    99,    99,   132,   133,   133,   133,   133,   133,
     133,   142,   133,   133,   133,   201,   201,   201,   109,   110,
     111,   120,   121,   122,   123,   126,   127,   128,   134,    19,
     132,    19,   132,   202,    19,   132,   132,   132,   132,   132,
     132,   132,   169,   202,   169,   169,   169,   132,   132,   132,
      72,   132,    72,   108,   132,   132,   202,   132,    72,   202,
     202,   121,   126,   202,   202,   132,    99,   132,   132,   132,
     132,   132,   132,   202,   132,   133,   132,   202,   132,   132,
     132,   132,   202,   132,   132,   132,   132,   132,   132,   132,
     132,   202,   132,   202,   202,   132,   132,   132,   102,   132,
     132,   132,   202,    72,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     137,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   132,   132,
     132,   202,   202,   202,   202,   202,   202,   132,   202,   202,
     202,   137,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,    72,    72,    19,   132,    72,   169,   169,
     169,   169,   132,    72,   132,    72,   132,   132,   132,   202,
     132,   202,   202,   202,   132,   132,   132,   202,    99,   132,
     132,   132,   202,   132,   202,   132,   202,   132,   132,   132,
     202,   132,   132,   132,   132,   132,   132,   132,   132,   132,
      19,   132,   132,   132,    72,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
      72,   132,   132
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   139,   140,   140,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   142,   142,   143,   143,   144,   144,   144,   144,
     144,   144,   145,   145,   146,   147,   148,   148,   148,   148,
     148,   148,   149,   150,   151,   152,   153,   154,   155,   155,
     156,   156,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   158,   158,   158,   158,   159,   160,   161,
     161,   162,   162,   162,   162,   163,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   166,
     167,   167,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   169,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   183,   183,   183,   184,   185,
     186,   187,   188,   189,   190,   190,   190,   190,   190,   191,
     191,   191,   191,   191,   191,   191,   191,   192,   192,   192,
     193,   194,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   196,   197,   198,   199,   200,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202
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
       3,     3,     3,     2,     3,     3,     2,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     2,     2,     3
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
#line 215 "parser.y"
                 { eval_value = (yyvsp[0].uval); }
#line 2426 "y.tab.c"
    break;

  case 61: /* command: '\n'  */
#line 218 "parser.y"
      {
      }
#line 2433 "y.tab.c"
    break;

  case 63: /* BX_TOKEN_TOGGLE_ON_OFF: BX_TOKEN_OFF  */
#line 225 "parser.y"
    { (yyval.bval)=(yyvsp[0].bval); }
#line 2439 "y.tab.c"
    break;

  case 65: /* BX_TOKEN_REGISTERS: BX_TOKEN_REGS  */
#line 231 "parser.y"
    { (yyval.sval)=(yyvsp[0].sval); }
#line 2445 "y.tab.c"
    break;

  case 71: /* BX_TOKEN_SEGREG: BX_TOKEN_GS  */
#line 241 "parser.y"
    { (yyval.uval)=(yyvsp[0].uval); }
#line 2451 "y.tab.c"
    break;

  case 72: /* timebp_command: BX_TOKEN_TIMEBP expression '\n'  */
#line 246 "parser.y"
      {
          bx_dbg_timebp_command(0, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2460 "y.tab.c"
    break;

  case 73: /* timebp_command: BX_TOKEN_TIMEBP_ABSOLUTE expression '\n'  */
#line 251 "parser.y"
      {
          bx_dbg_timebp_command(1, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2469 "y.tab.c"
    break;

  case 74: /* modebp_command: BX_TOKEN_MODEBP '\n'  */
#line 259 "parser.y"
      {
          bx_dbg_modebp_command();
          free((yyvsp[-1].sval));
      }
#line 2478 "y.tab.c"
    break;

  case 75: /* vmexitbp_command: BX_TOKEN_VMEXITBP '\n'  */
#line 267 "parser.y"
      {
          bx_dbg_vmexitbp_command();
          free((yyvsp[-1].sval));
      }
#line 2487 "y.tab.c"
    break;

  case 76: /* show_command: BX_TOKEN_SHOW BX_TOKEN_GENERIC '\n'  */
#line 275 "parser.y"
      {
          bx_dbg_show_command((yyvsp[-1].sval));
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2496 "y.tab.c"
    break;

  case 77: /* show_command: BX_TOKEN_SHOW BX_TOKEN_ALL '\n'  */
#line 280 "parser.y"
      {
          bx_dbg_show_command("all");
          free((yyvsp[-2].sval));
      }
#line 2505 "y.tab.c"
    break;

  case 78: /* show_command: BX_TOKEN_SHOW BX_TOKEN_OFF '\n'  */
#line 285 "parser.y"
      {
          bx_dbg_show_command("off");
          free((yyvsp[-2].sval));
      }
#line 2514 "y.tab.c"
    break;

  case 79: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING '\n'  */
#line 290 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-1].sval), 0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2523 "y.tab.c"
    break;

  case 80: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING BX_TOKEN_XML '\n'  */
#line 295 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-2].sval), 1);
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2532 "y.tab.c"
    break;

  case 81: /* show_command: BX_TOKEN_SHOW '\n'  */
#line 300 "parser.y"
      {
          bx_dbg_show_command(0);
          free((yyvsp[-1].sval));
      }
#line 2541 "y.tab.c"
    break;

  case 82: /* page_command: BX_TOKEN_PAGE expression '\n'  */
#line 308 "parser.y"
      {
          bx_dbg_xlate_address((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2550 "y.tab.c"
    break;

  case 83: /* tlb_command: BX_TOKEN_TLB expression '\n'  */
#line 316 "parser.y"
      {
          bx_dbg_tlb_lookup((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2559 "y.tab.c"
    break;

  case 84: /* ptime_command: BX_TOKEN_PTIME '\n'  */
#line 324 "parser.y"
      {
          bx_dbg_ptime_command();
          free((yyvsp[-1].sval));
      }
#line 2568 "y.tab.c"
    break;

  case 85: /* trace_command: BX_TOKEN_TRACE BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 332 "parser.y"
      {
          bx_dbg_trace_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2577 "y.tab.c"
    break;

  case 86: /* trace_reg_command: BX_TOKEN_TRACEREG BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 340 "parser.y"
      {
          bx_dbg_trace_reg_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2586 "y.tab.c"
    break;

  case 87: /* trace_mem_command: BX_TOKEN_TRACEMEM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 348 "parser.y"
      {
          bx_dbg_trace_mem_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2595 "y.tab.c"
    break;

  case 88: /* print_stack_command: BX_TOKEN_PRINT_STACK '\n'  */
#line 356 "parser.y"
      {
          bx_dbg_print_stack_command(16);
          free((yyvsp[-1].sval));
      }
#line 2604 "y.tab.c"
    break;

  case 89: /* print_stack_command: BX_TOKEN_PRINT_STACK BX_TOKEN_NUMERIC '\n'  */
#line 361 "parser.y"
      {
          bx_dbg_print_stack_command((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2613 "y.tab.c"
    break;

  case 90: /* backtrace_command: BX_TOKEN_BT '\n'  */
#line 369 "parser.y"
      {
        bx_dbg_bt_command(16);
        free((yyvsp[-1].sval));
      }
#line 2622 "y.tab.c"
    break;

  case 91: /* backtrace_command: BX_TOKEN_BT BX_TOKEN_NUMERIC '\n'  */
#line 374 "parser.y"
      {
        bx_dbg_bt_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2631 "y.tab.c"
    break;

  case 92: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_STOP '\n'  */
#line 382 "parser.y"
      {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2640 "y.tab.c"
    break;

  case 93: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_CONTINUE '\n'  */
#line 387 "parser.y"
      {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2649 "y.tab.c"
    break;

  case 94: /* watch_point_command: BX_TOKEN_WATCH '\n'  */
#line 392 "parser.y"
      {
          bx_dbg_print_watchpoints();
          free((yyvsp[-1].sval));
      }
#line 2658 "y.tab.c"
    break;

  case 95: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression '\n'  */
#line 397 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2667 "y.tab.c"
    break;

  case 96: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression '\n'  */
#line 402 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2676 "y.tab.c"
    break;

  case 97: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression '\n'  */
#line 407 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-1].uval), 1); /* BX_WRITE */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2685 "y.tab.c"
    break;

  case 98: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression expression '\n'  */
#line 412 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2694 "y.tab.c"
    break;

  case 99: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression expression '\n'  */
#line 417 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2703 "y.tab.c"
    break;

  case 100: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression expression '\n'  */
#line 422 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_WRITE */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2712 "y.tab.c"
    break;

  case 101: /* watch_point_command: BX_TOKEN_UNWATCH '\n'  */
#line 427 "parser.y"
      {
          bx_dbg_unwatch_all();
          free((yyvsp[-1].sval));
      }
#line 2721 "y.tab.c"
    break;

  case 102: /* watch_point_command: BX_TOKEN_UNWATCH expression '\n'  */
#line 432 "parser.y"
      {
          bx_dbg_unwatch((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2730 "y.tab.c"
    break;

  case 103: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 440 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2739 "y.tab.c"
    break;

  case 104: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING expression '\n'  */
#line 445 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 0, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2748 "y.tab.c"
    break;

  case 105: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING '\n'  */
#line 450 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 1, 0);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2757 "y.tab.c"
    break;

  case 106: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING expression '\n'  */
#line 455 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 1, (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2766 "y.tab.c"
    break;

  case 107: /* where_command: BX_TOKEN_WHERE '\n'  */
#line 463 "parser.y"
      {
        bx_dbg_where_command();
        free((yyvsp[-1].sval));
      }
#line 2775 "y.tab.c"
    break;

  case 108: /* print_string_command: BX_TOKEN_PRINT_STRING expression '\n'  */
#line 471 "parser.y"
      {
        bx_dbg_print_string_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2784 "y.tab.c"
    break;

  case 109: /* continue_command: BX_TOKEN_CONTINUE '\n'  */
#line 479 "parser.y"
      {
        bx_dbg_continue_command(1);
        free((yyvsp[-1].sval));
      }
#line 2793 "y.tab.c"
    break;

  case 110: /* continue_command: BX_TOKEN_CONTINUE BX_TOKEN_IF expression '\n'  */
#line 484 "parser.y"
      {
        bx_dbg_continue_command((yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2802 "y.tab.c"
    break;

  case 111: /* stepN_command: BX_TOKEN_STEPN '\n'  */
#line 492 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[-1].sval));
      }
#line 2811 "y.tab.c"
    break;

  case 112: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC '\n'  */
#line 497 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2820 "y.tab.c"
    break;

  case 113: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_ALL BX_TOKEN_NUMERIC '\n'  */
#line 502 "parser.y"
      {
        bx_dbg_stepN_command(-1, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2829 "y.tab.c"
    break;

  case 114: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'  */
#line 507 "parser.y"
      {
        bx_dbg_stepN_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2838 "y.tab.c"
    break;

  case 115: /* step_over_command: BX_TOKEN_STEP_OVER '\n'  */
#line 515 "parser.y"
      {
        bx_dbg_step_over_command();
        free((yyvsp[-1].sval));
      }
#line 2847 "y.tab.c"
    break;

  case 116: /* set_command: BX_TOKEN_SET BX_TOKEN_DISASM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 523 "parser.y"
      {
        bx_dbg_set_auto_disassemble((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2856 "y.tab.c"
    break;

  case 117: /* set_command: BX_TOKEN_SET BX_TOKEN_SYMBOLNAME '=' expression '\n'  */
#line 528 "parser.y"
      {
        bx_dbg_set_symbol_command((yyvsp[-3].sval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2865 "y.tab.c"
    break;

  case 118: /* set_command: BX_TOKEN_SET BX_TOKEN_8BL_REG '=' expression '\n'  */
#line 533 "parser.y"
      {
        bx_dbg_set_reg8l_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2873 "y.tab.c"
    break;

  case 119: /* set_command: BX_TOKEN_SET BX_TOKEN_8BH_REG '=' expression '\n'  */
#line 537 "parser.y"
      {
        bx_dbg_set_reg8h_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2881 "y.tab.c"
    break;

  case 120: /* set_command: BX_TOKEN_SET BX_TOKEN_16B_REG '=' expression '\n'  */
#line 541 "parser.y"
      {
        bx_dbg_set_reg16_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2889 "y.tab.c"
    break;

  case 121: /* set_command: BX_TOKEN_SET BX_TOKEN_32B_REG '=' expression '\n'  */
#line 545 "parser.y"
      {
        bx_dbg_set_reg32_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2897 "y.tab.c"
    break;

  case 122: /* set_command: BX_TOKEN_SET BX_TOKEN_64B_REG '=' expression '\n'  */
#line 549 "parser.y"
      {
        bx_dbg_set_reg64_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2905 "y.tab.c"
    break;

  case 123: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_EIP '=' expression '\n'  */
#line 553 "parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2913 "y.tab.c"
    break;

  case 124: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_RIP '=' expression '\n'  */
#line 557 "parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2921 "y.tab.c"
    break;

  case 125: /* set_command: BX_TOKEN_SET BX_TOKEN_SEGREG '=' expression '\n'  */
#line 561 "parser.y"
      {
        bx_dbg_load_segreg((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2929 "y.tab.c"
    break;

  case 126: /* breakpoint_command: BX_TOKEN_VBREAKPOINT '\n'  */
#line 568 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2938 "y.tab.c"
    break;

  case 127: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression '\n'  */
#line 573 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].uval), NULL);
        free((yyvsp[-4].sval));
      }
#line 2947 "y.tab.c"
    break;

  case 128: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 578 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-5].uval), (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-6].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2956 "y.tab.c"
    break;

  case 129: /* breakpoint_command: BX_TOKEN_LBREAKPOINT '\n'  */
#line 583 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2965 "y.tab.c"
    break;

  case 130: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression '\n'  */
#line 588 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 2974 "y.tab.c"
    break;

  case 131: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 593 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2983 "y.tab.c"
    break;

  case 132: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING '\n'  */
#line 598 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-1].sval), NULL);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2992 "y.tab.c"
    break;

  case 133: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 603 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-3].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3001 "y.tab.c"
    break;

  case 134: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '\n'  */
#line 608 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3010 "y.tab.c"
    break;

  case 135: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression '\n'  */
#line 613 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 3019 "y.tab.c"
    break;

  case 136: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 618 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3028 "y.tab.c"
    break;

  case 137: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression '\n'  */
#line 623 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-3].sval));
      }
#line 3037 "y.tab.c"
    break;

  case 138: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 628 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-5].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3046 "y.tab.c"
    break;

  case 139: /* blist_command: BX_TOKEN_LIST_BREAK '\n'  */
#line 636 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-1].sval));
      }
#line 3055 "y.tab.c"
    break;

  case 140: /* slist_command: BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 644 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-1].sval));
      }
#line 3064 "y.tab.c"
    break;

  case 141: /* slist_command: BX_TOKEN_LIST_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 649 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 3073 "y.tab.c"
    break;

  case 142: /* info_command: BX_TOKEN_INFO BX_TOKEN_PBREAKPOINT '\n'  */
#line 657 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3082 "y.tab.c"
    break;

  case 143: /* info_command: BX_TOKEN_INFO BX_TOKEN_CPU '\n'  */
#line 662 "parser.y"
      {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3091 "y.tab.c"
    break;

  case 144: /* info_command: BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric optional_numeric '\n'  */
#line 667 "parser.y"
      {
        bx_dbg_info_idt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3100 "y.tab.c"
    break;

  case 145: /* info_command: BX_TOKEN_INFO BX_TOKEN_IVT optional_numeric optional_numeric '\n'  */
#line 672 "parser.y"
      {
        bx_dbg_info_ivt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3109 "y.tab.c"
    break;

  case 146: /* info_command: BX_TOKEN_INFO BX_TOKEN_GDT optional_numeric optional_numeric '\n'  */
#line 677 "parser.y"
      {
        bx_dbg_info_gdt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3118 "y.tab.c"
    break;

  case 147: /* info_command: BX_TOKEN_INFO BX_TOKEN_LDT optional_numeric optional_numeric '\n'  */
#line 682 "parser.y"
      {
        bx_dbg_info_ldt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3127 "y.tab.c"
    break;

  case 148: /* info_command: BX_TOKEN_INFO BX_TOKEN_TAB '\n'  */
#line 687 "parser.y"
      {
        bx_dbg_dump_table();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3136 "y.tab.c"
    break;

  case 149: /* info_command: BX_TOKEN_INFO BX_TOKEN_TSS '\n'  */
#line 692 "parser.y"
      {
        bx_dbg_info_tss_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3145 "y.tab.c"
    break;

  case 150: /* info_command: BX_TOKEN_INFO BX_TOKEN_FLAGS '\n'  */
#line 697 "parser.y"
      {
        bx_dbg_info_flags();
        free((yyvsp[-2].sval));
      }
#line 3154 "y.tab.c"
    break;

  case 151: /* info_command: BX_TOKEN_INFO BX_TOKEN_LINUX '\n'  */
#line 702 "parser.y"
      {
        bx_dbg_info_linux_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3163 "y.tab.c"
    break;

  case 152: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS '\n'  */
#line 707 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3172 "y.tab.c"
    break;

  case 153: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 712 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3181 "y.tab.c"
    break;

  case 154: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE '\n'  */
#line 717 "parser.y"
      {
        bx_dbg_info_device("", "");
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3190 "y.tab.c"
    break;

  case 155: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC '\n'  */
#line 722 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3199 "y.tab.c"
    break;

  case 156: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC BX_TOKEN_STRING '\n'  */
#line 727 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3208 "y.tab.c"
    break;

  case 157: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING '\n'  */
#line 732 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3217 "y.tab.c"
    break;

  case 158: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 737 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3226 "y.tab.c"
    break;

  case 159: /* optional_numeric: %empty  */
#line 744 "parser.y"
               { (yyval.uval) = EMPTY_ARG; }
#line 3232 "y.tab.c"
    break;

  case 161: /* regs_command: BX_TOKEN_REGISTERS '\n'  */
#line 749 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3241 "y.tab.c"
    break;

  case 162: /* fpu_regs_command: BX_TOKEN_FPU '\n'  */
#line 757 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[-1].sval));
      }
#line 3250 "y.tab.c"
    break;

  case 163: /* mmx_regs_command: BX_TOKEN_MMX '\n'  */
#line 765 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3259 "y.tab.c"
    break;

  case 164: /* xmm_regs_command: BX_TOKEN_XMM '\n'  */
#line 773 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3268 "y.tab.c"
    break;

  case 165: /* ymm_regs_command: BX_TOKEN_YMM '\n'  */
#line 781 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3277 "y.tab.c"
    break;

  case 166: /* zmm_regs_command: BX_TOKEN_ZMM '\n'  */
#line 789 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3286 "y.tab.c"
    break;

  case 167: /* segment_regs_command: BX_TOKEN_SEGMENT_REGS '\n'  */
#line 797 "parser.y"
      {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3295 "y.tab.c"
    break;

  case 168: /* control_regs_command: BX_TOKEN_CONTROL_REGS '\n'  */
#line 805 "parser.y"
      {
        bx_dbg_info_control_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3304 "y.tab.c"
    break;

  case 169: /* debug_regs_command: BX_TOKEN_DEBUG_REGS '\n'  */
#line 813 "parser.y"
      {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3313 "y.tab.c"
    break;

  case 170: /* delete_command: BX_TOKEN_DEL_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 821 "parser.y"
      {
        bx_dbg_del_breakpoint_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3322 "y.tab.c"
    break;

  case 171: /* bpe_command: BX_TOKEN_ENABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 829 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3331 "y.tab.c"
    break;

  case 172: /* bpd_command: BX_TOKEN_DISABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 836 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 0);
        free((yyvsp[-2].sval));
      }
#line 3340 "y.tab.c"
    break;

  case 173: /* quit_command: BX_TOKEN_QUIT '\n'  */
#line 844 "parser.y"
      {
        bx_dbg_quit_command();
        free((yyvsp[-1].sval));
      }
#line 3349 "y.tab.c"
    break;

  case 174: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT expression '\n'  */
#line 852 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-3].sval), (yyvsp[-2].sval),1, (yyvsp[-1].uval), 1);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3358 "y.tab.c"
    break;

  case 175: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT '\n'  */
#line 857 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), (yyvsp[-1].sval),1, 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3367 "y.tab.c"
    break;

  case 176: /* examine_command: BX_TOKEN_EXAMINE expression '\n'  */
#line 862 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), NULL,0, (yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3376 "y.tab.c"
    break;

  case 177: /* examine_command: BX_TOKEN_EXAMINE '\n'  */
#line 867 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-1].sval), NULL,0, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 3385 "y.tab.c"
    break;

  case 178: /* restore_command: BX_TOKEN_RESTORE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 875 "parser.y"
      {
        bx_dbg_restore_command((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3394 "y.tab.c"
    break;

  case 179: /* writemem_command: BX_TOKEN_WRITEMEM BX_TOKEN_STRING expression expression '\n'  */
#line 883 "parser.y"
      {
        bx_dbg_writemem_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3403 "y.tab.c"
    break;

  case 180: /* loadmem_command: BX_TOKEN_LOADMEM BX_TOKEN_STRING expression '\n'  */
#line 891 "parser.y"
      {
        bx_dbg_loadmem_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3412 "y.tab.c"
    break;

  case 181: /* setpmem_command: BX_TOKEN_SETPMEM expression expression expression '\n'  */
#line 899 "parser.y"
      {
        bx_dbg_setpmem_command((yyvsp[-3].uval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3421 "y.tab.c"
    break;

  case 182: /* deref_command: BX_TOKEN_DEREF expression expression '\n'  */
#line 907 "parser.y"
      {
        bx_dbg_deref_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3430 "y.tab.c"
    break;

  case 183: /* query_command: BX_TOKEN_QUERY BX_TOKEN_PENDING '\n'  */
#line 915 "parser.y"
      {
        bx_dbg_query_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3439 "y.tab.c"
    break;

  case 184: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA '\n'  */
#line 923 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3448 "y.tab.c"
    break;

  case 185: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA BX_TOKEN_NUMERIC '\n'  */
#line 928 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3457 "y.tab.c"
    break;

  case 186: /* take_command: BX_TOKEN_TAKE BX_TOKEN_IRQ '\n'  */
#line 933 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3466 "y.tab.c"
    break;

  case 187: /* take_command: BX_TOKEN_TAKE BX_TOKEN_SMI '\n'  */
#line 938 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3475 "y.tab.c"
    break;

  case 188: /* take_command: BX_TOKEN_TAKE BX_TOKEN_NMI '\n'  */
#line 943 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3484 "y.tab.c"
    break;

  case 189: /* disassemble_command: BX_TOKEN_DISASM '\n'  */
#line 951 "parser.y"
      {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[-1].sval));
      }
#line 3493 "y.tab.c"
    break;

  case 190: /* disassemble_command: BX_TOKEN_DISASM expression '\n'  */
#line 956 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3502 "y.tab.c"
    break;

  case 191: /* disassemble_command: BX_TOKEN_DISASM expression expression '\n'  */
#line 961 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3511 "y.tab.c"
    break;

  case 192: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT '\n'  */
#line 966 "parser.y"
      {
        bx_dbg_disassemble_current((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3520 "y.tab.c"
    break;

  case 193: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression '\n'  */
#line 971 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-2].sval), (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3529 "y.tab.c"
    break;

  case 194: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression expression '\n'  */
#line 976 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3538 "y.tab.c"
    break;

  case 195: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SWITCH_MODE '\n'  */
#line 981 "parser.y"
      {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3547 "y.tab.c"
    break;

  case 196: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SIZE '=' BX_TOKEN_NUMERIC '\n'  */
#line 986 "parser.y"
      {
        bx_dbg_set_disassemble_size((yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3556 "y.tab.c"
    break;

  case 197: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STOP '\n'  */
#line 994 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3565 "y.tab.c"
    break;

  case 198: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STRING '\n'  */
#line 1000 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3574 "y.tab.c"
    break;

  case 199: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_GENERIC '\n'  */
#line 1005 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3583 "y.tab.c"
    break;

  case 200: /* doit_command: BX_TOKEN_DOIT expression '\n'  */
#line 1013 "parser.y"
      {
        bx_dbg_doit_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3592 "y.tab.c"
    break;

  case 201: /* crc_command: BX_TOKEN_CRC expression expression '\n'  */
#line 1021 "parser.y"
      {
        bx_dbg_crc_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3601 "y.tab.c"
    break;

  case 202: /* help_command: BX_TOKEN_HELP BX_TOKEN_QUIT '\n'  */
#line 1029 "parser.y"
       {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3610 "y.tab.c"
    break;

  case 203: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTINUE '\n'  */
#line 1034 "parser.y"
       {
         dbg_printf("c|cont|continue - continue executing\n");
         dbg_printf("c|cont|continue if \"expression\" - continue executing only if expression is true\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3620 "y.tab.c"
    break;

  case 204: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEPN '\n'  */
#line 1040 "parser.y"
       {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3631 "y.tab.c"
    break;

  case 205: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEP_OVER '\n'  */
#line 1047 "parser.y"
       {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3640 "y.tab.c"
    break;

  case 206: /* help_command: BX_TOKEN_HELP BX_TOKEN_VBREAKPOINT '\n'  */
#line 1052 "parser.y"
       {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         dbg_printf("vb|vbreak <seg:offset> if \"expression\" - set a conditional virtual address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3650 "y.tab.c"
    break;

  case 207: /* help_command: BX_TOKEN_HELP BX_TOKEN_LBREAKPOINT '\n'  */
#line 1058 "parser.y"
       {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         dbg_printf("lb|lbreak <addr> if \"expression\" - set a conditional linear address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3660 "y.tab.c"
    break;

  case 208: /* help_command: BX_TOKEN_HELP BX_TOKEN_PBREAKPOINT '\n'  */
#line 1064 "parser.y"
       {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         dbg_printf("p|pb|break|pbreak <addr> if \"expression\" - set a conditional physical address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3670 "y.tab.c"
    break;

  case 209: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEL_BREAKPOINT '\n'  */
#line 1070 "parser.y"
       {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3679 "y.tab.c"
    break;

  case 210: /* help_command: BX_TOKEN_HELP BX_TOKEN_ENABLE_BREAKPOINT '\n'  */
#line 1075 "parser.y"
       {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3688 "y.tab.c"
    break;

  case 211: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISABLE_BREAKPOINT '\n'  */
#line 1080 "parser.y"
       {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3697 "y.tab.c"
    break;

  case 212: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_BREAK '\n'  */
#line 1085 "parser.y"
       {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3706 "y.tab.c"
    break;

  case 213: /* help_command: BX_TOKEN_HELP BX_TOKEN_MODEBP '\n'  */
#line 1090 "parser.y"
       {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3715 "y.tab.c"
    break;

  case 214: /* help_command: BX_TOKEN_HELP BX_TOKEN_VMEXITBP '\n'  */
#line 1095 "parser.y"
       {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3724 "y.tab.c"
    break;

  case 215: /* help_command: BX_TOKEN_HELP BX_TOKEN_CRC '\n'  */
#line 1100 "parser.y"
       {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3733 "y.tab.c"
    break;

  case 216: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACE '\n'  */
#line 1105 "parser.y"
       {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3743 "y.tab.c"
    break;

  case 217: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEREG '\n'  */
#line 1111 "parser.y"
       {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3753 "y.tab.c"
    break;

  case 218: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEMEM '\n'  */
#line 1117 "parser.y"
       {
         dbg_printf("trace-mem on  - print all memory accesses occurred during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3763 "y.tab.c"
    break;

  case 219: /* help_command: BX_TOKEN_HELP BX_TOKEN_RESTORE '\n'  */
#line 1123 "parser.y"
       {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3775 "y.tab.c"
    break;

  case 220: /* help_command: BX_TOKEN_HELP BX_TOKEN_PTIME '\n'  */
#line 1131 "parser.y"
       {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3784 "y.tab.c"
    break;

  case 221: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP '\n'  */
#line 1136 "parser.y"
       {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3793 "y.tab.c"
    break;

  case 222: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP_ABSOLUTE '\n'  */
#line 1141 "parser.y"
       {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3802 "y.tab.c"
    break;

  case 223: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STACK '\n'  */
#line 1146 "parser.y"
       {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3811 "y.tab.c"
    break;

  case 224: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOAD_SYMBOLS '\n'  */
#line 1151 "parser.y"
       {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3820 "y.tab.c"
    break;

  case 225: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 1156 "parser.y"
       {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3829 "y.tab.c"
    break;

  case 226: /* help_command: BX_TOKEN_HELP BX_TOKEN_REGISTERS '\n'  */
#line 1161 "parser.y"
       {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3838 "y.tab.c"
    break;

  case 227: /* help_command: BX_TOKEN_HELP BX_TOKEN_FPU '\n'  */
#line 1166 "parser.y"
       {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3847 "y.tab.c"
    break;

  case 228: /* help_command: BX_TOKEN_HELP BX_TOKEN_MMX '\n'  */
#line 1171 "parser.y"
       {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3856 "y.tab.c"
    break;

  case 229: /* help_command: BX_TOKEN_HELP BX_TOKEN_XMM '\n'  */
#line 1176 "parser.y"
       {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3865 "y.tab.c"
    break;

  case 230: /* help_command: BX_TOKEN_HELP BX_TOKEN_YMM '\n'  */
#line 1181 "parser.y"
       {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3874 "y.tab.c"
    break;

  case 231: /* help_command: BX_TOKEN_HELP BX_TOKEN_ZMM '\n'  */
#line 1186 "parser.y"
       {
         dbg_printf("zmm - print AVX-512 state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3883 "y.tab.c"
    break;

  case 232: /* help_command: BX_TOKEN_HELP BX_TOKEN_SEGMENT_REGS '\n'  */
#line 1191 "parser.y"
       {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3892 "y.tab.c"
    break;

  case 233: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTROL_REGS '\n'  */
#line 1196 "parser.y"
       {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3901 "y.tab.c"
    break;

  case 234: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEBUG_REGS '\n'  */
#line 1201 "parser.y"
       {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3910 "y.tab.c"
    break;

  case 235: /* help_command: BX_TOKEN_HELP BX_TOKEN_WRITEMEM '\n'  */
#line 1206 "parser.y"
       {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3919 "y.tab.c"
    break;

  case 236: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOADMEM '\n'  */
#line 1211 "parser.y"
       {
         dbg_printf("loadmem <filename> <laddr> - load file bytes to virtual memory starting from the linear address 'laddr'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3928 "y.tab.c"
    break;

  case 237: /* help_command: BX_TOKEN_HELP BX_TOKEN_SETPMEM '\n'  */
#line 1216 "parser.y"
       {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3937 "y.tab.c"
    break;

  case 238: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEREF '\n'  */
#line 1221 "parser.y"
       {
         dbg_printf("deref <addr> <deep> - pointer dereference. For example: get value of [[[rax]]] or ***rax: deref rax 3\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3946 "y.tab.c"
    break;

  case 239: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISASM '\n'  */
#line 1226 "parser.y"
       {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm hex on/off - control disasm offsets and displacements format\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3960 "y.tab.c"
    break;

  case 240: /* help_command: BX_TOKEN_HELP BX_TOKEN_WATCH '\n'  */
#line 1236 "parser.y"
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
#line 3975 "y.tab.c"
    break;

  case 241: /* help_command: BX_TOKEN_HELP BX_TOKEN_UNWATCH '\n'  */
#line 1247 "parser.y"
       {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3985 "y.tab.c"
    break;

  case 242: /* help_command: BX_TOKEN_HELP BX_TOKEN_EXAMINE '\n'  */
#line 1253 "parser.y"
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
#line 4002 "y.tab.c"
    break;

  case 243: /* help_command: BX_TOKEN_HELP BX_TOKEN_INSTRUMENT '\n'  */
#line 1266 "parser.y"
       {
         dbg_printf("instrument <command|\"string command\"> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command|\"string command\">\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4011 "y.tab.c"
    break;

  case 244: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET '\n'  */
#line 1271 "parser.y"
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
#line 4026 "y.tab.c"
    break;

  case 245: /* help_command: BX_TOKEN_HELP BX_TOKEN_PAGE '\n'  */
#line 1282 "parser.y"
       {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4035 "y.tab.c"
    break;

  case 246: /* help_command: BX_TOKEN_HELP BX_TOKEN_INFO '\n'  */
#line 1287 "parser.y"
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
#line 4055 "y.tab.c"
    break;

  case 247: /* help_command: BX_TOKEN_HELP BX_TOKEN_SHOW '\n'  */
#line 1303 "parser.y"
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
#line 4075 "y.tab.c"
    break;

  case 248: /* help_command: BX_TOKEN_HELP BX_TOKEN_CALC '\n'  */
#line 1319 "parser.y"
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
#line 4091 "y.tab.c"
    break;

  case 249: /* help_command: BX_TOKEN_HELP BX_TOKEN_ADDLYT '\n'  */
#line 1331 "parser.y"
       {
         dbg_printf("addlyt file - cause debugger to execute a script file every time execution stops.\n");
         dbg_printf("    Example of use: 1. Create a script file (script.txt) with the following content:\n");
         dbg_printf("             regs\n");
         dbg_printf("             print-stack 7\n");
         dbg_printf("             u /10\n");
         dbg_printf("             <EMPTY NEW LINE>\n");
         dbg_printf("    2. Execute: addlyt \"script.txt\"\n");
         dbg_printf("    Then, when you execute a step/DebugBreak... you will see: registers, stack and disasm.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4107 "y.tab.c"
    break;

  case 250: /* help_command: BX_TOKEN_HELP BX_TOKEN_REMLYT '\n'  */
#line 1343 "parser.y"
       {
         dbg_printf("remlyt - stops debugger to execute the script file added previously with addlyt command.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4116 "y.tab.c"
    break;

  case 251: /* help_command: BX_TOKEN_HELP BX_TOKEN_LYT '\n'  */
#line 1348 "parser.y"
       {
         dbg_printf("lyt - cause debugger to execute script file added previously with addlyt command.\n");
         dbg_printf("    Use it as a refresh/context.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4126 "y.tab.c"
    break;

  case 252: /* help_command: BX_TOKEN_HELP BX_TOKEN_HELP '\n'  */
#line 1354 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4135 "y.tab.c"
    break;

  case 253: /* help_command: BX_TOKEN_HELP '\n'  */
#line 1359 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-1].sval));
       }
#line 4144 "y.tab.c"
    break;

  case 254: /* calc_command: BX_TOKEN_CALC expression '\n'  */
#line 1367 "parser.y"
   {
     eval_value = (yyvsp[-1].uval);
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4154 "y.tab.c"
    break;

  case 255: /* addlyt_command: BX_TOKEN_ADDLYT BX_TOKEN_STRING '\n'  */
#line 1376 "parser.y"
   {
     bx_dbg_addlyt((yyvsp[-1].sval));
     free((yyvsp[-2].sval));
     free((yyvsp[-1].sval));
   }
#line 4164 "y.tab.c"
    break;

  case 256: /* remlyt_command: BX_TOKEN_REMLYT '\n'  */
#line 1385 "parser.y"
   {
     bx_dbg_remlyt();
     free((yyvsp[-1].sval));
   }
#line 4173 "y.tab.c"
    break;

  case 257: /* lyt_command: BX_TOKEN_LYT '\n'  */
#line 1393 "parser.y"
   {
     bx_dbg_lyt();
     free((yyvsp[-1].sval));
   }
#line 4182 "y.tab.c"
    break;

  case 258: /* if_command: BX_TOKEN_IF expression '\n'  */
#line 1401 "parser.y"
   {
     eval_value = (yyvsp[-1].uval) != 0;
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4192 "y.tab.c"
    break;

  case 259: /* vexpression: BX_TOKEN_NUMERIC  */
#line 1410 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4198 "y.tab.c"
    break;

  case 260: /* vexpression: BX_TOKEN_STRING  */
#line 1411 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4204 "y.tab.c"
    break;

  case 261: /* vexpression: BX_TOKEN_8BL_REG  */
#line 1412 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4210 "y.tab.c"
    break;

  case 262: /* vexpression: BX_TOKEN_8BH_REG  */
#line 1413 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4216 "y.tab.c"
    break;

  case 263: /* vexpression: BX_TOKEN_16B_REG  */
#line 1414 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4222 "y.tab.c"
    break;

  case 264: /* vexpression: BX_TOKEN_32B_REG  */
#line 1415 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4228 "y.tab.c"
    break;

  case 265: /* vexpression: BX_TOKEN_64B_REG  */
#line 1416 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4234 "y.tab.c"
    break;

  case 266: /* vexpression: BX_TOKEN_OPMASK_REG  */
#line 1417 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4240 "y.tab.c"
    break;

  case 267: /* vexpression: BX_TOKEN_SEGREG  */
#line 1418 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4246 "y.tab.c"
    break;

  case 268: /* vexpression: BX_TOKEN_REG_IP  */
#line 1419 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4252 "y.tab.c"
    break;

  case 269: /* vexpression: BX_TOKEN_REG_EIP  */
#line 1420 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4258 "y.tab.c"
    break;

  case 270: /* vexpression: BX_TOKEN_REG_RIP  */
#line 1421 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4264 "y.tab.c"
    break;

  case 271: /* vexpression: BX_TOKEN_REG_SSP  */
#line 1422 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4270 "y.tab.c"
    break;

  case 272: /* vexpression: vexpression '+' vexpression  */
#line 1423 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4276 "y.tab.c"
    break;

  case 273: /* vexpression: vexpression '-' vexpression  */
#line 1424 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4282 "y.tab.c"
    break;

  case 274: /* vexpression: vexpression '*' vexpression  */
#line 1425 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4288 "y.tab.c"
    break;

  case 275: /* vexpression: vexpression '/' vexpression  */
#line 1426 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) / (yyvsp[0].uval); }
#line 4294 "y.tab.c"
    break;

  case 276: /* vexpression: vexpression BX_TOKEN_DEREF_CHR vexpression  */
#line 1427 "parser.y"
                                                { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 4300 "y.tab.c"
    break;

  case 277: /* vexpression: vexpression BX_TOKEN_RSHIFT vexpression  */
#line 1428 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4306 "y.tab.c"
    break;

  case 278: /* vexpression: vexpression BX_TOKEN_LSHIFT vexpression  */
#line 1429 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4312 "y.tab.c"
    break;

  case 279: /* vexpression: vexpression '|' vexpression  */
#line 1430 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4318 "y.tab.c"
    break;

  case 280: /* vexpression: vexpression '^' vexpression  */
#line 1431 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4324 "y.tab.c"
    break;

  case 281: /* vexpression: vexpression '&' vexpression  */
#line 1432 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4330 "y.tab.c"
    break;

  case 282: /* vexpression: '!' vexpression  */
#line 1433 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4336 "y.tab.c"
    break;

  case 283: /* vexpression: '-' vexpression  */
#line 1434 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4342 "y.tab.c"
    break;

  case 284: /* vexpression: '(' vexpression ')'  */
#line 1435 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4348 "y.tab.c"
    break;

  case 285: /* expression: BX_TOKEN_NUMERIC  */
#line 1441 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4354 "y.tab.c"
    break;

  case 286: /* expression: BX_TOKEN_STRING  */
#line 1442 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4360 "y.tab.c"
    break;

  case 287: /* expression: BX_TOKEN_8BL_REG  */
#line 1443 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4366 "y.tab.c"
    break;

  case 288: /* expression: BX_TOKEN_8BH_REG  */
#line 1444 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4372 "y.tab.c"
    break;

  case 289: /* expression: BX_TOKEN_16B_REG  */
#line 1445 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4378 "y.tab.c"
    break;

  case 290: /* expression: BX_TOKEN_32B_REG  */
#line 1446 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4384 "y.tab.c"
    break;

  case 291: /* expression: BX_TOKEN_64B_REG  */
#line 1447 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4390 "y.tab.c"
    break;

  case 292: /* expression: BX_TOKEN_OPMASK_REG  */
#line 1448 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4396 "y.tab.c"
    break;

  case 293: /* expression: BX_TOKEN_SEGREG  */
#line 1449 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4402 "y.tab.c"
    break;

  case 294: /* expression: BX_TOKEN_REG_IP  */
#line 1450 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4408 "y.tab.c"
    break;

  case 295: /* expression: BX_TOKEN_REG_EIP  */
#line 1451 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4414 "y.tab.c"
    break;

  case 296: /* expression: BX_TOKEN_REG_RIP  */
#line 1452 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4420 "y.tab.c"
    break;

  case 297: /* expression: BX_TOKEN_REG_SSP  */
#line 1453 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4426 "y.tab.c"
    break;

  case 298: /* expression: expression ':' expression  */
#line 1454 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[-2].uval), (yyvsp[0].uval)); }
#line 4432 "y.tab.c"
    break;

  case 299: /* expression: expression '+' expression  */
#line 1455 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4438 "y.tab.c"
    break;

  case 300: /* expression: expression '-' expression  */
#line 1456 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4444 "y.tab.c"
    break;

  case 301: /* expression: expression '*' expression  */
#line 1457 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4450 "y.tab.c"
    break;

  case 302: /* expression: expression '/' expression  */
#line 1458 "parser.y"
                                     { (yyval.uval) = ((yyvsp[0].uval) != 0) ? (yyvsp[-2].uval) / (yyvsp[0].uval) : 0; }
#line 4456 "y.tab.c"
    break;

  case 303: /* expression: expression BX_TOKEN_DEREF_CHR expression  */
#line 1459 "parser.y"
                                              { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 4462 "y.tab.c"
    break;

  case 304: /* expression: expression BX_TOKEN_RSHIFT expression  */
#line 1460 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4468 "y.tab.c"
    break;

  case 305: /* expression: expression BX_TOKEN_LSHIFT expression  */
#line 1461 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4474 "y.tab.c"
    break;

  case 306: /* expression: expression '|' expression  */
#line 1462 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4480 "y.tab.c"
    break;

  case 307: /* expression: expression '^' expression  */
#line 1463 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4486 "y.tab.c"
    break;

  case 308: /* expression: expression '&' expression  */
#line 1464 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4492 "y.tab.c"
    break;

  case 309: /* expression: expression '>' expression  */
#line 1465 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) > (yyvsp[0].uval); }
#line 4498 "y.tab.c"
    break;

  case 310: /* expression: expression '<' expression  */
#line 1466 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) < (yyvsp[0].uval); }
#line 4504 "y.tab.c"
    break;

  case 311: /* expression: expression BX_TOKEN_EQ expression  */
#line 1467 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) == (yyvsp[0].uval); }
#line 4510 "y.tab.c"
    break;

  case 312: /* expression: expression BX_TOKEN_NE expression  */
#line 1468 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) != (yyvsp[0].uval); }
#line 4516 "y.tab.c"
    break;

  case 313: /* expression: expression BX_TOKEN_LE expression  */
#line 1469 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) <= (yyvsp[0].uval); }
#line 4522 "y.tab.c"
    break;

  case 314: /* expression: expression BX_TOKEN_GE expression  */
#line 1470 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) >= (yyvsp[0].uval); }
#line 4528 "y.tab.c"
    break;

  case 315: /* expression: '!' expression  */
#line 1471 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4534 "y.tab.c"
    break;

  case 316: /* expression: '-' expression  */
#line 1472 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4540 "y.tab.c"
    break;

  case 317: /* expression: '*' expression  */
#line 1473 "parser.y"
                                     { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[0].uval)); }
#line 4546 "y.tab.c"
    break;

  case 318: /* expression: '@' expression  */
#line 1474 "parser.y"
                                     { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[0].uval)); }
#line 4552 "y.tab.c"
    break;

  case 319: /* expression: '(' expression ')'  */
#line 1475 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4558 "y.tab.c"
    break;


#line 4562 "y.tab.c"

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

#line 1478 "parser.y"

#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
