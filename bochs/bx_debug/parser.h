/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_BX_Y_TAB_H_INCLUDED
# define YY_BX_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int bxdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    BX_TOKEN_8BH_REG = 258,
    BX_TOKEN_8BL_REG = 259,
    BX_TOKEN_16B_REG = 260,
    BX_TOKEN_32B_REG = 261,
    BX_TOKEN_64B_REG = 262,
    BX_TOKEN_CS = 263,
    BX_TOKEN_ES = 264,
    BX_TOKEN_SS = 265,
    BX_TOKEN_DS = 266,
    BX_TOKEN_FS = 267,
    BX_TOKEN_GS = 268,
    BX_TOKEN_OPMASK_REG = 269,
    BX_TOKEN_FLAGS = 270,
    BX_TOKEN_ON = 271,
    BX_TOKEN_OFF = 272,
    BX_TOKEN_CONTINUE = 273,
    BX_TOKEN_IF = 274,
    BX_TOKEN_STEPN = 275,
    BX_TOKEN_STEP_OVER = 276,
    BX_TOKEN_SET = 277,
    BX_TOKEN_DEBUGGER = 278,
    BX_TOKEN_LIST_BREAK = 279,
    BX_TOKEN_VBREAKPOINT = 280,
    BX_TOKEN_LBREAKPOINT = 281,
    BX_TOKEN_PBREAKPOINT = 282,
    BX_TOKEN_DEL_BREAKPOINT = 283,
    BX_TOKEN_ENABLE_BREAKPOINT = 284,
    BX_TOKEN_DISABLE_BREAKPOINT = 285,
    BX_TOKEN_INFO = 286,
    BX_TOKEN_QUIT = 287,
    BX_TOKEN_R = 288,
    BX_TOKEN_REGS = 289,
    BX_TOKEN_CPU = 290,
    BX_TOKEN_FPU = 291,
    BX_TOKEN_MMX = 292,
    BX_TOKEN_XMM = 293,
    BX_TOKEN_YMM = 294,
    BX_TOKEN_ZMM = 295,
    BX_TOKEN_AVX = 296,
    BX_TOKEN_IDT = 297,
    BX_TOKEN_IVT = 298,
    BX_TOKEN_GDT = 299,
    BX_TOKEN_LDT = 300,
    BX_TOKEN_TSS = 301,
    BX_TOKEN_TAB = 302,
    BX_TOKEN_ALL = 303,
    BX_TOKEN_LINUX = 304,
    BX_TOKEN_DEBUG_REGS = 305,
    BX_TOKEN_CONTROL_REGS = 306,
    BX_TOKEN_SEGMENT_REGS = 307,
    BX_TOKEN_EXAMINE = 308,
    BX_TOKEN_XFORMAT = 309,
    BX_TOKEN_DISFORMAT = 310,
    BX_TOKEN_RESTORE = 311,
    BX_TOKEN_WRITEMEM = 312,
    BX_TOKEN_SETPMEM = 313,
    BX_TOKEN_SYMBOLNAME = 314,
    BX_TOKEN_QUERY = 315,
    BX_TOKEN_PENDING = 316,
    BX_TOKEN_TAKE = 317,
    BX_TOKEN_DMA = 318,
    BX_TOKEN_IRQ = 319,
    BX_TOKEN_TLB = 320,
    BX_TOKEN_HEX = 321,
    BX_TOKEN_DISASM = 322,
    BX_TOKEN_INSTRUMENT = 323,
    BX_TOKEN_STRING = 324,
    BX_TOKEN_STOP = 325,
    BX_TOKEN_DOIT = 326,
    BX_TOKEN_CRC = 327,
    BX_TOKEN_TRACE = 328,
    BX_TOKEN_TRACEREG = 329,
    BX_TOKEN_TRACEMEM = 330,
    BX_TOKEN_SWITCH_MODE = 331,
    BX_TOKEN_SIZE = 332,
    BX_TOKEN_PTIME = 333,
    BX_TOKEN_TIMEBP_ABSOLUTE = 334,
    BX_TOKEN_TIMEBP = 335,
    BX_TOKEN_MODEBP = 336,
    BX_TOKEN_VMEXITBP = 337,
    BX_TOKEN_PRINT_STACK = 338,
    BX_TOKEN_BT = 339,
    BX_TOKEN_WATCH = 340,
    BX_TOKEN_UNWATCH = 341,
    BX_TOKEN_READ = 342,
    BX_TOKEN_WRITE = 343,
    BX_TOKEN_SHOW = 344,
    BX_TOKEN_LOAD_SYMBOLS = 345,
    BX_TOKEN_SYMBOLS = 346,
    BX_TOKEN_LIST_SYMBOLS = 347,
    BX_TOKEN_GLOBAL = 348,
    BX_TOKEN_WHERE = 349,
    BX_TOKEN_PRINT_STRING = 350,
    BX_TOKEN_NUMERIC = 351,
    BX_TOKEN_PAGE = 352,
    BX_TOKEN_HELP = 353,
    BX_TOKEN_XML = 354,
    BX_TOKEN_CALC = 355,
    BX_TOKEN_DEVICE = 356,
    BX_TOKEN_GENERIC = 357,
    BX_TOKEN_RSHIFT = 358,
    BX_TOKEN_LSHIFT = 359,
    BX_TOKEN_EQ = 360,
    BX_TOKEN_NE = 361,
    BX_TOKEN_LE = 362,
    BX_TOKEN_GE = 363,
    BX_TOKEN_REG_IP = 364,
    BX_TOKEN_REG_EIP = 365,
    BX_TOKEN_REG_RIP = 366,
    NOT = 367,
    NEG = 368,
    INDIRECT = 369
  };
