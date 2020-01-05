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
Bit64u eval_value;

#line 83 "y.tab.c" /* yacc.c:339  */

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
    BX_TOKEN_SMI = 320,
    BX_TOKEN_NMI = 321,
    BX_TOKEN_TLB = 322,
    BX_TOKEN_HEX = 323,
    BX_TOKEN_DISASM = 324,
    BX_TOKEN_INSTRUMENT = 325,
    BX_TOKEN_STRING = 326,
    BX_TOKEN_STOP = 327,
    BX_TOKEN_DOIT = 328,
    BX_TOKEN_CRC = 329,
    BX_TOKEN_TRACE = 330,
    BX_TOKEN_TRACEREG = 331,
    BX_TOKEN_TRACEMEM = 332,
    BX_TOKEN_SWITCH_MODE = 333,
    BX_TOKEN_SIZE = 334,
    BX_TOKEN_PTIME = 335,
    BX_TOKEN_TIMEBP_ABSOLUTE = 336,
    BX_TOKEN_TIMEBP = 337,
    BX_TOKEN_MODEBP = 338,
    BX_TOKEN_VMEXITBP = 339,
    BX_TOKEN_PRINT_STACK = 340,
    BX_TOKEN_BT = 341,
    BX_TOKEN_WATCH = 342,
    BX_TOKEN_UNWATCH = 343,
    BX_TOKEN_READ = 344,
    BX_TOKEN_WRITE = 345,
    BX_TOKEN_SHOW = 346,
    BX_TOKEN_LOAD_SYMBOLS = 347,
    BX_TOKEN_SYMBOLS = 348,
    BX_TOKEN_LIST_SYMBOLS = 349,
    BX_TOKEN_GLOBAL = 350,
    BX_TOKEN_WHERE = 351,
    BX_TOKEN_PRINT_STRING = 352,
    BX_TOKEN_NUMERIC = 353,
    BX_TOKEN_PAGE = 354,
    BX_TOKEN_HELP = 355,
    BX_TOKEN_XML = 356,
    BX_TOKEN_CALC = 357,
    BX_TOKEN_DEVICE = 358,
    BX_TOKEN_GENERIC = 359,
    BX_TOKEN_RSHIFT = 360,
    BX_TOKEN_LSHIFT = 361,
    BX_TOKEN_EQ = 362,
    BX_TOKEN_NE = 363,
    BX_TOKEN_LE = 364,
    BX_TOKEN_GE = 365,
    BX_TOKEN_REG_IP = 366,
    BX_TOKEN_REG_EIP = 367,
    BX_TOKEN_REG_RIP = 368,
    BX_TOKEN_REG_SSP = 369,
    NOT = 370,
    NEG = 371,
    INDIRECT = 372
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
#define BX_TOKEN_SMI 320
#define BX_TOKEN_NMI 321
#define BX_TOKEN_TLB 322
#define BX_TOKEN_HEX 323
#define BX_TOKEN_DISASM 324
#define BX_TOKEN_INSTRUMENT 325
#define BX_TOKEN_STRING 326
#define BX_TOKEN_STOP 327
#define BX_TOKEN_DOIT 328
#define BX_TOKEN_CRC 329
#define BX_TOKEN_TRACE 330
#define BX_TOKEN_TRACEREG 331
#define BX_TOKEN_TRACEMEM 332
#define BX_TOKEN_SWITCH_MODE 333
#define BX_TOKEN_SIZE 334
#define BX_TOKEN_PTIME 335
#define BX_TOKEN_TIMEBP_ABSOLUTE 336
#define BX_TOKEN_TIMEBP 337
#define BX_TOKEN_MODEBP 338
#define BX_TOKEN_VMEXITBP 339
#define BX_TOKEN_PRINT_STACK 340
#define BX_TOKEN_BT 341
#define BX_TOKEN_WATCH 342
#define BX_TOKEN_UNWATCH 343
#define BX_TOKEN_READ 344
#define BX_TOKEN_WRITE 345
#define BX_TOKEN_SHOW 346
#define BX_TOKEN_LOAD_SYMBOLS 347
#define BX_TOKEN_SYMBOLS 348
#define BX_TOKEN_LIST_SYMBOLS 349
#define BX_TOKEN_GLOBAL 350
#define BX_TOKEN_WHERE 351
#define BX_TOKEN_PRINT_STRING 352
#define BX_TOKEN_NUMERIC 353
#define BX_TOKEN_PAGE 354
#define BX_TOKEN_HELP 355
#define BX_TOKEN_XML 356
#define BX_TOKEN_CALC 357
#define BX_TOKEN_DEVICE 358
#define BX_TOKEN_GENERIC 359
#define BX_TOKEN_RSHIFT 360
#define BX_TOKEN_LSHIFT 361
#define BX_TOKEN_EQ 362
#define BX_TOKEN_NE 363
#define BX_TOKEN_LE 364
#define BX_TOKEN_GE 365
#define BX_TOKEN_REG_IP 366
#define BX_TOKEN_REG_EIP 367
#define BX_TOKEN_REG_RIP 368
#define BX_TOKEN_REG_SSP 369
#define NOT 370
#define NEG 371
#define INDIRECT 372

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 14 "parser.y" /* yacc.c:355  */

  char    *sval;
  Bit64u   uval;
  bx_bool  bval;

#line 363 "y.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE bxlval;

int bxparse (void);

#endif /* !YY_BX_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 380 "y.tab.c" /* yacc.c:358  */

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
#define YYFINAL  310
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2465

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  134
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  59
/* YYNRULES -- Number of rules.  */
#define YYNRULES  302
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  590

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   372

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     127,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   130,     2,     2,     2,     2,   123,     2,
     131,   132,   121,   115,     2,   116,     2,   122,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   129,     2,
     119,   128,   120,     2,   133,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   118,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   117,     2,     2,     2,     2,     2,
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
     124,   125,   126
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   150,   150,   151,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   213,   214,   219,
     220,   225,   226,   227,   228,   229,   230,   235,   240,   248,
     256,   264,   269,   274,   279,   284,   289,   297,   305,   313,
     321,   329,   337,   345,   350,   358,   363,   371,   376,   381,
     386,   391,   396,   401,   406,   411,   416,   421,   429,   434,
     439,   444,   452,   460,   468,   473,   481,   486,   491,   496,
     504,   512,   517,   522,   526,   530,   534,   538,   542,   546,
     550,   557,   562,   567,   572,   577,   582,   587,   592,   597,
     602,   607,   612,   617,   625,   633,   638,   646,   651,   656,
     661,   666,   671,   676,   681,   686,   691,   696,   701,   706,
     711,   716,   721,   726,   734,   735,   738,   746,   754,   762,
     770,   778,   786,   794,   802,   810,   818,   825,   833,   841,
     846,   851,   856,   864,   872,   880,   888,   896,   901,   906,
     911,   916,   924,   929,   934,   939,   944,   949,   954,   959,
     964,   972,   977,   985,   993,  1001,  1006,  1012,  1019,  1024,
    1030,  1036,  1042,  1047,  1052,  1057,  1062,  1067,  1072,  1077,
    1083,  1089,  1095,  1103,  1108,  1113,  1118,  1123,  1128,  1133,
    1138,  1143,  1148,  1153,  1158,  1163,  1168,  1173,  1178,  1183,
    1188,  1198,  1209,  1215,  1228,  1233,  1244,  1249,  1265,  1281,
    1291,  1296,  1304,  1313,  1323,  1324,  1325,  1326,  1327,  1328,
    1329,  1330,  1331,  1332,  1333,  1334,  1335,  1336,  1337,  1338,
    1339,  1340,  1341,  1342,  1343,  1344,  1345,  1346,  1347,  1353,
    1354,  1355,  1356,  1357,  1358,  1359,  1360,  1361,  1362,  1363,
    1364,  1365,  1366,  1367,  1368,  1369,  1370,  1371,  1372,  1373,
    1374,  1375,  1376,  1377,  1378,  1379,  1380,  1381,  1382,  1383,
    1384,  1385,  1386
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
  "BX_TOKEN_SMI", "BX_TOKEN_NMI", "BX_TOKEN_TLB", "BX_TOKEN_HEX",
  "BX_TOKEN_DISASM", "BX_TOKEN_INSTRUMENT", "BX_TOKEN_STRING",
  "BX_TOKEN_STOP", "BX_TOKEN_DOIT", "BX_TOKEN_CRC", "BX_TOKEN_TRACE",
  "BX_TOKEN_TRACEREG", "BX_TOKEN_TRACEMEM", "BX_TOKEN_SWITCH_MODE",
  "BX_TOKEN_SIZE", "BX_TOKEN_PTIME", "BX_TOKEN_TIMEBP_ABSOLUTE",
  "BX_TOKEN_TIMEBP", "BX_TOKEN_MODEBP", "BX_TOKEN_VMEXITBP",
  "BX_TOKEN_PRINT_STACK", "BX_TOKEN_BT", "BX_TOKEN_WATCH",
  "BX_TOKEN_UNWATCH", "BX_TOKEN_READ", "BX_TOKEN_WRITE", "BX_TOKEN_SHOW",
  "BX_TOKEN_LOAD_SYMBOLS", "BX_TOKEN_SYMBOLS", "BX_TOKEN_LIST_SYMBOLS",
  "BX_TOKEN_GLOBAL", "BX_TOKEN_WHERE", "BX_TOKEN_PRINT_STRING",
  "BX_TOKEN_NUMERIC", "BX_TOKEN_PAGE", "BX_TOKEN_HELP", "BX_TOKEN_XML",
  "BX_TOKEN_CALC", "BX_TOKEN_DEVICE", "BX_TOKEN_GENERIC",
  "BX_TOKEN_RSHIFT", "BX_TOKEN_LSHIFT", "BX_TOKEN_EQ", "BX_TOKEN_NE",
  "BX_TOKEN_LE", "BX_TOKEN_GE", "BX_TOKEN_REG_IP", "BX_TOKEN_REG_EIP",
  "BX_TOKEN_REG_RIP", "BX_TOKEN_REG_SSP", "'+'", "'-'", "'|'", "'^'",
  "'<'", "'>'", "'*'", "'/'", "'&'", "NOT", "NEG", "INDIRECT", "'\\n'",
  "'='", "':'", "'!'", "'('", "')'", "'@'", "$accept", "commands",
  "command", "BX_TOKEN_TOGGLE_ON_OFF", "BX_TOKEN_REGISTERS",
  "BX_TOKEN_SEGREG", "timebp_command", "modebp_command",
  "vmexitbp_command", "show_command", "page_command", "tlb_command",
  "ptime_command", "trace_command", "trace_reg_command",
  "trace_mem_command", "print_stack_command", "backtrace_command",
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
  "crc_command", "help_command", "calc_command", "if_command",
  "vexpression", "expression", YY_NULLPTR
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
     365,   366,   367,   368,   369,    43,    45,   124,    94,    60,
      62,    42,    47,    38,   370,   371,   372,    10,    61,    58,
      33,    40,    41,    64
};
# endif

