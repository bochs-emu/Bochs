#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
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
#define YYPREFIX "bx"
#line 2 "parser.y"
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
/* %left '-' '+'*/
/* %left '*' '/'*/
/* %right*/
/* %nonassoc UMINUS*/
#line 12 "parser.y"
typedef union {
  char    *sval;
  Bit32u   uval;
  Bit64u   ulval;
  bx_num_range   uval_range;
  } YYSTYPE;
#line 51 "y.tab.c"
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
#define BX_TOKEN_IDT 270
#define BX_TOKEN_GDT 271
#define BX_TOKEN_LDT 272
#define BX_TOKEN_TSS 273
#define BX_TOKEN_DIRTY 274
#define BX_TOKEN_LINUX 275
#define BX_TOKEN_CONTROL_REGS 276
#define BX_TOKEN_EXAMINE 277
#define BX_TOKEN_XFORMAT 278
#define BX_TOKEN_SETPMEM 279
#define BX_TOKEN_SYMBOLNAME 280
#define BX_TOKEN_QUERY 281
#define BX_TOKEN_PENDING 282
#define BX_TOKEN_TAKE 283
#define BX_TOKEN_DMA 284
#define BX_TOKEN_IRQ 285
#define BX_TOKEN_DUMP_CPU 286
#define BX_TOKEN_SET_CPU 287
#define BX_TOKEN_DIS 288
#define BX_TOKEN_ON 289
#define BX_TOKEN_OFF 290
#define BX_TOKEN_DISASSEMBLE 291
#define BX_TOKEN_INSTRUMENT 292
#define BX_TOKEN_START 293
#define BX_TOKEN_STOP 294
#define BX_TOKEN_RESET 295
#define BX_TOKEN_PRINT 296
#define BX_TOKEN_LOADER 297
#define BX_TOKEN_STRING 298
#define BX_TOKEN_DOIT 299
#define BX_TOKEN_CRC 300
#define BX_TOKEN_TRACEON 301
#define BX_TOKEN_TRACEOFF 302
#define BX_TOKEN_PTIME 303
#define BX_TOKEN_TIMEBP_ABSOLUTE 304
#define BX_TOKEN_TIMEBP 305
#define BX_TOKEN_RECORD 306
#define BX_TOKEN_PLAYBACK 307
#define BX_TOKEN_MODEBP 308
#define BX_TOKEN_PRINT_STACK 309
#define BX_TOKEN_WATCH 310
#define BX_TOKEN_UNWATCH 311
#define BX_TOKEN_READ 312
#define BX_TOKEN_WRITE 313
#define BX_TOKEN_SHOW 314
#define BX_TOKEN_SYMBOL 315
#define BX_TOKEN_GLOBAL 316
#define BX_TOKEN_WHERE 317
#define BX_TOKEN_PRINT_STRING 318
#define BX_TOKEN_DIFF_MEMORY 319
#define BX_TOKEN_SYNC_MEMORY 320
#define BX_TOKEN_SYNC_CPU 321
#define BX_TOKEN_FAST_FORWARD 322
#define BX_TOKEN_PHY_2_LOG 323
#define BX_TOKEN_NUMERIC 324
#define BX_TOKEN_LONG_NUMERIC 325
#define BX_TOKEN_INFO_ADDRESS 326
#define BX_TOKEN_CS 327
#define BX_TOKEN_ES 328
#define BX_TOKEN_SS 329
#define BX_TOKEN_DS 330
#define BX_TOKEN_FS 331
#define BX_TOKEN_GS 332
#define BX_TOKEN_ALWAYS_CHECK 333
#define BX_TOKEN_MATHS 334
#define BX_TOKEN_ADD 335
#define BX_TOKEN_SUB 336
#define BX_TOKEN_MUL 337
#define BX_TOKEN_DIV 338
#define BX_TOKEN_V2L 339
#define YYERRCODE 256
short bxlhs[] = {                                        -1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   37,   37,   37,   37,
   37,   37,   37,   37,   37,   37,    1,    1,    1,    1,
    1,    1,   27,   27,   28,   29,   30,   30,   33,   33,
   26,   24,   25,   31,   31,   32,   32,   32,   32,   32,
   32,   32,   32,   34,   34,   34,   35,   36,    5,    6,
    6,    7,    7,    7,    8,    8,    8,    8,    8,    8,
    8,    9,    9,    9,    9,    9,    9,    9,    9,    9,
    9,    2,    2,    4,    4,    3,    3,    3,   10,   11,
   12,   13,   13,   13,   13,   14,   15,   16,   16,   16,
   17,   18,   19,   19,   19,   19,   20,   21,   22,   23,
   23,   23,   23,   23,   38,
};
short bxlen[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    0,    1,    2,    3,    3,    3,
    3,    3,    3,    5,    4,    4,    1,    1,    1,    1,
    1,    1,    3,    3,    3,    3,    3,    2,    3,    2,
    2,    2,    2,    2,    3,    3,    3,    2,    2,    4,
    4,    4,    4,    3,    4,    5,    2,    3,    2,    2,
    3,    4,    4,    5,    2,    5,    2,    3,    2,    3,
    4,    3,    3,    3,    3,    4,    4,    4,    4,    3,
    3,    0,    1,    0,    1,    1,    2,    3,    2,    3,
    2,    4,    3,    3,    2,    5,    3,    3,    4,    3,
    2,    3,    3,    3,    3,    3,    3,    3,    4,    5,
    5,    5,    5,    3,    5,
};
short bxdefred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   36,    0,    1,    2,    3,    4,
    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
   15,   16,   17,   18,   19,   20,   21,   22,   23,   24,
   25,   26,   27,   28,   29,   30,   31,   32,   33,   34,
   79,    0,   80,    0,    0,    0,   85,    0,   87,    0,
   89,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  111,    0,    0,  115,    0,    0,    0,
    0,  109,  121,    0,  105,    0,    0,    0,    0,    0,
    0,    0,    0,   62,   63,   61,    0,    0,    0,    0,
    0,   58,    0,   64,    0,    0,    0,    0,   68,    0,
    0,   69,    0,   60,    0,    0,   77,    0,   37,    0,
    0,    0,    0,    0,    0,   47,   48,   49,   50,   51,
   52,    0,    0,    0,    0,    0,    0,    0,    0,   81,
    0,    0,    0,    0,   88,   90,    0,  110,   92,   93,
   94,    0,    0,    0,    0,   95,  100,  101,    0,  113,
  114,    0,  117,    0,  118,  120,  107,    0,  122,  123,
  124,  125,  126,  127,  128,    0,   54,   53,   55,   56,
   57,   65,   67,   66,    0,    0,    0,    0,   59,    0,
   74,    0,   78,   38,   39,   40,   41,   42,   43,    0,
    0,    0,  134,    0,    0,    0,    0,    0,    0,   82,
   83,    0,   91,   96,   97,   98,   99,  112,    0,  119,
  108,  129,   70,   72,   71,   73,   75,    0,    0,   45,
   46,    0,    0,    0,    0,    0,   84,   86,  116,   76,
   44,  130,  131,  132,  133,  135,
};
short bxdgoto[] = {                                      46,
  162,    0,  115,  116,   47,   48,   49,   50,   51,   52,
   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,
   63,   64,   65,   66,   67,   68,   69,   70,   71,   72,
   73,   74,   75,   76,   77,   78,   79,   80,
};
short bxsindex[] = {                                    -10,
    4,   -7, -267,   -5,   -4,   -9, -302, -234,   19,   -6,
 -281, -238, -268,   35,   44, -257, -243, -232, -256, -255,
   60,   61,   62, -252, -251, -223, -222,    5,   -3,   45,
   15,    8, -288,   67, -246,   69, -270, -266, -244, -242,
 -271, -241, -289, -271,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   71,    0,   23, -263,   27,    0,   76,    0,   77,
    0, -236,   79,   80,   81,   82, -257, -257, -257, -257,
   83,   84,   85,    0,   -2,   86,    0, -227,   88,    1,
   89,    0,    0,  -56,    0,   90,   91,   92,   93,   94,
   95,   96, -217,    0,    0,    0,   98,   99,  100,  101,
  102,    0,  103,    0,  104,  105, -208, -207,    0, -206,
 -205,    0,  110,    0,    2, -177,    0,  112,    0,  113,
  114,  115,  116,  117,  118,    0,    0,    0,    0,    0,
    0,   72, -258,  119, -193, -192, -191, -190,   78,    0,
 -189,  127,  128, -185,    0,    0,  130,    0,    0,    0,
    0,  131,  132,  133,  134,    0,    0,    0,  135,    0,
    0, -178,    0,  137,    0,    0,    0, -176,    0,    0,
    0,    0,    0,    0,    0,  139,    0,    0,    0,    0,
    0,    0,    0,    0,  140,  141,  142,  143,    0,  144,
    0, -169,    0,    0,    0,    0,    0,    0,    0, -168,
  147,  148,    0, -165, -164, -163, -162, -161,  154,    0,
    0,  155,    0,    0,    0,    0,    0,    0,  156,    0,
    0,    0,    0,    0,    0,    0,    0,  157,  158,    0,
    0,  159,  160,  161,  162,  163,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,
};
short bxrindex[] = {                                    174,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   19,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   19,   19,   19,   19,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  165,    0,    0,    0,    0,    0,    0,
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
    0,    0,    0,    0,    0,    0,
};
short bxgindex[] = {                                      0,
  136,    0,    0,  -35,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 358
short bxtable[] = {                                      45,
   91,  198,   83,  107,   87,   89,  134,  190,  164,  145,
  195,  221,   84,   81,  132,  110,  111,  144,  150,  151,
   85,   93,  152,  153,  142,  172,  173,  146,  104,   94,
  231,  232,   92,   95,   96,   97,   98,   99,  100,  101,
  102,  103,  108,  109,  112,  165,  166,  167,  168,  117,
  118,  119,  120,  113,  139,  156,  157,  158,  159,  160,
  161,  182,  183,  184,  185,  121,  114,  122,  123,  124,
  125,  126,  127,  128,  129,  130,  147,  148,  149,  154,
  170,  155,  163,  171,  174,  175,  176,  177,  178,  179,
  180,  181,  186,  187,  188,  191,  192,  193,  196,  199,
  200,  201,  202,  203,  204,  205,  206,  207,  208,  209,
  210,  211,  212,  213,  214,  215,  216,  217,  218,  219,
  222,  223,  224,  225,  226,  227,  228,  229,  233,  230,
  234,  235,  236,  237,  239,  238,  240,  241,  242,  243,
  244,  245,  246,  247,  248,  249,  250,  251,  252,  253,
  254,  255,  256,  257,  258,  259,  260,  261,  262,  263,
  264,  265,  266,  267,  268,  269,  270,  271,  272,  273,
  274,  275,  276,   35,  106,    0,    0,    0,    0,  169,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    1,    2,    0,    3,
    0,    4,    5,    6,    7,    8,    9,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   10,  197,   11,    0,
   12,  105,   13,    0,    0,   14,   15,    0,    0,    0,
   16,   17,    0,    0,    0,    0,   18,    0,   19,   20,
   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,
   31,  135,  131,   32,   33,  143,   34,   35,   36,   37,
   38,   39,   40,    0,   90,   41,   82,  106,   86,   88,
  133,  189,   42,   43,  194,  220,  140,  141,   44,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  136,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  137,  138,
};
short bxcheck[] = {                                      10,
   10,   58,   10,   10,   10,   10,   10,   10,  298,  298,
   10,   10,  280,   10,   10,  284,  285,   10,  289,  290,
  288,  324,  289,  290,   10,  289,  290,  316,   10,  264,
  289,  290,   42,  268,  269,  270,  271,  272,  273,  274,
  275,  276,  324,  282,   10,  335,  336,  337,  338,  293,
  294,  295,  296,   10,   10,  327,  328,  329,  330,  331,
  332,   97,   98,   99,  100,  298,  324,  324,  324,   10,
   10,   10,  325,  325,  298,  298,   10,  324,   10,  324,
   10,  324,  324,   61,   58,   10,   10,  324,   10,   10,
   10,   10,   10,   10,   10,   10,  324,   10,   10,   10,
   10,   10,   10,   10,   10,   10,  324,   10,   10,   10,
   10,   10,   10,   10,   10,  324,  324,  324,  324,   10,
  298,   10,   10,   10,   10,   10,   10,   10,   10,   58,
  324,  324,  324,  324,  324,   58,   10,   10,  324,   10,
   10,   10,   10,   10,   10,  324,   10,  324,   10,   10,
   10,   10,   10,   10,  324,  324,   10,   10,  324,  324,
  324,  324,  324,   10,   10,   10,   10,   10,   10,   10,
   10,   10,   10,    0,   10,   -1,   -1,   -1,   -1,   44,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,   -1,  260,
   -1,  262,  263,  264,  265,  266,  267,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  277,  324,  279,   -1,
  281,  278,  283,   -1,   -1,  286,  287,   -1,   -1,   -1,
  291,  292,   -1,   -1,   -1,   -1,  297,   -1,  299,  300,
  301,  302,  303,  304,  305,  306,  307,  308,  309,  310,
  311,  257,  298,  314,  315,  298,  317,  318,  319,  320,
  321,  322,  323,   -1,  324,  326,  324,  324,  324,  324,
  324,  324,  333,  334,  324,  324,  312,  313,  339,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  294,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  312,  313,
};
#define YYFINAL 46
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 339
#if YYDEBUG
char *bxname[] = {
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
"BX_TOKEN_QUIT","BX_TOKEN_PROGRAM","BX_TOKEN_REGISTERS","BX_TOKEN_IDT",
"BX_TOKEN_GDT","BX_TOKEN_LDT","BX_TOKEN_TSS","BX_TOKEN_DIRTY","BX_TOKEN_LINUX",
"BX_TOKEN_CONTROL_REGS","BX_TOKEN_EXAMINE","BX_TOKEN_XFORMAT",
"BX_TOKEN_SETPMEM","BX_TOKEN_SYMBOLNAME","BX_TOKEN_QUERY","BX_TOKEN_PENDING",
"BX_TOKEN_TAKE","BX_TOKEN_DMA","BX_TOKEN_IRQ","BX_TOKEN_DUMP_CPU",
"BX_TOKEN_SET_CPU","BX_TOKEN_DIS","BX_TOKEN_ON","BX_TOKEN_OFF",
"BX_TOKEN_DISASSEMBLE","BX_TOKEN_INSTRUMENT","BX_TOKEN_START","BX_TOKEN_STOP",
"BX_TOKEN_RESET","BX_TOKEN_PRINT","BX_TOKEN_LOADER","BX_TOKEN_STRING",
"BX_TOKEN_DOIT","BX_TOKEN_CRC","BX_TOKEN_TRACEON","BX_TOKEN_TRACEOFF",
"BX_TOKEN_PTIME","BX_TOKEN_TIMEBP_ABSOLUTE","BX_TOKEN_TIMEBP","BX_TOKEN_RECORD",
"BX_TOKEN_PLAYBACK","BX_TOKEN_MODEBP","BX_TOKEN_PRINT_STACK","BX_TOKEN_WATCH",
"BX_TOKEN_UNWATCH","BX_TOKEN_READ","BX_TOKEN_WRITE","BX_TOKEN_SHOW",
"BX_TOKEN_SYMBOL","BX_TOKEN_GLOBAL","BX_TOKEN_WHERE","BX_TOKEN_PRINT_STRING",
"BX_TOKEN_DIFF_MEMORY","BX_TOKEN_SYNC_MEMORY","BX_TOKEN_SYNC_CPU",
"BX_TOKEN_FAST_FORWARD","BX_TOKEN_PHY_2_LOG","BX_TOKEN_NUMERIC",
"BX_TOKEN_LONG_NUMERIC","BX_TOKEN_INFO_ADDRESS","BX_TOKEN_CS","BX_TOKEN_ES",
"BX_TOKEN_SS","BX_TOKEN_DS","BX_TOKEN_FS","BX_TOKEN_GS","BX_TOKEN_ALWAYS_CHECK",
"BX_TOKEN_MATHS","BX_TOKEN_ADD","BX_TOKEN_SUB","BX_TOKEN_MUL","BX_TOKEN_DIV",
"BX_TOKEN_V2L",
};
char *bxrule[] = {
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
"info_command : BX_TOKEN_INFO BX_TOKEN_DIRTY '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric_range '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_GDT optional_numeric_range '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_LDT optional_numeric_range '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_TSS optional_numeric_range '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_LINUX '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_CONTROL_REGS '\\n'",
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
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
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
        if (yyssp >= yyss + yystacksize - 1)
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
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
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
                if (yyssp >= yyss + yystacksize - 1)
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
case 36:
#line 146 "parser.y"
{
      }
break;
case 37:
#line 152 "parser.y"
{
		bx_dbg_diff_memory();
		free(yyvsp[-1].sval);
	}
break;
case 38:
#line 157 "parser.y"
{
		bx_dbg_sync_memory(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 39:
#line 162 "parser.y"
{
		bx_dbg_sync_memory(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 40:
#line 167 "parser.y"
{
		bx_dbg_sync_cpu(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 41:
#line 172 "parser.y"
{
		bx_dbg_sync_cpu(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 42:
#line 177 "parser.y"
{
		free(yyvsp[-2].sval);
		bx_dbg_fast_forward(yyvsp[-1].uval);
	}
break;
case 43:
#line 182 "parser.y"
{
	}
break;
case 44:
#line 185 "parser.y"
{
		free(yyvsp[-4].sval);
		bx_dbg_info_address(yyvsp[-3].uval, yyvsp[-1].uval);
        }
break;
case 45:
#line 190 "parser.y"
{
        }
break;
case 46:
#line 193 "parser.y"
{
        }
break;
case 47:
#line 198 "parser.y"
{ yyval.uval = 1; }
break;
case 48:
#line 199 "parser.y"
{ yyval.uval = 0; }
break;
case 49:
#line 200 "parser.y"
{ yyval.uval = 2; }
break;
case 50:
#line 201 "parser.y"
{ yyval.uval = 3; }
break;
case 51:
#line 202 "parser.y"
{ yyval.uval = 4; }
break;
case 52:
#line 203 "parser.y"
{ yyval.uval = 5; }
break;
case 53:
#line 208 "parser.y"
{
        bx_dbg_timebp_command(0, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
break;
case 54:
#line 213 "parser.y"
{
        bx_dbg_timebp_command(1, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
break;
case 55:
#line 221 "parser.y"
{
          bx_dbg_record_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 56:
#line 229 "parser.y"
{
          bx_dbg_playback_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 57:
#line 237 "parser.y"
{
          bx_dbg_modebp_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 58:
#line 242 "parser.y"
{
          bx_dbg_modebp_command(0);
          free(yyvsp[-1].sval);
          }
break;
case 59:
#line 250 "parser.y"
{
          bx_dbg_show_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 60:
#line 255 "parser.y"
{
          bx_dbg_show_command(0);
          free(yyvsp[-1].sval);
          }
break;
case 61:
#line 263 "parser.y"
{
        bx_dbg_ptime_command();
        free(yyvsp[-1].sval);
	}
break;
case 62:
#line 271 "parser.y"
{
        bx_dbg_trace_on_command();
        free(yyvsp[-1].sval);
	}
break;
case 63:
#line 279 "parser.y"
{
        bx_dbg_trace_off_command();
        free(yyvsp[-1].sval);
	}
break;
case 64:
#line 287 "parser.y"
{
          bx_dbg_print_stack_command(16);
          free(yyvsp[-1].sval);
	  }
break;
case 65:
#line 292 "parser.y"
{
          bx_dbg_print_stack_command(yyvsp[-1].uval);
          free(yyvsp[-2].sval);
	  }
break;
case 66:
#line 300 "parser.y"
{
          watchpoint_continue = 0;
	  fprintf(stderr, "Will stop on watch points\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 67:
#line 306 "parser.y"
{
          watchpoint_continue = 1;
          fprintf(stderr, "Will not stop on watch points (they will still be logged)\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 68:
#line 312 "parser.y"
{
          bx_dbg_watch(-1, 0);
          free(yyvsp[-1].sval);
          }
break;
case 69:
#line 317 "parser.y"
{
          bx_dbg_unwatch(-1, 0);
          free(yyvsp[-1].sval);
          }
break;
case 70:
#line 322 "parser.y"
{
          bx_dbg_watch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 71:
#line 327 "parser.y"
{
          bx_dbg_unwatch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 72:
#line 332 "parser.y"
{
          bx_dbg_watch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 73:
#line 337 "parser.y"
{
          bx_dbg_unwatch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 74:
#line 345 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-1].sval, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 75:
#line 350 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-2].sval, 0, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 76:
#line 355 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-2].sval, 1, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 77:
#line 363 "parser.y"
{
        bx_dbg_where_command();
        free(yyvsp[-1].sval);
        }
break;
case 78:
#line 371 "parser.y"
{
        bx_dbg_print_string_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 79:
#line 379 "parser.y"
{
        bx_dbg_continue_command();
        free(yyvsp[-1].sval);
        }
break;
case 80:
#line 387 "parser.y"
{
        bx_dbg_stepN_command(1);
        free(yyvsp[-1].sval);
        }
break;
case 81:
#line 392 "parser.y"
{
        bx_dbg_stepN_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 82:
#line 400 "parser.y"
{
        bx_dbg_set_command(yyvsp[-3].sval, yyvsp[-2].sval, yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 83:
#line 405 "parser.y"
{
        bx_dbg_set_command(yyvsp[-3].sval, yyvsp[-2].sval, yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 84:
#line 410 "parser.y"
{
        bx_dbg_set_symbol_command(yyvsp[-3].sval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 85:
#line 418 "parser.y"
{
        bx_dbg_vbreakpoint_command(0, 0, 0);
        free(yyvsp[-1].sval);
        }
break;
case 86:
#line 423 "parser.y"
{
        bx_dbg_vbreakpoint_command(1, yyvsp[-3].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
break;
case 87:
#line 428 "parser.y"
{
        bx_dbg_lbreakpoint_command(0, 0);
        free(yyvsp[-1].sval);
        }
break;
case 88:
#line 433 "parser.y"
{
        bx_dbg_lbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 89:
#line 438 "parser.y"
{
        bx_dbg_pbreakpoint_command(0, 0);
        free(yyvsp[-1].sval);
        }
break;
case 90:
#line 443 "parser.y"
{
        bx_dbg_pbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 91:
#line 448 "parser.y"
{
        bx_dbg_pbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
break;
case 92:
#line 456 "parser.y"
{
        bx_dbg_info_bpoints_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 93:
#line 461 "parser.y"
{
        bx_dbg_info_program_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 94:
#line 466 "parser.y"
{
        bx_dbg_info_registers_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 95:
#line 471 "parser.y"
{
        bx_dbg_info_dirty_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 96:
#line 476 "parser.y"
{
        bx_dbg_info_idt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 97:
#line 481 "parser.y"
{
        bx_dbg_info_gdt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 98:
#line 486 "parser.y"
{
        bx_dbg_info_ldt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 99:
#line 491 "parser.y"
{
        bx_dbg_info_tss_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 100:
#line 496 "parser.y"
{
        bx_dbg_info_linux_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 101:
#line 501 "parser.y"
{
        bx_dbg_info_control_regs_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 102:
#line 508 "parser.y"
{ yyval.uval = EMPTY_ARG; }
break;
case 104:
#line 512 "parser.y"
{ yyval.uval_range = make_num_range (EMPTY_ARG, EMPTY_ARG); }
break;
case 106:
#line 517 "parser.y"
{
    yyval.uval_range = make_num_range (yyvsp[0].uval, yyvsp[0].uval);
  }
break;
case 107:
#line 522 "parser.y"
{
    yyval.uval_range = make_num_range (yyvsp[-1].uval, yyvsp[0].uval);
  }
break;
case 108:
#line 527 "parser.y"
{
    yyval.uval_range = make_num_range (yyvsp[-2].uval, yyvsp[0].uval);
  }
break;
case 109:
#line 534 "parser.y"
{
        bx_dbg_dump_cpu_command();
        free(yyvsp[-1].sval);
        }
break;
case 110:
#line 542 "parser.y"
{
        bx_dbg_del_breakpoint_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 111:
#line 550 "parser.y"
{
	  bx_dbg_quit_command();
	  free(yyvsp[-1].sval);
        }
break;
case 112:
#line 559 "parser.y"
{
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].uval,1, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].uval,1, 1);
#endif
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 113:
#line 567 "parser.y"
{
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 1);
#endif
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 114:
#line 575 "parser.y"
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
case 115:
#line 588 "parser.y"
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
case 116:
#line 604 "parser.y"
{
        bx_dbg_setpmem_command(yyvsp[-3].uval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
break;
case 117:
#line 612 "parser.y"
{
        bx_dbg_query_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 118:
#line 620 "parser.y"
{
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 119:
#line 625 "parser.y"
{
        bx_dbg_take_command(yyvsp[-2].sval, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 120:
#line 630 "parser.y"
{
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 121:
#line 638 "parser.y"
{
        bx_dbg_set_cpu_command();
        free(yyvsp[-1].sval);
        }
break;
case 122:
#line 646 "parser.y"
{
        bx_dbg_disassemble_command(yyvsp[-1].uval_range);
        free(yyvsp[-2].sval);
        }
break;
case 123:
#line 654 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 124:
#line 659 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 125:
#line 664 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 126:
#line 669 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 127:
#line 677 "parser.y"
{
        bx_dbg_loader_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 128:
#line 685 "parser.y"
{
        bx_dbg_doit_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 129:
#line 693 "parser.y"
{
        bx_dbg_crc_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
break;
case 130:
#line 701 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 131:
#line 706 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 132:
#line 711 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 133:
#line 716 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 134:
#line 721 "parser.y"
{
        bx_dbg_maths_expression_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 135:
#line 728 "parser.y"
{
        bx_dbg_v2l_command(yyvsp[-3].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
break;
#line 1360 "y.tab.c"
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
    if (yyssp >= yyss + yystacksize - 1)
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