#endif
/* Tokens.  */
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
#define BX_TOKEN_TLB 320
#define BX_TOKEN_HEX 321
#define BX_TOKEN_DISASM 322
#define BX_TOKEN_INSTRUMENT 323
#define BX_TOKEN_STRING 324
#define BX_TOKEN_STOP 325
#define BX_TOKEN_DOIT 326
#define BX_TOKEN_CRC 327
#define BX_TOKEN_TRACE 328
#define BX_TOKEN_TRACEREG 329
#define BX_TOKEN_TRACEMEM 330
#define BX_TOKEN_SWITCH_MODE 331
#define BX_TOKEN_SIZE 332
#define BX_TOKEN_PTIME 333
#define BX_TOKEN_TIMEBP_ABSOLUTE 334
#define BX_TOKEN_TIMEBP 335
#define BX_TOKEN_MODEBP 336
#define BX_TOKEN_VMEXITBP 337
#define BX_TOKEN_PRINT_STACK 338
#define BX_TOKEN_BT 339
#define BX_TOKEN_WATCH 340
#define BX_TOKEN_UNWATCH 341
#define BX_TOKEN_READ 342
#define BX_TOKEN_WRITE 343
#define BX_TOKEN_SHOW 344
#define BX_TOKEN_LOAD_SYMBOLS 345
#define BX_TOKEN_SYMBOLS 346
#define BX_TOKEN_LIST_SYMBOLS 347
#define BX_TOKEN_GLOBAL 348
#define BX_TOKEN_WHERE 349
#define BX_TOKEN_PRINT_STRING 350
#define BX_TOKEN_NUMERIC 351
#define BX_TOKEN_PAGE 352
#define BX_TOKEN_HELP 353
#define BX_TOKEN_XML 354
#define BX_TOKEN_CALC 355
#define BX_TOKEN_DEVICE 356
#define BX_TOKEN_GENERIC 357
#define BX_TOKEN_RSHIFT 358
#define BX_TOKEN_LSHIFT 359
#define BX_TOKEN_EQ 360
#define BX_TOKEN_NE 361
#define BX_TOKEN_LE 362
#define BX_TOKEN_GE 363
#define BX_TOKEN_REG_IP 364
#define BX_TOKEN_REG_EIP 365
#define BX_TOKEN_REG_RIP 366
#define NOT 367
#define NEG 368
#define INDIRECT 369

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 14 "parser.y" /* yacc.c:1909  */

  char    *sval;
  Bit64u   uval;
  bx_bool  bval;

#line 288 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE bxlval;

int bxparse (void);

#endif /* !YY_BX_Y_TAB_H_INCLUDED  */
