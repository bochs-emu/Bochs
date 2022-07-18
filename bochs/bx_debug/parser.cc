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
    BX_TOKEN_SETPMEM = 313,        /* BX_TOKEN_SETPMEM  */
    BX_TOKEN_SYMBOLNAME = 314,     /* BX_TOKEN_SYMBOLNAME  */
    BX_TOKEN_QUERY = 315,          /* BX_TOKEN_QUERY  */
    BX_TOKEN_PENDING = 316,        /* BX_TOKEN_PENDING  */
    BX_TOKEN_TAKE = 317,           /* BX_TOKEN_TAKE  */
    BX_TOKEN_DMA = 318,            /* BX_TOKEN_DMA  */
    BX_TOKEN_IRQ = 319,            /* BX_TOKEN_IRQ  */
    BX_TOKEN_SMI = 320,            /* BX_TOKEN_SMI  */
    BX_TOKEN_NMI = 321,            /* BX_TOKEN_NMI  */
    BX_TOKEN_TLB = 322,            /* BX_TOKEN_TLB  */
    BX_TOKEN_DISASM = 323,         /* BX_TOKEN_DISASM  */
    BX_TOKEN_INSTRUMENT = 324,     /* BX_TOKEN_INSTRUMENT  */
    BX_TOKEN_STRING = 325,         /* BX_TOKEN_STRING  */
    BX_TOKEN_STOP = 326,           /* BX_TOKEN_STOP  */
    BX_TOKEN_DOIT = 327,           /* BX_TOKEN_DOIT  */
    BX_TOKEN_CRC = 328,            /* BX_TOKEN_CRC  */
    BX_TOKEN_TRACE = 329,          /* BX_TOKEN_TRACE  */
    BX_TOKEN_TRACEREG = 330,       /* BX_TOKEN_TRACEREG  */
    BX_TOKEN_TRACEMEM = 331,       /* BX_TOKEN_TRACEMEM  */
    BX_TOKEN_SWITCH_MODE = 332,    /* BX_TOKEN_SWITCH_MODE  */
    BX_TOKEN_SIZE = 333,           /* BX_TOKEN_SIZE  */
    BX_TOKEN_PTIME = 334,          /* BX_TOKEN_PTIME  */
    BX_TOKEN_TIMEBP_ABSOLUTE = 335, /* BX_TOKEN_TIMEBP_ABSOLUTE  */
    BX_TOKEN_TIMEBP = 336,         /* BX_TOKEN_TIMEBP  */
    BX_TOKEN_MODEBP = 337,         /* BX_TOKEN_MODEBP  */
    BX_TOKEN_VMEXITBP = 338,       /* BX_TOKEN_VMEXITBP  */
    BX_TOKEN_PRINT_STACK = 339,    /* BX_TOKEN_PRINT_STACK  */
    BX_TOKEN_BT = 340,             /* BX_TOKEN_BT  */
    BX_TOKEN_WATCH = 341,          /* BX_TOKEN_WATCH  */
    BX_TOKEN_UNWATCH = 342,        /* BX_TOKEN_UNWATCH  */
    BX_TOKEN_READ = 343,           /* BX_TOKEN_READ  */
    BX_TOKEN_WRITE = 344,          /* BX_TOKEN_WRITE  */
    BX_TOKEN_SHOW = 345,           /* BX_TOKEN_SHOW  */
    BX_TOKEN_LOAD_SYMBOLS = 346,   /* BX_TOKEN_LOAD_SYMBOLS  */
    BX_TOKEN_SYMBOLS = 347,        /* BX_TOKEN_SYMBOLS  */
    BX_TOKEN_LIST_SYMBOLS = 348,   /* BX_TOKEN_LIST_SYMBOLS  */
    BX_TOKEN_GLOBAL = 349,         /* BX_TOKEN_GLOBAL  */
    BX_TOKEN_WHERE = 350,          /* BX_TOKEN_WHERE  */
    BX_TOKEN_PRINT_STRING = 351,   /* BX_TOKEN_PRINT_STRING  */
    BX_TOKEN_NUMERIC = 352,        /* BX_TOKEN_NUMERIC  */
    BX_TOKEN_PAGE = 353,           /* BX_TOKEN_PAGE  */
    BX_TOKEN_HELP = 354,           /* BX_TOKEN_HELP  */
    BX_TOKEN_XML = 355,            /* BX_TOKEN_XML  */
    BX_TOKEN_CALC = 356,           /* BX_TOKEN_CALC  */
    BX_TOKEN_DEVICE = 357,         /* BX_TOKEN_DEVICE  */
    BX_TOKEN_GENERIC = 358,        /* BX_TOKEN_GENERIC  */
    BX_TOKEN_RSHIFT = 359,         /* BX_TOKEN_RSHIFT  */
    BX_TOKEN_LSHIFT = 360,         /* BX_TOKEN_LSHIFT  */
    BX_TOKEN_EQ = 361,             /* BX_TOKEN_EQ  */
    BX_TOKEN_NE = 362,             /* BX_TOKEN_NE  */
    BX_TOKEN_LE = 363,             /* BX_TOKEN_LE  */
    BX_TOKEN_GE = 364,             /* BX_TOKEN_GE  */
    BX_TOKEN_REG_IP = 365,         /* BX_TOKEN_REG_IP  */
    BX_TOKEN_REG_EIP = 366,        /* BX_TOKEN_REG_EIP  */
    BX_TOKEN_REG_RIP = 367,        /* BX_TOKEN_REG_RIP  */
    BX_TOKEN_REG_SSP = 368,        /* BX_TOKEN_REG_SSP  */
    NOT = 369,                     /* NOT  */
    NEG = 370,                     /* NEG  */
    INDIRECT = 371                 /* INDIRECT  */
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
#define BX_TOKEN_SETPMEM 313
#define BX_TOKEN_SYMBOLNAME 314
#define BX_TOKEN_QUERY 315
#define BX_TOKEN_PENDING 316
#define BX_TOKEN_TAKE 317
#define BX_TOKEN_DMA 318
#define BX_TOKEN_IRQ 319
#define BX_TOKEN_SMI 320
#define BX_TOKEN_NMI 321
#define BX_TOKEN_TLB 322
#define BX_TOKEN_DISASM 323
#define BX_TOKEN_INSTRUMENT 324
#define BX_TOKEN_STRING 325
#define BX_TOKEN_STOP 326
#define BX_TOKEN_DOIT 327
#define BX_TOKEN_CRC 328
#define BX_TOKEN_TRACE 329
#define BX_TOKEN_TRACEREG 330
#define BX_TOKEN_TRACEMEM 331
#define BX_TOKEN_SWITCH_MODE 332
#define BX_TOKEN_SIZE 333
#define BX_TOKEN_PTIME 334
#define BX_TOKEN_TIMEBP_ABSOLUTE 335
#define BX_TOKEN_TIMEBP 336
#define BX_TOKEN_MODEBP 337
#define BX_TOKEN_VMEXITBP 338
#define BX_TOKEN_PRINT_STACK 339
#define BX_TOKEN_BT 340
#define BX_TOKEN_WATCH 341
#define BX_TOKEN_UNWATCH 342
#define BX_TOKEN_READ 343
#define BX_TOKEN_WRITE 344
#define BX_TOKEN_SHOW 345
#define BX_TOKEN_LOAD_SYMBOLS 346
#define BX_TOKEN_SYMBOLS 347
#define BX_TOKEN_LIST_SYMBOLS 348
#define BX_TOKEN_GLOBAL 349
#define BX_TOKEN_WHERE 350
#define BX_TOKEN_PRINT_STRING 351
#define BX_TOKEN_NUMERIC 352
#define BX_TOKEN_PAGE 353
#define BX_TOKEN_HELP 354
#define BX_TOKEN_XML 355
#define BX_TOKEN_CALC 356
#define BX_TOKEN_DEVICE 357
#define BX_TOKEN_GENERIC 358
#define BX_TOKEN_RSHIFT 359
#define BX_TOKEN_LSHIFT 360
#define BX_TOKEN_EQ 361
#define BX_TOKEN_NE 362
#define BX_TOKEN_LE 363
#define BX_TOKEN_GE 364
#define BX_TOKEN_REG_IP 365
#define BX_TOKEN_REG_EIP 366
#define BX_TOKEN_REG_RIP 367
#define BX_TOKEN_REG_SSP 368
#define NOT 369
#define NEG 370
#define INDIRECT 371

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 14 "parser.y"

  char    *sval;
  Bit64u   uval;
  unsigned bval;

