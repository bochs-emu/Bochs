/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON_VERSION "2.4.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         bxparse
#define yylex           bxlex
#define yyerror         bxerror
#define yylval          bxlval
#define yychar          bxchar
#define yydebug         bxdebug
#define yynerrs         bxnerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 5 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

#if BX_DEBUGGER


/* Line 189 of yacc.c  */
#line 89 "y.tab.c"

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
     BX_TOKEN_MMX = 291,
     BX_TOKEN_SSE = 292,
     BX_TOKEN_AVX = 293,
     BX_TOKEN_IDT = 294,
     BX_TOKEN_IVT = 295,
     BX_TOKEN_GDT = 296,
     BX_TOKEN_LDT = 297,
     BX_TOKEN_TSS = 298,
     BX_TOKEN_TAB = 299,
     BX_TOKEN_ALL = 300,
     BX_TOKEN_LINUX = 301,
     BX_TOKEN_DEBUG_REGS = 302,
     BX_TOKEN_CONTROL_REGS = 303,
     BX_TOKEN_SEGMENT_REGS = 304,
     BX_TOKEN_EXAMINE = 305,
     BX_TOKEN_XFORMAT = 306,
     BX_TOKEN_DISFORMAT = 307,
     BX_TOKEN_RESTORE = 308,
     BX_TOKEN_SETPMEM = 309,
     BX_TOKEN_SYMBOLNAME = 310,
     BX_TOKEN_QUERY = 311,
     BX_TOKEN_PENDING = 312,
     BX_TOKEN_TAKE = 313,
     BX_TOKEN_DMA = 314,
     BX_TOKEN_IRQ = 315,
     BX_TOKEN_HEX = 316,
     BX_TOKEN_DISASM = 317,
     BX_TOKEN_INSTRUMENT = 318,
     BX_TOKEN_STRING = 319,
     BX_TOKEN_STOP = 320,
     BX_TOKEN_DOIT = 321,
     BX_TOKEN_CRC = 322,
     BX_TOKEN_TRACE = 323,
     BX_TOKEN_TRACEREG = 324,
     BX_TOKEN_TRACEMEM = 325,
     BX_TOKEN_SWITCH_MODE = 326,
     BX_TOKEN_SIZE = 327,
     BX_TOKEN_PTIME = 328,
     BX_TOKEN_TIMEBP_ABSOLUTE = 329,
     BX_TOKEN_TIMEBP = 330,
     BX_TOKEN_MODEBP = 331,
     BX_TOKEN_VMEXITBP = 332,
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
     INDIRECT = 360,
     NEG = 361,
     NOT = 362
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
#define BX_TOKEN_MMX 291
#define BX_TOKEN_SSE 292
#define BX_TOKEN_AVX 293
#define BX_TOKEN_IDT 294
#define BX_TOKEN_IVT 295
#define BX_TOKEN_GDT 296
#define BX_TOKEN_LDT 297
#define BX_TOKEN_TSS 298
#define BX_TOKEN_TAB 299
#define BX_TOKEN_ALL 300
#define BX_TOKEN_LINUX 301
#define BX_TOKEN_DEBUG_REGS 302
#define BX_TOKEN_CONTROL_REGS 303
#define BX_TOKEN_SEGMENT_REGS 304
#define BX_TOKEN_EXAMINE 305
#define BX_TOKEN_XFORMAT 306
#define BX_TOKEN_DISFORMAT 307
#define BX_TOKEN_RESTORE 308
#define BX_TOKEN_SETPMEM 309
#define BX_TOKEN_SYMBOLNAME 310
#define BX_TOKEN_QUERY 311
#define BX_TOKEN_PENDING 312
#define BX_TOKEN_TAKE 313
#define BX_TOKEN_DMA 314
#define BX_TOKEN_IRQ 315
#define BX_TOKEN_HEX 316
#define BX_TOKEN_DISASM 317
#define BX_TOKEN_INSTRUMENT 318
#define BX_TOKEN_STRING 319
#define BX_TOKEN_STOP 320
#define BX_TOKEN_DOIT 321
#define BX_TOKEN_CRC 322
#define BX_TOKEN_TRACE 323
#define BX_TOKEN_TRACEREG 324
#define BX_TOKEN_TRACEMEM 325
#define BX_TOKEN_SWITCH_MODE 326
#define BX_TOKEN_SIZE 327
#define BX_TOKEN_PTIME 328
#define BX_TOKEN_TIMEBP_ABSOLUTE 329
#define BX_TOKEN_TIMEBP 330
#define BX_TOKEN_MODEBP 331
#define BX_TOKEN_VMEXITBP 332
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
#define INDIRECT 360
#define NEG 361
#define NOT 362




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 13 "parser.y"

  char    *sval;
  Bit64u   uval;
  bx_bool  bval;



