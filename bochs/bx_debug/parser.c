/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
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
     BX_TOKEN_R = 287,
     BX_TOKEN_REGS = 288,
     BX_TOKEN_CPU = 289,
     BX_TOKEN_FPU = 290,
     BX_TOKEN_SSE = 291,
     BX_TOKEN_MMX = 292,
     BX_TOKEN_IDT = 293,
     BX_TOKEN_IVT = 294,
     BX_TOKEN_GDT = 295,
     BX_TOKEN_LDT = 296,
     BX_TOKEN_TSS = 297,
     BX_TOKEN_TAB = 298,
     BX_TOKEN_ALL = 299,
     BX_TOKEN_LINUX = 300,
     BX_TOKEN_DEBUG_REGS = 301,
     BX_TOKEN_CONTROL_REGS = 302,
     BX_TOKEN_SEGMENT_REGS = 303,
     BX_TOKEN_EXAMINE = 304,
     BX_TOKEN_XFORMAT = 305,
     BX_TOKEN_DISFORMAT = 306,
     BX_TOKEN_RESTORE = 307,
     BX_TOKEN_SETPMEM = 308,
     BX_TOKEN_SYMBOLNAME = 309,
     BX_TOKEN_QUERY = 310,
     BX_TOKEN_PENDING = 311,
     BX_TOKEN_TAKE = 312,
     BX_TOKEN_DMA = 313,
     BX_TOKEN_IRQ = 314,
     BX_TOKEN_HEX = 315,
     BX_TOKEN_DISASM = 316,
     BX_TOKEN_INSTRUMENT = 317,
     BX_TOKEN_STRING = 318,
     BX_TOKEN_STOP = 319,
     BX_TOKEN_DOIT = 320,
     BX_TOKEN_CRC = 321,
     BX_TOKEN_TRACE = 322,
     BX_TOKEN_TRACEREG = 323,
     BX_TOKEN_TRACEMEM = 324,
     BX_TOKEN_SWITCH_MODE = 325,
     BX_TOKEN_SIZE = 326,
     BX_TOKEN_PTIME = 327,
     BX_TOKEN_TIMEBP_ABSOLUTE = 328,
     BX_TOKEN_TIMEBP = 329,
     BX_TOKEN_RECORD = 330,
     BX_TOKEN_PLAYBACK = 331,
     BX_TOKEN_MODEBP = 332,
     BX_TOKEN_PRINT_STACK = 333,
     BX_TOKEN_WATCH = 334,
     BX_TOKEN_UNWATCH = 335,
     BX_TOKEN_READ = 336,
     BX_TOKEN_WRITE = 337,
     BX_TOKEN_SHOW = 338,
     BX_TOKEN_LOAD_SYMBOLS = 339,
     BX_TOKEN_SYMBOLS = 340,
     BX_TOKEN_LIST_SYMBOLS = 341,
     BX_TOKEN_GLOBAL = 342,
     BX_TOKEN_WHERE = 343,
     BX_TOKEN_PRINT_STRING = 344,
     BX_TOKEN_NUMERIC = 345,
     BX_TOKEN_NE2000 = 346,
     BX_TOKEN_PIC = 347,
     BX_TOKEN_PAGE = 348,
     BX_TOKEN_HELP = 349,
     BX_TOKEN_CALC = 350,
     BX_TOKEN_VGA = 351,
     BX_TOKEN_PCI = 352,
     BX_TOKEN_COMMAND = 353,
     BX_TOKEN_GENERIC = 354,
     BX_TOKEN_RSHIFT = 355,
     BX_TOKEN_LSHIFT = 356,
     BX_TOKEN_REG_IP = 357,
     BX_TOKEN_REG_EIP = 358,
     BX_TOKEN_REG_RIP = 359,
     NEG = 360,
     NOT = 361
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
#define BX_TOKEN_R 287
#define BX_TOKEN_REGS 288
#define BX_TOKEN_CPU 289
#define BX_TOKEN_FPU 290
#define BX_TOKEN_SSE 291
#define BX_TOKEN_MMX 292
#define BX_TOKEN_IDT 293
#define BX_TOKEN_IVT 294
#define BX_TOKEN_GDT 295
#define BX_TOKEN_LDT 296
#define BX_TOKEN_TSS 297
#define BX_TOKEN_TAB 298
#define BX_TOKEN_ALL 299
#define BX_TOKEN_LINUX 300
#define BX_TOKEN_DEBUG_REGS 301
#define BX_TOKEN_CONTROL_REGS 302
#define BX_TOKEN_SEGMENT_REGS 303
#define BX_TOKEN_EXAMINE 304
#define BX_TOKEN_XFORMAT 305
#define BX_TOKEN_DISFORMAT 306
#define BX_TOKEN_RESTORE 307
#define BX_TOKEN_SETPMEM 308
#define BX_TOKEN_SYMBOLNAME 309
#define BX_TOKEN_QUERY 310
#define BX_TOKEN_PENDING 311
#define BX_TOKEN_TAKE 312
#define BX_TOKEN_DMA 313
#define BX_TOKEN_IRQ 314
#define BX_TOKEN_HEX 315
#define BX_TOKEN_DISASM 316
#define BX_TOKEN_INSTRUMENT 317
#define BX_TOKEN_STRING 318
#define BX_TOKEN_STOP 319
#define BX_TOKEN_DOIT 320
#define BX_TOKEN_CRC 321
#define BX_TOKEN_TRACE 322
#define BX_TOKEN_TRACEREG 323
#define BX_TOKEN_TRACEMEM 324
#define BX_TOKEN_SWITCH_MODE 325
#define BX_TOKEN_SIZE 326
#define BX_TOKEN_PTIME 327
#define BX_TOKEN_TIMEBP_ABSOLUTE 328
#define BX_TOKEN_TIMEBP 329
#define BX_TOKEN_RECORD 330
#define BX_TOKEN_PLAYBACK 331
#define BX_TOKEN_MODEBP 332
#define BX_TOKEN_PRINT_STACK 333
#define BX_TOKEN_WATCH 334
#define BX_TOKEN_UNWATCH 335
#define BX_TOKEN_READ 336
#define BX_TOKEN_WRITE 337
#define BX_TOKEN_SHOW 338
#define BX_TOKEN_LOAD_SYMBOLS 339
#define BX_TOKEN_SYMBOLS 340
#define BX_TOKEN_LIST_SYMBOLS 341
#define BX_TOKEN_GLOBAL 342
#define BX_TOKEN_WHERE 343
#define BX_TOKEN_PRINT_STRING 344
#define BX_TOKEN_NUMERIC 345
#define BX_TOKEN_NE2000 346
#define BX_TOKEN_PIC 347
#define BX_TOKEN_PAGE 348
#define BX_TOKEN_HELP 349
#define BX_TOKEN_CALC 350
#define BX_TOKEN_VGA 351
#define BX_TOKEN_PCI 352
#define BX_TOKEN_COMMAND 353
#define BX_TOKEN_GENERIC 354
#define BX_TOKEN_RSHIFT 355
#define BX_TOKEN_LSHIFT 356
#define BX_TOKEN_REG_IP 357
#define BX_TOKEN_REG_EIP 358
#define BX_TOKEN_REG_RIP 359
#define NEG 360
#define NOT 361




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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 13 "parser.y"
{
  char    *sval;
  Bit64u   uval;
  bx_bool  bval;
}
/* Line 187 of yacc.c.  */
#line 330 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 343 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
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
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  275
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1367

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  120
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  54
/* YYNRULES -- Number of rules.  */
#define YYNRULES  263
/* YYNRULES -- Number of states.  */
#define YYNSTATES  509

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   361

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     114,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   117,     2,     2,     2,     2,   111,     2,
     118,   119,   109,   105,     2,   106,     2,   110,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   116,     2,
       2,   115,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   108,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   107,     2,     2,     2,     2,     2,
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
     112,   113
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     8,    10,    12,    14,    16,    18,
      20,    22,    24,    26,    28,    30,    32,    34,    36,    38,
      40,    42,    44,    46,    48,    50,    52,    54,    56,    58,
      60,    62,    64,    66,    68,    70,    72,    74,    76,    78,
      80,    82,    84,    86,    88,    90,    92,    94,    96,    98,
      99,   101,   103,   105,   107,   109,   111,   113,   115,   117,
     119,   121,   125,   129,   133,   137,   140,   144,   148,   151,
     155,   158,   162,   166,   170,   173,   177,   181,   185,   188,
     193,   198,   203,   209,   215,   221,   224,   228,   232,   237,
     242,   248,   251,   255,   258,   261,   265,   270,   275,   278,
     283,   289,   295,   301,   307,   313,   319,   325,   328,   334,
     337,   341,   345,   348,   352,   357,   360,   363,   367,   371,
     375,   381,   387,   393,   399,   403,   407,   411,   415,   419,
     424,   428,   434,   442,   446,   450,   454,   455,   457,   460,
     463,   466,   469,   472,   475,   478,   482,   486,   490,   493,
     498,   502,   506,   509,   514,   520,   524,   528,   533,   537,
     540,   544,   549,   553,   558,   564,   568,   573,   579,   583,
     587,   591,   596,   600,   604,   608,   612,   616,   620,   624,
     628,   632,   636,   640,   644,   648,   652,   656,   660,   664,
     668,   672,   676,   680,   684,   688,   692,   696,   700,   704,
     708,   712,   716,   720,   724,   728,   732,   736,   740,   744,
     748,   752,   756,   760,   764,   768,   771,   775,   777,   779,
     781,   783,   785,   787,   789,   791,   793,   795,   797,   801,
     805,   809,   813,   817,   821,   825,   829,   833,   836,   839,
     843,   845,   847,   849,   851,   853,   855,   857,   859,   861,
     863,   865,   869,   873,   877,   881,   885,   889,   893,   897,
     901,   905,   908,   911
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     121,     0,    -1,   121,   122,    -1,   122,    -1,   141,    -1,
     142,    -1,   143,    -1,   144,    -1,   145,    -1,   148,    -1,
     150,    -1,   151,    -1,   152,    -1,   153,    -1,   154,    -1,
     156,    -1,   155,    -1,   146,    -1,   147,    -1,   157,    -1,
     158,    -1,   159,    -1,   160,    -1,   161,    -1,   162,    -1,
     163,    -1,   164,    -1,   165,    -1,   166,    -1,   167,    -1,
     168,    -1,   169,    -1,   133,    -1,   134,    -1,   135,    -1,
     132,    -1,   126,    -1,   127,    -1,   128,    -1,   129,    -1,
     136,    -1,   137,    -1,   131,    -1,   130,    -1,   138,    -1,
     139,    -1,   140,    -1,   170,    -1,   171,    -1,    -1,   114,
      -1,    15,    -1,    16,    -1,    32,    -1,    33,    -1,     8,
      -1,     9,    -1,    10,    -1,    11,    -1,    12,    -1,    13,
      -1,    74,    90,   114,    -1,    73,    90,   114,    -1,    75,
      63,   114,    -1,    76,    63,   114,    -1,    77,   114,    -1,
      83,    98,   114,    -1,    83,    63,   114,    -1,    83,   114,
      -1,    93,   173,   114,    -1,    72,   114,    -1,    67,   123,
     114,    -1,    68,   123,   114,    -1,    69,   123,   114,    -1,
      78,   114,    -1,    78,    90,   114,    -1,    79,    64,   114,
      -1,    79,    17,   114,    -1,    79,   114,    -1,    79,    32,
     173,   114,    -1,    79,    81,   173,   114,    -1,    79,    82,
     173,   114,    -1,    79,    32,   173,   173,   114,    -1,    79,
      81,   173,   173,   114,    -1,    79,    82,   173,   173,   114,
      -1,    80,   114,    -1,    80,   173,   114,    -1,    84,    63,
     114,    -1,    84,    63,    90,   114,    -1,    84,    87,    63,
     114,    -1,    84,    87,    63,    90,   114,    -1,    88,   114,
      -1,    89,    90,   114,    -1,    17,   114,    -1,    18,   114,
      -1,    18,    90,   114,    -1,    18,    44,    90,   114,    -1,
      18,    90,    90,   114,    -1,    19,   114,    -1,    21,    61,
     123,   114,    -1,    21,    54,   115,    90,   114,    -1,    21,
       4,   115,   173,   114,    -1,    21,     3,   115,   173,   114,
      -1,    21,     5,   115,   173,   114,    -1,    21,     6,   115,
     173,   114,    -1,    21,     7,   115,   173,   114,    -1,    21,
     125,   115,   173,   114,    -1,    24,   114,    -1,    24,   172,
     116,   172,   114,    -1,    25,   114,    -1,    25,   173,   114,
      -1,    25,    63,   114,    -1,    26,   114,    -1,    26,   173,
     114,    -1,    26,   109,   173,   114,    -1,    23,   114,    -1,
      86,   114,    -1,    86,    63,   114,    -1,    30,    26,   114,
      -1,    30,    34,   114,    -1,    30,    38,   149,   149,   114,
      -1,    30,    39,   149,   149,   114,    -1,    30,    40,   149,
     149,   114,    -1,    30,    41,   149,   149,   114,    -1,    30,
      43,   114,    -1,    30,    42,   114,    -1,    30,    14,   114,
      -1,    30,    45,   114,    -1,    30,    85,   114,    -1,    30,
      85,    63,   114,    -1,    30,    91,   114,    -1,    30,    91,
      93,    90,   114,    -1,    30,    91,    93,    90,   124,    90,
     114,    -1,    30,    92,   114,    -1,    30,    96,   114,    -1,
      30,    97,   114,    -1,    -1,    90,    -1,   124,   114,    -1,
      35,   114,    -1,    37,   114,    -1,    36,   114,    -1,    48,
     114,    -1,    47,   114,    -1,    46,   114,    -1,    27,    90,
     114,    -1,    28,    90,   114,    -1,    29,    90,   114,    -1,
      31,   114,    -1,    49,    50,   173,   114,    -1,    49,    50,
     114,    -1,    49,   173,   114,    -1,    49,   114,    -1,    52,
      63,    63,   114,    -1,    53,    90,    90,    90,   114,    -1,
      55,    56,   114,    -1,    57,    58,   114,    -1,    57,    58,
      90,   114,    -1,    57,    59,   114,    -1,    61,   114,    -1,
      61,   173,   114,    -1,    61,   173,   173,   114,    -1,    61,
      51,   114,    -1,    61,    51,   173,   114,    -1,    61,    51,
     173,   173,   114,    -1,    61,    70,   114,    -1,    61,    60,
     123,   114,    -1,    61,    71,   115,    90,   114,    -1,    62,
      64,   114,    -1,    62,    98,   114,    -1,    65,    90,   114,
      -1,    66,    90,    90,   114,    -1,    94,    31,   114,    -1,
      94,    17,   114,    -1,    94,    18,   114,    -1,    94,    19,
     114,    -1,    94,    24,   114,    -1,    94,    25,   114,    -1,
      94,    26,   114,    -1,    94,    27,   114,    -1,    94,    28,
     114,    -1,    94,    29,   114,    -1,    94,    23,   114,    -1,
      94,    77,   114,    -1,    94,    66,   114,    -1,    94,    67,
     114,    -1,    94,    68,   114,    -1,    94,    69,   114,    -1,
      94,    52,   114,    -1,    94,    72,   114,    -1,    94,    74,
     114,    -1,    94,    73,   114,    -1,    94,    78,   114,    -1,
      94,    75,   114,    -1,    94,    76,   114,    -1,    94,    84,
     114,    -1,    94,    86,   114,    -1,    94,   124,   114,    -1,
      94,    35,   114,    -1,    94,    37,   114,    -1,    94,    36,
     114,    -1,    94,    48,   114,    -1,    94,    47,   114,    -1,
      94,    53,   114,    -1,    94,    61,   114,    -1,    94,    79,
     114,    -1,    94,    80,   114,    -1,    94,    49,   114,    -1,
      94,    62,   114,    -1,    94,    21,   114,    -1,    94,    93,
     114,    -1,    94,    30,   114,    -1,    94,    83,   114,    -1,
      94,    95,   114,    -1,    94,    94,   114,    -1,    94,   114,
      -1,    95,   173,   114,    -1,    90,    -1,    63,    -1,     4,
      -1,     3,    -1,     5,    -1,     6,    -1,     7,    -1,   125,
      -1,   102,    -1,   103,    -1,   104,    -1,   172,   105,   172,
      -1,   172,   106,   172,    -1,   172,   109,   172,    -1,   172,
     110,   172,    -1,   172,   100,   172,    -1,   172,   101,   172,
      -1,   172,   107,   172,    -1,   172,   108,   172,    -1,   172,
     111,   172,    -1,   117,   172,    -1,   106,   172,    -1,   118,
     172,   119,    -1,    90,    -1,    63,    -1,     4,    -1,     3,
      -1,     5,    -1,     6,    -1,     7,    -1,   125,    -1,   102,
      -1,   103,    -1,   104,    -1,   173,   116,   173,    -1,   173,
     105,   173,    -1,   173,   106,   173,    -1,   173,   109,   173,
      -1,   173,   110,   173,    -1,   173,   100,   173,    -1,   173,
     101,   173,    -1,   173,   107,   173,    -1,   173,   108,   173,
      -1,   173,   111,   173,    -1,   117,   173,    -1,   106,   173,
      -1,   118,   173,   119,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   139,   139,   140,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   196,   197,   202,   203,   208,   209,   210,   211,   212,
     213,   218,   223,   231,   239,   247,   255,   260,   265,   273,
     281,   289,   297,   305,   313,   318,   326,   331,   336,   341,
     346,   351,   356,   361,   366,   371,   376,   384,   389,   394,
     399,   407,   415,   423,   431,   436,   441,   446,   454,   462,
     467,   472,   476,   480,   484,   488,   492,   499,   504,   509,
     514,   519,   524,   529,   534,   542,   550,   555,   563,   568,
     573,   578,   583,   588,   593,   598,   603,   608,   613,   618,
     623,   628,   633,   638,   643,   648,   656,   657,   660,   668,
     676,   684,   692,   700,   708,   716,   724,   731,   739,   747,
     752,   757,   762,   770,   778,   786,   794,   799,   804,   812,
     817,   822,   827,   832,   837,   842,   847,   852,   860,   865,
     873,   881,   889,   894,   899,   906,   911,   916,   921,   926,
     931,   936,   941,   946,   951,   956,   962,   968,   974,   982,
     987,   992,   997,  1002,  1007,  1012,  1017,  1022,  1027,  1032,
    1037,  1042,  1047,  1052,  1057,  1067,  1078,  1084,  1097,  1102,
    1112,  1117,  1133,  1145,  1155,  1160,  1168,  1186,  1187,  1188,
    1189,  1190,  1191,  1192,  1193,  1194,  1195,  1196,  1197,  1198,
    1199,  1200,  1201,  1202,  1203,  1204,  1205,  1206,  1207,  1208,
    1213,  1214,  1215,  1216,  1217,  1218,  1219,  1220,  1221,  1222,
    1223,  1224,  1225,  1226,  1227,  1228,  1229,  1230,  1231,  1232,
    1233,  1234,  1235,  1236
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
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
  "BX_TOKEN_R", "BX_TOKEN_REGS", "BX_TOKEN_CPU", "BX_TOKEN_FPU",
  "BX_TOKEN_SSE", "BX_TOKEN_MMX", "BX_TOKEN_IDT", "BX_TOKEN_IVT",
  "BX_TOKEN_GDT", "BX_TOKEN_LDT", "BX_TOKEN_TSS", "BX_TOKEN_TAB",
  "BX_TOKEN_ALL", "BX_TOKEN_LINUX", "BX_TOKEN_DEBUG_REGS",
  "BX_TOKEN_CONTROL_REGS", "BX_TOKEN_SEGMENT_REGS", "BX_TOKEN_EXAMINE",
  "BX_TOKEN_XFORMAT", "BX_TOKEN_DISFORMAT", "BX_TOKEN_RESTORE",
  "BX_TOKEN_SETPMEM", "BX_TOKEN_SYMBOLNAME", "BX_TOKEN_QUERY",
  "BX_TOKEN_PENDING", "BX_TOKEN_TAKE", "BX_TOKEN_DMA", "BX_TOKEN_IRQ",
  "BX_TOKEN_HEX", "BX_TOKEN_DISASM", "BX_TOKEN_INSTRUMENT",
  "BX_TOKEN_STRING", "BX_TOKEN_STOP", "BX_TOKEN_DOIT", "BX_TOKEN_CRC",
  "BX_TOKEN_TRACE", "BX_TOKEN_TRACEREG", "BX_TOKEN_TRACEMEM",
  "BX_TOKEN_SWITCH_MODE", "BX_TOKEN_SIZE", "BX_TOKEN_PTIME",
  "BX_TOKEN_TIMEBP_ABSOLUTE", "BX_TOKEN_TIMEBP", "BX_TOKEN_RECORD",
  "BX_TOKEN_PLAYBACK", "BX_TOKEN_MODEBP", "BX_TOKEN_PRINT_STACK",
  "BX_TOKEN_WATCH", "BX_TOKEN_UNWATCH", "BX_TOKEN_READ", "BX_TOKEN_WRITE",
  "BX_TOKEN_SHOW", "BX_TOKEN_LOAD_SYMBOLS", "BX_TOKEN_SYMBOLS",
  "BX_TOKEN_LIST_SYMBOLS", "BX_TOKEN_GLOBAL", "BX_TOKEN_WHERE",
  "BX_TOKEN_PRINT_STRING", "BX_TOKEN_NUMERIC", "BX_TOKEN_NE2000",
  "BX_TOKEN_PIC", "BX_TOKEN_PAGE", "BX_TOKEN_HELP", "BX_TOKEN_CALC",
  "BX_TOKEN_VGA", "BX_TOKEN_PCI", "BX_TOKEN_COMMAND", "BX_TOKEN_GENERIC",
  "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT", "BX_TOKEN_REG_IP",
  "BX_TOKEN_REG_EIP", "BX_TOKEN_REG_RIP", "'+'", "'-'", "'|'", "'^'",
  "'*'", "'/'", "'&'", "NEG", "NOT", "'\\n'", "'='", "':'", "'!'", "'('",
  "')'", "$accept", "commands", "command", "BX_TOKEN_TOGGLE_ON_OFF",
  "BX_TOKEN_REGISTERS", "BX_TOKEN_SEGREG", "timebp_command",
  "record_command", "playback_command", "modebp_command", "show_command",
  "page_command", "ptime_command", "trace_command", "trace_reg_command",
  "trace_mem_command", "print_stack_command", "watch_point_command",
  "symbol_command", "where_command", "print_string_command",
  "continue_command", "stepN_command", "step_over_command", "set_command",
  "breakpoint_command", "blist_command", "slist_command", "info_command",
  "optional_numeric", "regs_command", "fpu_regs_command",
  "mmx_regs_command", "sse_regs_command", "segment_regs_command",
  "control_regs_command", "debug_regs_command", "delete_command",
  "bpe_command", "bpd_command", "quit_command", "examine_command",
  "restore_command", "setpmem_command", "query_command", "take_command",
  "disassemble_command", "instrument_command", "doit_command",
  "crc_command", "help_command", "calc_command", "vexpression",
  "expression", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
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
     355,   356,   357,   358,   359,    43,    45,   124,    94,    42,
      47,    38,   360,   361,    10,    61,    58,    33,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   120,   121,   121,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   123,   123,   124,   124,   125,   125,   125,   125,   125,
     125,   126,   126,   127,   128,   129,   130,   130,   130,   131,
     132,   133,   134,   135,   136,   136,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   138,   138,   138,
     138,   139,   140,   141,   142,   142,   142,   142,   143,   144,
     144,   144,   144,   144,   144,   144,   144,   145,   145,   145,
     145,   145,   145,   145,   145,   146,   147,   147,   148,   148,
     148,   148,   148,   148,   148,   148,   148,   148,   148,   148,
     148,   148,   148,   148,   148,   148,   149,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     161,   161,   161,   162,   163,   164,   165,   165,   165,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   167,   167,
     168,   169,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   171,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     2,     3,     3,     2,     3,
       2,     3,     3,     3,     2,     3,     3,     3,     2,     4,
       4,     4,     5,     5,     5,     2,     3,     3,     4,     4,
       5,     2,     3,     2,     2,     3,     4,     4,     2,     4,
       5,     5,     5,     5,     5,     5,     5,     2,     5,     2,
       3,     3,     2,     3,     4,     2,     2,     3,     3,     3,
       5,     5,     5,     5,     3,     3,     3,     3,     3,     4,
       3,     5,     7,     3,     3,     3,     0,     1,     2,     2,
       2,     2,     2,     2,     2,     3,     3,     3,     2,     4,
       3,     3,     2,     4,     5,     3,     3,     4,     3,     2,
       3,     4,     3,     4,     5,     3,     4,     5,     3,     3,
       3,     4,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      49,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    53,    54,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    50,     0,     3,     0,    36,    37,    38,    39,    43,
      42,    35,    32,    33,    34,    40,    41,    44,    45,    46,
       4,     5,     6,     7,     8,    17,    18,     9,    10,    11,
      12,    13,    14,    16,    15,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    47,    48,
      93,     0,     0,    94,    98,     0,     0,     0,     0,     0,
      55,    56,    57,    58,    59,    60,     0,     0,     0,   115,
     220,   219,   221,   222,   223,   218,   217,   225,   226,   227,
       0,   107,     0,     0,   224,     0,   243,   242,   244,   245,
     246,   241,   240,   248,   249,   250,     0,   109,     0,     0,
     247,     0,   241,     0,   112,     0,     0,     0,     0,     0,
       0,     0,   136,   136,   136,   136,     0,     0,     0,     0,
       0,     0,     0,     0,   148,   139,   141,   140,   144,   143,
     142,     0,   152,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   159,     0,     0,     0,     0,     0,    51,
      52,     0,     0,     0,    70,     0,     0,     0,     0,    65,
       0,    74,     0,     0,     0,     0,     0,    78,    85,     0,
       0,     0,    68,     0,     0,     0,   116,    91,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   215,     0,     0,     1,     2,   138,     0,     0,
      95,     0,     0,     0,     0,     0,     0,     0,     0,   238,
     237,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   111,   262,   261,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   110,     0,     0,   113,   145,
     146,   147,   126,   118,   119,   137,   136,   136,   136,   136,
     125,   124,   127,     0,   128,     0,   130,   133,   134,   135,
     150,     0,   151,     0,     0,   155,     0,   156,   158,   162,
       0,     0,   165,     0,     0,   160,     0,   168,   169,   170,
       0,    71,    72,    73,    62,    61,    63,    64,    75,    77,
       0,    76,     0,     0,    86,    67,    66,     0,    87,     0,
     117,    92,    69,   173,   174,   175,   209,   182,   176,   177,
     178,   179,   180,   181,   211,   172,   198,   200,   199,   202,
     201,   207,   188,   203,   204,   208,   184,   185,   186,   187,
     189,   191,   190,   193,   194,   183,   192,   205,   206,   212,
     195,   196,   210,   214,   213,   197,   216,    96,    97,     0,
       0,     0,     0,     0,     0,    99,     0,   239,   232,   233,
     228,   229,   234,   235,   230,   231,   236,     0,   263,   256,
     257,   252,   253,   258,   259,   254,   255,   260,   251,   114,
       0,     0,     0,     0,   129,     0,   149,   153,     0,   157,
     163,     0,   166,     0,   253,   161,   171,    79,     0,    80,
       0,    81,     0,    88,     0,    89,   102,   101,   103,   104,
     105,   100,   106,   108,   120,   121,   122,   123,   131,     0,
     154,   164,   167,    82,    83,    84,    90,     0,   132
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    52,    53,   201,    54,   150,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,   326,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   135,   151
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -152
static const yytype_int16 yypact[] =
{
     957,   -95,   -11,   -91,   186,   -89,   589,   660,   525,   -58,
     -56,   -35,  1114,   -85,  -152,  -152,   -50,   -43,   -42,   -39,
     -37,   -30,   572,    39,     2,    52,   -41,   456,   -61,    16,
      27,    50,    50,    50,     4,    41,    42,    70,    71,    22,
     -84,    -1,   705,    49,   -27,   -55,    23,    48,   838,  1037,
     838,  -152,    21,  -152,    26,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,    54,   -29,  -152,  -152,    30,    34,    68,    73,    85,
    -152,  -152,  -152,  -152,  -152,  -152,    86,    50,    87,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
     855,  -152,   855,   855,  -152,  1239,  -152,  -152,  -152,  -152,
    -152,    72,  -152,  -152,  -152,  -152,   838,  -152,   838,   838,
    -152,   503,  -152,   838,  -152,   540,    90,    96,    97,    99,
     100,   101,    95,    95,    95,    95,   102,   115,   116,   -52,
     -86,   118,   122,   123,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,   722,  -152,   637,   175,   151,   128,    29,   129,   793,
      50,   130,   131,  -152,   117,   134,   150,   154,   155,  -152,
    -152,   159,   165,   166,  -152,   179,   180,   181,   182,  -152,
     183,  -152,   184,   838,   194,   838,   838,  -152,  -152,   770,
     206,   207,  -152,    89,   259,   209,  -152,  -152,   210,   815,
     211,   212,   213,   214,   215,   216,   230,   234,   235,   243,
     244,   246,   248,   253,   254,   256,   257,   258,   260,   261,
     262,   263,   264,   278,   280,   295,   298,   299,   300,   301,
     303,   308,   309,   311,   313,   329,   333,   334,   338,   339,
     341,   343,  -152,   358,  1060,  -152,  -152,  -152,   359,   360,
    -152,   838,   838,   838,   838,   838,   269,   368,   838,  -152,
    -152,   370,   855,   855,   855,   855,   855,   855,   855,   855,
     855,   855,  -152,   153,   153,   245,   838,   838,   838,   838,
     838,   838,   838,   838,   838,  -152,   838,  1072,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,    95,    95,    95,    95,
    -152,  -152,  -152,   369,  -152,   283,  -152,  -152,  -152,  -152,
    -152,  1084,  -152,   372,   364,  -152,   373,  -152,  -152,  -152,
     149,   374,  -152,   394,   838,  -152,  1107,  -152,  -152,  -152,
     376,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
     279,  -152,   328,   395,  -152,  -152,  -152,   377,  -152,    91,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  1119,
    1131,  1143,  1155,  1167,   378,  -152,  1179,  -152,  -152,  -152,
     161,   161,   161,   161,  -152,  -152,  -152,   442,  -152,   153,
     153,   310,   310,   310,   310,   153,   153,   153,  1251,  -152,
     379,   380,   396,   400,  -152,   -23,  -152,  -152,   401,  -152,
    -152,  1191,  -152,   403,   340,  -152,  -152,  -152,  1203,  -152,
    1215,  -152,  1227,  -152,   404,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,   418,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,   406,  -152
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -152,  -152,   469,    -6,   -49,    18,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -151,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -128,    -7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -263
static const yytype_int16 yytable[] =
{
     273,   155,   289,   195,   290,   291,   210,   335,   225,    14,
      15,   333,   327,   328,   329,   183,   212,   187,   188,   100,
     194,   275,   118,   104,   134,   119,   202,   203,   336,   174,
     211,   213,   156,   101,   157,   219,   223,   196,     1,     2,
       3,   229,     4,   274,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,   158,    16,    17,    18,   226,
     224,   279,   334,   214,   175,   199,   200,    19,    20,    21,
      22,   176,   177,    23,    24,   178,    25,   179,    26,   102,
     215,   216,    27,    28,   180,   280,    29,    30,    31,    32,
      33,   498,   185,    34,    35,    36,    37,    38,    39,    40,
      41,    42,   184,   103,    43,    44,   197,    45,   186,    46,
      47,   287,   220,   217,    48,    49,    50,   198,   204,   346,
     136,   137,   138,   139,   140,   110,   111,   112,   113,   114,
     115,   205,   206,   207,   208,    51,   209,   227,   228,   303,
     277,   304,   305,   347,   278,   281,   317,   221,   134,   282,
     134,   134,   136,   137,   138,   139,   140,   110,   111,   112,
     113,   114,   115,   222,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   341,   460,   461,   462,   463,   377,
     152,   484,   350,   283,   351,   325,   302,   356,   284,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     285,   286,   288,   378,   319,   485,   370,   142,   372,   373,
     320,   321,   152,   322,   323,   324,   330,   306,   307,   143,
     144,   145,   308,   354,   310,   311,   312,   313,   314,   331,
     332,   355,   337,   316,   148,   149,   338,   339,   343,   142,
     116,   344,   345,   348,   352,   360,   353,   117,   357,   306,
     307,   143,   144,   145,   308,   354,   310,   311,   312,   313,
     314,   292,   293,   470,   358,   316,   148,   149,   359,   316,
     298,   299,   300,   361,   429,   430,   431,   432,   433,   362,
     363,   436,   136,   137,   138,   139,   140,   110,   111,   112,
     113,   114,   115,   364,   365,   366,   367,   368,   369,   449,
     450,   451,   452,   453,   454,   455,   456,   457,   371,   458,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     375,   376,   379,   380,   381,   383,   384,   385,   386,   387,
     388,   136,   137,   138,   139,   140,   110,   111,   112,   113,
     114,   115,   152,   471,   389,   306,   307,   474,   390,   391,
     308,   309,   310,   311,   312,   313,   314,   392,   393,   434,
     394,   316,   395,   478,   448,   480,   482,   396,   397,   142,
     398,   399,   400,   465,   401,   402,   403,   404,   405,   306,
     307,   143,   144,   145,   308,   354,   310,   311,   312,   313,
     314,   152,   406,   477,   407,   316,   148,   149,   136,   137,
     138,   139,   140,   110,   111,   112,   113,   114,   115,   408,
     306,   307,   409,   410,   411,   412,   499,   413,   142,   312,
     313,   314,   414,   415,     0,   416,   316,   417,   306,   307,
     143,   144,   145,   308,   354,   310,   311,   312,   313,   314,
    -262,  -262,   479,   418,   316,   148,   149,   419,   420,  -262,
    -262,  -262,   421,   422,   468,   423,   316,   424,   152,   136,
     137,   138,   139,   140,   110,   111,   112,   113,   114,   115,
     292,   293,   425,   427,   428,   294,   295,   296,   297,   298,
     299,   300,   435,   464,   473,   142,   467,   469,   472,   437,
     476,   483,   491,   494,   495,   306,   307,   143,   144,   145,
     308,   354,   310,   311,   312,   313,   314,   189,   507,   481,
     496,   316,   148,   149,   497,   500,   190,   502,   506,   152,
     508,   276,     0,     0,     0,     0,   191,   192,   136,   137,
     138,   139,   140,   110,   111,   112,   113,   114,   115,     0,
       0,     0,   292,   293,     0,     0,   142,   294,   295,   296,
     297,   298,   299,   300,     0,     0,   493,     0,   143,   144,
     145,     0,   146,     0,     0,     0,     0,     0,     0,     0,
     193,     0,     0,   148,   149,   136,   137,   138,   139,   140,
     110,   111,   112,   113,   114,   115,     0,     0,   152,     0,
       0,     0,   120,   121,   122,   123,   124,   110,   111,   112,
     113,   114,   115,   306,   307,     0,     0,     0,   308,   309,
     310,   311,   312,   313,   314,   142,     0,   315,     0,   316,
       0,     0,   181,     0,     0,     0,     0,   143,   144,   145,
       0,   146,     0,     0,   153,   152,     0,     0,     0,   154,
     306,   307,   148,   149,     0,   308,   309,   310,   311,   312,
     313,   314,   125,     0,   318,     0,   316,     0,     0,     0,
       0,     0,   142,   136,   137,   138,   139,   140,   110,   111,
     112,   113,   114,   115,   143,   144,   145,     0,   146,   126,
       0,     0,     0,     0,     0,     0,   182,     0,     0,   148,
     149,   127,   128,   129,     0,   130,     0,     0,     0,     0,
       0,     0,     0,   131,     0,     0,   132,   133,   136,   137,
     138,   139,   140,   110,   111,   112,   113,   114,   115,     0,
       0,     0,     0,   141,     0,   136,   137,   138,   139,   140,
     110,   111,   112,   113,   114,   115,     0,   306,   307,     0,
       0,     0,   308,   309,   310,   311,   312,   313,   314,     0,
     142,   342,     0,   316,     0,     0,     0,     0,     0,     0,
       0,     0,   143,   144,   145,     0,   146,     0,   152,     0,
       0,     0,     0,     0,   147,     0,     0,   148,   149,     0,
       0,     0,     0,     0,     0,   152,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   142,   136,   137,   138,   139,
     140,   110,   111,   112,   113,   114,   115,   143,   144,   145,
       0,   146,   142,     0,     0,     0,     0,     0,     0,   218,
       0,     0,   148,   149,   143,   144,   145,     0,   146,     0,
       0,     0,     0,     0,     0,     0,   340,     0,     0,   148,
     149,   136,   137,   138,   139,   140,   110,   111,   112,   113,
     114,   115,     0,     0,     0,     0,   152,     0,   120,   121,
     122,   123,   124,   110,   111,   112,   113,   114,   115,     0,
     306,   307,     0,     0,     0,   308,   309,   310,   311,   312,
     313,   314,     0,   142,   374,     0,   316,     0,     0,     0,
       0,     0,     0,     0,     0,   143,   144,   145,     0,   146,
       0,   152,     0,     0,     0,     0,     0,   349,     0,     0,
     148,   149,     0,     0,     0,   306,   307,     0,   125,     0,
     308,   309,   310,   311,   312,   313,   314,     0,   142,   382,
       0,   316,     0,     0,     0,     0,     0,     0,     0,     0,
     143,   144,   145,     0,   146,   126,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   148,   149,   127,   128,   129,
       0,   130,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   132,   133,     1,     2,     3,     0,     4,     0,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,     0,    16,    17,    18,     0,     0,     0,     0,     0,
       0,     0,     0,    19,    20,    21,    22,     0,     0,    23,
      24,     0,    25,     0,    26,     0,     0,     0,    27,    28,
       0,     0,    29,    30,    31,    32,    33,     0,     0,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,     0,
      43,    44,     0,    45,     0,    46,    47,     0,     0,     0,
      48,    49,    50,     0,   230,   231,   232,     0,   233,     0,
     234,   235,   236,   237,   238,   239,   240,   241,   242,    14,
      15,    51,   243,   244,   245,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   246,   247,   248,     0,     0,   249,
     250,     0,     0,     0,     0,     0,     0,     0,   251,   252,
       0,     0,     0,   253,   254,   255,   256,     0,     0,   257,
     258,   259,   260,   261,   262,   263,   264,   265,     0,     0,
     266,   267,     0,   268,     0,     0,     0,     0,   159,     0,
     269,   270,   271,     0,     0,     0,     0,     0,     0,     0,
     160,     0,     0,     0,     0,     0,     0,     0,   161,     0,
       0,   272,   162,   163,   164,   165,   166,   167,     0,   168,
     306,   307,     0,     0,     0,   308,   309,   310,   311,   312,
     313,   314,   306,   307,   426,     0,   316,   308,   309,   310,
     311,   312,   313,   314,   306,   307,   459,     0,   316,   308,
     309,   310,   311,   312,   313,   314,     0,     0,   466,   169,
     316,     0,     0,     0,     0,   170,   171,   306,   307,     0,
     172,   173,   308,   309,   310,   311,   312,   313,   314,   306,
     307,   475,     0,   316,   308,   309,   310,   311,   312,   313,
     314,   306,   307,   486,     0,   316,   308,   309,   310,   311,
     312,   313,   314,   306,   307,   487,     0,   316,   308,   309,
     310,   311,   312,   313,   314,   306,   307,   488,     0,   316,
     308,   309,   310,   311,   312,   313,   314,   306,   307,   489,
       0,   316,   308,   309,   310,   311,   312,   313,   314,   306,
     307,   490,     0,   316,   308,   309,   310,   311,   312,   313,
     314,   306,   307,   492,     0,   316,   308,   309,   310,   311,
     312,   313,   314,   306,   307,   501,     0,   316,   308,   309,
     310,   311,   312,   313,   314,   306,   307,   503,     0,   316,
     308,   309,   310,   311,   312,   313,   314,   306,   307,   504,
       0,   316,   308,   309,   310,   311,   312,   313,   314,   292,
     293,   505,     0,   316,   294,   295,   296,   297,   298,   299,
     300,   306,   307,     0,     0,   301,   308,   309,   310,   311,
     312,   313,   314,     0,     0,     0,     0,   316
};

static const yytype_int16 yycheck[] =
{
      49,     8,   130,    64,   132,   133,    90,    93,    63,    32,
      33,    63,   163,   164,   165,    22,    17,    58,    59,   114,
      27,     0,     4,   114,     6,   114,    32,    33,   114,   114,
     114,    32,    90,    44,    90,    42,    63,    98,    17,    18,
      19,    48,    21,    50,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    90,    35,    36,    37,   114,
      87,    90,   114,    64,   114,    15,    16,    46,    47,    48,
      49,   114,   114,    52,    53,   114,    55,   114,    57,    90,
      81,    82,    61,    62,   114,   114,    65,    66,    67,    68,
      69,   114,    90,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    63,   114,    83,    84,    90,    86,    56,    88,
      89,   117,    63,   114,    93,    94,    95,    90,   114,    90,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    90,    90,    63,    63,   114,   114,   114,    90,   146,
     114,   148,   149,   114,    90,   115,   153,    98,   130,   115,
     132,   133,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,   114,   292,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   181,   326,   327,   328,   329,    90,
      63,    90,   189,   115,   190,    90,   114,   194,   115,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
     115,   115,   115,   114,   114,   114,   213,    90,   215,   216,
     114,   114,    63,   114,   114,   114,   114,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   114,
     114,   114,   114,   116,   117,   118,   114,   114,    63,    90,
      54,    90,   114,   114,   114,    90,   115,    61,   114,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   100,   101,   114,   114,   116,   117,   118,   114,   116,
     109,   110,   111,   114,   281,   282,   283,   284,   285,   114,
     114,   288,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,   114,   114,   114,   114,   114,   114,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   114,   316,
     292,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     114,   114,    63,   114,   114,   114,   114,   114,   114,   114,
     114,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    63,   350,   114,   100,   101,   354,   114,   114,
     105,   106,   107,   108,   109,   110,   111,   114,   114,    90,
     114,   116,   114,   370,   119,   372,   373,   114,   114,    90,
     114,   114,   114,    90,   114,   114,   114,   114,   114,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,    63,   114,   114,   114,   116,   117,   118,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,   114,
     100,   101,   114,   114,   114,   114,   465,   114,    90,   109,
     110,   111,   114,   114,    -1,   114,   116,   114,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     100,   101,   114,   114,   116,   117,   118,   114,   114,   109,
     110,   111,   114,   114,    90,   114,   116,   114,    63,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
     100,   101,   114,   114,   114,   105,   106,   107,   108,   109,
     110,   111,   114,   114,    90,    90,   114,   114,   114,   119,
     114,   114,   114,   114,   114,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,    51,    90,   114,
     114,   116,   117,   118,   114,   114,    60,   114,   114,    63,
     114,    52,    -1,    -1,    -1,    -1,    70,    71,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    -1,
      -1,    -1,   100,   101,    -1,    -1,    90,   105,   106,   107,
     108,   109,   110,   111,    -1,    -1,   114,    -1,   102,   103,
     104,    -1,   106,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     114,    -1,    -1,   117,   118,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    -1,    -1,    63,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,   100,   101,    -1,    -1,    -1,   105,   106,
     107,   108,   109,   110,   111,    90,    -1,   114,    -1,   116,
      -1,    -1,    50,    -1,    -1,    -1,    -1,   102,   103,   104,
      -1,   106,    -1,    -1,   109,    63,    -1,    -1,    -1,   114,
     100,   101,   117,   118,    -1,   105,   106,   107,   108,   109,
     110,   111,    63,    -1,   114,    -1,   116,    -1,    -1,    -1,
      -1,    -1,    90,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,   102,   103,   104,    -1,   106,    90,
      -1,    -1,    -1,    -1,    -1,    -1,   114,    -1,    -1,   117,
     118,   102,   103,   104,    -1,   106,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   114,    -1,    -1,   117,   118,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    -1,
      -1,    -1,    -1,    63,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    -1,   100,   101,    -1,
      -1,    -1,   105,   106,   107,   108,   109,   110,   111,    -1,
      90,   114,    -1,   116,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   102,   103,   104,    -1,   106,    -1,    63,    -1,
      -1,    -1,    -1,    -1,   114,    -1,    -1,   117,   118,    -1,
      -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    90,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,   102,   103,   104,
      -1,   106,    90,    -1,    -1,    -1,    -1,    -1,    -1,   114,
      -1,    -1,   117,   118,   102,   103,   104,    -1,   106,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   114,    -1,    -1,   117,
     118,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    -1,    -1,    -1,    -1,    63,    -1,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    -1,
     100,   101,    -1,    -1,    -1,   105,   106,   107,   108,   109,
     110,   111,    -1,    90,   114,    -1,   116,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   102,   103,   104,    -1,   106,
      -1,    63,    -1,    -1,    -1,    -1,    -1,   114,    -1,    -1,
     117,   118,    -1,    -1,    -1,   100,   101,    -1,    63,    -1,
     105,   106,   107,   108,   109,   110,   111,    -1,    90,   114,
      -1,   116,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     102,   103,   104,    -1,   106,    90,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   117,   118,   102,   103,   104,
      -1,   106,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   117,   118,    17,    18,    19,    -1,    21,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    -1,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    47,    48,    49,    -1,    -1,    52,
      53,    -1,    55,    -1,    57,    -1,    -1,    -1,    61,    62,
      -1,    -1,    65,    66,    67,    68,    69,    -1,    -1,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    -1,    86,    -1,    88,    89,    -1,    -1,    -1,
      93,    94,    95,    -1,    17,    18,    19,    -1,    21,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,   114,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    48,    49,    -1,    -1,    52,
      53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    62,
      -1,    -1,    -1,    66,    67,    68,    69,    -1,    -1,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    -1,    86,    -1,    -1,    -1,    -1,    14,    -1,
      93,    94,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,    -1,
      -1,   114,    38,    39,    40,    41,    42,    43,    -1,    45,
     100,   101,    -1,    -1,    -1,   105,   106,   107,   108,   109,
     110,   111,   100,   101,   114,    -1,   116,   105,   106,   107,
     108,   109,   110,   111,   100,   101,   114,    -1,   116,   105,
     106,   107,   108,   109,   110,   111,    -1,    -1,   114,    85,
     116,    -1,    -1,    -1,    -1,    91,    92,   100,   101,    -1,
      96,    97,   105,   106,   107,   108,   109,   110,   111,   100,
     101,   114,    -1,   116,   105,   106,   107,   108,   109,   110,
     111,   100,   101,   114,    -1,   116,   105,   106,   107,   108,
     109,   110,   111,   100,   101,   114,    -1,   116,   105,   106,
     107,   108,   109,   110,   111,   100,   101,   114,    -1,   116,
     105,   106,   107,   108,   109,   110,   111,   100,   101,   114,
      -1,   116,   105,   106,   107,   108,   109,   110,   111,   100,
     101,   114,    -1,   116,   105,   106,   107,   108,   109,   110,
     111,   100,   101,   114,    -1,   116,   105,   106,   107,   108,
     109,   110,   111,   100,   101,   114,    -1,   116,   105,   106,
     107,   108,   109,   110,   111,   100,   101,   114,    -1,   116,
     105,   106,   107,   108,   109,   110,   111,   100,   101,   114,
      -1,   116,   105,   106,   107,   108,   109,   110,   111,   100,
     101,   114,    -1,   116,   105,   106,   107,   108,   109,   110,
     111,   100,   101,    -1,    -1,   116,   105,   106,   107,   108,
     109,   110,   111,    -1,    -1,    -1,    -1,   116
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    17,    18,    19,    21,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    35,    36,    37,    46,
      47,    48,    49,    52,    53,    55,    57,    61,    62,    65,
      66,    67,    68,    69,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    83,    84,    86,    88,    89,    93,    94,
      95,   114,   121,   122,   124,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     114,    44,    90,   114,   114,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    54,    61,   125,   114,
       3,     4,     5,     6,     7,    63,    90,   102,   103,   104,
     106,   114,   117,   118,   125,   172,     3,     4,     5,     6,
       7,    63,    90,   102,   103,   104,   106,   114,   117,   118,
     125,   173,    63,   109,   114,   173,    90,    90,    90,    14,
      26,    34,    38,    39,    40,    41,    42,    43,    45,    85,
      91,    92,    96,    97,   114,   114,   114,   114,   114,   114,
     114,    50,   114,   173,    63,    90,    56,    58,    59,    51,
      60,    70,    71,   114,   173,    64,    98,    90,    90,    15,
      16,   123,   123,   123,   114,    90,    90,    63,    63,   114,
      90,   114,    17,    32,    64,    81,    82,   114,   114,   173,
      63,    98,   114,    63,    87,    63,   114,   114,    90,   173,
      17,    18,    19,    21,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    35,    36,    37,    47,    48,    49,    52,
      53,    61,    62,    66,    67,    68,    69,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    83,    84,    86,    93,
      94,    95,   114,   124,   173,     0,   122,   114,    90,    90,
     114,   115,   115,   115,   115,   115,   115,   123,   115,   172,
     172,   172,   100,   101,   105,   106,   107,   108,   109,   110,
     111,   116,   114,   173,   173,   173,   100,   101,   105,   106,
     107,   108,   109,   110,   111,   114,   116,   173,   114,   114,
     114,   114,   114,   114,   114,    90,   149,   149,   149,   149,
     114,   114,   114,    63,   114,    93,   114,   114,   114,   114,
     114,   173,   114,    63,    90,   114,    90,   114,   114,   114,
     173,   123,   114,   115,   106,   114,   173,   114,   114,   114,
      90,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     173,   114,   173,   173,   114,   114,   114,    90,   114,    63,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   173,
     173,   173,   173,   173,    90,   114,   173,   119,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   119,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   114,
     149,   149,   149,   149,   114,    90,   114,   114,    90,   114,
     114,   173,   114,    90,   173,   114,   114,   114,   173,   114,
     173,   114,   173,   114,    90,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   124,
     114,   114,   114,   114,   114,   114,   114,    90,   114
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


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
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

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
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
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

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
        case 50:
#line 191 "parser.y"
    {
      }
    break;

  case 52:
#line 198 "parser.y"
    { (yyval.bval)=(yyvsp[(1) - (1)].bval); }
    break;

  case 54:
#line 204 "parser.y"
    { (yyval.sval)=(yyvsp[(1) - (1)].sval); }
    break;

  case 60:
#line 214 "parser.y"
    { (yyval.uval)=(yyvsp[(1) - (1)].uval); }
    break;

  case 61:
#line 219 "parser.y"
    {
          bx_dbg_timebp_command(0, (yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 62:
#line 224 "parser.y"
    {
          bx_dbg_timebp_command(1, (yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 63:
#line 232 "parser.y"
    {
          bx_dbg_record_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 64:
#line 240 "parser.y"
    {
          bx_dbg_playback_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 65:
#line 248 "parser.y"
    {
          bx_dbg_modebp_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 66:
#line 256 "parser.y"
    {
          bx_dbg_show_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 67:
#line 261 "parser.y"
    {
          bx_dbg_show_param_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 68:
#line 266 "parser.y"
    {
          bx_dbg_show_command(0);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 69:
#line 274 "parser.y"
    {
          bx_dbg_xlate_address((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 70:
#line 282 "parser.y"
    {
          bx_dbg_ptime_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 71:
#line 290 "parser.y"
    {
          bx_dbg_trace_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 72:
#line 298 "parser.y"
    {
          bx_dbg_trace_reg_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 73:
#line 306 "parser.y"
    {
          bx_dbg_trace_mem_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 74:
#line 314 "parser.y"
    {
          bx_dbg_print_stack_command(16);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 75:
#line 319 "parser.y"
    {
          bx_dbg_print_stack_command((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 76:
#line 327 "parser.y"
    {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 77:
#line 332 "parser.y"
    {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 78:
#line 337 "parser.y"
    {
          bx_dbg_print_watchpoints();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 79:
#line 342 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (4)].uval), 1); /* BX_READ */
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 80:
#line 347 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (4)].uval), 1); /* BX_READ */
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 81:
#line 352 "parser.y"
    {
          bx_dbg_watch(1, (yyvsp[(3) - (4)].uval), 1); /* BX_WRITE */
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 82:
#line 357 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval)); /* BX_READ */
          free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 83:
#line 362 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval)); /* BX_READ */
          free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 84:
#line 367 "parser.y"
    {
          bx_dbg_watch(1, (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval)); /* BX_WRITE */
          free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 85:
#line 372 "parser.y"
    {
          bx_dbg_unwatch_all();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 86:
#line 377 "parser.y"
    {
          bx_dbg_unwatch((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 87:
#line 385 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(2) - (3)].sval), 0, 0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 88:
#line 390 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(2) - (4)].sval), 0, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 89:
#line 395 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(3) - (4)].sval), 1, 0);
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 90:
#line 400 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(3) - (5)].sval), 1, (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval)); free((yyvsp[(3) - (5)].sval));
      }
    break;

  case 91:
#line 408 "parser.y"
    {
        bx_dbg_where_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 92:
#line 416 "parser.y"
    {
        bx_dbg_print_string_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 93:
#line 424 "parser.y"
    {
        bx_dbg_continue_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 94:
#line 432 "parser.y"
    {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 95:
#line 437 "parser.y"
    {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 96:
#line 442 "parser.y"
    {
        bx_dbg_stepN_command(-1, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 97:
#line 447 "parser.y"
    {
        bx_dbg_stepN_command((yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 98:
#line 455 "parser.y"
    {
        bx_dbg_step_over_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 99:
#line 463 "parser.y"
    {
        bx_dbg_set_auto_disassemble((yyvsp[(3) - (4)].bval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 100:
#line 468 "parser.y"
    {
        bx_dbg_set_symbol_command((yyvsp[(2) - (5)].sval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 101:
#line 473 "parser.y"
    { 
        bx_dbg_set_reg8l_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 102:
#line 477 "parser.y"
    { 
        bx_dbg_set_reg8h_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 103:
#line 481 "parser.y"
    { 
        bx_dbg_set_reg16_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 104:
#line 485 "parser.y"
    { 
        bx_dbg_set_reg32_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 105:
#line 489 "parser.y"
    { 
        bx_dbg_set_reg64_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 106:
#line 493 "parser.y"
    { 
        bx_dbg_load_segreg((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 107:
#line 500 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 108:
#line 505 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval));
      }
    break;

  case 109:
#line 510 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 110:
#line 515 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 111:
#line 520 "parser.y"
    {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 112:
#line 525 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 113:
#line 530 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 114:
#line 535 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 115:
#line 543 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 116:
#line 551 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 117:
#line 556 "parser.y"
    {
        bx_dbg_info_symbols_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 118:
#line 564 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 119:
#line 569 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS | BX_INFO_FPU_REGS | BX_INFO_SSE_REGS);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 120:
#line 574 "parser.y"
    {
        bx_dbg_info_idt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 121:
#line 579 "parser.y"
    {
        bx_dbg_info_ivt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 122:
#line 584 "parser.y"
    {
        bx_dbg_info_gdt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 123:
#line 589 "parser.y"
    {
        bx_dbg_info_ldt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 124:
#line 594 "parser.y"
    {
        bx_dbg_dump_table();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 125:
#line 599 "parser.y"
    {
        bx_dbg_info_tss_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 126:
#line 604 "parser.y"
    {
        bx_dbg_info_flags();
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 127:
#line 609 "parser.y"
    {
        bx_dbg_info_linux_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 128:
#line 614 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 129:
#line 619 "parser.y"
    {
        bx_dbg_info_symbols_command((yyvsp[(3) - (4)].sval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 130:
#line 624 "parser.y"
    {
        bx_dbg_info_ne2k(-1, -1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 131:
#line 629 "parser.y"
    {
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval)); free((yyvsp[(3) - (5)].sval));
        bx_dbg_info_ne2k((yyvsp[(4) - (5)].uval), -1);
      }
    break;

  case 132:
#line 634 "parser.y"
    {
        free((yyvsp[(1) - (7)].sval)); free((yyvsp[(2) - (7)].sval)); free((yyvsp[(3) - (7)].sval)); free((yyvsp[(5) - (7)].sval));
        bx_dbg_info_ne2k((yyvsp[(4) - (7)].uval), (yyvsp[(6) - (7)].uval));
      }
    break;

  case 133:
#line 639 "parser.y"
    {
        bx_dbg_info_pic();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 134:
#line 644 "parser.y"
    {
        bx_dbg_info_vga();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 135:
#line 649 "parser.y"
    {
        bx_dbg_info_pci();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 136:
#line 656 "parser.y"
    { (yyval.uval) = EMPTY_ARG; }
    break;

  case 138:
#line 661 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 139:
#line 669 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 140:
#line 677 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 141:
#line 685 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 142:
#line 693 "parser.y"
    {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 143:
#line 701 "parser.y"
    {
        bx_dbg_info_control_regs_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 144:
#line 709 "parser.y"
    {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 145:
#line 717 "parser.y"
    {
        bx_dbg_del_breakpoint_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 146:
#line 725 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[(2) - (3)].uval), 1);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 147:
#line 732 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[(2) - (3)].uval), 0);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 148:
#line 740 "parser.y"
    {
        bx_dbg_quit_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 149:
#line 748 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (4)].sval), (yyvsp[(2) - (4)].sval),1, (yyvsp[(3) - (4)].uval), 1);
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 150:
#line 753 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (3)].sval), (yyvsp[(2) - (3)].sval),1, 0, 0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 151:
#line 758 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (3)].sval), NULL,0, (yyvsp[(2) - (3)].uval), 1);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 152:
#line 763 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (2)].sval), NULL,0, 0, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 153:
#line 771 "parser.y"
    {
        bx_dbg_restore_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].sval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 154:
#line 779 "parser.y"
    {
        bx_dbg_setpmem_command((yyvsp[(2) - (5)].uval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval));
      }
    break;

  case 155:
#line 787 "parser.y"
    {
        bx_dbg_query_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 156:
#line 795 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (3)].sval), 1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 157:
#line 800 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 158:
#line 805 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (3)].sval), 1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 159:
#line 813 "parser.y"
    {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 160:
#line 818 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[(2) - (3)].uval), (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 161:
#line 823 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 162:
#line 828 "parser.y"
    {
        bx_dbg_disassemble_current((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 163:
#line 833 "parser.y"
    {
        bx_dbg_disassemble_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 164:
#line 838 "parser.y"
    {
        bx_dbg_disassemble_command((yyvsp[(2) - (5)].sval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 165:
#line 843 "parser.y"
    {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 166:
#line 848 "parser.y"
    {
        bx_dbg_disassemble_hex_mode_switch((yyvsp[(3) - (4)].bval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 167:
#line 853 "parser.y"
    {
        bx_dbg_set_disassemble_size((yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 168:
#line 861 "parser.y"
    {
        bx_dbg_instrument_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 169:
#line 866 "parser.y"
    {
        bx_dbg_instrument_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 170:
#line 874 "parser.y"
    {
        bx_dbg_doit_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 171:
#line 882 "parser.y"
    {
        bx_dbg_crc_command((yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 172:
#line 890 "parser.y"
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 173:
#line 895 "parser.y"
    {
         dbg_printf("c|cont|continue - continue executing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 174:
#line 900 "parser.y"
    {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 175:
#line 907 "parser.y"
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 176:
#line 912 "parser.y"
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 177:
#line 917 "parser.y"
    {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 178:
#line 922 "parser.y"
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 179:
#line 927 "parser.y"
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 180:
#line 932 "parser.y"
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 181:
#line 937 "parser.y"
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 182:
#line 942 "parser.y"
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 183:
#line 947 "parser.y"
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 184:
#line 952 "parser.y"
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 185:
#line 957 "parser.y"
    {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 186:
#line 963 "parser.y"
    {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 187:
#line 969 "parser.y"
    {
         dbg_printf("trace-mem on  - print all memory accesses occured during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 188:
#line 975 "parser.y"
    {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 189:
#line 983 "parser.y"
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 190:
#line 988 "parser.y"
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 191:
#line 993 "parser.y"
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 192:
#line 998 "parser.y"
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 193:
#line 1003 "parser.y"
    {
         dbg_printf("record <filename> - record console input to file filename\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 194:
#line 1008 "parser.y"
    {
         dbg_printf("playback <filename> - playback console input from file filename\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 195:
#line 1013 "parser.y"
    {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 196:
#line 1018 "parser.y"
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 197:
#line 1023 "parser.y"
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 198:
#line 1028 "parser.y"
    {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 199:
#line 1033 "parser.y"
    {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 200:
#line 1038 "parser.y"
    {
         dbg_printf("sse|xmm - print SSE state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 201:
#line 1043 "parser.y"
    {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 202:
#line 1048 "parser.y"
    {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 203:
#line 1053 "parser.y"
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 204:
#line 1058 "parser.y"
    {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm hex on/off - control disasm offsets and displacements format\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 205:
#line 1068 "parser.y"
    {
         dbg_printf("watch - print current watch point status\n");
         dbg_printf("watch stop - stop simulation when a watchpoint is encountred\n");
         dbg_printf("watch continue - do not stop the simulation when watch point is encountred\n");
         dbg_printf("watch r|read addr - insert a read watch point at physical address addr\n");
         dbg_printf("watch w|write addr - insert a write watch point at physical address addr\n");
         dbg_printf("watch r|read addr <len> - insert a read watch point at physical address addr with range <len>\n");
         dbg_printf("watch w|write addr <len> - insert a write watch point at physical address addr with range <len>\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 206:
#line 1079 "parser.y"
    {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 207:
#line 1085 "parser.y"
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
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 208:
#line 1098 "parser.y"
    {
         dbg_printf("instrument <command> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command>\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 209:
#line 1103 "parser.y"
    {
         dbg_printf("set <regname> = <expr> - set register value to expression\n");
         dbg_printf("set $reg = val - set CPU register to value val\n");
         dbg_printf("set $auto_disassemble = 1 - cause debugger to disassemble current instruction\n");
         dbg_printf("       every time execution stops\n");
         dbg_printf("set u|disasm|disassemble on  - same as 'set $auto_disassemble = 1'\n");
         dbg_printf("set u|disasm|disassemble off - same as 'set $auto_disassemble = 0'\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 210:
#line 1113 "parser.y"
    {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 211:
#line 1118 "parser.y"
    {
         dbg_printf("info break - show information about current breakpoint status\n");
         dbg_printf("info idt - show interrupt descriptor table\n");
         dbg_printf("info ivt - show interrupt vector table\n");
         dbg_printf("info gdt - show global descriptor table\n");
         dbg_printf("info tss - show current task state segment\n");
         dbg_printf("info tab - show page tables\n");
         dbg_printf("info eflags - show decoded EFLAGS register\n");
         dbg_printf("info symbols [string] - list symbols whose prefix is string\n");
         dbg_printf("info pic - show PICs registers\n");
         dbg_printf("info ne2000 - show NE2000 registers\n");
         dbg_printf("info vga - show vga registers\n");
         dbg_printf("info pci - show i440fx PCI state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 212:
#line 1134 "parser.y"
    {
         dbg_printf("show <command> - toggles show symbolic info (calls to begin with)\n");
         dbg_printf("show - shows current show mode\n");
         dbg_printf("show mode - show, when processor switch mode\n");
         dbg_printf("show int - show, when interrupt is happens\n");
         dbg_printf("show call - show, when call is happens\n");
         dbg_printf("show off - toggles off symbolic info\n");
         dbg_printf("show dbg-all - turn on all show flags\n");
         dbg_printf("show dbg-none - turn off all show flags\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 213:
#line 1146 "parser.y"
    {
         dbg_printf("calc|? <expr> - calculate a expression and display the result.\n");
         dbg_printf("    'expr' can reference any general-purpose and segment\n");
         dbg_printf("    registers, use any arithmetic and logic operations, and\n");
         dbg_printf("    also the special ':' operator which computes the linear\n");
         dbg_printf("    address for a segment:offset (in real and v86 mode) or\n");
         dbg_printf("    of a selector:offset (in protected mode) pair.\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 214:
#line 1156 "parser.y"
    {
         bx_dbg_print_help();
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 215:
#line 1161 "parser.y"
    {
         bx_dbg_print_help();
         free((yyvsp[(1) - (2)].sval));
       }
    break;

  case 216:
#line 1169 "parser.y"
    {
     bx_dbg_calc_command((yyvsp[(2) - (3)].uval));
     free((yyvsp[(1) - (3)].sval));
   }
    break;

  case 217:
#line 1186 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (1)].uval); }
    break;

  case 218:
#line 1187 "parser.y"
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[(1) - (1)].sval)); free((yyvsp[(1) - (1)].sval));}
    break;

  case 219:
#line 1188 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 220:
#line 1189 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 221:
#line 1190 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 222:
#line 1191 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 223:
#line 1192 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 224:
#line 1193 "parser.y"
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 225:
#line 1194 "parser.y"
    { (yyval.uval) = bx_dbg_get_ip (); }
    break;

  case 226:
#line 1195 "parser.y"
    { (yyval.uval) = bx_dbg_get_eip(); }
    break;

  case 227:
#line 1196 "parser.y"
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
    break;

  case 228:
#line 1197 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) + (yyvsp[(3) - (3)].uval); }
    break;

  case 229:
#line 1198 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) - (yyvsp[(3) - (3)].uval); }
    break;

  case 230:
#line 1199 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) * (yyvsp[(3) - (3)].uval); }
    break;

  case 231:
#line 1200 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) / (yyvsp[(3) - (3)].uval); }
    break;

  case 232:
#line 1201 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) >> (yyvsp[(3) - (3)].uval); }
    break;

  case 233:
#line 1202 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) << (yyvsp[(3) - (3)].uval); }
    break;

  case 234:
#line 1203 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) | (yyvsp[(3) - (3)].uval); }
    break;

  case 235:
#line 1204 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) ^ (yyvsp[(3) - (3)].uval); }
    break;

  case 236:
#line 1205 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) & (yyvsp[(3) - (3)].uval); }
    break;

  case 237:
#line 1206 "parser.y"
    { (yyval.uval) = !(yyvsp[(2) - (2)].uval); }
    break;

  case 238:
#line 1207 "parser.y"
    { (yyval.uval) = -(yyvsp[(2) - (2)].uval); }
    break;

  case 239:
#line 1208 "parser.y"
    { (yyval.uval) = (yyvsp[(2) - (3)].uval); }
    break;

  case 240:
#line 1213 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (1)].uval); }
    break;

  case 241:
#line 1214 "parser.y"
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[(1) - (1)].sval)); free((yyvsp[(1) - (1)].sval));}
    break;

  case 242:
#line 1215 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 243:
#line 1216 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 244:
#line 1217 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 245:
#line 1218 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 246:
#line 1219 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 247:
#line 1220 "parser.y"
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 248:
#line 1221 "parser.y"
    { (yyval.uval) = bx_dbg_get_ip (); }
    break;

  case 249:
#line 1222 "parser.y"
    { (yyval.uval) = bx_dbg_get_eip(); }
    break;

  case 250:
#line 1223 "parser.y"
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
    break;

  case 251:
#line 1224 "parser.y"
    { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[(1) - (3)].uval), (yyvsp[(3) - (3)].uval)); }
    break;

  case 252:
#line 1225 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) + (yyvsp[(3) - (3)].uval); }
    break;

  case 253:
#line 1226 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) - (yyvsp[(3) - (3)].uval); }
    break;

  case 254:
#line 1227 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) * (yyvsp[(3) - (3)].uval); }
    break;

  case 255:
#line 1228 "parser.y"
    { (yyval.uval) = ((yyvsp[(3) - (3)].uval) != 0) ? (yyvsp[(1) - (3)].uval) / (yyvsp[(3) - (3)].uval) : 0; }
    break;

  case 256:
#line 1229 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) >> (yyvsp[(3) - (3)].uval); }
    break;

  case 257:
#line 1230 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) << (yyvsp[(3) - (3)].uval); }
    break;

  case 258:
#line 1231 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) | (yyvsp[(3) - (3)].uval); }
    break;

  case 259:
#line 1232 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) ^ (yyvsp[(3) - (3)].uval); }
    break;

  case 260:
#line 1233 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) & (yyvsp[(3) - (3)].uval); }
    break;

  case 261:
#line 1234 "parser.y"
    { (yyval.uval) = !(yyvsp[(2) - (2)].uval); }
    break;

  case 262:
#line 1235 "parser.y"
    { (yyval.uval) = -(yyvsp[(2) - (2)].uval); }
    break;

  case 263:
#line 1236 "parser.y"
    { (yyval.uval) = (yyvsp[(2) - (3)].uval); }
    break;


/* Line 1267 of yacc.c.  */
#line 3761 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
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
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1239 "parser.y"


#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