#line 378 "y.tab.c"

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
  YYSYMBOL_BX_TOKEN_SETPMEM = 58,          /* BX_TOKEN_SETPMEM  */
  YYSYMBOL_BX_TOKEN_SYMBOLNAME = 59,       /* BX_TOKEN_SYMBOLNAME  */
  YYSYMBOL_BX_TOKEN_QUERY = 60,            /* BX_TOKEN_QUERY  */
  YYSYMBOL_BX_TOKEN_PENDING = 61,          /* BX_TOKEN_PENDING  */
  YYSYMBOL_BX_TOKEN_TAKE = 62,             /* BX_TOKEN_TAKE  */
  YYSYMBOL_BX_TOKEN_DMA = 63,              /* BX_TOKEN_DMA  */
  YYSYMBOL_BX_TOKEN_IRQ = 64,              /* BX_TOKEN_IRQ  */
  YYSYMBOL_BX_TOKEN_SMI = 65,              /* BX_TOKEN_SMI  */
  YYSYMBOL_BX_TOKEN_NMI = 66,              /* BX_TOKEN_NMI  */
  YYSYMBOL_BX_TOKEN_TLB = 67,              /* BX_TOKEN_TLB  */
  YYSYMBOL_BX_TOKEN_DISASM = 68,           /* BX_TOKEN_DISASM  */
  YYSYMBOL_BX_TOKEN_INSTRUMENT = 69,       /* BX_TOKEN_INSTRUMENT  */
  YYSYMBOL_BX_TOKEN_STRING = 70,           /* BX_TOKEN_STRING  */
  YYSYMBOL_BX_TOKEN_STOP = 71,             /* BX_TOKEN_STOP  */
  YYSYMBOL_BX_TOKEN_DOIT = 72,             /* BX_TOKEN_DOIT  */
  YYSYMBOL_BX_TOKEN_CRC = 73,              /* BX_TOKEN_CRC  */
  YYSYMBOL_BX_TOKEN_TRACE = 74,            /* BX_TOKEN_TRACE  */
  YYSYMBOL_BX_TOKEN_TRACEREG = 75,         /* BX_TOKEN_TRACEREG  */
  YYSYMBOL_BX_TOKEN_TRACEMEM = 76,         /* BX_TOKEN_TRACEMEM  */
  YYSYMBOL_BX_TOKEN_SWITCH_MODE = 77,      /* BX_TOKEN_SWITCH_MODE  */
  YYSYMBOL_BX_TOKEN_SIZE = 78,             /* BX_TOKEN_SIZE  */
  YYSYMBOL_BX_TOKEN_PTIME = 79,            /* BX_TOKEN_PTIME  */
  YYSYMBOL_BX_TOKEN_TIMEBP_ABSOLUTE = 80,  /* BX_TOKEN_TIMEBP_ABSOLUTE  */
  YYSYMBOL_BX_TOKEN_TIMEBP = 81,           /* BX_TOKEN_TIMEBP  */
  YYSYMBOL_BX_TOKEN_MODEBP = 82,           /* BX_TOKEN_MODEBP  */
  YYSYMBOL_BX_TOKEN_VMEXITBP = 83,         /* BX_TOKEN_VMEXITBP  */
  YYSYMBOL_BX_TOKEN_PRINT_STACK = 84,      /* BX_TOKEN_PRINT_STACK  */
  YYSYMBOL_BX_TOKEN_BT = 85,               /* BX_TOKEN_BT  */
  YYSYMBOL_BX_TOKEN_WATCH = 86,            /* BX_TOKEN_WATCH  */
  YYSYMBOL_BX_TOKEN_UNWATCH = 87,          /* BX_TOKEN_UNWATCH  */
  YYSYMBOL_BX_TOKEN_READ = 88,             /* BX_TOKEN_READ  */
  YYSYMBOL_BX_TOKEN_WRITE = 89,            /* BX_TOKEN_WRITE  */
  YYSYMBOL_BX_TOKEN_SHOW = 90,             /* BX_TOKEN_SHOW  */
  YYSYMBOL_BX_TOKEN_LOAD_SYMBOLS = 91,     /* BX_TOKEN_LOAD_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_SYMBOLS = 92,          /* BX_TOKEN_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_LIST_SYMBOLS = 93,     /* BX_TOKEN_LIST_SYMBOLS  */
  YYSYMBOL_BX_TOKEN_GLOBAL = 94,           /* BX_TOKEN_GLOBAL  */
  YYSYMBOL_BX_TOKEN_WHERE = 95,            /* BX_TOKEN_WHERE  */
  YYSYMBOL_BX_TOKEN_PRINT_STRING = 96,     /* BX_TOKEN_PRINT_STRING  */
  YYSYMBOL_BX_TOKEN_NUMERIC = 97,          /* BX_TOKEN_NUMERIC  */
  YYSYMBOL_BX_TOKEN_PAGE = 98,             /* BX_TOKEN_PAGE  */
  YYSYMBOL_BX_TOKEN_HELP = 99,             /* BX_TOKEN_HELP  */
  YYSYMBOL_BX_TOKEN_XML = 100,             /* BX_TOKEN_XML  */
  YYSYMBOL_BX_TOKEN_CALC = 101,            /* BX_TOKEN_CALC  */
  YYSYMBOL_BX_TOKEN_DEVICE = 102,          /* BX_TOKEN_DEVICE  */
  YYSYMBOL_BX_TOKEN_GENERIC = 103,         /* BX_TOKEN_GENERIC  */
  YYSYMBOL_BX_TOKEN_RSHIFT = 104,          /* BX_TOKEN_RSHIFT  */
  YYSYMBOL_BX_TOKEN_LSHIFT = 105,          /* BX_TOKEN_LSHIFT  */
  YYSYMBOL_BX_TOKEN_EQ = 106,              /* BX_TOKEN_EQ  */
  YYSYMBOL_BX_TOKEN_NE = 107,              /* BX_TOKEN_NE  */
  YYSYMBOL_BX_TOKEN_LE = 108,              /* BX_TOKEN_LE  */
  YYSYMBOL_BX_TOKEN_GE = 109,              /* BX_TOKEN_GE  */
  YYSYMBOL_BX_TOKEN_REG_IP = 110,          /* BX_TOKEN_REG_IP  */
  YYSYMBOL_BX_TOKEN_REG_EIP = 111,         /* BX_TOKEN_REG_EIP  */
  YYSYMBOL_BX_TOKEN_REG_RIP = 112,         /* BX_TOKEN_REG_RIP  */
  YYSYMBOL_BX_TOKEN_REG_SSP = 113,         /* BX_TOKEN_REG_SSP  */
  YYSYMBOL_114_ = 114,                     /* '+'  */
  YYSYMBOL_115_ = 115,                     /* '-'  */
  YYSYMBOL_116_ = 116,                     /* '|'  */
  YYSYMBOL_117_ = 117,                     /* '^'  */
  YYSYMBOL_118_ = 118,                     /* '<'  */
  YYSYMBOL_119_ = 119,                     /* '>'  */
  YYSYMBOL_120_ = 120,                     /* '*'  */
  YYSYMBOL_121_ = 121,                     /* '/'  */
  YYSYMBOL_122_ = 122,                     /* '&'  */
  YYSYMBOL_NOT = 123,                      /* NOT  */
  YYSYMBOL_NEG = 124,                      /* NEG  */
  YYSYMBOL_INDIRECT = 125,                 /* INDIRECT  */
  YYSYMBOL_126_n_ = 126,                   /* '\n'  */
  YYSYMBOL_127_ = 127,                     /* '='  */
  YYSYMBOL_128_ = 128,                     /* ':'  */
  YYSYMBOL_129_ = 129,                     /* '!'  */
  YYSYMBOL_130_ = 130,                     /* '('  */
  YYSYMBOL_131_ = 131,                     /* ')'  */
  YYSYMBOL_132_ = 132,                     /* '@'  */
  YYSYMBOL_YYACCEPT = 133,                 /* $accept  */
  YYSYMBOL_commands = 134,                 /* commands  */
  YYSYMBOL_command = 135,                  /* command  */
  YYSYMBOL_BX_TOKEN_TOGGLE_ON_OFF = 136,   /* BX_TOKEN_TOGGLE_ON_OFF  */
  YYSYMBOL_BX_TOKEN_REGISTERS = 137,       /* BX_TOKEN_REGISTERS  */
  YYSYMBOL_BX_TOKEN_SEGREG = 138,          /* BX_TOKEN_SEGREG  */
  YYSYMBOL_timebp_command = 139,           /* timebp_command  */
  YYSYMBOL_modebp_command = 140,           /* modebp_command  */
  YYSYMBOL_vmexitbp_command = 141,         /* vmexitbp_command  */
  YYSYMBOL_show_command = 142,             /* show_command  */
  YYSYMBOL_page_command = 143,             /* page_command  */
  YYSYMBOL_tlb_command = 144,              /* tlb_command  */
  YYSYMBOL_ptime_command = 145,            /* ptime_command  */
  YYSYMBOL_trace_command = 146,            /* trace_command  */
  YYSYMBOL_trace_reg_command = 147,        /* trace_reg_command  */
  YYSYMBOL_trace_mem_command = 148,        /* trace_mem_command  */
  YYSYMBOL_print_stack_command = 149,      /* print_stack_command  */
  YYSYMBOL_backtrace_command = 150,        /* backtrace_command  */
  YYSYMBOL_watch_point_command = 151,      /* watch_point_command  */
  YYSYMBOL_symbol_command = 152,           /* symbol_command  */
  YYSYMBOL_where_command = 153,            /* where_command  */
  YYSYMBOL_print_string_command = 154,     /* print_string_command  */
  YYSYMBOL_continue_command = 155,         /* continue_command  */
  YYSYMBOL_stepN_command = 156,            /* stepN_command  */
  YYSYMBOL_step_over_command = 157,        /* step_over_command  */
  YYSYMBOL_set_command = 158,              /* set_command  */
  YYSYMBOL_breakpoint_command = 159,       /* breakpoint_command  */
  YYSYMBOL_blist_command = 160,            /* blist_command  */
  YYSYMBOL_slist_command = 161,            /* slist_command  */
  YYSYMBOL_info_command = 162,             /* info_command  */
  YYSYMBOL_optional_numeric = 163,         /* optional_numeric  */
  YYSYMBOL_regs_command = 164,             /* regs_command  */
  YYSYMBOL_fpu_regs_command = 165,         /* fpu_regs_command  */
  YYSYMBOL_mmx_regs_command = 166,         /* mmx_regs_command  */
  YYSYMBOL_xmm_regs_command = 167,         /* xmm_regs_command  */
  YYSYMBOL_ymm_regs_command = 168,         /* ymm_regs_command  */
  YYSYMBOL_zmm_regs_command = 169,         /* zmm_regs_command  */
  YYSYMBOL_segment_regs_command = 170,     /* segment_regs_command  */
  YYSYMBOL_control_regs_command = 171,     /* control_regs_command  */
  YYSYMBOL_debug_regs_command = 172,       /* debug_regs_command  */
  YYSYMBOL_delete_command = 173,           /* delete_command  */
  YYSYMBOL_bpe_command = 174,              /* bpe_command  */
  YYSYMBOL_bpd_command = 175,              /* bpd_command  */
  YYSYMBOL_quit_command = 176,             /* quit_command  */
  YYSYMBOL_examine_command = 177,          /* examine_command  */
  YYSYMBOL_restore_command = 178,          /* restore_command  */
  YYSYMBOL_writemem_command = 179,         /* writemem_command  */
  YYSYMBOL_setpmem_command = 180,          /* setpmem_command  */
  YYSYMBOL_query_command = 181,            /* query_command  */
  YYSYMBOL_take_command = 182,             /* take_command  */
  YYSYMBOL_disassemble_command = 183,      /* disassemble_command  */
  YYSYMBOL_instrument_command = 184,       /* instrument_command  */
  YYSYMBOL_doit_command = 185,             /* doit_command  */
  YYSYMBOL_crc_command = 186,              /* crc_command  */
  YYSYMBOL_help_command = 187,             /* help_command  */
  YYSYMBOL_calc_command = 188,             /* calc_command  */
  YYSYMBOL_if_command = 189,               /* if_command  */
  YYSYMBOL_vexpression = 190,              /* vexpression  */
  YYSYMBOL_expression = 191                /* expression  */
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
#define YYFINAL  310
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2460

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  133
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  59
/* YYNRULES -- Number of rules.  */
#define YYNRULES  302
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  589

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   371


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
     126,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   129,     2,     2,     2,     2,   122,     2,
     130,   131,   120,   114,     2,   115,     2,   121,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   128,     2,
     118,   127,   119,     2,   132,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   117,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   116,     2,     2,     2,     2,     2,
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
     105,   106,   107,   108,   109,   110,   111,   112,   113,   123,
     124,   125
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   149,   149,   150,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   212,   213,   218,
     219,   224,   225,   226,   227,   228,   229,   234,   239,   247,
     255,   263,   268,   273,   278,   283,   288,   296,   304,   312,
     320,   328,   336,   344,   349,   357,   362,   370,   375,   380,
     385,   390,   395,   400,   405,   410,   415,   420,   428,   433,
     438,   443,   451,   459,   467,   472,   480,   485,   490,   495,
     503,   511,   516,   521,   525,   529,   533,   537,   541,   545,
     549,   556,   561,   566,   571,   576,   581,   586,   591,   596,
     601,   606,   611,   616,   624,   632,   637,   645,   650,   655,
     660,   665,   670,   675,   680,   685,   690,   695,   700,   705,
     710,   715,   720,   725,   733,   734,   737,   745,   753,   761,
     769,   777,   785,   793,   801,   809,   817,   824,   832,   840,
     845,   850,   855,   863,   871,   879,   887,   895,   900,   905,
     910,   915,   923,   928,   933,   938,   943,   948,   953,   958,
     966,   972,   977,   985,   993,  1001,  1006,  1012,  1019,  1024,
    1030,  1036,  1042,  1047,  1052,  1057,  1062,  1067,  1072,  1077,
    1083,  1089,  1095,  1103,  1108,  1113,  1118,  1123,  1128,  1133,
    1138,  1143,  1148,  1153,  1158,  1163,  1168,  1173,  1178,  1183,
    1188,  1198,  1209,  1215,  1228,  1233,  1244,  1249,  1265,  1281,
    1291,  1296,  1304,  1313,  1323,  1324,  1325,  1326,  1327,  1328,
    1329,  1330,  1331,  1332,  1333,  1334,  1335,  1336,  1337,  1338,
    1339,  1340,  1341,  1342,  1343,  1344,  1345,  1346,  1347,  1353,
    1354,  1355,  1356,  1357,  1358,  1359,  1360,  1361,  1362,  1363,
    1364,  1365,  1366,  1367,  1368,  1369,  1370,  1371,  1372,  1373,
    1374,  1375,  1376,  1377,  1378,  1379,  1380,  1381,  1382,  1383,
    1384,  1385,  1386
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
  "BX_TOKEN_SETPMEM", "BX_TOKEN_SYMBOLNAME", "BX_TOKEN_QUERY",
  "BX_TOKEN_PENDING", "BX_TOKEN_TAKE", "BX_TOKEN_DMA", "BX_TOKEN_IRQ",
  "BX_TOKEN_SMI", "BX_TOKEN_NMI", "BX_TOKEN_TLB", "BX_TOKEN_DISASM",
  "BX_TOKEN_INSTRUMENT", "BX_TOKEN_STRING", "BX_TOKEN_STOP",
  "BX_TOKEN_DOIT", "BX_TOKEN_CRC", "BX_TOKEN_TRACE", "BX_TOKEN_TRACEREG",
  "BX_TOKEN_TRACEMEM", "BX_TOKEN_SWITCH_MODE", "BX_TOKEN_SIZE",
  "BX_TOKEN_PTIME", "BX_TOKEN_TIMEBP_ABSOLUTE", "BX_TOKEN_TIMEBP",
  "BX_TOKEN_MODEBP", "BX_TOKEN_VMEXITBP", "BX_TOKEN_PRINT_STACK",
  "BX_TOKEN_BT", "BX_TOKEN_WATCH", "BX_TOKEN_UNWATCH", "BX_TOKEN_READ",
  "BX_TOKEN_WRITE", "BX_TOKEN_SHOW", "BX_TOKEN_LOAD_SYMBOLS",
  "BX_TOKEN_SYMBOLS", "BX_TOKEN_LIST_SYMBOLS", "BX_TOKEN_GLOBAL",
  "BX_TOKEN_WHERE", "BX_TOKEN_PRINT_STRING", "BX_TOKEN_NUMERIC",
  "BX_TOKEN_PAGE", "BX_TOKEN_HELP", "BX_TOKEN_XML", "BX_TOKEN_CALC",
  "BX_TOKEN_DEVICE", "BX_TOKEN_GENERIC", "BX_TOKEN_RSHIFT",
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
  "setpmem_command", "query_command", "take_command",
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

