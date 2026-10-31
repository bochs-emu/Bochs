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

#include "config.h"

#if BX_DEBUGGER && BX_DEBUGGER_GUI

#include <math.h>

#include "bochs.h"
#include "siminterface.h"
#include "bx_debug/debug.h"
#include "memory/memory-bochs.h"
#include "pc_system.h"
#include "cpu/cpu.h"

extern const char *stringify_EFLAGS(Bit32u eflags, char *s);

#include "enh_dbg.h"

#define LOG_THIS genlog->

// Match stuff
#define MATCH_TRUE 1
#define MATCH_FALSE 0
#define MATCH_ABORT -1

const char* DC0txt[2] = {"P.Address","L.Address"};    // DumpMode definitions in text

const char* BTxt[NBUTTONS] = {
  "Continue [c]",
  "Step [s]",
  "Step N [s ###]",
  "Step Over",
  "Refresh",
  "Break [^C]",
};

int BtnLkup[NBUTTONS] = {
    CMD_CONT, CMD_STEP1, CMD_STEPN, CMD_STEPOVER, CMD_RFRSH, CMD_BREAK
};

#ifdef WIN32
int useCR = 1;                    // Win32 needs CRLF pairs for an EOL
bool NeedSysRresize = TRUE;       // use Sys Reg to help autosize Reg "hex" column
#else
int useCR = 0;
bool NeedSysRresize = FALSE;      // use Sys Reg to help autosize Reg "hex" column
#endif

bool SeeReg[8] = {
    TRUE,   // in 64bit mode, show 32bit versions of registers also (EAX, ...)
    TRUE,  // show segment registers (CS, ...)
    TRUE,  // show GDTR, IDTR, LDTR, Task Reg
    TRUE,  // show control register (CR0, ...)
    FALSE,  // show FPU (STi) / MMX registers
    FALSE,  // show XMM registers
    FALSE,  // show the Debug Registers (DR0, ...)
    FALSE   // Test Registers not yet supported in bochs
};

bool SingleCPU = FALSE;      // Display all SMP CPUs
bool ShowIOWindows = TRUE;   // Display the Input and Output Internal Debugger windows
bool ShowButtons = TRUE;     // Display the top-row Step/Continue pushbuttons
bool SeeRegColors = TRUE;    // Display registers with background color "groups"
bool ignoreNxtT = TRUE;      // Do not show "Next at t=" output lines
bool ignSSDisasm = TRUE;     // Do not show extra disassembly line at each break
bool UprCase = 0;            // 1 = convert all Asm, Register names, Register values to uppercase
int DumpInAsciiMode = 3;     // bit 1 = show ASCII in dumps, bit 2 = show hex, value=0 is illegal
int DumpWSIndex = 0;         // word size index for memory dump
bool LogView = 0;            // Send log to output window

bool isLittleEndian = TRUE;
int DefaultAsmLines = 512;      // default # of asm lines disassembled and "cached"
int bottommargin = 6;           // ASM autoscroller tries to leave this many lines below
int topmargin = 3;              // autoscroller tries to leave this many lines above
// Note: topmargin must be less than bottommargin

// How to use DockOrder: the Register list is window 1, ASM is window 2, MemDump is window 3
// Create the hex value with the digits in the order you want the windows to be.
// 0x312 would have MemDump on the left, Register in the middle, ASM on the right
short DockOrder = 0x123;        // set the default List "docking" (Reg, ASM, Dump)

bool SA_valid = FALSE;
Bit64u SelectedDataAddress = 0;
Bit64u CurrentAsmLA = 0;    // = EIP/RIP -- for highlighting in ASM window
Bit64u BottomAsmLA;         // beginning and end addrs on ASM window
Bit64u TopAsmLA;

Bit32u PrevStepNSize = 50;  // cpu_loop control variables
unsigned TotCPUs;           // # of CPUs in a multi-CPU simulation
unsigned CpuSupportSSE = 0; // cpu supports SSE
unsigned CurrentCPU;        // cpu that is being displayed

struct ASKTEXT ask_str;

// window resizing/docking stuff
int OneCharWide;    // average width of a char in current font (pixels)
int Sizing = 0;     // current "resizing/docking mode"
int Resize_HiX;     // horizontal limits of the current resize operation (pixels)
int Resize_LoX;
unsigned ListWidthPix[3] = {5,7,8}; // set initial proportions of Reg, Asm, Dump windows
int CurCenterList = 0;
bool DumpHasFocus = FALSE;
// BarClix holds the x-axis position (in pixels or logical units) of the two resizing bars,
// in parent coordinates (ie. any window that contains the lists)
unsigned short BarClix[2];

bool AtBreak = FALSE;    // Status indicators
bool CpuModeChange;
bool StatusChange = TRUE;

bool In64Mode = FALSE;       // CPU modes
bool In32Mode = FALSE;
unsigned CpuMode = 0;
Bit32u InPaging = 0;            // Storage for the top bit of CR0, unmodified

bool doOneTimeInit = TRUE;   // Internal flag #1
bool doSimuInit;             // Internal flag #2
bool ResizeColmns;           // address/value column autosize flag
bool FWflag = FALSE;         // friendly warning has been shown to user once already

static char *PrevStack;             // buffer for testing changes in stack values
Bit64u PStackLA = 0;                // to calculate alignment between prev and current stack
bool StackEntChg[STACK_ENTRIES];     // flag for "change detected" on each stack line

// param tree names which exist only in some configurations (NULL = not bound)
#if BX_SUPPORT_X86_64
#define REG64_PARAM(name) name
#define REG32_PARAM(name) NULL      // EAX-EIP are copied from RAX-RIP in FillRegs
#else
#define REG64_PARAM(name) NULL
#define REG32_PARAM(name) name
#endif
#if BX_SUPPORT_X86_64 && BX_SUPPORT_APX
#define APX_PARAM(name) name
#else
#define APX_PARAM(name) NULL
#endif
#if BX_CPU_LEVEL >= 4
#define CR4_PARAM(name) name
#else
#define CR4_PARAM(name) NULL
#endif
#if BX_CPU_LEVEL >= 6
#define EFER_PARAM(name) name
#else
#define EFER_PARAM(name) NULL
#endif

// registers with a display name and a single param tree value
// (FPU and SSE registers have more than one part and are bound in InitRegObjects)
struct RegDesc {
    int rnum;               // register number (index into RegObject, rV, RDispName)
    const char *name;       // display name, in lower case
    const char *param;      // param tree name, relative to "bochs.cpuN"
};

static const RegDesc RegTable[] = {
    { RAX_Rnum,  "rax",    REG64_PARAM("RAX") },
    { RBX_Rnum,  "rbx",    REG64_PARAM("RBX") },
    { RCX_Rnum,  "rcx",    REG64_PARAM("RCX") },
    { RDX_Rnum,  "rdx",    REG64_PARAM("RDX") },
    { RSI_Rnum,  "rsi",    REG64_PARAM("RSI") },
    { RDI_Rnum,  "rdi",    REG64_PARAM("RDI") },
    { RBP_Rnum,  "rbp",    REG64_PARAM("RBP") },
    { RSP_Rnum,  "rsp",    REG64_PARAM("RSP") },
    { RIP_Rnum,  "rip",    REG64_PARAM("RIP") },
    { R8_Rnum,   "r8",     REG64_PARAM("R8")  },
    { R9_Rnum,   "r9",     REG64_PARAM("R9")  },
    { R10_Rnum,  "r10",    REG64_PARAM("R10") },
    { R11_Rnum,  "r11",    REG64_PARAM("R11") },
    { R12_Rnum,  "r12",    REG64_PARAM("R12") },
    { R13_Rnum,  "r13",    REG64_PARAM("R13") },
    { R14_Rnum,  "r14",    REG64_PARAM("R14") },
    { R15_Rnum,  "r15",    REG64_PARAM("R15") },
    { R16_Rnum,  "r16",    APX_PARAM("R16")   },
    { R17_Rnum,  "r17",    APX_PARAM("R17")   },
    { R18_Rnum,  "r18",    APX_PARAM("R18")   },
    { R19_Rnum,  "r19",    APX_PARAM("R19")   },
    { R20_Rnum,  "r20",    APX_PARAM("R20")   },
    { R21_Rnum,  "r21",    APX_PARAM("R21")   },
    { R22_Rnum,  "r22",    APX_PARAM("R22")   },
    { R23_Rnum,  "r23",    APX_PARAM("R23")   },
    { R24_Rnum,  "r24",    APX_PARAM("R24")   },
    { R25_Rnum,  "r25",    APX_PARAM("R25")   },
    { R26_Rnum,  "r26",    APX_PARAM("R26")   },
    { R27_Rnum,  "r27",    APX_PARAM("R27")   },
    { R28_Rnum,  "r28",    APX_PARAM("R28")   },
    { R29_Rnum,  "r29",    APX_PARAM("R29")   },
    { R30_Rnum,  "r30",    APX_PARAM("R30")   },
    { R31_Rnum,  "r31",    APX_PARAM("R31")   },
    { EFL_Rnum,  "eflags", "EFLAGS"           },
    { EAX_Rnum,  "eax",    REG32_PARAM("EAX") },
    { EBX_Rnum,  "ebx",    REG32_PARAM("EBX") },
    { ECX_Rnum,  "ecx",    REG32_PARAM("ECX") },
    { EDX_Rnum,  "edx",    REG32_PARAM("EDX") },
    { ESI_Rnum,  "esi",    REG32_PARAM("ESI") },
    { EDI_Rnum,  "edi",    REG32_PARAM("EDI") },
    { EBP_Rnum,  "ebp",    REG32_PARAM("EBP") },
    { ESP_Rnum,  "esp",    REG32_PARAM("ESP") },
    { EIP_Rnum,  "eip",    REG32_PARAM("EIP") },
    { CS_Rnum,   "cs",     "CS.selector"      },
    { DS_Rnum,   "ds",     "DS.selector"      },
    { ES_Rnum,   "es",     "ES.selector"      },
    { SS_Rnum,   "ss",     "SS.selector"      },
    { FS_Rnum,   "fs",     "FS.selector"      },
    { GS_Rnum,   "gs",     "GS.selector"      },
    { GDTRnum,   "gdtr",   "GDTR.base"        },
    { IDTRnum,   "idtr",   "IDTR.base"        },
    { LDTRnum,   "ldtr",   "LDTR.base"        },
    { TRRnum,    "tr",     "TR.base"          },
    { CR0_Rnum,  "cr0",    "CR0"              },
    { CR2_Rnum,  "cr2",    "CR2"              },
    { CR3_Rnum,  "cr3",    "CR3"              },
    { CR4_Rnum,  "cr4",    CR4_PARAM("CR4")   },
    { EFER_Rnum, "efer",   EFER_PARAM("MSR.EFER") },
    { DR0_Rnum,  "dr0",    "DR0"              },
    { DR1_Rnum,  "dr1",    "DR1"              },
    { DR2_Rnum,  "dr2",    "DR2"              },
    { DR3_Rnum,  "dr3",    "DR3"              },
    { DR6_Rnum,  "dr6",    "DR6"              },
    { DR7_Rnum,  "dr7",    "DR7"              }
};

#define REG_TABLE_SIZE  (sizeof(RegTable) / sizeof(RegTable[0]))

static char RDispName[STD_REG_NUM][8];  // display names (case per UprCase) of the RegTable registers
static bx_param_num_c *RegObject[BX_MAX_SMP_THREADS_SUPPORTED][STD_REG_NUM + EXTRA_REGS];
Bit64u rV[EFER_Rnum + 1];   // current values of registers
Bit64u PV[EFER_Rnum + 1];   // previous values of registers
Bit32s GDT_Len;             // "limits" (= bytesize-1) for GDT and IDT
Bit32s IDT_Len;
Bit8u RegColor[STD_REG_NUM+EXTRA_REGS];    // specifies foreground and background color of registers
// Text color is red if the upper bit is set. Background is set according to ColorList.
int RitemToRnum[STD_REG_NUM+EXTRA_REGS];   // mapping from Reg List Item# to register number

bool DumpInitted = FALSE;      // has the MemDump window ever been filled with data?
int DumpAlign = 1;
int PrevDAD;                   // saves "previous DumpAlign value" (forces column autosize)
char *DataDump;
Bit64u DumpStart = 0;          // current emulated address (lin or phys) of DataDump
bool doDumpRefresh;
int DViewMode = VIEW_MEMDUMP;
bool LinearDump = TRUE;        // FALSE = memdump uses physical addressing

char *tmpcb;                   // 512b is allocated in bigbuf
char *CurStack;                // Stack workspace (400b usually)

char bigbuf[outbufSIZE];       // 40K preallocated storage for all char buffers (see DoAllInit)
char *DbgAppendPtr = bigbuf;
char *OutWindow;               // buffer for the Output window
int OutWinCnt = OutWinSIZE;    // available size of OutWindow buffer
int PO_Tdelay = 0;             // delay before displaying partial output lines

int AsmLineCount = 1;          // # of disassembled asm lines loaded
int AsmPgSize = 0;
int ListLineRatio;             // number of vertical pixels in a ListView Item
int ListVerticalPix;           // number of vertical pixels in each List
Bit64u AsmLA[MAX_ASM];         // linear address of each disassembled ASM line

// Command stuff
int CommandHistoryIdx = 0;
char *CmdHistory[CmdHistorySize];  // 64 command History storage (fixed 80b each)
int CmdHInsert = 0;                // index of next history entry to store

int SizeList = 0;
Bit32s xClick = -1;         // halfway through a mouseclick flag + location
Bit32s yClick = 0;          // values are in Listview coordinates

static const char* GDTt2[8] = {
   "16-bit code",
   "64-bit code",
   "32-bit code",
   "16-bit data",
   "64-bit data",
   "32-bit data",
   "Illegal",
   "Unused"
};

static const char* GDTsT[] = { "","Available 16bit TSS","LDT","Busy 16bit TSS","16bit Call Gate",
                "Task Gate","16bit Interrupt Gate","16bit Trap Gate","Reserved",
                "Available 32bit TSS","Reserved","Busy 32bit TSS","32bit Call Gate",
                "Reserved","32bit Interrupt Gate","32bit Trap Gate"
};

// system segment types in long mode (IA-32e mode), LDT, TSS and call gate descriptors are 16 bytes
static const char* GDTsT64[] = { "Reserved","Reserved","LDT","Reserved","Reserved",
                "Reserved","Reserved","Reserved","Reserved",
                "Available 64bit TSS","Reserved","Busy 64bit TSS","64bit Call Gate",
                "Reserved","64bit Interrupt Gate","64bit Trap Gate"
};

static const char *BrkName[5] = {
   "Linear Breakpt",
   "Physical Breakpt",
   "Virtual Breakpt",
   "Write Watchpoint",
   "Read Watchpoint",
};

bx_address BrkLAddr[BX_DBG_MAX_LIN_BPOINTS+1];
unsigned BrkIdx[BX_DBG_MAX_LIN_BPOINTS+1];
int BreakCount = 0;

// Breakpoint Dump Window stuff
unsigned short BrkpIDMap[256];
unsigned short WWP_BaseEntry;
unsigned short RWP_BaseEntry;
unsigned short EndLinEntry;
unsigned short EndPhyEntry;
unsigned short WWPSnapCount;
unsigned short RWPSnapCount;
bx_phy_address WWP_Snapshot[16];
bx_phy_address RWP_Snapshot[16];

static bool global_ini;
static char ini_path[BX_PATHNAME_LEN + 16];
char *debug_cmd;
bool debug_cmd_ready;
bool vgaw_refresh;

static bxevent_handler old_callback = NULL;
static void *old_callback_arg = NULL;

