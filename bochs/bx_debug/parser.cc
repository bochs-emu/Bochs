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
    BX_TOKEN_SET_MAGIC_BREAK_POINTS = 349, /* BX_TOKEN_SET_MAGIC_BREAK_POINTS  */
    BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS = 350, /* BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS  */
    BX_TOKEN_SYMBOLS = 351,        /* BX_TOKEN_SYMBOLS  */
    BX_TOKEN_LIST_SYMBOLS = 352,   /* BX_TOKEN_LIST_SYMBOLS  */
    BX_TOKEN_GLOBAL = 353,         /* BX_TOKEN_GLOBAL  */
    BX_TOKEN_WHERE = 354,          /* BX_TOKEN_WHERE  */
    BX_TOKEN_PRINT_STRING = 355,   /* BX_TOKEN_PRINT_STRING  */
    BX_TOKEN_NUMERIC = 356,        /* BX_TOKEN_NUMERIC  */
    BX_TOKEN_PAGE = 357,           /* BX_TOKEN_PAGE  */
    BX_TOKEN_HELP = 358,           /* BX_TOKEN_HELP  */
    BX_TOKEN_XML = 359,            /* BX_TOKEN_XML  */
    BX_TOKEN_CALC = 360,           /* BX_TOKEN_CALC  */
    BX_TOKEN_ADDLYT = 361,         /* BX_TOKEN_ADDLYT  */
    BX_TOKEN_REMLYT = 362,         /* BX_TOKEN_REMLYT  */
    BX_TOKEN_LYT = 363,            /* BX_TOKEN_LYT  */
    BX_TOKEN_SOURCE = 364,         /* BX_TOKEN_SOURCE  */
    BX_TOKEN_DEVICE = 365,         /* BX_TOKEN_DEVICE  */
    BX_TOKEN_GENERIC = 366,        /* BX_TOKEN_GENERIC  */
    BX_TOKEN_DEREF_CHR = 367,      /* BX_TOKEN_DEREF_CHR  */
    BX_TOKEN_RSHIFT = 368,         /* BX_TOKEN_RSHIFT  */
    BX_TOKEN_LSHIFT = 369,         /* BX_TOKEN_LSHIFT  */
    BX_TOKEN_EQ = 370,             /* BX_TOKEN_EQ  */
    BX_TOKEN_NE = 371,             /* BX_TOKEN_NE  */
    BX_TOKEN_LE = 372,             /* BX_TOKEN_LE  */
    BX_TOKEN_GE = 373,             /* BX_TOKEN_GE  */
    BX_TOKEN_REG_IP = 374,         /* BX_TOKEN_REG_IP  */
    BX_TOKEN_REG_EIP = 375,        /* BX_TOKEN_REG_EIP  */
    BX_TOKEN_REG_RIP = 376,        /* BX_TOKEN_REG_RIP  */
    BX_TOKEN_REG_SSP = 377,        /* BX_TOKEN_REG_SSP  */
    NOT = 378,                     /* NOT  */
    NEG = 379,                     /* NEG  */
    INDIRECT = 380                 /* INDIRECT  */
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
#define BX_TOKEN_SET_MAGIC_BREAK_POINTS 349
#define BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS 350
#define BX_TOKEN_SYMBOLS 351
#define BX_TOKEN_LIST_SYMBOLS 352
#define BX_TOKEN_GLOBAL 353
#define BX_TOKEN_WHERE 354
#define BX_TOKEN_PRINT_STRING 355
#define BX_TOKEN_NUMERIC 356
#define BX_TOKEN_PAGE 357
#define BX_TOKEN_HELP 358
#define BX_TOKEN_XML 359
#define BX_TOKEN_CALC 360
#define BX_TOKEN_ADDLYT 361
#define BX_TOKEN_REMLYT 362
#define BX_TOKEN_LYT 363
#define BX_TOKEN_SOURCE 364
#define BX_TOKEN_DEVICE 365
#define BX_TOKEN_GENERIC 366
#define BX_TOKEN_DEREF_CHR 367
#define BX_TOKEN_RSHIFT 368
#define BX_TOKEN_LSHIFT 369
#define BX_TOKEN_EQ 370
#define BX_TOKEN_NE 371
#define BX_TOKEN_LE 372
#define BX_TOKEN_GE 373
#define BX_TOKEN_REG_IP 374
#define BX_TOKEN_REG_EIP 375
#define BX_TOKEN_REG_RIP 376
#define BX_TOKEN_REG_SSP 377
#define NOT 378
#define NEG 379
#define INDIRECT 380

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 14 "parser.y"

  char    *sval;
  Bit64u   uval;
  unsigned bval;

#line 396 "y.tab.c"

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
  YYSYMBOL_BX_TOKEN_SET_MAGIC_BREAK_POINTS = 94, /* BX_TOKEN_SET_MAGIC_BREAK_POINTS  */
  YYSYMBOL_BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS = 95, /* BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS  */
  YYSYMBOL_BX_TOKEN_SYMBOLS = 96,          /* BX_TOKEN_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_LIST_SYMBOLS = 97,     /* BX_TOKEN_LIST_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_GLOBAL = 98,           /* BX_TOKEN_GLOBAL  */
  YYSYMBOL_BX_TOKEN_WHERE = 99,            /* BX_TOKEN_WHERE  */
  YYSYMBOL_BX_TOKEN_PRINT_STRING = 100,    /* BX_TOKEN_PRINT_STRING  */
  YYSYMBOL_BX_TOKEN_NUMERIC = 101,         /* BX_TOKEN_NUMERIC  */
  YYSYMBOL_BX_TOKEN_PAGE = 102,            /* BX_TOKEN_PAGE  */
  YYSYMBOL_BX_TOKEN_HELP = 103,            /* BX_TOKEN_HELP  */
  YYSYMBOL_BX_TOKEN_XML = 104,             /* BX_TOKEN_XML  */
  YYSYMBOL_BX_TOKEN_CALC = 105,            /* BX_TOKEN_CALC  */
  YYSYMBOL_BX_TOKEN_ADDLYT = 106,          /* BX_TOKEN_ADDLYT  */
  YYSYMBOL_BX_TOKEN_REMLYT = 107,          /* BX_TOKEN_REMLYT  */
  YYSYMBOL_BX_TOKEN_LYT = 108,             /* BX_TOKEN_LYT  */
  YYSYMBOL_BX_TOKEN_SOURCE = 109,          /* BX_TOKEN_SOURCE  */
  YYSYMBOL_BX_TOKEN_DEVICE = 110,          /* BX_TOKEN_DEVICE  */
  YYSYMBOL_BX_TOKEN_GENERIC = 111,         /* BX_TOKEN_GENERIC  */
  YYSYMBOL_BX_TOKEN_DEREF_CHR = 112,       /* BX_TOKEN_DEREF_CHR  */
  YYSYMBOL_BX_TOKEN_RSHIFT = 113,          /* BX_TOKEN_RSHIFT  */
  YYSYMBOL_BX_TOKEN_LSHIFT = 114,          /* BX_TOKEN_LSHIFT  */
  YYSYMBOL_BX_TOKEN_EQ = 115,              /* BX_TOKEN_EQ  */
  YYSYMBOL_BX_TOKEN_NE = 116,              /* BX_TOKEN_NE  */
  YYSYMBOL_BX_TOKEN_LE = 117,              /* BX_TOKEN_LE  */
  YYSYMBOL_BX_TOKEN_GE = 118,              /* BX_TOKEN_GE  */
  YYSYMBOL_BX_TOKEN_REG_IP = 119,          /* BX_TOKEN_REG_IP  */
  YYSYMBOL_BX_TOKEN_REG_EIP = 120,         /* BX_TOKEN_REG_EIP  */
  YYSYMBOL_BX_TOKEN_REG_RIP = 121,         /* BX_TOKEN_REG_RIP  */
  YYSYMBOL_BX_TOKEN_REG_SSP = 122,         /* BX_TOKEN_REG_SSP  */
  YYSYMBOL_123_ = 123,                     /* '+'  */
  YYSYMBOL_124_ = 124,                     /* '-'  */
  YYSYMBOL_125_ = 125,                     /* '|'  */
  YYSYMBOL_126_ = 126,                     /* '^'  */
  YYSYMBOL_127_ = 127,                     /* '<'  */
  YYSYMBOL_128_ = 128,                     /* '>'  */
  YYSYMBOL_129_ = 129,                     /* '*'  */
  YYSYMBOL_130_ = 130,                     /* '/'  */
  YYSYMBOL_131_ = 131,                     /* '&'  */
  YYSYMBOL_NOT = 132,                      /* NOT  */
  YYSYMBOL_NEG = 133,                      /* NEG  */
  YYSYMBOL_INDIRECT = 134,                 /* INDIRECT  */
  YYSYMBOL_135_n_ = 135,                   /* '\n'  */
  YYSYMBOL_136_ = 136,                     /* '='  */
  YYSYMBOL_137_ = 137,                     /* ':'  */
  YYSYMBOL_138_ = 138,                     /* '!'  */
  YYSYMBOL_139_ = 139,                     /* '('  */
  YYSYMBOL_140_ = 140,                     /* ')'  */
  YYSYMBOL_141_ = 141,                     /* '@'  */
  YYSYMBOL_YYACCEPT = 142,                 /* $accept  */
  YYSYMBOL_commands = 143,                 /* commands  */
  YYSYMBOL_command = 144,                  /* command  */
  YYSYMBOL_BX_TOKEN_TOGGLE_ON_OFF = 145,   /* BX_TOKEN_TOGGLE_ON_OFF  */
  YYSYMBOL_BX_TOKEN_REGISTERS = 146,       /* BX_TOKEN_REGISTERS  */
  YYSYMBOL_BX_TOKEN_SEGREG = 147,          /* BX_TOKEN_SEGREG  */
  YYSYMBOL_timebp_command = 148,           /* timebp_command  */
  YYSYMBOL_modebp_command = 149,           /* modebp_command  */
  YYSYMBOL_vmexitbp_command = 150,         /* vmexitbp_command  */
  YYSYMBOL_show_command = 151,             /* show_command  */
  YYSYMBOL_page_command = 152,             /* page_command  */
  YYSYMBOL_tlb_command = 153,              /* tlb_command  */
  YYSYMBOL_ptime_command = 154,            /* ptime_command  */
  YYSYMBOL_trace_command = 155,            /* trace_command  */
  YYSYMBOL_trace_reg_command = 156,        /* trace_reg_command  */
  YYSYMBOL_trace_mem_command = 157,        /* trace_mem_command  */
  YYSYMBOL_print_stack_command = 158,      /* print_stack_command  */
  YYSYMBOL_backtrace_command = 159,        /* backtrace_command  */
  YYSYMBOL_watch_point_command = 160,      /* watch_point_command  */
  YYSYMBOL_symbol_command = 161,           /* symbol_command  */
  YYSYMBOL_set_magic_break_points_command = 162, /* set_magic_break_points_command  */
  YYSYMBOL_clr_magic_break_points_command = 163, /* clr_magic_break_points_command  */
  YYSYMBOL_where_command = 164,            /* where_command  */
  YYSYMBOL_print_string_command = 165,     /* print_string_command  */
  YYSYMBOL_continue_command = 166,         /* continue_command  */
  YYSYMBOL_stepN_command = 167,            /* stepN_command  */
  YYSYMBOL_step_over_command = 168,        /* step_over_command  */
  YYSYMBOL_set_command = 169,              /* set_command  */
  YYSYMBOL_breakpoint_command = 170,       /* breakpoint_command  */
  YYSYMBOL_blist_command = 171,            /* blist_command  */
  YYSYMBOL_slist_command = 172,            /* slist_command  */
  YYSYMBOL_info_command = 173,             /* info_command  */
  YYSYMBOL_optional_numeric = 174,         /* optional_numeric  */
  YYSYMBOL_regs_command = 175,             /* regs_command  */
  YYSYMBOL_fpu_regs_command = 176,         /* fpu_regs_command  */
  YYSYMBOL_mmx_regs_command = 177,         /* mmx_regs_command  */
  YYSYMBOL_xmm_regs_command = 178,         /* xmm_regs_command  */
  YYSYMBOL_ymm_regs_command = 179,         /* ymm_regs_command  */
  YYSYMBOL_zmm_regs_command = 180,         /* zmm_regs_command  */
  YYSYMBOL_segment_regs_command = 181,     /* segment_regs_command  */
  YYSYMBOL_control_regs_command = 182,     /* control_regs_command  */
  YYSYMBOL_debug_regs_command = 183,       /* debug_regs_command  */
  YYSYMBOL_delete_command = 184,           /* delete_command  */
  YYSYMBOL_bpe_command = 185,              /* bpe_command  */
  YYSYMBOL_bpd_command = 186,              /* bpd_command  */
  YYSYMBOL_quit_command = 187,             /* quit_command  */
  YYSYMBOL_examine_command = 188,          /* examine_command  */
  YYSYMBOL_restore_command = 189,          /* restore_command  */
  YYSYMBOL_writemem_command = 190,         /* writemem_command  */
  YYSYMBOL_loadmem_command = 191,          /* loadmem_command  */
  YYSYMBOL_setpmem_command = 192,          /* setpmem_command  */
  YYSYMBOL_deref_command = 193,            /* deref_command  */
  YYSYMBOL_query_command = 194,            /* query_command  */
  YYSYMBOL_take_command = 195,             /* take_command  */
  YYSYMBOL_disassemble_command = 196,      /* disassemble_command  */
  YYSYMBOL_instrument_command = 197,       /* instrument_command  */
  YYSYMBOL_doit_command = 198,             /* doit_command  */
  YYSYMBOL_crc_command = 199,              /* crc_command  */
  YYSYMBOL_help_command = 200,             /* help_command  */
  YYSYMBOL_calc_command = 201,             /* calc_command  */
  YYSYMBOL_addlyt_command = 202,           /* addlyt_command  */
  YYSYMBOL_remlyt_command = 203,           /* remlyt_command  */
  YYSYMBOL_lyt_command = 204,              /* lyt_command  */
  YYSYMBOL_if_command = 205,               /* if_command  */
  YYSYMBOL_vexpression = 206,              /* vexpression  */
  YYSYMBOL_expression = 207                /* expression  */
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
#define YYFINAL  343
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2556

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  142
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  66
/* YYNRULES -- Number of rules.  */
#define YYNRULES  330
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  643

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   380


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
     135,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   138,     2,     2,     2,     2,   131,     2,
     139,   140,   129,   123,     2,   124,     2,   130,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   137,     2,
     127,   136,   128,     2,   141,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   126,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   125,     2,     2,     2,     2,     2,
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
     115,   116,   117,   118,   119,   120,   121,   122,   132,   133,
     134
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   158,   158,   159,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   228,   229,   234,   235,   240,   241,
     242,   243,   244,   245,   250,   255,   263,   271,   279,   284,
     289,   294,   299,   304,   312,   320,   328,   336,   344,   352,
     360,   365,   373,   378,   386,   391,   396,   401,   406,   411,
     416,   421,   426,   431,   436,   444,   449,   454,   459,   467,
     472,   480,   485,   493,   501,   509,   514,   522,   527,   532,
     537,   545,   553,   558,   563,   567,   571,   575,   579,   583,
     587,   591,   598,   603,   608,   613,   618,   623,   628,   633,
     638,   643,   648,   653,   658,   666,   674,   679,   687,   692,
     697,   702,   707,   712,   717,   722,   727,   732,   737,   742,
     747,   752,   757,   762,   767,   775,   776,   779,   787,   795,
     803,   811,   819,   827,   835,   843,   851,   859,   866,   874,
     882,   887,   892,   897,   905,   913,   921,   929,   937,   945,
     953,   958,   963,   968,   973,   981,   986,   991,   996,  1001,
    1006,  1011,  1016,  1024,  1030,  1035,  1043,  1051,  1059,  1064,
    1070,  1077,  1082,  1088,  1094,  1100,  1105,  1110,  1115,  1120,
    1125,  1130,  1135,  1141,  1147,  1153,  1161,  1166,  1171,  1176,
    1181,  1186,  1191,  1196,  1201,  1206,  1211,  1216,  1221,  1226,
    1231,  1236,  1241,  1246,  1251,  1256,  1261,  1266,  1271,  1281,
    1292,  1298,  1311,  1316,  1327,  1332,  1348,  1364,  1376,  1388,
    1393,  1399,  1404,  1409,  1414,  1422,  1431,  1440,  1448,  1456,
    1466,  1467,  1468,  1469,  1470,  1471,  1472,  1473,  1474,  1475,
    1476,  1477,  1478,  1479,  1480,  1481,  1482,  1483,  1484,  1485,
    1486,  1487,  1488,  1489,  1490,  1491,  1497,  1498,  1499,  1500,
    1501,  1502,  1503,  1504,  1505,  1506,  1507,  1508,  1509,  1510,
    1511,  1512,  1513,  1514,  1515,  1516,  1517,  1518,  1519,  1520,
    1521,  1522,  1523,  1524,  1525,  1526,  1527,  1528,  1529,  1530,
    1531
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
  "BX_TOKEN_SHOW", "BX_TOKEN_LOAD_SYMBOLS",
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

