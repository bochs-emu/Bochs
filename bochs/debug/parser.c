#ifndef lint
static char const 
yyrcsid[] = "$FreeBSD: src/usr.bin/yacc/skeleton.c,v 1.28 2000/01/17 02:04:06 bde Exp $";
#endif
#include <stdlib.h>
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar=(YYEMPTY))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING() (yyerrflag!=0)
static int yygrowstack();
#define yyparse bxparse
#define yylex bxlex
#define yyerror bxerror
#define yychar bxchar
#define yyval bxval
#define yylval bxlval
#define yydebug bxdebug
#define yynerrs bxnerrs
#define yyerrflag bxerrflag
#define yyss bxss
#define yyssp bxssp
#define yyvs bxvs
#define yyvsp bxvsp
#define yylhs bxlhs
#define yylen bxlen
#define yydefred bxdefred
#define yydgoto bxdgoto
#define yysindex bxsindex
#define yyrindex bxrindex
#define yygindex bxgindex
#define yytable bxtable
#define yycheck bxcheck
#define yyname bxname
#define yyrule bxrule
#define yysslim bxsslim
#define yystacksize bxstacksize
#define YYPREFIX "bx"
#line 6 "parser.y"
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

#if BX_DEBUGGER
/*
NOTE: The #if comes from parser.y.  The makefile will add the matching #endif
at the end of parser.c.  I don't know any way to ask yacc to put it at the end.
*/

/* %left '-' '+'*/
/* %left '*' '/'*/
/* %right*/
/* %nonassoc UMINUS*/
#line 23 "parser.y"
typedef union {
  char    *sval;
  Bit32u   uval;
  Bit64u   ulval;
  bx_num_range   uval_range;
  } YYSTYPE;
