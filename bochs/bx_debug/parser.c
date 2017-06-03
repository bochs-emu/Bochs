/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.4"

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
#define yydebug         bxdebug
#define yynerrs         bxnerrs

#define yylval          bxlval
#define yychar          bxchar

/* Copy the first part of user declarations.  */
#line 5 "parser.y" /* yacc.c:339  */

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

#if BX_DEBUGGER

#line 82 "y.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
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
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int bxdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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
    BX_TOKEN_OPMASK_REG = 269,
    BX_TOKEN_FLAGS = 270,
    BX_TOKEN_ON = 271,
    BX_TOKEN_OFF = 272,
    BX_TOKEN_CONTINUE = 273,
    BX_TOKEN_IF = 274,
    BX_TOKEN_STEPN = 275,
    BX_TOKEN_STEP_OVER = 276,
    BX_TOKEN_SET = 277,
    BX_TOKEN_DEBUGGER = 278,
    BX_TOKEN_LIST_BREAK = 279,
    BX_TOKEN_VBREAKPOINT = 280,
    BX_TOKEN_LBREAKPOINT = 281,
    BX_TOKEN_PBREAKPOINT = 282,
    BX_TOKEN_DEL_BREAKPOINT = 283,
    BX_TOKEN_ENABLE_BREAKPOINT = 284,
    BX_TOKEN_DISABLE_BREAKPOINT = 285,
    BX_TOKEN_INFO = 286,
    BX_TOKEN_QUIT = 287,
    BX_TOKEN_R = 288,
    BX_TOKEN_REGS = 289,
    BX_TOKEN_CPU = 290,
    BX_TOKEN_FPU = 291,
    BX_TOKEN_MMX = 292,
    BX_TOKEN_XMM = 293,
    BX_TOKEN_YMM = 294,
    BX_TOKEN_ZMM = 295,
    BX_TOKEN_AVX = 296,
    BX_TOKEN_IDT = 297,
    BX_TOKEN_IVT = 298,
    BX_TOKEN_GDT = 299,
    BX_TOKEN_LDT = 300,
    BX_TOKEN_TSS = 301,
    BX_TOKEN_TAB = 302,
    BX_TOKEN_ALL = 303,
    BX_TOKEN_LINUX = 304,
    BX_TOKEN_DEBUG_REGS = 305,
    BX_TOKEN_CONTROL_REGS = 306,
    BX_TOKEN_SEGMENT_REGS = 307,
    BX_TOKEN_EXAMINE = 308,
    BX_TOKEN_XFORMAT = 309,
    BX_TOKEN_DISFORMAT = 310,
    BX_TOKEN_RESTORE = 311,
    BX_TOKEN_WRITEMEM = 312,
    BX_TOKEN_SETPMEM = 313,
    BX_TOKEN_SYMBOLNAME = 314,
    BX_TOKEN_QUERY = 315,
    BX_TOKEN_PENDING = 316,
    BX_TOKEN_TAKE = 317,
    BX_TOKEN_DMA = 318,
    BX_TOKEN_IRQ = 319,
    BX_TOKEN_TLB = 320,
    BX_TOKEN_HEX = 321,
    BX_TOKEN_DISASM = 322,
    BX_TOKEN_INSTRUMENT = 323,
    BX_TOKEN_STRING = 324,
    BX_TOKEN_STOP = 325,
    BX_TOKEN_DOIT = 326,
    BX_TOKEN_CRC = 327,
    BX_TOKEN_TRACE = 328,
    BX_TOKEN_TRACEREG = 329,
    BX_TOKEN_TRACEMEM = 330,
    BX_TOKEN_SWITCH_MODE = 331,
    BX_TOKEN_SIZE = 332,
    BX_TOKEN_PTIME = 333,
    BX_TOKEN_TIMEBP_ABSOLUTE = 334,
    BX_TOKEN_TIMEBP = 335,
    BX_TOKEN_MODEBP = 336,
    BX_TOKEN_VMEXITBP = 337,
    BX_TOKEN_PRINT_STACK = 338,
    BX_TOKEN_WATCH = 339,
    BX_TOKEN_UNWATCH = 340,
    BX_TOKEN_READ = 341,
    BX_TOKEN_WRITE = 342,
    BX_TOKEN_SHOW = 343,
    BX_TOKEN_LOAD_SYMBOLS = 344,
    BX_TOKEN_SYMBOLS = 345,
    BX_TOKEN_LIST_SYMBOLS = 346,
    BX_TOKEN_GLOBAL = 347,
    BX_TOKEN_WHERE = 348,
    BX_TOKEN_PRINT_STRING = 349,
    BX_TOKEN_NUMERIC = 350,
    BX_TOKEN_PAGE = 351,
    BX_TOKEN_HELP = 352,
    BX_TOKEN_XML = 353,
    BX_TOKEN_CALC = 354,
    BX_TOKEN_DEVICE = 355,
    BX_TOKEN_GENERIC = 356,
    BX_TOKEN_RSHIFT = 357,
    BX_TOKEN_LSHIFT = 358,
    BX_TOKEN_EQ = 359,
    BX_TOKEN_NE = 360,
    BX_TOKEN_LE = 361,
    BX_TOKEN_GE = 362,
    BX_TOKEN_REG_IP = 363,
    BX_TOKEN_REG_EIP = 364,
    BX_TOKEN_REG_RIP = 365,
    NOT = 366,
    NEG = 367,
    INDIRECT = 368
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
#define BX_TOKEN_OPMASK_REG 269
#define BX_TOKEN_FLAGS 270
#define BX_TOKEN_ON 271
#define BX_TOKEN_OFF 272
#define BX_TOKEN_CONTINUE 273
#define BX_TOKEN_IF 274
#define BX_TOKEN_STEPN 275
#define BX_TOKEN_STEP_OVER 276
#define BX_TOKEN_SET 277
#define BX_TOKEN_DEBUGGER 278
#define BX_TOKEN_LIST_BREAK 279
#define BX_TOKEN_VBREAKPOINT 280
#define BX_TOKEN_LBREAKPOINT 281
#define BX_TOKEN_PBREAKPOINT 282
#define BX_TOKEN_DEL_BREAKPOINT 283
#define BX_TOKEN_ENABLE_BREAKPOINT 284
#define BX_TOKEN_DISABLE_BREAKPOINT 285
#define BX_TOKEN_INFO 286
#define BX_TOKEN_QUIT 287
#define BX_TOKEN_R 288
#define BX_TOKEN_REGS 289
#define BX_TOKEN_CPU 290
#define BX_TOKEN_FPU 291
#define BX_TOKEN_MMX 292
#define BX_TOKEN_XMM 293
#define BX_TOKEN_YMM 294
#define BX_TOKEN_ZMM 295
#define BX_TOKEN_AVX 296
#define BX_TOKEN_IDT 297
#define BX_TOKEN_IVT 298
#define BX_TOKEN_GDT 299
#define BX_TOKEN_LDT 300
#define BX_TOKEN_TSS 301
#define BX_TOKEN_TAB 302
#define BX_TOKEN_ALL 303
#define BX_TOKEN_LINUX 304
#define BX_TOKEN_DEBUG_REGS 305
#define BX_TOKEN_CONTROL_REGS 306
#define BX_TOKEN_SEGMENT_REGS 307
#define BX_TOKEN_EXAMINE 308
#define BX_TOKEN_XFORMAT 309
#define BX_TOKEN_DISFORMAT 310
#define BX_TOKEN_RESTORE 311
#define BX_TOKEN_WRITEMEM 312
#define BX_TOKEN_SETPMEM 313
#define BX_TOKEN_SYMBOLNAME 314
#define BX_TOKEN_QUERY 315
#define BX_TOKEN_PENDING 316
#define BX_TOKEN_TAKE 317
#define BX_TOKEN_DMA 318
#define BX_TOKEN_IRQ 319
#define BX_TOKEN_TLB 320
#define BX_TOKEN_HEX 321
#define BX_TOKEN_DISASM 322
#define BX_TOKEN_INSTRUMENT 323
#define BX_TOKEN_STRING 324
#define BX_TOKEN_STOP 325
#define BX_TOKEN_DOIT 326
#define BX_TOKEN_CRC 327
#define BX_TOKEN_TRACE 328
#define BX_TOKEN_TRACEREG 329
#define BX_TOKEN_TRACEMEM 330
#define BX_TOKEN_SWITCH_MODE 331
#define BX_TOKEN_SIZE 332
#define BX_TOKEN_PTIME 333
#define BX_TOKEN_TIMEBP_ABSOLUTE 334
#define BX_TOKEN_TIMEBP 335
#define BX_TOKEN_MODEBP 336
#define BX_TOKEN_VMEXITBP 337
#define BX_TOKEN_PRINT_STACK 338
#define BX_TOKEN_WATCH 339
#define BX_TOKEN_UNWATCH 340
#define BX_TOKEN_READ 341
#define BX_TOKEN_WRITE 342
#define BX_TOKEN_SHOW 343
#define BX_TOKEN_LOAD_SYMBOLS 344
#define BX_TOKEN_SYMBOLS 345
#define BX_TOKEN_LIST_SYMBOLS 346
#define BX_TOKEN_GLOBAL 347
#define BX_TOKEN_WHERE 348
#define BX_TOKEN_PRINT_STRING 349
#define BX_TOKEN_NUMERIC 350
#define BX_TOKEN_PAGE 351
#define BX_TOKEN_HELP 352
#define BX_TOKEN_XML 353
#define BX_TOKEN_CALC 354
#define BX_TOKEN_DEVICE 355
#define BX_TOKEN_GENERIC 356
#define BX_TOKEN_RSHIFT 357
#define BX_TOKEN_LSHIFT 358
#define BX_TOKEN_EQ 359
#define BX_TOKEN_NE 360
#define BX_TOKEN_LE 361
#define BX_TOKEN_GE 362
#define BX_TOKEN_REG_IP 363
#define BX_TOKEN_REG_EIP 364
#define BX_TOKEN_REG_RIP 365
#define NOT 366
#define NEG 367
#define INDIRECT 368

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 13 "parser.y" /* yacc.c:355  */

  char    *sval;
  Bit64u   uval;
  bx_bool  bval;

#line 354 "y.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE bxlval;

int bxparse (void);

