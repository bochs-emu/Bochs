/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.5"

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

/* Line 268 of yacc.c  */
#line 5 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

#if BX_DEBUGGER


/* Line 268 of yacc.c  */
#line 88 "y.tab.c"

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
     BX_TOKEN_SET = 275,
     BX_TOKEN_DEBUGGER = 276,
     BX_TOKEN_LIST_BREAK = 277,
     BX_TOKEN_VBREAKPOINT = 278,
     BX_TOKEN_LBREAKPOINT = 279,
     BX_TOKEN_PBREAKPOINT = 280,
     BX_TOKEN_DEL_BREAKPOINT = 281,
     BX_TOKEN_ENABLE_BREAKPOINT = 282,
     BX_TOKEN_DISABLE_BREAKPOINT = 283,
     BX_TOKEN_INFO = 284,
     BX_TOKEN_QUIT = 285,
     BX_TOKEN_R = 286,
     BX_TOKEN_REGS = 287,
     BX_TOKEN_CPU = 288,
     BX_TOKEN_FPU = 289,
     BX_TOKEN_MMX = 290,
     BX_TOKEN_SSE = 291,
     BX_TOKEN_AVX = 292,
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
     BX_TOKEN_WRITEMEM = 308,
     BX_TOKEN_SETPMEM = 309,
     BX_TOKEN_SYMBOLNAME = 310,
     BX_TOKEN_QUERY = 311,
     BX_TOKEN_PENDING = 312,
     BX_TOKEN_TAKE = 313,
     BX_TOKEN_DMA = 314,
     BX_TOKEN_IRQ = 315,
     BX_TOKEN_TLB = 316,
     BX_TOKEN_HEX = 317,
     BX_TOKEN_DISASM = 318,
     BX_TOKEN_INSTRUMENT = 319,
     BX_TOKEN_STRING = 320,
     BX_TOKEN_STOP = 321,
     BX_TOKEN_DOIT = 322,
     BX_TOKEN_CRC = 323,
     BX_TOKEN_TRACE = 324,
     BX_TOKEN_TRACEREG = 325,
     BX_TOKEN_TRACEMEM = 326,
     BX_TOKEN_SWITCH_MODE = 327,
     BX_TOKEN_SIZE = 328,
     BX_TOKEN_PTIME = 329,
     BX_TOKEN_TIMEBP_ABSOLUTE = 330,
     BX_TOKEN_TIMEBP = 331,
     BX_TOKEN_MODEBP = 332,
     BX_TOKEN_VMEXITBP = 333,
     BX_TOKEN_PRINT_STACK = 334,
     BX_TOKEN_WATCH = 335,
     BX_TOKEN_UNWATCH = 336,
     BX_TOKEN_READ = 337,
     BX_TOKEN_WRITE = 338,
     BX_TOKEN_SHOW = 339,
     BX_TOKEN_LOAD_SYMBOLS = 340,
     BX_TOKEN_SYMBOLS = 341,
     BX_TOKEN_LIST_SYMBOLS = 342,
     BX_TOKEN_GLOBAL = 343,
     BX_TOKEN_WHERE = 344,
     BX_TOKEN_PRINT_STRING = 345,
     BX_TOKEN_NUMERIC = 346,
     BX_TOKEN_PAGE = 347,
     BX_TOKEN_HELP = 348,
     BX_TOKEN_CALC = 349,
     BX_TOKEN_VGA = 350,
     BX_TOKEN_DEVICE = 351,
     BX_TOKEN_COMMAND = 352,
     BX_TOKEN_GENERIC = 353,
     BX_TOKEN_RSHIFT = 354,
     BX_TOKEN_LSHIFT = 355,
     BX_TOKEN_REG_IP = 356,
     BX_TOKEN_REG_EIP = 357,
     BX_TOKEN_REG_RIP = 358,
     INDIRECT = 359,
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
#define BX_TOKEN_SET 275
#define BX_TOKEN_DEBUGGER 276
#define BX_TOKEN_LIST_BREAK 277
#define BX_TOKEN_VBREAKPOINT 278
#define BX_TOKEN_LBREAKPOINT 279
#define BX_TOKEN_PBREAKPOINT 280
#define BX_TOKEN_DEL_BREAKPOINT 281
#define BX_TOKEN_ENABLE_BREAKPOINT 282
#define BX_TOKEN_DISABLE_BREAKPOINT 283
#define BX_TOKEN_INFO 284
#define BX_TOKEN_QUIT 285
#define BX_TOKEN_R 286
#define BX_TOKEN_REGS 287
#define BX_TOKEN_CPU 288
#define BX_TOKEN_FPU 289
#define BX_TOKEN_MMX 290
#define BX_TOKEN_SSE 291
#define BX_TOKEN_AVX 292
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
#define BX_TOKEN_WRITEMEM 308
#define BX_TOKEN_SETPMEM 309
#define BX_TOKEN_SYMBOLNAME 310
#define BX_TOKEN_QUERY 311
#define BX_TOKEN_PENDING 312
#define BX_TOKEN_TAKE 313
#define BX_TOKEN_DMA 314
#define BX_TOKEN_IRQ 315
#define BX_TOKEN_TLB 316
#define BX_TOKEN_HEX 317
#define BX_TOKEN_DISASM 318
#define BX_TOKEN_INSTRUMENT 319
#define BX_TOKEN_STRING 320
#define BX_TOKEN_STOP 321
#define BX_TOKEN_DOIT 322
#define BX_TOKEN_CRC 323
#define BX_TOKEN_TRACE 324
#define BX_TOKEN_TRACEREG 325
#define BX_TOKEN_TRACEMEM 326
#define BX_TOKEN_SWITCH_MODE 327
#define BX_TOKEN_SIZE 328
#define BX_TOKEN_PTIME 329
#define BX_TOKEN_TIMEBP_ABSOLUTE 330
#define BX_TOKEN_TIMEBP 331
#define BX_TOKEN_MODEBP 332
#define BX_TOKEN_VMEXITBP 333
#define BX_TOKEN_PRINT_STACK 334
#define BX_TOKEN_WATCH 335
#define BX_TOKEN_UNWATCH 336
#define BX_TOKEN_READ 337
#define BX_TOKEN_WRITE 338
#define BX_TOKEN_SHOW 339
#define BX_TOKEN_LOAD_SYMBOLS 340
#define BX_TOKEN_SYMBOLS 341
#define BX_TOKEN_LIST_SYMBOLS 342
#define BX_TOKEN_GLOBAL 343
#define BX_TOKEN_WHERE 344
#define BX_TOKEN_PRINT_STRING 345
#define BX_TOKEN_NUMERIC 346
#define BX_TOKEN_PAGE 347
#define BX_TOKEN_HELP 348
#define BX_TOKEN_CALC 349
#define BX_TOKEN_VGA 350
#define BX_TOKEN_DEVICE 351
#define BX_TOKEN_COMMAND 352
#define BX_TOKEN_GENERIC 353
#define BX_TOKEN_RSHIFT 354
#define BX_TOKEN_LSHIFT 355
#define BX_TOKEN_REG_IP 356
#define BX_TOKEN_REG_EIP 357
#define BX_TOKEN_REG_RIP 358
#define INDIRECT 359
#define NEG 360
#define NOT 361




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 13 "parser.y"

  char    *sval;
  Bit64u   uval;
  bx_bool  bval;



/* Line 293 of yacc.c  */
#line 344 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 356 "y.tab.c"

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
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

