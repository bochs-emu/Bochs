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
#line 6 "parser.y"
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
/* %left '-' '+'*/
/* %left '*' '/'*/
/* %right*/
/* %nonassoc UMINUS*/
#line 16 "parser.y"
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
    9,    9,    9,    9,    9,    9,    2,    2,    4,    4,
    3,    3,    3,   10,   11,   12,   13,   13,   13,   13,
   14,   15,   16,   16,   16,   17,   18,   19,   19,   19,
   19,   20,   21,   22,   23,   23,   23,   23,   23,   38,
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
    4,    3,    3,    3,    3,    3,    3,    4,    4,    4,
    4,    3,    3,    3,    5,    7,    0,    1,    0,    1,
    1,    2,    3,    2,    3,    2,    4,    3,    3,    2,
    5,    3,    3,    4,    3,    2,    3,    3,    3,    3,
    3,    3,    3,    4,    5,    5,    5,    5,    3,    5,
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
    0,    0,    0,    0,    0,    0,  116,    0,    0,  120,
    0,    0,    0,    0,  114,  126,    0,  110,    0,    0,
    0,    0,    0,    0,    0,    0,   62,   63,   61,    0,
    0,    0,    0,    0,   58,    0,   64,    0,    0,    0,
    0,   68,    0,    0,   69,    0,   60,    0,    0,   77,
    0,   37,    0,    0,    0,    0,    0,    0,   47,   48,
   49,   50,   51,   52,    0,    0,    0,    0,    0,    0,
    0,    0,   81,    0,    0,    0,    0,   88,   90,    0,
  115,   92,   93,   94,   95,   96,    0,    0,    0,    0,
   97,  102,  103,    0,  104,    0,  118,  119,    0,  122,
    0,  123,  125,  112,    0,  127,  128,  129,  130,  131,
  132,  133,    0,   54,   53,   55,   56,   57,   65,   67,
   66,    0,    0,    0,    0,   59,    0,   74,    0,   78,
   38,   39,   40,   41,   42,   43,    0,    0,    0,  139,
    0,    0,    0,    0,    0,    0,   82,   83,    0,   91,
   98,   99,  100,  101,    0,  117,    0,  124,  113,  134,
   70,   72,   71,   73,   75,    0,    0,   45,   46,    0,
    0,    0,    0,    0,   84,   86,    0,  105,  121,   76,
   44,  135,  136,  137,  138,  140,    0,  106,
};
short bxdgoto[] = {                                      46,
  165,    0,  118,  119,   47,   48,   49,   50,   51,   52,
   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,
   63,   64,   65,   66,   67,   68,   69,   70,   71,   72,
   73,   74,   75,   76,   77,   78,   79,   80,
};
short bxsindex[] = {                                    -10,
    9,   -4, -270,   -3,   -2,   -7, -298, -231,   21,   -6,
 -292, -248, -225,   38,   59, -256, -242, -229, -254, -253,
   64,   65,   66, -250, -249, -221, -220,    8,   -1,   20,
   22,   17, -289,   71, -244,   73, -275, -228, -241, -240,
 -310, -239, -290, -310,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   74,    0,   27, -226,   31,    0,   80,    0,   81,
    0, -234,   83,   84,   85,   86,   87,   89, -256, -256,
 -256, -256,   90,   91,   92,   -9,    0,    3,   93,    0,
 -222,   95,    4,   96,    0,    0,  -56,    0,   97,   98,
   99,  100,  101,  102,  103, -212,    0,    0,    0,  105,
  106,  107,  108,  109,    0,  110,    0,  111,  112, -203,
 -202,    0, -201, -200,    0,  117,    0,    5, -172,    0,
  119,    0,  120,  121,  122,  123,  124,  125,    0,    0,
    0,    0,    0,    0,   78, -224,  127, -188, -187, -186,
 -185,   88,    0, -184,  133,  134, -181,    0,    0,  137,
    0,    0,    0,    0,    0,    0,  138,  139,  140,  141,
    0,    0,    0, -174,    0,  143,    0,    0, -171,    0,
  144,    0,    0,    0, -170,    0,    0,    0,    0,    0,
    0,    0,  147,    0,    0,    0,    0,    0,    0,    0,
    0,  148,  149,  150,  151,    0,  152,    0, -163,    0,
    0,    0,    0,    0,    0,    0, -162,  155,  156,    0,
 -159, -158, -157, -156, -155,  162,    0,    0,  163,    0,
    0,    0,    0,    0,   -5,    0,  164,    0,    0,    0,
    0,    0,    0,    0,    0,  165,  166,    0,    0,  167,
  168,  169,  170,  171,    0,    0, -144,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  173,    0,
};
short bxrindex[] = {                                    184,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  175,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  175,  175,
  175,  175,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  176,    0,    0,    0,
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
    0,    0,    0,    0,    0,    0,    0,    0,
};
short bxgindex[] = {                                      0,
  145,    0,    0,  -42,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 337
short bxtable[] = {                                      45,
  195,  205,   91,  110,  278,   83,   87,   89,  137,  167,
  148,   84,  197,  202,  228,  153,  154,  135,   81,   85,
  159,  160,  161,  162,  163,  164,  147,   93,  149,  142,
  107,  145,   94,  111,   92,  112,   95,   96,   97,   98,
   99,  100,  101,  102,  103,  104,  105,  115,  168,  169,
  170,  171,  120,  121,  122,  123,  187,  188,  189,  190,
  113,  114,  155,  156,  175,  176,  238,  239,  116,  117,
  124,  125,  126,  127,  128,  129,  130,  131,  132,  133,
  150,  151,  152,  173,  157,  158,  166,  174,  177,  178,
  179,  180,  181,  182,  183,  184,  185,  106,  186,  191,
  192,  193,  198,  199,  200,  203,  206,  207,  208,  209,
  210,  211,  212,  213,  214,  215,  216,  217,  218,  219,
  220,  221,  222,  223,  224,  225,  226,  229,  230,  231,
  232,  233,  234,  235,  236,  237,  240,  241,  242,  243,
  244,  246,  247,  248,  249,  245,  250,  251,  252,  253,
  254,  255,  256,  258,  257,  259,  260,  261,  262,  263,
  264,  265,  266,  267,  268,  269,  270,  271,  272,  273,
  274,  275,  276,  279,  280,  281,  282,  283,  284,  285,
  286,  287,  288,   35,  109,  111,    0,    0,  172,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    1,    2,    0,    3,
    0,    4,    5,    6,    7,    8,    9,    0,    0,    0,
    0,    0,    0,  277,    0,    0,    0,    0,   10,  204,
   11,    0,   12,  108,   13,    0,  138,   14,   15,    0,
    0,    0,   16,   17,    0,    0,    0,    0,   18,    0,
   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,
   29,   30,   31,    0,    0,   32,   33,  134,   34,   35,
   36,   37,   38,   39,   40,  139,  146,   41,   90,  109,
  194,   82,   86,   88,  136,    0,   42,   43,  196,  201,
  227,    0,   44,  140,  141,  143,  144,
};
short bxcheck[] = {                                      10,
   10,   58,   10,   10,   10,   10,   10,   10,   10,  300,
  300,  282,   10,   10,   10,  291,  292,   10,   10,  290,
  331,  332,  333,  334,  335,  336,   10,  326,  318,   10,
   10,   10,  264,  326,   42,  284,  268,  269,  270,  271,
  272,  273,  274,  275,  276,  277,  278,   10,  339,  340,
  341,  342,  295,  296,  297,  298,   99,  100,  101,  102,
  286,  287,  291,  292,  291,  292,  291,  292,   10,  326,
  300,  326,  326,   10,   10,   10,  327,  327,  300,  300,
   10,  326,   10,   10,  326,  326,  326,   61,   58,   10,
   10,  326,   10,   10,   10,   10,   10,  329,   10,   10,
   10,   10,   10,  326,   10,   10,   10,   10,   10,   10,
   10,   10,   10,  326,   10,   10,   10,   10,   10,   10,
   10,   10,  326,  326,  326,  326,   10,  300,   10,   10,
   10,   10,   10,   10,   10,   58,   10,  326,  326,  326,
  326,  326,   10,   10,  326,   58,   10,   10,   10,   10,
   10,  326,   10,   10,  326,  326,   10,   10,   10,   10,
   10,   10,  326,  326,   10,   10,  326,  326,  326,  326,
  326,   10,   10,   10,   10,   10,   10,   10,   10,   10,
   10,  326,   10,    0,   10,   10,   -1,   -1,   44,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,   -1,  260,
   -1,  262,  263,  264,  265,  266,  267,   -1,   -1,   -1,
   -1,   -1,   -1,  269,   -1,   -1,   -1,   -1,  279,  326,
  281,   -1,  283,  280,  285,   -1,  257,  288,  289,   -1,
   -1,   -1,  293,  294,   -1,   -1,   -1,   -1,  299,   -1,
  301,  302,  303,  304,  305,  306,  307,  308,  309,  310,
  311,  312,  313,   -1,   -1,  316,  317,  300,  319,  320,
  321,  322,  323,  324,  325,  296,  300,  328,  326,  326,
  330,  326,  326,  326,  326,   -1,  337,  338,  326,  326,
  326,   -1,  343,  314,  315,  314,  315,
};
#define YYFINAL 46
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 343
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
"BX_TOKEN_MUL","BX_TOKEN_DIV","BX_TOKEN_V2L",
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
"info_command : BX_TOKEN_INFO BX_TOKEN_FPU '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_ALL '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_DIRTY '\\n'",
"info_command : BX_TOKEN_INFO BX_TOKEN_IDT optional_numeric_range '\\n'",
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
#line 154 "parser.y"
{
      }
break;
case 37:
#line 160 "parser.y"
{
		bx_dbg_diff_memory();
		free(yyvsp[-1].sval);
	}
break;
case 38:
#line 165 "parser.y"
{
		bx_dbg_sync_memory(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 39:
#line 170 "parser.y"
{
		bx_dbg_sync_memory(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 40:
#line 175 "parser.y"
{
		bx_dbg_sync_cpu(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 41:
#line 180 "parser.y"
{
		bx_dbg_sync_cpu(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 42:
#line 185 "parser.y"
{
		free(yyvsp[-2].sval);
		bx_dbg_fast_forward(yyvsp[-1].uval);
	}
break;
case 43:
#line 190 "parser.y"
{
	}
break;
case 44:
#line 193 "parser.y"
{
		free(yyvsp[-4].sval);
		bx_dbg_info_address(yyvsp[-3].uval, yyvsp[-1].uval);
        }
break;
case 45:
#line 198 "parser.y"
{
        }
break;
case 46:
#line 201 "parser.y"
{
        }
break;
case 47:
#line 206 "parser.y"
{ yyval.uval = 1; }
break;
case 48:
#line 207 "parser.y"
{ yyval.uval = 0; }
break;
case 49:
#line 208 "parser.y"
{ yyval.uval = 2; }
break;
case 50:
#line 209 "parser.y"
{ yyval.uval = 3; }
break;
case 51:
#line 210 "parser.y"
{ yyval.uval = 4; }
break;
case 52:
#line 211 "parser.y"
{ yyval.uval = 5; }
break;
case 53:
#line 216 "parser.y"
{
        bx_dbg_timebp_command(0, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
break;
case 54:
#line 221 "parser.y"
{
        bx_dbg_timebp_command(1, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
break;
case 55:
#line 229 "parser.y"
{
          bx_dbg_record_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 56:
#line 237 "parser.y"
{
          bx_dbg_playback_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 57:
#line 245 "parser.y"
{
          bx_dbg_modebp_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 58:
#line 250 "parser.y"
{
          bx_dbg_modebp_command(0);
          free(yyvsp[-1].sval);
          }
break;
case 59:
#line 258 "parser.y"
{
          bx_dbg_show_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 60:
#line 263 "parser.y"
{
          bx_dbg_show_command(0);
          free(yyvsp[-1].sval);
          }
break;
case 61:
#line 271 "parser.y"
{
        bx_dbg_ptime_command();
        free(yyvsp[-1].sval);
	}
break;
case 62:
#line 279 "parser.y"
{
        bx_dbg_trace_on_command();
        free(yyvsp[-1].sval);
	}
break;
case 63:
#line 287 "parser.y"
{
        bx_dbg_trace_off_command();
        free(yyvsp[-1].sval);
	}
break;
case 64:
#line 295 "parser.y"
{
          bx_dbg_print_stack_command(16);
          free(yyvsp[-1].sval);
	  }
break;
case 65:
#line 300 "parser.y"
{
          bx_dbg_print_stack_command(yyvsp[-1].uval);
          free(yyvsp[-2].sval);
	  }
break;
case 66:
#line 308 "parser.y"
{
          watchpoint_continue = 0;
	  fprintf(stderr, "Will stop on watch points\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 67:
#line 314 "parser.y"
{
          watchpoint_continue = 1;
          fprintf(stderr, "Will not stop on watch points (they will still be logged)\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 68:
#line 320 "parser.y"
{
          bx_dbg_watch(-1, 0);
          free(yyvsp[-1].sval);
          }
break;
case 69:
#line 325 "parser.y"
{
          bx_dbg_unwatch(-1, 0);
          free(yyvsp[-1].sval);
          }
break;
case 70:
#line 330 "parser.y"
{
          bx_dbg_watch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 71:
#line 335 "parser.y"
{
          bx_dbg_unwatch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 72:
#line 340 "parser.y"
{
          bx_dbg_watch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 73:
#line 345 "parser.y"
{
          bx_dbg_unwatch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 74:
#line 353 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-1].sval, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 75:
#line 358 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-2].sval, 0, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 76:
#line 363 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-2].sval, 1, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 77:
#line 371 "parser.y"
{
        bx_dbg_where_command();
        free(yyvsp[-1].sval);
        }
break;
case 78:
#line 379 "parser.y"
{
        bx_dbg_print_string_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 79:
#line 387 "parser.y"
{
        bx_dbg_continue_command();
        free(yyvsp[-1].sval);
        }
break;
case 80:
#line 395 "parser.y"
{
        bx_dbg_stepN_command(1);
        free(yyvsp[-1].sval);
        }
break;
case 81:
#line 400 "parser.y"
{
        bx_dbg_stepN_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 82:
#line 408 "parser.y"
{
        bx_dbg_set_command(yyvsp[-3].sval, yyvsp[-2].sval, yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 83:
#line 413 "parser.y"
{
        bx_dbg_set_command(yyvsp[-3].sval, yyvsp[-2].sval, yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 84:
#line 418 "parser.y"
{
        bx_dbg_set_symbol_command(yyvsp[-3].sval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 85:
#line 426 "parser.y"
{
        bx_dbg_vbreakpoint_command(0, 0, 0);
        free(yyvsp[-1].sval);
        }
break;
case 86:
#line 431 "parser.y"
{
        bx_dbg_vbreakpoint_command(1, yyvsp[-3].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
break;
case 87:
#line 436 "parser.y"
{
        bx_dbg_lbreakpoint_command(0, 0);
        free(yyvsp[-1].sval);
        }
break;
case 88:
#line 441 "parser.y"
{
        bx_dbg_lbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 89:
#line 446 "parser.y"
{
        bx_dbg_pbreakpoint_command(0, 0);
        free(yyvsp[-1].sval);
        }
break;
case 90:
#line 451 "parser.y"
{
        bx_dbg_pbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 91:
#line 456 "parser.y"
{
        bx_dbg_pbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
break;
case 92:
#line 464 "parser.y"
{
        bx_dbg_info_bpoints_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 93:
#line 469 "parser.y"
{
        bx_dbg_info_program_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 94:
#line 474 "parser.y"
{
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 95:
#line 479 "parser.y"
{
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 96:
#line 484 "parser.y"
{
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS | BX_INFO_FPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 97:
#line 489 "parser.y"
{
        bx_dbg_info_dirty_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 98:
#line 494 "parser.y"
{
        bx_dbg_info_idt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 99:
#line 499 "parser.y"
{
        bx_dbg_info_gdt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 100:
#line 504 "parser.y"
{
        bx_dbg_info_ldt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 101:
#line 509 "parser.y"
{
        bx_dbg_info_tss_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 102:
#line 514 "parser.y"
{
        bx_dbg_info_linux_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 103:
#line 519 "parser.y"
{
        bx_dbg_info_control_regs_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 104:
#line 524 "parser.y"
{
        bx_dbg_info_ne2k(-1, -1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 105:
#line 529 "parser.y"
{
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-1].uval, -1);
        }
break;
case 106:
#line 534 "parser.y"
{
        free(yyvsp[-6].sval); free(yyvsp[-5].sval); free(yyvsp[-4].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-3].uval, yyvsp[-1].uval);
        }
break;
case 107:
#line 541 "parser.y"
{ yyval.uval = EMPTY_ARG; }
break;
case 109:
#line 545 "parser.y"
{ yyval.uval_range = make_num_range (EMPTY_ARG, EMPTY_ARG); }
break;
case 111:
#line 550 "parser.y"
{
    yyval.uval_range = make_num_range (yyvsp[0].uval, yyvsp[0].uval);
  }
break;
case 112:
#line 555 "parser.y"
{
    yyval.uval_range = make_num_range (yyvsp[-1].uval, yyvsp[0].uval);
  }
break;
case 113:
#line 560 "parser.y"
{
    yyval.uval_range = make_num_range (yyvsp[-2].uval, yyvsp[0].uval);
  }
break;
case 114:
#line 567 "parser.y"
{
        bx_dbg_dump_cpu_command();
        free(yyvsp[-1].sval);
        }
break;
case 115:
#line 575 "parser.y"
{
        bx_dbg_del_breakpoint_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 116:
#line 583 "parser.y"
{
	  bx_dbg_quit_command();
	  free(yyvsp[-1].sval);
        }
break;
case 117:
#line 592 "parser.y"
{
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].uval,1, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].uval,1, 1);
#endif
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 118:
#line 600 "parser.y"
{
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 1);
#endif
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 119:
#line 608 "parser.y"
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
case 120:
#line 621 "parser.y"
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
case 121:
#line 637 "parser.y"
{
        bx_dbg_setpmem_command(yyvsp[-3].uval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
break;
case 122:
#line 645 "parser.y"
{
        bx_dbg_query_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 123:
#line 653 "parser.y"
{
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 124:
#line 658 "parser.y"
{
        bx_dbg_take_command(yyvsp[-2].sval, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 125:
#line 663 "parser.y"
{
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 126:
#line 671 "parser.y"
{
        bx_dbg_set_cpu_command();
        free(yyvsp[-1].sval);
        }
break;
case 127:
#line 679 "parser.y"
{
        bx_dbg_disassemble_command(yyvsp[-1].uval_range);
        free(yyvsp[-2].sval);
        }
break;
case 128:
#line 687 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 129:
#line 692 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 130:
#line 697 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 131:
#line 702 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 132:
#line 710 "parser.y"
{
        bx_dbg_loader_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 133:
#line 718 "parser.y"
{
        bx_dbg_doit_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 134:
#line 726 "parser.y"
{
        bx_dbg_crc_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
break;
case 135:
#line 734 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 136:
#line 739 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 137:
#line 744 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 138:
#line 749 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 139:
#line 754 "parser.y"
{
        bx_dbg_maths_expression_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 140:
#line 761 "parser.y"
{
        bx_dbg_v2l_command(yyvsp[-3].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
break;
#line 1404 "y.tab.c"
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
