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




/* Copy the first part of user declarations.  */
#line 5 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

#if BX_DEBUGGER


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
#line 13 "parser.y"
typedef union YYSTYPE {
  char    *sval;
  Bit64u   uval;
  bx_bool  bval;
  bx_num_range uval_range;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 302 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 314 "y.tab.c"

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
#define YYFINAL  248
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   971

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  115
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  48
/* YYNRULES -- Number of rules. */
#define YYNRULES  236
/* YYNRULES -- Number of states. */
#define YYNSTATES  456

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   356

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     109,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   112,     2,     2,     2,     2,   106,     2,
     113,   114,   104,   100,     2,   101,     2,   105,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   111,     2,
       2,   110,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   103,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   102,     2,     2,     2,     2,     2,
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
      95,    96,    97,    98,    99,   107,   108
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
      99,   101,   103,   107,   111,   115,   119,   122,   126,   129,
     132,   136,   140,   143,   147,   151,   155,   158,   161,   166,
     171,   176,   181,   185,   190,   196,   199,   203,   206,   209,
     213,   216,   221,   227,   233,   239,   245,   251,   257,   260,
     266,   269,   273,   277,   280,   284,   289,   292,   295,   299,
     303,   307,   311,   315,   319,   323,   327,   333,   339,   345,
     351,   355,   359,   363,   367,   371,   376,   380,   384,   390,
     398,   402,   406,   407,   409,   410,   412,   414,   417,   420,
     423,   427,   431,   435,   438,   443,   447,   451,   454,   460,
     464,   468,   473,   477,   480,   484,   489,   493,   499,   503,
     507,   511,   516,   520,   524,   528,   532,   536,   540,   544,
     548,   552,   556,   560,   564,   568,   572,   576,   580,   584,
     588,   592,   596,   600,   604,   608,   612,   616,   620,   624,
     628,   632,   636,   640,   644,   648,   652,   656,   660,   664,
     667,   671,   673,   675,   677,   679,   681,   683,   685,   687,
     689,   691,   695,   699,   703,   707,   711,   715,   719,   723,
     727,   730,   733,   737,   739,   741,   743,   745,   747,   749,
     751,   753,   755,   757,   759,   763,   767,   771,   775,   779,
     783,   787,   791,   795,   799,   802,   805
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     116,     0,    -1,   116,   117,    -1,   117,    -1,   133,    -1,
     134,    -1,   135,    -1,   136,    -1,   137,    -1,   140,    -1,
     144,    -1,   138,    -1,   139,    -1,   145,    -1,   146,    -1,
     147,    -1,   148,    -1,   149,    -1,   150,    -1,   151,    -1,
     152,    -1,   153,    -1,   154,    -1,   155,    -1,   156,    -1,
     157,    -1,   158,    -1,   126,    -1,   127,    -1,   125,    -1,
     120,    -1,   121,    -1,   122,    -1,   123,    -1,   128,    -1,
     129,    -1,   124,    -1,   130,    -1,   131,    -1,   132,    -1,
     159,    -1,   160,    -1,    -1,   109,    -1,    15,    -1,    16,
      -1,     8,    -1,     9,    -1,    10,    -1,    11,    -1,    12,
      -1,    13,    -1,    70,    86,   109,    -1,    69,    86,   109,
      -1,    71,    60,   109,    -1,    72,    60,   109,    -1,    73,
     109,    -1,    79,    93,   109,    -1,    79,   109,    -1,    68,
     109,    -1,    64,   118,   109,    -1,    65,   118,   109,    -1,
      74,   109,    -1,    74,    86,   109,    -1,    75,    61,   109,
      -1,    75,    17,   109,    -1,    75,   109,    -1,    76,   109,
      -1,    75,    77,    86,   109,    -1,    76,    77,    86,   109,
      -1,    75,    78,    86,   109,    -1,    76,    78,    86,   109,
      -1,    80,    60,   109,    -1,    80,    60,    86,   109,    -1,
      80,    83,    60,    86,   109,    -1,    84,   109,    -1,    85,
      86,   109,    -1,    17,   109,    -1,    18,   109,    -1,    18,
      86,   109,    -1,    19,   109,    -1,    21,    58,   118,   109,
      -1,    21,    50,   110,    86,   109,    -1,    21,     4,   110,
     162,   109,    -1,    21,     3,   110,   162,   109,    -1,    21,
       5,   110,   162,   109,    -1,    21,     6,   110,   162,   109,
      -1,    21,     7,   110,   162,   109,    -1,    24,   109,    -1,
      24,   161,   111,   162,   109,    -1,    25,   109,    -1,    25,
     162,   109,    -1,    25,    60,   109,    -1,    26,   109,    -1,
      26,   162,   109,    -1,    26,   104,   162,   109,    -1,    23,
     109,    -1,    82,   109,    -1,    82,    60,   109,    -1,    30,
      26,   109,    -1,    30,    33,   109,    -1,    30,    32,   109,
      -1,    30,    34,   109,    -1,    30,    35,   109,    -1,    30,
      36,   109,    -1,    30,    43,   109,    -1,    30,    37,   141,
     141,   109,    -1,    30,    38,   141,   141,   109,    -1,    30,
      39,   141,   141,   109,    -1,    30,    40,   141,   141,   109,
      -1,    30,    42,   109,    -1,    30,    41,   109,    -1,    30,
      14,   109,    -1,    30,    44,   109,    -1,    30,    81,   109,
      -1,    30,    81,    60,   109,    -1,    30,    45,   109,    -1,
      30,    87,   109,    -1,    30,    87,    89,    86,   109,    -1,
      30,    87,    89,    86,    32,    86,   109,    -1,    30,    88,
     109,    -1,    30,    92,   109,    -1,    -1,    86,    -1,    -1,
     143,    -1,   162,    -1,   162,   162,    -1,    32,   109,    -1,
      56,   109,    -1,    27,    86,   109,    -1,    28,    86,   109,
      -1,    29,    86,   109,    -1,    31,   109,    -1,    46,    47,
     162,   109,    -1,    46,    47,   109,    -1,    46,   162,   109,
      -1,    46,   109,    -1,    49,    86,    86,    86,   109,    -1,
      51,    52,   109,    -1,    53,    54,   109,    -1,    53,    54,
      86,   109,    -1,    53,    55,   109,    -1,    57,   109,    -1,
      58,   142,   109,    -1,    58,    48,   142,   109,    -1,    58,
      66,   109,    -1,    58,    67,   110,    86,   109,    -1,    59,
      61,   109,    -1,    59,    93,   109,    -1,    62,    86,   109,
      -1,    63,    86,    86,   109,    -1,    90,    31,   109,    -1,
      90,    17,   109,    -1,    90,    18,   109,    -1,    90,    19,
     109,    -1,    90,    24,   109,    -1,    90,    25,   109,    -1,
      90,    26,   109,    -1,    90,    27,   109,    -1,    90,    28,
     109,    -1,    90,    29,   109,    -1,    90,    23,   109,    -1,
      90,    73,   109,    -1,    90,    63,   109,    -1,    90,    64,
     109,    -1,    90,    65,   109,    -1,    90,    56,   109,    -1,
      90,    57,   109,    -1,    90,    68,   109,    -1,    90,    70,
     109,    -1,    90,    69,   109,    -1,    90,    74,   109,    -1,
      90,    71,   109,    -1,    90,    72,   109,    -1,    90,    80,
     109,    -1,    90,    82,   109,    -1,    90,    32,   109,    -1,
      90,    49,   109,    -1,    90,    58,   109,    -1,    90,    75,
     109,    -1,    90,    76,   109,    -1,    90,    46,   109,    -1,
      90,    59,   109,    -1,    90,    21,   109,    -1,    90,    30,
     109,    -1,    90,    79,   109,    -1,    90,    91,   109,    -1,
      90,    90,   109,    -1,    90,   109,    -1,    91,   162,   109,
      -1,    86,    -1,     4,    -1,     3,    -1,     5,    -1,     6,
      -1,     7,    -1,   119,    -1,    97,    -1,    98,    -1,    99,
      -1,   161,   100,   161,    -1,   161,   101,   161,    -1,   161,
     104,   161,    -1,   161,   105,   161,    -1,   161,    95,   161,
      -1,   161,    96,   161,    -1,   161,   102,   161,    -1,   161,
     103,   161,    -1,   161,   106,   161,    -1,   112,   161,    -1,
     101,   161,    -1,   113,   161,   114,    -1,    86,    -1,    60,
      -1,     4,    -1,     3,    -1,     5,    -1,     6,    -1,     7,
      -1,   119,    -1,    97,    -1,    98,    -1,    99,    -1,   162,
     111,   162,    -1,   162,   100,   162,    -1,   162,   101,   162,
      -1,   162,   104,   162,    -1,   162,   105,   162,    -1,   162,
      95,   162,    -1,   162,    96,   162,    -1,   162,   102,   162,
      -1,   162,   103,   162,    -1,   162,   106,   162,    -1,   112,
     162,    -1,   101,   162,    -1,   113,   162,   114,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   135,   135,   136,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   185,   186,   191,   192,   193,   194,
     195,   196,   201,   206,   214,   222,   230,   238,   243,   251,
     259,   267,   275,   280,   288,   294,   300,   305,   310,   315,
     320,   325,   333,   338,   343,   351,   359,   367,   375,   380,
     388,   396,   401,   406,   410,   414,   418,   422,   429,   434,
     439,   444,   449,   454,   459,   464,   472,   480,   485,   493,
     498,   503,   508,   513,   518,   523,   528,   533,   538,   543,
     548,   553,   558,   563,   568,   573,   578,   583,   588,   593,
     598,   603,   611,   612,   615,   616,   619,   623,   629,   637,
     645,   653,   660,   668,   676,   681,   686,   691,   699,   707,
     715,   720,   725,   733,   741,   746,   751,   756,   764,   769,
     777,   785,   793,   798,   803,   808,   813,   818,   823,   828,
     833,   838,   843,   848,   853,   858,   864,   870,   875,   880,
     885,   890,   895,   900,   905,   910,   915,   920,   925,   930,
     939,   948,   955,   968,   976,   986,  1007,  1019,  1029,  1034,
    1042,  1060,  1061,  1062,  1063,  1064,  1065,  1066,  1067,  1068,
    1069,  1070,  1071,  1072,  1073,  1074,  1075,  1076,  1077,  1078,
    1079,  1080,  1081,  1086,  1087,  1088,  1089,  1090,  1091,  1092,
    1093,  1094,  1095,  1096,  1097,  1098,  1099,  1100,  1101,  1102,
    1103,  1104,  1105,  1106,  1107,  1108,  1109
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "BX_TOKEN_8BH_REG", "BX_TOKEN_8BL_REG", 
  "BX_TOKEN_16B_REG", "BX_TOKEN_32B_REG", "BX_TOKEN_64B_REG", 
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
  "BX_TOKEN_STRING", "BX_TOKEN_STOP", "BX_TOKEN_DOIT", "BX_TOKEN_CRC", 
  "BX_TOKEN_TRACE", "BX_TOKEN_TRACEREG", "BX_TOKEN_SWITCH_MODE", 
  "BX_TOKEN_SIZE", "BX_TOKEN_PTIME", "BX_TOKEN_TIMEBP_ABSOLUTE", 
  "BX_TOKEN_TIMEBP", "BX_TOKEN_RECORD", "BX_TOKEN_PLAYBACK", 
  "BX_TOKEN_MODEBP", "BX_TOKEN_PRINT_STACK", "BX_TOKEN_WATCH", 
  "BX_TOKEN_UNWATCH", "BX_TOKEN_READ", "BX_TOKEN_WRITE", "BX_TOKEN_SHOW", 
  "BX_TOKEN_LOAD_SYMBOLS", "BX_TOKEN_SYMBOLS", "BX_TOKEN_LIST_SYMBOLS", 
  "BX_TOKEN_GLOBAL", "BX_TOKEN_WHERE", "BX_TOKEN_PRINT_STRING", 
  "BX_TOKEN_NUMERIC", "BX_TOKEN_NE2000", "BX_TOKEN_PIC", "BX_TOKEN_PAGE", 
  "BX_TOKEN_HELP", "BX_TOKEN_CALC", "BX_TOKEN_VGA", "BX_TOKEN_COMMAND", 
  "BX_TOKEN_GENERIC", "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT", 
  "BX_TOKEN_REG_IP", "BX_TOKEN_REG_EIP", "BX_TOKEN_REG_RIP", "'+'", "'-'", 
  "'|'", "'^'", "'*'", "'/'", "'&'", "NEG", "NOT", "'\\n'", "'='", "':'", 
  "'!'", "'('", "')'", "$accept", "commands", "command", 
  "BX_TOKEN_TOGGLE_ON_OFF", "BX_TOKEN_SEGREG", "timebp_command", 
  "record_command", "playback_command", "modebp_command", "show_command", 
  "ptime_command", "trace_command", "trace_reg_command", 
  "print_stack_command", "watch_point_command", "symbol_command", 
  "where_command", "print_string_command", "continue_command", 
  "stepN_command", "step_over_command", "set_command", 
  "breakpoint_command", "blist_command", "slist_command", "info_command", 
  "optional_numeric", "optional_numeric_range", "numeric_range", 
  "regs_command", "dump_cpu_command", "delete_command", "bpe_command", 
  "bpd_command", "quit_command", "examine_command", "setpmem_command", 
  "query_command", "take_command", "set_cpu_command", 
  "disassemble_command", "instrument_command", "doit_command", 
  "crc_command", "help_command", "calc_command", "vexpression", 
  "expression", 0
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
      43,    45,   124,    94,    42,    47,    38,   355,   356,    10,
      61,    58,    33,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   115,   116,   116,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   118,   118,   119,   119,   119,   119,
     119,   119,   120,   120,   121,   122,   123,   124,   124,   125,
     126,   127,   128,   128,   129,   129,   129,   129,   129,   129,
     129,   129,   130,   130,   130,   131,   132,   133,   134,   134,
     135,   136,   136,   136,   136,   136,   136,   136,   137,   137,
     137,   137,   137,   137,   137,   137,   138,   139,   139,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   141,   141,   142,   142,   143,   143,   144,   145,
     146,   147,   148,   149,   150,   150,   150,   150,   151,   152,
     153,   153,   153,   154,   155,   155,   155,   155,   156,   156,
     157,   158,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     160,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     2,     3,     2,     2,
       3,     3,     2,     3,     3,     3,     2,     2,     4,     4,
       4,     4,     3,     4,     5,     2,     3,     2,     2,     3,
       2,     4,     5,     5,     5,     5,     5,     5,     2,     5,
       2,     3,     3,     2,     3,     4,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     5,     5,     5,     5,
       3,     3,     3,     3,     3,     4,     3,     3,     5,     7,
       3,     3,     0,     1,     0,     1,     1,     2,     2,     2,
       3,     3,     3,     2,     4,     3,     3,     2,     5,     3,
       3,     4,     3,     2,     3,     4,     3,     5,     3,     3,
       3,     4,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
      42,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   124,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,     0,     3,    30,    31,    32,    33,
      36,    29,    27,    28,    34,    35,    37,    38,    39,     4,
       5,     6,     7,     8,    11,    12,     9,    10,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    40,    41,    77,     0,    78,    80,     0,     0,
       0,     0,     0,     0,     0,    96,   193,   192,   194,   195,
     196,    46,    47,    48,    49,    50,    51,   191,   198,   199,
     200,     0,    88,     0,     0,   197,     0,   216,   215,   217,
     218,   219,   214,   213,   221,   222,   223,     0,    90,     0,
       0,   220,     0,   214,     0,    93,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   122,   122,   122,
     122,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     133,   128,     0,   137,     0,     0,     0,     0,     0,   129,
     143,   124,     0,     0,     0,   125,   126,     0,     0,     0,
       0,    44,    45,     0,     0,    59,     0,     0,     0,     0,
      56,     0,    62,     0,     0,     0,     0,    66,     0,     0,
      67,     0,    58,     0,     0,     0,    97,    75,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   189,     0,     1,     2,
      79,     0,     0,     0,     0,     0,     0,     0,   211,   210,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    92,   235,   234,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    91,     0,     0,    94,   130,   131,
     132,   112,    99,   101,   100,   102,   103,   104,   123,   122,
     122,   122,   122,   111,   110,   105,   113,   116,     0,   114,
       0,   117,   120,   121,   135,     0,   136,     0,   139,     0,
     140,   142,     0,   146,     0,   144,     0,   127,   148,   149,
     150,     0,    60,    61,    53,    52,    54,    55,    63,    65,
      64,     0,     0,     0,     0,    57,     0,    72,     0,    98,
      76,   153,   154,   155,   184,   162,   156,   157,   158,   159,
     160,   161,   185,   152,   177,   182,   178,   167,   168,   179,
     183,   164,   165,   166,   169,   171,   170,   173,   174,   163,
     172,   180,   181,   186,   175,   176,   188,   187,   190,     0,
       0,     0,     0,     0,     0,    81,   212,   205,   206,   201,
     202,   207,   208,   203,   204,   209,     0,   236,   229,   230,
     225,   226,   231,   232,   227,   228,   233,   224,    95,     0,
       0,     0,     0,   115,     0,   134,     0,   141,   145,     0,
     226,   151,    68,    70,    69,    71,    73,     0,    84,    83,
      85,    86,    87,    82,    89,   106,   107,   108,   109,     0,
     118,   138,   147,    74,     0,   119
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    44,    45,   183,   131,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,   299,   174,   175,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,   116,   176
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -130
static const short yypact[] =
{
     667,   -81,   -80,   -77,     9,   -73,    80,   336,   302,   -47,
     -24,   -23,   514,   -45,   -44,   319,   -20,    16,   -28,   -40,
     -39,   191,   -36,   -12,   -11,    15,    15,   -33,    -9,    -5,
      20,    22,   -15,   -69,    -6,   -55,   -75,   -27,   -57,   -14,
      10,   742,   475,  -130,   592,  -130,  -130,  -130,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,    -8,  -130,  -130,   -13,     1,
       2,     3,     6,     7,    15,  -130,  -130,  -130,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,
    -130,   492,  -130,   492,   492,  -130,   848,  -130,  -130,  -130,
    -130,  -130,     5,  -130,  -130,  -130,  -130,   475,  -130,   475,
     475,  -130,   125,  -130,   475,  -130,   411,    12,    17,    19,
      21,    36,    37,    39,    43,    44,    45,    33,    33,    33,
      33,    47,    58,    59,    67,    71,   -56,   -31,    73,    77,
    -130,  -130,   364,  -130,   531,    61,    78,   -49,    82,  -130,
    -130,   475,    98,   112,   114,  -130,    38,   123,   124,   126,
     102,  -130,  -130,   128,   129,  -130,   131,   132,   133,   134,
    -130,   140,  -130,   141,   143,   167,   168,  -130,   169,   170,
    -130,   150,  -130,   -48,    42,   151,  -130,  -130,   152,   153,
     155,   156,   157,   158,   182,   184,   185,   186,   187,   188,
     189,   190,   192,   193,   207,   208,   209,   211,   212,   224,
     225,   226,   227,   228,   229,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,  -130,   734,  -130,  -130,
    -130,   475,   475,   475,   475,   475,   214,   252,  -130,  -130,
     110,   492,   492,   492,   492,   492,   492,   492,   492,   492,
     475,  -130,    14,    14,     4,   475,   475,   475,   475,   475,
     475,   475,   475,   475,  -130,   475,   752,  -130,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,    33,
      33,    33,    33,  -130,  -130,  -130,  -130,  -130,   254,  -130,
     278,  -130,  -130,  -130,  -130,   764,  -130,   279,  -130,   269,
    -130,  -130,   271,  -130,   295,  -130,   475,   860,  -130,  -130,
    -130,   273,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,
    -130,   274,   275,   276,   277,  -130,   280,  -130,   301,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,   776,
     788,   800,   812,   824,   281,  -130,  -130,  -130,  -130,   113,
     113,   113,   113,  -130,  -130,  -130,   836,  -130,    14,    14,
      79,    79,    79,    79,    14,    14,    14,   860,  -130,   282,
     283,   284,   285,  -130,   -30,  -130,   286,  -130,  -130,   288,
      79,  -130,  -130,  -130,  -130,  -130,  -130,   289,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,   316,
    -130,  -130,  -130,  -130,   298,  -130
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -130,  -130,   360,   -21,    18,  -130,  -130,  -130,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -129,   237,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -104,    -7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned short yytable[] =
{
     132,   136,   449,   205,   308,   184,    85,   258,   164,   259,
     260,   193,    88,    89,    90,    91,    92,   191,   201,   300,
     301,   302,   198,   199,   115,   177,   167,   168,    84,    86,
     181,   182,    87,   203,   202,   247,    95,   319,   346,   137,
     192,   117,   118,   119,   120,   121,   101,   102,   103,   104,
     105,   106,   206,   309,   200,   194,   204,   178,   310,    93,
     320,   347,   138,   139,   160,   161,   165,    94,   166,   169,
     170,   195,   196,   257,   179,   180,   185,   186,   311,   450,
     188,   187,   189,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   190,   207,   208,   251,   133,   275,
     276,   250,   348,   197,   277,   278,   279,   280,   281,   282,
     283,   252,   253,   254,   271,   285,   255,   256,   407,   298,
     272,   288,   273,   274,   123,   285,   289,   286,   290,   115,
     291,   115,   115,   275,   276,   124,   125,   126,   277,   326,
     279,   280,   281,   282,   283,   292,   293,   317,   294,   285,
     129,   130,   295,   296,   297,   315,   303,   397,   398,   399,
     400,   401,   402,   403,   404,   405,   107,   304,   305,   327,
     419,   420,   421,   422,   275,   276,   306,   108,   109,   110,
     307,   111,   312,   281,   282,   283,   313,   318,   331,   112,
     285,   321,   113,   114,   117,   118,   119,   120,   121,   101,
     102,   103,   104,   105,   106,   261,   262,   323,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   267,   268,   269,
     275,   276,   324,   325,   396,   277,   278,   279,   280,   281,
     282,   283,   328,   329,   284,   330,   285,   332,   333,   171,
     334,   335,   336,   337,   389,   390,   391,   392,   393,   338,
     339,   133,   340,   341,   342,   343,   344,   172,   173,   345,
     349,   350,   351,   406,   352,   353,   354,   355,   408,   409,
     410,   411,   412,   413,   414,   415,   416,   123,   417,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   124,   125,
     126,   356,   127,   357,   358,   359,   360,   361,   362,   363,
     394,   364,   365,   129,   130,   117,   118,   119,   120,   121,
     101,   102,   103,   104,   105,   106,   366,   367,   368,   430,
     369,   370,   117,   118,   119,   120,   121,   101,   102,   103,
     104,   105,   106,   371,   372,   373,   374,   375,   376,   117,
     118,   119,   120,   121,   101,   102,   103,   104,   105,   106,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   395,   133,   423,   424,   426,   162,   117,   118,   119,
     120,   121,   101,   102,   103,   104,   105,   106,   427,   133,
     428,   429,   431,   432,   433,   434,   435,   437,   123,   436,
     443,   445,   446,   447,   448,   451,   122,   452,   453,   124,
     125,   126,   454,   127,   249,   123,   134,   455,   322,     0,
       0,   135,     0,     0,   129,   130,   124,   125,   126,     0,
     127,     0,   123,     0,   133,     0,     0,     0,   163,     0,
       0,   129,   130,   124,   125,   126,     0,   127,     0,     0,
       0,     0,     0,     0,     0,   128,     0,     0,   129,   130,
     123,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   124,   125,   126,     0,   127,     0,     0,     0,     0,
       0,     0,     0,   314,     0,     0,   129,   130,   117,   118,
     119,   120,   121,   101,   102,   103,   104,   105,   106,     0,
       0,     0,     0,     0,     0,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   275,   276,     0,     0,
       0,   277,   278,   279,   280,   281,   282,   283,     0,     0,
     287,     0,   285,     0,     0,     0,     0,     0,   140,     0,
       0,     0,     0,     0,     0,   133,     0,     0,     0,     0,
     141,     0,     0,     0,     0,     0,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
       0,   123,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   124,   125,   126,     0,   127,     0,   107,     0,
       0,     0,     0,     0,     0,     0,     0,   129,   130,   108,
     109,   110,   248,   111,     0,   156,     0,     0,     0,     0,
       0,   157,   158,     0,   113,   114,   159,     0,     0,     1,
       2,     3,     0,     4,     0,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,     0,   275,   276,     0,     0,
       0,   277,   278,   279,   280,   281,   282,   283,    15,     0,
     316,    16,   285,    17,     0,    18,     0,     0,    19,    20,
      21,    22,     0,     0,    23,    24,    25,    26,     0,     0,
      27,    28,    29,    30,    31,    32,    33,    34,    35,     0,
       0,    36,    37,     0,    38,     0,    39,    40,     0,     0,
       0,     0,    41,    42,     1,     2,     3,     0,     4,     0,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
       0,    43,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15,     0,     0,    16,     0,    17,     0,
      18,     0,     0,    19,    20,    21,    22,     0,     0,    23,
      24,    25,    26,     0,     0,    27,    28,    29,    30,    31,
      32,    33,    34,    35,     0,     0,    36,    37,     0,    38,
       0,    39,    40,     0,     0,     0,     0,    41,    42,   209,
     210,   211,     0,   212,     0,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,     0,    43,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   223,     0,
       0,   224,     0,     0,     0,     0,     0,     0,   225,   226,
     227,   228,     0,     0,     0,   229,   230,   231,     0,     0,
     232,   233,   234,   235,   236,   237,   238,   239,   240,     0,
       0,   241,   242,     0,   243,     0,     0,     0,     0,   275,
     276,     0,   244,   245,   277,   278,   279,   280,   281,   282,
     283,     0,     0,   388,     0,   285,     0,   275,   276,     0,
       0,   246,   277,   278,   279,   280,   281,   282,   283,   275,
     276,   418,     0,   285,   277,   278,   279,   280,   281,   282,
     283,   275,   276,   425,     0,   285,   277,   278,   279,   280,
     281,   282,   283,   275,   276,   438,     0,   285,   277,   278,
     279,   280,   281,   282,   283,   275,   276,   439,     0,   285,
     277,   278,   279,   280,   281,   282,   283,   275,   276,   440,
       0,   285,   277,   278,   279,   280,   281,   282,   283,   275,
     276,   441,     0,   285,   277,   278,   279,   280,   281,   282,
     283,   275,   276,   442,     0,   285,   277,   278,   279,   280,
     281,   282,   283,   261,   262,   444,     0,   285,   263,   264,
     265,   266,   267,   268,   269,   275,   276,     0,     0,   270,
     277,   278,   279,   280,   281,   282,   283,     0,     0,     0,
       0,   285
};

static const short yycheck[] =
{
       7,     8,    32,    60,    60,    26,    86,   111,    15,   113,
     114,    17,     3,     4,     5,     6,     7,    86,    93,   148,
     149,   150,    77,    78,     6,    61,    54,    55,   109,   109,
      15,    16,   109,    60,   109,    42,   109,    86,    86,    86,
     109,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,   109,   109,   109,    61,    83,    93,    89,    50,
     109,   109,    86,    86,   109,   109,    86,    58,    52,   109,
     109,    77,    78,    94,    86,    86,   109,    86,   109,   109,
      60,    86,    60,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,   109,   109,    86,   110,    60,    95,
      96,   109,    60,   109,   100,   101,   102,   103,   104,   105,
     106,   110,   110,   110,   109,   111,   110,   110,   114,    86,
     127,   109,   129,   130,    86,   111,   109,   134,   109,   111,
     109,   113,   114,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   109,   109,    86,   109,   111,
     112,   113,   109,   109,   109,   162,   109,   261,   262,   263,
     264,   265,   266,   267,   268,   269,    86,   109,   109,   176,
     299,   300,   301,   302,    95,    96,   109,    97,    98,    99,
     109,   101,   109,   104,   105,   106,   109,   109,    86,   109,
     111,   109,   112,   113,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    95,    96,   109,    95,    96,
     100,   101,   102,   103,   104,   105,   106,   104,   105,   106,
      95,    96,   110,   109,   114,   100,   101,   102,   103,   104,
     105,   106,   109,   109,   109,   109,   111,   109,   109,    48,
     109,   109,   109,   109,   251,   252,   253,   254,   255,   109,
     109,    60,   109,    86,    86,    86,    86,    66,    67,   109,
     109,   109,   109,   270,   109,   109,   109,   109,   275,   276,
     277,   278,   279,   280,   281,   282,   283,    86,   285,   261,
     262,   263,   264,   265,   266,   267,   268,   269,    97,    98,
      99,   109,   101,   109,   109,   109,   109,   109,   109,   109,
      86,   109,   109,   112,   113,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,   109,   109,   109,   326,
     109,   109,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,   109,   109,   109,   109,   109,   109,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,    60,   109,    86,    86,    47,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,   109,    60,
     109,    86,   109,   109,   109,   109,   109,    86,    86,   109,
     109,   109,   109,   109,   109,   109,    60,   109,   109,    97,
      98,    99,    86,   101,    44,    86,   104,   109,   171,    -1,
      -1,   109,    -1,    -1,   112,   113,    97,    98,    99,    -1,
     101,    -1,    86,    -1,    60,    -1,    -1,    -1,   109,    -1,
      -1,   112,   113,    97,    98,    99,    -1,   101,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   109,    -1,    -1,   112,   113,
      86,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    97,    98,    99,    -1,   101,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   109,    -1,    -1,   112,   113,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    95,    96,    -1,    -1,
      -1,   100,   101,   102,   103,   104,   105,   106,    -1,    -1,
     109,    -1,   111,    -1,    -1,    -1,    -1,    -1,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    60,    -1,    -1,    -1,    -1,
      26,    -1,    -1,    -1,    -1,    -1,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      -1,    86,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    97,    98,    99,    -1,   101,    -1,    86,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,   113,    97,
      98,    99,     0,   101,    -1,    81,    -1,    -1,    -1,    -1,
      -1,    87,    88,    -1,   112,   113,    92,    -1,    -1,    17,
      18,    19,    -1,    21,    -1,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    95,    96,    -1,    -1,
      -1,   100,   101,   102,   103,   104,   105,   106,    46,    -1,
     109,    49,   111,    51,    -1,    53,    -1,    -1,    56,    57,
      58,    59,    -1,    -1,    62,    63,    64,    65,    -1,    -1,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    -1,
      -1,    79,    80,    -1,    82,    -1,    84,    85,    -1,    -1,
      -1,    -1,    90,    91,    17,    18,    19,    -1,    21,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    -1,    49,    -1,    51,    -1,
      53,    -1,    -1,    56,    57,    58,    59,    -1,    -1,    62,
      63,    64,    65,    -1,    -1,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    -1,    -1,    79,    80,    -1,    82,
      -1,    84,    85,    -1,    -1,    -1,    -1,    90,    91,    17,
      18,    19,    -1,    21,    -1,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,   109,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,
      -1,    49,    -1,    -1,    -1,    -1,    -1,    -1,    56,    57,
      58,    59,    -1,    -1,    -1,    63,    64,    65,    -1,    -1,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    -1,
      -1,    79,    80,    -1,    82,    -1,    -1,    -1,    -1,    95,
      96,    -1,    90,    91,   100,   101,   102,   103,   104,   105,
     106,    -1,    -1,   109,    -1,   111,    -1,    95,    96,    -1,
      -1,   109,   100,   101,   102,   103,   104,   105,   106,    95,
      96,   109,    -1,   111,   100,   101,   102,   103,   104,   105,
     106,    95,    96,   109,    -1,   111,   100,   101,   102,   103,
     104,   105,   106,    95,    96,   109,    -1,   111,   100,   101,
     102,   103,   104,   105,   106,    95,    96,   109,    -1,   111,
     100,   101,   102,   103,   104,   105,   106,    95,    96,   109,
      -1,   111,   100,   101,   102,   103,   104,   105,   106,    95,
      96,   109,    -1,   111,   100,   101,   102,   103,   104,   105,
     106,    95,    96,   109,    -1,   111,   100,   101,   102,   103,
     104,   105,   106,    95,    96,   109,    -1,   111,   100,   101,
     102,   103,   104,   105,   106,    95,    96,    -1,    -1,   111,
     100,   101,   102,   103,   104,   105,   106,    -1,    -1,    -1,
      -1,   111
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    17,    18,    19,    21,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    46,    49,    51,    53,    56,
      57,    58,    59,    62,    63,    64,    65,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    79,    80,    82,    84,
      85,    90,    91,   109,   116,   117,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   109,    86,   109,   109,     3,     4,
       5,     6,     7,    50,    58,   109,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    86,    97,    98,
      99,   101,   109,   112,   113,   119,   161,     3,     4,     5,
       6,     7,    60,    86,    97,    98,    99,   101,   109,   112,
     113,   119,   162,    60,   104,   109,   162,    86,    86,    86,
      14,    26,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    81,    87,    88,    92,
     109,   109,    47,   109,   162,    86,    52,    54,    55,   109,
     109,    48,    66,    67,   142,   143,   162,    61,    93,    86,
      86,    15,    16,   118,   118,   109,    86,    86,    60,    60,
     109,    86,   109,    17,    61,    77,    78,   109,    77,    78,
     109,    93,   109,    60,    83,    60,   109,   109,    86,    17,
      18,    19,    21,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    46,    49,    56,    57,    58,    59,    63,
      64,    65,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    79,    80,    82,    90,    91,   109,   162,     0,   117,
     109,   110,   110,   110,   110,   110,   110,   118,   161,   161,
     161,    95,    96,   100,   101,   102,   103,   104,   105,   106,
     111,   109,   162,   162,   162,    95,    96,   100,   101,   102,
     103,   104,   105,   106,   109,   111,   162,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,    86,   141,
     141,   141,   141,   109,   109,   109,   109,   109,    60,   109,
      89,   109,   109,   109,   109,   162,   109,    86,   109,    86,
     109,   109,   142,   109,   110,   109,   101,   162,   109,   109,
     109,    86,   109,   109,   109,   109,   109,   109,   109,   109,
     109,    86,    86,    86,    86,   109,    86,   109,    60,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   162,
     162,   162,   162,   162,    86,   109,   114,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   162,   114,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   109,   141,
     141,   141,   141,   109,    86,   109,    86,   109,   109,    86,
     162,   109,   109,   109,   109,   109,   109,    86,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,    32,
     109,   109,   109,   109,    86,   109
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
#line 180 "parser.y"
    {
      }
    break;

  case 45:
#line 187 "parser.y"
    { yyval.bval=yyvsp[0].bval; }
    break;

  case 51:
#line 197 "parser.y"
    { yyval.uval=yyvsp[0].uval; }
    break;

  case 52:
#line 202 "parser.y"
    {
          bx_dbg_timebp_command(0, yyvsp[-1].uval);
          free(yyvsp[-2].sval);
      }
    break;

  case 53:
#line 207 "parser.y"
    {
          bx_dbg_timebp_command(1, yyvsp[-1].uval);
          free(yyvsp[-2].sval);
      }
    break;

  case 54:
#line 215 "parser.y"
    {
          bx_dbg_record_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 55:
#line 223 "parser.y"
    {
          bx_dbg_playback_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 56:
#line 231 "parser.y"
    {
          bx_dbg_modebp_command();
          free(yyvsp[-1].sval);
      }
    break;

  case 57:
#line 239 "parser.y"
    {
          bx_dbg_show_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 58:
#line 244 "parser.y"
    {
          bx_dbg_show_command(0);
          free(yyvsp[-1].sval);
      }
    break;

  case 59:
#line 252 "parser.y"
    {
          bx_dbg_ptime_command();
          free(yyvsp[-1].sval);
      }
    break;

  case 60:
#line 260 "parser.y"
    {
          bx_dbg_trace_command(yyvsp[-1].bval);
          free(yyvsp[-2].sval);
      }
    break;

  case 61:
#line 268 "parser.y"
    {
          bx_dbg_trace_reg_command(yyvsp[-1].bval);
          free(yyvsp[-2].sval);
      }
    break;

  case 62:
#line 276 "parser.y"
    {
          bx_dbg_print_stack_command(16);
          free(yyvsp[-1].sval);
      }
    break;

  case 63:
#line 281 "parser.y"
    {
          bx_dbg_print_stack_command(yyvsp[-1].uval);
          free(yyvsp[-2].sval);
      }
    break;

  case 64:
#line 289 "parser.y"
    {
          watchpoint_continue = 0;
          dbg_printf("Will stop on watch points\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 65:
#line 295 "parser.y"
    {
          watchpoint_continue = 1;
          dbg_printf("Will not stop on watch points (they will still be logged)\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 66:
#line 301 "parser.y"
    {
          bx_dbg_watch(-1, 0);
          free(yyvsp[-1].sval);
      }
    break;

  case 67:
#line 306 "parser.y"
    {
          bx_dbg_unwatch(-1, 0);
          free(yyvsp[-1].sval);
      }
    break;

  case 68:
#line 311 "parser.y"
    {
          bx_dbg_watch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
      }
    break;

  case 69:
#line 316 "parser.y"
    {
          bx_dbg_unwatch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
      }
    break;

  case 70:
#line 321 "parser.y"
    {
          bx_dbg_watch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
      }
    break;

  case 71:
#line 326 "parser.y"
    {
          bx_dbg_unwatch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
      }
    break;

  case 72:
#line 334 "parser.y"
    {
        bx_dbg_symbol_command(yyvsp[-1].sval, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 73:
#line 339 "parser.y"
    {
        bx_dbg_symbol_command(yyvsp[-2].sval, 0, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
      }
    break;

  case 74:
#line 344 "parser.y"
    {
        bx_dbg_symbol_command(yyvsp[-2].sval, 1, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
      }
    break;

  case 75:
#line 352 "parser.y"
    {
        bx_dbg_where_command();
        free(yyvsp[-1].sval);
      }
    break;

  case 76:
#line 360 "parser.y"
    {
        bx_dbg_print_string_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
      }
    break;

  case 77:
#line 368 "parser.y"
    {
        bx_dbg_continue_command();
        free(yyvsp[-1].sval);
      }
    break;

  case 78:
#line 376 "parser.y"
    {
        bx_dbg_stepN_command(1);
        free(yyvsp[-1].sval);
      }
    break;

  case 79:
#line 381 "parser.y"
    {
        bx_dbg_stepN_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
      }
    break;

  case 80:
#line 389 "parser.y"
    {
        bx_dbg_step_over_command();
        free(yyvsp[-1].sval);
      }
    break;

  case 81:
#line 397 "parser.y"
    {
        bx_dbg_set_auto_disassemble(yyvsp[-1].bval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
      }
    break;

  case 82:
#line 402 "parser.y"
    {
        bx_dbg_set_symbol_command(yyvsp[-3].sval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
      }
    break;

  case 83:
#line 407 "parser.y"
    { 
        bx_dbg_set_reg8l_value(yyvsp[-3].uval, yyvsp[-1].uval);
      }
    break;

  case 84:
#line 411 "parser.y"
    { 
        bx_dbg_set_reg8h_value(yyvsp[-3].uval, yyvsp[-1].uval);
      }
    break;

  case 85:
#line 415 "parser.y"
    { 
        bx_dbg_set_reg16_value(yyvsp[-3].uval, yyvsp[-1].uval);
      }
    break;

  case 86:
#line 419 "parser.y"
    { 
        bx_dbg_set_reg32_value(yyvsp[-3].uval, yyvsp[-1].uval);
      }
    break;

  case 87:
#line 423 "parser.y"
    { 
        bx_dbg_set_reg64_value(yyvsp[-3].uval, yyvsp[-1].uval);
      }
    break;

  case 88:
#line 430 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free(yyvsp[-1].sval);
      }
    break;

  case 89:
#line 435 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkRegular, yyvsp[-3].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
      }
    break;

  case 90:
#line 440 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free(yyvsp[-1].sval);
      }
    break;

  case 91:
#line 445 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkRegular, yyvsp[-1].uval);
        free(yyvsp[-2].sval);
      }
    break;

  case 92:
#line 450 "parser.y"
    {
        bx_dbg_lbreakpoint_symbol_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval);free(yyvsp[-1].sval);
      }
    break;

  case 93:
#line 455 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free(yyvsp[-1].sval);
      }
    break;

  case 94:
#line 460 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, yyvsp[-1].uval);
        free(yyvsp[-2].sval);
      }
    break;

  case 95:
#line 465 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
      }
    break;

  case 96:
#line 473 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free(yyvsp[-1].sval);
      }
    break;

  case 97:
#line 481 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free(yyvsp[-1].sval);
      }
    break;

  case 98:
#line 486 "parser.y"
    {
        bx_dbg_info_symbols_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval);free(yyvsp[-1].sval);
      }
    break;

  case 99:
#line 494 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 100:
#line 499 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 101:
#line 504 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 102:
#line 509 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 103:
#line 514 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 104:
#line 519 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS | BX_INFO_FPU_REGS | BX_INFO_SSE_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 105:
#line 524 "parser.y"
    {
        bx_dbg_info_dirty_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 106:
#line 529 "parser.y"
    {
        bx_dbg_info_idt_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
      }
    break;

  case 107:
#line 534 "parser.y"
    {
        bx_dbg_info_ivt_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
      }
    break;

  case 108:
#line 539 "parser.y"
    {
        bx_dbg_info_gdt_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
      }
    break;

  case 109:
#line 544 "parser.y"
    {
        bx_dbg_info_ldt_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
      }
    break;

  case 110:
#line 549 "parser.y"
    {
        bx_dbg_dump_table();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 111:
#line 554 "parser.y"
    {
        bx_dbg_info_tss_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 112:
#line 559 "parser.y"
    {
        bx_dbg_info_flags();
        free(yyvsp[-2].sval);
      }
    break;

  case 113:
#line 564 "parser.y"
    {
        bx_dbg_info_linux_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 114:
#line 569 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 115:
#line 574 "parser.y"
    {
        bx_dbg_info_symbols_command(yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 116:
#line 579 "parser.y"
    {
        bx_dbg_info_control_regs_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 117:
#line 584 "parser.y"
    {
        bx_dbg_info_ne2k(-1, -1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 118:
#line 589 "parser.y"
    {
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-1].uval, -1);
      }
    break;

  case 119:
#line 594 "parser.y"
    {
        free(yyvsp[-6].sval); free(yyvsp[-5].sval); free(yyvsp[-4].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-3].uval, yyvsp[-1].uval);
      }
    break;

  case 120:
#line 599 "parser.y"
    {
        bx_dbg_info_pic();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 121:
#line 604 "parser.y"
    {
        bx_dbg_info_vga();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 122:
#line 611 "parser.y"
    { yyval.uval = EMPTY_ARG; }
    break;

  case 124:
#line 615 "parser.y"
    { yyval.uval_range = make_num_range (EMPTY_ARG, EMPTY_ARG); }
    break;

  case 126:
#line 620 "parser.y"
    {
      yyval.uval_range = make_num_range (yyvsp[0].uval, yyvsp[0].uval);
    }
    break;

  case 127:
#line 624 "parser.y"
    {
      yyval.uval_range = make_num_range (yyvsp[-1].uval, yyvsp[0].uval);
    }
    break;

  case 128:
#line 630 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free(yyvsp[-1].sval);
      }
    break;

  case 129:
#line 638 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free(yyvsp[-1].sval);
      }
    break;

  case 130:
#line 646 "parser.y"
    {
        bx_dbg_del_breakpoint_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
      }
    break;

  case 131:
#line 654 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command(yyvsp[-1].uval, 1);
        free(yyvsp[-2].sval);
      }
    break;

  case 132:
#line 661 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command(yyvsp[-1].uval, 0);
        free(yyvsp[-2].sval);
      }
    break;

  case 133:
#line 669 "parser.y"
    {
        bx_dbg_quit_command();
        free(yyvsp[-1].sval);
      }
    break;

  case 134:
#line 677 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].uval, 1);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
      }
    break;

  case 135:
#line 682 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 136:
#line 687 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-2].sval, NULL,0, yyvsp[-1].uval, 1);
        free(yyvsp[-2].sval);
      }
    break;

  case 137:
#line 692 "parser.y"
    {
        bx_dbg_examine_command(yyvsp[-1].sval, NULL,0, 0, 0);
        free(yyvsp[-1].sval);
      }
    break;

  case 138:
#line 700 "parser.y"
    {
        bx_dbg_setpmem_command(yyvsp[-3].uval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
      }
    break;

  case 139:
#line 708 "parser.y"
    {
        bx_dbg_query_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 140:
#line 716 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 141:
#line 721 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-2].sval, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
      }
    break;

  case 142:
#line 726 "parser.y"
    {
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 143:
#line 734 "parser.y"
    {
        bx_dbg_set_cpu_command();
        free(yyvsp[-1].sval);
      }
    break;

  case 144:
#line 742 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, yyvsp[-1].uval_range);
        free(yyvsp[-2].sval);
      }
    break;

  case 145:
#line 747 "parser.y"
    {
        bx_dbg_disassemble_command(yyvsp[-2].sval, yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
      }
    break;

  case 146:
#line 752 "parser.y"
    {
        bx_dbg_disassemble_switch_mode();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 147:
#line 757 "parser.y"
    {
        bx_dbg_set_disassemble_size(yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
      }
    break;

  case 148:
#line 765 "parser.y"
    {
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 149:
#line 770 "parser.y"
    {
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
      }
    break;

  case 150:
#line 778 "parser.y"
    {
        bx_dbg_doit_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
      }
    break;

  case 151:
#line 786 "parser.y"
    {
        bx_dbg_crc_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
      }
    break;

  case 152:
#line 794 "parser.y"
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 153:
#line 799 "parser.y"
    {
         dbg_printf("c|cont|continue - continue executing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 154:
#line 804 "parser.y"
    {
         dbg_printf("s|step|stepi [count] - execute #count instructions (default is one instruction)\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 155:
#line 809 "parser.y"
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 156:
#line 814 "parser.y"
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 157:
#line 819 "parser.y"
    {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 158:
#line 824 "parser.y"
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 159:
#line 829 "parser.y"
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 160:
#line 834 "parser.y"
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 161:
#line 839 "parser.y"
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 162:
#line 844 "parser.y"
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 163:
#line 849 "parser.y"
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 164:
#line 854 "parser.y"
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 165:
#line 859 "parser.y"
    {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 166:
#line 865 "parser.y"
    {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 167:
#line 871 "parser.y"
    {
         dbg_printf("dump_cpu - dump complete cpu state\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 168:
#line 876 "parser.y"
    {
         dbg_printf("set_cpu - set complete cpu state\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 169:
#line 881 "parser.y"
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 170:
#line 886 "parser.y"
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 171:
#line 891 "parser.y"
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 172:
#line 896 "parser.y"
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 173:
#line 901 "parser.y"
    {
         dbg_printf("record <filename> - record console input to file filename\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 174:
#line 906 "parser.y"
    {
         dbg_printf("playback <filename> - playback console input from file filename\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 175:
#line 911 "parser.y"
    {
         dbg_printf("load-symbols [global] <filename> [offset] - load symbols from file\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 176:
#line 916 "parser.y"
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 177:
#line 921 "parser.y"
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 178:
#line 926 "parser.y"
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 179:
#line 931 "parser.y"
    {
         dbg_printf("u|disasm|disassemble [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm|disassemble switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm|disassemble size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 180:
#line 940 "parser.y"
    {
         dbg_printf("watch - print current watch point status\n");
         dbg_printf("watch stop - stop simulation when a watchpoint is encountred\n");
         dbg_printf("watch continue - do not stop the simulation when watch point is encountred\n");
         dbg_printf("watch read addr - insert a read watch point at physical address addr\n");
         dbg_printf("watch write addr - insert a write watch point at physical address addr\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 181:
#line 949 "parser.y"
    {
         dbg_printf("unwatch - remove all watch points\n");
         dbg_printf("unwatch read addr - remove a read watch point at physical address addr\n");
         dbg_printf("unwatch write addr - remove a write watch point at physical address addr\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 182:
#line 956 "parser.y"
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

  case 183:
#line 969 "parser.y"
    {
         dbg_printf("instrument start - calls bx_instr_start() callback\n");
         dbg_printf("instrument stop  - calls bx_instr_stop () callback\n");
         dbg_printf("instrument reset - calls bx_instr_reset() callback\n");
         dbg_printf("instrument print - calls bx_instr_print() callback\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 184:
#line 977 "parser.y"
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

  case 185:
#line 987 "parser.y"
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

  case 186:
#line 1008 "parser.y"
    {
         dbg_printf("show <command> - toggles show symbolic info (calls to begin with)\n");
         dbg_printf("show - shows current show mode\n");
         dbg_printf("show mode - show, when processor switch mode\n");
         dbg_printf("show int - show, when interrupt is happens\n");
         dbg_printf("show call - show, when call is happens\n");
         dbg_printf("show off - toggles off symbolic info\n");
         dbg_printf("show dbg-all - turn on all show flags\n");
         dbg_printf("show dbg-none - turn off all show flags\n");
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 187:
#line 1020 "parser.y"
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

  case 188:
#line 1030 "parser.y"
    {
         bx_dbg_print_help();
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
       }
    break;

  case 189:
#line 1035 "parser.y"
    {
         bx_dbg_print_help();
         free(yyvsp[-1].sval);
       }
    break;

  case 190:
#line 1043 "parser.y"
    {
     bx_dbg_calc_command(yyvsp[-1].uval);
     free(yyvsp[-2].sval);
   }
    break;

  case 191:
#line 1060 "parser.y"
    { yyval.uval = yyvsp[0].uval; }
    break;

  case 192:
#line 1061 "parser.y"
    { yyval.uval = bx_dbg_get_reg8l_value(yyvsp[0].uval); }
    break;

  case 193:
#line 1062 "parser.y"
    { yyval.uval = bx_dbg_get_reg8h_value(yyvsp[0].uval); }
    break;

  case 194:
#line 1063 "parser.y"
    { yyval.uval = bx_dbg_get_reg16_value(yyvsp[0].uval); }
    break;

  case 195:
#line 1064 "parser.y"
    { yyval.uval = bx_dbg_get_reg32_value(yyvsp[0].uval); }
    break;

  case 196:
#line 1065 "parser.y"
    { yyval.uval = bx_dbg_get_reg64_value(yyvsp[0].uval); }
    break;

  case 197:
#line 1066 "parser.y"
    { yyval.uval = bx_dbg_get_selector_value(yyvsp[0].uval); }
    break;

  case 198:
#line 1067 "parser.y"
    { yyval.uval = bx_dbg_get_ip (); }
    break;

  case 199:
#line 1068 "parser.y"
    { yyval.uval = bx_dbg_get_eip(); }
    break;

  case 200:
#line 1069 "parser.y"
    { yyval.uval = bx_dbg_get_instruction_pointer(); }
    break;

  case 201:
#line 1070 "parser.y"
    { yyval.uval = yyvsp[-2].uval + yyvsp[0].uval; }
    break;

  case 202:
#line 1071 "parser.y"
    { yyval.uval = yyvsp[-2].uval - yyvsp[0].uval; }
    break;

  case 203:
#line 1072 "parser.y"
    { yyval.uval = yyvsp[-2].uval * yyvsp[0].uval; }
    break;

  case 204:
#line 1073 "parser.y"
    { yyval.uval = yyvsp[-2].uval / yyvsp[0].uval; }
    break;

  case 205:
#line 1074 "parser.y"
    { yyval.uval = yyvsp[-2].uval >> yyvsp[0].uval; }
    break;

  case 206:
#line 1075 "parser.y"
    { yyval.uval = yyvsp[-2].uval << yyvsp[0].uval; }
    break;

  case 207:
#line 1076 "parser.y"
    { yyval.uval = yyvsp[-2].uval | yyvsp[0].uval; }
    break;

  case 208:
#line 1077 "parser.y"
    { yyval.uval = yyvsp[-2].uval ^ yyvsp[0].uval; }
    break;

  case 209:
#line 1078 "parser.y"
    { yyval.uval = yyvsp[-2].uval & yyvsp[0].uval; }
    break;

  case 210:
#line 1079 "parser.y"
    { yyval.uval = !yyvsp[0].uval; }
    break;

  case 211:
#line 1080 "parser.y"
    { yyval.uval = -yyvsp[0].uval; }
    break;

  case 212:
#line 1081 "parser.y"
    { yyval.uval = yyvsp[-1].uval; }
    break;

  case 213:
#line 1086 "parser.y"
    { yyval.uval = yyvsp[0].uval; }
    break;

  case 214:
#line 1087 "parser.y"
    { yyval.uval = bx_dbg_get_symbol_value(yyvsp[0].sval); free(yyvsp[0].sval);}
    break;

  case 215:
#line 1088 "parser.y"
    { yyval.uval = bx_dbg_get_reg8l_value(yyvsp[0].uval); }
    break;

  case 216:
#line 1089 "parser.y"
    { yyval.uval = bx_dbg_get_reg8h_value(yyvsp[0].uval); }
    break;

  case 217:
#line 1090 "parser.y"
    { yyval.uval = bx_dbg_get_reg16_value(yyvsp[0].uval); }
    break;

  case 218:
#line 1091 "parser.y"
    { yyval.uval = bx_dbg_get_reg32_value(yyvsp[0].uval); }
    break;

  case 219:
#line 1092 "parser.y"
    { yyval.uval = bx_dbg_get_reg64_value(yyvsp[0].uval); }
    break;

  case 220:
#line 1093 "parser.y"
    { yyval.uval = bx_dbg_get_selector_value(yyvsp[0].uval); }
    break;

  case 221:
#line 1094 "parser.y"
    { yyval.uval = bx_dbg_get_ip (); }
    break;

  case 222:
#line 1095 "parser.y"
    { yyval.uval = bx_dbg_get_eip(); }
    break;

  case 223:
#line 1096 "parser.y"
    { yyval.uval = bx_dbg_get_instruction_pointer(); }
    break;

  case 224:
#line 1097 "parser.y"
    { yyval.uval = bx_dbg_get_laddr (yyvsp[-2].uval, yyvsp[0].uval); }
    break;

  case 225:
#line 1098 "parser.y"
    { yyval.uval = yyvsp[-2].uval + yyvsp[0].uval; }
    break;

  case 226:
#line 1099 "parser.y"
    { yyval.uval = yyvsp[-2].uval - yyvsp[0].uval; }
    break;

  case 227:
#line 1100 "parser.y"
    { yyval.uval = yyvsp[-2].uval * yyvsp[0].uval; }
    break;

  case 228:
#line 1101 "parser.y"
    { yyval.uval = (yyvsp[0].uval != 0) ? yyvsp[-2].uval / yyvsp[0].uval : 0; }
    break;

  case 229:
#line 1102 "parser.y"
    { yyval.uval = yyvsp[-2].uval >> yyvsp[0].uval; }
    break;

  case 230:
#line 1103 "parser.y"
    { yyval.uval = yyvsp[-2].uval << yyvsp[0].uval; }
    break;

  case 231:
#line 1104 "parser.y"
    { yyval.uval = yyvsp[-2].uval | yyvsp[0].uval; }
    break;

  case 232:
#line 1105 "parser.y"
    { yyval.uval = yyvsp[-2].uval ^ yyvsp[0].uval; }
    break;

  case 233:
#line 1106 "parser.y"
    { yyval.uval = yyvsp[-2].uval & yyvsp[0].uval; }
    break;

  case 234:
#line 1107 "parser.y"
    { yyval.uval = !yyvsp[0].uval; }
    break;

  case 235:
#line 1108 "parser.y"
    { yyval.uval = -yyvsp[0].uval; }
    break;

  case 236:
#line 1109 "parser.y"
    { yyval.uval = yyvsp[-1].uval; }
    break;


    }

/* Line 999 of yacc.c.  */
#line 3136 "y.tab.c"

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


#line 1112 "parser.y"


#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
