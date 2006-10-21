/* A Bison parser, made by GNU Bison 1.875b.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
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
     BX_TOKEN_FLAGS = 269,
     BX_TOKEN_ON = 270,
     BX_TOKEN_OFF = 271,
     BX_TOKEN_CONTINUE = 272,
     BX_TOKEN_STEPN = 273,
     BX_TOKEN_STEP_OVER = 274,
     BX_TOKEN_NEXT_STEP = 275,
     BX_TOKEN_SET = 276,
     BX_TOKEN_DEBUGGER = 277,
     BX_TOKEN_LIST_BREAK = 278,
     BX_TOKEN_VBREAKPOINT = 279,
     BX_TOKEN_LBREAKPOINT = 280,
     BX_TOKEN_PBREAKPOINT = 281,
     BX_TOKEN_DEL_BREAKPOINT = 282,
     BX_TOKEN_ENABLE_BREAKPOINT = 283,
     BX_TOKEN_DISABLE_BREAKPOINT = 284,
     BX_TOKEN_INFO = 285,
     BX_TOKEN_QUIT = 286,
     BX_TOKEN_REGISTERS = 287,
     BX_TOKEN_CPU = 288,
     BX_TOKEN_FPU = 289,
     BX_TOKEN_SSE = 290,
     BX_TOKEN_ALL = 291,
     BX_TOKEN_IDT = 292,
     BX_TOKEN_IVT = 293,
     BX_TOKEN_GDT = 294,
     BX_TOKEN_LDT = 295,
     BX_TOKEN_TSS = 296,
     BX_TOKEN_TAB = 297,
     BX_TOKEN_DIRTY = 298,
     BX_TOKEN_LINUX = 299,
     BX_TOKEN_CONTROL_REGS = 300,
     BX_TOKEN_EXAMINE = 301,
     BX_TOKEN_XFORMAT = 302,
     BX_TOKEN_DISFORMAT = 303,
     BX_TOKEN_SETPMEM = 304,
     BX_TOKEN_SYMBOLNAME = 305,
     BX_TOKEN_QUERY = 306,
     BX_TOKEN_PENDING = 307,
     BX_TOKEN_TAKE = 308,
     BX_TOKEN_DMA = 309,
     BX_TOKEN_IRQ = 310,
     BX_TOKEN_DUMP_CPU = 311,
     BX_TOKEN_SET_CPU = 312,
     BX_TOKEN_DISASSEMBLE = 313,
     BX_TOKEN_INSTRUMENT = 314,
     BX_TOKEN_STRING = 315,
     BX_TOKEN_STOP = 316,
     BX_TOKEN_DOIT = 317,
     BX_TOKEN_CRC = 318,
     BX_TOKEN_TRACE = 319,
     BX_TOKEN_TRACEREG = 320,
     BX_TOKEN_SWITCH_MODE = 321,
     BX_TOKEN_SIZE = 322,
     BX_TOKEN_PTIME = 323,
     BX_TOKEN_TIMEBP_ABSOLUTE = 324,
     BX_TOKEN_TIMEBP = 325,
     BX_TOKEN_RECORD = 326,
     BX_TOKEN_PLAYBACK = 327,
     BX_TOKEN_MODEBP = 328,
     BX_TOKEN_PRINT_STACK = 329,
     BX_TOKEN_WATCH = 330,
     BX_TOKEN_UNWATCH = 331,
     BX_TOKEN_READ = 332,
     BX_TOKEN_WRITE = 333,
     BX_TOKEN_SHOW = 334,
     BX_TOKEN_LOAD_SYMBOLS = 335,
     BX_TOKEN_SYMBOLS = 336,
     BX_TOKEN_LIST_SYMBOLS = 337,
     BX_TOKEN_GLOBAL = 338,
     BX_TOKEN_WHERE = 339,
     BX_TOKEN_PRINT_STRING = 340,
     BX_TOKEN_NUMERIC = 341,
     BX_TOKEN_NE2000 = 342,
     BX_TOKEN_PIC = 343,
     BX_TOKEN_PAGE = 344,
     BX_TOKEN_HELP = 345,
     BX_TOKEN_CALC = 346,
     BX_TOKEN_VGA = 347,
     BX_TOKEN_COMMAND = 348,
     BX_TOKEN_GENERIC = 349,
     BX_TOKEN_RSHIFT = 350,
     BX_TOKEN_LSHIFT = 351,
     BX_TOKEN_REG_IP = 352,
     BX_TOKEN_REG_EIP = 353,
     BX_TOKEN_REG_RIP = 354,
     NEG = 355,
     NOT = 356
   };
#endif
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
#define BX_TOKEN_FLAGS 269
#define BX_TOKEN_ON 270
#define BX_TOKEN_OFF 271
#define BX_TOKEN_CONTINUE 272
#define BX_TOKEN_STEPN 273
#define BX_TOKEN_STEP_OVER 274
#define BX_TOKEN_NEXT_STEP 275
#define BX_TOKEN_SET 276
#define BX_TOKEN_DEBUGGER 277
#define BX_TOKEN_LIST_BREAK 278
#define BX_TOKEN_VBREAKPOINT 279
#define BX_TOKEN_LBREAKPOINT 280
#define BX_TOKEN_PBREAKPOINT 281
#define BX_TOKEN_DEL_BREAKPOINT 282
#define BX_TOKEN_ENABLE_BREAKPOINT 283
#define BX_TOKEN_DISABLE_BREAKPOINT 284
#define BX_TOKEN_INFO 285
#define BX_TOKEN_QUIT 286
#define BX_TOKEN_REGISTERS 287
#define BX_TOKEN_CPU 288
#define BX_TOKEN_FPU 289
#define BX_TOKEN_SSE 290
#define BX_TOKEN_ALL 291
#define BX_TOKEN_IDT 292
#define BX_TOKEN_IVT 293
#define BX_TOKEN_GDT 294
#define BX_TOKEN_LDT 295
#define BX_TOKEN_TSS 296
#define BX_TOKEN_TAB 297
#define BX_TOKEN_DIRTY 298
#define BX_TOKEN_LINUX 299
#define BX_TOKEN_CONTROL_REGS 300
#define BX_TOKEN_EXAMINE 301
#define BX_TOKEN_XFORMAT 302
#define BX_TOKEN_DISFORMAT 303
#define BX_TOKEN_SETPMEM 304
#define BX_TOKEN_SYMBOLNAME 305
#define BX_TOKEN_QUERY 306
#define BX_TOKEN_PENDING 307
#define BX_TOKEN_TAKE 308
#define BX_TOKEN_DMA 309
#define BX_TOKEN_IRQ 310
#define BX_TOKEN_DUMP_CPU 311
#define BX_TOKEN_SET_CPU 312
#define BX_TOKEN_DISASSEMBLE 313
#define BX_TOKEN_INSTRUMENT 314
#define BX_TOKEN_STRING 315
#define BX_TOKEN_STOP 316
#define BX_TOKEN_DOIT 317
#define BX_TOKEN_CRC 318
#define BX_TOKEN_TRACE 319
#define BX_TOKEN_TRACEREG 320
#define BX_TOKEN_SWITCH_MODE 321
#define BX_TOKEN_SIZE 322
#define BX_TOKEN_PTIME 323
#define BX_TOKEN_TIMEBP_ABSOLUTE 324
#define BX_TOKEN_TIMEBP 325
#define BX_TOKEN_RECORD 326
#define BX_TOKEN_PLAYBACK 327
#define BX_TOKEN_MODEBP 328
#define BX_TOKEN_PRINT_STACK 329
#define BX_TOKEN_WATCH 330
#define BX_TOKEN_UNWATCH 331
#define BX_TOKEN_READ 332
#define BX_TOKEN_WRITE 333
#define BX_TOKEN_SHOW 334
#define BX_TOKEN_LOAD_SYMBOLS 335
#define BX_TOKEN_SYMBOLS 336
#define BX_TOKEN_LIST_SYMBOLS 337
#define BX_TOKEN_GLOBAL 338
#define BX_TOKEN_WHERE 339
#define BX_TOKEN_PRINT_STRING 340
#define BX_TOKEN_NUMERIC 341
#define BX_TOKEN_NE2000 342
#define BX_TOKEN_PIC 343
#define BX_TOKEN_PAGE 344
#define BX_TOKEN_HELP 345
#define BX_TOKEN_CALC 346
#define BX_TOKEN_VGA 347
#define BX_TOKEN_COMMAND 348
#define BX_TOKEN_GENERIC 349
#define BX_TOKEN_RSHIFT 350
#define BX_TOKEN_LSHIFT 351
#define BX_TOKEN_REG_IP 352
#define BX_TOKEN_REG_EIP 353
#define BX_TOKEN_REG_RIP 354
#define NEG 355
#define NOT 356




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 13 "parser.y"
typedef union YYSTYPE {
  char    *sval;
  Bit64u   uval;
  bx_bool  bval;
} YYSTYPE;
/* Line 1252 of yacc.c.  */
#line 245 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE bxlval;



