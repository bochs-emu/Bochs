// include file for the Win32 Enhanced GUI debugger frontend
// User Customizable initial settings:

bx_bool SeeReg[8] = {
    TRUE,   // in 64bit mode, show 32bit versions of registers also (EAX, ...)
    FALSE,  // show segment registers (CS, ...)
    FALSE,  // show GDTR, IDTR, LDTR, Task Reg
    FALSE,  // show control register (CR0, ...)
    FALSE,  // show FPU (STi) / MMX registers
    FALSE,  // show XMM registers
    FALSE,  // show the Debug Registers (DR0, ...)
    FALSE       // Test Registers not yet supported in bochs (2.3.7)
};

bx_bool ShowIOWindows = TRUE;   // Display the top-row Step/Continue pushbuttons
bx_bool ShowButtons = TRUE;     // Display the top-row Step/Continue pushbuttons
bx_bool SeeRegColors = TRUE;    // Display registers with background color "groups"
bx_bool ignoreNxtT = TRUE;      // Do not show "Next at t=" output lines
bx_bool ignSSDisasm = TRUE;     // Do not show extra disassembly line at each break
int UprCase = 1;            // 1 = convert all Asm, Register names, Register values to uppercase
int DumpInAsciiMode = 3;    // bit 1 = show ASCII in dumps, bit 2 = show hex, value=0 is illegal

bx_bool isLittleEndian = TRUE;
int DefaultAsmLines = 250;      // default # of asm lines disassembled and "cached"
int bottommargin = 6;           // ASM autoscroller tries to leave this many lines below
int topmargin = 3;              // autoscroller tries to leave this many lines above
// Note: topmargin must be less than bottommargin

// How to use DockOrder: the Register list is window 1, ASM is window 2, MemDump is window 3
// Create the hex value with the digits in the order you want the windows to be.
// 0x312 would have MemDump on the left, Register in the middle, ASM on the right
short DockOrder = 0x123;        // set the default List "docking" (Reg, ASM, Dump)

COLORREF ColorList[16] = {  // background "register type" colors indexed by RegColor value
    RGB(255,255,255),   // white
    RGB(200,255,255),   // blue (aqua)
    RGB(230,230,230),   // gray
    RGB(248,255,200),   // yellow
    RGB(216,216,255),   // purple
    RGB(200,255,200),   // green
    RGB(255,230,200),   // orange
    RGB(255,255,255),   // user definable
    RGB(255,255,255),
    RGB(255,255,255),
    RGB(255,255,255),
    RGB(255,255,255),
    RGB(255,255,255),
    RGB(255,255,255),
    RGB(255,255,255),
    RGB(255,255,255)
};

#define STATUS_WIN_OVERLAP  4       // # of "extra" pixels at the top of a Status window
#define STACK_ENTRIES       50      // # of words/dwords/qwords to display in stack window
// Note: STACK_ENTRIES must be <= 512

// END of User Customizable settings

#include <math.h>
#include "iodev/iodev.h"
#include "wenhdbg_res.h"    // MenuIDs


#ifndef LVS_EX_DOUBLEBUFFER
#define IS_WIN98
// these two defines may not exist in some compilers
#define LVS_EX_DOUBLEBUFFER 0
#define LVIF_GROUPID    0
#endif

#define MAX_ASM         2048    // max entries in ASM window

// The wordsize popup is the 13th entry in the Opt menu -- COUNTING SEPARATORS
// Index = (entry number - 1) -- if the Opt menu is modified, then update this value
// -- or else the checkmarks won't work
#define WS_POPUP_IDX    12


// Handles to Windows and global stuff
HWND hY;            // complete parent window
HWND hL[3];         // 0=registers, 1=Asm, 2=MemDump
HWND hE_I;          // command input window
HWND hS_S;          // "status" window at bottom
HWND hE_O;          // debugger text output
HWND hT;            // param_tree window
HWND hBTN[5];       // button row
HWND hCPUt[BX_MAX_SMP_THREADS_SUPPORTED];   // "tabs" for the individual CPUs
HFONT CustomFont = NULL;
HMENU hOptMenu;     // "Options" popup menu (needed to set check marks)
HMENU hViewMenu;    // "View" popup menu (needed to gray entries)
HMENU hCmdMenu;     // "Command" popup menu (needed to gray entries)
// one "defualtProc" for each edit window (Input and Output)
WNDPROC wEdit[2];
WNDPROC wBtn;       // all the buttons have the same Proc
WNDPROC wTreeView;
WNDPROC wListView;  // all the lists use the same Proc