#define YYPACT_NINF (-178)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-301)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     557,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,    -9,  1442,   -36,  -124,   700,  -113,    19,
     296,  1170,   -38,   -35,   -27,   878,   -60,  -178,  -178,   -55,
     -53,   -44,   -41,   -40,   -39,   -34,   -33,  1036,    26,    29,
    1442,    58,   105,  1442,  1075,   -52,  -178,  1442,  1442,    40,
      40,    40,    -6,  1442,  1442,     1,     2,   -63,   -62,    50,
    1206,   -12,    18,   -32,     7,  1442,  -178,  1442,  1555,  1442,
    -178,  -178,  -178,  -178,  1442,  1442,  -178,  1442,  1442,  1442,
     427,  -178,    10,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  2332,  1442,  -178,   947,    43,   -45,
    -178,  -178,    -1,    14,    15,    20,    23,    37,    40,    45,
      46,    48,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,    97,  -178,    97,    97,  -178,
     814,    -8,  -178,   253,  1442,  -178,   630,    11,    55,    56,
      72,    74,    76,  1442,  1442,  1442,  1442,    77,    78,    85,
     -31,   -54,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  1240,  -178,  1582,   144,  1442,  1007,    89,   -42,    90,
      95,    98,  1607,  1370,    99,    96,  -178,   148,   102,   107,
     108,  1632,  1007,  -178,  -178,   113,   116,   117,  -178,  1657,
    1682,  -178,  -178,   118,  -178,   121,  -178,   122,  1442,   123,
    1442,  1442,  -178,  -178,  1707,   125,   145,   -50,   147,  -178,
    1394,   205,   153,  -178,  -178,  1732,  1757,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   185,   186,   203,   204,   206,
     207,   214,   218,   229,   230,   237,   238,   239,   250,   251,
     252,   254,   256,   257,   260,   265,   266,   268,   269,   270,
     271,   272,  -178,   277,  1782,   276,   276,   276,  1554,   276,
    -178,  -178,  -178,  1442,  1442,  1442,  1442,  1442,  1442,  1442,
    1442,  1442,  1442,  1442,  1442,  1442,  1442,  1442,  1442,  1807,
    -178,   279,   284,  -178,  1442,  1442,  1442,  1442,  1442,  1442,
     286,  1442,  1442,  1442,  -178,  -178,   115,    97,    97,    97,
      97,    97,    97,    97,    97,    97,    97,   261,  -178,   343,
    -178,   -13,   344,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    1442,  2332,  1442,  1442,  1442,  -178,  -178,  -178,   289,  -178,
     -29,   -28,  -178,  -178,  1832,  -178,   292,  1007,  1442,  1442,
    1007,  -178,   295,  -178,  -178,  -178,  -178,  -178,  -178,   687,
    -178,    49,  -178,  1857,  -178,  -178,  -178,  -178,  1882,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,   717,  -178,   847,
     877,  -178,  -178,  -178,   297,  -178,  -178,  -178,  1907,  1418,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,    16,    16,   276,   276,   276,   276,  1151,  1151,  1151,
    1151,  1151,  1151,    16,    16,    16,  2332,  -178,  -178,  -178,
    1932,  1957,  1982,  2007,  2032,  2057,  -178,  2082,  2107,  2132,
    -178,  -178,  -178,   280,   280,   280,   280,  -178,  -178,  -178,
     656,   298,   316,   373,  -178,   318,   324,   336,   342,   345,
    -178,   346,  -178,   347,  -178,  -178,  -178,  2157,  1236,    71,
    2182,  -178,  -178,  2207,   348,  -178,  -178,  -178,  2232,  -178,
    2257,  -178,  2282,  -178,  -178,  -178,  2307,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,   399,  -178,  -178,  -178,
     349,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,   350,  -178,  -178
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
      55,   272,   271,   273,   274,   275,    61,    62,    63,    64,
      65,    66,   276,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    59,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   270,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   269,     0,     0,     0,
     278,   279,   280,   281,     0,     0,    56,     0,     0,     0,
       0,     3,     0,   277,    39,    40,    41,    47,    45,    46,
      38,    35,    36,    37,    42,    43,    44,    48,    49,    50,
       4,     5,     6,     7,     8,    19,    20,     9,    10,    11,
      12,    13,    14,    15,    16,    18,    17,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    51,    52,    53,    54,     0,   104,     0,     0,     0,
     106,   110,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   134,   247,   246,   248,   249,   250,   251,   245,
     244,   253,   254,   255,   256,     0,   121,     0,     0,   252,
       0,   270,   124,     0,     0,   129,     0,     0,     0,     0,
       0,     0,     0,   154,   154,   154,   154,     0,     0,     0,
       0,     0,   168,   157,   158,   159,   160,   161,   164,   163,
     162,     0,   172,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   182,     0,     0,     0,
       0,     0,     0,    57,    58,     0,     0,     0,    79,     0,
       0,    69,    70,     0,    83,     0,    85,     0,     0,     0,
       0,     0,    89,    96,     0,     0,     0,     0,     0,    76,
       0,     0,     0,   135,   102,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   241,     0,     0,   299,   300,   298,     0,   301,
       1,     2,   156,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     243,     0,     0,   107,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   267,   266,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   127,     0,
     125,   300,     0,   130,   165,   166,   167,   145,   137,   138,
     154,   155,   154,   154,   154,   144,   143,   146,     0,   147,
       0,     0,   149,   170,     0,   171,     0,     0,     0,     0,
       0,   176,     0,   177,   179,   180,   181,    78,   185,     0,
     188,     0,   183,     0,   191,   190,   192,   193,     0,    80,
      81,    82,    68,    67,    84,    86,    88,     0,    87,     0,
       0,    97,    73,    72,     0,    74,    71,    98,     0,     0,
     136,   103,    77,   196,   197,   198,   235,   205,   199,   200,
     201,   202,   203,   204,   237,   195,   220,   221,   222,   223,
     224,   227,   226,   225,   233,   212,   228,   229,   230,   234,
     208,   209,   210,   211,   213,   215,   214,   206,   207,   216,
     231,   232,   238,   217,   218,   236,   240,   239,   219,   242,
     302,   287,   288,   294,   295,   296,   297,   283,   284,   289,
     290,   293,   292,   285,   286,   291,   282,   105,   108,   109,
       0,     0,     0,     0,     0,     0,   111,     0,     0,     0,
     268,   261,   262,   257,   258,   263,   264,   259,   260,   265,
       0,     0,     0,     0,   132,     0,     0,     0,     0,     0,
     148,     0,   152,     0,   150,   169,   173,     0,   284,   285,
       0,   178,   186,     0,     0,   184,   194,    90,     0,    91,
       0,    92,     0,    75,    99,   100,     0,   114,   113,   115,
     116,   117,   112,   118,   119,   120,     0,   122,   128,   126,
       0,   131,   139,   140,   141,   142,   153,   151,   174,   175,
     187,   189,    93,    94,    95,   101,     0,   133,   123
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -178,  -178,   390,    -5,   413,    -2,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -177,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -164,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    80,    81,   225,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     370,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   170,   371
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     134,   344,   141,   345,   346,   245,   523,   372,   373,   374,
     135,   357,   138,   152,   137,   151,   380,   169,   218,   219,
     173,   176,   153,   154,   155,   156,   157,     6,     7,     8,
       9,    10,    11,   158,   233,   235,   246,   203,   252,   378,
     206,   531,   533,   212,   217,   226,   227,   221,   222,   381,
     424,   220,   332,   229,   230,   392,   223,   224,   247,   177,
     244,   139,   178,   234,   236,   255,   192,   256,   237,   304,
     179,   193,   382,   194,   305,   306,   425,   307,   308,   309,
     134,   333,   195,   238,   393,   196,   197,   198,   250,   159,
     140,   248,   199,   200,   253,   379,   204,   532,   534,   205,
     153,   154,   155,   156,   157,     6,     7,     8,     9,    10,
      11,   158,   251,   524,   249,   328,   160,   136,   358,   207,
     228,   239,   315,   316,   317,   318,   334,   231,   232,   161,
     162,   163,   164,   254,   165,   329,   312,   364,   240,   241,
     331,   335,   336,   340,   328,   166,   544,   337,   167,   168,
     338,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,   169,   339,   169,   169,   159,   208,   209,
     210,   211,   341,   342,   361,   343,   242,  -300,  -300,  -300,
    -300,   365,   366,   511,   512,   513,   514,   515,   516,   517,
     518,   519,   520,   526,   160,   527,   528,   529,   367,   328,
     368,   384,   369,   375,   376,   387,   390,   161,   162,   163,
     164,   377,   165,   399,   386,   391,   394,   403,    46,   347,
     348,   395,   408,   401,   396,   400,   167,   168,   404,   349,
     350,   351,   352,   405,   406,   353,   354,   355,   417,   409,
     419,   420,   410,   411,   414,    66,   510,   415,   416,   418,
     428,   422,   313,   314,   315,   316,   317,   318,    70,    71,
      72,    73,   319,   388,   321,   322,   323,   324,   389,   326,
     327,   423,   359,   426,   402,   429,   328,    77,    78,   430,
      79,   433,   434,   435,   436,   437,   438,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   449,   450,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,   451,   452,   481,   482,   483,   484,   485,   486,   487,
     488,   489,   490,   491,   492,   493,   494,   495,   496,   453,
     454,   521,   455,   456,   500,   501,   502,   503,   504,   505,
     457,   507,   508,   509,   458,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   459,   460,   313,   314,   315,
     316,   317,   318,   461,   462,   463,   171,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   464,   465,   466,   360,
     467,   328,   468,   469,   347,   348,   470,   537,   538,   539,
     540,   471,   472,    66,   473,   474,   475,   476,   477,   543,
     353,   354,   355,   478,   328,   498,    70,    71,    72,    73,
     499,    74,   506,   522,   525,   530,    75,   548,   536,   550,
     552,   541,   172,   553,   568,    77,    78,   310,    79,   556,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,   569,   570,   571,    13,    14,    15,    16,    17,
     572,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,   573,    29,    30,    31,    32,    33,   574,   586,
     311,   575,   576,   577,   581,   587,   588,    34,    35,    36,
      37,   303,     0,    38,    39,    40,     0,    41,     0,    42,
       0,     0,     0,     0,    43,    44,    45,    46,     0,    47,
      48,    49,    50,    51,     0,     0,    52,    53,    54,    55,
      56,    57,    58,    59,    60,     0,     0,    61,    62,     0,
      63,     0,    64,    65,    66,    67,    68,     0,    69,     0,
       0,     0,     0,     0,     0,     0,     0,    70,    71,    72,
      73,     0,    74,     0,     0,     0,     0,    75,     0,     0,
       0,     0,     0,    76,     0,     0,    77,    78,     0,    79,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,     0,     0,     0,    13,    14,    15,    16,    17,
       0,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,     0,    29,    30,    31,    32,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    34,    35,    36,
      37,     0,     0,    38,    39,    40,     0,    41,     0,    42,
       0,     0,     0,     0,    43,    44,    45,    46,     0,    47,
      48,    49,    50,    51,     0,     0,    52,    53,    54,    55,
      56,    57,    58,    59,    60,     0,     0,    61,    62,   362,
      63,     0,    64,    65,    66,    67,    68,     0,    69,     0,
       0,     0,     0,     0,     0,     0,     0,    70,    71,    72,
      73,     0,    74,     0,     0,   566,     0,    75,     0,     0,
       0,     0,     0,    76,     0,     0,    77,    78,     0,    79,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,     0,   142,   143,   144,   145,   146,     6,     7,
       8,     9,    10,    11,     0,     0,     0,     0,     0,     0,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,     0,     0,   313,   314,   315,   316,   317,   318,
       0,     0,     0,     0,   319,   320,   321,   322,   323,   324,
     325,   326,   327,     0,     0,     0,   363,    46,   328,   147,
     347,   348,     0,     0,     0,     0,     0,     0,   148,     0,
     349,   350,   351,   352,     0,     0,   353,   354,   355,     0,
       0,     0,   567,     0,    66,     0,     0,    46,     0,     0,
       0,   313,   314,   315,   316,   317,   318,    70,    71,    72,
      73,   319,   388,   321,   322,   323,   324,   389,   326,   327,
       0,   149,   150,   542,    66,   328,    77,    78,     0,    79,
       0,   313,   314,   315,   316,   317,   318,    70,    71,    72,
      73,   319,   388,   321,   322,   323,   324,   389,   326,   327,
       0,     0,     0,   547,     0,   328,    77,    78,     0,    79,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,     0,   180,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   181,     0,     0,     0,     0,
       0,     0,     0,   182,     0,     0,     0,    46,   347,   348,
     183,   184,   185,   186,   187,   188,     0,   189,   349,   350,
     351,   352,     0,     0,   353,   354,   355,     0,     0,     0,
       0,     0,   356,     0,    66,     0,     0,    46,     0,     0,
       0,   313,   314,   315,   316,   317,   318,    70,    71,    72,
      73,   319,   388,   321,   322,   323,   324,   389,   326,   327,
     190,     0,     0,   549,    66,   328,    77,    78,     0,    79,
     191,   313,   314,   315,   316,   317,   318,    70,    71,    72,
      73,   319,   388,   321,   322,   323,   324,   389,   326,   327,
       0,     0,     0,   551,     0,   328,    77,    78,     0,    79,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   330,     0,   328,     0,    46,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
     201,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    66,     0,    46,     0,     0,     0,
       0,   313,   314,   315,   316,   317,   318,    70,    71,    72,
      73,   319,   388,   321,   322,   323,   324,   389,   326,   327,
     213,     0,     0,    66,     0,   328,    77,    78,     0,    79,
       0,     0,     0,     0,     0,    46,    70,    71,    72,    73,
       0,    74,   214,   215,     0,     0,    75,     0,     0,     0,
       0,     0,   202,     0,     0,    77,    78,     0,    79,     0,
       0,     0,    66,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    70,    71,    72,    73,     0,
      74,     0,     0,     0,     0,    75,     0,     0,     0,     0,
       0,   216,     0,     0,    77,    78,     0,    79,     0,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      46,     0,     0,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,   313,   314,   315,   316,   317,
     318,     0,     0,     0,     0,     0,     0,    66,     0,     0,
       0,   325,   326,   327,     0,     0,    46,     0,     0,   328,
      70,    71,    72,    73,     0,    74,     0,     0,     0,     0,
     174,     0,     0,     0,     0,     0,   175,     0,     0,    77,
      78,     0,    79,    66,     0,     0,     0,     0,     0,     0,
      46,     0,     0,     0,     0,     0,    70,    71,    72,    73,
       0,    74,     0,     0,     0,     0,    75,     0,     0,     0,
       0,     0,   243,     0,     0,    77,    78,    66,    79,     0,
    -299,  -299,  -299,  -299,  -299,  -299,     0,     0,     0,     0,
      70,    71,    72,    73,     0,    74,  -299,  -299,  -299,     0,
      75,     0,     0,     0,   328,     0,   383,     0,     0,    77,
      78,     0,    79,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,     0,     0,     0,     0,
      46,     0,     0,     0,     0,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,     0,     0,     0,
       0,     0,     0,     0,    46,     0,     0,    66,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      70,    71,    72,    73,     0,    74,     0,     0,    46,     0,
      75,    66,     0,     0,     0,     0,   398,     0,     0,    77,
      78,     0,    79,     0,    70,    71,    72,    73,     0,    74,
       0,     0,    46,     0,    75,    66,     0,     0,     0,     0,
     427,     0,     0,    77,    78,     0,    79,     0,    70,    71,
      72,    73,     0,    74,     0,     0,     0,     0,    75,    66,
       0,     0,     0,     0,   555,     0,     0,    77,    78,     0,
      79,     0,    70,    71,    72,    73,     0,    74,     0,     0,
       0,     0,    75,     0,     0,     0,     0,     0,     0,     0,
       0,    77,    78,   257,    79,   258,   259,   260,     0,   261,
     262,   263,   264,   265,   266,   267,   268,   269,    27,    28,
       0,   270,   271,   272,   273,   274,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   275,   276,   277,   278,     0,
       0,   279,   280,   281,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   282,   283,     0,     0,     0,   284,   285,
     286,   287,     0,     0,   288,   289,   290,   291,   292,   293,
       0,   294,   295,     0,     0,   296,   297,     0,   298,     0,
       0,     0,     0,   299,   300,     0,   301,     0,   313,   314,
     315,   316,   317,   318,     0,     0,     0,     0,   319,   320,
     321,   322,   323,   324,   325,   326,   327,     0,     0,     0,
       0,   302,   328,     0,     0,   480,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   385,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   397,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   407,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   412,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   413,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   421,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   431,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   432,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   479,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   497,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   535,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   545,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   546,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   554,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   557,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   558,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   559,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   560,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   561,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   562,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   563,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   564,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   565,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   578,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   579,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   580,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   582,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   583,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   584,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   585,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,     0,     0,
     328
};

