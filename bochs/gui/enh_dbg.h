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
//  Copyright (C) 2008-2026  The Bochs Project


#ifndef BX_ENH_DBG_DEF_H
#define BX_ENH_DBG_DEF_H

#if BX_DEBUGGER && BX_DEBUGGER_GUI

#ifndef IS_WIN98
// workaround broken code if IS_WIN98 not defined
#define IS_WIN98
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

void MoveLists();
void SetStatusText(int column, const char *buf); // should it be here ?
void MakeListsGray();
void DispMessage(const char *msg, const char *title);
void InsertListRow(char *ColumnText[], int ColumnCount, int listnum, int LineCount, int grouping);
void StartListUpdate(int listnum);
void EndListUpdate(int listnum);
void RedrawColumns(int listnum);
void Invalidate(int i);
void TakeInputFocus();
bool ShowAskDialog();
bool NewFont();
void GrayMenuItem(int flag, int CmdIndex);
void ShowMemData(bool initting);
void SetMenuCheckmark (int flag, int CmdIndex);
void ClearInputWindow();
void VSizeChange();
void ToggleWSchecks(int newWS, int oldWS);
void SetOutWinTxt();
void ShowFW();
void GetInputEntry(char *buf);
void SelectHistory(int UpDown);
void DelWatchpoint(bx_watchpoint *wp_array, unsigned *TotEntries, int i);
void SetWatchpoint(unsigned *num_watchpoints, bx_watchpoint *watchpoint);

void HideTree();
void FillPTree();

int GetASMTopIdx();
void ScrollASM(int pixels);

void GetLIText(int listnum, int itemnum, int column, char *buf);
void SetLIState(int listnum, int itemnum, bool Select);
int GetNextSelectedLI(int listnum, int StartPt);

bool OSInit();
void SpecialInit();
void CloseDialog();
bool ParseOSSettings(const char *param, const char *value);
void WriteOSSettings(FILE *fd);

void HitBreak();
void SetCPUButton(unsigned oldCPU, unsigned newCPU);
void SelectCPU(unsigned newCPU);
void ParseIDText(const char *x);

bool DetachDebugger();

extern bool vgaw_refresh;

#ifdef WIN32

#define CHK_CMD_MODEB   CMD_MODEB
#define CHK_CMD_ONECPU  CMD_ONECPU
#define CHK_CMD_UCASE   CMD_UCASE
#define CHK_CMD_IOWIN   CMD_IOWIN
#define CHK_CMD_SBTN    CMD_SBTN
#define CHK_CMD_MHEX    CMD_MHEX
#define CHK_CMD_MASCII  CMD_MASCII
#define CHK_CMD_LEND    CMD_LEND
#define CHK_CMD_IGNSA   CMD_IGNSA
#define CHK_CMD_IGNNT   CMD_IGNNT
#define CHK_CMD_RCLR    CMD_RCLR
#define CHK_CMD_EREG    CMD_EREG
#define CHK_CMD_LOGVIEW CMD_LOGVIEW

#else // GTK+

// checkmark indices
#define MODE_BRK    0
#define ONE_CPU     1
#define U_CASE      2
#define IO_WIN      3
#define SHOW_BTN    4
#define MD_HEX      5
#define MD_ASC      6
#define L_END       7
#define IGN_SA      8
#define IGN_NT      9
#define R_CLR       10
#define E_REG       11
#define S_REG       12
#define SYS_R       13
#define C_REG       14
#define FPU_R       15
#define XMM_R       16
#define D_REG       17
//#define T_REG     18
#define LOG_VIEW    19

#define CHK_CMD_MODEB   MODE_BRK
#define CHK_CMD_ONECPU  ONE_CPU
#define CHK_CMD_UCASE   U_CASE
#define CHK_CMD_IOWIN   IO_WIN
#define CHK_CMD_SBTN    SHOW_BTN
#define CHK_CMD_MHEX    MD_HEX
#define CHK_CMD_MASCII  MD_ASC
#define CHK_CMD_LEND    L_END
#define CHK_CMD_IGNSA   IGN_SA
#define CHK_CMD_IGNNT   IGN_NT
#define CHK_CMD_RCLR    R_CLR
#define CHK_CMD_EREG    E_REG
#define CHK_CMD_LOGVIEW LOG_VIEW

#endif

#ifndef WIN32

// Need to define my version of the win32 "virtual keys"

#define VK_PRIOR    65365
#define VK_NEXT     65366
#define VK_F2       65471
#define VK_F3       65472
#define VK_F4       65473
#define VK_F5       65474
#define VK_F6       65475
#define VK_F7       65476
#define VK_F8       65477
#define VK_F9       65478
#define VK_F11      65480
#define VK_UP       65362
#define VK_DOWN     65364
#define VK_RETURN   65293
#define VK_LEFT     65361
#define VK_RIGHT    65363
#define VK_END      65367
#define VK_HOME     65360
#define VK_DELETE   65535
#define VK_ESCAPE   65307

#endif

// User Customizable initial settings:

extern bool SeeReg[8];