#define YYPACT_NINF -178

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-178)))

#define YYTABLE_NINF -301

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     558,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,    -3,  1399,   -46,  -104,   135,  -102,  1452,
     295,  1059,   -67,   -60,   -57,   720,   -88,  -178,  -178,   -81,
     -70,   -69,   -68,   -63,   -61,   -56,   -54,  1035,    -1,    11,
    1399,    30,    24,  1399,   982,   -53,  -178,  1399,  1399,    19,
      19,    19,   -35,  1399,  1399,   -34,   -33,   -85,   -72,    43,
    1113,     1,   -39,   -44,   -32,  1399,  -178,  1399,  1587,  1399,
    -178,  -178,  -178,  -178,  1399,  1399,  -178,  1399,  1399,  1399,
     427,  -178,   -25,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  1385,  1399,  -178,  1586,     6,   -65,
    -178,  -178,   -12,   -11,    -6,    -5,     8,     9,    19,    33,
      34,    36,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  1473,  -178,  1473,  1473,  -178,
    1419,     3,  -178,    -9,  1399,  -178,   252,     4,     7,    40,
      41,    44,    45,  1399,  1399,  1399,  1399,    46,    48,    53,
     -43,   -59,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  1190,  -178,  1611,    54,  1399,   903,    55,   -64,    72,
      73,    75,  1636,  1244,    19,    76,    83,  -178,   146,    85,
      87,  1661,   903,  -178,  -178,    92,    93,    94,  -178,  1686,
    1711,  -178,  -178,    96,  -178,    97,  -178,   103,  1399,   107,
    1399,  1399,  -178,  -178,  1736,   108,   109,   -77,   112,  -178,
    1268,    56,   115,  -178,  -178,  1761,  1786,   116,   118,   119,
     122,   143,   145,   147,   151,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   169,
     170,   202,   203,   204,   205,   206,   213,   217,   228,   229,
     236,   237,   238,   249,   250,   251,   253,   255,   256,   257,
     264,   265,  -178,   267,  1811,    52,    52,    52,   817,    52,
    -178,  -178,  -178,  1399,  1399,  1399,  1399,  1399,  1399,  1399,
    1399,  1399,  1399,  1399,  1399,  1399,  1399,  1399,  1399,  1836,
    -178,   273,   277,  -178,  1399,  1399,  1399,  1399,  1399,  1399,
     278,  1399,  1399,  1399,  -178,  -178,   280,  1473,  1473,  1473,
    1473,  1473,  1473,  1473,  1473,  1473,  1473,   339,  -178,   342,
    -178,    -8,   343,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    1399,  1385,  1399,  1399,  1399,  -178,  -178,  -178,   288,  -178,
     -42,   -41,  -178,  -178,  1861,  -178,   291,   903,  1399,  1399,
     903,  -178,   294,  -178,  -178,  -178,  -178,  -178,  -178,   689,
     296,  -178,    28,  -178,  1886,  -178,  -178,  -178,  1911,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,   719,  -178,   772,
     850,  -178,  -178,  -178,   297,  -178,  -178,  -178,  1936,  1321,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,    69,    69,    52,    52,    52,    52,   978,   978,   978,
     978,   978,   978,    69,    69,    69,  1385,  -178,  -178,  -178,
    1961,  1986,  2011,  2036,  2061,  2086,  -178,  2111,  2136,  2161,
    -178,  -178,  -178,   189,   189,   189,   189,  -178,  -178,  -178,
     110,   315,   316,   373,  -178,   323,   335,   341,   344,   345,
    -178,   346,  -178,   347,  -178,  -178,  -178,  2186,  1031,   100,
    2211,  -178,  -178,  2236,  -178,   348,  -178,  -178,  -178,  2261,
    -178,  2286,  -178,  2311,  -178,  -178,  -178,  2336,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,   398,  -178,  -178,
    -178,   349,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,   354,  -178,  -178
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      55,   272,   271,   273,   274,   275,    61,    62,    63,    64,
      65,    66,   276,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    59,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   270,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   269,     0,     0,     0,
     278,   279,   280,   281,     0,     0,    56,     0,     0,     0,
       0,     3,     0,   277,    39,    40,    41,    47,    45,    46,
      38,    35,    36,    37,    42,    43,    44,    48,    49,    50,
       4,     5,     6,     7,     8,    19,    20,     9,    10,    11,
      12,    13,    14,    15,    16,    18,    17,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    51,    52,    53,    54,     0,   104,     0,     0,     0,
     106,   110,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   134,   247,   246,   248,   249,   250,   251,   245,
     244,   253,   254,   255,   256,     0,   121,     0,     0,   252,
       0,   270,   124,     0,     0,   129,     0,     0,     0,     0,
       0,     0,     0,   154,   154,   154,   154,     0,     0,     0,
       0,     0,   168,   157,   158,   159,   160,   161,   164,   163,
     162,     0,   172,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   182,     0,     0,
       0,     0,     0,    57,    58,     0,     0,     0,    79,     0,
       0,    69,    70,     0,    83,     0,    85,     0,     0,     0,
       0,     0,    89,    96,     0,     0,     0,     0,     0,    76,
       0,     0,     0,   135,   102,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   241,     0,     0,   299,   300,   298,     0,   301,
       1,     2,   156,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     243,     0,     0,   107,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   267,   266,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   127,     0,
     125,   300,     0,   130,   165,   166,   167,   145,   137,   138,
     154,   155,   154,   154,   154,   144,   143,   146,     0,   147,
       0,     0,   149,   170,     0,   171,     0,     0,     0,     0,
       0,   176,     0,   177,   179,   180,   181,    78,   185,     0,
       0,   188,     0,   183,     0,   191,   192,   193,     0,    80,
      81,    82,    68,    67,    84,    86,    88,     0,    87,     0,
       0,    97,    73,    72,     0,    74,    71,    98,     0,     0,
     136,   103,    77,   196,   197,   198,   235,   205,   199,   200,
     201,   202,   203,   204,   237,   195,   220,   221,   222,   223,
     224,   227,   226,   225,   233,   212,   228,   229,   230,   234,
     208,   209,   210,   211,   213,   215,   214,   206,   207,   216,
     231,   232,   238,   217,   218,   236,   240,   239,   219,   242,
     302,   287,   288,   294,   295,   296,   297,   283,   284,   289,
     290,   293,   292,   285,   286,   291,   282,   105,   108,   109,
       0,     0,     0,     0,     0,     0,   111,     0,     0,     0,
     268,   261,   262,   257,   258,   263,   264,   259,   260,   265,
       0,     0,     0,     0,   132,     0,     0,     0,     0,     0,
     148,     0,   152,     0,   150,   169,   173,     0,   284,   285,
       0,   178,   186,     0,   189,     0,   184,   194,    90,     0,
      91,     0,    92,     0,    75,    99,   100,     0,   114,   113,
     115,   116,   117,   112,   118,   119,   120,     0,   122,   128,
     126,     0,   131,   139,   140,   141,   142,   153,   151,   174,
     175,   187,   190,    93,    94,    95,   101,     0,   133,   123
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -178,  -178,   390,   -45,   414,    -2,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -177,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -164,     0
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    80,    81,   225,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     370,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   170,   371
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     134,   344,   138,   345,   346,   226,   227,   372,   373,   374,
     359,   523,   380,   233,   137,   151,   135,   169,   245,   219,
     173,   176,   357,   141,   424,   152,   235,   252,   378,   531,
     533,   177,   250,   332,   392,   223,   224,   203,   178,   192,
     206,   179,   234,   212,   218,   381,   193,   221,   222,   246,
     425,   220,   139,   229,   230,   236,   251,   194,   195,   196,
     244,   237,   333,   393,   197,   255,   198,   256,   382,   304,
     204,   199,   247,   200,   305,   306,   238,   307,   308,   309,
     134,   140,   205,   253,   379,   532,   534,   208,   209,   210,
     211,   207,   228,   231,   232,   254,   313,   314,   315,   316,
     317,   318,   312,   340,   331,   248,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   239,   334,   335,   360,   524,
     328,   328,   336,   337,   136,   386,   545,   429,   249,   567,
     358,   364,   240,   241,   365,   329,   338,   339,   142,   143,
     144,   145,   146,     6,     7,     8,     9,    10,    11,     1,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,   341,   342,   169,   343,   169,   169,   366,   367,   400,
     242,   368,   369,   375,   361,   376,   315,   316,   317,   318,
     377,   328,   391,   511,   512,   513,   514,   515,   516,   517,
     518,   519,   520,   526,   147,   527,   528,   529,   328,   394,
     395,   384,   396,   401,   148,   387,   390,  -300,  -300,  -300,
    -300,   402,   405,   399,   406,   347,   348,    46,   404,   409,
     410,   411,   408,   414,   415,   349,   350,   351,   352,   328,
     416,   353,   354,   355,   418,   422,   423,   568,   417,   426,
     419,   420,   430,   433,    66,   434,   435,   149,   150,   436,
     428,   313,   314,   315,   316,   317,   318,    70,    71,    72,
      73,   319,   388,   321,   322,   323,   324,   389,   326,   327,
     437,   362,   438,   403,   439,   328,    77,    78,   440,    79,
     441,   442,   443,   444,   445,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   347,   348,   455,   456,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
     353,   354,   355,   481,   482,   483,   484,   485,   486,   487,
     488,   489,   490,   491,   492,   493,   494,   495,   496,   457,
     458,   459,   460,   461,   500,   501,   502,   503,   504,   505,
     462,   507,   508,   509,   463,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   464,   465,   313,   314,   315,
     316,   317,   318,   466,   467,   468,   171,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   469,   470,   471,   363,
     472,   328,   473,   474,   475,   347,   348,   537,   538,   539,
     540,   476,   477,    66,   478,   349,   350,   351,   352,   543,
     498,   353,   354,   355,   499,   506,    70,    71,    72,    73,
     521,    74,   510,   522,   525,   530,    75,   549,   536,   551,
     553,   541,   172,   544,   554,    77,    78,   310,    79,   557,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,   569,   570,   571,    13,    14,    15,    16,    17,
     572,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,   573,    29,    30,    31,    32,    33,   574,   587,
     311,   575,   576,   577,   578,   582,   588,    34,    35,    36,
      37,   589,   303,    38,    39,    40,     0,    41,     0,    42,
       0,     0,     0,     0,    43,     0,    44,    45,    46,     0,
      47,    48,    49,    50,    51,     0,     0,    52,    53,    54,
      55,    56,    57,    58,    59,    60,     0,     0,    61,    62,
       0,    63,     0,    64,    65,    66,    67,    68,     0,    69,
       0,     0,     0,     0,     0,     0,     0,     0,    70,    71,
      72,    73,     0,    74,     0,     0,     0,     0,    75,     0,
       0,     0,     0,     0,    76,     0,     0,    77,    78,     0,
      79,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,    13,    14,    15,    16,
      17,     0,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,     0,    29,    30,    31,    32,    33,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,    37,     0,     0,    38,    39,    40,     0,    41,     0,
      42,     0,     0,     0,     0,    43,     0,    44,    45,    46,
       0,    47,    48,    49,    50,    51,     0,     0,    52,    53,
      54,    55,    56,    57,    58,    59,    60,     0,     0,    61,
      62,     0,    63,     0,    64,    65,    66,    67,    68,     0,
      69,     0,     0,     0,     0,     0,     0,     0,     0,    70,
      71,    72,    73,     0,    74,     0,     0,     0,     0,    75,
       0,     0,     0,     0,     0,    76,     0,     0,    77,    78,
       0,    79,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,   180,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   181,     0,     0,
       0,     0,     0,     0,     0,   182,     0,     0,     0,     0,
      46,     0,   183,   184,   185,   186,   187,   188,     0,   189,
       0,     0,     0,     0,     0,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    66,     0,     0,
      46,     0,     0,     0,   313,   314,   315,   316,   317,   318,
      70,    71,    72,    73,   319,   388,   321,   322,   323,   324,
     389,   326,   327,   190,     0,     0,   542,    66,   328,    77,
      78,     0,    79,   191,   313,   314,   315,   316,   317,   318,
      70,    71,    72,    73,   319,   388,   321,   322,   323,   324,
     389,   326,   327,    46,     0,     0,   548,     0,   328,    77,
      78,     0,    79,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,     0,     0,     0,     0,     0,
      66,     0,     0,     0,     0,     0,     0,   313,   314,   315,
     316,   317,   318,    70,    71,    72,    73,   319,   388,   321,
     322,   323,   324,   389,   326,   327,     0,     0,     0,   550,
       0,   328,    77,    78,     0,    79,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     0,     0,
       0,    46,   313,   314,   315,   316,   317,   318,     0,     0,
       0,     0,   319,   320,   321,   322,   323,   324,   325,   326,
     327,     0,     0,     0,     0,     0,   328,     0,    66,   480,
       0,     0,     0,     0,     0,   313,   314,   315,   316,   317,
     318,    70,    71,    72,    73,   319,   388,   321,   322,   323,
     324,   389,   326,   327,    46,     0,     0,   552,     0,   328,
      77,    78,     0,    79,     0,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,     0,     0,     0,
       0,    66,     0,     0,     0,     0,     0,     0,   313,   314,
     315,   316,   317,   318,    70,    71,    72,    73,   319,   388,
     321,   322,   323,   324,   389,   326,   327,     0,     0,     0,
       0,     0,   328,    77,    78,     0,    79,   213,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
     214,     0,     0,    46,     0,     0,     0,     0,     0,     0,
     215,   216,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,     0,     0,     0,     0,     0,
      66,     0,     0,   313,   314,   315,   316,   317,   318,   201,
       0,     0,     0,    70,    71,    72,    73,     0,    74,   325,
     326,   327,     0,    75,     0,     0,    46,   328,     0,   217,
       0,     0,    77,    78,     0,    79,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     0,     0,
      46,     0,     0,    66,     0,     0,  -299,  -299,  -299,  -299,
    -299,  -299,     0,     0,     0,     0,    70,    71,    72,    73,
       0,    74,  -299,  -299,  -299,     0,    75,    66,     0,     0,
     328,     0,   202,     0,     0,    77,    78,     0,    79,     0,
      70,    71,    72,    73,     0,    74,     0,     0,     0,     0,
     174,     0,     0,     0,    46,     0,   175,     0,     0,    77,
      78,     0,    79,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,     0,     0,     0,     0,     0,
       0,    66,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    70,    71,    72,    73,     0,    74,
       0,     0,     0,     0,    75,     0,     0,     0,     0,     0,
     243,     0,     0,    77,    78,     0,    79,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,    46,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,     0,     0,    66,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    70,    71,    72,    73,     0,    74,     0,     0,     0,
       0,    75,     0,     0,     0,    46,     0,   383,     0,     0,
      77,    78,     0,    79,     1,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,     0,     0,     0,    46,
       0,     0,    66,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    70,    71,    72,    73,     0,
      74,     0,     0,     0,     0,    75,    66,     0,     0,     0,
       0,   398,     0,     0,    77,    78,     0,    79,     0,    70,
      71,    72,    73,     0,    74,     0,     0,     0,     0,    75,
       0,     0,    46,     0,     0,   427,     0,     0,    77,    78,
       0,    79,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,     0,     0,     0,     0,     0,    66,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    70,    71,    72,    73,     0,    74,     0,     0,
       0,     0,    75,     0,     0,     0,     0,     0,   556,     0,
       0,    77,    78,     0,    79,   153,   154,   155,   156,   157,
       6,     7,     8,     9,    10,    11,   158,     0,     0,     0,
      46,     0,     0,     0,     0,     0,   153,   154,   155,   156,
     157,     6,     7,     8,     9,    10,    11,   158,     0,     0,
     313,   314,   315,   316,   317,   318,     0,    66,     0,     0,
     319,   320,   321,   322,   323,   324,   325,   326,   327,     0,
      70,    71,    72,    73,   328,    74,     0,     0,     0,     0,
      75,     0,     0,   159,   347,   348,     0,     0,     0,    77,
      78,     0,    79,     0,   349,   350,   351,   352,     0,     0,
     353,   354,   355,     0,   159,     0,     0,     0,   356,     0,
     160,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   161,   162,   163,   164,     0,   165,     0,
       0,   160,     0,     0,     0,     0,     0,     0,     0,   166,
       0,     0,   167,   168,   161,   162,   163,   164,     0,   165,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   167,   168,   257,     0,   258,   259,   260,
       0,   261,   262,   263,   264,   265,   266,   267,   268,   269,
      27,    28,     0,   270,   271,   272,   273,   274,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   275,   276,   277,
     278,     0,     0,   279,   280,   281,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   282,   283,     0,     0,
       0,   284,   285,   286,   287,     0,     0,   288,   289,   290,
     291,   292,   293,     0,   294,   295,     0,     0,   296,   297,
       0,   298,     0,     0,     0,     0,   299,   300,     0,   301,
       0,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   330,   302,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   385,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   397,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   407,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   412,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   413,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   421,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   431,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   432,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   479,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   497,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   535,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   546,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   547,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   555,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   558,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   559,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   560,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   561,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   562,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   563,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   564,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   565,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   566,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   579,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   580,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   581,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   583,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   584,     0,   328,   313,   314,   315,   316,
     317,   318,     0,     0,     0,     0,   319,   320,   321,   322,
     323,   324,   325,   326,   327,     0,     0,     0,   585,     0,
     328,   313,   314,   315,   316,   317,   318,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   325,   326,   327,
       0,     0,     0,   586,     0,   328
};

