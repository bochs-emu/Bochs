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
  } YYSTYPE;
#line 50 "y.tab.c"
#define BX_TOKEN_CONTINUE 257
#define BX_TOKEN_STEPN 258
#define BX_TOKEN_NEXT_STEP 259
#define BX_TOKEN_SET 260
#define BX_TOKEN_VBREAKPOINT 261
#define BX_TOKEN_LBREAKPOINT 262
#define BX_TOKEN_PBREAKPOINT 263
#define BX_TOKEN_DEL_BREAKPOINT 264
#define BX_TOKEN_INFO 265
#define BX_TOKEN_QUIT 266
#define BX_TOKEN_PROGRAM 267
#define BX_TOKEN_REGISTERS 268
#define BX_TOKEN_DIRTY 269
#define BX_TOKEN_EXAMINE 270
#define BX_TOKEN_XFORMAT 271
#define BX_TOKEN_SETPMEM 272
#define BX_TOKEN_SYMBOLNAME 273
#define BX_TOKEN_QUERY 274
#define BX_TOKEN_PENDING 275
#define BX_TOKEN_TAKE 276
#define BX_TOKEN_DMA 277
#define BX_TOKEN_IRQ 278
#define BX_TOKEN_DUMP_CPU 279
#define BX_TOKEN_SET_CPU 280
#define BX_TOKEN_DIS 281
#define BX_TOKEN_ON 282
#define BX_TOKEN_OFF 283
#define BX_TOKEN_DISASSEMBLE 284
#define BX_TOKEN_INSTRUMENT 285
#define BX_TOKEN_START 286
#define BX_TOKEN_STOP 287
#define BX_TOKEN_RESET 288
#define BX_TOKEN_PRINT 289
#define BX_TOKEN_LOADER 290
#define BX_TOKEN_STRING 291
#define BX_TOKEN_DOIT 292
#define BX_TOKEN_CRC 293
#define BX_TOKEN_TRACEON 294
#define BX_TOKEN_TRACEOFF 295
#define BX_TOKEN_PTIME 296
#define BX_TOKEN_TIMEBP_ABSOLUTE 297
#define BX_TOKEN_TIMEBP 298
#define BX_TOKEN_RECORD 299
#define BX_TOKEN_PLAYBACK 300
#define BX_TOKEN_MODEBP 301
#define BX_TOKEN_PRINT_STACK 302
#define BX_TOKEN_WATCH 303
#define BX_TOKEN_UNWATCH 304
#define BX_TOKEN_READ 305
#define BX_TOKEN_WRITE 306
#define BX_TOKEN_SHOW 307
#define BX_TOKEN_SYMBOL 308
#define BX_TOKEN_GLOBAL 309
#define BX_TOKEN_WHERE 310
#define BX_TOKEN_PRINT_STRING 311
#define BX_TOKEN_DIFF_MEMORY 312
#define BX_TOKEN_SYNC_MEMORY 313
#define BX_TOKEN_SYNC_CPU 314
#define BX_TOKEN_FAST_FORWARD 315
#define BX_TOKEN_PHY_2_LOG 316
#define BX_TOKEN_NUMERIC 317
#define BX_TOKEN_LONG_NUMERIC 318
#define BX_TOKEN_INFO_ADDRESS 319
#define BX_TOKEN_CS 320
#define BX_TOKEN_ES 321
#define BX_TOKEN_SS 322
#define BX_TOKEN_DS 323
#define BX_TOKEN_FS 324
#define BX_TOKEN_GS 325
#define BX_TOKEN_ALWAYS_CHECK 326
#define BX_TOKEN_MATHS 327
#define BX_TOKEN_ADD 328
#define BX_TOKEN_SUB 329
#define BX_TOKEN_MUL 330
#define BX_TOKEN_DIV 331
#define BX_TOKEN_V2L 332
#define YYERRCODE 256
short bxlhs[] = {                                        -1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   34,   34,   34,   34,   34,
   34,   34,   34,   34,   34,    1,    1,    1,    1,    1,
    1,   24,   24,   25,   26,   27,   27,   30,   30,   23,
   21,   22,   28,   28,   29,   29,   29,   29,   29,   29,
   29,   29,   31,   31,   31,   32,   33,    2,    3,    3,
    4,    4,    4,    5,    5,    5,    5,    5,    5,    5,
    6,    6,    6,    6,    7,    8,    9,   10,   10,   10,
   10,   11,   12,   13,   13,   13,   14,   15,   16,   16,
   16,   16,   17,   18,   19,   20,   20,   20,   20,   20,
   35,
};
short bxlen[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    2,    3,    3,    3,    3,
    3,    3,    5,    4,    4,    1,    1,    1,    1,    1,
    1,    3,    3,    3,    3,    3,    2,    3,    2,    2,
    2,    2,    2,    3,    3,    3,    2,    2,    4,    4,
    4,    4,    3,    4,    5,    2,    3,    2,    2,    3,
    4,    4,    5,    2,    5,    2,    3,    2,    3,    4,
    3,    3,    3,    3,    2,    3,    2,    4,    3,    3,
    2,    5,    3,    3,    4,    3,    2,    4,    3,    3,
    3,    3,    3,    3,    4,    5,    5,    5,    5,    3,
    5,
};
short bxdefred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   35,    0,    1,    2,    3,    4,
    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
   15,   16,   17,   18,   19,   20,   21,   22,   23,   24,
   25,   26,   27,   28,   29,   30,   31,   32,   33,   34,
   78,    0,   79,    0,    0,    0,   84,    0,   86,    0,
   88,    0,    0,    0,    0,    0,    0,   97,    0,    0,
  101,    0,    0,    0,    0,   95,  107,    0,    0,    0,
    0,    0,    0,    0,    0,   61,   62,   60,    0,    0,
    0,    0,    0,   57,    0,   63,    0,    0,    0,    0,
   67,    0,    0,   68,    0,   59,    0,    0,   76,    0,
   36,    0,    0,    0,    0,    0,    0,   46,   47,   48,
   49,   50,   51,    0,    0,    0,    0,    0,    0,    0,
    0,   80,    0,    0,    0,    0,   87,   89,    0,   96,
   91,   92,   93,   94,    0,   99,  100,    0,  103,    0,
  104,  106,    0,  109,  110,  111,  112,  113,  114,    0,
   53,   52,   54,   55,   56,   64,   66,   65,    0,    0,
    0,    0,   58,    0,   73,    0,   77,   37,   38,   39,
   40,   41,   42,    0,    0,    0,  120,    0,    0,    0,
    0,    0,    0,   81,   82,    0,   90,   98,    0,  105,
  108,  115,   69,   71,   70,   72,   74,    0,    0,   44,
   45,    0,    0,    0,    0,    0,   83,   85,  102,   75,
   43,  116,  117,  118,  119,  121,
};
short bxdgoto[] = {                                      46,
  154,   47,   48,   49,   50,   51,   52,   53,   54,   55,
   56,   57,   58,   59,   60,   61,   62,   63,   64,   65,
   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,
   76,   77,   78,   79,   80,
};
short bxsindex[] = {                                    -10,
    3,   -7, -263,   -5,   -4,   -9, -300, -247,    9,   -6,
 -291, -246, -254,   18,   28, -262, -252, -235, -260, -259,
   49,   50,   51, -256, -255, -227, -226,    4,   -3,   20,
   15,    5, -282,   56, -250,   58, -251, -233, -248, -245,
 -277, -244, -289, -277,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   60,    0,   10, -231,   16,    0,   65,    0,   66,
    0, -240,   68,   69,   70,   71,   72,    0,   -2,   73,
    0, -232,   74,    1,   76,    0,    0, -230,   78,   79,
   80,   81,   82,   83, -223,    0,    0,    0,   85,   86,
   87,   88,   89,    0,   90,    0,   91,   92, -214, -213,
    0, -212, -211,    0,   97,    0,    2, -183,    0,   99,
    0,  100,  101,  102,  103,  104,  105,    0,    0,    0,
    0,    0,    0,   59, -229,  106, -199, -198, -197, -196,
   64,    0, -194,  114,  115, -191,    0,    0,  117,    0,
    0,    0,    0,    0,  118,    0,    0, -188,    0,  120,
    0,    0,  121,    0,    0,    0,    0,    0,    0,  122,
    0,    0,    0,    0,    0,    0,    0,    0,  123,  124,
  125,  126,    0,  127,    0, -179,    0,    0,    0,    0,
    0,    0,    0, -178,  130,  131,    0, -175, -174, -173,
 -172, -171,  137,    0,    0,  138,    0,    0,  139,    0,
    0,    0,    0,    0,    0,    0,    0,  140,  141,    0,
    0,  142,  143,  144,  145,  146,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,
};
short bxrindex[] = {                                      0,
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
  113,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 326
short bxtable[] = {                                      45,
   91,  156,   83,  101,   87,   89,  126,  176,  137,   84,
  181,  205,   81,  124,  136,   94,   93,   85,   98,   95,
   96,   97,  104,  105,  134,  102,  138,  106,  103,  131,
  142,  143,   92,  109,  110,  111,  112,  107,  157,  158,
  159,  160,  148,  149,  150,  151,  152,  153,  144,  145,
  164,  165,  215,  216,  108,  113,  114,  115,  116,  117,
  118,  119,  120,  121,  122,  139,  140,  141,  146,  162,
  163,  147,  155,  166,  167,  168,  169,  170,  171,  172,
  173,  174,  177,  179,  178,  182,  183,  184,  185,  186,
  187,  188,  189,  190,  191,  192,  193,  194,  195,  196,
  197,  198,  199,  200,  201,  202,  203,  206,  207,  208,
  209,  210,  211,  212,  213,  217,  214,  218,  219,  220,
  221,  222,  223,  224,  225,  226,  227,  228,  229,  230,
  231,  232,  233,  234,  235,  236,  237,  238,  239,  240,
  241,  242,  243,  244,  245,  246,  247,  248,  249,  250,
  251,  252,  253,  254,  255,  256,  161,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    1,    2,    0,    3,
    4,    5,    6,    7,    8,    9,    0,    0,    0,   10,
    0,   11,    0,   12,   99,   13,    0,    0,   14,   15,
    0,    0,    0,   16,   17,    0,  127,    0,    0,   18,
    0,   19,   20,   21,   22,   23,   24,   25,   26,   27,
   28,   29,   30,   31,  123,  135,   32,   33,    0,   34,
   35,   36,   37,   38,   39,   40,  128,   90,   41,   82,
  100,   86,   88,  125,  175,   42,   43,  180,  204,  132,
  133,   44,    0,    0,  129,  130,
};
short bxcheck[] = {                                      10,
   10,  291,   10,   10,   10,   10,   10,   10,  291,  273,
   10,   10,   10,   10,   10,  263,  317,  281,   10,  267,
  268,  269,  277,  278,   10,  317,  309,   10,  275,   10,
  282,  283,   42,  286,  287,  288,  289,   10,  328,  329,
  330,  331,  320,  321,  322,  323,  324,  325,  282,  283,
  282,  283,  282,  283,  317,  291,  317,  317,   10,   10,
   10,  318,  318,  291,  291,   10,  317,   10,  317,   10,
   61,  317,  317,   58,   10,   10,  317,   10,   10,   10,
   10,   10,   10,   10,  317,   10,  317,   10,   10,   10,
   10,   10,   10,  317,   10,   10,   10,   10,   10,   10,
   10,   10,  317,  317,  317,  317,   10,  291,   10,   10,
   10,   10,   10,   10,   10,   10,   58,  317,  317,  317,
  317,   58,  317,   10,   10,  317,   10,   10,  317,   10,
   10,   10,   10,   10,   10,   10,   10,  317,  317,   10,
   10,  317,  317,  317,  317,  317,   10,   10,   10,   10,
   10,   10,   10,   10,   10,   10,   44,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,   -1,  260,
  261,  262,  263,  264,  265,  266,   -1,   -1,   -1,  270,
   -1,  272,   -1,  274,  271,  276,   -1,   -1,  279,  280,
   -1,   -1,   -1,  284,  285,   -1,  257,   -1,   -1,  290,
   -1,  292,  293,  294,  295,  296,  297,  298,  299,  300,
  301,  302,  303,  304,  291,  291,  307,  308,   -1,  310,
  311,  312,  313,  314,  315,  316,  287,  317,  319,  317,
  317,  317,  317,  317,  317,  326,  327,  317,  317,  305,
  306,  332,   -1,   -1,  305,  306,
};
#define YYFINAL 46
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 332
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
"BX_TOKEN_VBREAKPOINT","BX_TOKEN_LBREAKPOINT","BX_TOKEN_PBREAKPOINT",
"BX_TOKEN_DEL_BREAKPOINT","BX_TOKEN_INFO","BX_TOKEN_QUIT","BX_TOKEN_PROGRAM",
"BX_TOKEN_REGISTERS","BX_TOKEN_DIRTY","BX_TOKEN_EXAMINE","BX_TOKEN_XFORMAT",
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
"disassemble_command : BX_TOKEN_DISASSEMBLE BX_TOKEN_NUMERIC BX_TOKEN_NUMERIC '\\n'",
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
case 35:
#line 137 "parser.y"
{
      }
break;
case 36:
#line 143 "parser.y"
{
		bx_dbg_diff_memory();
		free(yyvsp[-1].sval);
	}
break;
case 37:
#line 148 "parser.y"
{
		bx_dbg_sync_memory(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 38:
#line 153 "parser.y"
{
		bx_dbg_sync_memory(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 39:
#line 158 "parser.y"
{
		bx_dbg_sync_cpu(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 40:
#line 163 "parser.y"
{
		bx_dbg_sync_cpu(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	}
break;
case 41:
#line 168 "parser.y"
{
		free(yyvsp[-2].sval);
		bx_dbg_fast_forward(yyvsp[-1].uval);
	}
break;
case 42:
#line 173 "parser.y"
{
	}
break;
case 43:
#line 176 "parser.y"
{
		free(yyvsp[-4].sval);
		bx_dbg_info_address(yyvsp[-3].uval, yyvsp[-1].uval);
        }
break;
case 44:
#line 181 "parser.y"
{
        }
break;
case 45:
#line 184 "parser.y"
{
        }
break;
case 46:
#line 189 "parser.y"
{ yyval.uval = 1; }
break;
case 47:
#line 190 "parser.y"
{ yyval.uval = 0; }
break;
case 48:
#line 191 "parser.y"
{ yyval.uval = 2; }
break;
case 49:
#line 192 "parser.y"
{ yyval.uval = 3; }
break;
case 50:
#line 193 "parser.y"
{ yyval.uval = 4; }
break;
case 51:
#line 194 "parser.y"
{ yyval.uval = 5; }
break;
case 52:
#line 199 "parser.y"
{
        bx_dbg_timebp_command(0, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
break;
case 53:
#line 204 "parser.y"
{
        bx_dbg_timebp_command(1, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	}
break;
case 54:
#line 212 "parser.y"
{
          bx_dbg_record_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 55:
#line 220 "parser.y"
{
          bx_dbg_playback_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 56:
#line 228 "parser.y"
{
          bx_dbg_modebp_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 57:
#line 233 "parser.y"
{
          bx_dbg_modebp_command(0);
          free(yyvsp[-1].sval);
          }
break;
case 58:
#line 241 "parser.y"
{
          bx_dbg_show_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 59:
#line 246 "parser.y"
{
          bx_dbg_show_command(0);
          free(yyvsp[-1].sval);
          }
break;
case 60:
#line 254 "parser.y"
{
        bx_dbg_ptime_command();
        free(yyvsp[-1].sval);
	}
break;
case 61:
#line 262 "parser.y"
{
        bx_dbg_trace_on_command();
        free(yyvsp[-1].sval);
	}
break;
case 62:
#line 270 "parser.y"
{
        bx_dbg_trace_off_command();
        free(yyvsp[-1].sval);
	}
break;
case 63:
#line 278 "parser.y"
{
          bx_dbg_print_stack_command(16);
          free(yyvsp[-1].sval);
	  }
break;
case 64:
#line 283 "parser.y"
{
          bx_dbg_print_stack_command(yyvsp[-1].uval);
          free(yyvsp[-2].sval);
	  }
break;
case 65:
#line 291 "parser.y"
{
          watchpoint_continue = 0;
	  fprintf(stderr, "Will stop on watch points\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 66:
#line 297 "parser.y"
{
          watchpoint_continue = 1;
          fprintf(stderr, "Will not stop on watch points (they will still be logged)\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          }
break;
case 67:
#line 303 "parser.y"
{
          bx_dbg_watch(-1, 0);
          free(yyvsp[-1].sval);
          }
break;
case 68:
#line 308 "parser.y"
{
          bx_dbg_unwatch(-1, 0);
          free(yyvsp[-1].sval);
          }
break;
case 69:
#line 313 "parser.y"
{
          bx_dbg_watch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 70:
#line 318 "parser.y"
{
          bx_dbg_unwatch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 71:
#line 323 "parser.y"
{
          bx_dbg_watch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 72:
#line 328 "parser.y"
{
          bx_dbg_unwatch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          }
break;
case 73:
#line 336 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-1].sval, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 74:
#line 341 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-2].sval, 0, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 75:
#line 346 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-2].sval, 1, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 76:
#line 354 "parser.y"
{
        bx_dbg_where_command();
        free(yyvsp[-1].sval);
        }
break;
case 77:
#line 362 "parser.y"
{
        bx_dbg_print_string_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 78:
#line 370 "parser.y"
{
        bx_dbg_continue_command();
        free(yyvsp[-1].sval);
        }
break;
case 79:
#line 378 "parser.y"
{
        bx_dbg_stepN_command(1);
        free(yyvsp[-1].sval);
        }
break;
case 80:
#line 383 "parser.y"
{
        bx_dbg_stepN_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 81:
#line 391 "parser.y"
{
        bx_dbg_set_command(yyvsp[-3].sval, yyvsp[-2].sval, yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 82:
#line 396 "parser.y"
{
        bx_dbg_set_command(yyvsp[-3].sval, yyvsp[-2].sval, yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 83:
#line 401 "parser.y"
{
        bx_dbg_set_symbol_command(yyvsp[-3].sval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 84:
#line 409 "parser.y"
{
        bx_dbg_vbreakpoint_command(0, 0, 0);
        free(yyvsp[-1].sval);
        }
break;
case 85:
#line 414 "parser.y"
{
        bx_dbg_vbreakpoint_command(1, yyvsp[-3].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
break;
case 86:
#line 419 "parser.y"
{
        bx_dbg_lbreakpoint_command(0, 0);
        free(yyvsp[-1].sval);
        }
break;
case 87:
#line 424 "parser.y"
{
        bx_dbg_lbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 88:
#line 429 "parser.y"
{
        bx_dbg_pbreakpoint_command(0, 0);
        free(yyvsp[-1].sval);
        }
break;
case 89:
#line 434 "parser.y"
{
        bx_dbg_pbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 90:
#line 439 "parser.y"
{
        bx_dbg_pbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
break;
case 91:
#line 447 "parser.y"
{
        bx_dbg_info_bpoints_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 92:
#line 452 "parser.y"
{
        bx_dbg_info_program_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 93:
#line 457 "parser.y"
{
        bx_dbg_info_registers_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 94:
#line 462 "parser.y"
{
        bx_dbg_info_dirty_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 95:
#line 470 "parser.y"
{
        bx_dbg_dump_cpu_command();
        free(yyvsp[-1].sval);
        }
break;
case 96:
#line 478 "parser.y"
{
        bx_dbg_del_breakpoint_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 97:
#line 486 "parser.y"
{
	  bx_dbg_quit_command();
	  free(yyvsp[-1].sval);
        }
break;
case 98:
#line 495 "parser.y"
{
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].uval,1, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].uval,1, 1);
#endif
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 99:
#line 503 "parser.y"
{
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 1);
#endif
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 100:
#line 511 "parser.y"
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
case 101:
#line 524 "parser.y"
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
case 102:
#line 540 "parser.y"
{
        bx_dbg_setpmem_command(yyvsp[-3].uval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
break;
case 103:
#line 548 "parser.y"
{
        bx_dbg_query_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 104:
#line 556 "parser.y"
{
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 105:
#line 561 "parser.y"
{
        bx_dbg_take_command(yyvsp[-2].sval, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        }
break;
case 106:
#line 566 "parser.y"
{
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 107:
#line 574 "parser.y"
{
        bx_dbg_set_cpu_command();
        free(yyvsp[-1].sval);
        }
break;
case 108:
#line 582 "parser.y"
{
        bx_dbg_disassemble_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
break;
case 109:
#line 590 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 110:
#line 595 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 111:
#line 600 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 112:
#line 605 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 113:
#line 613 "parser.y"
{
        bx_dbg_loader_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 114:
#line 621 "parser.y"
{
        bx_dbg_doit_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        }
break;
case 115:
#line 629 "parser.y"
{
        bx_dbg_crc_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        }
break;
case 116:
#line 637 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 117:
#line 642 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 118:
#line 647 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 119:
#line 652 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        }
break;
case 120:
#line 657 "parser.y"
{
        bx_dbg_maths_expression_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        }
break;
case 121:
#line 664 "parser.y"
{
        bx_dbg_v2l_command(yyvsp[-3].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        }
break;
#line 1254 "y.tab.c"
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
