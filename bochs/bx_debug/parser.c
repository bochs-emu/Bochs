/* A Bison parser, made by GNU Bison 3.0.2.  */

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
#define YYBISON_VERSION "3.0.2"

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
    BX_TOKEN_STEPN = 274,
    BX_TOKEN_STEP_OVER = 275,
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
    BX_TOKEN_XMM = 292,
    BX_TOKEN_YMM = 293,
    BX_TOKEN_ZMM = 294,
    BX_TOKEN_AVX = 295,
    BX_TOKEN_IDT = 296,
    BX_TOKEN_IVT = 297,
    BX_TOKEN_GDT = 298,
    BX_TOKEN_LDT = 299,
    BX_TOKEN_TSS = 300,
    BX_TOKEN_TAB = 301,
    BX_TOKEN_ALL = 302,
    BX_TOKEN_LINUX = 303,
    BX_TOKEN_DEBUG_REGS = 304,
    BX_TOKEN_CONTROL_REGS = 305,
    BX_TOKEN_SEGMENT_REGS = 306,
    BX_TOKEN_EXAMINE = 307,
    BX_TOKEN_XFORMAT = 308,
    BX_TOKEN_DISFORMAT = 309,
    BX_TOKEN_RESTORE = 310,
    BX_TOKEN_WRITEMEM = 311,
    BX_TOKEN_SETPMEM = 312,
    BX_TOKEN_SYMBOLNAME = 313,
    BX_TOKEN_QUERY = 314,
    BX_TOKEN_PENDING = 315,
    BX_TOKEN_TAKE = 316,
    BX_TOKEN_DMA = 317,
    BX_TOKEN_IRQ = 318,
    BX_TOKEN_TLB = 319,
    BX_TOKEN_HEX = 320,
    BX_TOKEN_DISASM = 321,
    BX_TOKEN_INSTRUMENT = 322,
    BX_TOKEN_STRING = 323,
    BX_TOKEN_STOP = 324,
    BX_TOKEN_DOIT = 325,
    BX_TOKEN_CRC = 326,
    BX_TOKEN_TRACE = 327,
    BX_TOKEN_TRACEREG = 328,
    BX_TOKEN_TRACEMEM = 329,
    BX_TOKEN_SWITCH_MODE = 330,
    BX_TOKEN_SIZE = 331,
    BX_TOKEN_PTIME = 332,
    BX_TOKEN_TIMEBP_ABSOLUTE = 333,
    BX_TOKEN_TIMEBP = 334,
    BX_TOKEN_MODEBP = 335,
    BX_TOKEN_VMEXITBP = 336,
    BX_TOKEN_PRINT_STACK = 337,
    BX_TOKEN_WATCH = 338,
    BX_TOKEN_UNWATCH = 339,
    BX_TOKEN_READ = 340,
    BX_TOKEN_WRITE = 341,
    BX_TOKEN_SHOW = 342,
    BX_TOKEN_LOAD_SYMBOLS = 343,
    BX_TOKEN_SYMBOLS = 344,
    BX_TOKEN_LIST_SYMBOLS = 345,
    BX_TOKEN_GLOBAL = 346,
    BX_TOKEN_WHERE = 347,
    BX_TOKEN_PRINT_STRING = 348,
    BX_TOKEN_NUMERIC = 349,
    BX_TOKEN_PAGE = 350,
    BX_TOKEN_HELP = 351,
    BX_TOKEN_XML = 352,
    BX_TOKEN_CALC = 353,
    BX_TOKEN_DEVICE = 354,
    BX_TOKEN_GENERIC = 355,
    BX_TOKEN_RSHIFT = 356,
    BX_TOKEN_LSHIFT = 357,
    BX_TOKEN_REG_IP = 358,
    BX_TOKEN_REG_EIP = 359,
    BX_TOKEN_REG_RIP = 360,
    NOT = 361,
    NEG = 362,
    INDIRECT = 363
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
#define BX_TOKEN_STEPN 274
#define BX_TOKEN_STEP_OVER 275
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
#define BX_TOKEN_XMM 292
#define BX_TOKEN_YMM 293
#define BX_TOKEN_ZMM 294
#define BX_TOKEN_AVX 295
#define BX_TOKEN_IDT 296
#define BX_TOKEN_IVT 297
#define BX_TOKEN_GDT 298
#define BX_TOKEN_LDT 299
#define BX_TOKEN_TSS 300
#define BX_TOKEN_TAB 301
#define BX_TOKEN_ALL 302
#define BX_TOKEN_LINUX 303
#define BX_TOKEN_DEBUG_REGS 304
#define BX_TOKEN_CONTROL_REGS 305
#define BX_TOKEN_SEGMENT_REGS 306
#define BX_TOKEN_EXAMINE 307
#define BX_TOKEN_XFORMAT 308
#define BX_TOKEN_DISFORMAT 309
#define BX_TOKEN_RESTORE 310
#define BX_TOKEN_WRITEMEM 311
#define BX_TOKEN_SETPMEM 312
#define BX_TOKEN_SYMBOLNAME 313
#define BX_TOKEN_QUERY 314
#define BX_TOKEN_PENDING 315
#define BX_TOKEN_TAKE 316
#define BX_TOKEN_DMA 317
#define BX_TOKEN_IRQ 318
#define BX_TOKEN_TLB 319
#define BX_TOKEN_HEX 320
#define BX_TOKEN_DISASM 321
#define BX_TOKEN_INSTRUMENT 322
#define BX_TOKEN_STRING 323
#define BX_TOKEN_STOP 324
#define BX_TOKEN_DOIT 325
#define BX_TOKEN_CRC 326
#define BX_TOKEN_TRACE 327
#define BX_TOKEN_TRACEREG 328
#define BX_TOKEN_TRACEMEM 329
#define BX_TOKEN_SWITCH_MODE 330
#define BX_TOKEN_SIZE 331
#define BX_TOKEN_PTIME 332
#define BX_TOKEN_TIMEBP_ABSOLUTE 333
#define BX_TOKEN_TIMEBP 334
#define BX_TOKEN_MODEBP 335
#define BX_TOKEN_VMEXITBP 336
#define BX_TOKEN_PRINT_STACK 337
#define BX_TOKEN_WATCH 338
#define BX_TOKEN_UNWATCH 339
#define BX_TOKEN_READ 340
#define BX_TOKEN_WRITE 341
#define BX_TOKEN_SHOW 342
#define BX_TOKEN_LOAD_SYMBOLS 343
#define BX_TOKEN_SYMBOLS 344
#define BX_TOKEN_LIST_SYMBOLS 345
#define BX_TOKEN_GLOBAL 346
#define BX_TOKEN_WHERE 347
#define BX_TOKEN_PRINT_STRING 348
#define BX_TOKEN_NUMERIC 349
#define BX_TOKEN_PAGE 350
#define BX_TOKEN_HELP 351
#define BX_TOKEN_XML 352
#define BX_TOKEN_CALC 353
#define BX_TOKEN_DEVICE 354
#define BX_TOKEN_GENERIC 355
#define BX_TOKEN_RSHIFT 356
#define BX_TOKEN_LSHIFT 357
#define BX_TOKEN_REG_IP 358
#define BX_TOKEN_REG_EIP 359
#define BX_TOKEN_REG_RIP 360
#define NOT 361
#define NEG 362
#define INDIRECT 363

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 13 "parser.y" /* yacc.c:355  */

  char    *sval;
  Bit64u   uval;
  bx_bool  bval;

#line 344 "y.tab.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE bxlval;

int bxparse (void);

