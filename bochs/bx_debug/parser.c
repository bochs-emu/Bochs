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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse bxparse
#define yylex   bxlex
#define yyerror bxerror
#define yylval  bxlval
#define yychar  bxchar
#define yydebug bxdebug
#define yynerrs bxnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     BX_TOKEN_REG_AL = 258,
     BX_TOKEN_REG_BL = 259,
     BX_TOKEN_REG_CL = 260,
     BX_TOKEN_REG_DL = 261,
     BX_TOKEN_REG_AH = 262,
     BX_TOKEN_REG_BH = 263,
     BX_TOKEN_REG_CH = 264,
     BX_TOKEN_REG_DH = 265,
     BX_TOKEN_REG_AX = 266,
     BX_TOKEN_REG_BX = 267,
     BX_TOKEN_REG_CX = 268,
     BX_TOKEN_REG_DX = 269,
     BX_TOKEN_REG_EAX = 270,
     BX_TOKEN_REG_EBX = 271,
     BX_TOKEN_REG_ECX = 272,
     BX_TOKEN_REG_EDX = 273,
     BX_TOKEN_REG_SI = 274,
     BX_TOKEN_REG_DI = 275,
     BX_TOKEN_REG_BP = 276,
     BX_TOKEN_REG_SP = 277,
     BX_TOKEN_REG_IP = 278,
     BX_TOKEN_REG_ESI = 279,
     BX_TOKEN_REG_EDI = 280,
     BX_TOKEN_REG_EBP = 281,
     BX_TOKEN_REG_ESP = 282,
     BX_TOKEN_REG_EIP = 283,
     BX_TOKEN_CONTINUE = 284,
     BX_TOKEN_STEPN = 285,
     BX_TOKEN_STEP_OVER = 286,
     BX_TOKEN_NEXT_STEP = 287,
     BX_TOKEN_SET = 288,
     BX_TOKEN_DEBUGGER = 289,
     BX_TOKEN_LIST_BREAK = 290,
     BX_TOKEN_VBREAKPOINT = 291,
     BX_TOKEN_LBREAKPOINT = 292,
     BX_TOKEN_PBREAKPOINT = 293,
     BX_TOKEN_DEL_BREAKPOINT = 294,
     BX_TOKEN_ENABLE_BREAKPOINT = 295,
     BX_TOKEN_DISABLE_BREAKPOINT = 296,
     BX_TOKEN_INFO = 297,
     BX_TOKEN_QUIT = 298,
     BX_TOKEN_PROGRAM = 299,
     BX_TOKEN_REGISTERS = 300,
     BX_TOKEN_CPU = 301,
     BX_TOKEN_FPU = 302,
     BX_TOKEN_ALL = 303,
     BX_TOKEN_IDT = 304,
     BX_TOKEN_GDT = 305,
     BX_TOKEN_LDT = 306,
     BX_TOKEN_TSS = 307,
     BX_TOKEN_DIRTY = 308,
     BX_TOKEN_LINUX = 309,
     BX_TOKEN_CONTROL_REGS = 310,
     BX_TOKEN_EXAMINE = 311,
     BX_TOKEN_XFORMAT = 312,
     BX_TOKEN_DISFORMAT = 313,
     BX_TOKEN_SETPMEM = 314,
     BX_TOKEN_SYMBOLNAME = 315,
     BX_TOKEN_QUERY = 316,
     BX_TOKEN_PENDING = 317,
     BX_TOKEN_TAKE = 318,
     BX_TOKEN_DMA = 319,
     BX_TOKEN_IRQ = 320,
     BX_TOKEN_DUMP_CPU = 321,
     BX_TOKEN_SET_CPU = 322,
     BX_TOKEN_DIS = 323,
     BX_TOKEN_ON = 324,
     BX_TOKEN_OFF = 325,
     BX_TOKEN_DISASSEMBLE = 326,
     BX_TOKEN_INSTRUMENT = 327,
     BX_TOKEN_START = 328,
     BX_TOKEN_STOP = 329,
     BX_TOKEN_RESET = 330,
     BX_TOKEN_PRINT = 331,
     BX_TOKEN_LOADER = 332,
     BX_TOKEN_STRING = 333,
     BX_TOKEN_DOIT = 334,
     BX_TOKEN_CRC = 335,
     BX_TOKEN_TRACEON = 336,
     BX_TOKEN_TRACEOFF = 337,
     BX_TOKEN_PTIME = 338,
     BX_TOKEN_TIMEBP_ABSOLUTE = 339,
     BX_TOKEN_TIMEBP = 340,
     BX_TOKEN_RECORD = 341,
     BX_TOKEN_PLAYBACK = 342,
     BX_TOKEN_MODEBP = 343,
     BX_TOKEN_PRINT_STACK = 344,
     BX_TOKEN_WATCH = 345,
     BX_TOKEN_UNWATCH = 346,
     BX_TOKEN_READ = 347,
     BX_TOKEN_WRITE = 348,
     BX_TOKEN_SHOW = 349,
     BX_TOKEN_SYMBOL = 350,
     BX_TOKEN_SYMBOLS = 351,
     BX_TOKEN_LIST_SYMBOLS = 352,
     BX_TOKEN_GLOBAL = 353,
     BX_TOKEN_WHERE = 354,
     BX_TOKEN_PRINT_STRING = 355,
     BX_TOKEN_DIFF_MEMORY = 356,
     BX_TOKEN_SYNC_MEMORY = 357,
     BX_TOKEN_SYNC_CPU = 358,
     BX_TOKEN_FAST_FORWARD = 359,
     BX_TOKEN_PHY_2_LOG = 360,
     BX_TOKEN_NUMERIC = 361,
     BX_TOKEN_LONG_NUMERIC = 362,
     BX_TOKEN_INFO_ADDRESS = 363,
     BX_TOKEN_NE2000 = 364,
     BX_TOKEN_PIC = 365,
     BX_TOKEN_PAGE = 366,
     BX_TOKEN_CS = 367,
     BX_TOKEN_ES = 368,
     BX_TOKEN_SS = 369,
     BX_TOKEN_DS = 370,
     BX_TOKEN_FS = 371,
     BX_TOKEN_GS = 372,
     BX_TOKEN_FLAGS = 373,
     BX_TOKEN_ALWAYS_CHECK = 374,
     BX_TOKEN_TRACEREGON = 375,
     BX_TOKEN_TRACEREGOFF = 376,
     BX_TOKEN_HELP = 377,
     BX_TOKEN_CALC = 378,
     BX_TOKEN_VGA = 379,
     BX_TOKEN_RSHIFT = 380,
     BX_TOKEN_LSHIFT = 381,
     BX_TOKEN_IVT = 382,
     NEG = 383,
     NOT = 384
   };