extern bool SingleCPU;       // Display all SMP CPUs
extern bool ShowIOWindows;   // Display the Input and Output Internal Debugger windows
extern bool ShowButtons;     // Display the top-row Step/Continue pushbuttons
extern bool SeeRegColors;    // Display registers with background color "groups"
extern bool ignoreNxtT;      // Do not show "Next at t=" output lines
extern bool ignSSDisasm;     // Do not show extra disassembly line at each break
extern bool UprCase;         // 1 = convert all Asm, Register names, Register values to uppercase
extern int DumpInAsciiMode;  // bit 1 = show ASCII in dumps, bit 2 = show hex, value=0 is illegal
extern int DumpWSIndex;      // word size index for memory dump
extern bool LogView;         // Send log to output window

extern bool isLittleEndian;

// How to use DockOrder: the Register list is window 1, ASM is window 2, MemDump is window 3
// Create the hex value with the digits in the order you want the windows to be.
// 0x312 would have MemDump on the left, Register in the middle, ASM on the right
extern short DockOrder;        // set the default List "docking" (Reg, ASM, Dump)

#define STACK_ENTRIES       50 // # of words/dwords/qwords to display in stack window
// Note: STACK_ENTRIES must be <= 512

// END of User Customizable settings

#if !defined(_MSC_VER)
#define FMT_LLCAPX  FMT_LL"X"
#else
#define FMT_LLCAPX  "%016I64X"
#endif

#define MAX_ASM         2048    // max entries in ASM window

extern bool SA_valid;
extern Bit64u SelectedDataAddress;
extern Bit64u CurrentAsmLA;    // = EIP/RIP -- for highlighting in ASM window
extern Bit64u BottomAsmLA;     // beginning and end addrs on ASM window
extern Bit64u TopAsmLA;

extern Bit64u PrevPtime;       // any number other than 0
extern Bit64u NewPtime;        // used to test whether sim has "updated"
extern unsigned TotCPUs;       // # of CPUs in a multi-CPU simulation
extern unsigned CpuSupportSSE; // cpu supports SSE
extern unsigned CurrentCPU;    // cpu that is being displayed

// AskTest stuff -- for the generic dialog box
#define ASKTEXT_MAX 100    // max size of the reply text in the "ask" dialog, including the terminating zero

struct ASKTEXT {
    const char* title;
    const char* prompt;
    char* reply;
};

extern struct ASKTEXT ask_str;

// window resizing/docking stuff
extern int OneCharWide;    // average width of a char in current font (pixels)
extern int Sizing;         // current "resizing/docking mode"
extern int Resize_HiX;     // horizontal limits of the current resize operation (pixels)
extern int Resize_LoX;
extern unsigned ListWidthPix[3]; // set initial proportions of Reg, Asm, Dump windows
extern int CurCenterList;
extern bool DumpHasFocus;

// BarClix holds the x-axis position (in pixels or logical units) of the two resizing bars,
// in parent coordinates (ie. any window that contains the lists)
extern unsigned short BarClix[2];

extern bool AtBreak;        // Status indicators
extern bool StatusChange;

extern bool doOneTimeInit;  // Internal flag #1
extern bool doSimuInit;     // Internal flag #2
extern bool ResizeColmns;   // address/value column autosize flag
extern bool FWflag;         // friendly warning has been shown to user once already

extern Bit64u PStackLA;        // to calculate alignment between prev and current stack
extern bool StackEntChg[STACK_ENTRIES];     // flag for "change detected" on each stack line

// List Window indexes = DockOrder window numbers -1
#define REG_WND     0
#define ASM_WND     1
#define DUMP_WND    2

// DViewModes
#define VIEW_MEMDUMP    0
#define VIEW_GDT        1
#define VIEW_IDT        2
#define VIEW_PAGING     3
#define VIEW_STACK      4
#define VIEW_BREAK      5
#define VIEW_PTREE      6

extern bool DumpInitted;       // has the MemDump window ever been filled with data?
extern int DumpAlign;
extern int PrevDAD;            // saves "previous DumpAlign value" (forces column autosize)
extern Bit64u DumpStart;       // current emulated address (lin or phys) of DataDump
extern bool doDumpRefresh;
extern int DViewMode;
extern bool LinearDump;        // FALSE = memdump uses physical addressing

extern char *tmpcb;            // 512b is allocated in bigbuf
extern char AsciiHex[512];     // Unsigned char to printable hex xlat table

#define outbufSIZE      40960
extern char bigbuf[outbufSIZE];    // 40K preallocated storage for all char buffers (see DoAllInit)
extern char *OutWindow;            // buffer for the Output window
#define OutWinSIZE  10240
extern int PO_Tdelay;              // delay before displaying partial output lines

extern int AsmPgSize;
extern int ListLineRatio;          // number of vertical pixels in a ListView Item
extern int ListVerticalPix;        // number of vertical pixels in each List
extern int AsmLineCount;           // # of disassembled asm lines loaded
extern Bit64u AsmLA[MAX_ASM];      // linear address of each disassembled ASM line

