
/*  A Bison parser, made from parser.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse bxparse
#define yylex bxlex
#define yyerror bxerror
#define yylval bxlval
#define yychar bxchar
#define yydebug bxdebug
#define yynerrs bxnerrs
#define	BX_TOKEN_CONTINUE	257
#define	BX_TOKEN_STEPN	258
#define	BX_TOKEN_NEXT_STEP	259
#define	BX_TOKEN_SET	260
#define	BX_TOKEN_DEBUGGER	261
#define	BX_TOKEN_VBREAKPOINT	262
#define	BX_TOKEN_LBREAKPOINT	263
#define	BX_TOKEN_PBREAKPOINT	264
#define	BX_TOKEN_DEL_BREAKPOINT	265
#define	BX_TOKEN_INFO	266
#define	BX_TOKEN_QUIT	267
#define	BX_TOKEN_PROGRAM	268
#define	BX_TOKEN_REGISTERS	269
#define	BX_TOKEN_FPU	270
#define	BX_TOKEN_ALL	271
#define	BX_TOKEN_IDT	272
#define	BX_TOKEN_GDT	273
#define	BX_TOKEN_LDT	274
#define	BX_TOKEN_TSS	275
#define	BX_TOKEN_DIRTY	276
#define	BX_TOKEN_LINUX	277
#define	BX_TOKEN_CONTROL_REGS	278
#define	BX_TOKEN_EXAMINE	279
#define	BX_TOKEN_XFORMAT	280
#define	BX_TOKEN_SETPMEM	281
#define	BX_TOKEN_SYMBOLNAME	282
#define	BX_TOKEN_QUERY	283
#define	BX_TOKEN_PENDING	284
#define	BX_TOKEN_TAKE	285
#define	BX_TOKEN_DMA	286
#define	BX_TOKEN_IRQ	287
#define	BX_TOKEN_DUMP_CPU	288
#define	BX_TOKEN_SET_CPU	289
#define	BX_TOKEN_DIS	290
#define	BX_TOKEN_ON	291
#define	BX_TOKEN_OFF	292
#define	BX_TOKEN_DISASSEMBLE	293
#define	BX_TOKEN_INSTRUMENT	294
#define	BX_TOKEN_START	295
#define	BX_TOKEN_STOP	296
#define	BX_TOKEN_RESET	297
#define	BX_TOKEN_PRINT	298
#define	BX_TOKEN_LOADER	299
#define	BX_TOKEN_STRING	300
#define	BX_TOKEN_DOIT	301
#define	BX_TOKEN_CRC	302
#define	BX_TOKEN_TRACEON	303
#define	BX_TOKEN_TRACEOFF	304
#define	BX_TOKEN_PTIME	305
#define	BX_TOKEN_TIMEBP_ABSOLUTE	306
#define	BX_TOKEN_TIMEBP	307
#define	BX_TOKEN_RECORD	308
#define	BX_TOKEN_PLAYBACK	309
#define	BX_TOKEN_MODEBP	310
#define	BX_TOKEN_PRINT_STACK	311
#define	BX_TOKEN_WATCH	312
#define	BX_TOKEN_UNWATCH	313
#define	BX_TOKEN_READ	314
#define	BX_TOKEN_WRITE	315
#define	BX_TOKEN_SHOW	316
#define	BX_TOKEN_SYMBOL	317
#define	BX_TOKEN_GLOBAL	318
#define	BX_TOKEN_WHERE	319
#define	BX_TOKEN_PRINT_STRING	320
#define	BX_TOKEN_DIFF_MEMORY	321
#define	BX_TOKEN_SYNC_MEMORY	322
#define	BX_TOKEN_SYNC_CPU	323
#define	BX_TOKEN_FAST_FORWARD	324
#define	BX_TOKEN_PHY_2_LOG	325
#define	BX_TOKEN_NUMERIC	326
#define	BX_TOKEN_LONG_NUMERIC	327
#define	BX_TOKEN_INFO_ADDRESS	328
#define	BX_TOKEN_NE2000	329
#define	BX_TOKEN_PAGE	330
#define	BX_TOKEN_CS	331
#define	BX_TOKEN_ES	332
#define	BX_TOKEN_SS	333
#define	BX_TOKEN_DS	334
#define	BX_TOKEN_FS	335
#define	BX_TOKEN_GS	336
#define	BX_TOKEN_ALWAYS_CHECK	337
#define	BX_TOKEN_MATHS	338
#define	BX_TOKEN_ADD	339
#define	BX_TOKEN_SUB	340
#define	BX_TOKEN_MUL	341
#define	BX_TOKEN_DIV	342
#define	BX_TOKEN_V2L	343
#define	BX_TOKEN_TRACEREGON	344
#define	BX_TOKEN_TRACEREGOFF	345

#line 5 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
// %left '-' '+'
// %left '*' '/'
// %right
// %nonassoc UMINUS

#line 16 "parser.y"
typedef union {
  char    *sval;
  Bit32u   uval;
  Bit64u   ulval;
  bx_num_range   uval_range;
  } YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		296
#define	YYFLAG		-32768
#define	YYNTBASE	96

#define YYTRANSLATE(x) ((unsigned)(x) <= 345 ? yytranslate[x] : 136)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    92,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,    95,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    93,     2,     2,
    94,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
    87,    88,    89,    90,    91
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     6,     8,    10,    12,    14,    16,    18,
    20,    22,    24,    26,    28,    30,    32,    34,    36,    38,
    40,    42,    44,    46,    48,    50,    52,    54,    56,    58,
    60,    62,    64,    66,    68,    70,    72,    73,    75,    78,
    82,    86,    90,    94,    98,   102,   108,   113,   118,   120,
   122,   124,   126,   128,   130,   134,   138,   142,   146,   150,
   153,   157,   160,   163,   166,   169,   172,   176,   180,   184,
   187,   190,   195,   200,   205,   210,   214,   219,   225,   228,
   232,   235,   238,   242,   247,   252,   258,   261,   267,   270,
   274,   277,   281,   286,   290,   294,   298,   302,   306,   310,
   315,   320,   325,   330,   334,   338,   342,   348,   356,   357,
   359,   360,   362,   364,   367,   371,   374,   378,   381,   386,
   390,   394,   397,   403,   407,   411,   416,   420,   423,   427,
   431,   435,   439,   443,   447,   451,   456,   462,   468,   474,
   480,   484,   490,   493
};

static const short yyrhs[] = {   112,
     0,   113,     0,   114,     0,   115,     0,   116,     0,   119,
     0,   120,     0,   121,     0,   122,     0,   123,     0,   124,
     0,   125,     0,   126,     0,   127,     0,   128,     0,   129,
     0,   130,     0,   131,     0,   132,     0,   105,     0,   106,
     0,   104,     0,    99,     0,   100,     0,   101,     0,   102,
     0,   107,     0,   108,     0,   103,     0,   109,     0,   110,
     0,   111,     0,    97,     0,   133,     0,   134,     0,   135,
     0,     0,    92,     0,    67,    92,     0,    68,    37,    92,
     0,    68,    38,    92,     0,    69,    37,    92,     0,    69,
    38,    92,     0,    70,    72,    92,     0,    71,    72,    92,
     0,    74,    98,    93,    72,    92,     0,    83,    72,    37,
    92,     0,    83,    72,    38,    92,     0,    77,     0,    78,
     0,    79,     0,    80,     0,    81,     0,    82,     0,    53,
    73,    92,     0,    52,    73,    92,     0,    54,    46,    92,
     0,    55,    46,    92,     0,    56,    46,    92,     0,    56,
    92,     0,    62,    46,    92,     0,    62,    92,     0,    51,
    92,     0,    49,    92,     0,    50,    92,     0,    57,    92,
     0,    57,    72,    92,     0,    58,    42,    92,     0,    58,
     3,    92,     0,    58,    92,     0,    59,    92,     0,    58,
    60,    72,    92,     0,    59,    60,    72,    92,     0,    58,
    61,    72,    92,     0,    59,    61,    72,    92,     0,    63,
    46,    92,     0,    63,    46,    72,    92,     0,    63,    64,
    46,    72,    92,     0,    65,    92,     0,    66,    72,    92,
     0,     3,    92,     0,     4,    92,     0,     4,    72,    92,
     0,     6,    36,    37,    92,     0,     6,    36,    38,    92,
     0,     6,    28,    94,    72,    92,     0,     8,    92,     0,
     8,    72,    93,    72,    92,     0,     9,    92,     0,     9,
    72,    92,     0,    10,    92,     0,    10,    72,    92,     0,
    10,    95,    72,    92,     0,    12,    10,    92,     0,    12,
    14,    92,     0,    12,    15,    92,     0,    12,    16,    92,
     0,    12,    17,    92,     0,    12,    22,    92,     0,    12,
    18,   117,    92,     0,    12,    19,   117,    92,     0,    12,
    20,   117,    92,     0,    12,    21,   117,    92,     0,    12,
    23,    92,     0,    12,    24,    92,     0,    12,    75,    92,
     0,    12,    75,    76,    72,    92,     0,    12,    75,    76,
    72,    15,    72,    92,     0,     0,    72,     0,     0,   118,
     0,    72,     0,    72,    72,     0,    72,    93,    72,     0,
    34,    92,     0,    11,    72,    92,     0,    13,    92,     0,
    25,    26,    72,    92,     0,    25,    26,    92,     0,    25,
    72,    92,     0,    25,    92,     0,    27,    72,    72,    72,
    92,     0,    29,    30,    92,     0,    31,    32,    92,     0,
    31,    32,    72,    92,     0,    31,    33,    92,     0,    35,
    92,     0,    39,   117,    92,     0,    40,    41,    92,     0,
    40,    42,    92,     0,    40,    43,    92,     0,    40,    44,
    92,     0,    45,    46,    92,     0,    47,    72,    92,     0,
    48,    72,    72,    92,     0,    84,    85,    72,    72,    92,
     0,    84,    86,    72,    72,    92,     0,    84,    87,    72,
    72,    92,     0,    84,    88,    72,    72,    92,     0,    84,
    46,    92,     0,    89,    98,    93,    72,    92,     0,    90,
    92,     0,    91,    92,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   119,   121,   122,   123,   124,   125,   126,   127,   128,   129,
   130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
   140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
   150,   151,   152,   153,   154,   155,   156,   157,   162,   168,
   173,   178,   183,   188,   193,   196,   201,   204,   209,   211,
   212,   213,   214,   215,   218,   224,   231,   239,   247,   253,
   260,   266,   273,   281,   289,   297,   303,   310,   317,   323,
   328,   333,   338,   343,   348,   355,   361,   366,   373,   381,
   389,   397,   403,   410,   416,   421,   428,   434,   439,   444,
   449,   454,   459,   466,   472,   477,   482,   487,   492,   497,
   502,   507,   512,   517,   522,   527,   532,   537,   544,   546,
   548,   550,   552,   557,   562,   569,   577,   585,   594,   603,
   611,   624,   639,   647,   655,   661,   666,   673,   681,   689,
   695,   700,   705,   712,   720,   728,   736,   742,   747,   752,
   757,   763,   771,   779
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","BX_TOKEN_CONTINUE",
"BX_TOKEN_STEPN","BX_TOKEN_NEXT_STEP","BX_TOKEN_SET","BX_TOKEN_DEBUGGER","BX_TOKEN_VBREAKPOINT",
"BX_TOKEN_LBREAKPOINT","BX_TOKEN_PBREAKPOINT","BX_TOKEN_DEL_BREAKPOINT","BX_TOKEN_INFO",
"BX_TOKEN_QUIT","BX_TOKEN_PROGRAM","BX_TOKEN_REGISTERS","BX_TOKEN_FPU","BX_TOKEN_ALL",
"BX_TOKEN_IDT","BX_TOKEN_GDT","BX_TOKEN_LDT","BX_TOKEN_TSS","BX_TOKEN_DIRTY",
"BX_TOKEN_LINUX","BX_TOKEN_CONTROL_REGS","BX_TOKEN_EXAMINE","BX_TOKEN_XFORMAT",
"BX_TOKEN_SETPMEM","BX_TOKEN_SYMBOLNAME","BX_TOKEN_QUERY","BX_TOKEN_PENDING",
"BX_TOKEN_TAKE","BX_TOKEN_DMA","BX_TOKEN_IRQ","BX_TOKEN_DUMP_CPU","BX_TOKEN_SET_CPU",
"BX_TOKEN_DIS","BX_TOKEN_ON","BX_TOKEN_OFF","BX_TOKEN_DISASSEMBLE","BX_TOKEN_INSTRUMENT",
"BX_TOKEN_START","BX_TOKEN_STOP","BX_TOKEN_RESET","BX_TOKEN_PRINT","BX_TOKEN_LOADER",
"BX_TOKEN_STRING","BX_TOKEN_DOIT","BX_TOKEN_CRC","BX_TOKEN_TRACEON","BX_TOKEN_TRACEOFF",
"BX_TOKEN_PTIME","BX_TOKEN_TIMEBP_ABSOLUTE","BX_TOKEN_TIMEBP","BX_TOKEN_RECORD",
"BX_TOKEN_PLAYBACK","BX_TOKEN_MODEBP","BX_TOKEN_PRINT_STACK","BX_TOKEN_WATCH",
"BX_TOKEN_UNWATCH","BX_TOKEN_READ","BX_TOKEN_WRITE","BX_TOKEN_SHOW","BX_TOKEN_SYMBOL",
"BX_TOKEN_GLOBAL","BX_TOKEN_WHERE","BX_TOKEN_PRINT_STRING","BX_TOKEN_DIFF_MEMORY",
"BX_TOKEN_SYNC_MEMORY","BX_TOKEN_SYNC_CPU","BX_TOKEN_FAST_FORWARD","BX_TOKEN_PHY_2_LOG",
"BX_TOKEN_NUMERIC","BX_TOKEN_LONG_NUMERIC","BX_TOKEN_INFO_ADDRESS","BX_TOKEN_NE2000",
"BX_TOKEN_PAGE","BX_TOKEN_CS","BX_TOKEN_ES","BX_TOKEN_SS","BX_TOKEN_DS","BX_TOKEN_FS",
"BX_TOKEN_GS","BX_TOKEN_ALWAYS_CHECK","BX_TOKEN_MATHS","BX_TOKEN_ADD","BX_TOKEN_SUB",
"BX_TOKEN_MUL","BX_TOKEN_DIV","BX_TOKEN_V2L","BX_TOKEN_TRACEREGON","BX_TOKEN_TRACEREGOFF",
"'\\n'","':'","'='","'*'","command","cosim_commands","segment_register","timebp_command",
"record_command","playback_command","modebp_command","show_command","ptime_command",
"trace_on_command","trace_off_command","print_stack_command","watch_point_command",
"symbol_command","where_command","print_string_command","continue_command","stepN_command",
"set_command","breakpoint_command","info_command","optional_numeric_range","numeric_range",
"dump_cpu_command","delete_command","quit_command","examine_command","setpmem_command",
"query_command","take_command","set_cpu_command","disassemble_command","instrument_command",
"loader_command","doit_command","crc_command","maths_command","v2l_command",
"trace_reg_on_command","trace_reg_off_command", NULL
};
#endif

static const short yyr1[] = {     0,
    96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
    96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
    96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
    96,    96,    96,    96,    96,    96,    96,    96,    97,    97,
    97,    97,    97,    97,    97,    97,    97,    97,    98,    98,
    98,    98,    98,    98,    99,    99,   100,   101,   102,   102,
   103,   103,   104,   105,   106,   107,   107,   108,   108,   108,
   108,   108,   108,   108,   108,   109,   109,   109,   110,   111,
   112,   113,   113,   114,   114,   114,   115,   115,   115,   115,
   115,   115,   115,   116,   116,   116,   116,   116,   116,   116,
   116,   116,   116,   116,   116,   116,   116,   116,    -1,    -1,
   117,   117,   118,   118,   118,   119,   120,   121,   122,   122,
   122,   122,   123,   124,   125,   125,   125,   126,   127,   128,
   128,   128,   128,   129,   130,   131,   132,   132,   132,   132,
   132,   133,   134,   135
};

static const short yyr2[] = {     0,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     0,     1,     2,     3,
     3,     3,     3,     3,     3,     5,     4,     4,     1,     1,
     1,     1,     1,     1,     3,     3,     3,     3,     3,     2,
     3,     2,     2,     2,     2,     2,     3,     3,     3,     2,
     2,     4,     4,     4,     4,     3,     4,     5,     2,     3,
     2,     2,     3,     4,     4,     5,     2,     5,     2,     3,
     2,     3,     4,     3,     3,     3,     3,     3,     3,     4,
     4,     4,     4,     3,     3,     3,     5,     7,     0,     1,
     0,     1,     1,     2,     3,     2,     3,     2,     4,     3,
     3,     2,     5,     3,     3,     4,     3,     2,     3,     3,
     3,     3,     3,     3,     3,     4,     5,     5,     5,     5,
     3,     5,     2,     2
};

static const short yydefact[] = {    37,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   111,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    38,    33,    23,    24,
    25,    26,    29,    22,    20,    21,    27,    28,    30,    31,
    32,     1,     2,     3,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
    34,    35,    36,    81,     0,    82,     0,     0,     0,    87,
     0,    89,     0,    91,     0,     0,     0,     0,     0,     0,
     0,   111,   111,   111,   111,     0,     0,     0,     0,   118,
     0,     0,   122,     0,     0,     0,     0,   116,   128,   113,
     0,   112,     0,     0,     0,     0,     0,     0,     0,    64,
    65,    63,     0,     0,     0,     0,     0,    60,     0,    66,
     0,     0,     0,     0,    70,     0,     0,    71,     0,    62,
     0,     0,    79,     0,    39,     0,     0,     0,     0,     0,
     0,    49,    50,    51,    52,    53,    54,     0,     0,     0,
     0,     0,     0,     0,     0,   143,   144,    83,     0,     0,
     0,     0,    90,    92,     0,   117,    94,    95,    96,    97,
    98,     0,     0,     0,     0,    99,   104,   105,     0,   106,
     0,   120,   121,     0,   124,     0,   125,   127,   114,     0,
   129,   130,   131,   132,   133,   134,   135,     0,    56,    55,
    57,    58,    59,    67,    69,    68,     0,     0,     0,     0,
    61,     0,    76,     0,    80,    40,    41,    42,    43,    44,
    45,     0,     0,     0,   141,     0,     0,     0,     0,     0,
     0,    84,    85,     0,    93,   100,   101,   102,   103,     0,
   119,     0,   126,   115,   136,    72,    74,    73,    75,    77,
     0,     0,    47,    48,     0,     0,     0,     0,     0,    86,
    88,     0,   107,   123,    78,    46,   137,   138,   139,   140,
   142,     0,   108,     0,     0,     0
};

static const short yydefgoto[] = {   294,
    48,   168,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
   121,   122,    67,    68,    69,    70,    71,    72,    73,    74,
    75,    76,    77,    78,    79,    80,    81,    82,    83
};

static const short yypact[] = {    -3,
   -57,   -59,     2,   -58,   -53,    13,     5,    80,   -18,   -14,
    12,    91,   -16,    43,    44,    65,    81,    92,    67,    68,
    49,    50,    51,    71,    72,   100,   101,   -31,     1,    15,
   -20,   -23,   -35,    56,    77,    58,   -17,    45,    79,    82,
    38,    84,    24,    38,    60,    61,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    66,-32768,    63,    54,    69,-32768,
    73,-32768,    74,-32768,    87,    75,    76,    78,    83,    85,
    86,    65,    65,    65,    65,    88,    89,    90,   -49,-32768,
    34,    93,-32768,    97,    95,    41,    96,-32768,-32768,   -68,
    98,-32768,    99,   102,   103,   104,   105,   106,   107,-32768,
-32768,-32768,   108,   109,   110,   111,   112,-32768,   113,-32768,
   114,   115,   117,   120,-32768,   121,   127,-32768,   116,-32768,
    42,   118,-32768,   119,-32768,   122,   123,   124,   125,   126,
   128,-32768,-32768,-32768,-32768,-32768,-32768,    70,    94,   129,
   137,   138,   140,   141,   130,-32768,-32768,-32768,   147,   132,
   133,   150,-32768,-32768,   134,-32768,-32768,-32768,-32768,-32768,
-32768,   135,   136,   139,   142,-32768,-32768,-32768,   157,-32768,
   143,-32768,-32768,   158,-32768,   144,-32768,-32768,-32768,   160,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   145,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   146,   148,   149,   151,
-32768,   152,-32768,   161,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   167,   153,   154,-32768,   170,   175,   176,   177,   178,
   159,-32768,-32768,   162,-32768,-32768,-32768,-32768,-32768,   -13,
-32768,   163,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   164,   165,-32768,-32768,   166,   168,   169,   171,   172,-32768,
-32768,   180,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   173,-32768,   174,   183,-32768
};

static const short yypgoto[] = {-32768,
-32768,   209,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    25,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768
};


#define	YYLAST		265


static const short yytable[] = {     1,
     2,   282,     3,   209,     4,     5,     6,     7,     8,     9,
   151,   111,    85,    89,   137,   116,   117,   141,    91,   156,
   157,    10,   149,    11,   210,    12,   199,    13,   152,    87,
    14,    15,    86,    90,    84,    16,    17,    88,    92,   146,
   147,    18,   200,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,   142,   112,    32,    33,
   138,    34,    35,    36,    37,    38,    39,    40,   150,   170,
    41,   148,   139,   110,   143,   144,    96,   113,   283,    42,
    43,   158,   159,   114,    93,    44,    45,    46,    47,    97,
   180,   181,   140,    98,    99,   100,   101,   102,   103,   104,
   105,   106,   107,   108,    94,   201,   145,    95,   171,   172,
   173,   174,   206,   232,   162,   163,   164,   165,   166,   167,
   115,   123,   124,   125,   126,   202,   192,   193,   194,   195,
   243,   244,   207,   233,   118,   119,   120,   127,   128,   129,
   130,   131,   132,   133,   134,   135,   136,   153,   154,   155,
   160,   176,   177,   161,   109,   169,   179,   178,   185,     0,
     0,   182,   242,   234,   183,   184,   186,   187,   204,   188,
     0,     0,     0,   295,   189,     0,   190,   191,   218,   196,
   197,   198,   296,     0,   203,     0,   205,   208,   227,   211,
   212,   228,   229,   213,   214,   215,   216,   217,   230,   219,
   220,   221,   222,   223,   224,   225,   226,   231,   246,   247,
   235,   248,   249,   236,   237,   238,   239,   240,   251,   241,
   245,   254,   250,   252,   253,   255,   256,   257,   260,   262,
   258,   264,   271,   259,   261,   263,   265,   266,   272,   267,
   268,   275,   269,   270,   273,   274,   276,   277,   278,   279,
   280,   292,   175,   281,   284,   285,   286,   287,     0,   288,
   289,     0,   290,   291,   293
};

static const short yycheck[] = {     3,
     4,    15,     6,    72,     8,     9,    10,    11,    12,    13,
    46,    26,    72,    72,    46,    32,    33,     3,    72,    37,
    38,    25,    46,    27,    93,    29,    76,    31,    64,    28,
    34,    35,    92,    92,    92,    39,    40,    36,    92,    60,
    61,    45,    92,    47,    48,    49,    50,    51,    52,    53,
    54,    55,    56,    57,    58,    59,    42,    72,    62,    63,
    92,    65,    66,    67,    68,    69,    70,    71,    92,    46,
    74,    92,    72,    92,    60,    61,    72,    92,    92,    83,
    84,    37,    38,    72,    72,    89,    90,    91,    92,    10,
    37,    38,    92,    14,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    92,    72,    92,    95,    85,    86,
    87,    88,    72,    72,    77,    78,    79,    80,    81,    82,
    30,    41,    42,    43,    44,    92,   102,   103,   104,   105,
    37,    38,    92,    92,    92,    92,    72,    46,    72,    72,
    92,    92,    92,    73,    73,    46,    46,    92,    72,    92,
    72,    92,    92,    72,    75,    72,    94,    92,    72,    -1,
    -1,    93,    93,    46,    92,    92,    92,    92,    72,    92,
    -1,    -1,    -1,     0,    92,    -1,    92,    92,    72,    92,
    92,    92,     0,    -1,    92,    -1,    92,    92,    72,    92,
    92,    72,    72,    92,    92,    92,    92,    92,    72,    92,
    92,    92,    92,    92,    92,    92,    92,    92,    72,    72,
    92,    72,    72,    92,    92,    92,    92,    92,    72,    92,
    92,    72,    93,    92,    92,    92,    92,    92,    72,    72,
    92,    72,    72,    92,    92,    92,    92,    92,    72,    92,
    92,    72,    92,    92,    92,    92,    72,    72,    72,    72,
    92,    72,    44,    92,    92,    92,    92,    92,    -1,    92,
    92,    -1,    92,    92,    92
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 38:
#line 158 "parser.y"
{
      ;
    break;}
case 39:
#line 164 "parser.y"
{
		bx_dbg_diff_memory();
		free(yyvsp[-1].sval);
	;
    break;}
case 40:
#line 169 "parser.y"
{
		bx_dbg_sync_memory(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	;
    break;}
case 41:
#line 174 "parser.y"
{
		bx_dbg_sync_memory(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	;
    break;}
case 42:
#line 179 "parser.y"
{
		bx_dbg_sync_cpu(1);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	;
    break;}
case 43:
#line 184 "parser.y"
{
		bx_dbg_sync_cpu(0);
		free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	;
    break;}
case 44:
#line 189 "parser.y"
{
		free(yyvsp[-2].sval);
		bx_dbg_fast_forward(yyvsp[-1].uval);
	;
    break;}
case 45:
#line 194 "parser.y"
{
	;
    break;}
case 46:
#line 197 "parser.y"
{
		free(yyvsp[-4].sval);
		bx_dbg_info_address(yyvsp[-3].uval, yyvsp[-1].uval);
        ;
    break;}
case 47:
#line 202 "parser.y"
{
        ;
    break;}
case 48:
#line 205 "parser.y"
{
        ;
    break;}
case 49:
#line 210 "parser.y"
{ yyval.uval = 1; ;
    break;}
case 50:
#line 211 "parser.y"
{ yyval.uval = 0; ;
    break;}
case 51:
#line 212 "parser.y"
{ yyval.uval = 2; ;
    break;}
case 52:
#line 213 "parser.y"
{ yyval.uval = 3; ;
    break;}
case 53:
#line 214 "parser.y"
{ yyval.uval = 4; ;
    break;}
case 54:
#line 215 "parser.y"
{ yyval.uval = 5; ;
    break;}
case 55:
#line 220 "parser.y"
{
        bx_dbg_timebp_command(0, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	;
    break;}
case 56:
#line 225 "parser.y"
{
        bx_dbg_timebp_command(1, yyvsp[-1].ulval);
	free(yyvsp[-2].sval);
	;
    break;}
case 57:
#line 233 "parser.y"
{
          bx_dbg_record_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          ;
    break;}
case 58:
#line 241 "parser.y"
{
          bx_dbg_playback_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          ;
    break;}
case 59:
#line 249 "parser.y"
{
          bx_dbg_modebp_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          ;
    break;}
case 60:
#line 254 "parser.y"
{
          bx_dbg_modebp_command(0);
          free(yyvsp[-1].sval);
          ;
    break;}
case 61:
#line 262 "parser.y"
{
          bx_dbg_show_command(yyvsp[-1].sval);
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          ;
    break;}
case 62:
#line 267 "parser.y"
{
          bx_dbg_show_command(0);
          free(yyvsp[-1].sval);
          ;
    break;}
case 63:
#line 275 "parser.y"
{
        bx_dbg_ptime_command();
        free(yyvsp[-1].sval);
	;
    break;}
case 64:
#line 283 "parser.y"
{
        bx_dbg_trace_on_command();
        free(yyvsp[-1].sval);
	;
    break;}
case 65:
#line 291 "parser.y"
{
        bx_dbg_trace_off_command();
        free(yyvsp[-1].sval);
	;
    break;}
case 66:
#line 299 "parser.y"
{
          bx_dbg_print_stack_command(16);
          free(yyvsp[-1].sval);
	  ;
    break;}
case 67:
#line 304 "parser.y"
{
          bx_dbg_print_stack_command(yyvsp[-1].uval);
          free(yyvsp[-2].sval);
	  ;
    break;}
case 68:
#line 312 "parser.y"
{
          watchpoint_continue = 0;
	  fprintf(stderr, "Will stop on watch points\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          ;
    break;}
case 69:
#line 318 "parser.y"
{
          watchpoint_continue = 1;
          fprintf(stderr, "Will not stop on watch points (they will still be logged)\n");
          free(yyvsp[-2].sval); free(yyvsp[-1].sval);
          ;
    break;}
case 70:
#line 324 "parser.y"
{
          bx_dbg_watch(-1, 0);
          free(yyvsp[-1].sval);
          ;
    break;}
case 71:
#line 329 "parser.y"
{
          bx_dbg_unwatch(-1, 0);
          free(yyvsp[-1].sval);
          ;
    break;}
case 72:
#line 334 "parser.y"
{
          bx_dbg_watch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          ;
    break;}
case 73:
#line 339 "parser.y"
{
          bx_dbg_unwatch(1, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          ;
    break;}
case 74:
#line 344 "parser.y"
{
          bx_dbg_watch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          ;
    break;}
case 75:
#line 349 "parser.y"
{
          bx_dbg_unwatch(0, yyvsp[-1].uval);
          free(yyvsp[-3].sval); free(yyvsp[-2].sval);
          ;
    break;}
case 76:
#line 357 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-1].sval, 0, 0);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 77:
#line 362 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-2].sval, 0, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        ;
    break;}
case 78:
#line 367 "parser.y"
{
	bx_dbg_symbol_command(yyvsp[-2].sval, 1, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        ;
    break;}
case 79:
#line 375 "parser.y"
{
        bx_dbg_where_command();
        free(yyvsp[-1].sval);
        ;
    break;}
case 80:
#line 383 "parser.y"
{
        bx_dbg_print_string_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        ;
    break;}
case 81:
#line 391 "parser.y"
{
        bx_dbg_continue_command();
        free(yyvsp[-1].sval);
        ;
    break;}
case 82:
#line 399 "parser.y"
{
        bx_dbg_stepN_command(1);
        free(yyvsp[-1].sval);
        ;
    break;}
case 83:
#line 404 "parser.y"
{
        bx_dbg_stepN_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        ;
    break;}
case 84:
#line 412 "parser.y"
{
        bx_dbg_set_command(yyvsp[-3].sval, yyvsp[-2].sval, yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 85:
#line 417 "parser.y"
{
        bx_dbg_set_command(yyvsp[-3].sval, yyvsp[-2].sval, yyvsp[-1].sval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 86:
#line 422 "parser.y"
{
        bx_dbg_set_symbol_command(yyvsp[-3].sval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        ;
    break;}
case 87:
#line 430 "parser.y"
{
        bx_dbg_vbreakpoint_command(0, 0, 0);
        free(yyvsp[-1].sval);
        ;
    break;}
case 88:
#line 435 "parser.y"
{
        bx_dbg_vbreakpoint_command(1, yyvsp[-3].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        ;
    break;}
case 89:
#line 440 "parser.y"
{
        bx_dbg_lbreakpoint_command(0, 0);
        free(yyvsp[-1].sval);
        ;
    break;}
case 90:
#line 445 "parser.y"
{
        bx_dbg_lbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        ;
    break;}
case 91:
#line 450 "parser.y"
{
        bx_dbg_pbreakpoint_command(0, 0);
        free(yyvsp[-1].sval);
        ;
    break;}
case 92:
#line 455 "parser.y"
{
        bx_dbg_pbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        ;
    break;}
case 93:
#line 460 "parser.y"
{
        bx_dbg_pbreakpoint_command(1, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        ;
    break;}
case 94:
#line 468 "parser.y"
{
        bx_dbg_info_bpoints_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 95:
#line 473 "parser.y"
{
        bx_dbg_info_program_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 96:
#line 478 "parser.y"
{
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 97:
#line 483 "parser.y"
{
        bx_dbg_info_registers_command(BX_INFO_FPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 98:
#line 488 "parser.y"
{
        bx_dbg_info_registers_command(BX_INFO_CPU_REGS | BX_INFO_FPU_REGS);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 99:
#line 493 "parser.y"
{
        bx_dbg_info_dirty_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
	;
    break;}
case 100:
#line 498 "parser.y"
{
        bx_dbg_info_idt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        ;
    break;}
case 101:
#line 503 "parser.y"
{
        bx_dbg_info_gdt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        ;
    break;}
case 102:
#line 508 "parser.y"
{
        bx_dbg_info_ldt_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        ;
    break;}
case 103:
#line 513 "parser.y"
{
        bx_dbg_info_tss_command(yyvsp[-1].uval_range);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        ;
    break;}
case 104:
#line 518 "parser.y"
{
        bx_dbg_info_linux_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 105:
#line 523 "parser.y"
{
        bx_dbg_info_control_regs_command();
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 106:
#line 528 "parser.y"
{
        bx_dbg_info_ne2k(-1, -1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 107:
#line 533 "parser.y"
{
        free(yyvsp[-4].sval); free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-1].uval, -1);
        ;
    break;}
case 108:
#line 538 "parser.y"
{
        free(yyvsp[-6].sval); free(yyvsp[-5].sval); free(yyvsp[-4].sval); free(yyvsp[-2].sval);
        bx_dbg_info_ne2k(yyvsp[-3].uval, yyvsp[-1].uval);
        ;
    break;}
case 109:
#line 545 "parser.y"
{ yyval.uval = EMPTY_ARG; ;
    break;}
case 111:
#line 549 "parser.y"
{ yyval.uval_range = make_num_range (EMPTY_ARG, EMPTY_ARG); ;
    break;}
case 113:
#line 554 "parser.y"
{
    yyval.uval_range = make_num_range (yyvsp[0].uval, yyvsp[0].uval);
  ;
    break;}
case 114:
#line 559 "parser.y"
{
    yyval.uval_range = make_num_range (yyvsp[-1].uval, yyvsp[0].uval);
  ;
    break;}
case 115:
#line 564 "parser.y"
{
    yyval.uval_range = make_num_range (yyvsp[-2].uval, yyvsp[0].uval);
  ;
    break;}
case 116:
#line 571 "parser.y"
{
        bx_dbg_dump_cpu_command();
        free(yyvsp[-1].sval);
        ;
    break;}
case 117:
#line 579 "parser.y"
{
        bx_dbg_del_breakpoint_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        ;
    break;}
case 118:
#line 587 "parser.y"
{
	  bx_dbg_quit_command();
	  free(yyvsp[-1].sval);
        ;
    break;}
case 119:
#line 596 "parser.y"
{
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].uval,1, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-3].sval, yyvsp[-2].sval,1, yyvsp[-1].uval,1, 1);
#endif
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        ;
    break;}
case 120:
#line 604 "parser.y"
{
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 0);
#if BX_NUM_SIMULATORS >= 2
        bx_dbg_examine_command(yyvsp[-2].sval, yyvsp[-1].sval,1, 0,0, 1);
#endif
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 121:
#line 612 "parser.y"
{
        //FIXME HanishKVC This method of hunting thro all the 
        //      simulators may be better than using 2 calls if 
        //      BX_NUM_SIMULATORS greater than or equal to 2 as 
        //      done for other cases of BX_TOKEN_EXAMINE
        int iCurSim; 
        for(iCurSim = 0; iCurSim < BX_NUM_SIMULATORS; iCurSim++)
        {
          bx_dbg_examine_command(yyvsp[-2].sval, NULL,0, yyvsp[-1].uval,1, iCurSim);
        }
        free(yyvsp[-2].sval);
        ;
    break;}
case 122:
#line 625 "parser.y"
{
        //FIXME HanishKVC This method of hunting thro all the 
        //      simulators may be better than using 2 calls if 
        //      BX_NUM_SIMULATORS greater than or equal to 2 as 
        //      done for other cases of BX_TOKEN_EXAMINE
        int iCurSim; 
        for(iCurSim = 0; iCurSim < BX_NUM_SIMULATORS; iCurSim++)
        {
          bx_dbg_examine_command(yyvsp[-1].sval, NULL,0, 0,0, iCurSim);
        }
        free(yyvsp[-1].sval);
        ;
    break;}
case 123:
#line 641 "parser.y"
{
        bx_dbg_setpmem_command(yyvsp[-3].uval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        ;
    break;}
case 124:
#line 649 "parser.y"
{
        bx_dbg_query_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 125:
#line 657 "parser.y"
{
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 126:
#line 662 "parser.y"
{
        bx_dbg_take_command(yyvsp[-2].sval, yyvsp[-1].uval);
        free(yyvsp[-3].sval); free(yyvsp[-2].sval);
        ;
    break;}
case 127:
#line 667 "parser.y"
{
        bx_dbg_take_command(yyvsp[-1].sval, 1);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 128:
#line 675 "parser.y"
{
        bx_dbg_set_cpu_command();
        free(yyvsp[-1].sval);
        ;
    break;}
case 129:
#line 683 "parser.y"
{
        bx_dbg_disassemble_command(yyvsp[-1].uval_range);
        free(yyvsp[-2].sval);
        ;
    break;}
case 130:
#line 691 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 131:
#line 696 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 132:
#line 701 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 133:
#line 706 "parser.y"
{
        bx_dbg_instrument_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 134:
#line 714 "parser.y"
{
        bx_dbg_loader_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 135:
#line 722 "parser.y"
{
        bx_dbg_doit_command(yyvsp[-1].uval);
        free(yyvsp[-2].sval);
        ;
    break;}
case 136:
#line 730 "parser.y"
{
        bx_dbg_crc_command(yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-3].sval);
        ;
    break;}
case 137:
#line 738 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        ;
    break;}
case 138:
#line 743 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        ;
    break;}
case 139:
#line 748 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        ;
    break;}
case 140:
#line 753 "parser.y"
{
        bx_dbg_maths_command(yyvsp[-3].sval, yyvsp[-2].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval); free(yyvsp[-3].sval);
        ;
    break;}
case 141:
#line 758 "parser.y"
{
        bx_dbg_maths_expression_command(yyvsp[-1].sval);
        free(yyvsp[-2].sval); free(yyvsp[-1].sval);
        ;
    break;}
case 142:
#line 765 "parser.y"
{
        bx_dbg_v2l_command(yyvsp[-3].uval, yyvsp[-1].uval);
        free(yyvsp[-4].sval);
        ;
    break;}
case 143:
#line 773 "parser.y"
{
	bx_dbg_trace_reg_on_command();
	free(yyvsp[-1].sval);
	;
    break;}
case 144:
#line 781 "parser.y"
{
	bx_dbg_trace_reg_off_command();
	free(yyvsp[-1].sval);
	;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 787 "parser.y"