#endif /* !YY_BX_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 371 "y.tab.c" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
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
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

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
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  293
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2341

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  130
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  57
/* YYNRULES -- Number of rules.  */
#define YYNRULES  287
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  559

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   368

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     123,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   126,     2,     2,     2,     2,   119,     2,
     127,   128,   117,   111,     2,   112,     2,   118,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   125,     2,
     115,   124,   116,     2,   129,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   114,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   113,     2,     2,     2,     2,     2,
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
     105,   106,   107,   108,   109,   110,   120,   121,   122
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   146,   146,   147,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   206,   207,   212,   213,   218,   219,
     220,   221,   222,   223,   228,   233,   241,   249,   257,   262,
     267,   272,   277,   282,   290,   298,   306,   314,   322,   330,
     338,   343,   351,   356,   361,   366,   371,   376,   381,   386,
     391,   396,   401,   409,   414,   419,   424,   432,   440,   448,
     453,   461,   466,   471,   476,   484,   492,   497,   502,   506,
     510,   514,   518,   522,   526,   530,   537,   542,   547,   552,
     557,   562,   567,   572,   580,   588,   593,   601,   606,   611,
     616,   621,   626,   631,   636,   641,   646,   651,   656,   661,
     666,   671,   676,   681,   689,   690,   693,   701,   709,   717,
     725,   733,   741,   749,   757,   765,   773,   780,   788,   796,
     801,   806,   811,   819,   827,   835,   843,   851,   856,   861,
     869,   874,   879,   884,   889,   894,   899,   904,   909,   917,
     922,   930,   938,   946,   951,   957,   964,   969,   974,   979,
     984,   989,   994,   999,  1004,  1009,  1014,  1019,  1025,  1031,
    1037,  1045,  1050,  1055,  1060,  1065,  1070,  1075,  1080,  1085,
    1090,  1095,  1100,  1105,  1110,  1115,  1120,  1125,  1130,  1140,
    1151,  1157,  1170,  1175,  1186,  1191,  1207,  1223,  1233,  1238,
    1246,  1265,  1266,  1267,  1268,  1269,  1270,  1271,  1272,  1273,
    1274,  1275,  1276,  1277,  1278,  1279,  1280,  1281,  1282,  1283,
    1284,  1285,  1286,  1287,  1288,  1294,  1295,  1296,  1297,  1298,
    1299,  1300,  1301,  1302,  1303,  1304,  1305,  1306,  1307,  1308,
    1309,  1310,  1311,  1312,  1313,  1314,  1315,  1316,  1317,  1318,
    1319,  1320,  1321,  1322,  1323,  1324,  1325,  1326
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
  "BX_TOKEN_FS", "BX_TOKEN_GS", "BX_TOKEN_OPMASK_REG", "BX_TOKEN_FLAGS",
  "BX_TOKEN_ON", "BX_TOKEN_OFF", "BX_TOKEN_CONTINUE", "BX_TOKEN_IF",
  "BX_TOKEN_STEPN", "BX_TOKEN_STEP_OVER", "BX_TOKEN_SET",
  "BX_TOKEN_DEBUGGER", "BX_TOKEN_LIST_BREAK", "BX_TOKEN_VBREAKPOINT",
  "BX_TOKEN_LBREAKPOINT", "BX_TOKEN_PBREAKPOINT",
  "BX_TOKEN_DEL_BREAKPOINT", "BX_TOKEN_ENABLE_BREAKPOINT",
  "BX_TOKEN_DISABLE_BREAKPOINT", "BX_TOKEN_INFO", "BX_TOKEN_QUIT",
  "BX_TOKEN_R", "BX_TOKEN_REGS", "BX_TOKEN_CPU", "BX_TOKEN_FPU",
  "BX_TOKEN_MMX", "BX_TOKEN_XMM", "BX_TOKEN_YMM", "BX_TOKEN_ZMM",
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
  "BX_TOKEN_NUMERIC", "BX_TOKEN_PAGE", "BX_TOKEN_HELP", "BX_TOKEN_XML",
  "BX_TOKEN_CALC", "BX_TOKEN_DEVICE", "BX_TOKEN_GENERIC",
  "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT", "BX_TOKEN_EQ", "BX_TOKEN_NE",
  "BX_TOKEN_LE", "BX_TOKEN_GE", "BX_TOKEN_REG_IP", "BX_TOKEN_REG_EIP",
  "BX_TOKEN_REG_RIP", "'+'", "'-'", "'|'", "'^'", "'<'", "'>'", "'*'",
  "'/'", "'&'", "NOT", "NEG", "INDIRECT", "'\\n'", "'='", "':'", "'!'",
  "'('", "')'", "'@'", "$accept", "commands", "command",
  "BX_TOKEN_TOGGLE_ON_OFF", "BX_TOKEN_REGISTERS", "BX_TOKEN_SEGREG",
  "timebp_command", "modebp_command", "vmexitbp_command", "show_command",
  "page_command", "tlb_command", "ptime_command", "trace_command",
  "trace_reg_command", "trace_mem_command", "print_stack_command",
  "watch_point_command", "symbol_command", "where_command",
  "print_string_command", "continue_command", "stepN_command",
  "step_over_command", "set_command", "breakpoint_command",
  "blist_command", "slist_command", "info_command", "optional_numeric",
  "regs_command", "fpu_regs_command", "mmx_regs_command",
  "xmm_regs_command", "ymm_regs_command", "zmm_regs_command",
  "segment_regs_command", "control_regs_command", "debug_regs_command",
  "delete_command", "bpe_command", "bpd_command", "quit_command",
  "examine_command", "restore_command", "writemem_command",
  "setpmem_command", "query_command", "take_command",
  "disassemble_command", "instrument_command", "doit_command",
  "crc_command", "help_command", "calc_command", "vexpression",
  "expression", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
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
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,    43,    45,   124,    94,    60,    62,    42,    47,    38,
     366,   367,   368,    10,    61,    58,    33,    40,    41,    64
};
# endif

#define YYPACT_NINF -149

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-149)))