static const short nDock[36] = {     // lookup table for alternate DockOrders
    0x231, 0x312, 0x231, 0x213, 0x132, 0x132,
    0x213, 0x321, 0x123, 0x123, 0x321, 0x312,
    0x213, 0x213, 0x123, 0x312, 0x321, 0x312,
    0x132, 0x123, 0x132, 0x321, 0x231, 0x231,
    0x312, 0x312, 0x231, 0x213, 0x132, 0x213,
    0x132, 0x123, 0x321, 0x321, 0x123, 0x231
};

static int DoMatch(const char *text, const char *p, bool IsCaseSensitive)
{
    // probably the MOST DIFFICULT FUNCTION in TurboIRC
    // Thanks to BitchX for copying this function
    int matched;
    int pT = 0;
    int pP = 0;

    for (; p[pP] != '\0'; pP++, pT++)
    {
        if (text[pT] == '\0' && p[pP] != '*')
            return MATCH_ABORT;
        switch (p[pP])
        {
            default:
                if (IsCaseSensitive)
                {
                    if (text[pT] != p[pP])
                        return MATCH_FALSE;
                    else
                        continue;
                }
                if (toupper((unsigned char) text[pT]) != toupper((unsigned char) p[pP]))
                    return MATCH_FALSE;
                continue;

            case '?':
                continue;

            case '*':
                if (p[pP] == '*')
                    pP++;
                if (p[pP] == '\0')
                    return MATCH_TRUE;
                while (text[pT] != FALSE)
                {
                    matched = DoMatch(text + pT++, p + pP, FALSE);
                    if (matched != MATCH_FALSE)
                        return matched;
                }
                return MATCH_ABORT;

        }
    }
    return (text[pT] == '\0');
}

static int IsMatching(const char *text, const char *p, bool IsCaseSensitive)
{
    if (p[0] == '*' && p[1] == '\0')
        return MATCH_TRUE;
    return (DoMatch(text, p, IsCaseSensitive) == MATCH_TRUE);
}

// utility function for list resizing operation -- set LoX and HiX
// the resize operation exits if the mouse moves beyond LoX or HiX
void SetHorzLimits()
{
    int i;
    if (Sizing == -2)  // is it the left or right bar?
    {
        Resize_LoX = OneCharWide << 2;      // set horizontal limits
        i = ListWidthPix[(DockOrder >> 8) -1];  // col1 width
        // calculate end of col2 - 4 charwidths in parent coordinates
        Resize_HiX = i + ListWidthPix[CurCenterList] - (OneCharWide << 2);
        Sizing = 1;
    }
    else
    {
        i = ListWidthPix[(DockOrder >> 8) -1];  // col1 width
        Resize_LoX = i + (OneCharWide << 2);    // set horizontal limits
        // calculate total width - 4 charwidths in parent coordinates
        i = ListWidthPix[REG_WND] + ListWidthPix[ASM_WND] + ListWidthPix[DUMP_WND];
        Resize_HiX = i - (OneCharWide << 2);
        Sizing = 2;
    }
}

void DockResize (int DestIdx, Bit32u ParentX)
{
    if (Sizing >= 10)       // dock operation
    {
        int Siz = Sizing - 10;  // calculate which list initiated dock = moving window
        if (Siz != DestIdx)     // moving window = destination window is a no-op
        {
            // Convert Sizing and DestIdx into a table lookup index (j)
            // -- otherwise, the "algorithm" to compute new DockOrder is annoying
            int j = (Siz*2 + ((Siz | DestIdx) & 1)) *6;
            if (Siz == 1)
                j = (Siz*4 + (DestIdx & 2)) *3;

            // convert current DockOrder to a number from 0 to 5, add to j
            j += ((DockOrder >> 7) - 2) &6;
            if (((DockOrder >> 4) &3) > (DockOrder & 3))
                j += 1;
            DockOrder = nDock[j];
            MoveLists();
        }
    }
    else    // resize operation
    {
        int idx, totpix;
        if (Sizing == 1)
        {
            idx = (DockOrder >> 8) -1;          // sizing the left bar
            totpix = ListWidthPix[idx] + ListWidthPix[CurCenterList];
            ListWidthPix[idx] = ParentX;
            ListWidthPix[CurCenterList] = totpix - ParentX; // reset the widths of the left and center windows
        }
        else
        {
            ParentX -= ListWidthPix[(DockOrder >> 8) -1];       // caclulate new width of center window
            idx = (DockOrder & 3) -1;
            totpix = ListWidthPix[idx] + ListWidthPix[CurCenterList];
            ListWidthPix[CurCenterList] = ParentX;
            ListWidthPix[idx] = totpix - ParentX;   // reset the widths of the right and center windows
        }

        MoveLists();
    }
}

// Convert a string (except for the 0x in a hex number) to uppercase
static void upr(char* d)
{
    char *p;
    p = d;
    while (*p != 0)
    {
        if (*p == '0' && p[1] == 'x')
            p += 2;
        else
        {
            *p = toupper((unsigned char) *p);
            ++p;
        }
    }
}

// print val as a zero-padded hex number of 4, 8 or 16 digits (no "0x"),
// in upper case if UprCase is set
static void FmtHex(char *buf, size_t len, Bit64u val, int digits)
{
    if (digits <= 4)
        snprintf(buf, len, "%04x", (unsigned) (val & 0xffff));
    else if (digits <= 8)
        snprintf(buf, len, "%08x", (Bit32u) val);
    else
        snprintf(buf, len, FMT_ADDRX64, val);
    if (UprCase)
        upr(buf);
}

// get the current value of a register directly from the param tree (0 if not available)
static Bit64u GetRegObjValue(int rnum)
{
    bx_param_num_c *p = RegObject[CurrentCPU][rnum];
    if (p == NULL)
        return 0;
    return p->get64();
}

// insert one row into the Register list -- hex and dec columns are optional (NULL)
static int AddRegRow(int itemnum, int rnum, const char *name, const char *hex, const char *dec, int group)
{
    char *cols[3];
    int ncols = 1;

    cols[0] = (char *) name;
    if (hex != NULL)
    {
        cols[ncols++] = (char *) hex;
        if (dec != NULL)
            cols[ncols++] = (char *) dec;
    }
    RitemToRnum[itemnum] = rnum;    // always recreate the register -> itemnum mapping
    InsertListRow(cols, ncols, REG_WND, itemnum, group);
    return itemnum + 1;
}

// change the display on the status line if anything has changed
void UpdateStatus()
{
    static bool PrevAtBreak = FALSE;

    if (! StatusChange) return;  // avoid sending unnecessary messages/invalidations
    StatusChange = FALSE;

    if (AtBreak)   // modify status line only during a break
    {
        char eflags_buf[80];
        stringify_EFLAGS((Bit32u) rV[EFL_Rnum], eflags_buf);     // prints out eflags
        SetStatusText(3, eflags_buf);   // display eflags

        if (CpuModeChange)     // Did CR0 bits or EFER bits change value?
        {
            CpuModeChange = FALSE;
            char mode[32];
            switch (CpuMode) {
                case BX_MODE_IA32_REAL:
                    if (! In32Mode)
                        strcpy (mode, "CPU: Real Mode 16");
                    else
                        strcpy (mode, "CPU: Real Mode 32");
                    break;
                case BX_MODE_IA32_V8086:
                    strcpy (mode, "CPU: V8086 Mode");
                    break;
                case BX_MODE_IA32_PROTECTED:
                    if (! In32Mode) {
                        if (InPaging)
                            strcpy (mode, "CPU: Protected Mode 16 (PG)");
                        else
                            strcpy (mode, "CPU: Protected Mode 16");
                    }
                    else {
                        if (InPaging)
                            strcpy (mode, "CPU: Protected Mode 32 (PG)");
                        else
                            strcpy (mode, "CPU: Protected Mode 32");
                    }
                    break;
                case BX_MODE_LONG_COMPAT:
                    strcpy (mode, "CPU: Compatibility Mode");
                    break;
                case BX_MODE_LONG_64:
                    strcpy (mode, "CPU: Long Mode");
                    break;
            }
            SetStatusText(1, mode);    // display CPU mode in status col#1
        }
    }

    if (AtBreak != PrevAtBreak)
        // switch window color ("active"=white or gray), depending on AtBreak value
        MakeListsGray();
    PrevAtBreak = AtBreak;
}

// convert a user typed number to binary: decimal, hex with 0x prefix, or
// hex without prefix if it contains hex letters (e.g. "1f").
// A leading '-' is allowed only if negok (decimal only).
static Bit64u ParseNumber(const char *s, bool negok)
{
    char *end;

    while (*s == ' ' || *s == '\t')
        ++s;
    if (*s == '-')
    {
        if (! negok)
            return 0;
        return 0 - (Bit64u) strtoull(s + 1, NULL, 10);
    }
    Bit64u val = strtoull(s, &end, 10);
    if (isxdigit((unsigned char) *end) || *end == 'x' || *end == 'X')
        val = strtoull(s, NULL, 16);    // also accepts the 0x prefix
    return val;
}

// "singlestep" disassembly lines from the internal debugger are sometimes ignored
static bool isSSDisasm(const char *s)
{
    if (! ignSSDisasm)   // ignoring those lines?
        return FALSE;

    while (*s == ' ')   // need to parse the line to see if it is ASM
        ++s;
    if (*s != '(')      // first char must be (
        return FALSE;
    while (*s != '[' && *s != 0)    // then there must be a [
        ++s;
    if (*s == 0)
        return FALSE;
    while (*s != 0 && (*s != ')' || s[1] != ':' || s[2] != ' '))
        ++s;
    if (*s == 0)
        return FALSE;
    while (*s != ';' && *s != 0)    // last, there must be a ;
        ++s;
    if (*s == 0)
        return FALSE;
    return TRUE;
}

// dump output from the bochs internal debugger to Output window
// Note: this routine may be called *DIRECTLY* from bochs!
void ParseIDText(const char *x)
{
    int i = 0;
    int overflow = 0;
    while (*x !=0 && *x != '\r' && *x != '\n' && DbgAppendPtr < tmpcb)
        *(DbgAppendPtr++)= *(x++);  // append the chars from x into the bigbuf
    if (DbgAppendPtr >= tmpcb)      // overflow error?
    {
        DispMessage("Debugger output cannot be parsed -- line too long","Buffer overflow");
        DbgAppendPtr = bigbuf;      // throw away the line
        return;
    }

    *DbgAppendPtr = 0;
    if (*x == 0)            // automatically process only complete lines further
    {
        PO_Tdelay = 2;      // wait a half second, then force display of partial lines
        return;
    }
    PO_Tdelay = 0;          // line completed -- cancel any partial output time delay

    // restart DbgAppendPtr at the beginning of a new line buffer
    char *s = DbgAppendPtr = bigbuf;    // s -> raw text line from debugger
    if (ignoreNxtT)
    {
        if (strncmp(s,"Next at t",9) == 0)
            return;
    }
    if (isSSDisasm(s))
        return;

    while ((*s >= ' ' || *s == '\t') && i < 204)    // scan out to eol, count chars
    {
        ++i;
        ++s;
    }
    if (i > 203)    // max out at 203 chars per line (should never happen)
    {
        i = 200;
        overflow = 3;
    }
    char *p = OutWindow;
    if ((i+overflow+2+useCR) > OutWinCnt)       // amt needed vs. space available
    {
        s = OutWindow;      // need to toss lines off beginning of OutWindow
        int j = OutWinCnt - overflow - 2 - useCR;
        while (j < i)       // throw away one line at a time
        {
                // stop on any unprintable char < ' '
            while ((unsigned char)*s >= ' ' || *s == '\t')
            {
                ++s;
                ++j;    // increase available space as chars are tossed
            }
             // in reality, s must be pointing at an EOL
            s += 1 + useCR;
            j += 1 + useCR;
        }
        OutWinCnt = j + overflow + 2 + useCR;
        j = OutWinSIZE - OutWinCnt; // chars to copy, without the terminal zero
        while (j-- > 0)         // recopy the OutWindow buffer up
            *(p++) = *(s++);
    }
    else
        p = OutWindow + OutWinSIZE - OutWinCnt;
    OutWinCnt -= i + overflow + 1 + useCR;
    if (useCR != 0)
        *(p++) = '\r';          // end of buf only had a 0 in it,
    *(p++) = '\n';          // and needs an EOL to display properly
    s = bigbuf;
    while (i-- > 0)         // copy the new output line onto the buffer
        *(p++) = *(s++);
    if (overflow != 0)
    {
        *(p++) = '.';       // just for fun, if the line overflows
        *(p++) = '.';
        *(p++) = '.';
    }
    *p = 0;
    SetOutWinTxt();     // OS specific routine to replace Output window's text
}

// load appropriate register values from simulation into local rV[] array
static void FillRegs()
{
    int i = EFER_Rnum + 1;      // EFER is the highest reg # in rV
    while (--i >= 0)
    {
        if (RegObject[CurrentCPU][i] != NULL)
            rV[i] = RegObject[CurrentCPU][i]->get64();
    }
#if BX_SUPPORT_X86_64 == 0
    // copy RIP, RSP from EIP, ESP -- so LAs for both are always easily available
    rV[RIP_Rnum] = rV[EIP_Rnum];
    rV[RSP_Rnum] = rV[ESP_Rnum];
#else
    // copy the lower dwords of RAX - RIP to EAX - EIP (with 32bit truncation)
    i = RIP_Rnum + 1;
    while (--i >= 0)
        rV[i + (EAX_Rnum - RAX_Rnum)] = GET32L(rV[i]);
#endif
    if (RegObject[CurrentCPU][GDTR_Lim] != NULL)        // get the limits on GDT and IDT
        GDT_Len = RegObject[CurrentCPU][GDTR_Lim]->get();
    if (RegObject[CurrentCPU][IDTR_Lim] != NULL)
        IDT_Len = RegObject[CurrentCPU][IDTR_Lim]->get();

    // Check CR0 bit 31 -- Paging bit
    Bit32u NewPg = (Bit32u) rV[CR0_Rnum] & 0x80000000;
    if (InPaging != NewPg)
    {
        GrayMenuItem ((int) NewPg, CMD_PAGEV);
        StatusChange = TRUE;
    }
    InPaging = NewPg;
}

// grab linear breakpoints out of internal debugger's bx_guard structures, and sort them
void ParseBkpt()
{
    extern bx_guard_t bx_guard;
    int k;
    int j = 0;
    int i = bx_guard.iaddr.num_linear;
    while (--i >= 0)
    {
        if (bx_guard.iaddr.lin[i].enabled)
        {
            BrkLAddr[j] = bx_guard.iaddr.lin[i].addr;
            BrkIdx[j] = bx_guard.iaddr.lin[i].bpoint_id;
            ++j;
        }
    }
    BreakCount = i = j;
    // sort the breakpoint list (linear sort), to make it faster to search
    while (--i > 0)
    {
        j = k = i;
        while (--j >= 0)
        {
            if (BrkLAddr[j] > BrkLAddr[k])      // find the next biggest
                k = j;
        }
        if (k < i)
        {
            bx_address h = BrkLAddr[i]; // do the swap on BOTH arrays
            j = BrkIdx[i];
            BrkLAddr[i] = BrkLAddr[k];
            BrkIdx[i] = BrkIdx[k];
            BrkLAddr[k] = h;
            BrkIdx[k] = j;
        }
    }
}