# define YYCOPY_NEEDED 1

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

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
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
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  285
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1443

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  121
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  56
/* YYNRULES -- Number of rules.  */
#define YYNRULES  271
/* YYNRULES -- Number of states.  */
#define YYNSTATES  529

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
       2,     2,     2,   117,     2,     2,     2,     2,   110,     2,
     118,   119,   108,   104,     2,   105,     2,   109,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   116,     2,
       2,   115,     2,     2,   120,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   107,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   106,     2,     2,     2,     2,     2,
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
      95,    96,    97,    98,    99,   100,   101,   102,   103,   111,
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
     100,   102,   103,   105,   107,   109,   111,   113,   115,   117,
     119,   121,   123,   125,   129,   133,   136,   139,   143,   147,
     151,   154,   158,   162,   165,   169,   173,   177,   180,   184,
     188,   192,   195,   200,   205,   210,   216,   222,   228,   231,
     235,   239,   244,   249,   255,   258,   262,   265,   268,   272,
     277,   282,   285,   290,   296,   302,   308,   314,   320,   326,
     332,   338,   344,   347,   353,   356,   360,   364,   367,   371,
     376,   379,   382,   386,   390,   394,   400,   406,   412,   418,
     422,   426,   430,   434,   438,   443,   447,   452,   458,   459,
     461,   464,   467,   470,   473,   476,   479,   482,   485,   489,
     493,   497,   500,   505,   509,   513,   516,   521,   527,   533,
     537,   541,   546,   550,   553,   557,   562,   566,   571,   577,
     581,   586,   592,   596,   600,   604,   609,   613,   617,   621,
     625,   629,   633,   637,   641,   645,   649,   653,   657,   661,
     665,   669,   673,   677,   681,   685,   689,   693,   697,   701,
     705,   709,   713,   717,   721,   725,   729,   733,   737,   741,
     745,   749,   753,   757,   761,   765,   769,   773,   777,   781,
     785,   789,   792,   796,   798,   800,   802,   804,   806,   808,
     810,   812,   814,   816,   818,   822,   826,   830,   834,   838,
     842,   846,   850,   854,   857,   860,   864,   866,   868,   870,
     872,   874,   876,   878,   880,   882,   884,   886,   890,   894,
     898,   902,   906,   910,   914,   918,   922,   926,   929,   932,
     935,   938
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     122,     0,    -1,   122,   123,    -1,   123,    -1,   142,    -1,
     143,    -1,   144,    -1,   145,    -1,   146,    -1,   149,    -1,
     151,    -1,   152,    -1,   153,    -1,   154,    -1,   155,    -1,
     156,    -1,   158,    -1,   157,    -1,   147,    -1,   148,    -1,
     159,    -1,   160,    -1,   161,    -1,   162,    -1,   163,    -1,
     164,    -1,   165,    -1,   166,    -1,   167,    -1,   168,    -1,
     169,    -1,   170,    -1,   171,    -1,   172,    -1,   134,    -1,
     135,    -1,   136,    -1,   133,    -1,   127,    -1,   128,    -1,
     129,    -1,   137,    -1,   138,    -1,   131,    -1,   132,    -1,
     130,    -1,   139,    -1,   140,    -1,   141,    -1,   173,    -1,
     174,    -1,    -1,   114,    -1,    15,    -1,    16,    -1,    31,
      -1,    32,    -1,     8,    -1,     9,    -1,    10,    -1,    11,
      -1,    12,    -1,    13,    -1,    76,    91,   114,    -1,    75,
      91,   114,    -1,    77,   114,    -1,    78,   114,    -1,    84,
      97,   114,    -1,    84,    16,   114,    -1,    84,    65,   114,
      -1,    84,   114,    -1,    92,   176,   114,    -1,    61,   176,
     114,    -1,    74,   114,    -1,    69,   124,   114,    -1,    70,
     124,   114,    -1,    71,   124,   114,    -1,    79,   114,    -1,
      79,    91,   114,    -1,    80,    66,   114,    -1,    80,    17,
     114,    -1,    80,   114,    -1,    80,    31,   176,   114,    -1,
      80,    82,   176,   114,    -1,    80,    83,   176,   114,    -1,
      80,    31,   176,   176,   114,    -1,    80,    82,   176,   176,
     114,    -1,    80,    83,   176,   176,   114,    -1,    81,   114,
      -1,    81,   176,   114,    -1,    85,    65,   114,    -1,    85,
      65,    91,   114,    -1,    85,    88,    65,   114,    -1,    85,
      88,    65,    91,   114,    -1,    89,   114,    -1,    90,    91,
     114,    -1,    17,   114,    -1,    18,   114,    -1,    18,    91,
     114,    -1,    18,    44,    91,   114,    -1,    18,    91,    91,
     114,    -1,    19,   114,    -1,    20,    63,   124,   114,    -1,
      20,    55,   115,    91,   114,    -1,    20,     4,   115,   176,
     114,    -1,    20,     3,   115,   176,   114,    -1,    20,     5,
     115,   176,   114,    -1,    20,     6,   115,   176,   114,    -1,
      20,     7,   115,   176,   114,    -1,    20,   102,   115,   176,
     114,    -1,    20,   103,   115,   176,   114,    -1,    20,   126,
     115,   176,   114,    -1,    23,   114,    -1,    23,   175,   116,
     175,   114,    -1,    24,   114,    -1,    24,   176,   114,    -1,
      24,    65,   114,    -1,    25,   114,    -1,    25,   176,   114,
      -1,    25,   108,   176,   114,    -1,    22,   114,    -1,    87,
     114,    -1,    87,    65,   114,    -1,    29,    25,   114,    -1,
      29,    33,   114,    -1,    29,    38,   150,   150,   114,    -1,
      29,    39,   150,   150,   114,    -1,    29,    40,   150,   150,
     114,    -1,    29,    41,   150,   150,   114,    -1,    29,    43,
     114,    -1,    29,    42,   114,    -1,    29,    14,   114,    -1,
      29,    45,   114,    -1,    29,    86,   114,    -1,    29,    86,
      65,   114,    -1,    29,    96,   114,    -1,    29,    96,    65,
     114,    -1,    29,    96,    65,    65,   114,    -1,    -1,    91,
      -1,   125,   114,    -1,    34,   114,    -1,    35,   114,    -1,
      36,   114,    -1,    37,   114,    -1,    48,   114,    -1,    47,
     114,    -1,    46,   114,    -1,    26,    91,   114,    -1,    27,
      91,   114,    -1,    28,    91,   114,    -1,    30,   114,    -1,
      49,    50,   176,   114,    -1,    49,    50,   114,    -1,    49,
     176,   114,    -1,    49,   114,    -1,    52,    65,    65,   114,
      -1,    53,    65,    91,    91,   114,    -1,    54,    91,    91,
      91,   114,    -1,    56,    57,   114,    -1,    58,    59,   114,
      -1,    58,    59,    91,   114,    -1,    58,    60,   114,    -1,
      63,   114,    -1,    63,   176,   114,    -1,    63,   176,   176,
     114,    -1,    63,    51,   114,    -1,    63,    51,   176,   114,
      -1,    63,    51,   176,   176,   114,    -1,    63,    72,   114,
      -1,    63,    62,   124,   114,    -1,    63,    73,   115,    91,
     114,    -1,    64,    66,   114,    -1,    64,    97,   114,    -1,
      67,    91,   114,    -1,    68,    91,    91,   114,    -1,    93,
      30,   114,    -1,    93,    17,   114,    -1,    93,    18,   114,
      -1,    93,    19,   114,    -1,    93,    23,   114,    -1,    93,
      24,   114,    -1,    93,    25,   114,    -1,    93,    26,   114,
      -1,    93,    27,   114,    -1,    93,    28,   114,    -1,    93,
      22,   114,    -1,    93,    77,   114,    -1,    93,    78,   114,
      -1,    93,    68,   114,    -1,    93,    69,   114,    -1,    93,
      70,   114,    -1,    93,    71,   114,    -1,    93,    52,   114,
      -1,    93,    74,   114,    -1,    93,    76,   114,    -1,    93,
      75,   114,    -1,    93,    79,   114,    -1,    93,    85,   114,
      -1,    93,    87,   114,    -1,    93,   125,   114,    -1,    93,
      34,   114,    -1,    93,    35,   114,    -1,    93,    36,   114,
      -1,    93,    37,   114,    -1,    93,    48,   114,    -1,    93,
      47,   114,    -1,    93,    46,   114,    -1,    93,    53,   114,
      -1,    93,    54,   114,    -1,    93,    63,   114,    -1,    93,
      80,   114,    -1,    93,    81,   114,    -1,    93,    49,   114,
      -1,    93,    64,   114,    -1,    93,    20,   114,    -1,    93,
      92,   114,    -1,    93,    29,   114,    -1,    93,    84,   114,
      -1,    93,    94,   114,    -1,    93,    93,   114,    -1,    93,
     114,    -1,    94,   176,   114,    -1,    91,    -1,    65,    -1,
       4,    -1,     3,    -1,     5,    -1,     6,    -1,     7,    -1,
     126,    -1,   101,    -1,   102,    -1,   103,    -1,   175,   104,
     175,    -1,   175,   105,   175,    -1,   175,   108,   175,    -1,
     175,   109,   175,    -1,   175,    99,   175,    -1,   175,   100,
     175,    -1,   175,   106,   175,    -1,   175,   107,   175,    -1,
     175,   110,   175,    -1,   117,   175,    -1,   105,   175,    -1,
     118,   175,   119,    -1,    91,    -1,    65,    -1,     4,    -1,
       3,    -1,     5,    -1,     6,    -1,     7,    -1,   126,    -1,
     101,    -1,   102,    -1,   103,    -1,   176,   116,   176,    -1,
     176,   104,   176,    -1,   176,   105,   176,    -1,   176,   108,
     176,    -1,   176,   109,   176,    -1,   176,    99,   176,    -1,
     176,   100,   176,    -1,   176,   106,   176,    -1,   176,   107,
     176,    -1,   176,   110,   176,    -1,   117,   176,    -1,   105,
     176,    -1,   108,   176,    -1,   120,   176,    -1,   118,   176,
     119,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   138,   138,   139,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   197,   198,   203,   204,   209,   210,   211,
     212,   213,   214,   219,   224,   232,   240,   248,   253,   258,
     263,   271,   279,   287,   295,   303,   311,   319,   324,   332,
     337,   342,   347,   352,   357,   362,   367,   372,   377,   382,
     390,   395,   400,   405,   413,   421,   429,   437,   442,   447,
     452,   460,   468,   473,   478,   482,   486,   490,   494,   498,
     502,   506,   513,   518,   523,   528,   533,   538,   543,   548,
     556,   564,   569,   577,   582,   587,   592,   597,   602,   607,
     612,   617,   622,   627,   632,   637,   642,   647,   655,   656,
     659,   667,   675,   683,   691,   699,   707,   715,   723,   731,
     738,   746,   754,   759,   764,   769,   777,   785,   793,   801,
     809,   814,   819,   827,   832,   837,   842,   847,   852,   857,
     862,   867,   875,   880,   888,   896,   904,   909,   914,   921,
     926,   931,   936,   941,   946,   951,   956,   961,   966,   971,
     976,   982,   988,   994,  1002,  1007,  1012,  1017,  1022,  1027,
    1032,  1037,  1042,  1047,  1052,  1057,  1062,  1067,  1072,  1077,
    1082,  1092,  1103,  1109,  1122,  1127,  1138,  1143,  1158,  1170,
    1180,  1185,  1193,  1211,  1212,  1213,  1214,  1215,  1216,  1217,
    1218,  1219,  1220,  1221,  1222,  1223,  1224,  1225,  1226,  1227,
    1228,  1229,  1230,  1231,  1232,  1233,  1239,  1240,  1241,  1242,
    1243,  1244,  1245,  1246,  1247,  1248,  1249,  1250,  1251,  1252,
    1253,  1254,  1255,  1256,  1257,  1258,  1259,  1260,  1261,  1262,
    1263,  1264
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
  "BX_TOKEN_STEP_OVER", "BX_TOKEN_SET", "BX_TOKEN_DEBUGGER",
  "BX_TOKEN_LIST_BREAK", "BX_TOKEN_VBREAKPOINT", "BX_TOKEN_LBREAKPOINT",
  "BX_TOKEN_PBREAKPOINT", "BX_TOKEN_DEL_BREAKPOINT",
  "BX_TOKEN_ENABLE_BREAKPOINT", "BX_TOKEN_DISABLE_BREAKPOINT",
  "BX_TOKEN_INFO", "BX_TOKEN_QUIT", "BX_TOKEN_R", "BX_TOKEN_REGS",
  "BX_TOKEN_CPU", "BX_TOKEN_FPU", "BX_TOKEN_MMX", "BX_TOKEN_SSE",
  "BX_TOKEN_AVX", "BX_TOKEN_IDT", "BX_TOKEN_IVT", "BX_TOKEN_GDT",
  "BX_TOKEN_LDT", "BX_TOKEN_TSS", "BX_TOKEN_TAB", "BX_TOKEN_ALL",
  "BX_TOKEN_LINUX", "BX_TOKEN_DEBUG_REGS", "BX_TOKEN_CONTROL_REGS",
  "BX_TOKEN_SEGMENT_REGS", "BX_TOKEN_EXAMINE", "BX_TOKEN_XFORMAT",
  "BX_TOKEN_DISFORMAT", "BX_TOKEN_RESTORE", "BX_TOKEN_WRITEMEM",
  "BX_TOKEN_SETPMEM", "BX_TOKEN_SYMBOLNAME", "BX_TOKEN_QUERY",
  "BX_TOKEN_PENDING", "BX_TOKEN_TAKE", "BX_TOKEN_DMA", "BX_TOKEN_IRQ",
  "BX_TOKEN_TLB", "BX_TOKEN_HEX", "BX_TOKEN_DISASM", "BX_TOKEN_INSTRUMENT",
  "BX_TOKEN_STRING", "BX_TOKEN_STOP", "BX_TOKEN_DOIT", "BX_TOKEN_CRC",
  "BX_TOKEN_TRACE", "BX_TOKEN_TRACEREG", "BX_TOKEN_TRACEMEM",
  "BX_TOKEN_SWITCH_MODE", "BX_TOKEN_SIZE", "BX_TOKEN_PTIME",
  "BX_TOKEN_TIMEBP_ABSOLUTE", "BX_TOKEN_TIMEBP", "BX_TOKEN_MODEBP",
  "BX_TOKEN_VMEXITBP", "BX_TOKEN_PRINT_STACK", "BX_TOKEN_WATCH",
  "BX_TOKEN_UNWATCH", "BX_TOKEN_READ", "BX_TOKEN_WRITE", "BX_TOKEN_SHOW",
  "BX_TOKEN_LOAD_SYMBOLS", "BX_TOKEN_SYMBOLS", "BX_TOKEN_LIST_SYMBOLS",
  "BX_TOKEN_GLOBAL", "BX_TOKEN_WHERE", "BX_TOKEN_PRINT_STRING",
  "BX_TOKEN_NUMERIC", "BX_TOKEN_PAGE", "BX_TOKEN_HELP", "BX_TOKEN_CALC",
  "BX_TOKEN_VGA", "BX_TOKEN_DEVICE", "BX_TOKEN_COMMAND",
  "BX_TOKEN_GENERIC", "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT",
  "BX_TOKEN_REG_IP", "BX_TOKEN_REG_EIP", "BX_TOKEN_REG_RIP", "'+'", "'-'",
  "'|'", "'^'", "'*'", "'/'", "'&'", "INDIRECT", "NEG", "NOT", "'\\n'",
  "'='", "':'", "'!'", "'('", "')'", "'@'", "$accept", "commands",
  "command", "BX_TOKEN_TOGGLE_ON_OFF", "BX_TOKEN_REGISTERS",
  "BX_TOKEN_SEGREG", "timebp_command", "modebp_command",
  "vmexitbp_command", "show_command", "page_command", "tlb_command",
  "ptime_command", "trace_command", "trace_reg_command",
  "trace_mem_command", "print_stack_command", "watch_point_command",
  "symbol_command", "where_command", "print_string_command",
  "continue_command", "stepN_command", "step_over_command", "set_command",
  "breakpoint_command", "blist_command", "slist_command", "info_command",
  "optional_numeric", "regs_command", "fpu_regs_command",
  "mmx_regs_command", "sse_regs_command", "avx_regs_command",
  "segment_regs_command", "control_regs_command", "debug_regs_command",
  "delete_command", "bpe_command", "bpd_command", "quit_command",
  "examine_command", "restore_command", "writemem_command",
  "setpmem_command", "query_command", "take_command",
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
     355,   356,   357,   358,    43,    45,   124,    94,    42,    47,
      38,   359,   360,   361,    10,    61,    58,    33,    40,    41,
      64
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   121,   122,   122,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   124,   124,   125,   125,   126,   126,   126,
     126,   126,   126,   127,   127,   128,   129,   130,   130,   130,
     130,   131,   132,   133,   134,   135,   136,   137,   137,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     139,   139,   139,   139,   140,   141,   142,   143,   143,   143,
     143,   144,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   146,   146,   146,   146,   146,   146,   146,   146,
     147,   148,   148,   149,   149,   149,   149,   149,   149,   149,
     149,   149,   149,   149,   149,   149,   149,   149,   150,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   163,   163,   163,   164,   165,   166,   167,
     168,   168,   168,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   170,   170,   171,   172,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   174,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     2,     2,     3,     3,     3,
       2,     3,     3,     2,     3,     3,     3,     2,     3,     3,
       3,     2,     4,     4,     4,     5,     5,     5,     2,     3,
       3,     4,     4,     5,     2,     3,     2,     2,     3,     4,
       4,     2,     4,     5,     5,     5,     5,     5,     5,     5,
       5,     5,     2,     5,     2,     3,     3,     2,     3,     4,
       2,     2,     3,     3,     3,     5,     5,     5,     5,     3,
       3,     3,     3,     3,     4,     3,     4,     5,     0,     1,
       2,     2,     2,     2,     2,     2,     2,     2,     3,     3,
       3,     2,     4,     3,     3,     2,     4,     5,     5,     3,
       3,     4,     3,     2,     3,     4,     3,     4,     5,     3,
       4,     5,     3,     3,     3,     4,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     2,
       2,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      51,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    55,    56,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    52,     0,     3,     0,    38,    39,    40,
      45,    43,    44,    37,    34,    35,    36,    41,    42,    46,
      47,    48,     4,     5,     6,     7,     8,    18,    19,     9,
      10,    11,    12,    13,    14,    15,    17,    16,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    49,    50,    96,     0,     0,    97,   101,     0,
       0,     0,     0,     0,    57,    58,    59,    60,    61,    62,
       0,     0,     0,     0,     0,   120,   226,   225,   227,   228,
     229,   224,   223,   231,   232,   233,     0,   112,     0,     0,
     230,     0,   249,   248,   250,   251,   252,   247,   246,   254,
     255,   256,     0,     0,   114,     0,     0,     0,   253,     0,
     247,     0,   117,     0,     0,     0,     0,     0,     0,     0,
     138,   138,   138,   138,     0,     0,     0,     0,     0,   151,
     141,   142,   143,   144,   147,   146,   145,     0,   155,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   163,     0,     0,     0,     0,     0,    53,    54,     0,
       0,     0,    73,     0,     0,    65,    66,     0,    77,     0,
       0,     0,     0,     0,    81,    88,     0,     0,     0,     0,
      70,     0,     0,     0,   121,    94,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   221,     0,     0,     1,     2,   140,     0,     0,
      98,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   244,   243,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   116,   268,   269,   267,     0,   270,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   115,
       0,   269,   118,   148,   149,   150,   131,   123,   124,   139,
     138,   138,   138,   138,   130,   129,   132,     0,   133,     0,
     135,   153,     0,   154,     0,     0,     0,   159,     0,   160,
     162,    72,   166,     0,     0,   169,     0,     0,     0,   164,
       0,   172,   173,   174,     0,    74,    75,    76,    64,    63,
      78,    80,     0,    79,     0,     0,    89,    68,    69,    67,
       0,    90,     0,   122,    95,    71,   177,   178,   179,   215,
     186,   180,   181,   182,   183,   184,   185,   217,   176,   201,
     202,   203,   204,   207,   206,   205,   213,   193,   208,   209,
     210,   214,   189,   190,   191,   192,   194,   196,   195,   187,
     188,   197,   211,   212,   218,   198,   199,   216,   220,   219,
     200,   222,    99,   100,     0,     0,     0,     0,     0,     0,
     102,     0,     0,     0,   245,   238,   239,   234,   235,   240,
     241,   236,   237,   242,     0,   271,   262,   263,   258,   259,
     264,   265,   260,   261,   266,   257,   119,     0,     0,     0,
       0,   134,     0,   136,   152,   156,     0,     0,   161,   167,
       0,   170,     0,   259,   260,   165,   175,    82,     0,    83,
       0,    84,     0,    91,     0,    92,   105,   104,   106,   107,
     108,   103,   109,   110,   111,   113,   125,   126,   127,   128,
     137,   157,   158,   168,   171,    85,    86,    87,    93
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    54,    55,   209,    56,   158,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,   340,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   141,   159
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -168
static const yytype_int16 yypact[] =
{
    1146,  -104,   -27,  -101,  1094,   -86,   880,   644,   691,   -50,
     -48,   -46,   927,   -76,  -168,  -168,   -64,   -40,   -32,   -26,
     -25,   -23,   -22,    49,     1,    21,     6,    37,    13,   246,
     573,   -57,     7,    10,    16,    16,    16,     4,    24,    40,
      18,    20,   -80,     2,   711,   -14,   -53,   -51,    23,    47,
     246,  1227,   246,  -168,  1002,  -168,    25,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,    50,   -68,  -168,  -168,    27,
      28,    31,    41,    43,  -168,  -168,  -168,  -168,  -168,  -168,
      44,    16,    45,    46,    53,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,   898,  -168,   898,   898,
    -168,  1303,  -168,  -168,  -168,  -168,  -168,    48,  -168,  -168,
    -168,  -168,   246,   246,  -168,   246,   246,   246,  -168,     3,
    -168,   246,  -168,   236,    51,    57,    58,    63,    64,    66,
      73,    73,    73,    73,    67,    69,    70,   -49,   -47,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,   762,  -168,   298,
      90,    94,    95,    74,   -67,    76,   361,   829,    16,    77,
      72,  -168,   117,    78,    79,    91,   115,  -168,  -168,    93,
      96,    97,  -168,    99,   114,  -168,  -168,   118,  -168,   122,
     246,   127,   246,   246,  -168,  -168,   424,   128,   129,   131,
    -168,   -66,   144,   133,  -168,  -168,   134,   488,   148,   149,
     150,   158,   159,   160,   162,   164,   165,   167,   168,   174,
     175,   179,   180,   181,   182,   183,   184,   185,   186,   207,
     220,   224,   225,   239,   242,   243,   244,   251,   253,   254,
     255,   259,   261,   265,   266,   267,   268,   269,   270,   285,
     286,   287,  -168,   295,   563,  -168,  -168,  -168,   296,   299,
    -168,   246,   246,   246,   246,   246,   153,   301,   246,   246,
     246,  -168,  -168,   -29,   898,   898,   898,   898,   898,   898,
     898,   898,   898,   898,  -168,   300,   300,   300,   161,   300,
     246,   246,   246,   246,   246,   246,   246,   246,   246,  -168,
     246,   -77,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
      73,    73,    73,    73,  -168,  -168,  -168,   303,  -168,   -45,
    -168,  -168,   634,  -168,   304,   340,   341,  -168,   319,  -168,
    -168,  -168,  -168,   320,   321,  -168,   348,   246,   246,  -168,
     681,  -168,  -168,  -168,   327,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,   383,  -168,   446,   509,  -168,  -168,  -168,  -168,
     328,  -168,   -65,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,   752,   819,  1013,  1026,  1046,   329,
    -168,  1178,  1218,  1238,  -168,  -168,  -168,    -4,    -4,    -4,
      -4,  -168,  -168,  -168,  1329,  -168,   300,   300,   130,   130,
     130,   130,   300,   300,   300,  1316,  -168,   330,   331,   332,
     333,  -168,   349,  -168,  -168,  -168,   350,   358,  -168,  -168,
    1251,  -168,   359,   262,   300,  -168,  -168,  -168,  1264,  -168,
    1277,  -168,  1290,  -168,   362,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -168,  -168,   221,   -28,   411,    -3,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -167,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -109,    -8
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -269
static const yytype_int16 yytable[] =
{
     163,   124,   227,   140,   341,   342,   343,   210,   211,   203,
     104,   217,   231,   108,   233,   189,   347,   105,   349,   219,
     482,   196,   202,   289,   358,   390,   504,   301,   125,   302,
     303,   207,   208,   220,   218,   232,   226,   476,   179,   330,
     204,   164,   237,   165,   284,   166,   290,   359,   391,   505,
     180,   228,   142,   143,   144,   145,   146,   114,   115,   116,
     117,   118,   119,   234,   106,   348,   190,   350,   221,   483,
     304,   305,   194,   195,   181,   306,   307,   308,   309,   310,
     311,   312,   182,   229,   222,   223,   191,   107,   183,   184,
     454,   185,   186,   297,   193,   304,   305,   192,   205,   187,
     230,   206,   320,   321,   310,   311,   312,   322,   323,   324,
     325,   326,   327,   328,   160,   213,   224,   329,   212,   330,
     142,   143,   144,   145,   146,   114,   115,   116,   117,   118,
     119,   214,   215,   140,   216,   140,   140,   235,   236,   287,
     148,   288,   291,   292,   315,   316,   293,   317,   318,   319,
     149,   150,   151,   331,   152,   354,   294,   153,   295,   296,
     298,   299,   314,   188,   339,   333,   155,   156,   300,   157,
     364,   334,   335,   477,   478,   479,   480,   336,   337,   352,
     338,   344,   160,   345,   346,   355,   356,   366,   357,   363,
     360,   365,   371,   372,   370,   455,   456,   457,   458,   459,
     460,   461,   462,   463,   464,   373,   374,   375,   148,   392,
     376,   377,   382,   378,   384,   385,   320,   321,   149,   150,
     151,   322,   367,   324,   325,   368,   327,   328,   379,   320,
     321,   369,   380,   330,   155,   156,   381,   157,   326,   327,
     328,   383,   387,   388,   449,   389,   330,   393,   394,   142,
     143,   144,   145,   146,   114,   115,   116,   117,   118,   119,
     320,   321,   396,   397,   398,   322,   323,   324,   325,   326,
     327,   328,   399,   400,   401,   286,   402,   330,   403,   404,
     465,   405,   406,   444,   445,   446,   447,   448,   407,   408,
     451,   452,   453,   409,   410,   411,   412,   413,   414,   415,
     416,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   160,   466,   467,   468,   469,   470,   471,   472,   473,
     474,   417,   475,   142,   143,   144,   145,   146,   114,   115,
     116,   117,   118,   119,   418,   320,   321,   148,   419,   420,
     322,   323,   324,   325,   326,   327,   328,   149,   150,   151,
     332,   152,   330,   421,   153,   490,   422,   423,   424,   493,
     494,  -268,  -268,   155,   156,   425,   157,   426,   427,   428,
    -268,  -268,  -268,   429,   498,   430,   500,   502,   330,   431,
     432,   433,   434,   435,   436,   160,   142,   143,   144,   145,
     146,   114,   115,   116,   117,   118,   119,   320,   321,   437,
     438,   439,   322,   323,   324,   325,   326,   327,   328,   440,
     442,   148,   353,   443,   330,   450,   330,   481,   485,   320,
     321,   149,   150,   151,   322,   367,   324,   325,   368,   327,
     328,   486,   487,   488,   489,   491,   330,   155,   156,   492,
     157,   496,   503,   511,   516,   517,   518,   519,   160,   142,
     143,   144,   145,   146,   114,   115,   116,   117,   118,   119,
     320,   321,   283,   520,   521,   322,   323,   324,   325,   326,
     327,   328,   522,   524,   148,   361,   528,   330,     0,     0,
       0,     0,   320,   321,   149,   150,   151,   322,   367,   324,
     325,   368,   327,   328,     0,     0,     0,   497,     0,   330,
     155,   156,     0,   157,     0,     0,     0,     0,     0,     0,
       0,   160,   142,   143,   144,   145,   146,   114,   115,   116,
     117,   118,   119,   320,   321,     0,     0,     0,   322,   323,
     324,   325,   326,   327,   328,     0,     0,   148,   386,     0,
     330,     0,     0,     0,     0,   320,   321,   149,   150,   151,
     322,   367,   324,   325,   368,   327,   328,     0,     0,     0,
     499,     0,   330,   155,   156,     0,   157,     0,     0,     0,
       0,     0,     0,     0,   160,     0,   142,   143,   144,   145,
     146,   114,   115,   116,   117,   118,   119,   320,   321,     0,
       0,     0,   322,   323,   324,   325,   326,   327,   328,     0,
     148,     0,   395,     0,   330,     0,     0,     0,   320,   321,
     149,   150,   151,   322,   367,   324,   325,   368,   327,   328,
       0,     0,     0,   501,   197,   330,   155,   156,     0,   157,
       0,     0,     0,     0,     0,   198,     0,     0,   160,     0,
       0,     0,     0,     0,     0,   199,   200,   142,   143,   144,
     145,   146,   114,   115,   116,   117,   118,   119,     0,     0,
       0,     0,   320,   321,   148,     0,     0,   322,   323,   324,
     325,   326,   327,   328,   149,   150,   151,   441,   152,   330,
       0,   153,     0,     0,     0,     0,     0,   201,     0,     0,
     155,   156,     0,   157,   142,   143,   144,   145,   146,   114,
     115,   116,   117,   118,   119,     0,     0,     0,     0,   147,
       0,     0,     0,     0,   142,   143,   144,   145,   146,   114,
     115,   116,   117,   118,   119,     0,     0,     0,     0,     0,
       0,     0,     0,   320,   321,   148,     0,     0,   322,   323,
     324,   325,   326,   327,   328,   149,   150,   151,   484,   152,
     330,     0,   153,     0,     0,     0,   160,     0,   154,     0,
       0,   155,   156,     0,   157,   142,   143,   144,   145,   146,
     114,   115,   116,   117,   118,   119,   160,     0,     0,     0,
     320,   321,   148,     0,     0,   322,   323,   324,   325,   326,
     327,   328,   149,   150,   151,   495,   152,   330,     0,   161,
       0,     0,   148,     0,     0,   162,     0,     0,   155,   156,
       0,   157,   149,   150,   151,     0,   152,     0,     0,   153,
       0,     0,     0,     0,     0,   225,     0,   160,   155,   156,
       0,   157,   142,   143,   144,   145,   146,   114,   115,   116,
     117,   118,   119,     0,     0,     0,     0,     0,     0,     0,
       0,   320,   321,   148,     0,     0,   322,   323,   324,   325,
     326,   327,   328,   149,   150,   151,   506,   152,   330,     0,
     153,     0,     0,     0,     0,     0,   351,     0,     0,   155,
     156,     0,   157,   126,   127,   128,   129,   130,   114,   115,
     116,   117,   118,   119,   160,     0,     0,     0,     0,     0,
       0,   126,   127,   128,   129,   130,   114,   115,   116,   117,
     118,   119,     0,     0,     0,     0,     0,     0,   320,   321,
     148,     0,     0,   322,   323,   324,   325,   326,   327,   328,
     149,   150,   151,   507,   152,   330,     0,   153,     0,     0,
       0,   167,     0,   362,     0,   131,   155,   156,     0,   157,
       0,     0,   168,     0,     0,     0,     0,     0,     0,     0,
     169,     0,     0,   131,     0,   170,   171,   172,   173,   174,
     175,   132,   176,     0,     0,     0,     0,     0,     0,     0,
       0,   133,   134,   135,     0,   136,     0,     0,     0,   132,
       0,     0,     0,     0,   137,     0,     0,   138,   139,   133,
     134,   135,   285,   136,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   177,     0,   138,   139,     0,     0,     1,
       2,     3,     4,   178,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,     0,    16,    17,    18,    19,
       0,     0,     0,     0,     0,     0,     0,     0,    20,    21,
      22,    23,     0,     0,    24,    25,    26,     0,    27,     0,
      28,     0,     0,    29,     0,    30,    31,     0,     0,    32,
      33,    34,    35,    36,     0,     0,    37,    38,    39,    40,
      41,    42,    43,    44,     0,     0,    45,    46,     0,    47,
       0,    48,    49,     0,    50,    51,    52,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,     0,     0,
       0,     0,   320,   321,     0,     0,    53,   322,   323,   324,
     325,   326,   327,   328,     0,   320,   321,   508,     0,   330,
     322,   323,   324,   325,   326,   327,   328,     0,     0,     0,
     509,     0,   330,     0,     0,   320,   321,     0,     0,   120,
     322,   323,   324,   325,   326,   327,   328,   121,     0,     0,
     510,     0,   330,     1,     2,     3,     4,     0,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,     0,
      16,    17,    18,    19,     0,     0,     0,     0,     0,     0,
       0,     0,    20,    21,    22,    23,   122,   123,    24,    25,
      26,     0,    27,     0,    28,     0,     0,    29,     0,    30,
      31,     0,     0,    32,    33,    34,    35,    36,     0,     0,
      37,    38,    39,    40,    41,    42,    43,    44,     0,     0,
      45,    46,     0,    47,     0,    48,    49,     0,    50,    51,
      52,     0,     0,     0,   238,   239,   240,   241,     0,   242,
     243,   244,   245,   246,   247,   248,   249,   250,    14,    15,
      53,   251,   252,   253,   254,     0,     0,     0,     0,     0,
       0,     0,     0,   255,   256,   257,   258,   320,   321,   259,
     260,   261,   322,   323,   324,   325,   326,   327,   328,     0,
     262,   263,   512,     0,   330,   264,   265,   266,   267,     0,
       0,   268,   269,   270,   271,   272,   273,   274,   275,     0,
       0,   276,   277,     0,   278,     0,     0,   320,   321,   279,
     280,   281,   322,   323,   324,   325,   326,   327,   328,     0,
       0,     0,   513,     0,   330,     0,     0,   320,   321,     0,
       0,   282,   322,   323,   324,   325,   326,   327,   328,     0,
     320,   321,   514,     0,   330,   322,   323,   324,   325,   326,
     327,   328,     0,   320,   321,   523,     0,   330,   322,   323,
     324,   325,   326,   327,   328,     0,   320,   321,   525,     0,
     330,   322,   323,   324,   325,   326,   327,   328,     0,   320,
     321,   526,     0,   330,   322,   323,   324,   325,   326,   327,
     328,     0,   304,   305,   527,     0,   330,   306,   307,   308,
     309,   310,   311,   312,     0,   320,   321,     0,     0,   313,
     322,   323,   324,   325,   326,   327,   328,     0,   304,   305,
       0,     0,   330,   306,   307,   308,   309,   310,   311,   312,
       0,     0,     0,   515
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-168))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       8,     4,    16,     6,   171,   172,   173,    35,    36,    66,
     114,    91,    65,   114,    65,    23,    65,    44,    65,    17,
      65,    29,    30,    91,    91,    91,    91,   136,   114,   138,
     139,    15,    16,    31,   114,    88,    44,   114,   114,   116,
      97,    91,    50,    91,    52,    91,   114,   114,   114,   114,
     114,    65,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,   114,    91,   114,    65,   114,    66,   114,
      99,   100,    59,    60,   114,   104,   105,   106,   107,   108,
     109,   110,   114,    97,    82,    83,    65,   114,   114,   114,
     119,   114,   114,   121,    57,    99,   100,    91,    91,    50,
     114,    91,    99,   100,   108,   109,   110,   104,   105,   106,
     107,   108,   109,   110,    65,    91,   114,   114,   114,   116,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    91,   114,   136,   114,   138,   139,   114,    91,   114,
      91,    91,   115,   115,   152,   153,   115,   155,   156,   157,
     101,   102,   103,   161,   105,    65,   115,   108,   115,   115,
     115,   115,   114,   114,    91,   114,   117,   118,   115,   120,
     198,   114,   114,   340,   341,   342,   343,   114,   114,   187,
     114,   114,    65,   114,   114,    91,    91,   115,   114,   197,
     114,   114,   114,   114,   202,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   114,    91,   114,    91,    65,
     114,   114,   220,   114,   222,   223,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   114,    99,
     100,   114,   114,   116,   117,   118,   114,   120,   108,   109,
     110,   114,   114,   114,    91,   114,   116,   114,   114,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      99,   100,   114,   114,   114,   104,   105,   106,   107,   108,
     109,   110,   114,   114,   114,    54,   114,   116,   114,   114,
     119,   114,   114,   291,   292,   293,   294,   295,   114,   114,
     298,   299,   300,   114,   114,   114,   114,   114,   114,   114,
     114,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,    65,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   114,   330,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,   114,    99,   100,    91,   114,   114,
     104,   105,   106,   107,   108,   109,   110,   101,   102,   103,
     114,   105,   116,   114,   108,   363,   114,   114,   114,   367,
     368,    99,   100,   117,   118,   114,   120,   114,   114,   114,
     108,   109,   110,   114,   382,   114,   384,   385,   116,   114,
     114,   114,   114,   114,   114,    65,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    99,   100,   114,
     114,   114,   104,   105,   106,   107,   108,   109,   110,   114,
     114,    91,   114,   114,   116,   114,   116,   114,   114,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,    91,    91,   114,   114,   114,   116,   117,   118,    91,
     120,   114,   114,   114,   114,   114,   114,   114,    65,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      99,   100,    51,   114,   114,   104,   105,   106,   107,   108,
     109,   110,   114,   114,    91,   114,   114,   116,    -1,    -1,
      -1,    -1,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,    -1,    -1,    -1,   114,    -1,   116,
     117,   118,    -1,   120,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    65,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    99,   100,    -1,    -1,    -1,   104,   105,
     106,   107,   108,   109,   110,    -1,    -1,    91,   114,    -1,
     116,    -1,    -1,    -1,    -1,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
     114,    -1,   116,   117,   118,    -1,   120,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    65,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    99,   100,    -1,
      -1,    -1,   104,   105,   106,   107,   108,   109,   110,    -1,
      91,    -1,   114,    -1,   116,    -1,    -1,    -1,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
      -1,    -1,    -1,   114,    51,   116,   117,   118,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    62,    -1,    -1,    65,    -1,
      -1,    -1,    -1,    -1,    -1,    72,    73,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    99,   100,    91,    -1,    -1,   104,   105,   106,
     107,   108,   109,   110,   101,   102,   103,   114,   105,   116,
      -1,   108,    -1,    -1,    -1,    -1,    -1,   114,    -1,    -1,
     117,   118,    -1,   120,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    -1,    -1,    -1,    -1,    65,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    99,   100,    91,    -1,    -1,   104,   105,
     106,   107,   108,   109,   110,   101,   102,   103,   114,   105,
     116,    -1,   108,    -1,    -1,    -1,    65,    -1,   114,    -1,
      -1,   117,   118,    -1,   120,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    65,    -1,    -1,    -1,
      99,   100,    91,    -1,    -1,   104,   105,   106,   107,   108,
     109,   110,   101,   102,   103,   114,   105,   116,    -1,   108,
      -1,    -1,    91,    -1,    -1,   114,    -1,    -1,   117,   118,
      -1,   120,   101,   102,   103,    -1,   105,    -1,    -1,   108,
      -1,    -1,    -1,    -1,    -1,   114,    -1,    65,   117,   118,
      -1,   120,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    99,   100,    91,    -1,    -1,   104,   105,   106,   107,
     108,   109,   110,   101,   102,   103,   114,   105,   116,    -1,
     108,    -1,    -1,    -1,    -1,    -1,   114,    -1,    -1,   117,
     118,    -1,   120,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    65,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    -1,    -1,    -1,    -1,    -1,    -1,    99,   100,
      91,    -1,    -1,   104,   105,   106,   107,   108,   109,   110,
     101,   102,   103,   114,   105,   116,    -1,   108,    -1,    -1,
      -1,    14,    -1,   114,    -1,    65,   117,   118,    -1,   120,
      -1,    -1,    25,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    -1,    -1,    65,    -1,    38,    39,    40,    41,    42,
      43,    91,    45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   101,   102,   103,    -1,   105,    -1,    -1,    -1,    91,
      -1,    -1,    -1,    -1,   114,    -1,    -1,   117,   118,   101,
     102,   103,     0,   105,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,   117,   118,    -1,    -1,    17,
      18,    19,    20,    96,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    34,    35,    36,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    47,
      48,    49,    -1,    -1,    52,    53,    54,    -1,    56,    -1,
      58,    -1,    -1,    61,    -1,    63,    64,    -1,    -1,    67,
      68,    69,    70,    71,    -1,    -1,    74,    75,    76,    77,
      78,    79,    80,    81,    -1,    -1,    84,    85,    -1,    87,
      -1,    89,    90,    -1,    92,    93,    94,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    99,   100,    -1,    -1,   114,   104,   105,   106,
     107,   108,   109,   110,    -1,    99,   100,   114,    -1,   116,
     104,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
     114,    -1,   116,    -1,    -1,    99,   100,    -1,    -1,    55,
     104,   105,   106,   107,   108,   109,   110,    63,    -1,    -1,
     114,    -1,   116,    17,    18,    19,    20,    -1,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      34,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    47,    48,    49,   102,   103,    52,    53,
      54,    -1,    56,    -1,    58,    -1,    -1,    61,    -1,    63,
      64,    -1,    -1,    67,    68,    69,    70,    71,    -1,    -1,
      74,    75,    76,    77,    78,    79,    80,    81,    -1,    -1,
      84,    85,    -1,    87,    -1,    89,    90,    -1,    92,    93,
      94,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
     114,    34,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    47,    48,    49,    99,   100,    52,
      53,    54,   104,   105,   106,   107,   108,   109,   110,    -1,
      63,    64,   114,    -1,   116,    68,    69,    70,    71,    -1,
      -1,    74,    75,    76,    77,    78,    79,    80,    81,    -1,
      -1,    84,    85,    -1,    87,    -1,    -1,    99,   100,    92,
      93,    94,   104,   105,   106,   107,   108,   109,   110,    -1,
      -1,    -1,   114,    -1,   116,    -1,    -1,    99,   100,    -1,
      -1,   114,   104,   105,   106,   107,   108,   109,   110,    -1,
      99,   100,   114,    -1,   116,   104,   105,   106,   107,   108,
     109,   110,    -1,    99,   100,   114,    -1,   116,   104,   105,
     106,   107,   108,   109,   110,    -1,    99,   100,   114,    -1,
     116,   104,   105,   106,   107,   108,   109,   110,    -1,    99,
     100,   114,    -1,   116,   104,   105,   106,   107,   108,   109,
     110,    -1,    99,   100,   114,    -1,   116,   104,   105,   106,
     107,   108,   109,   110,    -1,    99,   100,    -1,    -1,   116,
     104,   105,   106,   107,   108,   109,   110,    -1,    99,   100,
      -1,    -1,   116,   104,   105,   106,   107,   108,   109,   110,
      -1,    -1,    -1,   114
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    17,    18,    19,    20,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    34,    35,    36,    37,
      46,    47,    48,    49,    52,    53,    54,    56,    58,    61,
      63,    64,    67,    68,    69,    70,    71,    74,    75,    76,
      77,    78,    79,    80,    81,    84,    85,    87,    89,    90,
      92,    93,    94,   114,   122,   123,   125,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   114,    44,    91,   114,   114,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      55,    63,   102,   103,   126,   114,     3,     4,     5,     6,
       7,    65,    91,   101,   102,   103,   105,   114,   117,   118,
     126,   175,     3,     4,     5,     6,     7,    65,    91,   101,
     102,   103,   105,   108,   114,   117,   118,   120,   126,   176,
      65,   108,   114,   176,    91,    91,    91,    14,    25,    33,
      38,    39,    40,    41,    42,    43,    45,    86,    96,   114,
     114,   114,   114,   114,   114,   114,   114,    50,   114,   176,
      65,    65,    91,    57,    59,    60,   176,    51,    62,    72,
      73,   114,   176,    66,    97,    91,    91,    15,    16,   124,
     124,   124,   114,    91,    91,   114,   114,    91,   114,    17,
      31,    66,    82,    83,   114,   114,   176,    16,    65,    97,
     114,    65,    88,    65,   114,   114,    91,   176,    17,    18,
      19,    20,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    34,    35,    36,    37,    46,    47,    48,    49,    52,
      53,    54,    63,    64,    68,    69,    70,    71,    74,    75,
      76,    77,    78,    79,    80,    81,    84,    85,    87,    92,
      93,    94,   114,   125,   176,     0,   123,   114,    91,    91,
     114,   115,   115,   115,   115,   115,   115,   124,   115,   115,
     115,   175,   175,   175,    99,   100,   104,   105,   106,   107,
     108,   109,   110,   116,   114,   176,   176,   176,   176,   176,
      99,   100,   104,   105,   106,   107,   108,   109,   110,   114,
     116,   176,   114,   114,   114,   114,   114,   114,   114,    91,
     150,   150,   150,   150,   114,   114,   114,    65,   114,    65,
     114,   114,   176,   114,    65,    91,    91,   114,    91,   114,
     114,   114,   114,   176,   124,   114,   115,   105,   108,   114,
     176,   114,   114,   114,    91,   114,   114,   114,   114,   114,
     114,   114,   176,   114,   176,   176,   114,   114,   114,   114,
      91,   114,    65,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   176,   176,   176,   176,   176,    91,
     114,   176,   176,   176,   119,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   119,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   114,   150,   150,   150,
     150,   114,    65,   114,   114,   114,    91,    91,   114,   114,
     176,   114,    91,   176,   176,   114,   114,   114,   176,   114,
     176,   114,   176,   114,    91,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114
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


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
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
  if (yypact_value_is_default (yyn))
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
      if (yytable_value_is_error (yyn))
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
        case 52:

/* Line 1806 of yacc.c  */
#line 192 "parser.y"
    {
      }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 199 "parser.y"
    { (yyval.bval)=(yyvsp[(1) - (1)].bval); }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 205 "parser.y"
    { (yyval.sval)=(yyvsp[(1) - (1)].sval); }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 215 "parser.y"
    { (yyval.uval)=(yyvsp[(1) - (1)].uval); }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 220 "parser.y"
    {
          bx_dbg_timebp_command(0, (yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 225 "parser.y"
    {
          bx_dbg_timebp_command(1, (yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 233 "parser.y"
    {
          bx_dbg_modebp_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 241 "parser.y"
    {
          bx_dbg_vmexitbp_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 249 "parser.y"
    {
          bx_dbg_show_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 254 "parser.y"
    {
          bx_dbg_show_command("off");
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 259 "parser.y"
    {
          bx_dbg_show_param_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 264 "parser.y"
    {
          bx_dbg_show_command(0);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 272 "parser.y"
    {
          bx_dbg_xlate_address((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 280 "parser.y"
    {
          bx_dbg_tlb_lookup((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 288 "parser.y"
    {
          bx_dbg_ptime_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 296 "parser.y"
    {
          bx_dbg_trace_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 304 "parser.y"
    {
          bx_dbg_trace_reg_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 312 "parser.y"
    {
          bx_dbg_trace_mem_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 320 "parser.y"
    {
          bx_dbg_print_stack_command(16);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 325 "parser.y"
    {
          bx_dbg_print_stack_command((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 333 "parser.y"
    {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 338 "parser.y"
    {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 343 "parser.y"
    {
          bx_dbg_print_watchpoints();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 348 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (4)].uval), 1); /* BX_READ */
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 353 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (4)].uval), 1); /* BX_READ */
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 358 "parser.y"
    {
          bx_dbg_watch(1, (yyvsp[(3) - (4)].uval), 1); /* BX_WRITE */
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 363 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval)); /* BX_READ */
          free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 368 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval)); /* BX_READ */
          free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 373 "parser.y"
    {
          bx_dbg_watch(1, (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval)); /* BX_WRITE */
          free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 378 "parser.y"
    {
          bx_dbg_unwatch_all();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 383 "parser.y"
    {
          bx_dbg_unwatch((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 391 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(2) - (3)].sval), 0, 0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 396 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(2) - (4)].sval), 0, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 401 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(3) - (4)].sval), 1, 0);
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 406 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(3) - (5)].sval), 1, (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval)); free((yyvsp[(3) - (5)].sval));
      }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 414 "parser.y"
    {
        bx_dbg_where_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 422 "parser.y"
    {
        bx_dbg_print_string_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 430 "parser.y"
    {
        bx_dbg_continue_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 438 "parser.y"
    {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 443 "parser.y"
    {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 448 "parser.y"
    {
        bx_dbg_stepN_command(-1, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 453 "parser.y"
    {
        bx_dbg_stepN_command((yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 461 "parser.y"
    {
        bx_dbg_step_over_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 469 "parser.y"
    {
        bx_dbg_set_auto_disassemble((yyvsp[(3) - (4)].bval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 474 "parser.y"
    {
        bx_dbg_set_symbol_command((yyvsp[(2) - (5)].sval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 479 "parser.y"
    { 
        bx_dbg_set_reg8l_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 483 "parser.y"
    { 
        bx_dbg_set_reg8h_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 487 "parser.y"
    { 
        bx_dbg_set_reg16_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 107:

/* Line 1806 of yacc.c  */
#line 491 "parser.y"
    { 
        bx_dbg_set_reg32_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 495 "parser.y"
    { 
        bx_dbg_set_reg64_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 499 "parser.y"
    { 
        bx_dbg_set_rip_value((yyvsp[(4) - (5)].uval));
      }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 503 "parser.y"
    { 
        bx_dbg_set_rip_value((yyvsp[(4) - (5)].uval));
      }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 507 "parser.y"
    { 
        bx_dbg_load_segreg((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 514 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 519 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval));
      }
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 524 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 529 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 116:

/* Line 1806 of yacc.c  */
#line 534 "parser.y"
    {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 117:

/* Line 1806 of yacc.c  */
#line 539 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 118:

/* Line 1806 of yacc.c  */
#line 544 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 119:

/* Line 1806 of yacc.c  */
#line 549 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 120:

/* Line 1806 of yacc.c  */
#line 557 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 121:

/* Line 1806 of yacc.c  */
#line 565 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 570 "parser.y"
    {
        bx_dbg_info_symbols_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 123:

/* Line 1806 of yacc.c  */
#line 578 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 583 "parser.y"
    {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 588 "parser.y"
    {
        bx_dbg_info_idt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 126:

/* Line 1806 of yacc.c  */
#line 593 "parser.y"
    {
        bx_dbg_info_ivt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 598 "parser.y"
    {
        bx_dbg_info_gdt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 603 "parser.y"
    {
        bx_dbg_info_ldt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 129:

/* Line 1806 of yacc.c  */
#line 608 "parser.y"
    {
        bx_dbg_dump_table();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 130:

/* Line 1806 of yacc.c  */
#line 613 "parser.y"
    {
        bx_dbg_info_tss_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 131:

/* Line 1806 of yacc.c  */
#line 618 "parser.y"
    {
        bx_dbg_info_flags();
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 623 "parser.y"
    {
        bx_dbg_info_linux_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 628 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 134:

/* Line 1806 of yacc.c  */
#line 633 "parser.y"
    {
        bx_dbg_info_symbols_command((yyvsp[(3) - (4)].sval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 135:

/* Line 1806 of yacc.c  */
#line 638 "parser.y"
    {
        bx_dbg_info_device("", "");
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 136:

/* Line 1806 of yacc.c  */
#line 643 "parser.y"
    {
        bx_dbg_info_device((yyvsp[(3) - (4)].sval), "");
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 137:

/* Line 1806 of yacc.c  */
#line 648 "parser.y"
    {
        bx_dbg_info_device((yyvsp[(3) - (5)].sval), (yyvsp[(4) - (5)].sval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 138:

/* Line 1806 of yacc.c  */
#line 655 "parser.y"
    { (yyval.uval) = EMPTY_ARG; }
    break;

  case 140:

/* Line 1806 of yacc.c  */
#line 660 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 141:

/* Line 1806 of yacc.c  */
#line 668 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 142:

/* Line 1806 of yacc.c  */
#line 676 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 143:

/* Line 1806 of yacc.c  */
#line 684 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 144:

/* Line 1806 of yacc.c  */
#line 692 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_AVX_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 145:

/* Line 1806 of yacc.c  */
#line 700 "parser.y"
    {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 146:

/* Line 1806 of yacc.c  */
#line 708 "parser.y"
    {
        bx_dbg_info_control_regs_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 147:

/* Line 1806 of yacc.c  */
#line 716 "parser.y"
    {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 148:

/* Line 1806 of yacc.c  */
#line 724 "parser.y"
    {
        bx_dbg_del_breakpoint_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 149:

/* Line 1806 of yacc.c  */
#line 732 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[(2) - (3)].uval), 1);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 150:

/* Line 1806 of yacc.c  */
#line 739 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[(2) - (3)].uval), 0);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 151:

/* Line 1806 of yacc.c  */
#line 747 "parser.y"
    {
        bx_dbg_quit_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 152:

/* Line 1806 of yacc.c  */
#line 755 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (4)].sval), (yyvsp[(2) - (4)].sval),1, (yyvsp[(3) - (4)].uval), 1);
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 153:

/* Line 1806 of yacc.c  */
#line 760 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (3)].sval), (yyvsp[(2) - (3)].sval),1, 0, 0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 154:

/* Line 1806 of yacc.c  */
#line 765 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (3)].sval), NULL,0, (yyvsp[(2) - (3)].uval), 1);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 155:

/* Line 1806 of yacc.c  */
#line 770 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (2)].sval), NULL,0, 0, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 156:

/* Line 1806 of yacc.c  */
#line 778 "parser.y"
    {
        bx_dbg_restore_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].sval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 157:

/* Line 1806 of yacc.c  */
#line 786 "parser.y"
    {
        bx_dbg_writemem_command((yyvsp[(2) - (5)].sval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 158:

/* Line 1806 of yacc.c  */
#line 794 "parser.y"
    {
        bx_dbg_setpmem_command((yyvsp[(2) - (5)].uval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval));
      }
    break;

  case 159:

/* Line 1806 of yacc.c  */
#line 802 "parser.y"
    {
        bx_dbg_query_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 160:

/* Line 1806 of yacc.c  */
#line 810 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (3)].sval), 1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 161:

/* Line 1806 of yacc.c  */
#line 815 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 162:

/* Line 1806 of yacc.c  */
#line 820 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (3)].sval), 1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 163:

/* Line 1806 of yacc.c  */
#line 828 "parser.y"
    {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 164:

/* Line 1806 of yacc.c  */
#line 833 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[(2) - (3)].uval), (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 165:

/* Line 1806 of yacc.c  */
#line 838 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 166:

/* Line 1806 of yacc.c  */
#line 843 "parser.y"
    {
        bx_dbg_disassemble_current((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 167:

/* Line 1806 of yacc.c  */
#line 848 "parser.y"
    {
        bx_dbg_disassemble_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 168:

/* Line 1806 of yacc.c  */
#line 853 "parser.y"
    {
        bx_dbg_disassemble_command((yyvsp[(2) - (5)].sval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 169:

/* Line 1806 of yacc.c  */
#line 858 "parser.y"
    {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 170:

/* Line 1806 of yacc.c  */
#line 863 "parser.y"
    {
        bx_dbg_disassemble_hex_mode_switch((yyvsp[(3) - (4)].bval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 171:

/* Line 1806 of yacc.c  */
#line 868 "parser.y"
    {
        bx_dbg_set_disassemble_size((yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 172:

/* Line 1806 of yacc.c  */
#line 876 "parser.y"
    {
        bx_dbg_instrument_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 173:

/* Line 1806 of yacc.c  */
#line 881 "parser.y"
    {
        bx_dbg_instrument_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 174:

/* Line 1806 of yacc.c  */
#line 889 "parser.y"
    {
        bx_dbg_doit_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 175:

/* Line 1806 of yacc.c  */
#line 897 "parser.y"
    {
        bx_dbg_crc_command((yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 176:

/* Line 1806 of yacc.c  */
#line 905 "parser.y"
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 177:

/* Line 1806 of yacc.c  */
#line 910 "parser.y"
    {
         dbg_printf("c|cont|continue - continue executing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 178:

/* Line 1806 of yacc.c  */
#line 915 "parser.y"
    {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 179:

/* Line 1806 of yacc.c  */
#line 922 "parser.y"
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 180:

/* Line 1806 of yacc.c  */
#line 927 "parser.y"
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 181:

/* Line 1806 of yacc.c  */
#line 932 "parser.y"
    {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 182:

/* Line 1806 of yacc.c  */
#line 937 "parser.y"
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 183:

/* Line 1806 of yacc.c  */
#line 942 "parser.y"
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 184:

/* Line 1806 of yacc.c  */
#line 947 "parser.y"
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 185:

/* Line 1806 of yacc.c  */
#line 952 "parser.y"
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 186:

/* Line 1806 of yacc.c  */
#line 957 "parser.y"
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 187:

/* Line 1806 of yacc.c  */
#line 962 "parser.y"
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 188:

/* Line 1806 of yacc.c  */
#line 967 "parser.y"
    {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 189:

/* Line 1806 of yacc.c  */
#line 972 "parser.y"
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 190:

/* Line 1806 of yacc.c  */
#line 977 "parser.y"
    {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 191:

/* Line 1806 of yacc.c  */
#line 983 "parser.y"
    {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 192:

/* Line 1806 of yacc.c  */
#line 989 "parser.y"
    {
         dbg_printf("trace-mem on  - print all memory accesses occured during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 193:

/* Line 1806 of yacc.c  */
#line 995 "parser.y"
    {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 194:

/* Line 1806 of yacc.c  */
#line 1003 "parser.y"
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 195:

/* Line 1806 of yacc.c  */
#line 1008 "parser.y"
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 196:

/* Line 1806 of yacc.c  */
#line 1013 "parser.y"
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 197:

/* Line 1806 of yacc.c  */
#line 1018 "parser.y"
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 198:

/* Line 1806 of yacc.c  */
#line 1023 "parser.y"
    {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 199:

/* Line 1806 of yacc.c  */
#line 1028 "parser.y"
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 200:

/* Line 1806 of yacc.c  */
#line 1033 "parser.y"
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 201:

/* Line 1806 of yacc.c  */
#line 1038 "parser.y"
    {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 202:

/* Line 1806 of yacc.c  */
#line 1043 "parser.y"
    {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 203:

/* Line 1806 of yacc.c  */
#line 1048 "parser.y"
    {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 204:

/* Line 1806 of yacc.c  */
#line 1053 "parser.y"
    {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 205:

/* Line 1806 of yacc.c  */
#line 1058 "parser.y"
    {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 206:

/* Line 1806 of yacc.c  */
#line 1063 "parser.y"
    {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 207:

/* Line 1806 of yacc.c  */
#line 1068 "parser.y"
    {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 208:

/* Line 1806 of yacc.c  */
#line 1073 "parser.y"
    {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 209:

/* Line 1806 of yacc.c  */
#line 1078 "parser.y"
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 210:

/* Line 1806 of yacc.c  */
#line 1083 "parser.y"
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

  case 211:

/* Line 1806 of yacc.c  */
#line 1093 "parser.y"
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

  case 212:

/* Line 1806 of yacc.c  */
#line 1104 "parser.y"
    {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 213:

/* Line 1806 of yacc.c  */
#line 1110 "parser.y"
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

  case 214:

/* Line 1806 of yacc.c  */
#line 1123 "parser.y"
    {
         dbg_printf("instrument <command> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command>\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 215:

/* Line 1806 of yacc.c  */
#line 1128 "parser.y"
    {
         dbg_printf("set <regname> = <expr> - set register value to expression\n");
         dbg_printf("set eflags = <expr> - set eflags value to expression, not all flags can be modified\n");
         dbg_printf("set $cpu = <N> - move debugger control to cpu <N> in SMP simulation\n");
         dbg_printf("set $auto_disassemble = 1 - cause debugger to disassemble current instruction\n");
         dbg_printf("       every time execution stops\n");
         dbg_printf("set u|disasm|disassemble on  - same as 'set $auto_disassemble = 1'\n");
         dbg_printf("set u|disasm|disassemble off - same as 'set $auto_disassemble = 0'\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 216:

/* Line 1806 of yacc.c  */
#line 1139 "parser.y"
    {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 217:

/* Line 1806 of yacc.c  */
#line 1144 "parser.y"
    {
         dbg_printf("info break - show information about current breakpoint status\n");
         dbg_printf("info idt - show interrupt descriptor table\n");
         dbg_printf("info ivt - show interrupt vector table\n");
         dbg_printf("info gdt - show global descriptor table\n");
         dbg_printf("info tss - show current task state segment\n");
         dbg_printf("info tab - show page tables\n");
         dbg_printf("info eflags - show decoded EFLAGS register\n");
         dbg_printf("info symbols [string] - list symbols whose prefix is string\n");
         dbg_printf("info device - show list of devices supported by this command\n");
         dbg_printf("info device [string] - show state of device specified in string\n");
         dbg_printf("info device [string] [string] - show state of device with options\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 218:

/* Line 1806 of yacc.c  */
#line 1159 "parser.y"
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

  case 219:

/* Line 1806 of yacc.c  */
#line 1171 "parser.y"
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

  case 220:

/* Line 1806 of yacc.c  */
#line 1181 "parser.y"
    {
         bx_dbg_print_help();
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 221:

/* Line 1806 of yacc.c  */
#line 1186 "parser.y"
    {
         bx_dbg_print_help();
         free((yyvsp[(1) - (2)].sval));
       }
    break;

  case 222:

/* Line 1806 of yacc.c  */
#line 1194 "parser.y"
    {
     bx_dbg_calc_command((yyvsp[(2) - (3)].uval));
     free((yyvsp[(1) - (3)].sval));
   }
    break;

  case 223:

/* Line 1806 of yacc.c  */
#line 1211 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (1)].uval); }
    break;

  case 224:

/* Line 1806 of yacc.c  */
#line 1212 "parser.y"
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[(1) - (1)].sval)); free((yyvsp[(1) - (1)].sval));}
    break;

  case 225:

/* Line 1806 of yacc.c  */
#line 1213 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 226:

/* Line 1806 of yacc.c  */
#line 1214 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 227:

/* Line 1806 of yacc.c  */
#line 1215 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 228:

/* Line 1806 of yacc.c  */
#line 1216 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 229:

/* Line 1806 of yacc.c  */
#line 1217 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 230:

/* Line 1806 of yacc.c  */
#line 1218 "parser.y"
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 231:

/* Line 1806 of yacc.c  */
#line 1219 "parser.y"
    { (yyval.uval) = bx_dbg_get_ip (); }
    break;

  case 232:

/* Line 1806 of yacc.c  */
#line 1220 "parser.y"
    { (yyval.uval) = bx_dbg_get_eip(); }
    break;

  case 233:

/* Line 1806 of yacc.c  */
#line 1221 "parser.y"
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
    break;

  case 234:

/* Line 1806 of yacc.c  */
#line 1222 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) + (yyvsp[(3) - (3)].uval); }
    break;

  case 235:

/* Line 1806 of yacc.c  */
#line 1223 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) - (yyvsp[(3) - (3)].uval); }
    break;

  case 236:

/* Line 1806 of yacc.c  */
#line 1224 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) * (yyvsp[(3) - (3)].uval); }
    break;

  case 237:

/* Line 1806 of yacc.c  */
#line 1225 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) / (yyvsp[(3) - (3)].uval); }
    break;

  case 238:

/* Line 1806 of yacc.c  */
#line 1226 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) >> (yyvsp[(3) - (3)].uval); }
    break;

  case 239:

/* Line 1806 of yacc.c  */
#line 1227 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) << (yyvsp[(3) - (3)].uval); }
    break;

  case 240:

/* Line 1806 of yacc.c  */
#line 1228 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) | (yyvsp[(3) - (3)].uval); }
    break;

  case 241:

/* Line 1806 of yacc.c  */
#line 1229 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) ^ (yyvsp[(3) - (3)].uval); }
    break;

  case 242:

/* Line 1806 of yacc.c  */
#line 1230 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) & (yyvsp[(3) - (3)].uval); }
    break;

  case 243:

/* Line 1806 of yacc.c  */
#line 1231 "parser.y"
    { (yyval.uval) = !(yyvsp[(2) - (2)].uval); }
    break;

  case 244:

/* Line 1806 of yacc.c  */
#line 1232 "parser.y"
    { (yyval.uval) = -(yyvsp[(2) - (2)].uval); }
    break;

  case 245:

/* Line 1806 of yacc.c  */
#line 1233 "parser.y"
    { (yyval.uval) = (yyvsp[(2) - (3)].uval); }
    break;

  case 246:

/* Line 1806 of yacc.c  */
#line 1239 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (1)].uval); }
    break;

  case 247:

/* Line 1806 of yacc.c  */
#line 1240 "parser.y"
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[(1) - (1)].sval)); free((yyvsp[(1) - (1)].sval));}
    break;

  case 248:

/* Line 1806 of yacc.c  */
#line 1241 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 249:

/* Line 1806 of yacc.c  */
#line 1242 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 250:

/* Line 1806 of yacc.c  */
#line 1243 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 251:

/* Line 1806 of yacc.c  */
#line 1244 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 252:

/* Line 1806 of yacc.c  */
#line 1245 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 253:

/* Line 1806 of yacc.c  */
#line 1246 "parser.y"
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 254:

/* Line 1806 of yacc.c  */
#line 1247 "parser.y"
    { (yyval.uval) = bx_dbg_get_ip (); }
    break;

  case 255:

/* Line 1806 of yacc.c  */
#line 1248 "parser.y"
    { (yyval.uval) = bx_dbg_get_eip(); }
    break;

  case 256:

/* Line 1806 of yacc.c  */
#line 1249 "parser.y"
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
    break;

  case 257:

/* Line 1806 of yacc.c  */
#line 1250 "parser.y"
    { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[(1) - (3)].uval), (yyvsp[(3) - (3)].uval)); }
    break;

  case 258:

/* Line 1806 of yacc.c  */
#line 1251 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) + (yyvsp[(3) - (3)].uval); }
    break;

  case 259:

/* Line 1806 of yacc.c  */
#line 1252 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) - (yyvsp[(3) - (3)].uval); }
    break;

  case 260:

/* Line 1806 of yacc.c  */
#line 1253 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) * (yyvsp[(3) - (3)].uval); }
    break;

  case 261:

/* Line 1806 of yacc.c  */
#line 1254 "parser.y"
    { (yyval.uval) = ((yyvsp[(3) - (3)].uval) != 0) ? (yyvsp[(1) - (3)].uval) / (yyvsp[(3) - (3)].uval) : 0; }
    break;

  case 262:

/* Line 1806 of yacc.c  */
#line 1255 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) >> (yyvsp[(3) - (3)].uval); }
    break;

  case 263:

/* Line 1806 of yacc.c  */
#line 1256 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) << (yyvsp[(3) - (3)].uval); }
    break;

  case 264:

/* Line 1806 of yacc.c  */
#line 1257 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) | (yyvsp[(3) - (3)].uval); }
    break;

  case 265:

/* Line 1806 of yacc.c  */
#line 1258 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) ^ (yyvsp[(3) - (3)].uval); }
    break;

  case 266:

/* Line 1806 of yacc.c  */
#line 1259 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) & (yyvsp[(3) - (3)].uval); }
    break;

  case 267:

/* Line 1806 of yacc.c  */
#line 1260 "parser.y"
    { (yyval.uval) = !(yyvsp[(2) - (2)].uval); }
    break;

  case 268:

/* Line 1806 of yacc.c  */
#line 1261 "parser.y"
    { (yyval.uval) = -(yyvsp[(2) - (2)].uval); }
    break;

  case 269:

/* Line 1806 of yacc.c  */
#line 1262 "parser.y"
    { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[(2) - (2)].uval)); }
    break;

  case 270:

/* Line 1806 of yacc.c  */
#line 1263 "parser.y"
    { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[(2) - (2)].uval)); }
    break;

  case 271:

/* Line 1806 of yacc.c  */
#line 1264 "parser.y"
    { (yyval.uval) = (yyvsp[(2) - (3)].uval); }
    break;



/* Line 1806 of yacc.c  */
#line 4299 "y.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
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
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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
      if (!yypact_value_is_default (yyn))
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
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
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



/* Line 2067 of yacc.c  */
#line 1267 "parser.y"


#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