static const yytype_int16 yycheck[] =
{
       0,   165,   126,   167,   168,    17,    19,   184,   185,   186,
      19,    19,    48,   126,    14,    17,    70,    19,    70,    71,
      20,    21,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    97,    97,    48,    37,    70,    70,
      40,    70,    70,    43,    44,    50,    51,    47,    48,   103,
     100,   103,    97,    53,    54,    97,    16,    17,    70,    97,
      60,    97,    97,   126,   126,    65,   126,    67,    18,    69,
      97,   126,   126,   126,    74,    75,   126,    77,    78,    79,
      80,   126,   126,    33,   126,   126,   126,   126,    70,    70,
     126,   103,   126,   126,   126,   126,    70,   126,   126,    70,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    94,   126,   126,   128,    97,   126,   126,    61,
     126,    71,   106,   107,   108,   109,   127,   126,   126,   110,
     111,   112,   113,   126,   115,   135,   126,   126,    88,    89,
      97,   127,   127,   148,   128,   126,    97,   127,   129,   130,
     127,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,   165,   127,   167,   168,    70,    63,    64,
      65,    66,   127,   127,   174,   127,   126,   106,   107,   108,
     109,   126,   126,   347,   348,   349,   350,   351,   352,   353,
     354,   355,   356,   370,    97,   372,   373,   374,   126,   128,
     126,   201,   126,   126,   126,   205,   206,   110,   111,   112,
     113,   126,   115,   213,    70,   126,   126,   217,    70,   104,
     105,   126,   222,   127,   126,   126,   129,   130,   126,   114,
     115,   116,   117,   126,   126,   120,   121,   122,   238,   126,
     240,   241,   126,   126,   126,    97,   131,   126,   126,   126,
     250,   126,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   126,    19,   126,   126,    70,   128,   129,   130,   126,
     132,   126,   126,   126,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   126,   126,   126,   126,   126,   126,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,   126,   126,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   126,
     126,    70,   126,   126,   334,   335,   336,   337,   338,   339,
     126,   341,   342,   343,   126,   347,   348,   349,   350,   351,
     352,   353,   354,   355,   356,   126,   126,   104,   105,   106,
     107,   108,   109,   126,   126,   126,    70,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   126,   126,   126,   126,
     126,   128,   126,   126,   104,   105,   126,   387,   388,   389,
     390,   126,   126,    97,   126,   126,   126,   126,   126,   399,
     120,   121,   122,   126,   128,   126,   110,   111,   112,   113,
     126,   115,   126,    70,    70,   126,   120,   417,   126,   419,
     420,   126,   126,   126,   126,   129,   130,     0,   132,   429,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,   126,    70,   126,    18,    19,    20,    21,    22,
     126,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,   126,    36,    37,    38,    39,    40,   126,    70,
      80,   126,   126,   126,   126,   126,   126,    50,    51,    52,
      53,    68,    -1,    56,    57,    58,    -1,    60,    -1,    62,
      -1,    -1,    -1,    -1,    67,    68,    69,    70,    -1,    72,
      73,    74,    75,    76,    -1,    -1,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    -1,    -1,    90,    91,    -1,
      93,    -1,    95,    96,    97,    98,    99,    -1,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,   111,   112,
     113,    -1,   115,    -1,    -1,    -1,    -1,   120,    -1,    -1,
      -1,    -1,    -1,   126,    -1,    -1,   129,   130,    -1,   132,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    18,    19,    20,    21,    22,
      -1,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    -1,    36,    37,    38,    39,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    51,    52,
      53,    -1,    -1,    56,    57,    58,    -1,    60,    -1,    62,
      -1,    -1,    -1,    -1,    67,    68,    69,    70,    -1,    72,
      73,    74,    75,    76,    -1,    -1,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    -1,    -1,    90,    91,    19,
      93,    -1,    95,    96,    97,    98,    99,    -1,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,   111,   112,
     113,    -1,   115,    -1,    -1,    19,    -1,   120,    -1,    -1,
      -1,    -1,    -1,   126,    -1,    -1,   129,   130,    -1,   132,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,   104,   105,   106,   107,   108,   109,
      -1,    -1,    -1,    -1,   114,   115,   116,   117,   118,   119,
     120,   121,   122,    -1,    -1,    -1,   126,    70,   128,    59,
     104,   105,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,
     114,   115,   116,   117,    -1,    -1,   120,   121,   122,    -1,
      -1,    -1,   126,    -1,    97,    -1,    -1,    70,    -1,    -1,
      -1,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,   111,   112,   126,    97,   128,   129,   130,    -1,   132,
      -1,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   129,   130,    -1,   132,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    35,    -1,    -1,    -1,    70,   104,   105,
      42,    43,    44,    45,    46,    47,    -1,    49,   114,   115,
     116,   117,    -1,    -1,   120,   121,   122,    -1,    -1,    -1,
      -1,    -1,   128,    -1,    97,    -1,    -1,    70,    -1,    -1,
      -1,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      92,    -1,    -1,   126,    97,   128,   129,   130,    -1,   132,
     102,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   129,   130,    -1,   132,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,    -1,    70,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    97,    -1,    70,    -1,    -1,    -1,
      -1,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      55,    -1,    -1,    97,    -1,   128,   129,   130,    -1,   132,
      -1,    -1,    -1,    -1,    -1,    70,   110,   111,   112,   113,
      -1,   115,    77,    78,    -1,    -1,   120,    -1,    -1,    -1,
      -1,    -1,   126,    -1,    -1,   129,   130,    -1,   132,    -1,
      -1,    -1,    97,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,   110,   111,   112,   113,    -1,
     115,    -1,    -1,    -1,    -1,   120,    -1,    -1,    -1,    -1,
      -1,   126,    -1,    -1,   129,   130,    -1,   132,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    -1,    -1,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,   104,   105,   106,   107,   108,
     109,    -1,    -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,
      -1,   120,   121,   122,    -1,    -1,    70,    -1,    -1,   128,
     110,   111,   112,   113,    -1,   115,    -1,    -1,    -1,    -1,
     120,    -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,   129,
     130,    -1,   132,    97,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    -1,    -1,    -1,    -1,    -1,   110,   111,   112,   113,
      -1,   115,    -1,    -1,    -1,    -1,   120,    -1,    -1,    -1,
      -1,    -1,   126,    -1,    -1,   129,   130,    97,   132,    -1,
     104,   105,   106,   107,   108,   109,    -1,    -1,    -1,    -1,
     110,   111,   112,   113,    -1,   115,   120,   121,   122,    -1,
     120,    -1,    -1,    -1,   128,    -1,   126,    -1,    -1,   129,
     130,    -1,   132,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    70,    -1,    -1,    97,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     110,   111,   112,   113,    -1,   115,    -1,    -1,    70,    -1,
     120,    97,    -1,    -1,    -1,    -1,   126,    -1,    -1,   129,
     130,    -1,   132,    -1,   110,   111,   112,   113,    -1,   115,
      -1,    -1,    70,    -1,   120,    97,    -1,    -1,    -1,    -1,
     126,    -1,    -1,   129,   130,    -1,   132,    -1,   110,   111,
     112,   113,    -1,   115,    -1,    -1,    -1,    -1,   120,    97,
      -1,    -1,    -1,    -1,   126,    -1,    -1,   129,   130,    -1,
     132,    -1,   110,   111,   112,   113,    -1,   115,    -1,    -1,
      -1,    -1,   120,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   129,   130,    18,   132,    20,    21,    22,    -1,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      -1,    36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    51,    52,    53,    -1,
      -1,    56,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    68,    69,    -1,    -1,    -1,    73,    74,
      75,    76,    -1,    -1,    79,    80,    81,    82,    83,    84,
      -1,    86,    87,    -1,    -1,    90,    91,    -1,    93,    -1,
      -1,    -1,    -1,    98,    99,    -1,   101,    -1,   104,   105,
     106,   107,   108,   109,    -1,    -1,    -1,    -1,   114,   115,
     116,   117,   118,   119,   120,   121,   122,    -1,    -1,    -1,
      -1,   126,   128,    -1,    -1,   131,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,   126,    -1,
     128,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,    -1,    -1,   126,    -1,   128,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,    -1,    -1,
     128
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    18,    19,    20,    21,    22,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    36,
      37,    38,    39,    40,    50,    51,    52,    53,    56,    57,
      58,    60,    62,    67,    68,    69,    70,    72,    73,    74,
      75,    76,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    90,    91,    93,    95,    96,    97,    98,    99,   101,
     110,   111,   112,   113,   115,   120,   126,   129,   130,   132,
     134,   135,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   191,    19,   126,   191,    48,    97,
     126,   126,     3,     4,     5,     6,     7,    59,    68,   111,
     112,   138,   126,     3,     4,     5,     6,     7,    14,    70,
      97,   110,   111,   112,   113,   115,   126,   129,   130,   138,
     190,    70,   126,   191,   120,   126,   191,    97,    97,    97,
      15,    27,    35,    42,    43,    44,    45,    46,    47,    49,
      92,   102,   126,   126,   126,   126,   126,   126,   126,   126,
     126,    54,   126,   191,    70,    70,   191,    61,    63,    64,
      65,    66,   191,    55,    77,    78,   126,   191,    70,    71,
     103,   191,   191,    16,    17,   136,   136,   136,   126,   191,
     191,   126,   126,    97,   126,    97,   126,    18,    33,    71,
      88,    89,   126,   126,   191,    17,    48,    70,   103,   126,
      70,    94,    70,   126,   126,   191,   191,    18,    20,    21,
      22,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      36,    37,    38,    39,    40,    50,    51,    52,    53,    56,
      57,    58,    68,    69,    73,    74,    75,    76,    79,    80,
      81,    82,    83,    84,    86,    87,    90,    91,    93,    98,
      99,   101,   126,   137,   191,   191,   191,   191,   191,   191,
       0,   135,   126,   104,   105,   106,   107,   108,   109,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   128,   191,
     126,    97,    97,   126,   127,   127,   127,   127,   127,   127,
     136,   127,   127,   127,   190,   190,   190,   104,   105,   114,
     115,   116,   117,   120,   121,   122,   128,    19,   126,    19,
     126,   191,    19,   126,   126,   126,   126,   126,   126,   126,
     163,   191,   163,   163,   163,   126,   126,   126,    70,   126,
      70,   103,   126,   126,   191,   126,    70,   191,   115,   120,
     191,   126,    97,   126,   126,   126,   126,   126,   126,   191,
     126,   127,   126,   191,   126,   126,   126,   126,   191,   126,
     126,   126,   126,   126,   126,   126,   126,   191,   126,   191,
     191,   126,   126,   126,   100,   126,   126,   126,   191,    70,
     126,   126,   126,   126,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   126,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   126,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   126,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   126,   126,   126,   126,   126,   126,   126,
     131,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   126,   126,   126,
     191,   191,   191,   191,   191,   191,   126,   191,   191,   191,
     131,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,    70,    70,    19,   126,    70,   163,   163,   163,   163,
     126,    70,   126,    70,   126,   126,   126,   191,   191,   191,
     191,   126,   126,   191,    97,   126,   126,   126,   191,   126,
     191,   126,   191,   126,   126,   126,   191,   126,   126,   126,
     126,   126,   126,   126,   126,   126,    19,   126,   126,   126,
      70,   126,   126,   126,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   126,   126,   126,    70,   126,   126
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   133,   134,   134,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   136,   136,   137,
     137,   138,   138,   138,   138,   138,   138,   139,   139,   140,
     141,   142,   142,   142,   142,   142,   142,   143,   144,   145,
     146,   147,   148,   149,   149,   150,   150,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   152,   152,
     152,   152,   153,   154,   155,   155,   156,   156,   156,   156,
     157,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   160,   161,   161,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   163,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     177,   177,   177,   178,   179,   180,   181,   182,   182,   182,
     182,   182,   183,   183,   183,   183,   183,   183,   183,   183,
     184,   184,   184,   185,   186,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   188,   189,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     2,
       2,     3,     3,     3,     3,     4,     2,     3,     3,     2,
       3,     3,     3,     2,     3,     2,     3,     3,     3,     2,
       4,     4,     4,     5,     5,     5,     2,     3,     3,     4,
       4,     5,     2,     3,     2,     4,     2,     3,     4,     4,
       2,     4,     5,     5,     5,     5,     5,     5,     5,     5,
       5,     2,     5,     7,     2,     3,     5,     3,     5,     2,
       3,     5,     4,     6,     2,     2,     3,     3,     3,     5,
       5,     5,     5,     3,     3,     3,     3,     3,     4,     3,
       4,     5,     4,     5,     0,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     3,     3,     3,     2,     4,
       3,     3,     2,     4,     5,     5,     3,     3,     4,     3,
       3,     3,     2,     3,     4,     3,     4,     5,     3,     5,
       3,     3,     3,     3,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
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
  case 54: /* command: expression  */
#line 204 "parser.y"
                 { eval_value = (yyvsp[0].uval); }
#line 2363 "y.tab.c"
    break;

  case 56: /* command: '\n'  */
#line 207 "parser.y"
      {
      }
#line 2370 "y.tab.c"
    break;

  case 58: /* BX_TOKEN_TOGGLE_ON_OFF: BX_TOKEN_OFF  */
#line 214 "parser.y"
    { (yyval.bval)=(yyvsp[0].bval); }
#line 2376 "y.tab.c"
    break;

  case 60: /* BX_TOKEN_REGISTERS: BX_TOKEN_REGS  */
#line 220 "parser.y"
    { (yyval.sval)=(yyvsp[0].sval); }
#line 2382 "y.tab.c"
    break;

  case 66: /* BX_TOKEN_SEGREG: BX_TOKEN_GS  */
#line 230 "parser.y"
    { (yyval.uval)=(yyvsp[0].uval); }
#line 2388 "y.tab.c"
    break;

  case 67: /* timebp_command: BX_TOKEN_TIMEBP expression '\n'  */
#line 235 "parser.y"
      {
          bx_dbg_timebp_command(0, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2397 "y.tab.c"
    break;

  case 68: /* timebp_command: BX_TOKEN_TIMEBP_ABSOLUTE expression '\n'  */
#line 240 "parser.y"
      {
          bx_dbg_timebp_command(1, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2406 "y.tab.c"
    break;

  case 69: /* modebp_command: BX_TOKEN_MODEBP '\n'  */
#line 248 "parser.y"
      {
          bx_dbg_modebp_command();
          free((yyvsp[-1].sval));
      }
#line 2415 "y.tab.c"
    break;

  case 70: /* vmexitbp_command: BX_TOKEN_VMEXITBP '\n'  */
#line 256 "parser.y"
      {
          bx_dbg_vmexitbp_command();
          free((yyvsp[-1].sval));
      }
#line 2424 "y.tab.c"
    break;

  case 71: /* show_command: BX_TOKEN_SHOW BX_TOKEN_GENERIC '\n'  */
#line 264 "parser.y"
      {
          bx_dbg_show_command((yyvsp[-1].sval));
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2433 "y.tab.c"
    break;

  case 72: /* show_command: BX_TOKEN_SHOW BX_TOKEN_ALL '\n'  */
#line 269 "parser.y"
      {
          bx_dbg_show_command("all");
          free((yyvsp[-2].sval));
      }
#line 2442 "y.tab.c"
    break;

  case 73: /* show_command: BX_TOKEN_SHOW BX_TOKEN_OFF '\n'  */
#line 274 "parser.y"
      {
          bx_dbg_show_command("off");
          free((yyvsp[-2].sval));
      }
#line 2451 "y.tab.c"
    break;

  case 74: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING '\n'  */
#line 279 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-1].sval), 0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2460 "y.tab.c"
    break;

  case 75: /* show_command: BX_TOKEN_SHOW BX_TOKEN_STRING BX_TOKEN_XML '\n'  */
#line 284 "parser.y"
      {
          bx_dbg_show_param_command((yyvsp[-2].sval), 1);
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2469 "y.tab.c"
    break;

  case 76: /* show_command: BX_TOKEN_SHOW '\n'  */
#line 289 "parser.y"
      {
          bx_dbg_show_command(0);
          free((yyvsp[-1].sval));
      }
#line 2478 "y.tab.c"
    break;

  case 77: /* page_command: BX_TOKEN_PAGE expression '\n'  */
#line 297 "parser.y"
      {
          bx_dbg_xlate_address((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2487 "y.tab.c"
    break;

  case 78: /* tlb_command: BX_TOKEN_TLB expression '\n'  */
#line 305 "parser.y"
      {
          bx_dbg_tlb_lookup((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2496 "y.tab.c"
    break;

  case 79: /* ptime_command: BX_TOKEN_PTIME '\n'  */
#line 313 "parser.y"
      {
          bx_dbg_ptime_command();
          free((yyvsp[-1].sval));
      }
#line 2505 "y.tab.c"
    break;

  case 80: /* trace_command: BX_TOKEN_TRACE BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 321 "parser.y"
      {
          bx_dbg_trace_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2514 "y.tab.c"
    break;

  case 81: /* trace_reg_command: BX_TOKEN_TRACEREG BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 329 "parser.y"
      {
          bx_dbg_trace_reg_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2523 "y.tab.c"
    break;

  case 82: /* trace_mem_command: BX_TOKEN_TRACEMEM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 337 "parser.y"
      {
          bx_dbg_trace_mem_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2532 "y.tab.c"
    break;

  case 83: /* print_stack_command: BX_TOKEN_PRINT_STACK '\n'  */
#line 345 "parser.y"
      {
          bx_dbg_print_stack_command(16);
          free((yyvsp[-1].sval));
      }
#line 2541 "y.tab.c"
    break;

  case 84: /* print_stack_command: BX_TOKEN_PRINT_STACK BX_TOKEN_NUMERIC '\n'  */
#line 350 "parser.y"
      {
          bx_dbg_print_stack_command((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2550 "y.tab.c"
    break;

  case 85: /* backtrace_command: BX_TOKEN_BT '\n'  */
#line 358 "parser.y"
      {
        bx_dbg_bt_command(16);
        free((yyvsp[-1].sval));
      }
#line 2559 "y.tab.c"
    break;

  case 86: /* backtrace_command: BX_TOKEN_BT BX_TOKEN_NUMERIC '\n'  */
#line 363 "parser.y"
      {
        bx_dbg_bt_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2568 "y.tab.c"
    break;

  case 87: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_STOP '\n'  */
#line 371 "parser.y"
      {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2577 "y.tab.c"
    break;

  case 88: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_CONTINUE '\n'  */
#line 376 "parser.y"
      {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2586 "y.tab.c"
    break;

  case 89: /* watch_point_command: BX_TOKEN_WATCH '\n'  */
#line 381 "parser.y"
      {
          bx_dbg_print_watchpoints();
          free((yyvsp[-1].sval));
      }
#line 2595 "y.tab.c"
    break;

  case 90: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression '\n'  */
#line 386 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2604 "y.tab.c"
    break;

  case 91: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression '\n'  */
#line 391 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2613 "y.tab.c"
    break;

  case 92: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression '\n'  */
#line 396 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-1].uval), 1); /* BX_WRITE */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2622 "y.tab.c"
    break;

  case 93: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_R expression expression '\n'  */
#line 401 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2631 "y.tab.c"
    break;

  case 94: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_READ expression expression '\n'  */
#line 406 "parser.y"
      {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2640 "y.tab.c"
    break;

  case 95: /* watch_point_command: BX_TOKEN_WATCH BX_TOKEN_WRITE expression expression '\n'  */
#line 411 "parser.y"
      {
          bx_dbg_watch(1, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_WRITE */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2649 "y.tab.c"
    break;

  case 96: /* watch_point_command: BX_TOKEN_UNWATCH '\n'  */
#line 416 "parser.y"
      {
          bx_dbg_unwatch_all();
          free((yyvsp[-1].sval));
      }
#line 2658 "y.tab.c"
    break;

  case 97: /* watch_point_command: BX_TOKEN_UNWATCH expression '\n'  */
#line 421 "parser.y"
      {
          bx_dbg_unwatch((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2667 "y.tab.c"
    break;

  case 98: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 429 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2676 "y.tab.c"
    break;

  case 99: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_STRING expression '\n'  */
#line 434 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 0, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2685 "y.tab.c"
    break;

  case 100: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING '\n'  */
#line 439 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-1].sval), 1, 0);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2694 "y.tab.c"
    break;

  case 101: /* symbol_command: BX_TOKEN_LOAD_SYMBOLS BX_TOKEN_GLOBAL BX_TOKEN_STRING expression '\n'  */
#line 444 "parser.y"
      {
        bx_dbg_symbol_command((yyvsp[-2].sval), 1, (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2703 "y.tab.c"
    break;

  case 102: /* where_command: BX_TOKEN_WHERE '\n'  */
#line 452 "parser.y"
      {
        bx_dbg_where_command();
        free((yyvsp[-1].sval));
      }
#line 2712 "y.tab.c"
    break;

  case 103: /* print_string_command: BX_TOKEN_PRINT_STRING expression '\n'  */
#line 460 "parser.y"
      {
        bx_dbg_print_string_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2721 "y.tab.c"
    break;

  case 104: /* continue_command: BX_TOKEN_CONTINUE '\n'  */
#line 468 "parser.y"
      {
        bx_dbg_continue_command(1);
        free((yyvsp[-1].sval));
      }
#line 2730 "y.tab.c"
    break;

  case 105: /* continue_command: BX_TOKEN_CONTINUE BX_TOKEN_IF expression '\n'  */
#line 473 "parser.y"
      {
        bx_dbg_continue_command((yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2739 "y.tab.c"
    break;

  case 106: /* stepN_command: BX_TOKEN_STEPN '\n'  */
#line 481 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[-1].sval));
      }
#line 2748 "y.tab.c"
    break;

  case 107: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC '\n'  */
#line 486 "parser.y"
      {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2757 "y.tab.c"
    break;

  case 108: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_ALL BX_TOKEN_NUMERIC '\n'  */
#line 491 "parser.y"
      {
        bx_dbg_stepN_command(-1, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2766 "y.tab.c"
    break;

  case 109: /* stepN_command: BX_TOKEN_STEPN BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\n'  */
#line 496 "parser.y"
      {
        bx_dbg_stepN_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2775 "y.tab.c"
    break;

  case 110: /* step_over_command: BX_TOKEN_STEP_OVER '\n'  */
#line 504 "parser.y"
      {
        bx_dbg_step_over_command();
        free((yyvsp[-1].sval));
      }
#line 2784 "y.tab.c"
    break;

  case 111: /* set_command: BX_TOKEN_SET BX_TOKEN_DISASM BX_TOKEN_TOGGLE_ON_OFF '\n'  */
#line 512 "parser.y"
      {
        bx_dbg_set_auto_disassemble((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2793 "y.tab.c"
    break;

  case 112: /* set_command: BX_TOKEN_SET BX_TOKEN_SYMBOLNAME '=' expression '\n'  */
#line 517 "parser.y"
      {
        bx_dbg_set_symbol_command((yyvsp[-3].sval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2802 "y.tab.c"
    break;

  case 113: /* set_command: BX_TOKEN_SET BX_TOKEN_8BL_REG '=' expression '\n'  */
#line 522 "parser.y"
      { 
        bx_dbg_set_reg8l_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2810 "y.tab.c"
    break;

  case 114: /* set_command: BX_TOKEN_SET BX_TOKEN_8BH_REG '=' expression '\n'  */
#line 526 "parser.y"
      { 
        bx_dbg_set_reg8h_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2818 "y.tab.c"
    break;

  case 115: /* set_command: BX_TOKEN_SET BX_TOKEN_16B_REG '=' expression '\n'  */
#line 530 "parser.y"
      { 
        bx_dbg_set_reg16_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2826 "y.tab.c"
    break;

  case 116: /* set_command: BX_TOKEN_SET BX_TOKEN_32B_REG '=' expression '\n'  */
#line 534 "parser.y"
      { 
        bx_dbg_set_reg32_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2834 "y.tab.c"
    break;

  case 117: /* set_command: BX_TOKEN_SET BX_TOKEN_64B_REG '=' expression '\n'  */
#line 538 "parser.y"
      { 
        bx_dbg_set_reg64_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2842 "y.tab.c"
    break;

  case 118: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_EIP '=' expression '\n'  */
#line 542 "parser.y"
      { 
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2850 "y.tab.c"
    break;

  case 119: /* set_command: BX_TOKEN_SET BX_TOKEN_REG_RIP '=' expression '\n'  */
#line 546 "parser.y"
      { 
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2858 "y.tab.c"
    break;

  case 120: /* set_command: BX_TOKEN_SET BX_TOKEN_SEGREG '=' expression '\n'  */
#line 550 "parser.y"
      { 
        bx_dbg_load_segreg((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2866 "y.tab.c"
    break;

  case 121: /* breakpoint_command: BX_TOKEN_VBREAKPOINT '\n'  */
#line 557 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2875 "y.tab.c"
    break;

  case 122: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression '\n'  */
#line 562 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].uval), NULL);
        free((yyvsp[-4].sval));
      }
#line 2884 "y.tab.c"
    break;

  case 123: /* breakpoint_command: BX_TOKEN_VBREAKPOINT vexpression ':' vexpression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 567 "parser.y"
      {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-5].uval), (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-6].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2893 "y.tab.c"
    break;

  case 124: /* breakpoint_command: BX_TOKEN_LBREAKPOINT '\n'  */
#line 572 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2902 "y.tab.c"
    break;

  case 125: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression '\n'  */
#line 577 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 2911 "y.tab.c"
    break;

  case 126: /* breakpoint_command: BX_TOKEN_LBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 582 "parser.y"
      {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2920 "y.tab.c"
    break;

  case 127: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING '\n'  */
#line 587 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-1].sval), NULL);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2929 "y.tab.c"
    break;

  case 128: /* breakpoint_command: BX_TOKEN_LBREAKPOINT BX_TOKEN_STRING BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 592 "parser.y"
      {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-3].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2938 "y.tab.c"
    break;

  case 129: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '\n'  */
#line 597 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2947 "y.tab.c"
    break;

  case 130: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression '\n'  */
#line 602 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 2956 "y.tab.c"
    break;

  case 131: /* breakpoint_command: BX_TOKEN_PBREAKPOINT expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 607 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2965 "y.tab.c"
    break;

  case 132: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression '\n'  */
#line 612 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-3].sval));
      }
#line 2974 "y.tab.c"
    break;

  case 133: /* breakpoint_command: BX_TOKEN_PBREAKPOINT '*' expression BX_TOKEN_IF BX_TOKEN_STRING '\n'  */
#line 617 "parser.y"
      {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-5].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2983 "y.tab.c"
    break;

  case 134: /* blist_command: BX_TOKEN_LIST_BREAK '\n'  */
#line 625 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-1].sval));
      }
#line 2992 "y.tab.c"
    break;

  case 135: /* slist_command: BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 633 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-1].sval));
      }
#line 3001 "y.tab.c"
    break;

  case 136: /* slist_command: BX_TOKEN_LIST_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 638 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 3010 "y.tab.c"
    break;

  case 137: /* info_command: BX_TOKEN_INFO BX_TOKEN_PBREAKPOINT '\n'  */
#line 646 "parser.y"
      {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3019 "y.tab.c"
    break;

  case 138: /* info_command: BX_TOKEN_INFO BX_TOKEN_CPU '\n'  */
#line 651 "parser.y"
      {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3028 "y.tab.c"
    break;

  case 139: /* info_command: BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric optional_numeric '\n'  */
#line 656 "parser.y"
      {
        bx_dbg_info_idt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3037 "y.tab.c"
    break;

  case 140: /* info_command: BX_TOKEN_INFO BX_TOKEN_IVT optional_numeric optional_numeric '\n'  */
#line 661 "parser.y"
      {
        bx_dbg_info_ivt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3046 "y.tab.c"
    break;

  case 141: /* info_command: BX_TOKEN_INFO BX_TOKEN_GDT optional_numeric optional_numeric '\n'  */
#line 666 "parser.y"
      {
        bx_dbg_info_gdt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3055 "y.tab.c"
    break;

  case 142: /* info_command: BX_TOKEN_INFO BX_TOKEN_LDT optional_numeric optional_numeric '\n'  */
#line 671 "parser.y"
      {
        bx_dbg_info_ldt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3064 "y.tab.c"
    break;

  case 143: /* info_command: BX_TOKEN_INFO BX_TOKEN_TAB '\n'  */
#line 676 "parser.y"
      {
        bx_dbg_dump_table();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3073 "y.tab.c"
    break;

  case 144: /* info_command: BX_TOKEN_INFO BX_TOKEN_TSS '\n'  */
#line 681 "parser.y"
      {
        bx_dbg_info_tss_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3082 "y.tab.c"
    break;

  case 145: /* info_command: BX_TOKEN_INFO BX_TOKEN_FLAGS '\n'  */
#line 686 "parser.y"
      {
        bx_dbg_info_flags();
        free((yyvsp[-2].sval));
      }
#line 3091 "y.tab.c"
    break;

  case 146: /* info_command: BX_TOKEN_INFO BX_TOKEN_LINUX '\n'  */
#line 691 "parser.y"
      {
        bx_dbg_info_linux_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3100 "y.tab.c"
    break;

  case 147: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS '\n'  */
#line 696 "parser.y"
      {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3109 "y.tab.c"
    break;

  case 148: /* info_command: BX_TOKEN_INFO BX_TOKEN_SYMBOLS BX_TOKEN_STRING '\n'  */
#line 701 "parser.y"
      {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3118 "y.tab.c"
    break;

  case 149: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE '\n'  */
#line 706 "parser.y"
      {
        bx_dbg_info_device("", "");
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3127 "y.tab.c"
    break;

  case 150: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC '\n'  */
#line 711 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3136 "y.tab.c"
    break;

  case 151: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_GENERIC BX_TOKEN_STRING '\n'  */
#line 716 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3145 "y.tab.c"
    break;

  case 152: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING '\n'  */
#line 721 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3154 "y.tab.c"
    break;

  case 153: /* info_command: BX_TOKEN_INFO BX_TOKEN_DEVICE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 726 "parser.y"
      {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3163 "y.tab.c"
    break;

  case 154: /* optional_numeric: %empty  */
#line 733 "parser.y"
               { (yyval.uval) = EMPTY_ARG; }
#line 3169 "y.tab.c"
    break;

  case 156: /* regs_command: BX_TOKEN_REGISTERS '\n'  */
#line 738 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3178 "y.tab.c"
    break;

  case 157: /* fpu_regs_command: BX_TOKEN_FPU '\n'  */
#line 746 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[-1].sval));
      }
#line 3187 "y.tab.c"
    break;

  case 158: /* mmx_regs_command: BX_TOKEN_MMX '\n'  */
#line 754 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3196 "y.tab.c"
    break;

  case 159: /* xmm_regs_command: BX_TOKEN_XMM '\n'  */
#line 762 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3205 "y.tab.c"
    break;

  case 160: /* ymm_regs_command: BX_TOKEN_YMM '\n'  */
#line 770 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3214 "y.tab.c"
    break;

  case 161: /* zmm_regs_command: BX_TOKEN_ZMM '\n'  */
#line 778 "parser.y"
      {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3223 "y.tab.c"
    break;

  case 162: /* segment_regs_command: BX_TOKEN_SEGMENT_REGS '\n'  */
#line 786 "parser.y"
      {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3232 "y.tab.c"
    break;

  case 163: /* control_regs_command: BX_TOKEN_CONTROL_REGS '\n'  */
#line 794 "parser.y"
      {
        bx_dbg_info_control_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3241 "y.tab.c"
    break;

  case 164: /* debug_regs_command: BX_TOKEN_DEBUG_REGS '\n'  */
#line 802 "parser.y"
      {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3250 "y.tab.c"
    break;

  case 165: /* delete_command: BX_TOKEN_DEL_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 810 "parser.y"
      {
        bx_dbg_del_breakpoint_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3259 "y.tab.c"
    break;

  case 166: /* bpe_command: BX_TOKEN_ENABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 818 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3268 "y.tab.c"
    break;

  case 167: /* bpd_command: BX_TOKEN_DISABLE_BREAKPOINT BX_TOKEN_NUMERIC '\n'  */
#line 825 "parser.y"
      {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 0);
        free((yyvsp[-2].sval));
      }
#line 3277 "y.tab.c"
    break;

  case 168: /* quit_command: BX_TOKEN_QUIT '\n'  */
#line 833 "parser.y"
      {
        bx_dbg_quit_command();
        free((yyvsp[-1].sval));
      }
#line 3286 "y.tab.c"
    break;

  case 169: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT expression '\n'  */
#line 841 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-3].sval), (yyvsp[-2].sval),1, (yyvsp[-1].uval), 1);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3295 "y.tab.c"
    break;

  case 170: /* examine_command: BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT '\n'  */
#line 846 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), (yyvsp[-1].sval),1, 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3304 "y.tab.c"
    break;

  case 171: /* examine_command: BX_TOKEN_EXAMINE expression '\n'  */
#line 851 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-2].sval), NULL,0, (yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3313 "y.tab.c"
    break;

  case 172: /* examine_command: BX_TOKEN_EXAMINE '\n'  */
#line 856 "parser.y"
      {
        bx_dbg_examine_command((yyvsp[-1].sval), NULL,0, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 3322 "y.tab.c"
    break;

  case 173: /* restore_command: BX_TOKEN_RESTORE BX_TOKEN_STRING BX_TOKEN_STRING '\n'  */
#line 864 "parser.y"
      {
        bx_dbg_restore_command((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3331 "y.tab.c"
    break;

  case 174: /* writemem_command: BX_TOKEN_WRITEMEM BX_TOKEN_STRING expression expression '\n'  */
#line 872 "parser.y"
      {
        bx_dbg_writemem_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3340 "y.tab.c"
    break;

  case 175: /* setpmem_command: BX_TOKEN_SETPMEM expression expression expression '\n'  */
#line 880 "parser.y"
      {
        bx_dbg_setpmem_command((yyvsp[-3].uval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3349 "y.tab.c"
    break;

  case 176: /* query_command: BX_TOKEN_QUERY BX_TOKEN_PENDING '\n'  */
#line 888 "parser.y"
      {
        bx_dbg_query_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3358 "y.tab.c"
    break;

  case 177: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA '\n'  */
#line 896 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3367 "y.tab.c"
    break;

  case 178: /* take_command: BX_TOKEN_TAKE BX_TOKEN_DMA BX_TOKEN_NUMERIC '\n'  */
#line 901 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3376 "y.tab.c"
    break;

  case 179: /* take_command: BX_TOKEN_TAKE BX_TOKEN_IRQ '\n'  */
#line 906 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3385 "y.tab.c"
    break;

  case 180: /* take_command: BX_TOKEN_TAKE BX_TOKEN_SMI '\n'  */
#line 911 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3394 "y.tab.c"
    break;

  case 181: /* take_command: BX_TOKEN_TAKE BX_TOKEN_NMI '\n'  */
#line 916 "parser.y"
      {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3403 "y.tab.c"
    break;

  case 182: /* disassemble_command: BX_TOKEN_DISASM '\n'  */
#line 924 "parser.y"
      {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[-1].sval));
      }
#line 3412 "y.tab.c"
    break;

  case 183: /* disassemble_command: BX_TOKEN_DISASM expression '\n'  */
#line 929 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3421 "y.tab.c"
    break;

  case 184: /* disassemble_command: BX_TOKEN_DISASM expression expression '\n'  */
#line 934 "parser.y"
      {
        bx_dbg_disassemble_command(NULL, (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3430 "y.tab.c"
    break;

  case 185: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT '\n'  */
#line 939 "parser.y"
      {
        bx_dbg_disassemble_current((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3439 "y.tab.c"
    break;

  case 186: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression '\n'  */
#line 944 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-2].sval), (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3448 "y.tab.c"
    break;

  case 187: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_DISFORMAT expression expression '\n'  */
#line 949 "parser.y"
      {
        bx_dbg_disassemble_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3457 "y.tab.c"
    break;

  case 188: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SWITCH_MODE '\n'  */
#line 954 "parser.y"
      {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3466 "y.tab.c"
    break;

  case 189: /* disassemble_command: BX_TOKEN_DISASM BX_TOKEN_SIZE '=' BX_TOKEN_NUMERIC '\n'  */
#line 959 "parser.y"
      {
        bx_dbg_set_disassemble_size((yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3475 "y.tab.c"
    break;

  case 190: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STOP '\n'  */
#line 967 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3484 "y.tab.c"
    break;

  case 191: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_STRING '\n'  */
#line 973 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3493 "y.tab.c"
    break;

  case 192: /* instrument_command: BX_TOKEN_INSTRUMENT BX_TOKEN_GENERIC '\n'  */
#line 978 "parser.y"
      {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3502 "y.tab.c"
    break;

  case 193: /* doit_command: BX_TOKEN_DOIT expression '\n'  */
#line 986 "parser.y"
      {
        bx_dbg_doit_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3511 "y.tab.c"
    break;

  case 194: /* crc_command: BX_TOKEN_CRC expression expression '\n'  */
#line 994 "parser.y"
      {
        bx_dbg_crc_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3520 "y.tab.c"
    break;

  case 195: /* help_command: BX_TOKEN_HELP BX_TOKEN_QUIT '\n'  */
#line 1002 "parser.y"
       {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3529 "y.tab.c"
    break;

  case 196: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTINUE '\n'  */
#line 1007 "parser.y"
       {
         dbg_printf("c|cont|continue - continue executing\n");
         dbg_printf("c|cont|continue if \"expression\" - continue executing only if expression is true\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3539 "y.tab.c"
    break;

  case 197: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEPN '\n'  */
#line 1013 "parser.y"
       {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3550 "y.tab.c"
    break;

  case 198: /* help_command: BX_TOKEN_HELP BX_TOKEN_STEP_OVER '\n'  */
#line 1020 "parser.y"
       {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3559 "y.tab.c"
    break;

  case 199: /* help_command: BX_TOKEN_HELP BX_TOKEN_VBREAKPOINT '\n'  */
#line 1025 "parser.y"
       {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         dbg_printf("vb|vbreak <seg:offset> if \"expression\" - set a conditional virtual address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3569 "y.tab.c"
    break;

  case 200: /* help_command: BX_TOKEN_HELP BX_TOKEN_LBREAKPOINT '\n'  */
#line 1031 "parser.y"
       {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         dbg_printf("lb|lbreak <addr> if \"expression\" - set a conditional linear address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3579 "y.tab.c"
    break;

  case 201: /* help_command: BX_TOKEN_HELP BX_TOKEN_PBREAKPOINT '\n'  */
#line 1037 "parser.y"
       {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         dbg_printf("p|pb|break|pbreak <addr> if \"expression\" - set a conditional physical address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3589 "y.tab.c"
    break;

  case 202: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEL_BREAKPOINT '\n'  */
#line 1043 "parser.y"
       {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3598 "y.tab.c"
    break;

  case 203: /* help_command: BX_TOKEN_HELP BX_TOKEN_ENABLE_BREAKPOINT '\n'  */
#line 1048 "parser.y"
       {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3607 "y.tab.c"
    break;

  case 204: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISABLE_BREAKPOINT '\n'  */
#line 1053 "parser.y"
       {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3616 "y.tab.c"
    break;

  case 205: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_BREAK '\n'  */
#line 1058 "parser.y"
       {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3625 "y.tab.c"
    break;

  case 206: /* help_command: BX_TOKEN_HELP BX_TOKEN_MODEBP '\n'  */
#line 1063 "parser.y"
       {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3634 "y.tab.c"
    break;

  case 207: /* help_command: BX_TOKEN_HELP BX_TOKEN_VMEXITBP '\n'  */
#line 1068 "parser.y"
       {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3643 "y.tab.c"
    break;

  case 208: /* help_command: BX_TOKEN_HELP BX_TOKEN_CRC '\n'  */
#line 1073 "parser.y"
       {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3652 "y.tab.c"
    break;

  case 209: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACE '\n'  */
#line 1078 "parser.y"
       {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3662 "y.tab.c"
    break;

  case 210: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEREG '\n'  */
#line 1084 "parser.y"
       {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3672 "y.tab.c"
    break;

  case 211: /* help_command: BX_TOKEN_HELP BX_TOKEN_TRACEMEM '\n'  */
#line 1090 "parser.y"
       {
         dbg_printf("trace-mem on  - print all memory accesses occurred during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3682 "y.tab.c"
    break;

  case 212: /* help_command: BX_TOKEN_HELP BX_TOKEN_RESTORE '\n'  */
#line 1096 "parser.y"
       {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3694 "y.tab.c"
    break;

  case 213: /* help_command: BX_TOKEN_HELP BX_TOKEN_PTIME '\n'  */
#line 1104 "parser.y"
       {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3703 "y.tab.c"
    break;

  case 214: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP '\n'  */
#line 1109 "parser.y"
       {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3712 "y.tab.c"
    break;

  case 215: /* help_command: BX_TOKEN_HELP BX_TOKEN_TIMEBP_ABSOLUTE '\n'  */
#line 1114 "parser.y"
       {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3721 "y.tab.c"
    break;

  case 216: /* help_command: BX_TOKEN_HELP BX_TOKEN_PRINT_STACK '\n'  */
#line 1119 "parser.y"
       {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3730 "y.tab.c"
    break;

  case 217: /* help_command: BX_TOKEN_HELP BX_TOKEN_LOAD_SYMBOLS '\n'  */
#line 1124 "parser.y"
       {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3739 "y.tab.c"
    break;

  case 218: /* help_command: BX_TOKEN_HELP BX_TOKEN_LIST_SYMBOLS '\n'  */
#line 1129 "parser.y"
       {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3748 "y.tab.c"
    break;

  case 219: /* help_command: BX_TOKEN_HELP BX_TOKEN_REGISTERS '\n'  */
#line 1134 "parser.y"
       {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3757 "y.tab.c"
    break;

  case 220: /* help_command: BX_TOKEN_HELP BX_TOKEN_FPU '\n'  */
#line 1139 "parser.y"
       {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3766 "y.tab.c"
    break;

  case 221: /* help_command: BX_TOKEN_HELP BX_TOKEN_MMX '\n'  */
#line 1144 "parser.y"
       {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3775 "y.tab.c"
    break;

  case 222: /* help_command: BX_TOKEN_HELP BX_TOKEN_XMM '\n'  */
#line 1149 "parser.y"
       {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3784 "y.tab.c"
    break;

  case 223: /* help_command: BX_TOKEN_HELP BX_TOKEN_YMM '\n'  */
#line 1154 "parser.y"
       {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3793 "y.tab.c"
    break;

  case 224: /* help_command: BX_TOKEN_HELP BX_TOKEN_ZMM '\n'  */
#line 1159 "parser.y"
       {
         dbg_printf("zmm - print AVX-512 state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3802 "y.tab.c"
    break;

  case 225: /* help_command: BX_TOKEN_HELP BX_TOKEN_SEGMENT_REGS '\n'  */
#line 1164 "parser.y"
       {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3811 "y.tab.c"
    break;

  case 226: /* help_command: BX_TOKEN_HELP BX_TOKEN_CONTROL_REGS '\n'  */
#line 1169 "parser.y"
       {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3820 "y.tab.c"
    break;

  case 227: /* help_command: BX_TOKEN_HELP BX_TOKEN_DEBUG_REGS '\n'  */
#line 1174 "parser.y"
       {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3829 "y.tab.c"
    break;

  case 228: /* help_command: BX_TOKEN_HELP BX_TOKEN_WRITEMEM '\n'  */
#line 1179 "parser.y"
       {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3838 "y.tab.c"
    break;

  case 229: /* help_command: BX_TOKEN_HELP BX_TOKEN_SETPMEM '\n'  */
#line 1184 "parser.y"
       {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3847 "y.tab.c"
    break;

  case 230: /* help_command: BX_TOKEN_HELP BX_TOKEN_DISASM '\n'  */
#line 1189 "parser.y"
       {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm hex on/off - control disasm offsets and displacements format\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3861 "y.tab.c"
    break;

  case 231: /* help_command: BX_TOKEN_HELP BX_TOKEN_WATCH '\n'  */
#line 1199 "parser.y"
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
#line 3876 "y.tab.c"
    break;

  case 232: /* help_command: BX_TOKEN_HELP BX_TOKEN_UNWATCH '\n'  */
#line 1210 "parser.y"
       {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3886 "y.tab.c"
    break;

  case 233: /* help_command: BX_TOKEN_HELP BX_TOKEN_EXAMINE '\n'  */
#line 1216 "parser.y"
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
#line 3903 "y.tab.c"
    break;

  case 234: /* help_command: BX_TOKEN_HELP BX_TOKEN_INSTRUMENT '\n'  */
#line 1229 "parser.y"
       {
         dbg_printf("instrument <command|\"string command\"> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command|\"string command\">\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3912 "y.tab.c"
    break;

  case 235: /* help_command: BX_TOKEN_HELP BX_TOKEN_SET '\n'  */
#line 1234 "parser.y"
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
#line 3927 "y.tab.c"
    break;

  case 236: /* help_command: BX_TOKEN_HELP BX_TOKEN_PAGE '\n'  */
#line 1245 "parser.y"
       {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3936 "y.tab.c"
    break;

  case 237: /* help_command: BX_TOKEN_HELP BX_TOKEN_INFO '\n'  */
#line 1250 "parser.y"
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
#line 3956 "y.tab.c"
    break;

  case 238: /* help_command: BX_TOKEN_HELP BX_TOKEN_SHOW '\n'  */
#line 1266 "parser.y"
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
#line 3976 "y.tab.c"
    break;

  case 239: /* help_command: BX_TOKEN_HELP BX_TOKEN_CALC '\n'  */
#line 1282 "parser.y"
       {
         dbg_printf("calc|? <expr> - calculate a expression and display the result.\n");
         dbg_printf("    'expr' can reference any general-purpose, opmask and segment\n");
         dbg_printf("    registers, use any arithmetic and logic operations, and\n");
         dbg_printf("    also the special ':' operator which computes the linear\n");
         dbg_printf("    address of a segment:offset (in real and v86 mode) or of\n");
         dbg_printf("    a selector:offset (in protected mode) pair.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3990 "y.tab.c"
    break;

  case 240: /* help_command: BX_TOKEN_HELP BX_TOKEN_HELP '\n'  */
#line 1292 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3999 "y.tab.c"
    break;

  case 241: /* help_command: BX_TOKEN_HELP '\n'  */
#line 1297 "parser.y"
       {
         bx_dbg_print_help();
         free((yyvsp[-1].sval));
       }
#line 4008 "y.tab.c"
    break;

  case 242: /* calc_command: BX_TOKEN_CALC expression '\n'  */
#line 1305 "parser.y"
   {
     eval_value = (yyvsp[-1].uval);
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4018 "y.tab.c"
    break;

  case 243: /* if_command: BX_TOKEN_IF expression '\n'  */
#line 1314 "parser.y"
   {
     eval_value = (yyvsp[-1].uval) != 0;
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 4028 "y.tab.c"
    break;

  case 244: /* vexpression: BX_TOKEN_NUMERIC  */
#line 1323 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4034 "y.tab.c"
    break;

  case 245: /* vexpression: BX_TOKEN_STRING  */
#line 1324 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4040 "y.tab.c"
    break;

  case 246: /* vexpression: BX_TOKEN_8BL_REG  */
#line 1325 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4046 "y.tab.c"
    break;

  case 247: /* vexpression: BX_TOKEN_8BH_REG  */
#line 1326 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4052 "y.tab.c"
    break;

  case 248: /* vexpression: BX_TOKEN_16B_REG  */
#line 1327 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4058 "y.tab.c"
    break;

  case 249: /* vexpression: BX_TOKEN_32B_REG  */
#line 1328 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4064 "y.tab.c"
    break;

  case 250: /* vexpression: BX_TOKEN_64B_REG  */
#line 1329 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4070 "y.tab.c"
    break;

  case 251: /* vexpression: BX_TOKEN_OPMASK_REG  */
#line 1330 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4076 "y.tab.c"
    break;

  case 252: /* vexpression: BX_TOKEN_SEGREG  */
#line 1331 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4082 "y.tab.c"
    break;

  case 253: /* vexpression: BX_TOKEN_REG_IP  */
#line 1332 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4088 "y.tab.c"
    break;

  case 254: /* vexpression: BX_TOKEN_REG_EIP  */
#line 1333 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4094 "y.tab.c"
    break;

  case 255: /* vexpression: BX_TOKEN_REG_RIP  */
#line 1334 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4100 "y.tab.c"
    break;

  case 256: /* vexpression: BX_TOKEN_REG_SSP  */
#line 1335 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4106 "y.tab.c"
    break;

  case 257: /* vexpression: vexpression '+' vexpression  */
#line 1336 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4112 "y.tab.c"
    break;

  case 258: /* vexpression: vexpression '-' vexpression  */
#line 1337 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4118 "y.tab.c"
    break;

  case 259: /* vexpression: vexpression '*' vexpression  */
#line 1338 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4124 "y.tab.c"
    break;

  case 260: /* vexpression: vexpression '/' vexpression  */
#line 1339 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) / (yyvsp[0].uval); }
#line 4130 "y.tab.c"
    break;

  case 261: /* vexpression: vexpression BX_TOKEN_RSHIFT vexpression  */
#line 1340 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4136 "y.tab.c"
    break;

  case 262: /* vexpression: vexpression BX_TOKEN_LSHIFT vexpression  */
#line 1341 "parser.y"
                                             { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4142 "y.tab.c"
    break;

  case 263: /* vexpression: vexpression '|' vexpression  */
#line 1342 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4148 "y.tab.c"
    break;

  case 264: /* vexpression: vexpression '^' vexpression  */
#line 1343 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4154 "y.tab.c"
    break;

  case 265: /* vexpression: vexpression '&' vexpression  */
#line 1344 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4160 "y.tab.c"
    break;

  case 266: /* vexpression: '!' vexpression  */
#line 1345 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4166 "y.tab.c"
    break;

  case 267: /* vexpression: '-' vexpression  */
#line 1346 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4172 "y.tab.c"
    break;

  case 268: /* vexpression: '(' vexpression ')'  */
#line 1347 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4178 "y.tab.c"
    break;

  case 269: /* expression: BX_TOKEN_NUMERIC  */
#line 1353 "parser.y"
                                     { (yyval.uval) = (yyvsp[0].uval); }
#line 4184 "y.tab.c"
    break;

  case 270: /* expression: BX_TOKEN_STRING  */
#line 1354 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4190 "y.tab.c"
    break;

  case 271: /* expression: BX_TOKEN_8BL_REG  */
#line 1355 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4196 "y.tab.c"
    break;

  case 272: /* expression: BX_TOKEN_8BH_REG  */
#line 1356 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4202 "y.tab.c"
    break;

  case 273: /* expression: BX_TOKEN_16B_REG  */
#line 1357 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4208 "y.tab.c"
    break;

  case 274: /* expression: BX_TOKEN_32B_REG  */
#line 1358 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4214 "y.tab.c"
    break;

  case 275: /* expression: BX_TOKEN_64B_REG  */
#line 1359 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4220 "y.tab.c"
    break;

  case 276: /* expression: BX_TOKEN_OPMASK_REG  */
#line 1360 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4226 "y.tab.c"
    break;

  case 277: /* expression: BX_TOKEN_SEGREG  */
#line 1361 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4232 "y.tab.c"
    break;

  case 278: /* expression: BX_TOKEN_REG_IP  */
#line 1362 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ip (); }
#line 4238 "y.tab.c"
    break;

  case 279: /* expression: BX_TOKEN_REG_EIP  */
#line 1363 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_eip(); }
#line 4244 "y.tab.c"
    break;

  case 280: /* expression: BX_TOKEN_REG_RIP  */
#line 1364 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_rip(); }
#line 4250 "y.tab.c"
    break;

  case 281: /* expression: BX_TOKEN_REG_SSP  */
#line 1365 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4256 "y.tab.c"
    break;

  case 282: /* expression: expression ':' expression  */
#line 1366 "parser.y"
                                     { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[-2].uval), (yyvsp[0].uval)); }
#line 4262 "y.tab.c"
    break;

  case 283: /* expression: expression '+' expression  */
#line 1367 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4268 "y.tab.c"
    break;

  case 284: /* expression: expression '-' expression  */
#line 1368 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4274 "y.tab.c"
    break;

  case 285: /* expression: expression '*' expression  */
#line 1369 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4280 "y.tab.c"
    break;

  case 286: /* expression: expression '/' expression  */
#line 1370 "parser.y"
                                     { (yyval.uval) = ((yyvsp[0].uval) != 0) ? (yyvsp[-2].uval) / (yyvsp[0].uval) : 0; }
#line 4286 "y.tab.c"
    break;

  case 287: /* expression: expression BX_TOKEN_RSHIFT expression  */
#line 1371 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4292 "y.tab.c"
    break;

  case 288: /* expression: expression BX_TOKEN_LSHIFT expression  */
#line 1372 "parser.y"
                                           { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4298 "y.tab.c"
    break;

  case 289: /* expression: expression '|' expression  */
#line 1373 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4304 "y.tab.c"
    break;

  case 290: /* expression: expression '^' expression  */
#line 1374 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4310 "y.tab.c"
    break;

  case 291: /* expression: expression '&' expression  */
#line 1375 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4316 "y.tab.c"
    break;

  case 292: /* expression: expression '>' expression  */
#line 1376 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) > (yyvsp[0].uval); }
#line 4322 "y.tab.c"
    break;

  case 293: /* expression: expression '<' expression  */
#line 1377 "parser.y"
                                     { (yyval.uval) = (yyvsp[-2].uval) < (yyvsp[0].uval); }
#line 4328 "y.tab.c"
    break;

  case 294: /* expression: expression BX_TOKEN_EQ expression  */
#line 1378 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) == (yyvsp[0].uval); }
#line 4334 "y.tab.c"
    break;

  case 295: /* expression: expression BX_TOKEN_NE expression  */
#line 1379 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) != (yyvsp[0].uval); }
#line 4340 "y.tab.c"
    break;

  case 296: /* expression: expression BX_TOKEN_LE expression  */
#line 1380 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) <= (yyvsp[0].uval); }
#line 4346 "y.tab.c"
    break;

  case 297: /* expression: expression BX_TOKEN_GE expression  */
#line 1381 "parser.y"
                                       { (yyval.uval) = (yyvsp[-2].uval) >= (yyvsp[0].uval); }
#line 4352 "y.tab.c"
    break;

  case 298: /* expression: '!' expression  */
#line 1382 "parser.y"
                                     { (yyval.uval) = !(yyvsp[0].uval); }
#line 4358 "y.tab.c"
    break;

  case 299: /* expression: '-' expression  */
#line 1383 "parser.y"
                                     { (yyval.uval) = -(yyvsp[0].uval); }
#line 4364 "y.tab.c"
    break;

  case 300: /* expression: '*' expression  */
#line 1384 "parser.y"
                                     { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[0].uval)); }
#line 4370 "y.tab.c"
    break;

  case 301: /* expression: '@' expression  */
#line 1385 "parser.y"
                                     { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[0].uval)); }
#line 4376 "y.tab.c"
    break;

  case 302: /* expression: '(' expression ')'  */
#line 1386 "parser.y"
                                     { (yyval.uval) = (yyvsp[-1].uval); }
#line 4382 "y.tab.c"
    break;


#line 4386 "y.tab.c"

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

#line 1389 "parser.y"

#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