// this routine is only called if debugger already knows SSE is supported
// -- but it might not be "turned on", either
static int FillSSE(int LineCount)
{
#if BX_CPU_LEVEL >= 6
    char name[12];
    char hi[20], lo[20];
    char val[48];

    if ((rV[CR0_Rnum] & 0xc) != 0)  // TS or EM flags in CR0 temporarily disable SSE
        return AddRegRow(LineCount, XMM0_Rnum, "SSE-off", NULL, NULL, LGRP_SSE);

    // format: XMM[#] 0x00000000:00000000 (each 16 hex digits)
    // the hex value goes into the decimal column -- more room there!
    for (int i = 0; i < /*BX_XMM_REGISTERS*/ 16; i++)
    {
        sprintf(name, "XMM[%d]", i);
        FmtHex(hi, sizeof(hi), GetRegObjValue(XMM0_hi + i), 16);   // "SSE.xmm[i]_hi"
        FmtHex(lo, sizeof(lo), GetRegObjValue(XMM0_Rnum + i), 16); // "SSE.xmm[i]_lo"
        snprintf(val, sizeof(val), "0x%s : %s", hi, lo);
        LineCount = AddRegRow(LineCount, XMM0_Rnum + i, name, "", val, LGRP_SSE);
    }
#endif
    return LineCount;
}

#if BX_SUPPORT_FPU
extern double x87_to_double(Bit16u signExp, Bit64u signif);

// this routine is only called if debugger already knows FPU is supported
// -- but it might not be active
static int FillMMX(int LineCount)
{
    Bit16u exponent[8];
    char name[12];
    char hex[20];
    char dec[32];
    int i;

    if ((rV[CR0_Rnum] & 0xc) != 0)  // TS or EM flags in CR0 temporarily disable MMX/FPU/SSE
        return AddRegRow(LineCount, ST0_Rnum, "FPU-off", NULL, NULL, LGRP_FPU);

    // format: MM#-ST# 0000000000000000 then FPU float value in "decimal" column
    for (i = 0; i < 8; i++)
    {
        Bit64u mmreg = GetRegObjValue(ST0_Rnum + i);        // "mmx(i)" register
        exponent[i] = (Bit16u) GetRegObjValue(ST0_exp + i); // exponent for this FPU register
        sprintf(name, "MM%d-ST%d", i, i);
        FmtHex(hex, sizeof(hex), mmreg, 16);
        snprintf(dec, sizeof(dec), "%.3e", x87_to_double(exponent[i], mmreg));
        LineCount = AddRegRow(LineCount, ST0_Rnum + i, name, hex, dec, LGRP_FPU);
    }
    for (i = 0; i < 8; i++)
    {
        sprintf(name, "ST%d.exp", i);
        FmtHex(hex, sizeof(hex), exponent[i], 4);
        sprintf(dec, "%u", exponent[i]);
        LineCount = AddRegRow(LineCount, ST0_exp + i, name, hex, dec, LGRP_FPU);
    }
    return LineCount;
}
#endif

// get values of Debug registers from simulation
static int FillDebugRegs(int itemnum)
{
    char hex[20];
    int digits = 8;

#if BX_SUPPORT_X86_64
    if (In64Mode)
        digits = 16;
#endif
    for (int i = DR0_Rnum; i <= DR7_Rnum; i++)
    {
        FmtHex(hex, sizeof(hex), GetRegObjValue(i), digits);
        itemnum = AddRegRow(itemnum, i, RDispName[i], hex, NULL, LGRP_DBG);
    }
    return itemnum;
}

#define BX_GUI_DB_ASM_DATA (4400)

// Disassemble a linear memory area, in a loop, loading text into ASM window
// completely update the ASM display with new data
static void FillAsm(Bit64u LAddr, int MaxLines)
{
    Bit64u ReadAddr = LAddr;
    int BufLen = 0;
    int i, len;
    bool BufEmpty;
    bool Go = TRUE;
    char AsmData[BX_GUI_DB_ASM_DATA];  // 5K for binary disassembly data
    char *s, *p = AsmData;  // just to avoid a compiler warning
    char *cols[3];
    char asmtxt[200];

    cols[0] = asmtxt;
    cols[1] = asmtxt + 36;
    cols[2] = asmtxt + 100;
    AsmLineCount = 0;           // initialize for disasm window update
    StartListUpdate(ASM_WND);
    if (MaxLines > MAX_ASM)     // just for protection
        MaxLines = MAX_ASM;

    while (Go)
    {
        // copydown buffer -- buffer size must be 4K + 16
        s= AsmData;
        i= BufLen;      // BufLen is guaranteed < 16
        while (i-- > 0)
            *(s++)= *(p++);
        // load buffer, up to the next 4k boundary
        len = 4096 - (((int) ReadAddr) & 0xfff);    // calculate read amount
        Go = bx_dbg_read_linear(CurrentCPU, ReadAddr, len, (Bit8u *) s);
        BufLen += len;
        ReadAddr += len;
        if (Go == FALSE)
            break;
        BufEmpty = FALSE;
        p= AsmData;         // start at the beginning of the new buffer
        while (AsmLineCount < MaxLines && ! BufEmpty)
        {
            // disassemble 1 line with a direct call, into asmtxt
            len = bx_dbg_disasm_wrapper(In32Mode, In64Mode, (bx_address) 0,
                  (bx_address) LAddr, (Bit8u *) p, cols[2]);

            if (len <= BufLen)      // disassembly was successful?
            {
                AsmLA[AsmLineCount] = LAddr;        // save, and
                // "display" linear addy of the opcode
                FmtHex(asmtxt, 36, LAddr, In64Mode ? 16 : 8);

                BufLen -= len;  // used up len bytes from buffer
                LAddr += len;   // calculate next LAddr

                // then build the "bytes" column entry
                s = cols[1];    // begin with the bytecount in parens
                s += sprintf(s, "(%d) ", len);
                while (len-- > 0)
                    s += sprintf(s, "%02X", (unsigned char) *(p++));

                // then, finalize the ASM disassembly text
                if (UprCase)   // do any requested uppercase conversion on the text
                    upr(cols[2]);
                InsertListRow(cols, 3, ASM_WND, AsmLineCount, LGRP_NONE);   // 3 cols, "no" group
                ++AsmLineCount;
            }
            else
                BufEmpty = TRUE;
        }
        if (AsmLineCount >= MaxLines)       // disassembled enough lines?
            Go = FALSE;
        if (! In64Mode && ReadAddr > 0xffffffff)
            break;
    }
    if (ResizeColmns)
        RedrawColumns(ASM_WND);
    EndListUpdate(ASM_WND);
}


// add GP registers (64 bit registers first, if they exist) and EFLAGS to the Register list
static int AddGPRegs(int itemnum)
{
    char hex[20];
    char dec[24];
    bool showEreg = TRUE;
    int i;

#if BX_SUPPORT_X86_64
    if (In64Mode)
    {
        showEreg = SeeReg[0];       // get user option setting for EAX, etc.
        int LastGPR = R15_Rnum;
#if BX_SUPPORT_APX
        if (BX_CPU(CurrentCPU)->is_cpu_extension_supported(BX_ISA_APX))
            LastGPR = R31_Rnum;     // APX extended GPRs R16-R31
#endif
        for (i = RAX_Rnum; i <= LastGPR; i++)
        {
            FmtHex(hex, sizeof(hex), rV[i], 16);
            sprintf(dec, FMT_LL "d", rV[i]);
            itemnum = AddRegRow(itemnum, i, RDispName[i], hex, dec, LGRP_GPR);
        }
    }
#endif

    // then 32bit GP registers (if appropriate)
    if (showEreg)
    {
        for (i = EAX_Rnum; i <= EIP_Rnum; i++)
        {
            const char *name = RDispName[i];
            if (! In32Mode && i == EIP_Rnum)   // Check for Real Mode (Pmode is TRUE in Long Mode)
            {
                rV[EIP_Rnum] &= 0xffff;     // in Real Mode, mask IP to 2 bytes
                ++name;                     // and shorten name to 2 letters
            }
            FmtHex(hex, sizeof(hex), rV[i], 8);
            sprintf(dec, FMT_LL "d", rV[i]);
            itemnum = AddRegRow(itemnum, i, name, hex, dec, LGRP_GPR);
        }
    }

    // always insert eflags next
    FmtHex(hex, sizeof(hex), rV[EFL_Rnum], 8);
    return AddRegRow(itemnum, EFL_Rnum, RDispName[EFL_Rnum], hex, NULL, LGRP_GPR);
}

// add segment registers to the Register list
static int AddSegRegs(int itemnum)
{
    char hex[8];

    for (int i = CS_Rnum; i <= GS_Rnum; i++)
    {
        FmtHex(hex, sizeof(hex), rV[i], 4);
        itemnum = AddRegRow(itemnum, i, RDispName[i], hex, NULL, LGRP_SEG_SYS);
    }
    return itemnum;
}

// add system registers (GDTR, IDTR, LDTR, TR) to the Register list
static int AddSysRegs(int itemnum)
{
    char hex[32];
    int last = TRRnum;

    if (! In32Mode)      // don't show ldtr or tr in Real mode
        last = IDTRnum;
    for (int i = GDTRnum; i <= last; i++)
    {
        if (i == GDTRnum || i == IDTRnum)
        {
            // base, then limit in parens
            Bit16u limit = (i == GDTRnum) ? GDT_Len : IDT_Len;
            FmtHex(hex, sizeof(hex), rV[i], In64Mode ? 16 : 8);
            size_t len = strlen(hex);
            snprintf(hex + len, sizeof(hex) - len, UprCase ? " (%4X)" : " (%4x)", (unsigned) limit);
        }
        else
            FmtHex(hex, sizeof(hex), rV[i], 4);     // ldtr, tr
        itemnum = AddRegRow(itemnum, i, RDispName[i], hex, NULL, LGRP_SEG_SYS);
    }
    return itemnum;
}

// add control registers (CR0 - CR4, EFER) to the Register list
static int AddCtrlRegs(int itemnum)
{
    char hex[20];

    for (int i = CR0_Rnum; i <= EFER_Rnum; i++)
    {
        int digits = 8;
        if (In64Mode && (i == CR2_Rnum || i == CR3_Rnum))
            digits = 16;
        FmtHex(hex, sizeof(hex), rV[i], digits);
        itemnum = AddRegRow(itemnum, i, RDispName[i], hex, NULL, LGRP_CTRL);
    }
    return itemnum;
}

// Reload the entire Register window with data
static void LoadRegList()
{
    int i, itemnum = 0;

    StartListUpdate(REG_WND);
    FillRegs();         // get new values for rV local register array

    itemnum = AddGPRegs(itemnum);
    if (rV[EFL_Rnum] != PV[EFL_Rnum])
        StatusChange = TRUE;    // if eflags changed, force a status update

    if (SeeReg[1])      // display Segment registers (if requested)
        itemnum = AddSegRegs(itemnum);
    // display System registers (if requested)
    // displaying these once may be necessary for column resizing
    if (SeeReg[2] || (ResizeColmns != FALSE && NeedSysRresize))
        itemnum = AddSysRegs(itemnum);
    if (SeeReg[3])      // display Control Registers (if requested)
        itemnum = AddCtrlRegs(itemnum);

    // set the "red" flag in the register colors if value changed
    for (i = 0; i <= EFER_Rnum; i++)
    {
        if (rV[i] != PV[i])
            RegColor[i] |= RCLR_CHANGED;
        else
            RegColor[i] &= ~RCLR_CHANGED;
    }

// Load any optional STi, MMX, SSE, DRx, TRx register info into the Register window
#if BX_SUPPORT_FPU
    // MMX-FPU registers
    if (SeeReg[4] != FALSE)
        itemnum = FillMMX(itemnum);
#endif

    // SSE registers
    if (CpuSupportSSE && SeeReg[5] != FALSE)
        itemnum = FillSSE(itemnum);

    // Internal x86 Debug Registers
    if (SeeReg[6] != FALSE)
        itemnum = FillDebugRegs(itemnum);

    RedrawColumns(REG_WND);     // resize Hex Value column sometimes
    EndListUpdate(REG_WND);
}

// scroll ASM window so that the current line is in the "middle"
static void doAsmScroll()
{
    int j;
    int CurTopIdx = GetASMTopIdx();
    int nli = -2;
    // Can the current line be displayed at all?
    if (CurrentAsmLA < *AsmLA || CurrentAsmLA > AsmLA[AsmLineCount-1])
        return;
    //  convert from LA to a Line Index (nli) with a search
    j = CurTopIdx;      // try to start at CurTopIdx
    if (AsmLA[j] > CurrentAsmLA)
        j = 0;
    while (nli < 0 && j < AsmLineCount && AsmLA[j] <= CurrentAsmLA)
    {
        if (AsmLA[j] == CurrentAsmLA)
            nli = j;
        ++j;
    }
    // not found -> CurrentAsmLA is an illegal opcode address
    if (nli < 0)
        return;
    // is the current line ALREADY in the middle of the window?
    if (nli < CurTopIdx || nli >= CurTopIdx + AsmPgSize - bottommargin)
    {
        // need to scroll!
        int ScrollLines = nli - CurTopIdx - topmargin;
        j = AsmLineCount - CurTopIdx - AsmPgSize;
        // limit ScrollLines by the theoretical max and min
        if (ScrollLines > j)
            ScrollLines = j + 1;        // just a little extra to make sure
        if (ScrollLines < -CurTopIdx)
            ScrollLines = -CurTopIdx - 1;   // just a little extra to make sure
        // convert # of scroll lines to pixels
        ScrollASM (ScrollLines * ListLineRatio);
    }
    Invalidate(ASM_WND);        // "current opcode" in ASM window needs redrawing
}

// try to find a Linear Address to start a "pretty" autodisassembly
static void CanDoLA(Bit64u *h)
{
    int index;
    if (TopAsmLA > *h || *h > AsmLA[AsmLineCount-1])    // is it hopeless?
        return;
    if (bottommargin > AsmLineCount)
        index = 0;
    else
        index = AsmLineCount - bottommargin;
    while (++index < AsmLineCount)
    {
        if (AsmLA[index] == *h)
        {
            *h = AsmLA[index - topmargin];
            return;
        }
    }
}

// reload the ASM window around the current instruction
static void ReloadAsm()
{
    if (AtBreak)
    {
        Bit64u h = CurrentAsmLA;
        CanDoLA(&h);
        FillAsm(h, DefaultAsmLines);
    }
    else
        BottomAsmLA = ~0;   // force an ASM autoload
}

// set the scroll limits for a freshly loaded ASM window
static void SetAsmScrollLimits()
{
    BottomAsmLA = *AsmLA;
    int j = bottommargin;       // try to use this bottom margin on ASM window
    if (j > AsmLineCount)
        j = AsmLineCount;
    TopAsmLA = AsmLA[AsmLineCount - j];     //TopAsmLA is the scroll point
}

