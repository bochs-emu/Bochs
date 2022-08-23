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
    BX_TOKEN_SYMBOLNAME = 315,     /* BX_TOKEN_SYMBOLNAME  */
    BX_TOKEN_QUERY = 316,          /* BX_TOKEN_QUERY  */
    BX_TOKEN_PENDING = 317,        /* BX_TOKEN_PENDING  */
    BX_TOKEN_TAKE = 318,           /* BX_TOKEN_TAKE  */
    BX_TOKEN_DMA = 319,            /* BX_TOKEN_DMA  */
    BX_TOKEN_IRQ = 320,            /* BX_TOKEN_IRQ  */
    BX_TOKEN_SMI = 321,            /* BX_TOKEN_SMI  */
    BX_TOKEN_NMI = 322,            /* BX_TOKEN_NMI  */
    BX_TOKEN_TLB = 323,            /* BX_TOKEN_TLB  */
    BX_TOKEN_DISASM = 324,         /* BX_TOKEN_DISASM  */
    BX_TOKEN_INSTRUMENT = 325,     /* BX_TOKEN_INSTRUMENT  */
    BX_TOKEN_STRING = 326,         /* BX_TOKEN_STRING  */
    BX_TOKEN_STOP = 327,           /* BX_TOKEN_STOP  */
    BX_TOKEN_DOIT = 328,           /* BX_TOKEN_DOIT  */
    BX_TOKEN_CRC = 329,            /* BX_TOKEN_CRC  */
    BX_TOKEN_TRACE = 330,          /* BX_TOKEN_TRACE  */
    BX_TOKEN_TRACEREG = 331,       /* BX_TOKEN_TRACEREG  */
    BX_TOKEN_TRACEMEM = 332,       /* BX_TOKEN_TRACEMEM  */
    BX_TOKEN_SWITCH_MODE = 333,    /* BX_TOKEN_SWITCH_MODE  */
    BX_TOKEN_SIZE = 334,           /* BX_TOKEN_SIZE  */
    BX_TOKEN_PTIME = 335,          /* BX_TOKEN_PTIME  */
    BX_TOKEN_TIMEBP_ABSOLUTE = 336, /* BX_TOKEN_TIMEBP_ABSOLUTE  */
    BX_TOKEN_TIMEBP = 337,         /* BX_TOKEN_TIMEBP  */
    BX_TOKEN_MODEBP = 338,         /* BX_TOKEN_MODEBP  */
    BX_TOKEN_VMEXITBP = 339,       /* BX_TOKEN_VMEXITBP  */
    BX_TOKEN_PRINT_STACK = 340,    /* BX_TOKEN_PRINT_STACK  */
    BX_TOKEN_BT = 341,             /* BX_TOKEN_BT  */
    BX_TOKEN_WATCH = 342,          /* BX_TOKEN_WATCH  */
    BX_TOKEN_UNWATCH = 343,        /* BX_TOKEN_UNWATCH  */
    BX_TOKEN_READ = 344,           /* BX_TOKEN_READ  */
    BX_TOKEN_WRITE = 345,          /* BX_TOKEN_WRITE  */
    BX_TOKEN_SHOW = 346,           /* BX_TOKEN_SHOW  */
    BX_TOKEN_LOAD_SYMBOLS = 347,   /* BX_TOKEN_LOAD_SYMBOLS  */
    BX_TOKEN_SYMBOLS = 348,        /* BX_TOKEN_SYMBOLS  */
    BX_TOKEN_LIST_SYMBOLS = 349,   /* BX_TOKEN_LIST_SYMBOLS  */
    BX_TOKEN_GLOBAL = 350,         /* BX_TOKEN_GLOBAL  */
    BX_TOKEN_WHERE = 351,          /* BX_TOKEN_WHERE  */
    BX_TOKEN_PRINT_STRING = 352,   /* BX_TOKEN_PRINT_STRING  */
    BX_TOKEN_NUMERIC = 353,        /* BX_TOKEN_NUMERIC  */
    BX_TOKEN_PAGE = 354,           /* BX_TOKEN_PAGE  */
    BX_TOKEN_HELP = 355,           /* BX_TOKEN_HELP  */
    BX_TOKEN_XML = 356,            /* BX_TOKEN_XML  */
    BX_TOKEN_CALC = 357,           /* BX_TOKEN_CALC  */
    BX_TOKEN_DEVICE = 358,         /* BX_TOKEN_DEVICE  */
    BX_TOKEN_GENERIC = 359,        /* BX_TOKEN_GENERIC  */
    BX_TOKEN_RSHIFT = 360,         /* BX_TOKEN_RSHIFT  */
    BX_TOKEN_LSHIFT = 361,         /* BX_TOKEN_LSHIFT  */
    BX_TOKEN_EQ = 362,             /* BX_TOKEN_EQ  */
    BX_TOKEN_NE = 363,             /* BX_TOKEN_NE  */
    BX_TOKEN_LE = 364,             /* BX_TOKEN_LE  */
    BX_TOKEN_GE = 365,             /* BX_TOKEN_GE  */
    BX_TOKEN_REG_IP = 366,         /* BX_TOKEN_REG_IP  */
    BX_TOKEN_REG_EIP = 367,        /* BX_TOKEN_REG_EIP  */
    BX_TOKEN_REG_RIP = 368,        /* BX_TOKEN_REG_RIP  */
    BX_TOKEN_REG_SSP = 369,        /* BX_TOKEN_REG_SSP  */
    NOT = 370,                     /* NOT  */
    NEG = 371,                     /* NEG  */
    INDIRECT = 372                 /* INDIRECT  */
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
#define BX_TOKEN_SYMBOLNAME 315
#define BX_TOKEN_QUERY 316
#define BX_TOKEN_PENDING 317
#define BX_TOKEN_TAKE 318
#define BX_TOKEN_DMA 319
#define BX_TOKEN_IRQ 320
#define BX_TOKEN_SMI 321
#define BX_TOKEN_NMI 322
#define BX_TOKEN_TLB 323
#define BX_TOKEN_DISASM 324
#define BX_TOKEN_INSTRUMENT 325
#define BX_TOKEN_STRING 326
#define BX_TOKEN_STOP 327
#define BX_TOKEN_DOIT 328
#define BX_TOKEN_CRC 329
#define BX_TOKEN_TRACE 330
#define BX_TOKEN_TRACEREG 331
#define BX_TOKEN_TRACEMEM 332
#define BX_TOKEN_SWITCH_MODE 333
#define BX_TOKEN_SIZE 334
#define BX_TOKEN_PTIME 335
#define BX_TOKEN_TIMEBP_ABSOLUTE 336
#define BX_TOKEN_TIMEBP 337
#define BX_TOKEN_MODEBP 338
#define BX_TOKEN_VMEXITBP 339
#define BX_TOKEN_PRINT_STACK 340
#define BX_TOKEN_BT 341
#define BX_TOKEN_WATCH 342
#define BX_TOKEN_UNWATCH 343
#define BX_TOKEN_READ 344
#define BX_TOKEN_WRITE 345
#define BX_TOKEN_SHOW 346
#define BX_TOKEN_LOAD_SYMBOLS 347
#define BX_TOKEN_SYMBOLS 348
#define BX_TOKEN_LIST_SYMBOLS 349
#define BX_TOKEN_GLOBAL 350
#define BX_TOKEN_WHERE 351
#define BX_TOKEN_PRINT_STRING 352
#define BX_TOKEN_NUMERIC 353
#define BX_TOKEN_PAGE 354
#define BX_TOKEN_HELP 355
#define BX_TOKEN_XML 356
#define BX_TOKEN_CALC 357
#define BX_TOKEN_DEVICE 358
#define BX_TOKEN_GENERIC 359
#define BX_TOKEN_RSHIFT 360
#define BX_TOKEN_LSHIFT 361
#define BX_TOKEN_EQ 362
#define BX_TOKEN_NE 363
#define BX_TOKEN_LE 364
#define BX_TOKEN_GE 365
#define BX_TOKEN_REG_IP 366
#define BX_TOKEN_REG_EIP 367
#define BX_TOKEN_REG_RIP 368
#define BX_TOKEN_REG_SSP 369
#define NOT 370
#define NEG 371
#define INDIRECT 372

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 14 "parser.y"

  char    *sval;
  Bit64u   uval;
  unsigned bval;