#endif /* !YY_BX_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 359 "y.tab.c" /* yacc.c:358  */

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
#define YYFINAL  292
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1817

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  123
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  57
/* YYNRULES -- Number of rules.  */
#define YYNRULES  280
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  544

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   363

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
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
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   140,   140,   141,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   200,   201,   206,   207,   212,   213,
     214,   215,   216,   217,   222,   227,   235,   243,   251,   256,
     261,   266,   271,   276,   284,   292,   300,   308,   316,   324,
     332,   337,   345,   350,   355,   360,   365,   370,   375,   380,
     385,   390,   395,   403,   408,   413,   418,   426,   434,   442,
     450,   455,   460,   465,   473,   481,   486,   491,   495,   499,
     503,   507,   511,   515,   519,   526,   531,   536,   541,   546,
     551,   556,   561,   569,   577,   582,   590,   595,   600,   605,
     610,   615,   620,   625,   630,   635,   640,   645,   650,   655,
     660,   665,   670,   678,   679,   682,   690,   698,   706,   714,
     722,   730,   738,   746,   754,   762,   769,   777,   785,   790,
     795,   800,   808,   816,   824,   832,   840,   845,   850,   858,
     863,   868,   873,   878,   883,   888,   893,   898,   906,   911,
     919,   927,   935,   940,   945,   952,   957,   962,   967,   972,
     977,   982,   987,   992,   997,  1002,  1007,  1013,  1019,  1025,
    1033,  1038,  1043,  1048,  1053,  1058,  1063,  1068,  1073,  1078,
    1083,  1088,  1093,  1098,  1103,  1108,  1113,  1118,  1128,  1139,
    1145,  1158,  1163,  1174,  1179,  1195,  1211,  1221,  1226,  1234,
    1253,  1254,  1255,  1256,  1257,  1258,  1259,  1260,  1261,  1262,
    1263,  1264,  1265,  1266,  1267,  1268,  1269,  1270,  1271,  1272,
    1273,  1274,  1275,  1276,  1282,  1283,  1284,  1285,  1286,  1287,
    1288,  1289,  1290,  1291,  1292,  1293,  1294,  1295,  1296,  1297,
    1298,  1299,  1300,  1301,  1302,  1303,  1304,  1305,  1306,  1307,
    1308
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
  "BX_TOKEN_ON", "BX_TOKEN_OFF", "BX_TOKEN_CONTINUE", "BX_TOKEN_STEPN",
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
  "BX_TOKEN_HELP", "BX_TOKEN_XML", "BX_TOKEN_CALC", "BX_TOKEN_DEVICE",
  "BX_TOKEN_GENERIC", "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT",
  "BX_TOKEN_REG_IP", "BX_TOKEN_REG_EIP", "BX_TOKEN_REG_RIP", "'+'", "'-'",
  "'|'", "'^'", "'*'", "'/'", "'&'", "NOT", "NEG", "INDIRECT", "'\\n'",
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
  "mmx_regs_command", "xmm_regs_command", "ymm_regs_command",
  "zmm_regs_command", "segment_regs_command", "control_regs_command",
  "debug_regs_command", "delete_command", "bpe_command", "bpd_command",
  "quit_command", "examine_command", "restore_command", "writemem_command",
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
     355,   356,   357,   358,   359,   360,    43,    45,   124,    94,
      42,    47,    38,   361,   362,   363,    10,    61,    58,    33,
      40,    41,    64
};
# endif

#define YYPACT_NINF -148

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-148)))