static void InitRegObjects()
{
    bx_list_c *cpu_list;
    extern bx_list_c *root_param;
    char pname[20];
    unsigned n;
    int i;
    int cpu = BX_SMP_PROCESSORS;
    // get the param tree interface objects for every single register on all CPUs
    while (--cpu >= 0)
    {
        // RegObject[j]s are all initialized to NULL when allocated in the BSS area
        // but it doesn't hurt anything to do it again, once
        for (i = 0; i < STD_REG_NUM + EXTRA_REGS; i++)
            RegObject[cpu][i] = (bx_param_num_c *) NULL;

        sprintf (pname,"bochs.cpu%d", cpu);    // set the "cpu number" for cpu_list
        cpu_list = (bx_list_c *) SIM->get_param(pname, root_param);

        // all registers with a single param tree value
        for (n = 0; n < REG_TABLE_SIZE; n++)
        {
            if (RegTable[n].param != NULL)
                RegObject[cpu][RegTable[n].rnum] = SIM->get_param_num(RegTable[n].param, cpu_list);
        }
        // limit parts of the GDTR and IDTR
        RegObject[cpu][GDTR_Lim] = SIM->get_param_num("GDTR.limit", cpu_list);
        RegObject[cpu][IDTR_Lim] = SIM->get_param_num("IDTR.limit", cpu_list);

#if BX_SUPPORT_FPU
        for (i = 0; i < 8; i++)
        {
            sprintf (pname, "FPU.st%d.fraction", i);
            RegObject[cpu][ST0_Rnum + i] = SIM->get_param_num(pname, cpu_list);
            sprintf (pname, "FPU.st%d.exp", i);
            RegObject[cpu][ST0_exp + i] = SIM->get_param_num(pname, cpu_list);
        }
#endif

#if BX_CPU_LEVEL >= 6
        CpuSupportSSE = SIM->get_param("SSE", cpu_list) != NULL;

        if (CpuSupportSSE) {
#if BX_SUPPORT_X86_64
            int nxmm = 16;
#else
            int nxmm = 8;
#endif
            for (i = 0; i < nxmm; i++)
            {
                sprintf (pname, "SSE.xmm%02d_0", i);
                RegObject[cpu][XMM0_Rnum + i] = SIM->get_param_num(pname, cpu_list);
                sprintf (pname, "SSE.xmm%02d_1", i);
                RegObject[cpu][XMM0_hi + i] = SIM->get_param_num(pname, cpu_list);
            }
        }
#endif
    }
}

static void FillStack();
static void RefreshDataDump(Bit8u *buffer);

void doUpdate()
{
    Bit8u tmpbuffer[4096];

    if (doSimuInit)
        SpecialInit();
    // begin an autoupdate of Register and Asm windows
    LoadRegList();      // build and show ListView
    ParseBkpt();        // get the linear breakpoint list
    if (DViewMode == VIEW_STACK) {  // in stack view mode, keep the stack updated
        FillStack();
    } else if ((DViewMode == VIEW_MEMDUMP) && DumpInitted) {
        RefreshDataDump(tmpbuffer);
        if (memcmp(DataDump, tmpbuffer, 4096)) {
            doDumpRefresh = TRUE;
        }
    }
    CurrentAsmLA = BX_CPU(CurrentCPU)->get_laddr(BX_SEG_REG_CS, (bx_address) rV[RIP_Rnum]);
    if (CurrentAsmLA < BottomAsmLA || CurrentAsmLA > TopAsmLA)
    {
        Bit64u h = CurrentAsmLA;
        // generate a startLA (= h) that overlaps by topmargin, if possible
        CanDoLA(&h);

        FillAsm(h, DefaultAsmLines);

        // Set the scroll position for the new ASM window
        SetAsmScrollLimits();
    }
    else
        doAsmScroll();      // ASM window may need to scroll
    ResizeColmns = FALSE;   // done with reformatting, if it was needed
    UpdateStatus();         // Mode and Eflags may have changed status
}

#define DUMP_COLS       18  // number of columns in the DUMP_WND list
#define DUMP_INFO_COL   17  // last column: "info" text, decimal value or ascii dump
#define DUMP_COL_LEN    48  // fits 32 hex digits (16 byte word size) or 16 ascii chars

// one row of the DUMP_WND list, every column has its own storage and starts empty
// (do not copy a DumpRow -- cols[] points into its own txt[])
struct DumpRow {
    char *cols[DUMP_COLS];              // what InsertListRow receives
    char txt[DUMP_COLS][DUMP_COL_LEN];  // private storage for each column

    DumpRow() {
        for (int i = 0; i < DUMP_COLS; i++) {
            txt[i][0] = 0;
            cols[i] = txt[i];
        }
    }
};

// Fill the GDT ListView, reading GDT data directly from bochs linear mem
static void FillGDT()
{
    unsigned int i, j, GroupId;
    unsigned int row = 0;
    unsigned int k = (GDT_Len + 1) / 8;
    Bit8u gdtbuf[16];
    doDumpRefresh = FALSE;

    Bit64u laddr = rV[GDTRnum];
    StartListUpdate(DUMP_WND);

#if BX_SUPPORT_X86_64
    bool LongMode = (CpuMode == BX_MODE_LONG_64 || CpuMode == BX_MODE_LONG_COMPAT);
#endif

    for(i = 0; i < k; i++)
    {
        DumpRow r;      // cols: 0 = selector, 1 = base, 2 = limit, 3 = DPL, 17 = info
        // read 2 dwords from bochs linear mem into "buffer"
        sprintf(r.txt[0], "%02u (Selector 0x%04X)", i, i << 3);
        if (bx_dbg_read_linear(CurrentCPU, laddr, 8, gdtbuf) == FALSE)      // abort the current GDT dump on a memory error
        {
            strcpy (r.txt[DUMP_INFO_COL],"illegal address");
            InsertListRow(r.cols, DUMP_COLS, DUMP_WND, row, LGRP_NONE);
            break;      // the list is finalized after the loop
        }
        laddr += 8;

        // in long mode LDT, TSS and call gate descriptors take two GDT slots (16 bytes)
        bool Desc16 = FALSE;
#if BX_SUPPORT_X86_64
        if (LongMode && (gdtbuf[5] & 0x10) == 0 && (i + 1) < k)
        {
            unsigned type = gdtbuf[5] & 0xf;
            if (type == 0x2 || type == 0x9 || type == 0xb || type == 0xc)
            {
                if (bx_dbg_read_linear(CurrentCPU, laddr, 8, gdtbuf + 8) != FALSE)
                {
                    Desc16 = TRUE;
                    laddr += 8;
                }
            }
        }
#endif

        // enforce proper littleendianness on the gdtbuf bytes
        Bit32u limit = gdtbuf[0] | ((Bit32u) gdtbuf[1] << 8);
        limit |= ((Bit32u)gdtbuf[6] & 0xf) << 16;
        if ((gdtbuf[6] & 0x80) != 0)       // 'Granularity' bit = 4K limit multiplier
            limit = limit * 4096 + 4095;   // and the bottom 12 bits aren't tested

        GroupId = LGRP_NONE; // default to "blank" group
        j = 8;              // default GDT type (group) is blank
        if ((gdtbuf[5] & 0x10) == 0)    // 'S' bit clear = System segment
        {
            GroupId = 1;
            if (limit == 0 && (gdtbuf[5] & 0x80) == 0)      // 'P' (present) bit
                GroupId = 0;
            // the approprate info string for the GDT system segment type
#if BX_SUPPORT_X86_64
            if (LongMode)
                strcpy(r.txt[DUMP_INFO_COL], GDTsT64[(int) (gdtbuf[5] & 0xf)]);
            else
#endif
                strcpy(r.txt[DUMP_INFO_COL], GDTsT[(int) (gdtbuf[5] & 0xf)]);
        }
        else        // it's an 'executable' code or data segment
        {
            j = (gdtbuf[6] & 0x60) >> 5;    // get the 'L' and 'D/B' bits
            if (j == 3)         // both bits set is illegal
                j = 6;
            else if ((gdtbuf[5] & 0x8) == 0)
                j += 3;         // data seg -> j= 3 to 5, code seg -> j= 0 to 2

#ifndef IS_WIN98
            GroupId = j;            // use GroupIDs on XP and higher systems
#else
            strcpy(r.txt[DUMP_INFO_COL], GDTt2[j]);    // otherwise, put descriptive text in "info" column
#endif
        }

        // enforce proper littleendianness on the gdtbuf bytes
        Bit64u base = gdtbuf[2] | ((Bit32u) gdtbuf[3] << 8);
        base |= (Bit32u) gdtbuf[4] << 16;
        base |= (Bit32u) gdtbuf[7] << 24;
        if (Desc16)     // upper 32 bits of the base are in the second half of the descriptor
            base |= (Bit64u) (gdtbuf[8] | ((Bit32u) gdtbuf[9] << 8) | ((Bit32u) gdtbuf[10] << 16) | ((Bit32u) gdtbuf[11] << 24)) << 32;

        if ((gdtbuf[6] & 0x60) == 0x20)     // test for longmode segment
        {
            base = 0;       // the base is always 0 in longmode, with "no" limit
            sprintf(r.txt[2],"0xFFFFFFFFFFFFFFFF");
        }
        else
            sprintf(r.txt[2],"0x%X",limit);
        sprintf(r.txt[1],"0x" FMT_LLCAPX,base);
        sprintf(r.txt[3],"%u", (gdtbuf[5] & 0x60) >> 5);

        if (i == 0)
            strcpy(r.txt[DUMP_INFO_COL], GDTt2[7]);    // call "Null" selector "unused"
        InsertListRow(r.cols, DUMP_COLS, DUMP_WND, row, GroupId);
        ++row;
        if (Desc16)
            ++i;        // skip the GDT slot holding the second half of the descriptor
    }

    RedrawColumns(DUMP_WND);
    EndListUpdate(DUMP_WND);
}

// Fills the IDT ListView, reading IDT data directly from bochs linear mem
static void FillIDT()
{
    Bit64u laddr;
    Bit8u idtbuf[16];
    Bit16u sel;
    Bit32u ofs;
    unsigned int i;
    DumpRow r;          // cols: 0 = vector, 17 = gate selector:offset
    char *info = r.txt[DUMP_INFO_COL];
    doDumpRefresh = FALSE;

    unsigned entrysize = 8;     // protected and V8086 mode: 8 byte gates
    if (CpuMode == BX_MODE_IA32_REAL)
        entrysize = 4;          // real mode: 4 byte IVT entries (even with 32-bit CS)
    else if (CpuMode == BX_MODE_LONG_64 || CpuMode == BX_MODE_LONG_COMPAT)
        entrysize = 16;         // long mode (also compatibility mode): 16 byte gates
    unsigned int k = (IDT_Len + 1) / entrysize;

    StartListUpdate(DUMP_WND);

    // recover the IDT linear base address
    laddr = rV[IDTRnum];

    if (k > 256)    // if IDT_Len is unreasonably large, set a reasonable maximum
        k = 256;
    for(i = 0; i < k; i++)
    {
        sprintf(r.txt[0], "%02X", i);     // interrupt vector number
        if (bx_dbg_read_linear(CurrentCPU, laddr, entrysize, idtbuf) == FALSE)      // abort the current IDT dump on a memory error
        {
            strcpy (info,"illegal address");
            InsertListRow(r.cols, DUMP_COLS, DUMP_WND, i, LGRP_NONE);
            break;      // the list is finalized after the loop
        }
        laddr += entrysize;
        // enforce proper littleendianness on the idtbuf bytes
        ofs = idtbuf[0] | ((Bit32u) idtbuf[1] << 8);
        sel = idtbuf[2] | ((Bit16u) idtbuf[3] << 8);

        switch (CpuMode)
        {
            case BX_MODE_IA32_REAL:
                sprintf(info,"0x%04X:0x%04X", sel, ofs);
                break;

            case BX_MODE_IA32_V8086:
            case BX_MODE_IA32_PROTECTED:
                ofs |= ((Bit32u) idtbuf[6] << 16) | ((Bit32u) idtbuf[7] << 24);
                sprintf(info,"0x%04X:0x%08X", sel, ofs);
                // TODO: also print some flags from idtbuf[5], maybe, in another column
                break;

            case BX_MODE_LONG_COMPAT:
            case BX_MODE_LONG_64:
                Bit64u off64 = (Bit64u)(ofs | ((Bit32u) idtbuf[6] << 16) | ((Bit32u) idtbuf[7] << 24));
                off64 |= ((Bit64u) idtbuf[8] << 32) | ((Bit64u) idtbuf[9] << 40);
                off64 |= ((Bit64u) idtbuf[10] << 48) | ((Bit64u) idtbuf[11] << 56);
                sprintf(info,"0x%04X:0x" FMT_LLCAPX, sel, off64);
                // TODO: also print some flags from idtbuf[5], maybe, in another column
                break;
        }

        InsertListRow(r.cols, DUMP_COLS, DUMP_WND, i, LGRP_NONE);
    }
    RedrawColumns(DUMP_WND);
    EndListUpdate(DUMP_WND);
}


// insert one entry into the Paging data list (a linear and a physical addy)
static void AddPagingLine(int LC, const char *pa_lin, const char *pa_phy)
{
    DumpRow r;
    strcpy(r.txt[0], pa_lin);
    strcpy(r.txt[DUMP_INFO_COL], pa_phy);
    InsertListRow(r.cols, DUMP_COLS, DUMP_WND, LC, LGRP_NONE);
}

// lifted from bx_dbg_dump_table in dbg_main of the internal debugger
static void FillPAGE()
{
    Bit64u lin, start_lin; // show only low 32 bit
    bx_phy_address phy, start_phy; // start of a valid translation interval
    bx_address lpf_mask = 0;
    int LineCount = 0;
    doDumpRefresh = FALSE;

    StartListUpdate(DUMP_WND);

    lin = 0;   // always start at linear address 0
    phy = 0;
    start_lin = 1;      // force a mismatch on the first line
    start_phy = 2;

    char pa_lin[50];
    char pa_phy[50];

    while(LineCount < 10000) {
        bool valid = BX_CPU(CurrentCPU)->dbg_xlate_linear2phy(lin, &phy, &lpf_mask);
        if(valid) {
            if((lin - start_lin) != (phy - start_phy)) {
                if(start_lin != 1) {
                    sprintf (pa_lin,"0x" FMT_ADDRX64 " - 0x" FMT_ADDRX64, start_lin, lin - 1);
                    sprintf (pa_phy,"0x" FMT_ADDRX64 " - 0x" FMT_ADDRX64,
                        start_phy, start_phy + (lin-1-start_lin));
                    AddPagingLine (LineCount,pa_lin,pa_phy);
                    ++LineCount;
                }
                start_lin = lin;
                start_phy = phy;
            }
        } else {
            if(start_lin != 1) {
                sprintf (pa_lin,"0x" FMT_ADDRX64 " - 0x" FMT_ADDRX64, start_lin, lin - 1);
                sprintf (pa_phy,"0x" FMT_ADDRX64 " - 0x" FMT_ADDRX64,
                    start_phy, start_phy + (lin-1-start_lin));
                AddPagingLine (LineCount,pa_lin,pa_phy);
                ++LineCount;
            }
            start_lin = 1;
            start_phy = 2;
        }

        lin += lpf_mask;
        if (!BX_CPU(CurrentCPU)->long64_mode() && lin >= BX_CONST64(0xfffff000)) break;
        if (lin >= BX_CONST64(0x00007ffffffff000)) {
            if (lin < BX_CONST64(0xfffff00000000000))
                lin = BX_CONST64(0xfffff00000000000) - 1;
            if (lin >= BX_CONST64(0xfffffffffffff000))
                break;
        }
        lin++;
    }
    if(start_lin != 1) {
        sprintf (pa_lin,"0x" FMT_ADDRX64 " - 0x" FMT_ADDRX64, start_lin, (Bit64u)-1);
        sprintf (pa_phy,"0x" FMT_ADDRX64 " - 0x" FMT_ADDRX64, start_phy, start_phy + (lin-1-start_lin));
        AddPagingLine (LineCount,pa_lin,pa_phy);
    }
    RedrawColumns(DUMP_WND);
    EndListUpdate(DUMP_WND);
}