#endif
#define BX_TOKEN_REG_AL 258
#define BX_TOKEN_REG_BL 259
#define BX_TOKEN_REG_CL 260
#define BX_TOKEN_REG_DL 261
#define BX_TOKEN_REG_AH 262
#define BX_TOKEN_REG_BH 263
#define BX_TOKEN_REG_CH 264
#define BX_TOKEN_REG_DH 265
#define BX_TOKEN_REG_AX 266
#define BX_TOKEN_REG_BX 267
#define BX_TOKEN_REG_CX 268
#define BX_TOKEN_REG_DX 269
#define BX_TOKEN_REG_EAX 270
#define BX_TOKEN_REG_EBX 271
#define BX_TOKEN_REG_ECX 272
#define BX_TOKEN_REG_EDX 273
#define BX_TOKEN_REG_SI 274
#define BX_TOKEN_REG_DI 275
#define BX_TOKEN_REG_BP 276
#define BX_TOKEN_REG_SP 277
#define BX_TOKEN_REG_IP 278
#define BX_TOKEN_REG_ESI 279
#define BX_TOKEN_REG_EDI 280
#define BX_TOKEN_REG_EBP 281
#define BX_TOKEN_REG_ESP 282
#define BX_TOKEN_REG_EIP 283
#define BX_TOKEN_CONTINUE 284
#define BX_TOKEN_STEPN 285
#define BX_TOKEN_STEP_OVER 286
#define BX_TOKEN_NEXT_STEP 287
#define BX_TOKEN_SET 288
#define BX_TOKEN_DEBUGGER 289
#define BX_TOKEN_LIST_BREAK 290
#define BX_TOKEN_VBREAKPOINT 291
#define BX_TOKEN_LBREAKPOINT 292
#define BX_TOKEN_PBREAKPOINT 293
#define BX_TOKEN_DEL_BREAKPOINT 294
#define BX_TOKEN_ENABLE_BREAKPOINT 295
#define BX_TOKEN_DISABLE_BREAKPOINT 296
#define BX_TOKEN_INFO 297
#define BX_TOKEN_QUIT 298
#define BX_TOKEN_PROGRAM 299
#define BX_TOKEN_REGISTERS 300
#define BX_TOKEN_CPU 301
#define BX_TOKEN_FPU 302
#define BX_TOKEN_ALL 303
#define BX_TOKEN_IDT 304
#define BX_TOKEN_GDT 305
#define BX_TOKEN_LDT 306
#define BX_TOKEN_TSS 307
#define BX_TOKEN_DIRTY 308
#define BX_TOKEN_LINUX 309
#define BX_TOKEN_CONTROL_REGS 310
#define BX_TOKEN_EXAMINE 311
#define BX_TOKEN_XFORMAT 312
#define BX_TOKEN_DISFORMAT 313
#define BX_TOKEN_SETPMEM 314
#define BX_TOKEN_SYMBOLNAME 315
#define BX_TOKEN_QUERY 316
#define BX_TOKEN_PENDING 317
#define BX_TOKEN_TAKE 318
#define BX_TOKEN_DMA 319
#define BX_TOKEN_IRQ 320
#define BX_TOKEN_DUMP_CPU 321
#define BX_TOKEN_SET_CPU 322
#define BX_TOKEN_DIS 323
#define BX_TOKEN_ON 324
#define BX_TOKEN_OFF 325
#define BX_TOKEN_DISASSEMBLE 326
#define BX_TOKEN_INSTRUMENT 327
#define BX_TOKEN_START 328
#define BX_TOKEN_STOP 329
#define BX_TOKEN_RESET 330
#define BX_TOKEN_PRINT 331
#define BX_TOKEN_LOADER 332
#define BX_TOKEN_STRING 333
#define BX_TOKEN_DOIT 334
#define BX_TOKEN_CRC 335
#define BX_TOKEN_TRACEON 336
#define BX_TOKEN_TRACEOFF 337
#define BX_TOKEN_PTIME 338
#define BX_TOKEN_TIMEBP_ABSOLUTE 339
#define BX_TOKEN_TIMEBP 340
#define BX_TOKEN_RECORD 341
#define BX_TOKEN_PLAYBACK 342
#define BX_TOKEN_MODEBP 343
#define BX_TOKEN_PRINT_STACK 344
#define BX_TOKEN_WATCH 345
#define BX_TOKEN_UNWATCH 346
#define BX_TOKEN_READ 347
#define BX_TOKEN_WRITE 348
#define BX_TOKEN_SHOW 349
#define BX_TOKEN_SYMBOL 350
#define BX_TOKEN_SYMBOLS 351
#define BX_TOKEN_LIST_SYMBOLS 352
#define BX_TOKEN_GLOBAL 353
#define BX_TOKEN_WHERE 354
#define BX_TOKEN_PRINT_STRING 355
#define BX_TOKEN_DIFF_MEMORY 356
#define BX_TOKEN_SYNC_MEMORY 357
#define BX_TOKEN_SYNC_CPU 358
#define BX_TOKEN_FAST_FORWARD 359
#define BX_TOKEN_PHY_2_LOG 360
#define BX_TOKEN_NUMERIC 361
#define BX_TOKEN_LONG_NUMERIC 362
#define BX_TOKEN_INFO_ADDRESS 363
#define BX_TOKEN_NE2000 364
#define BX_TOKEN_PIC 365
#define BX_TOKEN_PAGE 366
#define BX_TOKEN_CS 367
#define BX_TOKEN_ES 368
#define BX_TOKEN_SS 369
#define BX_TOKEN_DS 370
#define BX_TOKEN_FS 371
#define BX_TOKEN_GS 372
#define BX_TOKEN_FLAGS 373
#define BX_TOKEN_ALWAYS_CHECK 374
#define BX_TOKEN_TRACEREGON 375
#define BX_TOKEN_TRACEREGOFF 376
#define BX_TOKEN_HELP 377
#define BX_TOKEN_CALC 378
#define BX_TOKEN_VGA 379
#define BX_TOKEN_RSHIFT 380
#define BX_TOKEN_LSHIFT 381
#define BX_TOKEN_IVT 382
#define NEG 383
#define NOT 384




/* Copy the first part of user declarations.  */
#line 5 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

#if BX_DEBUGGER

// %left '-' '+'
// %left '*' '/'
// %right
// %nonassoc UMINUS


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 19 "parser.y"
typedef union YYSTYPE {
  char    *sval;
  Bit32u   uval;
  Bit64u   ulval;
  bx_num_range   uval_range;
  Regs     reg;
  } YYSTYPE;