#line 65 "y.tab.c"
#define YYERRCODE 256
#define BX_TOKEN_CONTINUE 257
#define BX_TOKEN_STEPN 258
#define BX_TOKEN_NEXT_STEP 259
#define BX_TOKEN_SET 260
#define BX_TOKEN_DEBUGGER 261
#define BX_TOKEN_VBREAKPOINT 262
#define BX_TOKEN_LBREAKPOINT 263
#define BX_TOKEN_PBREAKPOINT 264
#define BX_TOKEN_DEL_BREAKPOINT 265
#define BX_TOKEN_INFO 266
#define BX_TOKEN_QUIT 267
#define BX_TOKEN_PROGRAM 268
#define BX_TOKEN_REGISTERS 269
#define BX_TOKEN_FPU 270
#define BX_TOKEN_ALL 271
#define BX_TOKEN_IDT 272
#define BX_TOKEN_GDT 273
#define BX_TOKEN_LDT 274
#define BX_TOKEN_TSS 275
#define BX_TOKEN_DIRTY 276
#define BX_TOKEN_LINUX 277
#define BX_TOKEN_CONTROL_REGS 278
#define BX_TOKEN_EXAMINE 279
#define BX_TOKEN_XFORMAT 280
#define BX_TOKEN_SETPMEM 281
#define BX_TOKEN_SYMBOLNAME 282
#define BX_TOKEN_QUERY 283
#define BX_TOKEN_PENDING 284
#define BX_TOKEN_TAKE 285
#define BX_TOKEN_DMA 286
#define BX_TOKEN_IRQ 287
#define BX_TOKEN_DUMP_CPU 288
#define BX_TOKEN_SET_CPU 289
#define BX_TOKEN_DIS 290
#define BX_TOKEN_ON 291
#define BX_TOKEN_OFF 292
#define BX_TOKEN_DISASSEMBLE 293
#define BX_TOKEN_INSTRUMENT 294
#define BX_TOKEN_START 295
#define BX_TOKEN_STOP 296
#define BX_TOKEN_RESET 297
#define BX_TOKEN_PRINT 298
#define BX_TOKEN_LOADER 299
#define BX_TOKEN_STRING 300
#define BX_TOKEN_DOIT 301
#define BX_TOKEN_CRC 302
#define BX_TOKEN_TRACEON 303
#define BX_TOKEN_TRACEOFF 304
#define BX_TOKEN_PTIME 305
#define BX_TOKEN_TIMEBP_ABSOLUTE 306
#define BX_TOKEN_TIMEBP 307
#define BX_TOKEN_RECORD 308
#define BX_TOKEN_PLAYBACK 309
#define BX_TOKEN_MODEBP 310
#define BX_TOKEN_PRINT_STACK 311
#define BX_TOKEN_WATCH 312
#define BX_TOKEN_UNWATCH 313
#define BX_TOKEN_READ 314
#define BX_TOKEN_WRITE 315
#define BX_TOKEN_SHOW 316
#define BX_TOKEN_SYMBOL 317
#define BX_TOKEN_GLOBAL 318
#define BX_TOKEN_WHERE 319
#define BX_TOKEN_PRINT_STRING 320
#define BX_TOKEN_DIFF_MEMORY 321
#define BX_TOKEN_SYNC_MEMORY 322
#define BX_TOKEN_SYNC_CPU 323
#define BX_TOKEN_FAST_FORWARD 324
#define BX_TOKEN_PHY_2_LOG 325
#define BX_TOKEN_NUMERIC 326
#define BX_TOKEN_LONG_NUMERIC 327
#define BX_TOKEN_INFO_ADDRESS 328
#define BX_TOKEN_NE2000 329
#define BX_TOKEN_PAGE 330
#define BX_TOKEN_CS 331
#define BX_TOKEN_ES 332
#define BX_TOKEN_SS 333
#define BX_TOKEN_DS 334
#define BX_TOKEN_FS 335
#define BX_TOKEN_GS 336
#define BX_TOKEN_ALWAYS_CHECK 337
#define BX_TOKEN_MATHS 338
#define BX_TOKEN_ADD 339
#define BX_TOKEN_SUB 340
#define BX_TOKEN_MUL 341
#define BX_TOKEN_DIV 342
#define BX_TOKEN_V2L 343
#define BX_TOKEN_TRACEREGON 344
#define BX_TOKEN_TRACEREGOFF 345
#define BX_TOKEN_HELP 346
#define BX_TOKEN_IVT 347
const short bxlhs[] = {                                        -1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   37,
   37,   37,   37,   37,   37,   37,   37,   37,   37,    1,
    1,    1,    1,    1,    1,   27,   27,   28,   29,   30,
   30,   33,   33,   26,   24,   25,   31,   31,   32,   32,
   32,   32,   32,   32,   32,   32,   34,   34,   34,   35,
   36,    5,    6,    6,    7,    7,    7,    8,    8,    8,
    8,    8,    8,    8,    9,    9,    9,    9,    9,    9,
    9,    9,    9,    9,    9,    9,    9,    9,    9,    9,
    2,    2,    4,    4,    3,    3,    3,   10,   11,   12,
   13,   13,   13,   13,   14,   15,   16,   16,   16,   17,
   18,   19,   19,   19,   19,   20,   21,   22,   23,   23,
   23,   23,   23,   38,   39,   40,   41,   41,
};
const short bxlen[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    0,    1,    2,
    3,    3,    3,    3,    3,    3,    5,    4,    4,    1,
    1,    1,    1,    1,    1,    3,    3,    3,    3,    3,
    2,    3,    2,    2,    2,    2,    2,    3,    3,    3,
    2,    2,    4,    4,    4,    4,    3,    4,    5,    2,
    3,    2,    2,    3,    4,    4,    5,    2,    5,    2,
    3,    2,    3,    4,    3,    3,    3,    3,    3,    3,
    4,    4,    4,    4,    4,    3,    3,    3,    5,    7,
    0,    1,    0,    1,    1,    2,    3,    2,    3,    2,
    4,    3,    3,    2,    5,    3,    3,    4,    3,    2,
    3,    3,    3,    3,    3,    3,    3,    4,    5,    5,
    5,    5,    3,    5,    2,    2,    3,    2,
};
const short bxdefred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   39,    0,    1,
    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
   12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
   22,   23,   24,   25,   26,   27,   28,   29,   30,   31,
   32,   33,   34,   35,   36,   37,   82,    0,   83,    0,
    0,    0,   88,    0,   90,    0,   92,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  120,    0,    0,  124,    0,    0,    0,
    0,  118,  130,    0,  114,    0,    0,    0,    0,    0,
    0,    0,    0,   65,   66,   64,    0,    0,    0,    0,
    0,   61,    0,   67,    0,    0,    0,    0,   71,    0,
    0,   72,    0,   63,    0,    0,   80,    0,   40,    0,
    0,    0,    0,    0,    0,   50,   51,   52,   53,   54,
   55,    0,    0,    0,    0,    0,    0,    0,    0,  145,
  146,    0,  148,   84,    0,    0,    0,    0,   91,   93,
    0,  119,   95,   96,   97,   98,   99,    0,    0,    0,
    0,  100,  106,  107,    0,  108,    0,    0,  122,  123,
    0,  126,    0,  127,  129,  116,    0,  131,  132,  133,
  134,  135,  136,  137,    0,   57,   56,   58,   59,   60,
   68,   70,   69,    0,    0,    0,    0,   62,    0,   77,
    0,   81,   41,   42,   43,   44,   45,   46,    0,    0,
    0,  143,    0,    0,    0,    0,    0,  147,    0,   85,
   86,    0,   94,  101,  103,  104,  105,    0,  102,  121,
    0,  128,  117,  138,   73,   75,   74,   76,   78,    0,
    0,   48,   49,    0,    0,    0,    0,    0,   87,   89,
    0,  109,  125,   79,   47,  139,  140,  141,  142,  144,
    0,  110,
};
const short bxdgoto[] = {                                      49,
  172,    0,  125,  126,   50,   51,   52,   53,   54,   55,
   56,   57,   58,   59,   60,   61,   62,   63,   64,   65,
   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,
   76,   77,   78,   79,   80,   81,   82,   83,   84,   85,
   86,
};
const short bxsindex[] = {                                    -10,
    1,   -4, -246,   -3,   -2,   -7, -309, -248,   28,   -6,
 -285, -213, -241,   33,   38, -254, -236, -227, -252, -251,
   66,   67,   68, -247, -245, -221, -217,    8,   -1,   30,
    2,   32, -281,   74, -240,   75, -228, -226, -239, -238,
 -278, -237, -290, -278,   80,   81,   37,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   82,    0,   34,
 -224,   35,    0,   84,    0,   86,    0, -229,   88,   90,
   91,   92,   93,   94, -254, -254, -254, -254,   95,   96,
   97,   -9, -254,    0,    3,   98,    0, -216,   99,    4,
  101,    0,    0,  -56,    0,  102,  103,  104,  105,  106,
  107,  108, -207,    0,    0,    0,  110,  111,  112,  113,
  114,    0,  115,    0,  116,  117, -198, -197,    0, -196,
 -195,    0,  122,    0,    5, -167,    0,  124,    0,  125,
  126,  127,  128,  129,  130,    0,    0,    0,    0,    0,
    0,   83, -222,  132, -183, -182, -181, -180,   89,    0,
    0,  138,    0,    0, -177,  140,  141, -174,    0,    0,
  143,    0,    0,    0,    0,    0,    0,  144,  145,  146,
  147,    0,    0,    0, -168,    0,  149,  150,    0,    0,
 -165,    0,  152,    0,    0,    0, -163,    0,    0,    0,
    0,    0,    0,    0,  154,    0,    0,    0,    0,    0,
    0,    0,    0,  155,  156,  157,  158,    0,  159,    0,
 -156,    0,    0,    0,    0,    0,    0,    0, -155,  162,
  163,    0, -152, -151, -150, -149, -148,    0,  169,    0,
    0,  170,    0,    0,    0,    0,    0,   -5,    0,    0,
  171,    0,    0,    0,    0,    0,    0,    0,    0,  172,
  173,    0,    0,  174,  175,  176,  177,  178,    0,    0,
 -137,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  180,    0,
};
const short bxrindex[] = {                                    191,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  182,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  182,  182,  182,  182,    0,    0,
    0,    0,  182,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  183,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,
};
const short bxgindex[] = {                                      0,
  151,    0,    0,  -74,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,
};
#define YYTABLESIZE 345
const short bxtable[] = {                                      48,
  206,  217,   97,  117,  292,   89,   93,   95,  144,  174,
   87,  152,  209,  214,  240,  100,   99,  142,  155,  101,
  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,
  198,  199,  200,  201,   98,   90,  156,  114,  207,  149,
  118,  154,  122,   91,  120,  121,  183,  123,  175,  176,
  177,  178,  166,  167,  168,  169,  170,  171,  127,  128,
  129,  130,  160,  161,  162,  163,  186,  187,  250,  251,
  119,  124,  131,  132,  133,  134,  135,  136,  139,  137,
  112,  138,  140,  157,  159,  158,  164,  165,  173,  180,
  181,  184,  188,  189,  185,  190,  191,  192,  113,  193,
  194,  195,  196,  197,  202,  203,  204,  210,  212,  211,
  215,  218,  219,  220,  221,  222,  223,  224,  225,  226,
  227,  228,  229,  230,  231,  232,  233,  234,  235,  236,
  237,  238,  241,  242,  243,  244,  245,  246,  247,  248,
  249,  252,  253,  254,  255,  256,  257,  258,  259,  260,
  261,  262,  263,  264,  265,  266,  267,  268,  269,  270,
  271,  272,  273,  274,  275,  276,  277,  278,  279,  280,
  281,  282,  283,  284,  285,  286,  287,  288,  289,  290,
  293,  294,  295,  296,  297,  298,  299,  300,  301,  302,
   38,  113,  115,    0,  179,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    1,    2,    0,    3,
    0,    4,    5,    6,    7,    8,    9,    0,    0,    0,
    0,    0,    0,  291,    0,    0,    0,    0,   10,  216,
   11,    0,   12,  115,   13,    0,    0,   14,   15,    0,
    0,    0,   16,   17,    0,    0,  145,    0,   18,    0,
   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,
   29,   30,   31,    0,    0,   32,   33,  141,   34,   35,
   36,   37,   38,   39,   40,  150,  151,   41,   96,  116,
  205,   88,   92,   94,  143,  146,   42,   43,  208,  213,
  239,  153,   44,   45,   46,   47,  182,    0,    0,    0,
    0,    0,    0,  147,  148,
};
const short bxcheck[] = {                                      10,
   10,   58,   10,   10,   10,   10,   10,   10,   10,  300,
   10,   10,   10,   10,   10,  264,  326,   10,  300,  268,
  269,  270,  271,  272,  273,  274,  275,  276,  277,  278,
  105,  106,  107,  108,   42,  282,  318,   10,  113,   10,
  326,   10,   10,  290,  286,  287,   10,   10,  339,  340,
  341,  342,  331,  332,  333,  334,  335,  336,  295,  296,
  297,  298,  291,  292,  291,  292,  291,  292,  291,  292,
  284,  326,  300,  326,  326,   10,   10,   10,  300,  327,
  329,  327,  300,   10,   10,  326,  326,  326,  326,   10,
   10,   10,   58,   10,   61,   10,  326,   10,  347,   10,
   10,   10,   10,   10,   10,   10,   10,   10,   10,  326,
   10,   10,   10,   10,   10,   10,   10,   10,  326,   10,
   10,   10,   10,   10,   10,   10,   10,  326,  326,  326,
  326,   10,  300,   10,   10,   10,   10,   10,   10,   10,
   58,   10,  326,  326,  326,  326,   58,   10,  326,   10,
   10,  326,   10,   10,   10,   10,   10,  326,   10,   10,
  326,   10,  326,   10,   10,   10,   10,   10,   10,  326,
  326,   10,   10,  326,  326,  326,  326,  326,   10,   10,
   10,   10,   10,   10,   10,   10,   10,   10,  326,   10,
    0,   10,   10,   -1,   44,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,   -1,  260,
   -1,  262,  263,  264,  265,  266,  267,   -1,   -1,   -1,
   -1,   -1,   -1,  269,   -1,   -1,   -1,   -1,  279,  326,
  281,   -1,  283,  280,  285,   -1,   -1,  288,  289,   -1,
   -1,   -1,  293,  294,   -1,   -1,  257,   -1,  299,   -1,
  301,  302,  303,  304,  305,  306,  307,  308,  309,  310,
  311,  312,  313,   -1,   -1,  316,  317,  300,  319,  320,
  321,  322,  323,  324,  325,  314,  315,  328,  326,  326,
  330,  326,  326,  326,  326,  296,  337,  338,  326,  326,
  326,  300,  343,  344,  345,  346,  300,   -1,   -1,   -1,
   -1,   -1,   -1,  314,  315,
};
#define YYFINAL 49
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 347
#if YYDEBUG
const char * const bxname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,"'\\n'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,"'*'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"':'",0,0,"'='",0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"BX_TOKEN_CONTINUE","BX_TOKEN_STEPN","BX_TOKEN_NEXT_STEP","BX_TOKEN_SET",
"BX_TOKEN_DEBUGGER","BX_TOKEN_VBREAKPOINT","BX_TOKEN_LBREAKPOINT",
"BX_TOKEN_PBREAKPOINT","BX_TOKEN_DEL_BREAKPOINT","BX_TOKEN_INFO",
"BX_TOKEN_QUIT","BX_TOKEN_PROGRAM","BX_TOKEN_REGISTERS","BX_TOKEN_FPU",
"BX_TOKEN_ALL","BX_TOKEN_IDT","BX_TOKEN_GDT","BX_TOKEN_LDT","BX_TOKEN_TSS",
"BX_TOKEN_DIRTY","BX_TOKEN_LINUX","BX_TOKEN_CONTROL_REGS","BX_TOKEN_EXAMINE",
"BX_TOKEN_XFORMAT","BX_TOKEN_SETPMEM","BX_TOKEN_SYMBOLNAME","BX_TOKEN_QUERY",
"BX_TOKEN_PENDING","BX_TOKEN_TAKE","BX_TOKEN_DMA","BX_TOKEN_IRQ",
"BX_TOKEN_DUMP_CPU","BX_TOKEN_SET_CPU","BX_TOKEN_DIS","BX_TOKEN_ON",
"BX_TOKEN_OFF","BX_TOKEN_DISASSEMBLE","BX_TOKEN_INSTRUMENT","BX_TOKEN_START",
"BX_TOKEN_STOP","BX_TOKEN_RESET","BX_TOKEN_PRINT","BX_TOKEN_LOADER",
"BX_TOKEN_STRING","BX_TOKEN_DOIT","BX_TOKEN_CRC","BX_TOKEN_TRACEON",
"BX_TOKEN_TRACEOFF","BX_TOKEN_PTIME","BX_TOKEN_TIMEBP_ABSOLUTE",
"BX_TOKEN_TIMEBP","BX_TOKEN_RECORD","BX_TOKEN_PLAYBACK","BX_TOKEN_MODEBP",
"BX_TOKEN_PRINT_STACK","BX_TOKEN_WATCH","BX_TOKEN_UNWATCH","BX_TOKEN_READ",
"BX_TOKEN_WRITE","BX_TOKEN_SHOW","BX_TOKEN_SYMBOL","BX_TOKEN_GLOBAL",
"BX_TOKEN_WHERE","BX_TOKEN_PRINT_STRING","BX_TOKEN_DIFF_MEMORY",
"BX_TOKEN_SYNC_MEMORY","BX_TOKEN_SYNC_CPU","BX_TOKEN_FAST_FORWARD",
"BX_TOKEN_PHY_2_LOG","BX_TOKEN_NUMERIC","BX_TOKEN_LONG_NUMERIC",
"BX_TOKEN_INFO_ADDRESS","BX_TOKEN_NE2000","BX_TOKEN_PAGE","BX_TOKEN_CS",
"BX_TOKEN_ES","BX_TOKEN_SS","BX_TOKEN_DS","BX_TOKEN_FS","BX_TOKEN_GS",
"BX_TOKEN_ALWAYS_CHECK","BX_TOKEN_MATHS","BX_TOKEN_ADD","BX_TOKEN_SUB",
"BX_TOKEN_MUL","BX_TOKEN_DIV","BX_TOKEN_V2L","BX_TOKEN_TRACEREGON",
"BX_TOKEN_TRACEREGOFF","BX_TOKEN_HELP","BX_TOKEN_IVT",
};
const char * const bxrule[] = {
"$accept : command",
"command : continue_command",
"command : stepN_command",
"command : set_command",
"command : breakpoint_command",
"command : info_command",
"command : dump_cpu_command",
"command : delete_command",
"command : quit_command",
"command : examine_command",
"command : setpmem_command",
"command : query_command",
"command : take_command",
"command : set_cpu_command",
"command : disassemble_command",
"command : instrument_command",
"command : loader_command",
"command : doit_command",
"command : crc_command",
"command : maths_command",
"command : trace_on_command",
"command : trace_off_command",
"command : ptime_command",
"command : timebp_command",
"command : record_command",
"command : playback_command",
"command : modebp_command",
"command : print_stack_command",
"command : watch_point_command",
"command : show_command",
"command : symbol_command",
"command : where_command",
"command : print_string_command",
"command : cosim_commands",
"command : v2l_command",
"command : trace_reg_on_command",
"command : trace_reg_off_command",
"command : help_command",
"command :",
"command : '\\n'",
"cosim_commands : BX_TOKEN_DIFF_MEMORY '\\n'",
"cosim_commands : BX_TOKEN_SYNC_MEMORY BX_TOKEN_ON '\\n'",
"cosim_commands : BX_TOKEN_SYNC_MEMORY BX_TOKEN_OFF '\\n'",
"cosim_commands : BX_TOKEN_SYNC_CPU BX_TOKEN_ON '\\n'",
"cosim_commands : BX_TOKEN_SYNC_CPU BX_TOKEN_OFF '\\n'",
"cosim_commands : BX_TOKEN_FAST_FORWARD BX_TOKEN_NUMERIC '\\n'",
"cosim_commands : BX_TOKEN_PHY_2_LOG BX_TOKEN_NUMERIC '\\n'",
"cosim_commands : BX_TOKEN_INFO_ADDRESS segment_register ':' BX_TOKEN_NUMERIC '\\n'",
"cosim_commands : BX_TOKEN_ALWAYS_CHECK BX_TOKEN_NUMERIC BX_TOKEN_ON '\\n'",
"cosim_commands : BX_TOKEN_ALWAYS_CHECK BX_TOKEN_NUMERIC BX_TOKEN_OFF '\\n'",
"segment_register : BX_TOKEN_CS",
"segment_register : BX_TOKEN_ES",
"segment_register : BX_TOKEN_SS",
"segment_register : BX_TOKEN_DS",
"segment_register : BX_TOKEN_FS",
"segment_register : BX_TOKEN_GS",
"timebp_command : BX_TOKEN_TIMEBP BX_TOKEN_LONG_NUMERIC '\\n'",
"timebp_command : BX_TOKEN_TIMEBP_ABSOLUTE BX_TOKEN_LONG_NUMERIC '\\n'",
"record_command : BX_TOKEN_RECORD BX_TOKEN_STRING '\\n'",
"playback_command : BX_TOKEN_PLAYBACK BX_TOKEN_STRING '\\n'",
"modebp_command : BX_TOKEN_MODEBP BX_TOKEN_STRING '\\n'",
"modebp_command : BX_TOKEN_MODEBP '\\n'",
"show_command : BX_TOKEN_SHOW BX_TOKEN_STRING '\\n'",
"show_command : BX_TOKEN_SHOW '\\n'",
"ptime_command : BX_TOKEN_PTIME '\\n'",
"trace_on_command : BX_TOKEN_TRACEON '\\n'",
"trace_off_command : BX_TOKEN_TRACEOFF '\\n'",
"print_stack_command : BX_TOKEN_PRINT_STACK '\\n'",
"print_stack_command : BX_TOKEN_PRINT_STACK BX_TOKEN_NUMERIC '\\n'",
"watch_point_command : BX_TOKEN_WATCH BX_TOKEN_STOP '\\n'",
"watch_point_command : BX_TOKEN_WATCH BX_TOKEN_CONTINUE '\\n'",
"watch_point_command : BX_TOKEN_WATCH '\\n'",
"watch_point_command : BX_TOKEN_UNWATCH '\\n'",
"watch_point_command : BX_TOKEN_WATCH BX_TOKEN_READ BX_TOKEN_NUMERIC '\\n'",
"watch_point_command : BX_TOKEN_UNWATCH BX_TOKEN_READ BX_TOKEN_NUMERIC '\\n'",
"watch_point_command : BX_TOKEN_WATCH BX_TOKEN_WRITE BX_TOKEN_NUMERIC '\\n'",
"watch_point_command : BX_TOKEN_UNWATCH BX_TOKEN_WRITE BX_TOKEN_NUMERIC '\\n'",
"symbol_command : BX_TOKEN_SYMBOL BX_TOKEN_STRING '\\n'",
"symbol_command : BX_TOKEN_SYMBOL BX_TOKEN_STRING BX_TOKEN_NUMERIC '\\n'",
"symbol_command : BX_TOKEN_SYMBOL BX_TOKEN_GLOBAL BX_TOKEN_STRING BX_TOKEN_NUMERIC '\\n'",
"where_command : BX_TOKEN_WHERE '\\n'",
"print_string_command : BX_TOKEN_PRINT_STRING BX_TOKEN_NUMERIC '\\n'",
"continue_command : BX_TOKEN_CONTINUE '\\n'",
"stepN_command : BX_TOKEN_STEPN '\\n'",
"stepN_command : BX_TOKEN_STEPN BX_TOKEN_NUMERIC '\\n'",
"set_command : BX_TOKEN_SET BX_TOKEN_DIS BX_TOKEN_ON '\\n'",
"set_command : BX_TOKEN_SET BX_TOKEN_DIS BX_TOKEN_OFF '\\n'",
"set_command : BX_TOKEN_SET BX_TOKEN_SYMBOLNAME '=' BX_TOKEN_NUMERIC '\\n'",
"breakpoint_command : BX_TOKEN_VBREAKPOINT '\\n'",
"breakpoint_command : BX_TOKEN_VBREAKPOINT BX_TOKEN_NUMERIC ':' BX_TOKEN_NUMERIC '\\n'",
"breakpoint_command : BX_TOKEN_LBREAKPOINT '\\n'",
"breakpoint_command : BX_TOKEN_LBREAKPOINT BX_TOKEN_NUMERIC '\\n'",
"breakpoint_command : BX_TOKEN_PBREAKPOINT '\\n'",
"breakpoint_command : BX_TOKEN_PBREAKPOINT BX_TOKEN_NUMERIC '\\n'",
"breakpoint_command : BX_TOKEN_PBREAKPOINT '*' BX_TOKEN_NUMERIC '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_PBREAKPOINT '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_PROGRAM '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_REGISTERS '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_FPU '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_ALL '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_DIRTY '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric_range '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_IVT optional_numeric_range '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_GDT optional_numeric_range '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_LDT optional_numeric_range '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_TSS optional_numeric_range '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_LINUX '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_CONTROL_REGS '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_NE2000 '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_NE2000 BX_TOKEN_PAGE BX_TOKEN_NUMERIC '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_NE2000 BX_TOKEN_PAGE BX_TOKEN_NUMERIC BX_TOKEN_REGISTERS BX_TOKEN_NUMERIC '\\n'",
"optional_numeric :",
"optional_numeric : BX_TOKEN_NUMERIC",
"optional_numeric_range :",
"optional_numeric_range : numeric_range",
"numeric_range : BX_TOKEN_NUMERIC",
"numeric_range : BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC",
"numeric_range : BX_TOKEN_NUMERIC ':' BX_TOKEN_NUMERIC",
"dump_cpu_command : BX_TOKEN_DUMP_CPU '\\n'",
"delete_command : BX_TOKEN_DEL_BREAKPOINT BX_TOKEN_NUMERIC '\\n'",
"quit_command : BX_TOKEN_QUIT '\\n'",
"examine_command : BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT BX_TOKEN_NUMERIC '\\n'",
"examine_command : BX_TOKEN_EXAMINE BX_TOKEN_XFORMAT '\\n'",
"examine_command : BX_TOKEN_EXAMINE BX_TOKEN_NUMERIC '\\n'",
"examine_command : BX_TOKEN_EXAMINE '\\n'",
"setpmem_command : BX_TOKEN_SETPMEM BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\\n'",
"query_command : BX_TOKEN_QUERY BX_TOKEN_PENDING '\\n'",
"take_command : BX_TOKEN_TAKE BX_TOKEN_DMA '\\n'",
"take_command : BX_TOKEN_TAKE BX_TOKEN_DMA BX_TOKEN_NUMERIC '\\n'",
"take_command : BX_TOKEN_TAKE BX_TOKEN_IRQ '\\n'",
"set_cpu_command : BX_TOKEN_SET_CPU '\\n'",
"disassemble_command : BX_TOKEN_DISASSEMBLE optional_numeric_range '\\n'",
"instrument_command : BX_TOKEN_INSTRUMENT BX_TOKEN_START '\\n'",
"instrument_command : BX_TOKEN_INSTRUMENT BX_TOKEN_STOP '\\n'",
"instrument_command : BX_TOKEN_INSTRUMENT BX_TOKEN_RESET '\\n'",
"instrument_command : BX_TOKEN_INSTRUMENT BX_TOKEN_PRINT '\\n'",
"loader_command : BX_TOKEN_LOADER BX_TOKEN_STRING '\\n'",
"doit_command : BX_TOKEN_DOIT BX_TOKEN_NUMERIC '\\n'",
"crc_command : BX_TOKEN_CRC BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\\n'",
"maths_command : BX_TOKEN_MATHS BX_TOKEN_ADD BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\\n'",
"maths_command : BX_TOKEN_MATHS BX_TOKEN_SUB BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\\n'",
"maths_command : BX_TOKEN_MATHS BX_TOKEN_MUL BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\\n'",
"maths_command : BX_TOKEN_MATHS BX_TOKEN_DIV BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\\n'",
"maths_command : BX_TOKEN_MATHS BX_TOKEN_STRING '\\n'",
"v2l_command : BX_TOKEN_V2L segment_register ':' BX_TOKEN_NUMERIC '\\n'",
"trace_reg_on_command : BX_TOKEN_TRACEREGON '\\n'",
"trace_reg_off_command : BX_TOKEN_TRACEREGOFF '\\n'",
"help_command : BX_TOKEN_HELP BX_TOKEN_STRING '\\n'",
"help_command : BX_TOKEN_HELP '\\n'",
};
#endif
#if YYDEBUG
#include <stdio.h>
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif
#define YYINITSTACKSIZE 200
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short *yyss;
short *yysslim;
YYSTYPE *yyvs;
int yystacksize;
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack()
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;
    i = yyssp - yyss;
    newss = yyss ? (short *)realloc(yyss, newsize * sizeof *newss) :
      (short *)malloc(newsize * sizeof *newss);
    if (newss == NULL)
        return -1;
    yyss = newss;
    yyssp = newss + i;
    newvs = yyvs ? (YYSTYPE *)realloc(yyvs, newsize * sizeof *newvs) :
      (YYSTYPE *)malloc(newsize * sizeof *newvs);
    if (newvs == NULL)
        return -1;
    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab

#ifndef YYPARSE_PARAM
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG void
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif	/* ANSI-C/C++ */
#else	/* YYPARSE_PARAM */
#ifndef YYPARSE_PARAM_TYPE
#define YYPARSE_PARAM_TYPE void *
#endif
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG YYPARSE_PARAM_TYPE YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL YYPARSE_PARAM_TYPE YYPARSE_PARAM;
#endif	/* ANSI-C/C++ */
#endif	/* ! YYPARSE_PARAM */

int
yyparse (YYPARSE_PARAM_ARG)
    YYPARSE_PARAM_DECL
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register const char *yys;

    if ((yys = getenv("YYDEBUG")))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate])) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#if defined(lint) || defined(__GNUC__)
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#if defined(lint) || defined(__GNUC__)
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 39:
#line 168 "parser.y"
{
      }
break;
case 40:
#line 174 "parser.y"
{
		bx_dbg_diff_memory();
		free(yyvsp[-1].sval);
	}
break;
case 41:
#line 179 "parser.y"
{
		bx_dbg_sync_memory(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 42:
#line 184 "parser.y"
{
		bx_dbg_sync_memory(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 43:
#line 189 "parser.y"
{
		bx_dbg_sync_cpu(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 44:
#line 194 "parser.y"
{
		bx_dbg_sync_cpu(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 45:
#line 199 "parser.y"
{
		free(yyvsp[-2].sval);
		bx_dbg_fast_forward(yyvsp[-1].uval);
	}
break;
case 46:
#line 204 "parser.y"
{
	}
break;
case 47:
#line 207 "parser.y"
{
		free(yyvsp[-4].sval);
		bx_dbg_info_address(yyvsp[-3].uval, yyvsp[-1].uval);
        }
break;
case 48:
#line 212 "parser.y"
{
        }
break;
case 49:
#line 215 "parser.y"
{
        }
break;
case 50:
#line 220 "parser.y"
{ yyval.uval = 1; }
break;
case 51:
#line 221 "parser.y"
{ yyval.uval = 0; }
break;
case 52:
#line 222 "parser.y"
{ yyval.uval = 2; }
break;
case 53:
#line 223 "parser.y"
{ yyval.uval = 3; }
break;
case 54:
#line 224 "parser.y"
{ yyval.uval = 4; }
break;
case 55:
#line 225 "parser.y"
{ yyval.uval = 5; }
break;
case 56:
#line 230 "parser.y"
{
        bx_dbg_timebp_command(0, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
break;
case 57:
#line 235 "parser.y"
{
        bx_dbg_timebp_command(1, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
break;
case 58:
#line 243 "parser.y"
{
          bx_dbg_record_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 59:
#line 251 "parser.y"
{
          bx_dbg_playback_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 60:
#line 259 "parser.y"
{
          bx_dbg_modebp_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 61:
#line 264 "parser.y"
{
          bx_dbg_modebp_command(0);
          free(yyvsp[-1].sval);
          }
break;
case 62:
#line 272 "parser.y"
{
          bx_dbg_show_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 63:
#line 277 "parser.y"
{
          bx_dbg_show_command(0);
          free(yyvsp[-1].sval);
          }
break;
case 64:
#line 285 "parser.y"
{
        bx_dbg_ptime_command();
        free(yyvsp[-1].sval);
	}
break;
case 65:
#line 293 "parser.y"
{
        bx_dbg_trace_on_command();
        free(yyvsp[-1].sval);
	}
break;
case 66:
#line 301 "parser.y"
{
        bx_dbg_trace_off_command();
        free(yyvsp[-1].sval);
	}
break;
case 67:
#line 309 "parser.y"
{
          bx_dbg_print_stack_command(16);
          free(yyvsp[-1].sval);
	  }
break;
case 68:
#line 314 "parser.y"
{
          bx_dbg_print_stack_command(yyvsp[-1].uval);
          free(yyvsp[-2].sval);
	  }
break;
case 69:
#line 322 "parser.y"
{
          watchpoint_continue = 0;
	  fprintf(stderr, "Will stop on watch points\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 70:
#line 328 "parser.y"
{
          watchpoint_continue = 1;
          fprintf(stderr, "Will not stop on watch points (they will still be logged)\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 71:
#line 334 "parser.y"
{
          bx_dbg_watch(-1, 0);
          free(yyvsp[-1].sval);
          }
break;
case 72:
#line 339 "parser.y"
{
          bx_dbg_unwatch(-1, 0);
          free(yyvsp[-1].sval);
          }
break;
case 73:
#line 344 "parser.y"
{
          bx_dbg_watch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 74:
#line 349 "parser.y"
{
          bx_dbg_unwatch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 75:
#line 354 "parser.y"
{
          bx_dbg_watch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 76:
#line 359 "parser.y"
{
          bx_dbg_unwatch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 77:
#line 367 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-1].sval, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 78:
#line 372 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-2].sval, 0, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 79:
#line 377 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-2].sval, 1, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 80:
#line 385 "parser.y"
{
        bx_dbg_where_command();
        free(yyvsp[-1].sval);
        }
break;
case 81:
#line 393 "parser.y"
{
        bx_dbg_print_string_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 82:
#line 401 "parser.y"
{
        bx_dbg_continue_command();
        free(yyvsp[-1].sval);
        }
break;
case 83:
#line 409 "parser.y"
{
        bx_dbg_stepN_command(1);
        free(yyvsp[-1].sval);
        }
break;
case 84:
#line 414 "parser.y"
{
        bx_dbg_stepN_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 85:
#line 422 "parser.y"
{
        bx_dbg_set_command(yyvsp[-3].sval, yyvsp[-2].sval, yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 86:
#line 427 "parser.y"
{
        bx_dbg_set_command(yyvsp[-3].sval, yyvsp[-2].sval, yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 87:
#line 432 "parser.y"
{
        bx_dbg_set_symbol_command(yyvsp[-3].sval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 88:
#line 440 "parser.y"
{
        bx_dbg_vbreakpoint_command(0, 0, 0);
        free(yyvsp[-1].sval);
        }
break;
case 89:
#line 445 "parser.y"
{
        bx_dbg_vbreakpoint_command(1, yyvsp[-3].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
break;
case 90:
#line 450 "parser.y"
{
        bx_dbg_lbreakpoint_command(0, 0);
        free(yyvsp[-1].sval);
        }
break;
case 91:
#line 455 "parser.y"
{
        bx_dbg_lbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 92:
#line 460 "parser.y"
{
        bx_dbg_pbreakpoint_command(0, 0);
        free(yyvsp[-1].sval);
        }
break;
case 93:
#line 465 "parser.y"
{
        bx_dbg_pbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 94:
#line 470 "parser.y"
{
        bx_dbg_pbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
break;
case 95:
#line 478 "parser.y"
{
        bx_dbg_info_bpoints_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 96:
#line 483 "parser.y"
{
        bx_dbg_info_program_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 97:
#line 488 "parser.y"
{
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 98:
#line 493 "parser.y"
{
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 99:
#line 498 "parser.y"
{
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS | BX_INFO_FPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 100:
#line 503 "parser.y"
{
        bx_dbg_info_dirty_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 101:
#line 508 "parser.y"
{
        bx_dbg_info_idt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 102:
#line 513 "parser.y"
{
       bx_dbg_info_ivt_command(yyvsp[-1].uval_range);
       free(yyvsp[-3].sval); free(yyvsp[-2].sval);
       }
break;
case 103:
#line 518 "parser.y"
{
        bx_dbg_info_gdt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 104:
#line 523 "parser.y"
{
        bx_dbg_info_ldt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 105:
#line 528 "parser.y"
{
        bx_dbg_info_tss_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 106:
#line 533 "parser.y"
{
        bx_dbg_info_linux_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 107:
#line 538 "parser.y"
{
        bx_dbg_info_control_regs_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 108:
#line 543 "parser.y"
{
        bx_dbg_info_ne2k(-1, -1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 109:
#line 548 "parser.y"
{
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-1].uval, -1);
        }
break;
case 110:
#line 553 "parser.y"
{
        free(yyvsp[-6].sval); free(yyvsp[-5].sval); free(yyvsp[-4].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-3].uval, yyvsp[-1].uval);
        }
break;
case 111:
#line 560 "parser.y"
{ yyval.uval = EMPTY_ARG; }
break;
case 113:
#line 564 "parser.y"
{ yyval.uval_range = make_num_range (EMPTY_ARG, EMPTY_ARG); }
break;
case 115:
#line 569 "parser.y"
{
    yyval.uval_range = make_num_range (yyvsp[0].uval, yyvsp[0].uval);
  }
break;
case 116:
#line 574 "parser.y"
{
    yyval.uval_range = make_num_range (yyvsp[-1].uval, yyvsp[0].uval);
  }
break;
case 117:
#line 579 "parser.y"
{
    yyval.uval_range = make_num_range (yyvsp[-2].uval, yyvsp[0].uval);
  }
break;
case 118:
#line 586 "parser.y"
{
        bx_dbg_dump_cpu_command();
        free(yyvsp[-1].sval);
        }
break;
case 119:
#line 594 "parser.y"
{
        bx_dbg_del_breakpoint_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 120:
#line 602 "parser.y"
{
	  bx_dbg_quit_command();
	  free(yyvsp[-1].sval);
        }
break;
case 121:
#line 611 "parser.y"
{
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].uval,1, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].uval,1, 1);
#endif
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 122:
#line 619 "parser.y"
{
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 1);
#endif
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 123:
#line 627 "parser.y"
{
        /*FIXME HanishKVC This method of hunting thro all the */
        /*      simulators may be better than using 2 calls if */
        /*      BX_NUM_SIMULATORS greater than or equal to 2 as */
        /*      done for other cases of BX_TOKEN_EXAMINE*/
        int iCurSim; 
        for(iCurSim = 0; iCurSim < BX_NUM_SIMULATORS; iCurSim++)
        {
          bx_dbg_examine_command(yyvsp[-2].sval, NULL,0, yyvsp[-1].uval,1, iCurSim);
        }
        free(yyvsp[-2].sval);
        }
break;
case 124:
#line 640 "parser.y"
{
        /*FIXME HanishKVC This method of hunting thro all the */
        /*      simulators may be better than using 2 calls if */
        /*      BX_NUM_SIMULATORS greater than or equal to 2 as */
        /*      done for other cases of BX_TOKEN_EXAMINE*/
        int iCurSim; 
        for(iCurSim = 0; iCurSim < BX_NUM_SIMULATORS; iCurSim++)
        {
          bx_dbg_examine_command(yyvsp[-1].sval, NULL,0, 0,0, iCurSim);
        }
        free(yyvsp[-1].sval);
        }
break;
case 125:
#line 656 "parser.y"
{
        bx_dbg_setpmem_command(yyvsp[-3].uval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
break;
case 126:
#line 664 "parser.y"
{
        bx_dbg_query_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 127:
#line 672 "parser.y"
{
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 128:
#line 677 "parser.y"
{
        bx_dbg_take_command(yyvsp[-2].sval, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 129:
#line 682 "parser.y"
{
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 130:
#line 690 "parser.y"
{
        bx_dbg_set_cpu_command();
        free(yyvsp[-1].sval);
        }
break;
case 131:
#line 698 "parser.y"
{
        bx_dbg_disassemble_command(yyvsp[-1].uval_range);
        free(yyvsp[-2].sval);
        }
break;
case 132:
#line 706 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 133:
#line 711 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 134:
#line 716 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 135:
#line 721 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 136:
#line 729 "parser.y"
{
        bx_dbg_loader_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 137:
#line 737 "parser.y"
{
        bx_dbg_doit_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 138:
#line 745 "parser.y"
{
        bx_dbg_crc_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
break;
case 139:
#line 753 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 140:
#line 758 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 141:
#line 763 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 142:
#line 768 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 143:
#line 773 "parser.y"
{
        bx_dbg_maths_expression_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 144:
#line 780 "parser.y"
{
        bx_dbg_v2l_command(yyvsp[-3].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
break;
case 145:
#line 788 "parser.y"
{
	bx_dbg_trace_reg_on_command();
	free(yyvsp[-1].sval);
	}
break;
case 146:
#line 796 "parser.y"
{
	bx_dbg_trace_reg_off_command();
	free(yyvsp[-1].sval);
	}
break;
case 147:
#line 804 "parser.y"
{
         bx_dbg_help_command(yyvsp[-1].sval);
         free(yyvsp[-2].sval);free(yyvsp[-1].sval);
         }
break;
case 148:
#line 809 "parser.y"
{
         bx_dbg_help_command(0);
         free(yyvsp[-1].sval);
         }
break;
#line 1537 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
#endif  /* if BX_DEBUGGER */
/* The #endif is appended by the makefile after running yacc. */