// build the stack display
static void FillStack()
{
    // sometimes need to specially invalidate the stack window
    static bool StkInvOnce = FALSE;
    Bit64u StackLA, EndLA;
    unsigned int len, i, wordsize, overlap;
    int j;
    bool LglAddy;
    bool UpdateDisp;
    char *cp, *cpp;
    DumpRow r;          // cols: 0 = address, 1 = hex value, 17 = decimal value
    doDumpRefresh = FALSE;
    bx_address StackPtr = (bx_address) rV[RSP_Rnum];
    if (! In64Mode && BX_CPU(CurrentCPU)->sregs[BX_SEG_REG_SS].cache.u.segment.d_b == 0)
        StackPtr &= 0xffff;     // 16-bit stack segment (SS.B=0) uses SP only
    StackLA = (Bit64u) BX_CPU(CurrentCPU)->get_laddr(BX_SEG_REG_SS, StackPtr);

    if (PStackLA == 1)               // illegal value requests a full refresh
        PStackLA = StackLA ^ 0x4000; // force a non-match below (kludge)

    wordsize = 4;       // assume Pmode
    if (! In32Mode)
        wordsize = 2;
    else if (In64Mode)
        wordsize = 8;
    len = STACK_ENTRIES * wordsize;

    // TODO: enforce that cp is wordsize aligned
    // also -- enforce that StackLA is wordsize aligned
    cp = CurStack;
    i = (unsigned) StackLA & 0xfff; // where is stack bottom, in its 4K memory page?
    if (i > 0x1000 - len)           // does len cross a 4K boundary?
    {
        unsigned int ReadSize = 0x1000 - i;
        // read up to the 4K boundary, then try to read the last chunk
        if (bx_dbg_read_linear(CurrentCPU, StackLA, ReadSize, (Bit8u *) cp) == FALSE)
        {
            // no data to show -- just one error message
            sprintf (r.txt[DUMP_INFO_COL],"illegal address");
            StartListUpdate(DUMP_WND);
            InsertListRow(r.cols, DUMP_COLS, DUMP_WND, 0, LGRP_NONE);
            EndListUpdate(DUMP_WND);
            return;
        }
        LglAddy = bx_dbg_read_linear(CurrentCPU, StackLA + ReadSize, len + i - 0x1000, (Bit8u *) cp + ReadSize);
        if (! LglAddy)
            len = ReadSize;
    }
    else
        bx_dbg_read_linear(CurrentCPU, StackLA, len, (Bit8u *) cp);

    UpdateDisp = CpuModeChange;     // calculate which stack entries have changed
    cp = CurStack;
    cpp = PrevStack;
    j = overlap = len / wordsize;
    while (--j >= 0)
        StackEntChg[j] = TRUE;      // assume that all lines have changed
    if (PStackLA > StackLA)         // calculate the overlap between the prev and current stacks
    {
        EndLA = PStackLA - StackLA;
        if (EndLA < len)
        {
            i = (unsigned int) (EndLA / wordsize);
            cp += i * wordsize;
        }
        else
            i = overlap;        // force the next loop to exit
    }
    else
    {
        EndLA = StackLA - PStackLA;
        if (EndLA < len)
        {
            i = 0;
            j = (int) (EndLA / wordsize);
            cpp += j * wordsize;
            overlap -= j;
        }
        else
            i = overlap;        // force the next loop to exit
    }
    while (i < overlap)
    {
        j = wordsize;           // if the two entries match, cancel the EntryChange flag for that entry
        while (j > 0 && *cp == *cpp)
        {
            --j;
            ++cp;
            ++cpp;
        }
        if (j == 0)
            StackEntChg[i] = FALSE;     // got a match on all bytes
        else
        {
            cp += j;    // bump the pointers to the next stack entry
            cpp += j;
        }
        ++i;
    }
    j = len / wordsize;
    while (--j >= 0)
        UpdateDisp |= StackEntChg[j];
    if (! UpdateDisp)            // Don't need to update the list? (no changes?)
    {
        if (! StkInvOnce)
            Invalidate(DUMP_WND);       // Invalidate ONCE to turn off all the red stuff
        StkInvOnce = TRUE;
        return;
    }
    StartListUpdate(DUMP_WND);
    StkInvOnce = FALSE;
    PStackLA = StackLA;
    cp = CurStack;
    cpp = PrevStack;
    j = len;
    while (--j >= 0)
        *(cpp++)= *(cp++);      // copy the stack to the Prev buffer
    j= STACK_ENTRIES * 8 - len;
    while (--j >= 0)
        *(cpp++)= 0;            // zero out the unused tail end of the prev buffer
    cp = CurStack;                  // the following display loop runs on the cp pointer

    EndLA = StackLA + len - 1;
    i = 0;
    while (StackLA < EndLA)
    {
#if BX_SUPPORT_X86_64
        if (In64Mode) {
            Bit64s tmp = *((Bit64s *) cp);
            FmtHex(r.txt[0], DUMP_COL_LEN, StackLA, 16);
            FmtHex(r.txt[1], DUMP_COL_LEN, tmp, 16);
            upr(r.txt[1]);      // 64-bit stack values are always shown in upper case
            sprintf (r.txt[DUMP_INFO_COL], FMT_LL "d",tmp);
        }
        else
#endif
        {
            int tmp;
            FmtHex(r.txt[0], DUMP_COL_LEN, StackLA, 8);
            upr(r.txt[0]);      // 32-bit stack addresses are always shown in upper case
            if (! In32Mode)
            {
                tmp = *((Bit16s *) cp);
                FmtHex(r.txt[1], DUMP_COL_LEN, (Bit16u) tmp, 4);  // 16-bit stack entry, no sign extension
            }
            else
            {
                tmp = *((Bit32s *) cp);
                FmtHex(r.txt[1], DUMP_COL_LEN, (Bit32u) tmp, 8);
            }
            sprintf (r.txt[DUMP_INFO_COL],"%d",tmp);
        }
        InsertListRow(r.cols, DUMP_COLS, DUMP_WND, i, LGRP_NONE);
        StackLA += wordsize;
        cp += wordsize;
        ++i;
    }

    RedrawColumns(DUMP_WND);
    EndListUpdate(DUMP_WND);
}

// utility function to print breakpoints in a unified way
// cols: 0 = [seg:]address, 1 = enabled (y/n), 17 = breakpoint id
static void prtbrk (Bit32u seg, Bit64u addy, unsigned int id, bool enabled, DumpRow *r)
{
    int i = 0;
    if (! enabled)
        strcpy(r->txt[1], "n");
    else
        strcpy(r->txt[1], "y");
    sprintf (r->txt[DUMP_INFO_COL],"%u",id);
    if (seg <= 0xffff){
        i= 5;
        sprintf (r->txt[0],"%04X:",seg);
    }

    sprintf (r->txt[0] + i,FMT_LLCAPX,addy);
}

// Displays all Breakpoints and Watchpoints
static void FillBrkp()
{
    int LineCount, totqty, i;
    unsigned int brktype;
    extern bx_guard_t bx_guard;

    doDumpRefresh = FALSE;

    StartListUpdate(DUMP_WND);
    i = 256;
    while (--i >= 0)
        BrkpIDMap[i] = 0;
    i = 16;
    while (--i >= 0)
    {
        WWP_Snapshot[i] = 0;
        RWP_Snapshot[i] = 0;
    }

    LineCount = 0;
    for (brktype = 0; brktype < 5; brktype++)
    {
        DumpRow blank;
        InsertListRow(blank.cols, DUMP_COLS, DUMP_WND, LineCount++, LGRP_NONE);  // make a blank row
        DumpRow title;
        strcpy(title.txt[0], BrkName[brktype]);
        InsertListRow(title.cols, DUMP_COLS, DUMP_WND, LineCount++, LGRP_NONE);  // brkpt "type" as only text on row
        if (brktype < 3)
        {
            // only breakpoints have IDs and can be "enabled"
            if (brktype == 0)
            {
#if (BX_DBG_MAX_LIN_BPOINTS > 0)
                totqty = bx_guard.iaddr.num_linear;
                for (i = 0; i < totqty; i++)
                {
                    DumpRow r;
                    BrkpIDMap[LineCount] = bx_guard.iaddr.lin[i].bpoint_id;
                    prtbrk (0xf0000, (Bit64u) bx_guard.iaddr.lin[i].addr,
                        (unsigned) BrkpIDMap[LineCount],
                        bx_guard.iaddr.lin[i].enabled, &r);
                    InsertListRow(r.cols, DUMP_COLS, DUMP_WND, LineCount++, LGRP_NONE);
                }
#endif
                EndLinEntry = LineCount;
            }
            else if (brktype == 1)
            {
#if (BX_DBG_MAX_PHY_BPOINTS > 0)
                totqty = bx_guard.iaddr.num_physical;
                for (i = 0; i < totqty; i++)
                {
                    DumpRow r;
                    BrkpIDMap[LineCount] = bx_guard.iaddr.phy[i].bpoint_id;
                    prtbrk (0xf0000, (Bit64u) bx_guard.iaddr.phy[i].addr,
                        (unsigned) BrkpIDMap[LineCount],
                        bx_guard.iaddr.phy[i].enabled, &r);
                    InsertListRow(r.cols, DUMP_COLS, DUMP_WND, LineCount++, LGRP_NONE);
                }
#endif
                EndPhyEntry = LineCount;
            }
            else
            {
#if (BX_DBG_MAX_VIR_BPOINTS > 0)
                totqty = bx_guard.iaddr.num_virtual;
                for (i = 0; i < totqty; i++)
                {
                    DumpRow r;
                    BrkpIDMap[LineCount] = bx_guard.iaddr.vir[i].bpoint_id;
                    prtbrk (bx_guard.iaddr.vir[i].cs,
                    (Bit64u) bx_guard.iaddr.vir[i].eip,
                        (unsigned) BrkpIDMap[LineCount],
                        bx_guard.iaddr.vir[i].enabled, &r);
                    InsertListRow(r.cols, DUMP_COLS, DUMP_WND, LineCount++, LGRP_NONE);
                }
#endif
            }
        }
        else if (brktype == 3)
        {
            WWP_BaseEntry = LineCount;
            totqty = num_write_watchpoints;
            WWPSnapCount = num_write_watchpoints;
            for (i = 0; i < totqty; i++)
            {
                DumpRow r;
                WWP_Snapshot[i] = write_watchpoint[i].addr;
                sprintf (r.txt[0], FMT_PHY_ADDRX, (bx_phy_address) write_watchpoint[i].addr);
                InsertListRow(r.cols, DUMP_COLS, DUMP_WND, LineCount++, LGRP_NONE);
            }
        }
        else
        {
            RWP_BaseEntry = LineCount;
            totqty = num_read_watchpoints;
            RWPSnapCount = num_read_watchpoints;
            for (i = 0; i < totqty; i++)
            {
                DumpRow r;
                RWP_Snapshot[i] = read_watchpoint[i].addr;
                sprintf (r.txt[0], FMT_PHY_ADDRX, (bx_phy_address) read_watchpoint[i].addr);
                InsertListRow(r.cols, DUMP_COLS, DUMP_WND, LineCount++, LGRP_NONE);
            }
        }
    }
    RedrawColumns(DUMP_WND);
    EndListUpdate(DUMP_WND);
}

// performs endian byteswapping the hard way, for a Data dump
static void FillDataX(char* t, unsigned char C, bool doHex)
{
    char tmpbuf[40];
    char *d = tmpbuf;
    if (! isLittleEndian || ! doHex)
        d = t + strlen(t);  // bigendian can always be appended directly
    *d = C;
    d[1] = 0;
    if (isprint(C) == 0)
        *d = '.';

    if (doHex)
    {
        sprintf(d, "%02X", C);
        if (isLittleEndian) // little endian => reverse hex digits
        {
            strcat(d,t);
            strcpy(t,d);    // so append the new bytes to the FRONT of t
        }
    }
}

static void RefreshDataDump(Bit8u *buffer)
{
    bool retval = TRUE;

    // re-load 4k DataDump array from bochs emulated linear or physical memory
    if (LinearDump) {
        // cannot read linear mem across a 4K boundary -- so break the read in two
        // -- calculate location of 4K boundary (h):
        unsigned len = (int) DumpStart & 0xfff;
        unsigned i = 4096 - len;
        Bit64u h = DumpStart + i;
        retval = bx_dbg_read_linear(CurrentCPU, DumpStart, i, buffer);
        if (retval != FALSE && len != 0)
            retval = bx_dbg_read_linear(CurrentCPU, h, len, buffer + i);
    } else {
        retval = bx_mem.dbg_fetch_mem(BX_CPU(CurrentCPU),
            (bx_phy_address)DumpStart, 4096, buffer);
    }
}

// display the MemDump window
void ShowData()
{
    unsigned int i;
    char *x;
    char tmphex[40];

    if (doDumpRefresh) {
        RefreshDataDump((Bit8u*)DataDump);
    }
    doDumpRefresh = FALSE;
    StartListUpdate(DUMP_WND);

    x = DataDump;       // data dumps are ALWAYS 4K
    for(i = 0; i < 4096; i += 16)
    {
        // cols: 0 = address, 1..16 = hex words (one every DumpAlign columns), 17 = ascii
        DumpRow r;
#if BX_SUPPORT_X86_64
        if (In64Mode)
            sprintf(r.txt[0],"0x" FMT_LLCAPX,DumpStart + i);
        else
#endif
            sprintf(r.txt[0],"0x%08X",(Bit32u) (DumpStart + i));

        *tmphex = 0;
        for(unsigned y = 0; y < 16; y++)
        {
            if ((DumpInAsciiMode & 1) != 0)
                // verify the char is printable, then append it to the "ascii" column
                FillDataX(r.txt[DUMP_INFO_COL],x[y],FALSE);

            if ((DumpInAsciiMode & 2) != 0)
            {
                // convert char to hex, build "endian" hex value, 2 digits at a time
                FillDataX(tmphex,x[y],TRUE);
                if (((y + 1) & (DumpAlign - 1)) == 0)
                {
                    strcpy (r.txt[y+2-DumpAlign], tmphex);
                    *tmphex = 0;        // FillDataX APPENDS, so you need to clear the buffer
                }
            }
        }
        InsertListRow(r.cols, DUMP_COLS, DUMP_WND, i>>4, LGRP_NONE);
        x+= 16;         // bump to the next row of data
    }

    RedrawColumns(DUMP_WND);
    EndListUpdate(DUMP_WND);
}


// build Register "display" names from the lower case names in RegTable
static void MakeRDnames()
{
    for (unsigned n = 0; n < REG_TABLE_SIZE; n++)
    {
        char *d = RDispName[RegTable[n].rnum];
        strcpy(d, RegTable[n].name);
        if (UprCase)
            upr(d);
    }
}

// background color of each register type (index into the ColorList of the osdep files)
static Bit8u GetRegColor(int rnum)
{
    if (rnum >= DR0_Rnum)  return RCLR_DBG;
    if (rnum >= XMM0_Rnum) return RCLR_SSE;
    if (rnum >= ST0_Rnum)  return RCLR_FPU;
    if (rnum >= CR0_Rnum)  return RCLR_CTRL;
    if (rnum >= GDTRnum)   return RCLR_SYS;
    if (rnum >= CS_Rnum)   return RCLR_SEG;
    if (rnum >= EAX_Rnum)  return RCLR_GPR32;
    return RCLR_GPR64;      // 64bit GP Registers and EFLAGS
}

