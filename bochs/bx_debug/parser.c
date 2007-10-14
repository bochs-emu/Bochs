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
     BX_TOKEN_REGISTERS = 287,
     BX_TOKEN_CPU = 288,
     BX_TOKEN_FPU = 289,
     BX_TOKEN_SSE = 290,
     BX_TOKEN_MMX = 291,
     BX_TOKEN_ALL = 292,
     BX_TOKEN_IDT = 293,
     BX_TOKEN_IVT = 294,
     BX_TOKEN_GDT = 295,
     BX_TOKEN_LDT = 296,
     BX_TOKEN_TSS = 297,
     BX_TOKEN_TAB = 298,
     BX_TOKEN_DIRTY = 299,
     BX_TOKEN_LINUX = 300,
     BX_TOKEN_CONTROL_REGS = 301,
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
     BX_TOKEN_DUMP_CPU = 313,
     BX_TOKEN_DISASSEMBLE = 314,
     BX_TOKEN_INSTRUMENT = 315,
     BX_TOKEN_STRING = 316,
     BX_TOKEN_STOP = 317,
     BX_TOKEN_DOIT = 318,
     BX_TOKEN_CRC = 319,
     BX_TOKEN_TRACE = 320,
     BX_TOKEN_TRACEREG = 321,
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
     BX_TOKEN_COMMAND = 349,
     BX_TOKEN_GENERIC = 350,
     BX_TOKEN_RSHIFT = 351,
     BX_TOKEN_LSHIFT = 352,
     BX_TOKEN_REG_IP = 353,
     BX_TOKEN_REG_EIP = 354,
     BX_TOKEN_REG_RIP = 355,
     NEG = 356,
     NOT = 357
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
#define BX_TOKEN_ALL 292
#define BX_TOKEN_IDT 293
#define BX_TOKEN_IVT 294
#define BX_TOKEN_GDT 295
#define BX_TOKEN_LDT 296
#define BX_TOKEN_TSS 297
#define BX_TOKEN_TAB 298
#define BX_TOKEN_DIRTY 299
#define BX_TOKEN_LINUX 300
#define BX_TOKEN_CONTROL_REGS 301
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
#define BX_TOKEN_DUMP_CPU 313
#define BX_TOKEN_DISASSEMBLE 314
#define BX_TOKEN_INSTRUMENT 315
#define BX_TOKEN_STRING 316
#define BX_TOKEN_STOP 317
#define BX_TOKEN_DOIT 318
#define BX_TOKEN_CRC 319
#define BX_TOKEN_TRACE 320
#define BX_TOKEN_TRACEREG 321
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
#define BX_TOKEN_COMMAND 349
#define BX_TOKEN_GENERIC 350
#define BX_TOKEN_RSHIFT 351
#define BX_TOKEN_LSHIFT 352
#define BX_TOKEN_REG_IP 353
#define BX_TOKEN_REG_EIP 354
#define BX_TOKEN_REG_RIP 355
#define NEG 356
#define NOT 357




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
#line 322 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 335 "y.tab.c"

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
#define YYFINAL  253
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1051

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  116
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  47
/* YYNRULES -- Number of rules.  */
#define YYNRULES  241
/* YYNRULES -- Number of states.  */
#define YYNSTATES  471

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   357

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     110,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   113,     2,     2,     2,     2,   107,     2,
     114,   115,   105,   101,     2,   102,     2,   106,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   112,     2,
       2,   111,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   104,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   103,     2,     2,     2,     2,     2,
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
      95,    96,    97,    98,    99,   100,   108,   109
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
      80,    82,    84,    86,    87,    89,    91,    93,    95,    97,
      99,   101,   103,   105,   109,   113,   117,   121,   124,   128,
     132,   135,   139,   142,   146,   150,   153,   157,   161,   165,
     168,   171,   176,   181,   186,   191,   195,   200,   206,   209,
     213,   216,   219,   223,   226,   231,   237,   243,   249,   255,
     261,   267,   270,   276,   279,   283,   287,   290,   294,   299,
     302,   305,   309,   313,   317,   321,   325,   329,   333,   337,
     341,   347,   353,   359,   365,   369,   373,   377,   381,   385,
     390,   394,   398,   404,   412,   416,   420,   421,   423,   426,
     429,   433,   437,   441,   444,   449,   453,   457,   460,   465,
     471,   475,   479,   484,   488,   491,   495,   500,   504,   509,
     515,   519,   525,   529,   533,   537,   542,   546,   550,   554,
     558,   562,   566,   570,   574,   578,   582,   586,   590,   594,
     598,   602,   606,   610,   614,   618,   622,   626,   630,   634,
     638,   642,   646,   650,   654,   658,   662,   666,   670,   674,
     678,   682,   686,   690,   694,   697,   701,   703,   705,   707,
     709,   711,   713,   715,   717,   719,   721,   725,   729,   733,
     737,   741,   745,   749,   753,   757,   760,   763,   767,   769,
     771,   773,   775,   777,   779,   781,   783,   785,   787,   789,
     793,   797,   801,   805,   809,   813,   817,   821,   825,   829,
     832,   835
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     117,     0,    -1,   117,   118,    -1,   118,    -1,   135,    -1,
     136,    -1,   137,    -1,   138,    -1,   139,    -1,   142,    -1,
     144,    -1,   140,    -1,   141,    -1,   145,    -1,   146,    -1,
     147,    -1,   148,    -1,   149,    -1,   150,    -1,   151,    -1,
     152,    -1,   153,    -1,   154,    -1,   155,    -1,   156,    -1,
     157,    -1,   158,    -1,   128,    -1,   129,    -1,   127,    -1,
     121,    -1,   122,    -1,   123,    -1,   124,    -1,   130,    -1,
     131,    -1,   126,    -1,   125,    -1,   132,    -1,   133,    -1,
     134,    -1,   159,    -1,   160,    -1,    -1,   110,    -1,    15,
      -1,    16,    -1,     8,    -1,     9,    -1,    10,    -1,    11,
      -1,    12,    -1,    13,    -1,    71,    87,   110,    -1,    70,
      87,   110,    -1,    72,    61,   110,    -1,    73,    61,   110,
      -1,    74,   110,    -1,    80,    94,   110,    -1,    80,    61,
     110,    -1,    80,   110,    -1,    90,    87,   110,    -1,    69,
     110,    -1,    65,   119,   110,    -1,    66,   119,   110,    -1,
      75,   110,    -1,    75,    87,   110,    -1,    76,    62,   110,
      -1,    76,    17,   110,    -1,    76,   110,    -1,    77,   110,
      -1,    76,    78,    87,   110,    -1,    77,    78,    87,   110,
      -1,    76,    79,    87,   110,    -1,    77,    79,    87,   110,
      -1,    81,    61,   110,    -1,    81,    61,    87,   110,    -1,
      81,    84,    61,    87,   110,    -1,    85,   110,    -1,    86,
      87,   110,    -1,    17,   110,    -1,    18,   110,    -1,    18,
      87,   110,    -1,    19,   110,    -1,    21,    59,   119,   110,
      -1,    21,    52,   111,    87,   110,    -1,    21,     4,   111,
     162,   110,    -1,    21,     3,   111,   162,   110,    -1,    21,
       5,   111,   162,   110,    -1,    21,     6,   111,   162,   110,
      -1,    21,     7,   111,   162,   110,    -1,    24,   110,    -1,
      24,   161,   112,   161,   110,    -1,    25,   110,    -1,    25,
     162,   110,    -1,    25,    61,   110,    -1,    26,   110,    -1,
      26,   162,   110,    -1,    26,   105,   162,   110,    -1,    23,
     110,    -1,    83,   110,    -1,    83,    61,   110,    -1,    30,
      26,   110,    -1,    30,    33,   110,    -1,    30,    32,   110,
      -1,    30,    34,   110,    -1,    30,    36,   110,    -1,    30,
      35,   110,    -1,    30,    37,   110,    -1,    30,    44,   110,
      -1,    30,    38,   143,   143,   110,    -1,    30,    39,   143,
     143,   110,    -1,    30,    40,   143,   143,   110,    -1,    30,
      41,   143,   143,   110,    -1,    30,    43,   110,    -1,    30,
      42,   110,    -1,    30,    14,   110,    -1,    30,    45,   110,
      -1,    30,    82,   110,    -1,    30,    82,    61,   110,    -1,
      30,    46,   110,    -1,    30,    88,   110,    -1,    30,    88,
      90,    87,   110,    -1,    30,    88,    90,    87,    32,    87,
     110,    -1,    30,    89,   110,    -1,    30,    93,   110,    -1,
      -1,    87,    -1,    32,   110,    -1,    58,   110,    -1,    27,
      87,   110,    -1,    28,    87,   110,    -1,    29,    87,   110,
      -1,    31,   110,    -1,    47,    48,   162,   110,    -1,    47,
      48,   110,    -1,    47,   162,   110,    -1,    47,   110,    -1,
      50,    61,    61,   110,    -1,    51,    87,    87,    87,   110,
      -1,    53,    54,   110,    -1,    55,    56,   110,    -1,    55,
      56,    87,   110,    -1,    55,    57,   110,    -1,    59,   110,
      -1,    59,   162,   110,    -1,    59,   162,   162,   110,    -1,
      59,    49,   110,    -1,    59,    49,   162,   110,    -1,    59,
      49,   162,   162,   110,    -1,    59,    67,   110,    -1,    59,
      68,   111,    87,   110,    -1,    60,    62,   110,    -1,    60,
      94,   110,    -1,    63,    87,   110,    -1,    64,    87,    87,
     110,    -1,    91,    31,   110,    -1,    91,    17,   110,    -1,
      91,    18,   110,    -1,    91,    19,   110,    -1,    91,    24,
     110,    -1,    91,    25,   110,    -1,    91,    26,   110,    -1,
      91,    27,   110,    -1,    91,    28,   110,    -1,    91,    29,
     110,    -1,    91,    23,   110,    -1,    91,    74,   110,    -1,
      91,    64,   110,    -1,    91,    65,   110,    -1,    91,    66,
     110,    -1,    91,    58,   110,    -1,    91,    50,   110,    -1,
      91,    69,   110,    -1,    91,    71,   110,    -1,    91,    70,
     110,    -1,    91,    75,   110,    -1,    91,    72,   110,    -1,
      91,    73,   110,    -1,    91,    81,   110,    -1,    91,    83,
     110,    -1,    91,    32,   110,    -1,    91,    51,   110,    -1,
      91,    59,   110,    -1,    91,    76,   110,    -1,    91,    77,
     110,    -1,    91,    47,   110,    -1,    91,    60,   110,    -1,
      91,    21,   110,    -1,    91,    90,   110,    -1,    91,    30,
     110,    -1,    91,    80,   110,    -1,    91,    92,   110,    -1,
      91,    91,   110,    -1,    91,   110,    -1,    92,   162,   110,
      -1,    87,    -1,     4,    -1,     3,    -1,     5,    -1,     6,
      -1,     7,    -1,   120,    -1,    98,    -1,    99,    -1,   100,
      -1,   161,   101,   161,    -1,   161,   102,   161,    -1,   161,
     105,   161,    -1,   161,   106,   161,    -1,   161,    96,   161,
      -1,   161,    97,   161,    -1,   161,   103,   161,    -1,   161,
     104,   161,    -1,   161,   107,   161,    -1,   113,   161,    -1,
     102,   161,    -1,   114,   161,   115,    -1,    87,    -1,    61,
      -1,     4,    -1,     3,    -1,     5,    -1,     6,    -1,     7,
      -1,   120,    -1,    98,    -1,    99,    -1,   100,    -1,   162,
     112,   162,    -1,   162,   101,   162,    -1,   162,   102,   162,
      -1,   162,   105,   162,    -1,   162,   106,   162,    -1,   162,
      96,   162,    -1,   162,    97,   162,    -1,   162,   103,   162,
      -1,   162,   104,   162,    -1,   162,   107,   162,    -1,   113,
     162,    -1,   102,   162,    -1,   114,   162,   115,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   134,   134,   135,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   185,   186,   191,   192,   193,
     194,   195,   196,   201,   206,   214,   222,   230,   238,   243,
     248,   256,   264,   272,   280,   288,   293,   301,   307,   313,
     318,   323,   328,   333,   338,   346,   351,   356,   364,   372,
     380,   388,   393,   401,   409,   414,   419,   423,   427,   431,
     435,   442,   447,   452,   457,   462,   467,   472,   477,   485,
     493,   498,   506,   511,   516,   521,   526,   531,   536,   541,
     546,   551,   556,   561,   566,   571,   576,   581,   586,   591,
     596,   601,   606,   611,   616,   621,   629,   630,   633,   641,
     649,   657,   664,   672,   680,   685,   690,   695,   703,   711,
     719,   727,   732,   737,   745,   750,   755,   760,   765,   770,
     775,   780,   788,   793,   801,   809,   817,   822,   827,   832,
     837,   842,   847,   852,   857,   862,   867,   872,   877,   882,
     888,   894,   899,   907,   912,   917,   922,   927,   932,   937,
     942,   947,   952,   957,   966,   975,   982,   995,  1003,  1013,
    1018,  1040,  1052,  1062,  1067,  1075,  1093,  1094,  1095,  1096,
    1097,  1098,  1099,  1100,  1101,  1102,  1103,  1104,  1105,  1106,
    1107,  1108,  1109,  1110,  1111,  1112,  1113,  1114,  1119,  1120,
    1121,  1122,  1123,  1124,  1125,  1126,  1127,  1128,  1129,  1130,
    1131,  1132,  1133,  1134,  1135,  1136,  1137,  1138,  1139,  1140,
    1141,  1142
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
  "BX_TOKEN_REGISTERS", "BX_TOKEN_CPU", "BX_TOKEN_FPU", "BX_TOKEN_SSE",
  "BX_TOKEN_MMX", "BX_TOKEN_ALL", "BX_TOKEN_IDT", "BX_TOKEN_IVT",
  "BX_TOKEN_GDT", "BX_TOKEN_LDT", "BX_TOKEN_TSS", "BX_TOKEN_TAB",
  "BX_TOKEN_DIRTY", "BX_TOKEN_LINUX", "BX_TOKEN_CONTROL_REGS",
  "BX_TOKEN_EXAMINE", "BX_TOKEN_XFORMAT", "BX_TOKEN_DISFORMAT",
  "BX_TOKEN_RESTORE", "BX_TOKEN_SETPMEM", "BX_TOKEN_SYMBOLNAME",
  "BX_TOKEN_QUERY", "BX_TOKEN_PENDING", "BX_TOKEN_TAKE", "BX_TOKEN_DMA",
  "BX_TOKEN_IRQ", "BX_TOKEN_DUMP_CPU", "BX_TOKEN_DISASSEMBLE",
  "BX_TOKEN_INSTRUMENT", "BX_TOKEN_STRING", "BX_TOKEN_STOP",
  "BX_TOKEN_DOIT", "BX_TOKEN_CRC", "BX_TOKEN_TRACE", "BX_TOKEN_TRACEREG",
  "BX_TOKEN_SWITCH_MODE", "BX_TOKEN_SIZE", "BX_TOKEN_PTIME",
  "BX_TOKEN_TIMEBP_ABSOLUTE", "BX_TOKEN_TIMEBP", "BX_TOKEN_RECORD",
  "BX_TOKEN_PLAYBACK", "BX_TOKEN_MODEBP", "BX_TOKEN_PRINT_STACK",
  "BX_TOKEN_WATCH", "BX_TOKEN_UNWATCH", "BX_TOKEN_READ", "BX_TOKEN_WRITE",
  "BX_TOKEN_SHOW", "BX_TOKEN_LOAD_SYMBOLS", "BX_TOKEN_SYMBOLS",
  "BX_TOKEN_LIST_SYMBOLS", "BX_TOKEN_GLOBAL", "BX_TOKEN_WHERE",
  "BX_TOKEN_PRINT_STRING", "BX_TOKEN_NUMERIC", "BX_TOKEN_NE2000",
  "BX_TOKEN_PIC", "BX_TOKEN_PAGE", "BX_TOKEN_HELP", "BX_TOKEN_CALC",
  "BX_TOKEN_VGA", "BX_TOKEN_COMMAND", "BX_TOKEN_GENERIC",
  "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT", "BX_TOKEN_REG_IP",
  "BX_TOKEN_REG_EIP", "BX_TOKEN_REG_RIP", "'+'", "'-'", "'|'", "'^'",
  "'*'", "'/'", "'&'", "NEG", "NOT", "'\\n'", "'='", "':'", "'!'", "'('",
  "')'", "$accept", "commands", "command", "BX_TOKEN_TOGGLE_ON_OFF",
  "BX_TOKEN_SEGREG", "timebp_command", "record_command",
  "playback_command", "modebp_command", "show_command", "page_command",
  "ptime_command", "trace_command", "trace_reg_command",
  "print_stack_command", "watch_point_command", "symbol_command",
  "where_command", "print_string_command", "continue_command",
  "stepN_command", "step_over_command", "set_command",
  "breakpoint_command", "blist_command", "slist_command", "info_command",
  "optional_numeric", "regs_command", "dump_cpu_command", "delete_command",
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
     355,    43,    45,   124,    94,    42,    47,    38,   356,   357,
      10,    61,    58,    33,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   116,   117,   117,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   119,   119,   120,   120,   120,
     120,   120,   120,   121,   121,   122,   123,   124,   125,   125,
     125,   126,   127,   128,   129,   130,   130,   131,   131,   131,
     131,   131,   131,   131,   131,   132,   132,   132,   133,   134,
     135,   136,   136,   137,   138,   138,   138,   138,   138,   138,
     138,   139,   139,   139,   139,   139,   139,   139,   139,   140,
     141,   141,   142,   142,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   143,   143,   144,   145,
     146,   147,   148,   149,   150,   150,   150,   150,   151,   152,
     153,   154,   154,   154,   155,   155,   155,   155,   155,   155,
     155,   155,   156,   156,   157,   158,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   160,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     2,     3,     3,
       2,     3,     2,     3,     3,     2,     3,     3,     3,     2,
       2,     4,     4,     4,     4,     3,     4,     5,     2,     3,
       2,     2,     3,     2,     4,     5,     5,     5,     5,     5,
       5,     2,     5,     2,     3,     3,     2,     3,     4,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       5,     5,     5,     5,     3,     3,     3,     3,     3,     4,
       3,     3,     5,     7,     3,     3,     0,     1,     2,     2,
       3,     3,     3,     2,     4,     3,     3,     2,     4,     5,
       3,     3,     4,     3,     2,     3,     4,     3,     4,     5,
       3,     5,     3,     3,     3,     4,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      43,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    44,     0,     3,    30,    31,    32,
      33,    37,    36,    29,    27,    28,    34,    35,    38,    39,
      40,     4,     5,     6,     7,     8,    11,    12,     9,    10,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    41,    42,    80,     0,    81,    83,
       0,     0,     0,     0,     0,     0,     0,    99,   198,   197,
     199,   200,   201,    47,    48,    49,    50,    51,    52,   196,
     203,   204,   205,     0,    91,     0,     0,   202,     0,   221,
     220,   222,   223,   224,   219,   218,   226,   227,   228,     0,
      93,     0,     0,   225,     0,   219,     0,    96,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     126,   126,   126,   126,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   133,   128,     0,   137,     0,     0,     0,
       0,     0,     0,   129,     0,     0,     0,   144,     0,     0,
       0,     0,     0,    45,    46,     0,     0,    62,     0,     0,
       0,     0,    57,     0,    65,     0,     0,     0,     0,    69,
       0,     0,    70,     0,     0,    60,     0,     0,     0,   100,
      78,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   194,     0,     1,     2,    82,     0,     0,     0,     0,
       0,     0,     0,   216,   215,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    95,   240,   239,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    94,
       0,     0,    97,   130,   131,   132,   116,   102,   104,   103,
     105,   107,   106,   108,   127,   126,   126,   126,   126,   115,
     114,   109,   117,   120,     0,   118,     0,   121,   124,   125,
     135,     0,   136,     0,     0,   140,     0,   141,   143,   147,
       0,   150,     0,     0,   145,     0,   152,   153,   154,     0,
      63,    64,    54,    53,    55,    56,    66,    68,    67,     0,
       0,     0,     0,    59,    58,     0,    75,     0,   101,    79,
      61,   157,   158,   159,   188,   166,   160,   161,   162,   163,
     164,   165,   190,   156,   181,   186,   172,   182,   171,   183,
     187,   168,   169,   170,   173,   175,   174,   177,   178,   167,
     176,   184,   185,   191,   179,   180,   189,   193,   192,   195,
       0,     0,     0,     0,     0,     0,    84,   217,   210,   211,
     206,   207,   212,   213,   208,   209,   214,     0,   241,   234,
     235,   230,   231,   236,   237,   232,   233,   238,   229,    98,
       0,     0,     0,     0,   119,     0,   134,   138,     0,   142,
     148,     0,     0,   231,   146,   155,    71,    73,    72,    74,
      76,     0,    87,    86,    88,    89,    90,    85,    92,   110,
     111,   112,   113,     0,   122,   139,   149,   151,    77,     0,
     123
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    45,    46,   185,   133,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,   305,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,   118,   134
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -148
static const yytype_int16 yypact[] =
{
     709,  -100,   -79,   -98,    22,   -96,   403,    54,   210,   -68,
     -36,   -33,   554,   -86,   -62,   344,     8,   -15,     2,    34,
     -27,   232,   -45,     5,    25,    80,    80,   -21,    26,    27,
      48,    56,     9,   -78,     6,   -63,   -39,   -50,   -31,    12,
      31,    38,   785,   515,  -148,   633,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,    16,  -148,  -148,
      37,    39,    40,    51,    52,    58,    80,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,   532,  -148,   532,   532,  -148,   570,  -148,
    -148,  -148,  -148,  -148,    17,  -148,  -148,  -148,  -148,   515,
    -148,   515,   515,  -148,   777,  -148,   515,  -148,   795,    32,
      55,    72,    73,    74,    75,    76,    77,    78,    79,    92,
      57,    57,    57,    57,    93,    95,    99,   100,   101,   -28,
     -40,   102,   114,  -148,  -148,   361,  -148,   807,    88,   138,
     116,   -35,   117,  -148,   378,   118,   119,  -148,    33,   121,
     122,   123,   142,  -148,  -148,   124,   143,  -148,   144,   148,
     149,   173,  -148,   174,  -148,   177,   178,   202,   203,  -148,
     204,   205,  -148,   188,   192,  -148,   -34,   199,   193,  -148,
    -148,   194,   195,   196,   197,   201,   206,   207,   208,   211,
     216,   217,   218,   219,   223,   225,   226,   227,   228,   229,
     230,   231,   233,   234,   248,   249,   250,   251,   252,   253,
     265,   266,   267,   268,   269,   270,   283,   284,   285,   286,
     287,  -148,   819,  -148,  -148,  -148,   515,   515,   515,   515,
     515,   311,   289,  -148,  -148,   450,   532,   532,   532,   532,
     532,   532,   532,   532,   532,   532,  -148,   288,   288,   322,
     515,   515,   515,   515,   515,   515,   515,   515,   515,  -148,
     515,   831,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,    57,    57,    57,    57,  -148,
    -148,  -148,  -148,  -148,   291,  -148,   315,  -148,  -148,  -148,
    -148,   843,  -148,   293,   317,  -148,   307,  -148,  -148,  -148,
      94,  -148,   333,   515,  -148,   855,  -148,  -148,  -148,   320,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,   323,
     325,   326,   328,  -148,  -148,   330,  -148,   334,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
     867,   879,   891,   903,   915,   331,  -148,  -148,  -148,  -148,
     150,   150,   150,   150,  -148,  -148,  -148,   648,  -148,   288,
     288,   -19,   -19,   -19,   -19,   288,   288,   288,   939,  -148,
     335,   337,   339,   340,  -148,   -30,  -148,  -148,   341,  -148,
    -148,   927,   342,   189,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,   343,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,   345,  -148,  -148,  -148,  -148,  -148,   346,
    -148
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -148,  -148,   410,   -23,    -5,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -147,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,   -95,    -8
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -241
static const yytype_int16 yytable[] =
{
     138,   117,   463,   186,   306,   307,   308,   167,    87,   193,
      86,   206,    89,   178,    97,   200,   201,   179,   263,   139,
     264,   265,   203,   195,   163,    90,    91,    92,    93,    94,
     208,    88,   194,   314,   207,   252,   119,   120,   121,   122,
     123,   103,   104,   105,   106,   107,   108,   202,   164,   180,
     316,   140,   326,   355,   141,   204,   170,   119,   120,   121,
     122,   123,   103,   104,   105,   106,   107,   108,   196,   168,
     317,   205,   169,   262,    95,   327,   356,   280,   281,   209,
     464,    96,   315,   173,   197,   198,   286,   287,   288,   187,
     171,   172,   181,   290,   135,   183,   184,   119,   120,   121,
     122,   123,   103,   104,   105,   106,   107,   108,   117,   190,
     117,   117,   182,   188,   189,   124,   199,   191,   211,   192,
     125,   277,   210,   278,   279,   212,   255,   276,   291,   280,
     281,   126,   127,   128,   282,   333,   284,   285,   286,   287,
     288,   125,   293,   334,   304,   290,   131,   132,   256,   323,
     257,   258,   126,   127,   128,   135,   129,   321,   430,   431,
     432,   433,   259,   260,   130,   294,   330,   131,   132,   261,
     335,   408,   409,   410,   411,   412,   413,   414,   415,   416,
     417,   125,   295,   296,   297,   298,   299,   300,   301,   302,
     280,   281,   126,   127,   128,   282,   333,   284,   285,   286,
     287,   288,   303,   309,   440,   310,   290,   131,   132,   311,
     312,   313,   318,   119,   120,   121,   122,   123,   103,   104,
     105,   106,   107,   108,   319,   324,   325,   328,   331,   339,
     332,   336,   337,   338,   340,   119,   120,   121,   122,   123,
     103,   104,   105,   106,   107,   108,   266,   267,   400,   401,
     402,   403,   404,   341,   342,   272,   273,   274,   343,   344,
     357,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   135,   419,   420,   421,   422,   423,   424,   425,   426,
     427,   174,   428,   345,   346,  -240,  -240,   347,   348,   349,
     350,   351,   352,   135,  -240,  -240,  -240,   125,   353,   175,
     176,   290,   354,   358,   359,   360,   361,   362,   126,   127,
     128,   363,   129,     0,     0,   136,   364,   365,   366,   125,
     137,   367,   441,   131,   132,   443,   368,   369,   370,   371,
     126,   127,   128,   372,   129,   373,   374,   375,   376,   377,
     378,   379,   177,   380,   381,   131,   132,   119,   120,   121,
     122,   123,   103,   104,   105,   106,   107,   108,   382,   383,
     384,   385,   386,   387,   119,   120,   121,   122,   123,   103,
     104,   105,   106,   107,   108,   388,   389,   390,   391,   392,
     393,   119,   120,   121,   122,   123,   103,   104,   105,   106,
     107,   108,   165,   394,   395,   396,   397,   398,   405,   406,
     290,   434,   435,   437,   438,   135,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   439,   280,   281,
     442,   451,   135,   282,   283,   284,   285,   286,   287,   288,
     445,   125,   469,   446,   290,   447,   448,   418,   449,   135,
     450,   457,   126,   127,   128,   459,   129,   460,   125,   461,
     462,   465,   467,   468,   166,   254,   470,   131,   132,   126,
     127,   128,     0,   129,     0,   125,     0,     0,     0,     0,
       0,   320,     0,     0,   131,   132,   126,   127,   128,     0,
     129,     0,     0,     0,     0,     0,     0,     0,   329,     0,
     109,   131,   132,     0,     0,     0,     0,     0,     0,     0,
       0,   110,   111,   112,     0,   113,     0,     0,     0,     0,
       0,     0,     0,   114,     0,     0,   115,   116,   119,   120,
     121,   122,   123,   103,   104,   105,   106,   107,   108,     0,
       0,     0,     0,     0,     0,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   266,   267,     0,     0,
       0,   268,   269,   270,   271,   272,   273,   274,     0,     0,
       0,     0,     0,     0,     0,   407,     0,     0,   142,     0,
       0,     0,     0,     0,     0,     0,   135,     0,     0,     0,
     143,     0,     0,     0,     0,     0,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,     0,   125,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   126,   127,   128,     0,   129,     0,   109,
       0,     0,     0,     0,     0,     0,     0,     0,   131,   132,
     110,   111,   112,   253,   113,     0,   159,     0,     0,     0,
       0,     0,   160,   161,     0,   115,   116,   162,     0,     0,
       1,     2,     3,     0,     4,     0,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,   266,   267,     0,     0,
       0,   268,   269,   270,   271,   272,   273,   274,     0,     0,
      15,     0,   275,    16,    17,     0,    18,     0,    19,     0,
       0,    20,    21,    22,     0,     0,    23,    24,    25,    26,
       0,     0,    27,    28,    29,    30,    31,    32,    33,    34,
      35,     0,     0,    36,    37,     0,    38,     0,    39,    40,
       0,     0,     0,    41,    42,    43,     1,     2,     3,     0,
       4,     0,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,     0,    44,   266,   267,     0,     0,     0,   268,
     269,   270,   271,   272,   273,   274,    15,     0,   458,    16,
      17,     0,    18,     0,    19,     0,     0,    20,    21,    22,
       0,     0,    23,    24,    25,    26,     0,     0,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
      37,     0,    38,     0,    39,    40,     0,     0,     0,    41,
      42,    43,   213,   214,   215,     0,   216,     0,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,     0,    44,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   227,     0,     0,   228,   229,     0,     0,     0,
       0,     0,     0,   230,   231,   232,     0,     0,     0,   233,
     234,   235,     0,     0,   236,   237,   238,   239,   240,   241,
     242,   243,   244,     0,     0,   245,   246,     0,   247,     0,
       0,     0,     0,   280,   281,   248,   249,   250,   282,   283,
     284,   285,   286,   287,   288,     0,     0,   289,     0,   290,
       0,   280,   281,     0,     0,   251,   282,   283,   284,   285,
     286,   287,   288,   280,   281,   292,     0,   290,   282,   283,
     284,   285,   286,   287,   288,   280,   281,   322,     0,   290,
     282,   283,   284,   285,   286,   287,   288,   280,   281,   399,
       0,   290,   282,   283,   284,   285,   286,   287,   288,   280,
     281,   429,     0,   290,   282,   283,   284,   285,   286,   287,
     288,   280,   281,   436,     0,   290,   282,   283,   284,   285,
     286,   287,   288,   280,   281,   444,     0,   290,   282,   283,
     284,   285,   286,   287,   288,   280,   281,   452,     0,   290,
     282,   283,   284,   285,   286,   287,   288,   280,   281,   453,
       0,   290,   282,   283,   284,   285,   286,   287,   288,   280,
     281,   454,     0,   290,   282,   283,   284,   285,   286,   287,
     288,   280,   281,   455,     0,   290,   282,   283,   284,   285,
     286,   287,   288,   280,   281,   456,     0,   290,   282,   283,
     284,   285,   286,   287,   288,   280,   281,   466,     0,   290,
     282,   283,   284,   285,   286,   287,   288,     0,     0,     0,
       0,   290
};

static const yytype_int16 yycheck[] =
{
       8,     6,    32,    26,   151,   152,   153,    15,    87,    87,
     110,    61,   110,    21,   110,    78,    79,    62,   113,    87,
     115,   116,    61,    17,   110,     3,     4,     5,     6,     7,
      61,   110,   110,    61,    84,    43,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,   110,   110,    94,
      90,    87,    87,    87,    87,    94,    54,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    62,    61,
     110,   110,    87,    96,    52,   110,   110,    96,    97,   110,
     110,    59,   110,   110,    78,    79,   105,   106,   107,   110,
      56,    57,    87,   112,    61,    15,    16,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,   113,    61,
     115,   116,    87,    87,    87,    61,   110,    61,    87,   110,
      87,   129,   110,   131,   132,    87,   110,   110,   136,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,    87,   110,   110,    87,   112,   113,   114,   111,    61,
     111,   111,    98,    99,   100,    61,   102,   165,   305,   306,
     307,   308,   111,   111,   110,   110,   174,   113,   114,   111,
     178,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,    87,   110,   110,   110,   110,   110,   110,   110,   110,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   110,   110,   110,   110,   112,   113,   114,   110,
     110,   110,   110,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,   110,    87,   110,   110,   110,    87,
     111,   110,   110,   110,   110,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    96,    97,   256,   257,
     258,   259,   260,   110,   110,   105,   106,   107,   110,   110,
      61,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,    61,   280,   281,   282,   283,   284,   285,   286,   287,
     288,    49,   290,   110,   110,    96,    97,   110,   110,    87,
      87,    87,    87,    61,   105,   106,   107,    87,   110,    67,
      68,   112,   110,   110,   110,   110,   110,   110,    98,    99,
     100,   110,   102,    -1,    -1,   105,   110,   110,   110,    87,
     110,   110,   330,   113,   114,   333,   110,   110,   110,   110,
      98,    99,   100,   110,   102,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   113,   114,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,   110,   110,
     110,   110,   110,   110,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,   110,   110,   110,   110,   110,
     110,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    48,   110,   110,   110,   110,   110,    87,   110,
     112,   110,    87,   110,    87,    61,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,   110,    96,    97,
      87,    87,    61,   101,   102,   103,   104,   105,   106,   107,
     110,    87,    87,   110,   112,   110,   110,   115,   110,    61,
     110,   110,    98,    99,   100,   110,   102,   110,    87,   110,
     110,   110,   110,   110,   110,    45,   110,   113,   114,    98,
      99,   100,    -1,   102,    -1,    87,    -1,    -1,    -1,    -1,
      -1,   110,    -1,    -1,   113,   114,    98,    99,   100,    -1,
     102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,
      87,   113,   114,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    98,    99,   100,    -1,   102,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   110,    -1,    -1,   113,   114,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    96,    97,    -1,    -1,
      -1,   101,   102,   103,   104,   105,   106,   107,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   115,    -1,    -1,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    -1,
      26,    -1,    -1,    -1,    -1,    -1,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    -1,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    98,    99,   100,    -1,   102,    -1,    87,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,   114,
      98,    99,   100,     0,   102,    -1,    82,    -1,    -1,    -1,
      -1,    -1,    88,    89,    -1,   113,   114,    93,    -1,    -1,
      17,    18,    19,    -1,    21,    -1,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    96,    97,    -1,    -1,
      -1,   101,   102,   103,   104,   105,   106,   107,    -1,    -1,
      47,    -1,   112,    50,    51,    -1,    53,    -1,    55,    -1,
      -1,    58,    59,    60,    -1,    -1,    63,    64,    65,    66,
      -1,    -1,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    -1,    -1,    80,    81,    -1,    83,    -1,    85,    86,
      -1,    -1,    -1,    90,    91,    92,    17,    18,    19,    -1,
      21,    -1,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,   110,    96,    97,    -1,    -1,    -1,   101,
     102,   103,   104,   105,   106,   107,    47,    -1,   110,    50,
      51,    -1,    53,    -1,    55,    -1,    -1,    58,    59,    60,
      -1,    -1,    63,    64,    65,    66,    -1,    -1,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    -1,    -1,    80,
      81,    -1,    83,    -1,    85,    86,    -1,    -1,    -1,    90,
      91,    92,    17,    18,    19,    -1,    21,    -1,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,   110,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    50,    51,    -1,    -1,    -1,
      -1,    -1,    -1,    58,    59,    60,    -1,    -1,    -1,    64,
      65,    66,    -1,    -1,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    -1,    -1,    80,    81,    -1,    83,    -1,
      -1,    -1,    -1,    96,    97,    90,    91,    92,   101,   102,
     103,   104,   105,   106,   107,    -1,    -1,   110,    -1,   112,
      -1,    96,    97,    -1,    -1,   110,   101,   102,   103,   104,
     105,   106,   107,    96,    97,   110,    -1,   112,   101,   102,
     103,   104,   105,   106,   107,    96,    97,   110,    -1,   112,
     101,   102,   103,   104,   105,   106,   107,    96,    97,   110,
      -1,   112,   101,   102,   103,   104,   105,   106,   107,    96,
      97,   110,    -1,   112,   101,   102,   103,   104,   105,   106,
     107,    96,    97,   110,    -1,   112,   101,   102,   103,   104,
     105,   106,   107,    96,    97,   110,    -1,   112,   101,   102,
     103,   104,   105,   106,   107,    96,    97,   110,    -1,   112,
     101,   102,   103,   104,   105,   106,   107,    96,    97,   110,
      -1,   112,   101,   102,   103,   104,   105,   106,   107,    96,
      97,   110,    -1,   112,   101,   102,   103,   104,   105,   106,
     107,    96,    97,   110,    -1,   112,   101,   102,   103,   104,
     105,   106,   107,    96,    97,   110,    -1,   112,   101,   102,
     103,   104,   105,   106,   107,    96,    97,   110,    -1,   112,
     101,   102,   103,   104,   105,   106,   107,    -1,    -1,    -1,
      -1,   112
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    17,    18,    19,    21,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    47,    50,    51,    53,    55,
      58,    59,    60,    63,    64,    65,    66,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    80,    81,    83,    85,
      86,    90,    91,    92,   110,   117,   118,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   110,    87,   110,   110,
       3,     4,     5,     6,     7,    52,    59,   110,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    87,
      98,    99,   100,   102,   110,   113,   114,   120,   161,     3,
       4,     5,     6,     7,    61,    87,    98,    99,   100,   102,
     110,   113,   114,   120,   162,    61,   105,   110,   162,    87,
      87,    87,    14,    26,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    82,
      88,    89,    93,   110,   110,    48,   110,   162,    61,    87,
      54,    56,    57,   110,    49,    67,    68,   110,   162,    62,
      94,    87,    87,    15,    16,   119,   119,   110,    87,    87,
      61,    61,   110,    87,   110,    17,    62,    78,    79,   110,
      78,    79,   110,    61,    94,   110,    61,    84,    61,   110,
     110,    87,    87,    17,    18,    19,    21,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    47,    50,    51,
      58,    59,    60,    64,    65,    66,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    80,    81,    83,    90,    91,
      92,   110,   162,     0,   118,   110,   111,   111,   111,   111,
     111,   111,   119,   161,   161,   161,    96,    97,   101,   102,
     103,   104,   105,   106,   107,   112,   110,   162,   162,   162,
      96,    97,   101,   102,   103,   104,   105,   106,   107,   110,
     112,   162,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,    87,   143,   143,   143,   143,   110,
     110,   110,   110,   110,    61,   110,    90,   110,   110,   110,
     110,   162,   110,    61,    87,   110,    87,   110,   110,   110,
     162,   110,   111,   102,   110,   162,   110,   110,   110,    87,
     110,   110,   110,   110,   110,   110,   110,   110,   110,    87,
      87,    87,    87,   110,   110,    87,   110,    61,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     162,   162,   162,   162,   162,    87,   110,   115,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   115,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   110,
     143,   143,   143,   143,   110,    87,   110,   110,    87,   110,
     110,   162,    87,   162,   110,   110,   110,   110,   110,   110,
     110,    87,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,    32,   110,   110,   110,   110,   110,    87,
     110
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
        case 44:
#line 180 "parser.y"
    {
      }
    break;

  case 46:
#line 187 "parser.y"
    { (yyval.bval)=(yyvsp[(1) - (1)].bval); }
    break;

  case 52:
#line 197 "parser.y"
    { (yyval.uval)=(yyvsp[(1) - (1)].uval); }
    break;

  case 53:
#line 202 "parser.y"
    {
          bx_dbg_timebp_command(0, (yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 54:
#line 207 "parser.y"
    {
          bx_dbg_timebp_command(1, (yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 55:
#line 215 "parser.y"
    {
          bx_dbg_record_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 56:
#line 223 "parser.y"
    {
          bx_dbg_playback_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 57:
#line 231 "parser.y"
    {
          bx_dbg_modebp_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 58:
#line 239 "parser.y"
    {
          bx_dbg_show_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 59:
#line 244 "parser.y"
    {
          bx_dbg_show_param_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 60:
#line 249 "parser.y"
    {
          bx_dbg_show_command(0);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 61:
#line 257 "parser.y"
    {
          bx_dbg_xlate_address((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 62:
#line 265 "parser.y"
    {
          bx_dbg_ptime_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 63:
#line 273 "parser.y"
    {
          bx_dbg_trace_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 64:
#line 281 "parser.y"
    {
          bx_dbg_trace_reg_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 65:
#line 289 "parser.y"
    {
          bx_dbg_print_stack_command(16);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 66:
#line 294 "parser.y"
    {
          bx_dbg_print_stack_command((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 67:
#line 302 "parser.y"
    {
          watchpoint_continue = 0;
          dbg_printf("Will stop on watch points\n");
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 68:
#line 308 "parser.y"
    {
          watchpoint_continue = 1;
          dbg_printf("Will not stop on watch points (they will still be logged)\n");
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 69:
#line 314 "parser.y"
    {
          bx_dbg_watch(-1, 0);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 70:
#line 319 "parser.y"
    {
          bx_dbg_unwatch(-1, 0);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 71:
#line 324 "parser.y"
    {
          bx_dbg_watch(1, (yyvsp[(3) - (4)].uval));
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 72:
#line 329 "parser.y"
    {
          bx_dbg_unwatch(1, (yyvsp[(3) - (4)].uval));
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 73:
#line 334 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (4)].uval));
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 74:
#line 339 "parser.y"
    {
          bx_dbg_unwatch(0, (yyvsp[(3) - (4)].uval));
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 75:
#line 347 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(2) - (3)].sval), 0, 0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 76:
#line 352 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(2) - (4)].sval), 0, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 77:
#line 357 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(3) - (5)].sval), 1, (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval)); free((yyvsp[(3) - (5)].sval));
      }
    break;

  case 78:
#line 365 "parser.y"
    {
        bx_dbg_where_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 79:
#line 373 "parser.y"
    {
        bx_dbg_print_string_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 80:
#line 381 "parser.y"
    {
        bx_dbg_continue_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 81:
#line 389 "parser.y"
    {
        bx_dbg_stepN_command(1);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 82:
#line 394 "parser.y"
    {
        bx_dbg_stepN_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 83:
#line 402 "parser.y"
    {
        bx_dbg_step_over_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 84:
#line 410 "parser.y"
    {
        bx_dbg_set_auto_disassemble((yyvsp[(3) - (4)].bval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 85:
#line 415 "parser.y"
    {
        bx_dbg_set_symbol_command((yyvsp[(2) - (5)].sval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 86:
#line 420 "parser.y"
    { 
        bx_dbg_set_reg8l_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 87:
#line 424 "parser.y"
    { 
        bx_dbg_set_reg8h_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 88:
#line 428 "parser.y"
    { 
        bx_dbg_set_reg16_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 89:
#line 432 "parser.y"
    { 
        bx_dbg_set_reg32_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 90:
#line 436 "parser.y"
    { 
        bx_dbg_set_reg64_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 91:
#line 443 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 92:
#line 448 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval));
      }
    break;

  case 93:
#line 453 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 94:
#line 458 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 95:
#line 463 "parser.y"
    {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 96:
#line 468 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 97:
#line 473 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 98:
#line 478 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 99:
#line 486 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 100:
#line 494 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 101:
#line 499 "parser.y"
    {
        bx_dbg_info_symbols_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 102:
#line 507 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 103:
#line 512 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 104:
#line 517 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 105:
#line 522 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 106:
#line 527 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 107:
#line 532 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 108:
#line 537 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS | BX_INFO_FPU_REGS | BX_INFO_SSE_REGS);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 109:
#line 542 "parser.y"
    {
        bx_dbg_info_dirty_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 110:
#line 547 "parser.y"
    {
        bx_dbg_info_idt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 111:
#line 552 "parser.y"
    {
        bx_dbg_info_ivt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 112:
#line 557 "parser.y"
    {
        bx_dbg_info_gdt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 113:
#line 562 "parser.y"
    {
        bx_dbg_info_ldt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 114:
#line 567 "parser.y"
    {
        bx_dbg_dump_table();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 115:
#line 572 "parser.y"
    {
        bx_dbg_info_tss_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 116:
#line 577 "parser.y"
    {
        bx_dbg_info_flags();
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 117:
#line 582 "parser.y"
    {
        bx_dbg_info_linux_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 118:
#line 587 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 119:
#line 592 "parser.y"
    {
        bx_dbg_info_symbols_command((yyvsp[(3) - (4)].sval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 120:
#line 597 "parser.y"
    {
        bx_dbg_info_control_regs_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 121:
#line 602 "parser.y"
    {
        bx_dbg_info_ne2k(-1, -1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 122:
#line 607 "parser.y"
    {
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval)); free((yyvsp[(3) - (5)].sval));
        bx_dbg_info_ne2k((yyvsp[(4) - (5)].uval), -1);
      }
    break;

  case 123:
#line 612 "parser.y"
    {
        free((yyvsp[(1) - (7)].sval)); free((yyvsp[(2) - (7)].sval)); free((yyvsp[(3) - (7)].sval)); free((yyvsp[(5) - (7)].sval));
        bx_dbg_info_ne2k((yyvsp[(4) - (7)].uval), (yyvsp[(6) - (7)].uval));
      }
    break;

  case 124:
#line 617 "parser.y"
    {
        bx_dbg_info_pic();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 125:
#line 622 "parser.y"
    {
        bx_dbg_info_vga();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 126:
#line 629 "parser.y"
    { (yyval.uval) = EMPTY_ARG; }
    break;

  case 128:
#line 634 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 129:
#line 642 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 130:
#line 650 "parser.y"
    {
        bx_dbg_del_breakpoint_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 131:
#line 658 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[(2) - (3)].uval), 1);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 132:
#line 665 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[(2) - (3)].uval), 0);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 133:
#line 673 "parser.y"
    {
        bx_dbg_quit_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 134:
#line 681 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (4)].sval), (yyvsp[(2) - (4)].sval),1, (yyvsp[(3) - (4)].uval), 1);
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 135:
#line 686 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (3)].sval), (yyvsp[(2) - (3)].sval),1, 0, 0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 136:
#line 691 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (3)].sval), NULL,0, (yyvsp[(2) - (3)].uval), 1);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 137:
#line 696 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (2)].sval), NULL,0, 0, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 138:
#line 704 "parser.y"
    {
        bx_dbg_restore_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].sval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 139:
#line 712 "parser.y"
    {
        bx_dbg_setpmem_command((yyvsp[(2) - (5)].uval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval));
      }
    break;

  case 140:
#line 720 "parser.y"
    {
        bx_dbg_query_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 141:
#line 728 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (3)].sval), 1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 142:
#line 733 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 143:
#line 738 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (3)].sval), 1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 144:
#line 746 "parser.y"
    {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 145:
#line 751 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[(2) - (3)].uval), (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 146:
#line 756 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 147:
#line 761 "parser.y"
    {
        bx_dbg_disassemble_current((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 148:
#line 766 "parser.y"
    {
        bx_dbg_disassemble_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 149:
#line 771 "parser.y"
    {
        bx_dbg_disassemble_command((yyvsp[(2) - (5)].sval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 150:
#line 776 "parser.y"
    {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 151:
#line 781 "parser.y"
    {
        bx_dbg_set_disassemble_size((yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 152:
#line 789 "parser.y"
    {
        bx_dbg_instrument_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 153:
#line 794 "parser.y"
    {
        bx_dbg_instrument_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 154:
#line 802 "parser.y"
    {
        bx_dbg_doit_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 155:
#line 810 "parser.y"
    {
        bx_dbg_crc_command((yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 156:
#line 818 "parser.y"
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 157:
#line 823 "parser.y"
    {
         dbg_printf("c|cont|continue - continue executing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 158:
#line 828 "parser.y"
    {
         dbg_printf("s|step|stepi [count] - execute #count instructions (default is one instruction)\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 159:
#line 833 "parser.y"
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 160:
#line 838 "parser.y"
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 161:
#line 843 "parser.y"
    {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 162:
#line 848 "parser.y"
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 163:
#line 853 "parser.y"
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 164:
#line 858 "parser.y"
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 165:
#line 863 "parser.y"
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 166:
#line 868 "parser.y"
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 167:
#line 873 "parser.y"
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 168:
#line 878 "parser.y"
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 169:
#line 883 "parser.y"
    {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 170:
#line 889 "parser.y"
    {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 171:
#line 895 "parser.y"
    {
         dbg_printf("dump_cpu - dump complete cpu state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 172:
#line 900 "parser.y"
    {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 173:
#line 908 "parser.y"
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 174:
#line 913 "parser.y"
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 175:
#line 918 "parser.y"
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 176:
#line 923 "parser.y"
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 177:
#line 928 "parser.y"
    {
         dbg_printf("record <filename> - record console input to file filename\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 178:
#line 933 "parser.y"
    {
         dbg_printf("playback <filename> - playback console input from file filename\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 179:
#line 938 "parser.y"
    {
         dbg_printf("load-symbols [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 180:
#line 943 "parser.y"
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 181:
#line 948 "parser.y"
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 182:
#line 953 "parser.y"
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 183:
#line 958 "parser.y"
    {
         dbg_printf("u|disasm|disassemble [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm|disassemble switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm|disassemble size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 184:
#line 967 "parser.y"
    {
         dbg_printf("watch - print current watch point status\n");
         dbg_printf("watch stop - stop simulation when a watchpoint is encountred\n");
         dbg_printf("watch continue - do not stop the simulation when watch point is encountred\n");
         dbg_printf("watch read addr - insert a read watch point at physical address addr\n");
         dbg_printf("watch write addr - insert a write watch point at physical address addr\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 185:
#line 976 "parser.y"
    {
         dbg_printf("unwatch - remove all watch points\n");
         dbg_printf("unwatch read addr - remove a read watch point at physical address addr\n");
         dbg_printf("unwatch write addr - remove a write watch point at physical address addr\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 186:
#line 983 "parser.y"
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

  case 187:
#line 996 "parser.y"
    {
         dbg_printf("instrument start - calls bx_instr_start() callback\n");
         dbg_printf("instrument stop  - calls bx_instr_stop () callback\n");
         dbg_printf("instrument reset - calls bx_instr_reset() callback\n");
         dbg_printf("instrument print - calls bx_instr_print() callback\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 188:
#line 1004 "parser.y"
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

  case 189:
#line 1014 "parser.y"
    {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 190:
#line 1019 "parser.y"
    {
         dbg_printf("info break - show information about current breakpoint status\n");
         dbg_printf("info dirty - show physical pages dirtied (written to) since last display\n");
         dbg_printf("info r|reg|regs|registers - list of CPU integer registers and their contents\n");
         dbg_printf("info cpu - list of CPU registers and their contents\n");
         dbg_printf("info fpu - list of FPU registers and their contents\n");
         dbg_printf("info mmx - list of MMX registers and their contents\n");
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
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 191:
#line 1041 "parser.y"
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

  case 192:
#line 1053 "parser.y"
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

  case 193:
#line 1063 "parser.y"
    {
         bx_dbg_print_help();
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 194:
#line 1068 "parser.y"
    {
         bx_dbg_print_help();
         free((yyvsp[(1) - (2)].sval));
       }
    break;

  case 195:
#line 1076 "parser.y"
    {
     bx_dbg_calc_command((yyvsp[(2) - (3)].uval));
     free((yyvsp[(1) - (3)].sval));
   }
    break;

  case 196:
#line 1093 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (1)].uval); }
    break;

  case 197:
#line 1094 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 198:
#line 1095 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 199:
#line 1096 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 200:
#line 1097 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 201:
#line 1098 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 202:
#line 1099 "parser.y"
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 203:
#line 1100 "parser.y"
    { (yyval.uval) = bx_dbg_get_ip (); }
    break;

  case 204:
#line 1101 "parser.y"
    { (yyval.uval) = bx_dbg_get_eip(); }
    break;

  case 205:
#line 1102 "parser.y"
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
    break;

  case 206:
#line 1103 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) + (yyvsp[(3) - (3)].uval); }
    break;

  case 207:
#line 1104 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) - (yyvsp[(3) - (3)].uval); }
    break;

  case 208:
#line 1105 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) * (yyvsp[(3) - (3)].uval); }
    break;

  case 209:
#line 1106 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) / (yyvsp[(3) - (3)].uval); }
    break;

  case 210:
#line 1107 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) >> (yyvsp[(3) - (3)].uval); }
    break;

  case 211:
#line 1108 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) << (yyvsp[(3) - (3)].uval); }
    break;

  case 212:
#line 1109 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) | (yyvsp[(3) - (3)].uval); }
    break;

  case 213:
#line 1110 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) ^ (yyvsp[(3) - (3)].uval); }
    break;

  case 214:
#line 1111 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) & (yyvsp[(3) - (3)].uval); }
    break;

  case 215:
#line 1112 "parser.y"
    { (yyval.uval) = !(yyvsp[(2) - (2)].uval); }
    break;

  case 216:
#line 1113 "parser.y"
    { (yyval.uval) = -(yyvsp[(2) - (2)].uval); }
    break;

  case 217:
#line 1114 "parser.y"
    { (yyval.uval) = (yyvsp[(2) - (3)].uval); }
    break;

  case 218:
#line 1119 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (1)].uval); }
    break;

  case 219:
#line 1120 "parser.y"
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[(1) - (1)].sval)); free((yyvsp[(1) - (1)].sval));}
    break;

  case 220:
#line 1121 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 221:
#line 1122 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 222:
#line 1123 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 223:
#line 1124 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 224:
#line 1125 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 225:
#line 1126 "parser.y"
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 226:
#line 1127 "parser.y"
    { (yyval.uval) = bx_dbg_get_ip (); }
    break;

  case 227:
#line 1128 "parser.y"
    { (yyval.uval) = bx_dbg_get_eip(); }
    break;

  case 228:
#line 1129 "parser.y"
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
    break;

  case 229:
#line 1130 "parser.y"
    { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[(1) - (3)].uval), (yyvsp[(3) - (3)].uval)); }
    break;

  case 230:
#line 1131 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) + (yyvsp[(3) - (3)].uval); }
    break;

  case 231:
#line 1132 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) - (yyvsp[(3) - (3)].uval); }
    break;

  case 232:
#line 1133 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) * (yyvsp[(3) - (3)].uval); }
    break;

  case 233:
#line 1134 "parser.y"
    { (yyval.uval) = ((yyvsp[(3) - (3)].uval) != 0) ? (yyvsp[(1) - (3)].uval) / (yyvsp[(3) - (3)].uval) : 0; }
    break;

  case 234:
#line 1135 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) >> (yyvsp[(3) - (3)].uval); }
    break;

  case 235:
#line 1136 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) << (yyvsp[(3) - (3)].uval); }
    break;

  case 236:
#line 1137 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) | (yyvsp[(3) - (3)].uval); }
    break;

  case 237:
#line 1138 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) ^ (yyvsp[(3) - (3)].uval); }
    break;

  case 238:
#line 1139 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) & (yyvsp[(3) - (3)].uval); }
    break;

  case 239:
#line 1140 "parser.y"
    { (yyval.uval) = !(yyvsp[(2) - (2)].uval); }
    break;

  case 240:
#line 1141 "parser.y"
    { (yyval.uval) = -(yyvsp[(2) - (2)].uval); }
    break;

  case 241:
#line 1142 "parser.y"
    { (yyval.uval) = (yyvsp[(2) - (3)].uval); }
    break;


/* Line 1267 of yacc.c.  */
#line 3553 "y.tab.c"
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


#line 1145 "parser.y"


#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