#define YYTABLE_NINF -278

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1332,   -94,   -33,   -74,    68,   -68,  1126,   733,   779,   -39,
     -32,   -30,   329,   -51,  -148,  -148,   -50,   -49,   -47,   -31,
     -28,   -27,   -26,   -25,   103,     0,    16,  1106,    32,   -12,
    1106,   659,   -48,  1106,  1106,    37,    37,    37,   -23,  1106,
    1106,   -16,   -11,   -76,    13,   799,     2,   -55,   -58,     3,
    1106,  1106,  1414,  1106,  -148,  1250,  -148,     4,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,     7,   -69,  -148,
    -148,   -13,     6,     8,    10,    11,  -148,  -148,  -148,  -148,
    -148,  -148,    24,    37,    25,    26,    30,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  1144,
    -148,  1144,  1144,  -148,  1096,  -148,  -148,  -148,  -148,  -148,
    -148,    17,  -148,  -148,  -148,  -148,  1106,  1106,  -148,  1106,
    1106,  1106,  -148,   374,  -148,  1106,  -148,   440,    35,    39,
      41,    70,    71,    72,  1106,  1106,  1106,  1106,    75,    76,
      77,   -57,   -59,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,   919,  -148,   506,    56,  1106,   592,    78,   -60,
      80,   573,   939,    37,    82,    31,  -148,   156,    89,    93,
     713,   592,  -148,  -148,    95,    96,    98,  -148,   769,  1059,
    -148,  -148,    99,  -148,   100,  1106,   101,  1106,  1106,  -148,
    -148,  1077,   112,   113,   -81,   122,  -148,   959,   171,   124,
    -148,  -148,  1366,  1413,   126,   128,   129,   131,   132,   133,
     135,   136,   137,   138,   139,   140,   153,   154,   155,   157,
     161,   163,   164,   165,   166,   167,   168,   169,   170,   172,
     173,   174,   181,   185,   186,   187,   188,   189,   190,   191,
     192,   203,   213,   227,   229,   230,   231,   232,   233,  -148,
     234,  1426,  -148,  -148,  -148,   235,   236,  -148,  1106,  1106,
    1106,  1106,  1106,  1106,   237,  1106,  1106,  1106,  -148,  -148,
     649,  1144,  1144,  1144,  1144,  1144,  1144,  1144,  1144,  1144,
    1144,  -148,   123,   123,   123,   125,   123,  1106,  1106,  1106,
    1106,  1106,  1106,  1106,  1106,  1106,  -148,  1106,   -79,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  1106,  1699,  1106,  1106,
    1106,  -148,  -148,  -148,   238,  -148,   -56,   -53,  -148,  -148,
    1439,  -148,   244,   592,  1106,  1106,   592,  -148,   245,  -148,
    -148,  -148,  -148,   328,   246,  -148,   193,  -148,  1452,  -148,
    -148,  -148,  1465,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
     394,  -148,   460,   526,  -148,  -148,  -148,   248,  -148,  -148,
    -148,  1478,   986,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  1491,  1504,  1517,  1530,  1543,
    1556,  -148,  1569,  1582,  1595,  -148,  -148,  -148,    34,    34,
      34,    34,  -148,  -148,  -148,   909,  -148,   123,   123,   -15,
     -15,   -15,   -15,   123,   123,   123,  1699,  -148,   249,   251,
     252,   253,  -148,   260,  -148,   262,  -148,  -148,  -148,  1608,
      20,   123,  1621,  -148,  -148,  1634,  -148,   263,  -148,  -148,
    -148,  1647,  -148,  1660,  -148,  1673,  -148,  -148,  -148,  1686,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148
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
      31,    32,    33,    34,    50,    51,    99,     0,     0,   100,
     104,     0,     0,     0,     0,     0,    58,    59,    60,    61,
      62,    63,     0,     0,     0,     0,     0,   123,   233,   232,
     234,   235,   236,   237,   231,   230,   239,   240,   241,     0,
     115,     0,     0,   238,     0,   257,   256,   258,   259,   260,
     261,   255,   254,   263,   264,   265,     0,     0,   117,     0,
       0,     0,   262,     0,   255,     0,   120,     0,     0,     0,
       0,     0,     0,     0,   143,   143,   143,   143,     0,     0,
       0,     0,     0,   157,   146,   147,   148,   149,   150,   153,
     152,   151,     0,   161,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   169,     0,     0,     0,
       0,     0,    54,    55,     0,     0,     0,    76,     0,     0,
      66,    67,     0,    80,     0,     0,     0,     0,     0,    84,
      91,     0,     0,     0,     0,     0,    73,     0,     0,     0,
     124,    97,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   228,
       0,     0,     1,     2,   145,     0,     0,   101,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   252,   251,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   119,   277,   278,   276,     0,   279,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   118,     0,   278,   121,
     154,   155,   156,   134,   126,   127,   143,   144,   143,   143,
     143,   133,   132,   135,     0,   136,     0,     0,   138,   159,
       0,   160,     0,     0,     0,     0,     0,   165,     0,   166,
     168,    75,   172,     0,     0,   175,     0,   170,     0,   178,
     179,   180,     0,    77,    78,    79,    65,    64,    81,    83,
       0,    82,     0,     0,    92,    70,    69,     0,    71,    68,
      93,     0,     0,   125,    98,    74,   183,   184,   185,   222,
     192,   186,   187,   188,   189,   190,   191,   224,   182,   207,
     208,   209,   210,   211,   214,   213,   212,   220,   199,   215,
     216,   217,   221,   195,   196,   197,   198,   200,   202,   201,
     193,   194,   203,   218,   219,   225,   204,   205,   223,   227,
     226,   206,   229,   102,   103,     0,     0,     0,     0,     0,
       0,   105,     0,     0,     0,   253,   246,   247,   242,   243,
     248,   249,   244,   245,   250,     0,   280,   271,   272,   267,
     268,   273,   274,   269,   270,   275,   266,   122,     0,     0,
       0,     0,   137,     0,   141,     0,   139,   158,   162,     0,
     268,   269,     0,   167,   173,     0,   176,     0,   171,   181,
      85,     0,    86,     0,    87,     0,    72,    94,    95,     0,
     108,   107,   109,   110,   111,   106,   112,   113,   114,   116,
     128,   129,   130,   131,   142,   140,   163,   164,   174,   177,
      88,    89,    90,    96
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -148,  -148,   325,   -29,   330,    -2,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -147,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -136,    -7
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    55,    56,   214,    57,   162,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,   346,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   144,   347
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     163,   167,   126,   308,   143,   309,   310,   215,   216,   356,
     239,   354,   493,   237,   107,   495,   397,   194,   222,   232,
     197,   208,   106,   201,   207,   296,   210,   211,   348,   349,
     350,   224,   218,   219,   368,   398,   238,   487,   231,   337,
     223,   357,   110,   242,   243,   225,   291,   297,   127,   233,
     199,   200,   209,   212,   213,   168,   369,   358,   240,   355,
     494,   108,   169,   496,   170,   183,   184,   185,   195,   186,
     234,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   226,   109,   196,   187,   327,   328,   188,   189,
     190,   191,   198,   217,   304,   333,   334,   335,   227,   228,
     220,   295,   235,   337,   298,   221,   145,   146,   147,   148,
     149,   116,   117,   118,   119,   120,   121,   150,   236,   241,
     294,  -277,  -277,   299,   362,   300,   122,   301,   302,   229,
    -277,  -277,  -277,   321,   123,   311,   312,   143,   337,   143,
     143,   303,   305,   306,   317,   318,   319,   307,   376,   322,
     323,   340,   324,   325,   326,   341,   192,   342,   338,   145,
     146,   147,   148,   149,   116,   117,   118,   119,   120,   121,
     150,   164,   124,   125,   374,   466,   467,   468,   469,   470,
     471,   472,   473,   474,   475,   360,   343,   344,   345,   363,
     366,   351,   352,   353,   367,   373,   370,   152,   375,   488,
     378,   489,   490,   491,   382,   379,   153,   154,   155,   380,
     156,   383,   384,   157,   385,   388,   389,   391,   390,   193,
     392,   393,   159,   160,   164,   161,   327,   328,   395,   396,
     401,   329,   330,   331,   332,   333,   334,   335,   399,   402,
     403,   337,   406,   337,   407,   408,   476,   409,   410,   411,
     152,   412,   413,   414,   415,   416,   417,   327,   328,   153,
     154,   155,   329,   364,   331,   332,   365,   334,   335,   418,
     419,   420,   377,   421,   337,   159,   160,   422,   161,   423,
     424,   425,   426,   427,   428,   429,   430,   507,   431,   432,
     433,   455,   456,   457,   458,   459,   460,   434,   462,   463,
     464,   435,   436,   437,   438,   439,   440,   441,   442,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   443,
     477,   478,   479,   480,   481,   482,   483,   484,   485,   444,
     486,   145,   146,   147,   148,   149,   116,   117,   118,   119,
     120,   121,   150,   445,   171,   446,   447,   448,   449,   450,
     451,   453,   454,   461,   492,   172,   499,   500,   501,   502,
     498,   503,   506,   173,   516,   530,   505,   531,   532,   533,
     174,   175,   176,   177,   178,   179,   534,   180,   535,   539,
     293,     0,   290,   511,     0,   513,   515,     0,     0,     0,
       0,     0,     0,     0,     0,   519,   164,   145,   146,   147,
     148,   149,   116,   117,   118,   119,   120,   121,   150,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   181,     0,
       0,     0,   152,     0,     0,     0,     0,     0,   182,   327,
     328,   153,   154,   155,   329,   364,   331,   332,   365,   334,
     335,     0,     0,     0,   504,     0,   337,   159,   160,     0,
     161,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   164,   145,   146,   147,   148,   149,   116,   117,
     118,   119,   120,   121,   150,   327,   328,     0,     0,     0,
     329,   330,   331,   332,   333,   334,   335,     0,   152,     0,
     336,     0,   337,     0,     0,   327,   328,   153,   154,   155,
     329,   364,   331,   332,   365,   334,   335,     0,     0,     0,
     510,     0,   337,   159,   160,     0,   161,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   164,   145,
     146,   147,   148,   149,   116,   117,   118,   119,   120,   121,
     150,   327,   328,     0,     0,     0,   329,   330,   331,   332,
     333,   334,   335,     0,   152,     0,   339,     0,   337,     0,
       0,   327,   328,   153,   154,   155,   329,   364,   331,   332,
     365,   334,   335,     0,     0,     0,   512,     0,   337,   159,
     160,     0,   161,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   164,   145,   146,   147,   148,   149,
     116,   117,   118,   119,   120,   121,   150,   327,   328,     0,
       0,     0,   329,   330,   331,   332,   333,   334,   335,     0,
     152,     0,   361,     0,   337,     0,     0,   327,   328,   153,
     154,   155,   329,   364,   331,   332,   365,   334,   335,     0,
       0,     0,   514,     0,   337,   159,   160,     0,   161,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     164,     0,   145,   146,   147,   148,   149,   116,   117,   118,
     119,   120,   121,   150,   327,   328,     0,     0,     0,   329,
     330,   331,   332,   333,   334,   335,   152,     0,     0,   371,
       0,   337,     0,   327,   328,   153,   154,   155,   329,   364,
     331,   332,   365,   334,   335,     0,     0,     0,     0,     0,
     337,   159,   160,   202,   161,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   203,     0,     0,   164,     0,     0,
       0,     0,     0,     0,   204,   205,   145,   146,   147,   148,
     149,   116,   117,   118,   119,   120,   121,   150,     0,     0,
     311,   312,     0,   152,     0,   313,   314,   315,   316,   317,
     318,   319,   153,   154,   155,     0,   156,     0,     0,   157,
     465,     0,     0,     0,     0,   206,     0,     0,   159,   160,
       0,   161,   145,   146,   147,   148,   149,   116,   117,   118,
     119,   120,   121,   150,     0,     0,     0,     0,     0,     0,
       0,   151,   145,   146,   147,   148,   149,   116,   117,   118,
     119,   120,   121,   150,   327,   328,     0,     0,     0,   329,
     330,   331,   332,   333,   334,   335,     0,   152,     0,   381,
       0,   337,     0,     0,     0,     0,   153,   154,   155,     0,
     156,     0,     0,   157,     0,     0,     0,   164,     0,   158,
       0,     0,   159,   160,     0,   161,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   164,     0,     0,
     327,   328,     0,   152,     0,   329,   330,   331,   332,   333,
     334,   335,   153,   154,   155,   386,   156,   337,     0,   165,
       0,     0,     0,   152,     0,   166,     0,     0,   159,   160,
       0,   161,   153,   154,   155,     0,   156,     0,     0,   157,
       0,     0,     0,     0,     0,   230,     0,     0,   159,   160,
       0,   161,   145,   146,   147,   148,   149,   116,   117,   118,
     119,   120,   121,   150,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,   149,   116,   117,   118,
     119,   120,   121,   150,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,   149,   116,   117,   118,
     119,   120,   121,   150,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   164,     0,   145,
     146,   147,   148,   149,   116,   117,   118,   119,   120,   121,
     150,     0,     0,     0,     0,     0,     0,   164,     0,     0,
     311,   312,     0,   152,     0,   313,   314,   315,   316,   317,
     318,   319,   153,   154,   155,   529,   156,   164,     0,   157,
       0,     0,     0,   152,     0,   359,     0,     0,   159,   160,
       0,   161,   153,   154,   155,     0,   156,     0,     0,   157,
       0,     0,     0,   152,   164,   372,     0,     0,   159,   160,
       0,   161,   153,   154,   155,     0,   156,     0,     0,   157,
       0,     0,     0,     0,     0,   400,     0,     0,   159,   160,
     152,   161,     0,     0,     0,     0,     0,     0,     0,   153,
     154,   155,     0,   156,     0,     0,   157,     0,     0,     0,
       0,     0,   518,     0,     0,   159,   160,     0,   161,   145,
     146,   147,   148,   149,   116,   117,   118,   119,   120,   121,
     150,     0,     0,     0,     0,     0,     0,     0,     0,   128,
     129,   130,   131,   132,   116,   117,   118,   119,   120,   121,
     133,     0,     0,     0,     0,     0,     0,   128,   129,   130,
     131,   132,   116,   117,   118,   119,   120,   121,   133,     0,
     327,   328,     0,     0,     0,   329,   330,   331,   332,   333,
     334,   335,     0,     0,   164,   387,     0,   337,   327,   328,
       0,     0,     0,   329,   330,   331,   332,   333,   334,   335,
       0,     0,     0,   394,   134,   337,     0,   311,   312,     0,
     152,     0,   313,   314,   315,   316,   317,   318,   319,   153,
     154,   155,   134,   156,   320,     0,   157,     0,     0,     0,
     135,     0,     0,     0,     0,   159,   160,     0,   161,   136,
     137,   138,     0,   139,     0,     0,     0,     0,   135,     0,
       0,     0,   140,     0,     0,   141,   142,   136,   137,   138,
     292,   139,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   141,   142,     0,     0,     0,     1,     2,
       3,     4,     0,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,     0,    16,    17,    18,    19,    20,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    21,
      22,    23,    24,     0,     0,    25,    26,    27,     0,    28,
       0,    29,     0,     0,    30,     0,    31,    32,     0,     0,
      33,    34,    35,    36,    37,     0,     0,    38,    39,    40,
      41,    42,    43,    44,    45,     0,     0,    46,    47,     0,
      48,     0,    49,    50,     0,    51,    52,     0,    53,     0,
       1,     2,     3,     4,     0,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    54,    16,    17,    18,
      19,    20,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    21,    22,    23,    24,     0,     0,    25,    26,    27,
       0,    28,     0,    29,     0,     0,    30,     0,    31,    32,
       0,     0,    33,    34,    35,    36,    37,     0,     0,    38,
      39,    40,    41,    42,    43,    44,    45,     0,     0,    46,
      47,     0,    48,     0,    49,    50,     0,    51,    52,     0,
      53,     0,   244,   245,   246,   247,     0,   248,   249,   250,
     251,   252,   253,   254,   255,   256,    14,    15,    54,   257,
     258,   259,   260,   261,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   262,   263,   264,   265,   327,   328,   266,
     267,   268,   329,   330,   331,   332,   333,   334,   335,     0,
     269,   270,   404,     0,   337,   271,   272,   273,   274,     0,
       0,   275,   276,   277,   278,   279,   280,   281,   282,     0,
       0,   283,   284,     0,   285,     0,     0,     0,     0,   286,
     287,     0,   288,     0,   327,   328,     0,     0,     0,   329,
     330,   331,   332,   333,   334,   335,     0,   327,   328,   405,
     289,   337,   329,   330,   331,   332,   333,   334,   335,     0,
     327,   328,   452,     0,   337,   329,   330,   331,   332,   333,
     334,   335,     0,   327,   328,   497,     0,   337,   329,   330,
     331,   332,   333,   334,   335,     0,   327,   328,   508,     0,
     337,   329,   330,   331,   332,   333,   334,   335,     0,   327,
     328,   509,     0,   337,   329,   330,   331,   332,   333,   334,
     335,     0,   327,   328,   517,     0,   337,   329,   330,   331,
     332,   333,   334,   335,     0,   327,   328,   520,     0,   337,
     329,   330,   331,   332,   333,   334,   335,     0,   327,   328,
     521,     0,   337,   329,   330,   331,   332,   333,   334,   335,
       0,   327,   328,   522,     0,   337,   329,   330,   331,   332,
     333,   334,   335,     0,   327,   328,   523,     0,   337,   329,
     330,   331,   332,   333,   334,   335,     0,   327,   328,   524,
       0,   337,   329,   330,   331,   332,   333,   334,   335,     0,
     327,   328,   525,     0,   337,   329,   330,   331,   332,   333,
     334,   335,     0,   327,   328,   526,     0,   337,   329,   330,
     331,   332,   333,   334,   335,     0,   327,   328,   527,     0,
     337,   329,   330,   331,   332,   333,   334,   335,     0,   327,
     328,   528,     0,   337,   329,   330,   331,   332,   333,   334,
     335,     0,   327,   328,   536,     0,   337,   329,   330,   331,
     332,   333,   334,   335,     0,   327,   328,   537,     0,   337,
     329,   330,   331,   332,   333,   334,   335,     0,   327,   328,
     538,     0,   337,   329,   330,   331,   332,   333,   334,   335,
       0,   327,   328,   540,     0,   337,   329,   330,   331,   332,
     333,   334,   335,     0,   327,   328,   541,     0,   337,   329,
     330,   331,   332,   333,   334,   335,     0,   327,   328,   542,
       0,   337,   329,   330,   331,   332,   333,   334,   335,     0,
     327,   328,   543,     0,   337,   329,   330,   331,   332,   333,
     334,   335,     0,     0,     0,     0,     0,   337
};