#line 380 "y.tab.c"

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
  YYSYMBOL_BX_TOKEN_SYMBOLNAME = 60,       /* BX_TOKEN_SYMBOLNAME  */
  YYSYMBOL_BX_TOKEN_QUERY = 61,            /* BX_TOKEN_QUERY  */
  YYSYMBOL_BX_TOKEN_PENDING = 62,          /* BX_TOKEN_PENDING  */
  YYSYMBOL_BX_TOKEN_TAKE = 63,             /* BX_TOKEN_TAKE  */
  YYSYMBOL_BX_TOKEN_DMA = 64,              /* BX_TOKEN_DMA  */
  YYSYMBOL_BX_TOKEN_IRQ = 65,              /* BX_TOKEN_IRQ  */
  YYSYMBOL_BX_TOKEN_SMI = 66,              /* BX_TOKEN_SMI  */
  YYSYMBOL_BX_TOKEN_NMI = 67,              /* BX_TOKEN_NMI  */
  YYSYMBOL_BX_TOKEN_TLB = 68,              /* BX_TOKEN_TLB  */
  YYSYMBOL_BX_TOKEN_DISASM = 69,           /* BX_TOKEN_DISASM  */
  YYSYMBOL_BX_TOKEN_INSTRUMENT = 70,       /* BX_TOKEN_INSTRUMENT  */
  YYSYMBOL_BX_TOKEN_STRING = 71,           /* BX_TOKEN_STRING  */
  YYSYMBOL_BX_TOKEN_STOP = 72,             /* BX_TOKEN_STOP  */
  YYSYMBOL_BX_TOKEN_DOIT = 73,             /* BX_TOKEN_DOIT  */
  YYSYMBOL_BX_TOKEN_CRC = 74,              /* BX_TOKEN_CRC  */
  YYSYMBOL_BX_TOKEN_TRACE = 75,            /* BX_TOKEN_TRACE  */
  YYSYMBOL_BX_TOKEN_TRACEREG = 76,         /* BX_TOKEN_TRACEREG  */
  YYSYMBOL_BX_TOKEN_TRACEMEM = 77,         /* BX_TOKEN_TRACEMEM  */
  YYSYMBOL_BX_TOKEN_SWITCH_MODE = 78,      /* BX_TOKEN_SWITCH_MODE  */
  YYSYMBOL_BX_TOKEN_SIZE = 79,             /* BX_TOKEN_SIZE  */
  YYSYMBOL_BX_TOKEN_PTIME = 80,            /* BX_TOKEN_PTIME  */
  YYSYMBOL_BX_TOKEN_TIMEBP_ABSOLUTE = 81,  /* BX_TOKEN_TIMEBP_ABSOLUTE  */
  YYSYMBOL_BX_TOKEN_TIMEBP = 82,           /* BX_TOKEN_TIMEBP  */
  YYSYMBOL_BX_TOKEN_MODEBP = 83,           /* BX_TOKEN_MODEBP  */
  YYSYMBOL_BX_TOKEN_VMEXITBP = 84,         /* BX_TOKEN_VMEXITBP  */
  YYSYMBOL_BX_TOKEN_PRINT_STACK = 85,      /* BX_TOKEN_PRINT_STACK  */
  YYSYMBOL_BX_TOKEN_BT = 86,               /* BX_TOKEN_BT  */
  YYSYMBOL_BX_TOKEN_WATCH = 87,            /* BX_TOKEN_WATCH  */
  YYSYMBOL_BX_TOKEN_UNWATCH = 88,          /* BX_TOKEN_UNWATCH  */
  YYSYMBOL_BX_TOKEN_READ = 89,             /* BX_TOKEN_READ  */
  YYSYMBOL_BX_TOKEN_WRITE = 90,            /* BX_TOKEN_WRITE  */
  YYSYMBOL_BX_TOKEN_SHOW = 91,             /* BX_TOKEN_SHOW  */
  YYSYMBOL_BX_TOKEN_LOAD_SYMBOLS = 92,     /* BX_TOKEN_LOAD_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_SYMBOLS = 93,          /* BX_TOKEN_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_LIST_SYMBOLS = 94,     /* BX_TOKEN_LIST_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_GLOBAL = 95,           /* BX_TOKEN_GLOBAL  */
  YYSYMBOL_BX_TOKEN_WHERE = 96,            /* BX_TOKEN_WHERE  */
  YYSYMBOL_BX_TOKEN_PRINT_STRING = 97,     /* BX_TOKEN_PRINT_STRING  */
  YYSYMBOL_BX_TOKEN_NUMERIC = 98,          /* BX_TOKEN_NUMERIC  */
  YYSYMBOL_BX_TOKEN_PAGE = 99,             /* BX_TOKEN_PAGE  */
  YYSYMBOL_BX_TOKEN_HELP = 100,            /* BX_TOKEN_HELP  */
  YYSYMBOL_BX_TOKEN_XML = 101,             /* BX_TOKEN_XML  */
  YYSYMBOL_BX_TOKEN_CALC = 102,            /* BX_TOKEN_CALC  */
  YYSYMBOL_BX_TOKEN_DEVICE = 103,          /* BX_TOKEN_DEVICE  */
  YYSYMBOL_BX_TOKEN_GENERIC = 104,         /* BX_TOKEN_GENERIC  */
  YYSYMBOL_BX_TOKEN_RSHIFT = 105,          /* BX_TOKEN_RSHIFT  */
  YYSYMBOL_BX_TOKEN_LSHIFT = 106,          /* BX_TOKEN_LSHIFT  */
  YYSYMBOL_BX_TOKEN_EQ = 107,              /* BX_TOKEN_EQ  */
  YYSYMBOL_BX_TOKEN_NE = 108,              /* BX_TOKEN_NE  */
  YYSYMBOL_BX_TOKEN_LE = 109,              /* BX_TOKEN_LE  */
  YYSYMBOL_BX_TOKEN_GE = 110,              /* BX_TOKEN_GE  */
  YYSYMBOL_BX_TOKEN_REG_IP = 111,          /* BX_TOKEN_REG_IP  */
  YYSYMBOL_BX_TOKEN_REG_EIP = 112,         /* BX_TOKEN_REG_EIP  */
  YYSYMBOL_BX_TOKEN_REG_RIP = 113,         /* BX_TOKEN_REG_RIP  */
  YYSYMBOL_BX_TOKEN_REG_SSP = 114,         /* BX_TOKEN_REG_SSP  */
  YYSYMBOL_115_ = 115,                     /* '+'  */
  YYSYMBOL_116_ = 116,                     /* '-'  */
  YYSYMBOL_117_ = 117,                     /* '|'  */
  YYSYMBOL_118_ = 118,                     /* '^'  */
  YYSYMBOL_119_ = 119,                     /* '<'  */
  YYSYMBOL_120_ = 120,                     /* '>'  */
  YYSYMBOL_121_ = 121,                     /* '*'  */
  YYSYMBOL_122_ = 122,                     /* '/'  */
  YYSYMBOL_123_ = 123,                     /* '&'  */
  YYSYMBOL_NOT = 124,                      /* NOT  */
  YYSYMBOL_NEG = 125,                      /* NEG  */
  YYSYMBOL_INDIRECT = 126,                 /* INDIRECT  */
  YYSYMBOL_127_n_ = 127,                   /* '\n'  */
  YYSYMBOL_128_ = 128,                     /* '='  */
  YYSYMBOL_129_ = 129,                     /* ':'  */
  YYSYMBOL_130_ = 130,                     /* '!'  */
  YYSYMBOL_131_ = 131,                     /* '('  */
  YYSYMBOL_132_ = 132,                     /* ')'  */
  YYSYMBOL_133_ = 133,                     /* '@'  */
  YYSYMBOL_YYACCEPT = 134,                 /* $accept  */
  YYSYMBOL_commands = 135,                 /* commands  */
  YYSYMBOL_command = 136,                  /* command  */
  YYSYMBOL_BX_TOKEN_TOGGLE_ON_OFF = 137,   /* BX_TOKEN_TOGGLE_ON_OFF  */
  YYSYMBOL_BX_TOKEN_REGISTERS = 138,       /* BX_TOKEN_REGISTERS  */
  YYSYMBOL_BX_TOKEN_SEGREG = 139,          /* BX_TOKEN_SEGREG  */
  YYSYMBOL_timebp_command = 140,           /* timebp_command  */
  YYSYMBOL_modebp_command = 141,           /* modebp_command  */
  YYSYMBOL_vmexitbp_command = 142,         /* vmexitbp_command  */
  YYSYMBOL_show_command = 143,             /* show_command  */
  YYSYMBOL_page_command = 144,             /* page_command  */
  YYSYMBOL_tlb_command = 145,              /* tlb_command  */
  YYSYMBOL_ptime_command = 146,            /* ptime_command  */
  YYSYMBOL_trace_command = 147,            /* trace_command  */
  YYSYMBOL_trace_reg_command = 148,        /* trace_reg_command  */
  YYSYMBOL_trace_mem_command = 149,        /* trace_mem_command  */
  YYSYMBOL_print_stack_command = 150,      /* print_stack_command  */
  YYSYMBOL_backtrace_command = 151,        /* backtrace_command  */
  YYSYMBOL_watch_point_command = 152,      /* watch_point_command  */
  YYSYMBOL_symbol_command = 153,           /* symbol_command  */
  YYSYMBOL_where_command = 154,            /* where_command  */
  YYSYMBOL_print_string_command = 155,     /* print_string_command  */
  YYSYMBOL_continue_command = 156,         /* continue_command  */
  YYSYMBOL_stepN_command = 157,            /* stepN_command  */
  YYSYMBOL_step_over_command = 158,        /* step_over_command  */
  YYSYMBOL_set_command = 159,              /* set_command  */
  YYSYMBOL_breakpoint_command = 160,       /* breakpoint_command  */
  YYSYMBOL_blist_command = 161,            /* blist_command  */
  YYSYMBOL_slist_command = 162,            /* slist_command  */
  YYSYMBOL_info_command = 163,             /* info_command  */
  YYSYMBOL_optional_numeric = 164,         /* optional_numeric  */
  YYSYMBOL_regs_command = 165,             /* regs_command  */
  YYSYMBOL_fpu_regs_command = 166,         /* fpu_regs_command  */
  YYSYMBOL_mmx_regs_command = 167,         /* mmx_regs_command  */
  YYSYMBOL_xmm_regs_command = 168,         /* xmm_regs_command  */
  YYSYMBOL_ymm_regs_command = 169,         /* ymm_regs_command  */
  YYSYMBOL_zmm_regs_command = 170,         /* zmm_regs_command  */
  YYSYMBOL_segment_regs_command = 171,     /* segment_regs_command  */
  YYSYMBOL_control_regs_command = 172,     /* control_regs_command  */
  YYSYMBOL_debug_regs_command = 173,       /* debug_regs_command  */
  YYSYMBOL_delete_command = 174,           /* delete_command  */
  YYSYMBOL_bpe_command = 175,              /* bpe_command  */
  YYSYMBOL_bpd_command = 176,              /* bpd_command  */
  YYSYMBOL_quit_command = 177,             /* quit_command  */
  YYSYMBOL_examine_command = 178,          /* examine_command  */
  YYSYMBOL_restore_command = 179,          /* restore_command  */
  YYSYMBOL_writemem_command = 180,         /* writemem_command  */
  YYSYMBOL_loadmem_command = 181,          /* loadmem_command  */
  YYSYMBOL_setpmem_command = 182,          /* setpmem_command  */
  YYSYMBOL_query_command = 183,            /* query_command  */
  YYSYMBOL_take_command = 184,             /* take_command  */
  YYSYMBOL_disassemble_command = 185,      /* disassemble_command  */
  YYSYMBOL_instrument_command = 186,       /* instrument_command  */
  YYSYMBOL_doit_command = 187,             /* doit_command  */
  YYSYMBOL_crc_command = 188,              /* crc_command  */
  YYSYMBOL_help_command = 189,             /* help_command  */
  YYSYMBOL_calc_command = 190,             /* calc_command  */
  YYSYMBOL_if_command = 191,               /* if_command  */
  YYSYMBOL_vexpression = 192,              /* vexpression  */
  YYSYMBOL_expression = 193                /* expression  */
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
#define YYFINAL  314
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2422

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  134
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  60
/* YYNRULES -- Number of rules.  */
#define YYNRULES  305
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  596

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   372


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
     127,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   130,     2,     2,     2,     2,   123,     2,
     131,   132,   121,   115,     2,   116,     2,   122,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   129,     2,
     119,   128,   120,     2,   133,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   118,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   117,     2,     2,     2,     2,     2,
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
     124,   125,   126
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   150,   150,   151,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   214,   215,
     220,   221,   226,   227,   228,   229,   230,   231,   236,   241,
     249,   257,   265,   270,   275,   280,   285,   290,   298,   306,
     314,   322,   330,   338,   346,   351,   359,   364,   372,   377,
     382,   387,   392,   397,   402,   407,   412,   417,   422,   430,
     435,   440,   445,   453,   461,   469,   474,   482,   487,   492,
     497,   505,   513,   518,   523,   527,   531,   535,   539,   543,
     547,   551,   558,   563,   568,   573,   578,   583,   588,   593,
     598,   603,   608,   613,   618,   626,   634,   639,   647,   652,
     657,   662,   667,   672,   677,   682,   687,   692,   697,   702,
     707,   712,   717,   722,   727,   735,   736,   739,   747,   755,
     763,   771,   779,   787,   795,   803,   811,   819,   826,   834,
     842,   847,   852,   857,   865,   873,   881,   889,   897,   905,
     910,   915,   920,   925,   933,   938,   943,   948,   953,   958,
     963,   968,   976,   982,   987,   995,  1003,  1011,  1016,  1022,
    1029,  1034,  1040,  1046,  1052,  1057,  1062,  1067,  1072,  1077,
    1082,  1087,  1093,  1099,  1105,  1113,  1118,  1123,  1128,  1133,
    1138,  1143,  1148,  1153,  1158,  1163,  1168,  1173,  1178,  1183,
    1188,  1193,  1198,  1203,  1213,  1224,  1230,  1243,  1248,  1259,
    1264,  1280,  1296,  1306,  1311,  1319,  1328,  1338,  1339,  1340,
    1341,  1342,  1343,  1344,  1345,  1346,  1347,  1348,  1349,  1350,
    1351,  1352,  1353,  1354,  1355,  1356,  1357,  1358,  1359,  1360,
    1361,  1362,  1368,  1369,  1370,  1371,  1372,  1373,  1374,  1375,
    1376,  1377,  1378,  1379,  1380,  1381,  1382,  1383,  1384,  1385,
    1386,  1387,  1388,  1389,  1390,  1391,  1392,  1393,  1394,  1395,
    1396,  1397,  1398,  1399,  1400,  1401
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
  "BX_TOKEN_LOADMEM", "BX_TOKEN_SETPMEM", "BX_TOKEN_SYMBOLNAME",
  "BX_TOKEN_QUERY", "BX_TOKEN_PENDING", "BX_TOKEN_TAKE", "BX_TOKEN_DMA",
  "BX_TOKEN_IRQ", "BX_TOKEN_SMI", "BX_TOKEN_NMI", "BX_TOKEN_TLB",
  "BX_TOKEN_DISASM", "BX_TOKEN_INSTRUMENT", "BX_TOKEN_STRING",
  "BX_TOKEN_STOP", "BX_TOKEN_DOIT", "BX_TOKEN_CRC", "BX_TOKEN_TRACE",
  "BX_TOKEN_TRACEREG", "BX_TOKEN_TRACEMEM", "BX_TOKEN_SWITCH_MODE",
  "BX_TOKEN_SIZE", "BX_TOKEN_PTIME", "BX_TOKEN_TIMEBP_ABSOLUTE",
  "BX_TOKEN_TIMEBP", "BX_TOKEN_MODEBP", "BX_TOKEN_VMEXITBP",
  "BX_TOKEN_PRINT_STACK", "BX_TOKEN_BT", "BX_TOKEN_WATCH",
  "BX_TOKEN_UNWATCH", "BX_TOKEN_READ", "BX_TOKEN_WRITE", "BX_TOKEN_SHOW",
  "BX_TOKEN_LOAD_SYMBOLS", "BX_TOKEN_SYMBOLS", "BX_TOKEN_LIST_SYMBOLS",
  "BX_TOKEN_GLOBAL", "BX_TOKEN_WHERE", "BX_TOKEN_PRINT_STRING",
  "BX_TOKEN_NUMERIC", "BX_TOKEN_PAGE", "BX_TOKEN_HELP", "BX_TOKEN_XML",
  "BX_TOKEN_CALC", "BX_TOKEN_DEVICE", "BX_TOKEN_GENERIC",
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
  "watch_point_command", "symbol_command", "where_command",
  "print_string_command", "continue_command", "stepN_command",
  "step_over_command", "set_command", "breakpoint_command",
  "blist_command", "slist_command", "info_command", "optional_numeric",
  "regs_command", "fpu_regs_command", "mmx_regs_command",
  "xmm_regs_command", "ymm_regs_command", "zmm_regs_command",
  "segment_regs_command", "control_regs_command", "debug_regs_command",
  "delete_command", "bpe_command", "bpd_command", "quit_command",
  "examine_command", "restore_command", "writemem_command",
  "loadmem_command", "setpmem_command", "query_command", "take_command",
  "disassemble_command", "instrument_command", "doit_command",
  "crc_command", "help_command", "calc_command", "if_command",
  "vexpression", "expression", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-180)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-304)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     563,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,    15,  1329,   -38,   -77,   115,   -75,  1353,
     300,   988,   -36,   -35,   -25,  1480,   -68,  -180,  -180,   -44,
     -43,   -42,   -27,   -15,     7,     9,    12,   936,    17,    70,
      74,  1329,    86,   105,  1329,    19,   -53,  -180,  1329,  1329,
      30,    30,    30,    24,  1329,  1329,    46,    47,   -62,   -58,
     111,  1067,    39,   -18,   -60,    50,  1329,  -180,  1329,  1519,
    1329,  -180,  -180,  -180,  -180,  1329,  1329,  -180,  1329,  1329,
    1329,   432,  -180,    51,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  1184,  1329,  -180,  1518,
      81,   -56,  -180,  -180,    52,    53,    54,    68,    76,    77,
      30,    78,    87,    91,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  1405,  -180,  1405,
    1405,  -180,  1315,    20,  -180,   -14,  1329,  -180,   257,    75,
      95,    96,    97,    99,   102,  1329,  1329,  1329,  1329,   104,
     109,   110,   -55,   -69,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  1119,  -180,  1543,   143,  1329,  1329,   908,
     113,   -41,   118,   119,   120,  1568,  1143,   122,   114,  -180,
     150,   123,   124,   125,  1593,   908,  -180,  -180,   127,   151,
     155,  -180,  1618,  1643,  -180,  -180,   161,  -180,   162,  -180,
     170,  1329,   171,  1329,  1329,  -180,  -180,  1668,   172,   173,
     -63,   175,  -180,  1198,   159,   189,  -180,  -180,  1693,  1718,
     206,   207,   208,   209,   210,   217,   221,   232,   233,   234,
     241,   242,   243,   254,   255,   256,   258,   260,   261,   262,
     263,   265,   269,   270,   272,   273,   274,   275,   276,   278,
     279,   280,   281,   282,   283,   288,   290,   291,   292,   293,
     296,   299,   301,   302,   320,   321,  -180,   322,  1743,   326,
     326,   326,  1053,   326,  -180,  -180,  -180,  1329,  1329,  1329,
    1329,  1329,  1329,  1329,  1329,  1329,  1329,  1329,  1329,  1329,
    1329,  1329,  1329,  1768,  -180,   340,   346,  -180,  1329,  1329,
    1329,  1329,  1329,  1329,   347,  1329,  1329,  1329,  -180,  -180,
     169,  1405,  1405,  1405,  1405,  1405,  1405,  1405,  1405,  1405,
    1405,   168,  -180,   404,  -180,   -13,   405,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  1329,  1184,  1329,  1329,  1329,  -180,
    -180,  -180,   350,  -180,   -28,    11,  -180,  -180,  1793,  -180,
     351,   908,  1818,  1329,  1329,   908,  -180,   352,  -180,  -180,
    -180,  -180,  -180,  -180,   694,  -180,   382,  -180,  1843,  -180,
    -180,  -180,  -180,  1868,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,   724,  -180,   777,   855,  -180,  -180,  -180,   354,
    -180,  -180,  -180,  1893,  1274,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,   103,   103,   326,
     326,   326,   326,   637,   637,   637,   637,   637,   637,   103,
     103,   103,  1184,  -180,  -180,  -180,  1918,  1943,  1968,  1993,
    2018,  2043,  -180,  2068,  2093,  2118,  -180,  -180,  -180,   112,
     112,   112,   112,  -180,  -180,  -180,   634,   359,   360,   421,
    -180,   367,   369,   370,   371,   372,  -180,   377,  -180,   383,
    -180,  -180,  -180,  2143,  -180,   984,   186,  2168,  -180,  -180,
    2193,   384,  -180,  -180,  -180,  2218,  -180,  2243,  -180,  2268,
    -180,  -180,  -180,  2293,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,   450,  -180,  -180,  -180,   395,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,   398,  -180,  -180
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
      56,   275,   274,   276,   277,   278,    62,    63,    64,    65,
      66,    67,   279,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    60,    61,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   273,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   272,     0,     0,
       0,   281,   282,   283,   284,     0,     0,    57,     0,     0,
       0,     0,     3,     0,   280,    40,    41,    42,    48,    46,
      47,    39,    36,    37,    38,    43,    44,    45,    49,    50,
      51,     4,     5,     6,     7,     8,    19,    20,     9,    10,
      11,    12,    13,    14,    15,    16,    18,    17,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    52,    53,    54,    55,     0,   105,     0,
       0,     0,   107,   111,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   135,   250,   249,   251,   252,   253,
     254,   248,   247,   256,   257,   258,   259,     0,   122,     0,
       0,   255,     0,   273,   125,     0,     0,   130,     0,     0,
       0,     0,     0,     0,     0,   155,   155,   155,   155,     0,
       0,     0,     0,     0,   169,   158,   159,   160,   161,   162,
     165,   164,   163,     0,   173,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   184,
       0,     0,     0,     0,     0,     0,    58,    59,     0,     0,
       0,    80,     0,     0,    70,    71,     0,    84,     0,    86,
       0,     0,     0,     0,     0,    90,    97,     0,     0,     0,
       0,     0,    77,     0,     0,     0,   136,   103,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   244,     0,     0,   302,
     303,   301,     0,   304,     1,     2,   157,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   246,     0,     0,   108,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   270,   269,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   128,     0,   126,   303,     0,   131,   166,   167,
     168,   146,   138,   139,   155,   156,   155,   155,   155,   145,
     144,   147,     0,   148,     0,     0,   150,   171,     0,   172,
       0,     0,     0,     0,     0,     0,   178,     0,   179,   181,
     182,   183,    79,   187,     0,   190,     0,   185,     0,   193,
     192,   194,   195,     0,    81,    82,    83,    69,    68,    85,
      87,    89,     0,    88,     0,     0,    98,    74,    73,     0,
      75,    72,    99,     0,     0,   137,   104,    78,   198,   199,
     200,   238,   207,   201,   202,   203,   204,   205,   206,   240,
     197,   222,   223,   224,   225,   226,   229,   228,   227,   236,
     214,   230,   231,   232,   233,   237,   210,   211,   212,   213,
     215,   217,   216,   208,   209,   218,   234,   235,   241,   219,
     220,   239,   243,   242,   221,   245,   305,   290,   291,   297,
     298,   299,   300,   286,   287,   292,   293,   296,   295,   288,
     289,   294,   285,   106,   109,   110,     0,     0,     0,     0,
       0,     0,   112,     0,     0,     0,   271,   264,   265,   260,
     261,   266,   267,   262,   263,   268,     0,     0,     0,     0,
     133,     0,     0,     0,     0,     0,   149,     0,   153,     0,
     151,   170,   174,     0,   176,   287,   288,     0,   180,   188,
       0,     0,   186,   196,    91,     0,    92,     0,    93,     0,
      76,   100,   101,     0,   115,   114,   116,   117,   118,   113,
     119,   120,   121,     0,   123,   129,   127,     0,   132,   140,
     141,   142,   143,   154,   152,   175,   177,   189,   191,    94,
      95,    96,   102,     0,   134,   124
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -180,  -180,   446,   -39,   464,    -2,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -179,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,
    -180,  -180,  -180,  -180,  -180,  -180,  -180,  -180,  -166,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    81,    82,   228,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     374,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   172,   375
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     136,   348,   384,   349,   350,   363,   529,   376,   377,   378,
     140,   255,   229,   230,   139,   153,   382,   171,   221,   222,
     175,   178,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,   137,   385,   236,   205,   429,   361,
     238,   209,   336,   537,   215,   220,   226,   227,   224,   225,
     143,   223,   154,   253,   232,   233,   248,   397,   386,   194,
     141,   247,   179,   180,   430,   237,   258,   256,   259,   239,
     308,   337,   383,   181,   216,   309,   310,   254,   311,   312,
     313,   136,   539,   195,   196,   197,   398,   249,   206,   142,
      47,   317,   318,   319,   320,   321,   322,   217,   218,   538,
     198,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     250,   344,   199,   364,   530,   332,   332,    67,   144,   145,
     146,   147,   148,     6,     7,     8,     9,    10,    11,   240,
      71,    72,    73,    74,   200,    75,   201,   333,   540,   202,
      76,   207,   138,   251,   241,   208,   219,   362,   210,    78,
      79,   231,    80,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,   171,   252,   171,   171,   211,
     212,   213,   214,   234,   235,   149,   365,   257,   316,   335,
     338,   339,   340,   242,   150,   517,   518,   519,   520,   521,
     522,   523,   524,   525,   526,   532,   341,   533,   534,   535,
     243,   244,   368,   388,   342,   343,   345,   391,   392,   395,
     319,   320,   321,   322,   390,   346,   404,   351,   352,   347,
     408,    47,   369,   370,   371,   413,   372,   151,   152,   373,
     434,   379,   332,   357,   358,   359,   380,   381,   245,   527,
     396,   422,   406,   424,   425,   399,   400,   401,    67,   405,
     409,   410,   411,   433,   414,   317,   318,   319,   320,   321,
     322,    71,    72,    73,    74,   323,   393,   325,   326,   327,
     328,   394,   330,   331,   351,   352,   366,   407,   415,   332,
      78,    79,   416,    80,   353,   354,   355,   356,   419,   420,
     357,   358,   359,  -303,  -303,  -303,  -303,   421,   423,   427,
     428,   516,   431,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,   332,   435,   487,   488,   489,
     490,   491,   492,   493,   494,   495,   496,   497,   498,   499,
     500,   501,   502,   438,   439,   440,   441,   442,   506,   507,
     508,   509,   510,   511,   443,   513,   514,   515,   444,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   445,
     446,   447,   317,   318,   319,   320,   321,   322,   448,   449,
     450,   173,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   451,   452,   453,   367,   454,   332,   455,   456,   457,
     458,   543,   459,   545,   546,   547,   460,   461,    67,   462,
     463,   464,   465,   466,   550,   467,   468,   469,   470,   471,
     472,    71,    72,    73,    74,   473,    75,   474,   475,   476,
     477,    76,   555,   478,   557,   559,   479,   174,   480,   481,
      78,    79,   314,    80,   563,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,   482,   483,   484,
      13,    14,    15,    16,    17,   332,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,   504,    29,    30,
      31,    32,    33,   505,   512,   528,   531,   536,   542,   548,
     551,   560,    34,    35,    36,    37,   575,   576,    38,    39,
      40,    41,   577,    42,   578,    43,   579,   580,   581,   582,
      44,    45,    46,    47,   583,    48,    49,    50,    51,    52,
     584,   588,    53,    54,    55,    56,    57,    58,    59,    60,
      61,   593,   594,    62,    63,   595,    64,   315,    65,    66,
      67,    68,    69,   307,    70,     0,     0,     0,     0,     0,
       0,     0,     0,    71,    72,    73,    74,     0,    75,     0,
       0,     0,     0,    76,     0,     0,     0,     0,     0,    77,
       0,     0,    78,    79,     0,    80,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     0,     0,
       0,    13,    14,    15,    16,    17,     0,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,     0,    29,
      30,    31,    32,    33,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    34,    35,    36,    37,     0,     0,    38,
      39,    40,    41,     0,    42,     0,    43,     0,     0,     0,
       0,    44,    45,    46,    47,     0,    48,    49,    50,    51,
      52,     0,     0,    53,    54,    55,    56,    57,    58,    59,
      60,    61,     0,   573,    62,    63,     0,    64,     0,    65,
      66,    67,    68,    69,     0,    70,     0,     0,     0,     0,
       0,     0,     0,     0,    71,    72,    73,    74,     0,    75,
       0,     0,     0,     0,    76,     0,     0,     0,     0,     0,
      77,     0,     0,    78,    79,     0,    80,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,   351,
     352,     0,   317,   318,   319,   320,   321,   322,     0,   353,
     354,   355,   356,     0,     0,   357,   358,   359,   329,   330,
     331,   574,     0,     0,     0,    47,   332,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    67,     0,     0,    47,     0,     0,     0,   317,
     318,   319,   320,   321,   322,    71,    72,    73,    74,   323,
     393,   325,   326,   327,   328,   394,   330,   331,     0,     0,
       0,   549,    67,   332,    78,    79,     0,    80,     0,   317,
     318,   319,   320,   321,   322,    71,    72,    73,    74,   323,
     393,   325,   326,   327,   328,   394,   330,   331,    47,     0,
       0,   554,     0,   332,    78,    79,     0,    80,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
       0,     0,     0,     0,     0,    67,     0,     0,     0,     0,
       0,     0,   317,   318,   319,   320,   321,   322,    71,    72,
      73,    74,   323,   393,   325,   326,   327,   328,   394,   330,
     331,     0,     0,     0,   556,     0,   332,    78,    79,     0,
      80,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,    67,     0,     0,     0,     0,     0,     0,
     317,   318,   319,   320,   321,   322,    71,    72,    73,    74,
     323,   393,   325,   326,   327,   328,   394,   330,   331,    47,
       0,     0,   558,     0,   332,    78,    79,     0,    80,     0,
     203,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,    67,    47,     0,     0,
       0,     0,     0,   317,   318,   319,   320,   321,   322,    71,
      72,    73,    74,   323,   393,   325,   326,   327,   328,   394,
     330,   331,     0,     0,    67,     0,     0,   332,    78,    79,
       0,    80,     0,     0,     0,     0,     0,    71,    72,    73,
      74,     0,    75,     0,     0,     0,     0,    76,     0,    47,
       0,     0,     0,   204,     0,     0,    78,    79,     0,    80,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,     0,     0,     0,     0,    67,     0,     0,  -302,
    -302,  -302,  -302,  -302,  -302,     0,     0,     0,     0,    71,
      72,    73,    74,     0,    75,  -302,  -302,  -302,     0,   176,
       0,     0,     0,   332,     0,   177,     0,     0,    78,    79,
       0,    80,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,     0,     0,     0,    47,     0,
       0,     0,     0,     0,     0,     0,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,   317,   318,
     319,   320,   321,   322,     0,    67,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,    71,    72,
      73,    74,   332,    75,     0,   486,     0,     0,    76,     0,
      47,     0,     0,     0,   246,     0,     0,    78,    79,     0,
      80,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,    47,     0,     0,    67,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      71,    72,    73,    74,     0,    75,     0,     0,     0,     0,
      76,    67,     0,     0,     0,     0,   387,     0,     0,    78,
      79,     0,    80,     0,    71,    72,    73,    74,     0,    75,
       0,     0,     0,     0,    76,     0,     0,     0,     0,    47,
     403,     0,     0,    78,    79,     0,    80,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,   317,
     318,   319,   320,   321,   322,     0,    67,     0,     0,   323,
     324,   325,   326,   327,   328,   329,   330,   331,     0,    71,
      72,    73,    74,   332,    75,     0,     0,     0,     0,    76,
       0,     0,     0,     0,     0,   432,     0,     0,    78,    79,
       0,    80,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   155,   156,   157,   158,
     159,     6,     7,     8,     9,    10,    11,   160,     0,     0,
       0,     0,    67,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    71,    72,    73,    74,     0,
      75,     0,     0,     0,     0,    76,     0,     0,     0,     0,
      47,   562,     0,     0,    78,    79,     0,    80,   155,   156,
     157,   158,   159,     6,     7,     8,     9,    10,    11,   160,
     351,   352,     0,     0,   161,     0,     0,    67,     0,     0,
     353,   354,   355,   356,     0,     0,   357,   358,   359,     0,
      71,    72,    73,    74,   360,    75,     0,     0,     0,     0,
      76,   162,     0,     0,     0,     0,     0,     0,     0,    78,
      79,     0,    80,     0,   163,   164,   165,   166,     0,   167,
       0,     0,     0,     0,     0,     0,   161,     0,     0,     0,
     168,     0,     0,   169,   170,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   182,     0,     0,     0,     0,
       0,     0,     0,   162,     0,     0,     0,   183,     0,     0,
       0,     0,     0,     0,     0,   184,   163,   164,   165,   166,
       0,   167,   185,   186,   187,   188,   189,   190,     0,   191,
       0,     0,     0,     0,     0,   169,   170,   260,     0,   261,
     262,   263,     0,   264,   265,   266,   267,   268,   269,   270,
     271,   272,    27,    28,     0,   273,   274,   275,   276,   277,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   278,
     279,   280,   281,   192,     0,   282,   283,   284,   285,     0,
       0,     0,     0,   193,     0,     0,     0,     0,   286,   287,
       0,     0,     0,   288,   289,   290,   291,     0,     0,   292,
     293,   294,   295,   296,   297,     0,   298,   299,     0,     0,
     300,   301,     0,   302,     0,     0,     0,     0,   303,   304,
       0,   305,     0,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   334,   306,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     389,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   402,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     412,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   417,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     418,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   426,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     436,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   437,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     485,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   503,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     541,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   544,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     552,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   553,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     561,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   564,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     565,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   566,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     567,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   568,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     569,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   570,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     571,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   572,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     585,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   586,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     587,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   589,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     590,     0,   332,   317,   318,   319,   320,   321,   322,     0,
       0,     0,     0,   323,   324,   325,   326,   327,   328,   329,
     330,   331,     0,     0,     0,   591,     0,   332,   317,   318,
     319,   320,   321,   322,     0,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,     0,     0,     0,
     592,     0,   332
};