#define YYTABLE_NINF -286

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1393,    -6,   -33,  -105,    47,  -102,  1150,   746,   798,   -70,
     -64,   -15,  1151,   -75,  -149,  -149,   -38,   -23,   -22,   -16,
     -14,   -13,   -12,   -11,   120,    25,    39,  1075,    52,    11,
    1075,   671,   -34,  1075,  1075,    75,    75,    75,    -5,  1075,
    1075,    -1,    12,   -76,    16,   821,    -8,   -47,   -58,    13,
    1075,  1075,  1473,  1075,  -149,  1313,  -149,    14,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  1075,  -149,    21,   -60,
    -149,  -149,    18,    19,    20,    22,    23,  -149,  -149,  -149,
    -149,  -149,  -149,    24,    75,    28,    34,    48,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    1202,  -149,  1202,  1202,  -149,   258,  -149,  -149,  -149,  -149,
    -149,  -149,    26,  -149,  -149,  -149,  -149,  1075,  1075,  -149,
    1075,  1075,  1075,  -149,  1176,  -149,  1075,  -149,  1472,    50,
      64,    65,    74,    76,    77,  1075,  1075,  1075,  1075,    79,
      80,    81,   -57,   -59,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,   873,  -149,  1496,   129,  1075,   604,    84,
     -54,    89,  1520,   948,    75,    91,    87,  -149,   157,    93,
      94,  1544,   604,  -149,  -149,    95,    97,   100,  -149,  1568,
    1592,  -149,  -149,   104,  -149,   115,  1075,   119,  1075,  1075,
    -149,  -149,  1616,   121,   125,   -27,   127,  -149,  1000,   182,
     130,  -149,  -149,  1640,  1664,   131,   132,   133,   134,   135,
     154,   155,   156,   158,   162,   164,   165,   166,   167,   168,
     169,   176,   180,   181,   183,   184,   185,   186,   187,   198,
     214,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     239,   240,   245,   250,   255,   256,   257,   259,   261,   262,
    -149,   263,  1688,  -149,  -149,  -149,  1712,   264,   265,  -149,
    1075,  1075,  1075,  1075,  1075,  1075,   266,  1075,  1075,  1075,
    -149,  -149,   -30,  1202,  1202,  1202,  1202,  1202,  1202,  1202,
    1202,  1202,  1202,  -149,   267,   267,   267,  1118,   267,  1075,
    1075,  1075,  1075,  1075,  1075,  1075,  1075,  1075,  1075,  1075,
    1075,  1075,  1075,  1075,  -149,  1075,   -86,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  1075,  2216,  1075,  1075,  1075,  -149,
    -149,  -149,   272,  -149,   -55,   -53,  -149,  -149,  1736,  -149,
     273,   604,  1075,  1075,   604,  -149,   274,  -149,  -149,  -149,
    -149,   336,   275,  -149,   210,  -149,  1760,  -149,  -149,  -149,
    1784,  -149,  -149,  -149,  -149,  -149,  -149,  -149,   403,  -149,
     470,   537,  -149,  -149,  -149,   276,  -149,  -149,  -149,  1808,
    1023,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  1832,  1856,  1880,  1904,  1928,  1952,
    -149,  1976,  2000,  2024,  -149,  -149,  -149,     2,     2,     2,
       2,  -149,  -149,  -149,   122,  -149,   -28,   -28,   267,   267,
     267,   267,   667,   667,   667,   667,   667,   667,   -28,   -28,
     -28,  2216,  -149,   277,   278,   279,   281,  -149,   295,  -149,
     296,  -149,  -149,  -149,  2048,   743,    88,  2072,  -149,  -149,
    2096,  -149,   297,  -149,  -149,  -149,  2120,  -149,  2144,  -149,
    2168,  -149,  -149,  -149,  2192,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
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
      31,    32,    33,    34,    50,    51,     0,    99,     0,     0,
     101,   105,     0,     0,     0,     0,     0,    58,    59,    60,
      61,    62,    63,     0,     0,     0,     0,     0,   124,   234,
     233,   235,   236,   237,   238,   232,   231,   240,   241,   242,
       0,   116,     0,     0,   239,     0,   258,   257,   259,   260,
     261,   262,   256,   255,   264,   265,   266,     0,     0,   118,
       0,     0,     0,   263,     0,   256,     0,   121,     0,     0,
       0,     0,     0,     0,     0,   144,   144,   144,   144,     0,
       0,     0,     0,     0,   158,   147,   148,   149,   150,   151,
     154,   153,   152,     0,   162,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   170,     0,     0,
       0,     0,     0,    54,    55,     0,     0,     0,    76,     0,
       0,    66,    67,     0,    80,     0,     0,     0,     0,     0,
      84,    91,     0,     0,     0,     0,     0,    73,     0,     0,
       0,   125,    97,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     229,     0,     0,     1,     2,   146,     0,     0,     0,   102,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     253,   252,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   120,   284,   285,   283,     0,   286,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   119,     0,   285,   122,   155,   156,
     157,   135,   127,   128,   144,   145,   144,   144,   144,   134,
     133,   136,     0,   137,     0,     0,   139,   160,     0,   161,
       0,     0,     0,     0,     0,   166,     0,   167,   169,    75,
     173,     0,     0,   176,     0,   171,     0,   179,   180,   181,
       0,    77,    78,    79,    65,    64,    81,    83,     0,    82,
       0,     0,    92,    70,    69,     0,    71,    68,    93,     0,
       0,   126,    98,    74,   184,   185,   186,   223,   193,   187,
     188,   189,   190,   191,   192,   225,   183,   208,   209,   210,
     211,   212,   215,   214,   213,   221,   200,   216,   217,   218,
     222,   196,   197,   198,   199,   201,   203,   202,   194,   195,
     204,   219,   220,   226,   205,   206,   224,   228,   227,   207,
     230,   100,   103,   104,     0,     0,     0,     0,     0,     0,
     106,     0,     0,     0,   254,   247,   248,   243,   244,   249,
     250,   245,   246,   251,     0,   287,   272,   273,   279,   280,
     281,   282,   268,   269,   274,   275,   278,   277,   270,   271,
     276,   267,   123,     0,     0,     0,     0,   138,     0,   142,
       0,   140,   159,   163,     0,   269,   270,     0,   168,   174,
       0,   177,     0,   172,   182,    85,     0,    86,     0,    87,
       0,    72,    94,    95,     0,   109,   108,   110,   111,   112,
     107,   113,   114,   115,   117,   129,   130,   131,   132,   143,
     141,   164,   165,   175,   178,    88,    89,    90,    96
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -149,  -149,    90,   -29,   329,    -2,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -148,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -137,    -7
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    55,    56,   215,    57,   163,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,   354,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   145,   355
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     164,   168,   127,   310,   144,   311,   312,   216,   217,   233,
     364,   240,   362,   106,   508,   108,   510,   195,   111,   223,
     198,   128,   238,   202,   208,   169,   211,   212,   356,   357,
     358,   170,   219,   220,   225,   298,   209,   502,   232,   345,
     234,   376,   365,   243,   244,   239,   292,   224,   184,   226,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   235,   109,   299,   366,   241,   363,   210,   509,   377,
     511,   405,   313,   314,   200,   201,   331,   332,   333,   334,
     171,   315,   316,   317,   318,   185,   227,   319,   320,   321,
     110,   213,   214,   236,   196,   306,   406,   345,   474,   296,
     186,   187,   228,   229,   313,   314,   123,   188,   197,   189,
     190,   191,   192,   199,   124,   237,   297,   107,   218,   319,
     320,   321,   221,   146,   147,   148,   149,   150,   117,   118,
     119,   120,   121,   122,   151,   222,   242,   295,   144,   230,
     144,   144,   300,   301,   302,   294,   303,   304,   305,   323,
     324,   325,   307,   326,   327,   328,   125,   126,   308,   346,
     146,   147,   148,   149,   150,   117,   118,   119,   120,   121,
     122,   151,   309,   348,   193,   382,   475,   476,   477,   478,
     479,   480,   481,   482,   483,   484,   368,   349,   350,   165,
     371,   374,  -285,  -285,  -285,  -285,   381,   351,   370,   352,
     353,   386,   359,   360,   361,   390,   503,   375,   504,   505,
     506,   384,   378,   345,   383,   153,   387,   388,   391,   398,
     392,   400,   401,   393,   313,   314,   165,   396,   154,   155,
     156,   409,   157,   315,   316,   317,   318,   158,   397,   319,
     320,   321,   399,   194,   403,   544,   160,   161,   404,   162,
     407,   410,   153,   411,   414,   415,   416,   417,   418,   329,
     330,   331,   332,   333,   334,   154,   155,   156,   335,   372,
     337,   338,   339,   340,   373,   342,   343,   419,   420,   421,
     385,   422,   345,   160,   161,   423,   162,   424,   425,   426,
     427,   428,   429,   464,   465,   466,   467,   468,   469,   430,
     471,   472,   473,   431,   432,   522,   433,   434,   435,   436,
     437,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   438,   486,   487,   488,   489,   490,   491,   492,   493,
     494,   495,   496,   497,   498,   499,   500,   439,   501,   146,
     147,   148,   149,   150,   117,   118,   119,   120,   121,   122,
     151,   440,   441,   442,   443,   444,   445,   446,   447,   448,
     313,   314,   449,   450,   514,   515,   516,   517,   451,   315,
     316,   317,   318,   452,   520,   319,   320,   321,   453,   454,
     455,   291,   456,   322,   457,   458,   459,   462,   463,   470,
       0,   526,   345,   528,   530,   507,   513,   518,   521,   531,
     545,   546,   547,   534,   548,   165,   146,   147,   148,   149,
     150,   117,   118,   119,   120,   121,   122,   151,   549,   550,
     554,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   153,     0,     0,     0,     0,     0,     0,   329,   330,
     331,   332,   333,   334,   154,   155,   156,   335,   372,   337,
     338,   339,   340,   373,   342,   343,     0,     0,     0,   519,
       0,   345,   160,   161,     0,   162,     0,     0,     0,     0,
       0,     0,   165,   146,   147,   148,   149,   150,   117,   118,
     119,   120,   121,   122,   151,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   153,     0,
       0,     0,     0,     0,     0,   329,   330,   331,   332,   333,
     334,   154,   155,   156,   335,   372,   337,   338,   339,   340,
     373,   342,   343,     0,     0,     0,   525,     0,   345,   160,
     161,     0,   162,     0,     0,     0,     0,     0,     0,   165,
     146,   147,   148,   149,   150,   117,   118,   119,   120,   121,
     122,   151,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   153,     0,     0,     0,     0,
       0,     0,   329,   330,   331,   332,   333,   334,   154,   155,
     156,   335,   372,   337,   338,   339,   340,   373,   342,   343,
       0,     0,     0,   527,     0,   345,   160,   161,     0,   162,
       0,     0,     0,     0,     0,     0,   165,   146,   147,   148,
     149,   150,   117,   118,   119,   120,   121,   122,   151,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   153,     0,     0,     0,     0,     0,     0,   329,
     330,   331,   332,   333,   334,   154,   155,   156,   335,   372,
     337,   338,   339,   340,   373,   342,   343,     0,     0,     0,
     529,     0,   345,   160,   161,     0,   162,     0,     0,     0,
       0,     0,     0,   165,   146,   147,   148,   149,   150,   117,
     118,   119,   120,   121,   122,   151,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   153,
       0,     0,     0,     0,     0,     0,   329,   330,   331,   332,
     333,   334,   154,   155,   156,   335,   372,   337,   338,   339,
     340,   373,   342,   343,     0,     0,   203,     0,     0,   345,
     160,   161,     0,   162,     0,     0,     0,   204,     0,     0,
     165,     0,     0,     0,     0,     0,     0,   205,   206,   146,
     147,   148,   149,   150,   117,   118,   119,   120,   121,   122,
     151,     0,     0,     0,     0,     0,   153,     0,     0,   329,
     330,   331,   332,   333,   334,     0,     0,     0,     0,   154,
     155,   156,     0,   157,   341,   342,   343,     0,   158,     0,
       0,     0,   345,     0,   207,     0,     0,   160,   161,     0,
     162,   146,   147,   148,   149,   150,   117,   118,   119,   120,
     121,   122,   151,     0,     0,   152,     0,     0,     0,     0,
       0,     0,     0,     0,   146,   147,   148,   149,   150,   117,
     118,   119,   120,   121,   122,   151,     0,     0,     0,     0,
       0,   153,     0,     0,     0,  -284,  -284,  -284,  -284,  -284,
    -284,     0,     0,     0,   154,   155,   156,     0,   157,     0,
    -284,  -284,  -284,   158,     0,     0,     0,   165,   345,   159,
       0,     0,   160,   161,     0,   162,   146,   147,   148,   149,
     150,   117,   118,   119,   120,   121,   122,   151,     0,     0,
     165,     0,     0,   153,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   154,   155,   156,     0,
     157,     0,     0,     0,     0,   166,   153,     0,     0,     0,
       0,   167,     0,     0,   160,   161,     0,   162,     0,   154,
     155,   156,     0,   157,     0,     0,     0,     0,   158,     0,
       0,     0,   165,     0,   231,     0,     0,   160,   161,     0,
     162,   146,   147,   148,   149,   150,   117,   118,   119,   120,
     121,   122,   151,     0,     0,     0,     0,     0,   153,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   154,   155,   156,     0,   157,     0,     0,     0,     0,
     158,     0,     0,     0,     0,     0,   367,     0,     0,   160,
     161,     0,   162,   146,   147,   148,   149,   150,   117,   118,
     119,   120,   121,   122,   151,     0,     0,   165,     0,     0,
       0,     0,     0,     0,     0,     0,   146,   147,   148,   149,
     150,   117,   118,   119,   120,   121,   122,   151,     0,     0,
       0,     0,     0,   153,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   154,   155,   156,     0,
     157,     0,     0,     0,     0,   158,     0,     0,     0,   165,
       0,   380,     0,     0,   160,   161,     0,   162,   146,   147,
     148,   149,   150,   117,   118,   119,   120,   121,   122,   151,
       0,     0,   165,     0,     0,   153,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   154,   155,
     156,     0,   157,     0,     0,     0,     0,   158,   153,     0,
       0,     0,     0,   408,     0,     0,   160,   161,     0,   162,
       0,   154,   155,   156,     0,   157,     0,     0,     0,     0,
     158,     0,     0,     0,   165,     0,   533,     0,     0,   160,
     161,     0,   162,   129,   130,   131,   132,   133,   117,   118,
     119,   120,   121,   122,   134,     0,   172,     0,     0,     0,
     153,     0,     0,     0,     0,     0,     0,     0,   173,     0,
       0,     0,     0,   154,   155,   156,   174,   157,     0,     0,
       0,     0,   158,   175,   176,   177,   178,   179,   180,     0,
     181,   160,   161,     0,   162,   129,   130,   131,   132,   133,
     117,   118,   119,   120,   121,   122,   134,     0,     0,   135,
     329,   330,   331,   332,   333,   334,     0,     0,     0,   335,
     336,   337,   338,   339,   340,   341,   342,   343,     0,     0,
       0,   182,     0,   345,     0,   136,   485,     0,     0,     0,
       0,   183,     0,     0,     0,     0,     0,     0,   137,   138,
     139,     0,   140,     0,     0,     0,     0,     0,     0,     0,
       0,   135,     0,   141,     0,     0,   142,   143,   329,   330,
     331,   332,   333,   334,     0,     0,     0,   335,   336,   337,
     338,   339,   340,   341,   342,   343,     0,   136,     0,   344,
       0,   345,     0,     0,     0,     0,     0,     0,     0,     0,
     137,   138,   139,   293,   140,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   142,   143,
       0,     1,     0,     2,     3,     4,     0,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,     0,    16,
      17,    18,    19,    20,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    21,    22,    23,    24,     0,     0,    25,
      26,    27,     0,    28,     0,    29,     0,     0,    30,     0,
      31,    32,     0,     0,    33,    34,    35,    36,    37,     0,
       0,    38,    39,    40,    41,    42,    43,    44,    45,     0,
       0,    46,    47,     0,    48,     0,    49,    50,     0,    51,
      52,     1,    53,     2,     3,     4,     0,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,     0,    16,
      17,    18,    19,    20,     0,     0,    54,     0,     0,     0,
       0,     0,     0,    21,    22,    23,    24,     0,     0,    25,
      26,    27,     0,    28,     0,    29,     0,     0,    30,     0,
      31,    32,     0,     0,    33,    34,    35,    36,    37,     0,
       0,    38,    39,    40,    41,    42,    43,    44,    45,     0,
       0,    46,    47,     0,    48,     0,    49,    50,     0,    51,
      52,   245,    53,   246,   247,   248,     0,   249,   250,   251,
     252,   253,   254,   255,   256,   257,    14,    15,     0,   258,
     259,   260,   261,   262,     0,     0,    54,     0,     0,     0,
       0,     0,     0,   263,   264,   265,   266,     0,     0,   267,
     268,   269,     0,     0,     0,     0,     0,     0,     0,     0,
     270,   271,     0,     0,     0,   272,   273,   274,   275,     0,
       0,   276,   277,   278,   279,   280,   281,   282,   283,     0,
       0,   284,   285,     0,   286,     0,     0,     0,     0,   287,
     288,     0,   289,     0,   329,   330,   331,   332,   333,   334,
       0,     0,     0,   335,   336,   337,   338,   339,   340,   341,
     342,   343,     0,     0,     0,   347,   290,   345,   329,   330,
     331,   332,   333,   334,     0,     0,     0,   335,   336,   337,
     338,   339,   340,   341,   342,   343,     0,     0,     0,   369,
       0,   345,   329,   330,   331,   332,   333,   334,     0,     0,
       0,   335,   336,   337,   338,   339,   340,   341,   342,   343,
       0,     0,     0,   379,     0,   345,   329,   330,   331,   332,
     333,   334,     0,     0,     0,   335,   336,   337,   338,   339,
     340,   341,   342,   343,     0,     0,     0,   389,     0,   345,
     329,   330,   331,   332,   333,   334,     0,     0,     0,   335,
     336,   337,   338,   339,   340,   341,   342,   343,     0,     0,
       0,   394,     0,   345,   329,   330,   331,   332,   333,   334,
       0,     0,     0,   335,   336,   337,   338,   339,   340,   341,
     342,   343,     0,     0,     0,   395,     0,   345,   329,   330,
     331,   332,   333,   334,     0,     0,     0,   335,   336,   337,
     338,   339,   340,   341,   342,   343,     0,     0,     0,   402,
       0,   345,   329,   330,   331,   332,   333,   334,     0,     0,
       0,   335,   336,   337,   338,   339,   340,   341,   342,   343,
       0,     0,     0,   412,     0,   345,   329,   330,   331,   332,
     333,   334,     0,     0,     0,   335,   336,   337,   338,   339,
     340,   341,   342,   343,     0,     0,     0,   413,     0,   345,
     329,   330,   331,   332,   333,   334,     0,     0,     0,   335,
     336,   337,   338,   339,   340,   341,   342,   343,     0,     0,
       0,   460,     0,   345,   329,   330,   331,   332,   333,   334,
       0,     0,     0,   335,   336,   337,   338,   339,   340,   341,
     342,   343,     0,     0,     0,   461,     0,   345,   329,   330,
     331,   332,   333,   334,     0,     0,     0,   335,   336,   337,
     338,   339,   340,   341,   342,   343,     0,     0,     0,   512,
       0,   345,   329,   330,   331,   332,   333,   334,     0,     0,
       0,   335,   336,   337,   338,   339,   340,   341,   342,   343,
       0,     0,     0,   523,     0,   345,   329,   330,   331,   332,
     333,   334,     0,     0,     0,   335,   336,   337,   338,   339,
     340,   341,   342,   343,     0,     0,     0,   524,     0,   345,
     329,   330,   331,   332,   333,   334,     0,     0,     0,   335,
     336,   337,   338,   339,   340,   341,   342,   343,     0,     0,
       0,   532,     0,   345,   329,   330,   331,   332,   333,   334,
       0,     0,     0,   335,   336,   337,   338,   339,   340,   341,
     342,   343,     0,     0,     0,   535,     0,   345,   329,   330,
     331,   332,   333,   334,     0,     0,     0,   335,   336,   337,
     338,   339,   340,   341,   342,   343,     0,     0,     0,   536,
       0,   345,   329,   330,   331,   332,   333,   334,     0,     0,
       0,   335,   336,   337,   338,   339,   340,   341,   342,   343,
       0,     0,     0,   537,     0,   345,   329,   330,   331,   332,
     333,   334,     0,     0,     0,   335,   336,   337,   338,   339,
     340,   341,   342,   343,     0,     0,     0,   538,     0,   345,
     329,   330,   331,   332,   333,   334,     0,     0,     0,   335,
     336,   337,   338,   339,   340,   341,   342,   343,     0,     0,
       0,   539,     0,   345,   329,   330,   331,   332,   333,   334,
       0,     0,     0,   335,   336,   337,   338,   339,   340,   341,
     342,   343,     0,     0,     0,   540,     0,   345,   329,   330,
     331,   332,   333,   334,     0,     0,     0,   335,   336,   337,
     338,   339,   340,   341,   342,   343,     0,     0,     0,   541,
       0,   345,   329,   330,   331,   332,   333,   334,     0,     0,
       0,   335,   336,   337,   338,   339,   340,   341,   342,   343,
       0,     0,     0,   542,     0,   345,   329,   330,   331,   332,
     333,   334,     0,     0,     0,   335,   336,   337,   338,   339,
     340,   341,   342,   343,     0,     0,     0,   543,     0,   345,
     329,   330,   331,   332,   333,   334,     0,     0,     0,   335,
     336,   337,   338,   339,   340,   341,   342,   343,     0,     0,
       0,   551,     0,   345,   329,   330,   331,   332,   333,   334,
       0,     0,     0,   335,   336,   337,   338,   339,   340,   341,
     342,   343,     0,     0,     0,   552,     0,   345,   329,   330,
     331,   332,   333,   334,     0,     0,     0,   335,   336,   337,
     338,   339,   340,   341,   342,   343,     0,     0,     0,   553,
       0,   345,   329,   330,   331,   332,   333,   334,     0,     0,
       0,   335,   336,   337,   338,   339,   340,   341,   342,   343,
       0,     0,     0,   555,     0,   345,   329,   330,   331,   332,
     333,   334,     0,     0,     0,   335,   336,   337,   338,   339,
     340,   341,   342,   343,     0,     0,     0,   556,     0,   345,
     329,   330,   331,   332,   333,   334,     0,     0,     0,   335,
     336,   337,   338,   339,   340,   341,   342,   343,     0,     0,
       0,   557,     0,   345,   329,   330,   331,   332,   333,   334,
       0,     0,     0,   335,   336,   337,   338,   339,   340,   341,
     342,   343,     0,     0,     0,   558,     0,   345,   329,   330,
     331,   332,   333,   334,     0,     0,     0,   335,   336,   337,
     338,   339,   340,   341,   342,   343,     0,     0,     0,     0,
       0,   345
};

