/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
     BX_TOKEN_MMX = 291,
     BX_TOKEN_IDT = 292,
     BX_TOKEN_IVT = 293,
     BX_TOKEN_GDT = 294,
     BX_TOKEN_LDT = 295,
     BX_TOKEN_TSS = 296,
     BX_TOKEN_TAB = 297,
     BX_TOKEN_DIRTY = 298,
     BX_TOKEN_LINUX = 299,
     BX_TOKEN_CONTROL_REGS = 300,
     BX_TOKEN_SEGMENT_REGS = 301,
     BX_TOKEN_EXAMINE = 302,
     BX_TOKEN_XFORMAT = 303,
     BX_TOKEN_DISFORMAT = 304,
     BX_TOKEN_RESTORE = 305,
     BX_TOKEN_SETPMEM = 306,
     BX_TOKEN_SYMBOLNAME = 307,
     BX_TOKEN_QUERY = 308,
     BX_TOKEN_PENDING = 309,
     BX_TOKEN_TAKE = 310,
     BX_TOKEN_DMA = 311,
     BX_TOKEN_IRQ = 312,
     BX_TOKEN_DISASSEMBLE = 313,
     BX_TOKEN_INSTRUMENT = 314,
     BX_TOKEN_STRING = 315,
     BX_TOKEN_STOP = 316,
     BX_TOKEN_DOIT = 317,
     BX_TOKEN_CRC = 318,
     BX_TOKEN_TRACE = 319,
     BX_TOKEN_TRACEREG = 320,
     BX_TOKEN_TRACEMEM = 321,
     BX_TOKEN_SWITCH_MODE = 322,
     BX_TOKEN_SIZE = 323,
     BX_TOKEN_PTIME = 324,
     BX_TOKEN_TIMEBP_ABSOLUTE = 325,
     BX_TOKEN_TIMEBP = 326,
     BX_TOKEN_RECORD = 327,
     BX_TOKEN_PLAYBACK = 328,
     BX_TOKEN_MODEBP = 329,
     BX_TOKEN_PRINT_STACK = 330,
     BX_TOKEN_WATCH = 331,
     BX_TOKEN_UNWATCH = 332,
     BX_TOKEN_READ = 333,
     BX_TOKEN_WRITE = 334,
     BX_TOKEN_SHOW = 335,
     BX_TOKEN_LOAD_SYMBOLS = 336,
     BX_TOKEN_SYMBOLS = 337,
     BX_TOKEN_LIST_SYMBOLS = 338,
     BX_TOKEN_GLOBAL = 339,
     BX_TOKEN_WHERE = 340,
     BX_TOKEN_PRINT_STRING = 341,
     BX_TOKEN_NUMERIC = 342,
     BX_TOKEN_NE2000 = 343,
     BX_TOKEN_PIC = 344,
     BX_TOKEN_PAGE = 345,
     BX_TOKEN_HELP = 346,
     BX_TOKEN_CALC = 347,
     BX_TOKEN_VGA = 348,
     BX_TOKEN_PCI = 349,
     BX_TOKEN_COMMAND = 350,
     BX_TOKEN_GENERIC = 351,
     BX_TOKEN_RSHIFT = 352,
     BX_TOKEN_LSHIFT = 353,
     BX_TOKEN_REG_IP = 354,
     BX_TOKEN_REG_EIP = 355,
     BX_TOKEN_REG_RIP = 356,
     NEG = 357,
     NOT = 358
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
#define BX_TOKEN_MMX 291
#define BX_TOKEN_IDT 292
#define BX_TOKEN_IVT 293
#define BX_TOKEN_GDT 294
#define BX_TOKEN_LDT 295
#define BX_TOKEN_TSS 296
#define BX_TOKEN_TAB 297
#define BX_TOKEN_DIRTY 298
#define BX_TOKEN_LINUX 299
#define BX_TOKEN_CONTROL_REGS 300
#define BX_TOKEN_SEGMENT_REGS 301
#define BX_TOKEN_EXAMINE 302
#define BX_TOKEN_XFORMAT 303
#define BX_TOKEN_DISFORMAT 304
#define BX_TOKEN_RESTORE 305
#define BX_TOKEN_SETPMEM 306
#define BX_TOKEN_SYMBOLNAME 307
#define BX_TOKEN_QUERY 308
#define BX_TOKEN_PENDING 309
#define BX_TOKEN_TAKE 310
#define BX_TOKEN_DMA 311
#define BX_TOKEN_IRQ 312
#define BX_TOKEN_DISASSEMBLE 313
#define BX_TOKEN_INSTRUMENT 314
#define BX_TOKEN_STRING 315
#define BX_TOKEN_STOP 316
#define BX_TOKEN_DOIT 317
#define BX_TOKEN_CRC 318
#define BX_TOKEN_TRACE 319
#define BX_TOKEN_TRACEREG 320
#define BX_TOKEN_TRACEMEM 321
#define BX_TOKEN_SWITCH_MODE 322
#define BX_TOKEN_SIZE 323
#define BX_TOKEN_PTIME 324
#define BX_TOKEN_TIMEBP_ABSOLUTE 325
#define BX_TOKEN_TIMEBP 326
#define BX_TOKEN_RECORD 327
#define BX_TOKEN_PLAYBACK 328
#define BX_TOKEN_MODEBP 329
#define BX_TOKEN_PRINT_STACK 330
#define BX_TOKEN_WATCH 331
#define BX_TOKEN_UNWATCH 332
#define BX_TOKEN_READ 333
#define BX_TOKEN_WRITE 334
#define BX_TOKEN_SHOW 335
#define BX_TOKEN_LOAD_SYMBOLS 336
#define BX_TOKEN_SYMBOLS 337
#define BX_TOKEN_LIST_SYMBOLS 338
#define BX_TOKEN_GLOBAL 339
#define BX_TOKEN_WHERE 340
#define BX_TOKEN_PRINT_STRING 341
#define BX_TOKEN_NUMERIC 342
#define BX_TOKEN_NE2000 343
#define BX_TOKEN_PIC 344
#define BX_TOKEN_PAGE 345
#define BX_TOKEN_HELP 346
#define BX_TOKEN_CALC 347
#define BX_TOKEN_VGA 348
#define BX_TOKEN_PCI 349
#define BX_TOKEN_COMMAND 350
#define BX_TOKEN_GENERIC 351
#define BX_TOKEN_RSHIFT 352
#define BX_TOKEN_LSHIFT 353
#define BX_TOKEN_REG_IP 354
#define BX_TOKEN_REG_EIP 355
#define BX_TOKEN_REG_RIP 356
#define NEG 357
#define NOT 358




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 13 "parser.y"
{
  char    *sval;
  Bit64u   uval;
  bx_bool  bval;
}
/* Line 1489 of yacc.c.  */
#line 261 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE bxlval;