//HANDLE hTCevent[BX_MAX_SMP_THREADS_SUPPORTED];    // Start/Sleep Control for cpu_loop threads

// get a "class" to access the disassebler
// Note; any instance has access to all the member functions -- that is enough!
// -- i.e. No further initialization necessary.
static disassembler bx_disassemble;
// for refreshing the bochs VGA window:
BOCHSAPI extern bx_devices_c bx_devices;

// Note: all these BSS global variables are initialized to 0 automatically
Bit64u SelectedDataAddress = 0;
Bit64u CurrentAsmLA = 0;    // = EIP/RIP -- for highlighting in ASM window
Bit64u BottomAsmLA;     // beginning and end addrs on ASM window
Bit64u TopAsmLA;

Bit32u PrevStepNSize = 50;  // cpu_loop control variables
//Bit32u RequestedLoops;        // number of cpu_loops the user is asking for
//bx_bool DoBreak;          // stop a local subthreaded cpu_loop
Bit64u PrevPtime = 9;       // any number other than 0
Bit64u NewPtime;            // used to test whether sim has "updated"
unsigned int TotCPUs;       // # of CPUs in a multi-CPU simulation
unsigned int CurrentCPU;    // cpu that is being displayed


// window resizing/docking stuff
HCURSOR hCursResize;
HCURSOR hCursDock;
HCURSOR hCursArrow;
LVHITTESTINFO lvht;     // calculate and remember which list item was clicked
int OneCharWide;        // average width of a char in current font
int Sizing = 0;     // current "resizing/docking mode"
int Resize_HiX;     // horizontal limits of the current resize operation
int Resize_LoX;
unsigned short BarScrx[2];      // position of central listview's left and right borders in SCREEN coordinates
unsigned short BarClix[2];      // in client coordinates
unsigned int ListWidthPix[3] = {5,7,8}; // set initial proportions of Reg, Asm, Dump windows
unsigned int LstTop = 0;
int CurCenterList;
int SizeList;               // pre-docking list #
bx_bool DumpHasFocus = FALSE;
Bit32s xClick = -1;         // halfway through a mouseclick flag + location
Bit32s yClick;              // values are in Listview coordinates
Bit32u CurTimeStamp;        // last mousedown time
short nDock[36] = {     // lookup table for alternate DockOrders
    0x231, 0x312, 0x231, 0x213, 0x132, 0x132,
    0x213, 0x321, 0x123, 0x123, 0x321, 0x312,
    0x213, 0x213, 0x123, 0x312, 0x321, 0x312,
    0x132, 0x123, 0x132, 0x321, 0x231, 0x231,
    0x312, 0x312, 0x231, 0x213, 0x132, 0x213,
    0x132, 0x123, 0x321, 0x321, 0x123, 0x231
};

// base window "styles" for the 3 listviews.
long LVStyle[3] = {
    LVS_REPORT | WS_CHILD,
    LVS_SHOWSELALWAYS | LVS_REPORT | WS_CHILD,
    LVS_SHOWSELALWAYS | LVS_REPORT | WS_CHILD | WS_VISIBLE
};

bx_bool AtBreak = FALSE;        // Status indicators
bx_bool PrevAtBreak = FALSE;
bx_bool CpuModeChange = TRUE;
bx_bool StatusChange = TRUE;

bx_bool In64Mode = FALSE;       // CPU modes
bx_bool In32Mode = FALSE;
unsigned CpuMode = 0;
Bit32u InPaging = 0;            // Storage for the top bit of CR0, unmodified

bx_bool doOneTimeInit = TRUE;   // Internal flags
bx_bool ResizeColmns;           // address/value column autosize flag
bx_bool dbgOn = FALSE;          // the bochs internal debugger needs some time to get started
bx_bool FWflag = FALSE;         // friendly warning has been shown to user once already
int StackSized;                 // autosize flag for stack window

char *PrevStack;                // buffer for testing changes in stack values
Bit64u PStackLA = 0;                // to calculate alignment between prev and current stack
bx_bool StackEntChg[STACK_ENTRIES];     // flag for "change detected" on each stack line
bx_bool StkInvOnce = FALSE;         // sometimes need to specially invalidate the stack window

// DViewModes
#define VIEW_MEMDUMP    0
#define VIEW_GDT        1
#define VIEW_IDT        2
#define VIEW_PAGING     3
#define VIEW_STACK      4
#define VIEW_PTREE      5