// Command stuff
extern int CommandHistoryIdx;
#define CmdHistorySize 64
extern char *CmdHistory[CmdHistorySize];   // 64 command History storage (fixed 80b each)
extern int CmdHInsert;                     // index of next history entry to store

// register storage -- names and values
enum {
  RAX_Rnum,
  RBX_Rnum,
  RCX_Rnum,
  RDX_Rnum,
  RSI_Rnum,
  RDI_Rnum,
  RBP_Rnum,
  RSP_Rnum,
  RIP_Rnum,
  R8_Rnum,
  R9_Rnum,
  R10_Rnum,
  R11_Rnum,
  R12_Rnum,
  R13_Rnum,
  R14_Rnum,
  R15_Rnum,
  R16_Rnum,                 // APX extended GPRs
  R17_Rnum,
  R18_Rnum,
  R19_Rnum,
  R20_Rnum,
  R21_Rnum,
  R22_Rnum,
  R23_Rnum,
  R24_Rnum,
  R25_Rnum,
  R26_Rnum,
  R27_Rnum,
  R28_Rnum,
  R29_Rnum,
  R30_Rnum,
  R31_Rnum,
  EFL_Rnum,                 // it makes the most sense if EFLAGS is between RXX and EXX
  EAX_Rnum,                 // because it "belongs with" both sets of GP registers
  EBX_Rnum,
  ECX_Rnum,
  EDX_Rnum,
  ESI_Rnum,
  EDI_Rnum,
  EBP_Rnum,
  ESP_Rnum,
  EIP_Rnum,
  CS_Rnum,
  DS_Rnum,
  ES_Rnum,
  SS_Rnum,
  FS_Rnum,
  GS_Rnum,
  GDTRnum,
  IDTRnum,
  LDTRnum,
  TRRnum,
  CR0_Rnum,
  CR2_Rnum,
  CR3_Rnum,
  CR4_Rnum,
  EFER_Rnum,                // currently used to mark the last entry in rV[]
  ST0_Rnum,
  ST1_Rnum,
  ST2_Rnum,
  ST3_Rnum,
  ST4_Rnum,
  ST5_Rnum,
  ST6_Rnum,
  ST7_Rnum,
  XMM0_Rnum,
  XMM1_Rnum,
  XMM2_Rnum,
  XMM3_Rnum,
  XMM4_Rnum,
  XMM5_Rnum,
  XMM6_Rnum,
  XMM7_Rnum,
  XMM8_Rnum,
  XMM9_Rnum,
  XMMA_Rnum,
  XMMB_Rnum,
  XMMC_Rnum,
  XMMD_Rnum,
  XMME_Rnum,
  XMMF_Rnum,
  DR0_Rnum,
  DR1_Rnum,
  DR2_Rnum,
  DR3_Rnum,
  DR6_Rnum,
  DR7_Rnum,
  // TR0_Rnum -- put Test Registers in here when they are supported
  STD_REG_NUM,
  // extra "register numbers" for registers that have more than one part
  GDTR_Lim = STD_REG_NUM,
  IDTR_Lim,
  ST0_exp,
  ST1_exp,
  ST2_exp,
  ST3_exp,
  ST4_exp,
  ST5_exp,
  ST6_exp,
  ST7_exp,
  XMM0_hi,
  XMM1_hi,
  XMM2_hi,
  XMM3_hi,
  XMM4_hi,
  XMM5_hi,
  XMM6_hi,
  XMM7_hi,
  XMM8_hi,
  XMM9_hi,
  XMMA_hi,
  XMMB_hi,
  XMMC_hi,
  XMMD_hi,
  XMME_hi,
  XMMF_hi
};

#define EXTRA_REGS  28

extern Bit8u RegColor[STD_REG_NUM+EXTRA_REGS];   // specifies foreground and background color of registers
// Text color is red if the upper bit is set. Background is set according to ColorList.
extern int RitemToRnum[STD_REG_NUM+EXTRA_REGS];   // mapping from Reg List Item# to register number

// do the linear breakpoint list as 2 arrays, rather than a structure -- much easier to search!
extern bx_address BrkLAddr[BX_DBG_MAX_LIN_BPOINTS+1];
extern int BreakCount;

// Breakpoint Dump Window stuff
extern unsigned short BrkpIDMap[256];
extern unsigned short WWP_BaseEntry;
extern unsigned short RWP_BaseEntry;
extern unsigned short EndLinEntry;
extern unsigned short EndPhyEntry;
extern unsigned short WWPSnapCount;
extern unsigned short RWPSnapCount;
extern bx_phy_address WWP_Snapshot[16];
extern bx_phy_address RWP_Snapshot[16];

extern int SizeList;
extern Bit32s xClick;          // halfway through a mouseclick flag + location
extern Bit32s yClick;          // values are in Listview coordinates

#include "wenhdbg_res.h"    // MenuIDs

extern const char* DC0txt[2];    // DumpMode definitions in text

#define NBUTTONS 6
extern const char* BTxt[NBUTTONS];
extern int BtnLkup[NBUTTONS];

#endif

#endif