// generic initialization routine -- called once, only at startup
static void DoAllInit()
{
    char *p;
    int i;

    doSimuInit = TRUE;
    CpuModeChange = TRUE;
    CurrentCPU = 0;     // need to init CPU info once only
    if (! SingleCPU)
        TotCPUs = BX_SMP_PROCESSORS;
    else
        TotCPUs = 1;

    // divide up the pre-allocated char buffer into smaller pieces
    p = bigbuf + outbufSIZE;    // point at the end of preallocated mem
    p -= 256;           // unused (was the storage for the register names)
    p -= 4096;
    DataDump = p;       // storage for 4K memory dumps
    p -= OutWinCnt;     // 10K for Output Window buffer
    OutWindow = p;
    i = 64;
    while (--i >= 0)
    {
        p -= 80;            // command history buffers are 80b each
        CmdHistory[i] = p;  // set up 64 of them (5120b)
        *p = 0;             // and clear each one
    }
    p -= STACK_ENTRIES * 8;     // usually a 400 byte buffer for the stack values
    PrevStack = p;
    p -= STACK_ENTRIES * 8;     // and another one
    CurStack = p;
    p -= 512;
    p -= 512;           // 2 "hex" bytes per byte value
    tmpcb = p;

    // the EXTRA_REGS parts keep color 0 (RCLR_GPR64)
    for (i = 0; i < STD_REG_NUM; i++)
        RegColor[i] = GetRegColor(i);

    MakeRDnames();      // create Rnames from lower-case register names
    InitRegObjects();   // get/store all the bx_param_num_c objects for the registers
}

// refill whichever "data window" is active -- or param_tree
void RefreshDataWin()
{
    switch (DViewMode)
    {
        case VIEW_MEMDUMP:
            if (DumpInitted)
                ShowData();
            else
                EndListUpdate(2);   // list is empty, so end (show) it!
            break;
        case VIEW_GDT:
            FillGDT();
            break;
        case VIEW_IDT:
            FillIDT();
            break;
        case VIEW_PAGING:
            FillPAGE();
            break;
        case VIEW_STACK:
            PStackLA = 1;   // flag to force a full stack refresh
            FillStack();
            break;
        case VIEW_BREAK:
            FillBrkp();
            break;
        case VIEW_PTREE:
            FillPTree();
    }
}

// change the CPU shown in the gui debugger, the commands typed by the user
// are executed on the same CPU
void SelectCPU(unsigned newCPU)
{
    if (newCPU >= (unsigned) BX_SMP_PROCESSORS || CurrentCPU == newCPU)
        return;
    if (BX_SMP_PROCESSORS > 1)      // CPU buttons exist only for SMP simulation
        SetCPUButton(CurrentCPU, newCPU);
    CurrentCPU = newCPU;
    dbg_cpu = newCPU;
    BottomAsmLA = ~0;       // force an ASM autoload, to repaint
}

// performs tasks whenever the simulation "breaks"
void OnBreak()
{
    int i = EFER_Rnum + 1;
    // show the CPU selected by the debugger (the CPU which hit a breakpoint
    // or requested the debugger activation, or changed by "cpu" command)
    SelectCPU(dbg_cpu);
    // check if Ptime has changed
    TakeInputFocus();

    // display the new ptime on the status bar
    char time_buf[20];
    sprintf (time_buf,"t= " FMT_LL "d", bx_pc_system.time_ticks());
    SetStatusText (2, time_buf);

    // remember register values from before the last run
    while (--i >= 0)
        PV[i] = rV[i];

    // then detect current CPU mode the *right* way -- look for changes
    // TODO: create param Objects for CS.d_b and cpu_mode for each CPU
    unsigned PrevCpuMode = CpuMode;
    CpuMode = BX_CPU(CurrentCPU)->get_cpu_mode();
#if BX_SUPPORT_X86_64
    if (CpuMode == BX_MODE_LONG_64)
    {
        if (! In64Mode)          // Entering LongMode?
        {
            CpuModeChange = TRUE;
            In64Mode = TRUE;
            In32Mode = TRUE;            // In32Mode must be TRUE in LongMode
            ResizeColmns = TRUE;        // if so, some formatting has changed
        }
    }
    else
#endif
    {
        bool d_b = BX_CPU(CurrentCPU)->sregs[BX_SEG_REG_CS].cache.u.segment.d_b;
        if (In32Mode != d_b || In64Mode != FALSE || PrevCpuMode != CpuMode)
        {
            CpuModeChange = TRUE;
            In64Mode = FALSE;
            In32Mode = d_b;
        }
    }
    if (CpuModeChange)
    {
        GrayMenuItem ((int) In64Mode, CMD_EREG);
        BottomAsmLA = ~0;       // force an ASM autoload
        StatusChange = TRUE;
    }
    doUpdate();     // do a full "autoupdate"
    if (doDumpRefresh)
        RefreshDataWin();
}

static int HexFromAsk(const char* ask, char* b, int maxlen)    // this routine converts a user-typed hex string into binary bytes
{                   // it ignores any bigendian issues -- binary is converted front to end as chars
    int y = 0;
    int i = 0;
    while (y < maxlen)  // never write more than maxlen bytes into b
    {
        unsigned int C = 0;
        if (strlen(ask + i) < 2)
            break;
        if (!sscanf(ask + i,"%02X",&C))
            break;
        b[y++] = C;
        i += 2;
    }
    return y;
}

static bool FindHex(const unsigned char* b1,int bs,int avail,const unsigned char* b2,int by)
{
    // search bs bytes of b1, a match must fit into the avail bytes of b1 (end of DataDump)
    for(int i = 0; i < bs && i + by <= avail; i++)       // TODO: this loop could be a little more efficient.
    {                       // -- it just scans an input byte string against DataDump memory
        bool Match = TRUE;
        for(int y = 0; y < by; y++)
        {
            if (b1[i + y] != b2[y])
            {
                Match = FALSE;
                break;
            }
        }
        if (Match)
            return TRUE;
    }
    return FALSE;
}

static bool AskText(const char *title, const char *prompt, char *DefaultText)
{
    ask_str.title= title;
    ask_str.prompt= prompt;
    ask_str.reply= DefaultText;
    return ShowAskDialog();
}

// load new memory for a MemDump
// newDS = illegal (1) is a flag to ask the user for a DumpStart address
static bool InitDataDump(bool isLinear, Bit64u newDS)
{
    bool retval = TRUE;
    bool MsgOnErr = FALSE;
    if (! AtBreak)
        return FALSE;
    if (((int) newDS & 0xf) != 0)       // legal addys must be on 16byte boundary
    {
#if BX_SUPPORT_X86_64
        if (In64Mode)
            sprintf(tmpcb,"0x" FMT_LL "X",DumpStart);
        else
#endif
            sprintf(tmpcb,"0x%X",(Bit32u) DumpStart);

        if (AskText("4K Memory Dump","4K Memory Dump -- Enter Address (use 0x for hex):",tmpcb) == FALSE)
            return FALSE;

        newDS = ParseNumber(tmpcb,FALSE);     // input either hex or decimal
        newDS &= ~15;                   // force Mem Dump to be 16b aligned
        MsgOnErr = TRUE;
    }

    // load 4k DataDump array from bochs emulated linear or physical memory
    if (isLinear)
    {
        // cannot read linear mem across a 4K boundary -- so break the read in two
        // -- calculate location of 4K boundary (h):
        unsigned len = (int) newDS & 0xfff;
        unsigned i = 4096 - len;
        Bit64u h = newDS + i;
        retval = bx_dbg_read_linear(CurrentCPU, newDS,i,(Bit8u *)DataDump);
        if (retval != FALSE && len != 0)
            retval = bx_dbg_read_linear(CurrentCPU, h,len,(Bit8u *)DataDump + i);
    }
    else {
        retval = (bool) bx_mem.dbg_fetch_mem(BX_CPU(CurrentCPU),
            (bx_phy_address)newDS, 4096, (Bit8u *)DataDump);
    }
    if (retval == FALSE)
    {
        // assume that the DataDump array is still valid -- fetch_mem should error without damage
        if (MsgOnErr)
            DispMessage ("Address range was not legal memory","Memory Error");
        return retval;
    }

    SA_valid = FALSE;       // any previous MemDump click is now irrelevant
    ResizeColmns = TRUE;    // autosize column 0 once
    DumpInitted = TRUE;     // OK to refresh the Dump window in the future (it has data)
    DumpStart = newDS;
    LinearDump = isLinear;  // finalize dump mode, since it worked
    ShowMemData(TRUE);      // Display DataDump using these new parameters/data
    return TRUE;
}

// User is changing which registers are displaying in the Register list
static void ToggleSeeReg(int cmd)
{
    int i = cmd - CMD_EREG;
    if (i < 0 || i > 7)
        return;
    if (i == 4 || i == 5)
        ResizeColmns = TRUE;    // may need to resize the register value column

    SeeReg[i] ^= TRUE;
    SetMenuCheckmark ((int) SeeReg[i], i + CHK_CMD_EREG);
    if (AtBreak)
        LoadRegList();  // do a register window update
}

// redisplay the MemDump window, if it is shown, after a display option change
static void RedrawMemDump()
{
    if (DViewMode == VIEW_MEMDUMP && DumpInitted)
    {
        if (! AtBreak)
            doDumpRefresh = TRUE;
        else
            ShowData();
    }
}

// toggle ASCII (bit 0) or hex (bit 1) MemDump display, at least one must stay on
static void ToggleDumpMode(int bit, int chk_cmd, int other_cmd)
{
    int i = DumpInAsciiMode ^ bit;
    if (i != 0)
    {
        DumpInAsciiMode = i;
        i &= bit;
        SetMenuCheckmark (i, chk_cmd);
        GrayMenuItem (i, other_cmd);
        PrevDAD = 0;        // force columns to resize
        RedrawMemDump();
    }
}

static void doNewWSize(int i)
{
    // DumpAlign is the "wordsize" in bytes -- need to "calculate" the power of 2
    int j = 0;
    if (DumpAlign == 2) j = 1;
    else if (DumpAlign == 4) j = 2;
    else if (DumpAlign == 8) j = 3;
    else if (DumpAlign == 16) j = 4;
    if (j != i)
    {
        ToggleWSchecks(i, j);
        DumpWSIndex = i;
        DumpAlign = 1<<i;
        RedrawMemDump();
    }
}

static void ToggleGDT()
{
    if (! AtBreak)
        return;
    GrayMenuItem (0, CMD_WPTWR);
    GrayMenuItem (0, CMD_WPTRD);
    if (DViewMode == VIEW_GDT || /*(GDT_Len & 7) != 7 ||*/ (unsigned) GDT_Len >= 0x10000)
    {
        if (DViewMode != VIEW_GDT)
            DispMessage("GDT limit is illegal","Simulation error");
        ShowMemData(FALSE);
    }
    else
    {
        HideTree();
        DViewMode = VIEW_GDT;       // displaying a GDT
        FillGDT();
    }
}

static void ToggleIDT()
{
    if (! AtBreak)
        return;
    GrayMenuItem (0, CMD_WPTWR);
    GrayMenuItem (0, CMD_WPTRD);
    if (DViewMode == VIEW_IDT || /*(IDT_Len & 3) != 3 ||*/ (unsigned) IDT_Len >= 0x10000)
    {
        if (DViewMode != VIEW_IDT)
            DispMessage("IDT limit is illegal","Simulation error");
        ShowMemData(FALSE);
    }
    else
    {
        HideTree();
        DViewMode = VIEW_IDT;       // displaying an IDT
        FillIDT();
    }
}

static void TogglePAGE()
{
    if (! AtBreak)
        return;
    GrayMenuItem (0, CMD_WPTWR);
    GrayMenuItem (0, CMD_WPTRD);
    if (DViewMode == VIEW_PAGING || ! InPaging)
        ShowMemData(FALSE);
    else
    {
        HideTree();
        DViewMode = VIEW_PAGING;    // currently displaying Paging info
        FillPAGE();
    }
}

static void ToggleStack()
{
    if (! AtBreak)
        return;
    GrayMenuItem (0, CMD_WPTWR);
    GrayMenuItem (0, CMD_WPTRD);
    if (DViewMode == VIEW_STACK)
        ShowMemData(FALSE);
    else
    {
        HideTree();
        DViewMode = VIEW_STACK;     // currently displaying stack
        PStackLA = 1;       // flag to force a full refresh
        FillStack();
    }
}

static void ToggleBrkpt()
{
    if (! AtBreak)
        return;
    GrayMenuItem (0, CMD_WPTWR);
    GrayMenuItem (0, CMD_WPTRD);
    if (DViewMode == VIEW_BREAK)
        ShowMemData(FALSE);
    else
    {
        HideTree();             // HideTree needs to know the "prev" DViewMode
        DViewMode = VIEW_BREAK; // currently displaying breakpoint info
        FillBrkp();
    }
}

static void TogglePTree()
{
    if (! AtBreak)
        return;
    GrayMenuItem (0, CMD_WPTWR);
    GrayMenuItem (0, CMD_WPTRD);
    if (DViewMode == VIEW_PTREE)
        ShowMemData(FALSE);
    else
    {
        // FillPTree needs to know the "prev" DViewMode, to handle "refresh" events properly
        FillPTree();        // get all info from param_tree into tree-view window
        DViewMode = VIEW_PTREE;     // currently displaying param_tree
    }
}

static void doFind()
{
    unsigned int i, L;
    bool Select;
    char srchstr[ASKTEXT_MAX + 2];  // the reply text, possibly wrapped in "*...*"
    if (! AtBreak)
        return;
    *tmpcb = 0;
        // read ASM text or MemDump data, find matches, select all matching lines
    if (! DumpHasFocus)
    {
        if (AskText("Find text in mnemonic lines","ASM Search text:",tmpcb) == FALSE)
            return;
        if (strchr(tmpcb,'*') == 0 && strchr(tmpcb,'?') == 0)
            sprintf(srchstr,"*%s*",tmpcb);
        else
            strcpy(srchstr,tmpcb);

        if (UprCase)   // convert search string to uppercase if ASM is that way
            upr(srchstr);
        for(i = 0; i < (unsigned) AsmLineCount; i++)
        {
            GetLIText(ASM_WND, i, 2, tmpcb);       // retrieve the ASM column 2 text for row i
            Select = FALSE;
            if (IsMatching(tmpcb, srchstr, TRUE))
                Select = TRUE;
            SetLIState(ASM_WND, i, Select);
        }
    }
    else
    {
        if (AskText("Memory Dump Search",
            "Sequential hex bytes (e.g 00FEFA - no spaces), or ascii string (max. 16b):",tmpcb) == FALSE)
            return;

        int by = HexFromAsk(tmpcb,srchstr,sizeof(srchstr));     // by = len of binary search string

        // Find in all rows of 16 bytes -- must do rows, so they can be selected
        for(i = 0, L = 0; i < 4096; i += 16, L++)
        {
            Select = FALSE;
            if (by != 0 && FindHex((unsigned char *)DataDump + i,16,4096 - i,(unsigned char *)srchstr,by))
                Select = TRUE;
            SetLIState(DUMP_WND, L, Select);
        }

        // Try ascii for additional matches and selected lines
        Select = TRUE;          // this loop, only add selected lines to the display
        by = (int)strlen(tmpcb);
        for(i = 0, L = 0; i < 4096; i += 16, L++)
        {
            if (by != 0 && FindHex((unsigned char *)DataDump + i,16,4096 - i,(unsigned char *)tmpcb,by))
                SetLIState(DUMP_WND, L, Select);
        }
    }
}

// pass a command to the internal debugger (sim thread picks it up from debug_cmd)
// The VGAW *MUST* be refreshed periodically -- it's best to use the timer.
// Which means that the sim cannot be directly run from this msglp thread.
static void SendDebugCmd(const char *cmd)
{
    AtBreak = FALSE;
    StatusChange = TRUE;
    strcpy(debug_cmd, cmd);
    debug_cmd_ready = TRUE;     // must be last, the sim thread is polling it
}

static void doStepOver()
{
    // can't run sim until everything is ready
    if (! AtBreak || debug_cmd_ready)
        return;

    SendDebugCmd("p");  // send a "step over" command to the internal debugger
}

