/* A Bison parser, made by GNU Bison 2.7.1.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.7.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         bxparse
#define yylex           bxlex
#define yyerror         bxerror
#define yylval          bxlval
#define yychar          bxchar
#define yydebug         bxdebug
#define yynerrs         bxnerrs

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 5 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

#if BX_DEBUGGER

/* Line 371 of yacc.c  */
#line 83 "y.tab.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_BX_Y_TAB_H_INCLUDED
# define YY_BX_Y_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int bxdebug;
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
     BX_TOKEN_XMM = 291,
     BX_TOKEN_YMM = 292,
     BX_TOKEN_ZMM = 293,
     BX_TOKEN_AVX = 294,
     BX_TOKEN_IDT = 295,
     BX_TOKEN_IVT = 296,
     BX_TOKEN_GDT = 297,
     BX_TOKEN_LDT = 298,
     BX_TOKEN_TSS = 299,
     BX_TOKEN_TAB = 300,
     BX_TOKEN_ALL = 301,
     BX_TOKEN_LINUX = 302,
     BX_TOKEN_DEBUG_REGS = 303,
     BX_TOKEN_CONTROL_REGS = 304,
     BX_TOKEN_SEGMENT_REGS = 305,
     BX_TOKEN_EXAMINE = 306,
     BX_TOKEN_XFORMAT = 307,
     BX_TOKEN_DISFORMAT = 308,
     BX_TOKEN_RESTORE = 309,
     BX_TOKEN_WRITEMEM = 310,
     BX_TOKEN_SETPMEM = 311,
     BX_TOKEN_SYMBOLNAME = 312,
     BX_TOKEN_QUERY = 313,
     BX_TOKEN_PENDING = 314,
     BX_TOKEN_TAKE = 315,
     BX_TOKEN_DMA = 316,
     BX_TOKEN_IRQ = 317,
     BX_TOKEN_TLB = 318,
     BX_TOKEN_HEX = 319,
     BX_TOKEN_DISASM = 320,
     BX_TOKEN_INSTRUMENT = 321,
     BX_TOKEN_STRING = 322,
     BX_TOKEN_STOP = 323,
     BX_TOKEN_DOIT = 324,
     BX_TOKEN_CRC = 325,
     BX_TOKEN_TRACE = 326,
     BX_TOKEN_TRACEREG = 327,
     BX_TOKEN_TRACEMEM = 328,
     BX_TOKEN_SWITCH_MODE = 329,
     BX_TOKEN_SIZE = 330,
     BX_TOKEN_PTIME = 331,
     BX_TOKEN_TIMEBP_ABSOLUTE = 332,
     BX_TOKEN_TIMEBP = 333,
     BX_TOKEN_MODEBP = 334,
     BX_TOKEN_VMEXITBP = 335,
     BX_TOKEN_PRINT_STACK = 336,
     BX_TOKEN_WATCH = 337,
     BX_TOKEN_UNWATCH = 338,
     BX_TOKEN_READ = 339,
     BX_TOKEN_WRITE = 340,
     BX_TOKEN_SHOW = 341,
     BX_TOKEN_LOAD_SYMBOLS = 342,
     BX_TOKEN_SYMBOLS = 343,
     BX_TOKEN_LIST_SYMBOLS = 344,
     BX_TOKEN_GLOBAL = 345,
     BX_TOKEN_WHERE = 346,
     BX_TOKEN_PRINT_STRING = 347,
     BX_TOKEN_NUMERIC = 348,
     BX_TOKEN_PAGE = 349,
     BX_TOKEN_HELP = 350,
     BX_TOKEN_CALC = 351,
     BX_TOKEN_VGA = 352,
     BX_TOKEN_DEVICE = 353,
     BX_TOKEN_COMMAND = 354,
     BX_TOKEN_GENERIC = 355,
     BX_TOKEN_RSHIFT = 356,
     BX_TOKEN_LSHIFT = 357,
     BX_TOKEN_REG_IP = 358,
     BX_TOKEN_REG_EIP = 359,
     BX_TOKEN_REG_RIP = 360,
     INDIRECT = 361,
     NEG = 362,
     NOT = 363
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
#define BX_TOKEN_XMM 291
#define BX_TOKEN_YMM 292
#define BX_TOKEN_ZMM 293
#define BX_TOKEN_AVX 294
#define BX_TOKEN_IDT 295
#define BX_TOKEN_IVT 296
#define BX_TOKEN_GDT 297
#define BX_TOKEN_LDT 298
#define BX_TOKEN_TSS 299
#define BX_TOKEN_TAB 300
#define BX_TOKEN_ALL 301
#define BX_TOKEN_LINUX 302
#define BX_TOKEN_DEBUG_REGS 303
#define BX_TOKEN_CONTROL_REGS 304
#define BX_TOKEN_SEGMENT_REGS 305
#define BX_TOKEN_EXAMINE 306
#define BX_TOKEN_XFORMAT 307
#define BX_TOKEN_DISFORMAT 308
#define BX_TOKEN_RESTORE 309
#define BX_TOKEN_WRITEMEM 310
#define BX_TOKEN_SETPMEM 311
#define BX_TOKEN_SYMBOLNAME 312
#define BX_TOKEN_QUERY 313
#define BX_TOKEN_PENDING 314
#define BX_TOKEN_TAKE 315
#define BX_TOKEN_DMA 316
#define BX_TOKEN_IRQ 317
#define BX_TOKEN_TLB 318
#define BX_TOKEN_HEX 319
#define BX_TOKEN_DISASM 320
#define BX_TOKEN_INSTRUMENT 321
#define BX_TOKEN_STRING 322
#define BX_TOKEN_STOP 323
#define BX_TOKEN_DOIT 324
#define BX_TOKEN_CRC 325
#define BX_TOKEN_TRACE 326
#define BX_TOKEN_TRACEREG 327
#define BX_TOKEN_TRACEMEM 328
#define BX_TOKEN_SWITCH_MODE 329
#define BX_TOKEN_SIZE 330
#define BX_TOKEN_PTIME 331
#define BX_TOKEN_TIMEBP_ABSOLUTE 332
#define BX_TOKEN_TIMEBP 333
#define BX_TOKEN_MODEBP 334
#define BX_TOKEN_VMEXITBP 335
#define BX_TOKEN_PRINT_STACK 336
#define BX_TOKEN_WATCH 337
#define BX_TOKEN_UNWATCH 338
#define BX_TOKEN_READ 339
#define BX_TOKEN_WRITE 340
#define BX_TOKEN_SHOW 341
#define BX_TOKEN_LOAD_SYMBOLS 342
#define BX_TOKEN_SYMBOLS 343
#define BX_TOKEN_LIST_SYMBOLS 344
#define BX_TOKEN_GLOBAL 345
#define BX_TOKEN_WHERE 346
#define BX_TOKEN_PRINT_STRING 347
#define BX_TOKEN_NUMERIC 348
#define BX_TOKEN_PAGE 349
#define BX_TOKEN_HELP 350
#define BX_TOKEN_CALC 351
#define BX_TOKEN_VGA 352
#define BX_TOKEN_DEVICE 353
#define BX_TOKEN_COMMAND 354
#define BX_TOKEN_GENERIC 355
#define BX_TOKEN_RSHIFT 356
#define BX_TOKEN_LSHIFT 357
#define BX_TOKEN_REG_IP 358
#define BX_TOKEN_REG_EIP 359
#define BX_TOKEN_REG_RIP 360
#define INDIRECT 361
#define NEG 362
#define NOT 363



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 13 "parser.y"

  char    *sval;
  Bit64u   uval;
  bx_bool  bval;


/* Line 387 of yacc.c  */
#line 349 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE bxlval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int bxparse (void *YYPARSE_PARAM);
#else
int bxparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int bxparse (void);
#else
int bxparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_BX_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 377 "y.tab.c"

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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif


/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
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
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
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
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  289
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1822

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  123
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  57
/* YYNRULES -- Number of rules.  */
#define YYNRULES  276
/* YYNRULES -- Number of states.  */
#define YYNSTATES  538

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   363

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     116,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   119,     2,     2,     2,     2,   112,     2,
     120,   121,   110,   106,     2,   107,     2,   111,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   118,     2,
       2,   117,     2,     2,   122,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   109,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   108,     2,     2,     2,     2,     2,
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
     105,   113,   114,   115
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
     100,   102,   104,   105,   107,   109,   111,   113,   115,   117,
     119,   121,   123,   125,   127,   131,   135,   138,   141,   145,
     149,   153,   156,   160,   164,   167,   171,   175,   179,   182,
     186,   190,   194,   197,   202,   207,   212,   218,   224,   230,
     233,   237,   241,   246,   251,   257,   260,   264,   267,   270,
     274,   279,   284,   287,   292,   298,   304,   310,   316,   322,
     328,   334,   340,   346,   349,   355,   358,   362,   366,   369,
     373,   378,   381,   384,   388,   392,   396,   402,   408,   414,
     420,   424,   428,   432,   436,   440,   445,   449,   454,   460,
     465,   471,   472,   474,   477,   480,   483,   486,   489,   492,
     495,   498,   501,   505,   509,   513,   516,   521,   525,   529,
     532,   537,   543,   549,   553,   557,   562,   566,   569,   573,
     578,   582,   587,   593,   597,   602,   608,   612,   616,   620,
     625,   629,   633,   637,   641,   645,   649,   653,   657,   661,
     665,   669,   673,   677,   681,   685,   689,   693,   697,   701,
     705,   709,   713,   717,   721,   725,   729,   733,   737,   741,
     745,   749,   753,   757,   761,   765,   769,   773,   777,   781,
     785,   789,   793,   797,   801,   805,   809,   812,   816,   818,
     820,   822,   824,   826,   828,   830,   832,   834,   836,   838,
     842,   846,   850,   854,   858,   862,   866,   870,   874,   877,
     880,   884,   886,   888,   890,   892,   894,   896,   898,   900,
     902,   904,   906,   910,   914,   918,   922,   926,   930,   934,
     938,   942,   946,   949,   952,   955,   958
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     124,     0,    -1,   124,   125,    -1,   125,    -1,   144,    -1,
     145,    -1,   146,    -1,   147,    -1,   148,    -1,   151,    -1,
     153,    -1,   154,    -1,   155,    -1,   156,    -1,   157,    -1,
     158,    -1,   159,    -1,   161,    -1,   160,    -1,   149,    -1,
     150,    -1,   162,    -1,   163,    -1,   164,    -1,   165,    -1,
     166,    -1,   167,    -1,   168,    -1,   169,    -1,   170,    -1,
     171,    -1,   172,    -1,   173,    -1,   174,    -1,   175,    -1,
     136,    -1,   137,    -1,   138,    -1,   135,    -1,   129,    -1,
     130,    -1,   131,    -1,   139,    -1,   140,    -1,   133,    -1,
     134,    -1,   132,    -1,   141,    -1,   142,    -1,   143,    -1,
     176,    -1,   177,    -1,    -1,   116,    -1,    15,    -1,    16,
      -1,    31,    -1,    32,    -1,     8,    -1,     9,    -1,    10,
      -1,    11,    -1,    12,    -1,    13,    -1,    78,   179,   116,
      -1,    77,   179,   116,    -1,    79,   116,    -1,    80,   116,
      -1,    86,    99,   116,    -1,    86,    16,   116,    -1,    86,
      67,   116,    -1,    86,   116,    -1,    94,   179,   116,    -1,
      63,   179,   116,    -1,    76,   116,    -1,    71,   126,   116,
      -1,    72,   126,   116,    -1,    73,   126,   116,    -1,    81,
     116,    -1,    81,    93,   116,    -1,    82,    68,   116,    -1,
      82,    17,   116,    -1,    82,   116,    -1,    82,    31,   179,
     116,    -1,    82,    84,   179,   116,    -1,    82,    85,   179,
     116,    -1,    82,    31,   179,   179,   116,    -1,    82,    84,
     179,   179,   116,    -1,    82,    85,   179,   179,   116,    -1,
      83,   116,    -1,    83,   179,   116,    -1,    87,    67,   116,
      -1,    87,    67,   179,   116,    -1,    87,    90,    67,   116,
      -1,    87,    90,    67,   179,   116,    -1,    91,   116,    -1,
      92,   179,   116,    -1,    17,   116,    -1,    18,   116,    -1,
      18,    93,   116,    -1,    18,    46,    93,   116,    -1,    18,
      93,    93,   116,    -1,    19,   116,    -1,    20,    65,   126,
     116,    -1,    20,    57,   117,   179,   116,    -1,    20,     4,
     117,   179,   116,    -1,    20,     3,   117,   179,   116,    -1,
      20,     5,   117,   179,   116,    -1,    20,     6,   117,   179,
     116,    -1,    20,     7,   117,   179,   116,    -1,    20,   104,
     117,   179,   116,    -1,    20,   105,   117,   179,   116,    -1,
      20,   128,   117,   179,   116,    -1,    23,   116,    -1,    23,
     178,   118,   178,   116,    -1,    24,   116,    -1,    24,   179,
     116,    -1,    24,    67,   116,    -1,    25,   116,    -1,    25,
     179,   116,    -1,    25,   110,   179,   116,    -1,    22,   116,
      -1,    89,   116,    -1,    89,    67,   116,    -1,    29,    25,
     116,    -1,    29,    33,   116,    -1,    29,    40,   152,   152,
     116,    -1,    29,    41,   152,   152,   116,    -1,    29,    42,
     152,   152,   116,    -1,    29,    43,   152,   152,   116,    -1,
      29,    45,   116,    -1,    29,    44,   116,    -1,    29,    14,
     116,    -1,    29,    47,   116,    -1,    29,    88,   116,    -1,
      29,    88,    67,   116,    -1,    29,    98,   116,    -1,    29,
      98,    99,   116,    -1,    29,    98,    99,    67,   116,    -1,
      29,    98,    67,   116,    -1,    29,    98,    67,    67,   116,
      -1,    -1,   179,    -1,   127,   116,    -1,    34,   116,    -1,
      35,   116,    -1,    36,   116,    -1,    37,   116,    -1,    38,
     116,    -1,    50,   116,    -1,    49,   116,    -1,    48,   116,
      -1,    26,    93,   116,    -1,    27,    93,   116,    -1,    28,
      93,   116,    -1,    30,   116,    -1,    51,    52,   179,   116,
      -1,    51,    52,   116,    -1,    51,   179,   116,    -1,    51,
     116,    -1,    54,    67,    67,   116,    -1,    55,    67,   179,
     179,   116,    -1,    56,   179,   179,   179,   116,    -1,    58,
      59,   116,    -1,    60,    61,   116,    -1,    60,    61,    93,
     116,    -1,    60,    62,   116,    -1,    65,   116,    -1,    65,
     179,   116,    -1,    65,   179,   179,   116,    -1,    65,    53,
     116,    -1,    65,    53,   179,   116,    -1,    65,    53,   179,
     179,   116,    -1,    65,    74,   116,    -1,    65,    64,   126,
     116,    -1,    65,    75,   117,    93,   116,    -1,    66,    68,
     116,    -1,    66,    99,   116,    -1,    69,   179,   116,    -1,
      70,   179,   179,   116,    -1,    95,    30,   116,    -1,    95,
      17,   116,    -1,    95,    18,   116,    -1,    95,    19,   116,
      -1,    95,    23,   116,    -1,    95,    24,   116,    -1,    95,
      25,   116,    -1,    95,    26,   116,    -1,    95,    27,   116,
      -1,    95,    28,   116,    -1,    95,    22,   116,    -1,    95,
      79,   116,    -1,    95,    80,   116,    -1,    95,    70,   116,
      -1,    95,    71,   116,    -1,    95,    72,   116,    -1,    95,
      73,   116,    -1,    95,    54,   116,    -1,    95,    76,   116,
      -1,    95,    78,   116,    -1,    95,    77,   116,    -1,    95,
      81,   116,    -1,    95,    87,   116,    -1,    95,    89,   116,
      -1,    95,   127,   116,    -1,    95,    34,   116,    -1,    95,
      35,   116,    -1,    95,    36,   116,    -1,    95,    37,   116,
      -1,    95,    38,   116,    -1,    95,    50,   116,    -1,    95,
      49,   116,    -1,    95,    48,   116,    -1,    95,    55,   116,
      -1,    95,    56,   116,    -1,    95,    65,   116,    -1,    95,
      82,   116,    -1,    95,    83,   116,    -1,    95,    51,   116,
      -1,    95,    66,   116,    -1,    95,    20,   116,    -1,    95,
      94,   116,    -1,    95,    29,   116,    -1,    95,    86,   116,
      -1,    95,    96,   116,    -1,    95,    95,   116,    -1,    95,
     116,    -1,    96,   179,   116,    -1,    93,    -1,    67,    -1,
       4,    -1,     3,    -1,     5,    -1,     6,    -1,     7,    -1,
     128,    -1,   103,    -1,   104,    -1,   105,    -1,   178,   106,
     178,    -1,   178,   107,   178,    -1,   178,   110,   178,    -1,
     178,   111,   178,    -1,   178,   101,   178,    -1,   178,   102,
     178,    -1,   178,   108,   178,    -1,   178,   109,   178,    -1,
     178,   112,   178,    -1,   119,   178,    -1,   107,   178,    -1,
     120,   178,   121,    -1,    93,    -1,    67,    -1,     4,    -1,
       3,    -1,     5,    -1,     6,    -1,     7,    -1,   128,    -1,
     103,    -1,   104,    -1,   105,    -1,   179,   118,   179,    -1,
     179,   106,   179,    -1,   179,   107,   179,    -1,   179,   110,
     179,    -1,   179,   111,   179,    -1,   179,   101,   179,    -1,
     179,   102,   179,    -1,   179,   108,   179,    -1,   179,   109,
     179,    -1,   179,   112,   179,    -1,   119,   179,    -1,   107,
     179,    -1,   110,   179,    -1,   122,   179,    -1,   120,   179,
     121,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   140,   140,   141,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   200,   201,   206,   207,   212,   213,
     214,   215,   216,   217,   222,   227,   235,   243,   251,   256,
     261,   266,   274,   282,   290,   298,   306,   314,   322,   327,
     335,   340,   345,   350,   355,   360,   365,   370,   375,   380,
     385,   393,   398,   403,   408,   416,   424,   432,   440,   445,
     450,   455,   463,   471,   476,   481,   485,   489,   493,   497,
     501,   505,   509,   516,   521,   526,   531,   536,   541,   546,
     551,   559,   567,   572,   580,   585,   590,   595,   600,   605,
     610,   615,   620,   625,   630,   635,   640,   645,   650,   655,
     660,   668,   669,   672,   680,   688,   696,   704,   712,   720,
     728,   736,   744,   752,   759,   767,   775,   780,   785,   790,
     798,   806,   814,   822,   830,   835,   840,   848,   853,   858,
     863,   868,   873,   878,   883,   888,   896,   901,   909,   917,
     925,   930,   935,   942,   947,   952,   957,   962,   967,   972,
     977,   982,   987,   992,   997,  1003,  1009,  1015,  1023,  1028,
    1033,  1038,  1043,  1048,  1053,  1058,  1063,  1068,  1073,  1078,
    1083,  1088,  1093,  1098,  1103,  1108,  1118,  1129,  1135,  1148,
    1153,  1164,  1169,  1185,  1197,  1207,  1212,  1220,  1238,  1239,
    1240,  1241,  1242,  1243,  1244,  1245,  1246,  1247,  1248,  1249,
    1250,  1251,  1252,  1253,  1254,  1255,  1256,  1257,  1258,  1259,
    1260,  1266,  1267,  1268,  1269,  1270,  1271,  1272,  1273,  1274,
    1275,  1276,  1277,  1278,  1279,  1280,  1281,  1282,  1283,  1284,
    1285,  1286,  1287,  1288,  1289,  1290,  1291
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
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
  "BX_TOKEN_CPU", "BX_TOKEN_FPU", "BX_TOKEN_MMX", "BX_TOKEN_XMM",
  "BX_TOKEN_YMM", "BX_TOKEN_ZMM", "BX_TOKEN_AVX", "BX_TOKEN_IDT",
  "BX_TOKEN_IVT", "BX_TOKEN_GDT", "BX_TOKEN_LDT", "BX_TOKEN_TSS",
  "BX_TOKEN_TAB", "BX_TOKEN_ALL", "BX_TOKEN_LINUX", "BX_TOKEN_DEBUG_REGS",
  "BX_TOKEN_CONTROL_REGS", "BX_TOKEN_SEGMENT_REGS", "BX_TOKEN_EXAMINE",
  "BX_TOKEN_XFORMAT", "BX_TOKEN_DISFORMAT", "BX_TOKEN_RESTORE",
  "BX_TOKEN_WRITEMEM", "BX_TOKEN_SETPMEM", "BX_TOKEN_SYMBOLNAME",
  "BX_TOKEN_QUERY", "BX_TOKEN_PENDING", "BX_TOKEN_TAKE", "BX_TOKEN_DMA",
  "BX_TOKEN_IRQ", "BX_TOKEN_TLB", "BX_TOKEN_HEX", "BX_TOKEN_DISASM",
  "BX_TOKEN_INSTRUMENT", "BX_TOKEN_STRING", "BX_TOKEN_STOP",
  "BX_TOKEN_DOIT", "BX_TOKEN_CRC", "BX_TOKEN_TRACE", "BX_TOKEN_TRACEREG",
  "BX_TOKEN_TRACEMEM", "BX_TOKEN_SWITCH_MODE", "BX_TOKEN_SIZE",
  "BX_TOKEN_PTIME", "BX_TOKEN_TIMEBP_ABSOLUTE", "BX_TOKEN_TIMEBP",
  "BX_TOKEN_MODEBP", "BX_TOKEN_VMEXITBP", "BX_TOKEN_PRINT_STACK",
  "BX_TOKEN_WATCH", "BX_TOKEN_UNWATCH", "BX_TOKEN_READ", "BX_TOKEN_WRITE",
  "BX_TOKEN_SHOW", "BX_TOKEN_LOAD_SYMBOLS", "BX_TOKEN_SYMBOLS",
  "BX_TOKEN_LIST_SYMBOLS", "BX_TOKEN_GLOBAL", "BX_TOKEN_WHERE",
  "BX_TOKEN_PRINT_STRING", "BX_TOKEN_NUMERIC", "BX_TOKEN_PAGE",
  "BX_TOKEN_HELP", "BX_TOKEN_CALC", "BX_TOKEN_VGA", "BX_TOKEN_DEVICE",
  "BX_TOKEN_COMMAND", "BX_TOKEN_GENERIC", "BX_TOKEN_RSHIFT",
  "BX_TOKEN_LSHIFT", "BX_TOKEN_REG_IP", "BX_TOKEN_REG_EIP",
  "BX_TOKEN_REG_RIP", "'+'", "'-'", "'|'", "'^'", "'*'", "'/'", "'&'",
  "INDIRECT", "NEG", "NOT", "'\\n'", "'='", "':'", "'!'", "'('", "')'",
  "'@'", "$accept", "commands", "command", "BX_TOKEN_TOGGLE_ON_OFF",
  "BX_TOKEN_REGISTERS", "BX_TOKEN_SEGREG", "timebp_command",
  "modebp_command", "vmexitbp_command", "show_command", "page_command",
  "tlb_command", "ptime_command", "trace_command", "trace_reg_command",
  "trace_mem_command", "print_stack_command", "watch_point_command",
  "symbol_command", "where_command", "print_string_command",
  "continue_command", "stepN_command", "step_over_command", "set_command",
  "breakpoint_command", "blist_command", "slist_command", "info_command",
  "optional_numeric", "regs_command", "fpu_regs_command",
  "mmx_regs_command", "xmm_regs_command", "ymm_regs_command",
  "zmm_regs_command", "segment_regs_command", "control_regs_command",
  "debug_regs_command", "delete_command", "bpe_command", "bpd_command",
  "quit_command", "examine_command", "restore_command", "writemem_command",
  "setpmem_command", "query_command", "take_command",
  "disassemble_command", "instrument_command", "doit_command",
  "crc_command", "help_command", "calc_command", "vexpression",
  "expression", YY_NULL
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
     355,   356,   357,   358,   359,   360,    43,    45,   124,    94,
      42,    47,    38,   361,   362,   363,    10,    61,    58,    33,
      40,    41,    64
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   123,   124,   124,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   126,   126,   127,   127,   128,   128,
     128,   128,   128,   128,   129,   129,   130,   131,   132,   132,
     132,   132,   133,   134,   135,   136,   137,   138,   139,   139,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   141,   141,   141,   141,   142,   143,   144,   145,   145,
     145,   145,   146,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   148,   148,   148,   148,   148,   148,   148,
     148,   149,   150,   150,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     151,   152,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   166,   166,   166,
     167,   168,   169,   170,   171,   171,   171,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   173,   173,   174,   175,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   177,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     2,     2,     3,     3,
       3,     2,     3,     3,     2,     3,     3,     3,     2,     3,
       3,     3,     2,     4,     4,     4,     5,     5,     5,     2,
       3,     3,     4,     4,     5,     2,     3,     2,     2,     3,
       4,     4,     2,     4,     5,     5,     5,     5,     5,     5,
       5,     5,     5,     2,     5,     2,     3,     3,     2,     3,
       4,     2,     2,     3,     3,     3,     5,     5,     5,     5,
       3,     3,     3,     3,     3,     4,     3,     4,     5,     4,
       5,     0,     1,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     3,     3,     3,     2,     4,     3,     3,     2,
       4,     5,     5,     3,     3,     4,     3,     2,     3,     4,
       3,     4,     5,     3,     4,     5,     3,     3,     3,     4,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     2,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      52,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    56,    57,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    53,     0,     3,     0,    39,    40,
      41,    46,    44,    45,    38,    35,    36,    37,    42,    43,
      47,    48,    49,     4,     5,     6,     7,     8,    19,    20,
       9,    10,    11,    12,    13,    14,    15,    16,    18,    17,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    50,    51,    97,     0,     0,    98,
     102,     0,     0,     0,     0,     0,    58,    59,    60,    61,
      62,    63,     0,     0,     0,     0,     0,   121,   231,   230,
     232,   233,   234,   229,   228,   236,   237,   238,     0,   113,
       0,     0,   235,     0,   254,   253,   255,   256,   257,   252,
     251,   259,   260,   261,     0,     0,   115,     0,     0,     0,
     258,     0,   252,     0,   118,     0,     0,     0,     0,     0,
       0,     0,   141,   141,   141,   141,     0,     0,     0,     0,
       0,   155,   144,   145,   146,   147,   148,   151,   150,   149,
       0,   159,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   167,     0,     0,     0,     0,     0,
      54,    55,     0,     0,     0,    74,     0,     0,    66,    67,
       0,    78,     0,     0,     0,     0,     0,    82,    89,     0,
       0,     0,     0,    71,     0,     0,     0,   122,    95,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   226,     0,     0,     1,
       2,   143,     0,     0,    99,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   249,   248,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   117,   273,
     274,   272,     0,   275,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   116,     0,   274,   119,   152,   153,   154,
     132,   124,   125,   141,   142,   141,   141,   141,   131,   130,
     133,     0,   134,     0,     0,   136,   157,     0,   158,     0,
       0,     0,     0,     0,   163,     0,   164,   166,    73,   170,
       0,     0,   173,     0,   168,     0,   176,   177,   178,     0,
      75,    76,    77,    65,    64,    79,    81,     0,    80,     0,
       0,    90,    69,    70,    68,    91,     0,     0,   123,    96,
      72,   181,   182,   183,   220,   190,   184,   185,   186,   187,
     188,   189,   222,   180,   205,   206,   207,   208,   209,   212,
     211,   210,   218,   197,   213,   214,   215,   219,   193,   194,
     195,   196,   198,   200,   199,   191,   192,   201,   216,   217,
     223,   202,   203,   221,   225,   224,   204,   227,   100,   101,
       0,     0,     0,     0,     0,     0,   103,     0,     0,     0,
     250,   243,   244,   239,   240,   245,   246,   241,   242,   247,
       0,   276,   267,   268,   263,   264,   269,   270,   265,   266,
     271,   262,   120,     0,     0,     0,     0,   135,     0,   139,
       0,   137,   156,   160,     0,   264,   265,     0,   165,   171,
       0,   174,     0,   169,   179,    83,     0,    84,     0,    85,
       0,    92,    93,     0,   106,   105,   107,   108,   109,   104,
     110,   111,   112,   114,   126,   127,   128,   129,   140,   138,
     161,   162,   172,   175,    86,    87,    88,    94
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    55,    56,   212,    57,   160,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,   343,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   143,   344
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -140
static const yytype_int16 yypact[] =
{
    1326,  -106,   -27,   -85,    49,   -79,  1118,   726,   773,   -24,
     -18,   -15,   326,   -68,  -140,  -140,   -65,   -37,   -36,   -33,
     -28,   -26,   -13,   -12,   119,    17,    38,  1098,    48,   -32,
    1098,   653,   -59,  1098,  1098,    26,    26,    26,    -1,  1098,
    1098,     0,     1,   -77,    -3,   793,    -4,   -22,   -49,     5,
    1098,  1098,  1409,  1098,  -140,  1243,  -140,    18,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,    40,   -44,  -140,
    -140,    20,    23,    24,    25,    27,  -140,  -140,  -140,  -140,
    -140,  -140,    28,    26,    29,    32,    51,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  1137,  -140,
    1137,  1137,  -140,   901,  -140,  -140,  -140,  -140,  -140,    19,
    -140,  -140,  -140,  -140,  1098,  1098,  -140,  1098,  1098,  1098,
    -140,   433,  -140,  1098,  -140,   498,    39,    53,    54,    68,
      69,    73,  1098,  1098,  1098,  1098,    74,    75,    76,   -46,
     -52,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
     913,  -140,   643,   127,  1098,   585,    79,   -43,    80,   706,
     933,    26,    81,    82,  -140,   154,    87,    89,   763,   585,
    -140,  -140,    97,    98,   104,  -140,  1050,  1068,  -140,  -140,
     109,  -140,   112,  1098,   114,  1098,  1098,  -140,  -140,  1360,
     115,   116,   117,  -140,   954,   148,   118,  -140,  -140,  1400,
    1420,   120,   121,   124,   126,   128,   129,   130,   132,   133,
     134,   135,   136,   137,   138,   151,   152,   153,   155,   159,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   178,   182,   183,   184,   185,   186,   187,   188,   189,
     200,   210,   223,   225,   226,   227,  -140,   228,  1433,  -140,
    -140,  -140,   229,   230,  -140,  1098,  1098,  1098,  1098,  1098,
    1098,   231,  1098,  1098,  1098,  -140,  -140,   368,  1137,  1137,
    1137,  1137,  1137,  1137,  1137,  1137,  1137,  1137,  -140,   125,
     125,   125,   -10,   125,  1098,  1098,  1098,  1098,  1098,  1098,
    1098,  1098,  1098,  -140,  1098,  -105,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  1098,  1088,  1098,  1098,  1098,  -140,  -140,
    -140,   232,  -140,   -45,   -42,  -140,  -140,  1446,  -140,   233,
     585,  1098,  1098,   585,  -140,   234,  -140,  -140,  -140,  -140,
     325,   236,  -140,    50,  -140,  1459,  -140,  -140,  -140,  1472,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,   390,  -140,   455,
     520,  -140,  -140,  -140,  -140,  -140,  1485,   978,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    1498,  1511,  1524,  1537,  1550,  1563,  -140,  1576,  1589,  1602,
    -140,  -140,  -140,     8,     8,     8,     8,  -140,  -140,  -140,
    1706,  -140,   125,   125,   -25,   -25,   -25,   -25,   125,   125,
     125,  1088,  -140,   241,   242,   244,   245,  -140,   246,  -140,
     248,  -140,  -140,  -140,  1615,    99,   125,  1628,  -140,  -140,
    1641,  -140,   249,  -140,  -140,  -140,  1654,  -140,  1667,  -140,
    1680,  -140,  -140,  1693,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -140,  -140,   317,   -29,   322,    -2,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -139,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,  -140,  -140,  -140,  -135,    -7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -274
static const yytype_int16 yytable[] =
{
     161,   165,   126,   305,   142,   306,   307,   213,   214,   206,
     106,   482,   230,   334,   222,   353,   220,   192,   236,   107,
     195,   351,   488,   199,   205,   490,   208,   209,   223,   197,
     198,   110,   216,   217,   345,   346,   347,   127,   229,   221,
     207,   210,   211,   239,   240,   234,   288,   354,   181,   293,
     365,   182,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   231,   355,   224,   108,   237,   235,   166,
     352,   489,   294,   366,   491,   167,   324,   325,   168,   183,
     184,   225,   226,   185,   193,   330,   331,   332,   186,   109,
     187,   324,   325,   334,   301,   232,   326,   327,   328,   329,
     330,   331,   332,   188,   189,   194,   122,   196,   334,   308,
     309,   471,   233,   227,   123,   215,   218,   219,   314,   315,
     316,   238,   144,   145,   146,   147,   148,   116,   117,   118,
     119,   120,   121,   292,   291,   318,   142,   295,   142,   142,
     296,   297,   298,   502,   299,   300,   302,   319,   320,   303,
     321,   322,   323,   124,   125,   337,   335,   144,   145,   146,
     147,   148,   116,   117,   118,   119,   120,   121,   304,   338,
     339,   190,   371,   461,   462,   463,   464,   465,   466,   467,
     468,   469,   470,   357,   340,   341,   162,   360,   363,   342,
     348,   349,   350,   370,   359,   364,   367,   372,   375,   373,
    -273,  -273,   379,   376,   483,   377,   484,   485,   486,  -273,
    -273,  -273,   150,   380,   381,   397,   387,   334,   389,   390,
     382,   162,   151,   152,   153,   385,   154,   396,   386,   155,
     388,   392,   393,   394,   398,   191,   401,   402,   157,   158,
     403,   159,   404,   334,   405,   406,   407,   150,   408,   409,
     410,   411,   412,   413,   414,   324,   325,   151,   152,   153,
     326,   361,   328,   329,   362,   331,   332,   415,   416,   417,
     374,   418,   334,   157,   158,   419,   159,   420,   421,   422,
     423,   424,   425,   426,   427,   428,   429,   430,   450,   451,
     452,   453,   454,   455,   431,   457,   458,   459,   432,   433,
     434,   435,   436,   437,   438,   439,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   440,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   441,   481,   144,   145,
     146,   147,   148,   116,   117,   118,   119,   120,   121,   442,
     169,   443,   444,   445,   446,   448,   449,   456,   487,   493,
     498,   170,   501,   494,   495,   496,   497,   524,   525,   171,
     526,   527,   528,   500,   529,   533,   172,   173,   174,   175,
     176,   177,   290,   178,   287,     0,     0,     0,     0,     0,
     506,     0,   508,   510,     0,     0,     0,     0,     0,     0,
     513,     0,   162,   144,   145,   146,   147,   148,   116,   117,
     118,   119,   120,   121,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   179,     0,     0,     0,   150,     0,
       0,     0,     0,     0,   180,     0,   324,   325,   151,   152,
     153,   326,   361,   328,   329,   362,   331,   332,     0,     0,
       0,   499,     0,   334,   157,   158,     0,   159,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   162,   144,   145,
     146,   147,   148,   116,   117,   118,   119,   120,   121,   308,
     309,     0,     0,     0,   310,   311,   312,   313,   314,   315,
     316,     0,     0,   150,     0,     0,     0,     0,     0,   460,
       0,   324,   325,   151,   152,   153,   326,   361,   328,   329,
     362,   331,   332,     0,     0,     0,   505,     0,   334,   157,
     158,     0,   159,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   162,   144,   145,   146,   147,   148,   116,   117,
     118,   119,   120,   121,   324,   325,     0,     0,     0,   326,
     327,   328,   329,   330,   331,   332,     0,     0,   150,   333,
       0,   334,     0,     0,     0,     0,   324,   325,   151,   152,
     153,   326,   361,   328,   329,   362,   331,   332,     0,     0,
       0,   507,     0,   334,   157,   158,     0,   159,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   162,   144,   145,
     146,   147,   148,   116,   117,   118,   119,   120,   121,   324,
     325,     0,     0,     0,   326,   327,   328,   329,   330,   331,
     332,     0,     0,   150,   336,     0,   334,     0,     0,     0,
       0,   324,   325,   151,   152,   153,   326,   361,   328,   329,
     362,   331,   332,     0,     0,     0,   509,     0,   334,   157,
     158,     0,   159,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   162,     0,     0,     0,   144,   145,   146,   147,
     148,   116,   117,   118,   119,   120,   121,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   150,     0,
       0,     0,     0,     0,     0,     0,   324,   325,   151,   152,
     153,   326,   361,   328,   329,   362,   331,   332,     0,     0,
       0,     0,     0,   334,   157,   158,   200,   159,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   201,     0,     0,
     162,     0,     0,     0,     0,     0,     0,   202,   203,   144,
     145,   146,   147,   148,   116,   117,   118,   119,   120,   121,
       0,     0,     0,     0,   324,   325,   150,     0,     0,   326,
     327,   328,   329,   330,   331,   332,   151,   152,   153,   358,
     154,   334,     0,   155,     0,     0,     0,     0,     0,   204,
       0,     0,   157,   158,     0,   159,   144,   145,   146,   147,
     148,   116,   117,   118,   119,   120,   121,     0,     0,     0,
       0,     0,     0,   149,     0,     0,   144,   145,   146,   147,
     148,   116,   117,   118,   119,   120,   121,   324,   325,     0,
       0,     0,   326,   327,   328,   329,   330,   331,   332,   150,
       0,     0,   368,     0,   334,     0,     0,     0,     0,   151,
     152,   153,     0,   154,     0,     0,   155,     0,     0,     0,
     162,     0,   156,     0,     0,   157,   158,     0,   159,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     162,     0,     0,     0,   324,   325,   150,     0,     0,   326,
     327,   328,   329,   330,   331,   332,   151,   152,   153,   378,
     154,   334,     0,   163,     0,     0,   150,     0,     0,   164,
       0,     0,   157,   158,     0,   159,   151,   152,   153,     0,
     154,     0,     0,   155,     0,     0,     0,     0,     0,   228,
       0,     0,   157,   158,     0,   159,   144,   145,   146,   147,
     148,   116,   117,   118,   119,   120,   121,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   144,   145,   146,   147,
     148,   116,   117,   118,   119,   120,   121,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   144,   145,   146,
     147,   148,   116,   117,   118,   119,   120,   121,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     162,   144,   145,   146,   147,   148,   116,   117,   118,   119,
     120,   121,     0,     0,     0,     0,     0,     0,     0,     0,
     162,     0,   308,   309,     0,     0,   150,   310,   311,   312,
     313,   314,   315,   316,     0,     0,   151,   152,   153,   317,
     154,   162,     0,   155,     0,     0,   150,     0,     0,   356,
       0,     0,   157,   158,     0,   159,   151,   152,   153,     0,
     154,     0,     0,   155,     0,   162,     0,   150,     0,   369,
       0,     0,   157,   158,     0,   159,     0,   151,   152,   153,
       0,   154,     0,     0,   155,     0,     0,     0,     0,     0,
     395,   150,     0,   157,   158,     0,   159,     0,     0,     0,
       0,   151,   152,   153,     0,   154,     0,     0,   155,     0,
       0,     0,     0,     0,   512,     0,     0,   157,   158,     0,
     159,   144,   145,   146,   147,   148,   116,   117,   118,   119,
     120,   121,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   128,   129,   130,   131,   132,   116,   117,   118,   119,
     120,   121,     0,     0,     0,     0,     0,     0,     0,     0,
     128,   129,   130,   131,   132,   116,   117,   118,   119,   120,
     121,   324,   325,     0,     0,     0,   326,   327,   328,   329,
     330,   331,   332,     0,     0,   162,   383,     0,   334,   324,
     325,     0,     0,     0,   326,   327,   328,   329,   330,   331,
     332,     0,     0,     0,   384,   133,   334,     0,     0,   324,
     325,   150,     0,     0,   326,   327,   328,   329,   330,   331,
     332,   151,   152,   153,   133,   154,   334,     0,   155,     0,
       0,   134,     0,     0,     0,     0,     0,   157,   158,     0,
     159,   135,   136,   137,     0,   138,     0,     0,     0,     0,
     134,     0,     0,     0,   139,     0,     0,   140,   141,     0,
     135,   136,   137,   289,   138,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   140,   141,     0,     0,
       1,     2,     3,     4,     0,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,     0,    16,    17,    18,
      19,    20,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    21,    22,    23,    24,     0,     0,    25,    26,    27,
       0,    28,     0,    29,     0,     0,    30,     0,    31,    32,
       0,     0,    33,    34,    35,    36,    37,     0,     0,    38,
      39,    40,    41,    42,    43,    44,    45,     0,     0,    46,
      47,     0,    48,     0,    49,    50,     0,    51,    52,    53,
       0,     0,     0,     1,     2,     3,     4,     0,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    54,
      16,    17,    18,    19,    20,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    21,    22,    23,    24,     0,     0,
      25,    26,    27,     0,    28,     0,    29,     0,     0,    30,
       0,    31,    32,     0,     0,    33,    34,    35,    36,    37,
       0,     0,    38,    39,    40,    41,    42,    43,    44,    45,
       0,     0,    46,    47,     0,    48,     0,    49,    50,     0,
      51,    52,    53,     0,     0,     0,   241,   242,   243,   244,
       0,   245,   246,   247,   248,   249,   250,   251,   252,   253,
      14,    15,    54,   254,   255,   256,   257,   258,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   259,   260,   261,
     262,   324,   325,   263,   264,   265,   326,   327,   328,   329,
     330,   331,   332,     0,   266,   267,   391,     0,   334,   268,
     269,   270,   271,     0,     0,   272,   273,   274,   275,   276,
     277,   278,   279,     0,     0,   280,   281,     0,   282,     0,
       0,   324,   325,   283,   284,   285,   326,   327,   328,   329,
     330,   331,   332,     0,     0,     0,   399,     0,   334,     0,
       0,   324,   325,     0,     0,   286,   326,   327,   328,   329,
     330,   331,   332,     0,   324,   325,   400,     0,   334,   326,
     327,   328,   329,   330,   331,   332,     0,   324,   325,   447,
       0,   334,   326,   327,   328,   329,   330,   331,   332,     0,
     324,   325,   492,     0,   334,   326,   327,   328,   329,   330,
     331,   332,     0,   324,   325,   503,     0,   334,   326,   327,
     328,   329,   330,   331,   332,     0,   324,   325,   504,     0,
     334,   326,   327,   328,   329,   330,   331,   332,     0,   324,
     325,   511,     0,   334,   326,   327,   328,   329,   330,   331,
     332,     0,   324,   325,   514,     0,   334,   326,   327,   328,
     329,   330,   331,   332,     0,   324,   325,   515,     0,   334,
     326,   327,   328,   329,   330,   331,   332,     0,   324,   325,
     516,     0,   334,   326,   327,   328,   329,   330,   331,   332,
       0,   324,   325,   517,     0,   334,   326,   327,   328,   329,
     330,   331,   332,     0,   324,   325,   518,     0,   334,   326,
     327,   328,   329,   330,   331,   332,     0,   324,   325,   519,
       0,   334,   326,   327,   328,   329,   330,   331,   332,     0,
     324,   325,   520,     0,   334,   326,   327,   328,   329,   330,
     331,   332,     0,   324,   325,   521,     0,   334,   326,   327,
     328,   329,   330,   331,   332,     0,   324,   325,   522,     0,
     334,   326,   327,   328,   329,   330,   331,   332,     0,   324,
     325,   530,     0,   334,   326,   327,   328,   329,   330,   331,
     332,     0,   324,   325,   531,     0,   334,   326,   327,   328,
     329,   330,   331,   332,     0,   324,   325,   532,     0,   334,
     326,   327,   328,   329,   330,   331,   332,     0,   324,   325,
     534,     0,   334,   326,   327,   328,   329,   330,   331,   332,
       0,   324,   325,   535,     0,   334,   326,   327,   328,   329,
     330,   331,   332,     0,   324,   325,   536,     0,   334,   326,
     327,   328,   329,   330,   331,   332,     0,   308,   309,   537,
       0,   334,   310,   311,   312,   313,   314,   315,   316,     0,
       0,     0,   523
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-140)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       7,     8,     4,   138,     6,   140,   141,    36,    37,    68,
     116,   116,    16,   118,    17,    67,    93,    24,    67,    46,
      27,    67,    67,    30,    31,    67,    33,    34,    31,    61,
      62,   116,    39,    40,   173,   174,   175,   116,    45,   116,
      99,    15,    16,    50,    51,    67,    53,    99,   116,    93,
      93,   116,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    67,   116,    68,    93,   116,    90,    93,
     116,   116,   116,   116,   116,    93,   101,   102,    93,   116,
     116,    84,    85,   116,    67,   110,   111,   112,   116,   116,
     116,   101,   102,   118,   123,    99,   106,   107,   108,   109,
     110,   111,   112,   116,   116,    67,    57,    59,   118,   101,
     102,   121,   116,   116,    65,   116,   116,   116,   110,   111,
     112,   116,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    93,   116,   116,   138,   117,   140,   141,
     117,   117,   117,    93,   117,   117,   117,   154,   155,   117,
     157,   158,   159,   104,   105,   116,   163,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,   117,   116,
     116,    52,   201,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   190,   116,   116,    67,   194,   195,   116,
     116,   116,   116,   200,    67,   116,   116,   116,   205,   117,
     101,   102,   209,   116,   343,   116,   345,   346,   347,   110,
     111,   112,    93,   116,   116,    67,   223,   118,   225,   226,
     116,    67,   103,   104,   105,   116,   107,   234,   116,   110,
     116,   116,   116,   116,   116,   116,   116,   116,   119,   120,
     116,   122,   116,   118,   116,   116,   116,    93,   116,   116,
     116,   116,   116,   116,   116,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   116,   116,   116,
     116,   116,   118,   119,   120,   116,   122,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   295,   296,
     297,   298,   299,   300,   116,   302,   303,   304,   116,   116,
     116,   116,   116,   116,   116,   116,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   116,   324,   325,   326,
     327,   328,   329,   330,   331,   332,   116,   334,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,   116,
      14,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,    25,   116,   360,   361,   362,   363,   116,   116,    33,
     116,   116,   116,   370,   116,   116,    40,    41,    42,    43,
      44,    45,    55,    47,    52,    -1,    -1,    -1,    -1,    -1,
     387,    -1,   389,   390,    -1,    -1,    -1,    -1,    -1,    -1,
     397,    -1,    67,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    88,    -1,    -1,    -1,    93,    -1,
      -1,    -1,    -1,    -1,    98,    -1,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,    -1,    -1,
      -1,   116,    -1,   118,   119,   120,    -1,   122,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,   101,
     102,    -1,    -1,    -1,   106,   107,   108,   109,   110,   111,
     112,    -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,   121,
      -1,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,    -1,    -1,    -1,   116,    -1,   118,   119,
     120,    -1,   122,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    67,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,   101,   102,    -1,    -1,    -1,   106,
     107,   108,   109,   110,   111,   112,    -1,    -1,    93,   116,
      -1,   118,    -1,    -1,    -1,    -1,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,    -1,    -1,
      -1,   116,    -1,   118,   119,   120,    -1,   122,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,   101,
     102,    -1,    -1,    -1,   106,   107,   108,   109,   110,   111,
     112,    -1,    -1,    93,   116,    -1,   118,    -1,    -1,    -1,
      -1,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,    -1,    -1,    -1,   116,    -1,   118,   119,
     120,    -1,   122,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    67,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    93,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,    -1,    -1,
      -1,    -1,    -1,   118,   119,   120,    53,   122,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    -1,    -1,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    74,    75,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      -1,    -1,    -1,    -1,   101,   102,    93,    -1,    -1,   106,
     107,   108,   109,   110,   111,   112,   103,   104,   105,   116,
     107,   118,    -1,   110,    -1,    -1,    -1,    -1,    -1,   116,
      -1,    -1,   119,   120,    -1,   122,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,   101,   102,    -1,
      -1,    -1,   106,   107,   108,   109,   110,   111,   112,    93,
      -1,    -1,   116,    -1,   118,    -1,    -1,    -1,    -1,   103,
     104,   105,    -1,   107,    -1,    -1,   110,    -1,    -1,    -1,
      67,    -1,   116,    -1,    -1,   119,   120,    -1,   122,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    -1,    -1,    -1,   101,   102,    93,    -1,    -1,   106,
     107,   108,   109,   110,   111,   112,   103,   104,   105,   116,
     107,   118,    -1,   110,    -1,    -1,    93,    -1,    -1,   116,
      -1,    -1,   119,   120,    -1,   122,   103,   104,   105,    -1,
     107,    -1,    -1,   110,    -1,    -1,    -1,    -1,    -1,   116,
      -1,    -1,   119,   120,    -1,   122,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    -1,   101,   102,    -1,    -1,    93,   106,   107,   108,
     109,   110,   111,   112,    -1,    -1,   103,   104,   105,   118,
     107,    67,    -1,   110,    -1,    -1,    93,    -1,    -1,   116,
      -1,    -1,   119,   120,    -1,   122,   103,   104,   105,    -1,
     107,    -1,    -1,   110,    -1,    67,    -1,    93,    -1,   116,
      -1,    -1,   119,   120,    -1,   122,    -1,   103,   104,   105,
      -1,   107,    -1,    -1,   110,    -1,    -1,    -1,    -1,    -1,
     116,    93,    -1,   119,   120,    -1,   122,    -1,    -1,    -1,
      -1,   103,   104,   105,    -1,   107,    -1,    -1,   110,    -1,
      -1,    -1,    -1,    -1,   116,    -1,    -1,   119,   120,    -1,
     122,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,   101,   102,    -1,    -1,    -1,   106,   107,   108,   109,
     110,   111,   112,    -1,    -1,    67,   116,    -1,   118,   101,
     102,    -1,    -1,    -1,   106,   107,   108,   109,   110,   111,
     112,    -1,    -1,    -1,   116,    67,   118,    -1,    -1,   101,
     102,    93,    -1,    -1,   106,   107,   108,   109,   110,   111,
     112,   103,   104,   105,    67,   107,   118,    -1,   110,    -1,
      -1,    93,    -1,    -1,    -1,    -1,    -1,   119,   120,    -1,
     122,   103,   104,   105,    -1,   107,    -1,    -1,    -1,    -1,
      93,    -1,    -1,    -1,   116,    -1,    -1,   119,   120,    -1,
     103,   104,   105,     0,   107,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   119,   120,    -1,    -1,
      17,    18,    19,    20,    -1,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    -1,    34,    35,    36,
      37,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    -1,    -1,    54,    55,    56,
      -1,    58,    -1,    60,    -1,    -1,    63,    -1,    65,    66,
      -1,    -1,    69,    70,    71,    72,    73,    -1,    -1,    76,
      77,    78,    79,    80,    81,    82,    83,    -1,    -1,    86,
      87,    -1,    89,    -1,    91,    92,    -1,    94,    95,    96,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,   116,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    48,    49,    50,    51,    -1,    -1,
      54,    55,    56,    -1,    58,    -1,    60,    -1,    -1,    63,
      -1,    65,    66,    -1,    -1,    69,    70,    71,    72,    73,
      -1,    -1,    76,    77,    78,    79,    80,    81,    82,    83,
      -1,    -1,    86,    87,    -1,    89,    -1,    91,    92,    -1,
      94,    95,    96,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,   116,    34,    35,    36,    37,    38,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    50,
      51,   101,   102,    54,    55,    56,   106,   107,   108,   109,
     110,   111,   112,    -1,    65,    66,   116,    -1,   118,    70,
      71,    72,    73,    -1,    -1,    76,    77,    78,    79,    80,
      81,    82,    83,    -1,    -1,    86,    87,    -1,    89,    -1,
      -1,   101,   102,    94,    95,    96,   106,   107,   108,   109,
     110,   111,   112,    -1,    -1,    -1,   116,    -1,   118,    -1,
      -1,   101,   102,    -1,    -1,   116,   106,   107,   108,   109,
     110,   111,   112,    -1,   101,   102,   116,    -1,   118,   106,
     107,   108,   109,   110,   111,   112,    -1,   101,   102,   116,
      -1,   118,   106,   107,   108,   109,   110,   111,   112,    -1,
     101,   102,   116,    -1,   118,   106,   107,   108,   109,   110,
     111,   112,    -1,   101,   102,   116,    -1,   118,   106,   107,
     108,   109,   110,   111,   112,    -1,   101,   102,   116,    -1,
     118,   106,   107,   108,   109,   110,   111,   112,    -1,   101,
     102,   116,    -1,   118,   106,   107,   108,   109,   110,   111,
     112,    -1,   101,   102,   116,    -1,   118,   106,   107,   108,
     109,   110,   111,   112,    -1,   101,   102,   116,    -1,   118,
     106,   107,   108,   109,   110,   111,   112,    -1,   101,   102,
     116,    -1,   118,   106,   107,   108,   109,   110,   111,   112,
      -1,   101,   102,   116,    -1,   118,   106,   107,   108,   109,
     110,   111,   112,    -1,   101,   102,   116,    -1,   118,   106,
     107,   108,   109,   110,   111,   112,    -1,   101,   102,   116,
      -1,   118,   106,   107,   108,   109,   110,   111,   112,    -1,
     101,   102,   116,    -1,   118,   106,   107,   108,   109,   110,
     111,   112,    -1,   101,   102,   116,    -1,   118,   106,   107,
     108,   109,   110,   111,   112,    -1,   101,   102,   116,    -1,
     118,   106,   107,   108,   109,   110,   111,   112,    -1,   101,
     102,   116,    -1,   118,   106,   107,   108,   109,   110,   111,
     112,    -1,   101,   102,   116,    -1,   118,   106,   107,   108,
     109,   110,   111,   112,    -1,   101,   102,   116,    -1,   118,
     106,   107,   108,   109,   110,   111,   112,    -1,   101,   102,
     116,    -1,   118,   106,   107,   108,   109,   110,   111,   112,
      -1,   101,   102,   116,    -1,   118,   106,   107,   108,   109,
     110,   111,   112,    -1,   101,   102,   116,    -1,   118,   106,
     107,   108,   109,   110,   111,   112,    -1,   101,   102,   116,
      -1,   118,   106,   107,   108,   109,   110,   111,   112,    -1,
      -1,    -1,   116
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    17,    18,    19,    20,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    34,    35,    36,    37,
      38,    48,    49,    50,    51,    54,    55,    56,    58,    60,
      63,    65,    66,    69,    70,    71,    72,    73,    76,    77,
      78,    79,    80,    81,    82,    83,    86,    87,    89,    91,
      92,    94,    95,    96,   116,   124,   125,   127,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   116,    46,    93,   116,
     116,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    57,    65,   104,   105,   128,   116,     3,     4,
       5,     6,     7,    67,    93,   103,   104,   105,   107,   116,
     119,   120,   128,   178,     3,     4,     5,     6,     7,    67,
      93,   103,   104,   105,   107,   110,   116,   119,   120,   122,
     128,   179,    67,   110,   116,   179,    93,    93,    93,    14,
      25,    33,    40,    41,    42,    43,    44,    45,    47,    88,
      98,   116,   116,   116,   116,   116,   116,   116,   116,   116,
      52,   116,   179,    67,    67,   179,    59,    61,    62,   179,
      53,    64,    74,    75,   116,   179,    68,    99,   179,   179,
      15,    16,   126,   126,   126,   116,   179,   179,   116,   116,
      93,   116,    17,    31,    68,    84,    85,   116,   116,   179,
      16,    67,    99,   116,    67,    90,    67,   116,   116,   179,
     179,    17,    18,    19,    20,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    34,    35,    36,    37,    38,    48,
      49,    50,    51,    54,    55,    56,    65,    66,    70,    71,
      72,    73,    76,    77,    78,    79,    80,    81,    82,    83,
      86,    87,    89,    94,    95,    96,   116,   127,   179,     0,
     125,   116,    93,    93,   116,   117,   117,   117,   117,   117,
     117,   126,   117,   117,   117,   178,   178,   178,   101,   102,
     106,   107,   108,   109,   110,   111,   112,   118,   116,   179,
     179,   179,   179,   179,   101,   102,   106,   107,   108,   109,
     110,   111,   112,   116,   118,   179,   116,   116,   116,   116,
     116,   116,   116,   152,   179,   152,   152,   152,   116,   116,
     116,    67,   116,    67,    99,   116,   116,   179,   116,    67,
     179,   107,   110,   179,   116,    93,   116,   116,   116,   116,
     179,   126,   116,   117,   116,   179,   116,   116,   116,   179,
     116,   116,   116,   116,   116,   116,   116,   179,   116,   179,
     179,   116,   116,   116,   116,   116,   179,    67,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     179,   179,   179,   179,   179,   179,   116,   179,   179,   179,
     121,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   121,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   116,   152,   152,   152,   152,   116,    67,   116,
      67,   116,   116,   116,   179,   179,   179,   179,   116,   116,
     179,   116,    93,   116,   116,   116,   179,   116,   179,   116,
     179,   116,   116,   179,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116
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

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


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
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  YYUSE (yytype);
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
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
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
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
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

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

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

  YYUSE (yytype);
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

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

       Refer to the stacks through separate pointers, to allow yyoverflow
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
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
        case 53:
/* Line 1787 of yacc.c  */
#line 195 "parser.y"
    {
      }
    break;

  case 55:
/* Line 1787 of yacc.c  */
#line 202 "parser.y"
    { (yyval.bval)=(yyvsp[(1) - (1)].bval); }
    break;

  case 57:
/* Line 1787 of yacc.c  */
#line 208 "parser.y"
    { (yyval.sval)=(yyvsp[(1) - (1)].sval); }
    break;

  case 63:
/* Line 1787 of yacc.c  */
#line 218 "parser.y"
    { (yyval.uval)=(yyvsp[(1) - (1)].uval); }
    break;

  case 64:
/* Line 1787 of yacc.c  */
#line 223 "parser.y"
    {
          bx_dbg_timebp_command(0, (yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 65:
/* Line 1787 of yacc.c  */
#line 228 "parser.y"
    {
          bx_dbg_timebp_command(1, (yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 66:
/* Line 1787 of yacc.c  */
#line 236 "parser.y"
    {
          bx_dbg_modebp_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 67:
/* Line 1787 of yacc.c  */
#line 244 "parser.y"
    {
          bx_dbg_vmexitbp_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 68:
/* Line 1787 of yacc.c  */
#line 252 "parser.y"
    {
          bx_dbg_show_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 69:
/* Line 1787 of yacc.c  */
#line 257 "parser.y"
    {
          bx_dbg_show_command("off");
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 70:
/* Line 1787 of yacc.c  */
#line 262 "parser.y"
    {
          bx_dbg_show_param_command((yyvsp[(2) - (3)].sval));
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 71:
/* Line 1787 of yacc.c  */
#line 267 "parser.y"
    {
          bx_dbg_show_command(0);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 72:
/* Line 1787 of yacc.c  */
#line 275 "parser.y"
    {
          bx_dbg_xlate_address((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 73:
/* Line 1787 of yacc.c  */
#line 283 "parser.y"
    {
          bx_dbg_tlb_lookup((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 74:
/* Line 1787 of yacc.c  */
#line 291 "parser.y"
    {
          bx_dbg_ptime_command();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 75:
/* Line 1787 of yacc.c  */
#line 299 "parser.y"
    {
          bx_dbg_trace_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 76:
/* Line 1787 of yacc.c  */
#line 307 "parser.y"
    {
          bx_dbg_trace_reg_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 77:
/* Line 1787 of yacc.c  */
#line 315 "parser.y"
    {
          bx_dbg_trace_mem_command((yyvsp[(2) - (3)].bval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 78:
/* Line 1787 of yacc.c  */
#line 323 "parser.y"
    {
          bx_dbg_print_stack_command(16);
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 79:
/* Line 1787 of yacc.c  */
#line 328 "parser.y"
    {
          bx_dbg_print_stack_command((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 80:
/* Line 1787 of yacc.c  */
#line 336 "parser.y"
    {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 81:
/* Line 1787 of yacc.c  */
#line 341 "parser.y"
    {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 82:
/* Line 1787 of yacc.c  */
#line 346 "parser.y"
    {
          bx_dbg_print_watchpoints();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 83:
/* Line 1787 of yacc.c  */
#line 351 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (4)].uval), 1); /* BX_READ */
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 84:
/* Line 1787 of yacc.c  */
#line 356 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (4)].uval), 1); /* BX_READ */
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 85:
/* Line 1787 of yacc.c  */
#line 361 "parser.y"
    {
          bx_dbg_watch(1, (yyvsp[(3) - (4)].uval), 1); /* BX_WRITE */
          free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 86:
/* Line 1787 of yacc.c  */
#line 366 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval)); /* BX_READ */
          free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 87:
/* Line 1787 of yacc.c  */
#line 371 "parser.y"
    {
          bx_dbg_watch(0, (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval)); /* BX_READ */
          free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 88:
/* Line 1787 of yacc.c  */
#line 376 "parser.y"
    {
          bx_dbg_watch(1, (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval)); /* BX_WRITE */
          free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 89:
/* Line 1787 of yacc.c  */
#line 381 "parser.y"
    {
          bx_dbg_unwatch_all();
          free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 90:
/* Line 1787 of yacc.c  */
#line 386 "parser.y"
    {
          bx_dbg_unwatch((yyvsp[(2) - (3)].uval));
          free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 91:
/* Line 1787 of yacc.c  */
#line 394 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(2) - (3)].sval), 0, 0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 92:
/* Line 1787 of yacc.c  */
#line 399 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(2) - (4)].sval), 0, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 93:
/* Line 1787 of yacc.c  */
#line 404 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(3) - (4)].sval), 1, 0);
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 94:
/* Line 1787 of yacc.c  */
#line 409 "parser.y"
    {
        bx_dbg_symbol_command((yyvsp[(3) - (5)].sval), 1, (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval)); free((yyvsp[(3) - (5)].sval));
      }
    break;

  case 95:
/* Line 1787 of yacc.c  */
#line 417 "parser.y"
    {
        bx_dbg_where_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 96:
/* Line 1787 of yacc.c  */
#line 425 "parser.y"
    {
        bx_dbg_print_string_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 97:
/* Line 1787 of yacc.c  */
#line 433 "parser.y"
    {
        bx_dbg_continue_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 98:
/* Line 1787 of yacc.c  */
#line 441 "parser.y"
    {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 99:
/* Line 1787 of yacc.c  */
#line 446 "parser.y"
    {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 100:
/* Line 1787 of yacc.c  */
#line 451 "parser.y"
    {
        bx_dbg_stepN_command(-1, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 101:
/* Line 1787 of yacc.c  */
#line 456 "parser.y"
    {
        bx_dbg_stepN_command((yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 102:
/* Line 1787 of yacc.c  */
#line 464 "parser.y"
    {
        bx_dbg_step_over_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 103:
/* Line 1787 of yacc.c  */
#line 472 "parser.y"
    {
        bx_dbg_set_auto_disassemble((yyvsp[(3) - (4)].bval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 104:
/* Line 1787 of yacc.c  */
#line 477 "parser.y"
    {
        bx_dbg_set_symbol_command((yyvsp[(2) - (5)].sval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 105:
/* Line 1787 of yacc.c  */
#line 482 "parser.y"
    { 
        bx_dbg_set_reg8l_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 106:
/* Line 1787 of yacc.c  */
#line 486 "parser.y"
    { 
        bx_dbg_set_reg8h_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 107:
/* Line 1787 of yacc.c  */
#line 490 "parser.y"
    { 
        bx_dbg_set_reg16_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 108:
/* Line 1787 of yacc.c  */
#line 494 "parser.y"
    { 
        bx_dbg_set_reg32_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 109:
/* Line 1787 of yacc.c  */
#line 498 "parser.y"
    { 
        bx_dbg_set_reg64_value((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 110:
/* Line 1787 of yacc.c  */
#line 502 "parser.y"
    { 
        bx_dbg_set_rip_value((yyvsp[(4) - (5)].uval));
      }
    break;

  case 111:
/* Line 1787 of yacc.c  */
#line 506 "parser.y"
    { 
        bx_dbg_set_rip_value((yyvsp[(4) - (5)].uval));
      }
    break;

  case 112:
/* Line 1787 of yacc.c  */
#line 510 "parser.y"
    { 
        bx_dbg_load_segreg((yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
      }
    break;

  case 113:
/* Line 1787 of yacc.c  */
#line 517 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 114:
/* Line 1787 of yacc.c  */
#line 522 "parser.y"
    {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[(2) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval));
      }
    break;

  case 115:
/* Line 1787 of yacc.c  */
#line 527 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 116:
/* Line 1787 of yacc.c  */
#line 532 "parser.y"
    {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 117:
/* Line 1787 of yacc.c  */
#line 537 "parser.y"
    {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 118:
/* Line 1787 of yacc.c  */
#line 542 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 119:
/* Line 1787 of yacc.c  */
#line 547 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 120:
/* Line 1787 of yacc.c  */
#line 552 "parser.y"
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 121:
/* Line 1787 of yacc.c  */
#line 560 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 122:
/* Line 1787 of yacc.c  */
#line 568 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 123:
/* Line 1787 of yacc.c  */
#line 573 "parser.y"
    {
        bx_dbg_info_symbols_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 124:
/* Line 1787 of yacc.c  */
#line 581 "parser.y"
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 125:
/* Line 1787 of yacc.c  */
#line 586 "parser.y"
    {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 126:
/* Line 1787 of yacc.c  */
#line 591 "parser.y"
    {
        bx_dbg_info_idt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 127:
/* Line 1787 of yacc.c  */
#line 596 "parser.y"
    {
        bx_dbg_info_ivt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 128:
/* Line 1787 of yacc.c  */
#line 601 "parser.y"
    {
        bx_dbg_info_gdt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 129:
/* Line 1787 of yacc.c  */
#line 606 "parser.y"
    {
        bx_dbg_info_ldt_command((yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 130:
/* Line 1787 of yacc.c  */
#line 611 "parser.y"
    {
        bx_dbg_dump_table();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 131:
/* Line 1787 of yacc.c  */
#line 616 "parser.y"
    {
        bx_dbg_info_tss_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 132:
/* Line 1787 of yacc.c  */
#line 621 "parser.y"
    {
        bx_dbg_info_flags();
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 133:
/* Line 1787 of yacc.c  */
#line 626 "parser.y"
    {
        bx_dbg_info_linux_command();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 134:
/* Line 1787 of yacc.c  */
#line 631 "parser.y"
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 135:
/* Line 1787 of yacc.c  */
#line 636 "parser.y"
    {
        bx_dbg_info_symbols_command((yyvsp[(3) - (4)].sval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 136:
/* Line 1787 of yacc.c  */
#line 641 "parser.y"
    {
        bx_dbg_info_device("", "");
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 137:
/* Line 1787 of yacc.c  */
#line 646 "parser.y"
    {
        bx_dbg_info_device((yyvsp[(3) - (4)].sval), "");
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 138:
/* Line 1787 of yacc.c  */
#line 651 "parser.y"
    {
        bx_dbg_info_device((yyvsp[(3) - (5)].sval), (yyvsp[(4) - (5)].sval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 139:
/* Line 1787 of yacc.c  */
#line 656 "parser.y"
    {
        bx_dbg_info_device((yyvsp[(3) - (4)].sval), "");
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 140:
/* Line 1787 of yacc.c  */
#line 661 "parser.y"
    {
        bx_dbg_info_device((yyvsp[(3) - (5)].sval), (yyvsp[(4) - (5)].sval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 141:
/* Line 1787 of yacc.c  */
#line 668 "parser.y"
    { (yyval.uval) = EMPTY_ARG; }
    break;

  case 143:
/* Line 1787 of yacc.c  */
#line 673 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 144:
/* Line 1787 of yacc.c  */
#line 681 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 145:
/* Line 1787 of yacc.c  */
#line 689 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 146:
/* Line 1787 of yacc.c  */
#line 697 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 147:
/* Line 1787 of yacc.c  */
#line 705 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 148:
/* Line 1787 of yacc.c  */
#line 713 "parser.y"
    {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 149:
/* Line 1787 of yacc.c  */
#line 721 "parser.y"
    {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 150:
/* Line 1787 of yacc.c  */
#line 729 "parser.y"
    {
        bx_dbg_info_control_regs_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 151:
/* Line 1787 of yacc.c  */
#line 737 "parser.y"
    {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 152:
/* Line 1787 of yacc.c  */
#line 745 "parser.y"
    {
        bx_dbg_del_breakpoint_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 153:
/* Line 1787 of yacc.c  */
#line 753 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[(2) - (3)].uval), 1);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 154:
/* Line 1787 of yacc.c  */
#line 760 "parser.y"
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[(2) - (3)].uval), 0);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 155:
/* Line 1787 of yacc.c  */
#line 768 "parser.y"
    {
        bx_dbg_quit_command();
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 156:
/* Line 1787 of yacc.c  */
#line 776 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (4)].sval), (yyvsp[(2) - (4)].sval),1, (yyvsp[(3) - (4)].uval), 1);
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 157:
/* Line 1787 of yacc.c  */
#line 781 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (3)].sval), (yyvsp[(2) - (3)].sval),1, 0, 0);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 158:
/* Line 1787 of yacc.c  */
#line 786 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (3)].sval), NULL,0, (yyvsp[(2) - (3)].uval), 1);
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 159:
/* Line 1787 of yacc.c  */
#line 791 "parser.y"
    {
        bx_dbg_examine_command((yyvsp[(1) - (2)].sval), NULL,0, 0, 0);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 160:
/* Line 1787 of yacc.c  */
#line 799 "parser.y"
    {
        bx_dbg_restore_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].sval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval)); free((yyvsp[(3) - (4)].sval));
      }
    break;

  case 161:
/* Line 1787 of yacc.c  */
#line 807 "parser.y"
    {
        bx_dbg_writemem_command((yyvsp[(2) - (5)].sval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 162:
/* Line 1787 of yacc.c  */
#line 815 "parser.y"
    {
        bx_dbg_setpmem_command((yyvsp[(2) - (5)].uval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval));
      }
    break;

  case 163:
/* Line 1787 of yacc.c  */
#line 823 "parser.y"
    {
        bx_dbg_query_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 164:
/* Line 1787 of yacc.c  */
#line 831 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (3)].sval), 1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 165:
/* Line 1787 of yacc.c  */
#line 836 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 166:
/* Line 1787 of yacc.c  */
#line 841 "parser.y"
    {
        bx_dbg_take_command((yyvsp[(2) - (3)].sval), 1);
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 167:
/* Line 1787 of yacc.c  */
#line 849 "parser.y"
    {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[(1) - (2)].sval));
      }
    break;

  case 168:
/* Line 1787 of yacc.c  */
#line 854 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[(2) - (3)].uval), (yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 169:
/* Line 1787 of yacc.c  */
#line 859 "parser.y"
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 170:
/* Line 1787 of yacc.c  */
#line 864 "parser.y"
    {
        bx_dbg_disassemble_current((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 171:
/* Line 1787 of yacc.c  */
#line 869 "parser.y"
    {
        bx_dbg_disassemble_command((yyvsp[(2) - (4)].sval), (yyvsp[(3) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 172:
/* Line 1787 of yacc.c  */
#line 874 "parser.y"
    {
        bx_dbg_disassemble_command((yyvsp[(2) - (5)].sval), (yyvsp[(3) - (5)].uval), (yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 173:
/* Line 1787 of yacc.c  */
#line 879 "parser.y"
    {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 174:
/* Line 1787 of yacc.c  */
#line 884 "parser.y"
    {
        bx_dbg_disassemble_hex_mode_switch((yyvsp[(3) - (4)].bval));
        free((yyvsp[(1) - (4)].sval)); free((yyvsp[(2) - (4)].sval));
      }
    break;

  case 175:
/* Line 1787 of yacc.c  */
#line 889 "parser.y"
    {
        bx_dbg_set_disassemble_size((yyvsp[(4) - (5)].uval));
        free((yyvsp[(1) - (5)].sval)); free((yyvsp[(2) - (5)].sval));
      }
    break;

  case 176:
/* Line 1787 of yacc.c  */
#line 897 "parser.y"
    {
        bx_dbg_instrument_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 177:
/* Line 1787 of yacc.c  */
#line 902 "parser.y"
    {
        bx_dbg_instrument_command((yyvsp[(2) - (3)].sval));
        free((yyvsp[(1) - (3)].sval)); free((yyvsp[(2) - (3)].sval));
      }
    break;

  case 178:
/* Line 1787 of yacc.c  */
#line 910 "parser.y"
    {
        bx_dbg_doit_command((yyvsp[(2) - (3)].uval));
        free((yyvsp[(1) - (3)].sval));
      }
    break;

  case 179:
/* Line 1787 of yacc.c  */
#line 918 "parser.y"
    {
        bx_dbg_crc_command((yyvsp[(2) - (4)].uval), (yyvsp[(3) - (4)].uval));
        free((yyvsp[(1) - (4)].sval));
      }
    break;

  case 180:
/* Line 1787 of yacc.c  */
#line 926 "parser.y"
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 181:
/* Line 1787 of yacc.c  */
#line 931 "parser.y"
    {
         dbg_printf("c|cont|continue - continue executing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 182:
/* Line 1787 of yacc.c  */
#line 936 "parser.y"
    {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 183:
/* Line 1787 of yacc.c  */
#line 943 "parser.y"
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 184:
/* Line 1787 of yacc.c  */
#line 948 "parser.y"
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 185:
/* Line 1787 of yacc.c  */
#line 953 "parser.y"
    {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 186:
/* Line 1787 of yacc.c  */
#line 958 "parser.y"
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 187:
/* Line 1787 of yacc.c  */
#line 963 "parser.y"
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 188:
/* Line 1787 of yacc.c  */
#line 968 "parser.y"
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 189:
/* Line 1787 of yacc.c  */
#line 973 "parser.y"
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 190:
/* Line 1787 of yacc.c  */
#line 978 "parser.y"
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 191:
/* Line 1787 of yacc.c  */
#line 983 "parser.y"
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 192:
/* Line 1787 of yacc.c  */
#line 988 "parser.y"
    {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 193:
/* Line 1787 of yacc.c  */
#line 993 "parser.y"
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 194:
/* Line 1787 of yacc.c  */
#line 998 "parser.y"
    {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 195:
/* Line 1787 of yacc.c  */
#line 1004 "parser.y"
    {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 196:
/* Line 1787 of yacc.c  */
#line 1010 "parser.y"
    {
         dbg_printf("trace-mem on  - print all memory accesses occured during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 197:
/* Line 1787 of yacc.c  */
#line 1016 "parser.y"
    {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 198:
/* Line 1787 of yacc.c  */
#line 1024 "parser.y"
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 199:
/* Line 1787 of yacc.c  */
#line 1029 "parser.y"
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 200:
/* Line 1787 of yacc.c  */
#line 1034 "parser.y"
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 201:
/* Line 1787 of yacc.c  */
#line 1039 "parser.y"
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 202:
/* Line 1787 of yacc.c  */
#line 1044 "parser.y"
    {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 203:
/* Line 1787 of yacc.c  */
#line 1049 "parser.y"
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 204:
/* Line 1787 of yacc.c  */
#line 1054 "parser.y"
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 205:
/* Line 1787 of yacc.c  */
#line 1059 "parser.y"
    {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 206:
/* Line 1787 of yacc.c  */
#line 1064 "parser.y"
    {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 207:
/* Line 1787 of yacc.c  */
#line 1069 "parser.y"
    {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 208:
/* Line 1787 of yacc.c  */
#line 1074 "parser.y"
    {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 209:
/* Line 1787 of yacc.c  */
#line 1079 "parser.y"
    {
         dbg_printf("zmm - print AVX-512 state\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 210:
/* Line 1787 of yacc.c  */
#line 1084 "parser.y"
    {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 211:
/* Line 1787 of yacc.c  */
#line 1089 "parser.y"
    {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 212:
/* Line 1787 of yacc.c  */
#line 1094 "parser.y"
    {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 213:
/* Line 1787 of yacc.c  */
#line 1099 "parser.y"
    {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 214:
/* Line 1787 of yacc.c  */
#line 1104 "parser.y"
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 215:
/* Line 1787 of yacc.c  */
#line 1109 "parser.y"
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

  case 216:
/* Line 1787 of yacc.c  */
#line 1119 "parser.y"
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

  case 217:
/* Line 1787 of yacc.c  */
#line 1130 "parser.y"
    {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 218:
/* Line 1787 of yacc.c  */
#line 1136 "parser.y"
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

  case 219:
/* Line 1787 of yacc.c  */
#line 1149 "parser.y"
    {
         dbg_printf("instrument <command> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command>\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 220:
/* Line 1787 of yacc.c  */
#line 1154 "parser.y"
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

  case 221:
/* Line 1787 of yacc.c  */
#line 1165 "parser.y"
    {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 222:
/* Line 1787 of yacc.c  */
#line 1170 "parser.y"
    {
         dbg_printf("info break - show information about current breakpoint status\n");
         dbg_printf("info cpu - show dump of all cpu registers\n");
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

  case 223:
/* Line 1787 of yacc.c  */
#line 1186 "parser.y"
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

  case 224:
/* Line 1787 of yacc.c  */
#line 1198 "parser.y"
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

  case 225:
/* Line 1787 of yacc.c  */
#line 1208 "parser.y"
    {
         bx_dbg_print_help();
         free((yyvsp[(1) - (3)].sval));free((yyvsp[(2) - (3)].sval));
       }
    break;

  case 226:
/* Line 1787 of yacc.c  */
#line 1213 "parser.y"
    {
         bx_dbg_print_help();
         free((yyvsp[(1) - (2)].sval));
       }
    break;

  case 227:
/* Line 1787 of yacc.c  */
#line 1221 "parser.y"
    {
     bx_dbg_calc_command((yyvsp[(2) - (3)].uval));
     free((yyvsp[(1) - (3)].sval));
   }
    break;

  case 228:
/* Line 1787 of yacc.c  */
#line 1238 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (1)].uval); }
    break;

  case 229:
/* Line 1787 of yacc.c  */
#line 1239 "parser.y"
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[(1) - (1)].sval)); free((yyvsp[(1) - (1)].sval));}
    break;

  case 230:
/* Line 1787 of yacc.c  */
#line 1240 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 231:
/* Line 1787 of yacc.c  */
#line 1241 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 232:
/* Line 1787 of yacc.c  */
#line 1242 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 233:
/* Line 1787 of yacc.c  */
#line 1243 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 234:
/* Line 1787 of yacc.c  */
#line 1244 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 235:
/* Line 1787 of yacc.c  */
#line 1245 "parser.y"
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 236:
/* Line 1787 of yacc.c  */
#line 1246 "parser.y"
    { (yyval.uval) = bx_dbg_get_ip (); }
    break;

  case 237:
/* Line 1787 of yacc.c  */
#line 1247 "parser.y"
    { (yyval.uval) = bx_dbg_get_eip(); }
    break;

  case 238:
/* Line 1787 of yacc.c  */
#line 1248 "parser.y"
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
    break;

  case 239:
/* Line 1787 of yacc.c  */
#line 1249 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) + (yyvsp[(3) - (3)].uval); }
    break;

  case 240:
/* Line 1787 of yacc.c  */
#line 1250 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) - (yyvsp[(3) - (3)].uval); }
    break;

  case 241:
/* Line 1787 of yacc.c  */
#line 1251 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) * (yyvsp[(3) - (3)].uval); }
    break;

  case 242:
/* Line 1787 of yacc.c  */
#line 1252 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) / (yyvsp[(3) - (3)].uval); }
    break;

  case 243:
/* Line 1787 of yacc.c  */
#line 1253 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) >> (yyvsp[(3) - (3)].uval); }
    break;

  case 244:
/* Line 1787 of yacc.c  */
#line 1254 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) << (yyvsp[(3) - (3)].uval); }
    break;

  case 245:
/* Line 1787 of yacc.c  */
#line 1255 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) | (yyvsp[(3) - (3)].uval); }
    break;

  case 246:
/* Line 1787 of yacc.c  */
#line 1256 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) ^ (yyvsp[(3) - (3)].uval); }
    break;

  case 247:
/* Line 1787 of yacc.c  */
#line 1257 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) & (yyvsp[(3) - (3)].uval); }
    break;

  case 248:
/* Line 1787 of yacc.c  */
#line 1258 "parser.y"
    { (yyval.uval) = !(yyvsp[(2) - (2)].uval); }
    break;

  case 249:
/* Line 1787 of yacc.c  */
#line 1259 "parser.y"
    { (yyval.uval) = -(yyvsp[(2) - (2)].uval); }
    break;

  case 250:
/* Line 1787 of yacc.c  */
#line 1260 "parser.y"
    { (yyval.uval) = (yyvsp[(2) - (3)].uval); }
    break;

  case 251:
/* Line 1787 of yacc.c  */
#line 1266 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (1)].uval); }
    break;

  case 252:
/* Line 1787 of yacc.c  */
#line 1267 "parser.y"
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[(1) - (1)].sval)); free((yyvsp[(1) - (1)].sval));}
    break;

  case 253:
/* Line 1787 of yacc.c  */
#line 1268 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 254:
/* Line 1787 of yacc.c  */
#line 1269 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 255:
/* Line 1787 of yacc.c  */
#line 1270 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 256:
/* Line 1787 of yacc.c  */
#line 1271 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 257:
/* Line 1787 of yacc.c  */
#line 1272 "parser.y"
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 258:
/* Line 1787 of yacc.c  */
#line 1273 "parser.y"
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[(1) - (1)].uval)); }
    break;

  case 259:
/* Line 1787 of yacc.c  */
#line 1274 "parser.y"
    { (yyval.uval) = bx_dbg_get_ip (); }
    break;

  case 260:
/* Line 1787 of yacc.c  */
#line 1275 "parser.y"
    { (yyval.uval) = bx_dbg_get_eip(); }
    break;

  case 261:
/* Line 1787 of yacc.c  */
#line 1276 "parser.y"
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
    break;

  case 262:
/* Line 1787 of yacc.c  */
#line 1277 "parser.y"
    { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[(1) - (3)].uval), (yyvsp[(3) - (3)].uval)); }
    break;

  case 263:
/* Line 1787 of yacc.c  */
#line 1278 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) + (yyvsp[(3) - (3)].uval); }
    break;

  case 264:
/* Line 1787 of yacc.c  */
#line 1279 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) - (yyvsp[(3) - (3)].uval); }
    break;

  case 265:
/* Line 1787 of yacc.c  */
#line 1280 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) * (yyvsp[(3) - (3)].uval); }
    break;

  case 266:
/* Line 1787 of yacc.c  */
#line 1281 "parser.y"
    { (yyval.uval) = ((yyvsp[(3) - (3)].uval) != 0) ? (yyvsp[(1) - (3)].uval) / (yyvsp[(3) - (3)].uval) : 0; }
    break;

  case 267:
/* Line 1787 of yacc.c  */
#line 1282 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) >> (yyvsp[(3) - (3)].uval); }
    break;

  case 268:
/* Line 1787 of yacc.c  */
#line 1283 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) << (yyvsp[(3) - (3)].uval); }
    break;

  case 269:
/* Line 1787 of yacc.c  */
#line 1284 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) | (yyvsp[(3) - (3)].uval); }
    break;

  case 270:
/* Line 1787 of yacc.c  */
#line 1285 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) ^ (yyvsp[(3) - (3)].uval); }
    break;

  case 271:
/* Line 1787 of yacc.c  */
#line 1286 "parser.y"
    { (yyval.uval) = (yyvsp[(1) - (3)].uval) & (yyvsp[(3) - (3)].uval); }
    break;

  case 272:
/* Line 1787 of yacc.c  */
#line 1287 "parser.y"
    { (yyval.uval) = !(yyvsp[(2) - (2)].uval); }
    break;

  case 273:
/* Line 1787 of yacc.c  */
#line 1288 "parser.y"
    { (yyval.uval) = -(yyvsp[(2) - (2)].uval); }
    break;

  case 274:
/* Line 1787 of yacc.c  */
#line 1289 "parser.y"
    { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[(2) - (2)].uval)); }
    break;

  case 275:
/* Line 1787 of yacc.c  */
#line 1290 "parser.y"
    { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[(2) - (2)].uval)); }
    break;

  case 276:
/* Line 1787 of yacc.c  */
#line 1291 "parser.y"
    { (yyval.uval) = (yyvsp[(2) - (3)].uval); }
    break;


/* Line 1787 of yacc.c  */
#line 4192 "y.tab.c"
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
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


/* Line 2050 of yacc.c  */
#line 1294 "parser.y"

#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
