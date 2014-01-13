/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  BOCHS ENHANCED DEBUGGER Ver 1.2
//  (C) Chourdakis Michael, 2008
//  http://www.turboirc.com
//
//  Modified by Bruce Ewing
//
//  Copyright (C) 2008-2014  The Bochs Project

#ifndef BX_ENH_DBG_RES_H
#define BX_ENH_DBG_RES_H

// The menuIDs should not have big "gaps" -- so the switch works well.
// A few small gaps is OK.
#define CMD_CONT    101
#define CMD_STEP1   102
#define CMD_STEPN   103
#define CMD_BREAK   104

#define CMD_BRKPT   107
#define CMD_WPTWR   108
#define CMD_WPTRD   109
#define CMD_FIND    110
#define CMD_RFRSH   111
// View Menu:
#define CMD_PHYDMP  113
#define CMD_LINDMP  114
#define CMD_STACK   115
#define CMD_GDTV    116
#define CMD_IDTV    117
#define CMD_PAGEV   118
#define CMD_VBRK    120
#define CMD_CMEM    121
#define CMD_PTREE   122
#define CMD_DISASM  123
// Options Menu:
#define CMD_MODEB   125
#define CMD_XCEPT   126
#define CMD_ONECPU  127
#define CMD_DADEF   128
#define CMD_ATTI    129
#define CMD_FONT    130
#define CMD_UCASE   131
#define CMD_IOWIN   132
#define CMD_SBTN    133
#define CMD_MHEX    134
#define CMD_MASCII  135
#define CMD_LEND    136

#define CMD_IGNSA   139
#define CMD_IGNNT   140
#define CMD_RCLR    141
// The next 8 menuID's must be strictly "contiguous" and in-order
#define CMD_EREG    142
#define CMD_SREG    143
#define CMD_SYSR    144
#define CMD_CREG    145
#define CMD_FPUR    146
#define CMD_XMMR    147
#define CMD_DREG    148
#define CMD_TREG    149

#define CMD_LOGVIEW 150

#define CMD_ABOUT   151

// The CMD_WS popup menuID's must be strictly "contiguous" and in-order
#define CMD_WS_1    160
#define CMD_WS_2    161
#define CMD_WS_4    162
#define CMD_WS_8    163
#define CMD_WS16    164

#define MI_FIRST_VIEWITEM   CMD_PHYDMP
#define MI_FIRST_OPTITEM    CMD_MODEB
#define CMD_IDX_HI          CMD_WS16
#define CMD_IDX_LO          CMD_CONT

#endif