static void doStepN()
{
    // can't run sim until everything is ready
    if (! AtBreak || debug_cmd_ready)
        return;

    sprintf (tmpcb,"%d",PrevStepNSize);
    if (AskText("Singlestep N times","Number of steps (use 0x for hex):",tmpcb) == FALSE)
        return;
    Bit32u i = (Bit32u) ParseNumber(tmpcb,FALSE);        // input either hex or decimal
    if (i == 0)
        return;
    PrevStepNSize = i;
    sprintf(tmpcb, "s %d %d", CurrentCPU, i);
    SendDebugCmd(tmpcb);
}

// User wants a custom disassembly
static void doDisAsm()
{
    int NumLines = DefaultAsmLines;
    if (! AtBreak)
        return;
    sprintf (tmpcb,"0x" FMT_LL "X",CurrentAsmLA);
    if (AskText("Disassemble",
        "Disassemble -- Enter Linear Start Address (use 0x for hex):",tmpcb) == FALSE)
        return;
    Bit64u h = ParseNumber(tmpcb,FALSE);             // input either hex or decimal
    sprintf (tmpcb,"%d",NumLines);
    if (AskText("Disassemble","Number of lines: (Max. 2048)",tmpcb) == FALSE)
        return;
    sscanf (tmpcb,"%d",&NumLines);
    if (NumLines <= 0 || NumLines > 2048)
        return;
    if (NumLines > 1000 && FWflag == FALSE)
        ShowFW();
    FillAsm(h, NumLines);
    // Set the scroll limits for the new ASM window
    SetAsmScrollLimits();
}

// Toggle all "selected" items as linear breakpoint on the ASM window
void SetBreak(int OneEntry)
{
    if (! AtBreak)
        return;

    int L;
    if (OneEntry >= 0)
        L = OneEntry;
    else
        // -1 is a flag to start the search at the beginning
        L = GetNextSelectedLI(ASM_WND, -1);
    while (L >= 0)
    {
        int iExist = -1;
        int i=0;
        while (i < BreakCount && iExist < 0)
        {
            if (BrkLAddr[i] == AsmLA[L])
                iExist = i;
            ++i;
        }
        if (iExist >= 0)
        {
            // existing, remove
            bx_dbg_del_lbreak(BrkIdx[iExist]);
            i = iExist;     // also compress it out of the local list
            while (++i < BreakCount)
            {
                BrkLAddr[i-1] = BrkLAddr[i];
                BrkIdx[i-1] = BrkIdx[i];
            }
            --BreakCount;
        }
        else
        {
            bx_address nbrk = (bx_address) AsmLA[L];
            // Set a "regular" bochs linear breakpoint to that address
            int BpId = bx_dbg_lbreakpoint_command(bkRegular, nbrk, NULL);
            if (BpId >= 0)
            {
                // insertion sort the new Brkpt into the local list
                i = BreakCount - 1;
                while (i >= 0 && BrkLAddr[i] > nbrk)
                {
                    BrkLAddr[i+1] = BrkLAddr[i];
                    BrkIdx[i+1] = BrkIdx[i];
                    --i;
                }
                BrkLAddr[i+1] = nbrk;
                BrkIdx[i+1] = BpId;
                ++BreakCount;
            }
        }
        if (OneEntry >= 0)      // do not loop, if only doing one entry
            L = -1;
        else
            // start the next ASM search at current item L
            L = GetNextSelectedLI(ASM_WND, L);
    }
    Invalidate(ASM_WND);    // redraw the ASM window -- colors may have changed
}

void DelWatchpoint(bx_watchpoint *wp_array, unsigned *TotEntries, int i)
{
    while (++i < (int) *TotEntries)
        wp_array[i-1] = wp_array[i];
    -- *TotEntries;
}

void SetWatchpoint(unsigned *num_watchpoints, bx_watchpoint *watchpoint)
{
    int iExist1 = -1;
    int i = (int) *num_watchpoints;
    if (! AtBreak || ! SA_valid)
        return;
    // the list is unsorted -- test all of them
    while (--i >= 0)
    {
        if (watchpoint[i].addr == SelectedDataAddress)
        {
            iExist1 = i;
            i = 0;
        }
    }
    if (iExist1 >= 0)
    {
        // existing watchpoint, remove by copying the list down
        DelWatchpoint(watchpoint, num_watchpoints, iExist1);
    }
    else
    {
        // Set a watchpoint to last clicked address -- the list is not sorted
        if (*num_watchpoints >= BX_DBG_MAX_WATCHPONTS) {
            DispMessage("Too many of that type of watchpoint. Max: 16", "Table Overflow");
        }
        else {
            watchpoint[*num_watchpoints].len  = 1;
            watchpoint[*num_watchpoints].addr = (bx_phy_address) SelectedDataAddress;
            ++(*num_watchpoints);
        }
    }
    Invalidate(DUMP_WND);   // redraw the MemDump window -- colors may have changed
}

void ChangeReg()
{
    // Change a register -- search for the first selected register
    int L = GetNextSelectedLI(REG_WND, -1);
    if (! AtBreak || L == -1 || L >= STD_REG_NUM+EXTRA_REGS)
        return;

    int i = RitemToRnum[L];
    if (i > EFER_Rnum)      // TODO: extend this to more reg -- need display names for all
        return;
    sprintf (tmpcb,"0x" FMT_LL "X", rV[i]);
    if (AskText("Change Register Value",RDispName[i],tmpcb))
    {
        Bit64u val = ParseNumber(tmpcb,TRUE);     // input either hex or decimal
#if BX_SUPPORT_X86_64
        if (i >= EAX_Rnum && i <= EIP_Rnum) // must use RAX-RIP when setting 32b registers
            i -= EAX_Rnum - RAX_Rnum;
#endif
        RegObject[CurrentCPU][i]->set(val);
        LoadRegList();      // update the register window
    }
}

// user wants to edit some memory
void SetMemLine(int L)
{
    // get base address of "line" of data -- each line (L) is 16 bytes
    char addrstr[64];
    Bit64u h = DumpStart + (L<<4);
    if (! AtBreak || L >= 256)
        return;

    if (! LinearDump)
        sprintf(addrstr,"Physical Address: 0x" FMT_LL "X",h);
    else
        sprintf(addrstr,"Linear Address: 0x" FMT_LL "X",h);

    unsigned char *u = (unsigned char *)(DataDump + (L<<4));    // important that it be unsigned!
    sprintf(tmpcb,"%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
        *u,u[1],u[2],u[3],u[4],u[5],u[6],u[7],u[8],u[9],u[10],u[11],u[12],u[13],u[14],u[15]);

    if (AskText("Change Memory Values",addrstr,tmpcb))
    {
        unsigned int byteval;
        Bit8u newval;
        bool written;
        int err=0;
        char *x = tmpcb;
        upr(x);         // force input string to uppercase

        while (*x != 0 && err == 0)
        {
            char *s = x;
            // verify that the next 2 chars are hex digits
            if ((*x < '0' || *x > '9') && (*x < 'A' || *x > 'F'))
                err = 1;
            else
            {
                ++x;
                if ((*x < '0' || *x > '9') && (*x < 'A' || *x > 'F'))
                    err = 1;
                else
                {
                    ++x;
                    if (*x != ' ' && *x != 0)   // followed by a space or 0
                        err = 1;
                }
            }
            if (err == 0)
            {
                // convert the hex to a byte, and try to store the byte in bochs memory
                sscanf (s,"%2X", &byteval);
                newval = (Bit8u) byteval;
                if (LinearDump)    // h is a LINEAR address, translate it through the CPU paging
                    written = bx_dbg_write_linear(CurrentCPU, (bx_address) h, 1, &newval);
                else
                    written = bx_mem.dbg_set_mem(BX_CPU(CurrentCPU), (bx_phy_address) h, 1, &newval);
                if (! written)
                    err = 2;
                else
                    *u++ = newval;      // update DataDump array so it will refresh on the screen
                ++h;                    // bump to the next mem address
                while (*x == ' ')       // scan past whitespace
                    ++x;
            }
        }

        if (err != 0)
        {
            if (err == 1)
                DispMessage ("Improper char hex format","Input Format Error");
            else
                DispMessage ("Illegal memory address error?","Memory Error");
        }
        ShowData();     // refresh the data dump, even if there were errors
    }
}

void ActivateMenuItem (int cmd);

// Alt, Shift, Control keys are "down" if negative
// Normal return value is 0, return != 0 has OS-specific meaning.
int HotKey (int ww, int Alt, int Shift, int Control)
{
    int cmd = 0;    // menu command to run, 0 = none

    if (Alt < 0){
        if (ww == '1')
            cmd = CMD_WS_1;
        else if (ww == '2')
            cmd = CMD_WS_2;
        else if (ww == '4')
            cmd = CMD_WS_4;
        else if (ww == '8')
            cmd = CMD_WS_8;
        else if (ww == '6')
            cmd = CMD_WS16;
        else if (ww == VK_F2)
            cmd = CMD_PAGEV;
#if BX_SUPPORT_FPU
        else if (ww == VK_F3)       // MMX/FPU toggle
            cmd = CMD_FPUR;
#endif
        else if (ww == VK_F6)       // AltF6 = Read Watchpt
        {
            if (DumpHasFocus)
                cmd = CMD_WPTRD;
        }
        else if (ww == VK_F7)       // Alt+F7 memdump hex toggle
            cmd = CMD_MHEX;
        if (cmd != 0)
            ActivateMenuItem(cmd);
        return 0;
    }
    switch (ww)
    {
        case VK_ESCAPE:
            cmd = CMD_CMEM;
            break;

        case VK_UP:
            // History from nextmost previous command
            SelectHistory(-1);
            break;

        case VK_DOWN:
            // Next History command
            SelectHistory(+1);
            break;

        case VK_PRIOR:
            // Page up on the MemDump window by 2K
            if (DumpInitted)
                InitDataDump(LinearDump, DumpStart - 2048);
            break;

        case VK_NEXT:
            // Page down on the MemDump window by 2K
            if (DumpInitted)
                InitDataDump(LinearDump, DumpStart + 2048);
            break;

        case VK_F2:
            if (Control < 0)
                cmd = CMD_GDTV;
            else if (Shift < 0)
                cmd = CMD_IDTV;
            else
                cmd = CMD_STACK;
            break;

        case VK_F3:     // ^F3 = param tree, F3 = toggle syntax
            if (Control < 0)
                cmd = CMD_PTREE;
            else
                cmd = CMD_ATTI;
            break;

        case VK_F4:
            if (Shift < 0)              // Debug register toggle
                cmd = CMD_DREG;
            else if (Control >= 0)      // Refresh
                cmd = CMD_RFRSH;
            else if (CpuSupportSSE)     // SSE toggle
                cmd = CMD_XMMR;
            break;

        case VK_F5:
            if (Shift < 0)          // ShiftF5 = Modechange brk toggle
                cmd = CMD_MODEB;
            else
                cmd = CMD_CONT;
            break;

        case VK_F7:
            if (Control < 0)        // ^F7 = PhysDump
                cmd = CMD_PHYDMP;
            else if (Shift < 0)     // ShiftF7 = ascii toggle
                cmd = CMD_MASCII;
            else                    // F7 = LinDump
                cmd = CMD_LINDMP;
            break;

        case VK_F6:
            if (Control < 0)        // ^F6 = Breakpoint window
                cmd = CMD_VBRK;
            else if (! DumpHasFocus)    // F6, ShiftF6 = Brkpt
                cmd = CMD_BRKPT;    // set or delete breakpoint(s) at the selected address(es)
            else                        // F6, ShiftF6 = Write Watchpt
                cmd = CMD_WPTWR;
            break;

        case VK_F8:
            cmd = CMD_STEPOVER;
            break;

        case VK_F11:
            cmd = CMD_STEP1;        // singlestep
            break;

        case VK_F9:
            cmd = CMD_STEPN;        // ask user for a step #
            break;

        case 'C':           // ^c = break
            if (Control < 0)
                cmd = CMD_BREAK;
            break;

        case 'D':
            if (Control < 0)
                cmd = CMD_DISASM;
            break;

        case 'F':
            if (Control < 0)
                cmd = CMD_FIND;
            break;

        case VK_RIGHT:  // Win32: send a few virtual movement keys back into the Input window
        case VK_LEFT:
        case VK_END:
        case VK_HOME:
        case VK_DELETE:
            return -1;

        case VK_RETURN:
            // can't run a command until everything is ready
            if (AtBreak && ! debug_cmd_ready)
            {
                *tmpcb = 0;
                GetInputEntry(tmpcb);
                StatusChange = TRUE;
                if (*tmpcb == 0)            // Hitting <CR> on a blank line means SINGLESTEP
                    cmd = CMD_STEP1;
                else
                {
                    // deal with the command history:
                    if (strlen(tmpcb) > 79)
                        DispMessage ("Running command, but history has an 80 char Max.",
                            "Command history overflow");
                    else
                    {
                        strcpy (CmdHistory[CmdHInsert], tmpcb);
                        CmdHInsert = (CmdHInsert + 1) & 63;     // circular buffer, 0 to 63
                    }
                    SendDebugCmd(tmpcb);        // send the command into the bochs internal debugger
                    ClearInputWindow();         // prepare for the next command
                    CommandHistoryIdx = 0;      // and reset the history queue to the new end
                }
            }
    }       // end the switch
    if (cmd != 0)
        ActivateMenuItem(cmd);
//  if (Control >= 0 && ww >= ' ' && ww < 0x7f) -- might be interesting to catch printable chars
//      return 1;
    return 0;
}