static const yytype_int16 yycheck[] =
{
       0,   165,    48,   167,   168,    50,    51,   184,   185,   186,
      19,    19,    71,    98,    14,    17,    19,    19,    17,    72,
      20,    21,    19,   127,   101,   127,    98,    71,    71,    71,
      71,    98,    71,    98,    98,    16,    17,    37,    98,   127,
      40,    98,   127,    43,    44,   104,   127,    47,    48,    48,
     127,   104,    98,    53,    54,   127,    95,   127,   127,   127,
      60,    18,   127,   127,   127,    65,   127,    67,   127,    69,
      71,   127,    71,   127,    74,    75,    33,    77,    78,    79,
      80,   127,    71,   127,   127,   127,   127,    63,    64,    65,
      66,    61,   127,   127,   127,   127,   105,   106,   107,   108,
     109,   110,   127,   148,    98,   104,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    72,   128,   128,   127,   127,
     129,   129,   128,   128,   127,    71,    98,    71,   127,    19,
     127,   127,    89,    90,   127,   135,   128,   128,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,   128,   128,   165,   128,   167,   168,   127,   127,   214,
     127,   127,   127,   127,   174,   127,   107,   108,   109,   110,
     127,   129,   127,   347,   348,   349,   350,   351,   352,   353,
     354,   355,   356,   370,    59,   372,   373,   374,   129,   127,
     127,   201,   127,   127,    69,   205,   206,   107,   108,   109,
     110,   128,   127,   213,   127,   105,   106,    71,   218,   127,
     127,   127,   222,   127,   127,   115,   116,   117,   118,   129,
     127,   121,   122,   123,   127,   127,   127,   127,   238,   127,
     240,   241,   127,   127,    98,   127,   127,   112,   113,   127,
     250,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     127,    19,   127,   127,   127,   129,   130,   131,   127,   133,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   105,   106,   127,   127,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     121,   122,   123,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   127,
     127,   127,   127,   127,   334,   335,   336,   337,   338,   339,
     127,   341,   342,   343,   127,   347,   348,   349,   350,   351,
     352,   353,   354,   355,   356,   127,   127,   105,   106,   107,
     108,   109,   110,   127,   127,   127,    71,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   127,   127,   127,   127,
     127,   129,   127,   127,   127,   105,   106,   387,   388,   389,
     390,   127,   127,    98,   127,   115,   116,   117,   118,   399,
     127,   121,   122,   123,   127,   127,   111,   112,   113,   114,
      71,   116,   132,    71,    71,   127,   121,   417,   127,   419,
     420,   127,   127,   127,   127,   130,   131,     0,   133,   429,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,   127,   127,    71,    18,    19,    20,    21,    22,
     127,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,   127,    36,    37,    38,    39,    40,   127,    71,
      80,   127,   127,   127,   127,   127,   127,    50,    51,    52,
      53,   127,    68,    56,    57,    58,    -1,    60,    -1,    62,
      -1,    -1,    -1,    -1,    67,    -1,    69,    70,    71,    -1,
      73,    74,    75,    76,    77,    -1,    -1,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    -1,    -1,    91,    92,
      -1,    94,    -1,    96,    97,    98,    99,   100,    -1,   102,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,   112,
     113,   114,    -1,   116,    -1,    -1,    -1,    -1,   121,    -1,
      -1,    -1,    -1,    -1,   127,    -1,    -1,   130,   131,    -1,
     133,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    18,    19,    20,    21,
      22,    -1,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    -1,    36,    37,    38,    39,    40,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    51,
      52,    53,    -1,    -1,    56,    57,    58,    -1,    60,    -1,
      62,    -1,    -1,    -1,    -1,    67,    -1,    69,    70,    71,
      -1,    73,    74,    75,    76,    77,    -1,    -1,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    -1,    -1,    91,
      92,    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,
     102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,
     112,   113,   114,    -1,   116,    -1,    -1,    -1,    -1,   121,
      -1,    -1,    -1,    -1,    -1,   127,    -1,    -1,   130,   131,
      -1,   133,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    15,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    35,    -1,    -1,    -1,    -1,
      71,    -1,    42,    43,    44,    45,    46,    47,    -1,    49,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    98,    -1,    -1,
      71,    -1,    -1,    -1,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,    93,    -1,    -1,   127,    98,   129,   130,
     131,    -1,   133,   103,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,    71,    -1,    -1,   127,    -1,   129,   130,
     131,    -1,   133,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      98,    -1,    -1,    -1,    -1,    -1,    -1,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,    -1,    -1,    -1,   127,
      -1,   129,   130,   131,    -1,   133,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    71,   105,   106,   107,   108,   109,   110,    -1,    -1,
      -1,    -1,   115,   116,   117,   118,   119,   120,   121,   122,
     123,    -1,    -1,    -1,    -1,    -1,   129,    -1,    98,   132,
      -1,    -1,    -1,    -1,    -1,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,    71,    -1,    -1,   127,    -1,   129,
     130,   131,    -1,   133,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
      -1,    -1,   129,   130,   131,    -1,   133,    55,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      68,    -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,
      78,    79,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      98,    -1,    -1,   105,   106,   107,   108,   109,   110,    54,
      -1,    -1,    -1,   111,   112,   113,   114,    -1,   116,   121,
     122,   123,    -1,   121,    -1,    -1,    71,   129,    -1,   127,
      -1,    -1,   130,   131,    -1,   133,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      71,    -1,    -1,    98,    -1,    -1,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   111,   112,   113,   114,
      -1,   116,   121,   122,   123,    -1,   121,    98,    -1,    -1,
     129,    -1,   127,    -1,    -1,   130,   131,    -1,   133,    -1,
     111,   112,   113,   114,    -1,   116,    -1,    -1,    -1,    -1,
     121,    -1,    -1,    -1,    71,    -1,   127,    -1,    -1,   130,
     131,    -1,   133,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   111,   112,   113,   114,    -1,   116,
      -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,    -1,    -1,
     127,    -1,    -1,   130,   131,    -1,   133,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    -1,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   111,   112,   113,   114,    -1,   116,    -1,    -1,    -1,
      -1,   121,    -1,    -1,    -1,    71,    -1,   127,    -1,    -1,
     130,   131,    -1,   133,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    71,
      -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   111,   112,   113,   114,    -1,
     116,    -1,    -1,    -1,    -1,   121,    98,    -1,    -1,    -1,
      -1,   127,    -1,    -1,   130,   131,    -1,   133,    -1,   111,
     112,   113,   114,    -1,   116,    -1,    -1,    -1,    -1,   121,
      -1,    -1,    71,    -1,    -1,   127,    -1,    -1,   130,   131,
      -1,   133,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    98,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   111,   112,   113,   114,    -1,   116,    -1,    -1,
      -1,    -1,   121,    -1,    -1,    -1,    -1,    -1,   127,    -1,
      -1,   130,   131,    -1,   133,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      71,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
     105,   106,   107,   108,   109,   110,    -1,    98,    -1,    -1,
     115,   116,   117,   118,   119,   120,   121,   122,   123,    -1,
     111,   112,   113,   114,   129,   116,    -1,    -1,    -1,    -1,
     121,    -1,    -1,    71,   105,   106,    -1,    -1,    -1,   130,
     131,    -1,   133,    -1,   115,   116,   117,   118,    -1,    -1,
     121,   122,   123,    -1,    71,    -1,    -1,    -1,   129,    -1,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   111,   112,   113,   114,    -1,   116,    -1,
      -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   127,
      -1,    -1,   130,   131,   111,   112,   113,   114,    -1,   116,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   130,   131,    18,    -1,    20,    21,    22,
      -1,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    -1,    36,    37,    38,    39,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    51,    52,
      53,    -1,    -1,    56,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    69,    70,    -1,    -1,
      -1,    74,    75,    76,    77,    -1,    -1,    80,    81,    82,
      83,    84,    85,    -1,    87,    88,    -1,    -1,    91,    92,
      -1,    94,    -1,    -1,    -1,    -1,    99,   100,    -1,   102,
      -1,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,   127,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129,   105,   106,   107,   108,
     109,   110,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,    -1,    -1,   127,    -1,
     129,   105,   106,   107,   108,   109,   110,    -1,    -1,    -1,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   127,    -1,   129
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    18,    19,    20,    21,    22,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    36,
      37,    38,    39,    40,    50,    51,    52,    53,    56,    57,
      58,    60,    62,    67,    69,    70,    71,    73,    74,    75,
      76,    77,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    91,    92,    94,    96,    97,    98,    99,   100,   102,
     111,   112,   113,   114,   116,   121,   127,   130,   131,   133,
     135,   136,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   192,    19,   127,   192,    48,    98,
     127,   127,     3,     4,     5,     6,     7,    59,    69,   112,
     113,   139,   127,     3,     4,     5,     6,     7,    14,    71,
      98,   111,   112,   113,   114,   116,   127,   130,   131,   139,
     191,    71,   127,   192,   121,   127,   192,    98,    98,    98,
      15,    27,    35,    42,    43,    44,    45,    46,    47,    49,
      93,   103,   127,   127,   127,   127,   127,   127,   127,   127,
     127,    54,   127,   192,    71,    71,   192,    61,    63,    64,
      65,    66,   192,    55,    68,    78,    79,   127,   192,    72,
     104,   192,   192,    16,    17,   137,   137,   137,   127,   192,
     192,   127,   127,    98,   127,    98,   127,    18,    33,    72,
      89,    90,   127,   127,   192,    17,    48,    71,   104,   127,
      71,    95,    71,   127,   127,   192,   192,    18,    20,    21,
      22,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      36,    37,    38,    39,    40,    50,    51,    52,    53,    56,
      57,    58,    69,    70,    74,    75,    76,    77,    80,    81,
      82,    83,    84,    85,    87,    88,    91,    92,    94,    99,
     100,   102,   127,   138,   192,   192,   192,   192,   192,   192,
       0,   136,   127,   105,   106,   107,   108,   109,   110,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   129,   192,
     127,    98,    98,   127,   128,   128,   128,   128,   128,   128,
     137,   128,   128,   128,   191,   191,   191,   105,   106,   115,
     116,   117,   118,   121,   122,   123,   129,    19,   127,    19,
     127,   192,    19,   127,   127,   127,   127,   127,   127,   127,
     164,   192,   164,   164,   164,   127,   127,   127,    71,   127,
      71,   104,   127,   127,   192,   127,    71,   192,   116,   121,
     192,   127,    98,   127,   127,   127,   127,   127,   127,   192,
     137,   127,   128,   127,   192,   127,   127,   127,   192,   127,
     127,   127,   127,   127,   127,   127,   127,   192,   127,   192,
     192,   127,   127,   127,   101,   127,   127,   127,   192,    71,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     132,   192,   192,   192,   192,   192,   192,   192,   192,   192,
     192,   192,   192,   192,   192,   192,   192,   127,   127,   127,
     192,   192,   192,   192,   192,   192,   127,   192,   192,   192,
     132,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,    71,    71,    19,   127,    71,   164,   164,   164,   164,
     127,    71,   127,    71,   127,   127,   127,   192,   192,   192,
     192,   127,   127,   192,   127,    98,   127,   127,   127,   192,
     127,   192,   127,   192,   127,   127,   127,   192,   127,   127,
     127,   127,   127,   127,   127,   127,   127,    19,   127,   127,
     127,    71,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,    71,   127,   127
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   134,   135,   135,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   137,   137,   138,
     138,   139,   139,   139,   139,   139,   139,   140,   140,   141,
     142,   143,   143,   143,   143,   143,   143,   144,   145,   146,
     147,   148,   149,   150,   150,   151,   151,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   153,   153,
     153,   153,   154,   155,   156,   156,   157,   157,   157,   157,
     158,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   161,   162,   162,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   164,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     178,   178,   178,   179,   180,   181,   182,   183,   183,   183,
     183,   183,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   185,   185,   186,   187,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   189,   190,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   192,
     192,   192,   192,   192,   192,   192,   192,   192,   192,   192,
     192,   192,   192,   192,   192,   192,   192,   192,   192,   192,
     192,   192,   192,   192,   192,   192,   192,   192,   192,   192,
     192,   192,   192
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     2,
       2,     3,     3,     3,     3,     4,     2,     3,     3,     2,
       3,     3,     3,     2,     3,     2,     3,     3,     3,     2,
       4,     4,     4,     5,     5,     5,     2,     3,     3,     4,
       4,     5,     2,     3,     2,     4,     2,     3,     4,     4,
       2,     4,     5,     5,     5,     5,     5,     5,     5,     5,
       5,     2,     5,     7,     2,     3,     5,     3,     5,     2,
       3,     5,     4,     6,     2,     2,     3,     3,     3,     5,
       5,     5,     5,     3,     3,     3,     3,     3,     4,     3,
       4,     5,     4,     5,     0,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     3,     3,     3,     2,     4,
       3,     3,     2,     4,     5,     5,     3,     3,     4,     3,
       3,     3,     2,     3,     4,     3,     4,     5,     3,     4,
       5,     3,     3,     3,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       2,     2,     3
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
        case 54:
#line 205 "parser.y" /* yacc.c:1646  */
    { eval_value = (yyvsp[0].uval); }
#line 2288 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 208 "parser.y" /* yacc.c:1646  */
    {
      }
#line 2295 "y.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 215 "parser.y" /* yacc.c:1646  */
    { (yyval.bval)=(yyvsp[0].bval); }
#line 2301 "y.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 221 "parser.y" /* yacc.c:1646  */
    { (yyval.sval)=(yyvsp[0].sval); }
#line 2307 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 231 "parser.y" /* yacc.c:1646  */
    { (yyval.uval)=(yyvsp[0].uval); }
#line 2313 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 236 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_timebp_command(0, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2322 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 241 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_timebp_command(1, (yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2331 "y.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 249 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_modebp_command();
          free((yyvsp[-1].sval));
      }