static const yytype_int16 yycheck[] =
{
       7,     8,     4,   139,     6,   141,   142,    36,    37,    68,
      68,    68,    68,    68,    47,    68,    97,    24,    94,    17,
      27,    69,   116,    30,    31,    94,    33,    34,   175,   176,
     177,    18,    39,    40,    94,   116,    91,   116,    45,   118,
     116,   100,   116,    50,    51,    32,    53,   116,   116,    47,
      62,    63,   100,    16,    17,    94,   116,   116,   116,   116,
     116,    94,    94,   116,    94,   116,   116,   116,    68,   116,
      68,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    69,   116,    68,   116,   101,   102,   116,   116,
     116,   116,    60,   116,   123,   110,   111,   112,    85,    86,
     116,    94,   100,   118,   117,   116,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,   116,   116,
     116,   101,   102,   117,    68,   117,    58,   117,   117,   116,
     110,   111,   112,   116,    66,   101,   102,   139,   118,   141,
     142,   117,   117,   117,   110,   111,   112,   117,   117,   156,
     157,   116,   159,   160,   161,   116,    53,   116,   165,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    68,   104,   105,   203,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   192,   116,   116,   116,   196,
     197,   116,   116,   116,   116,   202,   116,    94,   116,   346,
     207,   348,   349,   350,   211,   116,   103,   104,   105,   116,
     107,   116,   116,   110,   116,   116,   116,   116,   225,   116,
     227,   228,   119,   120,    68,   122,   101,   102,   116,   116,
     237,   106,   107,   108,   109,   110,   111,   112,   116,    68,
     116,   118,   116,   118,   116,   116,   121,   116,   116,   116,
      94,   116,   116,   116,   116,   116,   116,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   116,
     116,   116,   116,   116,   118,   119,   120,   116,   122,   116,
     116,   116,   116,   116,   116,   116,   116,    94,   116,   116,
     116,   298,   299,   300,   301,   302,   303,   116,   305,   306,
     307,   116,   116,   116,   116,   116,   116,   116,   116,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   116,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   116,
     337,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,   116,    15,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,    26,   363,   364,   365,   366,
     116,   116,   116,    34,   116,   116,   373,   116,   116,   116,
      41,    42,    43,    44,    45,    46,   116,    48,   116,   116,
      55,    -1,    52,   390,    -1,   392,   393,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   402,    68,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,
      -1,    -1,    94,    -1,    -1,    -1,    -1,    -1,    99,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,    -1,    -1,    -1,   116,    -1,   118,   119,   120,    -1,
     122,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    68,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,   101,   102,    -1,    -1,    -1,
     106,   107,   108,   109,   110,   111,   112,    -1,    94,    -1,
     116,    -1,   118,    -1,    -1,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,    -1,    -1,    -1,
     116,    -1,   118,   119,   120,    -1,   122,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,   101,   102,    -1,    -1,    -1,   106,   107,   108,   109,
     110,   111,   112,    -1,    94,    -1,   116,    -1,   118,    -1,
      -1,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,    -1,    -1,    -1,   116,    -1,   118,   119,
     120,    -1,   122,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    68,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,   101,   102,    -1,
      -1,    -1,   106,   107,   108,   109,   110,   111,   112,    -1,
      94,    -1,   116,    -1,   118,    -1,    -1,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,    -1,
      -1,    -1,   116,    -1,   118,   119,   120,    -1,   122,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,   101,   102,    -1,    -1,    -1,   106,
     107,   108,   109,   110,   111,   112,    94,    -1,    -1,   116,
      -1,   118,    -1,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,    -1,    -1,    -1,    -1,    -1,
     118,   119,   120,    54,   122,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    65,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
     101,   102,    -1,    94,    -1,   106,   107,   108,   109,   110,
     111,   112,   103,   104,   105,    -1,   107,    -1,    -1,   110,
     121,    -1,    -1,    -1,    -1,   116,    -1,    -1,   119,   120,
      -1,   122,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    68,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,   101,   102,    -1,    -1,    -1,   106,
     107,   108,   109,   110,   111,   112,    -1,    94,    -1,   116,
      -1,   118,    -1,    -1,    -1,    -1,   103,   104,   105,    -1,
     107,    -1,    -1,   110,    -1,    -1,    -1,    68,    -1,   116,
      -1,    -1,   119,   120,    -1,   122,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
     101,   102,    -1,    94,    -1,   106,   107,   108,   109,   110,
     111,   112,   103,   104,   105,   116,   107,   118,    -1,   110,
      -1,    -1,    -1,    94,    -1,   116,    -1,    -1,   119,   120,
      -1,   122,   103,   104,   105,    -1,   107,    -1,    -1,   110,
      -1,    -1,    -1,    -1,    -1,   116,    -1,    -1,   119,   120,
      -1,   122,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
     101,   102,    -1,    94,    -1,   106,   107,   108,   109,   110,
     111,   112,   103,   104,   105,   116,   107,    68,    -1,   110,
      -1,    -1,    -1,    94,    -1,   116,    -1,    -1,   119,   120,
      -1,   122,   103,   104,   105,    -1,   107,    -1,    -1,   110,
      -1,    -1,    -1,    94,    68,   116,    -1,    -1,   119,   120,
      -1,   122,   103,   104,   105,    -1,   107,    -1,    -1,   110,
      -1,    -1,    -1,    -1,    -1,   116,    -1,    -1,   119,   120,
      94,   122,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   103,
     104,   105,    -1,   107,    -1,    -1,   110,    -1,    -1,    -1,
      -1,    -1,   116,    -1,    -1,   119,   120,    -1,   122,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
     101,   102,    -1,    -1,    -1,   106,   107,   108,   109,   110,
     111,   112,    -1,    -1,    68,   116,    -1,   118,   101,   102,
      -1,    -1,    -1,   106,   107,   108,   109,   110,   111,   112,
      -1,    -1,    -1,   116,    68,   118,    -1,   101,   102,    -1,
      94,    -1,   106,   107,   108,   109,   110,   111,   112,   103,
     104,   105,    68,   107,   118,    -1,   110,    -1,    -1,    -1,
      94,    -1,    -1,    -1,    -1,   119,   120,    -1,   122,   103,
     104,   105,    -1,   107,    -1,    -1,    -1,    -1,    94,    -1,
      -1,    -1,   116,    -1,    -1,   119,   120,   103,   104,   105,
       0,   107,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   119,   120,    -1,    -1,    -1,    18,    19,
      20,    21,    -1,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    -1,    35,    36,    37,    38,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    51,    52,    -1,    -1,    55,    56,    57,    -1,    59,
      -1,    61,    -1,    -1,    64,    -1,    66,    67,    -1,    -1,
      70,    71,    72,    73,    74,    -1,    -1,    77,    78,    79,
      80,    81,    82,    83,    84,    -1,    -1,    87,    88,    -1,
      90,    -1,    92,    93,    -1,    95,    96,    -1,    98,    -1,
      18,    19,    20,    21,    -1,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,   116,    35,    36,    37,
      38,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    51,    52,    -1,    -1,    55,    56,    57,
      -1,    59,    -1,    61,    -1,    -1,    64,    -1,    66,    67,
      -1,    -1,    70,    71,    72,    73,    74,    -1,    -1,    77,
      78,    79,    80,    81,    82,    83,    84,    -1,    -1,    87,
      88,    -1,    90,    -1,    92,    93,    -1,    95,    96,    -1,
      98,    -1,    18,    19,    20,    21,    -1,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,   116,    35,
      36,    37,    38,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    50,    51,    52,   101,   102,    55,
      56,    57,   106,   107,   108,   109,   110,   111,   112,    -1,
      66,    67,   116,    -1,   118,    71,    72,    73,    74,    -1,
      -1,    77,    78,    79,    80,    81,    82,    83,    84,    -1,
      -1,    87,    88,    -1,    90,    -1,    -1,    -1,    -1,    95,
      96,    -1,    98,    -1,   101,   102,    -1,    -1,    -1,   106,
     107,   108,   109,   110,   111,   112,    -1,   101,   102,   116,
     116,   118,   106,   107,   108,   109,   110,   111,   112,    -1,
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
     101,   102,   116,    -1,   118,   106,   107,   108,   109,   110,
     111,   112,    -1,    -1,    -1,    -1,    -1,   118
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    18,    19,    20,    21,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    35,    36,    37,    38,
      39,    49,    50,    51,    52,    55,    56,    57,    59,    61,
      64,    66,    67,    70,    71,    72,    73,    74,    77,    78,
      79,    80,    81,    82,    83,    84,    87,    88,    90,    92,
      93,    95,    96,    98,   116,   124,   125,   127,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   116,    47,    94,   116,
     116,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    58,    66,   104,   105,   128,   116,     3,     4,
       5,     6,     7,    14,    68,    94,   103,   104,   105,   107,
     116,   119,   120,   128,   178,     3,     4,     5,     6,     7,
      14,    68,    94,   103,   104,   105,   107,   110,   116,   119,
     120,   122,   128,   179,    68,   110,   116,   179,    94,    94,
      94,    15,    26,    34,    41,    42,    43,    44,    45,    46,
      48,    89,    99,   116,   116,   116,   116,   116,   116,   116,
     116,   116,    53,   116,   179,    68,    68,   179,    60,    62,
      63,   179,    54,    65,    75,    76,   116,   179,    69,   100,
     179,   179,    16,    17,   126,   126,   126,   116,   179,   179,
     116,   116,    94,   116,    18,    32,    69,    85,    86,   116,
     116,   179,    17,    47,    68,   100,   116,    68,    91,    68,
     116,   116,   179,   179,    18,    19,    20,    21,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    35,    36,    37,
      38,    39,    49,    50,    51,    52,    55,    56,    57,    66,
      67,    71,    72,    73,    74,    77,    78,    79,    80,    81,
      82,    83,    84,    87,    88,    90,    95,    96,    98,   116,
     127,   179,     0,   125,   116,    94,    94,   116,   117,   117,
     117,   117,   117,   117,   126,   117,   117,   117,   178,   178,
     178,   101,   102,   106,   107,   108,   109,   110,   111,   112,
     118,   116,   179,   179,   179,   179,   179,   101,   102,   106,
     107,   108,   109,   110,   111,   112,   116,   118,   179,   116,
     116,   116,   116,   116,   116,   116,   152,   179,   152,   152,
     152,   116,   116,   116,    68,   116,    68,   100,   116,   116,
     179,   116,    68,   179,   107,   110,   179,   116,    94,   116,
     116,   116,   116,   179,   126,   116,   117,   116,   179,   116,
     116,   116,   179,   116,   116,   116,   116,   116,   116,   116,
     179,   116,   179,   179,   116,   116,   116,    97,   116,   116,
     116,   179,    68,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   179,   179,   179,   179,   179,
     179,   116,   179,   179,   179,   121,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   121,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   116,   152,   152,
     152,   152,   116,    68,   116,    68,   116,   116,   116,   179,
     179,   179,   179,   116,   116,   179,   116,    94,   116,   116,
     116,   179,   116,   179,   116,   179,   116,   116,   116,   179,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116
};

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
     132,   132,   132,   132,   133,   134,   135,   136,   137,   138,
     139,   139,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   141,   141,   141,   141,   142,   143,   144,
     145,   145,   145,   145,   146,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   148,   148,   148,   148,   148,
     148,   148,   148,   149,   150,   150,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   152,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   166,
     166,   166,   167,   168,   169,   170,   171,   171,   171,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   173,   173,
     174,   175,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   177,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179
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
       2,     3,     4,     4,     2,     4,     5,     5,     5,     5,
       5,     5,     5,     5,     5,     2,     5,     2,     3,     3,
       2,     3,     4,     2,     2,     3,     3,     3,     5,     5,
       5,     5,     3,     3,     3,     3,     3,     4,     3,     4,
       5,     4,     5,     0,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     3,     3,     2,     4,     3,
       3,     2,     4,     5,     5,     3,     3,     4,     3,     2,
       3,     4,     3,     4,     5,     3,     4,     5,     3,     3,
       3,     4,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     2,     2,
       3
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
#line 195 "parser.y" /* yacc.c:1646  */
    {
      }