bx_bool DumpInitted = FALSE;    // has the MemDump window ever been filled with data?
int DumpAlign = 1;
int PrevDAD;                // saves "previous DumpAlign value" (forces column autosize)
char *DataDump;
Bit64u DumpStart = 0;       // current emulated address (lin or phys) of DataDump
bx_bool doDumpRefresh;
int DViewMode = VIEW_MEMDUMP;
bx_bool DumpMode = 1; // 0 physical, 1 linear
char* DC0txt[2] = {"P.Address","L.Address"};    // DumpMode definitions in text

// make some global 512b temp char buffers -- they are needed to test against overflows anyway
char *tmpcb, *tmpcd;        // space is allocated in bigbuf
char *tmpce, *tmpcf;        // exclusively for use in subthreads
char *AsciiHex;             // Unsigned char to printable hex xlat table

#define outbufSIZE      30720
char bigbuf[outbufSIZE];        // 30K preallocated storage for all char buffers (see DoAllInit)
char *DbgAppendPtr = bigbuf;
char *OutWindow;                // buffer for the Output window
#define OutWinSIZE  10240
int OutWinCnt = OutWinSIZE;     // available size of OutWindow buffer
int PO_Tdelay = 0;              // delay before displaying partial output lines

int AsmLineCount = 1;           // # of disassembled asm lines loaded
int AsmPgSize = 0;
int ListLineRatio;              // number of vertical pixels in a ListView Item
int ListVerticalPix;            // number of vertical pixels in each List
Bit64u AsmLA[MAX_ASM];  // linear address of each disassembled ASM line

// Command stuff
int CommandHistoryIdx = 0;
char *CmdHistory[64];           // 64 command History storage (fixed 80b each)
int CmdHInsert = 0;             // index of next history entry to store

// register storage -- names and values
#define RAX_Rnum    0
#define RBX_Rnum    1
#define RCX_Rnum    2
#define RDX_Rnum    3
#define RSI_Rnum    4
#define RDI_Rnum    5
#define RBP_Rnum    6
#define RSP_Rnum    7
#define RIP_Rnum    8
#define R8_Rnum     9
#define R9_Rnum     10
#define R10_Rnum    11
#define R11_Rnum    12
#define R12_Rnum    13
#define R13_Rnum    14
#define R14_Rnum    15
#define R15_Rnum    16
#define EFL_Rnum    17      // it makes the most sense if EFLAGS is between RXX and EXX
#define EAX_Rnum    18      // because it "belongs with" both sets of GP registers
#define EBX_Rnum    19
#define ECX_Rnum    20
#define EDX_Rnum    21
#define ESI_Rnum    22
#define EDI_Rnum    23
#define EBP_Rnum    24
#define ESP_Rnum    25
#define EIP_Rnum    26
#define CS_Rnum     27
#define DS_Rnum     28
#define ES_Rnum     29
#define SS_Rnum     30
#define FS_Rnum     31
#define GS_Rnum     32
#define GDTRnum     33
#define IDTRnum     34
#define LDTRnum     35
#define TRRnum      36
#define CR0_Rnum    37
#define CR3_Rnum    38
#define CR4_Rnum    39
#define EFER_Rnum   40      // currently used to mark the last entry in rV[]
#define ST0_Rnum    41
#define ST1_Rnum    42
#define ST2_Rnum    43
#define ST3_Rnum    44
#define ST4_Rnum    45
#define ST5_Rnum    46
#define ST6_Rnum    47
#define ST7_Rnum    48
#define XMM0_Rnum   49
#define XMM1_Rnum   50
#define XMM2_Rnum   51
#define XMM3_Rnum   52
#define XMM4_Rnum   53
#define XMM5_Rnum   54
#define XMM6_Rnum   55
#define XMM7_Rnum   56
#define XMM8_Rnum   57
#define XMM9_Rnum   58
#define XMMA_Rnum   59
#define XMMB_Rnum   60
#define XMMC_Rnum   61
#define XMMD_Rnum   62
#define XMME_Rnum   63
#define XMMF_Rnum   64
#define DR0_Rnum    65
#define DR1_Rnum    66
#define DR2_Rnum    67
#define DR3_Rnum    68
#define DR6_Rnum    69
#define DR7_Rnum    70
// #define TR0_Rnum 71  -- put Test Registers in here when they are supported
#define TOT_REG_NUM 72
// extra "register numbers" for registers that have more than one part
#define GDTR_Lim    72
#define IDTR_Lim    73
#define ST0_exp     74
#define ST1_exp     75
#define ST2_exp     76
#define ST3_exp     77
#define ST4_exp     78
#define ST5_exp     79
#define ST6_exp     80
#define ST7_exp     81
#define XMM0_hi     82
#define XMM1_hi     82
#define XMM2_hi     83
#define XMM3_hi     84
#define XMM4_hi     85
#define XMM5_hi     86
#define XMM6_hi     87
#define XMM7_hi     88
#define XMM8_hi     89
#define XMM9_hi     90
#define XMMA_hi     91
#define XMMB_hi     92
#define XMMC_hi     93
#define XMMD_hi     94
#define XMME_hi     95
#define XMMF_hi     96
#define EXTRA_REGS  28