#line 2340 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 257 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_vmexitbp_command();
          free((yyvsp[-1].sval));
      }
#line 2349 "y.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 265 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_command((yyvsp[-1].sval));
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2358 "y.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 270 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_command("all");
          free((yyvsp[-2].sval));
      }
#line 2367 "y.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 275 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_command("off");
          free((yyvsp[-2].sval));
      }
#line 2376 "y.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 280 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_param_command((yyvsp[-1].sval), 0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2385 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 285 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_param_command((yyvsp[-2].sval), 1);
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2394 "y.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 290 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_show_command(0);
          free((yyvsp[-1].sval));
      }
#line 2403 "y.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 298 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_xlate_address((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2412 "y.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 306 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_tlb_lookup((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2421 "y.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 314 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_ptime_command();
          free((yyvsp[-1].sval));
      }
#line 2430 "y.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 322 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_trace_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2439 "y.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 330 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_trace_reg_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2448 "y.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 338 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_trace_mem_command((yyvsp[-1].bval));
          free((yyvsp[-2].sval));
      }
#line 2457 "y.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 346 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_print_stack_command(16);
          free((yyvsp[-1].sval));
      }
#line 2466 "y.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 351 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_print_stack_command((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2475 "y.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 359 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_bt_command(16);
        free((yyvsp[-1].sval));
      }
