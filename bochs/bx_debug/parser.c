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
     BX_TOKEN_CS = 284,
     BX_TOKEN_ES = 285,
     BX_TOKEN_SS = 286,
     BX_TOKEN_DS = 287,
     BX_TOKEN_FS = 288,
     BX_TOKEN_GS = 289,
     BX_TOKEN_FLAGS = 290,
     BX_TOKEN_ON = 291,
     BX_TOKEN_OFF = 292,
     BX_TOKEN_CONTINUE = 293,
     BX_TOKEN_STEPN = 294,
     BX_TOKEN_STEP_OVER = 295,
     BX_TOKEN_NEXT_STEP = 296,
     BX_TOKEN_SET = 297,
     BX_TOKEN_DEBUGGER = 298,
     BX_TOKEN_LIST_BREAK = 299,
     BX_TOKEN_VBREAKPOINT = 300,
     BX_TOKEN_LBREAKPOINT = 301,
     BX_TOKEN_PBREAKPOINT = 302,
     BX_TOKEN_DEL_BREAKPOINT = 303,
     BX_TOKEN_ENABLE_BREAKPOINT = 304,
     BX_TOKEN_DISABLE_BREAKPOINT = 305,
     BX_TOKEN_INFO = 306,
     BX_TOKEN_QUIT = 307,
     BX_TOKEN_REGISTERS = 308,
     BX_TOKEN_CPU = 309,
     BX_TOKEN_FPU = 310,
     BX_TOKEN_SSE = 311,
     BX_TOKEN_ALL = 312,
     BX_TOKEN_IDT = 313,
     BX_TOKEN_GDT = 314,
     BX_TOKEN_LDT = 315,
     BX_TOKEN_TSS = 316,
     BX_TOKEN_DIRTY = 317,
     BX_TOKEN_LINUX = 318,
     BX_TOKEN_CONTROL_REGS = 319,
     BX_TOKEN_EXAMINE = 320,
     BX_TOKEN_XFORMAT = 321,
     BX_TOKEN_DISFORMAT = 322,
     BX_TOKEN_SETPMEM = 323,
     BX_TOKEN_SYMBOLNAME = 324,
     BX_TOKEN_QUERY = 325,
     BX_TOKEN_PENDING = 326,
     BX_TOKEN_TAKE = 327,
     BX_TOKEN_DMA = 328,
     BX_TOKEN_IRQ = 329,
     BX_TOKEN_DUMP_CPU = 330,
     BX_TOKEN_SET_CPU = 331,
     BX_TOKEN_DISASSEMBLE = 332,
     BX_TOKEN_INSTRUMENT = 333,
     BX_TOKEN_START = 334,
     BX_TOKEN_STOP = 335,
     BX_TOKEN_RESET = 336,
     BX_TOKEN_PRINT = 337,
     BX_TOKEN_LOADER = 338,
     BX_TOKEN_STRING = 339,
     BX_TOKEN_DOIT = 340,
     BX_TOKEN_CRC = 341,
     BX_TOKEN_TRACEON = 342,
     BX_TOKEN_TRACEOFF = 343,
     BX_TOKEN_SWITCH_MODE = 344,
     BX_TOKEN_SIZE = 345,
     BX_TOKEN_PTIME = 346,
     BX_TOKEN_TIMEBP_ABSOLUTE = 347,
     BX_TOKEN_TIMEBP = 348,
     BX_TOKEN_RECORD = 349,
     BX_TOKEN_PLAYBACK = 350,
     BX_TOKEN_MODEBP = 351,
     BX_TOKEN_PRINT_STACK = 352,
     BX_TOKEN_WATCH = 353,
     BX_TOKEN_UNWATCH = 354,
     BX_TOKEN_READ = 355,
     BX_TOKEN_WRITE = 356,
     BX_TOKEN_SHOW = 357,
     BX_TOKEN_LOAD_SYMBOLS = 358,
     BX_TOKEN_SYMBOLS = 359,
     BX_TOKEN_LIST_SYMBOLS = 360,
     BX_TOKEN_GLOBAL = 361,
     BX_TOKEN_WHERE = 362,
     BX_TOKEN_PRINT_STRING = 363,
     BX_TOKEN_DIFF_MEMORY = 364,
     BX_TOKEN_SYNC_MEMORY = 365,
     BX_TOKEN_SYNC_CPU = 366,
     BX_TOKEN_FAST_FORWARD = 367,
     BX_TOKEN_PHY_2_LOG = 368,
     BX_TOKEN_NUMERIC = 369,
     BX_TOKEN_LONG_NUMERIC = 370,
     BX_TOKEN_INFO_ADDRESS = 371,
     BX_TOKEN_NE2000 = 372,
     BX_TOKEN_PIC = 373,
     BX_TOKEN_PAGE = 374,
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
#define BX_TOKEN_CS 284
#define BX_TOKEN_ES 285
#define BX_TOKEN_SS 286
#define BX_TOKEN_DS 287
#define BX_TOKEN_FS 288
#define BX_TOKEN_GS 289
#define BX_TOKEN_FLAGS 290
#define BX_TOKEN_ON 291
#define BX_TOKEN_OFF 292
#define BX_TOKEN_CONTINUE 293
#define BX_TOKEN_STEPN 294
#define BX_TOKEN_STEP_OVER 295
#define BX_TOKEN_NEXT_STEP 296
#define BX_TOKEN_SET 297
#define BX_TOKEN_DEBUGGER 298
#define BX_TOKEN_LIST_BREAK 299
#define BX_TOKEN_VBREAKPOINT 300
#define BX_TOKEN_LBREAKPOINT 301
#define BX_TOKEN_PBREAKPOINT 302
#define BX_TOKEN_DEL_BREAKPOINT 303
#define BX_TOKEN_ENABLE_BREAKPOINT 304
#define BX_TOKEN_DISABLE_BREAKPOINT 305
#define BX_TOKEN_INFO 306
#define BX_TOKEN_QUIT 307
#define BX_TOKEN_REGISTERS 308
#define BX_TOKEN_CPU 309
#define BX_TOKEN_FPU 310
#define BX_TOKEN_SSE 311
#define BX_TOKEN_ALL 312
#define BX_TOKEN_IDT 313
#define BX_TOKEN_GDT 314
#define BX_TOKEN_LDT 315
#define BX_TOKEN_TSS 316
#define BX_TOKEN_DIRTY 317
#define BX_TOKEN_LINUX 318
#define BX_TOKEN_CONTROL_REGS 319
#define BX_TOKEN_EXAMINE 320
#define BX_TOKEN_XFORMAT 321
#define BX_TOKEN_DISFORMAT 322
#define BX_TOKEN_SETPMEM 323
#define BX_TOKEN_SYMBOLNAME 324
#define BX_TOKEN_QUERY 325
#define BX_TOKEN_PENDING 326
#define BX_TOKEN_TAKE 327
#define BX_TOKEN_DMA 328
#define BX_TOKEN_IRQ 329
#define BX_TOKEN_DUMP_CPU 330
#define BX_TOKEN_SET_CPU 331
#define BX_TOKEN_DISASSEMBLE 332
#define BX_TOKEN_INSTRUMENT 333
#define BX_TOKEN_START 334
#define BX_TOKEN_STOP 335
#define BX_TOKEN_RESET 336
#define BX_TOKEN_PRINT 337
#define BX_TOKEN_LOADER 338
#define BX_TOKEN_STRING 339
#define BX_TOKEN_DOIT 340
#define BX_TOKEN_CRC 341
#define BX_TOKEN_TRACEON 342
#define BX_TOKEN_TRACEOFF 343
#define BX_TOKEN_SWITCH_MODE 344
#define BX_TOKEN_SIZE 345
#define BX_TOKEN_PTIME 346
#define BX_TOKEN_TIMEBP_ABSOLUTE 347
#define BX_TOKEN_TIMEBP 348
#define BX_TOKEN_RECORD 349
#define BX_TOKEN_PLAYBACK 350
#define BX_TOKEN_MODEBP 351
#define BX_TOKEN_PRINT_STACK 352
#define BX_TOKEN_WATCH 353
#define BX_TOKEN_UNWATCH 354
#define BX_TOKEN_READ 355
#define BX_TOKEN_WRITE 356
#define BX_TOKEN_SHOW 357
#define BX_TOKEN_LOAD_SYMBOLS 358
#define BX_TOKEN_SYMBOLS 359
#define BX_TOKEN_LIST_SYMBOLS 360
#define BX_TOKEN_GLOBAL 361
#define BX_TOKEN_WHERE 362
#define BX_TOKEN_PRINT_STRING 363
#define BX_TOKEN_DIFF_MEMORY 364
#define BX_TOKEN_SYNC_MEMORY 365
#define BX_TOKEN_SYNC_CPU 366
#define BX_TOKEN_FAST_FORWARD 367
#define BX_TOKEN_PHY_2_LOG 368
#define BX_TOKEN_NUMERIC 369
#define BX_TOKEN_LONG_NUMERIC 370
#define BX_TOKEN_INFO_ADDRESS 371
#define BX_TOKEN_NE2000 372
#define BX_TOKEN_PIC 373
#define BX_TOKEN_PAGE 374
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
  bx_bool  bval;
  bx_num_range uval_range;
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
#define YYFINAL  291
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1176

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  144
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  54
/* YYNRULES -- Number of rules. */
#define YYNRULES  269
/* YYNRULES -- Number of states. */
#define YYNSTATES  508

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
     251,   254,   258,   261,   266,   271,   277,   283,   289,   295,
     301,   304,   310,   313,   317,   321,   324,   328,   333,   336,
     339,   343,   347,   351,   355,   359,   363,   367,   371,   376,
     381,   386,   391,   396,   400,   404,   408,   413,   417,   421,
     427,   435,   439,   443,   444,   446,   448,   451,   454,   457,
     461,   465,   469,   472,   477,   481,   485,   488,   494,   498,
     502,   507,   511,   514,   518,   523,   527,   533,   537,   541,
     545,   549,   553,   557,   562,   565,   568,   572,   576,   580,
     584,   588,   592,   596,   600,   604,   608,   612,   616,   620,
     624,   628,   632,   636,   640,   644,   648,   652,   656,   660,
     664,   668,   672,   676,   680,   684,   688,   692,   696,   700,
     704,   708,   712,   716,   720,   723,   727,   729,   731,   733,
     735,   737,   739,   741,   743,   745,   747,   749,   751,   753,
     755,   757,   759,   761,   763,   765,   767,   769,   771,   773,
     775,   777,   779,   781,   783,   785,   787,   789,   793,   797,
     801,   805,   809,   813,   817,   821,   825,   828,   831,   835,
     837,   839,   841,   843,   845,   847,   849,   851,   855,   859,
     863,   867,   871,   875,   879,   883,   887,   891,   894,   897
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
     191,    -1,    -1,   138,    -1,   109,   138,    -1,   110,    36,
     138,    -1,   110,    37,   138,    -1,   111,    36,   138,    -1,
     111,    37,   138,    -1,   112,   114,   138,    -1,   113,   114,
     138,    -1,   116,   148,   139,   114,   138,    -1,   120,   114,
      36,   138,    -1,   120,   114,    37,   138,    -1,    29,    -1,
      30,    -1,    31,    -1,    32,    -1,    33,    -1,    34,    -1,
      93,   115,   138,    -1,    92,   115,   138,    -1,    94,    84,
     138,    -1,    95,    84,   138,    -1,    96,   138,    -1,   102,
      84,   138,    -1,   102,   138,    -1,    91,   138,    -1,    87,
     138,    -1,    88,   138,    -1,    97,   138,    -1,    97,   114,
     138,    -1,    98,    80,   138,    -1,    98,    38,   138,    -1,
      98,   138,    -1,    99,   138,    -1,    98,   100,   114,   138,
      -1,    99,   100,   114,   138,    -1,    98,   101,   114,   138,
      -1,    99,   101,   114,   138,    -1,   103,    84,   138,    -1,
     103,    84,   114,   138,    -1,   103,   106,    84,   114,   138,
      -1,   107,   138,    -1,   108,   114,   138,    -1,    38,   138,
      -1,    39,   138,    -1,    39,   114,   138,    -1,    40,   138,
      -1,    42,    77,    36,   138,    -1,    42,    77,    37,   138,
      -1,    42,    69,   140,   114,   138,    -1,    42,   193,   140,
     197,   138,    -1,    42,   192,   140,   197,   138,    -1,    42,
     194,   140,   197,   138,    -1,    42,   195,   140,   197,   138,
      -1,    45,   138,    -1,    45,   196,   139,   197,   138,    -1,
      46,   138,    -1,    46,   197,   138,    -1,    46,    84,   138,
      -1,    47,   138,    -1,    47,   197,   138,    -1,    47,   133,
     197,   138,    -1,    44,   138,    -1,   105,   138,    -1,   105,
      84,   138,    -1,    51,    47,   138,    -1,    51,    54,   138,
      -1,    51,    53,   138,    -1,    51,    55,   138,    -1,    51,
      56,   138,    -1,    51,    57,   138,    -1,    51,    62,   138,
      -1,    51,    58,   170,   138,    -1,    51,   128,   170,   138,
      -1,    51,    59,   170,   138,    -1,    51,    60,   170,   138,
      -1,    51,    61,   170,   138,    -1,    51,    35,   138,    -1,
      51,    63,   138,    -1,    51,   104,   138,    -1,    51,   104,
      84,   138,    -1,    51,    64,   138,    -1,    51,   117,   138,
      -1,    51,   117,   119,   114,   138,    -1,    51,   117,   119,
     114,    53,   114,   138,    -1,    51,   118,   138,    -1,    51,
     125,   138,    -1,    -1,   171,    -1,   197,    -1,   197,   197,
      -1,    53,   138,    -1,    75,   138,    -1,    48,   114,   138,
      -1,    49,   114,   138,    -1,    50,   114,   138,    -1,    52,
     138,    -1,    65,    66,   197,   138,    -1,    65,    66,   138,
      -1,    65,   197,   138,    -1,    65,   138,    -1,    68,   114,
     114,   114,   138,    -1,    70,    71,   138,    -1,    72,    73,
     138,    -1,    72,    73,   114,   138,    -1,    72,    74,   138,
      -1,    76,   138,    -1,    77,   170,   138,    -1,    77,    67,
     170,   138,    -1,    77,    89,   138,    -1,    77,    90,   140,
     114,   138,    -1,    78,    79,   138,    -1,    78,    80,   138,
      -1,    78,    81,   138,    -1,    78,    82,   138,    -1,    83,
      84,   138,    -1,    85,   114,   138,    -1,    86,   114,   114,
     138,    -1,   121,   138,    -1,   122,   138,    -1,   123,    52,
     138,    -1,   123,    38,   138,    -1,   123,    39,   138,    -1,
     123,    40,   138,    -1,   123,    45,   138,    -1,   123,    46,
     138,    -1,   123,    48,   138,    -1,   123,    49,   138,    -1,
     123,    50,   138,    -1,   123,    44,   138,    -1,   123,    96,
     138,    -1,   123,    86,   138,    -1,   123,    87,   138,    -1,
     123,    88,   138,    -1,   123,   121,   138,    -1,   123,   122,
     138,    -1,   123,    75,   138,    -1,   123,    76,   138,    -1,
     123,    91,   138,    -1,   123,    93,   138,    -1,   123,    92,
     138,    -1,   123,    97,   138,    -1,   123,    94,   138,    -1,
     123,    95,   138,    -1,   123,   103,   138,    -1,   123,   105,
     138,    -1,   123,    53,   138,    -1,   123,    68,   138,    -1,
     123,    77,   138,    -1,   123,    98,   138,    -1,   123,    99,
     138,    -1,   123,    65,   138,    -1,   123,    78,   138,    -1,
     123,    42,   138,    -1,   123,    51,   138,    -1,   123,   102,
     138,    -1,   123,   124,   138,    -1,   123,   123,   138,    -1,
     123,   138,    -1,   124,   197,   138,    -1,     7,    -1,     8,
      -1,     9,    -1,    10,    -1,     3,    -1,     4,    -1,     5,
      -1,     6,    -1,    11,    -1,    12,    -1,    13,    -1,    14,
      -1,    19,    -1,    20,    -1,    21,    -1,    22,    -1,    15,
      -1,    16,    -1,    17,    -1,    18,    -1,    24,    -1,    25,
      -1,    26,    -1,    27,    -1,   114,    -1,   115,    -1,   193,
      -1,   192,    -1,   194,    -1,   195,    -1,   148,    -1,   196,
     129,   196,    -1,   196,   130,   196,    -1,   196,   133,   196,
      -1,   196,   134,   196,    -1,   196,   126,   196,    -1,   196,
     127,   196,    -1,   196,   131,   196,    -1,   196,   132,   196,
      -1,   196,   135,   196,    -1,   141,   196,    -1,   130,   196,
      -1,   142,   196,   143,    -1,   114,    -1,   115,    -1,    84,
      -1,   193,    -1,   192,    -1,   194,    -1,   195,    -1,   148,
      -1,   197,   139,   197,    -1,   197,   129,   197,    -1,   197,
     130,   197,    -1,   197,   133,   197,    -1,   197,   134,   197,
      -1,   197,   126,   197,    -1,   197,   127,   197,    -1,   197,
     131,   197,    -1,   197,   132,   197,    -1,   197,   135,   197,
      -1,   141,   197,    -1,   130,   197,    -1,   142,   197,   143,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   175,   175,   176,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   229,   234,
     235,   240,   241,   246,   251,   255,   260,   264,   271,   272,
     273,   274,   275,   276,   281,   286,   294,   302,   310,   318,
     323,   331,   339,   347,   355,   360,   368,   374,   380,   385,
     390,   395,   400,   405,   413,   418,   423,   431,   439,   447,
     455,   460,   468,   476,   477,   482,   487,   491,   495,   499,
     506,   511,   516,   521,   526,   531,   536,   541,   549,   557,
     562,   570,   575,   580,   585,   590,   595,   600,   605,   610,
     615,   620,   625,   630,   635,   640,   645,   650,   655,   660,
     665,   670,   675,   687,   688,   691,   695,   701,   709,   717,
     725,   732,   740,   748,   756,   764,   777,   793,   801,   809,
     814,   819,   827,   835,   840,   845,   850,   858,   859,   860,
     861,   869,   877,   885,   893,   901,   909,   914,   919,   924,
     929,   934,   939,   944,   949,   954,   959,   964,   969,   974,
     979,   984,   989,   994,   999,  1004,  1009,  1014,  1019,  1024,
    1029,  1034,  1039,  1044,  1049,  1058,  1067,  1074,  1087,  1095,
    1105,  1125,  1139,  1149,  1154,  1162,  1170,  1171,  1172,  1173,
    1178,  1179,  1180,  1181,  1186,  1187,  1188,  1189,  1190,  1191,
    1192,  1193,  1198,  1199,  1200,  1201,  1202,  1203,  1204,  1205,
    1219,  1220,  1221,  1222,  1223,  1224,  1225,  1226,  1227,  1228,
    1229,  1230,  1231,  1232,  1233,  1234,  1235,  1236,  1237,  1242,
    1243,  1244,  1245,  1246,  1247,  1248,  1249,  1250,  1251,  1252,
    1253,  1254,  1255,  1256,  1257,  1258,  1259,  1260,  1261,  1262
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
  "BX_TOKEN_CS", "BX_TOKEN_ES", "BX_TOKEN_SS", "BX_TOKEN_DS", 
  "BX_TOKEN_FS", "BX_TOKEN_GS", "BX_TOKEN_FLAGS", "BX_TOKEN_ON", 
  "BX_TOKEN_OFF", "BX_TOKEN_CONTINUE", "BX_TOKEN_STEPN", 
  "BX_TOKEN_STEP_OVER", "BX_TOKEN_NEXT_STEP", "BX_TOKEN_SET", 
  "BX_TOKEN_DEBUGGER", "BX_TOKEN_LIST_BREAK", "BX_TOKEN_VBREAKPOINT", 
  "BX_TOKEN_LBREAKPOINT", "BX_TOKEN_PBREAKPOINT", 
  "BX_TOKEN_DEL_BREAKPOINT", "BX_TOKEN_ENABLE_BREAKPOINT", 
  "BX_TOKEN_DISABLE_BREAKPOINT", "BX_TOKEN_INFO", "BX_TOKEN_QUIT", 
  "BX_TOKEN_REGISTERS", "BX_TOKEN_CPU", "BX_TOKEN_FPU", "BX_TOKEN_SSE", 
  "BX_TOKEN_ALL", "BX_TOKEN_IDT", "BX_TOKEN_GDT", "BX_TOKEN_LDT", 
  "BX_TOKEN_TSS", "BX_TOKEN_DIRTY", "BX_TOKEN_LINUX", 
  "BX_TOKEN_CONTROL_REGS", "BX_TOKEN_EXAMINE", "BX_TOKEN_XFORMAT", 
  "BX_TOKEN_DISFORMAT", "BX_TOKEN_SETPMEM", "BX_TOKEN_SYMBOLNAME", 
  "BX_TOKEN_QUERY", "BX_TOKEN_PENDING", "BX_TOKEN_TAKE", "BX_TOKEN_DMA", 
  "BX_TOKEN_IRQ", "BX_TOKEN_DUMP_CPU", "BX_TOKEN_SET_CPU", 
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
  "BX_TOKEN_PIC", "BX_TOKEN_PAGE", "BX_TOKEN_ALWAYS_CHECK", 
  "BX_TOKEN_TRACEREGON", "BX_TOKEN_TRACEREGOFF", "BX_TOKEN_HELP", 
  "BX_TOKEN_CALC", "BX_TOKEN_VGA", "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT", 
  "BX_TOKEN_IVT", "'+'", "'-'", "'|'", "'^'", "'*'", "'/'", "'&'", "NEG", 
  "NOT", "'\\n'", "':'", "'='", "'!'", "'('", "')'", "$accept", 
  "commands", "command", "cosim_commands", "BX_TOKEN_SEGREG", 
  "timebp_command", "record_command", "playback_command", 
  "modebp_command", "show_command", "ptime_command", "trace_on_command", 
  "trace_off_command", "print_stack_command", "watch_point_command", 
  "symbol_command", "where_command", "print_string_command", 
  "continue_command", "stepN_command", "step_over_command", "set_command", 
  "breakpoint_command", "blist_command", "slist_command", "info_command", 
  "optional_numeric_range", "numeric_range", "regs_command", 
  "dump_cpu_command", "delete_command", "bpe_command", "bpd_command", 
  "quit_command", "examine_command", "setpmem_command", "query_command", 
  "take_command", "set_cpu_command", "disassemble_command", 
  "instrument_command", "loader_command", "doit_command", "crc_command", 
  "trace_reg_on_command", "trace_reg_off_command", "help_command", 
  "calc_command", "BX_TOKEN_8BH_REG", "BX_TOKEN_8BL_REG", 
  "BX_TOKEN_16B_REG", "BX_TOKEN_32B_REG", "vexpression", "expression", 0
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
     163,   163,   164,   165,   165,   165,   165,   165,   165,   165,
     166,   166,   166,   166,   166,   166,   166,   166,   167,   168,
     168,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   170,   170,   171,   171,   172,   173,   174,
     175,   176,   177,   178,   178,   178,   178,   179,   180,   181,
     181,   181,   182,   183,   183,   183,   183,   184,   184,   184,
     184,   185,   186,   187,   188,   189,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   191,   192,   192,   192,   192,
     193,   193,   193,   193,   194,   194,   194,   194,   194,   194,
     194,   194,   195,   195,   195,   195,   195,   195,   195,   195,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197
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
       2,     3,     2,     4,     4,     5,     5,     5,     5,     5,
       2,     5,     2,     3,     3,     2,     3,     4,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     4,     4,
       4,     4,     4,     3,     3,     3,     4,     3,     3,     5,
       7,     3,     3,     0,     1,     1,     2,     2,     2,     3,
       3,     3,     2,     4,     3,     3,     2,     5,     3,     3,
       4,     3,     2,     3,     4,     3,     5,     3,     3,     3,
       3,     3,     3,     4,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
      46,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   133,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    47,     0,     3,    41,    31,    32,    33,
      34,    37,    30,    28,    29,    35,    36,    38,    39,    40,
       4,     5,     6,     7,     8,    11,    12,     9,    10,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    42,    43,    44,    45,    89,     0,
      90,    92,   210,   211,   212,   213,   206,   207,   208,   209,
     214,   215,   216,   217,   222,   223,   224,   225,   218,   219,
     220,   221,   226,   227,   228,   229,     0,     0,     0,     0,
       0,     0,   108,    58,    59,    60,    61,    62,    63,   230,
     231,     0,   100,     0,     0,   236,   233,   232,   234,   235,
       0,   251,   249,   250,     0,   102,     0,     0,   256,   253,
     252,   254,   255,     0,   251,     0,   105,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   133,   133,
     133,   133,     0,     0,     0,     0,     0,     0,     0,   133,
     142,   137,     0,   146,     0,     0,     0,     0,     0,   138,
     152,   133,     0,     0,     0,   134,   135,     0,     0,     0,
       0,     0,     0,     0,    72,    73,    71,     0,     0,     0,
       0,    68,     0,    74,     0,     0,     0,     0,    78,     0,
       0,    79,     0,    70,     0,     0,     0,   109,    87,     0,
      48,     0,     0,     0,     0,     0,     0,     0,     0,   164,
     165,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   204,
       0,     1,     2,    91,     0,     0,     0,     0,     0,     0,
       0,   247,   246,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   104,   268,   267,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   103,     0,     0,
     106,   139,   140,   141,   123,   111,   113,   112,   114,   115,
     116,     0,     0,     0,     0,   117,   124,   127,     0,   125,
       0,   128,   131,   132,     0,   144,     0,   145,     0,   148,
       0,   149,   151,     0,   155,     0,   153,     0,   136,   157,
     158,   159,   160,   161,   162,     0,    65,    64,    66,    67,
      75,    77,    76,     0,     0,     0,     0,    69,     0,    84,
       0,   110,    88,    49,    50,    51,    52,    53,    54,     0,
       0,     0,   167,   168,   169,   199,   175,   170,   171,   172,
     173,   174,   200,   166,   192,   197,   193,   182,   183,   194,
     198,   177,   178,   179,   184,   186,   185,   188,   189,   176,
     187,   195,   196,   201,   190,   191,   180,   181,   203,   202,
     205,     0,    93,    94,     0,     0,     0,     0,   248,   241,
     242,   237,   238,   243,   244,   239,   240,   245,     0,   269,
     262,   263,   258,   259,   264,   265,   260,   261,   266,   257,
     107,   118,   120,   121,   122,   126,     0,   119,   143,     0,
     150,   154,     0,   259,   163,    80,    82,    81,    83,    85,
       0,     0,    56,    57,    95,    97,    96,    98,    99,   101,
       0,   129,   147,   156,    86,    55,     0,   130
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    54,    55,    56,   158,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,   204,   205,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,   159,   160,
     161,   162,   150,   206
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -164
static const short yypact[] =
{
     994,  -124,  -108,  -119,   621,  -117,   749,   577,   399,   -89,
     -80,   -68,   406,  -115,  -106,   539,   -66,   -21,   -46,   -86,
     -83,   367,    59,   -26,   -30,    -9,   -12,     6,     8,    33,
      40,    76,    82,    29,  -103,    93,   -91,   -82,   -77,   -81,
      34,    60,    43,    66,   106,    64,    68,    67,    69,    46,
      52,   747,    61,  -164,   886,  -164,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,    54,
    -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,  -164,  -164,    55,   115,    56,    73,
      74,    75,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
    -164,   889,  -164,   889,   889,  -164,  -164,  -164,  -164,  -164,
     313,    78,  -164,  -164,    61,  -164,    61,    61,  -164,  -164,
    -164,  -164,  -164,   150,  -164,    61,  -164,   169,    81,    83,
      84,    85,    86,    87,    88,    89,    90,   101,    61,    61,
      61,    61,   102,   105,   110,   -79,   -99,   118,   119,    61,
    -164,  -164,   717,  -164,   360,   127,   122,  -102,   133,  -164,
    -164,    61,   134,    80,   136,  -164,   103,   137,   148,   149,
     156,   159,   167,   144,  -164,  -164,  -164,   171,   172,   182,
     220,  -164,   221,  -164,   252,   257,   308,   321,  -164,   322,
     323,  -164,   289,  -164,  -101,   113,   300,  -164,  -164,   311,
    -164,   312,   316,   317,   320,   335,   347,   349,   117,  -164,
    -164,   358,   362,   363,   364,   365,   366,   368,   369,   373,
     374,   377,   378,   379,   380,   381,   382,   383,   384,   387,
     388,   389,   390,   392,   395,   397,   398,   400,   401,   424,
     429,   436,   437,   438,   439,   440,   441,   462,   474,  -164,
     533,  -164,  -164,  -164,   391,   475,   476,    61,    61,    61,
      61,  -164,  -164,   135,   889,   889,   889,   889,   889,   889,
     889,   889,   889,    61,  -164,   477,   477,   120,    61,    61,
      61,    61,    61,    61,    61,    61,    61,  -164,    61,   570,
    -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
    -164,   479,   480,   481,   482,  -164,  -164,  -164,   483,  -164,
     501,  -164,  -164,  -164,   484,  -164,   814,  -164,   530,  -164,
     511,  -164,  -164,   512,  -164,   537,  -164,    61,   345,  -164,
    -164,  -164,  -164,  -164,  -164,   514,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,   517,   518,   519,   520,  -164,   532,  -164,
     559,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,   560,
     538,   540,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
    -164,   541,  -164,  -164,   922,   995,  1009,  1023,  -164,  -164,
    -164,   -73,   -73,   -73,   -73,  -164,  -164,  -164,  1037,  -164,
     477,   477,    30,    30,    30,    30,   477,   477,   477,   345,
    -164,  -164,  -164,  -164,  -164,  -164,   -49,  -164,  -164,   544,
    -164,  -164,   545,    30,  -164,  -164,  -164,  -164,  -164,  -164,
     546,   547,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
     561,  -164,  -164,  -164,  -164,  -164,   548,  -164
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -164,  -164,   219,  -164,    57,  -164,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,  -164,  -164,  -163,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,
    -164,  -164,  -164,  -164,  -164,  -164,  -164,  -164,    18,    27,
      36,    45,  -100,    -7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned short yytable[] =
{
     163,   167,   232,   236,   500,   348,    99,   234,   194,   229,
     230,   222,   360,   388,    98,   341,   342,   343,   344,   101,
     350,   132,   128,   190,   146,   168,   354,   197,   198,   235,
     100,   129,   191,   147,   169,   223,   361,   389,   363,   351,
     130,   301,   148,   302,   303,   290,   170,   231,   195,   131,
     196,   149,   199,   304,   305,   200,   233,   237,   211,   349,
     310,   311,   312,   145,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   212,   122,   123,   124,   125,   501,
     133,   134,   135,   136,   137,   138,   133,   134,   135,   136,
     137,   138,   241,   242,   247,   213,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   214,   122,   123,   124,
     125,   224,   133,   134,   135,   136,   137,   138,   207,   208,
     209,   210,   243,   244,   215,   164,   216,   315,   217,   316,
     317,   295,   296,   400,   401,   218,   318,   319,   329,   146,
     219,   146,   146,   324,   325,   326,   220,   221,   147,   328,
     147,   147,   238,   225,   239,   152,   153,   148,   245,   148,
     148,   240,   246,   248,   249,   356,   149,   164,   149,   149,
     250,   154,   293,   226,   227,   294,   297,   390,   145,   368,
     145,   145,   156,   157,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   298,   299,   300,   314,   152,   153,   331,
     365,   332,   333,   334,   335,   336,   337,   338,   339,   318,
     319,   228,   320,   367,   322,   323,   324,   325,   326,   340,
     345,   358,   328,   346,   156,   157,   318,   319,   347,   320,
     321,   322,   323,   324,   325,   326,   352,   353,   375,   328,
     359,   304,   305,   459,   306,   307,   308,   309,   310,   311,
     312,   362,   364,   292,   366,   369,   318,   319,   448,   320,
     321,   322,   323,   324,   325,   326,   370,   371,   327,   328,
     444,   445,   446,   447,   372,   318,   319,   373,   320,   321,
     322,   323,   324,   325,   326,   374,   458,   330,   328,   376,
     377,   460,   461,   462,   463,   464,   465,   466,   467,   468,
     378,   469,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     148,   148,   148,   148,   148,   148,   148,   148,   148,   149,
     149,   149,   149,   149,   149,   149,   149,   149,   379,   380,
     483,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     381,   122,   123,   124,   125,   382,   133,   134,   135,   136,
     137,   138,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   383,   122,   123,   124,   125,   387,   133,   134,
     135,   136,   137,   138,   201,   384,   385,   386,   391,   304,
     305,   171,   306,   307,   308,   309,   310,   311,   312,   392,
     393,   164,   313,   172,   394,   395,   202,   203,   396,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   318,   319,   397,   320,   321,   322,   323,   324,   325,
     326,   152,   153,   164,   328,   398,   318,   319,   399,   320,
     321,   322,   323,   324,   325,   326,   402,   154,   357,   328,
     403,   404,   405,   406,   407,   441,   408,   409,   156,   157,
     185,   410,   411,   152,   153,   412,   413,   414,   415,   416,
     417,   418,   419,   186,   187,   420,   421,   422,   423,   154,
     424,   188,   165,   425,   189,   426,   427,   166,   428,   429,
     156,   157,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   430,   122,   123,   124,   125,   431,   133,   134,
     135,   136,   137,   138,   432,   433,   434,   435,   436,   437,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     438,   122,   123,   124,   125,   192,   133,   134,   135,   136,
     137,   138,   439,   442,   443,   476,   328,   471,   472,   473,
     474,   475,   477,   164,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   479,   122,   123,   124,   125,   480,
     481,   482,   484,   152,   153,   485,   486,   487,   488,   318,
     319,   151,   320,   321,   322,   323,   324,   325,   326,   154,
     489,   440,   328,   490,   491,   506,   492,   193,   493,   494,
     156,   157,   502,   503,   504,   505,   507,     0,     0,     0,
     126,   152,   153,     0,     0,     0,   318,   319,   127,   320,
     321,   322,   323,   324,   325,   326,     0,   154,   470,   328,
       0,     0,     0,     0,     0,   155,     0,     0,   156,   157,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
       0,   122,   123,   124,   125,     0,   133,   134,   135,   136,
     137,   138,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,     0,   122,   123,   124,   125,     0,   133,   134,
     135,   136,   137,   138,     0,   251,   252,   253,     0,   254,
       0,   255,   256,   257,     0,   258,   259,   260,   261,   262,
     263,   164,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   264,     0,     0,   265,     0,     0,     0,     0,
       0,     0,   266,   267,   268,   269,     0,     0,     0,     0,
       0,   152,   153,   270,   271,   272,     0,     0,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   154,     0,   282,
     283,     0,   284,     0,     0,   355,     0,     0,   156,   157,
       0,     0,     0,   139,   140,     0,     0,     0,   285,   286,
     287,   288,     0,     0,     0,     0,     0,     0,     0,   141,
       0,     0,     0,     0,     0,   289,   291,   142,     0,     0,
     143,   144,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,     0,   122,   123,   124,   125,     0,   133,   134,
     135,   136,   137,   138,     1,     2,     3,     0,     4,     0,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     318,   319,     0,   320,   321,   322,   323,   324,   325,   326,
       0,    15,   478,   328,    16,     0,    17,     0,    18,     0,
       0,    19,    20,    21,    22,     0,     0,     0,     0,    23,
       0,    24,    25,    26,    27,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,     0,    37,    38,
       0,    39,     0,    40,    41,    42,    43,    44,    45,    46,
       0,     0,    47,   139,   140,     0,    48,    49,    50,    51,
      52,     0,     0,     0,     0,     0,     0,     0,     0,   141,
       0,     0,     0,     0,    53,     0,     0,     0,     0,     0,
     143,   144,     1,     2,     3,     0,     4,     0,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,   318,   319,
       0,   320,   321,   322,   323,   324,   325,   326,     0,    15,
     495,   328,    16,     0,    17,     0,    18,     0,     0,    19,
      20,    21,    22,     0,     0,     0,     0,    23,     0,    24,
      25,    26,    27,     0,     0,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,     0,    37,    38,     0,    39,
       0,    40,    41,    42,    43,    44,    45,    46,     0,     0,
      47,     0,     0,     0,    48,    49,    50,    51,    52,     0,
       0,   318,   319,     0,   320,   321,   322,   323,   324,   325,
     326,     0,    53,   496,   328,   318,   319,     0,   320,   321,
     322,   323,   324,   325,   326,     0,     0,   497,   328,   318,
     319,     0,   320,   321,   322,   323,   324,   325,   326,     0,
       0,   498,   328,   318,   319,     0,   320,   321,   322,   323,
     324,   325,   326,     0,     0,   499,   328
};

static const short yycheck[] =
{
       7,     8,    84,    84,    53,    84,   114,    84,    15,   100,
     101,   114,   114,   114,   138,   178,   179,   180,   181,   138,
     119,   138,     4,   138,     6,   114,   189,    73,    74,   106,
     138,     4,   138,     6,   114,   138,   138,   138,   201,   138,
       4,   141,     6,   143,   144,    52,   114,   138,   114,     4,
      71,     6,   138,   126,   127,   138,   138,   138,    84,   138,
     133,   134,   135,     6,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,   114,    24,    25,    26,    27,   138,
      29,    30,    31,    32,    33,    34,    29,    30,    31,    32,
      33,    34,    36,    37,    47,   114,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,   138,    24,    25,    26,
      27,    38,    29,    30,    31,    32,    33,    34,    79,    80,
      81,    82,    36,    37,   138,    84,   138,   154,   115,   156,
     157,    36,    37,    36,    37,   115,   126,   127,   165,   141,
      84,   143,   144,   133,   134,   135,    84,   138,   141,   139,
     143,   144,   138,    80,   114,   114,   115,   141,   114,   143,
     144,   138,   114,   114,   138,   192,   141,    84,   143,   144,
     138,   130,   138,   100,   101,   140,   140,    84,   141,   206,
     143,   144,   141,   142,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   140,   140,   140,   138,   114,   115,   138,
     140,   138,   138,   138,   138,   138,   138,   138,   138,   126,
     127,   138,   129,   130,   131,   132,   133,   134,   135,   138,
     138,   114,   139,   138,   141,   142,   126,   127,   138,   129,
     130,   131,   132,   133,   134,   135,   138,   138,   114,   139,
     138,   126,   127,   143,   129,   130,   131,   132,   133,   134,
     135,   138,   138,    54,   138,   138,   126,   127,   143,   129,
     130,   131,   132,   133,   134,   135,   138,   138,   138,   139,
     297,   298,   299,   300,   138,   126,   127,   138,   129,   130,
     131,   132,   133,   134,   135,   138,   313,   138,   139,   138,
     138,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     138,   328,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   138,   138,
     367,   304,   305,   306,   307,   308,   309,   310,   311,   312,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
     138,    24,    25,    26,    27,   138,    29,    30,    31,    32,
      33,    34,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,   114,    24,    25,    26,    27,   138,    29,    30,
      31,    32,    33,    34,    67,   114,   114,   114,   138,   126,
     127,    35,   129,   130,   131,   132,   133,   134,   135,   138,
     138,    84,   139,    47,   138,   138,    89,    90,   138,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,   126,   127,   138,   129,   130,   131,   132,   133,   134,
     135,   114,   115,    84,   139,   138,   126,   127,   139,   129,
     130,   131,   132,   133,   134,   135,   138,   130,   138,   139,
     138,   138,   138,   138,   138,   114,   138,   138,   141,   142,
     104,   138,   138,   114,   115,   138,   138,   138,   138,   138,
     138,   138,   138,   117,   118,   138,   138,   138,   138,   130,
     138,   125,   133,   138,   128,   138,   138,   138,   138,   138,
     141,   142,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,   138,    24,    25,    26,    27,   138,    29,    30,
      31,    32,    33,    34,   138,   138,   138,   138,   138,   138,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
     138,    24,    25,    26,    27,    66,    29,    30,    31,    32,
      33,    34,   138,   138,   138,   114,   139,   138,   138,   138,
     138,   138,   138,    84,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,   114,    24,    25,    26,    27,   138,
     138,   114,   138,   114,   115,   138,   138,   138,   138,   126,
     127,    84,   129,   130,   131,   132,   133,   134,   135,   130,
     138,   138,   139,   114,   114,   114,   138,   138,   138,   138,
     141,   142,   138,   138,   138,   138,   138,    -1,    -1,    -1,
      69,   114,   115,    -1,    -1,    -1,   126,   127,    77,   129,
     130,   131,   132,   133,   134,   135,    -1,   130,   138,   139,
      -1,    -1,    -1,    -1,    -1,   138,    -1,    -1,   141,   142,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      -1,    24,    25,    26,    27,    -1,    29,    30,    31,    32,
      33,    34,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    -1,    24,    25,    26,    27,    -1,    29,    30,
      31,    32,    33,    34,    -1,    38,    39,    40,    -1,    42,
      -1,    44,    45,    46,    -1,    48,    49,    50,    51,    52,
      53,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    -1,    -1,    68,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    76,    77,    78,    -1,    -1,    -1,    -1,
      -1,   114,   115,    86,    87,    88,    -1,    -1,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   130,    -1,   102,
     103,    -1,   105,    -1,    -1,   138,    -1,    -1,   141,   142,
      -1,    -1,    -1,   114,   115,    -1,    -1,    -1,   121,   122,
     123,   124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,
      -1,    -1,    -1,    -1,    -1,   138,     0,   138,    -1,    -1,
     141,   142,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    -1,    24,    25,    26,    27,    -1,    29,    30,
      31,    32,    33,    34,    38,    39,    40,    -1,    42,    -1,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
     126,   127,    -1,   129,   130,   131,   132,   133,   134,   135,
      -1,    65,   138,   139,    68,    -1,    70,    -1,    72,    -1,
      -1,    75,    76,    77,    78,    -1,    -1,    -1,    -1,    83,
      -1,    85,    86,    87,    88,    -1,    -1,    91,    92,    93,
      94,    95,    96,    97,    98,    99,    -1,    -1,   102,   103,
      -1,   105,    -1,   107,   108,   109,   110,   111,   112,   113,
      -1,    -1,   116,   114,   115,    -1,   120,   121,   122,   123,
     124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,
      -1,    -1,    -1,    -1,   138,    -1,    -1,    -1,    -1,    -1,
     141,   142,    38,    39,    40,    -1,    42,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,   126,   127,
      -1,   129,   130,   131,   132,   133,   134,   135,    -1,    65,
     138,   139,    68,    -1,    70,    -1,    72,    -1,    -1,    75,
      76,    77,    78,    -1,    -1,    -1,    -1,    83,    -1,    85,
      86,    87,    88,    -1,    -1,    91,    92,    93,    94,    95,
      96,    97,    98,    99,    -1,    -1,   102,   103,    -1,   105,
      -1,   107,   108,   109,   110,   111,   112,   113,    -1,    -1,
     116,    -1,    -1,    -1,   120,   121,   122,   123,   124,    -1,
      -1,   126,   127,    -1,   129,   130,   131,   132,   133,   134,
     135,    -1,   138,   138,   139,   126,   127,    -1,   129,   130,
     131,   132,   133,   134,   135,    -1,    -1,   138,   139,   126,
     127,    -1,   129,   130,   131,   132,   133,   134,   135,    -1,
      -1,   138,   139,   126,   127,    -1,   129,   130,   131,   132,
     133,   134,   135,    -1,    -1,   138,   139
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    38,    39,    40,    42,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    65,    68,    70,    72,    75,
      76,    77,    78,    83,    85,    86,    87,    88,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   102,   103,   105,
     107,   108,   109,   110,   111,   112,   113,   116,   120,   121,
     122,   123,   124,   138,   145,   146,   147,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   138,   114,
     138,   138,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    24,    25,    26,    27,    69,    77,   192,   193,
     194,   195,   138,    29,    30,    31,    32,    33,    34,   114,
     115,   130,   138,   141,   142,   148,   192,   193,   194,   195,
     196,    84,   114,   115,   130,   138,   141,   142,   148,   192,
     193,   194,   195,   197,    84,   133,   138,   197,   114,   114,
     114,    35,    47,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,   104,   117,   118,   125,   128,
     138,   138,    66,   138,   197,   114,    71,    73,    74,   138,
     138,    67,    89,    90,   170,   171,   197,    79,    80,    81,
      82,    84,   114,   114,   138,   138,   138,   115,   115,    84,
      84,   138,   114,   138,    38,    80,   100,   101,   138,   100,
     101,   138,    84,   138,    84,   106,    84,   138,   138,   114,
     138,    36,    37,    36,    37,   114,   114,   148,   114,   138,
     138,    38,    39,    40,    42,    44,    45,    46,    48,    49,
      50,    51,    52,    53,    65,    68,    75,    76,    77,    78,
      86,    87,    88,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   102,   103,   105,   121,   122,   123,   124,   138,
     197,     0,   146,   138,   140,    36,    37,   140,   140,   140,
     140,   196,   196,   196,   126,   127,   129,   130,   131,   132,
     133,   134,   135,   139,   138,   197,   197,   197,   126,   127,
     129,   130,   131,   132,   133,   134,   135,   138,   139,   197,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   170,   170,   170,   170,   138,   138,   138,    84,   138,
     119,   138,   138,   138,   170,   138,   197,   138,   114,   138,
     114,   138,   138,   170,   138,   140,   138,   130,   197,   138,
     138,   138,   138,   138,   138,   114,   138,   138,   138,   138,
     138,   138,   138,   114,   114,   114,   114,   138,   114,   138,
      84,   138,   138,   138,   138,   138,   138,   138,   138,   139,
      36,    37,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   114,   138,   138,   197,   197,   197,   197,   143,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   197,   143,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     138,   138,   138,   138,   138,   138,   114,   138,   138,   114,
     138,   138,   114,   197,   138,   138,   138,   138,   138,   138,
     114,   114,   138,   138,   138,   138,   138,   138,   138,   138,
      53,   138,   138,   138,   138,   138,   114,   138
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
#line 224 "parser.y"
    {
      }
    break;

  case 48:
#line 230 "parser.y"
    {
		bx_dbg_diff_memory();
		free(yyvsp[-1].sval);
	}
    break;

  case 50:
#line 236 "parser.y"
    {
		bx_dbg_sync_memory(yyvsp[-1].bval);
		free(yyvsp[-2].sval);
	}
    break;

  case 52:
#line 242 "parser.y"
    {
		bx_dbg_sync_cpu(yyvsp[-1].bval);
		free(yyvsp[-2].sval);
	}
    break;

  case 53:
#line 247 "parser.y"
    {
		free(yyvsp[-2].sval);
		bx_dbg_fast_forward(yyvsp[-1].uval);
	}
    break;

  case 54:
#line 252 "parser.y"
    {
		free(yyvsp[-2].sval);
	}
    break;

  case 55:
#line 256 "parser.y"
    {
		free(yyvsp[-4].sval);
		bx_dbg_info_address(yyvsp[-3].uval, yyvsp[-1].uval);
        }
    break;

  case 56:
#line 261 "parser.y"
    {
		free(yyvsp[-3].sval);
        }
    break;

  case 57:
#line 265 "parser.y"
    {
		free(yyvsp[-3].sval);
        }
    break;

  case 63:
#line 277 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 64:
#line 282 "parser.y"
    {
        bx_dbg_timebp_command(0, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
    break;

  case 65:
#line 287 "parser.y"
    {
        bx_dbg_timebp_command(1, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
    break;

  case 66:
#line 295 "parser.y"
    {
          bx_dbg_record_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 67:
#line 303 "parser.y"
    {
          bx_dbg_playback_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 68:
#line 311 "parser.y"
    {
          bx_dbg_modebp_command();
          free(yyvsp[-1].sval);
          }
    break;

  case 69:
#line 319 "parser.y"
    {
          bx_dbg_show_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 70:
#line 324 "parser.y"
    {
          bx_dbg_show_command(0);
          free(yyvsp[-1].sval);
          }
    break;

  case 71:
#line 332 "parser.y"
    {
        bx_dbg_ptime_command();
        free(yyvsp[-1].sval);
	}
    break;

  case 72:
#line 340 "parser.y"
    {
        bx_dbg_trace_on_command();
        free(yyvsp[-1].sval);
	}
    break;

  case 73:
#line 348 "parser.y"
    {
        bx_dbg_trace_off_command();
        free(yyvsp[-1].sval);
	}
    break;

  case 74:
#line 356 "parser.y"
    {
          bx_dbg_print_stack_command(16);
          free(yyvsp[-1].sval);
	  }
    break;

  case 75:
#line 361 "parser.y"
    {
          bx_dbg_print_stack_command(yyvsp[-1].uval);
          free(yyvsp[-2].sval);
	  }
    break;

  case 76:
#line 369 "parser.y"
    {
          watchpoint_continue = 0;
	  dbg_printf("Will stop on watch points\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 77:
#line 375 "parser.y"
    {
          watchpoint_continue = 1;
          dbg_printf("Will not stop on watch points (they will still be logged)\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 78:
#line 381 "parser.y"
    {
          bx_dbg_watch(-1, 0);
          free(yyvsp[-1].sval);
          }
    break;

  case 79:
#line 386 "parser.y"
    {
          bx_dbg_unwatch(-1, 0);
          free(yyvsp[-1].sval);
          }
    break;

  case 80:
#line 391 "parser.y"
    {
          bx_dbg_watch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 81:
#line 396 "parser.y"
    {
          bx_dbg_unwatch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 82:
#line 401 "parser.y"
    {
          bx_dbg_watch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 83:
#line 406 "parser.y"
    {
          bx_dbg_unwatch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 84:
#line 414 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-1].sval, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 85:
#line 419 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-2].sval, 0, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 86:
#line 424 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-2].sval, 1, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 87:
#line 432 "parser.y"
    {
        bx_dbg_where_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 88:
#line 440 "parser.y"
    {
        bx_dbg_print_string_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 89:
#line 448 "parser.y"
    {
        bx_dbg_continue_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 90:
#line 456 "parser.y"
    {
        bx_dbg_stepN_command(1);
        free(yyvsp[-1].sval);
        }
    break;

  case 91:
#line 461 "parser.y"
    {
        bx_dbg_stepN_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 92:
#line 469 "parser.y"
    {
        bx_dbg_step_over_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 94:
#line 478 "parser.y"
    {
        bx_dbg_set_auto_disassemble(yyvsp[-1].bval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 95:
#line 483 "parser.y"
    {
        bx_dbg_set_symbol_command(yyvsp[-3].sval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
    break;

  case 96:
#line 488 "parser.y"
    { 
        bx_dbg_set_reg8l_value(yyvsp[-3].uval, yyvsp[-1].ulval);
        }
    break;

  case 97:
#line 492 "parser.y"
    { 
        bx_dbg_set_reg8h_value(yyvsp[-3].uval, yyvsp[-1].ulval);
        }
    break;

  case 98:
#line 496 "parser.y"
    { 
        bx_dbg_set_reg16_value(yyvsp[-3].uval, yyvsp[-1].ulval);
        }
    break;

  case 99:
#line 500 "parser.y"
    { 
        bx_dbg_set_reg32_value(yyvsp[-3].uval, yyvsp[-1].ulval);
        }
    break;

  case 100:
#line 507 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 101:
#line 512 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkRegular, yyvsp[-3].ulval, yyvsp[-1].ulval);
        free(yyvsp[-4].sval);
        }
    break;

  case 102:
#line 517 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 103:
#line 522 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-2].sval);
        }
    break;

  case 104:
#line 527 "parser.y"
    {
        bx_dbg_lbreakpoint_symbol_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval);free(yyvsp[-1].sval);
        }
    break;

  case 105:
#line 532 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 106:
#line 537 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-2].sval);
        }
    break;

  case 107:
#line 542 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-3].sval);
        }
    break;

  case 108:
#line 550 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 109:
#line 558 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free(yyvsp[-1].sval);
        }
    break;

  case 110:
#line 563 "parser.y"
    {
        bx_dbg_info_symbols_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval);free(yyvsp[-1].sval);
        }
    break;

  case 111:
#line 571 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 112:
#line 576 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 113:
#line 581 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 114:
#line 586 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 115:
#line 591 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 116:
#line 596 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS | BX_INFO_FPU_REGS | BX_INFO_SSE_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 117:
#line 601 "parser.y"
    {
        bx_dbg_info_dirty_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
    break;

  case 118:
#line 606 "parser.y"
    {
        bx_dbg_info_idt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 119:
#line 611 "parser.y"
    {
       bx_dbg_info_ivt_command(yyvsp[-1].uval_range);
       free(yyvsp[-3].sval); free(yyvsp[-2].sval);
       }
    break;

  case 120:
#line 616 "parser.y"
    {
        bx_dbg_info_gdt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 121:
#line 621 "parser.y"
    {
        bx_dbg_info_ldt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 122:
#line 626 "parser.y"
    {
        bx_dbg_info_tss_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 123:
#line 631 "parser.y"
    {
        bx_dbg_info_flags();
        free(yyvsp[-2].sval);
        }
    break;

  case 124:
#line 636 "parser.y"
    {
        bx_dbg_info_linux_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 125:
#line 641 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 126:
#line 646 "parser.y"
    {
        bx_dbg_info_symbols_command(yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 127:
#line 651 "parser.y"
    {
        bx_dbg_info_control_regs_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 128:
#line 656 "parser.y"
    {
        bx_dbg_info_ne2k(-1, -1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 129:
#line 661 "parser.y"
    {
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-1].uval, -1);
        }
    break;

  case 130:
#line 666 "parser.y"
    {
        free(yyvsp[-6].sval); free(yyvsp[-5].sval); free(yyvsp[-4].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-3].uval, yyvsp[-1].uval);
        }
    break;

  case 131:
#line 671 "parser.y"
    {
        bx_dbg_info_pic();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 132:
#line 676 "parser.y"
    {
        bx_dbg_info_vga();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 133:
#line 687 "parser.y"
    { yyval.uval_range = make_num_range (EMPTY_ARG, EMPTY_ARG); }
    break;

  case 135:
#line 692 "parser.y"
    {
      yyval.uval_range = make_num_range (yyvsp[0].ulval, yyvsp[0].ulval);
    }
    break;

  case 136:
#line 696 "parser.y"
    {
      yyval.uval_range = make_num_range (yyvsp[-1].ulval, yyvsp[0].ulval);
    }
    break;

  case 137:
#line 702 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free(yyvsp[-1].sval);
        }
    break;

  case 138:
#line 710 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 139:
#line 718 "parser.y"
    {
        bx_dbg_del_breakpoint_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 140:
#line 726 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command(yyvsp[-1].uval, 1);
        free(yyvsp[-2].sval);
       }
    break;

  case 141:
#line 733 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command(yyvsp[-1].uval, 0);
        free(yyvsp[-2].sval);
       }
    break;

  case 142:
#line 741 "parser.y"
    {
	  bx_dbg_quit_command();
	  free(yyvsp[-1].sval);
        }
    break;

  case 143:
#line 749 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].ulval,1, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].ulval,1, 1);
#endif
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 144:
#line 757 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 1);
#endif
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 145:
#line 765 "parser.y"
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

  case 146:
#line 778 "parser.y"
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

  case 147:
#line 794 "parser.y"
    {
        bx_dbg_setpmem_command(yyvsp[-3].uval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
    break;

  case 148:
#line 802 "parser.y"
    {
        bx_dbg_query_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 149:
#line 810 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 150:
#line 815 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-2].sval, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 151:
#line 820 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 152:
#line 828 "parser.y"
    {
        bx_dbg_set_cpu_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 153:
#line 836 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, yyvsp[-1].uval_range);
        free(yyvsp[-2].sval);
        }
    break;

  case 154:
#line 841 "parser.y"
    {
        bx_dbg_disassemble_command(yyvsp[-2].sval, yyvsp[-1].uval_range);
	free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 155:
#line 846 "parser.y"
    {
        bx_dbg_disassemble_switch_mode();
	free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 156:
#line 851 "parser.y"
    {
        bx_dbg_set_disassemble_size(yyvsp[-1].uval);
	free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
    break;

  case 160:
#line 862 "parser.y"
    {
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 161:
#line 870 "parser.y"
    {
        bx_dbg_loader_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 162:
#line 878 "parser.y"
    {
        bx_dbg_doit_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 163:
#line 886 "parser.y"
    {
        bx_dbg_crc_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
    break;

  case 164:
#line 894 "parser.y"
    {
	bx_dbg_trace_reg_on_command();
	free(yyvsp[-1].sval);
	}
    break;

  case 165:
#line 902 "parser.y"
    {
	bx_dbg_trace_reg_off_command();
	free(yyvsp[-1].sval);
	}
    break;

  case 166:
#line 910 "parser.y"
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 167:
#line 915 "parser.y"
    {
         dbg_printf("c|cont|continue - continue executing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 168:
#line 920 "parser.y"
    {
         dbg_printf("s|step|stepi [count] - execute #count instructions (default is one instruction)\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 169:
#line 925 "parser.y"
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 170:
#line 930 "parser.y"
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 171:
#line 935 "parser.y"
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 172:
#line 940 "parser.y"
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 173:
#line 945 "parser.y"
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 174:
#line 950 "parser.y"
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 175:
#line 955 "parser.y"
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 176:
#line 960 "parser.y"
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 177:
#line 965 "parser.y"
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 178:
#line 970 "parser.y"
    {
         dbg_printf("trace-on - disassemble every executed instruction\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 179:
#line 975 "parser.y"
    {
         dbg_printf("trace-off - disable instruction tracing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 180:
#line 980 "parser.y"
    {
         dbg_printf("trace-reg-on - print all registers every executed instruction\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 181:
#line 985 "parser.y"
    {
         dbg_printf("trace-reg-off - disable registers state tracing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 182:
#line 990 "parser.y"
    {
         dbg_printf("dump_cpu - dump complete cpu state\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 183:
#line 995 "parser.y"
    {
         dbg_printf("set_cpu - set complete cpu state\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 184:
#line 1000 "parser.y"
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 185:
#line 1005 "parser.y"
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 186:
#line 1010 "parser.y"
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 187:
#line 1015 "parser.y"
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 188:
#line 1020 "parser.y"
    {
         dbg_printf("record <filename> - record console input to file filename\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 189:
#line 1025 "parser.y"
    {
         dbg_printf("playback <filename> - playback console input from file filename\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 190:
#line 1030 "parser.y"
    {
         dbg_printf("load-symbols [global] <filename> [offset] - load symbols from file\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 191:
#line 1035 "parser.y"
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 192:
#line 1040 "parser.y"
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 193:
#line 1045 "parser.y"
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 194:
#line 1050 "parser.y"
    {
         dbg_printf("u|disasm|disassemble [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm|disassemble switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm|disassemble size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 195:
#line 1059 "parser.y"
    {
         dbg_printf("watch - print current watch point status\n");
         dbg_printf("watch stop - stop simulation when a watchpoint is encountred\n");
         dbg_printf("watch continue - do not stop the simulation when watch point is encountred\n");
         dbg_printf("watch read addr - insert a read watch point at physical address addr\n");
         dbg_printf("watch write addr - insert a write watch point at physical address addr\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 196:
#line 1068 "parser.y"
    {
         dbg_printf("unwatch - remove all watch points\n");
         dbg_printf("unwatch read addr - remove a read watch point at physical address addr\n");
         dbg_printf("unwatch write addr - remove a write watch point at physical address addr\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 197:
#line 1075 "parser.y"
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

  case 198:
#line 1088 "parser.y"
    {
         dbg_printf("instrument start - calls bx_instr_start() callback\n");
         dbg_printf("instrument stop  - calls bx_instr_stop() callback\n");
         dbg_printf("instrument reset - calls bx_instr_reset() callback\n");
         dbg_printf("instrument print - calls bx_instr_print() callback\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 199:
#line 1096 "parser.y"
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

  case 200:
#line 1106 "parser.y"
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

  case 201:
#line 1126 "parser.y"
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

  case 202:
#line 1140 "parser.y"
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

  case 203:
#line 1150 "parser.y"
    {
         bx_dbg_print_help();
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 204:
#line 1155 "parser.y"
    {
         bx_dbg_print_help();
         free(yyvsp[-1].sval);
         }
    break;

  case 205:
#line 1163 "parser.y"
    {
     bx_dbg_calc_command(yyvsp[-1].ulval);
     free(yyvsp[-2].sval);
     }
    break;

  case 209:
#line 1174 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 213:
#line 1182 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 221:
#line 1194 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 229:
#line 1206 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 230:
#line 1219 "parser.y"
    { yyval.ulval = yyvsp[0].uval; }
    break;

  case 231:
#line 1220 "parser.y"
    { yyval.ulval = yyvsp[0].ulval; }
    break;

  case 232:
#line 1221 "parser.y"
    { yyval.ulval = bx_dbg_get_reg8l_value(yyvsp[0].uval); }
    break;

  case 233:
#line 1222 "parser.y"
    { yyval.ulval = bx_dbg_get_reg8h_value(yyvsp[0].uval); }
    break;

  case 234:
#line 1223 "parser.y"
    { yyval.ulval = bx_dbg_get_reg16_value(yyvsp[0].uval); }
    break;

  case 235:
#line 1224 "parser.y"
    { yyval.ulval = bx_dbg_get_reg32_value(yyvsp[0].uval); }
    break;

  case 236:
#line 1225 "parser.y"
    { yyval.ulval = bx_dbg_get_selector_value(yyvsp[0].uval); }
    break;

  case 237:
#line 1226 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval + yyvsp[0].ulval; }
    break;

  case 238:
#line 1227 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval - yyvsp[0].ulval; }
    break;

  case 239:
#line 1228 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval * yyvsp[0].ulval; }
    break;

  case 240:
#line 1229 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval / yyvsp[0].ulval; }
    break;

  case 241:
#line 1230 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval >> yyvsp[0].ulval; }
    break;

  case 242:
#line 1231 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval << yyvsp[0].ulval; }
    break;

  case 243:
#line 1232 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval | yyvsp[0].ulval; }
    break;

  case 244:
#line 1233 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval ^ yyvsp[0].ulval; }
    break;

  case 245:
#line 1234 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval & yyvsp[0].ulval; }
    break;

  case 246:
#line 1235 "parser.y"
    { yyval.ulval = !yyvsp[0].ulval; }
    break;

  case 247:
#line 1236 "parser.y"
    { yyval.ulval = -yyvsp[0].ulval; }
    break;

  case 248:
#line 1237 "parser.y"
    { yyval.ulval = yyvsp[-1].ulval; }
    break;

  case 249:
#line 1242 "parser.y"
    { yyval.ulval = yyvsp[0].uval; }
    break;

  case 250:
#line 1243 "parser.y"
    { yyval.ulval = yyvsp[0].ulval; }
    break;

  case 251:
#line 1244 "parser.y"
    { yyval.ulval = bx_dbg_get_symbol_value(yyvsp[0].sval); free(yyvsp[0].sval);}
    break;

  case 252:
#line 1245 "parser.y"
    { yyval.ulval = bx_dbg_get_reg8l_value(yyvsp[0].uval); }
    break;

  case 253:
#line 1246 "parser.y"
    { yyval.ulval = bx_dbg_get_reg8h_value(yyvsp[0].uval); }
    break;

  case 254:
#line 1247 "parser.y"
    { yyval.ulval = bx_dbg_get_reg16_value(yyvsp[0].uval); }
    break;

  case 255:
#line 1248 "parser.y"
    { yyval.ulval = bx_dbg_get_reg32_value(yyvsp[0].uval); }
    break;

  case 256:
#line 1249 "parser.y"
    { yyval.ulval = bx_dbg_get_selector_value(yyvsp[0].uval);}
    break;

  case 257:
#line 1250 "parser.y"
    { yyval.ulval = bx_dbg_get_laddr (yyvsp[-2].ulval, yyvsp[0].ulval); }
    break;

  case 258:
#line 1251 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval + yyvsp[0].ulval; }
    break;

  case 259:
#line 1252 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval - yyvsp[0].ulval; }
    break;

  case 260:
#line 1253 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval * yyvsp[0].ulval; }
    break;

  case 261:
#line 1254 "parser.y"
    { yyval.ulval = (yyvsp[0].ulval != 0) ? yyvsp[-2].ulval / yyvsp[0].ulval : 0; }
    break;

  case 262:
#line 1255 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval >> yyvsp[0].ulval; }
    break;

  case 263:
#line 1256 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval << yyvsp[0].ulval; }
    break;

  case 264:
#line 1257 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval | yyvsp[0].ulval; }
    break;

  case 265:
#line 1258 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval ^ yyvsp[0].ulval; }
    break;

  case 266:
#line 1259 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval & yyvsp[0].ulval; }
    break;

  case 267:
#line 1260 "parser.y"
    { yyval.ulval = !yyvsp[0].ulval; }
    break;

  case 268:
#line 1261 "parser.y"
    { yyval.ulval = -yyvsp[0].ulval; }
    break;

  case 269:
#line 1262 "parser.y"
    { yyval.ulval = yyvsp[-1].ulval; }
    break;


    }

/* Line 999 of yacc.c.  */
#line 3367 "y.tab.c"

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


#line 1265 "parser.y"


#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