void ActivateMenuItem (int cmd)
{
    int i;

    switch(cmd)
    {
        case CMD_CONT: // run/go/continue
            if (AtBreak && ! debug_cmd_ready)
                SendDebugCmd("c");  // send a "continue" command to the internal debugger
            break;

        case CMD_STEP1: // step 1
            if (AtBreak && ! debug_cmd_ready)
            {
                sprintf(tmpcb, "s %d 1", CurrentCPU);       // singlestep
                SendDebugCmd(tmpcb);
            }
            break;

        case CMD_STEPN: // step N
            doStepN();
            break;

        case CMD_STEPOVER:
            doStepOver();
            break;

        case CMD_BREAK: // break/stop the sim
            // SIM->debug_break() only "break"s the internal debugger
            SIM->debug_break();
            break;

        case CMD_BRKPT: // set or delete breakpoint(s) at the selected address(es)
            SetBreak(-1);
            break;

        case CMD_WPTWR: // set or delete a data write watchpoint
            SetWatchpoint(&num_write_watchpoints,write_watchpoint);
            break;

        case CMD_WPTRD: // set or delete a data read watchpoint
            SetWatchpoint(&num_read_watchpoints,read_watchpoint);
            break;

        case CMD_FIND: // find -- Control-F
            doFind();
            break;

        case CMD_RFRSH: // force an update/refresh
            BottomAsmLA = ~0;       // force an ASM autoload
            ResizeColmns = TRUE;    // force everything to repaint
            doDumpRefresh = TRUE;   // force a data window reload on a break
            if (AtBreak)   // can't refresh the windows until a break!
            {
                doUpdate();         // refresh the ASM and Register windows
                RefreshDataWin();   // and whichever data window is up
            }
            break;

        case CMD_DETACH: // detach debugger (don't hide)
            DetachDebugger();
            break;

        case CMD_PHYDMP:        // "physical mem" data dump
            InitDataDump(0,(Bit64u) 1);
            break;

        case CMD_LINDMP:        // "linear memory" data dump
            InitDataDump(1,(Bit64u) 1);
            break;

        case CMD_STACK:     // toggle display of Stack
            ToggleStack();
            break;

        case CMD_GDTV:      // toggle display of GDT
            ToggleGDT();
            break;

        case CMD_IDTV:      // toggle display of IDT
            ToggleIDT();
            break;

        case CMD_PAGEV:     // display paging info
            TogglePAGE();
            break;

        case CMD_VBRK:      // display breakpoint/watchpoint info
            ToggleBrkpt();
            break;

        case CMD_CMEM:  // view current MemDump -- acts like "cancel"
            CommandHistoryIdx = 0;
            ClearInputWindow();
            ShowMemData(FALSE);         // force a "normal" MemDump window
            break;

        case CMD_PTREE:
            TogglePTree();
            break;

        case CMD_DISASM:    // disassemble starting at a particular address
            doDisAsm();
            break;

        case CMD_MODEB: // toggle the simulation's Mode-Change-Break flag
        {
            // toggle mode_break on cpu0, use that value to reset all CPUs
            bool nmb = BX_CPU(0)->mode_break ^ TRUE;
            i = TotCPUs;
            while (--i >= 0)
                BX_CPU(i)->mode_break = nmb;
            SetMenuCheckmark ((int) nmb, CHK_CMD_MODEB);
            break;
        }

        case CMD_ONECPU:    // toggle whether to show SMP CPUs
            if (! AtBreak)
                break;
            SingleCPU ^= TRUE;
            TotCPUs = 1;
            if (! SingleCPU)
                TotCPUs = BX_SMP_PROCESSORS;
            SetMenuCheckmark ((int) SingleCPU, CHK_CMD_ONECPU);
            VSizeChange();
            break;

        case CMD_DADEF: // set default # of disassembly lines in a list
            if (! AtBreak)
                break;
            sprintf (tmpcb,"%d",DefaultAsmLines);
            if (AskText("Disassembly default linecount","Max. 2048:",tmpcb) == FALSE)
                return;
            sscanf (tmpcb,"%u",&i);
            if (i > 0 && i <= 2048)
                DefaultAsmLines = i;
            if (i > 1000 && FWflag == FALSE)    // friendly warning
                ShowFW();
            break;

        case CMD_ATTI:      // Toggle ASM Syntax
            bx_dbg_disassemble_switch_mode();
            ReloadAsm();
            break;

        case CMD_IOWIN:  // toggle display of internal debugger Input and Output windows
            if (! AtBreak)
                break;
            ShowIOWindows ^= TRUE;
            SetMenuCheckmark ((int) ShowIOWindows, CHK_CMD_IOWIN);
            VSizeChange();
            break;

        case CMD_SBTN:      // Toggle showing top pushbutton-row
            if (! AtBreak)
                break;
            ShowButtons ^= TRUE;
            SetMenuCheckmark ((int) ShowButtons, CHK_CMD_SBTN);
            VSizeChange();
            break;

        case CMD_UCASE:     // Toggle showing everything in uppercase
            UprCase ^= 1;
            SetMenuCheckmark ((int) UprCase, CHK_CMD_UCASE);
            MakeRDnames();
            if (AtBreak)
                LoadRegList();
            ReloadAsm();
            break;

        case CMD_MHEX:      // Toggle showing hex in Dump window
            ToggleDumpMode(2, CHK_CMD_MHEX, CMD_MASCII);
            break;

        case CMD_MASCII:    // Toggle showing ASCII in Dump window
            ToggleDumpMode(1, CHK_CMD_MASCII, CMD_MHEX);
            break;

        case CMD_LEND:      // Toggle Endianness for the MemDumps
            isLittleEndian ^= TRUE;
            SetMenuCheckmark ((int) isLittleEndian, CHK_CMD_LEND);
            RedrawMemDump();
            break;

        case CMD_WS_1:      // set memory dump "wordsize"
        case CMD_WS_2:      // "Align" = "wordsize" -- from 1 to 16
        case CMD_WS_4:
        case CMD_WS_8:
        case CMD_WS16:
            doNewWSize(cmd - CMD_WS_1);     // CMD_WS_1 .. CMD_WS16 must be consecutive
            break;

        case CMD_IGNSA:     // Toggle ID disassembly output ignoring
            ignSSDisasm ^= TRUE;
            SetMenuCheckmark ((int) ignSSDisasm, CHK_CMD_IGNSA);
            break;

        case CMD_IGNNT:     // Toggle NextT ignoring
            ignoreNxtT ^= TRUE;
            SetMenuCheckmark ((int) ignoreNxtT, CHK_CMD_IGNNT);
            break;

        case CMD_RCLR:      // Toggle Register Coloring
            SeeRegColors ^= TRUE;
            SetMenuCheckmark ((int) SeeRegColors, CHK_CMD_RCLR);
            if (AtBreak)
                LoadRegList();
            break;


        case CMD_EREG: // Show Registers of various types
        case CMD_SREG:
        case CMD_SYSR:
        case CMD_CREG:
        case CMD_FPUR:
        case CMD_XMMR:
        case CMD_DREG:
        case CMD_TREG:
            ToggleSeeReg(cmd);
            break;

        case CMD_ABOUT:     // "About" box
            DispMessage ("Bochs Enhanced Debugger, Version 1.2\r\nCopyright (C) Chourdakis Michael.\r\nModified by Bruce Ewing",
                "About");
            break;

        case CMD_FONT: // font
            ResizeColmns = TRUE;        // column widths are font dependent
            if (NewFont() != FALSE)
                VSizeChange();
            break;

        case CMD_LOGVIEW: // Toggle sending log to output window
            LogView ^= 1;
            SIM->set_log_viewer(LogView);
            SetMenuCheckmark((int)LogView, CHK_CMD_LOGVIEW);
            break;
    }
}

static BxEvent *enh_dbg_notify_callback(void *unused, BxEvent *event)
{
  switch (event->type)
  {
    case BX_SYNC_EVT_GET_DBG_COMMAND:
      {
        debug_cmd = new char[512];
        debug_cmd_ready = 0;
        HitBreak();
        while (debug_cmd_ready == 0 && bx_user_quit == 0)
        {
          if (vgaw_refresh != 0)  // is the GUI frontend requesting a VGAW refresh?
            SIM->refresh_vga();
          vgaw_refresh = 0;
#ifdef WIN32
          Sleep(10);
#elif BX_HAVE_USLEEP
          usleep(10000);
#else
          sleep(1);
#endif
        }
        if (bx_user_quit != 0) {
          bx_dbg_exit(0);
        }
        event->u.debugcmd.command = debug_cmd;
        event->retcode = 1;
        return event;
      }
    case BX_ASYNC_EVT_DBG_MSG:
      {
        ParseIDText(event->u.logmsg.msg);
        return event;
      }
    case BX_ASYNC_EVT_LOG_MSG:
      if (LogView) {
        ParseIDText(event->u.logmsg.msg);
        delete [] event->u.logmsg.msg;
        return event;
      }
    default:
      return (*old_callback)(old_callback_arg, event);
  }
}

static size_t strip_whitespace(char *s)
{
  size_t ptr = 0;
  char *tmp = new char[strlen(s)+1];
  strcpy(tmp, s);
  while (s[ptr] == ' ') ptr++;
  if (ptr > 0) strcpy(s, tmp+ptr);
  delete [] tmp;
  ptr = strlen(s);
  while ((ptr > 0) && (s[ptr-1] == ' ')) {
    s[--ptr] = 0;
  }
  return ptr;
}

static void ReadSettings()
{
  FILE *fd = NULL;
  char line[512], path[BX_PATHNAME_LEN];
  char *ret, *param, *val;
  bool format_checked = 0;
  size_t len1 = 0, len2;
  int i, n;

  if (global_ini) {
    get_bxshare_path(path);
    sprintf(ini_path, "%s/bx_enh_dbg.ini", path);
    fd = fopen(ini_path, "r");
  }
  if (fd == NULL) {
    fd = fopen("bx_enh_dbg.ini", "r");
    if (fd == NULL) return;
    BX_INFO(("Reading gui debugger settings from local ini file"));
  } else {
    BX_INFO(("Reading gui debugger settings from '%s'", ini_path));
  }
  do {
    ret = fgets(line, sizeof(line)-1, fd);
    line[sizeof(line) - 1] = '\0';
    size_t len = strlen(line);
    if ((len>0) && (line[len-1] < ' '))
      line[len-1] = '\0';
    if ((ret != NULL) && (strlen(line) > 0)) {
      if (!format_checked) {
        if (!strncmp(line, "# bx_enh_dbg_ini", 16)) {
          format_checked = 1;
        } else {
          fprintf(stderr, "bx_enh_dbg.ini: wrong file format\n");
          fclose(fd);
          return;
        }
      } else {
        if (line[0] == '#') continue;
        param = strtok(line, "=");
        if (param != NULL) {
          len1 = strip_whitespace(param);
          val = strtok(NULL, "");
          if (val == NULL) {
            fprintf(stderr, "bx_enh_dbg.ini: missing value for parameter '%s'\n", param);
            continue;
          }
        } else {
          continue;
        }
        len2 = strip_whitespace(val);
        if ((len1 == 0) || (len2 == 0)) continue;
        if ((len1 == 9) && !strncmp(param, "SeeReg[", 7) && (param[8] == ']')) {
          if ((param[7] < '0') || (param[7] > '7')) {
            fprintf(stderr, "bx_enh_dbg.ini: invalid index for option SeeReg[x]\n");
            continue;
          }
          i = atoi(&param[7]);
          SeeReg[i] = !strcmp(val, "TRUE");
        } else if (!strcmp(param, "SingleCPU")) {
          SingleCPU = !strcmp(val, "TRUE");
        } else if (!strcmp(param, "ShowIOWindows")) {
          ShowIOWindows = !strcmp(val, "TRUE");
        } else if (!strcmp(param, "ShowButtons")) {
          ShowButtons = !strcmp(val, "TRUE");
        } else if (!strcmp(param, "SeeRegColors")) {
          SeeRegColors = !strcmp(val, "TRUE");
        } else if (!strcmp(param, "ignoreNxtT")) {
          ignoreNxtT = !strcmp(val, "TRUE");
        } else if (!strcmp(param, "ignSSDisasm")) {
          ignSSDisasm = !strcmp(val, "TRUE");
        } else if (!strcmp(param, "UprCase")) {
          n = atoi(val);
          if (n != 0 && n != 1) {
            fprintf(stderr, "bx_enh_dbg.ini: invalid value '%s' for option '%s'\n", val, param);
            continue;
          }
          UprCase = n;
        } else if (!strcmp(param, "DumpInAsciiMode")) {
          n = atoi(val);
          if (n < 1 || n > 3) {     // bit 0 = ASCII, bit 1 = hex, 0 is illegal
            fprintf(stderr, "bx_enh_dbg.ini: invalid value '%s' for option '%s'\n", val, param);
            continue;
          }
          DumpInAsciiMode = n;
        } else if (!strcmp(param, "isLittleEndian")) {
          isLittleEndian = !strcmp(val, "TRUE");
        } else if (!strcmp(param, "DefaultAsmLines")) {
          n = atoi(val);
          if (n < 1)
            n = 1;
          else if (n > MAX_ASM)
            n = MAX_ASM;
          DefaultAsmLines = n;
        } else if (!strcmp(param, "DumpWSIndex")) {
          n = atoi(val);
          if (n < 0 || n > 4) {     // word size 1, 2, 4, 8 or 16 bytes
            fprintf(stderr, "bx_enh_dbg.ini: invalid value '%s' for option '%s'\n", val, param);
            continue;
          }
          DumpWSIndex = n;
          DumpAlign = (1 << DumpWSIndex);
          PrevDAD = 0;
        } else if (!strcmp(param, "DockOrder")) {
          n = (int) strtoul(val, NULL, 16);
          // each of the 3 lists (1 = Reg, 2 = ASM, 3 = Dump) must appear exactly once
          if (n != 0x123 && n != 0x132 && n != 0x213 && n != 0x231 && n != 0x312 && n != 0x321) {
            fprintf(stderr, "bx_enh_dbg.ini: invalid value '%s' for option '%s'\n", val, param);
            continue;
          }
          DockOrder = (short) n;
        } else if ((len1 == 15) && !strncmp(param, "ListWidthPix[", 13) && (param[14] == ']')) {
          if ((param[13] < '0') || (param[13] > '2')) {
            fprintf(stderr, "bx_enh_dbg.ini: invalid index for option ListWidthPix[x]\n");
            continue;
          }
          i = atoi(&param[13]);
          n = atoi(val);
          if (n <= 0) {
            fprintf(stderr, "bx_enh_dbg.ini: invalid value '%s' for option '%s'\n", val, param);
            continue;
          }
          ListWidthPix[i] = n;
        } else if (!ParseOSSettings(param, val)) {
          fprintf(stderr, "bx_enh_dbg.ini: unknown option '%s'\n", line);
        }
      }
    }
  } while (!feof(fd));
  fclose(fd);
}

static void WriteSettings()
{
  FILE *fd = NULL;
  int i;

  if (global_ini) {
    fd = fopen(ini_path, "w");
  }
  if (fd == NULL) {
    fd = fopen("bx_enh_dbg.ini", "w");
    if (fd == NULL) return;
    BX_INFO(("Saving gui debugger settings to local ini file"));
  } else {
    BX_INFO(("Saving gui debugger settings to '%s'", ini_path));
  }
  fprintf(fd, "# bx_enh_dbg_ini\n");
  for (i = 0; i < 8; i++) {
    fprintf(fd, "SeeReg[%d] = %s\n", i, SeeReg[i] ? "TRUE" : "FALSE");
  }
  fprintf(fd, "SingleCPU = %s\n", SingleCPU ? "TRUE" : "FALSE");
  fprintf(fd, "ShowIOWindows = %s\n", ShowIOWindows ? "TRUE" : "FALSE");
  fprintf(fd, "ShowButtons = %s\n", ShowButtons ? "TRUE" : "FALSE");
  fprintf(fd, "SeeRegColors = %s\n", SeeRegColors ? "TRUE" : "FALSE");
  fprintf(fd, "ignoreNxtT = %s\n", ignoreNxtT ? "TRUE" : "FALSE");
  fprintf(fd, "ignSSDisasm = %s\n", ignSSDisasm ? "TRUE" : "FALSE");
  fprintf(fd, "UprCase = %d\n", UprCase);
  fprintf(fd, "DumpInAsciiMode = %d\n", DumpInAsciiMode);
  fprintf(fd, "isLittleEndian = %s\n", isLittleEndian ? "TRUE" : "FALSE");
  fprintf(fd, "DefaultAsmLines = %d\n", DefaultAsmLines);
  fprintf(fd, "DumpWSIndex = %d\n", DumpWSIndex);
  fprintf(fd, "DockOrder = 0x%03x\n", DockOrder);
  for (i = 0; i < 3; i++) {
    fprintf(fd, "ListWidthPix[%d] = %d\n", i, ListWidthPix[i]);
  }
  WriteOSSettings(fd);
  fclose(fd);
}

void InitDebugDialog(bool GlobalIni)
{
  // redirect notify callback to the debugger specific code
  SIM->get_notify_callback(&old_callback, &old_callback_arg);
  assert (old_callback != NULL);
  SIM->set_notify_callback(enh_dbg_notify_callback, NULL);
  global_ini = GlobalIni;
  ReadSettings();
  DoAllInit();    // non-os-specific init stuff
  OSInit();
}

void CloseDebugDialog()
{
  WriteSettings();
  SIM->set_log_viewer(0);
  SIM->set_notify_callback(old_callback, old_callback_arg);
  CloseDialog();
}

bool DetachDebugger()
{
    // can't run sim until everything is ready
    if (! AtBreak || debug_cmd_ready)
        return FALSE;

    SendDebugCmd("detach");
    return TRUE;
}

#endif
