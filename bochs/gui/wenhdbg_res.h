// The menuIDs should not have big "gaps" -- so the switch works well.
// A few small gaps is OK.
#define CMD_CONT    101
#define CMD_STEP1   102
#define CMD_STEPN   103

#define CMD_BREAK   105
#define CMD_BRKALL  106
#define CMD_BRKPT   107
#define CMD_WPTWR   108
#define CMD_WPTRD   109
#define CMD_FIND    110
#define CMD_RFRSH   111
#define CMD_PHYDMP  113
#define CMD_LINDMP  114
#define CMD_STACK   115
#define CMD_GDTV    116
#define CMD_IDTV    117
#define CMD_PAGEV   118
#define CMD_CMEM    119
#define CMD_PTREE   120
#define CMD_DISASM  121
// It is important that all the menuIDs on the "Options" menu be higher than on any other menu
// It is important that CMD_MODEB be the lowest menuID on the option window
#define CMD_MODEB   123
#define CMD_DADEF   124
#define CMD_ATTI    125
#define CMD_FONT    126
#define CMD_IOWIN   127
#define CMD_SBTN    128
#define CMD_UCASE   129
#define CMD_MHEX    130
#define CMD_MASCII  131
#define CMD_LEND    132
// The CMD_WS menuID's must be strictly "contiguous" and in-order
#define CMD_WS_1    133
#define CMD_WS_2    134
#define CMD_WS_4    135
#define CMD_WS_8    136
#define CMD_WS16    137
#define CMD_IGNSA   138
#define CMD_IGNNT   139
#define CMD_RCLR    140
// The next 8 menuID's must be strictly "contiguous" and in-order
#define CMD_EREG    141
#define CMD_SREG    142
#define CMD_SYSR    143
#define CMD_CREG    144
#define CMD_FPUR    145
#define CMD_XMMR    146
#define CMD_DREG    147
#define CMD_TREG    148

#define CMD_ABOUT   149