#line 2484 "y.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 364 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_bt_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2493 "y.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 372 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watchpoint_continue(0);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2502 "y.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 377 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watchpoint_continue(1);
          free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2511 "y.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 382 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_print_watchpoints();
          free((yyvsp[-1].sval));
      }
#line 2520 "y.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 387 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2529 "y.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 392 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(0, (yyvsp[-1].uval), 1); /* BX_READ */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2538 "y.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 397 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(1, (yyvsp[-1].uval), 1); /* BX_WRITE */
          free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2547 "y.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 402 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2556 "y.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 407 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(0, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_READ */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2565 "y.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 412 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_watch(1, (yyvsp[-2].uval), (yyvsp[-1].uval)); /* BX_WRITE */
          free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2574 "y.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 417 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_unwatch_all();
          free((yyvsp[-1].sval));
      }
#line 2583 "y.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 422 "parser.y" /* yacc.c:1646  */
    {
          bx_dbg_unwatch((yyvsp[-1].uval));
          free((yyvsp[-2].sval));
      }
#line 2592 "y.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 430 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_symbol_command((yyvsp[-1].sval), 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2601 "y.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 435 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_symbol_command((yyvsp[-2].sval), 0, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2610 "y.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 440 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_symbol_command((yyvsp[-1].sval), 1, 0);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2619 "y.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 445 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_symbol_command((yyvsp[-2].sval), 1, (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2628 "y.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 453 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_where_command();
        free((yyvsp[-1].sval));
      }
#line 2637 "y.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 461 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_print_string_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2646 "y.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 469 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_continue_command(1);
        free((yyvsp[-1].sval));
      }
#line 2655 "y.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 474 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_continue_command((yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2664 "y.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 482 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_stepN_command(dbg_cpu, 1);
        free((yyvsp[-1].sval));
      }
#line 2673 "y.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 487 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_stepN_command(dbg_cpu, (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 2682 "y.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 492 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_stepN_command(-1, (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2691 "y.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 497 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_stepN_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 2700 "y.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 505 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_step_over_command();
        free((yyvsp[-1].sval));
      }
#line 2709 "y.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 513 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_set_auto_disassemble((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 2718 "y.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 518 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_set_symbol_command((yyvsp[-3].sval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2727 "y.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 523 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg8l_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2735 "y.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 527 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg8h_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2743 "y.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 531 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg16_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2751 "y.tab.c" /* yacc.c:1646  */
    break;

  case 116:
#line 535 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg32_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2759 "y.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 539 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_reg64_value((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2767 "y.tab.c" /* yacc.c:1646  */
    break;

  case 118:
#line 543 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2775 "y.tab.c" /* yacc.c:1646  */
    break;

  case 119:
#line 547 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_set_rip_value((yyvsp[-1].uval));
      }
#line 2783 "y.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 551 "parser.y" /* yacc.c:1646  */
    { 
        bx_dbg_load_segreg((yyvsp[-3].uval), (yyvsp[-1].uval));
      }
#line 2791 "y.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 558 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_vbreakpoint_command(bkAtIP, 0, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2800 "y.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 563 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].uval), NULL);
        free((yyvsp[-4].sval));
      }
#line 2809 "y.tab.c" /* yacc.c:1646  */
    break;

  case 123:
#line 568 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_vbreakpoint_command(bkRegular, (yyvsp[-5].uval), (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-6].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2818 "y.tab.c" /* yacc.c:1646  */
    break;

  case 124:
#line 573 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_lbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2827 "y.tab.c" /* yacc.c:1646  */
    break;

  case 125:
#line 578 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 2836 "y.tab.c" /* yacc.c:1646  */
    break;

  case 126:
#line 583 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_lbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2845 "y.tab.c" /* yacc.c:1646  */
    break;

  case 127:
#line 588 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-1].sval), NULL);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2854 "y.tab.c" /* yacc.c:1646  */
    break;

  case 128:
#line 593 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_lbreakpoint_symbol_command((yyvsp[-3].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2863 "y.tab.c" /* yacc.c:1646  */
    break;

  case 129:
#line 598 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_pbreakpoint_command(bkAtIP, 0, NULL);
        free((yyvsp[-1].sval));
      }
#line 2872 "y.tab.c" /* yacc.c:1646  */
    break;

  case 130:
#line 603 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-2].sval));
      }
#line 2881 "y.tab.c" /* yacc.c:1646  */
    break;

  case 131:
#line 608 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2890 "y.tab.c" /* yacc.c:1646  */
    break;

  case 132:
#line 613 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-1].uval), NULL);
        free((yyvsp[-3].sval));
      }
#line 2899 "y.tab.c" /* yacc.c:1646  */
    break;

  case 133:
#line 618 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_pbreakpoint_command(bkRegular, (yyvsp[-3].uval), (yyvsp[-1].sval));
        free((yyvsp[-5].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2908 "y.tab.c" /* yacc.c:1646  */
    break;

  case 134:
#line 626 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-1].sval));
      }
#line 2917 "y.tab.c" /* yacc.c:1646  */
    break;

  case 135:
#line 634 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-1].sval));
      }
#line 2926 "y.tab.c" /* yacc.c:1646  */
    break;

  case 136:
#line 639 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval));free((yyvsp[-1].sval));
      }
#line 2935 "y.tab.c" /* yacc.c:1646  */
    break;

  case 137:
#line 647 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_bpoints_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2944 "y.tab.c" /* yacc.c:1646  */
    break;

  case 138:
#line 652 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(-1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2953 "y.tab.c" /* yacc.c:1646  */
    break;

  case 139:
#line 657 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_idt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2962 "y.tab.c" /* yacc.c:1646  */
    break;

  case 140:
#line 662 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_ivt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2971 "y.tab.c" /* yacc.c:1646  */
    break;

  case 141:
#line 667 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_gdt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2980 "y.tab.c" /* yacc.c:1646  */
    break;

  case 142:
#line 672 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_ldt_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 2989 "y.tab.c" /* yacc.c:1646  */
    break;

  case 143:
#line 677 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_dump_table();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 2998 "y.tab.c" /* yacc.c:1646  */
    break;

  case 144:
#line 682 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_tss_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3007 "y.tab.c" /* yacc.c:1646  */
    break;

  case 145:
#line 687 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_flags();
        free((yyvsp[-2].sval));
      }
#line 3016 "y.tab.c" /* yacc.c:1646  */
    break;

  case 146:
#line 692 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_linux_command();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3025 "y.tab.c" /* yacc.c:1646  */
    break;

  case 147:
#line 697 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_symbols_command(0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3034 "y.tab.c" /* yacc.c:1646  */
    break;

  case 148:
#line 702 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_symbols_command((yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3043 "y.tab.c" /* yacc.c:1646  */
    break;

  case 149:
#line 707 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device("", "");
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3052 "y.tab.c" /* yacc.c:1646  */
    break;

  case 150:
#line 712 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3061 "y.tab.c" /* yacc.c:1646  */
    break;

  case 151:
#line 717 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3070 "y.tab.c" /* yacc.c:1646  */
    break;

  case 152:
#line 722 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device((yyvsp[-1].sval), "");
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3079 "y.tab.c" /* yacc.c:1646  */
    break;

  case 153:
#line 727 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_device((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3088 "y.tab.c" /* yacc.c:1646  */
    break;

  case 154:
#line 734 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = EMPTY_ARG; }
#line 3094 "y.tab.c" /* yacc.c:1646  */
    break;

  case 156:
#line 739 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_GENERAL_PURPOSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3103 "y.tab.c" /* yacc.c:1646  */
    break;

  case 157:
#line 747 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free((yyvsp[-1].sval));
      }
#line 3112 "y.tab.c" /* yacc.c:1646  */
    break;

  case 158:
#line 755 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_MMX_REGS);
        free((yyvsp[-1].sval));
      }
#line 3121 "y.tab.c" /* yacc.c:1646  */
    break;

  case 159:
#line 763 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_SSE_REGS);
        free((yyvsp[-1].sval));
      }
#line 3130 "y.tab.c" /* yacc.c:1646  */
    break;

  case 160:
#line 771 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_YMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3139 "y.tab.c" /* yacc.c:1646  */
    break;

  case 161:
#line 779 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_registers_command(BX_INFO_ZMM_REGS);
        free((yyvsp[-1].sval));
      }
#line 3148 "y.tab.c" /* yacc.c:1646  */
    break;

  case 162:
#line 787 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_segment_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3157 "y.tab.c" /* yacc.c:1646  */
    break;

  case 163:
#line 795 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_control_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3166 "y.tab.c" /* yacc.c:1646  */
    break;

  case 164:
#line 803 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_info_debug_regs_command();
        free((yyvsp[-1].sval));
      }
#line 3175 "y.tab.c" /* yacc.c:1646  */
    break;

  case 165:
#line 811 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_del_breakpoint_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3184 "y.tab.c" /* yacc.c:1646  */
    break;

  case 166:
#line 819 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3193 "y.tab.c" /* yacc.c:1646  */
    break;

  case 167:
#line 826 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_en_dis_breakpoint_command((yyvsp[-1].uval), 0);
        free((yyvsp[-2].sval));
      }
#line 3202 "y.tab.c" /* yacc.c:1646  */
    break;

  case 168:
#line 834 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_quit_command();
        free((yyvsp[-1].sval));
      }
#line 3211 "y.tab.c" /* yacc.c:1646  */
    break;

  case 169:
#line 842 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_examine_command((yyvsp[-3].sval), (yyvsp[-2].sval),1, (yyvsp[-1].uval), 1);
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3220 "y.tab.c" /* yacc.c:1646  */
    break;

  case 170:
#line 847 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_examine_command((yyvsp[-2].sval), (yyvsp[-1].sval),1, 0, 0);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3229 "y.tab.c" /* yacc.c:1646  */
    break;

  case 171:
#line 852 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_examine_command((yyvsp[-2].sval), NULL,0, (yyvsp[-1].uval), 1);
        free((yyvsp[-2].sval));
      }
#line 3238 "y.tab.c" /* yacc.c:1646  */
    break;

  case 172:
#line 857 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_examine_command((yyvsp[-1].sval), NULL,0, 0, 0);
        free((yyvsp[-1].sval));
      }
#line 3247 "y.tab.c" /* yacc.c:1646  */
    break;

  case 173:
#line 865 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_restore_command((yyvsp[-2].sval), (yyvsp[-1].sval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3256 "y.tab.c" /* yacc.c:1646  */
    break;

  case 174:
#line 873 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_writemem_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3265 "y.tab.c" /* yacc.c:1646  */
    break;

  case 175:
#line 881 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_setpmem_command((yyvsp[-3].uval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval));
      }
#line 3274 "y.tab.c" /* yacc.c:1646  */
    break;

  case 176:
#line 889 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_query_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3283 "y.tab.c" /* yacc.c:1646  */
    break;

  case 177:
#line 897 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3292 "y.tab.c" /* yacc.c:1646  */
    break;

  case 178:
#line 902 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_take_command((yyvsp[-2].sval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3301 "y.tab.c" /* yacc.c:1646  */
    break;

  case 179:
#line 907 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3310 "y.tab.c" /* yacc.c:1646  */
    break;

  case 180:
#line 912 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3319 "y.tab.c" /* yacc.c:1646  */
    break;

  case 181:
#line 917 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_take_command((yyvsp[-1].sval), 1);
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3328 "y.tab.c" /* yacc.c:1646  */
    break;

  case 182:
#line 925 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_current(NULL);
        free((yyvsp[-1].sval));
      }
#line 3337 "y.tab.c" /* yacc.c:1646  */
    break;

  case 183:
#line 930 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3346 "y.tab.c" /* yacc.c:1646  */
    break;

  case 184:
#line 935 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_command(NULL, (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3355 "y.tab.c" /* yacc.c:1646  */
    break;

  case 185:
#line 940 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_current((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3364 "y.tab.c" /* yacc.c:1646  */
    break;

  case 186:
#line 945 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_command((yyvsp[-2].sval), (yyvsp[-1].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3373 "y.tab.c" /* yacc.c:1646  */
    break;

  case 187:
#line 950 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_command((yyvsp[-3].sval), (yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3382 "y.tab.c" /* yacc.c:1646  */
    break;

  case 188:
#line 955 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_switch_mode();
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3391 "y.tab.c" /* yacc.c:1646  */
    break;

  case 189:
#line 960 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_disassemble_hex_mode_switch((yyvsp[-1].bval));
        free((yyvsp[-3].sval)); free((yyvsp[-2].sval));
      }
#line 3400 "y.tab.c" /* yacc.c:1646  */
    break;

  case 190:
#line 965 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_set_disassemble_size((yyvsp[-1].uval));
        free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
      }
#line 3409 "y.tab.c" /* yacc.c:1646  */
    break;

  case 191:
#line 973 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3418 "y.tab.c" /* yacc.c:1646  */
    break;

  case 192:
#line 978 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_instrument_command((yyvsp[-1].sval));
        free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
      }
