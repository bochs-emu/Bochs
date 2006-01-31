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
     BX_TOKEN_STRING = 338,
     BX_TOKEN_DOIT = 339,
     BX_TOKEN_CRC = 340,
     BX_TOKEN_TRACE = 341,
     BX_TOKEN_TRACEREG = 342,
     BX_TOKEN_SWITCH_MODE = 343,
     BX_TOKEN_SIZE = 344,
     BX_TOKEN_PTIME = 345,
     BX_TOKEN_TIMEBP_ABSOLUTE = 346,
     BX_TOKEN_TIMEBP = 347,
     BX_TOKEN_RECORD = 348,
     BX_TOKEN_PLAYBACK = 349,
     BX_TOKEN_MODEBP = 350,
     BX_TOKEN_PRINT_STACK = 351,
     BX_TOKEN_WATCH = 352,
     BX_TOKEN_UNWATCH = 353,
     BX_TOKEN_READ = 354,
     BX_TOKEN_WRITE = 355,
     BX_TOKEN_SHOW = 356,
     BX_TOKEN_LOAD_SYMBOLS = 357,
     BX_TOKEN_SYMBOLS = 358,
     BX_TOKEN_LIST_SYMBOLS = 359,
     BX_TOKEN_GLOBAL = 360,
     BX_TOKEN_WHERE = 361,
     BX_TOKEN_PRINT_STRING = 362,
     BX_TOKEN_NUMERIC = 363,
     BX_TOKEN_LONG_NUMERIC = 364,
     BX_TOKEN_NE2000 = 365,
     BX_TOKEN_PIC = 366,
     BX_TOKEN_PAGE = 367,
     BX_TOKEN_HELP = 368,
     BX_TOKEN_CALC = 369,
     BX_TOKEN_VGA = 370,
     BX_TOKEN_RSHIFT = 371,
     BX_TOKEN_LSHIFT = 372,
     BX_TOKEN_IVT = 373,
     NEG = 374,
     NOT = 375
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
#define BX_TOKEN_STRING 338
#define BX_TOKEN_DOIT 339
#define BX_TOKEN_CRC 340
#define BX_TOKEN_TRACE 341
#define BX_TOKEN_TRACEREG 342
#define BX_TOKEN_SWITCH_MODE 343
#define BX_TOKEN_SIZE 344
#define BX_TOKEN_PTIME 345
#define BX_TOKEN_TIMEBP_ABSOLUTE 346
#define BX_TOKEN_TIMEBP 347
#define BX_TOKEN_RECORD 348
#define BX_TOKEN_PLAYBACK 349
#define BX_TOKEN_MODEBP 350
#define BX_TOKEN_PRINT_STACK 351
#define BX_TOKEN_WATCH 352
#define BX_TOKEN_UNWATCH 353
#define BX_TOKEN_READ 354
#define BX_TOKEN_WRITE 355
#define BX_TOKEN_SHOW 356
#define BX_TOKEN_LOAD_SYMBOLS 357
#define BX_TOKEN_SYMBOLS 358
#define BX_TOKEN_LIST_SYMBOLS 359
#define BX_TOKEN_GLOBAL 360
#define BX_TOKEN_WHERE 361
#define BX_TOKEN_PRINT_STRING 362
#define BX_TOKEN_NUMERIC 363
#define BX_TOKEN_LONG_NUMERIC 364
#define BX_TOKEN_NE2000 365
#define BX_TOKEN_PIC 366
#define BX_TOKEN_PAGE 367
#define BX_TOKEN_HELP 368
#define BX_TOKEN_CALC 369
#define BX_TOKEN_VGA 370
#define BX_TOKEN_RSHIFT 371
#define BX_TOKEN_LSHIFT 372
#define BX_TOKEN_IVT 373
#define NEG 374
#define NOT 375




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
#line 346 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 358 "y.tab.c"

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
#define YYFINAL  269
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1248

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  134
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  50
/* YYNRULES -- Number of rules. */
#define YYNRULES  256
/* YYNRULES -- Number of states. */
#define YYNSTATES  474

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   375

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     128,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   131,     2,     2,     2,     2,   125,     2,
     132,   133,   123,   119,     2,   120,     2,   124,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   130,     2,
       2,   129,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   122,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   121,     2,     2,     2,     2,     2,
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
     115,   116,   117,   118,   126,   127
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
      99,   103,   107,   111,   115,   118,   122,   125,   128,   132,
     136,   140,   144,   147,   151,   155,   159,   162,   165,   170,
     175,   180,   185,   189,   194,   200,   203,   207,   210,   213,
     217,   220,   225,   230,   236,   242,   248,   254,   260,   263,
     269,   272,   276,   280,   283,   287,   292,   295,   298,   302,
     306,   310,   314,   318,   322,   326,   330,   335,   340,   345,
     350,   355,   359,   363,   367,   372,   376,   380,   386,   394,
     398,   402,   403,   405,   407,   410,   413,   416,   420,   424,
     428,   431,   436,   440,   444,   447,   453,   457,   461,   466,
     470,   473,   477,   482,   486,   492,   496,   500,   504,   508,
     512,   517,   521,   525,   529,   533,   537,   541,   545,   549,
     553,   557,   561,   565,   569,   573,   577,   581,   585,   589,
     593,   597,   601,   605,   609,   613,   617,   621,   625,   629,
     633,   637,   641,   645,   649,   653,   657,   661,   664,   668,
     670,   672,   674,   676,   678,   680,   682,   684,   686,   688,
     690,   692,   694,   696,   698,   700,   702,   704,   706,   708,
     710,   712,   714,   716,   718,   720,   722,   724,   726,   728,
     730,   732,   734,   738,   742,   746,   750,   754,   758,   762,
     766,   770,   773,   776,   780,   782,   784,   786,   788,   790,
     792,   794,   796,   798,   800,   804,   808,   812,   816,   820,
     824,   828,   832,   836,   840,   843,   846
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     135,     0,    -1,   135,   136,    -1,   136,    -1,   151,    -1,
     152,    -1,   153,    -1,   154,    -1,   155,    -1,   158,    -1,
     161,    -1,   156,    -1,   157,    -1,   162,    -1,   163,    -1,
     164,    -1,   165,    -1,   166,    -1,   167,    -1,   168,    -1,
     169,    -1,   170,    -1,   171,    -1,   172,    -1,   173,    -1,
     174,    -1,   175,    -1,   144,    -1,   145,    -1,   143,    -1,
     138,    -1,   139,    -1,   140,    -1,   141,    -1,   146,    -1,
     147,    -1,   142,    -1,   148,    -1,   149,    -1,   150,    -1,
     176,    -1,   177,    -1,    -1,   128,    -1,    29,    -1,    30,
      -1,    31,    -1,    32,    -1,    33,    -1,    34,    -1,    92,
     109,   128,    -1,    91,   109,   128,    -1,    93,    83,   128,
      -1,    94,    83,   128,    -1,    95,   128,    -1,   101,    83,
     128,    -1,   101,   128,    -1,    90,   128,    -1,    86,    36,
     128,    -1,    86,    37,   128,    -1,    87,    36,   128,    -1,
      87,    37,   128,    -1,    96,   128,    -1,    96,   108,   128,
      -1,    97,    80,   128,    -1,    97,    38,   128,    -1,    97,
     128,    -1,    98,   128,    -1,    97,    99,   108,   128,    -1,
      98,    99,   108,   128,    -1,    97,   100,   108,   128,    -1,
      98,   100,   108,   128,    -1,   102,    83,   128,    -1,   102,
      83,   108,   128,    -1,   102,   105,    83,   108,   128,    -1,
     106,   128,    -1,   107,   108,   128,    -1,    38,   128,    -1,
      39,   128,    -1,    39,   108,   128,    -1,    40,   128,    -1,
      42,    77,    36,   128,    -1,    42,    77,    37,   128,    -1,
      42,    69,   129,   108,   128,    -1,    42,   179,   129,   183,
     128,    -1,    42,   178,   129,   183,   128,    -1,    42,   180,
     129,   183,   128,    -1,    42,   181,   129,   183,   128,    -1,
      45,   128,    -1,    45,   182,   130,   183,   128,    -1,    46,
     128,    -1,    46,   183,   128,    -1,    46,    83,   128,    -1,
      47,   128,    -1,    47,   183,   128,    -1,    47,   123,   183,
     128,    -1,    44,   128,    -1,   104,   128,    -1,   104,    83,
     128,    -1,    51,    47,   128,    -1,    51,    54,   128,    -1,
      51,    53,   128,    -1,    51,    55,   128,    -1,    51,    56,
     128,    -1,    51,    57,   128,    -1,    51,    62,   128,    -1,
      51,    58,   159,   128,    -1,    51,   118,   159,   128,    -1,
      51,    59,   159,   128,    -1,    51,    60,   159,   128,    -1,
      51,    61,   159,   128,    -1,    51,    35,   128,    -1,    51,
      63,   128,    -1,    51,   103,   128,    -1,    51,   103,    83,
     128,    -1,    51,    64,   128,    -1,    51,   110,   128,    -1,
      51,   110,   112,   108,   128,    -1,    51,   110,   112,   108,
      53,   108,   128,    -1,    51,   111,   128,    -1,    51,   115,
     128,    -1,    -1,   160,    -1,   183,    -1,   183,   183,    -1,
      53,   128,    -1,    75,   128,    -1,    48,   108,   128,    -1,
      49,   108,   128,    -1,    50,   108,   128,    -1,    52,   128,
      -1,    65,    66,   183,   128,    -1,    65,    66,   128,    -1,
      65,   183,   128,    -1,    65,   128,    -1,    68,   108,   108,
     108,   128,    -1,    70,    71,   128,    -1,    72,    73,   128,
      -1,    72,    73,   108,   128,    -1,    72,    74,   128,    -1,
      76,   128,    -1,    77,   159,   128,    -1,    77,    67,   159,
     128,    -1,    77,    88,   128,    -1,    77,    89,   129,   108,
     128,    -1,    78,    79,   128,    -1,    78,    80,   128,    -1,
      78,    81,   128,    -1,    78,    82,   128,    -1,    84,   108,
     128,    -1,    85,   108,   108,   128,    -1,   113,    52,   128,
      -1,   113,    38,   128,    -1,   113,    39,   128,    -1,   113,
      40,   128,    -1,   113,    45,   128,    -1,   113,    46,   128,
      -1,   113,    48,   128,    -1,   113,    49,   128,    -1,   113,
      50,   128,    -1,   113,    44,   128,    -1,   113,    95,   128,
      -1,   113,    85,   128,    -1,   113,    86,   128,    -1,   113,
      87,   128,    -1,   113,    75,   128,    -1,   113,    76,   128,
      -1,   113,    90,   128,    -1,   113,    92,   128,    -1,   113,
      91,   128,    -1,   113,    96,   128,    -1,   113,    93,   128,
      -1,   113,    94,   128,    -1,   113,   102,   128,    -1,   113,
     104,   128,    -1,   113,    53,   128,    -1,   113,    68,   128,
      -1,   113,    77,   128,    -1,   113,    97,   128,    -1,   113,
      98,   128,    -1,   113,    65,   128,    -1,   113,    78,   128,
      -1,   113,    42,   128,    -1,   113,    51,   128,    -1,   113,
     101,   128,    -1,   113,   114,   128,    -1,   113,   113,   128,
      -1,   113,   128,    -1,   114,   183,   128,    -1,     7,    -1,
       8,    -1,     9,    -1,    10,    -1,     3,    -1,     4,    -1,
       5,    -1,     6,    -1,    11,    -1,    12,    -1,    13,    -1,
      14,    -1,    19,    -1,    20,    -1,    21,    -1,    22,    -1,
      15,    -1,    16,    -1,    17,    -1,    18,    -1,    24,    -1,
      25,    -1,    26,    -1,    27,    -1,   108,    -1,   109,    -1,
     179,    -1,   178,    -1,   180,    -1,   181,    -1,   137,    -1,
      23,    -1,    28,    -1,   182,   119,   182,    -1,   182,   120,
     182,    -1,   182,   123,   182,    -1,   182,   124,   182,    -1,
     182,   116,   182,    -1,   182,   117,   182,    -1,   182,   121,
     182,    -1,   182,   122,   182,    -1,   182,   125,   182,    -1,
     131,   182,    -1,   120,   182,    -1,   132,   182,   133,    -1,
     108,    -1,   109,    -1,    83,    -1,   179,    -1,   178,    -1,
     180,    -1,   181,    -1,   137,    -1,    23,    -1,    28,    -1,
     183,   130,   183,    -1,   183,   119,   183,    -1,   183,   120,
     183,    -1,   183,   123,   183,    -1,   183,   124,   183,    -1,
     183,   116,   183,    -1,   183,   117,   183,    -1,   183,   121,
     183,    -1,   183,   122,   183,    -1,   183,   125,   183,    -1,
     131,   183,    -1,   120,   183,    -1,   132,   183,   133,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   164,   164,   165,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   214,   215,   216,   217,   218,   219,
     224,   229,   237,   245,   253,   261,   266,   274,   282,   287,
     295,   300,   308,   313,   321,   327,   333,   338,   343,   348,
     353,   358,   366,   371,   376,   384,   392,   400,   408,   413,
     421,   429,   434,   439,   444,   448,   452,   456,   463,   468,
     473,   478,   483,   488,   493,   498,   506,   514,   519,   527,
     532,   537,   542,   547,   552,   557,   562,   567,   572,   577,
     582,   587,   592,   597,   602,   607,   612,   617,   622,   627,
     632,   644,   645,   648,   652,   658,   666,   674,   682,   689,
     697,   705,   710,   715,   720,   728,   736,   744,   749,   754,
     762,   770,   775,   780,   785,   793,   794,   795,   796,   804,
     812,   820,   825,   830,   835,   840,   845,   850,   855,   860,
     865,   870,   875,   880,   886,   892,   897,   902,   907,   912,
     917,   922,   927,   932,   937,   942,   947,   952,   961,   970,
     977,   990,   998,  1008,  1028,  1042,  1052,  1057,  1065,  1073,
    1074,  1075,  1076,  1081,  1082,  1083,  1084,  1089,  1090,  1091,
    1092,  1093,  1094,  1095,  1096,  1101,  1102,  1103,  1104,  1105,
    1106,  1107,  1108,  1122,  1123,  1124,  1125,  1126,  1127,  1128,
    1129,  1130,  1131,  1132,  1133,  1134,  1135,  1136,  1137,  1138,
    1139,  1140,  1141,  1142,  1147,  1148,  1149,  1150,  1151,  1152,
    1153,  1154,  1155,  1156,  1157,  1158,  1159,  1160,  1161,  1162,
    1163,  1164,  1165,  1166,  1167,  1168,  1169
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
  "BX_TOKEN_STOP", "BX_TOKEN_RESET", "BX_TOKEN_PRINT", "BX_TOKEN_STRING", 
  "BX_TOKEN_DOIT", "BX_TOKEN_CRC", "BX_TOKEN_TRACE", "BX_TOKEN_TRACEREG", 
  "BX_TOKEN_SWITCH_MODE", "BX_TOKEN_SIZE", "BX_TOKEN_PTIME", 
  "BX_TOKEN_TIMEBP_ABSOLUTE", "BX_TOKEN_TIMEBP", "BX_TOKEN_RECORD", 
  "BX_TOKEN_PLAYBACK", "BX_TOKEN_MODEBP", "BX_TOKEN_PRINT_STACK", 
  "BX_TOKEN_WATCH", "BX_TOKEN_UNWATCH", "BX_TOKEN_READ", "BX_TOKEN_WRITE", 
  "BX_TOKEN_SHOW", "BX_TOKEN_LOAD_SYMBOLS", "BX_TOKEN_SYMBOLS", 
  "BX_TOKEN_LIST_SYMBOLS", "BX_TOKEN_GLOBAL", "BX_TOKEN_WHERE", 
  "BX_TOKEN_PRINT_STRING", "BX_TOKEN_NUMERIC", "BX_TOKEN_LONG_NUMERIC", 
  "BX_TOKEN_NE2000", "BX_TOKEN_PIC", "BX_TOKEN_PAGE", "BX_TOKEN_HELP", 
  "BX_TOKEN_CALC", "BX_TOKEN_VGA", "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT", 
  "BX_TOKEN_IVT", "'+'", "'-'", "'|'", "'^'", "'*'", "'/'", "'&'", "NEG", 
  "NOT", "'\\n'", "'='", "':'", "'!'", "'('", "')'", "$accept", 
  "commands", "command", "BX_TOKEN_SEGREG", "timebp_command", 
  "record_command", "playback_command", "modebp_command", "show_command", 
  "ptime_command", "trace_command", "trace_reg_command", 
  "print_stack_command", "watch_point_command", "symbol_command", 
  "where_command", "print_string_command", "continue_command", 
  "stepN_command", "step_over_command", "set_command", 
  "breakpoint_command", "blist_command", "slist_command", "info_command", 
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
     365,   366,   367,   368,   369,   370,   371,   372,   373,    43,
      45,   124,    94,    42,    47,    38,   374,   375,    10,    61,
      58,    33,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   134,   135,   135,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   137,   137,   137,   137,   137,   137,
     138,   138,   139,   140,   141,   142,   142,   143,   144,   144,
     145,   145,   146,   146,   147,   147,   147,   147,   147,   147,
     147,   147,   148,   148,   148,   149,   150,   151,   152,   152,
     153,   154,   154,   154,   154,   154,   154,   154,   155,   155,
     155,   155,   155,   155,   155,   155,   156,   157,   157,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   159,   159,   160,   160,   161,   162,   163,   164,   165,
     166,   167,   167,   167,   167,   168,   169,   170,   170,   170,
     171,   172,   172,   172,   172,   173,   173,   173,   173,   174,
     175,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   177,   178,
     178,   178,   178,   179,   179,   179,   179,   180,   180,   180,
     180,   180,   180,   180,   180,   181,   181,   181,   181,   181,
     181,   181,   181,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     2,     3,     2,     2,     3,     3,
       3,     3,     2,     3,     3,     3,     2,     2,     4,     4,
       4,     4,     3,     4,     5,     2,     3,     2,     2,     3,
       2,     4,     4,     5,     5,     5,     5,     5,     2,     5,
       2,     3,     3,     2,     3,     4,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     4,     4,     4,     4,
       4,     3,     3,     3,     4,     3,     3,     5,     7,     3,
       3,     0,     1,     1,     2,     2,     2,     3,     3,     3,
       2,     4,     3,     3,     2,     5,     3,     3,     4,     3,
       2,     3,     4,     3,     5,     3,     3,     3,     3,     3,
       4,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
      42,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   121,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,     0,     3,    30,    31,    32,    33,
      36,    29,    27,    28,    34,    35,    37,    38,    39,     4,
       5,     6,     7,     8,    11,    12,     9,    10,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    40,    41,    77,     0,    78,    80,   193,   194,
     195,   196,   189,   190,   191,   192,   197,   198,   199,   200,
     205,   206,   207,   208,   201,   202,   203,   204,   209,   210,
     211,   212,     0,     0,     0,     0,     0,     0,    96,   220,
     221,    44,    45,    46,    47,    48,    49,   213,   214,     0,
      88,     0,     0,   219,   216,   215,   217,   218,     0,   242,
     243,   236,   234,   235,     0,    90,     0,     0,   241,   238,
     237,   239,   240,     0,   236,     0,    93,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   121,   121,
     121,   121,     0,     0,     0,     0,     0,     0,     0,   121,
     130,   125,     0,   134,     0,     0,     0,     0,     0,   126,
     140,   121,     0,     0,     0,   122,   123,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    57,     0,     0,
       0,     0,    54,     0,    62,     0,     0,     0,     0,    66,
       0,     0,    67,     0,    56,     0,     0,     0,    97,    75,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   187,     0,     1,
       2,    79,     0,     0,     0,     0,     0,     0,     0,   232,
     231,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    92,   255,   254,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    91,     0,     0,    94,   127,
     128,   129,   111,    99,   101,   100,   102,   103,   104,     0,
       0,     0,     0,   105,   112,   115,     0,   113,     0,   116,
     119,   120,     0,   132,     0,   133,     0,   136,     0,   137,
     139,     0,   143,     0,   141,     0,   124,   145,   146,   147,
     148,   149,     0,    58,    59,    60,    61,    51,    50,    52,
      53,    63,    65,    64,     0,     0,     0,     0,    55,     0,
      72,     0,    98,    76,   152,   153,   154,   182,   160,   155,
     156,   157,   158,   159,   183,   151,   175,   180,   176,   165,
     166,   177,   181,   162,   163,   164,   167,   169,   168,   171,
     172,   161,   170,   178,   179,   184,   173,   174,   186,   185,
     188,     0,    81,    82,     0,     0,     0,     0,   233,   226,
     227,   222,   223,   228,   229,   224,   225,   230,     0,   256,
     249,   250,   245,   246,   251,   252,   247,   248,   253,   244,
      95,   106,   108,   109,   110,   114,     0,   107,   131,     0,
     138,   142,     0,   246,   150,    68,    70,    69,    71,    73,
       0,    83,    85,    84,    86,    87,    89,     0,   117,   135,
     144,    74,     0,   118
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    44,    45,   148,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,   194,   195,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,   149,   150,   151,   152,   138,   196
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -151
static const short yypact[] =
{
    1045,  -122,   -96,  -117,  1171,  -115,   801,   639,   379,   -93,
     -61,   -53,   386,   -98,   -62,   509,   -35,     5,   -10,   -44,
      -3,   347,   -36,    13,    20,    32,    35,     8,    29,    37,
      61,    62,    23,   -94,    42,   -90,   -80,   -79,   -78,    24,
      46,   508,   833,  -151,   960,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,    25,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,    26,    87,    28,    31,    33,    34,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,   865,
    -151,   865,   865,  -151,  -151,  -151,  -151,  -151,   447,  -151,
    -151,    38,  -151,  -151,   833,  -151,   833,   833,  -151,  -151,
    -151,  -151,  -151,   103,  -151,   833,  -151,   118,    43,    44,
      64,    65,    68,    69,    70,    71,    72,    73,   833,   833,
     833,   833,    76,    84,    85,   -76,   -89,    93,   101,   833,
    -151,  -151,   671,  -151,   133,    53,   102,   -92,   104,  -151,
    -151,   833,   108,    58,   116,  -151,    86,   117,   119,   123,
     131,   132,    56,   134,   136,   137,   138,  -151,   139,   144,
     147,   155,  -151,   158,  -151,   160,   170,   106,   107,  -151,
     228,   229,  -151,   211,  -151,   -91,    90,   212,  -151,  -151,
     287,   288,   289,   290,   291,   292,   294,   295,   296,   297,
     298,   299,   300,   301,   303,   304,   306,   309,   310,   323,
     324,   325,   326,   329,   330,   331,   332,   333,   335,   336,
     337,   338,   342,   353,   355,   356,   357,  -151,   157,  -151,
    -151,  -151,   378,   362,   363,   833,   833,   833,   833,  -151,
    -151,    10,   865,   865,   865,   865,   865,   865,   865,   865,
     865,   833,  -151,   364,   364,   -63,   833,   833,   833,   833,
     833,   833,   833,   833,   833,  -151,   833,   352,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,   365,
     367,   370,   372,  -151,  -151,  -151,   375,  -151,   384,  -151,
    -151,  -151,   377,  -151,   590,  -151,   398,  -151,   380,  -151,
    -151,   381,  -151,   436,  -151,   833,   984,  -151,  -151,  -151,
    -151,  -151,   417,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,   421,   423,   427,   434,  -151,   437,
    -151,   466,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,   450,  -151,  -151,   607,   665,   827,   859,  -151,  -151,
    -151,   -38,   -38,   -38,   -38,  -151,  -151,  -151,   899,  -151,
     364,   364,   -42,   -42,   -42,   -42,   364,   364,   364,   984,
    -151,  -151,  -151,  -151,  -151,  -151,   -51,  -151,  -151,   451,
    -151,  -151,   452,   -42,  -151,  -151,  -151,  -151,  -151,  -151,
     453,  -151,  -151,  -151,  -151,  -151,  -151,   474,  -151,  -151,
    -151,  -151,   459,  -151
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -151,  -151,   544,    59,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -150,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,    18,    27,    36,    45,  -104,    -7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned short yytable[] =
{
     153,   157,   467,   223,   225,   227,    84,   326,   184,   220,
     221,    87,    85,   118,   213,   158,   338,   369,   319,   320,
     321,   322,   114,   328,   134,   279,   226,   280,   281,   332,
     180,   115,    86,   135,   214,   268,   339,   370,   222,   329,
     116,   341,   136,   197,   198,   199,   200,   159,   224,   117,
     228,   137,   327,   296,   297,   160,   298,   299,   300,   301,
     302,   303,   304,   187,   188,   133,   181,   306,   203,   204,
     429,   205,   206,   185,   296,   297,   186,   468,   282,   283,
     215,   302,   303,   304,   189,   288,   289,   290,   306,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   139,
     108,   109,   110,   111,   140,   121,   122,   123,   124,   125,
     126,   201,   216,   273,   274,   190,   282,   283,   202,   284,
     285,   286,   287,   288,   289,   290,   207,   293,   208,   294,
     295,   217,   218,   418,   210,   211,   209,   134,   307,   134,
     134,   212,   229,   271,   230,   272,   135,   275,   135,   135,
     276,   336,   277,   278,   352,   136,   292,   136,   136,   154,
     219,   309,   310,   371,   137,   334,   137,   137,   419,   420,
     421,   422,   423,   424,   425,   426,   427,   343,   133,   346,
     133,   133,   311,   312,   142,   143,   313,   314,   315,   316,
     317,   318,   296,   297,   323,   298,   345,   300,   301,   302,
     303,   304,   324,   325,   364,   365,   306,   146,   147,   296,
     297,   330,   298,   299,   300,   301,   302,   303,   304,   331,
     337,   305,   340,   306,   296,   297,   342,   298,   299,   300,
     301,   302,   303,   304,   344,   347,   308,   348,   306,   296,
     297,   349,   298,   299,   300,   301,   302,   303,   304,   350,
     351,   335,   353,   306,   354,   355,   356,   357,   414,   415,
     416,   417,   358,   296,   297,   359,   298,   299,   300,   301,
     302,   303,   304,   360,   428,   410,   361,   306,   362,   430,
     431,   432,   433,   434,   435,   436,   437,   438,   363,   439,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   366,   367,   453,   368,
     372,   133,   133,   133,   133,   133,   133,   133,   133,   133,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     139,   108,   109,   110,   111,   140,   121,   122,   123,   124,
     125,   126,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   139,   108,   109,   110,   111,   140,   121,   122,
     123,   124,   125,   126,   191,   373,   374,   375,   376,   377,
     378,   161,   379,   380,   381,   382,   383,   384,   385,   386,
     154,   387,   388,   162,   389,   192,   193,   390,   391,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   392,   393,   394,   395,   142,   143,   396,   397,   398,
     399,   400,   154,   401,   402,   403,   404,   144,   296,   297,
     405,   298,   299,   300,   301,   302,   303,   304,   146,   147,
     440,   406,   306,   407,   408,   409,   411,   142,   143,   175,
     412,   413,   446,   441,   306,   442,   176,   177,   443,   144,
     444,   178,   155,   445,   179,   447,   449,   156,   450,   451,
     146,   147,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   139,   108,   109,   110,   111,   140,   121,   122,
     123,   124,   125,   126,   452,   454,   231,   232,   233,   455,
     234,   456,   235,   236,   237,   457,   238,   239,   240,   241,
     242,   243,   458,   282,   283,   459,   284,   285,   286,   287,
     288,   289,   290,   244,   460,   182,   245,   291,   461,   469,
     470,   471,   472,   246,   247,   248,   249,   473,   270,     0,
       0,     0,   154,   250,   251,   252,     0,     0,   253,   254,
     255,   256,   257,   258,   259,   260,   261,     0,     0,   262,
     263,     0,   264,     0,     0,     0,     0,   142,   143,     0,
       0,   265,   266,     0,     0,     0,     0,     0,     0,   144,
       0,     0,     0,     0,     0,     0,   267,   183,     0,     0,
     146,   147,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   139,   108,   109,   110,   111,   140,   121,   122,
     123,   124,   125,   126,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   139,   108,   109,   110,   111,   140,
     121,   122,   123,   124,   125,   126,   296,   297,     0,   298,
     299,   300,   301,   302,   303,   304,     0,     0,   448,     0,
     306,     0,   141,   296,   297,     0,   298,   299,   300,   301,
     302,   303,   304,     0,     0,   462,     0,   306,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   142,   143,     0,
       0,     0,     0,     0,   154,     0,     0,     0,     0,   144,
       0,     0,     0,     0,     0,     0,     0,   145,     0,     0,
     146,   147,     0,     0,     0,     0,     0,     0,     0,   142,
     143,   296,   297,     0,   298,   299,   300,   301,   302,   303,
     304,   144,     0,   463,     0,   306,     0,     0,     0,   333,
       0,     0,   146,   147,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   119,   108,   109,   110,   111,   120,
     121,   122,   123,   124,   125,   126,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   139,   108,   109,   110,
     111,   140,   121,   122,   123,   124,   125,   126,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   119,   108,
     109,   110,   111,   120,   121,   122,   123,   124,   125,   126,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   127,
     128,     0,     0,     0,     0,     0,   154,     0,     0,     0,
       0,   129,     0,     0,     0,     0,     0,     0,     0,   130,
       0,     0,   131,   132,     0,     0,     0,     0,     0,     0,
       0,   142,   143,   296,   297,     0,   298,   299,   300,   301,
     302,   303,   304,   144,     0,   464,     0,   306,     0,     0,
     269,     0,     0,     0,   146,   147,     0,     0,     0,     0,
       0,     0,     0,   127,   128,   296,   297,     0,   298,   299,
     300,   301,   302,   303,   304,   129,     0,   465,     0,   306,
       0,     0,     0,     0,     0,     0,   131,   132,     1,     2,
       3,     0,     4,     0,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,     0,   296,   297,     0,   298,   299,
     300,   301,   302,   303,   304,    15,     0,   466,    16,   306,
      17,     0,    18,     0,     0,    19,    20,    21,    22,     0,
       0,     0,     0,     0,    23,    24,    25,    26,     0,     0,
      27,    28,    29,    30,    31,    32,    33,    34,    35,     0,
       0,    36,    37,     0,    38,     0,    39,    40,     0,     0,
       0,     0,     0,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     1,     2,     3,     0,     4,    43,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,     0,
     296,   297,     0,   298,   299,   300,   301,   302,   303,   304,
      15,     0,     0,    16,   306,    17,     0,    18,     0,     0,
      19,    20,    21,    22,     0,     0,     0,     0,     0,    23,
      24,    25,    26,     0,     0,    27,    28,    29,    30,    31,
      32,    33,    34,    35,     0,     0,    36,    37,     0,    38,
       0,    39,    40,     0,     0,     0,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,     0,   108,   109,   110,   111,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     112,     0,     0,     0,     0,     0,     0,     0,   113
};

static const short yycheck[] =
{
       7,     8,    53,    83,    83,    83,   128,    83,    15,    99,
     100,   128,   108,   128,   108,   108,   108,   108,   168,   169,
     170,   171,     4,   112,     6,   129,   105,   131,   132,   179,
     128,     4,   128,     6,   128,    42,   128,   128,   128,   128,
       4,   191,     6,    79,    80,    81,    82,   108,   128,     4,
     128,     6,   128,   116,   117,   108,   119,   120,   121,   122,
     123,   124,   125,    73,    74,     6,   128,   130,    36,    37,
     133,    36,    37,   108,   116,   117,    71,   128,   116,   117,
      38,   123,   124,   125,   128,   123,   124,   125,   130,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,   108,    80,    36,    37,   128,   116,   117,   108,   119,
     120,   121,   122,   123,   124,   125,   128,   144,   109,   146,
     147,    99,   100,   133,    83,    83,   109,   129,   155,   131,
     132,   128,   128,   128,   108,   129,   129,   129,   131,   132,
     129,   108,   129,   129,   108,   129,   128,   131,   132,    83,
     128,   128,   128,    83,   129,   182,   131,   132,   282,   283,
     284,   285,   286,   287,   288,   289,   290,   129,   129,   196,
     131,   132,   128,   128,   108,   109,   128,   128,   128,   128,
     128,   128,   116,   117,   128,   119,   120,   121,   122,   123,
     124,   125,   128,   128,   108,   108,   130,   131,   132,   116,
     117,   128,   119,   120,   121,   122,   123,   124,   125,   128,
     128,   128,   128,   130,   116,   117,   128,   119,   120,   121,
     122,   123,   124,   125,   128,   128,   128,   128,   130,   116,
     117,   128,   119,   120,   121,   122,   123,   124,   125,   128,
     128,   128,   128,   130,   128,   128,   128,   128,   275,   276,
     277,   278,   128,   116,   117,   128,   119,   120,   121,   122,
     123,   124,   125,   128,   291,   128,   128,   130,   128,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   128,   306,
     282,   283,   284,   285,   286,   287,   288,   289,   290,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   282,   283,
     284,   285,   286,   287,   288,   289,   290,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   108,   108,   345,   128,
     128,   282,   283,   284,   285,   286,   287,   288,   289,   290,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    67,   128,   128,   128,   128,   128,
     128,    35,   128,   128,   128,   128,   128,   128,   128,   128,
      83,   128,   128,    47,   128,    88,    89,   128,   128,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,   128,   128,   128,   128,   108,   109,   128,   128,   128,
     128,   128,    83,   128,   128,   128,   128,   120,   116,   117,
     128,   119,   120,   121,   122,   123,   124,   125,   131,   132,
     128,   128,   130,   128,   128,   128,   108,   108,   109,   103,
     128,   128,   108,   128,   130,   128,   110,   111,   128,   120,
     128,   115,   123,   128,   118,   128,   108,   128,   128,   128,
     131,   132,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,   108,   128,    38,    39,    40,   128,
      42,   128,    44,    45,    46,   128,    48,    49,    50,    51,
      52,    53,   128,   116,   117,   128,   119,   120,   121,   122,
     123,   124,   125,    65,   108,    66,    68,   130,   128,   128,
     128,   128,   108,    75,    76,    77,    78,   128,    44,    -1,
      -1,    -1,    83,    85,    86,    87,    -1,    -1,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    -1,    -1,   101,
     102,    -1,   104,    -1,    -1,    -1,    -1,   108,   109,    -1,
      -1,   113,   114,    -1,    -1,    -1,    -1,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,   128,   128,    -1,    -1,
     131,   132,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,   116,   117,    -1,   119,
     120,   121,   122,   123,   124,   125,    -1,    -1,   128,    -1,
     130,    -1,    83,   116,   117,    -1,   119,   120,   121,   122,
     123,   124,   125,    -1,    -1,   128,    -1,   130,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,    -1,
      -1,    -1,    -1,    -1,    83,    -1,    -1,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,    -1,    -1,
     131,   132,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
     109,   116,   117,    -1,   119,   120,   121,   122,   123,   124,
     125,   120,    -1,   128,    -1,   130,    -1,    -1,    -1,   128,
      -1,    -1,   131,   132,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
     109,    -1,    -1,    -1,    -1,    -1,    83,    -1,    -1,    -1,
      -1,   120,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,
      -1,    -1,   131,   132,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   108,   109,   116,   117,    -1,   119,   120,   121,   122,
     123,   124,   125,   120,    -1,   128,    -1,   130,    -1,    -1,
       0,    -1,    -1,    -1,   131,   132,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,   109,   116,   117,    -1,   119,   120,
     121,   122,   123,   124,   125,   120,    -1,   128,    -1,   130,
      -1,    -1,    -1,    -1,    -1,    -1,   131,   132,    38,    39,
      40,    -1,    42,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    -1,   116,   117,    -1,   119,   120,
     121,   122,   123,   124,   125,    65,    -1,   128,    68,   130,
      70,    -1,    72,    -1,    -1,    75,    76,    77,    78,    -1,
      -1,    -1,    -1,    -1,    84,    85,    86,    87,    -1,    -1,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    -1,
      -1,   101,   102,    -1,   104,    -1,   106,   107,    -1,    -1,
      -1,    -1,    -1,   113,   114,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    38,    39,    40,    -1,    42,   128,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    -1,
     116,   117,    -1,   119,   120,   121,   122,   123,   124,   125,
      65,    -1,    -1,    68,   130,    70,    -1,    72,    -1,    -1,
      75,    76,    77,    78,    -1,    -1,    -1,    -1,    -1,    84,
      85,    86,    87,    -1,    -1,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    -1,    -1,   101,   102,    -1,   104,
      -1,   106,   107,    -1,    -1,    -1,    -1,    -1,   113,   114,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   128,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    -1,    24,    25,    26,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      69,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    77
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    38,    39,    40,    42,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    65,    68,    70,    72,    75,
      76,    77,    78,    84,    85,    86,    87,    90,    91,    92,
      93,    94,    95,    96,    97,    98,   101,   102,   104,   106,
     107,   113,   114,   128,   135,   136,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   128,   108,   128,   128,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    24,    25,
      26,    27,    69,    77,   178,   179,   180,   181,   128,    23,
      28,    29,    30,    31,    32,    33,    34,   108,   109,   120,
     128,   131,   132,   137,   178,   179,   180,   181,   182,    23,
      28,    83,   108,   109,   120,   128,   131,   132,   137,   178,
     179,   180,   181,   183,    83,   123,   128,   183,   108,   108,
     108,    35,    47,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,   103,   110,   111,   115,   118,
     128,   128,    66,   128,   183,   108,    71,    73,    74,   128,
     128,    67,    88,    89,   159,   160,   183,    79,    80,    81,
      82,   108,   108,    36,    37,    36,    37,   128,   109,   109,
      83,    83,   128,   108,   128,    38,    80,    99,   100,   128,
      99,   100,   128,    83,   128,    83,   105,    83,   128,   128,
     108,    38,    39,    40,    42,    44,    45,    46,    48,    49,
      50,    51,    52,    53,    65,    68,    75,    76,    77,    78,
      85,    86,    87,    90,    91,    92,    93,    94,    95,    96,
      97,    98,   101,   102,   104,   113,   114,   128,   183,     0,
     136,   128,   129,    36,    37,   129,   129,   129,   129,   182,
     182,   182,   116,   117,   119,   120,   121,   122,   123,   124,
     125,   130,   128,   183,   183,   183,   116,   117,   119,   120,
     121,   122,   123,   124,   125,   128,   130,   183,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   159,
     159,   159,   159,   128,   128,   128,    83,   128,   112,   128,
     128,   128,   159,   128,   183,   128,   108,   128,   108,   128,
     128,   159,   128,   129,   128,   120,   183,   128,   128,   128,
     128,   128,   108,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   108,   108,   108,   108,   128,   108,
     128,    83,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   108,   128,   128,   183,   183,   183,   183,   133,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   183,   133,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     128,   128,   128,   128,   128,   128,   108,   128,   128,   108,
     128,   128,   108,   183,   128,   128,   128,   128,   128,   128,
     108,   128,   128,   128,   128,   128,   128,    53,   128,   128,
     128,   128,   108,   128
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
#line 209 "parser.y"
    {
      }
    break;

  case 49:
#line 220 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 50:
#line 225 "parser.y"
    {
        bx_dbg_timebp_command(0, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
    break;

  case 51:
#line 230 "parser.y"
    {
        bx_dbg_timebp_command(1, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
    break;

  case 52:
#line 238 "parser.y"
    {
          bx_dbg_record_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 53:
#line 246 "parser.y"
    {
          bx_dbg_playback_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 54:
#line 254 "parser.y"
    {
          bx_dbg_modebp_command();
          free(yyvsp[-1].sval);
          }
    break;

  case 55:
#line 262 "parser.y"
    {
          bx_dbg_show_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 56:
#line 267 "parser.y"
    {
          bx_dbg_show_command(0);
          free(yyvsp[-1].sval);
          }
    break;

  case 57:
#line 275 "parser.y"
    {
        bx_dbg_ptime_command();
        free(yyvsp[-1].sval);
	}
    break;

  case 58:
#line 283 "parser.y"
    {
        bx_dbg_trace_command(yyvsp[-1].bval);
        free(yyvsp[-2].sval);
	}
    break;

  case 59:
#line 288 "parser.y"
    {
        bx_dbg_trace_command(yyvsp[-1].bval);
        free(yyvsp[-2].sval);
	}
    break;

  case 60:
#line 296 "parser.y"
    {
	bx_dbg_trace_reg_command(yyvsp[-1].bval);
	free(yyvsp[-2].sval);
	}
    break;

  case 61:
#line 301 "parser.y"
    {
	bx_dbg_trace_reg_command(yyvsp[-1].bval);
	free(yyvsp[-2].sval);
	}
    break;

  case 62:
#line 309 "parser.y"
    {
          bx_dbg_print_stack_command(16);
          free(yyvsp[-1].sval);
	  }
    break;

  case 63:
#line 314 "parser.y"
    {
          bx_dbg_print_stack_command(yyvsp[-1].uval);
          free(yyvsp[-2].sval);
	  }
    break;

  case 64:
#line 322 "parser.y"
    {
          watchpoint_continue = 0;
	  dbg_printf("Will stop on watch points\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 65:
#line 328 "parser.y"
    {
          watchpoint_continue = 1;
          dbg_printf("Will not stop on watch points (they will still be logged)\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
    break;

  case 66:
#line 334 "parser.y"
    {
          bx_dbg_watch(-1, 0);
          free(yyvsp[-1].sval);
          }
    break;

  case 67:
#line 339 "parser.y"
    {
          bx_dbg_unwatch(-1, 0);
          free(yyvsp[-1].sval);
          }
    break;

  case 68:
#line 344 "parser.y"
    {
          bx_dbg_watch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 69:
#line 349 "parser.y"
    {
          bx_dbg_unwatch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 70:
#line 354 "parser.y"
    {
          bx_dbg_watch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 71:
#line 359 "parser.y"
    {
          bx_dbg_unwatch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
    break;

  case 72:
#line 367 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-1].sval, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 73:
#line 372 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-2].sval, 0, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 74:
#line 377 "parser.y"
    {
	bx_dbg_symbol_command(yyvsp[-2].sval, 1, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 75:
#line 385 "parser.y"
    {
        bx_dbg_where_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 76:
#line 393 "parser.y"
    {
        bx_dbg_print_string_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 77:
#line 401 "parser.y"
    {
        bx_dbg_continue_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 78:
#line 409 "parser.y"
    {
        bx_dbg_stepN_command(1);
        free(yyvsp[-1].sval);
        }
    break;

  case 79:
#line 414 "parser.y"
    {
        bx_dbg_stepN_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 80:
#line 422 "parser.y"
    {
        bx_dbg_step_over_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 81:
#line 430 "parser.y"
    {
        bx_dbg_set_auto_disassemble(yyvsp[-1].bval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 82:
#line 435 "parser.y"
    {
        bx_dbg_set_auto_disassemble(yyvsp[-1].bval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 83:
#line 440 "parser.y"
    {
        bx_dbg_set_symbol_command(yyvsp[-3].sval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
    break;

  case 84:
#line 445 "parser.y"
    { 
        bx_dbg_set_reg8l_value(yyvsp[-3].uval, yyvsp[-1].ulval);
        }
    break;

  case 85:
#line 449 "parser.y"
    { 
        bx_dbg_set_reg8h_value(yyvsp[-3].uval, yyvsp[-1].ulval);
        }
    break;

  case 86:
#line 453 "parser.y"
    { 
        bx_dbg_set_reg16_value(yyvsp[-3].uval, yyvsp[-1].ulval);
        }
    break;

  case 87:
#line 457 "parser.y"
    { 
        bx_dbg_set_reg32_value(yyvsp[-3].uval, yyvsp[-1].ulval);
        }
    break;

  case 88:
#line 464 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 89:
#line 469 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkRegular, yyvsp[-3].ulval, yyvsp[-1].ulval);
        free(yyvsp[-4].sval);
        }
    break;

  case 90:
#line 474 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 91:
#line 479 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-2].sval);
        }
    break;

  case 92:
#line 484 "parser.y"
    {
        bx_dbg_lbreakpoint_symbol_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval);free(yyvsp[-1].sval);
        }
    break;

  case 93:
#line 489 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 94:
#line 494 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-2].sval);
        }
    break;

  case 95:
#line 499 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, yyvsp[-1].ulval);
        free(yyvsp[-3].sval);
        }
    break;

  case 96:
#line 507 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 97:
#line 515 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free(yyvsp[-1].sval);
        }
    break;

  case 98:
#line 520 "parser.y"
    {
        bx_dbg_info_symbols_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval);free(yyvsp[-1].sval);
        }
    break;

  case 99:
#line 528 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 100:
#line 533 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 101:
#line 538 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 102:
#line 543 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 103:
#line 548 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 104:
#line 553 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS | BX_INFO_FPU_REGS | BX_INFO_SSE_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 105:
#line 558 "parser.y"
    {
        bx_dbg_info_dirty_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
    break;

  case 106:
#line 563 "parser.y"
    {
        bx_dbg_info_idt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 107:
#line 568 "parser.y"
    {
       bx_dbg_info_ivt_command(yyvsp[-1].uval_range);
       free(yyvsp[-3].sval); free(yyvsp[-2].sval);
       }
    break;

  case 108:
#line 573 "parser.y"
    {
        bx_dbg_info_gdt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 109:
#line 578 "parser.y"
    {
        bx_dbg_info_ldt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 110:
#line 583 "parser.y"
    {
        bx_dbg_info_tss_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 111:
#line 588 "parser.y"
    {
        bx_dbg_info_flags();
        free(yyvsp[-2].sval);
        }
    break;

  case 112:
#line 593 "parser.y"
    {
        bx_dbg_info_linux_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 113:
#line 598 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 114:
#line 603 "parser.y"
    {
        bx_dbg_info_symbols_command(yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 115:
#line 608 "parser.y"
    {
        bx_dbg_info_control_regs_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 116:
#line 613 "parser.y"
    {
        bx_dbg_info_ne2k(-1, -1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 117:
#line 618 "parser.y"
    {
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-1].uval, -1);
        }
    break;

  case 118:
#line 623 "parser.y"
    {
        free(yyvsp[-6].sval); free(yyvsp[-5].sval); free(yyvsp[-4].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-3].uval, yyvsp[-1].uval);
        }
    break;

  case 119:
#line 628 "parser.y"
    {
        bx_dbg_info_pic();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 120:
#line 633 "parser.y"
    {
        bx_dbg_info_vga();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 121:
#line 644 "parser.y"
    { yyval.uval_range = make_num_range (EMPTY_ARG, EMPTY_ARG); }
    break;

  case 123:
#line 649 "parser.y"
    {
      yyval.uval_range = make_num_range (yyvsp[0].ulval, yyvsp[0].ulval);
    }
    break;

  case 124:
#line 653 "parser.y"
    {
      yyval.uval_range = make_num_range (yyvsp[-1].ulval, yyvsp[0].ulval);
    }
    break;

  case 125:
#line 659 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free(yyvsp[-1].sval);
        }
    break;

  case 126:
#line 667 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 127:
#line 675 "parser.y"
    {
        bx_dbg_del_breakpoint_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 128:
#line 683 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command(yyvsp[-1].uval, 1);
        free(yyvsp[-2].sval);
       }
    break;

  case 129:
#line 690 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command(yyvsp[-1].uval, 0);
        free(yyvsp[-2].sval);
       }
    break;

  case 130:
#line 698 "parser.y"
    {
	  bx_dbg_quit_command();
	  free(yyvsp[-1].sval);
        }
    break;

  case 131:
#line 706 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].ulval, 1);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 132:
#line 711 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 133:
#line 716 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-2].sval, NULL,0, yyvsp[-1].ulval, 1);
        free(yyvsp[-2].sval);
        }
    break;

  case 134:
#line 721 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-1].sval, NULL,0, 0, 0);
        free(yyvsp[-1].sval);
        }
    break;

  case 135:
#line 729 "parser.y"
    {
        bx_dbg_setpmem_command(yyvsp[-3].uval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
    break;

  case 136:
#line 737 "parser.y"
    {
        bx_dbg_query_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 137:
#line 745 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 138:
#line 750 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-2].sval, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 139:
#line 755 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 140:
#line 763 "parser.y"
    {
        bx_dbg_set_cpu_command();
        free(yyvsp[-1].sval);
        }
    break;

  case 141:
#line 771 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, yyvsp[-1].uval_range);
        free(yyvsp[-2].sval);
        }
    break;

  case 142:
#line 776 "parser.y"
    {
        bx_dbg_disassemble_command(yyvsp[-2].sval, yyvsp[-1].uval_range);
	free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
    break;

  case 143:
#line 781 "parser.y"
    {
        bx_dbg_disassemble_switch_mode();
	free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 144:
#line 786 "parser.y"
    {
        bx_dbg_set_disassemble_size(yyvsp[-1].uval);
	free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
    break;

  case 148:
#line 797 "parser.y"
    {
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
    break;

  case 149:
#line 805 "parser.y"
    {
        bx_dbg_doit_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
    break;

  case 150:
#line 813 "parser.y"
    {
        bx_dbg_crc_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
    break;

  case 151:
#line 821 "parser.y"
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 152:
#line 826 "parser.y"
    {
         dbg_printf("c|cont|continue - continue executing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 153:
#line 831 "parser.y"
    {
         dbg_printf("s|step|stepi [count] - execute #count instructions (default is one instruction)\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 154:
#line 836 "parser.y"
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 155:
#line 841 "parser.y"
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 156:
#line 846 "parser.y"
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 157:
#line 851 "parser.y"
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 158:
#line 856 "parser.y"
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 159:
#line 861 "parser.y"
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 160:
#line 866 "parser.y"
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 161:
#line 871 "parser.y"
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 162:
#line 876 "parser.y"
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 163:
#line 881 "parser.y"
    {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 164:
#line 887 "parser.y"
    {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 165:
#line 893 "parser.y"
    {
         dbg_printf("dump_cpu - dump complete cpu state\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 166:
#line 898 "parser.y"
    {
         dbg_printf("set_cpu - set complete cpu state\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 167:
#line 903 "parser.y"
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 168:
#line 908 "parser.y"
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 169:
#line 913 "parser.y"
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 170:
#line 918 "parser.y"
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 171:
#line 923 "parser.y"
    {
         dbg_printf("record <filename> - record console input to file filename\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 172:
#line 928 "parser.y"
    {
         dbg_printf("playback <filename> - playback console input from file filename\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 173:
#line 933 "parser.y"
    {
         dbg_printf("load-symbols [global] <filename> [offset] - load symbols from file\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 174:
#line 938 "parser.y"
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 175:
#line 943 "parser.y"
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 176:
#line 948 "parser.y"
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 177:
#line 953 "parser.y"
    {
         dbg_printf("u|disasm|disassemble [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm|disassemble switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm|disassemble size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 178:
#line 962 "parser.y"
    {
         dbg_printf("watch - print current watch point status\n");
         dbg_printf("watch stop - stop simulation when a watchpoint is encountred\n");
         dbg_printf("watch continue - do not stop the simulation when watch point is encountred\n");
         dbg_printf("watch read addr - insert a read watch point at physical address addr\n");
         dbg_printf("watch write addr - insert a write watch point at physical address addr\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 179:
#line 971 "parser.y"
    {
         dbg_printf("unwatch - remove all watch points\n");
         dbg_printf("unwatch read addr - remove a read watch point at physical address addr\n");
         dbg_printf("unwatch write addr - remove a write watch point at physical address addr\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 180:
#line 978 "parser.y"
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

  case 181:
#line 991 "parser.y"
    {
         dbg_printf("instrument start - calls bx_instr_start() callback\n");
         dbg_printf("instrument stop  - calls bx_instr_stop() callback\n");
         dbg_printf("instrument reset - calls bx_instr_reset() callback\n");
         dbg_printf("instrument print - calls bx_instr_print() callback\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 182:
#line 999 "parser.y"
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

  case 183:
#line 1009 "parser.y"
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

  case 184:
#line 1029 "parser.y"
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

  case 185:
#line 1043 "parser.y"
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

  case 186:
#line 1053 "parser.y"
    {
         bx_dbg_print_help();
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
    break;

  case 187:
#line 1058 "parser.y"
    {
         bx_dbg_print_help();
         free(yyvsp[-1].sval);
         }
    break;

  case 188:
#line 1066 "parser.y"
    {
     bx_dbg_calc_command(yyvsp[-1].ulval);
     free(yyvsp[-2].sval);
     }
    break;

  case 192:
#line 1077 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 196:
#line 1085 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 204:
#line 1097 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 212:
#line 1109 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 213:
#line 1122 "parser.y"
    { yyval.ulval = yyvsp[0].uval; }
    break;

  case 214:
#line 1123 "parser.y"
    { yyval.ulval = yyvsp[0].ulval; }
    break;

  case 215:
#line 1124 "parser.y"
    { yyval.ulval = bx_dbg_get_reg8l_value(yyvsp[0].uval); }
    break;

  case 216:
#line 1125 "parser.y"
    { yyval.ulval = bx_dbg_get_reg8h_value(yyvsp[0].uval); }
    break;

  case 217:
#line 1126 "parser.y"
    { yyval.ulval = bx_dbg_get_reg16_value(yyvsp[0].uval); }
    break;

  case 218:
#line 1127 "parser.y"
    { yyval.ulval = bx_dbg_get_reg32_value(yyvsp[0].uval); }
    break;

  case 219:
#line 1128 "parser.y"
    { yyval.ulval = bx_dbg_get_selector_value(yyvsp[0].uval); }
    break;

  case 220:
#line 1129 "parser.y"
    { yyval.ulval = bx_dbg_get_ip (); }
    break;

  case 221:
#line 1130 "parser.y"
    { yyval.ulval = bx_dbg_get_eip(); }
    break;

  case 222:
#line 1131 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval + yyvsp[0].ulval; }
    break;

  case 223:
#line 1132 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval - yyvsp[0].ulval; }
    break;

  case 224:
#line 1133 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval * yyvsp[0].ulval; }
    break;

  case 225:
#line 1134 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval / yyvsp[0].ulval; }
    break;

  case 226:
#line 1135 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval >> yyvsp[0].ulval; }
    break;

  case 227:
#line 1136 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval << yyvsp[0].ulval; }
    break;

  case 228:
#line 1137 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval | yyvsp[0].ulval; }
    break;

  case 229:
#line 1138 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval ^ yyvsp[0].ulval; }
    break;

  case 230:
#line 1139 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval & yyvsp[0].ulval; }
    break;

  case 231:
#line 1140 "parser.y"
    { yyval.ulval = !yyvsp[0].ulval; }
    break;

  case 232:
#line 1141 "parser.y"
    { yyval.ulval = -yyvsp[0].ulval; }
    break;

  case 233:
#line 1142 "parser.y"
    { yyval.ulval = yyvsp[-1].ulval; }
    break;

  case 234:
#line 1147 "parser.y"
    { yyval.ulval = yyvsp[0].uval; }
    break;

  case 235:
#line 1148 "parser.y"
    { yyval.ulval = yyvsp[0].ulval; }
    break;

  case 236:
#line 1149 "parser.y"
    { yyval.ulval = bx_dbg_get_symbol_value(yyvsp[0].sval); free(yyvsp[0].sval);}
    break;

  case 237:
#line 1150 "parser.y"
    { yyval.ulval = bx_dbg_get_reg8l_value(yyvsp[0].uval); }
    break;

  case 238:
#line 1151 "parser.y"
    { yyval.ulval = bx_dbg_get_reg8h_value(yyvsp[0].uval); }
    break;

  case 239:
#line 1152 "parser.y"
    { yyval.ulval = bx_dbg_get_reg16_value(yyvsp[0].uval); }
    break;

  case 240:
#line 1153 "parser.y"
    { yyval.ulval = bx_dbg_get_reg32_value(yyvsp[0].uval); }
    break;

  case 241:
#line 1154 "parser.y"
    { yyval.ulval = bx_dbg_get_selector_value(yyvsp[0].uval); }
    break;

  case 242:
#line 1155 "parser.y"
    { yyval.ulval = bx_dbg_get_ip (); }
    break;

  case 243:
#line 1156 "parser.y"
    { yyval.ulval = bx_dbg_get_eip(); }
    break;

  case 244:
#line 1157 "parser.y"
    { yyval.ulval = bx_dbg_get_laddr (yyvsp[-2].ulval, yyvsp[0].ulval); }
    break;

  case 245:
#line 1158 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval + yyvsp[0].ulval; }
    break;

  case 246:
#line 1159 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval - yyvsp[0].ulval; }
    break;

  case 247:
#line 1160 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval * yyvsp[0].ulval; }
    break;

  case 248:
#line 1161 "parser.y"
    { yyval.ulval = (yyvsp[0].ulval != 0) ? yyvsp[-2].ulval / yyvsp[0].ulval : 0; }
    break;

  case 249:
#line 1162 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval >> yyvsp[0].ulval; }
    break;

  case 250:
#line 1163 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval << yyvsp[0].ulval; }
    break;

  case 251:
#line 1164 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval | yyvsp[0].ulval; }
    break;

  case 252:
#line 1165 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval ^ yyvsp[0].ulval; }
    break;

  case 253:
#line 1166 "parser.y"
    { yyval.ulval = yyvsp[-2].ulval & yyvsp[0].ulval; }
    break;

  case 254:
#line 1167 "parser.y"
    { yyval.ulval = !yyvsp[0].ulval; }
    break;

  case 255:
#line 1168 "parser.y"
    { yyval.ulval = -yyvsp[0].ulval; }
    break;

  case 256:
#line 1169 "parser.y"
    { yyval.ulval = yyvsp[-1].ulval; }
    break;


    }

/* Line 999 of yacc.c.  */
#line 3256 "y.tab.c"

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


#line 1172 "parser.y"


#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