#define YYPACT_NINF (-192)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-329)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     603,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,   -17,  1361,   -38,  -124,  1176,  -116,  1402,
     123,  1021,   -53,   -47,   -42,  1580,   -74,  -192,  -192,   -63,
     -58,   -55,   -52,   -46,   -40,   -39,   -35,   320,    29,    33,
      36,  1361,  1361,    46,    48,  1361,    19,    -7,  -192,  1361,
    1361,    51,    51,    51,   -16,  1361,  1361,   -11,   -10,   -83,
     -65,    20,  1121,    40,    -8,   -59,   -56,   -33,     7,  1361,
    -192,  1361,  1560,  1361,    45,    10,    11,  -192,  -192,  -192,
    -192,  1361,  1361,  -192,  1361,  1361,  1361,   464,  -192,    12,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  2419,  1361,
    -192,   676,    49,   -43,  -192,  -192,    16,    17,    23,    37,
      38,    42,    51,    50,    53,    54,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  1414,
    -192,  1414,  1414,  -192,  1317,   -13,  -192,   281,  1361,  -192,
    1442,    21,    41,    52,    56,    57,    58,  1361,  1361,  1361,
    1361,    59,    61,    66,   -32,   -60,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  1160,  -192,   885,   112,  1361,
    1361,   982,   982,    67,   -41,    88,    90,    91,  1613,  1191,
      92,    95,  -192,   158,    93,    97,    99,  1639,   982,  -192,
    -192,   100,   101,   102,  -192,  1665,  1691,  -192,  -192,   104,
    -192,   105,  -192,   106,  1361,   111,  1361,  1361,  -192,  -192,
    1717,   113,   114,   -57,   115,  -192,  1261,   179,   118,  -192,
     120,  -192,   125,  -192,  -192,  1743,  1769,   128,   130,   132,
     133,   134,   159,   163,   170,   171,   175,   176,   177,   187,
     200,   202,   204,   206,   207,   208,   209,   210,   228,   229,
     230,   231,   232,   243,   255,   256,   265,   266,   267,   268,
     278,   279,   280,   282,   284,   285,   288,   292,   300,   302,
     303,   308,   313,   315,   317,   325,   327,   328,   330,   344,
     345,   346,  -192,   352,  1795,   364,  -192,  -192,   201,   201,
     201,  1584,   201,  -192,  -192,  -192,  1361,  1361,  1361,  1361,
    1361,  1361,  1361,  1361,  1361,  1361,  1361,  1361,  1361,  1361,
    1361,  1361,  1361,  1821,  -192,   370,   371,  -192,  1361,  1361,
    1361,  1361,  1361,  1361,   372,  1361,  1361,  1361,  -192,  -192,
    1094,  1414,  1414,  1414,  1414,  1414,  1414,  1414,  1414,  1414,
    1414,  1414,   379,  -192,   436,  -192,   -14,   437,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  1361,  2419,  1361,  1361,  1361,
    -192,  -192,  -192,   375,  -192,   -29,   -28,  -192,  -192,  1847,
    -192,   376,   982,  1873,  1361,  1361,   982,  1899,  -192,   377,
    -192,  -192,  -192,  -192,  -192,  -192,   742,  -192,    84,  -192,
    1925,  -192,  -192,  -192,  -192,  1951,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,   773,  -192,   812,   951,  -192,  -192,
    -192,   378,  -192,  -192,  -192,  1977,  1331,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,   199,   199,   199,   201,   201,   201,   201,   316,   316,
     316,   316,   316,   316,   199,   199,   199,  2419,  -192,  -192,
    -192,  2003,  2029,  2055,  2081,  2107,  2133,  -192,  2159,  2185,
    2211,  -192,  -192,  -192,  -192,    69,    69,    69,    69,  -192,
    -192,  -192,   178,   383,   384,   453,  -192,   392,   394,   395,
     396,   397,  -192,   402,  -192,   408,  -192,  -192,  -192,  2237,
    -192,   461,   203,  2263,  -192,  -192,  -192,  2289,   409,  -192,
    -192,  -192,  2315,  -192,  2341,  -192,  2367,  -192,  -192,  -192,
    2393,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
     482,  -192,  -192,  -192,   420,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
     425,  -192,  -192
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
      62,   299,   298,   300,   301,   302,    68,    69,    70,    71,
      72,    73,   303,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    66,    67,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   297,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     296,     0,     0,     0,     0,     0,     0,   305,   306,   307,
     308,     0,     0,    63,     0,     0,     0,     0,     3,     0,
     304,    41,    42,    43,    51,    49,    50,    40,    37,    38,
      39,    44,    45,    48,    52,    46,    47,    53,    54,     4,
       5,     6,     7,     8,    19,    20,     9,    10,    11,    12,
      13,    14,    15,    16,    18,    17,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    55,    56,    57,    58,    59,    60,    61,     0,
     115,     0,     0,     0,   117,   121,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   145,   273,   272,   274,
     275,   276,   277,   271,   270,   279,   280,   281,   282,     0,
     132,     0,     0,   278,     0,   297,   135,     0,     0,   140,
       0,     0,     0,     0,     0,     0,     0,   165,   165,   165,
     165,     0,     0,     0,     0,     0,   179,   168,   169,   170,
     171,   172,   175,   174,   173,     0,   183,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   195,     0,     0,     0,     0,     0,     0,    64,
      65,     0,     0,     0,    86,     0,     0,    76,    77,     0,
      90,     0,    92,     0,     0,     0,     0,     0,    96,   103,
       0,     0,     0,     0,     0,    83,     0,     0,     0,   109,
       0,   111,     0,   146,   113,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   264,     0,     0,     0,   267,   268,   327,   328,
     326,     0,   329,     1,     2,   167,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   269,     0,     0,   118,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   294,   293,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   138,     0,   136,   328,     0,   141,   176,
     177,   178,   156,   148,   149,   165,   166,   165,   165,   165,
     155,   154,   157,     0,   158,     0,     0,   160,   181,     0,
     182,     0,     0,     0,     0,     0,     0,     0,   189,     0,
     190,   192,   193,   194,    85,   198,     0,   201,     0,   196,
       0,   204,   203,   205,   206,     0,    87,    88,    89,    75,
      74,    91,    93,    95,     0,    94,     0,     0,   104,    80,
      79,     0,    81,    78,   105,     0,     0,   110,   112,   147,
     114,    84,   209,   210,   211,   253,   218,   212,   213,   214,
     215,   216,   217,   255,   208,   236,   237,   238,   239,   240,
     243,   242,   241,   251,   225,   244,   245,   246,   247,   248,
     252,   221,   222,   223,   224,   226,   228,   227,   219,   220,
     229,   230,   249,   250,   256,   231,   232,   233,   234,   261,
     254,   263,   257,   258,   259,   260,   262,   235,   265,   266,
     330,   314,   315,   316,   322,   323,   324,   325,   310,   311,
     317,   318,   321,   320,   312,   313,   319,   309,   116,   119,
     120,     0,     0,     0,     0,     0,     0,   122,     0,     0,
       0,   295,   287,   288,   289,   283,   284,   290,   291,   285,
     286,   292,     0,     0,     0,     0,   143,     0,     0,     0,
       0,     0,   159,     0,   163,     0,   161,   180,   184,     0,
     186,   311,   312,     0,   188,   191,   199,     0,     0,   197,
     207,    97,     0,    98,     0,    99,     0,    82,   106,   107,
       0,   125,   124,   126,   127,   128,   123,   129,   130,   131,
       0,   133,   139,   137,     0,   142,   150,   151,   152,   153,
     164,   162,   185,   187,   200,   202,   100,   101,   102,   108,
       0,   144,   134
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -192,  -192,   475,   -18,   496,    -2,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -191,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,  -192,
    -192,  -192,  -192,  -192,  -178,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    87,    88,   241,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   405,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   184,   406
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     148,   378,   149,   379,   380,   575,   392,   407,   408,   409,
     152,   155,   415,   268,   151,   165,   270,   183,   249,   166,
     187,   190,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,   242,   243,   251,   217,   253,   272,
     413,   221,   222,   583,   585,   228,   233,   461,   191,   237,
     238,   416,   250,   254,   192,   245,   246,   261,   366,   193,
     429,   206,   260,   153,   266,   234,   235,   239,   240,   275,
     252,   276,   207,   334,   229,   417,   269,   208,   462,   271,
     209,   338,   339,   210,   340,   341,   342,   148,   262,   211,
     267,    48,   367,   255,   430,   212,   213,   154,   230,   231,
     214,   218,   273,   414,   236,   219,   584,   586,   220,   223,
     256,   257,   263,   224,   225,   226,   227,   335,   150,   244,
      70,   576,   393,   362,   247,   248,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    77,    78,
      79,    80,   274,    81,   374,   336,   337,   345,    82,   363,
     365,   264,   368,   369,   232,   258,   399,    84,    85,   370,
      86,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,   371,   372,   265,   400,   183,   373,   183,
     183,   381,   382,   383,   421,   598,   375,   401,   396,   376,
     377,   402,   403,   404,   410,   185,   411,   620,   388,   389,
     390,   412,   428,   562,   563,   564,   565,   566,   567,   568,
     569,   570,   571,   572,   578,   419,   579,   580,   581,   422,
     423,   426,   427,   431,    70,   432,   433,   437,   441,   436,
      48,   438,   442,   440,   443,   446,   447,   448,   445,   451,
     452,   453,    77,    78,    79,    80,   455,    81,   459,   460,
     463,   466,    82,   467,   454,   468,   456,   457,   186,    70,
     469,    84,    85,   472,    86,   473,   465,   474,   475,   476,
     346,   347,   348,   349,   350,   351,   352,    77,    78,    79,
      80,   353,   424,   355,   356,   357,   358,   425,   360,   361,
     381,   382,   383,   439,   477,   362,    84,    85,   478,    86,
     394,   384,   385,   386,   387,   479,   480,   388,   389,   390,
     481,   482,   483,   621,   349,   350,   351,   352,  -328,  -328,
    -328,  -328,   484,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,   485,   362,   486,   362,   487,
     362,   488,   489,   490,   491,   492,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   493,   494,   495,   496,   497,   551,   552,
     553,   554,   555,   556,   215,   558,   559,   560,   498,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     499,   500,    48,   346,   347,   348,   349,   350,   351,   352,
     501,   502,   503,   504,   353,   354,   355,   356,   357,   358,
     359,   360,   361,   505,   506,   507,   395,   508,   362,   509,
     510,    70,   589,   511,   591,   592,   593,   512,   346,   347,
     348,   349,   350,   351,   352,   513,   597,   514,   515,    77,
      78,    79,    80,   516,    81,   359,   360,   361,   517,    82,
     518,   573,   519,   362,   602,   216,   604,   606,    84,    85,
     520,    86,   521,   522,   343,   523,   610,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,   524,
     525,   526,    13,    14,    15,    16,    17,   527,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,   529,
      29,    30,    31,    32,    33,   549,   550,   557,   574,   577,
     582,   588,   595,   607,    34,    35,    36,    37,   622,   623,
      38,    39,    40,    41,    42,   624,    43,   625,    44,   626,
     627,   628,   629,    45,    46,    47,    48,   630,    49,    50,
      51,    52,    53,   631,   635,    54,    55,    56,    57,    58,
      59,    60,    61,    62,   640,   641,    63,    64,    65,    66,
     642,    67,   344,    68,    69,    70,    71,    72,   333,    73,
      74,    75,    76,  -327,  -327,  -327,  -327,  -327,  -327,  -327,
       0,     0,     0,    77,    78,    79,    80,     0,    81,     0,
    -327,  -327,  -327,    82,     0,     0,     0,     0,   362,    83,
       0,     0,    84,    85,     0,    86,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     0,     0,
       0,    13,    14,    15,    16,    17,     0,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,     0,    29,
      30,    31,    32,    33,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    34,    35,    36,    37,     0,     0,    38,
      39,    40,    41,    42,     0,    43,     0,    44,     0,     0,
       0,     0,    45,    46,    47,    48,     0,    49,    50,    51,
      52,    53,     0,     0,    54,    55,    56,    57,    58,    59,
      60,    61,    62,     0,     0,    63,    64,    65,    66,     0,
      67,     0,    68,    69,    70,    71,    72,     0,    73,    74,
      75,    76,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    77,    78,    79,    80,     0,    81,     0,     0,
       0,     0,    82,     0,     0,     0,     0,     0,    83,     0,
       0,    84,    85,     0,    86,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,   346,   347,
     348,   349,   350,   351,   352,     0,     0,     0,     0,   353,
     354,   355,   356,   357,   358,   359,   360,   361,     0,     0,
       0,   364,     0,   362,    48,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    70,     0,    48,     0,     0,     0,     0,
       0,     0,     0,     0,   346,   347,   348,   349,   350,   351,
     352,    77,    78,    79,    80,   353,   424,   355,   356,   357,
     358,   425,   360,   361,    70,     0,     0,   596,     0,   362,
      84,    85,     0,    86,    48,   346,   347,   348,   349,   350,
     351,   352,    77,    78,    79,    80,   353,   424,   355,   356,
     357,   358,   425,   360,   361,     0,     0,     0,   601,     0,
     362,    84,    85,    70,    86,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   346,   347,   348,   349,   350,   351,
     352,    77,    78,    79,    80,   353,   424,   355,   356,   357,
     358,   425,   360,   361,     0,     0,     0,   603,     0,   362,
      84,    85,     0,    86,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,   346,   347,   348,
     349,   350,   351,   352,     0,     0,     0,     0,   353,   354,
     355,   356,   357,   358,   359,   360,   361,     0,     0,     0,
     420,     0,   362,    48,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    70,     0,    48,     0,     0,     0,     0,     0,
       0,     0,     0,   346,   347,   348,   349,   350,   351,   352,
      77,    78,    79,    80,   353,   424,   355,   356,   357,   358,
     425,   360,   361,    70,     0,     0,   605,     0,   362,    84,
      85,     0,    86,    48,   346,   347,   348,   349,   350,   351,
     352,    77,    78,    79,    80,   353,   424,   355,   356,   357,
     358,   425,   360,   361,     0,     0,     0,     0,     0,   362,
      84,    85,    70,    86,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,     0,     0,     0,     0,
      77,    78,    79,    80,     0,    81,     0,     0,     0,     0,
     188,     0,     0,     0,     0,     0,   189,     0,     0,    84,
      85,     0,    86,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,     0,     0,     0,     0,   156,
     157,   158,   159,   160,     6,     7,     8,     9,    10,    11,
       0,     0,     0,    48,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,   381,   382,   383,     0,
       0,     0,     0,     0,     0,     0,     0,   384,   385,   386,
     387,     0,    70,   388,   389,   390,     0,     0,     0,     0,
       0,     0,    48,     0,   561,     0,     0,   161,     0,     0,
      77,    78,    79,    80,     0,    81,   162,     0,     0,     0,
      82,     0,     0,     0,     0,     0,   259,     0,     0,    84,
      85,    70,    86,    48,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,     0,     0,     0,    77,
      78,    79,    80,     0,    81,     0,     0,     0,     0,    82,
       0,     0,    70,     0,     0,   418,   163,   164,    84,    85,
       0,    86,     0,     0,     0,     0,     0,     0,     0,     0,
      77,    78,    79,    80,     0,    81,     0,     0,     0,     0,
      82,     0,     0,     0,     0,     0,   435,     0,     0,    84,
      85,     0,    86,    48,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    70,     0,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,     0,     0,     0,     0,
      77,    78,    79,    80,     0,    81,     0,     0,     0,     0,
      82,     0,     0,     0,     0,     0,   464,     0,     0,    84,
      85,     0,    86,    48,     0,   167,   168,   169,   170,   171,
       6,     7,     8,     9,    10,    11,   172,   167,   168,   169,
     170,   171,     6,     7,     8,     9,    10,    11,   172,   381,
     382,   383,    70,    48,     0,     0,     0,     0,     0,     0,
     384,   385,   386,   387,     0,     0,   388,   389,   390,     0,
      77,    78,    79,    80,   391,    81,     0,     0,     0,     0,
      82,   397,    70,     0,     0,     0,   609,     0,     0,    84,
      85,     0,    86,     0,   173,     0,     0,     0,     0,     0,
      77,    78,    79,    80,     0,    81,   173,     0,     0,     0,
      82,     0,     0,     0,     0,     0,     0,     0,     0,    84,
      85,     0,    86,   174,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   174,     0,     0,     0,     0,
       0,   175,   176,   177,   178,     0,   179,     0,     0,     0,
       0,     0,     0,   175,   176,   177,   178,   180,   179,     0,
     181,   182,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   181,   182,   346,   347,   348,   349,   350,   351,
     352,     0,     0,     0,     0,   353,   354,   355,   356,   357,
     358,   359,   360,   361,     0,     0,     0,   398,   277,   362,
     278,   279,   280,     0,   281,   282,   283,   284,   285,   286,
     287,   288,   289,    27,    28,   194,   290,   291,   292,   293,
     294,     0,     0,     0,     0,     0,     0,   195,     0,     0,
     295,   296,   297,   298,     0,   196,   299,   300,   301,   302,
     303,     0,   197,   198,   199,   200,   201,   202,     0,   203,
     304,   305,     0,     0,     0,   306,   307,   308,   309,     0,
       0,   310,   311,   312,   313,   314,   315,   316,   317,   318,
       0,     0,   319,   320,   321,   322,     0,   323,     0,     0,
     324,     0,   325,   326,     0,   327,   328,   329,   330,   331,
       0,     0,     0,     0,     0,     0,   204,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     205,     0,     0,     0,     0,   332,   346,   347,   348,   349,
     350,   351,   352,     0,     0,     0,     0,   353,   354,   355,
     356,   357,   358,   359,   360,   361,     0,     0,     0,     0,
       0,   362,     0,     0,   530,   346,   347,   348,   349,   350,
     351,   352,     0,     0,     0,     0,   353,   354,   355,   356,
     357,   358,   359,   360,   361,     0,     0,     0,   434,     0,
     362,   346,   347,   348,   349,   350,   351,   352,     0,     0,
       0,     0,   353,   354,   355,   356,   357,   358,   359,   360,
     361,     0,     0,     0,   444,     0,   362,   346,   347,   348,
     349,   350,   351,   352,     0,     0,     0,     0,   353,   354,
     355,   356,   357,   358,   359,   360,   361,     0,     0,     0,
     449,     0,   362,   346,   347,   348,   349,   350,   351,   352,
       0,     0,     0,     0,   353,   354,   355,   356,   357,   358,
     359,   360,   361,     0,     0,     0,   450,     0,   362,   346,
     347,   348,   349,   350,   351,   352,     0,     0,     0,     0,
     353,   354,   355,   356,   357,   358,   359,   360,   361,     0,
       0,     0,   458,     0,   362,   346,   347,   348,   349,   350,
     351,   352,     0,     0,     0,     0,   353,   354,   355,   356,
     357,   358,   359,   360,   361,     0,     0,     0,   470,     0,
     362,   346,   347,   348,   349,   350,   351,   352,     0,     0,
       0,     0,   353,   354,   355,   356,   357,   358,   359,   360,
     361,     0,     0,     0,   471,     0,   362,   346,   347,   348,
     349,   350,   351,   352,     0,     0,     0,     0,   353,   354,
     355,   356,   357,   358,   359,   360,   361,     0,     0,     0,
     528,     0,   362,   346,   347,   348,   349,   350,   351,   352,
       0,     0,     0,     0,   353,   354,   355,   356,   357,   358,
     359,   360,   361,     0,     0,     0,   548,     0,   362,   346,
     347,   348,   349,   350,   351,   352,     0,     0,     0,     0,
     353,   354,   355,   356,   357,   358,   359,   360,   361,     0,
       0,     0,   587,     0,   362,   346,   347,   348,   349,   350,
     351,   352,     0,     0,     0,     0,   353,   354,   355,   356,
     357,   358,   359,   360,   361,     0,     0,     0,   590,     0,
     362,   346,   347,   348,   349,   350,   351,   352,     0,     0,
       0,     0,   353,   354,   355,   356,   357,   358,   359,   360,
     361,     0,     0,     0,   594,     0,   362,   346,   347,   348,
     349,   350,   351,   352,     0,     0,     0,     0,   353,   354,
     355,   356,   357,   358,   359,   360,   361,     0,     0,     0,
     599,     0,   362,   346,   347,   348,   349,   350,   351,   352,
       0,     0,     0,     0,   353,   354,   355,   356,   357,   358,
     359,   360,   361,     0,     0,     0,   600,     0,   362,   346,
     347,   348,   349,   350,   351,   352,     0,     0,     0,     0,
     353,   354,   355,   356,   357,   358,   359,   360,   361,     0,
       0,     0,   608,     0,   362,   346,   347,   348,   349,   350,
     351,   352,     0,     0,     0,     0,   353,   354,   355,   356,
     357,   358,   359,   360,   361,     0,     0,     0,   611,     0,
     362,   346,   347,   348,   349,   350,   351,   352,     0,     0,
       0,     0,   353,   354,   355,   356,   357,   358,   359,   360,
     361,     0,     0,     0,   612,     0,   362,   346,   347,   348,
     349,   350,   351,   352,     0,     0,     0,     0,   353,   354,
     355,   356,   357,   358,   359,   360,   361,     0,     0,     0,
     613,     0,   362,   346,   347,   348,   349,   350,   351,   352,
       0,     0,     0,     0,   353,   354,   355,   356,   357,   358,
     359,   360,   361,     0,     0,     0,   614,     0,   362,   346,
     347,   348,   349,   350,   351,   352,     0,     0,     0,     0,
     353,   354,   355,   356,   357,   358,   359,   360,   361,     0,
       0,     0,   615,     0,   362,   346,   347,   348,   349,   350,
     351,   352,     0,     0,     0,     0,   353,   354,   355,   356,
     357,   358,   359,   360,   361,     0,     0,     0,   616,     0,
     362,   346,   347,   348,   349,   350,   351,   352,     0,     0,
       0,     0,   353,   354,   355,   356,   357,   358,   359,   360,
     361,     0,     0,     0,   617,     0,   362,   346,   347,   348,
     349,   350,   351,   352,     0,     0,     0,     0,   353,   354,
     355,   356,   357,   358,   359,   360,   361,     0,     0,     0,
     618,     0,   362,   346,   347,   348,   349,   350,   351,   352,
       0,     0,     0,     0,   353,   354,   355,   356,   357,   358,
     359,   360,   361,     0,     0,     0,   619,     0,   362,   346,
     347,   348,   349,   350,   351,   352,     0,     0,     0,     0,
     353,   354,   355,   356,   357,   358,   359,   360,   361,     0,
       0,     0,   632,     0,   362,   346,   347,   348,   349,   350,
     351,   352,     0,     0,     0,     0,   353,   354,   355,   356,
     357,   358,   359,   360,   361,     0,     0,     0,   633,     0,
     362,   346,   347,   348,   349,   350,   351,   352,     0,     0,
       0,     0,   353,   354,   355,   356,   357,   358,   359,   360,
     361,     0,     0,     0,   634,     0,   362,   346,   347,   348,
     349,   350,   351,   352,     0,     0,     0,     0,   353,   354,
     355,   356,   357,   358,   359,   360,   361,     0,     0,     0,
     636,     0,   362,   346,   347,   348,   349,   350,   351,   352,
       0,     0,     0,     0,   353,   354,   355,   356,   357,   358,
     359,   360,   361,     0,     0,     0,   637,     0,   362,   346,
     347,   348,   349,   350,   351,   352,     0,     0,     0,     0,
     353,   354,   355,   356,   357,   358,   359,   360,   361,     0,
       0,     0,   638,     0,   362,   346,   347,   348,   349,   350,
     351,   352,     0,     0,     0,     0,   353,   354,   355,   356,
     357,   358,   359,   360,   361,     0,     0,     0,   639,     0,
     362,   346,   347,   348,   349,   350,   351,   352,     0,     0,
       0,     0,   353,   354,   355,   356,   357,   358,   359,   360,
     361,     0,     0,     0,     0,     0,   362
};