/* Line 214 of yacc.c  */
#line 347 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 359 "y.tab.c"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  279
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1430

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  122
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  54
/* YYNRULES -- Number of rules.  */
#define YYNRULES  267
/* YYNRULES -- Number of states.  */
#define YYNSTATES  517

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   362

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     115,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   118,     2,     2,     2,     2,   111,     2,
     119,   120,   109,   105,     2,   106,     2,   110,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   117,     2,
       2,   116,     2,     2,   121,     2,     2,     2,     2,     2,
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
     112,   113,   114
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
     119,   121,   125,   129,   132,   135,   139,   143,   147,   150,
     154,   157,   161,   165,   169,   172,   176,   180,   184,   187,
     192,   197,   202,   208,   214,   220,   223,   227,   231,   236,
     241,   247,   250,   254,   257,   260,   264,   269,   274,   277,
     282,   288,   294,   300,   306,   312,   318,   324,   327,   333,
     336,   340,   344,   347,   351,   356,   359,   362,   366,   370,
     374,   380,   386,   392,   398,   402,   406,   410,   414,   418,
     423,   427,   433,   441,   445,   449,   453,   454,   456,   459,
     462,   465,   468,   471,   474,   477,   480,   484,   488,   492,
     495,   500,   504,   508,   511,   516,   522,   526,   530,   535,
     539,   542,   546,   551,   555,   560,   566,   570,   575,   581,
     585,   589,   593,   598,   602,   606,   610,   614,   618,   622,
     626,   630,   634,   638,   642,   646,   650,   654,   658,   662,
     666,   670,   674,   678,   682,   686,   690,   694,   698,   702,
     706,   710,   714,   718,   722,   726,   730,   734,   738,   742,
     746,   750,   754,   758,   762,   766,   770,   774,   777,   781,
     783,   785,   787,   789,   791,   793,   795,   797,   799,   801,
     803,   807,   811,   815,   819,   823,   827,   831,   835,   839,
     842,   845,   849,   851,   853,   855,   857,   859,   861,   863,
     865,   867,   869,   871,   875,   879,   883,   887,   891,   895,
     899,   903,   907,   911,   914,   917,   920,   923
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     123,     0,    -1,   123,   124,    -1,   124,    -1,   142,    -1,
     143,    -1,   144,    -1,   145,    -1,   146,    -1,   149,    -1,
     151,    -1,   152,    -1,   153,    -1,   154,    -1,   155,    -1,
     156,    -1,   158,    -1,   157,    -1,   147,    -1,   148,    -1,
     159,    -1,   160,    -1,   161,    -1,   162,    -1,   163,    -1,
     164,    -1,   165,    -1,   166,    -1,   167,    -1,   168,    -1,
     169,    -1,   170,    -1,   171,    -1,   134,    -1,   135,    -1,
     136,    -1,   133,    -1,   128,    -1,   129,    -1,   130,    -1,
     137,    -1,   138,    -1,   132,    -1,   131,    -1,   139,    -1,
     140,    -1,   141,    -1,   172,    -1,   173,    -1,    -1,   115,
      -1,    15,    -1,    16,    -1,    32,    -1,    33,    -1,     8,
      -1,     9,    -1,    10,    -1,    11,    -1,    12,    -1,    13,
      -1,    75,    90,   115,    -1,    74,    90,   115,    -1,    76,
     115,    -1,    77,   115,    -1,    83,    98,   115,    -1,    83,
      16,   115,    -1,    83,    64,   115,    -1,    83,   115,    -1,
      93,   175,   115,    -1,    73,   115,    -1,    68,   125,   115,
      -1,    69,   125,   115,    -1,    70,   125,   115,    -1,    78,
     115,    -1,    78,    90,   115,    -1,    79,    65,   115,    -1,
      79,    17,   115,    -1,    79,   115,    -1,    79,    32,   175,
     115,    -1,    79,    81,   175,   115,    -1,    79,    82,   175,
     115,    -1,    79,    32,   175,   175,   115,    -1,    79,    81,
     175,   175,   115,    -1,    79,    82,   175,   175,   115,    -1,
      80,   115,    -1,    80,   175,   115,    -1,    84,    64,   115,
      -1,    84,    64,    90,   115,    -1,    84,    87,    64,   115,
      -1,    84,    87,    64,    90,   115,    -1,    88,   115,    -1,
      89,    90,   115,    -1,    17,   115,    -1,    18,   115,    -1,
      18,    90,   115,    -1,    18,    45,    90,   115,    -1,    18,
      90,    90,   115,    -1,    19,   115,    -1,    21,    62,   125,
     115,    -1,    21,    55,   116,    90,   115,    -1,    21,     4,
     116,   175,   115,    -1,    21,     3,   116,   175,   115,    -1,
      21,     5,   116,   175,   115,    -1,    21,     6,   116,   175,
     115,    -1,    21,     7,   116,   175,   115,    -1,    21,   127,
     116,   175,   115,    -1,    24,   115,    -1,    24,   174,   117,
     174,   115,    -1,    25,   115,    -1,    25,   175,   115,    -1,
      25,    64,   115,    -1,    26,   115,    -1,    26,   175,   115,
      -1,    26,   109,   175,   115,    -1,    23,   115,    -1,    86,
     115,    -1,    86,    64,   115,    -1,    30,    26,   115,    -1,
      30,    34,   115,    -1,    30,    39,   150,   150,   115,    -1,
      30,    40,   150,   150,   115,    -1,    30,    41,   150,   150,
     115,    -1,    30,    42,   150,   150,   115,    -1,    30,    44,
     115,    -1,    30,    43,   115,    -1,    30,    14,   115,    -1,
      30,    46,   115,    -1,    30,    85,   115,    -1,    30,    85,
      64,   115,    -1,    30,    91,   115,    -1,    30,    91,    93,
      90,   115,    -1,    30,    91,    93,    90,   126,    90,   115,
      -1,    30,    92,   115,    -1,    30,    96,   115,    -1,    30,
      97,   115,    -1,    -1,    90,    -1,   126,   115,    -1,    35,
     115,    -1,    36,   115,    -1,    37,   115,    -1,    38,   115,
      -1,    49,   115,    -1,    48,   115,    -1,    47,   115,    -1,
      27,    90,   115,    -1,    28,    90,   115,    -1,    29,    90,
     115,    -1,    31,   115,    -1,    50,    51,   175,   115,    -1,
      50,    51,   115,    -1,    50,   175,   115,    -1,    50,   115,
      -1,    53,    64,    64,   115,    -1,    54,    90,    90,    90,
     115,    -1,    56,    57,   115,    -1,    58,    59,   115,    -1,
      58,    59,    90,   115,    -1,    58,    60,   115,    -1,    62,
     115,    -1,    62,   175,   115,    -1,    62,   175,   175,   115,
      -1,    62,    52,   115,    -1,    62,    52,   175,   115,    -1,
      62,    52,   175,   175,   115,    -1,    62,    71,   115,    -1,
      62,    61,   125,   115,    -1,    62,    72,   116,    90,   115,
      -1,    63,    65,   115,    -1,    63,    98,   115,    -1,    66,
      90,   115,    -1,    67,    90,    90,   115,    -1,    94,    31,
     115,    -1,    94,    17,   115,    -1,    94,    18,   115,    -1,
      94,    19,   115,    -1,    94,    24,   115,    -1,    94,    25,
     115,    -1,    94,    26,   115,    -1,    94,    27,   115,    -1,
      94,    28,   115,    -1,    94,    29,   115,    -1,    94,    23,
     115,    -1,    94,    76,   115,    -1,    94,    77,   115,    -1,
      94,    67,   115,    -1,    94,    68,   115,    -1,    94,    69,
     115,    -1,    94,    70,   115,    -1,    94,    53,   115,    -1,
      94,    73,   115,    -1,    94,    75,   115,    -1,    94,    74,
     115,    -1,    94,    78,   115,    -1,    94,    84,   115,    -1,
      94,    86,   115,    -1,    94,   126,   115,    -1,    94,    35,
     115,    -1,    94,    36,   115,    -1,    94,    37,   115,    -1,
      94,    38,   115,    -1,    94,    49,   115,    -1,    94,    48,
     115,    -1,    94,    47,   115,    -1,    94,    54,   115,    -1,
      94,    62,   115,    -1,    94,    79,   115,    -1,    94,    80,
     115,    -1,    94,    50,   115,    -1,    94,    63,   115,    -1,
      94,    21,   115,    -1,    94,    93,   115,    -1,    94,    30,
     115,    -1,    94,    83,   115,    -1,    94,    95,   115,    -1,
      94,    94,   115,    -1,    94,   115,    -1,    95,   175,   115,
      -1,    90,    -1,    64,    -1,     4,    -1,     3,    -1,     5,
      -1,     6,    -1,     7,    -1,   127,    -1,   102,    -1,   103,
      -1,   104,    -1,   174,   105,   174,    -1,   174,   106,   174,
      -1,   174,   109,   174,    -1,   174,   110,   174,    -1,   174,
     100,   174,    -1,   174,   101,   174,    -1,   174,   107,   174,
      -1,   174,   108,   174,    -1,   174,   111,   174,    -1,   118,
     174,    -1,   106,   174,    -1,   119,   174,   120,    -1,    90,
      -1,    64,    -1,     4,    -1,     3,    -1,     5,    -1,     6,
      -1,     7,    -1,   127,    -1,   102,    -1,   103,    -1,   104,
      -1,   175,   117,   175,    -1,   175,   105,   175,    -1,   175,
     106,   175,    -1,   175,   109,   175,    -1,   175,   110,   175,
      -1,   175,   100,   175,    -1,   175,   101,   175,    -1,   175,
     107,   175,    -1,   175,   108,   175,    -1,   175,   111,   175,
      -1,   118,   175,    -1,   106,   175,    -1,   109,   175,    -1,
     121,   175,    -1,   119,   175,   120,    -1
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
     213,   218,   223,   231,   239,   247,   252,   257,   262,   270,
     278,   286,   294,   302,   310,   315,   323,   328,   333,   338,
     343,   348,   353,   358,   363,   368,   373,   381,   386,   391,
     396,   404,   412,   420,   428,   433,   438,   443,   451,   459,
     464,   469,   473,   477,   481,   485,   489,   496,   501,   506,
     511,   516,   521,   526,   531,   539,   547,   552,   560,   565,
     570,   575,   580,   585,   590,   595,   600,   605,   610,   615,
     620,   625,   630,   635,   640,   645,   653,   654,   657,   665,
     673,   681,   689,   697,   705,   713,   721,   729,   736,   744,
     752,   757,   762,   767,   775,   783,   791,   799,   804,   809,
     817,   822,   827,   832,   837,   842,   847,   852,   857,   865,
     870,   878,   886,   894,   899,   904,   911,   916,   921,   926,
     931,   936,   941,   946,   951,   956,   961,   966,   972,   978,
     984,   992,   997,  1002,  1007,  1012,  1017,  1022,  1027,  1032,
    1037,  1042,  1047,  1052,  1057,  1062,  1067,  1077,  1088,  1094,
    1107,  1112,  1122,  1127,  1143,  1155,  1165,  1170,  1178,  1196,
    1197,  1198,  1199,  1200,  1201,  1202,  1203,  1204,  1205,  1206,
    1207,  1208,  1209,  1210,  1211,  1212,  1213,  1214,  1215,  1216,
    1217,  1218,  1224,  1225,  1226,  1227,  1228,  1229,  1230,  1231,
    1232,  1233,  1234,  1235,  1236,  1237,  1238,  1239,  1240,  1241,
    1242,  1243,  1244,  1245,  1246,  1247,  1248,  1249
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
  "BX_TOKEN_MMX", "BX_TOKEN_SSE", "BX_TOKEN_AVX", "BX_TOKEN_IDT",
  "BX_TOKEN_IVT", "BX_TOKEN_GDT", "BX_TOKEN_LDT", "BX_TOKEN_TSS",
  "BX_TOKEN_TAB", "BX_TOKEN_ALL", "BX_TOKEN_LINUX", "BX_TOKEN_DEBUG_REGS",
  "BX_TOKEN_CONTROL_REGS", "BX_TOKEN_SEGMENT_REGS", "BX_TOKEN_EXAMINE",
  "BX_TOKEN_XFORMAT", "BX_TOKEN_DISFORMAT", "BX_TOKEN_RESTORE",
  "BX_TOKEN_SETPMEM", "BX_TOKEN_SYMBOLNAME", "BX_TOKEN_QUERY",
  "BX_TOKEN_PENDING", "BX_TOKEN_TAKE", "BX_TOKEN_DMA", "BX_TOKEN_IRQ",
  "BX_TOKEN_HEX", "BX_TOKEN_DISASM", "BX_TOKEN_INSTRUMENT",
  "BX_TOKEN_STRING", "BX_TOKEN_STOP", "BX_TOKEN_DOIT", "BX_TOKEN_CRC",
  "BX_TOKEN_TRACE", "BX_TOKEN_TRACEREG", "BX_TOKEN_TRACEMEM",
  "BX_TOKEN_SWITCH_MODE", "BX_TOKEN_SIZE", "BX_TOKEN_PTIME",
  "BX_TOKEN_TIMEBP_ABSOLUTE", "BX_TOKEN_TIMEBP", "BX_TOKEN_MODEBP",
  "BX_TOKEN_VMEXITBP", "BX_TOKEN_PRINT_STACK", "BX_TOKEN_WATCH",
  "BX_TOKEN_UNWATCH", "BX_TOKEN_READ", "BX_TOKEN_WRITE", "BX_TOKEN_SHOW",
  "BX_TOKEN_LOAD_SYMBOLS", "BX_TOKEN_SYMBOLS", "BX_TOKEN_LIST_SYMBOLS",
  "BX_TOKEN_GLOBAL", "BX_TOKEN_WHERE", "BX_TOKEN_PRINT_STRING",
  "BX_TOKEN_NUMERIC", "BX_TOKEN_NE2000", "BX_TOKEN_PIC", "BX_TOKEN_PAGE",
  "BX_TOKEN_HELP", "BX_TOKEN_CALC", "BX_TOKEN_VGA", "BX_TOKEN_PCI",
  "BX_TOKEN_COMMAND", "BX_TOKEN_GENERIC", "BX_TOKEN_RSHIFT",
  "BX_TOKEN_LSHIFT", "BX_TOKEN_REG_IP", "BX_TOKEN_REG_EIP",
  "BX_TOKEN_REG_RIP", "'+'", "'-'", "'|'", "'^'", "'*'", "'/'", "'&'",
  "INDIRECT", "NEG", "NOT", "'\\n'", "'='", "':'", "'!'", "'('", "')'",
  "'@'", "$accept", "commands", "command", "BX_TOKEN_TOGGLE_ON_OFF",
  "BX_TOKEN_REGISTERS", "BX_TOKEN_SEGREG", "timebp_command",
  "modebp_command", "vmexitbp_command", "show_command", "page_command",
  "ptime_command", "trace_command", "trace_reg_command",
  "trace_mem_command", "print_stack_command", "watch_point_command",
  "symbol_command", "where_command", "print_string_command",
  "continue_command", "stepN_command", "step_over_command", "set_command",
  "breakpoint_command", "blist_command", "slist_command", "info_command",
  "optional_numeric", "regs_command", "fpu_regs_command",
  "mmx_regs_command", "sse_regs_command", "avx_regs_command",
  "segment_regs_command", "control_regs_command", "debug_regs_command",
  "delete_command", "bpe_command", "bpd_command", "quit_command",
  "examine_command", "restore_command", "setpmem_command", "query_command",
  "take_command", "disassemble_command", "instrument_command",
  "doit_command", "crc_command", "help_command", "calc_command",
  "vexpression", "expression", 0
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
      47,    38,   360,   361,   362,    10,    61,    58,    33,    40,
      41,    64
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   122,   123,   123,   124,   124,   124,   124,   124,   124,
     124,   124,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   124,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   124,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   124,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   125,   125,   126,   126,   127,   127,   127,   127,   127,
     127,   128,   128,   129,   130,   131,   131,   131,   131,   132,
     133,   134,   135,   136,   137,   137,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   139,   139,   139,
     139,   140,   141,   142,   143,   143,   143,   143,   144,   145,
     145,   145,   145,   145,   145,   145,   145,   146,   146,   146,
     146,   146,   146,   146,   146,   147,   148,   148,   149,   149,
     149,   149,   149,   149,   149,   149,   149,   149,   149,   149,
     149,   149,   149,   149,   149,   149,   150,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   163,   163,   163,   164,   165,   166,   167,   167,   167,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   169,
     169,   170,   171,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   173,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175
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
       1,     3,     3,     2,     2,     3,     3,     3,     2,     3,
       2,     3,     3,     3,     2,     3,     3,     3,     2,     4,
       4,     4,     5,     5,     5,     2,     3,     3,     4,     4,
       5,     2,     3,     2,     2,     3,     4,     4,     2,     4,
       5,     5,     5,     5,     5,     5,     5,     2,     5,     2,
       3,     3,     2,     3,     4,     2,     2,     3,     3,     3,
       5,     5,     5,     5,     3,     3,     3,     3,     3,     4,
       3,     5,     7,     3,     3,     3,     0,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     3,     3,     3,     2,
       4,     3,     3,     2,     4,     5,     3,     3,     4,     3,
       2,     3,     4,     3,     4,     5,     3,     4,     5,     3,
       3,     3,     4,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     2,     2,     3
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
       0,    50,     0,     3,     0,    37,    38,    39,    43,    42,
      36,    33,    34,    35,    40,    41,    44,    45,    46,     4,
       5,     6,     7,     8,    18,    19,     9,    10,    11,    12,
      13,    14,    15,    17,    16,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    47,    48,
      93,     0,     0,    94,    98,     0,     0,     0,     0,     0,
      55,    56,    57,    58,    59,    60,     0,     0,     0,   115,
     222,   221,   223,   224,   225,   220,   219,   227,   228,   229,
       0,   107,     0,     0,   226,     0,   245,   244,   246,   247,
     248,   243,   242,   250,   251,   252,     0,     0,   109,     0,
       0,     0,   249,     0,   243,     0,   112,     0,     0,     0,
       0,     0,     0,     0,   136,   136,   136,   136,     0,     0,
       0,     0,     0,     0,     0,     0,   149,   139,   140,   141,
     142,   145,   144,   143,     0,   153,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   160,     0,     0,     0,
       0,     0,    51,    52,     0,     0,     0,    70,     0,     0,
      63,    64,     0,    74,     0,     0,     0,     0,     0,    78,
      85,     0,     0,     0,     0,    68,     0,     0,     0,   116,
      91,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   217,     0,     0,     1,
       2,   138,     0,     0,    95,     0,     0,     0,     0,     0,
       0,     0,     0,   240,   239,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   111,   264,   265,   263,
       0,   266,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   110,     0,   265,   113,   146,   147,   148,   126,   118,
     119,   137,   136,   136,   136,   136,   125,   124,   127,     0,
     128,     0,   130,   133,   134,   135,   151,     0,   152,     0,
       0,   156,     0,   157,   159,   163,     0,     0,   166,     0,
       0,     0,   161,     0,   169,   170,   171,     0,    71,    72,
      73,    62,    61,    75,    77,     0,    76,     0,     0,    86,
      66,    67,    65,     0,    87,     0,   117,    92,    69,   174,
     175,   176,   211,   183,   177,   178,   179,   180,   181,   182,
     213,   173,   198,   199,   200,   201,   204,   203,   202,   209,
     190,   205,   206,   210,   186,   187,   188,   189,   191,   193,
     192,   184,   185,   194,   207,   208,   214,   195,   196,   212,
     216,   215,   197,   218,    96,    97,     0,     0,     0,     0,
       0,     0,    99,     0,   241,   234,   235,   230,   231,   236,
     237,   232,   233,   238,     0,   267,   258,   259,   254,   255,
     260,   261,   256,   257,   262,   253,   114,     0,     0,     0,
       0,   129,     0,   150,   154,     0,   158,   164,     0,   167,
       0,   255,   256,   162,   172,    79,     0,    80,     0,    81,
       0,    88,     0,    89,   102,   101,   103,   104,   105,   100,
     106,   108,   120,   121,   122,   123,   131,     0,   155,   165,
     168,    82,    83,    84,    90,     0,   132
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    52,    53,   204,    54,   152,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,   332,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   135,   153
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -161
static const yytype_int16 yypact[] =
{
    1052,   -97,   -30,   -92,   516,   -81,    63,   554,   673,   -51,
      -9,    -8,   229,   -50,  -161,  -161,   -35,   -32,   -31,   -26,
     -12,    -6,    -4,    43,    38,    16,    48,   -22,   527,   -62,
      18,    23,    15,    15,    15,    -1,    36,    39,    17,    19,
     -73,    -3,   693,    -5,   -24,   -54,    20,    46,   855,  1133,
     855,  -161,   971,  -161,    22,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,    51,   -71,  -161,  -161,    34,    35,    40,    41,    44,
    -161,  -161,  -161,  -161,  -161,  -161,    47,    15,    54,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
     866,  -161,   866,   866,  -161,  1300,  -161,  -161,  -161,  -161,
    -161,    56,  -161,  -161,  -161,  -161,   855,   855,  -161,   855,
     855,   855,  -161,   475,  -161,   855,  -161,   780,    64,    65,
      68,    69,    71,    72,    98,    98,    98,    98,    74,    86,
      88,   -38,   -29,    89,    90,    91,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,   714,  -161,   793,    95,   117,    94,
     -70,   109,   736,    15,   113,   116,  -161,   112,   122,   123,
     126,   161,  -161,  -161,   137,   138,   142,  -161,   143,   145,
    -161,  -161,   146,  -161,   147,   855,   149,   855,   855,  -161,
    -161,   806,   150,   151,   152,  -161,   -58,   190,   159,  -161,
    -161,   162,   826,   168,   169,   171,   172,   173,   174,   175,
     176,   177,   178,   189,   201,   202,   203,   204,   207,   208,
     209,   223,   224,   225,   226,   227,   228,   230,   231,   232,
     233,   235,   236,   237,   240,   241,   254,   259,   260,   261,
     269,   270,   271,   287,   288,   289,  -161,   297,  1124,  -161,
    -161,  -161,   298,   301,  -161,   855,   855,   855,   855,   855,
     186,   302,   855,  -161,  -161,   300,   866,   866,   866,   866,
     866,   866,   866,   866,   866,   866,  -161,    21,    21,    21,
     139,    21,   855,   855,   855,   855,   855,   855,   855,   855,
     855,  -161,   855,   -93,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,    98,    98,    98,    98,  -161,  -161,  -161,   304,
    -161,   325,  -161,  -161,  -161,  -161,  -161,  1144,  -161,   306,
     332,  -161,   321,  -161,  -161,  -161,   324,   322,  -161,   348,
     855,   855,  -161,  1157,  -161,  -161,  -161,   331,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,   354,  -161,   386,   505,  -161,
    -161,  -161,  -161,   333,  -161,   -57,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  1170,  1183,  1196,  1209,
    1222,   334,  -161,  1235,  -161,  -161,  -161,   -10,   -10,   -10,
     -10,  -161,  -161,  -161,   272,  -161,    21,    21,   -13,   -13,
     -13,   -13,    21,    21,    21,  1313,  -161,   336,   337,   338,
     351,  -161,   -20,  -161,  -161,   352,  -161,  -161,  1248,  -161,
     353,   125,    21,  -161,  -161,  -161,  1261,  -161,  1274,  -161,
    1287,  -161,   355,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,   350,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,   359,  -161
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -161,  -161,   102,   -25,   -49,    -2,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -160,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -105,    -7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -265
static const yytype_int16 yytable[] =
{
     277,   157,   118,   198,   134,   333,   334,   335,   205,   206,
     228,   222,    14,    15,   214,   101,   186,   212,   100,   283,
     352,   197,   466,   104,   322,   293,   339,   294,   295,   215,
     202,   203,   383,   492,   119,   221,   199,   190,   191,   158,
     226,   232,   213,   278,   284,   353,   136,   137,   138,   139,
     140,   110,   111,   112,   113,   114,   115,   384,   493,   223,
     102,   229,   216,   227,   341,   176,   120,   121,   122,   123,
     124,   110,   111,   112,   113,   114,   115,   340,   217,   218,
     177,   159,   160,   178,   179,   103,   342,   312,   313,   180,
     296,   297,   291,   224,   184,   506,   318,   319,   320,   302,
     303,   304,   187,   181,   322,   189,   188,   154,   200,   182,
     225,   183,   219,   201,   207,   136,   137,   138,   139,   140,
     110,   111,   112,   113,   114,   115,   208,   125,   134,   209,
     134,   134,   210,   142,   211,   230,   231,   281,   322,   307,
     308,   282,   309,   310,   311,   143,   144,   145,   323,   146,
     285,   286,   147,   126,   280,     0,   287,   288,   185,   349,
     289,   149,   150,   290,   151,   127,   128,   129,   357,   130,
     292,   306,   467,   468,   469,   470,   154,   347,   131,   325,
     326,   132,   133,   327,   328,   356,   329,   330,   331,   336,
     363,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   337,   142,   338,   343,   344,   345,   350,   375,   351,
     377,   378,   312,   313,   143,   144,   145,   314,   360,   316,
     317,   361,   319,   320,   354,  -264,  -264,   362,   358,   322,
     149,   150,   359,   151,  -264,  -264,  -264,   364,   365,   312,
     313,   366,   322,   161,   314,   315,   316,   317,   318,   319,
     320,   367,   368,   369,   385,   162,   322,   370,   371,   455,
     372,   373,   374,   163,   376,   380,   381,   382,   164,   165,
     166,   167,   168,   169,   386,   170,   441,   387,   436,   437,
     438,   439,   440,   389,   390,   443,   391,   392,   393,   394,
     395,   396,   397,   398,   134,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   399,   456,   457,   458,   459,   460,
     461,   462,   463,   464,   171,   465,   400,   401,   402,   403,
     172,   173,   404,   405,   406,   174,   175,   136,   137,   138,
     139,   140,   110,   111,   112,   113,   114,   115,   407,   408,
     409,   410,   411,   412,     0,   413,   414,   415,   416,   478,
     417,   418,   419,   481,   482,   420,   421,   136,   137,   138,
     139,   140,   110,   111,   112,   113,   114,   115,   486,   422,
     488,   490,   296,   297,   423,   424,   425,   298,   299,   300,
     301,   302,   303,   304,   426,   427,   428,   501,   154,   136,
     137,   138,   139,   140,   110,   111,   112,   113,   114,   115,
     296,   297,   429,   430,   431,   298,   299,   300,   301,   302,
     303,   304,   432,   434,   142,   472,   435,   442,   154,   471,
     444,   474,   475,   507,   312,   313,   143,   144,   145,   314,
     360,   316,   317,   361,   319,   320,   476,   479,   480,   477,
     515,   322,   149,   150,   142,   151,   484,     0,   491,   499,
     154,   502,   503,   504,   312,   313,   143,   144,   145,   314,
     360,   316,   317,   361,   319,   320,   505,   508,   510,   485,
     514,   322,   149,   150,   516,   151,   142,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   312,   313,   143,   144,
     145,   314,   360,   316,   317,   361,   319,   320,     0,     0,
       0,   487,     0,   322,   149,   150,     0,   151,   136,   137,
     138,   139,   140,   110,   111,   112,   113,   114,   115,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     136,   137,   138,   139,   140,   110,   111,   112,   113,   114,
     115,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   136,   137,   138,
     139,   140,   110,   111,   112,   113,   114,   115,     0,   154,
       0,   116,     0,     0,     0,   312,   313,     0,   117,   192,
     314,   315,   316,   317,   318,   319,   320,     0,   193,     0,
     321,   154,   322,     0,     0,   142,     0,     0,   194,   195,
       0,     0,     0,     0,     0,   312,   313,   143,   144,   145,
     314,   360,   316,   317,   361,   319,   320,   142,   141,     0,
     489,     0,   322,   149,   150,     0,   151,     0,     0,   143,
     144,   145,     0,   146,     0,     0,   147,     0,     0,     0,
       0,     0,   196,     0,   142,   149,   150,     0,   151,     0,
       0,     0,     0,     0,     0,     0,   143,   144,   145,     0,
     146,     0,     0,   147,     0,     0,     0,     0,     0,   148,
       0,     0,   149,   150,     0,   151,   136,   137,   138,   139,
     140,   110,   111,   112,   113,   114,   115,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   136,   137,   138,   139,
     140,   110,   111,   112,   113,   114,   115,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   136,   137,   138,
     139,   140,   110,   111,   112,   113,   114,   115,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   154,     0,   136,
     137,   138,   139,   140,   110,   111,   112,   113,   114,   115,
       0,     0,     0,     0,     0,     0,     0,   154,     0,     0,
       0,     0,     0,   142,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   143,   144,   145,   154,   146,
       0,     0,   155,   142,     0,     0,     0,     0,   156,     0,
       0,   149,   150,     0,   151,   143,   144,   145,     0,   146,
     154,     0,   147,     0,   142,     0,     0,     0,   220,     0,
       0,   149,   150,     0,   151,     0,   143,   144,   145,     0,
     146,     0,     0,   147,     0,     0,   142,     0,     0,   346,
       0,     0,   149,   150,     0,   151,     0,     0,   143,   144,
     145,     0,   146,     0,     0,   147,     0,     0,     0,     0,
       0,   355,     0,     0,   149,   150,     0,   151,   136,   137,
     138,   139,   140,   110,   111,   112,   113,   114,   115,   120,
     121,   122,   123,   124,   110,   111,   112,   113,   114,   115,
     312,   313,     0,     0,     0,   314,   315,   316,   317,   318,
     319,   320,     0,   312,   313,   324,     0,   322,   314,   315,
     316,   317,   318,   319,   320,     0,   312,   313,   348,     0,
     322,   314,   315,   316,   317,   318,   319,   320,     0,   154,
       0,   379,     0,   322,     0,     0,   312,   313,     0,     0,
     125,   314,   315,   316,   317,   318,   319,   320,     0,     0,
       0,   388,     0,   322,     0,   142,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   126,   143,   144,   145,
       0,   146,     0,     0,   147,     0,     0,     0,   127,   128,
     129,   279,   130,   149,   150,     0,   151,     0,     0,     0,
       0,     0,     0,     0,   132,   133,     0,     0,     1,     2,
       3,     0,     4,     0,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,     0,    16,    17,    18,    19,
       0,     0,     0,     0,     0,     0,     0,     0,    20,    21,
      22,    23,     0,     0,    24,    25,     0,    26,     0,    27,
       0,     0,     0,    28,    29,     0,     0,    30,    31,    32,
      33,    34,     0,     0,    35,    36,    37,    38,    39,    40,
      41,    42,     0,     0,    43,    44,     0,    45,     0,    46,
      47,     0,     0,     0,    48,    49,    50,     0,     0,     1,
       2,     3,     0,     4,     0,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    51,    16,    17,    18,
      19,     0,     0,     0,     0,     0,     0,     0,     0,    20,
      21,    22,    23,     0,     0,    24,    25,     0,    26,     0,
      27,     0,     0,     0,    28,    29,     0,     0,    30,    31,
      32,    33,    34,     0,     0,    35,    36,    37,    38,    39,
      40,    41,    42,     0,     0,    43,    44,     0,    45,     0,
      46,    47,     0,     0,     0,    48,    49,    50,     0,     0,
     233,   234,   235,     0,   236,     0,   237,   238,   239,   240,
     241,   242,   243,   244,   245,    14,    15,    51,   246,   247,
     248,   249,     0,     0,     0,     0,     0,     0,     0,     0,
     250,   251,   252,   253,     0,     0,   254,   255,     0,     0,
       0,     0,     0,     0,     0,   256,   257,     0,     0,     0,
     258,   259,   260,   261,     0,     0,   262,   263,   264,   265,
     266,   267,   268,   269,     0,     0,   270,   271,     0,   272,
       0,     0,     0,     0,   312,   313,   273,   274,   275,   314,
     315,   316,   317,   318,   319,   320,     0,     0,     0,   433,
       0,   322,     0,     0,   312,   313,     0,     0,   276,   314,
     315,   316,   317,   318,   319,   320,     0,   312,   313,   473,
       0,   322,   314,   315,   316,   317,   318,   319,   320,     0,
     312,   313,   483,     0,   322,   314,   315,   316,   317,   318,
     319,   320,     0,   312,   313,   494,     0,   322,   314,   315,
     316,   317,   318,   319,   320,     0,   312,   313,   495,     0,
     322,   314,   315,   316,   317,   318,   319,   320,     0,   312,
     313,   496,     0,   322,   314,   315,   316,   317,   318,   319,
     320,     0,   312,   313,   497,     0,   322,   314,   315,   316,
     317,   318,   319,   320,     0,   312,   313,   498,     0,   322,
     314,   315,   316,   317,   318,   319,   320,     0,   312,   313,
     500,     0,   322,   314,   315,   316,   317,   318,   319,   320,
       0,   312,   313,   509,     0,   322,   314,   315,   316,   317,
     318,   319,   320,     0,   312,   313,   511,     0,   322,   314,
     315,   316,   317,   318,   319,   320,     0,   312,   313,   512,
       0,   322,   314,   315,   316,   317,   318,   319,   320,     0,
     296,   297,   513,     0,   322,   298,   299,   300,   301,   302,
     303,   304,     0,   312,   313,     0,     0,   305,   314,   315,
     316,   317,   318,   319,   320,     0,     0,     0,     0,     0,
     322
};

static const yytype_int16 yycheck[] =
{
      49,     8,     4,    65,     6,   165,   166,   167,    33,    34,
      64,    16,    32,    33,    17,    45,    23,    90,   115,    90,
      90,    28,   115,   115,   117,   130,    64,   132,   133,    32,
      15,    16,    90,    90,   115,    42,    98,    59,    60,    90,
      64,    48,   115,    50,   115,   115,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,   115,   115,    64,
      90,   115,    65,    87,    93,   115,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,   115,    81,    82,
     115,    90,    90,   115,   115,   115,   115,   100,   101,   115,
     100,   101,   117,    98,    51,   115,   109,   110,   111,   109,
     110,   111,    64,   115,   117,    57,    90,    64,    90,   115,
     115,   115,   115,    90,   115,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    90,    64,   130,    90,
     132,   133,   115,    90,   115,   115,    90,   115,   117,   146,
     147,    90,   149,   150,   151,   102,   103,   104,   155,   106,
     116,   116,   109,    90,    52,    -1,   116,   116,   115,    64,
     116,   118,   119,   116,   121,   102,   103,   104,   193,   106,
     116,   115,   332,   333,   334,   335,    64,   184,   115,   115,
     115,   118,   119,   115,   115,   192,   115,   115,    90,   115,
     197,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   115,    90,   115,   115,   115,   115,    90,   215,   115,
     217,   218,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   115,   100,   101,   115,   115,   117,
     118,   119,   116,   121,   109,   110,   111,   115,   115,   100,
     101,   115,   117,    14,   105,   106,   107,   108,   109,   110,
     111,    90,   115,   115,    64,    26,   117,   115,   115,   120,
     115,   115,   115,    34,   115,   115,   115,   115,    39,    40,
      41,    42,    43,    44,   115,    46,    90,   115,   285,   286,
     287,   288,   289,   115,   115,   292,   115,   115,   115,   115,
     115,   115,   115,   115,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   115,   312,   313,   314,   315,   316,
     317,   318,   319,   320,    85,   322,   115,   115,   115,   115,
      91,    92,   115,   115,   115,    96,    97,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,   115,   115,
     115,   115,   115,   115,    -1,   115,   115,   115,   115,   356,
     115,   115,   115,   360,   361,   115,   115,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,   375,   115,
     377,   378,   100,   101,   115,   115,   115,   105,   106,   107,
     108,   109,   110,   111,   115,   115,   115,   115,    64,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
     100,   101,   115,   115,   115,   105,   106,   107,   108,   109,
     110,   111,   115,   115,    90,    90,   115,   115,    64,   115,
     120,   115,    90,   472,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   115,   115,    90,   115,
      90,   117,   118,   119,    90,   121,   115,    -1,   115,   115,
      64,   115,   115,   115,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   115,   115,   115,   115,
     115,   117,   118,   119,   115,   121,    90,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,    -1,    -1,
      -1,   115,    -1,   117,   118,   119,    -1,   121,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    -1,    64,
      -1,    55,    -1,    -1,    -1,   100,   101,    -1,    62,    52,
     105,   106,   107,   108,   109,   110,   111,    -1,    61,    -1,
     115,    64,   117,    -1,    -1,    90,    -1,    -1,    71,    72,
      -1,    -1,    -1,    -1,    -1,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,    90,    64,    -1,
     115,    -1,   117,   118,   119,    -1,   121,    -1,    -1,   102,
     103,   104,    -1,   106,    -1,    -1,   109,    -1,    -1,    -1,
      -1,    -1,   115,    -1,    90,   118,   119,    -1,   121,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   102,   103,   104,    -1,
     106,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,   115,
      -1,    -1,   118,   119,    -1,   121,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    -1,    -1,
      -1,    -1,    -1,    90,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   102,   103,   104,    64,   106,
      -1,    -1,   109,    90,    -1,    -1,    -1,    -1,   115,    -1,
      -1,   118,   119,    -1,   121,   102,   103,   104,    -1,   106,
      64,    -1,   109,    -1,    90,    -1,    -1,    -1,   115,    -1,
      -1,   118,   119,    -1,   121,    -1,   102,   103,   104,    -1,
     106,    -1,    -1,   109,    -1,    -1,    90,    -1,    -1,   115,
      -1,    -1,   118,   119,    -1,   121,    -1,    -1,   102,   103,
     104,    -1,   106,    -1,    -1,   109,    -1,    -1,    -1,    -1,
      -1,   115,    -1,    -1,   118,   119,    -1,   121,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
     100,   101,    -1,    -1,    -1,   105,   106,   107,   108,   109,
     110,   111,    -1,   100,   101,   115,    -1,   117,   105,   106,
     107,   108,   109,   110,   111,    -1,   100,   101,   115,    -1,
     117,   105,   106,   107,   108,   109,   110,   111,    -1,    64,
      -1,   115,    -1,   117,    -1,    -1,   100,   101,    -1,    -1,
      64,   105,   106,   107,   108,   109,   110,   111,    -1,    -1,
      -1,   115,    -1,   117,    -1,    90,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    90,   102,   103,   104,
      -1,   106,    -1,    -1,   109,    -1,    -1,    -1,   102,   103,
     104,     0,   106,   118,   119,    -1,   121,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   118,   119,    -1,    -1,    17,    18,
      19,    -1,    21,    -1,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    -1,    35,    36,    37,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,
      49,    50,    -1,    -1,    53,    54,    -1,    56,    -1,    58,
      -1,    -1,    -1,    62,    63,    -1,    -1,    66,    67,    68,
      69,    70,    -1,    -1,    73,    74,    75,    76,    77,    78,
      79,    80,    -1,    -1,    83,    84,    -1,    86,    -1,    88,
      89,    -1,    -1,    -1,    93,    94,    95,    -1,    -1,    17,
      18,    19,    -1,    21,    -1,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,   115,    35,    36,    37,
      38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,
      48,    49,    50,    -1,    -1,    53,    54,    -1,    56,    -1,
      58,    -1,    -1,    -1,    62,    63,    -1,    -1,    66,    67,
      68,    69,    70,    -1,    -1,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    -1,    86,    -1,
      88,    89,    -1,    -1,    -1,    93,    94,    95,    -1,    -1,
      17,    18,    19,    -1,    21,    -1,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,   115,    35,    36,
      37,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    48,    49,    50,    -1,    -1,    53,    54,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    62,    63,    -1,    -1,    -1,
      67,    68,    69,    70,    -1,    -1,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,    -1,    83,    84,    -1,    86,
      -1,    -1,    -1,    -1,   100,   101,    93,    94,    95,   105,
     106,   107,   108,   109,   110,   111,    -1,    -1,    -1,   115,
      -1,   117,    -1,    -1,   100,   101,    -1,    -1,   115,   105,
     106,   107,   108,   109,   110,   111,    -1,   100,   101,   115,
      -1,   117,   105,   106,   107,   108,   109,   110,   111,    -1,
     100,   101,   115,    -1,   117,   105,   106,   107,   108,   109,
     110,   111,    -1,   100,   101,   115,    -1,   117,   105,   106,
     107,   108,   109,   110,   111,    -1,   100,   101,   115,    -1,
     117,   105,   106,   107,   108,   109,   110,   111,    -1,   100,
     101,   115,    -1,   117,   105,   106,   107,   108,   109,   110,
     111,    -1,   100,   101,   115,    -1,   117,   105,   106,   107,
     108,   109,   110,   111,    -1,   100,   101,   115,    -1,   117,
     105,   106,   107,   108,   109,   110,   111,    -1,   100,   101,
     115,    -1,   117,   105,   106,   107,   108,   109,   110,   111,
      -1,   100,   101,   115,    -1,   117,   105,   106,   107,   108,
     109,   110,   111,    -1,   100,   101,   115,    -1,   117,   105,
     106,   107,   108,   109,   110,   111,    -1,   100,   101,   115,
      -1,   117,   105,   106,   107,   108,   109,   110,   111,    -1,
     100,   101,   115,    -1,   117,   105,   106,   107,   108,   109,
     110,   111,    -1,   100,   101,    -1,    -1,   117,   105,   106,
     107,   108,   109,   110,   111,    -1,    -1,    -1,    -1,    -1,
     117
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    17,    18,    19,    21,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    35,    36,    37,    38,
      47,    48,    49,    50,    53,    54,    56,    58,    62,    63,
      66,    67,    68,    69,    70,    73,    74,    75,    76,    77,
      78,    79,    80,    83,    84,    86,    88,    89,    93,    94,
      95,   115,   123,   124,   126,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     115,    45,    90,   115,   115,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    55,    62,   127,   115,
       3,     4,     5,     6,     7,    64,    90,   102,   103,   104,
     106,   115,   118,   119,   127,   174,     3,     4,     5,     6,
       7,    64,    90,   102,   103,   104,   106,   109,   115,   118,
     119,   121,   127,   175,    64,   109,   115,   175,    90,    90,
      90,    14,    26,    34,    39,    40,    41,    42,    43,    44,
      46,    85,    91,    92,    96,    97,   115,   115,   115,   115,
     115,   115,   115,   115,    51,   115,   175,    64,    90,    57,
      59,    60,    52,    61,    71,    72,   115,   175,    65,    98,
      90,    90,    15,    16,   125,   125,   125,   115,    90,    90,
     115,   115,    90,   115,    17,    32,    65,    81,    82,   115,
     115,   175,    16,    64,    98,   115,    64,    87,    64,   115,
     115,    90,   175,    17,    18,    19,    21,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    35,    36,    37,    38,
      47,    48,    49,    50,    53,    54,    62,    63,    67,    68,
      69,    70,    73,    74,    75,    76,    77,    78,    79,    80,
      83,    84,    86,    93,    94,    95,   115,   126,   175,     0,
     124,   115,    90,    90,   115,   116,   116,   116,   116,   116,
     116,   125,   116,   174,   174,   174,   100,   101,   105,   106,
     107,   108,   109,   110,   111,   117,   115,   175,   175,   175,
     175,   175,   100,   101,   105,   106,   107,   108,   109,   110,
     111,   115,   117,   175,   115,   115,   115,   115,   115,   115,
     115,    90,   150,   150,   150,   150,   115,   115,   115,    64,
     115,    93,   115,   115,   115,   115,   115,   175,   115,    64,
      90,   115,    90,   115,   115,   115,   175,   125,   115,   116,
     106,   109,   115,   175,   115,   115,   115,    90,   115,   115,
     115,   115,   115,   115,   115,   175,   115,   175,   175,   115,
     115,   115,   115,    90,   115,    64,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   175,   175,   175,   175,
     175,    90,   115,   175,   120,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   120,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   115,   150,   150,   150,
     150,   115,    90,   115,   115,    90,   115,   115,   175,   115,
      90,   175,   175,   115,   115,   115,   175,   115,   175,   115,
     175,   115,    90,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   126,   115,   115,
     115,   115,   115,   115,   115,    90,   115
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

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
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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

/* Line 1464 of yacc.c  */
#line 191 "parser.y"
    {
      }
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 198 "parser.y"
    { (yyval.bval)=(yyvsp[(1) - (1)].bval); }
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 204 "parser.y"
    { (yyval.sval)=(yyvsp[(1) - (1)].sval); }
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 214 "parser.y"
    { (yyval.uval)=(yyvsp[(1) - (1)].uval); }
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 219 "parser.y"
    {
          bx_dbg_timebp_command(0, (yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 224 "parser.y"
    {
          bx_dbg_timebp_command(1, (yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 232 "parser.y"
    {
          bx_dbg_modebp_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 240 "parser.y"
    {
          bx_dbg_vmexitbp_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 248 "parser.y"
    {
          bx_dbg_show_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 253 "parser.y"
    {
          bx_dbg_show_command("off");
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 258 "parser.y"
    {
          bx_dbg_show_param_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 263 "parser.y"
    {
          bx_dbg_show_command(0);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 271 "parser.y"
    {
          bx_dbg_xlate_address((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 279 "parser.y"
    {
          bx_dbg_ptime_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 287 "parser.y"
    {
          bx_dbg_trace_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 295 "parser.y"
    {
          bx_dbg_trace_reg_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 303 "parser.y"
    {
          bx_dbg_trace_mem_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 311 "parser.y"
    {
          bx_dbg_print_stack_command(16);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 316 "parser.y"
    {
          bx_dbg_print_stack_command((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 324 "parser.y"
    {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 329 "parser.y"
    {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 334 "parser.y"
    {
          bx_dbg_print_watchpoints();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 339 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (4)].uval), 1); /* BX_READ */
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 344 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (4)].uval), 1); /* BX_READ */
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 349 "parser.y"
    {
          bx_dbg_watch(1, (yyvsp[(3) - (4)].uval), 1); /* BX_WRITE */
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 354 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval)); /* BX_READ */
          free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 359 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval)); /* BX_READ */
          free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 364 "parser.y"
    {
          bx_dbg_watch(1, (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval)); /* BX_WRITE */
          free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 369 "parser.y"
    {
          bx_dbg_unwatch_all();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 374 "parser.y"
    {
          bx_dbg_unwatch((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 382 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(2) - (3)].sval), 0, 0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 387 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(2) - (4)].sval), 0, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 392 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(3) - (4)].sval), 1, 0);
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 397 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(3) - (5)].sval), 1, (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval)); free((yyvsp[(3) - (5)].sval));
      }
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 405 "parser.y"
    {
        bx_dbg_where_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 413 "parser.y"
    {
        bx_dbg_print_string_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 421 "parser.y"
    {
        bx_dbg_continue_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 429 "parser.y"
    {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 434 "parser.y"
    {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 439 "parser.y"
    {
        bx_dbg_stepN_command(-1, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 444 "parser.y"
    {
        bx_dbg_stepN_command((yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 452 "parser.y"
    {
        bx_dbg_step_over_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 460 "parser.y"
    {
        bx_dbg_set_auto_disassemble((yyvsp[(3) - (4)].bval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 100:

/* Line 1464 of yacc.c  */
#line 465 "parser.y"
    {
        bx_dbg_set_symbol_command((yyvsp[(2) - (5)].sval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 101:

/* Line 1464 of yacc.c  */
#line 470 "parser.y"
    { 
        bx_dbg_set_reg8l_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 474 "parser.y"
    { 
        bx_dbg_set_reg8h_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 478 "parser.y"
    { 
        bx_dbg_set_reg16_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 482 "parser.y"
    { 
        bx_dbg_set_reg32_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 486 "parser.y"
    { 
        bx_dbg_set_reg64_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 490 "parser.y"
    { 
        bx_dbg_load_segreg((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 497 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 502 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval));
      }
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 507 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 512 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 517 "parser.y"
    {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 112:

/* Line 1464 of yacc.c  */
#line 522 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 113:

/* Line 1464 of yacc.c  */
#line 527 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 114:

/* Line 1464 of yacc.c  */
#line 532 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 115:

/* Line 1464 of yacc.c  */
#line 540 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 116:

/* Line 1464 of yacc.c  */
#line 548 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 117:

/* Line 1464 of yacc.c  */
#line 553 "parser.y"
    {
        bx_dbg_info_symbols_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 561 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 119:

/* Line 1464 of yacc.c  */
#line 566 "parser.y"
    {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 120:

/* Line 1464 of yacc.c  */
#line 571 "parser.y"
    {
        bx_dbg_info_idt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 121:

/* Line 1464 of yacc.c  */
#line 576 "parser.y"
    {
        bx_dbg_info_ivt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 581 "parser.y"
    {
        bx_dbg_info_gdt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 586 "parser.y"
    {
        bx_dbg_info_ldt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 591 "parser.y"
    {
        bx_dbg_dump_table();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 596 "parser.y"
    {
        bx_dbg_info_tss_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 601 "parser.y"
    {
        bx_dbg_info_flags();
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 606 "parser.y"
    {
        bx_dbg_info_linux_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 611 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 616 "parser.y"
    {
        bx_dbg_info_symbols_command((yyvsp[(3) - (4)].sval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 621 "parser.y"
    {
        bx_dbg_info_ne2k(-1, -1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 626 "parser.y"
    {
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval)); free((yyvsp[(3) - (5)].sval));
        bx_dbg_info_ne2k((yyvsp[(4) - (5)].uval), -1);
      }
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 631 "parser.y"
    {
        free((yyvsp[(1) - (7)].sval)); free((yyvsp[(2) - (7)].sval)); free((yyvsp[(3) - (7)].sval)); free((yyvsp[(5) - (7)].sval));
        bx_dbg_info_ne2k((yyvsp[(4) - (7)].uval), (yyvsp[(6) - (7)].uval));
      }
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 636 "parser.y"
    {
        bx_dbg_info_pic();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 134:

/* Line 1464 of yacc.c  */
#line 641 "parser.y"
    {
        bx_dbg_info_vga();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 135:

/* Line 1464 of yacc.c  */
#line 646 "parser.y"
    {
        bx_dbg_info_pci();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 136:

/* Line 1464 of yacc.c  */
#line 653 "parser.y"
    { (yyval.uval) = EMPTY_ARG; }
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 658 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 666 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 140:

/* Line 1464 of yacc.c  */
#line 674 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 141:

/* Line 1464 of yacc.c  */
#line 682 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 142:

/* Line 1464 of yacc.c  */
#line 690 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_AVX_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 143:

/* Line 1464 of yacc.c  */
#line 698 "parser.y"
    {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 706 "parser.y"
    {
        bx_dbg_info_control_regs_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 714 "parser.y"
    {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 722 "parser.y"
    {
        bx_dbg_del_breakpoint_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 730 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[(2) - (3)].uval), 1);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 737 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[(2) - (3)].uval), 0);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 745 "parser.y"
    {
        bx_dbg_quit_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 753 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (4)].sval), (yyvsp[(2) - (4)].sval),1, (yyvsp[(3) - (4)].uval), 1);
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 758 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (3)].sval), (yyvsp[(2) - (3)].sval),1, 0, 0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 763 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (3)].sval), NULL,0, (yyvsp[(2) - (3)].uval), 1);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 153:

/* Line 1464 of yacc.c  */
#line 768 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (2)].sval), NULL,0, 0, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 776 "parser.y"
    {
        bx_dbg_restore_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].sval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 784 "parser.y"
    {
        bx_dbg_setpmem_command((yyvsp[(2) - (5)].uval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval));
      }
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 792 "parser.y"
    {
        bx_dbg_query_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 157:

/* Line 1464 of yacc.c  */
#line 800 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (3)].sval), 1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 158:

/* Line 1464 of yacc.c  */
#line 805 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 159:

/* Line 1464 of yacc.c  */
#line 810 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (3)].sval), 1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 160:

/* Line 1464 of yacc.c  */
#line 818 "parser.y"
    {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 161:

/* Line 1464 of yacc.c  */
#line 823 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[(2) - (3)].uval), (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 162:

/* Line 1464 of yacc.c  */
#line 828 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 163:

/* Line 1464 of yacc.c  */
#line 833 "parser.y"
    {
        bx_dbg_disassemble_current((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 164:

/* Line 1464 of yacc.c  */
#line 838 "parser.y"
    {
        bx_dbg_disassemble_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 843 "parser.y"
    {
        bx_dbg_disassemble_command((yyvsp[(2) - (5)].sval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 848 "parser.y"
    {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 853 "parser.y"
    {
        bx_dbg_disassemble_hex_mode_switch((yyvsp[(3) - (4)].bval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 858 "parser.y"
    {
        bx_dbg_set_disassemble_size((yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 866 "parser.y"
    {
        bx_dbg_instrument_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 871 "parser.y"
    {
        bx_dbg_instrument_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 879 "parser.y"
    {
        bx_dbg_doit_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 172:

/* Line 1464 of yacc.c  */
#line 887 "parser.y"
    {
        bx_dbg_crc_command((yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 173:

/* Line 1464 of yacc.c  */
#line 895 "parser.y"
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 174:

/* Line 1464 of yacc.c  */
#line 900 "parser.y"
    {
         dbg_printf("c|cont|continue - continue executing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 175:

/* Line 1464 of yacc.c  */
#line 905 "parser.y"
    {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 176:

/* Line 1464 of yacc.c  */
#line 912 "parser.y"
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 177:

/* Line 1464 of yacc.c  */
#line 917 "parser.y"
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 178:

/* Line 1464 of yacc.c  */
#line 922 "parser.y"
    {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 927 "parser.y"
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 932 "parser.y"
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 937 "parser.y"
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 182:

/* Line 1464 of yacc.c  */
#line 942 "parser.y"
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 183:

/* Line 1464 of yacc.c  */
#line 947 "parser.y"
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 184:

/* Line 1464 of yacc.c  */
#line 952 "parser.y"
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 185:

/* Line 1464 of yacc.c  */
#line 957 "parser.y"
    {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 186:

/* Line 1464 of yacc.c  */
#line 962 "parser.y"
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 187:

/* Line 1464 of yacc.c  */
#line 967 "parser.y"
    {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 188:

/* Line 1464 of yacc.c  */
#line 973 "parser.y"
    {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 189:

/* Line 1464 of yacc.c  */
#line 979 "parser.y"
    {
         dbg_printf("trace-mem on  - print all memory accesses occured during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 190:

/* Line 1464 of yacc.c  */
#line 985 "parser.y"
    {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 191:

/* Line 1464 of yacc.c  */
#line 993 "parser.y"
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 192:

/* Line 1464 of yacc.c  */
#line 998 "parser.y"
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 193:

/* Line 1464 of yacc.c  */
#line 1003 "parser.y"
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 194:

/* Line 1464 of yacc.c  */
#line 1008 "parser.y"
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 195:

/* Line 1464 of yacc.c  */
#line 1013 "parser.y"
    {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 196:

/* Line 1464 of yacc.c  */
#line 1018 "parser.y"
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 197:

/* Line 1464 of yacc.c  */
#line 1023 "parser.y"
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 198:

/* Line 1464 of yacc.c  */
#line 1028 "parser.y"
    {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 199:

/* Line 1464 of yacc.c  */
#line 1033 "parser.y"
    {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 200:

/* Line 1464 of yacc.c  */
#line 1038 "parser.y"
    {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 201:

/* Line 1464 of yacc.c  */
#line 1043 "parser.y"
    {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 202:

/* Line 1464 of yacc.c  */
#line 1048 "parser.y"
    {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 203:

/* Line 1464 of yacc.c  */
#line 1053 "parser.y"
    {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 204:

/* Line 1464 of yacc.c  */
#line 1058 "parser.y"
    {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 205:

/* Line 1464 of yacc.c  */
#line 1063 "parser.y"
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 206:

/* Line 1464 of yacc.c  */
#line 1068 "parser.y"
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

  case 207:

/* Line 1464 of yacc.c  */
#line 1078 "parser.y"
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

  case 208:

/* Line 1464 of yacc.c  */
#line 1089 "parser.y"
    {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 1095 "parser.y"
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

  case 210:

/* Line 1464 of yacc.c  */
#line 1108 "parser.y"
    {
         dbg_printf("instrument <command> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command>\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 211:

/* Line 1464 of yacc.c  */
#line 1113 "parser.y"
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

  case 212:

/* Line 1464 of yacc.c  */
#line 1123 "parser.y"
    {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 213:

/* Line 1464 of yacc.c  */
#line 1128 "parser.y"
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

  case 214:

/* Line 1464 of yacc.c  */
#line 1144 "parser.y"
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

  case 215:

/* Line 1464 of yacc.c  */
#line 1156 "parser.y"
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

  case 216:

/* Line 1464 of yacc.c  */
#line 1166 "parser.y"
    {
         bx_dbg_print_help();
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 217:

/* Line 1464 of yacc.c  */
#line 1171 "parser.y"
    {
         bx_dbg_print_help();
         free((yyvsp[(1) - (2)].sval));
       }
    break;

  case 218:

/* Line 1464 of yacc.c  */
#line 1179 "parser.y"
    {
     bx_dbg_calc_command((yyvsp[(2) - (3)].uval));
     free((yyvsp[(1) - (3)].sval));
   }
    break;

  case 219:

/* Line 1464 of yacc.c  */
#line 1196 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (1)].uval); }
    break;

  case 220:

/* Line 1464 of yacc.c  */
#line 1197 "parser.y"
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[(1) - (1)].sval)); free((yyvsp[(1) - (1)].sval));}
    break;

  case 221:

/* Line 1464 of yacc.c  */
#line 1198 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 222:

/* Line 1464 of yacc.c  */
#line 1199 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 223:

/* Line 1464 of yacc.c  */
#line 1200 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 224:

/* Line 1464 of yacc.c  */
#line 1201 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 1202 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 226:

/* Line 1464 of yacc.c  */
#line 1203 "parser.y"
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 1204 "parser.y"
    { (yyval.uval) = bx_dbg_get_ip (); }
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 1205 "parser.y"
    { (yyval.uval) = bx_dbg_get_eip(); }
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 1206 "parser.y"
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 1207 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) + (yyvsp[(3) - (3)].uval); }
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 1208 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) - (yyvsp[(3) - (3)].uval); }
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 1209 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) * (yyvsp[(3) - (3)].uval); }
    break;

  case 233:

/* Line 1464 of yacc.c  */
#line 1210 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) / (yyvsp[(3) - (3)].uval); }
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 1211 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) >> (yyvsp[(3) - (3)].uval); }
    break;

  case 235:

/* Line 1464 of yacc.c  */
#line 1212 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) << (yyvsp[(3) - (3)].uval); }
    break;

  case 236:

/* Line 1464 of yacc.c  */
#line 1213 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) | (yyvsp[(3) - (3)].uval); }
    break;

  case 237:

/* Line 1464 of yacc.c  */
#line 1214 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) ^ (yyvsp[(3) - (3)].uval); }
    break;

  case 238:

/* Line 1464 of yacc.c  */
#line 1215 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) & (yyvsp[(3) - (3)].uval); }
    break;

  case 239:

/* Line 1464 of yacc.c  */
#line 1216 "parser.y"
    { (yyval.uval) = !(yyvsp[(2) - (2)].uval); }
    break;

  case 240:

/* Line 1464 of yacc.c  */
#line 1217 "parser.y"
    { (yyval.uval) = -(yyvsp[(2) - (2)].uval); }
    break;

  case 241:

/* Line 1464 of yacc.c  */
#line 1218 "parser.y"
    { (yyval.uval) = (yyvsp[(2) - (3)].uval); }
    break;

  case 242:

/* Line 1464 of yacc.c  */
#line 1224 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (1)].uval); }
    break;

  case 243:

/* Line 1464 of yacc.c  */
#line 1225 "parser.y"
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[(1) - (1)].sval)); free((yyvsp[(1) - (1)].sval));}
    break;

  case 244:

/* Line 1464 of yacc.c  */
#line 1226 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 245:

/* Line 1464 of yacc.c  */
#line 1227 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 246:

/* Line 1464 of yacc.c  */
#line 1228 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 247:

/* Line 1464 of yacc.c  */
#line 1229 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 1230 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 249:

/* Line 1464 of yacc.c  */
#line 1231 "parser.y"
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 250:

/* Line 1464 of yacc.c  */
#line 1232 "parser.y"
    { (yyval.uval) = bx_dbg_get_ip (); }
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 1233 "parser.y"
    { (yyval.uval) = bx_dbg_get_eip(); }
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 1234 "parser.y"
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
    break;

  case 253:

/* Line 1464 of yacc.c  */
#line 1235 "parser.y"
    { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[(1) - (3)].uval), (yyvsp[(3) - (3)].uval)); }
    break;

  case 254:

/* Line 1464 of yacc.c  */
#line 1236 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) + (yyvsp[(3) - (3)].uval); }
    break;

  case 255:

/* Line 1464 of yacc.c  */
#line 1237 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) - (yyvsp[(3) - (3)].uval); }
    break;

  case 256:

/* Line 1464 of yacc.c  */
#line 1238 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) * (yyvsp[(3) - (3)].uval); }
    break;

  case 257:

/* Line 1464 of yacc.c  */
#line 1239 "parser.y"
    { (yyval.uval) = ((yyvsp[(3) - (3)].uval) != 0) ? (yyvsp[(1) - (3)].uval) / (yyvsp[(3) - (3)].uval) : 0; }
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 1240 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) >> (yyvsp[(3) - (3)].uval); }
    break;

  case 259:

/* Line 1464 of yacc.c  */
#line 1241 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) << (yyvsp[(3) - (3)].uval); }
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 1242 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) | (yyvsp[(3) - (3)].uval); }
    break;

  case 261:

/* Line 1464 of yacc.c  */
#line 1243 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) ^ (yyvsp[(3) - (3)].uval); }
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 1244 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) & (yyvsp[(3) - (3)].uval); }
    break;

  case 263:

/* Line 1464 of yacc.c  */
#line 1245 "parser.y"
    { (yyval.uval) = !(yyvsp[(2) - (2)].uval); }
    break;

  case 264:

/* Line 1464 of yacc.c  */
#line 1246 "parser.y"
    { (yyval.uval) = -(yyvsp[(2) - (2)].uval); }
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 1247 "parser.y"
    { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[(2) - (2)].uval)); }
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 1248 "parser.y"
    { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[(2) - (2)].uval)); }
    break;

  case 267:

/* Line 1464 of yacc.c  */
#line 1249 "parser.y"
    { (yyval.uval) = (yyvsp[(2) - (3)].uval); }
    break;



/* Line 1464 of yacc.c  */
#line 4249 "y.tab.c"
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
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
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



/* Line 1684 of yacc.c  */
#line 1252 "parser.y"


#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
