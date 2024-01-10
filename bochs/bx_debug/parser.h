/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

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

#line 327 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE bxlval;


int bxparse (void);


#endif /* !YY_BX_Y_TAB_H_INCLUDED  */