static const yytype_int16 yycheck[] =
{
       0,   179,    19,   181,   182,    19,    19,   198,   199,   200,
      48,   135,    72,    72,    14,    17,    72,    19,   101,   135,
      20,    21,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    52,    53,   101,    37,    18,    72,
      72,    41,    42,    72,    72,    45,    46,   104,   101,    49,
      50,   111,   135,    33,   101,    55,    56,    17,   101,   101,
     101,   135,    62,   101,    72,    72,    73,    16,    17,    69,
     135,    71,   135,    73,    55,   135,   135,   135,   135,   135,
     135,    81,    82,   135,    84,    85,    86,    87,    48,   135,
      98,    72,   135,    73,   135,   135,   135,   135,    79,    80,
     135,    72,   135,   135,   111,    72,   135,   135,    72,    63,
      90,    91,    72,    65,    66,    67,    68,    72,   135,   135,
     101,   135,   135,   137,   135,   135,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,   119,   120,
     121,   122,   135,   124,   162,   135,   135,   135,   129,   149,
     101,   111,   136,   136,   135,   135,   135,   138,   139,   136,
     141,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,   136,   136,   135,   135,   179,   136,   181,
     182,   112,   113,   114,    72,   101,   136,   135,   188,   136,
     136,   135,   135,   135,   135,    72,   135,    19,   129,   130,
     131,   135,   135,   381,   382,   383,   384,   385,   386,   387,
     388,   389,   390,   391,   405,   215,   407,   408,   409,   219,
     220,   221,   222,   135,   101,   135,   135,   135,   135,   229,
      72,   136,   135,   233,   135,   135,   135,   135,   238,   135,
     135,   135,   119,   120,   121,   122,   135,   124,   135,   135,
     135,    72,   129,   135,   254,   135,   256,   257,   135,   101,
     135,   138,   139,   135,   141,   135,   266,   135,   135,   135,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     112,   113,   114,   135,   135,   137,   138,   139,   135,   141,
      19,   123,   124,   125,   126,   135,   135,   129,   130,   131,
     135,   135,   135,   135,   115,   116,   117,   118,   115,   116,
     117,   118,   135,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,   135,   137,   135,   137,   135,
     137,   135,   135,   135,   135,   135,   346,   347,   348,   349,
     350,   351,   352,   353,   354,   355,   356,   357,   358,   359,
     360,   361,   362,   135,   135,   135,   135,   135,   368,   369,
     370,   371,   372,   373,    54,   375,   376,   377,   135,   381,
     382,   383,   384,   385,   386,   387,   388,   389,   390,   391,
     135,   135,    72,   112,   113,   114,   115,   116,   117,   118,
     135,   135,   135,   135,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   135,   135,   135,   135,   135,   137,   135,
     135,   101,   422,   135,   424,   425,   426,   135,   112,   113,
     114,   115,   116,   117,   118,   135,   436,   135,   135,   119,
     120,   121,   122,   135,   124,   129,   130,   131,   135,   129,
     135,    72,   135,   137,   454,   135,   456,   457,   138,   139,
     135,   141,   135,   135,     0,   135,   466,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,   135,
     135,   135,    18,    19,    20,    21,    22,   135,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,   135,
      36,    37,    38,    39,    40,   135,   135,   135,    72,    72,
     135,   135,   135,   135,    50,    51,    52,    53,   135,   135,
      56,    57,    58,    59,    60,    72,    62,   135,    64,   135,
     135,   135,   135,    69,    70,    71,    72,   135,    74,    75,
      76,    77,    78,   135,   135,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    72,   135,    92,    93,    94,    95,
     135,    97,    87,    99,   100,   101,   102,   103,    72,   105,
     106,   107,   108,   112,   113,   114,   115,   116,   117,   118,
      -1,    -1,    -1,   119,   120,   121,   122,    -1,   124,    -1,
     129,   130,   131,   129,    -1,    -1,    -1,    -1,   137,   135,
      -1,    -1,   138,   139,    -1,   141,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    18,    19,    20,    21,    22,    -1,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    36,
      37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    50,    51,    52,    53,    -1,    -1,    56,
      57,    58,    59,    60,    -1,    62,    -1,    64,    -1,    -1,
      -1,    -1,    69,    70,    71,    72,    -1,    74,    75,    76,
      77,    78,    -1,    -1,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    -1,    -1,    92,    93,    94,    95,    -1,
      97,    -1,    99,   100,   101,   102,   103,    -1,   105,   106,
     107,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   119,   120,   121,   122,    -1,   124,    -1,    -1,
      -1,    -1,   129,    -1,    -1,    -1,    -1,    -1,   135,    -1,
      -1,   138,   139,    -1,   141,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,   112,   113,
     114,   115,   116,   117,   118,    -1,    -1,    -1,    -1,   123,
     124,   125,   126,   127,   128,   129,   130,   131,    -1,    -1,
      -1,   135,    -1,   137,    72,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   101,    -1,    72,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   101,    -1,    -1,   135,    -1,   137,
     138,   139,    -1,   141,    72,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,    -1,    -1,    -1,   135,    -1,
     137,   138,   139,   101,   141,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,    -1,    -1,    -1,   135,    -1,   137,
     138,   139,    -1,   141,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,    -1,    -1,   123,   124,
     125,   126,   127,   128,   129,   130,   131,    -1,    -1,    -1,
     135,    -1,   137,    72,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   101,    -1,    72,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   101,    -1,    -1,   135,    -1,   137,   138,
     139,    -1,   141,    72,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,    -1,    -1,    -1,    -1,    -1,   137,
     138,   139,   101,   141,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
     119,   120,   121,   122,    -1,   124,    -1,    -1,    -1,    -1,
     129,    -1,    -1,    -1,    -1,    -1,   135,    -1,    -1,   138,
     139,    -1,   141,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      -1,    -1,    -1,    72,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,   112,   113,   114,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   123,   124,   125,
     126,    -1,   101,   129,   130,   131,    -1,    -1,    -1,    -1,
      -1,    -1,    72,    -1,   140,    -1,    -1,    61,    -1,    -1,
     119,   120,   121,   122,    -1,   124,    70,    -1,    -1,    -1,
     129,    -1,    -1,    -1,    -1,    -1,   135,    -1,    -1,   138,
     139,   101,   141,    72,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,   119,
     120,   121,   122,    -1,   124,    -1,    -1,    -1,    -1,   129,
      -1,    -1,   101,    -1,    -1,   135,   120,   121,   138,   139,
      -1,   141,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     119,   120,   121,   122,    -1,   124,    -1,    -1,    -1,    -1,
     129,    -1,    -1,    -1,    -1,    -1,   135,    -1,    -1,   138,
     139,    -1,   141,    72,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   101,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
     119,   120,   121,   122,    -1,   124,    -1,    -1,    -1,    -1,
     129,    -1,    -1,    -1,    -1,    -1,   135,    -1,    -1,   138,
     139,    -1,   141,    72,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,   112,
     113,   114,   101,    72,    -1,    -1,    -1,    -1,    -1,    -1,
     123,   124,   125,   126,    -1,    -1,   129,   130,   131,    -1,
     119,   120,   121,   122,   137,   124,    -1,    -1,    -1,    -1,
     129,    19,   101,    -1,    -1,    -1,   135,    -1,    -1,   138,
     139,    -1,   141,    -1,    72,    -1,    -1,    -1,    -1,    -1,
     119,   120,   121,   122,    -1,   124,    72,    -1,    -1,    -1,
     129,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   138,
     139,    -1,   141,   101,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   101,    -1,    -1,    -1,    -1,
      -1,   119,   120,   121,   122,    -1,   124,    -1,    -1,    -1,
      -1,    -1,    -1,   119,   120,   121,   122,   135,   124,    -1,
     138,   139,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   138,   139,   112,   113,   114,   115,   116,   117,
     118,    -1,    -1,    -1,    -1,   123,   124,   125,   126,   127,
     128,   129,   130,   131,    -1,    -1,    -1,   135,    18,   137,
      20,    21,    22,    -1,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    15,    36,    37,    38,    39,
      40,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,
      50,    51,    52,    53,    -1,    35,    56,    57,    58,    59,
      60,    -1,    42,    43,    44,    45,    46,    47,    -1,    49,
      70,    71,    -1,    -1,    -1,    75,    76,    77,    78,    -1,
      -1,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      -1,    -1,    92,    93,    94,    95,    -1,    97,    -1,    -1,
     100,    -1,   102,   103,    -1,   105,   106,   107,   108,   109,
      -1,    -1,    -1,    -1,    -1,    -1,    96,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     110,    -1,    -1,    -1,    -1,   135,   112,   113,   114,   115,
     116,   117,   118,    -1,    -1,    -1,    -1,   123,   124,   125,
     126,   127,   128,   129,   130,   131,    -1,    -1,    -1,    -1,
      -1,   137,    -1,    -1,   140,   112,   113,   114,   115,   116,
     117,   118,    -1,    -1,    -1,    -1,   123,   124,   125,   126,
     127,   128,   129,   130,   131,    -1,    -1,    -1,   135,    -1,
     137,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,    -1,   123,   124,   125,   126,   127,   128,   129,   130,
     131,    -1,    -1,    -1,   135,    -1,   137,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,    -1,    -1,   123,   124,
     125,   126,   127,   128,   129,   130,   131,    -1,    -1,    -1,
     135,    -1,   137,   112,   113,   114,   115,   116,   117,   118,
      -1,    -1,    -1,    -1,   123,   124,   125,   126,   127,   128,
     129,   130,   131,    -1,    -1,    -1,   135,    -1,   137,   112,
     113,   114,   115,   116,   117,   118,    -1,    -1,    -1,    -1,
     123,   124,   125,   126,   127,   128,   129,   130,   131,    -1,
      -1,    -1,   135,    -1,   137,   112,   113,   114,   115,   116,
     117,   118,    -1,    -1,    -1,    -1,   123,   124,   125,   126,
     127,   128,   129,   130,   131,    -1,    -1,    -1,   135,    -1,
     137,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,    -1,   123,   124,   125,   126,   127,   128,   129,   130,
     131,    -1,    -1,    -1,   135,    -1,   137,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,    -1,    -1,   123,   124,
     125,   126,   127,   128,   129,   130,   131,    -1,    -1,    -1,
     135,    -1,   137,   112,   113,   114,   115,   116,   117,   118,
      -1,    -1,    -1,    -1,   123,   124,   125,   126,   127,   128,
     129,   130,   131,    -1,    -1,    -1,   135,    -1,   137,   112,
     113,   114,   115,   116,   117,   118,    -1,    -1,    -1,    -1,
     123,   124,   125,   126,   127,   128,   129,   130,   131,    -1,
      -1,    -1,   135,    -1,   137,   112,   113,   114,   115,   116,
     117,   118,    -1,    -1,    -1,    -1,   123,   124,   125,   126,
     127,   128,   129,   130,   131,    -1,    -1,    -1,   135,    -1,
     137,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,    -1,   123,   124,   125,   126,   127,   128,   129,   130,
     131,    -1,    -1,    -1,   135,    -1,   137,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,    -1,    -1,   123,   124,
     125,   126,   127,   128,   129,   130,   131,    -1,    -1,    -1,
     135,    -1,   137,   112,   113,   114,   115,   116,   117,   118,
      -1,    -1,    -1,    -1,   123,   124,   125,   126,   127,   128,
     129,   130,   131,    -1,    -1,    -1,   135,    -1,   137,   112,
     113,   114,   115,   116,   117,   118,    -1,    -1,    -1,    -1,
     123,   124,   125,   126,   127,   128,   129,   130,   131,    -1,
      -1,    -1,   135,    -1,   137,   112,   113,   114,   115,   116,
     117,   118,    -1,    -1,    -1,    -1,   123,   124,   125,   126,
     127,   128,   129,   130,   131,    -1,    -1,    -1,   135,    -1,
     137,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,    -1,   123,   124,   125,   126,   127,   128,   129,   130,
     131,    -1,    -1,    -1,   135,    -1,   137,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,    -1,    -1,   123,   124,
     125,   126,   127,   128,   129,   130,   131,    -1,    -1,    -1,
     135,    -1,   137,   112,   113,   114,   115,   116,   117,   118,
      -1,    -1,    -1,    -1,   123,   124,   125,   126,   127,   128,
     129,   130,   131,    -1,    -1,    -1,   135,    -1,   137,   112,
     113,   114,   115,   116,   117,   118,    -1,    -1,    -1,    -1,
     123,   124,   125,   126,   127,   128,   129,   130,   131,    -1,
      -1,    -1,   135,    -1,   137,   112,   113,   114,   115,   116,
     117,   118,    -1,    -1,    -1,    -1,   123,   124,   125,   126,
     127,   128,   129,   130,   131,    -1,    -1,    -1,   135,    -1,
     137,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,    -1,   123,   124,   125,   126,   127,   128,   129,   130,
     131,    -1,    -1,    -1,   135,    -1,   137,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,    -1,    -1,   123,   124,
     125,   126,   127,   128,   129,   130,   131,    -1,    -1,    -1,
     135,    -1,   137,   112,   113,   114,   115,   116,   117,   118,
      -1,    -1,    -1,    -1,   123,   124,   125,   126,   127,   128,
     129,   130,   131,    -1,    -1,    -1,   135,    -1,   137,   112,
     113,   114,   115,   116,   117,   118,    -1,    -1,    -1,    -1,
     123,   124,   125,   126,   127,   128,   129,   130,   131,    -1,
      -1,    -1,   135,    -1,   137,   112,   113,   114,   115,   116,
     117,   118,    -1,    -1,    -1,    -1,   123,   124,   125,   126,
     127,   128,   129,   130,   131,    -1,    -1,    -1,   135,    -1,
     137,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,    -1,   123,   124,   125,   126,   127,   128,   129,   130,
     131,    -1,    -1,    -1,   135,    -1,   137,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,    -1,    -1,   123,   124,
     125,   126,   127,   128,   129,   130,   131,    -1,    -1,    -1,
     135,    -1,   137,   112,   113,   114,   115,   116,   117,   118,
      -1,    -1,    -1,    -1,   123,   124,   125,   126,   127,   128,
     129,   130,   131,    -1,    -1,    -1,   135,    -1,   137,   112,
     113,   114,   115,   116,   117,   118,    -1,    -1,    -1,    -1,
     123,   124,   125,   126,   127,   128,   129,   130,   131,    -1,
      -1,    -1,   135,    -1,   137,   112,   113,   114,   115,   116,
     117,   118,    -1,    -1,    -1,    -1,   123,   124,   125,   126,
     127,   128,   129,   130,   131,    -1,    -1,    -1,   135,    -1,
     137,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,    -1,   123,   124,   125,   126,   127,   128,   129,   130,
     131,    -1,    -1,    -1,    -1,    -1,   137
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
      87,    88,    89,    92,    93,    94,    95,    97,    99,   100,
     101,   102,   103,   105,   106,   107,   108,   119,   120,   121,
     122,   124,   129,   135,   138,   139,   141,   143,   144,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   207,    19,
     135,   207,    48,   101,   135,   135,     3,     4,     5,     6,
       7,    61,    70,   120,   121,   147,   135,     3,     4,     5,
       6,     7,    14,    72,   101,   119,   120,   121,   122,   124,
     135,   138,   139,   147,   206,    72,   135,   207,   129,   135,
     207,   101,   101,   101,    15,    27,    35,    42,    43,    44,
      45,    46,    47,    49,    96,   110,   135,   135,   135,   135,
     135,   135,   135,   135,   135,    54,   135,   207,    72,    72,
      72,   207,   207,    63,    65,    66,    67,    68,   207,    55,
      79,    80,   135,   207,    72,    73,   111,   207,   207,    16,
      17,   145,   145,   145,   135,   207,   207,   135,   135,   101,
     135,   101,   135,    18,    33,    73,    90,    91,   135,   135,
     207,    17,    48,    72,   111,   135,    72,    98,    72,   135,
      72,   135,    72,   135,   135,   207,   207,    18,    20,    21,
      22,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      36,    37,    38,    39,    40,    50,    51,    52,    53,    56,
      57,    58,    59,    60,    70,    71,    75,    76,    77,    78,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    92,
      93,    94,    95,    97,   100,   102,   103,   105,   106,   107,
     108,   109,   135,   146,   207,    72,   135,   135,   207,   207,
     207,   207,   207,     0,   144,   135,   112,   113,   114,   115,
     116,   117,   118,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   137,   207,   135,   101,   101,   135,   136,   136,
     136,   136,   136,   136,   145,   136,   136,   136,   206,   206,
     206,   112,   113,   114,   123,   124,   125,   126,   129,   130,
     131,   137,    19,   135,    19,   135,   207,    19,   135,   135,
     135,   135,   135,   135,   135,   174,   207,   174,   174,   174,
     135,   135,   135,    72,   135,    72,   111,   135,   135,   207,
     135,    72,   207,   207,   124,   129,   207,   207,   135,   101,
     135,   135,   135,   135,   135,   135,   207,   135,   136,   135,
     207,   135,   135,   135,   135,   207,   135,   135,   135,   135,
     135,   135,   135,   135,   207,   135,   207,   207,   135,   135,
     135,   104,   135,   135,   135,   207,    72,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     140,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   135,   135,
     135,   207,   207,   207,   207,   207,   207,   135,   207,   207,
     207,   140,   206,   206,   206,   206,   206,   206,   206,   206,
     206,   206,   206,    72,    72,    19,   135,    72,   174,   174,
     174,   174,   135,    72,   135,    72,   135,   135,   135,   207,
     135,   207,   207,   207,   135,   135,   135,   207,   101,   135,
     135,   135,   207,   135,   207,   135,   207,   135,   135,   135,
     207,   135,   135,   135,   135,   135,   135,   135,   135,   135,
      19,   135,   135,   135,    72,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
      72,   135,   135
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   142,   143,   143,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   145,   145,   146,   146,   147,   147,
     147,   147,   147,   147,   148,   148,   149,   150,   151,   151,
     151,   151,   151,   151,   152,   153,   154,   155,   156,   157,
     158,   158,   159,   159,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   161,   161,   161,   161,   162,
     162,   163,   163,   164,   165,   166,   166,   167,   167,   167,
     167,   168,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   171,   172,   172,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   174,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   188,   188,   188,   189,   190,   191,   192,   193,   194,
     195,   195,   195,   195,   195,   196,   196,   196,   196,   196,
     196,   196,   196,   197,   197,   197,   198,   199,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   201,   202,   203,   204,   205,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   206,
     206,   206,   206,   206,   206,   206,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207
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
       1,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     2,     2,     3,     3,
       3,     3,     4,     2,     3,     3,     2,     3,     3,     3,
       2,     3,     2,     3,     3,     3,     2,     4,     4,     4,
       5,     5,     5,     2,     3,     3,     4,     4,     5,     2,
       3,     2,     3,     2,     3,     2,     4,     2,     3,     4,
       4,     2,     4,     5,     5,     5,     5,     5,     5,     5,
       5,     5,     2,     5,     7,     2,     3,     5,     3,     5,
       2,     3,     5,     4,     6,     2,     2,     3,     3,     3,
       5,     5,     5,     5,     3,     3,     3,     3,     3,     4,
       3,     4,     5,     4,     5,     0,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     3,     3,     3,     2,
       4,     3,     3,     2,     4,     5,     4,     5,     4,     3,
       3,     4,     3,     3,     3,     2,     3,     4,     3,     4,
       5,     3,     5,     3,     3,     3,     3,     4,     3,     3,
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
  case 61: /* command: expression  */
#line 220 "parser.y"
                 { eval_value = (yyvsp[0].uval); }
#line 2451 "y.tab.c"
    break;

  case 63: /* command: '\n'  */
#line 223 "parser.y"
      {
      }
#line 2458 "y.tab.c"
    break;

  case 65: /* BX_TOKEN_TOGGLE_ON_OFF: BX_TOKEN_OFF  */
#line 230 "parser.y"
    { (yyval.bval)=(yyvsp[0].bval); }
#line 2464 "y.tab.c"
    break;

  case 67: /* BX_TOKEN_REGISTERS: BX_TOKEN_REGS  */
#line 236 "parser.y"
    { (yyval.sval)=(yyvsp[0].sval); }
#line 2470 "y.tab.c"
    break;

  case 73: /* BX_TOKEN_SEGREG: BX_TOKEN_GS  */
#line 246 "parser.y"
    { (yyval.uval)=(yyvsp[0].uval); }
#line 2476 "y.tab.c"
    break;

  case 74: /* timebp_command: BX_TOKEN_TIMEBP expression '\n'  */
#line 251 "parser.y"
      {
          bx_dbg_timebp_command(0, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2485 "y.tab.c"
    break;

  case 75: /* timebp_command: BX_TOKEN_TIMEBP_ABSOLUTE expression '\n'  */
#line 256 "parser.y"
      {
          bx_dbg_timebp_command(1, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2494 "y.tab.c"
    break;

  case 76: /* modebp_command: BX_TOKEN_MODEBP '\n'  */
#line 264 "parser.y"
      {
          bx_dbg_modebp_command();
          free((yyvsp[-1].sval));
      }
#line 2503 "y.tab.c"
    break;

  case 77: /* vmexitbp_command: BX_TOKEN_VMEXITBP '\n'  */
#line 272 "parser.y"
      {
          bx_dbg_vmexitbp_command();
          free((yyvsp[-1].sval));
      }
#line 2512 "y.tab.c"
    break;

  case 78: /* show_command: BX_TOKEN_SHOW BX_TOKEN_GENERIC '\n'  */
#line 280 "parser.y"
      {
          bx_dbg_show_command((yyvsp[-1].sval));
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2521 "y.tab.c"
    break;

  case 79: /* show_command: BX_TOKEN_SHOW BX_TOKEN_ALL '\n'  */
#line 285 "parser.y"
      {
          bx_dbg_show_command("all");
          free((yyvsp[-2].sval));
      }
#line 2530 "y.tab.c"
    break;

  case 80: /* show_command: BX_TOKEN_SHOW BX_TOKEN_OFF '\n'  */
#line 290 "parser.y"
      {
          bx_dbg_show_command("off");
          free((yyvsp[-2].sval));
      }
#line 2539 "y.tab.c"
    break;

  case 81: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING '\n'  */
#line 295 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-1].sval), 0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2548 "y.tab.c"
    break;

  case 82: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING BX_TOKEN_XML '\n'  */
#line 300 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-2].sval), 1);
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2557 "y.tab.c"
    break;

  case 83: /* show_command: BX_TOKEN_SHOW '\n'  */
#line 305 "parser.y"
      {
          bx_dbg_show_command(0);
          free((yyvsp[-1].sval));
      }
#line 2566 "y.tab.c"
    break;

  case 84: /* page_command: BX_TOKEN_PAGE expression '\n'  */
#line 313 "parser.y"
      {
          bx_dbg_xlate_address((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2575 "y.tab.c"
    break;

  case 85: /* tlb_command: BX_TOKEN_TLB expression '\n'  */
#line 321 "parser.y"
      {
          bx_dbg_tlb_lookup((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2584 "y.tab.c"
    break;

  case 86: /* ptime_command: BX_TOKEN_PTIME '\n'  */
#line 329 "parser.y"
      {
          bx_dbg_ptime_command();
          free((yyvsp[-1].sval));
      }
#line 2593 "y.tab.c"
    break;

  case 87: /* trace_command: BX_TOKEN_TRACE BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 337 "parser.y"
      {
          bx_dbg_trace_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2602 "y.tab.c"
    break;

  case 88: /* trace_reg_command: BX_TOKEN_TRACEREG BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 345 "parser.y"
      {
          bx_dbg_trace_reg_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2611 "y.tab.c"
    break;

  case 89: /* trace_mem_command: BX_TOKEN_TRACEMEM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 353 "parser.y"
      {
          bx_dbg_trace_mem_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2620 "y.tab.c"
    break;

  case 90: /* print_stack_command: BX_TOKEN_PRINT_STACK '\n'  */
#line 361 "parser.y"
      {
          bx_dbg_print_stack_command(16);
          free((yyvsp[-1].sval));
      }
#line 2629 "y.tab.c"
    break;

  case 91: /* print_stack_command: BX_TOKEN_PRINT_STACK BX_TOKEN_NUMERIC '\n'  */
#line 366 "parser.y"
      {
          bx_dbg_print_stack_command((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2638 "y.tab.c"
    break;

  case 92: /* backtrace_command: BX_TOKEN_BT '\n'  */
#line 374 "parser.y"
      {
        bx_dbg_bt_command(16);
        free((yyvsp[-1].sval));
      }
#line 2647 "y.tab.c"
    break;

  case 93: /* backtrace_command: BX_TOKEN_BT BX_TOKEN_NUMERIC '\n'  */
#line 379 "parser.y"
      {
        bx_dbg_bt_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2656 "y.tab.c"
    break;

  case 94: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_STOP '\n'  */
#line 387 "parser.y"
      {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2665 "y.tab.c"
    break;

  case 95: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_CONTINUE '\n'  */
#line 392 "parser.y"
      {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2674 "y.tab.c"
    break;

  case 96: /* watch_point_command: BX_TOKEN_WATCH '\n'  */
#line 397 "parser.y"
      {
          bx_dbg_print_watchpoints();
          free((yyvsp[-1].sval));
      }
#line 2683 "y.tab.c"
    break;

  case 97: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression '\n'  */
#line 402 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2692 "y.tab.c"
    break;

  case 98: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression '\n'  */
#line 407 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2701 "y.tab.c"
    break;

  case 99: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression '\n'  */
#line 412 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-1].uval), 1); /* BX_WRITE */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2710 "y.tab.c"
    break;

  case 100: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression expression '\n'  */
#line 417 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2719 "y.tab.c"
    break;

  case 101: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression expression '\n'  */
#line 422 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2728 "y.tab.c"
    break;

  case 102: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression expression '\n'  */
#line 427 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_WRITE */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2737 "y.tab.c"
    break;

  case 103: /* watch_point_command: BX_TOKEN_UNWATCH '\n'  */
#line 432 "parser.y"
      {
          bx_dbg_unwatch_all();
          free((yyvsp[-1].sval));
      }
#line 2746 "y.tab.c"
    break;

  case 104: /* watch_point_command: BX_TOKEN_UNWATCH expression '\n'  */
#line 437 "parser.y"
      {
          bx_dbg_unwatch((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2755 "y.tab.c"
    break;

  case 105: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 445 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2764 "y.tab.c"
    break;

  case 106: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING expression '\n'  */
#line 450 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 0, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2773 "y.tab.c"
    break;

  case 107: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING '\n'  */
#line 455 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 1, 0);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2782 "y.tab.c"
    break;

  case 108: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING expression '\n'  */
#line 460 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 1, (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2791 "y.tab.c"
    break;

  case 109: /* set_magic_break_points_command: BX_TOKEN_SET_MAGIC_BREAK_POINTS '\n'  */
#line 468 "parser.y"
      {
        bx_dbg_set_magic_bp_mask(0);
        free((yyvsp[-1].sval));
      }
#line 2800 "y.tab.c"
    break;

  case 110: /* set_magic_break_points_command: BX_TOKEN_SET_MAGIC_BREAK_POINTS BX_TOKEN_STRING '\n'  */
#line 473 "parser.y"
      {
        bx_dbg_set_magic_bp_mask(bx_dbg_get_magic_bp_mask_from_str((yyvsp[-1].sval)));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2809 "y.tab.c"
    break;

  case 111: /* clr_magic_break_points_command: BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS '\n'  */
#line 481 "parser.y"
      {
        bx_dbg_set_magic_bp_mask(0);
        free((yyvsp[-1].sval));
      }
#line 2818 "y.tab.c"
    break;

  case 112: /* clr_magic_break_points_command: BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS BX_TOKEN_STRING '\n'  */
#line 486 "parser.y"
      {
        bx_dbg_clr_magic_bp_mask(bx_dbg_get_magic_bp_mask_from_str((yyvsp[-1].sval)));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2827 "y.tab.c"
    break;

  case 113: /* where_command: BX_TOKEN_WHERE '\n'  */
#line 494 "parser.y"
      {
        bx_dbg_where_command();
        free((yyvsp[-1].sval));
      }
#line 2836 "y.tab.c"
    break;

  case 114: /* print_string_command: BX_TOKEN_PRINT_STRING expression '\n'  */
#line 502 "parser.y"
      {
        bx_dbg_print_string_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2845 "y.tab.c"
    break;

  case 115: /* continue_command: BX_TOKEN_CONTINUE '\n'  */
#line 510 "parser.y"
      {
        bx_dbg_continue_command(1);
        free((yyvsp[-1].sval));
      }
#line 2854 "y.tab.c"
    break;

  case 116: /* continue_command: BX_TOKEN_CONTINUE BX_TOKEN_IF expression '\n'  */
#line 515 "parser.y"
      {
        bx_dbg_continue_command((yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2863 "y.tab.c"
    break;

  case 117: /* stepN_command: BX_TOKEN_STEPN '\n'  */
#line 523 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[-1].sval));
      }
#line 2872 "y.tab.c"
    break;

  case 118: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC '\n'  */
#line 528 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2881 "y.tab.c"
    break;

  case 119: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_ALL BX_TOKEN_NUMERIC '\n'  */
#line 533 "parser.y"
      {
        bx_dbg_stepN_command(-1, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2890 "y.tab.c"
    break;

  case 120: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'  */
#line 538 "parser.y"
      {
        bx_dbg_stepN_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2899 "y.tab.c"
    break;

  case 121: /* step_over_command: BX_TOKEN_STEP_OVER '\n'  */
#line 546 "parser.y"
      {
        bx_dbg_step_over_command();
        free((yyvsp[-1].sval));
      }
#line 2908 "y.tab.c"
    break;

  case 122: /* set_command: BX_TOKEN_SET BX_TOKEN_DISASM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 554 "parser.y"
      {
        bx_dbg_set_auto_disassemble((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2917 "y.tab.c"
    break;

  case 123: /* set_command: BX_TOKEN_SET BX_TOKEN_SYMBOLNAME '=' expression '\n'  */
#line 559 "parser.y"
      {
        bx_dbg_set_symbol_command((yyvsp[-3].sval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2926 "y.tab.c"
    break;

  case 124: /* set_command: BX_TOKEN_SET BX_TOKEN_8BL_REG '=' expression '\n'  */
#line 564 "parser.y"
      {
        bx_dbg_set_reg8l_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2934 "y.tab.c"
    break;

  case 125: /* set_command: BX_TOKEN_SET BX_TOKEN_8BH_REG '=' expression '\n'  */
#line 568 "parser.y"
      {
        bx_dbg_set_reg8h_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2942 "y.tab.c"
    break;

  case 126: /* set_command: BX_TOKEN_SET BX_TOKEN_16B_REG '=' expression '\n'  */
#line 572 "parser.y"
      {
        bx_dbg_set_reg16_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2950 "y.tab.c"
    break;

  case 127: /* set_command: BX_TOKEN_SET BX_TOKEN_32B_REG '=' expression '\n'  */
#line 576 "parser.y"
      {
        bx_dbg_set_reg32_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2958 "y.tab.c"
    break;

  case 128: /* set_command: BX_TOKEN_SET BX_TOKEN_64B_REG '=' expression '\n'  */
#line 580 "parser.y"
      {
        bx_dbg_set_reg64_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2966 "y.tab.c"
    break;

  case 129: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_EIP '=' expression '\n'  */
#line 584 "parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2974 "y.tab.c"
    break;

  case 130: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_RIP '=' expression '\n'  */
#line 588 "parser.y"
      {
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2982 "y.tab.c"
    break;

  case 131: /* set_command: BX_TOKEN_SET BX_TOKEN_SEGREG '=' expression '\n'  */
#line 592 "parser.y"
      {
        bx_dbg_load_segreg((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2990 "y.tab.c"
    break;

  case 132: /* breakpoint_command: BX_TOKEN_VBREAKPOINT '\n'  */
#line 599 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2999 "y.tab.c"
    break;

  case 133: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression '\n'  */
#line 604 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].uval), NULL);
        free((yyvsp[-4].sval));
      }
#line 3008 "y.tab.c"
    break;

  case 134: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 609 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-5].uval), (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-6].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3017 "y.tab.c"
    break;

  case 135: /* breakpoint_command: BX_TOKEN_LBREAKPOINT '\n'  */
#line 614 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3026 "y.tab.c"
    break;

  case 136: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression '\n'  */
#line 619 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 3035 "y.tab.c"
    break;

  case 137: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 624 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3044 "y.tab.c"
    break;

  case 138: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING '\n'  */
#line 629 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-1].sval), NULL);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3053 "y.tab.c"
    break;

  case 139: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 634 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-3].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3062 "y.tab.c"
    break;

  case 140: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '\n'  */
#line 639 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 3071 "y.tab.c"
    break;

  case 141: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression '\n'  */
#line 644 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 3080 "y.tab.c"
    break;

  case 142: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 649 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3089 "y.tab.c"
    break;

  case 143: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression '\n'  */
#line 654 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-3].sval));
      }
#line 3098 "y.tab.c"
    break;

  case 144: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 659 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-5].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3107 "y.tab.c"
    break;

  case 145: /* blist_command: BX_TOKEN_LIST_BREAK '\n'  */
#line 667 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-1].sval));
      }
#line 3116 "y.tab.c"
    break;

  case 146: /* slist_command: BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 675 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-1].sval));
      }
#line 3125 "y.tab.c"
    break;

  case 147: /* slist_command: BX_TOKEN_LIST_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 680 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 3134 "y.tab.c"
    break;

  case 148: /* info_command: BX_TOKEN_INFO BX_TOKEN_PBREAKPOINT '\n'  */
#line 688 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3143 "y.tab.c"
    break;

  case 149: /* info_command: BX_TOKEN_INFO BX_TOKEN_CPU '\n'  */
#line 693 "parser.y"
      {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3152 "y.tab.c"
    break;

  case 150: /* info_command: BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric optional_numeric '\n'  */
#line 698 "parser.y"
      {
        bx_dbg_info_idt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3161 "y.tab.c"
    break;

  case 151: /* info_command: BX_TOKEN_INFO BX_TOKEN_IVT optional_numeric optional_numeric '\n'  */
#line 703 "parser.y"
      {
        bx_dbg_info_ivt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3170 "y.tab.c"
    break;

  case 152: /* info_command: BX_TOKEN_INFO BX_TOKEN_GDT optional_numeric optional_numeric '\n'  */
#line 708 "parser.y"
      {
        bx_dbg_info_gdt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3179 "y.tab.c"
    break;

  case 153: /* info_command: BX_TOKEN_INFO BX_TOKEN_LDT optional_numeric optional_numeric '\n'  */
#line 713 "parser.y"
      {
        bx_dbg_info_ldt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3188 "y.tab.c"
    break;

  case 154: /* info_command: BX_TOKEN_INFO BX_TOKEN_TAB '\n'  */
#line 718 "parser.y"
      {
        bx_dbg_dump_table();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3197 "y.tab.c"
    break;

  case 155: /* info_command: BX_TOKEN_INFO BX_TOKEN_TSS '\n'  */
#line 723 "parser.y"
      {
        bx_dbg_info_tss_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3206 "y.tab.c"
    break;

  case 156: /* info_command: BX_TOKEN_INFO BX_TOKEN_FLAGS '\n'  */
#line 728 "parser.y"
      {
        bx_dbg_info_flags();
        free((yyvsp[-2].sval));
      }
#line 3215 "y.tab.c"
    break;

  case 157: /* info_command: BX_TOKEN_INFO BX_TOKEN_LINUX '\n'  */
#line 733 "parser.y"
      {
        bx_dbg_info_linux_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3224 "y.tab.c"
    break;

  case 158: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS '\n'  */
#line 738 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3233 "y.tab.c"
    break;

  case 159: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 743 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3242 "y.tab.c"
    break;

  case 160: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE '\n'  */
#line 748 "parser.y"
      {
        bx_dbg_info_device("", "");
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3251 "y.tab.c"
    break;

  case 161: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC '\n'  */
#line 753 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3260 "y.tab.c"
    break;

  case 162: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC BX_TOKEN_STRING '\n'  */
#line 758 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3269 "y.tab.c"
    break;

  case 163: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING '\n'  */
#line 763 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3278 "y.tab.c"
    break;

  case 164: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 768 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3287 "y.tab.c"
    break;

  case 165: /* optional_numeric: %empty  */
#line 775 "parser.y"
               { (yyval.uval) = EMPTY_ARG; }
#line 3293 "y.tab.c"
    break;

  case 167: /* regs_command: BX_TOKEN_REGISTERS '\n'  */
#line 780 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3302 "y.tab.c"
    break;

  case 168: /* fpu_regs_command: BX_TOKEN_FPU '\n'  */
#line 788 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[-1].sval));
      }
#line 3311 "y.tab.c"
    break;

  case 169: /* mmx_regs_command: BX_TOKEN_MMX '\n'  */
#line 796 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3320 "y.tab.c"
    break;

  case 170: /* xmm_regs_command: BX_TOKEN_XMM '\n'  */
#line 804 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3329 "y.tab.c"
    break;

  case 171: /* ymm_regs_command: BX_TOKEN_YMM '\n'  */
#line 812 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3338 "y.tab.c"
    break;

  case 172: /* zmm_regs_command: BX_TOKEN_ZMM '\n'  */
#line 820 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3347 "y.tab.c"
    break;

  case 173: /* segment_regs_command: BX_TOKEN_SEGMENT_REGS '\n'  */
#line 828 "parser.y"
      {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3356 "y.tab.c"
    break;

  case 174: /* control_regs_command: BX_TOKEN_CONTROL_REGS '\n'  */
#line 836 "parser.y"
      {
        bx_dbg_info_control_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3365 "y.tab.c"
    break;

  case 175: /* debug_regs_command: BX_TOKEN_DEBUG_REGS '\n'  */
#line 844 "parser.y"
      {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3374 "y.tab.c"
    break;

  case 176: /* delete_command: BX_TOKEN_DEL_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 852 "parser.y"
      {
        bx_dbg_del_breakpoint_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3383 "y.tab.c"
    break;

  case 177: /* bpe_command: BX_TOKEN_ENABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 860 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3392 "y.tab.c"
    break;

  case 178: /* bpd_command: BX_TOKEN_DISABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 867 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 0);
        free((yyvsp[-2].sval));
      }
#line 3401 "y.tab.c"
    break;

  case 179: /* quit_command: BX_TOKEN_QUIT '\n'  */
#line 875 "parser.y"
      {
        bx_dbg_quit_command();
        free((yyvsp[-1].sval));
      }
#line 3410 "y.tab.c"
    break;

  case 180: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT expression '\n'  */
#line 883 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-3].sval), (yyvsp[-2].sval),1, (yyvsp[-1].uval), 1);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3419 "y.tab.c"
    break;

  case 181: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT '\n'  */
#line 888 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), (yyvsp[-1].sval),1, 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3428 "y.tab.c"
    break;

  case 182: /* examine_command: BX_TOKEN_EXAMINE expression '\n'  */
#line 893 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), NULL,0, (yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3437 "y.tab.c"
    break;

  case 183: /* examine_command: BX_TOKEN_EXAMINE '\n'  */
#line 898 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-1].sval), NULL,0, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 3446 "y.tab.c"
    break;

  case 184: /* restore_command: BX_TOKEN_RESTORE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 906 "parser.y"
      {
        bx_dbg_restore_command((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3455 "y.tab.c"
    break;

  case 185: /* writemem_command: BX_TOKEN_WRITEMEM BX_TOKEN_STRING expression expression '\n'  */
#line 914 "parser.y"
      {
        bx_dbg_writemem_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3464 "y.tab.c"
    break;

  case 186: /* loadmem_command: BX_TOKEN_LOADMEM BX_TOKEN_STRING expression '\n'  */
#line 922 "parser.y"
      {
        bx_dbg_loadmem_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3473 "y.tab.c"
    break;

  case 187: /* setpmem_command: BX_TOKEN_SETPMEM expression expression expression '\n'  */
#line 930 "parser.y"
      {
        bx_dbg_setpmem_command((yyvsp[-3].uval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3482 "y.tab.c"
    break;

  case 188: /* deref_command: BX_TOKEN_DEREF expression expression '\n'  */
#line 938 "parser.y"
      {
        bx_dbg_deref_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3491 "y.tab.c"
    break;

  case 189: /* query_command: BX_TOKEN_QUERY BX_TOKEN_PENDING '\n'  */
#line 946 "parser.y"
      {
        bx_dbg_query_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3500 "y.tab.c"
    break;

  case 190: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA '\n'  */
#line 954 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3509 "y.tab.c"
    break;

  case 191: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA BX_TOKEN_NUMERIC '\n'  */
#line 959 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3518 "y.tab.c"
    break;

  case 192: /* take_command: BX_TOKEN_TAKE BX_TOKEN_IRQ '\n'  */
#line 964 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3527 "y.tab.c"
    break;

  case 193: /* take_command: BX_TOKEN_TAKE BX_TOKEN_SMI '\n'  */
#line 969 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3536 "y.tab.c"
    break;

  case 194: /* take_command: BX_TOKEN_TAKE BX_TOKEN_NMI '\n'  */
#line 974 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3545 "y.tab.c"
    break;

  case 195: /* disassemble_command: BX_TOKEN_DISASM '\n'  */
#line 982 "parser.y"
      {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[-1].sval));
      }
#line 3554 "y.tab.c"
    break;

  case 196: /* disassemble_command: BX_TOKEN_DISASM expression '\n'  */
#line 987 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3563 "y.tab.c"
    break;

  case 197: /* disassemble_command: BX_TOKEN_DISASM expression expression '\n'  */
#line 992 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3572 "y.tab.c"
    break;

  case 198: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT '\n'  */
#line 997 "parser.y"
      {
        bx_dbg_disassemble_current((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3581 "y.tab.c"
    break;

  case 199: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression '\n'  */
#line 1002 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-2].sval), (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3590 "y.tab.c"
    break;

  case 200: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression expression '\n'  */
#line 1007 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3599 "y.tab.c"
    break;

  case 201: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SWITCH_MODE '\n'  */
#line 1012 "parser.y"
      {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3608 "y.tab.c"
    break;

  case 202: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SIZE '=' BX_TOKEN_NUMERIC '\n'  */
#line 1017 "parser.y"
      {
        bx_dbg_set_disassemble_size((yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3617 "y.tab.c"
    break;

  case 203: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STOP '\n'  */
#line 1025 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3626 "y.tab.c"
    break;

  case 204: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STRING '\n'  */
#line 1031 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3635 "y.tab.c"
    break;

  case 205: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_GENERIC '\n'  */
#line 1036 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3644 "y.tab.c"
    break;

  case 206: /* doit_command: BX_TOKEN_DOIT expression '\n'  */
#line 1044 "parser.y"
      {
        bx_dbg_doit_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3653 "y.tab.c"
    break;

  case 207: /* crc_command: BX_TOKEN_CRC expression expression '\n'  */
#line 1052 "parser.y"
      {
        bx_dbg_crc_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3662 "y.tab.c"
    break;

  case 208: /* help_command: BX_TOKEN_HELP BX_TOKEN_QUIT '\n'  */
#line 1060 "parser.y"
       {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3671 "y.tab.c"
    break;

  case 209: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTINUE '\n'  */
#line 1065 "parser.y"
       {
         dbg_printf("c|cont|continue - continue executing\n");
         dbg_printf("c|cont|continue if \"expression\" - continue executing only if expression is true\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3681 "y.tab.c"
    break;

  case 210: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEPN '\n'  */
#line 1071 "parser.y"
       {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3692 "y.tab.c"
    break;

  case 211: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEP_OVER '\n'  */
#line 1078 "parser.y"
       {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3701 "y.tab.c"
    break;

  case 212: /* help_command: BX_TOKEN_HELP BX_TOKEN_VBREAKPOINT '\n'  */
#line 1083 "parser.y"
       {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         dbg_printf("vb|vbreak <seg:offset> if \"expression\" - set a conditional virtual address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3711 "y.tab.c"
    break;

  case 213: /* help_command: BX_TOKEN_HELP BX_TOKEN_LBREAKPOINT '\n'  */
#line 1089 "parser.y"
       {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         dbg_printf("lb|lbreak <addr> if \"expression\" - set a conditional linear address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3721 "y.tab.c"
    break;

  case 214: /* help_command: BX_TOKEN_HELP BX_TOKEN_PBREAKPOINT '\n'  */
#line 1095 "parser.y"
       {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         dbg_printf("p|pb|break|pbreak <addr> if \"expression\" - set a conditional physical address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3731 "y.tab.c"
    break;

  case 215: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEL_BREAKPOINT '\n'  */
#line 1101 "parser.y"
       {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3740 "y.tab.c"
    break;

  case 216: /* help_command: BX_TOKEN_HELP BX_TOKEN_ENABLE_BREAKPOINT '\n'  */
#line 1106 "parser.y"
       {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3749 "y.tab.c"
    break;

  case 217: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISABLE_BREAKPOINT '\n'  */
#line 1111 "parser.y"
       {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3758 "y.tab.c"
    break;

  case 218: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_BREAK '\n'  */
#line 1116 "parser.y"
       {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3767 "y.tab.c"
    break;

  case 219: /* help_command: BX_TOKEN_HELP BX_TOKEN_MODEBP '\n'  */
#line 1121 "parser.y"
       {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3776 "y.tab.c"
    break;

  case 220: /* help_command: BX_TOKEN_HELP BX_TOKEN_VMEXITBP '\n'  */
#line 1126 "parser.y"
       {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3785 "y.tab.c"
    break;

  case 221: /* help_command: BX_TOKEN_HELP BX_TOKEN_CRC '\n'  */
#line 1131 "parser.y"
       {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3794 "y.tab.c"
    break;

  case 222: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACE '\n'  */
#line 1136 "parser.y"
       {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3804 "y.tab.c"
    break;

  case 223: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEREG '\n'  */
#line 1142 "parser.y"
       {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3814 "y.tab.c"
    break;

  case 224: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEMEM '\n'  */
#line 1148 "parser.y"
       {
         dbg_printf("trace-mem on  - print all memory accesses occurred during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3824 "y.tab.c"
    break;

  case 225: /* help_command: BX_TOKEN_HELP BX_TOKEN_RESTORE '\n'  */
#line 1154 "parser.y"
       {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3836 "y.tab.c"
    break;

  case 226: /* help_command: BX_TOKEN_HELP BX_TOKEN_PTIME '\n'  */
#line 1162 "parser.y"
       {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3845 "y.tab.c"
    break;

  case 227: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP '\n'  */
#line 1167 "parser.y"
       {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3854 "y.tab.c"
    break;

  case 228: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP_ABSOLUTE '\n'  */
#line 1172 "parser.y"
       {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3863 "y.tab.c"
    break;

  case 229: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STACK '\n'  */
#line 1177 "parser.y"
       {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3872 "y.tab.c"
    break;

  case 230: /* help_command: BX_TOKEN_HELP BX_TOKEN_BT '\n'  */
#line 1182 "parser.y"
       {
         dbg_printf("bt [num_entries] - prints backtrace\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3881 "y.tab.c"
    break;

  case 231: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOAD_SYMBOLS '\n'  */
#line 1187 "parser.y"
       {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3890 "y.tab.c"
    break;

  case 232: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET_MAGIC_BREAK_POINTS '\n'  */
#line 1192 "parser.y"
       {
         dbg_printf("setmagicbps \"cx dx bx sp bp si di\" - set new magic breakpoints. You can specify multiple at once. Using the setmagicbps command without any arguments will disable all of them\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3899 "y.tab.c"
    break;

  case 233: /* help_command: BX_TOKEN_HELP BX_TOKEN_CLEAR_MAGIC_BREAK_POINTS '\n'  */
#line 1197 "parser.y"
       {
         dbg_printf("clrmagicbps \"cx dx bx sp bp si di\" - clear magic breakpoints. You can specify multiple at once. Using the clrmagicbps command without any arguments will disable all of them\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3908 "y.tab.c"
    break;

  case 234: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 1202 "parser.y"
       {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3917 "y.tab.c"
    break;

  case 235: /* help_command: BX_TOKEN_HELP BX_TOKEN_REGISTERS '\n'  */
#line 1207 "parser.y"
       {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3926 "y.tab.c"
    break;

  case 236: /* help_command: BX_TOKEN_HELP BX_TOKEN_FPU '\n'  */
#line 1212 "parser.y"
       {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3935 "y.tab.c"
    break;

  case 237: /* help_command: BX_TOKEN_HELP BX_TOKEN_MMX '\n'  */
#line 1217 "parser.y"
       {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3944 "y.tab.c"
    break;

  case 238: /* help_command: BX_TOKEN_HELP BX_TOKEN_XMM '\n'  */
#line 1222 "parser.y"
       {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3953 "y.tab.c"
    break;

  case 239: /* help_command: BX_TOKEN_HELP BX_TOKEN_YMM '\n'  */
#line 1227 "parser.y"
       {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3962 "y.tab.c"
    break;

  case 240: /* help_command: BX_TOKEN_HELP BX_TOKEN_ZMM '\n'  */
#line 1232 "parser.y"
       {
         dbg_printf("zmm - print AVX-512 state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3971 "y.tab.c"
    break;

  case 241: /* help_command: BX_TOKEN_HELP BX_TOKEN_SEGMENT_REGS '\n'  */
#line 1237 "parser.y"
       {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3980 "y.tab.c"
    break;

  case 242: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTROL_REGS '\n'  */
#line 1242 "parser.y"
       {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3989 "y.tab.c"
    break;

  case 243: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEBUG_REGS '\n'  */
#line 1247 "parser.y"
       {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3998 "y.tab.c"
    break;

  case 244: /* help_command: BX_TOKEN_HELP BX_TOKEN_WRITEMEM '\n'  */
#line 1252 "parser.y"
       {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4007 "y.tab.c"
    break;

  case 245: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOADMEM '\n'  */
#line 1257 "parser.y"
       {
         dbg_printf("loadmem <filename> <laddr> - load file bytes to virtual memory starting from the linear address 'laddr'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4016 "y.tab.c"
    break;

  case 246: /* help_command: BX_TOKEN_HELP BX_TOKEN_SETPMEM '\n'  */
#line 1262 "parser.y"
       {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4025 "y.tab.c"
    break;

  case 247: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEREF '\n'  */
#line 1267 "parser.y"
       {
         dbg_printf("deref <addr> <deep> - pointer dereference. For example: get value of [[[rax]]] or ***rax: deref rax 3\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4034 "y.tab.c"
    break;

  case 248: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISASM '\n'  */
#line 1272 "parser.y"
       {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm hex on/off - control disasm offsets and displacements format\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4048 "y.tab.c"
    break;

  case 249: /* help_command: BX_TOKEN_HELP BX_TOKEN_WATCH '\n'  */
#line 1282 "parser.y"
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
#line 4063 "y.tab.c"
    break;

  case 250: /* help_command: BX_TOKEN_HELP BX_TOKEN_UNWATCH '\n'  */
#line 1293 "parser.y"
       {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4073 "y.tab.c"
    break;

  case 251: /* help_command: BX_TOKEN_HELP BX_TOKEN_EXAMINE '\n'  */
#line 1299 "parser.y"
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
#line 4090 "y.tab.c"
    break;

  case 252: /* help_command: BX_TOKEN_HELP BX_TOKEN_INSTRUMENT '\n'  */
#line 1312 "parser.y"
       {
         dbg_printf("instrument <command|\"string command\"> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command|\"string command\">\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4099 "y.tab.c"
    break;

  case 253: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET '\n'  */
#line 1317 "parser.y"
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
#line 4114 "y.tab.c"
    break;

  case 254: /* help_command: BX_TOKEN_HELP BX_TOKEN_PAGE '\n'  */
#line 1328 "parser.y"
       {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4123 "y.tab.c"
    break;

  case 255: /* help_command: BX_TOKEN_HELP BX_TOKEN_INFO '\n'  */
#line 1333 "parser.y"
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
#line 4143 "y.tab.c"
    break;

  case 256: /* help_command: BX_TOKEN_HELP BX_TOKEN_SHOW '\n'  */
#line 1349 "parser.y"
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
#line 4163 "y.tab.c"
    break;

  case 257: /* help_command: BX_TOKEN_HELP BX_TOKEN_CALC '\n'  */
#line 1365 "parser.y"
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
#line 4179 "y.tab.c"
    break;

  case 258: /* help_command: BX_TOKEN_HELP BX_TOKEN_ADDLYT '\n'  */
#line 1377 "parser.y"
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
#line 4195 "y.tab.c"
    break;

  case 259: /* help_command: BX_TOKEN_HELP BX_TOKEN_REMLYT '\n'  */
#line 1389 "parser.y"
       {
         dbg_printf("remlyt - stops debugger to execute the script file added previously with addlyt command.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4204 "y.tab.c"
    break;

  case 260: /* help_command: BX_TOKEN_HELP BX_TOKEN_LYT '\n'  */
#line 1394 "parser.y"
       {
         dbg_printf("lyt - cause debugger to execute script file added previously with addlyt command.\n");
         dbg_printf("    Use it as a refresh/context.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4214 "y.tab.c"
    break;

  case 261: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STRING '\n'  */
#line 1400 "parser.y"
       {
         dbg_printf("print-string <addr> - prints a null-ended string from a linear address.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4223 "y.tab.c"
    break;

  case 262: /* help_command: BX_TOKEN_HELP BX_TOKEN_SOURCE '\n'  */
#line 1405 "parser.y"
       {
         dbg_printf("source <file> - cause debugger to execute a script file.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4232 "y.tab.c"
    break;

  case 263: /* help_command: BX_TOKEN_HELP BX_TOKEN_HELP '\n'  */
#line 1410 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 4241 "y.tab.c"
    break;

  case 264: /* help_command: BX_TOKEN_HELP '\n'  */
#line 1415 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-1].sval));
       }
#line 4250 "y.tab.c"
    break;

  case 265: /* calc_command: BX_TOKEN_CALC expression '\n'  */
#line 1423 "parser.y"
   {
     eval_value = (yyvsp[-1].uval);
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4260 "y.tab.c"
    break;

  case 266: /* addlyt_command: BX_TOKEN_ADDLYT BX_TOKEN_STRING '\n'  */
#line 1432 "parser.y"
   {
     bx_dbg_addlyt((yyvsp[-1].sval));
     free((yyvsp[-2].sval));
     free((yyvsp[-1].sval));
   }
#line 4270 "y.tab.c"
    break;

  case 267: /* remlyt_command: BX_TOKEN_REMLYT '\n'  */
#line 1441 "parser.y"
   {
     bx_dbg_remlyt();
     free((yyvsp[-1].sval));
   }
#line 4279 "y.tab.c"
    break;

  case 268: /* lyt_command: BX_TOKEN_LYT '\n'  */
#line 1449 "parser.y"
   {
     bx_dbg_lyt();
     free((yyvsp[-1].sval));
   }
#line 4288 "y.tab.c"
    break;

  case 269: /* if_command: BX_TOKEN_IF expression '\n'  */
#line 1457 "parser.y"
   {
     eval_value = (yyvsp[-1].uval) != 0;
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4298 "y.tab.c"
    break;

  case 270: /* vexpression: BX_TOKEN_NUMERIC  */
#line 1466 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4304 "y.tab.c"
    break;

  case 271: /* vexpression: BX_TOKEN_STRING  */
#line 1467 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4310 "y.tab.c"
    break;

  case 272: /* vexpression: BX_TOKEN_8BL_REG  */
#line 1468 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4316 "y.tab.c"
    break;

  case 273: /* vexpression: BX_TOKEN_8BH_REG  */
#line 1469 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4322 "y.tab.c"
    break;

  case 274: /* vexpression: BX_TOKEN_16B_REG  */
#line 1470 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4328 "y.tab.c"
    break;

  case 275: /* vexpression: BX_TOKEN_32B_REG  */
#line 1471 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4334 "y.tab.c"
    break;

  case 276: /* vexpression: BX_TOKEN_64B_REG  */
#line 1472 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4340 "y.tab.c"
    break;

  case 277: /* vexpression: BX_TOKEN_OPMASK_REG  */
#line 1473 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4346 "y.tab.c"
    break;

  case 278: /* vexpression: BX_TOKEN_SEGREG  */
#line 1474 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4352 "y.tab.c"
    break;

  case 279: /* vexpression: BX_TOKEN_REG_IP  */
#line 1475 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4358 "y.tab.c"
    break;

  case 280: /* vexpression: BX_TOKEN_REG_EIP  */
#line 1476 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4364 "y.tab.c"
    break;

  case 281: /* vexpression: BX_TOKEN_REG_RIP  */
#line 1477 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4370 "y.tab.c"
    break;

  case 282: /* vexpression: BX_TOKEN_REG_SSP  */
#line 1478 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4376 "y.tab.c"
    break;

  case 283: /* vexpression: vexpression '+' vexpression  */
#line 1479 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4382 "y.tab.c"
    break;

  case 284: /* vexpression: vexpression '-' vexpression  */
#line 1480 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4388 "y.tab.c"
    break;

  case 285: /* vexpression: vexpression '*' vexpression  */
#line 1481 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4394 "y.tab.c"
    break;

  case 286: /* vexpression: vexpression '/' vexpression  */
#line 1482 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) / (yyvsp[0].uval); }
#line 4400 "y.tab.c"
    break;

  case 287: /* vexpression: vexpression BX_TOKEN_DEREF_CHR vexpression  */
#line 1483 "parser.y"
                                                { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 4406 "y.tab.c"
    break;

  case 288: /* vexpression: vexpression BX_TOKEN_RSHIFT vexpression  */
#line 1484 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4412 "y.tab.c"
    break;

  case 289: /* vexpression: vexpression BX_TOKEN_LSHIFT vexpression  */
#line 1485 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4418 "y.tab.c"
    break;

  case 290: /* vexpression: vexpression '|' vexpression  */
#line 1486 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4424 "y.tab.c"
    break;

  case 291: /* vexpression: vexpression '^' vexpression  */
#line 1487 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4430 "y.tab.c"
    break;

  case 292: /* vexpression: vexpression '&' vexpression  */
#line 1488 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4436 "y.tab.c"
    break;

  case 293: /* vexpression: '!' vexpression  */
#line 1489 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4442 "y.tab.c"
    break;

  case 294: /* vexpression: '-' vexpression  */
#line 1490 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4448 "y.tab.c"
    break;

  case 295: /* vexpression: '(' vexpression ')'  */
#line 1491 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4454 "y.tab.c"
    break;

  case 296: /* expression: BX_TOKEN_NUMERIC  */
#line 1497 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4460 "y.tab.c"
    break;

  case 297: /* expression: BX_TOKEN_STRING  */
#line 1498 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4466 "y.tab.c"
    break;

  case 298: /* expression: BX_TOKEN_8BL_REG  */
#line 1499 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4472 "y.tab.c"
    break;

  case 299: /* expression: BX_TOKEN_8BH_REG  */
#line 1500 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4478 "y.tab.c"
    break;

  case 300: /* expression: BX_TOKEN_16B_REG  */
#line 1501 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4484 "y.tab.c"
    break;

  case 301: /* expression: BX_TOKEN_32B_REG  */
#line 1502 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4490 "y.tab.c"
    break;

  case 302: /* expression: BX_TOKEN_64B_REG  */
#line 1503 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4496 "y.tab.c"
    break;

  case 303: /* expression: BX_TOKEN_OPMASK_REG  */
#line 1504 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4502 "y.tab.c"
    break;

  case 304: /* expression: BX_TOKEN_SEGREG  */
#line 1505 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4508 "y.tab.c"
    break;

  case 305: /* expression: BX_TOKEN_REG_IP  */
#line 1506 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4514 "y.tab.c"
    break;

  case 306: /* expression: BX_TOKEN_REG_EIP  */
#line 1507 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4520 "y.tab.c"
    break;

  case 307: /* expression: BX_TOKEN_REG_RIP  */
#line 1508 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4526 "y.tab.c"
    break;

  case 308: /* expression: BX_TOKEN_REG_SSP  */
#line 1509 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4532 "y.tab.c"
    break;

  case 309: /* expression: expression ':' expression  */
#line 1510 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[-2].uval), (yyvsp[0].uval)); }
#line 4538 "y.tab.c"
    break;

  case 310: /* expression: expression '+' expression  */
#line 1511 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4544 "y.tab.c"
    break;

  case 311: /* expression: expression '-' expression  */
#line 1512 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4550 "y.tab.c"
    break;

  case 312: /* expression: expression '*' expression  */
#line 1513 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4556 "y.tab.c"
    break;

  case 313: /* expression: expression '/' expression  */
#line 1514 "parser.y"
                                     { (yyval.uval) = ((yyvsp[0].uval) != 0) ? (yyvsp[-2].uval) / (yyvsp[0].uval) : 0; }
#line 4562 "y.tab.c"
    break;

  case 314: /* expression: expression BX_TOKEN_DEREF_CHR expression  */
#line 1515 "parser.y"
                                              { (yyval.uval) = bx_dbg_deref((yyvsp[-2].uval), (yyvsp[0].uval), NULL, NULL); }
#line 4568 "y.tab.c"
    break;

  case 315: /* expression: expression BX_TOKEN_RSHIFT expression  */
#line 1516 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4574 "y.tab.c"
    break;

  case 316: /* expression: expression BX_TOKEN_LSHIFT expression  */
#line 1517 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4580 "y.tab.c"
    break;

  case 317: /* expression: expression '|' expression  */
#line 1518 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4586 "y.tab.c"
    break;

  case 318: /* expression: expression '^' expression  */
#line 1519 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4592 "y.tab.c"
    break;

  case 319: /* expression: expression '&' expression  */
#line 1520 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4598 "y.tab.c"
    break;

  case 320: /* expression: expression '>' expression  */
#line 1521 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) > (yyvsp[0].uval); }
#line 4604 "y.tab.c"
    break;

  case 321: /* expression: expression '<' expression  */
#line 1522 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) < (yyvsp[0].uval); }
#line 4610 "y.tab.c"
    break;

  case 322: /* expression: expression BX_TOKEN_EQ expression  */
#line 1523 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) == (yyvsp[0].uval); }
#line 4616 "y.tab.c"
    break;

  case 323: /* expression: expression BX_TOKEN_NE expression  */
#line 1524 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) != (yyvsp[0].uval); }
#line 4622 "y.tab.c"
    break;

  case 324: /* expression: expression BX_TOKEN_LE expression  */
#line 1525 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) <= (yyvsp[0].uval); }
#line 4628 "y.tab.c"
    break;

  case 325: /* expression: expression BX_TOKEN_GE expression  */
#line 1526 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) >= (yyvsp[0].uval); }
#line 4634 "y.tab.c"
    break;

  case 326: /* expression: '!' expression  */
#line 1527 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4640 "y.tab.c"
    break;

  case 327: /* expression: '-' expression  */
#line 1528 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4646 "y.tab.c"
    break;

  case 328: /* expression: '*' expression  */
#line 1529 "parser.y"
                                     { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[0].uval)); }
#line 4652 "y.tab.c"
    break;

  case 329: /* expression: '@' expression  */
#line 1530 "parser.y"
                                     { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[0].uval)); }
#line 4658 "y.tab.c"
    break;

  case 330: /* expression: '(' expression ')'  */
#line 1531 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4664 "y.tab.c"
    break;


#line 4668 "y.tab.c"

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

#line 1534 "parser.y"

#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