static const yytype_int16 yycheck[] =
{
       0,   167,    71,   169,   170,    19,    19,   186,   187,   188,
      48,    71,    51,    52,    14,    17,    71,    19,    71,    72,
      20,    21,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    19,   104,    98,    37,   101,    19,
      98,    41,    98,    71,    44,    45,    16,    17,    48,    49,
     127,   104,   127,    71,    54,    55,    17,    98,   127,   127,
      98,    61,    98,    98,   127,   127,    66,   127,    68,   127,
      70,   127,   127,    98,    55,    75,    76,    95,    78,    79,
      80,    81,    71,   127,   127,   127,   127,    48,    71,   127,
      71,   105,   106,   107,   108,   109,   110,    78,    79,   127,
     127,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      71,   150,   127,   127,   127,   129,   129,    98,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    18,
     111,   112,   113,   114,   127,   116,   127,   137,   127,   127,
     121,    71,   127,   104,    33,    71,   127,   127,    62,   130,
     131,   127,   133,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,   167,   127,   169,   170,    64,
      65,    66,    67,   127,   127,    60,   176,   127,   127,    98,
     128,   128,   128,    72,    69,   351,   352,   353,   354,   355,
     356,   357,   358,   359,   360,   374,   128,   376,   377,   378,
      89,    90,   127,   203,   128,   128,   128,   207,   208,   209,
     107,   108,   109,   110,    71,   128,   216,   105,   106,   128,
     220,    71,   127,   127,   127,   225,   127,   112,   113,   127,
      71,   127,   129,   121,   122,   123,   127,   127,   127,    71,
     127,   241,   128,   243,   244,   127,   127,   127,    98,   127,
     127,   127,   127,   253,   127,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   105,   106,    19,   127,   127,   129,
     130,   131,   127,   133,   115,   116,   117,   118,   127,   127,
     121,   122,   123,   107,   108,   109,   110,   127,   127,   127,
     127,   132,   127,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,   129,   127,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   127,   127,   127,   127,   127,   338,   339,
     340,   341,   342,   343,   127,   345,   346,   347,   127,   351,
     352,   353,   354,   355,   356,   357,   358,   359,   360,   127,
     127,   127,   105,   106,   107,   108,   109,   110,   127,   127,
     127,    71,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   127,   127,   127,   127,   127,   129,   127,   127,   127,
     127,   391,   127,   393,   394,   395,   127,   127,    98,   127,
     127,   127,   127,   127,   404,   127,   127,   127,   127,   127,
     127,   111,   112,   113,   114,   127,   116,   127,   127,   127,
     127,   121,   422,   127,   424,   425,   127,   127,   127,   127,
     130,   131,     0,   133,   434,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,   127,   127,   127,
      18,    19,    20,    21,    22,   129,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,   127,    36,    37,
      38,    39,    40,   127,   127,    71,    71,   127,   127,   127,
      98,   127,    50,    51,    52,    53,   127,   127,    56,    57,
      58,    59,    71,    61,   127,    63,   127,   127,   127,   127,
      68,    69,    70,    71,   127,    73,    74,    75,    76,    77,
     127,   127,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    71,   127,    91,    92,   127,    94,    81,    96,    97,
      98,    99,   100,    69,   102,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   111,   112,   113,   114,    -1,   116,    -1,
      -1,    -1,    -1,   121,    -1,    -1,    -1,    -1,    -1,   127,
      -1,    -1,   130,   131,    -1,   133,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    18,    19,    20,    21,    22,    -1,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    36,
      37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    50,    51,    52,    53,    -1,    -1,    56,
      57,    58,    59,    -1,    61,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    71,    -1,    73,    74,    75,    76,
      77,    -1,    -1,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    -1,    19,    91,    92,    -1,    94,    -1,    96,
      97,    98,    99,   100,    -1,   102,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   111,   112,   113,   114,    -1,   116,
      -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,    -1,    -1,
     127,    -1,    -1,   130,   131,    -1,   133,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,   105,
     106,    -1,   105,   106,   107,   108,   109,   110,    -1,   115,
     116,   117,   118,    -1,    -1,   121,   122,   123,   121,   122,
     123,   127,    -1,    -1,    -1,    71,   129,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    98,    -1,    -1,    71,    -1,    -1,    -1,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,    -1,    -1,
      -1,   127,    98,   129,   130,   131,    -1,   133,    -1,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,    71,    -1,
      -1,   127,    -1,   129,   130,   131,    -1,   133,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,
      -1,    -1,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,    -1,    -1,    -1,   127,    -1,   129,   130,   131,    -1,
     133,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    71,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,    71,
      -1,    -1,   127,    -1,   129,   130,   131,    -1,   133,    -1,
      54,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    98,    71,    -1,    -1,
      -1,    -1,    -1,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    98,    -1,    -1,   129,   130,   131,
      -1,   133,    -1,    -1,    -1,    -1,    -1,   111,   112,   113,
     114,    -1,   116,    -1,    -1,    -1,    -1,   121,    -1,    71,
      -1,    -1,    -1,   127,    -1,    -1,   130,   131,    -1,   133,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    98,    -1,    -1,   105,
     106,   107,   108,   109,   110,    -1,    -1,    -1,    -1,   111,
     112,   113,   114,    -1,   116,   121,   122,   123,    -1,   121,
      -1,    -1,    -1,   129,    -1,   127,    -1,    -1,   130,   131,
      -1,   133,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    71,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,   105,   106,
     107,   108,   109,   110,    -1,    98,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,   111,   112,
     113,   114,   129,   116,    -1,   132,    -1,    -1,   121,    -1,
      71,    -1,    -1,    -1,   127,    -1,    -1,   130,   131,    -1,
     133,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    71,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     111,   112,   113,   114,    -1,   116,    -1,    -1,    -1,    -1,
     121,    98,    -1,    -1,    -1,    -1,   127,    -1,    -1,   130,
     131,    -1,   133,    -1,   111,   112,   113,   114,    -1,   116,
      -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,    -1,    71,
     127,    -1,    -1,   130,   131,    -1,   133,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,   105,
     106,   107,   108,   109,   110,    -1,    98,    -1,    -1,   115,
     116,   117,   118,   119,   120,   121,   122,   123,    -1,   111,
     112,   113,   114,   129,   116,    -1,    -1,    -1,    -1,   121,
      -1,    -1,    -1,    -1,    -1,   127,    -1,    -1,   130,   131,
      -1,   133,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    71,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   111,   112,   113,   114,    -1,
     116,    -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,    -1,
      71,   127,    -1,    -1,   130,   131,    -1,   133,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     105,   106,    -1,    -1,    71,    -1,    -1,    98,    -1,    -1,
     115,   116,   117,   118,    -1,    -1,   121,   122,   123,    -1,
     111,   112,   113,   114,   129,   116,    -1,    -1,    -1,    -1,
     121,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,
     131,    -1,   133,    -1,   111,   112,   113,   114,    -1,   116,
      -1,    -1,    -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,
     127,    -1,    -1,   130,   131,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    98,    -1,    -1,    -1,    27,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    35,   111,   112,   113,   114,
      -1,   116,    42,    43,    44,    45,    46,    47,    -1,    49,
      -1,    -1,    -1,    -1,    -1,   130,   131,    18,    -1,    20,
      21,    22,    -1,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    -1,    36,    37,    38,    39,    40,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,
      51,    52,    53,    93,    -1,    56,    57,    58,    59,    -1,
      -1,    -1,    -1,   103,    -1,    -1,    -1,    -1,    69,    70,
      -1,    -1,    -1,    74,    75,    76,    77,    -1,    -1,    80,
      81,    82,    83,    84,    85,    -1,    87,    88,    -1,    -1,
      91,    92,    -1,    94,    -1,    -1,    -1,    -1,    99,   100,
      -1,   102,    -1,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,   127,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,   129,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     127,    -1,   129
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    18,    19,    20,    21,    22,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    36,
      37,    38,    39,    40,    50,    51,    52,    53,    56,    57,
      58,    59,    61,    63,    68,    69,    70,    71,    73,    74,
      75,    76,    77,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    91,    92,    94,    96,    97,    98,    99,   100,
     102,   111,   112,   113,   114,   116,   121,   127,   130,   131,
     133,   135,   136,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   193,    19,   127,   193,
      48,    98,   127,   127,     3,     4,     5,     6,     7,    60,
      69,   112,   113,   139,   127,     3,     4,     5,     6,     7,
      14,    71,    98,   111,   112,   113,   114,   116,   127,   130,
     131,   139,   192,    71,   127,   193,   121,   127,   193,    98,
      98,    98,    15,    27,    35,    42,    43,    44,    45,    46,
      47,    49,    93,   103,   127,   127,   127,   127,   127,   127,
     127,   127,   127,    54,   127,   193,    71,    71,    71,   193,
      62,    64,    65,    66,    67,   193,    55,    78,    79,   127,
     193,    71,    72,   104,   193,   193,    16,    17,   137,   137,
     137,   127,   193,   193,   127,   127,    98,   127,    98,   127,
      18,    33,    72,    89,    90,   127,   127,   193,    17,    48,
      71,   104,   127,    71,    95,    71,   127,   127,   193,   193,
      18,    20,    21,    22,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    36,    37,    38,    39,    40,    50,    51,
      52,    53,    56,    57,    58,    59,    69,    70,    74,    75,
      76,    77,    80,    81,    82,    83,    84,    85,    87,    88,
      91,    92,    94,    99,   100,   102,   127,   138,   193,   193,
     193,   193,   193,   193,     0,   136,   127,   105,   106,   107,
     108,   109,   110,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   129,   193,   127,    98,    98,   127,   128,   128,
     128,   128,   128,   128,   137,   128,   128,   128,   192,   192,
     192,   105,   106,   115,   116,   117,   118,   121,   122,   123,
     129,    19,   127,    19,   127,   193,    19,   127,   127,   127,
     127,   127,   127,   127,   164,   193,   164,   164,   164,   127,
     127,   127,    71,   127,    71,   104,   127,   127,   193,   127,
      71,   193,   193,   116,   121,   193,   127,    98,   127,   127,
     127,   127,   127,   127,   193,   127,   128,   127,   193,   127,
     127,   127,   127,   193,   127,   127,   127,   127,   127,   127,
     127,   127,   193,   127,   193,   193,   127,   127,   127,   101,
     127,   127,   127,   193,    71,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   132,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   127,   127,   127,   193,   193,   193,   193,
     193,   193,   127,   193,   193,   193,   132,   192,   192,   192,
     192,   192,   192,   192,   192,   192,   192,    71,    71,    19,
     127,    71,   164,   164,   164,   164,   127,    71,   127,    71,
     127,   127,   127,   193,   127,   193,   193,   193,   127,   127,
     193,    98,   127,   127,   127,   193,   127,   193,   127,   193,
     127,   127,   127,   193,   127,   127,   127,   127,   127,   127,
     127,   127,   127,    19,   127,   127,   127,    71,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,    71,   127,   127
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   134,   135,   135,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   137,   137,
     138,   138,   139,   139,   139,   139,   139,   139,   140,   140,
     141,   142,   143,   143,   143,   143,   143,   143,   144,   145,
     146,   147,   148,   149,   150,   150,   151,   151,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   153,
     153,   153,   153,   154,   155,   156,   156,   157,   157,   157,
     157,   158,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   161,   162,   162,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   164,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   178,   178,   178,   179,   180,   181,   182,   183,   184,
     184,   184,   184,   184,   185,   185,   185,   185,   185,   185,
     185,   185,   186,   186,   186,   187,   188,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   190,   191,   192,   192,   192,
     192,   192,   192,   192,   192,   192,   192,   192,   192,   192,
     192,   192,   192,   192,   192,   192,   192,   192,   192,   192,
     192,   192,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       2,     2,     3,     3,     3,     3,     4,     2,     3,     3,
       2,     3,     3,     3,     2,     3,     2,     3,     3,     3,
       2,     4,     4,     4,     5,     5,     5,     2,     3,     3,
       4,     4,     5,     2,     3,     2,     4,     2,     3,     4,
       4,     2,     4,     5,     5,     5,     5,     5,     5,     5,
       5,     5,     2,     5,     7,     2,     3,     5,     3,     5,
       2,     3,     5,     4,     6,     2,     2,     3,     3,     3,
       5,     5,     5,     5,     3,     3,     3,     3,     3,     4,
       3,     4,     5,     4,     5,     0,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     3,     3,     3,     2,
       4,     3,     3,     2,     4,     5,     4,     5,     3,     3,
       4,     3,     3,     3,     2,     3,     4,     3,     4,     5,
       3,     5,     3,     3,     3,     3,     4,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
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
  case 55: /* command: expression  */
#line 206 "parser.y"
                 { eval_value = (yyvsp[0].uval); }
#line 2363 "y.tab.c"
    break;

  case 57: /* command: '\n'  */
#line 209 "parser.y"
      {
      }
#line 2370 "y.tab.c"
    break;

  case 59: /* BX_TOKEN_TOGGLE_ON_OFF: BX_TOKEN_OFF  */
#line 216 "parser.y"
    { (yyval.bval)=(yyvsp[0].bval); }
#line 2376 "y.tab.c"
    break;

  case 61: /* BX_TOKEN_REGISTERS: BX_TOKEN_REGS  */
#line 222 "parser.y"
    { (yyval.sval)=(yyvsp[0].sval); }
#line 2382 "y.tab.c"
    break;

  case 67: /* BX_TOKEN_SEGREG: BX_TOKEN_GS  */
#line 232 "parser.y"
    { (yyval.uval)=(yyvsp[0].uval); }
#line 2388 "y.tab.c"
    break;

  case 68: /* timebp_command: BX_TOKEN_TIMEBP expression '\n'  */
#line 237 "parser.y"
      {
          bx_dbg_timebp_command(0, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2397 "y.tab.c"
    break;

  case 69: /* timebp_command: BX_TOKEN_TIMEBP_ABSOLUTE expression '\n'  */
#line 242 "parser.y"
      {
          bx_dbg_timebp_command(1, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2406 "y.tab.c"
    break;

  case 70: /* modebp_command: BX_TOKEN_MODEBP '\n'  */
#line 250 "parser.y"
      {
          bx_dbg_modebp_command();
          free((yyvsp[-1].sval));
      }
#line 2415 "y.tab.c"
    break;

  case 71: /* vmexitbp_command: BX_TOKEN_VMEXITBP '\n'  */
#line 258 "parser.y"
      {
          bx_dbg_vmexitbp_command();
          free((yyvsp[-1].sval));
      }
#line 2424 "y.tab.c"
    break;

  case 72: /* show_command: BX_TOKEN_SHOW BX_TOKEN_GENERIC '\n'  */
#line 266 "parser.y"
      {
          bx_dbg_show_command((yyvsp[-1].sval));
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2433 "y.tab.c"
    break;

  case 73: /* show_command: BX_TOKEN_SHOW BX_TOKEN_ALL '\n'  */
#line 271 "parser.y"
      {
          bx_dbg_show_command("all");
          free((yyvsp[-2].sval));
      }
#line 2442 "y.tab.c"
    break;

  case 74: /* show_command: BX_TOKEN_SHOW BX_TOKEN_OFF '\n'  */
#line 276 "parser.y"
      {
          bx_dbg_show_command("off");
          free((yyvsp[-2].sval));
      }
#line 2451 "y.tab.c"
    break;

  case 75: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING '\n'  */
#line 281 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-1].sval), 0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2460 "y.tab.c"
    break;

  case 76: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING BX_TOKEN_XML '\n'  */
#line 286 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-2].sval), 1);
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2469 "y.tab.c"
    break;

  case 77: /* show_command: BX_TOKEN_SHOW '\n'  */
#line 291 "parser.y"
      {
          bx_dbg_show_command(0);
          free((yyvsp[-1].sval));
      }
#line 2478 "y.tab.c"
    break;

  case 78: /* page_command: BX_TOKEN_PAGE expression '\n'  */
#line 299 "parser.y"
      {
          bx_dbg_xlate_address((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2487 "y.tab.c"
    break;

  case 79: /* tlb_command: BX_TOKEN_TLB expression '\n'  */
#line 307 "parser.y"
      {
          bx_dbg_tlb_lookup((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2496 "y.tab.c"
    break;

  case 80: /* ptime_command: BX_TOKEN_PTIME '\n'  */
#line 315 "parser.y"
      {
          bx_dbg_ptime_command();
          free((yyvsp[-1].sval));
      }
#line 2505 "y.tab.c"
    break;

  case 81: /* trace_command: BX_TOKEN_TRACE BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 323 "parser.y"
      {
          bx_dbg_trace_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2514 "y.tab.c"
    break;

  case 82: /* trace_reg_command: BX_TOKEN_TRACEREG BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 331 "parser.y"
      {
          bx_dbg_trace_reg_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2523 "y.tab.c"
    break;

  case 83: /* trace_mem_command: BX_TOKEN_TRACEMEM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 339 "parser.y"
      {
          bx_dbg_trace_mem_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2532 "y.tab.c"
    break;

  case 84: /* print_stack_command: BX_TOKEN_PRINT_STACK '\n'  */
#line 347 "parser.y"
      {
          bx_dbg_print_stack_command(16);
          free((yyvsp[-1].sval));
      }
#line 2541 "y.tab.c"
    break;

  case 85: /* print_stack_command: BX_TOKEN_PRINT_STACK BX_TOKEN_NUMERIC '\n'  */
#line 352 "parser.y"
      {
          bx_dbg_print_stack_command((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2550 "y.tab.c"
    break;

  case 86: /* backtrace_command: BX_TOKEN_BT '\n'  */
#line 360 "parser.y"
      {
        bx_dbg_bt_command(16);
        free((yyvsp[-1].sval));
      }
#line 2559 "y.tab.c"
    break;

  case 87: /* backtrace_command: BX_TOKEN_BT BX_TOKEN_NUMERIC '\n'  */
#line 365 "parser.y"
      {
        bx_dbg_bt_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2568 "y.tab.c"
    break;

  case 88: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_STOP '\n'  */
#line 373 "parser.y"
      {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2577 "y.tab.c"
    break;

  case 89: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_CONTINUE '\n'  */
#line 378 "parser.y"
      {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2586 "y.tab.c"
    break;

  case 90: /* watch_point_command: BX_TOKEN_WATCH '\n'  */
#line 383 "parser.y"
      {
          bx_dbg_print_watchpoints();
          free((yyvsp[-1].sval));
      }
#line 2595 "y.tab.c"
    break;

  case 91: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression '\n'  */
#line 388 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2604 "y.tab.c"
    break;

  case 92: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression '\n'  */
#line 393 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2613 "y.tab.c"
    break;

  case 93: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression '\n'  */
#line 398 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-1].uval), 1); /* BX_WRITE */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2622 "y.tab.c"
    break;

  case 94: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression expression '\n'  */
#line 403 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2631 "y.tab.c"
    break;

  case 95: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression expression '\n'  */
#line 408 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2640 "y.tab.c"
    break;

  case 96: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression expression '\n'  */
#line 413 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_WRITE */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2649 "y.tab.c"
    break;

  case 97: /* watch_point_command: BX_TOKEN_UNWATCH '\n'  */
#line 418 "parser.y"
      {
          bx_dbg_unwatch_all();
          free((yyvsp[-1].sval));
      }
#line 2658 "y.tab.c"
    break;

  case 98: /* watch_point_command: BX_TOKEN_UNWATCH expression '\n'  */
#line 423 "parser.y"
      {
          bx_dbg_unwatch((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2667 "y.tab.c"
    break;

  case 99: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 431 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2676 "y.tab.c"
    break;

  case 100: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING expression '\n'  */
#line 436 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 0, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2685 "y.tab.c"
    break;

  case 101: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING '\n'  */
#line 441 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 1, 0);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2694 "y.tab.c"
    break;

  case 102: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING expression '\n'  */
#line 446 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 1, (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2703 "y.tab.c"
    break;

  case 103: /* where_command: BX_TOKEN_WHERE '\n'  */
#line 454 "parser.y"
      {
        bx_dbg_where_command();
        free((yyvsp[-1].sval));
      }
#line 2712 "y.tab.c"
    break;

  case 104: /* print_string_command: BX_TOKEN_PRINT_STRING expression '\n'  */
#line 462 "parser.y"
      {
        bx_dbg_print_string_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2721 "y.tab.c"
    break;

  case 105: /* continue_command: BX_TOKEN_CONTINUE '\n'  */
#line 470 "parser.y"
      {
        bx_dbg_continue_command(1);
        free((yyvsp[-1].sval));
      }
#line 2730 "y.tab.c"
    break;

  case 106: /* continue_command: BX_TOKEN_CONTINUE BX_TOKEN_IF expression '\n'  */
#line 475 "parser.y"
      {
        bx_dbg_continue_command((yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2739 "y.tab.c"
    break;

  case 107: /* stepN_command: BX_TOKEN_STEPN '\n'  */
#line 483 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[-1].sval));
      }
#line 2748 "y.tab.c"
    break;

  case 108: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC '\n'  */
#line 488 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2757 "y.tab.c"
    break;

  case 109: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_ALL BX_TOKEN_NUMERIC '\n'  */
#line 493 "parser.y"
      {
        bx_dbg_stepN_command(-1, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2766 "y.tab.c"
    break;

  case 110: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'  */
#line 498 "parser.y"
      {
        bx_dbg_stepN_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2775 "y.tab.c"
    break;

  case 111: /* step_over_command: BX_TOKEN_STEP_OVER '\n'  */
#line 506 "parser.y"
      {
        bx_dbg_step_over_command();
        free((yyvsp[-1].sval));
      }
#line 2784 "y.tab.c"
    break;

  case 112: /* set_command: BX_TOKEN_SET BX_TOKEN_DISASM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 514 "parser.y"
      {
        bx_dbg_set_auto_disassemble((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2793 "y.tab.c"
    break;

  case 113: /* set_command: BX_TOKEN_SET BX_TOKEN_SYMBOLNAME '=' expression '\n'  */
#line 519 "parser.y"
      {
        bx_dbg_set_symbol_command((yyvsp[-3].sval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2802 "y.tab.c"
    break;

  case 114: /* set_command: BX_TOKEN_SET BX_TOKEN_8BL_REG '=' expression '\n'  */
#line 524 "parser.y"
      { 
        bx_dbg_set_reg8l_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2810 "y.tab.c"
    break;

  case 115: /* set_command: BX_TOKEN_SET BX_TOKEN_8BH_REG '=' expression '\n'  */
#line 528 "parser.y"
      { 
        bx_dbg_set_reg8h_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2818 "y.tab.c"
    break;

  case 116: /* set_command: BX_TOKEN_SET BX_TOKEN_16B_REG '=' expression '\n'  */
#line 532 "parser.y"
      { 
        bx_dbg_set_reg16_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2826 "y.tab.c"
    break;

  case 117: /* set_command: BX_TOKEN_SET BX_TOKEN_32B_REG '=' expression '\n'  */
#line 536 "parser.y"
      { 
        bx_dbg_set_reg32_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2834 "y.tab.c"
    break;

  case 118: /* set_command: BX_TOKEN_SET BX_TOKEN_64B_REG '=' expression '\n'  */
#line 540 "parser.y"
      { 
        bx_dbg_set_reg64_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2842 "y.tab.c"
    break;

  case 119: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_EIP '=' expression '\n'  */
#line 544 "parser.y"
      { 
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2850 "y.tab.c"
    break;

  case 120: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_RIP '=' expression '\n'  */
#line 548 "parser.y"
      { 
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2858 "y.tab.c"
    break;

  case 121: /* set_command: BX_TOKEN_SET BX_TOKEN_SEGREG '=' expression '\n'  */
#line 552 "parser.y"
      { 
        bx_dbg_load_segreg((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2866 "y.tab.c"
    break;

  case 122: /* breakpoint_command: BX_TOKEN_VBREAKPOINT '\n'  */
#line 559 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2875 "y.tab.c"
    break;

  case 123: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression '\n'  */
#line 564 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].uval), NULL);
        free((yyvsp[-4].sval));
      }
#line 2884 "y.tab.c"
    break;

  case 124: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 569 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-5].uval), (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-6].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2893 "y.tab.c"
    break;

  case 125: /* breakpoint_command: BX_TOKEN_LBREAKPOINT '\n'  */
#line 574 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2902 "y.tab.c"
    break;

  case 126: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression '\n'  */
#line 579 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 2911 "y.tab.c"
    break;

  case 127: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 584 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2920 "y.tab.c"
    break;

  case 128: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING '\n'  */
#line 589 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-1].sval), NULL);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2929 "y.tab.c"
    break;

  case 129: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 594 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-3].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2938 "y.tab.c"
    break;

  case 130: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '\n'  */
#line 599 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2947 "y.tab.c"
    break;

  case 131: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression '\n'  */
#line 604 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 2956 "y.tab.c"
    break;

  case 132: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 609 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2965 "y.tab.c"
    break;

  case 133: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression '\n'  */
#line 614 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-3].sval));
      }
#line 2974 "y.tab.c"
    break;

  case 134: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 619 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-5].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2983 "y.tab.c"
    break;

  case 135: /* blist_command: BX_TOKEN_LIST_BREAK '\n'  */
#line 627 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-1].sval));
      }
#line 2992 "y.tab.c"
    break;

  case 136: /* slist_command: BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 635 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-1].sval));
      }
#line 3001 "y.tab.c"
    break;

  case 137: /* slist_command: BX_TOKEN_LIST_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 640 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 3010 "y.tab.c"
    break;

  case 138: /* info_command: BX_TOKEN_INFO BX_TOKEN_PBREAKPOINT '\n'  */
#line 648 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3019 "y.tab.c"
    break;

  case 139: /* info_command: BX_TOKEN_INFO BX_TOKEN_CPU '\n'  */
#line 653 "parser.y"
      {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3028 "y.tab.c"
    break;

  case 140: /* info_command: BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric optional_numeric '\n'  */
#line 658 "parser.y"
      {
        bx_dbg_info_idt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3037 "y.tab.c"
    break;

  case 141: /* info_command: BX_TOKEN_INFO BX_TOKEN_IVT optional_numeric optional_numeric '\n'  */
#line 663 "parser.y"
      {
        bx_dbg_info_ivt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3046 "y.tab.c"
    break;

  case 142: /* info_command: BX_TOKEN_INFO BX_TOKEN_GDT optional_numeric optional_numeric '\n'  */
#line 668 "parser.y"
      {
        bx_dbg_info_gdt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3055 "y.tab.c"
    break;

  case 143: /* info_command: BX_TOKEN_INFO BX_TOKEN_LDT optional_numeric optional_numeric '\n'  */
#line 673 "parser.y"
      {
        bx_dbg_info_ldt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3064 "y.tab.c"
    break;

  case 144: /* info_command: BX_TOKEN_INFO BX_TOKEN_TAB '\n'  */
#line 678 "parser.y"
      {
        bx_dbg_dump_table();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3073 "y.tab.c"
    break;

  case 145: /* info_command: BX_TOKEN_INFO BX_TOKEN_TSS '\n'  */
#line 683 "parser.y"
      {
        bx_dbg_info_tss_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3082 "y.tab.c"
    break;

  case 146: /* info_command: BX_TOKEN_INFO BX_TOKEN_FLAGS '\n'  */
#line 688 "parser.y"
      {
        bx_dbg_info_flags();
        free((yyvsp[-2].sval));
      }
#line 3091 "y.tab.c"
    break;

  case 147: /* info_command: BX_TOKEN_INFO BX_TOKEN_LINUX '\n'  */
#line 693 "parser.y"
      {
        bx_dbg_info_linux_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3100 "y.tab.c"
    break;

  case 148: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS '\n'  */
#line 698 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3109 "y.tab.c"
    break;

  case 149: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 703 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3118 "y.tab.c"
    break;

  case 150: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE '\n'  */
#line 708 "parser.y"
      {
        bx_dbg_info_device("", "");
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3127 "y.tab.c"
    break;

  case 151: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC '\n'  */
#line 713 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3136 "y.tab.c"
    break;

  case 152: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC BX_TOKEN_STRING '\n'  */
#line 718 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3145 "y.tab.c"
    break;

  case 153: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING '\n'  */
#line 723 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3154 "y.tab.c"
    break;

  case 154: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 728 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3163 "y.tab.c"
    break;

  case 155: /* optional_numeric: %empty  */
#line 735 "parser.y"
               { (yyval.uval) = EMPTY_ARG; }
#line 3169 "y.tab.c"
    break;

  case 157: /* regs_command: BX_TOKEN_REGISTERS '\n'  */
#line 740 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3178 "y.tab.c"
    break;

  case 158: /* fpu_regs_command: BX_TOKEN_FPU '\n'  */
#line 748 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[-1].sval));
      }
#line 3187 "y.tab.c"
    break;

  case 159: /* mmx_regs_command: BX_TOKEN_MMX '\n'  */
#line 756 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3196 "y.tab.c"
    break;

  case 160: /* xmm_regs_command: BX_TOKEN_XMM '\n'  */
#line 764 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3205 "y.tab.c"
    break;

  case 161: /* ymm_regs_command: BX_TOKEN_YMM '\n'  */
#line 772 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3214 "y.tab.c"
    break;

  case 162: /* zmm_regs_command: BX_TOKEN_ZMM '\n'  */
#line 780 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3223 "y.tab.c"
    break;

  case 163: /* segment_regs_command: BX_TOKEN_SEGMENT_REGS '\n'  */
#line 788 "parser.y"
      {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3232 "y.tab.c"
    break;

  case 164: /* control_regs_command: BX_TOKEN_CONTROL_REGS '\n'  */
#line 796 "parser.y"
      {
        bx_dbg_info_control_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3241 "y.tab.c"
    break;

  case 165: /* debug_regs_command: BX_TOKEN_DEBUG_REGS '\n'  */
#line 804 "parser.y"
      {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3250 "y.tab.c"
    break;

  case 166: /* delete_command: BX_TOKEN_DEL_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 812 "parser.y"
      {
        bx_dbg_del_breakpoint_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3259 "y.tab.c"
    break;

  case 167: /* bpe_command: BX_TOKEN_ENABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 820 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3268 "y.tab.c"
    break;

  case 168: /* bpd_command: BX_TOKEN_DISABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 827 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 0);
        free((yyvsp[-2].sval));
      }
#line 3277 "y.tab.c"
    break;

  case 169: /* quit_command: BX_TOKEN_QUIT '\n'  */
#line 835 "parser.y"
      {
        bx_dbg_quit_command();
        free((yyvsp[-1].sval));
      }
#line 3286 "y.tab.c"
    break;

  case 170: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT expression '\n'  */
#line 843 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-3].sval), (yyvsp[-2].sval),1, (yyvsp[-1].uval), 1);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3295 "y.tab.c"
    break;

  case 171: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT '\n'  */
#line 848 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), (yyvsp[-1].sval),1, 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3304 "y.tab.c"
    break;

  case 172: /* examine_command: BX_TOKEN_EXAMINE expression '\n'  */
#line 853 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), NULL,0, (yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3313 "y.tab.c"
    break;

  case 173: /* examine_command: BX_TOKEN_EXAMINE '\n'  */
#line 858 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-1].sval), NULL,0, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 3322 "y.tab.c"
    break;

  case 174: /* restore_command: BX_TOKEN_RESTORE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 866 "parser.y"
      {
        bx_dbg_restore_command((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3331 "y.tab.c"
    break;

  case 175: /* writemem_command: BX_TOKEN_WRITEMEM BX_TOKEN_STRING expression expression '\n'  */
#line 874 "parser.y"
      {
        bx_dbg_writemem_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3340 "y.tab.c"
    break;

  case 176: /* loadmem_command: BX_TOKEN_LOADMEM BX_TOKEN_STRING expression '\n'  */
#line 882 "parser.y"
      {
        bx_dbg_loadmem_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3349 "y.tab.c"
    break;

  case 177: /* setpmem_command: BX_TOKEN_SETPMEM expression expression expression '\n'  */
#line 890 "parser.y"
      {
        bx_dbg_setpmem_command((yyvsp[-3].uval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3358 "y.tab.c"
    break;

  case 178: /* query_command: BX_TOKEN_QUERY BX_TOKEN_PENDING '\n'  */
#line 898 "parser.y"
      {
        bx_dbg_query_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3367 "y.tab.c"
    break;

  case 179: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA '\n'  */
#line 906 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3376 "y.tab.c"
    break;

  case 180: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA BX_TOKEN_NUMERIC '\n'  */
#line 911 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3385 "y.tab.c"
    break;

  case 181: /* take_command: BX_TOKEN_TAKE BX_TOKEN_IRQ '\n'  */
#line 916 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3394 "y.tab.c"
    break;

  case 182: /* take_command: BX_TOKEN_TAKE BX_TOKEN_SMI '\n'  */
#line 921 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3403 "y.tab.c"
    break;

  case 183: /* take_command: BX_TOKEN_TAKE BX_TOKEN_NMI '\n'  */
#line 926 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3412 "y.tab.c"
    break;

  case 184: /* disassemble_command: BX_TOKEN_DISASM '\n'  */
#line 934 "parser.y"
      {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[-1].sval));
      }
#line 3421 "y.tab.c"
    break;

  case 185: /* disassemble_command: BX_TOKEN_DISASM expression '\n'  */
#line 939 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3430 "y.tab.c"
    break;

  case 186: /* disassemble_command: BX_TOKEN_DISASM expression expression '\n'  */
#line 944 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3439 "y.tab.c"
    break;

  case 187: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT '\n'  */
#line 949 "parser.y"
      {
        bx_dbg_disassemble_current((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3448 "y.tab.c"
    break;

  case 188: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression '\n'  */
#line 954 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-2].sval), (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3457 "y.tab.c"
    break;

  case 189: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression expression '\n'  */
#line 959 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3466 "y.tab.c"
    break;

  case 190: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SWITCH_MODE '\n'  */
#line 964 "parser.y"
      {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3475 "y.tab.c"
    break;

  case 191: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SIZE '=' BX_TOKEN_NUMERIC '\n'  */
#line 969 "parser.y"
      {
        bx_dbg_set_disassemble_size((yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3484 "y.tab.c"
    break;

  case 192: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STOP '\n'  */
#line 977 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3493 "y.tab.c"
    break;

  case 193: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STRING '\n'  */
#line 983 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3502 "y.tab.c"
    break;

  case 194: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_GENERIC '\n'  */
#line 988 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3511 "y.tab.c"
    break;

  case 195: /* doit_command: BX_TOKEN_DOIT expression '\n'  */
#line 996 "parser.y"
      {
        bx_dbg_doit_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3520 "y.tab.c"
    break;

  case 196: /* crc_command: BX_TOKEN_CRC expression expression '\n'  */
#line 1004 "parser.y"
      {
        bx_dbg_crc_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3529 "y.tab.c"
    break;

  case 197: /* help_command: BX_TOKEN_HELP BX_TOKEN_QUIT '\n'  */
#line 1012 "parser.y"
       {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3538 "y.tab.c"
    break;

  case 198: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTINUE '\n'  */
#line 1017 "parser.y"
       {
         dbg_printf("c|cont|continue - continue executing\n");
         dbg_printf("c|cont|continue if \"expression\" - continue executing only if expression is true\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3548 "y.tab.c"
    break;

  case 199: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEPN '\n'  */
#line 1023 "parser.y"
       {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3559 "y.tab.c"
    break;

  case 200: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEP_OVER '\n'  */
#line 1030 "parser.y"
       {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3568 "y.tab.c"
    break;

  case 201: /* help_command: BX_TOKEN_HELP BX_TOKEN_VBREAKPOINT '\n'  */
#line 1035 "parser.y"
       {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         dbg_printf("vb|vbreak <seg:offset> if \"expression\" - set a conditional virtual address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3578 "y.tab.c"
    break;

  case 202: /* help_command: BX_TOKEN_HELP BX_TOKEN_LBREAKPOINT '\n'  */
#line 1041 "parser.y"
       {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         dbg_printf("lb|lbreak <addr> if \"expression\" - set a conditional linear address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3588 "y.tab.c"
    break;

  case 203: /* help_command: BX_TOKEN_HELP BX_TOKEN_PBREAKPOINT '\n'  */
#line 1047 "parser.y"
       {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         dbg_printf("p|pb|break|pbreak <addr> if \"expression\" - set a conditional physical address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3598 "y.tab.c"
    break;

  case 204: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEL_BREAKPOINT '\n'  */
#line 1053 "parser.y"
       {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3607 "y.tab.c"
    break;

  case 205: /* help_command: BX_TOKEN_HELP BX_TOKEN_ENABLE_BREAKPOINT '\n'  */
#line 1058 "parser.y"
       {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3616 "y.tab.c"
    break;

  case 206: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISABLE_BREAKPOINT '\n'  */
#line 1063 "parser.y"
       {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3625 "y.tab.c"
    break;

  case 207: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_BREAK '\n'  */
#line 1068 "parser.y"
       {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3634 "y.tab.c"
    break;

  case 208: /* help_command: BX_TOKEN_HELP BX_TOKEN_MODEBP '\n'  */
#line 1073 "parser.y"
       {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3643 "y.tab.c"
    break;

  case 209: /* help_command: BX_TOKEN_HELP BX_TOKEN_VMEXITBP '\n'  */
#line 1078 "parser.y"
       {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3652 "y.tab.c"
    break;

  case 210: /* help_command: BX_TOKEN_HELP BX_TOKEN_CRC '\n'  */
#line 1083 "parser.y"
       {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3661 "y.tab.c"
    break;

  case 211: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACE '\n'  */
#line 1088 "parser.y"
       {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3671 "y.tab.c"
    break;

  case 212: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEREG '\n'  */
#line 1094 "parser.y"
       {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3681 "y.tab.c"
    break;

  case 213: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEMEM '\n'  */
#line 1100 "parser.y"
       {
         dbg_printf("trace-mem on  - print all memory accesses occurred during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3691 "y.tab.c"
    break;

  case 214: /* help_command: BX_TOKEN_HELP BX_TOKEN_RESTORE '\n'  */
#line 1106 "parser.y"
       {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3703 "y.tab.c"
    break;

  case 215: /* help_command: BX_TOKEN_HELP BX_TOKEN_PTIME '\n'  */
#line 1114 "parser.y"
       {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3712 "y.tab.c"
    break;

  case 216: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP '\n'  */
#line 1119 "parser.y"
       {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3721 "y.tab.c"
    break;

  case 217: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP_ABSOLUTE '\n'  */
#line 1124 "parser.y"
       {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3730 "y.tab.c"
    break;

  case 218: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STACK '\n'  */
#line 1129 "parser.y"
       {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3739 "y.tab.c"
    break;

  case 219: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOAD_SYMBOLS '\n'  */
#line 1134 "parser.y"
       {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3748 "y.tab.c"
    break;

  case 220: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 1139 "parser.y"
       {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3757 "y.tab.c"
    break;

  case 221: /* help_command: BX_TOKEN_HELP BX_TOKEN_REGISTERS '\n'  */
#line 1144 "parser.y"
       {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3766 "y.tab.c"
    break;

  case 222: /* help_command: BX_TOKEN_HELP BX_TOKEN_FPU '\n'  */
#line 1149 "parser.y"
       {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3775 "y.tab.c"
    break;

  case 223: /* help_command: BX_TOKEN_HELP BX_TOKEN_MMX '\n'  */
#line 1154 "parser.y"
       {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3784 "y.tab.c"
    break;

  case 224: /* help_command: BX_TOKEN_HELP BX_TOKEN_XMM '\n'  */
#line 1159 "parser.y"
       {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3793 "y.tab.c"
    break;

  case 225: /* help_command: BX_TOKEN_HELP BX_TOKEN_YMM '\n'  */
#line 1164 "parser.y"
       {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3802 "y.tab.c"
    break;

  case 226: /* help_command: BX_TOKEN_HELP BX_TOKEN_ZMM '\n'  */
#line 1169 "parser.y"
       {
         dbg_printf("zmm - print AVX-512 state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3811 "y.tab.c"
    break;

  case 227: /* help_command: BX_TOKEN_HELP BX_TOKEN_SEGMENT_REGS '\n'  */
#line 1174 "parser.y"
       {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3820 "y.tab.c"
    break;

  case 228: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTROL_REGS '\n'  */
#line 1179 "parser.y"
       {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3829 "y.tab.c"
    break;

  case 229: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEBUG_REGS '\n'  */
#line 1184 "parser.y"
       {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3838 "y.tab.c"
    break;

  case 230: /* help_command: BX_TOKEN_HELP BX_TOKEN_WRITEMEM '\n'  */
#line 1189 "parser.y"
       {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3847 "y.tab.c"
    break;

  case 231: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOADMEM '\n'  */
#line 1194 "parser.y"
       {
         dbg_printf("loadmem <filename> <laddr> - load file bytes to virtual memory starting from the linear address 'laddr'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3856 "y.tab.c"
    break;

  case 232: /* help_command: BX_TOKEN_HELP BX_TOKEN_SETPMEM '\n'  */
#line 1199 "parser.y"
       {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3865 "y.tab.c"
    break;

  case 233: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISASM '\n'  */
#line 1204 "parser.y"
       {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm hex on/off - control disasm offsets and displacements format\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3879 "y.tab.c"
    break;

  case 234: /* help_command: BX_TOKEN_HELP BX_TOKEN_WATCH '\n'  */
#line 1214 "parser.y"
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
#line 3894 "y.tab.c"
    break;

  case 235: /* help_command: BX_TOKEN_HELP BX_TOKEN_UNWATCH '\n'  */
#line 1225 "parser.y"
       {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3904 "y.tab.c"
    break;

  case 236: /* help_command: BX_TOKEN_HELP BX_TOKEN_EXAMINE '\n'  */
#line 1231 "parser.y"
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
#line 3921 "y.tab.c"
    break;

  case 237: /* help_command: BX_TOKEN_HELP BX_TOKEN_INSTRUMENT '\n'  */
#line 1244 "parser.y"
       {
         dbg_printf("instrument <command|\"string command\"> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command|\"string command\">\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3930 "y.tab.c"
    break;

  case 238: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET '\n'  */
#line 1249 "parser.y"
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
#line 3945 "y.tab.c"
    break;

  case 239: /* help_command: BX_TOKEN_HELP BX_TOKEN_PAGE '\n'  */
#line 1260 "parser.y"
       {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3954 "y.tab.c"
    break;

  case 240: /* help_command: BX_TOKEN_HELP BX_TOKEN_INFO '\n'  */
#line 1265 "parser.y"
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
#line 3974 "y.tab.c"
    break;

  case 241: /* help_command: BX_TOKEN_HELP BX_TOKEN_SHOW '\n'  */
#line 1281 "parser.y"
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
#line 3994 "y.tab.c"
    break;

  case 242: /* help_command: BX_TOKEN_HELP BX_TOKEN_CALC '\n'  */
#line 1297 "parser.y"
       {
         dbg_printf("calc|? <expr> - calculate a expression and display the result.\n");
         dbg_printf("    'expr' can reference any general-purpose, opmask and segment\n");
         dbg_printf("    registers, use any arithmetic and logic operations, and\n");
         dbg_printf("    also the special ':' operator which computes the linear\n");
         dbg_printf("    address of a segment:offset (in real and v86 mode) or of\n");
         dbg_printf("    a selector:offset (in protected mode) pair.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4008 "y.tab.c"
    break;

  case 243: /* help_command: BX_TOKEN_HELP BX_TOKEN_HELP '\n'  */
#line 1307 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4017 "y.tab.c"
    break;

  case 244: /* help_command: BX_TOKEN_HELP '\n'  */
#line 1312 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-1].sval));
       }
#line 4026 "y.tab.c"
    break;

  case 245: /* calc_command: BX_TOKEN_CALC expression '\n'  */
#line 1320 "parser.y"
   {
     eval_value = (yyvsp[-1].uval);
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4036 "y.tab.c"
    break;

  case 246: /* if_command: BX_TOKEN_IF expression '\n'  */
#line 1329 "parser.y"
   {
     eval_value = (yyvsp[-1].uval) != 0;
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4046 "y.tab.c"
    break;

  case 247: /* vexpression: BX_TOKEN_NUMERIC  */
#line 1338 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4052 "y.tab.c"
    break;

  case 248: /* vexpression: BX_TOKEN_STRING  */
#line 1339 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4058 "y.tab.c"
    break;

  case 249: /* vexpression: BX_TOKEN_8BL_REG  */
#line 1340 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4064 "y.tab.c"
    break;

  case 250: /* vexpression: BX_TOKEN_8BH_REG  */
#line 1341 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4070 "y.tab.c"
    break;

  case 251: /* vexpression: BX_TOKEN_16B_REG  */
#line 1342 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4076 "y.tab.c"
    break;

  case 252: /* vexpression: BX_TOKEN_32B_REG  */
#line 1343 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4082 "y.tab.c"
    break;

  case 253: /* vexpression: BX_TOKEN_64B_REG  */
#line 1344 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4088 "y.tab.c"
    break;

  case 254: /* vexpression: BX_TOKEN_OPMASK_REG  */
#line 1345 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4094 "y.tab.c"
    break;

  case 255: /* vexpression: BX_TOKEN_SEGREG  */
#line 1346 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4100 "y.tab.c"
    break;

  case 256: /* vexpression: BX_TOKEN_REG_IP  */
#line 1347 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4106 "y.tab.c"
    break;

  case 257: /* vexpression: BX_TOKEN_REG_EIP  */
#line 1348 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4112 "y.tab.c"
    break;

  case 258: /* vexpression: BX_TOKEN_REG_RIP  */
#line 1349 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4118 "y.tab.c"
    break;

  case 259: /* vexpression: BX_TOKEN_REG_SSP  */
#line 1350 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4124 "y.tab.c"
    break;

  case 260: /* vexpression: vexpression '+' vexpression  */
#line 1351 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4130 "y.tab.c"
    break;

  case 261: /* vexpression: vexpression '-' vexpression  */
#line 1352 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4136 "y.tab.c"
    break;

  case 262: /* vexpression: vexpression '*' vexpression  */
#line 1353 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4142 "y.tab.c"
    break;

  case 263: /* vexpression: vexpression '/' vexpression  */
#line 1354 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) / (yyvsp[0].uval); }
#line 4148 "y.tab.c"
    break;

  case 264: /* vexpression: vexpression BX_TOKEN_RSHIFT vexpression  */
#line 1355 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4154 "y.tab.c"
    break;

  case 265: /* vexpression: vexpression BX_TOKEN_LSHIFT vexpression  */
#line 1356 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4160 "y.tab.c"
    break;

  case 266: /* vexpression: vexpression '|' vexpression  */
#line 1357 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4166 "y.tab.c"
    break;

  case 267: /* vexpression: vexpression '^' vexpression  */
#line 1358 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4172 "y.tab.c"
    break;

  case 268: /* vexpression: vexpression '&' vexpression  */
#line 1359 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4178 "y.tab.c"
    break;

  case 269: /* vexpression: '!' vexpression  */
#line 1360 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4184 "y.tab.c"
    break;

  case 270: /* vexpression: '-' vexpression  */
#line 1361 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4190 "y.tab.c"
    break;

  case 271: /* vexpression: '(' vexpression ')'  */
#line 1362 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4196 "y.tab.c"
    break;

  case 272: /* expression: BX_TOKEN_NUMERIC  */
#line 1368 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4202 "y.tab.c"
    break;

  case 273: /* expression: BX_TOKEN_STRING  */
#line 1369 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4208 "y.tab.c"
    break;

  case 274: /* expression: BX_TOKEN_8BL_REG  */
#line 1370 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4214 "y.tab.c"
    break;

  case 275: /* expression: BX_TOKEN_8BH_REG  */
#line 1371 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4220 "y.tab.c"
    break;

  case 276: /* expression: BX_TOKEN_16B_REG  */
#line 1372 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4226 "y.tab.c"
    break;

  case 277: /* expression: BX_TOKEN_32B_REG  */
#line 1373 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4232 "y.tab.c"
    break;

  case 278: /* expression: BX_TOKEN_64B_REG  */
#line 1374 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4238 "y.tab.c"
    break;

  case 279: /* expression: BX_TOKEN_OPMASK_REG  */
#line 1375 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4244 "y.tab.c"
    break;

  case 280: /* expression: BX_TOKEN_SEGREG  */
#line 1376 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4250 "y.tab.c"
    break;

  case 281: /* expression: BX_TOKEN_REG_IP  */
#line 1377 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4256 "y.tab.c"
    break;

  case 282: /* expression: BX_TOKEN_REG_EIP  */
#line 1378 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4262 "y.tab.c"
    break;

  case 283: /* expression: BX_TOKEN_REG_RIP  */
#line 1379 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4268 "y.tab.c"
    break;

  case 284: /* expression: BX_TOKEN_REG_SSP  */
#line 1380 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4274 "y.tab.c"
    break;

  case 285: /* expression: expression ':' expression  */
#line 1381 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[-2].uval), (yyvsp[0].uval)); }
#line 4280 "y.tab.c"
    break;

  case 286: /* expression: expression '+' expression  */
#line 1382 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4286 "y.tab.c"
    break;

  case 287: /* expression: expression '-' expression  */
#line 1383 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4292 "y.tab.c"
    break;

  case 288: /* expression: expression '*' expression  */
#line 1384 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4298 "y.tab.c"
    break;

  case 289: /* expression: expression '/' expression  */
#line 1385 "parser.y"
                                     { (yyval.uval) = ((yyvsp[0].uval) != 0) ? (yyvsp[-2].uval) / (yyvsp[0].uval) : 0; }
#line 4304 "y.tab.c"
    break;

  case 290: /* expression: expression BX_TOKEN_RSHIFT expression  */
#line 1386 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4310 "y.tab.c"
    break;

  case 291: /* expression: expression BX_TOKEN_LSHIFT expression  */
#line 1387 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4316 "y.tab.c"
    break;

  case 292: /* expression: expression '|' expression  */
#line 1388 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4322 "y.tab.c"
    break;

  case 293: /* expression: expression '^' expression  */
#line 1389 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4328 "y.tab.c"
    break;

  case 294: /* expression: expression '&' expression  */
#line 1390 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4334 "y.tab.c"
    break;

  case 295: /* expression: expression '>' expression  */
#line 1391 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) > (yyvsp[0].uval); }
#line 4340 "y.tab.c"
    break;

  case 296: /* expression: expression '<' expression  */
#line 1392 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) < (yyvsp[0].uval); }
#line 4346 "y.tab.c"
    break;

  case 297: /* expression: expression BX_TOKEN_EQ expression  */
#line 1393 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) == (yyvsp[0].uval); }
#line 4352 "y.tab.c"
    break;

  case 298: /* expression: expression BX_TOKEN_NE expression  */
#line 1394 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) != (yyvsp[0].uval); }
#line 4358 "y.tab.c"
    break;

  case 299: /* expression: expression BX_TOKEN_LE expression  */
#line 1395 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) <= (yyvsp[0].uval); }
#line 4364 "y.tab.c"
    break;

  case 300: /* expression: expression BX_TOKEN_GE expression  */
#line 1396 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) >= (yyvsp[0].uval); }
#line 4370 "y.tab.c"
    break;

  case 301: /* expression: '!' expression  */
#line 1397 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4376 "y.tab.c"
    break;

  case 302: /* expression: '-' expression  */
#line 1398 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4382 "y.tab.c"
    break;

  case 303: /* expression: '*' expression  */
#line 1399 "parser.y"
                                     { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[0].uval)); }
#line 4388 "y.tab.c"
    break;

  case 304: /* expression: '@' expression  */
#line 1400 "parser.y"
                                     { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[0].uval)); }
#line 4394 "y.tab.c"
    break;

  case 305: /* expression: '(' expression ')'  */
#line 1401 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4400 "y.tab.c"
    break;


#line 4404 "y.tab.c"

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

#line 1404 "parser.y"

#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