static const yytype_int16 yycheck[] =
{
       7,     8,     4,   140,     6,   142,   143,    36,    37,    17,
      69,    69,    69,    19,    69,    48,    69,    24,   123,    95,
      27,   123,    69,    30,    31,    95,    33,    34,   176,   177,
     178,    95,    39,    40,    18,    95,    70,   123,    45,   125,
      48,    95,   101,    50,    51,    92,    53,   123,   123,    33,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    69,    95,   123,   123,   123,   123,   101,   123,   123,
     123,    98,   102,   103,    63,    64,   104,   105,   106,   107,
      95,   111,   112,   113,   114,   123,    70,   117,   118,   119,
     123,    16,    17,   101,    69,   124,   123,   125,   128,   106,
     123,   123,    86,    87,   102,   103,    59,   123,    69,   123,
     123,   123,   123,    61,    67,   123,    95,   123,   123,   117,
     118,   119,   123,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,   123,   123,   123,   140,   123,
     142,   143,   124,   124,   124,    55,   124,   124,   124,   123,
     157,   158,   124,   160,   161,   162,   109,   110,   124,   166,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,   124,   123,    54,   204,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   193,   123,   123,    69,
     197,   198,   104,   105,   106,   107,   203,   123,    69,   123,
     123,   208,   123,   123,   123,   212,   354,   123,   356,   357,
     358,   124,   123,   125,   123,    95,   123,   123,   123,   226,
     123,   228,   229,   123,   102,   103,    69,   123,   108,   109,
     110,   238,   112,   111,   112,   113,   114,   117,   123,   117,
     118,   119,   123,   123,   123,   123,   126,   127,   123,   129,
     123,    69,    95,   123,   123,   123,   123,   123,   123,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   123,   123,   123,
     123,   123,   125,   126,   127,   123,   129,   123,   123,   123,
     123,   123,   123,   300,   301,   302,   303,   304,   305,   123,
     307,   308,   309,   123,   123,    95,   123,   123,   123,   123,
     123,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   123,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,   339,   340,   341,   342,   343,   123,   345,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     102,   103,   123,   123,   371,   372,   373,   374,   123,   111,
     112,   113,   114,   123,   381,   117,   118,   119,   123,   123,
     123,    52,   123,   125,   123,   123,   123,   123,   123,   123,
      -1,   398,   125,   400,   401,   123,   123,   123,   123,   123,
     123,   123,   123,   410,   123,    69,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,   123,   123,
     123,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,    -1,    -1,    -1,   123,
      -1,   125,   126,   127,    -1,   129,    -1,    -1,    -1,    -1,
      -1,    -1,    69,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,
      -1,    -1,    -1,    -1,    -1,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,    -1,    -1,    -1,   123,    -1,   125,   126,
     127,    -1,   129,    -1,    -1,    -1,    -1,    -1,    -1,    69,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,
      -1,    -1,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
      -1,    -1,    -1,   123,    -1,   125,   126,   127,    -1,   129,
      -1,    -1,    -1,    -1,    -1,    -1,    69,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,    -1,    -1,    -1,
     123,    -1,   125,   126,   127,    -1,   129,    -1,    -1,    -1,
      -1,    -1,    -1,    69,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      -1,    -1,    -1,    -1,    -1,    -1,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,    -1,    -1,    55,    -1,    -1,   125,
     126,   127,    -1,   129,    -1,    -1,    -1,    66,    -1,    -1,
      69,    -1,    -1,    -1,    -1,    -1,    -1,    76,    77,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,   102,
     103,   104,   105,   106,   107,    -1,    -1,    -1,    -1,   108,
     109,   110,    -1,   112,   117,   118,   119,    -1,   117,    -1,
      -1,    -1,   125,    -1,   123,    -1,    -1,   126,   127,    -1,
     129,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    69,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    95,    -1,    -1,    -1,   102,   103,   104,   105,   106,
     107,    -1,    -1,    -1,   108,   109,   110,    -1,   112,    -1,
     117,   118,   119,   117,    -1,    -1,    -1,    69,   125,   123,
      -1,    -1,   126,   127,    -1,   129,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      69,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,    -1,
     112,    -1,    -1,    -1,    -1,   117,    95,    -1,    -1,    -1,
      -1,   123,    -1,    -1,   126,   127,    -1,   129,    -1,   108,
     109,   110,    -1,   112,    -1,    -1,    -1,    -1,   117,    -1,
      -1,    -1,    69,    -1,   123,    -1,    -1,   126,   127,    -1,
     129,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    -1,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   108,   109,   110,    -1,   112,    -1,    -1,    -1,    -1,
     117,    -1,    -1,    -1,    -1,    -1,   123,    -1,    -1,   126,
     127,    -1,   129,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    69,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,    -1,
     112,    -1,    -1,    -1,    -1,   117,    -1,    -1,    -1,    69,
      -1,   123,    -1,    -1,   126,   127,    -1,   129,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    69,    -1,    -1,    95,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,
     110,    -1,   112,    -1,    -1,    -1,    -1,   117,    95,    -1,
      -1,    -1,    -1,   123,    -1,    -1,   126,   127,    -1,   129,
      -1,   108,   109,   110,    -1,   112,    -1,    -1,    -1,    -1,
     117,    -1,    -1,    -1,    69,    -1,   123,    -1,    -1,   126,
     127,    -1,   129,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    15,    -1,    -1,    -1,
      95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,
      -1,    -1,    -1,   108,   109,   110,    35,   112,    -1,    -1,
      -1,    -1,   117,    42,    43,    44,    45,    46,    47,    -1,
      49,   126,   127,    -1,   129,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    69,
     102,   103,   104,   105,   106,   107,    -1,    -1,    -1,   111,
     112,   113,   114,   115,   116,   117,   118,   119,    -1,    -1,
      -1,    90,    -1,   125,    -1,    95,   128,    -1,    -1,    -1,
      -1,   100,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,
     110,    -1,   112,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    69,    -1,   123,    -1,    -1,   126,   127,   102,   103,
     104,   105,   106,   107,    -1,    -1,    -1,   111,   112,   113,
     114,   115,   116,   117,   118,   119,    -1,    95,    -1,   123,
      -1,   125,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     108,   109,   110,     0,   112,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   126,   127,
      -1,    18,    -1,    20,    21,    22,    -1,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    36,
      37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    50,    51,    52,    53,    -1,    -1,    56,
      57,    58,    -1,    60,    -1,    62,    -1,    -1,    65,    -1,
      67,    68,    -1,    -1,    71,    72,    73,    74,    75,    -1,
      -1,    78,    79,    80,    81,    82,    83,    84,    85,    -1,
      -1,    88,    89,    -1,    91,    -1,    93,    94,    -1,    96,
      97,    18,    99,    20,    21,    22,    -1,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    36,
      37,    38,    39,    40,    -1,    -1,   123,    -1,    -1,    -1,
      -1,    -1,    -1,    50,    51,    52,    53,    -1,    -1,    56,
      57,    58,    -1,    60,    -1,    62,    -1,    -1,    65,    -1,
      67,    68,    -1,    -1,    71,    72,    73,    74,    75,    -1,
      -1,    78,    79,    80,    81,    82,    83,    84,    85,    -1,
      -1,    88,    89,    -1,    91,    -1,    93,    94,    -1,    96,
      97,    18,    99,    20,    21,    22,    -1,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    36,
      37,    38,    39,    40,    -1,    -1,   123,    -1,    -1,    -1,
      -1,    -1,    -1,    50,    51,    52,    53,    -1,    -1,    56,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    68,    -1,    -1,    -1,    72,    73,    74,    75,    -1,
      -1,    78,    79,    80,    81,    82,    83,    84,    85,    -1,
      -1,    88,    89,    -1,    91,    -1,    -1,    -1,    -1,    96,
      97,    -1,    99,    -1,   102,   103,   104,   105,   106,   107,
      -1,    -1,    -1,   111,   112,   113,   114,   115,   116,   117,
     118,   119,    -1,    -1,    -1,   123,   123,   125,   102,   103,
     104,   105,   106,   107,    -1,    -1,    -1,   111,   112,   113,
     114,   115,   116,   117,   118,   119,    -1,    -1,    -1,   123,
      -1,   125,   102,   103,   104,   105,   106,   107,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
      -1,    -1,    -1,   123,    -1,   125,   102,   103,   104,   105,
     106,   107,    -1,    -1,    -1,   111,   112,   113,   114,   115,
     116,   117,   118,   119,    -1,    -1,    -1,   123,    -1,   125,
     102,   103,   104,   105,   106,   107,    -1,    -1,    -1,   111,
     112,   113,   114,   115,   116,   117,   118,   119,    -1,    -1,
      -1,   123,    -1,   125,   102,   103,   104,   105,   106,   107,
      -1,    -1,    -1,   111,   112,   113,   114,   115,   116,   117,
     118,   119,    -1,    -1,    -1,   123,    -1,   125,   102,   103,
     104,   105,   106,   107,    -1,    -1,    -1,   111,   112,   113,
     114,   115,   116,   117,   118,   119,    -1,    -1,    -1,   123,
      -1,   125,   102,   103,   104,   105,   106,   107,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
      -1,    -1,    -1,   123,    -1,   125,   102,   103,   104,   105,
     106,   107,    -1,    -1,    -1,   111,   112,   113,   114,   115,
     116,   117,   118,   119,    -1,    -1,    -1,   123,    -1,   125,
     102,   103,   104,   105,   106,   107,    -1,    -1,    -1,   111,
     112,   113,   114,   115,   116,   117,   118,   119,    -1,    -1,
      -1,   123,    -1,   125,   102,   103,   104,   105,   106,   107,
      -1,    -1,    -1,   111,   112,   113,   114,   115,   116,   117,
     118,   119,    -1,    -1,    -1,   123,    -1,   125,   102,   103,
     104,   105,   106,   107,    -1,    -1,    -1,   111,   112,   113,
     114,   115,   116,   117,   118,   119,    -1,    -1,    -1,   123,
      -1,   125,   102,   103,   104,   105,   106,   107,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
      -1,    -1,    -1,   123,    -1,   125,   102,   103,   104,   105,
     106,   107,    -1,    -1,    -1,   111,   112,   113,   114,   115,
     116,   117,   118,   119,    -1,    -1,    -1,   123,    -1,   125,
     102,   103,   104,   105,   106,   107,    -1,    -1,    -1,   111,
     112,   113,   114,   115,   116,   117,   118,   119,    -1,    -1,
      -1,   123,    -1,   125,   102,   103,   104,   105,   106,   107,
      -1,    -1,    -1,   111,   112,   113,   114,   115,   116,   117,
     118,   119,    -1,    -1,    -1,   123,    -1,   125,   102,   103,
     104,   105,   106,   107,    -1,    -1,    -1,   111,   112,   113,
     114,   115,   116,   117,   118,   119,    -1,    -1,    -1,   123,
      -1,   125,   102,   103,   104,   105,   106,   107,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
      -1,    -1,    -1,   123,    -1,   125,   102,   103,   104,   105,
     106,   107,    -1,    -1,    -1,   111,   112,   113,   114,   115,
     116,   117,   118,   119,    -1,    -1,    -1,   123,    -1,   125,
     102,   103,   104,   105,   106,   107,    -1,    -1,    -1,   111,
     112,   113,   114,   115,   116,   117,   118,   119,    -1,    -1,
      -1,   123,    -1,   125,   102,   103,   104,   105,   106,   107,
      -1,    -1,    -1,   111,   112,   113,   114,   115,   116,   117,
     118,   119,    -1,    -1,    -1,   123,    -1,   125,   102,   103,
     104,   105,   106,   107,    -1,    -1,    -1,   111,   112,   113,
     114,   115,   116,   117,   118,   119,    -1,    -1,    -1,   123,
      -1,   125,   102,   103,   104,   105,   106,   107,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
      -1,    -1,    -1,   123,    -1,   125,   102,   103,   104,   105,
     106,   107,    -1,    -1,    -1,   111,   112,   113,   114,   115,
     116,   117,   118,   119,    -1,    -1,    -1,   123,    -1,   125,
     102,   103,   104,   105,   106,   107,    -1,    -1,    -1,   111,
     112,   113,   114,   115,   116,   117,   118,   119,    -1,    -1,
      -1,   123,    -1,   125,   102,   103,   104,   105,   106,   107,
      -1,    -1,    -1,   111,   112,   113,   114,   115,   116,   117,
     118,   119,    -1,    -1,    -1,   123,    -1,   125,   102,   103,
     104,   105,   106,   107,    -1,    -1,    -1,   111,   112,   113,
     114,   115,   116,   117,   118,   119,    -1,    -1,    -1,   123,
      -1,   125,   102,   103,   104,   105,   106,   107,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
      -1,    -1,    -1,   123,    -1,   125,   102,   103,   104,   105,
     106,   107,    -1,    -1,    -1,   111,   112,   113,   114,   115,
     116,   117,   118,   119,    -1,    -1,    -1,   123,    -1,   125,
     102,   103,   104,   105,   106,   107,    -1,    -1,    -1,   111,
     112,   113,   114,   115,   116,   117,   118,   119,    -1,    -1,
      -1,   123,    -1,   125,   102,   103,   104,   105,   106,   107,
      -1,    -1,    -1,   111,   112,   113,   114,   115,   116,   117,
     118,   119,    -1,    -1,    -1,   123,    -1,   125,   102,   103,
     104,   105,   106,   107,    -1,    -1,    -1,   111,   112,   113,
     114,   115,   116,   117,   118,   119,    -1,    -1,    -1,    -1,
      -1,   125
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    18,    20,    21,    22,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    36,    37,    38,    39,
      40,    50,    51,    52,    53,    56,    57,    58,    60,    62,
      65,    67,    68,    71,    72,    73,    74,    75,    78,    79,
      80,    81,    82,    83,    84,    85,    88,    89,    91,    93,
      94,    96,    97,    99,   123,   131,   132,   134,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,    19,   123,    48,    95,
     123,   123,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    59,    67,   109,   110,   135,   123,     3,
       4,     5,     6,     7,    14,    69,    95,   108,   109,   110,
     112,   123,   126,   127,   135,   185,     3,     4,     5,     6,
       7,    14,    69,    95,   108,   109,   110,   112,   117,   123,
     126,   127,   129,   135,   186,    69,   117,   123,   186,    95,
      95,    95,    15,    27,    35,    42,    43,    44,    45,    46,
      47,    49,    90,   100,   123,   123,   123,   123,   123,   123,
     123,   123,   123,    54,   123,   186,    69,    69,   186,    61,
      63,    64,   186,    55,    66,    76,    77,   123,   186,    70,
     101,   186,   186,    16,    17,   133,   133,   133,   123,   186,
     186,   123,   123,    95,   123,    18,    33,    70,    86,    87,
     123,   123,   186,    17,    48,    69,   101,   123,    69,    92,
      69,   123,   123,   186,   186,    18,    20,    21,    22,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    36,    37,
      38,    39,    40,    50,    51,    52,    53,    56,    57,    58,
      67,    68,    72,    73,    74,    75,    78,    79,    80,    81,
      82,    83,    84,    85,    88,    89,    91,    96,    97,    99,
     123,   134,   186,     0,   132,   123,   186,    95,    95,   123,
     124,   124,   124,   124,   124,   124,   133,   124,   124,   124,
     185,   185,   185,   102,   103,   111,   112,   113,   114,   117,
     118,   119,   125,   123,   186,   186,   186,   186,   186,   102,
     103,   104,   105,   106,   107,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   123,   125,   186,   123,   123,   123,
     123,   123,   123,   123,   159,   186,   159,   159,   159,   123,
     123,   123,    69,   123,    69,   101,   123,   123,   186,   123,
      69,   186,   112,   117,   186,   123,    95,   123,   123,   123,
     123,   186,   133,   123,   124,   123,   186,   123,   123,   123,
     186,   123,   123,   123,   123,   123,   123,   123,   186,   123,
     186,   186,   123,   123,   123,    98,   123,   123,   123,   186,
      69,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   186,   186,   186,   186,   186,   186,
     123,   186,   186,   186,   128,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   128,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   123,   159,   159,   159,   159,   123,    69,   123,
      69,   123,   123,   123,   186,   186,   186,   186,   123,   123,
     186,   123,    95,   123,   123,   123,   186,   123,   186,   123,
     186,   123,   123,   123,   186,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   130,   131,   131,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   133,   133,   134,   134,   135,   135,
     135,   135,   135,   135,   136,   136,   137,   138,   139,   139,
     139,   139,   139,   139,   140,   141,   142,   143,   144,   145,
     146,   146,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   148,   148,   148,   148,   149,   150,   151,
     151,   152,   152,   152,   152,   153,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   155,   155,   155,   155,
     155,   155,   155,   155,   156,   157,   157,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   159,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     173,   173,   173,   174,   175,   176,   177,   178,   178,   178,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   180,
     180,   181,   182,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     184,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     2,     2,     3,     3,
       3,     3,     4,     2,     3,     3,     2,     3,     3,     3,
       2,     3,     3,     3,     2,     4,     4,     4,     5,     5,
       5,     2,     3,     3,     4,     4,     5,     2,     3,     2,
       4,     2,     3,     4,     4,     2,     4,     5,     5,     5,
       5,     5,     5,     5,     5,     5,     2,     5,     2,     3,
       3,     2,     3,     4,     2,     2,     3,     3,     3,     5,
       5,     5,     5,     3,     3,     3,     3,     3,     4,     3,
       4,     5,     4,     5,     0,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     3,     3,     3,     2,     4,
       3,     3,     2,     4,     5,     5,     3,     3,     4,     3,
       2,     3,     4,     3,     4,     5,     3,     4,     5,     3,
       3,     3,     4,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     2,     2,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


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
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
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
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
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

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

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
      yychar = yylex ();
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
     '$$ = $1'.

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
#line 201 "parser.y" /* yacc.c:1646  */
    {
      }
#line 2237 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 208 "parser.y" /* yacc.c:1646  */
    { (yyval.bval)=(yyvsp[0].bval); }
#line 2243 "y.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 214 "parser.y" /* yacc.c:1646  */
    { (yyval.sval)=(yyvsp[0].sval); }
#line 2249 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 224 "parser.y" /* yacc.c:1646  */
    { (yyval.uval)=(yyvsp[0].uval); }
#line 2255 "y.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 229 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_timebp_command(0, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2264 "y.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 234 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_timebp_command(1, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2273 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 242 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_modebp_command();
          free((yyvsp[-1].sval));
      }