char * EflBName[16] = {
    "cf", "pf", "af", "zf", "sf", "tf", "if", "df", "of", "nt", "rf", "vm", "ac", "vif", "vip", "id"
};
int EflBNameLen[16] = {
    2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,2
};
int EflBitVal[16] = {
    1, 4, 0x10, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x4000, 0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000
};

// only pay special attention to registers up to EFER
char* RegLCName[EFER_Rnum + 1] = {
    "rax","rbx","rcx","rdx","rsi","rdi","rbp","rsp","rip",
    "r8","r9","r10","r11","r12","r13","r14","r15",
    "eflags","eax","ebx","ecx","edx","esi","edi","ebp","esp","eip",
    "cs","ds","es","ss","fs","gs",
    "gdtr","idtr","ldtr","tr","cr0","cr3","cr4","efer"
};
char* RDispName[EFER_Rnum + 1];
bx_param_num_c *RegObject[BX_MAX_SMP_THREADS_SUPPORTED][TOT_REG_NUM + EXTRA_REGS];
Bit64u rV[EFER_Rnum + 1];   // current values of registers
Bit64u PV[EFER_Rnum + 1];   // previous values of registers
Bit32s GDT_Len;         // "limits" (= bytesize-1) for GDT and IDT
Bit32s IDT_Len;
Bit8u RegColor[TOT_REG_NUM];    // specifies foreground and background color of registers
// Text color is red if the upper bit is set. Background is set according to ColorList.
int RitemToRnum[TOT_REG_NUM];   // mapping from Reg List Item# to register number

// lviG is a generic ListViewItem with GroupID -- reused many times
LV_ITEM lviG = {LVIF_TEXT | LVIF_GROUPID,0,0,0,0,(LPSTR)0,0,0,0};

TVINSERTSTRUCT tvis;    // tree-view generic item

Bit64u ladrmin = 0; // bochs linear addressing access variables
Bit64u ladrmax = 0;
Bit64u l_p_offset;

#define BTN_BASE            1024
#define MULTICPU_BTN_BASE   1030
int BtnLkup[6] = {
    CMD_CONT, CMD_STEP1, CMD_STEPN, CMD_BREAK, CMD_RFRSH, CMD_BRKALL
};
char* BTxt[6] = {"Continue [c]","Step [s]","Step N [s ###]","Break [^C]","Refresh","Break All"};
char* GDTt2[8] = {"16-bit code","64-bit code","32-bit code","16-bit data","64-bit data","32-bit data","Illegal","Unused"};
char* GDTsT[] = { "","Available 16bit TSS","LDT","Busy 16bit TSS","16bit Call Gate",
                "Task Gate","16bit Interrupt Gate","16bit Trap Gate","Reserved",
                "Available 32bit TSS","Reserved","Busy 32bit TSS","32bit Call Gate",
                "Reserved","32bit Interrupt Gate","32bit Trap Gate"
};
// Register hex display formats -- index by UprCase
char* Fmt64b[2] = { "%016I64x", "%016I64X" };
char* Fmt32b[2] = { "%08x", "%08X" };
char* Fmt16b[2] = { "%04x", "%04X" };
char* xDT64Fmt[2] = { "%016I64x (%4x)", "%016I64X (%4X)" };
char* xDT32Fmt[2] = { "%08x (%4x)", "%08X (%4X)" };

// do the linear breakpoint list as 2 arrays, rather than a structure -- much easier to search!
bx_address BrkLAddr[BX_DBG_MAX_LIN_BPOINTS];
unsigned int BrkIdx[BX_DBG_MAX_LIN_BPOINTS];
int BreakCount = 0;

// AskTest stuff
struct ASKTEXT {
    const TCHAR* ti;
    const TCHAR* as;
    TCHAR* re;
    int DefT;
};