#line 2114 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 202 "parser.y" /* yacc.c:1646  */
    { (yyval.bval)=(yyvsp[0].bval); }
#line 2120 "y.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 208 "parser.y" /* yacc.c:1646  */
    { (yyval.sval)=(yyvsp[0].sval); }
#line 2126 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 218 "parser.y" /* yacc.c:1646  */
    { (yyval.uval)=(yyvsp[0].uval); }
#line 2132 "y.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 223 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_timebp_command(0, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2141 "y.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 228 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_timebp_command(1, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2150 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 236 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_modebp_command();
          free((yyvsp[-1].sval));
      }
#line 2159 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 244 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_vmexitbp_command();
          free((yyvsp[-1].sval));
      }
#line 2168 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 252 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_command((yyvsp[-1].sval));
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2177 "y.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 257 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_command("all");
          free((yyvsp[-2].sval));
      }
#line 2186 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 262 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_command("off");
          free((yyvsp[-2].sval));
      }
#line 2195 "y.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 267 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_param_command((yyvsp[-1].sval), 0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2204 "y.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 272 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_param_command((yyvsp[-2].sval), 1);
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2213 "y.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 277 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_command(0);
          free((yyvsp[-1].sval));
      }
#line 2222 "y.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 285 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_xlate_address((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2231 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 293 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_tlb_lookup((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2240 "y.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 301 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_ptime_command();
          free((yyvsp[-1].sval));
      }