#line 2282 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 250 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_vmexitbp_command();
          free((yyvsp[-1].sval));
      }
#line 2291 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 258 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_command((yyvsp[-1].sval));
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2300 "y.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 263 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_command("all");
          free((yyvsp[-2].sval));
      }
#line 2309 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 268 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_command("off");
          free((yyvsp[-2].sval));
      }
#line 2318 "y.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 273 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_param_command((yyvsp[-1].sval), 0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2327 "y.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 278 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_param_command((yyvsp[-2].sval), 1);
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2336 "y.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 283 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_command(0);
          free((yyvsp[-1].sval));
      }
#line 2345 "y.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 291 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_xlate_address((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2354 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 299 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_tlb_lookup((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2363 "y.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 307 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_ptime_command();
          free((yyvsp[-1].sval));
      }
#line 2372 "y.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 315 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_trace_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2381 "y.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 323 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_trace_reg_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2390 "y.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 331 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_trace_mem_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2399 "y.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 339 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_print_stack_command(16);
          free((yyvsp[-1].sval));
      }
#line 2408 "y.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 344 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_print_stack_command((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2417 "y.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 352 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2426 "y.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 357 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2435 "y.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 362 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_print_watchpoints();
          free((yyvsp[-1].sval));
      }
#line 2444 "y.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 367 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2453 "y.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 372 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2462 "y.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 377 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(1, (yyvsp[-1].uval), 1); /* BX_WRITE */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2471 "y.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 382 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2480 "y.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 387 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2489 "y.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 392 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(1, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_WRITE */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2498 "y.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 397 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_unwatch_all();
          free((yyvsp[-1].sval));
      }