#line 3427 "y.tab.c" /* yacc.c:1646  */
    break;

  case 193:
#line 986 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_doit_command((yyvsp[-1].uval));
        free((yyvsp[-2].sval));
      }
#line 3436 "y.tab.c" /* yacc.c:1646  */
    break;

  case 194:
#line 994 "parser.y" /* yacc.c:1646  */
    {
        bx_dbg_crc_command((yyvsp[-2].uval), (yyvsp[-1].uval));
        free((yyvsp[-3].sval));
      }
#line 3445 "y.tab.c" /* yacc.c:1646  */
    break;

  case 195:
#line 1002 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("q|quit|exit - quit debugger and emulator execution\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3454 "y.tab.c" /* yacc.c:1646  */
    break;

  case 196:
#line 1007 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("c|cont|continue - continue executing\n");
         dbg_printf("c|cont|continue if \"expression\" - continue executing only if expression is true\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3464 "y.tab.c" /* yacc.c:1646  */
    break;

  case 197:
#line 1013 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("s|step [count] - execute #count instructions on current processor (default is one instruction)\n");
         dbg_printf("s|step [cpu] <count> - execute #count instructions on processor #cpu\n");
         dbg_printf("s|step all <count> - execute #count instructions on all the processors\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3475 "y.tab.c" /* yacc.c:1646  */
    break;

  case 198:
#line 1020 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("n|next|p - execute instruction stepping over subroutines\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3484 "y.tab.c" /* yacc.c:1646  */
    break;

  case 199:
#line 1025 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("vb|vbreak <seg:offset> - set a virtual address instruction breakpoint\n");
         dbg_printf("vb|vbreak <seg:offset> if \"expression\" - set a conditional virtual address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3494 "y.tab.c" /* yacc.c:1646  */
    break;

  case 200:
#line 1031 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("lb|lbreak <addr> - set a linear address instruction breakpoint\n");
         dbg_printf("lb|lbreak <addr> if \"expression\" - set a conditional linear address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3504 "y.tab.c" /* yacc.c:1646  */
    break;

  case 201:
#line 1037 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("p|pb|break|pbreak <addr> - set a physical address instruction breakpoint\n");
         dbg_printf("p|pb|break|pbreak <addr> if \"expression\" - set a conditional physical address instruction breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3514 "y.tab.c" /* yacc.c:1646  */
    break;

  case 202:
#line 1043 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("d|del|delete <n> - delete a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3523 "y.tab.c" /* yacc.c:1646  */
    break;

  case 203:
#line 1048 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("bpe <n> - enable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3532 "y.tab.c" /* yacc.c:1646  */
    break;

  case 204:
#line 1053 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("bpd <n> - disable a breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3541 "y.tab.c" /* yacc.c:1646  */
    break;

  case 205:
#line 1058 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("blist - list all breakpoints (same as 'info break')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3550 "y.tab.c" /* yacc.c:1646  */
    break;

  case 206:
#line 1063 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("modebp - toggles mode switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3559 "y.tab.c" /* yacc.c:1646  */
    break;

  case 207:
#line 1068 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("vmexitbp - toggles VMEXIT switch breakpoint\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3568 "y.tab.c" /* yacc.c:1646  */
    break;

  case 208:
#line 1073 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("crc <addr1> <addr2> - show CRC32 for physical memory range addr1..addr2\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3577 "y.tab.c" /* yacc.c:1646  */
    break;

  case 209:
#line 1078 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("trace on  - print disassembly for every executed instruction\n");
         dbg_printf("trace off - disable instruction tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3587 "y.tab.c" /* yacc.c:1646  */
    break;

  case 210:
#line 1084 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("trace-reg on  - print all registers before every executed instruction\n");
         dbg_printf("trace-reg off - disable registers state tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3597 "y.tab.c" /* yacc.c:1646  */
    break;

  case 211:
#line 1090 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("trace-mem on  - print all memory accesses occurred during instruction execution\n");
         dbg_printf("trace-mem off - disable memory accesses tracing\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3607 "y.tab.c" /* yacc.c:1646  */
    break;

  case 212:
#line 1096 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("restore <param_name> [path] - restore bochs root param from the file\n");
         dbg_printf("for example:\n");
         dbg_printf("restore \"cpu0\" - restore CPU #0 from file \"cpu0\" in current directory\n");
         dbg_printf("restore \"cpu0\" \"/save\" - restore CPU #0 from file \"cpu0\" located in directory \"/save\"\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3619 "y.tab.c" /* yacc.c:1646  */
    break;

  case 213:
#line 1104 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("ptime - print current time (number of ticks since start of simulation)\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3628 "y.tab.c" /* yacc.c:1646  */
    break;

  case 214:
#line 1109 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("sb <delta> - insert a time breakpoint delta instructions into the future\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3637 "y.tab.c" /* yacc.c:1646  */
    break;

  case 215:
#line 1114 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("sba <time> - insert breakpoint at specific time\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3646 "y.tab.c" /* yacc.c:1646  */
    break;

  case 216:
#line 1119 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("print-stack [num_words] - print the num_words top 16 bit words on the stack\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3655 "y.tab.c" /* yacc.c:1646  */
    break;

  case 217:
#line 1124 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("ldsym [global] <filename> [offset] - load symbols from file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3664 "y.tab.c" /* yacc.c:1646  */
    break;

  case 218:
#line 1129 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("slist [string] - list symbols whose preffix is string (same as 'info symbols')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3673 "y.tab.c" /* yacc.c:1646  */
    break;

  case 219:
#line 1134 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("r|reg|regs|registers - list of CPU registers and their contents (same as 'info registers')\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3682 "y.tab.c" /* yacc.c:1646  */
    break;

  case 220:
#line 1139 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("fp|fpu - print FPU state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3691 "y.tab.c" /* yacc.c:1646  */
    break;

  case 221:
#line 1144 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("mmx - print MMX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3700 "y.tab.c" /* yacc.c:1646  */
    break;

  case 222:
#line 1149 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("xmm|sse - print SSE state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3709 "y.tab.c" /* yacc.c:1646  */
    break;

  case 223:
#line 1154 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("ymm - print AVX state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3718 "y.tab.c" /* yacc.c:1646  */
    break;

  case 224:
#line 1159 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("zmm - print AVX-512 state\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3727 "y.tab.c" /* yacc.c:1646  */
    break;

  case 225:
#line 1164 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("sreg - show segment registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3736 "y.tab.c" /* yacc.c:1646  */
    break;

  case 226:
#line 1169 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("creg - show control registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3745 "y.tab.c" /* yacc.c:1646  */
    break;

  case 227:
#line 1174 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("dreg - show debug registers\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3754 "y.tab.c" /* yacc.c:1646  */
    break;

  case 228:
#line 1179 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("writemem <filename> <laddr> <len> - dump 'len' bytes of virtual memory starting from the linear address 'laddr' into the file\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3763 "y.tab.c" /* yacc.c:1646  */
    break;

  case 229:
#line 1184 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("setpmem <addr> <datasize> <val> - set physical memory location of size 'datasize' to value 'val'\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3772 "y.tab.c" /* yacc.c:1646  */
    break;

  case 230:
#line 1189 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("u|disasm [/count] <start> <end> - disassemble instructions for given linear address\n");
         dbg_printf("    Optional 'count' is the number of disassembled instructions\n");
         dbg_printf("u|disasm switch-mode - switch between Intel and AT&T disassembler syntax\n");
         dbg_printf("u|disasm hex on/off - control disasm offsets and displacements format\n");
         dbg_printf("u|disasm size = n - tell debugger what segment size [16|32|64] to use\n");
         dbg_printf("       when \"disassemble\" command is used.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3786 "y.tab.c" /* yacc.c:1646  */
    break;

  case 231:
#line 1199 "parser.y" /* yacc.c:1646  */
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
#line 3801 "y.tab.c" /* yacc.c:1646  */
    break;

  case 232:
#line 1210 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("unwatch      - remove all watch points\n");
         dbg_printf("unwatch addr - remove a watch point\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3811 "y.tab.c" /* yacc.c:1646  */
    break;

  case 233:
#line 1216 "parser.y" /* yacc.c:1646  */
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
#line 3828 "y.tab.c" /* yacc.c:1646  */
    break;

  case 234:
#line 1229 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("instrument <command> - calls BX_INSTR_DEBUG_CMD instrumentation callback with <command>\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3837 "y.tab.c" /* yacc.c:1646  */
    break;

  case 235:
#line 1234 "parser.y" /* yacc.c:1646  */
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
#line 3852 "y.tab.c" /* yacc.c:1646  */
    break;

  case 236:
#line 1245 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("page <laddr> - show linear to physical xlation for linear address laddr\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3861 "y.tab.c" /* yacc.c:1646  */
    break;

  case 237:
#line 1250 "parser.y" /* yacc.c:1646  */
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
#line 3881 "y.tab.c" /* yacc.c:1646  */
    break;

  case 238:
#line 1266 "parser.y" /* yacc.c:1646  */
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
#line 3901 "y.tab.c" /* yacc.c:1646  */
    break;

  case 239:
#line 1282 "parser.y" /* yacc.c:1646  */
    {
         dbg_printf("calc|? <expr> - calculate a expression and display the result.\n");
         dbg_printf("    'expr' can reference any general-purpose, opmask and segment\n");
         dbg_printf("    registers, use any arithmetic and logic operations, and\n");
         dbg_printf("    also the special ':' operator which computes the linear\n");
         dbg_printf("    address of a segment:offset (in real and v86 mode) or of\n");
         dbg_printf("    a selector:offset (in protected mode) pair.\n");
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3915 "y.tab.c" /* yacc.c:1646  */
    break;

  case 240:
#line 1292 "parser.y" /* yacc.c:1646  */
    {
         bx_dbg_print_help();
         free((yyvsp[-2].sval));free((yyvsp[-1].sval));
       }
#line 3924 "y.tab.c" /* yacc.c:1646  */
    break;

  case 241:
#line 1297 "parser.y" /* yacc.c:1646  */
    {
         bx_dbg_print_help();
         free((yyvsp[-1].sval));
       }
#line 3933 "y.tab.c" /* yacc.c:1646  */
    break;

  case 242:
#line 1305 "parser.y" /* yacc.c:1646  */
    {
     eval_value = (yyvsp[-1].uval);
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 3943 "y.tab.c" /* yacc.c:1646  */
    break;

  case 243:
#line 1314 "parser.y" /* yacc.c:1646  */
    {
     eval_value = (yyvsp[-1].uval) != 0;
     bx_dbg_calc_command((yyvsp[-1].uval));
     free((yyvsp[-2].sval));
   }
#line 3953 "y.tab.c" /* yacc.c:1646  */
    break;

  case 244:
#line 1323 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[0].uval); }
#line 3959 "y.tab.c" /* yacc.c:1646  */
    break;

  case 245:
#line 1324 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 3965 "y.tab.c" /* yacc.c:1646  */
    break;

  case 246:
#line 1325 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 3971 "y.tab.c" /* yacc.c:1646  */
    break;

  case 247:
#line 1326 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 3977 "y.tab.c" /* yacc.c:1646  */
    break;

  case 248:
#line 1327 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 3983 "y.tab.c" /* yacc.c:1646  */
    break;

  case 249:
#line 1328 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 3989 "y.tab.c" /* yacc.c:1646  */
    break;

  case 250:
#line 1329 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 3995 "y.tab.c" /* yacc.c:1646  */
    break;

  case 251:
#line 1330 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4001 "y.tab.c" /* yacc.c:1646  */
    break;

  case 252:
#line 1331 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4007 "y.tab.c" /* yacc.c:1646  */
    break;

  case 253:
#line 1332 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_ip (); }
#line 4013 "y.tab.c" /* yacc.c:1646  */
    break;

  case 254:
#line 1333 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_eip(); }
#line 4019 "y.tab.c" /* yacc.c:1646  */
    break;

  case 255:
#line 1334 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_rip(); }
#line 4025 "y.tab.c" /* yacc.c:1646  */
    break;

  case 256:
#line 1335 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4031 "y.tab.c" /* yacc.c:1646  */
    break;

  case 257:
#line 1336 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4037 "y.tab.c" /* yacc.c:1646  */
    break;

  case 258:
#line 1337 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4043 "y.tab.c" /* yacc.c:1646  */
    break;

  case 259:
#line 1338 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4049 "y.tab.c" /* yacc.c:1646  */
    break;

  case 260:
#line 1339 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) / (yyvsp[0].uval); }
#line 4055 "y.tab.c" /* yacc.c:1646  */
    break;

  case 261:
#line 1340 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4061 "y.tab.c" /* yacc.c:1646  */
    break;

  case 262:
#line 1341 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4067 "y.tab.c" /* yacc.c:1646  */
    break;

  case 263:
#line 1342 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4073 "y.tab.c" /* yacc.c:1646  */
    break;

  case 264:
#line 1343 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4079 "y.tab.c" /* yacc.c:1646  */
    break;

  case 265:
#line 1344 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4085 "y.tab.c" /* yacc.c:1646  */
    break;

  case 266:
#line 1345 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = !(yyvsp[0].uval); }
#line 4091 "y.tab.c" /* yacc.c:1646  */
    break;

  case 267:
#line 1346 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = -(yyvsp[0].uval); }
#line 4097 "y.tab.c" /* yacc.c:1646  */
    break;

  case 268:
#line 1347 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-1].uval); }
#line 4103 "y.tab.c" /* yacc.c:1646  */
    break;

  case 269:
#line 1353 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[0].uval); }
#line 4109 "y.tab.c" /* yacc.c:1646  */
    break;

  case 270:
#line 1354 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_symbol_value((yyvsp[0].sval)); free((yyvsp[0].sval));}
#line 4115 "y.tab.c" /* yacc.c:1646  */
    break;

  case 271:
#line 1355 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg8l_value((yyvsp[0].uval)); }
#line 4121 "y.tab.c" /* yacc.c:1646  */
    break;

  case 272:
#line 1356 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg8h_value((yyvsp[0].uval)); }
#line 4127 "y.tab.c" /* yacc.c:1646  */
    break;

  case 273:
#line 1357 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg16_value((yyvsp[0].uval)); }
#line 4133 "y.tab.c" /* yacc.c:1646  */
    break;

  case 274:
#line 1358 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg32_value((yyvsp[0].uval)); }
#line 4139 "y.tab.c" /* yacc.c:1646  */
    break;

  case 275:
#line 1359 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_reg64_value((yyvsp[0].uval)); }
#line 4145 "y.tab.c" /* yacc.c:1646  */
    break;

  case 276:
#line 1360 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_opmask_value((yyvsp[0].uval)); }
#line 4151 "y.tab.c" /* yacc.c:1646  */
    break;

  case 277:
#line 1361 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_selector_value((yyvsp[0].uval)); }
#line 4157 "y.tab.c" /* yacc.c:1646  */
    break;

  case 278:
#line 1362 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_ip (); }
#line 4163 "y.tab.c" /* yacc.c:1646  */
    break;

  case 279:
#line 1363 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_eip(); }
#line 4169 "y.tab.c" /* yacc.c:1646  */
    break;

  case 280:
#line 1364 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_rip(); }
#line 4175 "y.tab.c" /* yacc.c:1646  */
    break;

  case 281:
#line 1365 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_ssp(); }
#line 4181 "y.tab.c" /* yacc.c:1646  */
    break;

  case 282:
#line 1366 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_get_laddr ((yyvsp[-2].uval), (yyvsp[0].uval)); }
#line 4187 "y.tab.c" /* yacc.c:1646  */
    break;

  case 283:
#line 1367 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) + (yyvsp[0].uval); }
#line 4193 "y.tab.c" /* yacc.c:1646  */
    break;

  case 284:
#line 1368 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) - (yyvsp[0].uval); }
#line 4199 "y.tab.c" /* yacc.c:1646  */
    break;

  case 285:
#line 1369 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) * (yyvsp[0].uval); }
#line 4205 "y.tab.c" /* yacc.c:1646  */
    break;

  case 286:
#line 1370 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = ((yyvsp[0].uval) != 0) ? (yyvsp[-2].uval) / (yyvsp[0].uval) : 0; }
#line 4211 "y.tab.c" /* yacc.c:1646  */
    break;

  case 287:
#line 1371 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) >> (yyvsp[0].uval); }
#line 4217 "y.tab.c" /* yacc.c:1646  */
    break;

  case 288:
#line 1372 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) << (yyvsp[0].uval); }
#line 4223 "y.tab.c" /* yacc.c:1646  */
    break;

  case 289:
#line 1373 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) | (yyvsp[0].uval); }
#line 4229 "y.tab.c" /* yacc.c:1646  */
    break;

  case 290:
#line 1374 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) ^ (yyvsp[0].uval); }
#line 4235 "y.tab.c" /* yacc.c:1646  */
    break;

  case 291:
#line 1375 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) & (yyvsp[0].uval); }
#line 4241 "y.tab.c" /* yacc.c:1646  */
    break;

  case 292:
#line 1376 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) > (yyvsp[0].uval); }
#line 4247 "y.tab.c" /* yacc.c:1646  */
    break;

  case 293:
#line 1377 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) < (yyvsp[0].uval); }
#line 4253 "y.tab.c" /* yacc.c:1646  */
    break;

  case 294:
#line 1378 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) == (yyvsp[0].uval); }
#line 4259 "y.tab.c" /* yacc.c:1646  */
    break;

  case 295:
#line 1379 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) != (yyvsp[0].uval); }
#line 4265 "y.tab.c" /* yacc.c:1646  */
    break;

  case 296:
#line 1380 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) <= (yyvsp[0].uval); }
#line 4271 "y.tab.c" /* yacc.c:1646  */
    break;

  case 297:
#line 1381 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-2].uval) >= (yyvsp[0].uval); }
#line 4277 "y.tab.c" /* yacc.c:1646  */
    break;

  case 298:
#line 1382 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = !(yyvsp[0].uval); }
#line 4283 "y.tab.c" /* yacc.c:1646  */
    break;

  case 299:
#line 1383 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = -(yyvsp[0].uval); }
#line 4289 "y.tab.c" /* yacc.c:1646  */
    break;

  case 300:
#line 1384 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_lin_indirect((yyvsp[0].uval)); }
#line 4295 "y.tab.c" /* yacc.c:1646  */
    break;

  case 301:
#line 1385 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = bx_dbg_phy_indirect((yyvsp[0].uval)); }
#line 4301 "y.tab.c" /* yacc.c:1646  */
    break;

  case 302:
#line 1386 "parser.y" /* yacc.c:1646  */
    { (yyval.uval) = (yyvsp[-1].uval); }
#line 4307 "y.tab.c" /* yacc.c:1646  */
    break;


#line 4311 "y.tab.c" /* yacc.c:1646  */
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
#line 1389 "parser.y" /* yacc.c:1906  */

#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