#line 2249 "y.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 309 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_trace_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2258 "y.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 317 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_trace_reg_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2267 "y.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 325 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_trace_mem_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2276 "y.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 333 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_print_stack_command(16);
          free((yyvsp[-1].sval));
      }
#line 2285 "y.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 338 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_print_stack_command((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2294 "y.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 346 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2303 "y.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 351 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2312 "y.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 356 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_print_watchpoints();
          free((yyvsp[-1].sval));
      }
#line 2321 "y.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 361 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2330 "y.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 366 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2339 "y.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 371 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(1, (yyvsp[-1].uval), 1); /* BX_WRITE */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2348 "y.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 376 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2357 "y.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 381 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2366 "y.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 386 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(1, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_WRITE */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2375 "y.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 391 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_unwatch_all();
          free((yyvsp[-1].sval));
      }
#line 2384 "y.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 396 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_unwatch((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2393 "y.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 404 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_symbol_command((yyvsp[-1].sval), 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2402 "y.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 409 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_symbol_command((yyvsp[-2].sval), 0, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2411 "y.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 414 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_symbol_command((yyvsp[-1].sval), 1, 0);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2420 "y.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 419 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_symbol_command((yyvsp[-2].sval), 1, (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2429 "y.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 427 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_where_command();
        free((yyvsp[-1].sval));
      }
#line 2438 "y.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 435 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_print_string_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2447 "y.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 443 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_continue_command();
        free((yyvsp[-1].sval));
      }
#line 2456 "y.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 451 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[-1].sval));
      }
#line 2465 "y.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 456 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2474 "y.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 461 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_stepN_command(-1, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2483 "y.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 466 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_stepN_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2492 "y.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 474 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_step_over_command();
        free((yyvsp[-1].sval));
      }
#line 2501 "y.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 482 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_set_auto_disassemble((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2510 "y.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 487 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_set_symbol_command((yyvsp[-3].sval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2519 "y.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 492 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg8l_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2527 "y.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 496 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg8h_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2535 "y.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 500 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg16_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2543 "y.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 504 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg32_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2551 "y.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 508 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg64_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2559 "y.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 512 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2567 "y.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 516 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2575 "y.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 520 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_load_segreg((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2583 "y.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 527 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 2592 "y.tab.c" /* yacc.c:1646  */
    break;

  case 116:
#line 532 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 2601 "y.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 537 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0);
        free((yyvsp[-1].sval));
      }
#line 2610 "y.tab.c" /* yacc.c:1646  */
    break;

  case 118:
#line 542 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2619 "y.tab.c" /* yacc.c:1646  */
    break;

  case 119:
#line 547 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 2628 "y.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 552 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0);
        free((yyvsp[-1].sval));
      }
#line 2637 "y.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 557 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2646 "y.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 562 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2655 "y.tab.c" /* yacc.c:1646  */
    break;

  case 123:
#line 570 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-1].sval));
      }
#line 2664 "y.tab.c" /* yacc.c:1646  */
    break;

  case 124:
#line 578 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-1].sval));
      }
#line 2673 "y.tab.c" /* yacc.c:1646  */
    break;

  case 125:
#line 583 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 2682 "y.tab.c" /* yacc.c:1646  */
    break;

  case 126:
#line 591 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2691 "y.tab.c" /* yacc.c:1646  */
    break;

  case 127:
#line 596 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2700 "y.tab.c" /* yacc.c:1646  */
    break;

  case 128:
#line 601 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_idt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2709 "y.tab.c" /* yacc.c:1646  */
    break;

  case 129:
#line 606 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_ivt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2718 "y.tab.c" /* yacc.c:1646  */
    break;

  case 130:
#line 611 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_gdt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2727 "y.tab.c" /* yacc.c:1646  */
    break;

  case 131:
#line 616 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_ldt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2736 "y.tab.c" /* yacc.c:1646  */
    break;

  case 132:
#line 621 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_dump_table();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2745 "y.tab.c" /* yacc.c:1646  */
    break;

  case 133:
#line 626 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_tss_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2754 "y.tab.c" /* yacc.c:1646  */
    break;

  case 134:
#line 631 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_flags();
        free((yyvsp[-2].sval));
      }
#line 2763 "y.tab.c" /* yacc.c:1646  */
    break;

  case 135:
#line 636 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_linux_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2772 "y.tab.c" /* yacc.c:1646  */
    break;

  case 136:
#line 641 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2781 "y.tab.c" /* yacc.c:1646  */
    break;

  case 137:
#line 646 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2790 "y.tab.c" /* yacc.c:1646  */
    break;

  case 138:
#line 651 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device("", "");
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2799 "y.tab.c" /* yacc.c:1646  */
    break;

  case 139:
#line 656 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2808 "y.tab.c" /* yacc.c:1646  */
    break;

  case 140:
#line 661 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2817 "y.tab.c" /* yacc.c:1646  */
    break;

  case 141:
#line 666 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2826 "y.tab.c" /* yacc.c:1646  */
    break;

  case 142:
#line 671 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2835 "y.tab.c" /* yacc.c:1646  */
    break;

  case 143:
#line 678 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = EMPTY_ARG; }
#line 2841 "y.tab.c" /* yacc.c:1646  */
    break;

  case 145:
#line 683 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 2850 "y.tab.c" /* yacc.c:1646  */
    break;

  case 146:
#line 691 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[-1].sval));
      }
#line 2859 "y.tab.c" /* yacc.c:1646  */
    break;

  case 147:
#line 699 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 2868 "y.tab.c" /* yacc.c:1646  */
    break;

  case 148:
#line 707 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 2877 "y.tab.c" /* yacc.c:1646  */
    break;

  case 149:
#line 715 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 2886 "y.tab.c" /* yacc.c:1646  */
    break;

  case 150:
#line 723 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 2895 "y.tab.c" /* yacc.c:1646  */
    break;

  case 151:
#line 731 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[-1].sval));
      }
#line 2904 "y.tab.c" /* yacc.c:1646  */
    break;

  case 152:
#line 739 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_control_regs_command();
        free((yyvsp[-1].sval));
      }
#line 2913 "y.tab.c" /* yacc.c:1646  */
    break;

  case 153:
#line 747 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[-1].sval));
      }
#line 2922 "y.tab.c" /* yacc.c:1646  */
    break;

  case 154:
#line 755 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_del_breakpoint_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2931 "y.tab.c" /* yacc.c:1646  */
    break;

  case 155:
#line 763 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 2940 "y.tab.c" /* yacc.c:1646  */
    break;

  case 156:
#line 770 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 0);
        free((yyvsp[-2].sval));
      }
#line 2949 "y.tab.c" /* yacc.c:1646  */
    break;

  case 157:
#line 778 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_quit_command();
        free((yyvsp[-1].sval));
      }
#line 2958 "y.tab.c" /* yacc.c:1646  */
    break;

  case 158:
#line 786 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_examine_command((yyvsp[-3].sval), (yyvsp[-2].sval),1, (yyvsp[-1].uval), 1);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2967 "y.tab.c" /* yacc.c:1646  */
    break;

  case 159:
#line 791 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_examine_command((yyvsp[-2].sval), (yyvsp[-1].sval),1, 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2976 "y.tab.c" /* yacc.c:1646  */
    break;

  case 160:
#line 796 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_examine_command((yyvsp[-2].sval), NULL,0, (yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 2985 "y.tab.c" /* yacc.c:1646  */
    break;

  case 161:
#line 801 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_examine_command((yyvsp[-1].sval), NULL,0, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 2994 "y.tab.c" /* yacc.c:1646  */
    break;

  case 162:
#line 809 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_restore_command((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3003 "y.tab.c" /* yacc.c:1646  */
    break;

  case 163:
#line 817 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_writemem_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3012 "y.tab.c" /* yacc.c:1646  */
    break;

  case 164:
#line 825 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_setpmem_command((yyvsp[-3].uval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3021 "y.tab.c" /* yacc.c:1646  */
    break;

  case 165:
#line 833 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_query_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3030 "y.tab.c" /* yacc.c:1646  */
    break;

  case 166:
#line 841 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3039 "y.tab.c" /* yacc.c:1646  */
    break;

  case 167:
#line 846 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_take_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3048 "y.tab.c" /* yacc.c:1646  */
    break;

  case 168:
#line 851 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3057 "y.tab.c" /* yacc.c:1646  */
    break;

  case 169:
#line 859 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[-1].sval));
      }
#line 3066 "y.tab.c" /* yacc.c:1646  */
    break;

  case 170:
#line 864 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3075 "y.tab.c" /* yacc.c:1646  */
    break;

  case 171:
#line 869 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3084 "y.tab.c" /* yacc.c:1646  */
    break;

  case 172:
#line 874 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_current((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3093 "y.tab.c" /* yacc.c:1646  */
    break;

  case 173:
#line 879 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_command((yyvsp[-2].sval), (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3102 "y.tab.c" /* yacc.c:1646  */
    break;

  case 174:
#line 884 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3111 "y.tab.c" /* yacc.c:1646  */
    break;

  case 175:
#line 889 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3120 "y.tab.c" /* yacc.c:1646  */
    break;

  case 176:
#line 894 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_hex_mode_switch((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3129 "y.tab.c" /* yacc.c:1646  */
    break;

  case 177:
#line 899 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_set_disassemble_size((yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3138 "y.tab.c" /* yacc.c:1646  */
    break;

  case 178:
#line 907 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3147 "y.tab.c" /* yacc.c:1646  */
    break;

  case 179:
#line 912 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3156 "y.tab.c" /* yacc.c:1646  */
    break;

  case 180:
#line 920 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_doit_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3165 "y.tab.c" /* yacc.c:1646  */
    break;

  case 181:
#line 928 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_crc_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3174 "y.tab.c" /* yacc.c:1646  */
    break;

  case 182:
#line 936 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3183 "y.tab.c" /* yacc.c:1646  */
    break;

  case 183:
#line 941 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("c|cont|continue - continue executing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3192 "y.tab.c" /* yacc.c:1646  */
    break;

  case 184:
#line 946 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3203 "y.tab.c" /* yacc.c:1646  */
    break;

  case 185:
#line 953 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3212 "y.tab.c" /* yacc.c:1646  */
    break;

  case 186:
#line 958 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3221 "y.tab.c" /* yacc.c:1646  */
    break;

  case 187:
#line 963 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3230 "y.tab.c" /* yacc.c:1646  */
    break;

  case 188:
#line 968 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3239 "y.tab.c" /* yacc.c:1646  */
    break;

  case 189:
#line 973 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3248 "y.tab.c" /* yacc.c:1646  */
    break;

  case 190:
#line 978 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3257 "y.tab.c" /* yacc.c:1646  */
    break;

  case 191:
#line 983 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3266 "y.tab.c" /* yacc.c:1646  */
    break;

  case 192:
#line 988 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3275 "y.tab.c" /* yacc.c:1646  */
    break;

  case 193:
#line 993 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3284 "y.tab.c" /* yacc.c:1646  */
    break;

  case 194:
#line 998 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3293 "y.tab.c" /* yacc.c:1646  */
    break;

  case 195:
#line 1003 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3302 "y.tab.c" /* yacc.c:1646  */
    break;

  case 196:
#line 1008 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3312 "y.tab.c" /* yacc.c:1646  */
    break;

  case 197:
#line 1014 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3322 "y.tab.c" /* yacc.c:1646  */
    break;

  case 198:
#line 1020 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("trace-mem on  - print all memory accesses occured during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3332 "y.tab.c" /* yacc.c:1646  */
    break;

  case 199:
#line 1026 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3344 "y.tab.c" /* yacc.c:1646  */
    break;

  case 200:
#line 1034 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3353 "y.tab.c" /* yacc.c:1646  */
    break;

  case 201:
#line 1039 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3362 "y.tab.c" /* yacc.c:1646  */
    break;

  case 202:
#line 1044 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3371 "y.tab.c" /* yacc.c:1646  */
    break;

  case 203:
#line 1049 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3380 "y.tab.c" /* yacc.c:1646  */
    break;

  case 204:
#line 1054 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3389 "y.tab.c" /* yacc.c:1646  */
    break;

  case 205:
#line 1059 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3398 "y.tab.c" /* yacc.c:1646  */
    break;

  case 206:
#line 1064 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3407 "y.tab.c" /* yacc.c:1646  */
    break;

  case 207:
#line 1069 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3416 "y.tab.c" /* yacc.c:1646  */
    break;

  case 208:
#line 1074 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3425 "y.tab.c" /* yacc.c:1646  */
    break;

  case 209:
#line 1079 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3434 "y.tab.c" /* yacc.c:1646  */
    break;

  case 210:
#line 1084 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3443 "y.tab.c" /* yacc.c:1646  */
    break;

  case 211:
#line 1089 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("zmm - print AVX-512 state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3452 "y.tab.c" /* yacc.c:1646  */
    break;

  case 212:
#line 1094 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3461 "y.tab.c" /* yacc.c:1646  */
    break;

  case 213:
#line 1099 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3470 "y.tab.c" /* yacc.c:1646  */
    break;

  case 214:
#line 1104 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3479 "y.tab.c" /* yacc.c:1646  */
    break;

  case 215:
#line 1109 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3488 "y.tab.c" /* yacc.c:1646  */
    break;

  case 216:
#line 1114 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3497 "y.tab.c" /* yacc.c:1646  */
    break;

  case 217:
#line 1119 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm hex on/off - control disasm offsets and displacements format\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3511 "y.tab.c" /* yacc.c:1646  */
    break;

  case 218:
#line 1129 "parser.y" /* yacc.c:1646  */
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
#line 3526 "y.tab.c" /* yacc.c:1646  */
    break;

  case 219:
#line 1140 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3536 "y.tab.c" /* yacc.c:1646  */
    break;

  case 220:
#line 1146 "parser.y" /* yacc.c:1646  */
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
#line 3553 "y.tab.c" /* yacc.c:1646  */
    break;

  case 221:
#line 1159 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("instrument <command> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command>\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3562 "y.tab.c" /* yacc.c:1646  */
    break;

  case 222:
#line 1164 "parser.y" /* yacc.c:1646  */
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
#line 3577 "y.tab.c" /* yacc.c:1646  */
    break;

  case 223:
#line 1175 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3586 "y.tab.c" /* yacc.c:1646  */
    break;

  case 224:
#line 1180 "parser.y" /* yacc.c:1646  */
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
#line 3606 "y.tab.c" /* yacc.c:1646  */
    break;

  case 225:
#line 1196 "parser.y" /* yacc.c:1646  */
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
#line 3626 "y.tab.c" /* yacc.c:1646  */
    break;

  case 226:
#line 1212 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("calc|? <expr> - calculate a expression and display the result.\n");
         dbg_printf("    'expr' can reference any general-purpose, opmask and segment\n");
         dbg_printf("    registers, use any arithmetic and logic operations, and\n");
         dbg_printf("    also the special ':' operator which computes the linear\n");
         dbg_printf("    address of a segment:offset (in real and v86 mode) or of\n");
         dbg_printf("    a selector:offset (in protected mode) pair.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3640 "y.tab.c" /* yacc.c:1646  */
    break;

  case 227:
#line 1222 "parser.y" /* yacc.c:1646  */
    {
         bx_dbg_print_help();
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3649 "y.tab.c" /* yacc.c:1646  */
    break;

  case 228:
#line 1227 "parser.y" /* yacc.c:1646  */
    {
         bx_dbg_print_help();
         free((yyvsp[-1].sval));
       }
#line 3658 "y.tab.c" /* yacc.c:1646  */
    break;

  case 229:
#line 1235 "parser.y" /* yacc.c:1646  */
    {
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 3667 "y.tab.c" /* yacc.c:1646  */
    break;

  case 230:
#line 1253 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[0].uval); }
#line 3673 "y.tab.c" /* yacc.c:1646  */
    break;

  case 231:
#line 1254 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 3679 "y.tab.c" /* yacc.c:1646  */
    break;

  case 232:
#line 1255 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 3685 "y.tab.c" /* yacc.c:1646  */
    break;

  case 233:
#line 1256 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 3691 "y.tab.c" /* yacc.c:1646  */
    break;

  case 234:
#line 1257 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 3697 "y.tab.c" /* yacc.c:1646  */
    break;

  case 235:
#line 1258 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 3703 "y.tab.c" /* yacc.c:1646  */
    break;

  case 236:
#line 1259 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 3709 "y.tab.c" /* yacc.c:1646  */
    break;

  case 237:
#line 1260 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 3715 "y.tab.c" /* yacc.c:1646  */
    break;

  case 238:
#line 1261 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 3721 "y.tab.c" /* yacc.c:1646  */
    break;

  case 239:
#line 1262 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_ip (); }
#line 3727 "y.tab.c" /* yacc.c:1646  */
    break;

  case 240:
#line 1263 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_eip(); }
#line 3733 "y.tab.c" /* yacc.c:1646  */
    break;

  case 241:
#line 1264 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
#line 3739 "y.tab.c" /* yacc.c:1646  */
    break;

  case 242:
#line 1265 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 3745 "y.tab.c" /* yacc.c:1646  */
    break;

  case 243:
#line 1266 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 3751 "y.tab.c" /* yacc.c:1646  */
    break;

  case 244:
#line 1267 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 3757 "y.tab.c" /* yacc.c:1646  */
    break;

  case 245:
#line 1268 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) / (yyvsp[0].uval); }
#line 3763 "y.tab.c" /* yacc.c:1646  */
    break;

  case 246:
#line 1269 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 3769 "y.tab.c" /* yacc.c:1646  */
    break;

  case 247:
#line 1270 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 3775 "y.tab.c" /* yacc.c:1646  */
    break;

  case 248:
#line 1271 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 3781 "y.tab.c" /* yacc.c:1646  */
    break;

  case 249:
#line 1272 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 3787 "y.tab.c" /* yacc.c:1646  */
    break;

  case 250:
#line 1273 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 3793 "y.tab.c" /* yacc.c:1646  */
    break;

  case 251:
#line 1274 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = !(yyvsp[0].uval); }
#line 3799 "y.tab.c" /* yacc.c:1646  */
    break;

  case 252:
#line 1275 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = -(yyvsp[0].uval); }
#line 3805 "y.tab.c" /* yacc.c:1646  */
    break;

  case 253:
#line 1276 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-1].uval); }
#line 3811 "y.tab.c" /* yacc.c:1646  */
    break;

  case 254:
#line 1282 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[0].uval); }
#line 3817 "y.tab.c" /* yacc.c:1646  */
    break;

  case 255:
#line 1283 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 3823 "y.tab.c" /* yacc.c:1646  */
    break;

  case 256:
#line 1284 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 3829 "y.tab.c" /* yacc.c:1646  */
    break;

  case 257:
#line 1285 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 3835 "y.tab.c" /* yacc.c:1646  */
    break;

  case 258:
#line 1286 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 3841 "y.tab.c" /* yacc.c:1646  */
    break;

  case 259:
#line 1287 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 3847 "y.tab.c" /* yacc.c:1646  */
    break;

  case 260:
#line 1288 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 3853 "y.tab.c" /* yacc.c:1646  */
    break;

  case 261:
#line 1289 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 3859 "y.tab.c" /* yacc.c:1646  */
    break;

  case 262:
#line 1290 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 3865 "y.tab.c" /* yacc.c:1646  */
    break;

  case 263:
#line 1291 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_ip (); }
#line 3871 "y.tab.c" /* yacc.c:1646  */
    break;

  case 264:
#line 1292 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_eip(); }
#line 3877 "y.tab.c" /* yacc.c:1646  */
    break;

  case 265:
#line 1293 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_instruction_pointer(); }
#line 3883 "y.tab.c" /* yacc.c:1646  */
    break;

  case 266:
#line 1294 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[-2].uval), (yyvsp[0].uval)); }
#line 3889 "y.tab.c" /* yacc.c:1646  */
    break;

  case 267:
#line 1295 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 3895 "y.tab.c" /* yacc.c:1646  */
    break;

  case 268:
#line 1296 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 3901 "y.tab.c" /* yacc.c:1646  */
    break;

  case 269:
#line 1297 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 3907 "y.tab.c" /* yacc.c:1646  */
    break;

  case 270:
#line 1298 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = ((yyvsp[0].uval) != 0) ? (yyvsp[-2].uval) / (yyvsp[0].uval) : 0; }
#line 3913 "y.tab.c" /* yacc.c:1646  */
    break;

  case 271:
#line 1299 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 3919 "y.tab.c" /* yacc.c:1646  */
    break;

  case 272:
#line 1300 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 3925 "y.tab.c" /* yacc.c:1646  */
    break;

  case 273:
#line 1301 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 3931 "y.tab.c" /* yacc.c:1646  */
    break;

  case 274:
#line 1302 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 3937 "y.tab.c" /* yacc.c:1646  */
    break;

  case 275:
#line 1303 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 3943 "y.tab.c" /* yacc.c:1646  */
    break;

  case 276:
#line 1304 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = !(yyvsp[0].uval); }
#line 3949 "y.tab.c" /* yacc.c:1646  */
    break;

  case 277:
#line 1305 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = -(yyvsp[0].uval); }
#line 3955 "y.tab.c" /* yacc.c:1646  */
    break;

  case 278:
#line 1306 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[0].uval)); }
#line 3961 "y.tab.c" /* yacc.c:1646  */
    break;

  case 279:
#line 1307 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[0].uval)); }
#line 3967 "y.tab.c" /* yacc.c:1646  */
    break;

  case 280:
#line 1308 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-1].uval); }
#line 3973 "y.tab.c" /* yacc.c:1646  */
    break;


#line 3977 "y.tab.c" /* yacc.c:1646  */
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
#line 1311 "parser.y" /* yacc.c:1906  */

#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