#line 2507 "y.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 402 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_unwatch((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2516 "y.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 410 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_symbol_command((yyvsp[-1].sval), 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2525 "y.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 415 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_symbol_command((yyvsp[-2].sval), 0, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2534 "y.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 420 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_symbol_command((yyvsp[-1].sval), 1, 0);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2543 "y.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 425 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_symbol_command((yyvsp[-2].sval), 1, (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2552 "y.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 433 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_where_command();
        free((yyvsp[-1].sval));
      }
#line 2561 "y.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 441 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_print_string_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2570 "y.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 449 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_continue_command(1);
        free((yyvsp[-1].sval));
      }
#line 2579 "y.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 454 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_continue_command((yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2588 "y.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 462 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[-1].sval));
      }
#line 2597 "y.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 467 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2606 "y.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 472 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_stepN_command(-1, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2615 "y.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 477 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_stepN_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2624 "y.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 485 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_step_over_command();
        free((yyvsp[-1].sval));
      }
#line 2633 "y.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 493 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_set_auto_disassemble((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2642 "y.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 498 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_set_symbol_command((yyvsp[-3].sval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2651 "y.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 503 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg8l_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2659 "y.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 507 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg8h_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2667 "y.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 511 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg16_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2675 "y.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 515 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg32_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2683 "y.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 519 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg64_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2691 "y.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 523 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2699 "y.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 527 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2707 "y.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 531 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_load_segreg((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2715 "y.tab.c" /* yacc.c:1646  */
    break;

  case 116:
#line 538 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 2724 "y.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 543 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 2733 "y.tab.c" /* yacc.c:1646  */
    break;

  case 118:
#line 548 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free((yyvsp[-1].sval));
      }
#line 2742 "y.tab.c" /* yacc.c:1646  */
    break;

  case 119:
#line 553 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2751 "y.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 558 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 2760 "y.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 563 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free((yyvsp[-1].sval));
      }
#line 2769 "y.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 568 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2778 "y.tab.c" /* yacc.c:1646  */
    break;

  case 123:
#line 573 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2787 "y.tab.c" /* yacc.c:1646  */
    break;

  case 124:
#line 581 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-1].sval));
      }
#line 2796 "y.tab.c" /* yacc.c:1646  */
    break;

  case 125:
#line 589 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-1].sval));
      }
#line 2805 "y.tab.c" /* yacc.c:1646  */
    break;

  case 126:
#line 594 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 2814 "y.tab.c" /* yacc.c:1646  */
    break;

  case 127:
#line 602 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2823 "y.tab.c" /* yacc.c:1646  */
    break;

  case 128:
#line 607 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2832 "y.tab.c" /* yacc.c:1646  */
    break;

  case 129:
#line 612 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_idt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2841 "y.tab.c" /* yacc.c:1646  */
    break;

  case 130:
#line 617 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_ivt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2850 "y.tab.c" /* yacc.c:1646  */
    break;

  case 131:
#line 622 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_gdt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2859 "y.tab.c" /* yacc.c:1646  */
    break;

  case 132:
#line 627 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_ldt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2868 "y.tab.c" /* yacc.c:1646  */
    break;

  case 133:
#line 632 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_dump_table();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2877 "y.tab.c" /* yacc.c:1646  */
    break;

  case 134:
#line 637 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_tss_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2886 "y.tab.c" /* yacc.c:1646  */
    break;

  case 135:
#line 642 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_flags();
        free((yyvsp[-2].sval));
      }
#line 2895 "y.tab.c" /* yacc.c:1646  */
    break;

  case 136:
#line 647 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_linux_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2904 "y.tab.c" /* yacc.c:1646  */
    break;

  case 137:
#line 652 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2913 "y.tab.c" /* yacc.c:1646  */
    break;

  case 138:
#line 657 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2922 "y.tab.c" /* yacc.c:1646  */
    break;

  case 139:
#line 662 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device("", "");
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2931 "y.tab.c" /* yacc.c:1646  */
    break;

  case 140:
#line 667 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2940 "y.tab.c" /* yacc.c:1646  */
    break;

  case 141:
#line 672 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2949 "y.tab.c" /* yacc.c:1646  */
    break;

  case 142:
#line 677 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2958 "y.tab.c" /* yacc.c:1646  */
    break;

  case 143:
#line 682 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2967 "y.tab.c" /* yacc.c:1646  */
    break;

  case 144:
#line 689 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = EMPTY_ARG; }
#line 2973 "y.tab.c" /* yacc.c:1646  */
    break;

  case 146:
#line 694 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 2982 "y.tab.c" /* yacc.c:1646  */
    break;

  case 147:
#line 702 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[-1].sval));
      }
#line 2991 "y.tab.c" /* yacc.c:1646  */
    break;

  case 148:
#line 710 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3000 "y.tab.c" /* yacc.c:1646  */
    break;

  case 149:
#line 718 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3009 "y.tab.c" /* yacc.c:1646  */
    break;

  case 150:
#line 726 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3018 "y.tab.c" /* yacc.c:1646  */
    break;

  case 151:
#line 734 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3027 "y.tab.c" /* yacc.c:1646  */
    break;

  case 152:
#line 742 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3036 "y.tab.c" /* yacc.c:1646  */
    break;

  case 153:
#line 750 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_control_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3045 "y.tab.c" /* yacc.c:1646  */
    break;

  case 154:
#line 758 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3054 "y.tab.c" /* yacc.c:1646  */
    break;

  case 155:
#line 766 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_del_breakpoint_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3063 "y.tab.c" /* yacc.c:1646  */
    break;

  case 156:
#line 774 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3072 "y.tab.c" /* yacc.c:1646  */
    break;

  case 157:
#line 781 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 0);
        free((yyvsp[-2].sval));
      }
#line 3081 "y.tab.c" /* yacc.c:1646  */
    break;

  case 158:
#line 789 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_quit_command();
        free((yyvsp[-1].sval));
      }
#line 3090 "y.tab.c" /* yacc.c:1646  */
    break;

  case 159:
#line 797 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_examine_command((yyvsp[-3].sval), (yyvsp[-2].sval),1, (yyvsp[-1].uval), 1);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3099 "y.tab.c" /* yacc.c:1646  */
    break;

  case 160:
#line 802 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_examine_command((yyvsp[-2].sval), (yyvsp[-1].sval),1, 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3108 "y.tab.c" /* yacc.c:1646  */
    break;

  case 161:
#line 807 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_examine_command((yyvsp[-2].sval), NULL,0, (yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3117 "y.tab.c" /* yacc.c:1646  */
    break;

  case 162:
#line 812 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_examine_command((yyvsp[-1].sval), NULL,0, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 3126 "y.tab.c" /* yacc.c:1646  */
    break;

  case 163:
#line 820 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_restore_command((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3135 "y.tab.c" /* yacc.c:1646  */
    break;

  case 164:
#line 828 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_writemem_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3144 "y.tab.c" /* yacc.c:1646  */
    break;

  case 165:
#line 836 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_setpmem_command((yyvsp[-3].uval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3153 "y.tab.c" /* yacc.c:1646  */
    break;

  case 166:
#line 844 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_query_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3162 "y.tab.c" /* yacc.c:1646  */
    break;

  case 167:
#line 852 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3171 "y.tab.c" /* yacc.c:1646  */
    break;

  case 168:
#line 857 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_take_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3180 "y.tab.c" /* yacc.c:1646  */
    break;

  case 169:
#line 862 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3189 "y.tab.c" /* yacc.c:1646  */
    break;

  case 170:
#line 870 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[-1].sval));
      }
#line 3198 "y.tab.c" /* yacc.c:1646  */
    break;

  case 171:
#line 875 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3207 "y.tab.c" /* yacc.c:1646  */
    break;

  case 172:
#line 880 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3216 "y.tab.c" /* yacc.c:1646  */
    break;

  case 173:
#line 885 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_current((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3225 "y.tab.c" /* yacc.c:1646  */
    break;

  case 174:
#line 890 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_command((yyvsp[-2].sval), (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3234 "y.tab.c" /* yacc.c:1646  */
    break;

  case 175:
#line 895 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3243 "y.tab.c" /* yacc.c:1646  */
    break;

  case 176:
#line 900 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3252 "y.tab.c" /* yacc.c:1646  */
    break;

  case 177:
#line 905 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_hex_mode_switch((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3261 "y.tab.c" /* yacc.c:1646  */
    break;

  case 178:
#line 910 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_set_disassemble_size((yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3270 "y.tab.c" /* yacc.c:1646  */
    break;

  case 179:
#line 918 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3279 "y.tab.c" /* yacc.c:1646  */
    break;

  case 180:
#line 923 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3288 "y.tab.c" /* yacc.c:1646  */
    break;

  case 181:
#line 931 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_doit_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3297 "y.tab.c" /* yacc.c:1646  */
    break;

  case 182:
#line 939 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_crc_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3306 "y.tab.c" /* yacc.c:1646  */
    break;

  case 183:
#line 947 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3315 "y.tab.c" /* yacc.c:1646  */
    break;

  case 184:
#line 952 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("c|cont|continue - continue executing\n");
         dbg_printf("c|cont|continue if <expression> - continue executing only if expression is true\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3325 "y.tab.c" /* yacc.c:1646  */
    break;

  case 185:
#line 958 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3336 "y.tab.c" /* yacc.c:1646  */
    break;

  case 186:
#line 965 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3345 "y.tab.c" /* yacc.c:1646  */
    break;

  case 187:
#line 970 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3354 "y.tab.c" /* yacc.c:1646  */
    break;

  case 188:
#line 975 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3363 "y.tab.c" /* yacc.c:1646  */
    break;

  case 189:
#line 980 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3372 "y.tab.c" /* yacc.c:1646  */
    break;

  case 190:
#line 985 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3381 "y.tab.c" /* yacc.c:1646  */
    break;

  case 191:
#line 990 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3390 "y.tab.c" /* yacc.c:1646  */
    break;

  case 192:
#line 995 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3399 "y.tab.c" /* yacc.c:1646  */
    break;

  case 193:
#line 1000 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3408 "y.tab.c" /* yacc.c:1646  */
    break;

  case 194:
#line 1005 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3417 "y.tab.c" /* yacc.c:1646  */
    break;

  case 195:
#line 1010 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3426 "y.tab.c" /* yacc.c:1646  */
    break;

  case 196:
#line 1015 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3435 "y.tab.c" /* yacc.c:1646  */
    break;

  case 197:
#line 1020 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3445 "y.tab.c" /* yacc.c:1646  */
    break;

  case 198:
#line 1026 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3455 "y.tab.c" /* yacc.c:1646  */
    break;

  case 199:
#line 1032 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("trace-mem on  - print all memory accesses occured during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3465 "y.tab.c" /* yacc.c:1646  */
    break;

  case 200:
#line 1038 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3477 "y.tab.c" /* yacc.c:1646  */
    break;

  case 201:
#line 1046 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3486 "y.tab.c" /* yacc.c:1646  */
    break;

  case 202:
#line 1051 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3495 "y.tab.c" /* yacc.c:1646  */
    break;

  case 203:
#line 1056 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3504 "y.tab.c" /* yacc.c:1646  */
    break;

  case 204:
#line 1061 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3513 "y.tab.c" /* yacc.c:1646  */
    break;

  case 205:
#line 1066 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3522 "y.tab.c" /* yacc.c:1646  */
    break;

  case 206:
#line 1071 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3531 "y.tab.c" /* yacc.c:1646  */
    break;

  case 207:
#line 1076 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3540 "y.tab.c" /* yacc.c:1646  */
    break;

  case 208:
#line 1081 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3549 "y.tab.c" /* yacc.c:1646  */
    break;

  case 209:
#line 1086 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3558 "y.tab.c" /* yacc.c:1646  */
    break;

  case 210:
#line 1091 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3567 "y.tab.c" /* yacc.c:1646  */
    break;

  case 211:
#line 1096 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3576 "y.tab.c" /* yacc.c:1646  */
    break;

  case 212:
#line 1101 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("zmm - print AVX-512 state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3585 "y.tab.c" /* yacc.c:1646  */
    break;

  case 213:
#line 1106 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3594 "y.tab.c" /* yacc.c:1646  */
    break;

  case 214:
#line 1111 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3603 "y.tab.c" /* yacc.c:1646  */
    break;

  case 215:
#line 1116 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3612 "y.tab.c" /* yacc.c:1646  */
    break;

  case 216:
#line 1121 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3621 "y.tab.c" /* yacc.c:1646  */
    break;

  case 217:
#line 1126 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3630 "y.tab.c" /* yacc.c:1646  */
    break;

  case 218:
#line 1131 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm hex on/off - control disasm offsets and displacements format\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3644 "y.tab.c" /* yacc.c:1646  */
    break;

  case 219:
#line 1141 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("watch - print current watch point status\n");
         dbg_printf("watch stop - stop simulation when a watchpoint is encountred\n");
         dbg_printf("watch continue - do not stop the simulation when watch point is encountred\n");
         dbg_printf("watch r|read addr - insert a read watch point at physical address addr\n");
         dbg_printf("watch w|write addr - insert a write watch point at physical address addr\n");
         dbg_printf("watch r|read addr <len> - insert a read watch point at physical address addr with range <len>\n");
         dbg_printf("watch w|write addr <len> - insert a write watch point at physical address addr with range <len>\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3659 "y.tab.c" /* yacc.c:1646  */
    break;

  case 220:
#line 1152 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3669 "y.tab.c" /* yacc.c:1646  */
    break;

  case 221:
#line 1158 "parser.y" /* yacc.c:1646  */
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
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3686 "y.tab.c" /* yacc.c:1646  */
    break;

  case 222:
#line 1171 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("instrument <command> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command>\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3695 "y.tab.c" /* yacc.c:1646  */
    break;

  case 223:
#line 1176 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("set <regname> = <expr> - set register value to expression\n");
         dbg_printf("set eflags = <expr> - set eflags value to expression, not all flags can be modified\n");
         dbg_printf("set $cpu = <N> - move debugger control to cpu <N> in SMP simulation\n");
         dbg_printf("set $auto_disassemble = 1 - cause debugger to disassemble current instruction\n");
         dbg_printf("       every time execution stops\n");
         dbg_printf("set u|disasm|disassemble on  - same as 'set $auto_disassemble = 1'\n");
         dbg_printf("set u|disasm|disassemble off - same as 'set $auto_disassemble = 0'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3710 "y.tab.c" /* yacc.c:1646  */
    break;

  case 224:
#line 1187 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3719 "y.tab.c" /* yacc.c:1646  */
    break;

  case 225:
#line 1192 "parser.y" /* yacc.c:1646  */
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
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3739 "y.tab.c" /* yacc.c:1646  */
    break;

  case 226:
#line 1208 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("show <command> - toggles show symbolic info (calls to begin with)\n");
         dbg_printf("show - shows current show mode\n");
         dbg_printf("show mode - show, when processor switch mode\n");
         dbg_printf("show int - show, when an interrupt happens\n");
         dbg_printf("show softint - show, when software interrupt happens\n");
         dbg_printf("show extint - show, when external interrupt happens\n");
         dbg_printf("show call - show, when call is happens\n");
         dbg_printf("show iret - show, when iret is happens\n");
         dbg_printf("show all - turns on all symbolic info\n");
         dbg_printf("show off - turns off symbolic info\n");
         dbg_printf("show dbg_all - turn on all bx_dbg flags\n");
         dbg_printf("show dbg_none - turn off all bx_dbg flags\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3759 "y.tab.c" /* yacc.c:1646  */
    break;

  case 227:
#line 1224 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("calc|? <expr> - calculate a expression and display the result.\n");
         dbg_printf("    'expr' can reference any general-purpose, opmask and segment\n");
         dbg_printf("    registers, use any arithmetic and logic operations, and\n");
         dbg_printf("    also the special ':' operator which computes the linear\n");
         dbg_printf("    address of a segment:offset (in real and v86 mode) or of\n");
         dbg_printf("    a selector:offset (in protected mode) pair.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3773 "y.tab.c" /* yacc.c:1646  */
    break;

  case 228:
#line 1234 "parser.y" /* yacc.c:1646  */
    {
         bx_dbg_print_help();
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3782 "y.tab.c" /* yacc.c:1646  */
    break;

  case 229:
#line 1239 "parser.y" /* yacc.c:1646  */
    {
         bx_dbg_print_help();
         free((yyvsp[-1].sval));
       }
#line 3791 "y.tab.c" /* yacc.c:1646  */
    break;

  case 230:
#line 1247 "parser.y" /* yacc.c:1646  */
    {
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 3800 "y.tab.c" /* yacc.c:1646  */
    break;

  case 231:
#line 1265 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[0].uval); }
#line 3806 "y.tab.c" /* yacc.c:1646  */
    break;

  case 232:
#line 1266 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 3812 "y.tab.c" /* yacc.c:1646  */
    break;

  case 233:
#line 1267 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 3818 "y.tab.c" /* yacc.c:1646  */
    break;

  case 234:
#line 1268 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 3824 "y.tab.c" /* yacc.c:1646  */
    break;

  case 235:
#line 1269 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 3830 "y.tab.c" /* yacc.c:1646  */
    break;

  case 236:
#line 1270 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 3836 "y.tab.c" /* yacc.c:1646  */
    break;

  case 237:
#line 1271 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 3842 "y.tab.c" /* yacc.c:1646  */
    break;

  case 238:
#line 1272 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 3848 "y.tab.c" /* yacc.c:1646  */
    break;

  case 239:
#line 1273 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 3854 "y.tab.c" /* yacc.c:1646  */
    break;

  case 240:
#line 1274 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_ip (); }
#line 3860 "y.tab.c" /* yacc.c:1646  */
    break;

  case 241:
#line 1275 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_eip(); }
#line 3866 "y.tab.c" /* yacc.c:1646  */
    break;

  case 242:
#line 1276 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
#line 3872 "y.tab.c" /* yacc.c:1646  */
    break;

  case 243:
#line 1277 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 3878 "y.tab.c" /* yacc.c:1646  */
    break;

  case 244:
#line 1278 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 3884 "y.tab.c" /* yacc.c:1646  */
    break;

  case 245:
#line 1279 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 3890 "y.tab.c" /* yacc.c:1646  */
    break;

  case 246:
#line 1280 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) / (yyvsp[0].uval); }
#line 3896 "y.tab.c" /* yacc.c:1646  */
    break;

  case 247:
#line 1281 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 3902 "y.tab.c" /* yacc.c:1646  */
    break;

  case 248:
#line 1282 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 3908 "y.tab.c" /* yacc.c:1646  */
    break;

  case 249:
#line 1283 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 3914 "y.tab.c" /* yacc.c:1646  */
    break;

  case 250:
#line 1284 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 3920 "y.tab.c" /* yacc.c:1646  */
    break;

  case 251:
#line 1285 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 3926 "y.tab.c" /* yacc.c:1646  */
    break;

  case 252:
#line 1286 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = !(yyvsp[0].uval); }
#line 3932 "y.tab.c" /* yacc.c:1646  */
    break;

  case 253:
#line 1287 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = -(yyvsp[0].uval); }
#line 3938 "y.tab.c" /* yacc.c:1646  */
    break;

  case 254:
#line 1288 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-1].uval); }
#line 3944 "y.tab.c" /* yacc.c:1646  */
    break;

  case 255:
#line 1294 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[0].uval); }
#line 3950 "y.tab.c" /* yacc.c:1646  */
    break;

  case 256:
#line 1295 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 3956 "y.tab.c" /* yacc.c:1646  */
    break;

  case 257:
#line 1296 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 3962 "y.tab.c" /* yacc.c:1646  */
    break;

  case 258:
#line 1297 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 3968 "y.tab.c" /* yacc.c:1646  */
    break;

  case 259:
#line 1298 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 3974 "y.tab.c" /* yacc.c:1646  */
    break;

  case 260:
#line 1299 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 3980 "y.tab.c" /* yacc.c:1646  */
    break;

  case 261:
#line 1300 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 3986 "y.tab.c" /* yacc.c:1646  */
    break;

  case 262:
#line 1301 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 3992 "y.tab.c" /* yacc.c:1646  */
    break;

  case 263:
#line 1302 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 3998 "y.tab.c" /* yacc.c:1646  */
    break;

  case 264:
#line 1303 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_ip (); }
#line 4004 "y.tab.c" /* yacc.c:1646  */
    break;

  case 265:
#line 1304 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_eip(); }
#line 4010 "y.tab.c" /* yacc.c:1646  */
    break;

  case 266:
#line 1305 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
#line 4016 "y.tab.c" /* yacc.c:1646  */
    break;

  case 267:
#line 1306 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[-2].uval), (yyvsp[0].uval)); }
#line 4022 "y.tab.c" /* yacc.c:1646  */
    break;

  case 268:
#line 1307 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4028 "y.tab.c" /* yacc.c:1646  */
    break;

  case 269:
#line 1308 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4034 "y.tab.c" /* yacc.c:1646  */
    break;

  case 270:
#line 1309 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4040 "y.tab.c" /* yacc.c:1646  */
    break;

  case 271:
#line 1310 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = ((yyvsp[0].uval) != 0) ? (yyvsp[-2].uval) / (yyvsp[0].uval) : 0; }
#line 4046 "y.tab.c" /* yacc.c:1646  */
    break;

  case 272:
#line 1311 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4052 "y.tab.c" /* yacc.c:1646  */
    break;

  case 273:
#line 1312 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4058 "y.tab.c" /* yacc.c:1646  */
    break;

  case 274:
#line 1313 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4064 "y.tab.c" /* yacc.c:1646  */
    break;

  case 275:
#line 1314 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4070 "y.tab.c" /* yacc.c:1646  */
    break;

  case 276:
#line 1315 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4076 "y.tab.c" /* yacc.c:1646  */
    break;

  case 277:
#line 1316 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) > (yyvsp[0].uval); }
#line 4082 "y.tab.c" /* yacc.c:1646  */
    break;

  case 278:
#line 1317 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) < (yyvsp[0].uval); }
#line 4088 "y.tab.c" /* yacc.c:1646  */
    break;

  case 279:
#line 1318 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) == (yyvsp[0].uval); }
#line 4094 "y.tab.c" /* yacc.c:1646  */
    break;

  case 280:
#line 1319 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) != (yyvsp[0].uval); }
#line 4100 "y.tab.c" /* yacc.c:1646  */
    break;

  case 281:
#line 1320 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) <= (yyvsp[0].uval); }
#line 4106 "y.tab.c" /* yacc.c:1646  */
    break;

  case 282:
#line 1321 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) >= (yyvsp[0].uval); }
#line 4112 "y.tab.c" /* yacc.c:1646  */
    break;

  case 283:
#line 1322 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = !(yyvsp[0].uval); }
#line 4118 "y.tab.c" /* yacc.c:1646  */
    break;

  case 284:
#line 1323 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = -(yyvsp[0].uval); }
#line 4124 "y.tab.c" /* yacc.c:1646  */
    break;

  case 285:
#line 1324 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[0].uval)); }
#line 4130 "y.tab.c" /* yacc.c:1646  */
    break;

  case 286:
#line 1325 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[0].uval)); }
#line 4136 "y.tab.c" /* yacc.c:1646  */
    break;

  case 287:
#line 1326 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-1].uval); }
#line 4142 "y.tab.c" /* yacc.c:1646  */
    break;


#line 4146 "y.tab.c" /* yacc.c:1646  */
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

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

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
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 1329 "parser.y" /* yacc.c:1906  */

#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
