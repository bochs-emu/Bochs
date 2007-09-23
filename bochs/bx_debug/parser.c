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
#line 320 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 333 "y.tab.c"

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
#define YYFINAL  252
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1090

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  115
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  47
/* YYNRULES -- Number of rules.  */
#define YYNRULES  240
/* YYNRULES -- Number of states.  */
#define YYNSTATES  467

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   356

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
     343,   349,   355,   361,   365,   369,   373,   377,   381,   386,
     390,   394,   400,   408,   412,   416,   417,   419,   422,   425,
     429,   433,   437,   440,   445,   449,   453,   456,   462,   466,
     470,   475,   479,   482,   485,   489,   494,   498,   503,   509,
     513,   519,   523,   527,   531,   536,   540,   544,   548,   552,
     556,   560,   564,   568,   572,   576,   580,   584,   588,   592,
     596,   600,   604,   608,   612,   616,   620,   624,   628,   632,
     636,   640,   644,   648,   652,   656,   660,   664,   668,   672,
     676,   680,   684,   688,   691,   695,   697,   699,   701,   703,
     705,   707,   709,   711,   713,   715,   719,   723,   727,   731,
     735,   739,   743,   747,   751,   754,   757,   761,   763,   765,
     767,   769,   771,   773,   775,   777,   779,   781,   783,   787,
     791,   795,   799,   803,   807,   811,   815,   819,   823,   826,
     829
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     116,     0,    -1,   116,   117,    -1,   117,    -1,   134,    -1,
     135,    -1,   136,    -1,   137,    -1,   138,    -1,   141,    -1,
     143,    -1,   139,    -1,   140,    -1,   144,    -1,   145,    -1,
     146,    -1,   147,    -1,   148,    -1,   149,    -1,   150,    -1,
     151,    -1,   152,    -1,   153,    -1,   154,    -1,   155,    -1,
     156,    -1,   157,    -1,   127,    -1,   128,    -1,   126,    -1,
     120,    -1,   121,    -1,   122,    -1,   123,    -1,   129,    -1,
     130,    -1,   125,    -1,   124,    -1,   131,    -1,   132,    -1,
     133,    -1,   158,    -1,   159,    -1,    -1,   109,    -1,    15,
      -1,    16,    -1,     8,    -1,     9,    -1,    10,    -1,    11,
      -1,    12,    -1,    13,    -1,    70,    86,   109,    -1,    69,
      86,   109,    -1,    71,    60,   109,    -1,    72,    60,   109,
      -1,    73,   109,    -1,    79,    93,   109,    -1,    79,    60,
     109,    -1,    79,   109,    -1,    89,    86,   109,    -1,    68,
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
     161,   109,    -1,    21,     3,   110,   161,   109,    -1,    21,
       5,   110,   161,   109,    -1,    21,     6,   110,   161,   109,
      -1,    21,     7,   110,   161,   109,    -1,    24,   109,    -1,
      24,   160,   111,   160,   109,    -1,    25,   109,    -1,    25,
     161,   109,    -1,    25,    60,   109,    -1,    26,   109,    -1,
      26,   161,   109,    -1,    26,   104,   161,   109,    -1,    23,
     109,    -1,    82,   109,    -1,    82,    60,   109,    -1,    30,
      26,   109,    -1,    30,    33,   109,    -1,    30,    32,   109,
      -1,    30,    34,   109,    -1,    30,    35,   109,    -1,    30,
      36,   109,    -1,    30,    43,   109,    -1,    30,    37,   142,
     142,   109,    -1,    30,    38,   142,   142,   109,    -1,    30,
      39,   142,   142,   109,    -1,    30,    40,   142,   142,   109,
      -1,    30,    42,   109,    -1,    30,    41,   109,    -1,    30,
      14,   109,    -1,    30,    44,   109,    -1,    30,    81,   109,
      -1,    30,    81,    60,   109,    -1,    30,    45,   109,    -1,
      30,    87,   109,    -1,    30,    87,    89,    86,   109,    -1,
      30,    87,    89,    86,    32,    86,   109,    -1,    30,    88,
     109,    -1,    30,    92,   109,    -1,    -1,    86,    -1,    32,
     109,    -1,    56,   109,    -1,    27,    86,   109,    -1,    28,
      86,   109,    -1,    29,    86,   109,    -1,    31,   109,    -1,
      46,    47,   161,   109,    -1,    46,    47,   109,    -1,    46,
     161,   109,    -1,    46,   109,    -1,    49,    86,    86,    86,
     109,    -1,    51,    52,   109,    -1,    53,    54,   109,    -1,
      53,    54,    86,   109,    -1,    53,    55,   109,    -1,    57,
     109,    -1,    58,   109,    -1,    58,   161,   109,    -1,    58,
     161,   161,   109,    -1,    58,    48,   109,    -1,    58,    48,
     161,   109,    -1,    58,    48,   161,   161,   109,    -1,    58,
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
      90,    59,   109,    -1,    90,    21,   109,    -1,    90,    89,
     109,    -1,    90,    30,   109,    -1,    90,    79,   109,    -1,
      90,    91,   109,    -1,    90,    90,   109,    -1,    90,   109,
      -1,    91,   161,   109,    -1,    86,    -1,     4,    -1,     3,
      -1,     5,    -1,     6,    -1,     7,    -1,   119,    -1,    97,
      -1,    98,    -1,    99,    -1,   160,   100,   160,    -1,   160,
     101,   160,    -1,   160,   104,   160,    -1,   160,   105,   160,
      -1,   160,    95,   160,    -1,   160,    96,   160,    -1,   160,
     102,   160,    -1,   160,   103,   160,    -1,   160,   106,   160,
      -1,   112,   160,    -1,   101,   160,    -1,   113,   160,   114,
      -1,    86,    -1,    60,    -1,     4,    -1,     3,    -1,     5,
      -1,     6,    -1,     7,    -1,   119,    -1,    97,    -1,    98,
      -1,    99,    -1,   161,   111,   161,    -1,   161,   100,   161,
      -1,   161,   101,   161,    -1,   161,   104,   161,    -1,   161,
     105,   161,    -1,   161,    95,   161,    -1,   161,    96,   161,
      -1,   161,   102,   161,    -1,   161,   103,   161,    -1,   161,
     106,   161,    -1,   112,   161,    -1,   101,   161,    -1,   113,
     161,   114,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   133,   133,   134,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   184,   185,   190,   191,   192,
     193,   194,   195,   200,   205,   213,   221,   229,   237,   242,
     247,   255,   263,   271,   279,   287,   292,   300,   306,   312,
     317,   322,   327,   332,   337,   345,   350,   355,   363,   371,
     379,   387,   392,   400,   408,   413,   418,   422,   426,   430,
     434,   441,   446,   451,   456,   461,   466,   471,   476,   484,
     492,   497,   505,   510,   515,   520,   525,   530,   535,   540,
     545,   550,   555,   560,   565,   570,   575,   580,   585,   590,
     595,   600,   605,   610,   615,   623,   624,   627,   635,   643,
     651,   658,   666,   674,   679,   684,   689,   697,   705,   713,
     718,   723,   731,   739,   744,   749,   754,   759,   764,   769,
     774,   782,   787,   795,   803,   811,   816,   821,   826,   831,
     836,   841,   846,   851,   856,   861,   866,   871,   876,   882,
     888,   893,   898,   903,   908,   913,   918,   923,   928,   933,
     938,   943,   948,   957,   966,   973,   986,   994,  1004,  1009,
    1030,  1042,  1052,  1057,  1065,  1083,  1084,  1085,  1086,  1087,
    1088,  1089,  1090,  1091,  1092,  1093,  1094,  1095,  1096,  1097,
    1098,  1099,  1100,  1101,  1102,  1103,  1104,  1109,  1110,  1111,
    1112,  1113,  1114,  1115,  1116,  1117,  1118,  1119,  1120,  1121,
    1122,  1123,  1124,  1125,  1126,  1127,  1128,  1129,  1130,  1131,
    1132
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
  "page_command", "ptime_command", "trace_command", "trace_reg_command",
  "print_stack_command", "watch_point_command", "symbol_command",
  "where_command", "print_string_command", "continue_command",
  "stepN_command", "step_over_command", "set_command",
  "breakpoint_command", "blist_command", "slist_command", "info_command",
  "optional_numeric", "regs_command", "dump_cpu_command", "delete_command",
  "bpe_command", "bpd_command", "quit_command", "examine_command",
  "setpmem_command", "query_command", "take_command", "set_cpu_command",
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
      43,    45,   124,    94,    42,    47,    38,   355,   356,    10,
      61,    58,    33,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   115,   116,   116,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   118,   118,   119,   119,   119,
     119,   119,   119,   120,   120,   121,   122,   123,   124,   124,
     124,   125,   126,   127,   128,   129,   129,   130,   130,   130,
     130,   130,   130,   130,   130,   131,   131,   131,   132,   133,
     134,   135,   135,   136,   137,   137,   137,   137,   137,   137,
     137,   138,   138,   138,   138,   138,   138,   138,   138,   139,
     140,   140,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   142,   142,   143,   144,   145,
     146,   147,   148,   149,   149,   149,   149,   150,   151,   152,
     152,   152,   153,   154,   154,   154,   154,   154,   154,   154,
     154,   155,   155,   156,   157,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   159,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161
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
       2,     3,     3,     3,     3,     3,     3,     3,     3,     5,
       5,     5,     5,     3,     3,     3,     3,     3,     4,     3,
       3,     5,     7,     3,     3,     0,     1,     2,     2,     3,
       3,     3,     2,     4,     3,     3,     2,     5,     3,     3,
       4,     3,     2,     2,     3,     4,     3,     4,     5,     3,
       5,     3,     3,     3,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       3
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
       0,     0,     0,     0,     0,     0,     0,    99,   197,   196,
     198,   199,   200,    47,    48,    49,    50,    51,    52,   195,
     202,   203,   204,     0,    91,     0,     0,   201,     0,   220,
     219,   221,   222,   223,   218,   217,   225,   226,   227,     0,
      93,     0,     0,   224,     0,   218,     0,    96,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   125,
     125,   125,   125,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   132,   127,     0,   136,     0,     0,     0,     0,
       0,   128,   142,     0,     0,     0,   143,     0,     0,     0,
       0,     0,    45,    46,     0,     0,    62,     0,     0,     0,
       0,    57,     0,    65,     0,     0,     0,     0,    69,     0,
       0,    70,     0,     0,    60,     0,     0,     0,   100,    78,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     193,     0,     1,     2,    82,     0,     0,     0,     0,     0,
       0,     0,   215,   214,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    95,   239,   238,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    94,     0,
       0,    97,   129,   130,   131,   115,   102,   104,   103,   105,
     106,   107,   126,   125,   125,   125,   125,   114,   113,   108,
     116,   119,     0,   117,     0,   120,   123,   124,   134,     0,
     135,     0,   138,     0,   139,   141,   146,     0,   149,     0,
       0,   144,     0,   151,   152,   153,     0,    63,    64,    54,
      53,    55,    56,    66,    68,    67,     0,     0,     0,     0,
      59,    58,     0,    75,     0,   101,    79,    61,   156,   157,
     158,   187,   165,   159,   160,   161,   162,   163,   164,   189,
     155,   180,   185,   181,   170,   171,   182,   186,   167,   168,
     169,   172,   174,   173,   176,   177,   166,   175,   183,   184,
     190,   178,   179,   188,   192,   191,   194,     0,     0,     0,
       0,     0,     0,    84,   216,   209,   210,   205,   206,   211,
     212,   207,   208,   213,     0,   240,   233,   234,   229,   230,
     235,   236,   231,   232,   237,   228,    98,     0,     0,     0,
       0,   118,     0,   133,     0,   140,   147,     0,     0,   230,
     145,   154,    71,    73,    72,    74,    76,     0,    87,    86,
      88,    89,    90,    85,    92,   109,   110,   111,   112,     0,
     121,   137,   148,   150,    77,     0,   122
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    45,    46,   184,   133,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,   303,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,   118,   134
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -147
static const yytype_int16 yypact[] =
{
     738,   -84,   -60,   -78,     5,   -73,   505,   360,   297,   -27,
     -26,   -24,   891,   -43,   -42,   343,   -17,    18,   -22,   -38,
     -35,   232,   -37,   -14,     1,    42,    42,   -21,     4,     6,
      33,    35,   -13,   -59,     0,   -55,   -45,   -32,   -44,     3,
      27,    28,   813,   530,  -147,   639,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,     9,  -147,  -147,
      12,    31,    32,    38,    39,    40,    42,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,   641,  -147,   641,   641,  -147,   332,  -147,
    -147,  -147,  -147,  -147,    10,  -147,  -147,  -147,  -147,   530,
    -147,   530,   530,  -147,   -20,  -147,   530,  -147,   449,    46,
      55,    58,    72,    73,    74,    75,    76,    77,    78,    29,
      29,    29,    29,    79,    92,    93,    95,   101,   -41,   -75,
     102,   103,  -147,  -147,   471,  -147,   578,    65,   111,   -57,
     112,  -147,  -147,   488,   114,    56,  -147,    34,   116,   117,
     119,   136,  -147,  -147,   120,   121,  -147,   122,   123,   124,
     125,  -147,   137,  -147,   143,   144,   168,   169,  -147,   170,
     171,  -147,   149,   150,  -147,   -56,    84,   161,  -147,  -147,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     184,   185,   186,   187,   188,   202,   203,   204,   205,   206,
     211,   212,   214,   215,   219,   225,   226,   227,   228,   229,
     230,   231,   233,   234,   249,   250,   251,   252,   253,   265,
    -147,   677,  -147,  -147,  -147,   530,   530,   530,   530,   530,
     289,   267,  -147,  -147,   312,   641,   641,   641,   641,   641,
     641,   641,   641,   641,   641,  -147,   266,   266,   113,   530,
     530,   530,   530,   530,   530,   530,   530,   530,  -147,   530,
     842,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,    29,    29,    29,    29,  -147,  -147,  -147,
    -147,  -147,   269,  -147,   293,  -147,  -147,  -147,  -147,   854,
    -147,   294,  -147,   272,  -147,  -147,  -147,    94,  -147,   296,
     530,  -147,   885,  -147,  -147,  -147,   275,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,   276,   277,   278,   279,
    -147,  -147,   280,  -147,   305,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,   897,   909,   921,
     933,   945,   283,  -147,  -147,  -147,  -147,    21,    21,    21,
      21,  -147,  -147,  -147,   981,  -147,   266,   266,    57,    57,
      57,    57,   266,   266,   266,   969,  -147,   284,   288,   290,
     291,  -147,   -30,  -147,   295,  -147,  -147,   957,   302,   221,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,   310,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,   316,
    -147,  -147,  -147,  -147,  -147,   313,  -147
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -147,  -147,   376,   -23,    -5,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,  -146,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,   -95,    -8
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -240
static const yytype_int16 yytable[] =
{
     138,   117,   459,   185,   304,   305,   306,   166,    90,    91,
      92,    93,    94,   177,   314,   202,   207,   194,   262,   312,
     263,   264,   199,   200,   178,    86,    87,   192,   205,   323,
     352,    89,   169,   170,   315,   251,    97,   119,   120,   121,
     122,   123,   103,   104,   105,   106,   107,   108,   203,    88,
     193,   206,   324,   353,   201,    95,   179,   182,   183,   139,
     140,   195,   141,    96,   204,   208,   162,   163,   313,   167,
     168,   171,   180,   261,   172,   279,   280,   196,   197,   460,
     281,   282,   283,   284,   285,   286,   287,   181,   186,   288,
     187,   289,   188,   189,   135,   190,   191,   119,   120,   121,
     122,   123,   103,   104,   105,   106,   107,   108,   117,   198,
     117,   117,   209,   210,   211,   302,   265,   266,   254,   275,
     125,   276,   255,   277,   278,   271,   272,   273,   290,   279,
     280,   126,   127,   128,   281,   330,   283,   284,   285,   286,
     287,   256,   257,   331,   354,   289,   131,   132,   258,   259,
     260,   321,   279,   280,   135,   292,   319,   427,   428,   429,
     430,   285,   286,   287,   293,   327,   329,   294,   289,   332,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     125,   295,   296,   297,   298,   299,   300,   301,   307,   279,
     280,   126,   127,   128,   281,   330,   283,   284,   285,   286,
     287,   308,   309,   436,   310,   289,   131,   132,   279,   280,
     311,   316,   317,   281,   282,   283,   284,   285,   286,   287,
     322,   325,   336,   328,   289,   333,   334,   415,   335,   337,
     338,   339,   340,   341,   342,   119,   120,   121,   122,   123,
     103,   104,   105,   106,   107,   108,   343,   397,   398,   399,
     400,   401,   344,   345,   346,   347,   348,   349,   350,   351,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     355,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     173,   425,   356,   357,   358,   359,   360,   361,   362,   363,
     364,   365,   135,   366,   367,   368,   369,   370,   174,   175,
     119,   120,   121,   122,   123,   103,   104,   105,   106,   107,
     108,   371,   372,   373,   374,   375,  -239,  -239,   125,   437,
     376,   377,   439,   378,   379,  -239,  -239,  -239,   380,   126,
     127,   128,   289,   129,   381,   382,   383,   384,   385,   386,
     387,   176,   388,   389,   131,   132,   119,   120,   121,   122,
     123,   103,   104,   105,   106,   107,   108,   135,   390,   391,
     392,   393,   394,   119,   120,   121,   122,   123,   103,   104,
     105,   106,   107,   108,   395,   402,   403,   289,   431,   432,
     434,   435,   438,   125,   441,   442,   443,   444,   445,   446,
     164,   447,   453,   455,   126,   127,   128,   456,   129,   457,
     458,   136,   465,   135,   461,     0,   137,   265,   266,   131,
     132,   463,   267,   268,   269,   270,   271,   272,   273,   464,
     124,   253,   466,     0,     0,     0,   404,   265,   266,   125,
       0,     0,   267,   268,   269,   270,   271,   272,   273,     0,
     126,   127,   128,   274,   129,     0,   125,     0,     0,     0,
       0,     0,   165,     0,     0,   131,   132,   126,   127,   128,
       0,   129,     0,     0,     0,     0,     0,     0,     0,   130,
       0,     0,   131,   132,   119,   120,   121,   122,   123,   103,
     104,   105,   106,   107,   108,     0,     0,     0,     0,     0,
       0,   119,   120,   121,   122,   123,   103,   104,   105,   106,
     107,   108,     0,     0,     0,     0,     0,     0,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   135,     0,   119,   120,   121,   122,   123,   103,   104,
     105,   106,   107,   108,   279,   280,     0,     0,   135,   281,
     282,   283,   284,   285,   286,   287,     0,   125,   291,     0,
     289,     0,     0,     0,     0,     0,     0,     0,   126,   127,
     128,     0,   129,     0,   125,     0,     0,     0,     0,     0,
     318,     0,     0,   131,   132,   126,   127,   128,     0,   129,
     135,   109,     0,     0,     0,     0,     0,   326,     0,     0,
     131,   132,   110,   111,   112,     0,   113,     0,     0,     0,
       0,     0,     0,     0,   114,     0,   125,   115,   116,     0,
       0,     0,     0,     0,     0,     0,     0,   126,   127,   128,
       0,   129,     0,     0,     0,     0,     0,     0,     0,   252,
       0,     0,   131,   132,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,     0,     1,     2,     3,     0,
       4,     0,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,     0,   279,   280,     0,     0,     0,   281,   282,
     283,   284,   285,   286,   287,    15,     0,   320,    16,   289,
      17,     0,    18,     0,     0,    19,    20,    21,    22,     0,
       0,    23,    24,    25,    26,     0,     0,    27,    28,    29,
      30,    31,    32,    33,    34,    35,     0,     0,    36,    37,
       0,    38,     0,    39,    40,     0,     0,   109,    41,    42,
      43,     0,     0,     0,     0,     0,     0,     0,   110,   111,
     112,     0,   113,     0,     0,     0,     0,     0,    44,     0,
       0,     0,     0,   115,   116,     1,     2,     3,     0,     4,
       0,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,     0,   279,   280,     0,     0,     0,   281,   282,   283,
     284,   285,   286,   287,    15,     0,   396,    16,   289,    17,
       0,    18,     0,     0,    19,    20,    21,    22,     0,     0,
      23,    24,    25,    26,     0,     0,    27,    28,    29,    30,
      31,    32,    33,    34,    35,     0,     0,    36,    37,     0,
      38,     0,    39,    40,     0,     0,     0,    41,    42,    43,
     212,   213,   214,     0,   215,     0,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,     0,    44,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   226,
       0,     0,   227,     0,     0,     0,     0,     0,     0,   228,
     229,   230,   231,     0,     0,     0,   232,   233,   234,     0,
       0,   235,   236,   237,   238,   239,   240,   241,   242,   243,
       0,     0,   244,   245,     0,   246,     0,     0,     0,     0,
       0,     0,   247,   248,   249,   142,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   143,     0,     0,
       0,     0,   250,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   279,   280,     0,
       0,     0,   281,   282,   283,   284,   285,   286,   287,   279,
     280,   426,     0,   289,   281,   282,   283,   284,   285,   286,
     287,     0,     0,   433,     0,   289,     0,     0,     0,     0,
       0,     0,   158,     0,     0,     0,     0,     0,   159,   160,
     279,   280,     0,   161,     0,   281,   282,   283,   284,   285,
     286,   287,   279,   280,   440,     0,   289,   281,   282,   283,
     284,   285,   286,   287,   279,   280,   448,     0,   289,   281,
     282,   283,   284,   285,   286,   287,   279,   280,   449,     0,
     289,   281,   282,   283,   284,   285,   286,   287,   279,   280,
     450,     0,   289,   281,   282,   283,   284,   285,   286,   287,
     279,   280,   451,     0,   289,   281,   282,   283,   284,   285,
     286,   287,   279,   280,   452,     0,   289,   281,   282,   283,
     284,   285,   286,   287,   279,   280,   462,     0,   289,   281,
     282,   283,   284,   285,   286,   287,   265,   266,     0,     0,
     289,   267,   268,   269,   270,   271,   272,   273,     0,     0,
     454
};

static const yytype_int16 yycheck[] =
{
       8,     6,    32,    26,   150,   151,   152,    15,     3,     4,
       5,     6,     7,    21,    89,    60,    60,    17,   113,    60,
     115,   116,    77,    78,    61,   109,    86,    86,    60,    86,
      86,   109,    54,    55,   109,    43,   109,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    93,   109,
     109,    83,   109,   109,   109,    50,    93,    15,    16,    86,
      86,    61,    86,    58,   109,   109,   109,   109,   109,    86,
      52,   109,    86,    96,   109,    95,    96,    77,    78,   109,
     100,   101,   102,   103,   104,   105,   106,    86,   109,   109,
      86,   111,    86,    60,    60,    60,   109,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,   113,   109,
     115,   116,   109,    86,    86,    86,    95,    96,   109,   109,
      86,   129,   110,   131,   132,   104,   105,   106,   136,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   110,   110,   109,    60,   111,   112,   113,   110,   110,
     110,    86,    95,    96,    60,   109,   164,   303,   304,   305,
     306,   104,   105,   106,   109,   173,   110,   109,   111,   177,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      86,   109,   109,   109,   109,   109,   109,   109,   109,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   109,   109,   109,   109,   111,   112,   113,    95,    96,
     109,   109,   109,   100,   101,   102,   103,   104,   105,   106,
     109,   109,    86,   109,   111,   109,   109,   114,   109,   109,
     109,   109,   109,   109,   109,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,   109,   255,   256,   257,
     258,   259,   109,   109,    86,    86,    86,    86,   109,   109,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     109,   279,   280,   281,   282,   283,   284,   285,   286,   287,
      48,   289,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,    60,   109,   109,   109,   109,   109,    66,    67,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,   109,   109,   109,   109,   109,    95,    96,    86,   327,
     109,   109,   330,   109,   109,   104,   105,   106,   109,    97,
      98,    99,   111,   101,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   112,   113,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    60,   109,   109,
     109,   109,   109,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,   109,    86,   109,   111,   109,    86,
      86,   109,    86,    86,   109,   109,   109,   109,   109,   109,
      47,    86,   109,   109,    97,    98,    99,   109,   101,   109,
     109,   104,    86,    60,   109,    -1,   109,    95,    96,   112,
     113,   109,   100,   101,   102,   103,   104,   105,   106,   109,
      60,    45,   109,    -1,    -1,    -1,   114,    95,    96,    86,
      -1,    -1,   100,   101,   102,   103,   104,   105,   106,    -1,
      97,    98,    99,   111,   101,    -1,    86,    -1,    -1,    -1,
      -1,    -1,   109,    -1,    -1,   112,   113,    97,    98,    99,
      -1,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,
      -1,    -1,   112,   113,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    60,    -1,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    95,    96,    -1,    -1,    60,   100,
     101,   102,   103,   104,   105,   106,    -1,    86,   109,    -1,
     111,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,    98,
      99,    -1,   101,    -1,    86,    -1,    -1,    -1,    -1,    -1,
     109,    -1,    -1,   112,   113,    97,    98,    99,    -1,   101,
      60,    86,    -1,    -1,    -1,    -1,    -1,   109,    -1,    -1,
     112,   113,    97,    98,    99,    -1,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   109,    -1,    86,   112,   113,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,    98,    99,
      -1,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,
      -1,    -1,   112,   113,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    -1,    17,    18,    19,    -1,
      21,    -1,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    95,    96,    -1,    -1,    -1,   100,   101,
     102,   103,   104,   105,   106,    46,    -1,   109,    49,   111,
      51,    -1,    53,    -1,    -1,    56,    57,    58,    59,    -1,
      -1,    62,    63,    64,    65,    -1,    -1,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    -1,    -1,    79,    80,
      -1,    82,    -1,    84,    85,    -1,    -1,    86,    89,    90,
      91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,    98,
      99,    -1,   101,    -1,    -1,    -1,    -1,    -1,   109,    -1,
      -1,    -1,    -1,   112,   113,    17,    18,    19,    -1,    21,
      -1,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    95,    96,    -1,    -1,    -1,   100,   101,   102,
     103,   104,   105,   106,    46,    -1,   109,    49,   111,    51,
      -1,    53,    -1,    -1,    56,    57,    58,    59,    -1,    -1,
      62,    63,    64,    65,    -1,    -1,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    -1,    -1,    79,    80,    -1,
      82,    -1,    84,    85,    -1,    -1,    -1,    89,    90,    91,
      17,    18,    19,    -1,    21,    -1,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    -1,   109,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    49,    -1,    -1,    -1,    -1,    -1,    -1,    56,
      57,    58,    59,    -1,    -1,    -1,    63,    64,    65,    -1,
      -1,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      -1,    -1,    79,    80,    -1,    82,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    90,    91,    14,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,    -1,    -1,
      -1,    -1,   109,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    95,    96,    -1,
      -1,    -1,   100,   101,   102,   103,   104,   105,   106,    95,
      96,   109,    -1,   111,   100,   101,   102,   103,   104,   105,
     106,    -1,    -1,   109,    -1,   111,    -1,    -1,    -1,    -1,
      -1,    -1,    81,    -1,    -1,    -1,    -1,    -1,    87,    88,
      95,    96,    -1,    92,    -1,   100,   101,   102,   103,   104,
     105,   106,    95,    96,   109,    -1,   111,   100,   101,   102,
     103,   104,   105,   106,    95,    96,   109,    -1,   111,   100,
     101,   102,   103,   104,   105,   106,    95,    96,   109,    -1,
     111,   100,   101,   102,   103,   104,   105,   106,    95,    96,
     109,    -1,   111,   100,   101,   102,   103,   104,   105,   106,
      95,    96,   109,    -1,   111,   100,   101,   102,   103,   104,
     105,   106,    95,    96,   109,    -1,   111,   100,   101,   102,
     103,   104,   105,   106,    95,    96,   109,    -1,   111,   100,
     101,   102,   103,   104,   105,   106,    95,    96,    -1,    -1,
     111,   100,   101,   102,   103,   104,   105,   106,    -1,    -1,
     109
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    17,    18,    19,    21,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    46,    49,    51,    53,    56,
      57,    58,    59,    62,    63,    64,    65,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    79,    80,    82,    84,
      85,    89,    90,    91,   109,   116,   117,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   109,    86,   109,   109,
       3,     4,     5,     6,     7,    50,    58,   109,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    86,
      97,    98,    99,   101,   109,   112,   113,   119,   160,     3,
       4,     5,     6,     7,    60,    86,    97,    98,    99,   101,
     109,   112,   113,   119,   161,    60,   104,   109,   161,    86,
      86,    86,    14,    26,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    81,    87,
      88,    92,   109,   109,    47,   109,   161,    86,    52,    54,
      55,   109,   109,    48,    66,    67,   109,   161,    61,    93,
      86,    86,    15,    16,   118,   118,   109,    86,    86,    60,
      60,   109,    86,   109,    17,    61,    77,    78,   109,    77,
      78,   109,    60,    93,   109,    60,    83,    60,   109,   109,
      86,    86,    17,    18,    19,    21,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    46,    49,    56,    57,
      58,    59,    63,    64,    65,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    79,    80,    82,    89,    90,    91,
     109,   161,     0,   117,   109,   110,   110,   110,   110,   110,
     110,   118,   160,   160,   160,    95,    96,   100,   101,   102,
     103,   104,   105,   106,   111,   109,   161,   161,   161,    95,
      96,   100,   101,   102,   103,   104,   105,   106,   109,   111,
     161,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,    86,   142,   142,   142,   142,   109,   109,   109,
     109,   109,    60,   109,    89,   109,   109,   109,   109,   161,
     109,    86,   109,    86,   109,   109,   109,   161,   109,   110,
     101,   109,   161,   109,   109,   109,    86,   109,   109,   109,
     109,   109,   109,   109,   109,   109,    86,    86,    86,    86,
     109,   109,    86,   109,    60,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   161,   161,   161,
     161,   161,    86,   109,   114,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   114,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   109,   142,   142,   142,
     142,   109,    86,   109,    86,   109,   109,   161,    86,   161,
     109,   109,   109,   109,   109,   109,   109,    86,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,    32,
     109,   109,   109,   109,   109,    86,   109
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
#line 179 "parser.y"
    {
      }
    break;

  case 46:
#line 186 "parser.y"
    { (yyval.bval)=(yyvsp[(1) - (1)].bval); }
    break;

  case 52:
#line 196 "parser.y"
    { (yyval.uval)=(yyvsp[(1) - (1)].uval); }
    break;

  case 53:
#line 201 "parser.y"
    {
          bx_dbg_timebp_command(0, (yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 54:
#line 206 "parser.y"
    {
          bx_dbg_timebp_command(1, (yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 55:
#line 214 "parser.y"
    {
          bx_dbg_record_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 56:
#line 222 "parser.y"
    {
          bx_dbg_playback_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 57:
#line 230 "parser.y"
    {
          bx_dbg_modebp_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 58:
#line 238 "parser.y"
    {
          bx_dbg_show_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 59:
#line 243 "parser.y"
    {
          bx_dbg_show_param_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 60:
#line 248 "parser.y"
    {
          bx_dbg_show_command(0);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 61:
#line 256 "parser.y"
    {
          bx_dbg_xlate_address((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 62:
#line 264 "parser.y"
    {
          bx_dbg_ptime_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 63:
#line 272 "parser.y"
    {
          bx_dbg_trace_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 64:
#line 280 "parser.y"
    {
          bx_dbg_trace_reg_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 65:
#line 288 "parser.y"
    {
          bx_dbg_print_stack_command(16);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 66:
#line 293 "parser.y"
    {
          bx_dbg_print_stack_command((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 67:
#line 301 "parser.y"
    {
          watchpoint_continue = 0;
          dbg_printf("Will stop on watch points\n");
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 68:
#line 307 "parser.y"
    {
          watchpoint_continue = 1;
          dbg_printf("Will not stop on watch points (they will still be logged)\n");
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 69:
#line 313 "parser.y"
    {
          bx_dbg_watch(-1, 0);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 70:
#line 318 "parser.y"
    {
          bx_dbg_unwatch(-1, 0);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 71:
#line 323 "parser.y"
    {
          bx_dbg_watch(1, (yyvsp[(3) - (4)].uval));
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 72:
#line 328 "parser.y"
    {
          bx_dbg_unwatch(1, (yyvsp[(3) - (4)].uval));
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 73:
#line 333 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (4)].uval));
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 74:
#line 338 "parser.y"
    {
          bx_dbg_unwatch(0, (yyvsp[(3) - (4)].uval));
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 75:
#line 346 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(2) - (3)].sval), 0, 0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 76:
#line 351 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(2) - (4)].sval), 0, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 77:
#line 356 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(3) - (5)].sval), 1, (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval)); free((yyvsp[(3) - (5)].sval));
      }
    break;

  case 78:
#line 364 "parser.y"
    {
        bx_dbg_where_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 79:
#line 372 "parser.y"
    {
        bx_dbg_print_string_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 80:
#line 380 "parser.y"
    {
        bx_dbg_continue_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 81:
#line 388 "parser.y"
    {
        bx_dbg_stepN_command(1);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 82:
#line 393 "parser.y"
    {
        bx_dbg_stepN_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 83:
#line 401 "parser.y"
    {
        bx_dbg_step_over_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 84:
#line 409 "parser.y"
    {
        bx_dbg_set_auto_disassemble((yyvsp[(3) - (4)].bval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 85:
#line 414 "parser.y"
    {
        bx_dbg_set_symbol_command((yyvsp[(2) - (5)].sval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 86:
#line 419 "parser.y"
    { 
        bx_dbg_set_reg8l_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 87:
#line 423 "parser.y"
    { 
        bx_dbg_set_reg8h_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 88:
#line 427 "parser.y"
    { 
        bx_dbg_set_reg16_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 89:
#line 431 "parser.y"
    { 
        bx_dbg_set_reg32_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 90:
#line 435 "parser.y"
    { 
        bx_dbg_set_reg64_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 91:
#line 442 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 92:
#line 447 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval));
      }
    break;

  case 93:
#line 452 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 94:
#line 457 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 95:
#line 462 "parser.y"
    {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 96:
#line 467 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 97:
#line 472 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 98:
#line 477 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 99:
#line 485 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 100:
#line 493 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 101:
#line 498 "parser.y"
    {
        bx_dbg_info_symbols_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 102:
#line 506 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 103:
#line 511 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 104:
#line 516 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 105:
#line 521 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 106:
#line 526 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 107:
#line 531 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS | BX_INFO_FPU_REGS | BX_INFO_SSE_REGS);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 108:
#line 536 "parser.y"
    {
        bx_dbg_info_dirty_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 109:
#line 541 "parser.y"
    {
        bx_dbg_info_idt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 110:
#line 546 "parser.y"
    {
        bx_dbg_info_ivt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 111:
#line 551 "parser.y"
    {
        bx_dbg_info_gdt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 112:
#line 556 "parser.y"
    {
        bx_dbg_info_ldt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 113:
#line 561 "parser.y"
    {
        bx_dbg_dump_table();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 114:
#line 566 "parser.y"
    {
        bx_dbg_info_tss_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 115:
#line 571 "parser.y"
    {
        bx_dbg_info_flags();
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 116:
#line 576 "parser.y"
    {
        bx_dbg_info_linux_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 117:
#line 581 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 118:
#line 586 "parser.y"
    {
        bx_dbg_info_symbols_command((yyvsp[(3) - (4)].sval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 119:
#line 591 "parser.y"
    {
        bx_dbg_info_control_regs_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 120:
#line 596 "parser.y"
    {
        bx_dbg_info_ne2k(-1, -1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 121:
#line 601 "parser.y"
    {
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval)); free((yyvsp[(3) - (5)].sval));
        bx_dbg_info_ne2k((yyvsp[(4) - (5)].uval), -1);
      }
    break;

  case 122:
#line 606 "parser.y"
    {
        free((yyvsp[(1) - (7)].sval)); free((yyvsp[(2) - (7)].sval)); free((yyvsp[(3) - (7)].sval)); free((yyvsp[(5) - (7)].sval));
        bx_dbg_info_ne2k((yyvsp[(4) - (7)].uval), (yyvsp[(6) - (7)].uval));
      }
    break;

  case 123:
#line 611 "parser.y"
    {
        bx_dbg_info_pic();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 124:
#line 616 "parser.y"
    {
        bx_dbg_info_vga();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 125:
#line 623 "parser.y"
    { (yyval.uval) = EMPTY_ARG; }
    break;

  case 127:
#line 628 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 128:
#line 636 "parser.y"
    {
        bx_dbg_dump_cpu_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 129:
#line 644 "parser.y"
    {
        bx_dbg_del_breakpoint_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 130:
#line 652 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[(2) - (3)].uval), 1);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 131:
#line 659 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[(2) - (3)].uval), 0);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 132:
#line 667 "parser.y"
    {
        bx_dbg_quit_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 133:
#line 675 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (4)].sval), (yyvsp[(2) - (4)].sval),1, (yyvsp[(3) - (4)].uval), 1);
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 134:
#line 680 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (3)].sval), (yyvsp[(2) - (3)].sval),1, 0, 0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 135:
#line 685 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (3)].sval), NULL,0, (yyvsp[(2) - (3)].uval), 1);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 136:
#line 690 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (2)].sval), NULL,0, 0, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 137:
#line 698 "parser.y"
    {
        bx_dbg_setpmem_command((yyvsp[(2) - (5)].uval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval));
      }
    break;

  case 138:
#line 706 "parser.y"
    {
        bx_dbg_query_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 139:
#line 714 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (3)].sval), 1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 140:
#line 719 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 141:
#line 724 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (3)].sval), 1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 142:
#line 732 "parser.y"
    {
        bx_dbg_set_cpu_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 143:
#line 740 "parser.y"
    {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 144:
#line 745 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[(2) - (3)].uval), (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 145:
#line 750 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 146:
#line 755 "parser.y"
    {
        bx_dbg_disassemble_current((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 147:
#line 760 "parser.y"
    {
        bx_dbg_disassemble_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 148:
#line 765 "parser.y"
    {
        bx_dbg_disassemble_command((yyvsp[(2) - (5)].sval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 149:
#line 770 "parser.y"
    {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 150:
#line 775 "parser.y"
    {
        bx_dbg_set_disassemble_size((yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 151:
#line 783 "parser.y"
    {
        bx_dbg_instrument_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 152:
#line 788 "parser.y"
    {
        bx_dbg_instrument_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 153:
#line 796 "parser.y"
    {
        bx_dbg_doit_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 154:
#line 804 "parser.y"
    {
        bx_dbg_crc_command((yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 155:
#line 812 "parser.y"
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 156:
#line 817 "parser.y"
    {
         dbg_printf("c|cont|continue - continue executing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 157:
#line 822 "parser.y"
    {
         dbg_printf("s|step|stepi [count] - execute #count instructions (default is one instruction)\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 158:
#line 827 "parser.y"
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 159:
#line 832 "parser.y"
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 160:
#line 837 "parser.y"
    {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 161:
#line 842 "parser.y"
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 162:
#line 847 "parser.y"
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 163:
#line 852 "parser.y"
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 164:
#line 857 "parser.y"
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 165:
#line 862 "parser.y"
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 166:
#line 867 "parser.y"
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 167:
#line 872 "parser.y"
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 168:
#line 877 "parser.y"
    {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 169:
#line 883 "parser.y"
    {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 170:
#line 889 "parser.y"
    {
         dbg_printf("dump_cpu - dump complete cpu state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 171:
#line 894 "parser.y"
    {
         dbg_printf("set_cpu - set complete cpu state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 172:
#line 899 "parser.y"
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 173:
#line 904 "parser.y"
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 174:
#line 909 "parser.y"
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 175:
#line 914 "parser.y"
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 176:
#line 919 "parser.y"
    {
         dbg_printf("record <filename> - record console input to file filename\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 177:
#line 924 "parser.y"
    {
         dbg_printf("playback <filename> - playback console input from file filename\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 178:
#line 929 "parser.y"
    {
         dbg_printf("load-symbols [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 179:
#line 934 "parser.y"
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 180:
#line 939 "parser.y"
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 181:
#line 944 "parser.y"
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 182:
#line 949 "parser.y"
    {
         dbg_printf("u|disasm|disassemble [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm|disassemble switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm|disassemble size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 183:
#line 958 "parser.y"
    {
         dbg_printf("watch - print current watch point status\n");
         dbg_printf("watch stop - stop simulation when a watchpoint is encountred\n");
         dbg_printf("watch continue - do not stop the simulation when watch point is encountred\n");
         dbg_printf("watch read addr - insert a read watch point at physical address addr\n");
         dbg_printf("watch write addr - insert a write watch point at physical address addr\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 184:
#line 967 "parser.y"
    {
         dbg_printf("unwatch - remove all watch points\n");
         dbg_printf("unwatch read addr - remove a read watch point at physical address addr\n");
         dbg_printf("unwatch write addr - remove a write watch point at physical address addr\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 185:
#line 974 "parser.y"
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

  case 186:
#line 987 "parser.y"
    {
         dbg_printf("instrument start - calls bx_instr_start() callback\n");
         dbg_printf("instrument stop  - calls bx_instr_stop () callback\n");
         dbg_printf("instrument reset - calls bx_instr_reset() callback\n");
         dbg_printf("instrument print - calls bx_instr_print() callback\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 187:
#line 995 "parser.y"
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

  case 188:
#line 1005 "parser.y"
    {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 189:
#line 1010 "parser.y"
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
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 190:
#line 1031 "parser.y"
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

  case 191:
#line 1043 "parser.y"
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

  case 192:
#line 1053 "parser.y"
    {
         bx_dbg_print_help();
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 193:
#line 1058 "parser.y"
    {
         bx_dbg_print_help();
         free((yyvsp[(1) - (2)].sval));
       }
    break;

  case 194:
#line 1066 "parser.y"
    {
     bx_dbg_calc_command((yyvsp[(2) - (3)].uval));
     free((yyvsp[(1) - (3)].sval));
   }
    break;

  case 195:
#line 1083 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (1)].uval); }
    break;

  case 196:
#line 1084 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 197:
#line 1085 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 198:
#line 1086 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 199:
#line 1087 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 200:
#line 1088 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 201:
#line 1089 "parser.y"
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 202:
#line 1090 "parser.y"
    { (yyval.uval) = bx_dbg_get_ip (); }
    break;

  case 203:
#line 1091 "parser.y"
    { (yyval.uval) = bx_dbg_get_eip(); }
    break;

  case 204:
#line 1092 "parser.y"
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
    break;

  case 205:
#line 1093 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) + (yyvsp[(3) - (3)].uval); }
    break;

  case 206:
#line 1094 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) - (yyvsp[(3) - (3)].uval); }
    break;

  case 207:
#line 1095 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) * (yyvsp[(3) - (3)].uval); }
    break;

  case 208:
#line 1096 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) / (yyvsp[(3) - (3)].uval); }
    break;

  case 209:
#line 1097 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) >> (yyvsp[(3) - (3)].uval); }
    break;

  case 210:
#line 1098 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) << (yyvsp[(3) - (3)].uval); }
    break;

  case 211:
#line 1099 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) | (yyvsp[(3) - (3)].uval); }
    break;

  case 212:
#line 1100 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) ^ (yyvsp[(3) - (3)].uval); }
    break;

  case 213:
#line 1101 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) & (yyvsp[(3) - (3)].uval); }
    break;

  case 214:
#line 1102 "parser.y"
    { (yyval.uval) = !(yyvsp[(2) - (2)].uval); }
    break;

  case 215:
#line 1103 "parser.y"
    { (yyval.uval) = -(yyvsp[(2) - (2)].uval); }
    break;

  case 216:
#line 1104 "parser.y"
    { (yyval.uval) = (yyvsp[(2) - (3)].uval); }
    break;

  case 217:
#line 1109 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (1)].uval); }
    break;

  case 218:
#line 1110 "parser.y"
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[(1) - (1)].sval)); free((yyvsp[(1) - (1)].sval));}
    break;

  case 219:
#line 1111 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 220:
#line 1112 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 221:
#line 1113 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 222:
#line 1114 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 223:
#line 1115 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 224:
#line 1116 "parser.y"
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 225:
#line 1117 "parser.y"
    { (yyval.uval) = bx_dbg_get_ip (); }
    break;

  case 226:
#line 1118 "parser.y"
    { (yyval.uval) = bx_dbg_get_eip(); }
    break;

  case 227:
#line 1119 "parser.y"
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
    break;

  case 228:
#line 1120 "parser.y"
    { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[(1) - (3)].uval), (yyvsp[(3) - (3)].uval)); }
    break;

  case 229:
#line 1121 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) + (yyvsp[(3) - (3)].uval); }
    break;

  case 230:
#line 1122 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) - (yyvsp[(3) - (3)].uval); }
    break;

  case 231:
#line 1123 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) * (yyvsp[(3) - (3)].uval); }
    break;

  case 232:
#line 1124 "parser.y"
    { (yyval.uval) = ((yyvsp[(3) - (3)].uval) != 0) ? (yyvsp[(1) - (3)].uval) / (yyvsp[(3) - (3)].uval) : 0; }
    break;

  case 233:
#line 1125 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) >> (yyvsp[(3) - (3)].uval); }
    break;

  case 234:
#line 1126 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) << (yyvsp[(3) - (3)].uval); }
    break;

  case 235:
#line 1127 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) | (yyvsp[(3) - (3)].uval); }
    break;

  case 236:
#line 1128 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) ^ (yyvsp[(3) - (3)].uval); }
    break;

  case 237:
#line 1129 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) & (yyvsp[(3) - (3)].uval); }
    break;

  case 238:
#line 1130 "parser.y"
    { (yyval.uval) = !(yyvsp[(2) - (2)].uval); }
    break;

  case 239:
#line 1131 "parser.y"
    { (yyval.uval) = -(yyvsp[(2) - (2)].uval); }
    break;

  case 240:
#line 1132 "parser.y"
    { (yyval.uval) = (yyvsp[(2) - (3)].uval); }
    break;


/* Line 1267 of yacc.c.  */
#line 3543 "y.tab.c"
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


#line 1135 "parser.y"


#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