/* Line 191 of yacc.c.  */
#line 364 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 376 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  247
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1127

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  143
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  54
/* YYNRULES -- Number of rules. */
#define YYNRULES  225
/* YYNRULES -- Number of states. */
#define YYNSTATES  414

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   384

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     137,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   140,     2,     2,     2,     2,   134,     2,
     141,   142,   132,   128,     2,   129,     2,   133,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   138,     2,
       2,   139,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   131,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   130,     2,     2,     2,     2,     2,
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
     125,   126,   127,   135,   136
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     6,     8,    10,    12,    14,    16,    18,
      20,    22,    24,    26,    28,    30,    32,    34,    36,    38,
      40,    42,    44,    46,    48,    50,    52,    54,    56,    58,
      60,    62,    64,    66,    68,    70,    72,    74,    76,    78,
      80,    82,    84,    86,    88,    90,    92,    93,    95,    98,
     102,   106,   110,   114,   118,   122,   128,   133,   138,   140,
     142,   144,   146,   148,   150,   154,   158,   162,   166,   169,
     173,   176,   179,   182,   185,   188,   192,   196,   200,   203,
     206,   211,   216,   221,   226,   230,   235,   241,   244,   248,
     251,   254,   258,   261,   266,   271,   277,   280,   286,   289,
     293,   297,   300,   304,   309,   312,   315,   319,   323,   327,
     331,   335,   339,   343,   347,   352,   357,   362,   367,   372,
     376,   380,   384,   389,   393,   397,   403,   411,   415,   419,
     420,   422,   424,   427,   430,   434,   438,   442,   445,   450,
     454,   458,   461,   467,   471,   475,   480,   484,   487,   491,
     496,   500,   504,   508,   512,   516,   520,   525,   528,   531,
     535,   538,   542,   544,   546,   548,   550,   552,   554,   556,
     558,   560,   562,   564,   566,   568,   570,   572,   574,   576,
     578,   580,   582,   584,   586,   588,   590,   592,   594,   596,
     598,   600,   602,   604,   606,   608,   612,   616,   620,   624,
     628,   632,   636,   640,   644,   647,   650,   654,   656,   658,
     660,   662,   664,   668,   672,   676,   680,   684,   688,   692,
     696,   700,   704,   707,   710,   714
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     144,     0,    -1,   144,   145,    -1,   145,    -1,   161,    -1,
     162,    -1,   163,    -1,   164,    -1,   196,    -1,   165,    -1,
     168,    -1,   166,    -1,   167,    -1,   171,    -1,   172,    -1,
     173,    -1,   174,    -1,   175,    -1,   176,    -1,   177,    -1,
     178,    -1,   179,    -1,   180,    -1,   181,    -1,   182,    -1,
     183,    -1,   184,    -1,   185,    -1,   154,    -1,   155,    -1,
     153,    -1,   148,    -1,   149,    -1,   150,    -1,   151,    -1,
     156,    -1,   157,    -1,   152,    -1,   158,    -1,   159,    -1,
     160,    -1,   146,    -1,   186,    -1,   187,    -1,   188,    -1,
     189,    -1,    -1,   137,    -1,   101,   137,    -1,   102,    69,
     137,    -1,   102,    70,   137,    -1,   103,    69,   137,    -1,
     103,    70,   137,    -1,   104,   106,   137,    -1,   105,   106,
     137,    -1,   108,   147,   138,   106,   137,    -1,   119,   106,
      69,   137,    -1,   119,   106,    70,   137,    -1,   112,    -1,
     113,    -1,   114,    -1,   115,    -1,   116,    -1,   117,    -1,
      85,   107,   137,    -1,    84,   107,   137,    -1,    86,    78,
     137,    -1,    87,    78,   137,    -1,    88,   137,    -1,    94,
      78,   137,    -1,    94,   137,    -1,    83,   137,    -1,    81,
     137,    -1,    82,   137,    -1,    89,   137,    -1,    89,   106,
     137,    -1,    90,    74,   137,    -1,    90,    29,   137,    -1,
      90,   137,    -1,    91,   137,    -1,    90,    92,   106,   137,
      -1,    91,    92,   106,   137,    -1,    90,    93,   106,   137,
      -1,    91,    93,   106,   137,    -1,    95,    78,   137,    -1,
      95,    78,   106,   137,    -1,    95,    98,    78,   106,   137,
      -1,    99,   137,    -1,   100,   106,   137,    -1,    29,   137,
      -1,    30,   137,    -1,    30,   106,   137,    -1,    31,   137,
      -1,    33,    68,    69,   137,    -1,    33,    68,    70,   137,
      -1,    33,    60,   139,   106,   137,    -1,    36,   137,    -1,
      36,   194,   138,   195,   137,    -1,    37,   137,    -1,    37,
     195,   137,    -1,    37,    78,   137,    -1,    38,   137,    -1,
      38,   195,   137,    -1,    38,   132,   195,   137,    -1,    35,
     137,    -1,    97,   137,    -1,    97,    78,   137,    -1,    42,
      38,   137,    -1,    42,    46,   137,    -1,    42,    44,   137,
      -1,    42,    45,   137,    -1,    42,    47,   137,    -1,    42,
      48,   137,    -1,    42,    53,   137,    -1,    42,    49,   169,
     137,    -1,    42,   127,   169,   137,    -1,    42,    50,   169,
     137,    -1,    42,    51,   169,   137,    -1,    42,    52,   169,
     137,    -1,    42,   118,   137,    -1,    42,    54,   137,    -1,
      42,    96,   137,    -1,    42,    96,    78,   137,    -1,    42,
      55,   137,    -1,    42,   109,   137,    -1,    42,   109,   111,
     106,   137,    -1,    42,   109,   111,   106,    45,   106,   137,
      -1,    42,   110,   137,    -1,    42,   124,   137,    -1,    -1,
     170,    -1,   195,    -1,   195,   195,    -1,    66,   137,    -1,
      39,   106,   137,    -1,    40,   106,   137,    -1,    41,   106,
     137,    -1,    43,   137,    -1,    56,    57,   195,   137,    -1,
      56,    57,   137,    -1,    56,   195,   137,    -1,    56,   137,
      -1,    59,   106,   106,   106,   137,    -1,    61,    62,   137,
      -1,    63,    64,   137,    -1,    63,    64,   106,   137,    -1,
      63,    65,   137,    -1,    67,   137,    -1,    71,   169,   137,
      -1,    71,    58,   169,   137,    -1,    72,    73,   137,    -1,
      72,    74,   137,    -1,    72,    75,   137,    -1,    72,    76,
     137,    -1,    77,    78,   137,    -1,    79,   106,   137,    -1,
      80,   106,   106,   137,    -1,   120,   137,    -1,   121,   137,
      -1,   122,    78,   137,    -1,   122,   137,    -1,   123,   195,
     137,    -1,     3,    -1,     4,    -1,     5,    -1,     6,    -1,
       7,    -1,     8,    -1,     9,    -1,    10,    -1,    11,    -1,
      12,    -1,    13,    -1,    14,    -1,    15,    -1,    16,    -1,
      17,    -1,    18,    -1,    19,    -1,    20,    -1,    24,    -1,
      25,    -1,    21,    -1,    22,    -1,    23,    -1,    26,    -1,
      27,    -1,    28,    -1,   190,    -1,   191,    -1,   192,    -1,
     106,    -1,   107,    -1,   193,    -1,   147,    -1,   194,   128,
     194,    -1,   194,   129,   194,    -1,   194,   132,   194,    -1,
     194,   133,   194,    -1,   194,   125,   194,    -1,   194,   126,
     194,    -1,   194,   130,   194,    -1,   194,   131,   194,    -1,
     194,   134,   194,    -1,   140,   194,    -1,   129,   194,    -1,
     141,   194,   142,    -1,   106,    -1,   107,    -1,    78,    -1,
     193,    -1,   147,    -1,   195,   138,   195,    -1,   195,   128,
     195,    -1,   195,   129,   195,    -1,   195,   132,   195,    -1,
     195,   133,   195,    -1,   195,   125,   195,    -1,   195,   126,
     195,    -1,   195,   130,   195,    -1,   195,   131,   195,    -1,
     195,   134,   195,    -1,   140,   195,    -1,   129,   195,    -1,
     141,   195,   142,    -1,    45,   193,   139,   195,   137,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   172,   172,   173,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   226,   231,
     236,   241,   246,   251,   256,   259,   264,   267,   273,   274,
     275,   276,   277,   278,   282,   287,   295,   303,   311,   319,
     324,   332,   340,   348,   356,   361,   369,   375,   381,   386,
     391,   396,   401,   406,   414,   419,   424,   432,   440,   448,
     456,   461,   469,   477,   482,   487,   495,   500,   505,   510,
     515,   520,   525,   530,   538,   546,   551,   559,   564,   569,
     574,   579,   584,   589,   594,   599,   604,   609,   614,   619,
     624,   629,   634,   639,   644,   649,   654,   659,   664,   676,
     677,   680,   685,   692,   700,   708,   715,   723,   732,   740,
     748,   761,   777,   785,   793,   798,   803,   811,   819,   824,
     832,   837,   842,   847,   855,   863,   871,   879,   887,   895,
     900,   908,   916,   917,   918,   919,   920,   921,   922,   923,
     924,   925,   926,   927,   928,   929,   930,   931,   936,   937,
     938,   939,   944,   945,   946,   947,   948,   949,   954,   955,
     956,   962,   963,   964,   965,   966,   967,   968,   969,   970,
     971,   972,   973,   974,   975,   976,   977,   982,   983,   984,
     985,   986,   987,   988,   989,   990,   991,   992,   993,   994,
     995,   996,   997,   998,   999,  1003
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "BX_TOKEN_REG_AL", "BX_TOKEN_REG_BL", 
  "BX_TOKEN_REG_CL", "BX_TOKEN_REG_DL", "BX_TOKEN_REG_AH", 
  "BX_TOKEN_REG_BH", "BX_TOKEN_REG_CH", "BX_TOKEN_REG_DH", 
  "BX_TOKEN_REG_AX", "BX_TOKEN_REG_BX", "BX_TOKEN_REG_CX", 
  "BX_TOKEN_REG_DX", "BX_TOKEN_REG_EAX", "BX_TOKEN_REG_EBX", 
  "BX_TOKEN_REG_ECX", "BX_TOKEN_REG_EDX", "BX_TOKEN_REG_SI", 
  "BX_TOKEN_REG_DI", "BX_TOKEN_REG_BP", "BX_TOKEN_REG_SP", 
  "BX_TOKEN_REG_IP", "BX_TOKEN_REG_ESI", "BX_TOKEN_REG_EDI", 
  "BX_TOKEN_REG_EBP", "BX_TOKEN_REG_ESP", "BX_TOKEN_REG_EIP", 
  "BX_TOKEN_CONTINUE", "BX_TOKEN_STEPN", "BX_TOKEN_STEP_OVER", 
  "BX_TOKEN_NEXT_STEP", "BX_TOKEN_SET", "BX_TOKEN_DEBUGGER", 
  "BX_TOKEN_LIST_BREAK", "BX_TOKEN_VBREAKPOINT", "BX_TOKEN_LBREAKPOINT", 
  "BX_TOKEN_PBREAKPOINT", "BX_TOKEN_DEL_BREAKPOINT", 
  "BX_TOKEN_ENABLE_BREAKPOINT", "BX_TOKEN_DISABLE_BREAKPOINT", 
  "BX_TOKEN_INFO", "BX_TOKEN_QUIT", "BX_TOKEN_PROGRAM", 
  "BX_TOKEN_REGISTERS", "BX_TOKEN_CPU", "BX_TOKEN_FPU", "BX_TOKEN_ALL", 
  "BX_TOKEN_IDT", "BX_TOKEN_GDT", "BX_TOKEN_LDT", "BX_TOKEN_TSS", 
  "BX_TOKEN_DIRTY", "BX_TOKEN_LINUX", "BX_TOKEN_CONTROL_REGS", 
  "BX_TOKEN_EXAMINE", "BX_TOKEN_XFORMAT", "BX_TOKEN_DISFORMAT", 
  "BX_TOKEN_SETPMEM", "BX_TOKEN_SYMBOLNAME", "BX_TOKEN_QUERY", 
  "BX_TOKEN_PENDING", "BX_TOKEN_TAKE", "BX_TOKEN_DMA", "BX_TOKEN_IRQ", 
  "BX_TOKEN_DUMP_CPU", "BX_TOKEN_SET_CPU", "BX_TOKEN_DIS", "BX_TOKEN_ON", 
  "BX_TOKEN_OFF", "BX_TOKEN_DISASSEMBLE", "BX_TOKEN_INSTRUMENT", 
  "BX_TOKEN_START", "BX_TOKEN_STOP", "BX_TOKEN_RESET", "BX_TOKEN_PRINT", 
  "BX_TOKEN_LOADER", "BX_TOKEN_STRING", "BX_TOKEN_DOIT", "BX_TOKEN_CRC", 
  "BX_TOKEN_TRACEON", "BX_TOKEN_TRACEOFF", "BX_TOKEN_PTIME", 
  "BX_TOKEN_TIMEBP_ABSOLUTE", "BX_TOKEN_TIMEBP", "BX_TOKEN_RECORD", 
  "BX_TOKEN_PLAYBACK", "BX_TOKEN_MODEBP", "BX_TOKEN_PRINT_STACK", 
  "BX_TOKEN_WATCH", "BX_TOKEN_UNWATCH", "BX_TOKEN_READ", "BX_TOKEN_WRITE", 
  "BX_TOKEN_SHOW", "BX_TOKEN_SYMBOL", "BX_TOKEN_SYMBOLS", 
  "BX_TOKEN_LIST_SYMBOLS", "BX_TOKEN_GLOBAL", "BX_TOKEN_WHERE", 
  "BX_TOKEN_PRINT_STRING", "BX_TOKEN_DIFF_MEMORY", "BX_TOKEN_SYNC_MEMORY", 
  "BX_TOKEN_SYNC_CPU", "BX_TOKEN_FAST_FORWARD", "BX_TOKEN_PHY_2_LOG", 
  "BX_TOKEN_NUMERIC", "BX_TOKEN_LONG_NUMERIC", "BX_TOKEN_INFO_ADDRESS", 
  "BX_TOKEN_NE2000", "BX_TOKEN_PIC", "BX_TOKEN_PAGE", "BX_TOKEN_CS", 
  "BX_TOKEN_ES", "BX_TOKEN_SS", "BX_TOKEN_DS", "BX_TOKEN_FS", 
  "BX_TOKEN_GS", "BX_TOKEN_FLAGS", "BX_TOKEN_ALWAYS_CHECK", 
  "BX_TOKEN_TRACEREGON", "BX_TOKEN_TRACEREGOFF", "BX_TOKEN_HELP", 
  "BX_TOKEN_CALC", "BX_TOKEN_VGA", "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT", 
  "BX_TOKEN_IVT", "'+'", "'-'", "'|'", "'^'", "'*'", "'/'", "'&'", "NEG", 
  "NOT", "'\\n'", "':'", "'='", "'!'", "'('", "')'", "$accept", 
  "commands", "command", "cosim_commands", "segment_register", 
  "timebp_command", "record_command", "playback_command", 
  "modebp_command", "show_command", "ptime_command", "trace_on_command", 
  "trace_off_command", "print_stack_command", "watch_point_command", 
  "symbol_command", "where_command", "print_string_command", 
  "continue_command", "stepN_command", "step_over_command", "set_command", 
  "breakpoint_command", "blist_command", "slist_command", "info_command", 
  "optional_numeric_range", "numeric_range", "dump_cpu_command", 
  "delete_command", "bpe_command", "bpd_command", "quit_command", 
  "examine_command", "setpmem_command", "query_command", "take_command", 
  "set_cpu_command", "disassemble_command", "instrument_command", 
  "loader_command", "doit_command", "crc_command", "trace_reg_on_command", 
  "trace_reg_off_command", "help_command", "calc_command", 
  "BX_TOKEN_COMMON_REG", "BX_TOKEN_INDEX_REG", "BX_TOKEN_PTR_REG", 
  "BX_TOKEN_NONSEG_REG", "vexpression", "expression", "set_reg_command", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,    43,    45,
     124,    94,    42,    47,    38,   383,   384,    10,    58,    61,
      33,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   143,   144,   144,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   147,   147,
     147,   147,   147,   147,   148,   148,   149,   150,   151,   152,
     152,   153,   154,   155,   156,   156,   157,   157,   157,   157,
     157,   157,   157,   157,   158,   158,   158,   159,   160,   161,
     162,   162,   163,   164,   164,   164,   165,   165,   165,   165,
     165,   165,   165,   165,   166,   167,   167,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   169,
     169,   170,   170,   171,   172,   173,   174,   175,   176,   176,
     176,   176,   177,   178,   179,   179,   179,   180,   181,   181,
     182,   182,   182,   182,   183,   184,   185,   186,   187,   188,
     188,   189,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   191,   191,
     191,   191,   192,   192,   192,   192,   192,   192,   193,   193,
     193,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   196
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     1,     2,     3,
       3,     3,     3,     3,     3,     5,     4,     4,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     2,     3,
       2,     2,     2,     2,     2,     3,     3,     3,     2,     2,
       4,     4,     4,     4,     3,     4,     5,     2,     3,     2,
       2,     3,     2,     4,     4,     5,     2,     5,     2,     3,
       3,     2,     3,     4,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     4,     4,     4,     4,     4,     3,
       3,     3,     4,     3,     3,     5,     7,     3,     3,     0,
       1,     1,     2,     2,     3,     3,     3,     2,     4,     3,
       3,     2,     5,     3,     3,     4,     3,     2,     3,     4,
       3,     3,     3,     3,     3,     3,     4,     2,     2,     3,
       2,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     3,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     3,     5
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
      46,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   129,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    47,     0,     3,    41,    31,    32,    33,
      34,    37,    30,    28,    29,    35,    36,    38,    39,    40,
       4,     5,     6,     7,     9,    11,    12,    10,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    42,    43,    44,    45,     8,    89,     0,
      90,    92,     0,     0,   104,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   182,   183,   184,   180,   181,   185,   186,
     187,   191,   192,    58,    59,    60,    61,    62,    63,     0,
      96,     0,     0,   194,   188,   189,   190,   193,     0,   209,
     207,   208,     0,    98,     0,     0,   211,   210,     0,   209,
       0,   101,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   129,   129,   129,   129,     0,     0,     0,     0,
       0,     0,     0,     0,   129,   137,     0,     0,   141,     0,
       0,     0,     0,     0,   133,   147,   129,     0,   130,   131,
       0,     0,     0,     0,     0,     0,     0,    72,    73,    71,
       0,     0,     0,     0,    68,     0,    74,     0,     0,     0,
       0,    78,     0,     0,    79,     0,    70,     0,     0,     0,
     105,    87,     0,    48,     0,     0,     0,     0,     0,     0,
       0,     0,   157,   158,     0,   160,     0,     1,     2,    91,
       0,     0,     0,   205,   204,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   100,   223,   222,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    99,
       0,     0,   102,   134,   135,   136,   107,   109,   110,   108,
     111,   112,     0,     0,     0,     0,   113,   120,   123,     0,
     121,     0,   124,   127,   119,   128,     0,     0,   139,     0,
     140,     0,   143,     0,   144,   146,     0,   148,     0,   132,
     150,   151,   152,   153,   154,   155,     0,    65,    64,    66,
      67,    75,    77,    76,     0,     0,     0,     0,    69,     0,
      84,     0,   106,    88,    49,    50,    51,    52,    53,    54,
       0,     0,     0,   159,   161,     0,    93,    94,   206,   199,
     200,   195,   196,   201,   202,   197,   198,   203,     0,   224,
     217,   218,   213,   214,   219,   220,   215,   216,   221,   212,
     103,   114,   116,   117,   118,   122,     0,   115,     0,   138,
       0,   145,   149,   214,   156,    80,    82,    81,    83,    85,
       0,     0,    56,    57,    95,    97,     0,   125,   225,   142,
      86,    55,     0,   126
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    54,    55,    56,   156,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,   197,   198,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,   144,   145,   146,
     157,   148,   199,    97
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -153
static const short yypact[] =
{
     974,  -102,   -95,   -87,   -41,   -85,   613,   385,   101,   -48,
      -6,    -4,   324,   -77,   816,   309,    -3,    77,   -39,     4,
       5,   461,   -45,    62,    37,    38,     9,    13,    14,    45,
      49,    80,    86,    28,   -94,   160,   -86,   -76,   -64,   -75,
      30,    64,    34,   -32,   -30,    66,    67,   -46,    68,    39,
      44,   -74,   677,  -153,   871,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,    53,
    -153,  -153,    36,   -13,  -153,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,   753,
    -153,   753,   753,  -153,  -153,  -153,  -153,  -153,   975,    54,
    -153,  -153,   677,  -153,   677,   677,  -153,  -153,   378,  -153,
     677,  -153,   454,    56,    57,    58,    61,    69,    73,    84,
      92,    98,   677,   677,   677,   677,   100,   102,   103,   -73,
    -101,   106,   107,   108,   677,  -153,    43,   537,  -153,   530,
     125,   109,   -89,   110,  -153,  -153,   677,   111,  -153,    71,
     112,   113,   114,   117,   118,   119,   151,  -153,  -153,  -153,
     122,   123,   124,   135,  -153,   155,  -153,   156,   157,   189,
     190,  -153,   192,   193,  -153,   164,  -153,   -88,   184,   165,
    -153,  -153,   166,  -153,   167,   168,   169,   170,   171,   172,
     202,     3,  -153,  -153,   173,  -153,   584,  -153,  -153,  -153,
     242,   212,   214,  -153,  -153,   213,   753,   753,   753,   753,
     753,   753,   753,   753,   753,   677,  -153,   215,   215,    94,
     677,   677,   677,   677,   677,   677,   677,   677,   677,  -153,
     677,   606,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,   217,   219,   220,   221,  -153,  -153,  -153,   222,
    -153,   244,  -153,  -153,  -153,  -153,   223,   677,  -153,   670,
    -153,   246,  -153,   224,  -153,  -153,   226,  -153,   677,   989,
    -153,  -153,  -153,  -153,  -153,  -153,   227,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,   228,   230,   243,   245,  -153,   247,
    -153,   275,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,
     277,   248,   249,  -153,  -153,   280,  -153,  -153,  -153,  -153,
    -153,    29,    29,    29,    29,  -153,  -153,  -153,   720,  -153,
     215,   215,   -79,   -79,   -79,   -79,   215,   215,   215,   989,
    -153,  -153,  -153,  -153,  -153,  -153,   -36,  -153,   747,  -153,
     281,  -153,  -153,   -79,  -153,  -153,  -153,  -153,  -153,  -153,
     282,   290,  -153,  -153,  -153,  -153,   308,  -153,  -153,  -153,
    -153,  -153,   291,  -153
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -153,  -153,   375,  -153,    18,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,  -152,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,
      27,  -126,    -7,  -153
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned short yytable[] =
{
     158,   162,   225,   229,   244,   299,   222,   223,   189,   406,
     301,    99,   215,   253,   227,   254,   255,   313,   339,   102,
     292,   293,   294,   295,   143,   192,   193,   103,   200,   201,
     202,   203,   306,   147,   228,    98,   302,   234,   235,   236,
     237,   186,   100,   216,   316,   246,   270,   271,   314,   340,
     101,   224,   104,   276,   277,   278,   251,   252,   163,   280,
     185,   226,   230,   245,   300,   240,   133,   134,   135,   136,
     137,   138,   351,   352,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     164,   407,   165,   190,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     359,   360,   361,   362,   363,   364,   365,   366,   367,   191,
     204,   194,   195,   205,   206,   267,   207,   268,   269,   159,
     208,   209,   210,   281,   256,   257,   211,   143,   212,   143,
     143,   262,   263,   264,   213,   214,   147,   231,   147,   147,
     232,   233,   238,   239,   241,   250,   242,   150,   151,   159,
     309,   243,   307,   133,   134,   135,   136,   137,   138,   217,
     249,   266,   319,   283,   284,   285,   270,   271,   286,   272,
     318,   274,   275,   276,   277,   278,   287,   150,   151,   280,
     288,   154,   155,   133,   134,   135,   136,   137,   138,   270,
     271,   289,   272,   273,   274,   275,   276,   277,   278,   290,
     152,   311,   280,   160,   218,   291,   369,   296,   161,   297,
     298,   154,   155,   303,   304,   305,   312,   315,   317,   320,
     321,   322,   219,   220,   323,   324,   325,   326,   368,   327,
     328,   329,   341,   370,   371,   372,   373,   374,   375,   376,
     377,   378,   330,   379,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   331,   332,   333,   334,   335,   221,   336,   337,
     388,   338,   342,   343,   344,   345,   346,   347,   348,   349,
     353,   393,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   256,   257,
     350,   258,   259,   260,   261,   262,   263,   264,   355,   356,
     386,   357,   390,   280,   381,   358,   382,   383,   384,   385,
     387,   391,   166,   392,   394,   395,   187,   396,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     397,   400,   398,   401,   399,   402,   403,   159,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   412,   150,   151,   404,   409,   410,
     179,   133,   134,   135,   136,   137,   138,   411,   413,   248,
       0,     0,     0,   180,   181,     0,     0,     0,   152,     0,
       0,     0,   182,     0,     0,     0,   188,     0,   183,   154,
     155,   184,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   149,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
       0,   150,   151,     0,     0,     0,     0,   133,   134,   135,
     136,   137,   138,   270,   271,     0,   272,   273,   274,   275,
     276,   277,   278,     0,   152,   279,   280,     0,     0,   196,
       0,     0,   153,     0,     0,   154,   155,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   159,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,     0,   150,   151,     0,
       0,     0,     0,   133,   134,   135,   136,   137,   138,   270,
     271,     0,   272,   273,   274,   275,   276,   277,   278,     0,
     152,   282,   280,     0,     0,     0,     0,     0,     0,     0,
       0,   154,   155,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   159,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,     0,   150,   151,     0,     0,     0,     0,   133,
     134,   135,   136,   137,   138,   270,   271,     0,   272,   273,
     274,   275,   276,   277,   278,     0,   152,   310,   280,     0,
       0,     0,     0,     0,   308,     0,     0,   154,   155,     0,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,     0,     0,     0,   270,
     271,     0,   272,   273,   274,   275,   276,   277,   278,   131,
     132,   354,   280,     0,     0,   133,   134,   135,   136,   137,
     138,   270,   271,     0,   272,   273,   274,   275,   276,   277,
     278,     0,   139,   380,   280,     0,     0,     0,     0,     0,
     140,     0,     0,   141,   142,   159,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,     0,   150,   151,     0,     0,     0,     0,   133,
     134,   135,   136,   137,   138,   270,   271,     0,   272,   273,
     274,   275,   276,   277,   278,     0,   152,   389,   280,     0,
       0,     0,     0,     0,     0,     0,     0,   154,   155,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   270,   271,     0,   272,   273,
     274,   275,   276,   277,   278,     0,     0,   405,   280,   131,
     132,     0,     0,     0,     0,   133,   134,   135,   136,   137,
     138,   247,   270,   271,     0,   272,   273,   274,   275,   276,
     277,   278,   139,     0,   408,   280,     0,     0,     0,     0,
       0,     0,     0,   141,   142,     0,     0,     0,     0,     0,
       1,     2,     3,     0,     4,     0,     5,     6,     7,     8,
       9,    10,    11,    12,    13,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    15,     0,     0,
      16,     0,    17,     0,    18,     0,     0,    19,    20,     0,
       0,     0,    21,    22,     0,     0,     0,     0,    23,     0,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,     0,    37,    38,     0,    39,     0,
      40,    41,    42,    43,    44,    45,    46,     0,     0,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,    49,    50,    51,    52,     0,     0,     0,     0,     0,
       0,     0,     0,     1,     2,     3,     0,     4,    53,     5,
       6,     7,     8,     9,    10,    11,    12,    13,     0,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      15,     0,     0,    16,     0,    17,     0,    18,     0,     0,
      19,    20,     0,     0,     0,    21,    22,     0,     0,     0,
       0,    23,     0,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,     0,    37,    38,
       0,    39,     0,    40,    41,    42,    43,    44,    45,    46,
       0,     0,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,    49,    50,    51,    52,     0,     0,
     256,   257,     0,   258,   259,   260,   261,   262,   263,   264,
       0,    53,     0,   265,   270,   271,     0,   272,   273,   274,
     275,   276,   277,   278,     0,     0,     0,   280
};

static const short yycheck[] =
{
       7,     8,    78,    78,    78,    78,    92,    93,    15,    45,
     111,   106,   106,   139,    78,   141,   142,   106,   106,    60,
     172,   173,   174,   175,     6,    64,    65,    68,    73,    74,
      75,    76,   184,     6,    98,   137,   137,    69,    70,    69,
      70,    14,   137,   137,   196,    52,   125,   126,   137,   137,
     137,   137,   137,   132,   133,   134,    69,    70,   106,   138,
     137,   137,   137,   137,   137,    47,   112,   113,   114,   115,
     116,   117,    69,    70,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
     106,   137,   106,   106,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
     256,   257,   258,   259,   260,   261,   262,   263,   264,    62,
      78,   137,   137,   106,   106,   152,   137,   154,   155,    78,
     137,   137,   107,   160,   125,   126,   107,   139,    78,   141,
     142,   132,   133,   134,    78,   137,   139,   137,   141,   142,
     106,   137,   106,   106,   106,   139,   137,   106,   107,    78,
     187,   137,   139,   112,   113,   114,   115,   116,   117,    29,
     137,   137,   199,   137,   137,   137,   125,   126,   137,   128,
     129,   130,   131,   132,   133,   134,   137,   106,   107,   138,
     137,   140,   141,   112,   113,   114,   115,   116,   117,   125,
     126,   137,   128,   129,   130,   131,   132,   133,   134,   137,
     129,   106,   138,   132,    74,   137,   142,   137,   137,   137,
     137,   140,   141,   137,   137,   137,   137,   137,   137,   137,
     137,   137,    92,    93,   137,   137,   137,   106,   265,   137,
     137,   137,    78,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   137,   280,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   137,   137,   137,   106,   106,   137,   106,   106,
     307,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   318,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,   125,   126,
     138,   128,   129,   130,   131,   132,   133,   134,   106,   137,
     106,   137,   106,   138,   137,   142,   137,   137,   137,   137,
     137,   137,    38,   137,   137,   137,    57,   137,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
     137,   106,   137,   106,   137,   137,   137,    78,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,   106,   106,   107,   137,   137,   137,
      96,   112,   113,   114,   115,   116,   117,   137,   137,    54,
      -1,    -1,    -1,   109,   110,    -1,    -1,    -1,   129,    -1,
      -1,    -1,   118,    -1,    -1,    -1,   137,    -1,   124,   140,
     141,   127,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    78,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,   106,   107,    -1,    -1,    -1,    -1,   112,   113,   114,
     115,   116,   117,   125,   126,    -1,   128,   129,   130,   131,
     132,   133,   134,    -1,   129,   137,   138,    -1,    -1,    58,
      -1,    -1,   137,    -1,    -1,   140,   141,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,   106,   107,    -1,
      -1,    -1,    -1,   112,   113,   114,   115,   116,   117,   125,
     126,    -1,   128,   129,   130,   131,   132,   133,   134,    -1,
     129,   137,   138,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   140,   141,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    78,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,   106,   107,    -1,    -1,    -1,    -1,   112,
     113,   114,   115,   116,   117,   125,   126,    -1,   128,   129,
     130,   131,   132,   133,   134,    -1,   129,   137,   138,    -1,
      -1,    -1,    -1,    -1,   137,    -1,    -1,   140,   141,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    -1,    -1,   125,
     126,    -1,   128,   129,   130,   131,   132,   133,   134,   106,
     107,   137,   138,    -1,    -1,   112,   113,   114,   115,   116,
     117,   125,   126,    -1,   128,   129,   130,   131,   132,   133,
     134,    -1,   129,   137,   138,    -1,    -1,    -1,    -1,    -1,
     137,    -1,    -1,   140,   141,    78,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,   106,   107,    -1,    -1,    -1,    -1,   112,
     113,   114,   115,   116,   117,   125,   126,    -1,   128,   129,
     130,   131,   132,   133,   134,    -1,   129,   137,   138,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   140,   141,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,   125,   126,    -1,   128,   129,
     130,   131,   132,   133,   134,    -1,    -1,   137,   138,   106,
     107,    -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,
     117,     0,   125,   126,    -1,   128,   129,   130,   131,   132,
     133,   134,   129,    -1,   137,   138,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   140,   141,    -1,    -1,    -1,    -1,    -1,
      29,    30,    31,    -1,    33,    -1,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    45,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,    -1,    -1,
      59,    -1,    61,    -1,    63,    -1,    -1,    66,    67,    -1,
      -1,    -1,    71,    72,    -1,    -1,    -1,    -1,    77,    -1,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    -1,    -1,    94,    95,    -1,    97,    -1,
      99,   100,   101,   102,   103,   104,   105,    -1,    -1,   108,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     119,   120,   121,   122,   123,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    29,    30,    31,    -1,    33,   137,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    45,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      56,    -1,    -1,    59,    -1,    61,    -1,    63,    -1,    -1,
      66,    67,    -1,    -1,    -1,    71,    72,    -1,    -1,    -1,
      -1,    77,    -1,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    -1,    -1,    94,    95,
      -1,    97,    -1,    99,   100,   101,   102,   103,   104,   105,
      -1,    -1,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   119,   120,   121,   122,   123,    -1,    -1,
     125,   126,    -1,   128,   129,   130,   131,   132,   133,   134,
      -1,   137,    -1,   138,   125,   126,    -1,   128,   129,   130,
     131,   132,   133,   134,    -1,    -1,    -1,   138
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    29,    30,    31,    33,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    45,    56,    59,    61,    63,    66,
      67,    71,    72,    77,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    94,    95,    97,
      99,   100,   101,   102,   103,   104,   105,   108,   119,   120,
     121,   122,   123,   137,   144,   145,   146,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   196,   137,   106,
     137,   137,    60,    68,   137,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,   106,   107,   112,   113,   114,   115,   116,   117,   129,
     137,   140,   141,   147,   190,   191,   192,   193,   194,    78,
     106,   107,   129,   137,   140,   141,   147,   193,   195,    78,
     132,   137,   195,   106,   106,   106,    38,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    96,
     109,   110,   118,   124,   127,   137,   193,    57,   137,   195,
     106,    62,    64,    65,   137,   137,    58,   169,   170,   195,
      73,    74,    75,    76,    78,   106,   106,   137,   137,   137,
     107,   107,    78,    78,   137,   106,   137,    29,    74,    92,
      93,   137,    92,    93,   137,    78,   137,    78,    98,    78,
     137,   137,   106,   137,    69,    70,    69,    70,   106,   106,
     147,   106,   137,   137,    78,   137,   195,     0,   145,   137,
     139,    69,    70,   194,   194,   194,   125,   126,   128,   129,
     130,   131,   132,   133,   134,   138,   137,   195,   195,   195,
     125,   126,   128,   129,   130,   131,   132,   133,   134,   137,
     138,   195,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   169,   169,   169,   169,   137,   137,   137,    78,
     137,   111,   137,   137,   137,   137,   169,   139,   137,   195,
     137,   106,   137,   106,   137,   137,   169,   137,   129,   195,
     137,   137,   137,   137,   137,   137,   106,   137,   137,   137,
     137,   137,   137,   137,   106,   106,   106,   106,   137,   106,
     137,    78,   137,   137,   137,   137,   137,   137,   137,   137,
     138,    69,    70,   137,   137,   106,   137,   137,   142,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   195,   142,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     137,   137,   137,   137,   137,   137,   106,   137,   195,   137,
     106,   137,   137,   195,   137,   137,   137,   137,   137,   137,
     106,   106,   137,   137,   137,   137,    45,   137,   137,   137,
     137,   137,   106,   137
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 47:
#line 221 "parser.y"
    {
      }
    break;

  case 48:
#line 227 "parser.y"
    {
		bx_dbg_diff_memory();
		free(yyvsp[-1].sval);
	}
    break;

  case 49:
#line 232 "parser.y"
    {
		bx_dbg_sync_memory(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
    break;

  case 50:
#line 237 "parser.y"
    {
		bx_dbg_sync_memory(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
    break;

  case 51:
#line 242 "parser.y"
    {
		bx_dbg_sync_cpu(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
    break;

  case 52:
#line 247 "parser.y"
    {
		bx_dbg_sync_cpu(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
    break;

  case 53:
#line 252 "parser.y"
    {
		free(yyvsp[-2].sval);
		bx_dbg_fast_forward(yyvsp[-1].uval);
	}
    break;

  case 54:
#line 257 "parser.y"
    {
	}
    break;

  case 55:
#line 260 "parser.y"
    {
		free(yyvsp[-4].sval);
		bx_dbg_info_address(yyvsp[-3].uval, yyvsp[-1].uval);
        }
    break;

  case 56:
#line 265 "parser.y"
    {
        }
    break;

  case 57:
#line 268 "parser.y"
    {
        }
    break;

  case 58:
#line 273 "parser.y"
    { yyval.uval = 1; }
    break;

  case 59:
#line 274 "parser.y"
    { yyval.uval = 0; }
    break;

  case 60:
#line 275 "parser.y"
    { yyval.uval = 2; }
    break;

  case 61:
#line 276 "parser.y"
    { yyval.uval = 3; }
    break;

  case 62:
#line 277 "parser.y"
    { yyval.uval = 4; }
    break;

  case 63:
#line 278 "parser.y"
    { yyval.uval = 5; }
    break;

  case 64:
#line 283 "parser.y"
    {
        bx_dbg_timebp_command(0, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
    break;

  case 65:
#line 288 "parser.y"
    {
        bx_dbg_timebp_command(1, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
    break;

  case 66:
#line 296 "parser.y"
    {
          bx_dbg_record_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 67:
#line 304 "parser.y"
    {
          bx_dbg_playback_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 68:
#line 312 "parser.y"
    {
          bx_dbg_modebp_command();
          free(yyvsp[-1].sval);
          }
    break;

  case 69:
#line 320 "parser.y"
    {
          bx_dbg_show_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 70:
#line 325 "parser.y"
    {
          bx_dbg_show_command(0);
          free(yyvsp[-1].sval);
          }
    break;

  case 71:
#line 333 "parser.y"
    {
        bx_dbg_ptime_command();
        free(yyvsp[-1].sval);
	}
    break;

  case 72:
#line 341 "parser.y"
    {
        bx_dbg_trace_on_command();
        free(yyvsp[-1].sval);
	}
    break;

  case 73:
#line 349 "parser.y"
    {
        bx_dbg_trace_off_command();
        free(yyvsp[-1].sval);
	}
    break;

  case 74:
#line 357 "parser.y"
    {
          bx_dbg_print_stack_command(16);
          free(yyvsp[-1].sval);
	  }
    break;

  case 75:
#line 362 "parser.y"
    {
          bx_dbg_print_stack_command(yyvsp[-1].uval);
          free(yyvsp[-2].sval);
	  }
    break;

  case 76:
#line 370 "parser.y"
    {
          watchpoint_continue = 0;
	  fprintf(stderr, "Will stop on watch points\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 77:
#line 376 "parser.y"
    {
          watchpoint_continue = 1;
          fprintf(stderr, "Will not stop on watch points (they will still be logged)\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 78:
#line 382 "parser.y"
    {
          bx_dbg_watch(-1, 0);
          free(yyvsp[-1].sval);
          }
    break;

  case 79:
#line 387 "parser.y"
    {
          bx_dbg_unwatch(-1, 0);
          free(yyvsp[-1].sval);
          }
    break;

  case 80:
#line 392 "parser.y"
    {
          bx_dbg_watch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 81:
#line 397 "parser.y"
    {
          bx_dbg_unwatch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 82:
#line 402 "parser.y"
    {
          bx_dbg_watch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 83:
#line 407 "parser.y"
    {
          bx_dbg_unwatch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 84:
#line 415 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-1].sval, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 85:
#line 420 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-2].sval, 0, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 86:
#line 425 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-2].sval, 1, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 87:
#line 433 "parser.y"
    {
        bx_dbg_where_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 88:
#line 441 "parser.y"
    {
        bx_dbg_print_string_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 89:
#line 449 "parser.y"
    {
        bx_dbg_continue_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 90:
#line 457 "parser.y"
    {
        bx_dbg_stepN_command(1);
        free(yyvsp[-1].sval);
        }
    break;

  case 91:
#line 462 "parser.y"
    {
        bx_dbg_stepN_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 92:
#line 470 "parser.y"
    {
        bx_dbg_step_over_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 93:
#line 478 "parser.y"
    {
        bx_dbg_set_command(yyvsp[-3].sval, yyvsp[-2].sval, yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 94:
#line 483 "parser.y"
    {
        bx_dbg_set_command(yyvsp[-3].sval, yyvsp[-2].sval, yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 95:
#line 488 "parser.y"
    {
        bx_dbg_set_symbol_command(yyvsp[-3].sval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
    break;

  case 96:
#line 496 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 97:
#line 501 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkRegular, yyvsp[-3].ulval, yyvsp[-1].ulval);
        free(yyvsp[-4].sval);
        }
    break;

  case 98:
#line 506 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 99:
#line 511 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-2].sval);
        }
    break;

  case 100:
#line 516 "parser.y"
    {
        bx_dbg_lbreakpoint_symbol_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval);free(yyvsp[-1].sval);
        }
    break;

  case 101:
#line 521 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 102:
#line 526 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-2].sval);
        }
    break;

  case 103:
#line 531 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-3].sval);
        }
    break;

  case 104:
#line 539 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 105:
#line 547 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free(yyvsp[-1].sval);
        }
    break;

  case 106:
#line 552 "parser.y"
    {
        bx_dbg_info_symbols_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval);free(yyvsp[-1].sval);
        }
    break;

  case 107:
#line 560 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 108:
#line 565 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 109:
#line 570 "parser.y"
    {
        bx_dbg_info_program_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 110:
#line 575 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 111:
#line 580 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 112:
#line 585 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS | BX_INFO_FPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 113:
#line 590 "parser.y"
    {
        bx_dbg_info_dirty_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
    break;

  case 114:
#line 595 "parser.y"
    {
        bx_dbg_info_idt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 115:
#line 600 "parser.y"
    {
       bx_dbg_info_ivt_command(yyvsp[-1].uval_range);
       free(yyvsp[-3].sval); free(yyvsp[-2].sval);
       }
    break;

  case 116:
#line 605 "parser.y"
    {
        bx_dbg_info_gdt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 117:
#line 610 "parser.y"
    {
        bx_dbg_info_ldt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 118:
#line 615 "parser.y"
    {
        bx_dbg_info_tss_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 119:
#line 620 "parser.y"
    {
        bx_dbg_info_flags();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 120:
#line 625 "parser.y"
    {
        bx_dbg_info_linux_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 121:
#line 630 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 122:
#line 635 "parser.y"
    {
        bx_dbg_info_symbols_command(yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 123:
#line 640 "parser.y"
    {
        bx_dbg_info_control_regs_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 124:
#line 645 "parser.y"
    {
        bx_dbg_info_ne2k(-1, -1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 125:
#line 650 "parser.y"
    {
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-1].uval, -1);
        }
    break;

  case 126:
#line 655 "parser.y"
    {
        free(yyvsp[-6].sval); free(yyvsp[-5].sval); free(yyvsp[-4].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-3].uval, yyvsp[-1].uval);
        }
    break;

  case 127:
#line 660 "parser.y"
    {
        bx_dbg_info_pic();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 128:
#line 665 "parser.y"
    {
        bx_dbg_info_vga();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 129:
#line 676 "parser.y"
    { yyval.uval_range = make_num_range (EMPTY_ARG, EMPTY_ARG); }
    break;

  case 131:
#line 681 "parser.y"
    {
    yyval.uval_range = make_num_range (yyvsp[0].ulval, yyvsp[0].ulval);
  }
    break;

  case 132:
#line 686 "parser.y"
    {
    yyval.uval_range = make_num_range (yyvsp[-1].ulval, yyvsp[0].ulval);
  }
    break;

  case 133:
#line 693 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 134:
#line 701 "parser.y"
    {
        bx_dbg_del_breakpoint_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 135:
#line 709 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command(yyvsp[-1].uval, 1);
        free(yyvsp[-2].sval);
       }
    break;

  case 136:
#line 716 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command(yyvsp[-1].uval, 0);
        free(yyvsp[-2].sval);
       }
    break;

  case 137:
#line 724 "parser.y"
    {
	  bx_dbg_quit_command();
	  free(yyvsp[-1].sval);
        }
    break;

  case 138:
#line 733 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].ulval,1, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].ulval,1, 1);
#endif
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 139:
#line 741 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 1);
#endif
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 140:
#line 749 "parser.y"
    {
        //FIXME HanishKVC This method of hunting thro all the 
        //      simulators may be better than using 2 calls if 
        //      BX_NUM_SIMULATORS greater than or equal to 2 as 
        //      done for other cases of BX_TOKEN_EXAMINE
        int iCurSim; 
        for(iCurSim = 0; iCurSim < BX_NUM_SIMULATORS; iCurSim++)
        {
          bx_dbg_examine_command(yyvsp[-2].sval, NULL,0, yyvsp[-1].ulval,1, iCurSim);
        }
        free(yyvsp[-2].sval);
        }
    break;

  case 141:
#line 762 "parser.y"
    {
        //FIXME HanishKVC This method of hunting thro all the 
        //      simulators may be better than using 2 calls if 
        //      BX_NUM_SIMULATORS greater than or equal to 2 as 
        //      done for other cases of BX_TOKEN_EXAMINE
        int iCurSim; 
        for(iCurSim = 0; iCurSim < BX_NUM_SIMULATORS; iCurSim++)
        {
          bx_dbg_examine_command(yyvsp[-1].sval, NULL,0, 0,0, iCurSim);
        }
        free(yyvsp[-1].sval);
        }
    break;

  case 142:
#line 778 "parser.y"
    {
        bx_dbg_setpmem_command(yyvsp[-3].uval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
    break;

  case 143:
#line 786 "parser.y"
    {
        bx_dbg_query_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 144:
#line 794 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 145:
#line 799 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-2].sval, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 146:
#line 804 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 147:
#line 812 "parser.y"
    {
        bx_dbg_set_cpu_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 148:
#line 820 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, yyvsp[-1].uval_range);
        free(yyvsp[-2].sval);
        }
    break;

  case 149:
#line 825 "parser.y"
    {
        bx_dbg_disassemble_command(yyvsp[-2].sval, yyvsp[-1].uval_range);
	free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 150:
#line 833 "parser.y"
    {
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 151:
#line 838 "parser.y"
    {
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 152:
#line 843 "parser.y"
    {
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 153:
#line 848 "parser.y"
    {
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 154:
#line 856 "parser.y"
    {
        bx_dbg_loader_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 155:
#line 864 "parser.y"
    {
        bx_dbg_doit_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 156:
#line 872 "parser.y"
    {
        bx_dbg_crc_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
    break;

  case 157:
#line 880 "parser.y"
    {
	bx_dbg_trace_reg_on_command();
	free(yyvsp[-1].sval);
	}
    break;

  case 158:
#line 888 "parser.y"
    {
	bx_dbg_trace_reg_off_command();
	free(yyvsp[-1].sval);
	}
    break;

  case 159:
#line 896 "parser.y"
    {
         bx_dbg_help_command(yyvsp[-1].sval);
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 160:
#line 901 "parser.y"
    {
         bx_dbg_help_command(0);
         free(yyvsp[-1].sval);
         }
    break;

  case 161:
#line 909 "parser.y"
    {
     bx_dbg_calc_command(yyvsp[-1].ulval);
     free(yyvsp[-2].sval);
     }
    break;

  case 177:
#line 932 "parser.y"
    { yyval.reg=yyvsp[0].reg; }
    break;

  case 181:
#line 940 "parser.y"
    { yyval.reg=yyvsp[0].reg; }
    break;

  case 187:
#line 950 "parser.y"
    { yyval.reg=yyvsp[0].reg; }
    break;

  case 190:
#line 957 "parser.y"
    { yyval.reg=yyvsp[0].reg; }
    break;

  case 191:
#line 962 "parser.y"
    { yyval.ulval = yyvsp[0].uval; }
    break;

  case 192:
#line 963 "parser.y"
    { yyval.ulval = yyvsp[0].ulval; }
    break;

  case 193:
#line 964 "parser.y"
    { yyval.ulval = bx_dbg_get_reg_value(yyvsp[0].reg); }
    break;

  case 194:
#line 965 "parser.y"
    { yyval.ulval = bx_dbg_get_selector_value(yyvsp[0].uval); }
    break;

  case 195:
#line 966 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval + yyvsp[0].ulval; }
    break;

  case 196:
#line 967 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval - yyvsp[0].ulval; }
    break;

  case 197:
#line 968 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval * yyvsp[0].ulval; }
    break;

  case 198:
#line 969 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval / yyvsp[0].ulval; }
    break;

  case 199:
#line 970 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval >> yyvsp[0].ulval; }
    break;

  case 200:
#line 971 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval << yyvsp[0].ulval; }
    break;

  case 201:
#line 972 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval | yyvsp[0].ulval; }
    break;

  case 202:
#line 973 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval ^ yyvsp[0].ulval; }
    break;

  case 203:
#line 974 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval & yyvsp[0].ulval; }
    break;

  case 204:
#line 975 "parser.y"
    { yyval.ulval = !yyvsp[0].ulval; }
    break;

  case 205:
#line 976 "parser.y"
    { yyval.ulval = -yyvsp[0].ulval; }
    break;

  case 206:
#line 977 "parser.y"
    { yyval.ulval = yyvsp[-1].ulval; }
    break;

  case 207:
#line 982 "parser.y"
    { yyval.ulval = yyvsp[0].uval; }
    break;

  case 208:
#line 983 "parser.y"
    { yyval.ulval = yyvsp[0].ulval; }
    break;

  case 209:
#line 984 "parser.y"
    { yyval.ulval = bx_dbg_get_symbol_value(yyvsp[0].sval); free(yyvsp[0].sval);}
    break;

  case 210:
#line 985 "parser.y"
    { yyval.ulval = bx_dbg_get_reg_value(yyvsp[0].reg);}
    break;

  case 211:
#line 986 "parser.y"
    { yyval.ulval = bx_dbg_get_selector_value(yyvsp[0].uval);}
    break;

  case 212:
#line 987 "parser.y"
    { yyval.ulval = bx_dbg_get_laddr (yyvsp[-2].ulval, yyvsp[0].ulval); }
    break;

  case 213:
#line 988 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval + yyvsp[0].ulval; }
    break;

  case 214:
#line 989 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval - yyvsp[0].ulval; }
    break;

  case 215:
#line 990 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval * yyvsp[0].ulval; }
    break;

  case 216:
#line 991 "parser.y"
    { yyval.ulval = (yyvsp[0].ulval != 0) ? yyvsp[-2].ulval / yyvsp[0].ulval : 0; }
    break;

  case 217:
#line 992 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval >> yyvsp[0].ulval; }
    break;

  case 218:
#line 993 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval << yyvsp[0].ulval; }
    break;

  case 219:
#line 994 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval | yyvsp[0].ulval; }
    break;

  case 220:
#line 995 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval ^ yyvsp[0].ulval; }
    break;

  case 221:
#line 996 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval & yyvsp[0].ulval; }
    break;

  case 222:
#line 997 "parser.y"
    { yyval.ulval = !yyvsp[0].ulval; }
    break;

  case 223:
#line 998 "parser.y"
    { yyval.ulval = -yyvsp[0].ulval; }
    break;

  case 224:
#line 999 "parser.y"
    { yyval.ulval = yyvsp[-1].ulval; }
    break;

  case 225:
#line 1004 "parser.y"
    { bx_dbg_set_reg_value(yyvsp[-3].reg, yyvsp[-1].ulval); }
    break;


    }

/* Line 999 of yacc.c.  */
#line 2934 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 1006 "parser.y"


#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
