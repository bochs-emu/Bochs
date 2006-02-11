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
     BX_TOKEN_IVT = 314,
     BX_TOKEN_GDT = 315,
     BX_TOKEN_LDT = 316,
     BX_TOKEN_TSS = 317,
     BX_TOKEN_TAB = 318,
     BX_TOKEN_DIRTY = 319,
     BX_TOKEN_LINUX = 320,
     BX_TOKEN_CONTROL_REGS = 321,
     BX_TOKEN_EXAMINE = 322,
     BX_TOKEN_XFORMAT = 323,
     BX_TOKEN_DISFORMAT = 324,
     BX_TOKEN_SETPMEM = 325,
     BX_TOKEN_SYMBOLNAME = 326,
     BX_TOKEN_QUERY = 327,
     BX_TOKEN_PENDING = 328,
     BX_TOKEN_TAKE = 329,
     BX_TOKEN_DMA = 330,
     BX_TOKEN_IRQ = 331,
     BX_TOKEN_DUMP_CPU = 332,
     BX_TOKEN_SET_CPU = 333,
     BX_TOKEN_DISASSEMBLE = 334,
     BX_TOKEN_INSTRUMENT = 335,
     BX_TOKEN_START = 336,
     BX_TOKEN_STOP = 337,
     BX_TOKEN_RESET = 338,
     BX_TOKEN_PRINT = 339,
     BX_TOKEN_STRING = 340,
     BX_TOKEN_DOIT = 341,
     BX_TOKEN_CRC = 342,
     BX_TOKEN_TRACE = 343,
     BX_TOKEN_TRACEREG = 344,
     BX_TOKEN_SWITCH_MODE = 345,
     BX_TOKEN_SIZE = 346,
     BX_TOKEN_PTIME = 347,
     BX_TOKEN_TIMEBP_ABSOLUTE = 348,
     BX_TOKEN_TIMEBP = 349,
     BX_TOKEN_RECORD = 350,
     BX_TOKEN_PLAYBACK = 351,
     BX_TOKEN_MODEBP = 352,
     BX_TOKEN_PRINT_STACK = 353,
     BX_TOKEN_WATCH = 354,
     BX_TOKEN_UNWATCH = 355,
     BX_TOKEN_READ = 356,
     BX_TOKEN_WRITE = 357,
     BX_TOKEN_SHOW = 358,
     BX_TOKEN_LOAD_SYMBOLS = 359,
     BX_TOKEN_SYMBOLS = 360,
     BX_TOKEN_LIST_SYMBOLS = 361,
     BX_TOKEN_GLOBAL = 362,
     BX_TOKEN_WHERE = 363,
     BX_TOKEN_PRINT_STRING = 364,
     BX_TOKEN_NUMERIC = 365,
     BX_TOKEN_LONG_NUMERIC = 366,
     BX_TOKEN_NE2000 = 367,
     BX_TOKEN_PIC = 368,
     BX_TOKEN_PAGE = 369,
     BX_TOKEN_HELP = 370,
     BX_TOKEN_CALC = 371,
     BX_TOKEN_VGA = 372,
     BX_TOKEN_RSHIFT = 373,
     BX_TOKEN_LSHIFT = 374,
     NEG = 375,
     NOT = 376
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
#define BX_TOKEN_IVT 314
#define BX_TOKEN_GDT 315
#define BX_TOKEN_LDT 316
#define BX_TOKEN_TSS 317
#define BX_TOKEN_TAB 318
#define BX_TOKEN_DIRTY 319
#define BX_TOKEN_LINUX 320
#define BX_TOKEN_CONTROL_REGS 321
#define BX_TOKEN_EXAMINE 322
#define BX_TOKEN_XFORMAT 323
#define BX_TOKEN_DISFORMAT 324
#define BX_TOKEN_SETPMEM 325
#define BX_TOKEN_SYMBOLNAME 326
#define BX_TOKEN_QUERY 327
#define BX_TOKEN_PENDING 328
#define BX_TOKEN_TAKE 329
#define BX_TOKEN_DMA 330
#define BX_TOKEN_IRQ 331
#define BX_TOKEN_DUMP_CPU 332
#define BX_TOKEN_SET_CPU 333
#define BX_TOKEN_DISASSEMBLE 334
#define BX_TOKEN_INSTRUMENT 335
#define BX_TOKEN_START 336
#define BX_TOKEN_STOP 337
#define BX_TOKEN_RESET 338
#define BX_TOKEN_PRINT 339
#define BX_TOKEN_STRING 340
#define BX_TOKEN_DOIT 341
#define BX_TOKEN_CRC 342
#define BX_TOKEN_TRACE 343
#define BX_TOKEN_TRACEREG 344
#define BX_TOKEN_SWITCH_MODE 345
#define BX_TOKEN_SIZE 346
#define BX_TOKEN_PTIME 347
#define BX_TOKEN_TIMEBP_ABSOLUTE 348
#define BX_TOKEN_TIMEBP 349
#define BX_TOKEN_RECORD 350
#define BX_TOKEN_PLAYBACK 351
#define BX_TOKEN_MODEBP 352
#define BX_TOKEN_PRINT_STACK 353
#define BX_TOKEN_WATCH 354
#define BX_TOKEN_UNWATCH 355
#define BX_TOKEN_READ 356
#define BX_TOKEN_WRITE 357
#define BX_TOKEN_SHOW 358
#define BX_TOKEN_LOAD_SYMBOLS 359
#define BX_TOKEN_SYMBOLS 360
#define BX_TOKEN_LIST_SYMBOLS 361
#define BX_TOKEN_GLOBAL 362
#define BX_TOKEN_WHERE 363
#define BX_TOKEN_PRINT_STRING 364
#define BX_TOKEN_NUMERIC 365
#define BX_TOKEN_LONG_NUMERIC 366
#define BX_TOKEN_NE2000 367
#define BX_TOKEN_PIC 368
#define BX_TOKEN_PAGE 369
#define BX_TOKEN_HELP 370
#define BX_TOKEN_CALC 371
#define BX_TOKEN_VGA 372
#define BX_TOKEN_RSHIFT 373
#define BX_TOKEN_LSHIFT 374
#define NEG 375
#define NOT 376




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
#line 348 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 360 "y.tab.c"

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
#define YYFINAL  271
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1340

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  135
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  52
/* YYNRULES -- Number of rules. */
#define YYNRULES  257
/* YYNRULES -- Number of states. */
#define YYNSTATES  470

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   376

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     129,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   132,     2,     2,     2,     2,   126,     2,
     133,   134,   124,   120,     2,   121,     2,   125,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   131,     2,
       2,   130,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   123,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   122,     2,     2,     2,     2,     2,
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
     115,   116,   117,   118,   119,   127,   128
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
      80,    82,    84,    85,    87,    89,    91,    93,    95,    97,
      99,   101,   103,   105,   107,   109,   111,   115,   119,   123,
     127,   130,   134,   137,   140,   144,   148,   151,   155,   159,
     163,   166,   169,   174,   179,   184,   189,   193,   198,   204,
     207,   211,   214,   217,   221,   224,   229,   235,   241,   247,
     253,   259,   262,   268,   271,   275,   279,   282,   286,   291,
     294,   297,   301,   305,   309,   313,   317,   321,   325,   329,
     334,   339,   344,   349,   354,   358,   362,   366,   370,   375,
     379,   383,   389,   397,   401,   405,   406,   408,   410,   413,
     416,   419,   423,   427,   431,   434,   439,   443,   447,   450,
     456,   460,   464,   469,   473,   476,   480,   485,   489,   495,
     499,   503,   508,   512,   516,   520,   524,   528,   532,   536,
     540,   544,   548,   552,   556,   560,   564,   568,   572,   576,
     580,   584,   588,   592,   596,   600,   604,   608,   612,   616,
     620,   624,   628,   632,   636,   640,   644,   648,   652,   655,
     659,   661,   663,   665,   667,   669,   671,   673,   675,   677,
     679,   681,   683,   685,   687,   689,   691,   693,   695,   697,
     699,   701,   703,   705,   707,   709,   711,   713,   715,   717,
     719,   721,   723,   725,   729,   733,   737,   741,   745,   749,
     753,   757,   761,   764,   767,   771,   773,   775,   777,   779,
     781,   783,   785,   787,   789,   791,   795,   799,   803,   807,
     811,   815,   819,   823,   827,   831,   834,   837
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     136,     0,    -1,   136,   137,    -1,   137,    -1,   154,    -1,
     155,    -1,   156,    -1,   157,    -1,   158,    -1,   161,    -1,
     164,    -1,   159,    -1,   160,    -1,   165,    -1,   166,    -1,
     167,    -1,   168,    -1,   169,    -1,   170,    -1,   171,    -1,
     172,    -1,   173,    -1,   174,    -1,   175,    -1,   176,    -1,
     177,    -1,   178,    -1,   147,    -1,   148,    -1,   146,    -1,
     141,    -1,   142,    -1,   143,    -1,   144,    -1,   149,    -1,
     150,    -1,   145,    -1,   151,    -1,   152,    -1,   153,    -1,
     179,    -1,   180,    -1,    -1,   129,    -1,    36,    -1,    37,
      -1,    81,    -1,    82,    -1,    83,    -1,    84,    -1,    29,
      -1,    30,    -1,    31,    -1,    32,    -1,    33,    -1,    34,
      -1,    94,   111,   129,    -1,    93,   111,   129,    -1,    95,
      85,   129,    -1,    96,    85,   129,    -1,    97,   129,    -1,
     103,    85,   129,    -1,   103,   129,    -1,    92,   129,    -1,
      88,   138,   129,    -1,    89,   138,   129,    -1,    98,   129,
      -1,    98,   110,   129,    -1,    99,    82,   129,    -1,    99,
      38,   129,    -1,    99,   129,    -1,   100,   129,    -1,    99,
     101,   110,   129,    -1,   100,   101,   110,   129,    -1,    99,
     102,   110,   129,    -1,   100,   102,   110,   129,    -1,   104,
      85,   129,    -1,   104,    85,   110,   129,    -1,   104,   107,
      85,   110,   129,    -1,   108,   129,    -1,   109,   110,   129,
      -1,    38,   129,    -1,    39,   129,    -1,    39,   110,   129,
      -1,    40,   129,    -1,    42,    79,   138,   129,    -1,    42,
      71,   130,   110,   129,    -1,    42,   182,   130,   186,   129,
      -1,    42,   181,   130,   186,   129,    -1,    42,   183,   130,
     186,   129,    -1,    42,   184,   130,   186,   129,    -1,    45,
     129,    -1,    45,   185,   131,   186,   129,    -1,    46,   129,
      -1,    46,   186,   129,    -1,    46,    85,   129,    -1,    47,
     129,    -1,    47,   186,   129,    -1,    47,   124,   186,   129,
      -1,    44,   129,    -1,   106,   129,    -1,   106,    85,   129,
      -1,    51,    47,   129,    -1,    51,    54,   129,    -1,    51,
      53,   129,    -1,    51,    55,   129,    -1,    51,    56,   129,
      -1,    51,    57,   129,    -1,    51,    64,   129,    -1,    51,
      58,   162,   129,    -1,    51,    59,   162,   129,    -1,    51,
      60,   162,   129,    -1,    51,    61,   162,   129,    -1,    51,
      62,   162,   129,    -1,    51,    63,   129,    -1,    51,    35,
     129,    -1,    51,    65,   129,    -1,    51,   105,   129,    -1,
      51,   105,    85,   129,    -1,    51,    66,   129,    -1,    51,
     112,   129,    -1,    51,   112,   114,   110,   129,    -1,    51,
     112,   114,   110,    53,   110,   129,    -1,    51,   113,   129,
      -1,    51,   117,   129,    -1,    -1,   163,    -1,   186,    -1,
     186,   186,    -1,    53,   129,    -1,    77,   129,    -1,    48,
     110,   129,    -1,    49,   110,   129,    -1,    50,   110,   129,
      -1,    52,   129,    -1,    67,    68,   186,   129,    -1,    67,
      68,   129,    -1,    67,   186,   129,    -1,    67,   129,    -1,
      70,   110,   110,   110,   129,    -1,    72,    73,   129,    -1,
      74,    75,   129,    -1,    74,    75,   110,   129,    -1,    74,
      76,   129,    -1,    78,   129,    -1,    79,   162,   129,    -1,
      79,    69,   162,   129,    -1,    79,    90,   129,    -1,    79,
      91,   130,   110,   129,    -1,    80,   139,   129,    -1,    86,
     110,   129,    -1,    87,   110,   110,   129,    -1,   115,    52,
     129,    -1,   115,    38,   129,    -1,   115,    39,   129,    -1,
     115,    40,   129,    -1,   115,    45,   129,    -1,   115,    46,
     129,    -1,   115,    48,   129,    -1,   115,    49,   129,    -1,
     115,    50,   129,    -1,   115,    44,   129,    -1,   115,    97,
     129,    -1,   115,    87,   129,    -1,   115,    88,   129,    -1,
     115,    89,   129,    -1,   115,    77,   129,    -1,   115,    78,
     129,    -1,   115,    92,   129,    -1,   115,    94,   129,    -1,
     115,    93,   129,    -1,   115,    98,   129,    -1,   115,    95,
     129,    -1,   115,    96,   129,    -1,   115,   104,   129,    -1,
     115,   106,   129,    -1,   115,    53,   129,    -1,   115,    70,
     129,    -1,   115,    79,   129,    -1,   115,    99,   129,    -1,
     115,   100,   129,    -1,   115,    67,   129,    -1,   115,    80,
     129,    -1,   115,    42,   129,    -1,   115,    51,   129,    -1,
     115,   103,   129,    -1,   115,   116,   129,    -1,   115,   115,
     129,    -1,   115,   129,    -1,   116,   186,   129,    -1,     7,
      -1,     8,    -1,     9,    -1,    10,    -1,     3,    -1,     4,
      -1,     5,    -1,     6,    -1,    11,    -1,    12,    -1,    13,
      -1,    14,    -1,    19,    -1,    20,    -1,    21,    -1,    22,
      -1,    15,    -1,    16,    -1,    17,    -1,    18,    -1,    24,
      -1,    25,    -1,    26,    -1,    27,    -1,   110,    -1,   111,
      -1,   182,    -1,   181,    -1,   183,    -1,   184,    -1,   140,
      -1,    23,    -1,    28,    -1,   185,   120,   185,    -1,   185,
     121,   185,    -1,   185,   124,   185,    -1,   185,   125,   185,
      -1,   185,   118,   185,    -1,   185,   119,   185,    -1,   185,
     122,   185,    -1,   185,   123,   185,    -1,   185,   126,   185,
      -1,   132,   185,    -1,   121,   185,    -1,   133,   185,   134,
      -1,   110,    -1,   111,    -1,    85,    -1,   182,    -1,   181,
      -1,   183,    -1,   184,    -1,   140,    -1,    23,    -1,    28,
      -1,   186,   131,   186,    -1,   186,   120,   186,    -1,   186,
     121,   186,    -1,   186,   124,   186,    -1,   186,   125,   186,
      -1,   186,   118,   186,    -1,   186,   119,   186,    -1,   186,
     122,   186,    -1,   186,   123,   186,    -1,   186,   126,   186,
      -1,   132,   186,    -1,   121,   186,    -1,   133,   186,   134,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   167,   167,   168,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   217,   218,   223,   224,   225,   226,
     231,   232,   233,   234,   235,   236,   241,   246,   254,   262,
     270,   278,   283,   291,   299,   307,   315,   320,   328,   334,
     340,   345,   350,   355,   360,   365,   373,   378,   383,   391,
     399,   407,   415,   420,   428,   436,   441,   446,   450,   454,
     458,   465,   470,   475,   480,   485,   490,   495,   500,   508,
     516,   521,   529,   534,   539,   544,   549,   554,   559,   564,
     569,   574,   579,   584,   589,   594,   599,   604,   609,   614,
     619,   624,   629,   634,   639,   651,   652,   655,   659,   665,
     673,   681,   689,   696,   704,   712,   717,   722,   727,   735,
     743,   751,   756,   761,   769,   777,   782,   787,   792,   800,
     808,   816,   824,   829,   834,   839,   844,   849,   854,   859,
     864,   869,   874,   879,   884,   890,   896,   901,   906,   911,
     916,   921,   926,   931,   936,   941,   946,   951,   956,   965,
     974,   981,   994,  1002,  1012,  1033,  1046,  1056,  1061,  1069,
    1077,  1078,  1079,  1080,  1085,  1086,  1087,  1088,  1093,  1094,
    1095,  1096,  1097,  1098,  1099,  1100,  1105,  1106,  1107,  1108,
    1109,  1110,  1111,  1112,  1126,  1127,  1128,  1129,  1130,  1131,
    1132,  1133,  1134,  1135,  1136,  1137,  1138,  1139,  1140,  1141,
    1142,  1143,  1144,  1145,  1146,  1151,  1152,  1153,  1154,  1155,
    1156,  1157,  1158,  1159,  1160,  1161,  1162,  1163,  1164,  1165,
    1166,  1167,  1168,  1169,  1170,  1171,  1172,  1173
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
  "BX_TOKEN_ALL", "BX_TOKEN_IDT", "BX_TOKEN_IVT", "BX_TOKEN_GDT", 
  "BX_TOKEN_LDT", "BX_TOKEN_TSS", "BX_TOKEN_TAB", "BX_TOKEN_DIRTY", 
  "BX_TOKEN_LINUX", "BX_TOKEN_CONTROL_REGS", "BX_TOKEN_EXAMINE", 
  "BX_TOKEN_XFORMAT", "BX_TOKEN_DISFORMAT", "BX_TOKEN_SETPMEM", 
  "BX_TOKEN_SYMBOLNAME", "BX_TOKEN_QUERY", "BX_TOKEN_PENDING", 
  "BX_TOKEN_TAKE", "BX_TOKEN_DMA", "BX_TOKEN_IRQ", "BX_TOKEN_DUMP_CPU", 
  "BX_TOKEN_SET_CPU", "BX_TOKEN_DISASSEMBLE", "BX_TOKEN_INSTRUMENT", 
  "BX_TOKEN_START", "BX_TOKEN_STOP", "BX_TOKEN_RESET", "BX_TOKEN_PRINT", 
  "BX_TOKEN_STRING", "BX_TOKEN_DOIT", "BX_TOKEN_CRC", "BX_TOKEN_TRACE", 
  "BX_TOKEN_TRACEREG", "BX_TOKEN_SWITCH_MODE", "BX_TOKEN_SIZE", 
  "BX_TOKEN_PTIME", "BX_TOKEN_TIMEBP_ABSOLUTE", "BX_TOKEN_TIMEBP", 
  "BX_TOKEN_RECORD", "BX_TOKEN_PLAYBACK", "BX_TOKEN_MODEBP", 
  "BX_TOKEN_PRINT_STACK", "BX_TOKEN_WATCH", "BX_TOKEN_UNWATCH", 
  "BX_TOKEN_READ", "BX_TOKEN_WRITE", "BX_TOKEN_SHOW", 
  "BX_TOKEN_LOAD_SYMBOLS", "BX_TOKEN_SYMBOLS", "BX_TOKEN_LIST_SYMBOLS", 
  "BX_TOKEN_GLOBAL", "BX_TOKEN_WHERE", "BX_TOKEN_PRINT_STRING", 
  "BX_TOKEN_NUMERIC", "BX_TOKEN_LONG_NUMERIC", "BX_TOKEN_NE2000", 
  "BX_TOKEN_PIC", "BX_TOKEN_PAGE", "BX_TOKEN_HELP", "BX_TOKEN_CALC", 
  "BX_TOKEN_VGA", "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT", "'+'", "'-'", 
  "'|'", "'^'", "'*'", "'/'", "'&'", "NEG", "NOT", "'\\n'", "'='", "':'", 
  "'!'", "'('", "')'", "$accept", "commands", "command", 
  "BX_TOKEN_TOGGLE_ON_OFF", "BX_TOKEN_INSTRUMENT_COMMAND", 
  "BX_TOKEN_SEGREG", "timebp_command", "record_command", 
  "playback_command", "modebp_command", "show_command", "ptime_command", 
  "trace_command", "trace_reg_command", "print_stack_command", 
  "watch_point_command", "symbol_command", "where_command", 
  "print_string_command", "continue_command", "stepN_command", 
  "step_over_command", "set_command", "breakpoint_command", 
  "blist_command", "slist_command", "info_command", 
  "optional_numeric_range", "numeric_range", "regs_command", 
  "dump_cpu_command", "delete_command", "bpe_command", "bpd_command", 
  "quit_command", "examine_command", "setpmem_command", "query_command", 
  "take_command", "set_cpu_command", "disassemble_command", 
  "instrument_command", "doit_command", "crc_command", "help_command", 
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
      43,    45,   124,    94,    42,    47,    38,   375,   376,    10,
      61,    58,    33,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   135,   136,   136,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   138,   138,   139,   139,   139,   139,
     140,   140,   140,   140,   140,   140,   141,   141,   142,   143,
     144,   145,   145,   146,   147,   148,   149,   149,   150,   150,
     150,   150,   150,   150,   150,   150,   151,   151,   151,   152,
     153,   154,   155,   155,   156,   157,   157,   157,   157,   157,
     157,   158,   158,   158,   158,   158,   158,   158,   158,   159,
     160,   160,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   162,   162,   163,   163,   164,
     165,   166,   167,   168,   169,   170,   170,   170,   170,   171,
     172,   173,   173,   173,   174,   175,   175,   175,   175,   176,
     177,   178,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   180,
     181,   181,   181,   181,   182,   182,   182,   182,   183,   183,
     183,   183,   183,   183,   183,   183,   184,   184,   184,   184,
     184,   184,   184,   184,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       2,     3,     2,     2,     3,     3,     2,     3,     3,     3,
       2,     2,     4,     4,     4,     4,     3,     4,     5,     2,
       3,     2,     2,     3,     2,     4,     5,     5,     5,     5,
       5,     2,     5,     2,     3,     3,     2,     3,     4,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     4,
       4,     4,     4,     4,     3,     3,     3,     3,     4,     3,
       3,     5,     7,     3,     3,     0,     1,     1,     2,     2,
       2,     3,     3,     3,     2,     4,     3,     3,     2,     5,
       3,     3,     4,     3,     2,     3,     4,     3,     5,     3,
       3,     4,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
      42,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   125,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,     0,     3,    30,    31,    32,    33,
      36,    29,    27,    28,    34,    35,    37,    38,    39,     4,
       5,     6,     7,     8,    11,    12,     9,    10,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    40,    41,    81,     0,    82,    84,   194,   195,
     196,   197,   190,   191,   192,   193,   198,   199,   200,   201,
     206,   207,   208,   209,   202,   203,   204,   205,   210,   211,
     212,   213,     0,     0,     0,     0,     0,     0,    99,   221,
     222,    50,    51,    52,    53,    54,    55,   214,   215,     0,
      91,     0,     0,   220,   217,   216,   218,   219,     0,   243,
     244,   237,   235,   236,     0,    93,     0,     0,   242,   239,
     238,   240,   241,     0,   237,     0,    96,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   125,   125,
     125,   125,   125,     0,     0,     0,     0,     0,     0,     0,
       0,   134,   129,     0,   138,     0,     0,     0,     0,     0,
     130,   144,   125,     0,     0,     0,   126,   127,    46,    47,
      48,    49,     0,     0,     0,    44,    45,     0,     0,    63,
       0,     0,     0,     0,    60,     0,    66,     0,     0,     0,
       0,    70,     0,     0,    71,     0,    62,     0,     0,     0,
     100,    79,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   188,
       0,     1,     2,    83,     0,     0,     0,     0,     0,     0,
     233,   232,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    95,   256,   255,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    94,     0,     0,    97,
     131,   132,   133,   115,   102,   104,   103,   105,   106,   107,
       0,     0,     0,     0,     0,   114,   108,   116,   119,     0,
     117,     0,   120,   123,   124,   136,     0,   137,     0,   140,
       0,   141,   143,     0,   147,     0,   145,     0,   128,   149,
     150,     0,    64,    65,    57,    56,    58,    59,    67,    69,
      68,     0,     0,     0,     0,    61,     0,    76,     0,   101,
      80,   153,   154,   155,   183,   161,   156,   157,   158,   159,
     160,   184,   152,   176,   181,   177,   166,   167,   178,   182,
     163,   164,   165,   168,   170,   169,   172,   173,   162,   171,
     179,   180,   185,   174,   175,   187,   186,   189,     0,    85,
       0,     0,     0,     0,   234,   227,   228,   223,   224,   229,
     230,   225,   226,   231,     0,   257,   250,   251,   246,   247,
     252,   253,   248,   249,   254,   245,    98,   109,   110,   111,
     112,   113,   118,     0,   135,     0,   142,   146,     0,   247,
     151,    72,    74,    73,    75,    77,     0,    86,    88,    87,
      89,    90,    92,     0,   121,   139,   148,    78,     0,   122
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    44,    45,   207,   202,   148,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,   195,   196,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,   149,   150,   151,   152,
     138,   197
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -157
static const short yypact[] =
{
    1052,  -120,  -100,  -106,  1261,   -78,   707,   544,   381,   -57,
     -47,   -39,   391,   -56,   -52,   512,   -30,    -1,   -55,   -46,
     -45,   349,   -38,   -28,   -25,    28,    28,   -43,     9,    10,
      37,    39,     5,   -99,    41,   -95,   -82,   -68,   -81,    15,
      25,  1134,   838,  -157,   966,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,    16,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,     8,    28,    21,    22,    23,    24,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,   870,
    -157,   870,   870,  -157,  -157,  -157,  -157,  -157,   657,  -157,
    -157,    17,  -157,  -157,   838,  -157,   838,   838,  -157,  -157,
    -157,  -157,  -157,   100,  -157,   838,  -157,   114,    26,    31,
      32,    33,    34,    35,    42,    43,    44,    45,   838,   838,
     838,   838,   838,    59,    64,    67,    68,   -80,   -88,    69,
      70,  -157,  -157,   675,  -157,   128,    47,    71,   -92,    72,
    -157,  -157,   838,    82,    83,    85,  -157,    84,  -157,  -157,
    -157,  -157,    98,    99,    56,  -157,  -157,   101,   112,  -157,
     113,   115,   129,   131,  -157,   135,  -157,   136,   138,   102,
     156,  -157,   158,   172,  -157,   154,  -157,   -91,    51,   159,
    -157,  -157,   160,   170,   199,   209,   210,   212,   213,   287,
     288,   290,   291,   292,   293,   294,   295,   296,   298,   299,
     300,   301,   302,   303,   304,   306,   307,   308,   312,   313,
     314,   329,   332,   333,   334,   335,   336,   338,   339,  -157,
     155,  -157,  -157,  -157,   177,   340,   838,   838,   838,   838,
    -157,  -157,     7,   870,   870,   870,   870,   870,   870,   870,
     870,   870,   838,  -157,   353,   353,   -64,   838,   838,   838,
     838,   838,   838,   838,   838,   838,  -157,   838,   354,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
     342,   357,   358,   359,   360,  -157,  -157,  -157,  -157,   361,
    -157,   383,  -157,  -157,  -157,  -157,   463,  -157,   384,  -157,
     366,  -157,  -157,   368,  -157,   388,  -157,   838,   701,  -157,
    -157,   370,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
    -157,   371,   372,   377,   378,  -157,   380,  -157,   401,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,   450,  -157,
     480,   495,   538,   624,  -157,  -157,  -157,   137,   137,   137,
     137,  -157,  -157,  -157,   643,  -157,   353,   353,   -50,   -50,
     -50,   -50,   353,   353,   353,   701,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,   -51,  -157,   461,  -157,  -157,   462,   -50,
    -157,  -157,  -157,  -157,  -157,  -157,   464,  -157,  -157,  -157,
    -157,  -157,  -157,   402,  -157,  -157,  -157,  -157,   466,  -157
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -157,  -157,   552,     6,  -157,    60,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -156,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,    18,    27,    36,    46,
    -104,    -7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned short yytable[] =
{
     153,   157,   463,   225,   229,   329,   222,   223,   185,    84,
      85,   215,   320,   321,   322,   323,   324,   227,   340,   366,
     188,   189,   114,    87,   134,   280,   331,   281,   282,    86,
     216,   115,   208,   135,   224,   270,   343,   341,   367,   228,
     116,   332,   136,   198,   199,   200,   201,   226,   230,   330,
     117,   118,   137,   158,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   159,   205,   206,   133,   307,   297,   298,
     425,   160,   187,   181,   303,   304,   305,   182,   464,   217,
     186,   307,   203,   190,   191,   204,   209,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   139,   108,   109,
     110,   111,   140,   121,   122,   123,   124,   125,   126,   275,
     210,   211,   212,   218,   213,   283,   284,   285,   286,   287,
     288,   289,   290,   291,   214,   232,   368,   294,   274,   295,
     296,   414,   219,   220,   231,   273,   293,   134,   308,   134,
     134,   276,   277,   278,   279,   310,   135,   338,   135,   135,
     311,   312,   313,   314,   315,   136,   351,   136,   136,   154,
     221,   316,   317,   318,   319,   137,   336,   137,   137,   415,
     416,   417,   418,   419,   420,   421,   422,   423,   325,   133,
     348,   133,   133,   326,   142,   143,   327,   328,   333,   334,
     339,   342,   297,   298,   299,   347,   301,   302,   303,   304,
     305,   344,   361,   345,   346,   307,   146,   147,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   349,   350,   306,
     352,   307,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   353,   354,   309,   355,   307,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   283,   284,   337,   356,   307,
     357,   289,   290,   291,   358,   359,   362,   360,   363,   410,
     411,   412,   413,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   364,   365,   407,   424,   307,   408,   369,   370,
     426,   427,   428,   429,   430,   431,   432,   433,   434,   371,
     435,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   372,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   373,   374,
     449,   375,   376,   133,   133,   133,   133,   133,   133,   133,
     133,   133,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   139,   108,   109,   110,   111,   140,   121,   122,
     123,   124,   125,   126,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   139,   108,   109,   110,   111,   140,
     121,   122,   123,   124,   125,   126,   377,   378,   192,   379,
     380,   381,   382,   383,   384,   385,   161,   386,   387,   388,
     389,   390,   391,   392,   154,   393,   394,   395,   162,   193,
     194,   396,   397,   398,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   399,   142,
     143,   400,   401,   402,   403,   404,   154,   405,   406,   409,
     144,   437,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   146,   147,   436,   307,   307,   438,   439,   440,   441,
     442,   142,   143,   443,   445,   446,   177,   447,   448,   450,
     451,   452,   144,   178,   179,   155,   453,   454,   180,   455,
     156,   456,   468,   146,   147,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   139,   108,   109,   110,   111,
     140,   121,   122,   123,   124,   125,   126,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   139,   108,   109,
     110,   111,   140,   121,   122,   123,   124,   125,   126,   457,
     183,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     465,   466,   444,   467,   307,   469,   272,   154,   297,   298,
     299,   300,   301,   302,   303,   304,   305,     0,     0,   458,
       0,   307,     0,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   142,   143,   459,     0,   307,     0,     0,   141,
       0,     0,     0,   144,     0,     0,     0,     0,     0,     0,
       0,   184,     0,     0,   146,   147,     0,     0,     0,     0,
       0,     0,     0,     0,   142,   143,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   144,     0,   460,     0,   307,
       0,     0,     0,   145,     0,     0,   146,   147,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   139,   108,
     109,   110,   111,   140,   121,   122,   123,   124,   125,   126,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     119,   108,   109,   110,   111,   120,   121,   122,   123,   124,
     125,   126,   297,   298,   299,   300,   301,   302,   303,   304,
     305,     0,     0,   461,     0,   307,     0,     0,     0,     0,
     154,   297,   298,   299,   300,   301,   302,   303,   304,   305,
       0,     0,   462,     0,   307,   283,   284,   285,   286,   287,
     288,   289,   290,   291,     0,   142,   143,     0,   292,     0,
       0,     0,     0,     0,     0,     0,   144,     0,     0,     0,
       0,     0,     0,     0,   335,     0,     0,   146,   147,     0,
       0,     0,     0,     0,     0,     0,     0,   127,   128,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   129,     0,
       0,     0,   307,     0,     0,     0,   130,     0,     0,   131,
     132,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   139,   108,   109,   110,   111,   140,   121,   122,   123,
     124,   125,   126,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   119,   108,   109,   110,   111,   120,   121,
     122,   123,   124,   125,   126,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   154,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   142,   143,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   144,
       0,     0,     0,     0,     0,     0,   271,     0,     0,     0,
     146,   147,     0,     0,     0,     0,     0,     0,     0,     0,
     127,   128,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   129,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   131,   132,     1,     2,     3,     0,     4,     0,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15,     0,     0,    16,     0,    17,     0,
      18,     0,     0,    19,    20,    21,    22,     0,     0,     0,
       0,     0,    23,    24,    25,    26,     0,     0,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
      37,     0,    38,     0,    39,    40,     0,     0,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,     0,
       1,     2,     3,     0,     4,    43,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    15,
       0,     0,    16,     0,    17,     0,    18,     0,     0,    19,
      20,    21,    22,     0,     0,     0,     0,     0,    23,    24,
      25,    26,     0,     0,    27,    28,    29,    30,    31,    32,
      33,    34,    35,     0,     0,    36,    37,     0,    38,     0,
      39,    40,     0,     0,     0,     0,     0,    41,    42,     0,
       0,     0,   233,   234,   235,     0,   236,     0,   237,   238,
     239,    43,   240,   241,   242,   243,   244,   245,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   246,     0,     0,   247,     0,     0,     0,     0,     0,
       0,   248,   249,   250,   251,     0,     0,     0,     0,     0,
       0,   252,   253,   254,     0,     0,   255,   256,   257,   258,
     259,   260,   261,   262,   263,     0,     0,   264,   265,     0,
     266,     0,     0,     0,     0,     0,     0,     0,     0,   267,
     268,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   269,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,     0,   108,   109,   110,   111,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   112,     0,     0,     0,     0,     0,     0,     0,
     113
};

static const short yycheck[] =
{
       7,     8,    53,    85,    85,    85,   101,   102,    15,   129,
     110,   110,   168,   169,   170,   171,   172,    85,   110,   110,
      75,    76,     4,   129,     6,   129,   114,   131,   132,   129,
     129,     4,    26,     6,   129,    42,   192,   129,   129,   107,
       4,   129,     6,    81,    82,    83,    84,   129,   129,   129,
       4,   129,     6,   110,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   110,    36,    37,     6,   131,   118,   119,
     134,   110,    73,   129,   124,   125,   126,   129,   129,    38,
     110,   131,   110,   129,   129,   110,   129,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,   113,
     111,   111,    85,    82,    85,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   129,   110,    85,   144,   130,   146,
     147,   134,   101,   102,   129,   129,   129,   129,   155,   131,
     132,   130,   130,   130,   130,   129,   129,   110,   131,   132,
     129,   129,   129,   129,   129,   129,   110,   131,   132,    85,
     129,   129,   129,   129,   129,   129,   183,   131,   132,   283,
     284,   285,   286,   287,   288,   289,   290,   291,   129,   129,
     197,   131,   132,   129,   110,   111,   129,   129,   129,   129,
     129,   129,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   129,   110,   130,   129,   131,   132,   133,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   129,   129,   129,
     129,   131,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   129,   129,   129,   129,   131,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   118,   119,   129,   129,   131,
     129,   124,   125,   126,   129,   129,   110,   129,   110,   276,
     277,   278,   279,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   110,   129,   129,   292,   131,   110,   129,   129,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   129,
     307,   283,   284,   285,   286,   287,   288,   289,   290,   291,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   283,
     284,   285,   286,   287,   288,   289,   290,   291,   129,   283,
     284,   285,   286,   287,   288,   289,   290,   291,   129,   129,
     347,   129,   129,   283,   284,   285,   286,   287,   288,   289,
     290,   291,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,   129,   129,    69,   129,
     129,   129,   129,   129,   129,   129,    35,   129,   129,   129,
     129,   129,   129,   129,    85,   129,   129,   129,    47,    90,
      91,   129,   129,   129,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,   129,   110,
     111,   129,   129,   129,   129,   129,    85,   129,   129,   129,
     121,   129,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   132,   133,   129,   131,   131,   129,   129,   129,   129,
     129,   110,   111,   110,   110,   129,   105,   129,   110,   129,
     129,   129,   121,   112,   113,   124,   129,   129,   117,   129,
     129,   110,   110,   132,   133,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,   129,
      68,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     129,   129,   129,   129,   131,   129,    44,    85,   118,   119,
     120,   121,   122,   123,   124,   125,   126,    -1,    -1,   129,
      -1,   131,    -1,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   110,   111,   129,    -1,   131,    -1,    -1,    85,
      -1,    -1,    -1,   121,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   129,    -1,    -1,   132,   133,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   110,   111,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   121,    -1,   129,    -1,   131,
      -1,    -1,    -1,   129,    -1,    -1,   132,   133,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,   118,   119,   120,   121,   122,   123,   124,   125,
     126,    -1,    -1,   129,    -1,   131,    -1,    -1,    -1,    -1,
      85,   118,   119,   120,   121,   122,   123,   124,   125,   126,
      -1,    -1,   129,    -1,   131,   118,   119,   120,   121,   122,
     123,   124,   125,   126,    -1,   110,   111,    -1,   131,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   129,    -1,    -1,   132,   133,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,   111,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   121,    -1,
      -1,    -1,   131,    -1,    -1,    -1,   129,    -1,    -1,   132,
     133,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    85,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,   111,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   121,
      -1,    -1,    -1,    -1,    -1,    -1,     0,    -1,    -1,    -1,
     132,   133,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     110,   111,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   121,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   132,   133,    38,    39,    40,    -1,    42,    -1,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    -1,    -1,    70,    -1,    72,    -1,
      74,    -1,    -1,    77,    78,    79,    80,    -1,    -1,    -1,
      -1,    -1,    86,    87,    88,    89,    -1,    -1,    92,    93,
      94,    95,    96,    97,    98,    99,   100,    -1,    -1,   103,
     104,    -1,   106,    -1,   108,   109,    -1,    -1,    -1,    -1,
      -1,   115,   116,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    39,    40,    -1,    42,   129,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      -1,    -1,    70,    -1,    72,    -1,    74,    -1,    -1,    77,
      78,    79,    80,    -1,    -1,    -1,    -1,    -1,    86,    87,
      88,    89,    -1,    -1,    92,    93,    94,    95,    96,    97,
      98,    99,   100,    -1,    -1,   103,   104,    -1,   106,    -1,
     108,   109,    -1,    -1,    -1,    -1,    -1,   115,   116,    -1,
      -1,    -1,    38,    39,    40,    -1,    42,    -1,    44,    45,
      46,   129,    48,    49,    50,    51,    52,    53,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,
      -1,    77,    78,    79,    80,    -1,    -1,    -1,    -1,    -1,
      -1,    87,    88,    89,    -1,    -1,    92,    93,    94,    95,
      96,    97,    98,    99,   100,    -1,    -1,   103,   104,    -1,
     106,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   115,
     116,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   129,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    -1,    24,    25,    26,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      79
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    38,    39,    40,    42,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    67,    70,    72,    74,    77,
      78,    79,    80,    86,    87,    88,    89,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   103,   104,   106,   108,
     109,   115,   116,   129,   136,   137,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   129,   110,   129,   129,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    24,    25,
      26,    27,    71,    79,   181,   182,   183,   184,   129,    23,
      28,    29,    30,    31,    32,    33,    34,   110,   111,   121,
     129,   132,   133,   140,   181,   182,   183,   184,   185,    23,
      28,    85,   110,   111,   121,   129,   132,   133,   140,   181,
     182,   183,   184,   186,    85,   124,   129,   186,   110,   110,
     110,    35,    47,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,   105,   112,   113,
     117,   129,   129,    68,   129,   186,   110,    73,    75,    76,
     129,   129,    69,    90,    91,   162,   163,   186,    81,    82,
      83,    84,   139,   110,   110,    36,    37,   138,   138,   129,
     111,   111,    85,    85,   129,   110,   129,    38,    82,   101,
     102,   129,   101,   102,   129,    85,   129,    85,   107,    85,
     129,   129,   110,    38,    39,    40,    42,    44,    45,    46,
      48,    49,    50,    51,    52,    53,    67,    70,    77,    78,
      79,    80,    87,    88,    89,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   103,   104,   106,   115,   116,   129,
     186,     0,   137,   129,   130,   138,   130,   130,   130,   130,
     185,   185,   185,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   131,   129,   186,   186,   186,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   129,   131,   186,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     162,   162,   162,   162,   162,   129,   129,   129,   129,    85,
     129,   114,   129,   129,   129,   129,   186,   129,   110,   129,
     110,   129,   129,   162,   129,   130,   129,   121,   186,   129,
     129,   110,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   110,   110,   110,   110,   129,   110,   129,    85,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   110,   129,
     186,   186,   186,   186,   134,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   186,   134,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   129,   129,   129,   129,
     129,   129,   129,   110,   129,   110,   129,   129,   110,   186,
     129,   129,   129,   129,   129,   129,   110,   129,   129,   129,
     129,   129,   129,    53,   129,   129,   129,   129,   110,   129
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
        case 43:
#line 212 "parser.y"
    {
      }
    break;

  case 45:
#line 219 "parser.y"
    { yyval.bval=yyvsp[0].bval; }
    break;

  case 49:
#line 227 "parser.y"
    { yyval.sval=yyvsp[0].sval; }
    break;

  case 55:
#line 237 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 56:
#line 242 "parser.y"
    {
        bx_dbg_timebp_command(0, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
    break;

  case 57:
#line 247 "parser.y"
    {
        bx_dbg_timebp_command(1, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
    break;

  case 58:
#line 255 "parser.y"
    {
          bx_dbg_record_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 59:
#line 263 "parser.y"
    {
          bx_dbg_playback_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 60:
#line 271 "parser.y"
    {
          bx_dbg_modebp_command();
          free(yyvsp[-1].sval);
          }
    break;

  case 61:
#line 279 "parser.y"
    {
          bx_dbg_show_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 62:
#line 284 "parser.y"
    {
          bx_dbg_show_command(0);
          free(yyvsp[-1].sval);
          }
    break;

  case 63:
#line 292 "parser.y"
    {
        bx_dbg_ptime_command();
        free(yyvsp[-1].sval);
	}
    break;

  case 64:
#line 300 "parser.y"
    {
        bx_dbg_trace_command(yyvsp[-1].bval);
        free(yyvsp[-2].sval);
	}
    break;

  case 65:
#line 308 "parser.y"
    {
	bx_dbg_trace_reg_command(yyvsp[-1].bval);
	free(yyvsp[-2].sval);
	}
    break;

  case 66:
#line 316 "parser.y"
    {
          bx_dbg_print_stack_command(16);
          free(yyvsp[-1].sval);
	  }
    break;

  case 67:
#line 321 "parser.y"
    {
          bx_dbg_print_stack_command(yyvsp[-1].uval);
          free(yyvsp[-2].sval);
	  }
    break;

  case 68:
#line 329 "parser.y"
    {
          watchpoint_continue = 0;
	  dbg_printf("Will stop on watch points\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 69:
#line 335 "parser.y"
    {
          watchpoint_continue = 1;
          dbg_printf("Will not stop on watch points (they will still be logged)\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 70:
#line 341 "parser.y"
    {
          bx_dbg_watch(-1, 0);
          free(yyvsp[-1].sval);
          }
    break;

  case 71:
#line 346 "parser.y"
    {
          bx_dbg_unwatch(-1, 0);
          free(yyvsp[-1].sval);
          }
    break;

  case 72:
#line 351 "parser.y"
    {
          bx_dbg_watch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 73:
#line 356 "parser.y"
    {
          bx_dbg_unwatch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 74:
#line 361 "parser.y"
    {
          bx_dbg_watch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 75:
#line 366 "parser.y"
    {
          bx_dbg_unwatch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 76:
#line 374 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-1].sval, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 77:
#line 379 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-2].sval, 0, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 78:
#line 384 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-2].sval, 1, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 79:
#line 392 "parser.y"
    {
        bx_dbg_where_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 80:
#line 400 "parser.y"
    {
        bx_dbg_print_string_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 81:
#line 408 "parser.y"
    {
        bx_dbg_continue_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 82:
#line 416 "parser.y"
    {
        bx_dbg_stepN_command(1);
        free(yyvsp[-1].sval);
        }
    break;

  case 83:
#line 421 "parser.y"
    {
        bx_dbg_stepN_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 84:
#line 429 "parser.y"
    {
        bx_dbg_step_over_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 85:
#line 437 "parser.y"
    {
        bx_dbg_set_auto_disassemble(yyvsp[-1].bval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 86:
#line 442 "parser.y"
    {
        bx_dbg_set_symbol_command(yyvsp[-3].sval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
    break;

  case 87:
#line 447 "parser.y"
    { 
        bx_dbg_set_reg8l_value(yyvsp[-3].uval, yyvsp[-1].ulval);
        }
    break;

  case 88:
#line 451 "parser.y"
    { 
        bx_dbg_set_reg8h_value(yyvsp[-3].uval, yyvsp[-1].ulval);
        }
    break;

  case 89:
#line 455 "parser.y"
    { 
        bx_dbg_set_reg16_value(yyvsp[-3].uval, yyvsp[-1].ulval);
        }
    break;

  case 90:
#line 459 "parser.y"
    { 
        bx_dbg_set_reg32_value(yyvsp[-3].uval, yyvsp[-1].ulval);
        }
    break;

  case 91:
#line 466 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 92:
#line 471 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkRegular, yyvsp[-3].ulval, yyvsp[-1].ulval);
        free(yyvsp[-4].sval);
        }
    break;

  case 93:
#line 476 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 94:
#line 481 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-2].sval);
        }
    break;

  case 95:
#line 486 "parser.y"
    {
        bx_dbg_lbreakpoint_symbol_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval);free(yyvsp[-1].sval);
        }
    break;

  case 96:
#line 491 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 97:
#line 496 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-2].sval);
        }
    break;

  case 98:
#line 501 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-3].sval);
        }
    break;

  case 99:
#line 509 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 100:
#line 517 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free(yyvsp[-1].sval);
        }
    break;

  case 101:
#line 522 "parser.y"
    {
        bx_dbg_info_symbols_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval);free(yyvsp[-1].sval);
        }
    break;

  case 102:
#line 530 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 103:
#line 535 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 104:
#line 540 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 105:
#line 545 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 106:
#line 550 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 107:
#line 555 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS | BX_INFO_FPU_REGS | BX_INFO_SSE_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 108:
#line 560 "parser.y"
    {
        bx_dbg_info_dirty_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
    break;

  case 109:
#line 565 "parser.y"
    {
        bx_dbg_info_idt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 110:
#line 570 "parser.y"
    {
       bx_dbg_info_ivt_command(yyvsp[-1].uval_range);
       free(yyvsp[-3].sval); free(yyvsp[-2].sval);
       }
    break;

  case 111:
#line 575 "parser.y"
    {
        bx_dbg_info_gdt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 112:
#line 580 "parser.y"
    {
        bx_dbg_info_ldt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 113:
#line 585 "parser.y"
    {
        bx_dbg_info_tss_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 114:
#line 590 "parser.y"
    {
        bx_dbg_dump_table();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 115:
#line 595 "parser.y"
    {
        bx_dbg_info_flags();
        free(yyvsp[-2].sval);
        }
    break;

  case 116:
#line 600 "parser.y"
    {
        bx_dbg_info_linux_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 117:
#line 605 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 118:
#line 610 "parser.y"
    {
        bx_dbg_info_symbols_command(yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 119:
#line 615 "parser.y"
    {
        bx_dbg_info_control_regs_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 120:
#line 620 "parser.y"
    {
        bx_dbg_info_ne2k(-1, -1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 121:
#line 625 "parser.y"
    {
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-1].uval, -1);
        }
    break;

  case 122:
#line 630 "parser.y"
    {
        free(yyvsp[-6].sval); free(yyvsp[-5].sval); free(yyvsp[-4].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-3].uval, yyvsp[-1].uval);
        }
    break;

  case 123:
#line 635 "parser.y"
    {
        bx_dbg_info_pic();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 124:
#line 640 "parser.y"
    {
        bx_dbg_info_vga();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 125:
#line 651 "parser.y"
    { yyval.uval_range = make_num_range (EMPTY_ARG, EMPTY_ARG); }
    break;

  case 127:
#line 656 "parser.y"
    {
      yyval.uval_range = make_num_range (yyvsp[0].ulval, yyvsp[0].ulval);
    }
    break;

  case 128:
#line 660 "parser.y"
    {
      yyval.uval_range = make_num_range (yyvsp[-1].ulval, yyvsp[0].ulval);
    }
    break;

  case 129:
#line 666 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free(yyvsp[-1].sval);
        }
    break;

  case 130:
#line 674 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 131:
#line 682 "parser.y"
    {
        bx_dbg_del_breakpoint_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 132:
#line 690 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command(yyvsp[-1].uval, 1);
        free(yyvsp[-2].sval);
       }
    break;

  case 133:
#line 697 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command(yyvsp[-1].uval, 0);
        free(yyvsp[-2].sval);
       }
    break;

  case 134:
#line 705 "parser.y"
    {
	  bx_dbg_quit_command();
	  free(yyvsp[-1].sval);
        }
    break;

  case 135:
#line 713 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].ulval, 1);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 136:
#line 718 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 137:
#line 723 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-2].sval, NULL,0, yyvsp[-1].ulval, 1);
        free(yyvsp[-2].sval);
        }
    break;

  case 138:
#line 728 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-1].sval, NULL,0, 0, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 139:
#line 736 "parser.y"
    {
        bx_dbg_setpmem_command(yyvsp[-3].uval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
    break;

  case 140:
#line 744 "parser.y"
    {
        bx_dbg_query_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 141:
#line 752 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 142:
#line 757 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-2].sval, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 143:
#line 762 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 144:
#line 770 "parser.y"
    {
        bx_dbg_set_cpu_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 145:
#line 778 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, yyvsp[-1].uval_range);
        free(yyvsp[-2].sval);
        }
    break;

  case 146:
#line 783 "parser.y"
    {
        bx_dbg_disassemble_command(yyvsp[-2].sval, yyvsp[-1].uval_range);
	free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 147:
#line 788 "parser.y"
    {
        bx_dbg_disassemble_switch_mode();
	free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 148:
#line 793 "parser.y"
    {
        bx_dbg_set_disassemble_size(yyvsp[-1].uval);
	free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
    break;

  case 149:
#line 801 "parser.y"
    {
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 150:
#line 809 "parser.y"
    {
        bx_dbg_doit_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 151:
#line 817 "parser.y"
    {
        bx_dbg_crc_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
    break;

  case 152:
#line 825 "parser.y"
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 153:
#line 830 "parser.y"
    {
         dbg_printf("c|cont|continue - continue executing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 154:
#line 835 "parser.y"
    {
         dbg_printf("s|step|stepi [count] - execute #count instructions (default is one instruction)\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 155:
#line 840 "parser.y"
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 156:
#line 845 "parser.y"
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 157:
#line 850 "parser.y"
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 158:
#line 855 "parser.y"
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 159:
#line 860 "parser.y"
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 160:
#line 865 "parser.y"
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 161:
#line 870 "parser.y"
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 162:
#line 875 "parser.y"
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 163:
#line 880 "parser.y"
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 164:
#line 885 "parser.y"
    {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 165:
#line 891 "parser.y"
    {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 166:
#line 897 "parser.y"
    {
         dbg_printf("dump_cpu - dump complete cpu state\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 167:
#line 902 "parser.y"
    {
         dbg_printf("set_cpu - set complete cpu state\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 168:
#line 907 "parser.y"
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 169:
#line 912 "parser.y"
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 170:
#line 917 "parser.y"
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 171:
#line 922 "parser.y"
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 172:
#line 927 "parser.y"
    {
         dbg_printf("record <filename> - record console input to file filename\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 173:
#line 932 "parser.y"
    {
         dbg_printf("playback <filename> - playback console input from file filename\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 174:
#line 937 "parser.y"
    {
         dbg_printf("load-symbols [global] <filename> [offset] - load symbols from file\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 175:
#line 942 "parser.y"
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 176:
#line 947 "parser.y"
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 177:
#line 952 "parser.y"
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 178:
#line 957 "parser.y"
    {
         dbg_printf("u|disasm|disassemble [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm|disassemble switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm|disassemble size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 179:
#line 966 "parser.y"
    {
         dbg_printf("watch - print current watch point status\n");
         dbg_printf("watch stop - stop simulation when a watchpoint is encountred\n");
         dbg_printf("watch continue - do not stop the simulation when watch point is encountred\n");
         dbg_printf("watch read addr - insert a read watch point at physical address addr\n");
         dbg_printf("watch write addr - insert a write watch point at physical address addr\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 180:
#line 975 "parser.y"
    {
         dbg_printf("unwatch - remove all watch points\n");
         dbg_printf("unwatch read addr - remove a read watch point at physical address addr\n");
         dbg_printf("unwatch write addr - remove a write watch point at physical address addr\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 181:
#line 982 "parser.y"
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

  case 182:
#line 995 "parser.y"
    {
         dbg_printf("instrument start - calls bx_instr_start() callback\n");
         dbg_printf("instrument stop  - calls bx_instr_stop() callback\n");
         dbg_printf("instrument reset - calls bx_instr_reset() callback\n");
         dbg_printf("instrument print - calls bx_instr_print() callback\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 183:
#line 1003 "parser.y"
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

  case 184:
#line 1013 "parser.y"
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
         dbg_printf("info tab - show page tables\n");
         dbg_printf("info cr - show CR0-4 registers\n");
         dbg_printf("info eflags - show decoded EFLAGS register\n");
         dbg_printf("info symbols [string] - list symbols whose prefix is string\n");
         dbg_printf("info pic - show PICs registers\n");
         dbg_printf("info ne2000 - show NE2000 registers\n");
         dbg_printf("info vga - show vga registers\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 185:
#line 1034 "parser.y"
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
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 186:
#line 1047 "parser.y"
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

  case 187:
#line 1057 "parser.y"
    {
         bx_dbg_print_help();
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 188:
#line 1062 "parser.y"
    {
         bx_dbg_print_help();
         free(yyvsp[-1].sval);
         }
    break;

  case 189:
#line 1070 "parser.y"
    {
     bx_dbg_calc_command(yyvsp[-1].ulval);
     free(yyvsp[-2].sval);
     }
    break;

  case 193:
#line 1081 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 197:
#line 1089 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 205:
#line 1101 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 213:
#line 1113 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 214:
#line 1126 "parser.y"
    { yyval.ulval = yyvsp[0].uval; }
    break;

  case 215:
#line 1127 "parser.y"
    { yyval.ulval = yyvsp[0].ulval; }
    break;

  case 216:
#line 1128 "parser.y"
    { yyval.ulval = bx_dbg_get_reg8l_value(yyvsp[0].uval); }
    break;

  case 217:
#line 1129 "parser.y"
    { yyval.ulval = bx_dbg_get_reg8h_value(yyvsp[0].uval); }
    break;

  case 218:
#line 1130 "parser.y"
    { yyval.ulval = bx_dbg_get_reg16_value(yyvsp[0].uval); }
    break;

  case 219:
#line 1131 "parser.y"
    { yyval.ulval = bx_dbg_get_reg32_value(yyvsp[0].uval); }
    break;

  case 220:
#line 1132 "parser.y"
    { yyval.ulval = bx_dbg_get_selector_value(yyvsp[0].uval); }
    break;

  case 221:
#line 1133 "parser.y"
    { yyval.ulval = bx_dbg_get_ip (); }
    break;

  case 222:
#line 1134 "parser.y"
    { yyval.ulval = bx_dbg_get_eip(); }
    break;

  case 223:
#line 1135 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval + yyvsp[0].ulval; }
    break;

  case 224:
#line 1136 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval - yyvsp[0].ulval; }
    break;

  case 225:
#line 1137 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval * yyvsp[0].ulval; }
    break;

  case 226:
#line 1138 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval / yyvsp[0].ulval; }
    break;

  case 227:
#line 1139 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval >> yyvsp[0].ulval; }
    break;

  case 228:
#line 1140 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval << yyvsp[0].ulval; }
    break;

  case 229:
#line 1141 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval | yyvsp[0].ulval; }
    break;

  case 230:
#line 1142 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval ^ yyvsp[0].ulval; }
    break;

  case 231:
#line 1143 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval & yyvsp[0].ulval; }
    break;

  case 232:
#line 1144 "parser.y"
    { yyval.ulval = !yyvsp[0].ulval; }
    break;

  case 233:
#line 1145 "parser.y"
    { yyval.ulval = -yyvsp[0].ulval; }
    break;

  case 234:
#line 1146 "parser.y"
    { yyval.ulval = yyvsp[-1].ulval; }
    break;

  case 235:
#line 1151 "parser.y"
    { yyval.ulval = yyvsp[0].uval; }
    break;

  case 236:
#line 1152 "parser.y"
    { yyval.ulval = yyvsp[0].ulval; }
    break;

  case 237:
#line 1153 "parser.y"
    { yyval.ulval = bx_dbg_get_symbol_value(yyvsp[0].sval); free(yyvsp[0].sval);}
    break;

  case 238:
#line 1154 "parser.y"
    { yyval.ulval = bx_dbg_get_reg8l_value(yyvsp[0].uval); }
    break;

  case 239:
#line 1155 "parser.y"
    { yyval.ulval = bx_dbg_get_reg8h_value(yyvsp[0].uval); }
    break;

  case 240:
#line 1156 "parser.y"
    { yyval.ulval = bx_dbg_get_reg16_value(yyvsp[0].uval); }
    break;

  case 241:
#line 1157 "parser.y"
    { yyval.ulval = bx_dbg_get_reg32_value(yyvsp[0].uval); }
    break;

  case 242:
#line 1158 "parser.y"
    { yyval.ulval = bx_dbg_get_selector_value(yyvsp[0].uval); }
    break;

  case 243:
#line 1159 "parser.y"
    { yyval.ulval = bx_dbg_get_ip (); }
    break;

  case 244:
#line 1160 "parser.y"
    { yyval.ulval = bx_dbg_get_eip(); }
    break;

  case 245:
#line 1161 "parser.y"
    { yyval.ulval = bx_dbg_get_laddr (yyvsp[-2].ulval, yyvsp[0].ulval); }
    break;

  case 246:
#line 1162 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval + yyvsp[0].ulval; }
    break;

  case 247:
#line 1163 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval - yyvsp[0].ulval; }
    break;

  case 248:
#line 1164 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval * yyvsp[0].ulval; }
    break;

  case 249:
#line 1165 "parser.y"
    { yyval.ulval = (yyvsp[0].ulval != 0) ? yyvsp[-2].ulval / yyvsp[0].ulval : 0; }
    break;

  case 250:
#line 1166 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval >> yyvsp[0].ulval; }
    break;

  case 251:
#line 1167 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval << yyvsp[0].ulval; }
    break;

  case 252:
#line 1168 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval | yyvsp[0].ulval; }
    break;

  case 253:
#line 1169 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval ^ yyvsp[0].ulval; }
    break;

  case 254:
#line 1170 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval & yyvsp[0].ulval; }
    break;

  case 255:
#line 1171 "parser.y"
    { yyval.ulval = !yyvsp[0].ulval; }
    break;

  case 256:
#line 1172 "parser.y"
    { yyval.ulval = -yyvsp[0].ulval; }
    break;

  case 257:
#line 1173 "parser.y"
    { yyval.ulval = yyvsp[-1].ulval; }
    break;


    }

/* Line 999 of yacc.c.  */
#line 3273 "y.tab.c"

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


#line 1176 "parser.y"


#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
