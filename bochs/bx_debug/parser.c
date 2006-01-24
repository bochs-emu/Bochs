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
     BX_TOKEN_REGISTERS = 299,
     BX_TOKEN_CPU = 300,
     BX_TOKEN_FPU = 301,
     BX_TOKEN_SSE = 302,
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
     BX_TOKEN_ON = 323,
     BX_TOKEN_OFF = 324,
     BX_TOKEN_DISASSEMBLE = 325,
     BX_TOKEN_INSTRUMENT = 326,
     BX_TOKEN_START = 327,
     BX_TOKEN_STOP = 328,
     BX_TOKEN_RESET = 329,
     BX_TOKEN_PRINT = 330,
     BX_TOKEN_LOADER = 331,
     BX_TOKEN_STRING = 332,
     BX_TOKEN_DOIT = 333,
     BX_TOKEN_CRC = 334,
     BX_TOKEN_TRACEON = 335,
     BX_TOKEN_TRACEOFF = 336,
     BX_TOKEN_SWITCH_MODE = 337,
     BX_TOKEN_SIZE = 338,
     BX_TOKEN_PTIME = 339,
     BX_TOKEN_TIMEBP_ABSOLUTE = 340,
     BX_TOKEN_TIMEBP = 341,
     BX_TOKEN_RECORD = 342,
     BX_TOKEN_PLAYBACK = 343,
     BX_TOKEN_MODEBP = 344,
     BX_TOKEN_PRINT_STACK = 345,
     BX_TOKEN_WATCH = 346,
     BX_TOKEN_UNWATCH = 347,
     BX_TOKEN_READ = 348,
     BX_TOKEN_WRITE = 349,
     BX_TOKEN_SHOW = 350,
     BX_TOKEN_LOAD_SYMBOLS = 351,
     BX_TOKEN_SYMBOLS = 352,
     BX_TOKEN_LIST_SYMBOLS = 353,
     BX_TOKEN_GLOBAL = 354,
     BX_TOKEN_WHERE = 355,
     BX_TOKEN_PRINT_STRING = 356,
     BX_TOKEN_DIFF_MEMORY = 357,
     BX_TOKEN_SYNC_MEMORY = 358,
     BX_TOKEN_SYNC_CPU = 359,
     BX_TOKEN_FAST_FORWARD = 360,
     BX_TOKEN_PHY_2_LOG = 361,
     BX_TOKEN_NUMERIC = 362,
     BX_TOKEN_LONG_NUMERIC = 363,
     BX_TOKEN_INFO_ADDRESS = 364,
     BX_TOKEN_NE2000 = 365,
     BX_TOKEN_PIC = 366,
     BX_TOKEN_PAGE = 367,
     BX_TOKEN_CS = 368,
     BX_TOKEN_ES = 369,
     BX_TOKEN_SS = 370,
     BX_TOKEN_DS = 371,
     BX_TOKEN_FS = 372,
     BX_TOKEN_GS = 373,
     BX_TOKEN_FLAGS = 374,
     BX_TOKEN_ALWAYS_CHECK = 375,
     BX_TOKEN_TRACEREGON = 376,
     BX_TOKEN_TRACEREGOFF = 377,
     BX_TOKEN_HELP = 378,
     BX_TOKEN_CALC = 379,
     BX_TOKEN_VGA = 380,
     BX_TOKEN_RSHIFT = 381,
     BX_TOKEN_LSHIFT = 382,
     BX_TOKEN_IVT = 383,
     NEG = 384,
     NOT = 385
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
#define BX_TOKEN_REGISTERS 299
#define BX_TOKEN_CPU 300
#define BX_TOKEN_FPU 301
#define BX_TOKEN_SSE 302
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
#define BX_TOKEN_ON 323
#define BX_TOKEN_OFF 324
#define BX_TOKEN_DISASSEMBLE 325
#define BX_TOKEN_INSTRUMENT 326
#define BX_TOKEN_START 327
#define BX_TOKEN_STOP 328
#define BX_TOKEN_RESET 329
#define BX_TOKEN_PRINT 330
#define BX_TOKEN_LOADER 331
#define BX_TOKEN_STRING 332
#define BX_TOKEN_DOIT 333
#define BX_TOKEN_CRC 334
#define BX_TOKEN_TRACEON 335
#define BX_TOKEN_TRACEOFF 336
#define BX_TOKEN_SWITCH_MODE 337
#define BX_TOKEN_SIZE 338
#define BX_TOKEN_PTIME 339
#define BX_TOKEN_TIMEBP_ABSOLUTE 340
#define BX_TOKEN_TIMEBP 341
#define BX_TOKEN_RECORD 342
#define BX_TOKEN_PLAYBACK 343
#define BX_TOKEN_MODEBP 344
#define BX_TOKEN_PRINT_STACK 345
#define BX_TOKEN_WATCH 346
#define BX_TOKEN_UNWATCH 347
#define BX_TOKEN_READ 348
#define BX_TOKEN_WRITE 349
#define BX_TOKEN_SHOW 350
#define BX_TOKEN_LOAD_SYMBOLS 351
#define BX_TOKEN_SYMBOLS 352
#define BX_TOKEN_LIST_SYMBOLS 353
#define BX_TOKEN_GLOBAL 354
#define BX_TOKEN_WHERE 355
#define BX_TOKEN_PRINT_STRING 356
#define BX_TOKEN_DIFF_MEMORY 357
#define BX_TOKEN_SYNC_MEMORY 358
#define BX_TOKEN_SYNC_CPU 359
#define BX_TOKEN_FAST_FORWARD 360
#define BX_TOKEN_PHY_2_LOG 361
#define BX_TOKEN_NUMERIC 362
#define BX_TOKEN_LONG_NUMERIC 363
#define BX_TOKEN_INFO_ADDRESS 364
#define BX_TOKEN_NE2000 365
#define BX_TOKEN_PIC 366
#define BX_TOKEN_PAGE 367
#define BX_TOKEN_CS 368
#define BX_TOKEN_ES 369
#define BX_TOKEN_SS 370
#define BX_TOKEN_DS 371
#define BX_TOKEN_FS 372
#define BX_TOKEN_GS 373
#define BX_TOKEN_FLAGS 374
#define BX_TOKEN_ALWAYS_CHECK 375
#define BX_TOKEN_TRACEREGON 376
#define BX_TOKEN_TRACEREGOFF 377
#define BX_TOKEN_HELP 378
#define BX_TOKEN_CALC 379
#define BX_TOKEN_VGA 380
#define BX_TOKEN_RSHIFT 381
#define BX_TOKEN_LSHIFT 382
#define BX_TOKEN_IVT 383
#define NEG 384
#define NOT 385




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
#line 366 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 378 "y.tab.c"

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
#define YYFINAL  288
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1317

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  144
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  54
/* YYNRULES -- Number of rules. */
#define YYNRULES  266
/* YYNRULES -- Number of states. */
#define YYNSTATES  497

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   385

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     138,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   141,     2,     2,     2,     2,   135,     2,
     142,   143,   133,   129,     2,   130,     2,   134,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   139,     2,
       2,   140,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   132,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   131,     2,     2,     2,     2,     2,
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
     125,   126,   127,   128,   136,   137
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
     251,   254,   258,   261,   266,   271,   277,   283,   286,   292,
     295,   299,   303,   306,   310,   315,   318,   321,   325,   329,
     333,   337,   341,   345,   349,   353,   358,   363,   368,   373,
     378,   382,   386,   390,   395,   399,   403,   409,   417,   421,
     425,   426,   428,   430,   433,   436,   439,   443,   447,   451,
     454,   459,   463,   467,   470,   476,   480,   484,   489,   493,
     496,   500,   505,   509,   515,   519,   523,   527,   531,   535,
     539,   544,   547,   550,   554,   558,   562,   566,   570,   574,
     578,   582,   586,   590,   594,   598,   602,   606,   610,   614,
     618,   622,   626,   630,   634,   638,   642,   646,   650,   654,
     658,   662,   666,   670,   674,   678,   682,   686,   690,   694,
     698,   702,   706,   709,   713,   715,   717,   719,   721,   723,
     725,   727,   729,   731,   733,   735,   737,   739,   741,   743,
     745,   747,   749,   751,   753,   755,   757,   759,   761,   763,
     765,   767,   769,   771,   773,   775,   777,   779,   783,   787,
     791,   795,   799,   803,   807,   811,   815,   818,   821,   825,
     827,   829,   831,   833,   835,   839,   843,   847,   851,   855,
     859,   863,   867,   871,   875,   878,   881
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     145,     0,    -1,   145,   146,    -1,   146,    -1,   162,    -1,
     163,    -1,   164,    -1,   165,    -1,   166,    -1,   169,    -1,
     172,    -1,   167,    -1,   168,    -1,   173,    -1,   174,    -1,
     175,    -1,   176,    -1,   177,    -1,   178,    -1,   179,    -1,
     180,    -1,   181,    -1,   182,    -1,   183,    -1,   184,    -1,
     185,    -1,   186,    -1,   187,    -1,   155,    -1,   156,    -1,
     154,    -1,   149,    -1,   150,    -1,   151,    -1,   152,    -1,
     157,    -1,   158,    -1,   153,    -1,   159,    -1,   160,    -1,
     161,    -1,   147,    -1,   188,    -1,   189,    -1,   190,    -1,
     191,    -1,    -1,   138,    -1,   102,   138,    -1,   103,    68,
     138,    -1,   103,    69,   138,    -1,   104,    68,   138,    -1,
     104,    69,   138,    -1,   105,   107,   138,    -1,   106,   107,
     138,    -1,   109,   148,   139,   107,   138,    -1,   120,   107,
      68,   138,    -1,   120,   107,    69,   138,    -1,   113,    -1,
     114,    -1,   115,    -1,   116,    -1,   117,    -1,   118,    -1,
      86,   108,   138,    -1,    85,   108,   138,    -1,    87,    77,
     138,    -1,    88,    77,   138,    -1,    89,   138,    -1,    95,
      77,   138,    -1,    95,   138,    -1,    84,   138,    -1,    80,
     138,    -1,    81,   138,    -1,    90,   138,    -1,    90,   107,
     138,    -1,    91,    73,   138,    -1,    91,    29,   138,    -1,
      91,   138,    -1,    92,   138,    -1,    91,    93,   107,   138,
      -1,    92,    93,   107,   138,    -1,    91,    94,   107,   138,
      -1,    92,    94,   107,   138,    -1,    96,    77,   138,    -1,
      96,    77,   107,   138,    -1,    96,    99,    77,   107,   138,
      -1,   100,   138,    -1,   101,   107,   138,    -1,    29,   138,
      -1,    30,   138,    -1,    30,   107,   138,    -1,    31,   138,
      -1,    33,    70,    68,   138,    -1,    33,    70,    69,   138,
      -1,    33,    60,   140,   107,   138,    -1,    33,   195,   140,
     197,   138,    -1,    36,   138,    -1,    36,   196,   139,   197,
     138,    -1,    37,   138,    -1,    37,   197,   138,    -1,    37,
      77,   138,    -1,    38,   138,    -1,    38,   197,   138,    -1,
      38,   133,   197,   138,    -1,    35,   138,    -1,    98,   138,
      -1,    98,    77,   138,    -1,    42,    38,   138,    -1,    42,
      45,   138,    -1,    42,    44,   138,    -1,    42,    46,   138,
      -1,    42,    47,   138,    -1,    42,    48,   138,    -1,    42,
      53,   138,    -1,    42,    49,   170,   138,    -1,    42,   128,
     170,   138,    -1,    42,    50,   170,   138,    -1,    42,    51,
     170,   138,    -1,    42,    52,   170,   138,    -1,    42,   119,
     138,    -1,    42,    54,   138,    -1,    42,    97,   138,    -1,
      42,    97,    77,   138,    -1,    42,    55,   138,    -1,    42,
     110,   138,    -1,    42,   110,   112,   107,   138,    -1,    42,
     110,   112,   107,    44,   107,   138,    -1,    42,   111,   138,
      -1,    42,   125,   138,    -1,    -1,   171,    -1,   197,    -1,
     197,   197,    -1,    44,   138,    -1,    66,   138,    -1,    39,
     107,   138,    -1,    40,   107,   138,    -1,    41,   107,   138,
      -1,    43,   138,    -1,    56,    57,   197,   138,    -1,    56,
      57,   138,    -1,    56,   197,   138,    -1,    56,   138,    -1,
      59,   107,   107,   107,   138,    -1,    61,    62,   138,    -1,
      63,    64,   138,    -1,    63,    64,   107,   138,    -1,    63,
      65,   138,    -1,    67,   138,    -1,    70,   170,   138,    -1,
      70,    58,   170,   138,    -1,    70,    82,   138,    -1,    70,
      83,   140,   107,   138,    -1,    71,    72,   138,    -1,    71,
      73,   138,    -1,    71,    74,   138,    -1,    71,    75,   138,
      -1,    76,    77,   138,    -1,    78,   107,   138,    -1,    79,
     107,   107,   138,    -1,   121,   138,    -1,   122,   138,    -1,
     123,    43,   138,    -1,   123,    29,   138,    -1,   123,    30,
     138,    -1,   123,    31,   138,    -1,   123,    36,   138,    -1,
     123,    37,   138,    -1,   123,    39,   138,    -1,   123,    40,
     138,    -1,   123,    41,   138,    -1,   123,    35,   138,    -1,
     123,    89,   138,    -1,   123,    79,   138,    -1,   123,    80,
     138,    -1,   123,    81,   138,    -1,   123,   121,   138,    -1,
     123,   122,   138,    -1,   123,    66,   138,    -1,   123,    67,
     138,    -1,   123,    84,   138,    -1,   123,    86,   138,    -1,
     123,    85,   138,    -1,   123,    90,   138,    -1,   123,    87,
     138,    -1,   123,    88,   138,    -1,   123,    96,   138,    -1,
     123,    98,   138,    -1,   123,    44,   138,    -1,   123,    59,
     138,    -1,   123,    70,   138,    -1,   123,    91,   138,    -1,
     123,    92,   138,    -1,   123,    56,   138,    -1,   123,    71,
     138,    -1,   123,    33,   138,    -1,   123,    42,   138,    -1,
     123,    95,   138,    -1,   123,   124,   138,    -1,   123,   123,
     138,    -1,   123,    77,   138,    -1,   123,   138,    -1,   124,
     197,   138,    -1,     3,    -1,     4,    -1,     5,    -1,     6,
      -1,     7,    -1,     8,    -1,     9,    -1,    10,    -1,    11,
      -1,    12,    -1,    13,    -1,    14,    -1,    15,    -1,    16,
      -1,    17,    -1,    18,    -1,    19,    -1,    20,    -1,    24,
      -1,    25,    -1,    21,    -1,    22,    -1,    23,    -1,    26,
      -1,    27,    -1,    28,    -1,   192,    -1,   193,    -1,   194,
      -1,   107,    -1,   108,    -1,   195,    -1,   148,    -1,   196,
     129,   196,    -1,   196,   130,   196,    -1,   196,   133,   196,
      -1,   196,   134,   196,    -1,   196,   126,   196,    -1,   196,
     127,   196,    -1,   196,   131,   196,    -1,   196,   132,   196,
      -1,   196,   135,   196,    -1,   141,   196,    -1,   130,   196,
      -1,   142,   196,   143,    -1,   107,    -1,   108,    -1,    77,
      -1,   195,    -1,   148,    -1,   197,   139,   197,    -1,   197,
     129,   197,    -1,   197,   130,   197,    -1,   197,   133,   197,
      -1,   197,   134,   197,    -1,   197,   126,   197,    -1,   197,
     127,   197,    -1,   197,   131,   197,    -1,   197,   132,   197,
      -1,   197,   135,   197,    -1,   141,   197,    -1,   130,   197,
      -1,   142,   197,   143,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   173,   173,   174,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   227,   232,
     237,   242,   247,   252,   257,   260,   265,   268,   274,   275,
     276,   277,   278,   279,   283,   288,   296,   304,   312,   320,
     325,   333,   341,   349,   357,   362,   370,   376,   382,   387,
     392,   397,   402,   407,   415,   420,   425,   433,   441,   449,
     457,   462,   470,   478,   483,   488,   493,   500,   505,   510,
     515,   520,   525,   530,   535,   543,   551,   556,   564,   569,
     574,   579,   584,   589,   594,   599,   604,   609,   614,   619,
     624,   629,   634,   639,   644,   649,   654,   659,   664,   669,
     681,   682,   685,   690,   696,   704,   712,   720,   727,   735,
     744,   752,   760,   773,   789,   797,   805,   810,   815,   823,
     831,   836,   841,   846,   854,   855,   856,   857,   865,   873,
     881,   889,   897,   905,   910,   915,   920,   925,   930,   935,
     940,   945,   950,   955,   960,   965,   970,   975,   980,   985,
     990,   995,  1000,  1005,  1010,  1015,  1020,  1025,  1030,  1035,
    1040,  1045,  1054,  1063,  1070,  1083,  1091,  1101,  1121,  1135,
    1145,  1150,  1156,  1164,  1172,  1173,  1174,  1175,  1176,  1177,
    1178,  1179,  1180,  1181,  1182,  1183,  1184,  1185,  1186,  1187,
    1192,  1193,  1194,  1195,  1200,  1201,  1202,  1203,  1204,  1205,
    1210,  1211,  1212,  1218,  1219,  1220,  1221,  1222,  1223,  1224,
    1225,  1226,  1227,  1228,  1229,  1230,  1231,  1232,  1233,  1238,
    1239,  1240,  1241,  1242,  1243,  1244,  1245,  1246,  1247,  1248,
    1249,  1250,  1251,  1252,  1253,  1254,  1255
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
  "BX_TOKEN_INFO", "BX_TOKEN_QUIT", "BX_TOKEN_REGISTERS", "BX_TOKEN_CPU", 
  "BX_TOKEN_FPU", "BX_TOKEN_SSE", "BX_TOKEN_ALL", "BX_TOKEN_IDT", 
  "BX_TOKEN_GDT", "BX_TOKEN_LDT", "BX_TOKEN_TSS", "BX_TOKEN_DIRTY", 
  "BX_TOKEN_LINUX", "BX_TOKEN_CONTROL_REGS", "BX_TOKEN_EXAMINE", 
  "BX_TOKEN_XFORMAT", "BX_TOKEN_DISFORMAT", "BX_TOKEN_SETPMEM", 
  "BX_TOKEN_SYMBOLNAME", "BX_TOKEN_QUERY", "BX_TOKEN_PENDING", 
  "BX_TOKEN_TAKE", "BX_TOKEN_DMA", "BX_TOKEN_IRQ", "BX_TOKEN_DUMP_CPU", 
  "BX_TOKEN_SET_CPU", "BX_TOKEN_ON", "BX_TOKEN_OFF", 
  "BX_TOKEN_DISASSEMBLE", "BX_TOKEN_INSTRUMENT", "BX_TOKEN_START", 
  "BX_TOKEN_STOP", "BX_TOKEN_RESET", "BX_TOKEN_PRINT", "BX_TOKEN_LOADER", 
  "BX_TOKEN_STRING", "BX_TOKEN_DOIT", "BX_TOKEN_CRC", "BX_TOKEN_TRACEON", 
  "BX_TOKEN_TRACEOFF", "BX_TOKEN_SWITCH_MODE", "BX_TOKEN_SIZE", 
  "BX_TOKEN_PTIME", "BX_TOKEN_TIMEBP_ABSOLUTE", "BX_TOKEN_TIMEBP", 
  "BX_TOKEN_RECORD", "BX_TOKEN_PLAYBACK", "BX_TOKEN_MODEBP", 
  "BX_TOKEN_PRINT_STACK", "BX_TOKEN_WATCH", "BX_TOKEN_UNWATCH", 
  "BX_TOKEN_READ", "BX_TOKEN_WRITE", "BX_TOKEN_SHOW", 
  "BX_TOKEN_LOAD_SYMBOLS", "BX_TOKEN_SYMBOLS", "BX_TOKEN_LIST_SYMBOLS", 
  "BX_TOKEN_GLOBAL", "BX_TOKEN_WHERE", "BX_TOKEN_PRINT_STRING", 
  "BX_TOKEN_DIFF_MEMORY", "BX_TOKEN_SYNC_MEMORY", "BX_TOKEN_SYNC_CPU", 
  "BX_TOKEN_FAST_FORWARD", "BX_TOKEN_PHY_2_LOG", "BX_TOKEN_NUMERIC", 
  "BX_TOKEN_LONG_NUMERIC", "BX_TOKEN_INFO_ADDRESS", "BX_TOKEN_NE2000", 
  "BX_TOKEN_PIC", "BX_TOKEN_PAGE", "BX_TOKEN_CS", "BX_TOKEN_ES", 
  "BX_TOKEN_SS", "BX_TOKEN_DS", "BX_TOKEN_FS", "BX_TOKEN_GS", 
  "BX_TOKEN_FLAGS", "BX_TOKEN_ALWAYS_CHECK", "BX_TOKEN_TRACEREGON", 
  "BX_TOKEN_TRACEREGOFF", "BX_TOKEN_HELP", "BX_TOKEN_CALC", 
  "BX_TOKEN_VGA", "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT", "BX_TOKEN_IVT", 
  "'+'", "'-'", "'|'", "'^'", "'*'", "'/'", "'&'", "NEG", "NOT", "'\\n'", 
  "':'", "'='", "'!'", "'('", "')'", "$accept", "commands", "command", 
  "cosim_commands", "segment_register", "timebp_command", 
  "record_command", "playback_command", "modebp_command", "show_command", 
  "ptime_command", "trace_on_command", "trace_off_command", 
  "print_stack_command", "watch_point_command", "symbol_command", 
  "where_command", "print_string_command", "continue_command", 
  "stepN_command", "step_over_command", "set_command", 
  "breakpoint_command", "blist_command", "slist_command", "info_command", 
  "optional_numeric_range", "numeric_range", "regs_command", 
  "dump_cpu_command", "delete_command", "bpe_command", "bpd_command", 
  "quit_command", "examine_command", "setpmem_command", "query_command", 
  "take_command", "set_cpu_command", "disassemble_command", 
  "instrument_command", "loader_command", "doit_command", "crc_command", 
  "trace_reg_on_command", "trace_reg_off_command", "help_command", 
  "calc_command", "BX_TOKEN_COMMON_REG", "BX_TOKEN_INDEX_REG", 
  "BX_TOKEN_PTR_REG", "BX_TOKEN_NONSEG_REG", "vexpression", "expression", 0
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
     375,   376,   377,   378,   379,   380,   381,   382,   383,    43,
      45,   124,    94,    42,    47,    38,   384,   385,    10,    58,
      61,    33,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   144,   145,   145,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   148,   148,
     148,   148,   148,   148,   149,   149,   150,   151,   152,   153,
     153,   154,   155,   156,   157,   157,   158,   158,   158,   158,
     158,   158,   158,   158,   159,   159,   159,   160,   161,   162,
     163,   163,   164,   165,   165,   165,   165,   166,   166,   166,
     166,   166,   166,   166,   166,   167,   168,   168,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     170,   170,   171,   171,   172,   173,   174,   175,   176,   177,
     178,   178,   178,   178,   179,   180,   181,   181,   181,   182,
     183,   183,   183,   183,   184,   184,   184,   184,   185,   186,
     187,   188,   189,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   191,   192,   192,   192,   192,   192,   192,
     192,   192,   192,   192,   192,   192,   192,   192,   192,   192,
     193,   193,   193,   193,   194,   194,   194,   194,   194,   194,
     195,   195,   195,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197
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
       2,     3,     2,     4,     4,     5,     5,     2,     5,     2,
       3,     3,     2,     3,     4,     2,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     4,     4,     4,     4,     4,
       3,     3,     3,     4,     3,     3,     5,     7,     3,     3,
       0,     1,     1,     2,     2,     2,     3,     3,     3,     2,
       4,     3,     3,     2,     5,     3,     3,     4,     3,     2,
       3,     4,     3,     5,     3,     3,     3,     3,     3,     3,
       4,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     3,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
      46,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   130,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    47,     0,     3,    41,    31,    32,    33,
      34,    37,    30,    28,    29,    35,    36,    38,    39,    40,
       4,     5,     6,     7,     8,    11,    12,     9,    10,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    42,    43,    44,    45,    89,     0,
      90,    92,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     224,   225,   226,   222,   223,   227,   228,   229,     0,     0,
     230,   231,   232,     0,   105,   233,   234,    58,    59,    60,
      61,    62,    63,     0,    97,     0,     0,   236,   235,     0,
     251,   249,   250,     0,    99,     0,     0,   253,   252,     0,
     251,     0,   102,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   130,   130,   130,   130,     0,     0,     0,
       0,     0,     0,     0,     0,   130,   139,   134,     0,   143,
       0,     0,     0,     0,     0,   135,   149,   130,     0,     0,
       0,   131,   132,     0,     0,     0,     0,     0,     0,     0,
      72,    73,    71,     0,     0,     0,     0,    68,     0,    74,
       0,     0,     0,     0,    78,     0,     0,    79,     0,    70,
       0,     0,     0,   106,    87,     0,    48,     0,     0,     0,
       0,     0,     0,     0,     0,   161,   162,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   202,     0,     1,     2,
      91,     0,     0,     0,     0,   247,   246,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   101,   265,
     264,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   100,     0,     0,   103,   136,   137,   138,   108,   110,
     109,   111,   112,   113,     0,     0,     0,     0,   114,   121,
     124,     0,   122,     0,   125,   128,   120,   129,     0,   141,
       0,   142,     0,   145,     0,   146,   148,     0,   152,     0,
     150,     0,   133,   154,   155,   156,   157,   158,   159,     0,
      65,    64,    66,    67,    75,    77,    76,     0,     0,     0,
       0,    69,     0,    84,     0,   107,    88,    49,    50,    51,
      52,    53,    54,     0,     0,     0,   164,   165,   166,   196,
     172,   167,   168,   169,   170,   171,   197,   163,   189,   194,
     190,   179,   180,   191,   195,   201,   174,   175,   176,   181,
     183,   182,   185,   186,   173,   184,   192,   193,   198,   187,
     188,   177,   178,   200,   199,   203,     0,    93,    94,     0,
     248,   241,   242,   237,   238,   243,   244,   239,   240,   245,
       0,   266,   259,   260,   255,   256,   261,   262,   257,   258,
     263,   254,   104,   115,   117,   118,   119,   123,     0,   116,
     140,     0,   147,   151,     0,   256,   160,    80,    82,    81,
      83,    85,     0,     0,    56,    57,    95,    96,    98,     0,
     126,   144,   153,    86,    55,     0,   127
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    54,    55,    56,   157,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,   200,   201,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,   130,   131,
     132,   158,   149,   202
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -160
static const short yypact[] =
{
    1079,  -118,   -98,  -115,   906,  -110,   115,   664,   492,   -63,
     -59,   -57,   296,   -92,   -80,   524,   -45,    -1,   -46,   -51,
       9,   352,   -38,    -6,   -21,    43,    13,    15,    17,    44,
      49,    79,    83,    30,   -97,   -24,   -87,   -75,   -52,   -74,
      31,    67,    37,   -37,    14,    69,    70,   -61,    71,    41,
      42,  1179,   739,  -160,   975,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,    53,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,    54,    48,
    -160,  -160,  -160,    56,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,   879,  -160,   879,   879,  -160,  -160,   441,
      59,  -160,  -160,   739,  -160,   739,   739,  -160,  -160,   -54,
    -160,   739,  -160,   147,    66,    68,    72,    75,    87,    98,
     106,   110,   111,   739,   739,   739,   739,   112,   114,   116,
     -73,   -99,   117,   122,   130,   739,  -160,  -160,   696,  -160,
     163,   100,   131,   -96,   133,  -160,  -160,   739,   134,    65,
     145,  -160,    85,   146,   150,   153,   161,   165,   166,   101,
    -160,  -160,  -160,   176,   197,   198,   199,  -160,   200,  -160,
     201,   214,   102,   139,  -160,   246,   274,  -160,   254,  -160,
     -95,   144,   260,  -160,  -160,   273,  -160,   275,   276,   278,
     279,   280,   281,   245,    76,  -160,  -160,   282,   284,   285,
     287,   288,   289,   290,   292,   293,   294,   295,   300,   308,
     309,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     323,   324,   325,   326,   335,   343,   347,   348,   349,   350,
     351,   353,   354,   383,   384,   385,  -160,   256,  -160,  -160,
    -160,   305,   386,   387,   739,  -160,  -160,   132,   879,   879,
     879,   879,   879,   879,   879,   879,   879,   739,  -160,   416,
     416,   108,   739,   739,   739,   739,   739,   739,   739,   739,
     739,  -160,   739,   270,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,   388,   425,   426,   439,  -160,  -160,
    -160,   440,  -160,   472,  -160,  -160,  -160,  -160,   446,  -160,
     310,  -160,   485,  -160,   455,  -160,  -160,   458,  -160,   487,
    -160,   739,   456,  -160,  -160,  -160,  -160,  -160,  -160,   459,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,   460,   464,   465,
     466,  -160,   473,  -160,   505,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,   506,   476,   477,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,   478,  -160,  -160,   345,
    -160,  -160,  -160,   -67,   -67,   -67,   -67,  -160,  -160,  -160,
     427,  -160,   416,   416,    32,    32,    32,    32,   416,   416,
     416,   456,  -160,  -160,  -160,  -160,  -160,  -160,   -23,  -160,
    -160,   479,  -160,  -160,   480,    32,  -160,  -160,  -160,  -160,
    -160,  -160,   481,   482,  -160,  -160,  -160,  -160,  -160,   514,
    -160,  -160,  -160,  -160,  -160,   486,  -160
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -160,  -160,   216,  -160,    27,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -159,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,    18,  -116,    -7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned short yytable[] =
{
     159,   163,   228,   232,   341,   220,   225,   226,   190,    99,
     218,   354,   382,   343,   334,   335,   336,   337,   193,   194,
      98,   489,   133,   101,   148,   230,   348,   295,   134,   296,
     297,   237,   238,   147,   203,   204,   205,   206,   357,   344,
     100,   219,   355,   383,   164,   287,   186,   231,   165,   221,
     166,   227,   137,   138,   139,   140,   141,   142,   187,   298,
     299,   192,   191,   229,   233,   342,   304,   305,   306,   222,
     223,   207,   312,   313,   243,   314,   315,   316,   317,   318,
     319,   320,   239,   240,   321,   322,   208,   195,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   224,   490,   292,   293,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   394,   395,   309,   196,   310,   311,
     209,   210,   213,   211,   323,   212,   215,   214,   312,   313,
     216,   148,   160,   148,   148,   318,   319,   320,   217,   234,
     147,   322,   147,   147,   235,   236,   241,   242,   244,   245,
     246,   350,   441,   442,   443,   444,   445,   446,   447,   448,
     449,   290,   151,   152,   291,   362,   294,   308,   137,   138,
     139,   140,   141,   142,   325,   359,   326,   352,   369,   377,
     327,   312,   313,   328,   314,   361,   316,   317,   318,   319,
     320,   384,   135,   136,   322,   329,   155,   156,   137,   138,
     139,   140,   141,   142,   312,   313,   330,   314,   315,   316,
     317,   318,   319,   320,   331,   143,   378,   322,   332,   333,
     338,   451,   339,   144,   340,   345,   145,   146,   298,   299,
     346,   300,   301,   302,   303,   304,   305,   306,   347,   353,
     289,   356,   358,   312,   313,   440,   314,   315,   316,   317,
     318,   319,   320,   360,   363,   324,   322,   439,   364,   312,
     313,   365,   314,   315,   316,   317,   318,   319,   320,   366,
     450,   351,   322,   367,   368,   452,   453,   454,   455,   456,
     457,   458,   459,   460,   370,   461,   148,   148,   148,   148,
     148,   148,   148,   148,   148,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   167,   371,   372,   373,   374,   375,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   376,   379,   475,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   380,   312,   313,   393,   314,   315,   316,   317,   318,
     319,   320,   381,   180,   435,   322,   312,   313,   385,   314,
     315,   316,   317,   318,   319,   320,   181,   182,   462,   322,
     197,   386,   436,   387,   388,   183,   389,   390,   391,   392,
     396,   184,   397,   398,   185,   399,   400,   401,   402,   160,
     403,   404,   405,   406,   198,   199,   312,   313,   407,   314,
     315,   316,   317,   318,   319,   320,   408,   409,   470,   322,
     410,   411,   412,   413,   414,   415,   416,   417,   418,   151,
     152,   419,   420,   421,   422,   137,   138,   139,   140,   141,
     142,   312,   313,   423,   314,   315,   316,   317,   318,   319,
     320,   424,   153,   487,   322,   425,   426,   427,   428,   429,
       0,   430,   431,   155,   156,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   432,   433,   434,   437,   438,   463,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   312,   313,   322,   314,   315,   316,   317,
     318,   319,   320,   464,   465,   488,   322,   298,   299,   160,
     300,   301,   302,   303,   304,   305,   306,   466,   467,   468,
     307,   188,   312,   313,   469,   314,   315,   316,   317,   318,
     319,   320,   471,   472,   474,   322,   473,   476,   477,   151,
     152,   160,   478,   479,   480,   137,   138,   139,   140,   141,
     142,   481,   482,   483,   484,   485,   486,   491,   492,   493,
     494,   495,   153,     0,   496,   161,     0,     0,     0,     0,
     162,   151,   152,   155,   156,     0,     0,   137,   138,   139,
     140,   141,   142,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   153,     0,     0,     0,     0,     0,
       0,     0,   189,     0,     0,   155,   156,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,     0,     0,     0,     0,     0,     0,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   150,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,     0,     0,
       0,   151,   152,   160,     0,     0,     0,   137,   138,   139,
     140,   141,   142,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   153,     0,     0,     0,     0,     0,
       0,     0,   154,   151,   152,   155,   156,     0,     0,   137,
     138,   139,   140,   141,   142,     0,   160,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   153,     0,     0,     0,
       0,     0,     0,     0,   349,     0,     0,   155,   156,     0,
       0,     0,     0,     0,     0,     0,   151,   152,     0,     0,
       0,     0,   137,   138,   139,   140,   141,   142,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   153,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     155,   156,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,     0,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   128,     0,     0,     0,
       0,     0,     0,     0,     0,   288,   129,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   135,   136,     0,     0,
       0,     0,   137,   138,   139,   140,   141,   142,     0,     0,
       0,     0,     0,     0,     1,     2,     3,     0,     4,   143,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     145,   146,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    15,     0,     0,    16,     0,    17,     0,    18,     0,
       0,    19,    20,     0,     0,    21,    22,     0,     0,     0,
       0,    23,     0,    24,    25,    26,    27,     0,     0,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     0,     0,
      37,    38,     0,    39,     0,    40,    41,    42,    43,    44,
      45,    46,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    48,    49,    50,    51,    52,
       0,     0,     0,     0,     0,     0,     0,     0,     1,     2,
       3,     0,     4,    53,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    15,     0,     0,    16,     0,
      17,     0,    18,     0,     0,    19,    20,     0,     0,    21,
      22,     0,     0,     0,     0,    23,     0,    24,    25,    26,
      27,     0,     0,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,     0,    37,    38,     0,    39,     0,    40,
      41,    42,    43,    44,    45,    46,     0,     0,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
      49,    50,    51,    52,     0,     0,     0,     0,   247,   248,
     249,     0,   250,     0,   251,   252,   253,    53,   254,   255,
     256,   257,   258,   259,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   260,     0,     0,   261,     0,
       0,     0,     0,     0,     0,   262,   263,     0,     0,   264,
     265,     0,     0,     0,     0,     0,   266,     0,   267,   268,
     269,     0,     0,   270,   271,   272,   273,   274,   275,   276,
     277,   278,     0,     0,   279,   280,     0,   281,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     282,   283,   284,   285,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   286
};

static const short yycheck[] =
{
       7,     8,    77,    77,    77,    29,    93,    94,    15,   107,
     107,   107,   107,   112,   173,   174,   175,   176,    64,    65,
     138,    44,     4,   138,     6,    77,   185,   143,   138,   145,
     146,    68,    69,     6,    72,    73,    74,    75,   197,   138,
     138,   138,   138,   138,   107,    52,   138,    99,   107,    73,
     107,   138,   113,   114,   115,   116,   117,   118,   138,   126,
     127,    62,   107,   138,   138,   138,   133,   134,   135,    93,
      94,    77,   126,   127,    47,   129,   130,   131,   132,   133,
     134,   135,    68,    69,   138,   139,   107,   138,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,   138,   138,    68,    69,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    68,    69,   153,   138,   155,   156,
     107,   138,   108,   138,   161,   138,    77,   108,   126,   127,
      77,   143,    77,   145,   146,   133,   134,   135,   138,   138,
     143,   139,   145,   146,   107,   138,   107,   107,   107,   138,
     138,   188,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   138,   107,   108,   140,   202,   140,   138,   113,   114,
     115,   116,   117,   118,   138,   140,   138,   107,   107,   107,
     138,   126,   127,   138,   129,   130,   131,   132,   133,   134,
     135,    77,   107,   108,   139,   138,   141,   142,   113,   114,
     115,   116,   117,   118,   126,   127,   138,   129,   130,   131,
     132,   133,   134,   135,   138,   130,   107,   139,   138,   138,
     138,   143,   138,   138,   138,   138,   141,   142,   126,   127,
     138,   129,   130,   131,   132,   133,   134,   135,   138,   138,
      54,   138,   138,   126,   127,   143,   129,   130,   131,   132,
     133,   134,   135,   138,   138,   138,   139,   294,   138,   126,
     127,   138,   129,   130,   131,   132,   133,   134,   135,   138,
     307,   138,   139,   138,   138,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   138,   322,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   298,   299,   300,   301,   302,
     303,   304,   305,   306,    38,   138,   138,   138,   138,   138,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,   138,   107,   361,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,   107,   126,   127,   139,   129,   130,   131,   132,   133,
     134,   135,   138,    97,   138,   139,   126,   127,   138,   129,
     130,   131,   132,   133,   134,   135,   110,   111,   138,   139,
      58,   138,   107,   138,   138,   119,   138,   138,   138,   138,
     138,   125,   138,   138,   128,   138,   138,   138,   138,    77,
     138,   138,   138,   138,    82,    83,   126,   127,   138,   129,
     130,   131,   132,   133,   134,   135,   138,   138,   138,   139,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   107,
     108,   138,   138,   138,   138,   113,   114,   115,   116,   117,
     118,   126,   127,   138,   129,   130,   131,   132,   133,   134,
     135,   138,   130,   138,   139,   138,   138,   138,   138,   138,
      -1,   138,   138,   141,   142,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,   138,   138,   138,   138,   138,   138,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,   126,   127,   139,   129,   130,   131,   132,
     133,   134,   135,   138,   138,   138,   139,   126,   127,    77,
     129,   130,   131,   132,   133,   134,   135,   138,   138,   107,
     139,    57,   126,   127,   138,   129,   130,   131,   132,   133,
     134,   135,   107,   138,   107,   139,   138,   138,   138,   107,
     108,    77,   138,   138,   138,   113,   114,   115,   116,   117,
     118,   138,   107,   107,   138,   138,   138,   138,   138,   138,
     138,   107,   130,    -1,   138,   133,    -1,    -1,    -1,    -1,
     138,   107,   108,   141,   142,    -1,    -1,   113,   114,   115,
     116,   117,   118,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   130,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   138,    -1,    -1,   141,   142,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    77,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    -1,
      -1,   107,   108,    77,    -1,    -1,    -1,   113,   114,   115,
     116,   117,   118,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   130,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   138,   107,   108,   141,   142,    -1,    -1,   113,
     114,   115,   116,   117,   118,    -1,    77,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   138,    -1,    -1,   141,   142,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   107,   108,    -1,    -1,
      -1,    -1,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     141,   142,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     0,    70,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   107,   108,    -1,    -1,
      -1,    -1,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,    -1,    -1,    -1,    29,    30,    31,    -1,    33,   130,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
     141,   142,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    56,    -1,    -1,    59,    -1,    61,    -1,    63,    -1,
      -1,    66,    67,    -1,    -1,    70,    71,    -1,    -1,    -1,
      -1,    76,    -1,    78,    79,    80,    81,    -1,    -1,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    -1,    -1,
      95,    96,    -1,    98,    -1,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   120,   121,   122,   123,   124,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,
      31,    -1,    33,   138,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    56,    -1,    -1,    59,    -1,
      61,    -1,    63,    -1,    -1,    66,    67,    -1,    -1,    70,
      71,    -1,    -1,    -1,    -1,    76,    -1,    78,    79,    80,
      81,    -1,    -1,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    -1,    -1,    95,    96,    -1,    98,    -1,   100,
     101,   102,   103,   104,   105,   106,    -1,    -1,   109,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   120,
     121,   122,   123,   124,    -1,    -1,    -1,    -1,    29,    30,
      31,    -1,    33,    -1,    35,    36,    37,   138,    39,    40,
      41,    42,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    56,    -1,    -1,    59,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    67,    -1,    -1,    70,
      71,    -1,    -1,    -1,    -1,    -1,    77,    -1,    79,    80,
      81,    -1,    -1,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    -1,    -1,    95,    96,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     121,   122,   123,   124,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   138
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    29,    30,    31,    33,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    56,    59,    61,    63,    66,
      67,    70,    71,    76,    78,    79,    80,    81,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    95,    96,    98,
     100,   101,   102,   103,   104,   105,   106,   109,   120,   121,
     122,   123,   124,   138,   145,   146,   147,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   138,   107,
     138,   138,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    60,    70,
     192,   193,   194,   195,   138,   107,   108,   113,   114,   115,
     116,   117,   118,   130,   138,   141,   142,   148,   195,   196,
      77,   107,   108,   130,   138,   141,   142,   148,   195,   197,
      77,   133,   138,   197,   107,   107,   107,    38,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      97,   110,   111,   119,   125,   128,   138,   138,    57,   138,
     197,   107,    62,    64,    65,   138,   138,    58,    82,    83,
     170,   171,   197,    72,    73,    74,    75,    77,   107,   107,
     138,   138,   138,   108,   108,    77,    77,   138,   107,   138,
      29,    73,    93,    94,   138,    93,    94,   138,    77,   138,
      77,    99,    77,   138,   138,   107,   138,    68,    69,    68,
      69,   107,   107,   148,   107,   138,   138,    29,    30,    31,
      33,    35,    36,    37,    39,    40,    41,    42,    43,    44,
      56,    59,    66,    67,    70,    71,    77,    79,    80,    81,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    95,
      96,    98,   121,   122,   123,   124,   138,   197,     0,   146,
     138,   140,    68,    69,   140,   196,   196,   196,   126,   127,
     129,   130,   131,   132,   133,   134,   135,   139,   138,   197,
     197,   197,   126,   127,   129,   130,   131,   132,   133,   134,
     135,   138,   139,   197,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   170,   170,   170,   170,   138,   138,
     138,    77,   138,   112,   138,   138,   138,   138,   170,   138,
     197,   138,   107,   138,   107,   138,   138,   170,   138,   140,
     138,   130,   197,   138,   138,   138,   138,   138,   138,   107,
     138,   138,   138,   138,   138,   138,   138,   107,   107,   107,
     107,   138,   107,   138,    77,   138,   138,   138,   138,   138,
     138,   138,   138,   139,    68,    69,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   107,   138,   138,   197,
     143,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     197,   143,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   138,   138,   138,   138,   138,   138,   107,   138,
     138,   107,   138,   138,   107,   197,   138,   138,   138,   138,
     138,   138,   107,   107,   138,   138,   138,   138,   138,    44,
     138,   138,   138,   138,   138,   107,   138
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
#line 222 "parser.y"
    {
      }
    break;

  case 48:
#line 228 "parser.y"
    {
		bx_dbg_diff_memory();
		free(yyvsp[-1].sval);
	}
    break;

  case 49:
#line 233 "parser.y"
    {
		bx_dbg_sync_memory(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
    break;

  case 50:
#line 238 "parser.y"
    {
		bx_dbg_sync_memory(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
    break;

  case 51:
#line 243 "parser.y"
    {
		bx_dbg_sync_cpu(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
    break;

  case 52:
#line 248 "parser.y"
    {
		bx_dbg_sync_cpu(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
    break;

  case 53:
#line 253 "parser.y"
    {
		free(yyvsp[-2].sval);
		bx_dbg_fast_forward(yyvsp[-1].uval);
	}
    break;

  case 54:
#line 258 "parser.y"
    {
	}
    break;

  case 55:
#line 261 "parser.y"
    {
		free(yyvsp[-4].sval);
		bx_dbg_info_address(yyvsp[-3].uval, yyvsp[-1].uval);
        }
    break;

  case 56:
#line 266 "parser.y"
    {
        }
    break;

  case 57:
#line 269 "parser.y"
    {
        }
    break;

  case 58:
#line 274 "parser.y"
    { yyval.uval = 1; }
    break;

  case 59:
#line 275 "parser.y"
    { yyval.uval = 0; }
    break;

  case 60:
#line 276 "parser.y"
    { yyval.uval = 2; }
    break;

  case 61:
#line 277 "parser.y"
    { yyval.uval = 3; }
    break;

  case 62:
#line 278 "parser.y"
    { yyval.uval = 4; }
    break;

  case 63:
#line 279 "parser.y"
    { yyval.uval = 5; }
    break;

  case 64:
#line 284 "parser.y"
    {
        bx_dbg_timebp_command(0, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
    break;

  case 65:
#line 289 "parser.y"
    {
        bx_dbg_timebp_command(1, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
    break;

  case 66:
#line 297 "parser.y"
    {
          bx_dbg_record_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 67:
#line 305 "parser.y"
    {
          bx_dbg_playback_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 68:
#line 313 "parser.y"
    {
          bx_dbg_modebp_command();
          free(yyvsp[-1].sval);
          }
    break;

  case 69:
#line 321 "parser.y"
    {
          bx_dbg_show_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 70:
#line 326 "parser.y"
    {
          bx_dbg_show_command(0);
          free(yyvsp[-1].sval);
          }
    break;

  case 71:
#line 334 "parser.y"
    {
        bx_dbg_ptime_command();
        free(yyvsp[-1].sval);
	}
    break;

  case 72:
#line 342 "parser.y"
    {
        bx_dbg_trace_on_command();
        free(yyvsp[-1].sval);
	}
    break;

  case 73:
#line 350 "parser.y"
    {
        bx_dbg_trace_off_command();
        free(yyvsp[-1].sval);
	}
    break;

  case 74:
#line 358 "parser.y"
    {
          bx_dbg_print_stack_command(16);
          free(yyvsp[-1].sval);
	  }
    break;

  case 75:
#line 363 "parser.y"
    {
          bx_dbg_print_stack_command(yyvsp[-1].uval);
          free(yyvsp[-2].sval);
	  }
    break;

  case 76:
#line 371 "parser.y"
    {
          watchpoint_continue = 0;
	  fprintf(stderr, "Will stop on watch points\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 77:
#line 377 "parser.y"
    {
          watchpoint_continue = 1;
          fprintf(stderr, "Will not stop on watch points (they will still be logged)\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 78:
#line 383 "parser.y"
    {
          bx_dbg_watch(-1, 0);
          free(yyvsp[-1].sval);
          }
    break;

  case 79:
#line 388 "parser.y"
    {
          bx_dbg_unwatch(-1, 0);
          free(yyvsp[-1].sval);
          }
    break;

  case 80:
#line 393 "parser.y"
    {
          bx_dbg_watch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 81:
#line 398 "parser.y"
    {
          bx_dbg_unwatch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 82:
#line 403 "parser.y"
    {
          bx_dbg_watch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 83:
#line 408 "parser.y"
    {
          bx_dbg_unwatch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 84:
#line 416 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-1].sval, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 85:
#line 421 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-2].sval, 0, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 86:
#line 426 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-2].sval, 1, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 87:
#line 434 "parser.y"
    {
        bx_dbg_where_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 88:
#line 442 "parser.y"
    {
        bx_dbg_print_string_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 89:
#line 450 "parser.y"
    {
        bx_dbg_continue_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 90:
#line 458 "parser.y"
    {
        bx_dbg_stepN_command(1);
        free(yyvsp[-1].sval);
        }
    break;

  case 91:
#line 463 "parser.y"
    {
        bx_dbg_stepN_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 92:
#line 471 "parser.y"
    {
        bx_dbg_step_over_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 93:
#line 479 "parser.y"
    {
        bx_dbg_set_auto_disassemble(1);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 94:
#line 484 "parser.y"
    {
        bx_dbg_set_auto_disassemble(0);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 95:
#line 489 "parser.y"
    {
        bx_dbg_set_symbol_command(yyvsp[-3].sval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
    break;

  case 96:
#line 494 "parser.y"
    { 
        bx_dbg_set_reg_value(yyvsp[-3].reg, yyvsp[-1].ulval);
        }
    break;

  case 97:
#line 501 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 98:
#line 506 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkRegular, yyvsp[-3].ulval, yyvsp[-1].ulval);
        free(yyvsp[-4].sval);
        }
    break;

  case 99:
#line 511 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 100:
#line 516 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-2].sval);
        }
    break;

  case 101:
#line 521 "parser.y"
    {
        bx_dbg_lbreakpoint_symbol_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval);free(yyvsp[-1].sval);
        }
    break;

  case 102:
#line 526 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 103:
#line 531 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-2].sval);
        }
    break;

  case 104:
#line 536 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-3].sval);
        }
    break;

  case 105:
#line 544 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 106:
#line 552 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free(yyvsp[-1].sval);
        }
    break;

  case 107:
#line 557 "parser.y"
    {
        bx_dbg_info_symbols_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval);free(yyvsp[-1].sval);
        }
    break;

  case 108:
#line 565 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 109:
#line 570 "parser.y"
    {
        bx_dbg_dump_cpu_command();
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
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 113:
#line 590 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS | BX_INFO_FPU_REGS | BX_INFO_SSE_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 114:
#line 595 "parser.y"
    {
        bx_dbg_info_dirty_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
    break;

  case 115:
#line 600 "parser.y"
    {
        bx_dbg_info_idt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 116:
#line 605 "parser.y"
    {
       bx_dbg_info_ivt_command(yyvsp[-1].uval_range);
       free(yyvsp[-3].sval); free(yyvsp[-2].sval);
       }
    break;

  case 117:
#line 610 "parser.y"
    {
        bx_dbg_info_gdt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 118:
#line 615 "parser.y"
    {
        bx_dbg_info_ldt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 119:
#line 620 "parser.y"
    {
        bx_dbg_info_tss_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 120:
#line 625 "parser.y"
    {
        bx_dbg_info_flags();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 121:
#line 630 "parser.y"
    {
        bx_dbg_info_linux_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 122:
#line 635 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 123:
#line 640 "parser.y"
    {
        bx_dbg_info_symbols_command(yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 124:
#line 645 "parser.y"
    {
        bx_dbg_info_control_regs_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 125:
#line 650 "parser.y"
    {
        bx_dbg_info_ne2k(-1, -1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 126:
#line 655 "parser.y"
    {
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-1].uval, -1);
        }
    break;

  case 127:
#line 660 "parser.y"
    {
        free(yyvsp[-6].sval); free(yyvsp[-5].sval); free(yyvsp[-4].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-3].uval, yyvsp[-1].uval);
        }
    break;

  case 128:
#line 665 "parser.y"
    {
        bx_dbg_info_pic();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 129:
#line 670 "parser.y"
    {
        bx_dbg_info_vga();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 130:
#line 681 "parser.y"
    { yyval.uval_range = make_num_range (EMPTY_ARG, EMPTY_ARG); }
    break;

  case 132:
#line 686 "parser.y"
    {
    yyval.uval_range = make_num_range (yyvsp[0].ulval, yyvsp[0].ulval);
  }
    break;

  case 133:
#line 691 "parser.y"
    {
    yyval.uval_range = make_num_range (yyvsp[-1].ulval, yyvsp[0].ulval);
  }
    break;

  case 134:
#line 697 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free(yyvsp[-1].sval);
        }
    break;

  case 135:
#line 705 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 136:
#line 713 "parser.y"
    {
        bx_dbg_del_breakpoint_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 137:
#line 721 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command(yyvsp[-1].uval, 1);
        free(yyvsp[-2].sval);
       }
    break;

  case 138:
#line 728 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command(yyvsp[-1].uval, 0);
        free(yyvsp[-2].sval);
       }
    break;

  case 139:
#line 736 "parser.y"
    {
	  bx_dbg_quit_command();
	  free(yyvsp[-1].sval);
        }
    break;

  case 140:
#line 745 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].ulval,1, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].ulval,1, 1);
#endif
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 141:
#line 753 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 1);
#endif
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 142:
#line 761 "parser.y"
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

  case 143:
#line 774 "parser.y"
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

  case 144:
#line 790 "parser.y"
    {
        bx_dbg_setpmem_command(yyvsp[-3].uval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
    break;

  case 145:
#line 798 "parser.y"
    {
        bx_dbg_query_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 146:
#line 806 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 147:
#line 811 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-2].sval, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 148:
#line 816 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 149:
#line 824 "parser.y"
    {
        bx_dbg_set_cpu_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 150:
#line 832 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, yyvsp[-1].uval_range);
        free(yyvsp[-2].sval);
        }
    break;

  case 151:
#line 837 "parser.y"
    {
        bx_dbg_disassemble_command(yyvsp[-2].sval, yyvsp[-1].uval_range);
	free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 152:
#line 842 "parser.y"
    {
        bx_dbg_disassemble_switch_mode();
	free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 153:
#line 847 "parser.y"
    {
        bx_dbg_set_disassemble_size(yyvsp[-1].uval);
	free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
    break;

  case 157:
#line 858 "parser.y"
    {
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 158:
#line 866 "parser.y"
    {
        bx_dbg_loader_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 159:
#line 874 "parser.y"
    {
        bx_dbg_doit_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 160:
#line 882 "parser.y"
    {
        bx_dbg_crc_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
    break;

  case 161:
#line 890 "parser.y"
    {
	bx_dbg_trace_reg_on_command();
	free(yyvsp[-1].sval);
	}
    break;

  case 162:
#line 898 "parser.y"
    {
	bx_dbg_trace_reg_off_command();
	free(yyvsp[-1].sval);
	}
    break;

  case 163:
#line 906 "parser.y"
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 164:
#line 911 "parser.y"
    {
         dbg_printf("c|cont|continue - continue executing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 165:
#line 916 "parser.y"
    {
         dbg_printf("s|step|stepi [count] - execute #count instructions (default is one instruction)\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 166:
#line 921 "parser.y"
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 167:
#line 926 "parser.y"
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 168:
#line 931 "parser.y"
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 169:
#line 936 "parser.y"
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 170:
#line 941 "parser.y"
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 171:
#line 946 "parser.y"
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 172:
#line 951 "parser.y"
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 173:
#line 956 "parser.y"
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 174:
#line 961 "parser.y"
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 175:
#line 966 "parser.y"
    {
         dbg_printf("trace-on - disassemble every executed instruction\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 176:
#line 971 "parser.y"
    {
         dbg_printf("trace-off - disable instruction tracing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 177:
#line 976 "parser.y"
    {
         dbg_printf("trace-reg-on - print all registers every executed instruction\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 178:
#line 981 "parser.y"
    {
         dbg_printf("trace-reg-off - disable registers state tracing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 179:
#line 986 "parser.y"
    {
         dbg_printf("dump_cpu - dump complete cpu state\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 180:
#line 991 "parser.y"
    {
         dbg_printf("set_cpu - set complete cpu state\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 181:
#line 996 "parser.y"
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 182:
#line 1001 "parser.y"
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 183:
#line 1006 "parser.y"
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 184:
#line 1011 "parser.y"
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 185:
#line 1016 "parser.y"
    {
         dbg_printf("record <filename> - record console input to file filename\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 186:
#line 1021 "parser.y"
    {
         dbg_printf("playback <filename> - playback console input from file filename\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 187:
#line 1026 "parser.y"
    {
         dbg_printf("load-symbols [global] <filename> [offset] - load symbols from file\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 188:
#line 1031 "parser.y"
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 189:
#line 1036 "parser.y"
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info cpu')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 190:
#line 1041 "parser.y"
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 191:
#line 1046 "parser.y"
    {
         dbg_printf("u|disasm|disassemble [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm|disassemble switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm|disassemble size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 192:
#line 1055 "parser.y"
    {
         dbg_printf("watch - print current watch point status\n");
         dbg_printf("watch stop - stop simulation when a watchpoint is encountred\n");
         dbg_printf("watch continue - do not stop the simulation when watch point is encountred\n");
         dbg_printf("watch read addr - insert a read watch point at physical address addr\n");
         dbg_printf("watch write addr - insert a write watch point at physical address addr\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 193:
#line 1064 "parser.y"
    {
         dbg_printf("unwatch - remove all watch points\n");
         dbg_printf("unwatch read addr - remove a read watch point at physical address addr\n");
         dbg_printf("unwatch write addr - remove a write watch point at physical address addr\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 194:
#line 1071 "parser.y"
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
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 195:
#line 1084 "parser.y"
    {
         dbg_printf("instrument start - calls bx_instr_start() callback\n");
         dbg_printf("instrument stop  - calls bx_instr_stop() callback\n");
         dbg_printf("instrument reset - calls bx_instr_reset() callback\n");
         dbg_printf("instrument print - calls bx_instr_print() callback\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 196:
#line 1092 "parser.y"
    {
         dbg_printf("set <regname> = <expr> - set register value to expression\n");
         dbg_printf("set $reg = val - set CPU register to value val\n");
         dbg_printf("set $auto_disassemble = 1 - cause debugger to disassemble current instruction\n");
         dbg_printf("       every time execution stops\n");
         dbg_printf("set u|disasm|disassemble on  - same as 'set $auto_disassemble = 1'\n");
         dbg_printf("set u|disasm|disassemble off - same as 'set $auto_disassemble = 0'\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 197:
#line 1102 "parser.y"
    {
         dbg_printf("info break - show information about current breakpoint status\n");
         dbg_printf("info dirty - show physical pages dirtied (written to) since last display\n");
         dbg_printf("info r|reg|regs|registers - list of CPU integer registers and their contents\n");
         dbg_printf("info cpu - list of CPU registers and their contents\n");
         dbg_printf("info fpu - list of FPU registers and their contents\n");
         dbg_printf("info sse - list of SSE registers and their contents\n");
         dbg_printf("info idt - show interrupt descriptor table\n");
         dbg_printf("info ivt - show interrupt vector table\n");
         dbg_printf("info gdt - show global descriptor table\n");
         dbg_printf("info tss - show current task state segment\n");
         dbg_printf("info cr - show CR0-4 registers\n");
         dbg_printf("info eflags - show decoded EFLAGS register\n");
         dbg_printf("info symbols [string] - list symbols whose prefix is string\n");
         dbg_printf("info pic - show PICs registers\n");
         dbg_printf("info ne2000 - show NE2000 registers\n");
         dbg_printf("info vga - show vga registers\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 198:
#line 1122 "parser.y"
    {
         dbg_printf("show \"string\" - toggles show symbolic info (calls to begin with)\n");
         dbg_printf("show - shows current show mode\n");
         dbg_printf("show \"mode\" - show, when processor switch mode\n");
         dbg_printf("show \"int\" - show, when interrupt is happens\n");
         dbg_printf("show \"call\" - show, when call is happens\n");
         dbg_printf("show \"ret\" - show, when iret is happens\n");
         dbg_printf("show \"off\" - toggles off symbolic info\n");
         dbg_printf("show \"dbg-all\" - turn on all show flags\n");
         dbg_printf("show \"none\" - turn off all show flags\n");
         dbg_printf("show \"tab\" - show page tables\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 199:
#line 1136 "parser.y"
    {
         dbg_printf("calc|? <expr> - calculate a expression and display the result.\n");
         dbg_printf("    'expr' can reference any general-purpose and segment\n");
         dbg_printf("    registers, use any arithmetic and logic operations, and\n");
         dbg_printf("    also the special ':' operator which computes the linear\n");
         dbg_printf("    address for a segment:offset (in real and v86 mode) or\n");
         dbg_printf("    of a selector:offset (in protected mode) pair.\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 200:
#line 1146 "parser.y"
    {
         bx_dbg_print_help();
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 201:
#line 1151 "parser.y"
    {
         dbg_printf("unknown command '%s', help is not available\n", yyvsp[-1].sval);
         bx_dbg_print_help();
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 202:
#line 1157 "parser.y"
    {
         bx_dbg_print_help();
         free(yyvsp[-1].sval);
         }
    break;

  case 203:
#line 1165 "parser.y"
    {
     bx_dbg_calc_command(yyvsp[-1].ulval);
     free(yyvsp[-2].sval);
     }
    break;

  case 219:
#line 1188 "parser.y"
    { yyval.reg=yyvsp[0].reg; }
    break;

  case 223:
#line 1196 "parser.y"
    { yyval.reg=yyvsp[0].reg; }
    break;

  case 229:
#line 1206 "parser.y"
    { yyval.reg=yyvsp[0].reg; }
    break;

  case 232:
#line 1213 "parser.y"
    { yyval.reg=yyvsp[0].reg; }
    break;

  case 233:
#line 1218 "parser.y"
    { yyval.ulval = yyvsp[0].uval; }
    break;

  case 234:
#line 1219 "parser.y"
    { yyval.ulval = yyvsp[0].ulval; }
    break;

  case 235:
#line 1220 "parser.y"
    { yyval.ulval = bx_dbg_get_reg_value(yyvsp[0].reg); }
    break;

  case 236:
#line 1221 "parser.y"
    { yyval.ulval = bx_dbg_get_selector_value(yyvsp[0].uval); }
    break;

  case 237:
#line 1222 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval + yyvsp[0].ulval; }
    break;

  case 238:
#line 1223 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval - yyvsp[0].ulval; }
    break;

  case 239:
#line 1224 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval * yyvsp[0].ulval; }
    break;

  case 240:
#line 1225 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval / yyvsp[0].ulval; }
    break;

  case 241:
#line 1226 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval >> yyvsp[0].ulval; }
    break;

  case 242:
#line 1227 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval << yyvsp[0].ulval; }
    break;

  case 243:
#line 1228 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval | yyvsp[0].ulval; }
    break;

  case 244:
#line 1229 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval ^ yyvsp[0].ulval; }
    break;

  case 245:
#line 1230 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval & yyvsp[0].ulval; }
    break;

  case 246:
#line 1231 "parser.y"
    { yyval.ulval = !yyvsp[0].ulval; }
    break;

  case 247:
#line 1232 "parser.y"
    { yyval.ulval = -yyvsp[0].ulval; }
    break;

  case 248:
#line 1233 "parser.y"
    { yyval.ulval = yyvsp[-1].ulval; }
    break;

  case 249:
#line 1238 "parser.y"
    { yyval.ulval = yyvsp[0].uval; }
    break;

  case 250:
#line 1239 "parser.y"
    { yyval.ulval = yyvsp[0].ulval; }
    break;

  case 251:
#line 1240 "parser.y"
    { yyval.ulval = bx_dbg_get_symbol_value(yyvsp[0].sval); free(yyvsp[0].sval);}
    break;

  case 252:
#line 1241 "parser.y"
    { yyval.ulval = bx_dbg_get_reg_value(yyvsp[0].reg);}
    break;

  case 253:
#line 1242 "parser.y"
    { yyval.ulval = bx_dbg_get_selector_value(yyvsp[0].uval);}
    break;

  case 254:
#line 1243 "parser.y"
    { yyval.ulval = bx_dbg_get_laddr (yyvsp[-2].ulval, yyvsp[0].ulval); }
    break;

  case 255:
#line 1244 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval + yyvsp[0].ulval; }
    break;

  case 256:
#line 1245 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval - yyvsp[0].ulval; }
    break;

  case 257:
#line 1246 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval * yyvsp[0].ulval; }
    break;

  case 258:
#line 1247 "parser.y"
    { yyval.ulval = (yyvsp[0].ulval != 0) ? yyvsp[-2].ulval / yyvsp[0].ulval : 0; }
    break;

  case 259:
#line 1248 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval >> yyvsp[0].ulval; }
    break;

  case 260:
#line 1249 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval << yyvsp[0].ulval; }
    break;

  case 261:
#line 1250 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval | yyvsp[0].ulval; }
    break;

  case 262:
#line 1251 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval ^ yyvsp[0].ulval; }
    break;

  case 263:
#line 1252 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval & yyvsp[0].ulval; }
    break;

  case 264:
#line 1253 "parser.y"
    { yyval.ulval = !yyvsp[0].ulval; }
    break;

  case 265:
#line 1254 "parser.y"
    { yyval.ulval = -yyvsp[0].ulval; }
    break;

  case 266:
#line 1255 "parser.y"
    { yyval.ulval = yyvsp[-1].ulval; }
    break;


    }

/* Line 999 of yacc.c.  */
#line 3394 "y.tab.c"

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


#line 1258 "parser.y"


#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
