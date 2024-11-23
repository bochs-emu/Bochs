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
//  Copyright (C) 2008-2024  The Bochs Project

#include "config.h"

#if BX_DEBUGGER && BX_DEBUGGER_GUI

#include <math.h>

#include "bochs.h"
#include "siminterface.h"
#include "bx_debug/debug.h"
#include "memory/memory-bochs.h"
#include "pc_system.h"
#include "cpu/cpu.h"

extern char* disasm(const Bit8u *opcode, bool is_32, bool is_64, char *disbufptr, bxInstruction_c *i, bx_address cs_base, bx_address rip);

#include "enh_dbg.h"

// Match stuff
#define MATCH_TRUE 1
#define MATCH_FALSE 0
#define MATCH_ABORT -1

#define NEGATE_CLASS
#define OPTIMIZE_JUST_STAR

const char* DC0txt[2] = {"P.Address","L.Address"};    // DumpMode definitions in text

const char* BTxt[6] = {
  "Continue [c]",
  "Step [s]",
  "Step N [s ###]",
  "Refresh",
  "Break [^C]",
  "Break All"};

int BtnLkup[6] = {
    CMD_CONT, CMD_STEP1, CMD_STEPN, CMD_RFRSH, CMD_BREAK
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
int UprCase = 0;             // 1 = convert all Asm, Register names, Register values to uppercase
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

// only pay special attention to registers up to EFER
static const char* RegLCName[EFER_Rnum + 1] = {
    "rax","rbx","rcx","rdx","rsi","rdi","rbp","rsp","rip",
    "r8","r9","r10","r11","r12","r13","r14","r15",
    "eflags","eax","ebx","ecx","edx","esi","edi","ebp","esp","eip",
    "cs","ds","es","ss","fs","gs",
    "gdtr","idtr","ldtr","tr","cr0","cr2","cr3","cr4","efer"
};
static char* RDispName[EFER_Rnum + 1];
static bx_param_num_c *RegObject[BX_MAX_SMP_THREADS_SUPPORTED][TOT_REG_NUM + EXTRA_REGS];
Bit64u rV[EFER_Rnum + 1];   // current values of registers
Bit64u PV[EFER_Rnum + 1];   // previous values of registers
Bit32s GDT_Len;             // "limits" (= bytesize-1) for GDT and IDT
Bit32s IDT_Len;
Bit8u RegColor[TOT_REG_NUM];    // specifies foreground and background color of registers
// Text color is red if the upper bit is set. Background is set according to ColorList.
int RitemToRnum[TOT_REG_NUM];   // mapping from Reg List Item# to register number

Bit64u ladrmin = 0; // bochs linear addressing access variables
Bit64u ladrmax = 0;
Bit64u l_p_offset;

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
char AsciiHex[512];            // Unsigned char to printable hex xlat table
static char UCtable[256];

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

// Register hex display formats -- index by UprCase
static const char* Fmt64b[2] = { FMT_ADDRX64, "%016llX" };
static const char* Fmt32b[2] = { "%08x", "%08X" };
static const char* Fmt16b[2] = { "%04x", "%04X" };
static const char* xDT64Fmt[2] = { FMT_ADDRX64 " (%4x)", "%016llX (%4X)" };
static const char* xDT32Fmt[2] = { "%08x (%4x)", "%08X (%4X)" };

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

bool global_ini;
char ini_path[BX_PATHNAME_LEN];
char *debug_cmd;
bool debug_cmd_ready;
bool vgaw_refresh;

static bxevent_handler old_callback = NULL;
static void *old_callback_arg = NULL;

short nDock[36] = {     // lookup table for alternate DockOrders
    0x231, 0x312, 0x231, 0x213, 0x132, 0x132,
    0x213, 0x321, 0x123, 0x123, 0x321, 0x312,
    0x213, 0x213, 0x123, 0x312, 0x321, 0x312,
    0x132, 0x123, 0x132, 0x321, 0x231, 0x231,
    0x312, 0x312, 0x231, 0x213, 0x132, 0x213,
    0x132, 0x123, 0x321, 0x321, 0x123, 0x231
};

void MakeXlatTables()
{
    char *p, c;
    int i = 256;
    while (--i >= 0)            // make an upper case translation table
        UCtable[i]= toupper(i);
    p = AsciiHex;               //  then also make a "hex" table
    for ( i = 0; i < 256; i++)
    {
        c = i >> 4;
        if (c > 9)
            c += 'A' - 10;  // do all hex in uppercase
        else
            c += '0';
        *(p++)= c;
        c = i & 0xf;
        if (c > 9)
            c += 'A' - 10;
        else
            c += '0';
        *(p++)= c;
    }
}

int DoMatch(const char *text, const char *p, bool IsCaseSensitive)
{
    // probably the MOST DIFFICULT FUNCTION in TurboIRC
    // Thanks to BitchX for copying this function
    //int last;
    int matched;
    //int reverse;
    int pT = 0;
    int pP = 0;

    for (; p[pP] != '\0'; pP++, pT++)
    {
        if (text[pT] == '\0' && p[pP] != '*')
            return MATCH_ABORT;
        switch (p[pP])
        {
            //         case '\\': // Match with following char
            //                pP++;
            // NO BREAK HERE

            default:
                if (IsCaseSensitive)
                {
                    if (text[pT] != p[pP])
                        return MATCH_FALSE;
                    else
                        continue;
                }
                if (UCtable[(int) text[pT]] != UCtable[(int) p[pP]])
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

// This will be called from the other funcs
int VMatching(const char *text, const char *p, bool IsCaseSensitive)
{
#ifdef OPTIMIZE_JUST_STAR
    if (p[0] == '*' && p[1] == '\0')
        return MATCH_TRUE;
#endif
    return (DoMatch(text, p, IsCaseSensitive) == MATCH_TRUE);
}

int IsMatching(const char *text, const char *p, bool IsCaseSensitive)
{
    return VMatching(text, p, IsCaseSensitive);
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
void upr(char* d)
{
    char *p;
    p = d;
    while (*p != 0)
    {
        if (*p == '0' && p[1] == 'x')
            p += 2;
        else
        {
            *p = UCtable[(int) *p]; // use the lookup table created by MakeXlatTables
            ++p;
        }
    }
}

// create EFLAGS display for Status line
void ShowEflags(char *buf)
{
    static const char *EflBName[16] = {
        "cf", "pf", "af", "zf", "sf", "tf", "if", "df", "of", "nt", "rf", "vm", "ac", "vif", "vip", "id"
    };
    static const int EflBNameLen[16] = {
        2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,2
    };
    static const int EflBitVal[16] = {
        1, 4, 0x10, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x4000, 0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000
    };

    Bit32u Efl = (Bit32u) rV[EFL_Rnum];
    int i = 16;
    char *cp = buf + 6;

    sprintf(buf,"IOPL=%1u", (Efl & 0x3000) >> 12);
    while (--i >= 0)
    {
        *(cp++)= ' ';
        strcpy (cp, EflBName[i]);       // copy the name of the bitflag
        if ((Efl & EflBitVal[i]) != 0)  // if the bit is set, put the name in uppercase
            upr(cp);
        cp += EflBNameLen[i];
    }
}

// change the display on the status line if anything has changed
void UpdateStatus()
{
    static bool PrevAtBreak = FALSE;

    if (StatusChange == FALSE) return;  // avoid sending unnecessary messages/invalidations
    StatusChange = FALSE;

    if (AtBreak != FALSE)   // modify status line only during a break
    {
        char eflags_buf[80];
        ShowEflags(eflags_buf);         // prints out eflags
        SetStatusText(3, eflags_buf);   // display eflags

        if (CpuModeChange != FALSE)     // Did CR0 bits or EFER bits change value?
        {
            CpuModeChange = FALSE;
            char mode[32];
            switch (CpuMode) {
                case BX_MODE_IA32_REAL:
                    if (In32Mode == FALSE)
                        strcpy (mode, "CPU: Real Mode 16");
                    else
                        strcpy (mode, "CPU: Real Mode 32");
                    break;
                case BX_MODE_IA32_V8086:
                    strcpy (mode, "CPU: V8086 Mode");
                    break;
                case BX_MODE_IA32_PROTECTED:
                    if (In32Mode == FALSE) {
                        if (InPaging != 0)
                            strcpy (mode, "CPU: Protected Mode 16 (PG)");
                        else
                            strcpy (mode, "CPU: Protected Mode 16");
                    }
                    else {
                        if (InPaging != 0)
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

// Read a copy of some emulated linear bochs memory
// Note: laddr + len must not cross a 4K boundary -- otherwise, there are no limits
bool ReadBxLMem(Bit64u laddr, unsigned len, Bit8u *buf)
{
    return bx_dbg_read_linear(CurrentCPU, laddr, len, buf);
}

// binary conversion (and validity testing) on hex/decimal char string inputs
Bit64u cvthex(char *p, Bit64u errval)
{
    Bit64u ret = 0;
    bool end = FALSE;
    while (end == FALSE)
    {
        if (*p >= '0' && *p <= '9')             // test for digits
            ret = (ret << 4) | (*(p++) - '0');
        else if ((*p | 0x20) >= 'a' && (*p | 0x20) <= 'f')  // test for hex letters
            ret = (ret << 4) | ((*(p++) | 0x20) - 'a' + 10);    // 0xA = 10, of course
        else
            end = TRUE;
    }
    if (*p != 0 && *p != ' ' && *p != '\t')     // hex must end on a whitespace
        return errval;
    return ret;
}

Bit64u cvt64(char *nstr, bool negok)
{
    char *p, *s;
    Bit64u ret = 0;
    bool neg = FALSE;
    p= nstr;
    while (*p==' ' || *p == '\t')
        ++p;
    if (*p == '-' && negok != FALSE)
    {
        ++p;
        neg = TRUE;
    }
    if (*p == '0' && (p[1] | 0x20) == 'x' && neg == FALSE)
        return cvthex (p+2, 0);
    s = p;
    while (*p >= '0' && *p <= '9')
        ret = (ret * 10) + *(p++) - '0';
    if ((*p | 0x20) >= 'a' && (*p | 0x20) <= 'f' && neg == FALSE)
        return cvthex (s, ret);
    if (neg != FALSE)
        return 0 - ret;
    return ret;
}

// "singlestep" disassembly lines from the internal debugger are sometimes ignored
bool isSSDisasm(char *s)
{
    if (ignSSDisasm == FALSE)   // ignoring those lines?
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
    if (ignoreNxtT != FALSE)
    {
        if (strncmp(s,"Next at t",9) == 0)
            return;
    }
    if (isSSDisasm(s) != FALSE)
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
void FillRegs()
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
        if (bx_guard.iaddr.lin[i].enabled != FALSE)
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
int FillSSE(int LineCount)
{
#if BX_CPU_LEVEL >= 6
    Bit64u val = 0;
    bx_param_num_c *p;
    char *cols[3];
    char ssetxt[80];

    if ((rV[CR0_Rnum] & 0xc) != 0)  // TS or EM flags in CR0 temporarily disable SSE
    {
        cols[0] = ssetxt;
        strcpy (ssetxt, "SSE-off");
        InsertListRow(cols, 1, REG_WND, LineCount, 4);
        RitemToRnum[LineCount] = XMM0_Rnum;
        return ++LineCount;
    }

// format: XMM[#] 00000000:00000000 (each 16 hex digits)
    *ssetxt = 0;
    cols[1] = ssetxt;       // column 1 is being left blank
    cols[0] = ssetxt + 1;
    cols[2] = ssetxt + 10;
    strcpy (ssetxt+1, "XMM[0]");
    ssetxt[10] = '0';       // I'm putting a hex value in the decimal column -- more room there!
    ssetxt[11] = 'x';
    strcpy (ssetxt + 28, " : ");
    for (int i = 0; i < /*BX_XMM_REGISTERS*/ 16; i++)
    {
        if (i >= 10)
        {
            ssetxt[5] = '1';
            ssetxt[6] = i - 10 + '0';
            ssetxt[7] = ']';
            ssetxt[8] = 0;
        }
        else
            ssetxt[5] = i + '0';

        RitemToRnum[LineCount] = i + XMM0_Rnum;
        p = RegObject[CurrentCPU][XMM0_hi + i];
        if (p != NULL)
            val = p->get64();   // get the value of "xmm(i)_hi" register
        else
            val = 0;
        sprintf (ssetxt + 12,Fmt64b[UprCase],val);
        p = RegObject[CurrentCPU][XMM0_Rnum + i];
        if (p != NULL)
            val = p->get64();       // "SSE.xmm[i]_lo"
        else
            val = 0;
        sprintf (ssetxt + 31,Fmt64b[UprCase], val);
        InsertListRow(cols, 3, REG_WND, LineCount, 4);  // 3 cols, group 4
        ++LineCount;
    }
#endif
    return (LineCount);
}

// this routine is only called if debugger already knows FPU is supported
// -- but it might not be active
int FillMMX(int LineCount)
{
    static double scale_factor = pow(2.0, -63.0);
    int i;
    Bit16u exp = 0;
    Bit64u mmreg = 0;
    bx_param_num_c *p;
    unsigned short exponent[8];
    char *cols[3];
    char fputxt[60];

    cols[0] = fputxt;
    if ((rV[CR0_Rnum] & 0xc) != 0)  // TS or EM flags in CR0 temporarily disable MMX/FPU/SSE
    {
        strcpy (fputxt, "FPU-off");
        InsertListRow(cols, 1, REG_WND, LineCount, 3);
        RitemToRnum[LineCount] = ST0_Rnum;
        return ++LineCount;
    }

// format: MM#|ST# 00000000:00000000 then FPU float value in "decimal" column
    cols[1] = fputxt + 10;
    cols[2] = fputxt + 32;
    strcpy (fputxt, "MM0-ST0");
    strcpy (fputxt + 18, " : ");
    i = 7;
    for (i = 0; i < 8; i++)
    {
        fputxt[2] = i + '0';
        fputxt[6] = i + '0';
        RitemToRnum[LineCount] = i + ST0_Rnum;
        p = RegObject[CurrentCPU][ST0_Rnum + i];
        if (p != NULL)
            mmreg = p->get64(); // get the value of "mmx(i)" register
        else
            mmreg = 0;
        sprintf (fputxt + 10,Fmt32b[UprCase],GET32H(mmreg));
        sprintf (fputxt + 21,Fmt32b[UprCase], GET32L(mmreg));

        p = RegObject[CurrentCPU][ST0_exp + i];
        if (p != NULL)
            exp = (Bit16u) p->get64();  // get the exponent for this FPU register
        else
            exp = 0;
        exponent[i] = exp;              // save each one temporarily
        double f = pow(2.0, ((0x7fff & exp) - 0x3fff));
        if (exp & 0x8000)
            f = -f;
#ifdef _MSC_VER
        f *= (double)(signed __int64)(mmreg>>1) * scale_factor * 2;
#else
        f *= mmreg*scale_factor;
#endif
        sprintf (cols[2],"%.3e",f);
        InsertListRow(cols, 3, REG_WND, LineCount, 3);  // 3 cols, group 3
        ++LineCount;
    }
    strcpy (fputxt, "ST0.exp");
    for (i = 0; i < 8; i++)
    {
        fputxt[2] = i + '0';
        RitemToRnum[LineCount] = i + ST0_exp;
        sprintf (fputxt+10,Fmt16b[UprCase], exponent[i]);   // col1
        sprintf (fputxt+32,"%u", exponent[i]);      // col2
        InsertListRow(cols, 3, REG_WND, LineCount, 3);  // 3 cols, group 3
        ++LineCount;
    }
    return LineCount;
}

// get values of Debug registers from simulation
int FillDebugRegs(int itemnum)
{
    bx_param_num_c *bxp;
    Bit32u val;
    unsigned int i;
    char *cols[2];
    char drtxt[20];

    strcpy (drtxt,"dr0");
    if (UprCase != FALSE)
    {
        *drtxt = 'D';
        drtxt[1] = 'R';
    }
    cols[0] = drtxt;
    cols[1] = drtxt + 4;

    for(i = 0; i < 6; i++)
    {
        bxp = RegObject[CurrentCPU][DR0_Rnum + i];
        val = 0;
        if (bxp != NULL)
             val = bxp->get();
        RitemToRnum[itemnum] = i + DR0_Rnum;
        sprintf(drtxt + 4,Fmt32b[UprCase],val);

        InsertListRow(cols, 2, REG_WND, itemnum, 5);    // 3 cols, group 5
        ++drtxt[2];         // change the name, the cheap way
        if (i == 3) drtxt[2] += 2;  // jump from "DR3" to "DR6"
        ++itemnum;
    }
    return itemnum;
}

#define BX_GUI_DB_ASM_DATA (4400)

// Disassemble a linear memory area, in a loop, loading text into ASM window
// completely update the ASM display with new data
void FillAsm(Bit64u LAddr, int MaxLines)
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

    while (Go != FALSE)
    {
        // copydown buffer -- buffer size must be 4K + 16
        s= AsmData;
        i= BufLen;      // BufLen is guaranteed < 16
        while (i-- > 0)
            *(s++)= *(p++);
        // load buffer, up to the next 4k boundary
        len = 4096 - (((int) ReadAddr) & 0xfff);    // calculate read amount
        Go = ReadBxLMem(ReadAddr, len, (Bit8u *) s);
        BufLen += len;
        ReadAddr += len;
        if (Go == FALSE)
            break;
        BufEmpty = FALSE;
        p= AsmData;         // start at the beginning of the new buffer
        while (AsmLineCount < MaxLines && BufEmpty == FALSE)
        {
            // disassemble 1 line with a direct call, into asmtxt
            len = bx_dbg_disasm_wrapper(In32Mode, In64Mode, (bx_address) 0,
                  (bx_address) LAddr, (Bit8u *) p, cols[2]);

            if (len <= BufLen)      // disassembly was successful?
            {
                AsmLA[AsmLineCount] = LAddr;        // save, and
                if (In64Mode == FALSE)      // "display" linear addy of the opcode
                    sprintf (asmtxt,Fmt32b[UprCase],LAddr);
                else
                    sprintf (asmtxt,Fmt64b[UprCase],LAddr);

                BufLen -= len;  // used up len bytes from buffer
                LAddr += len;   // calculate next LAddr

                // then build the "bytes" column entry
                s = cols[1];
                *(s++) = '(';   // begin with the bytecount in parens
                i = len;
                if (len > 9)
                {
                    *(s++)=  '1';   // len < 16, so convert to decimal the easy way
                    i -= 10;
                }
                *(s++) = i + '0';
                *(s++) = ')';
                *(s++) = ' ';
                while (len-- > 0)
                {
                    i = (unsigned char) *(p++);
                    *(s++) = AsciiHex[2*i];
                    *(s++) = AsciiHex[2*i+1];
                }
                *s = 0;     // zero terminate the "bytes" string

                // then, finalize the ASM disassembly text
                if (UprCase != FALSE)   // do any requested uppercase conversion on the text
                    upr(cols[2]);
                InsertListRow(cols, 3, ASM_WND, AsmLineCount, 8);   // 3 cols, "no" group
                ++AsmLineCount;
            }
            else
                BufEmpty = TRUE;
        }
        if (AsmLineCount >= MaxLines)       // disassembled enough lines?
            Go = FALSE;
        if (In64Mode == FALSE && ReadAddr > 0xffffffff)
            break;
    }
    if (ResizeColmns != FALSE)
        RedrawColumns(ASM_WND);
    EndListUpdate(ASM_WND);
}


// Reload the entire Register window with data
void LoadRegList()
{
    int i, itemnum;     // TODO: This routine needs a big rewrite to make it pretty
    bool showEreg = TRUE;
    char *cols[3];
    char regtxt[100];
    StartListUpdate(REG_WND);
    FillRegs();         // get new values for rV local register array

// Display GP registers -- 64 bit registers first, if they exist
    cols[1] = regtxt;
    cols[2] = regtxt + 40;
    itemnum = 0;
#if BX_SUPPORT_X86_64
    if (In64Mode)
    {
        showEreg = SeeReg[0];       // get user option setting for EAX, etc.
        for (i = RAX_Rnum; i <= R15_Rnum; i++)
        {
            RitemToRnum[itemnum] = i;   // always recreate the register -> itemnum mapping
            sprintf(regtxt,Fmt64b[UprCase], rV[i]);  // print the hex column
            sprintf(cols[2], FMT_LL "d", rV[i]);     // and decimal
            cols[0] = RDispName[i];
            InsertListRow(cols, 3, REG_WND, itemnum, 0);    // 3 cols, group 0
            ++itemnum;
        }
    }
#endif

    // then 32bit GP registers (if appropriate)
    if (showEreg != FALSE)
    {
        for (i = EAX_Rnum; i <= EIP_Rnum; i++)
        {
            RitemToRnum[itemnum] = i;
            sprintf(regtxt, Fmt32b[UprCase], (Bit32u)rV[i]);  // print the hex column
            sprintf(cols[2], FMT_LL "d", rV[i]);     // and decimal
            cols[0] = RDispName[i];

            if (In32Mode == FALSE && i == 26)   // Check for Real Mode (Pmode is TRUE in Long Mode)
            {
                rV[EIP_Rnum] &= 0xffff;     // in Real Mode, mask IP to 2 bytes
                ++cols[0];                  // and shorten name to 2 letters
            }
            InsertListRow(cols, 3, REG_WND, itemnum, 0);    // 3 cols, group 0
            ++itemnum;
        }
    }
    // always insert eflags next
    RitemToRnum[itemnum] = EFL_Rnum;
    sprintf(regtxt,Fmt32b[UprCase],(Bit32u)rV[EFL_Rnum]);   // print the hex column
    cols[0] = RDispName[EFL_Rnum];
    InsertListRow(cols, 2, REG_WND, itemnum, 0);    // 2 cols, group 0
    ++itemnum;

    if (rV[EFL_Rnum] != PV[EFL_Rnum])
        StatusChange = TRUE;    // if eflags changed, force a status update

    // display Segment registers (if requested)
    if (SeeReg[1])
    {
        for(i = CS_Rnum; i <= GS_Rnum; i++)       // segment registers
        {
            RitemToRnum[itemnum] = i;
            sprintf(regtxt, Fmt16b[UprCase], rV[i] & 0xffff);
            cols[0] = RDispName[i];
            InsertListRow(cols, 2, REG_WND, itemnum, 1);    // 2 cols, group 1
            ++itemnum;
        }
    }
    // display System registers (if requested)
    // displaying these once may be necessary for column resizing
    if (SeeReg[2] || (ResizeColmns != FALSE && NeedSysRresize != FALSE))
    {
        int j = TRRnum;
        if (In32Mode == FALSE)      // don't show lgdt or tr in Real mode
            j= IDTRnum;
        for(i = GDTRnum; i <= j; i++)
        {
            RitemToRnum[itemnum] = i;
            if (i == GDTRnum || i == IDTRnum)
            {
                Bit16u limit = GDT_Len;
                if (i == IDTRnum)
                    limit = IDT_Len;
                if (In64Mode == FALSE)
                    sprintf(regtxt,xDT32Fmt[UprCase],(Bit32u)rV[i],limit);
                else
                    sprintf(regtxt,xDT64Fmt[UprCase],rV[i],limit);

            }
            else
                sprintf(regtxt,Fmt16b[UprCase], rV[i] & 0xffff);    // lgdt, tr

            cols[0] = RDispName[i];
            InsertListRow(cols, 2, REG_WND, itemnum, 1);    // 2 cols, group 1
            ++itemnum;
        }
    }
    // display Control Registers (if requested)
    if (SeeReg[3])
    {
        for(i = CR0_Rnum; i <= EFER_Rnum; i++)
        {
            RitemToRnum[itemnum] = i;

            if (In64Mode && (i == CR2_Rnum || i == CR3_Rnum))
                sprintf(regtxt,Fmt64b[UprCase],rV[i]);
            else
                sprintf(regtxt,Fmt32b[UprCase],(Bit32u)rV[i]);

            cols[0] = RDispName[i];
            InsertListRow(cols, 2, REG_WND, itemnum, 2);    // 2 cols, group 2
            ++itemnum;
        }
    }

    // set the register background colors for rV
    i = EFER_Rnum + 1;          // total number of registers stored in rV
    while (--i >= 0)
    {
        if (rV[i] != PV[i])     // set the "red" flag if value changed
            RegColor[i] |= 0x80;
        else
            RegColor[i] &= 0x7f;
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
void doAsmScroll()
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
void CanDoLA(Bit64u *h)
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

void InitRegObjects()
{
    bx_list_c *cpu_list;
    extern bx_list_c *root_param;
    int cpu = BX_SMP_PROCESSORS;
    // get the param tree interface objects for every single register on all CPUs
    while (--cpu >= 0)
    {
        // RegObject[j]s are all initialized to NULL when allocated in the BSS area
        // but it doesn't hurt anything to do it again, once
        int i = TOT_REG_NUM + EXTRA_REGS;
        while (--i >= 0)
            RegObject[cpu][i] = (bx_param_num_c *) NULL;

        char pname[20];
        sprintf (pname,"bochs.cpu%d", cpu);    // set the "cpu number" for cpu_list
        cpu_list = (bx_list_c *) SIM->get_param(pname, root_param);

        // TODO: in the next version, put all the names in an array, and loop
        // -- but that method is not compatible with bochs 2.3.7 or earlier
#if BX_SUPPORT_X86_64 == 0
        RegObject[cpu][EAX_Rnum] = SIM->get_param_num("EAX", cpu_list);
        RegObject[cpu][EBX_Rnum] = SIM->get_param_num("EBX", cpu_list);
        RegObject[cpu][ECX_Rnum] = SIM->get_param_num("ECX", cpu_list);
        RegObject[cpu][EDX_Rnum] = SIM->get_param_num("EDX", cpu_list);
        RegObject[cpu][ESI_Rnum] = SIM->get_param_num("ESI", cpu_list);
        RegObject[cpu][EDI_Rnum] = SIM->get_param_num("EDI", cpu_list);
        RegObject[cpu][EBP_Rnum] = SIM->get_param_num("EBP", cpu_list);
        RegObject[cpu][ESP_Rnum] = SIM->get_param_num("ESP", cpu_list);
        RegObject[cpu][EIP_Rnum] = SIM->get_param_num("EIP", cpu_list);
#else
        RegObject[cpu][RAX_Rnum] = SIM->get_param_num("RAX", cpu_list);
        RegObject[cpu][RBX_Rnum] = SIM->get_param_num("RBX", cpu_list);
        RegObject[cpu][RCX_Rnum] = SIM->get_param_num("RCX", cpu_list);
        RegObject[cpu][RDX_Rnum] = SIM->get_param_num("RDX", cpu_list);
        RegObject[cpu][RSI_Rnum] = SIM->get_param_num("RSI", cpu_list);
        RegObject[cpu][RDI_Rnum] = SIM->get_param_num("RDI", cpu_list);
        RegObject[cpu][RBP_Rnum] = SIM->get_param_num("RBP", cpu_list);
        RegObject[cpu][RSP_Rnum] = SIM->get_param_num("RSP", cpu_list);
        RegObject[cpu][RIP_Rnum] = SIM->get_param_num("RIP", cpu_list);
        RegObject[cpu][R8_Rnum] = SIM->get_param_num("R8", cpu_list);
        RegObject[cpu][R9_Rnum] = SIM->get_param_num("R9", cpu_list);
        RegObject[cpu][R10_Rnum] = SIM->get_param_num("R10", cpu_list);
        RegObject[cpu][R11_Rnum] = SIM->get_param_num("R11", cpu_list);
        RegObject[cpu][R12_Rnum] = SIM->get_param_num("R12", cpu_list);
        RegObject[cpu][R13_Rnum] = SIM->get_param_num("R13", cpu_list);
        RegObject[cpu][R14_Rnum] = SIM->get_param_num("R14", cpu_list);
        RegObject[cpu][R15_Rnum] = SIM->get_param_num("R15", cpu_list);
#endif
        RegObject[cpu][EFL_Rnum] = SIM->get_param_num("EFLAGS", cpu_list);
        RegObject[cpu][CS_Rnum] = SIM->get_param_num("CS.selector", cpu_list);
        RegObject[cpu][DS_Rnum] = SIM->get_param_num("DS.selector", cpu_list);
        RegObject[cpu][ES_Rnum] = SIM->get_param_num("ES.selector", cpu_list);
        RegObject[cpu][SS_Rnum] = SIM->get_param_num("SS.selector", cpu_list);
        RegObject[cpu][FS_Rnum] = SIM->get_param_num("FS.selector", cpu_list);
        RegObject[cpu][GS_Rnum] = SIM->get_param_num("GS.selector", cpu_list);
        RegObject[cpu][GDTRnum] = SIM->get_param_num("GDTR.base", cpu_list);
        RegObject[cpu][GDTR_Lim] = SIM->get_param_num("GDTR.limit", cpu_list);
        RegObject[cpu][IDTRnum] = SIM->get_param_num("IDTR.base", cpu_list);
        RegObject[cpu][IDTR_Lim] = SIM->get_param_num("IDTR.limit", cpu_list);
        RegObject[cpu][LDTRnum] = SIM->get_param_num("LDTR.base", cpu_list);
        RegObject[cpu][TRRnum] = SIM->get_param_num("TR.base", cpu_list);
        RegObject[cpu][CR0_Rnum] = SIM->get_param_num("CR0", cpu_list);
        RegObject[cpu][CR2_Rnum] = SIM->get_param_num("CR2", cpu_list);
        RegObject[cpu][CR3_Rnum] = SIM->get_param_num("CR3", cpu_list);
#if BX_CPU_LEVEL >= 5
        RegObject[cpu][CR4_Rnum] = SIM->get_param_num("CR4", cpu_list);
#endif
#if BX_CPU_LEVEL >= 6
        RegObject[cpu][EFER_Rnum] = SIM->get_param_num("MSR.EFER", cpu_list);
#endif
#if BX_SUPPORT_FPU
        RegObject[cpu][ST0_Rnum] = SIM->get_param_num("FPU.st0.fraction", cpu_list);
        RegObject[cpu][ST1_Rnum] = SIM->get_param_num("FPU.st1.fraction", cpu_list);
        RegObject[cpu][ST2_Rnum] = SIM->get_param_num("FPU.st2.fraction", cpu_list);
        RegObject[cpu][ST3_Rnum] = SIM->get_param_num("FPU.st3.fraction", cpu_list);
        RegObject[cpu][ST4_Rnum] = SIM->get_param_num("FPU.st4.fraction", cpu_list);
        RegObject[cpu][ST5_Rnum] = SIM->get_param_num("FPU.st5.fraction", cpu_list);
        RegObject[cpu][ST6_Rnum] = SIM->get_param_num("FPU.st6.fraction", cpu_list);
        RegObject[cpu][ST7_Rnum] = SIM->get_param_num("FPU.st7.fraction", cpu_list);
        RegObject[cpu][ST0_exp] = SIM->get_param_num("FPU.st0.exp", cpu_list);
        RegObject[cpu][ST1_exp] = SIM->get_param_num("FPU.st1.exp", cpu_list);
        RegObject[cpu][ST2_exp] = SIM->get_param_num("FPU.st2.exp", cpu_list);
        RegObject[cpu][ST3_exp] = SIM->get_param_num("FPU.st3.exp", cpu_list);
        RegObject[cpu][ST4_exp] = SIM->get_param_num("FPU.st4.exp", cpu_list);
        RegObject[cpu][ST5_exp] = SIM->get_param_num("FPU.st5.exp", cpu_list);
        RegObject[cpu][ST6_exp] = SIM->get_param_num("FPU.st6.exp", cpu_list);
        RegObject[cpu][ST7_exp] = SIM->get_param_num("FPU.st7.exp", cpu_list);
#endif

#if BX_CPU_LEVEL >= 6
        CpuSupportSSE = SIM->get_param("SSE", cpu_list) != NULL;

        if (CpuSupportSSE) {
            RegObject[cpu][XMM0_Rnum] = SIM->get_param_num("SSE.xmm00_0", cpu_list);
            RegObject[cpu][XMM1_Rnum] = SIM->get_param_num("SSE.xmm01_0", cpu_list);
            RegObject[cpu][XMM2_Rnum] = SIM->get_param_num("SSE.xmm02_0", cpu_list);
            RegObject[cpu][XMM3_Rnum] = SIM->get_param_num("SSE.xmm03_0", cpu_list);
            RegObject[cpu][XMM4_Rnum] = SIM->get_param_num("SSE.xmm04_0", cpu_list);
            RegObject[cpu][XMM5_Rnum] = SIM->get_param_num("SSE.xmm05_0", cpu_list);
            RegObject[cpu][XMM6_Rnum] = SIM->get_param_num("SSE.xmm06_0", cpu_list);
            RegObject[cpu][XMM7_Rnum] = SIM->get_param_num("SSE.xmm07_0", cpu_list);
            RegObject[cpu][XMM0_hi] = SIM->get_param_num("SSE.xmm00_1", cpu_list);
            RegObject[cpu][XMM1_hi] = SIM->get_param_num("SSE.xmm01_1", cpu_list);
            RegObject[cpu][XMM2_hi] = SIM->get_param_num("SSE.xmm02_1", cpu_list);
            RegObject[cpu][XMM3_hi] = SIM->get_param_num("SSE.xmm03_1", cpu_list);
            RegObject[cpu][XMM4_hi] = SIM->get_param_num("SSE.xmm04_1", cpu_list);
            RegObject[cpu][XMM5_hi] = SIM->get_param_num("SSE.xmm05_1", cpu_list);
            RegObject[cpu][XMM6_hi] = SIM->get_param_num("SSE.xmm06_1", cpu_list);
            RegObject[cpu][XMM7_hi] = SIM->get_param_num("SSE.xmm07_1", cpu_list);

#if BX_SUPPORT_X86_64
            RegObject[cpu][XMM8_Rnum] = SIM->get_param_num("SSE.xmm08_0", cpu_list);
            RegObject[cpu][XMM9_Rnum] = SIM->get_param_num("SSE.xmm09_0", cpu_list);
            RegObject[cpu][XMMA_Rnum] = SIM->get_param_num("SSE.xmm10_0", cpu_list);
            RegObject[cpu][XMMB_Rnum] = SIM->get_param_num("SSE.xmm11_0", cpu_list);
            RegObject[cpu][XMMC_Rnum] = SIM->get_param_num("SSE.xmm12_0", cpu_list);
            RegObject[cpu][XMMD_Rnum] = SIM->get_param_num("SSE.xmm13_0", cpu_list);
            RegObject[cpu][XMME_Rnum] = SIM->get_param_num("SSE.xmm14_0", cpu_list);
            RegObject[cpu][XMMF_Rnum] = SIM->get_param_num("SSE.xmm15_0", cpu_list);
            RegObject[cpu][XMM8_hi] = SIM->get_param_num("SSE.xmm08_1", cpu_list);
            RegObject[cpu][XMM9_hi] = SIM->get_param_num("SSE.xmm09_1", cpu_list);
            RegObject[cpu][XMMA_hi] = SIM->get_param_num("SSE.xmm10_1", cpu_list);
            RegObject[cpu][XMMB_hi] = SIM->get_param_num("SSE.xmm11_1", cpu_list);
            RegObject[cpu][XMMC_hi] = SIM->get_param_num("SSE.xmm12_1", cpu_list);
            RegObject[cpu][XMMD_hi] = SIM->get_param_num("SSE.xmm13_1", cpu_list);
            RegObject[cpu][XMME_hi] = SIM->get_param_num("SSE.xmm14_1", cpu_list);
            RegObject[cpu][XMMF_hi] = SIM->get_param_num("SSE.xmm15_1", cpu_list);
#endif
        }
#endif

        RegObject[cpu][DR0_Rnum] = SIM->get_param_num("DR0", cpu_list);
        RegObject[cpu][DR1_Rnum] = SIM->get_param_num("DR1", cpu_list);
        RegObject[cpu][DR2_Rnum] = SIM->get_param_num("DR2", cpu_list);
        RegObject[cpu][DR3_Rnum] = SIM->get_param_num("DR3", cpu_list);
        RegObject[cpu][DR6_Rnum] = SIM->get_param_num("DR6", cpu_list);
        RegObject[cpu][DR7_Rnum] = SIM->get_param_num("DR7", cpu_list);
    }
}


void doUpdate()
{
    Bit8u tmpbuffer[4096];

    void FillStack();
    void RefreshDataDump(Bit8u *buffer);

    if (doSimuInit != FALSE)
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
        BottomAsmLA = *AsmLA;
        int j = bottommargin;       // try to use this bottom margin on ASM window
        if (j > AsmLineCount)
            j = AsmLineCount;
        TopAsmLA = AsmLA[AsmLineCount - j];     //TopAsmLA is the scroll point
    }
    else
        doAsmScroll();      // ASM window may need to scroll
    ResizeColmns = FALSE;   // done with reformatting, if it was needed
    UpdateStatus();         // Mode and Eflags may have changed status
}

// Fill the GDT ListView, reading GDT data directly from bochs linear mem
void FillGDT()
{
    unsigned int i, j, GroupId;
    unsigned int k = (GDT_Len + 1) / 8;
    Bit8u gdtbuf[8];
    char *cols[18];
    char gdttxt[90];
    doDumpRefresh = FALSE;

    Bit64u laddr = rV[GDTRnum];
    StartListUpdate(DUMP_WND);

    *gdttxt = 0;
    cols[0]= gdttxt + 1;
    cols[1]= gdttxt + 30;
    cols[2]= gdttxt + 40;
    cols[3]= gdttxt + 80;
    cols[4]= gdttxt;    // columns #5 to 17 are blank
    cols[5]= gdttxt;
    cols[6]= gdttxt;
    cols[7]= gdttxt;
    cols[8]= gdttxt;
    cols[9]= gdttxt;
    cols[10]= gdttxt;
    cols[11]= gdttxt;
    cols[12]= gdttxt;
    cols[13]= gdttxt;
    cols[14]= gdttxt;
    cols[15]= gdttxt;
    cols[16]= gdttxt;

    for(i = 0; i < k; i++)
    {
        // read 2 dwords from bochs linear mem into "buffer"
        sprintf(cols[0], "%02u (Selector 0x%04X)", i, i << 3);
        if (ReadBxLMem(laddr, 8, gdtbuf) == FALSE)      // abort the current GDT dump on a memory error
        {
            cols[1]= gdttxt;    // ERROR - blank out cols #2 - 4 for this new row
            cols[2]= gdttxt;
            cols[3]= gdttxt;
            cols[17] = gdttxt + 30;
            strcpy (cols[17],"illegal address");
            InsertListRow(cols, 18, DUMP_WND, i, 8);    // 18 cols, "no group"
            RedrawColumns(DUMP_WND);
            EndListUpdate(DUMP_WND);
            break;
        }
        laddr += 8;

        // enforce proper littleendianness on the gdtbuf bytes
        Bit32u limit = gdtbuf[0] | ((Bit32u) gdtbuf[1] << 8);
        limit |= ((Bit32u)gdtbuf[6] & 0xf) << 16;
        if ((gdtbuf[6] & 0x80) != 0)       // 'Granularity' bit = 4K limit multiplier
            limit = limit * 4096 + 4095;   // and the bottom 12 bits aren't tested

        GroupId = 8;        // default to "blank" group
        cols[17]= (char*)GDTsT[0]; // default info string is blank
        j = 8;              // default GDT type (group) is blank
        if ((gdtbuf[5] & 0x10) == 0)    // 'S' bit clear = System segment
        {
            GroupId = 1;
            if (limit == 0 && (gdtbuf[5] & 0x80) == 0)      // 'P' (present) bit
                GroupId = 0;
            // point to the approprate info string for the GDT system segment type
            cols[17] = (char*)GDTsT[(int) (gdtbuf[5] & 0xf)];
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
            cols[17] = (char*)GDTt2[j];    // otherwise, put descriptive text in "info" column
#endif
        }

        // enforce proper littleendianness on the gdtbuf bytes
        Bit32u base = gdtbuf[2] | ((Bit32u) gdtbuf[3] << 8);
        base |= (Bit32u) gdtbuf[4] << 16;
        base |= (Bit32u) gdtbuf[7] << 24;

        if ((gdtbuf[6] & 0x60) == 0x20)     // test for longmode segment
        {
            base = 0;       // the base is always 0 in longmode, with "no" limit
            sprintf(cols[2],"0xFFFFFFFFFFFFFFFF");
        }
        else
            sprintf(cols[2],"0x%X",limit);
        sprintf(cols[1],"0x%X",base);
        sprintf(cols[3],"%u", (gdtbuf[5] & 0x60) >> 5);

        if (i == 0)
            cols[17] = (char*)GDTt2[7];    // call "Null" selector "unused"
        InsertListRow(cols, 18, DUMP_WND, i, GroupId);  // 18 cols
    }

    RedrawColumns(DUMP_WND);
    EndListUpdate(DUMP_WND);
}

// Fills the IDT ListView, reading IDT data directly from bochs linear mem
void FillIDT()
{
    Bit64u laddr;
    Bit8u idtbuf[16];
    Bit16u sel;
    Bit32u ofs;
    unsigned entrysize;
    unsigned int i;
    char *cols[18];
    char idttxt[80];
    unsigned int mode = 0;
    if (In32Mode)
        mode = 1;
    if (In64Mode)
        mode = 2;
    doDumpRefresh = FALSE;

    *idttxt = 0;
    cols[0]= idttxt + 1;
    cols[1]= idttxt;    // columns #2 to 17 are blank
    cols[2]= idttxt;
    cols[3]= idttxt;
    cols[4]= idttxt;
    cols[5]= idttxt;
    cols[6]= idttxt;
    cols[7]= idttxt;
    cols[8]= idttxt;
    cols[9]= idttxt;
    cols[10]= idttxt;
    cols[11]= idttxt;
    cols[12]= idttxt;
    cols[13]= idttxt;
    cols[14]= idttxt;
    cols[15]= idttxt;
    cols[16]= idttxt;
    cols[17]= idttxt + 10;
    entrysize = 4 << mode; // calculate the bytesize of the entries
    unsigned int k = (IDT_Len + 1) / entrysize;
    StartListUpdate(DUMP_WND);

    // recover the IDT linear base address
    laddr = rV[IDTRnum];

    if (k > 256)    // if IDT_Len is unreasonably large, set a reasonable maximum
        k = 256;
    for(i = 0; i < k; i++)
    {
        idttxt[1] = AsciiHex[2*i];
        idttxt[2] = AsciiHex[2*i+1];
        idttxt[3] = 0;
        if (ReadBxLMem(laddr, entrysize, idtbuf) == FALSE)      // abort the current IDT dump on a memory error
        {
            strcpy (cols[17],"illegal address");
            InsertListRow(cols, 18, DUMP_WND, i, 8);    // 18 cols, "no group"
            RedrawColumns(DUMP_WND);
            EndListUpdate(DUMP_WND);
            break;
        }
        laddr += entrysize;
        // enforce proper littleendianness on the idtbuf bytes
        ofs = idtbuf[0] | ((Bit32u) idtbuf[1] << 8);
        sel = idtbuf[2] | ((Bit16u) idtbuf[3] << 8);

        switch (mode)
        {
            case 0:     // Real Mode
                sprintf(cols[17],"0x%04X:0x%04X", sel, ofs);
                break;

            case 1:     // Pmode
                ofs |= ((Bit32u) idtbuf[6] << 16) | ((Bit32u) idtbuf[7] << 24);
                sprintf(cols[17],"0x%04X:0x%08X", sel, ofs);
                // TODO: also print some flags from idtbuf[5], maybe, in another column
                break;

            case 2:     // Lmode
                Bit64u off64 = (Bit64u)(ofs | ((Bit32u) idtbuf[6] << 16) | ((Bit32u) idtbuf[7] << 24));
                off64 |= ((Bit64u) idtbuf[8] << 32) | ((Bit64u) idtbuf[9] << 40);
                off64 |= ((Bit64u) idtbuf[10] << 48) | ((Bit64u) idtbuf[11] << 56);
                sprintf(cols[17],"0x%04X:0x" FMT_LLCAPX, sel, off64);
                // TODO: also print some flags from idtbuf[5], maybe, in another column
                break;
        }

        InsertListRow(cols, 18, DUMP_WND, i, 8);    // 18 cols, "no group"
    }
    RedrawColumns(DUMP_WND);
    EndListUpdate(DUMP_WND);
}


// insert one entry into the Paging data list (a linear and a physical addy)
void AddPagingLine(int LC, char *pa_lin, char *pa_phy)
{
    char zero = 0;
    char *cols[18];
    cols[0]= pa_lin;
    cols[1]= &zero; // columns #2 to 17 are blank
    cols[2]= &zero;
    cols[3]= &zero;
    cols[4]= &zero;
    cols[5]= &zero;
    cols[6]= &zero;
    cols[7]= &zero;
    cols[8]= &zero;
    cols[9]= &zero;
    cols[10]= &zero;
    cols[11]= &zero;
    cols[12]= &zero;
    cols[13]= &zero;
    cols[14]= &zero;
    cols[15]= &zero;
    cols[16]= &zero;
    cols[17]= pa_phy;
    InsertListRow(cols, 18, DUMP_WND, LC, 8);   // 18 cols, "no group"
}

// lifted from bx_dbg_dump_table in dbg_main of the internal debugger
void FillPAGE()
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
                    sprintf (pa_lin,"0x%08X - 0x%08X",start_lin, lin - 1);
                    sprintf (pa_phy,"0x" FMT_LLCAPX " - 0x" FMT_LLCAPX,
                        start_phy, start_phy + (lin-1-start_lin));
                    AddPagingLine (LineCount,pa_lin,pa_phy);
                    ++LineCount;
                }
                start_lin = lin;
                start_phy = phy;
            }
        } else {
            if(start_lin != 1) {
                sprintf (pa_lin,"0x%08X - 0x%08X",start_lin, lin - 1);
                sprintf (pa_phy,"0x" FMT_LLCAPX " - 0x" FMT_LLCAPX,
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
        sprintf (pa_lin,"0x%08X - 0x%08X", start_lin, -1);
        sprintf (pa_phy,"0x" FMT_LLCAPX " - 0x" FMT_LLCAPX,start_phy, start_phy + (lin-1-start_lin));
        AddPagingLine (LineCount,pa_lin,pa_phy);
    }
    RedrawColumns(DUMP_WND);
    EndListUpdate(DUMP_WND);
}

// build the stack display
void FillStack()
{
    // sometimes need to specially invalidate the stack window
    static bool StkInvOnce = FALSE;
    Bit64u StackLA, EndLA;
    unsigned int len, i, wordsize, overlap;
    int j;
    bool LglAddy;
    bool UpdateDisp;
    char *cp, *cpp;
    char *cols[18];
    char stktxt[120];

    *stktxt = 0;
    cols[0]= stktxt + 1;
    cols[1]= stktxt + 40;
    cols[2]= stktxt;    // columns #3 to 17 are blank
    cols[3]= stktxt;
    cols[4]= stktxt;
    cols[5]= stktxt;
    cols[6]= stktxt;
    cols[7]= stktxt;
    cols[8]= stktxt;
    cols[9]= stktxt;
    cols[10]= stktxt;
    cols[11]= stktxt;
    cols[12]= stktxt;
    cols[13]= stktxt;
    cols[14]= stktxt;
    cols[15]= stktxt;
    cols[16]= stktxt;
    cols[17]= stktxt + 80;
    doDumpRefresh = FALSE;
    StackLA = (Bit64u) BX_CPU(CurrentCPU)->get_laddr(BX_SEG_REG_SS, (bx_address) rV[RSP_Rnum]);

    if (PStackLA == 1)               // illegal value requests a full refresh
        PStackLA = StackLA ^ 0x4000; // force a non-match below (kludge)

    wordsize = 4;       // assume Pmode
    if (In32Mode == FALSE)
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
        if (ReadBxLMem(StackLA, ReadSize, (Bit8u *) cp) == FALSE)
        {
            // no data to show -- just one error message
            sprintf (cols[17],"illegal address");
            StartListUpdate(DUMP_WND);
            InsertListRow(cols, 18, DUMP_WND, 0, 8);    // 18 cols, "no group"
            EndListUpdate(DUMP_WND);
            return;
        }
        LglAddy = ReadBxLMem(StackLA + ReadSize, len + i - 0x1000, (Bit8u *) cp + ReadSize);
        if (LglAddy == FALSE)
            len = ReadSize;
    }
    else
        ReadBxLMem(StackLA, len, (Bit8u *) cp);

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
    if (UpdateDisp == FALSE)            // Don't need to update the list? (no changes?)
    {
        if (StkInvOnce == FALSE)
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
        if (In64Mode == FALSE)
        {
            int tmp;
            sprintf (cols[0],Fmt32b[1],StackLA);
            if (In32Mode == FALSE)
                tmp = *((Bit16s *) cp);
            else
                tmp = *((Bit32s *) cp);
            sprintf (cols[1],Fmt32b[UprCase],tmp);
            sprintf (cols[17],"%d",tmp);
        }
        else
        {
            Bit64s tmp = *((Bit64s *) cp);
            sprintf (cols[0],Fmt64b[UprCase],StackLA);
            sprintf (cols[1],Fmt64b[1],tmp);
            sprintf (cols[17], FMT_LL "d",tmp);
        }
        InsertListRow(cols, 18, DUMP_WND, i, 8);    // 18 cols, "no group"
        StackLA += wordsize;
        cp += wordsize;
        ++i;
    }

    RedrawColumns(DUMP_WND);
    EndListUpdate(DUMP_WND);
}

// utility function to print breakpoints in a unified way
void prtbrk (Bit32u seg, Bit64u addy, unsigned int id, bool enabled, char *cols[])
{
    int i = 0;
    if (enabled == FALSE)
        *cols[1] = 'n';
    else
        *cols[1] = 'y';
    sprintf (cols[17],"%u",id);
    if (seg <= 0xffff){
        i= 5;
        sprintf (cols[0],"%04X:",seg);
    }

    sprintf (cols[0] + i,FMT_LLCAPX,addy);
}

// Displays all Breakpoints and Watchpoints
void FillBrkp()
{
    int LineCount, totqty, i;
    char *cols[18];
    char brktxt[60];
    unsigned int brktype;
    extern bx_guard_t bx_guard;

    doDumpRefresh = FALSE;

    *brktxt = 0;
    brktxt[49] = 0;     // 0 terminate the "enabled" string
    cols[2]= brktxt;    // columns #3 to 17 are blank
    cols[3]= brktxt;
    cols[4]= brktxt;
    cols[5]= brktxt;
    cols[6]= brktxt;
    cols[7]= brktxt;
    cols[8]= brktxt;
    cols[9]= brktxt;
    cols[10]= brktxt;
    cols[11]= brktxt;
    cols[12]= brktxt;
    cols[13]= brktxt;
    cols[14]= brktxt;
    cols[15]= brktxt;
    cols[16]= brktxt;
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
        cols[0]= brktxt;
        cols[1]= brktxt;
        cols[17]= brktxt;
        InsertListRow(cols, 18, DUMP_WND, LineCount++, 8);  // make a blank row
        cols[0]= (char*)BrkName[brktype];
        InsertListRow(cols, 18, DUMP_WND, LineCount++, 8);  // brkpt "type" as only text on row
        cols[0]= brktxt + 1;
        if (brktype < 3)
        {
            cols[17]= brktxt + 50;  // only breakpoints have IDs
            cols[1]= brktxt + 48;   // and can be "enabled"
            if (brktype == 0)
            {
#if (BX_DBG_MAX_LIN_BPOINTS > 0)
                totqty = bx_guard.iaddr.num_linear;
                for (i = 0; i < totqty; i++)
                {
                    BrkpIDMap[LineCount] = bx_guard.iaddr.lin[i].bpoint_id;
                    prtbrk (0xf0000, (Bit64u) bx_guard.iaddr.lin[i].addr,
                        (unsigned) BrkpIDMap[LineCount],
                        bx_guard.iaddr.lin[i].enabled, cols);
                    InsertListRow(cols, 18, DUMP_WND, LineCount++, 8);
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
                    BrkpIDMap[LineCount] = bx_guard.iaddr.phy[i].bpoint_id;
                    prtbrk (0xf0000, (Bit64u) bx_guard.iaddr.phy[i].addr,
                        (unsigned) BrkpIDMap[LineCount],
                        bx_guard.iaddr.phy[i].enabled, cols);
                    InsertListRow(cols, 18, DUMP_WND, LineCount++, 8);
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
                    BrkpIDMap[LineCount] = bx_guard.iaddr.vir[i].bpoint_id;
                    prtbrk (bx_guard.iaddr.vir[i].cs,
                    (Bit64u) bx_guard.iaddr.vir[i].eip,
                        (unsigned) BrkpIDMap[LineCount],
                        bx_guard.iaddr.vir[i].enabled, cols);
                    InsertListRow(cols, 18, DUMP_WND, LineCount++, 8);
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
                WWP_Snapshot[i] = write_watchpoint[i].addr;
                sprintf (cols[0], FMT_PHY_ADDRX, (bx_phy_address) write_watchpoint[i].addr);
                InsertListRow(cols, 18, DUMP_WND, LineCount++, 8);
            }
        }
        else
        {
            RWP_BaseEntry = LineCount;
            totqty = num_read_watchpoints;
            RWPSnapCount = num_read_watchpoints;
            for (i = 0; i < totqty; i++)
            {
                RWP_Snapshot[i] = read_watchpoint[i].addr;
                sprintf (cols[0], FMT_PHY_ADDRX, (bx_phy_address) read_watchpoint[i].addr);
                InsertListRow(cols, 18, DUMP_WND, LineCount++, 8);
            }
        }
    }
    RedrawColumns(DUMP_WND);
    EndListUpdate(DUMP_WND);
}

// performs endian byteswapping the hard way, for a Data dump
void FillDataX(char* t, unsigned char C, bool doHex)
{
    char tmpbuf[40];
    char *d = tmpbuf;
    if (isLittleEndian == FALSE || doHex == FALSE)
        d = t + strlen(t);  // bigendian can always be appended directly
    *d = C;
    d[1] = 0;
    if (isprint(C) == 0)
        *d = '.';

    if (doHex != FALSE)
    {
        *d = AsciiHex[2*C];
        d[1] = AsciiHex[2*C + 1];
        d[2] = 0;
        if (isLittleEndian) // little endian => reverse hex digits
        {
            strcat(d,t);
            strcpy(t,d);    // so append the new bytes to the FRONT of t
        }
    }
}

void RefreshDataDump(Bit8u *buffer)
{
    bool retval = TRUE;

    // re-load 4k DataDump array from bochs emulated linear or physical memory
    if (LinearDump) {
        // cannot read linear mem across a 4K boundary -- so break the read in two
        // -- calculate location of 4K boundary (h):
        unsigned len = (int) DumpStart & 0xfff;
        unsigned i = 4096 - len;
        Bit64u h = DumpStart + i;
        retval = ReadBxLMem(DumpStart, i, buffer);
        if (retval != FALSE && len != 0)
            retval = ReadBxLMem(h, len, buffer + i);
    } else {
        retval = bx_mem.dbg_fetch_mem(BX_CPU(CurrentCPU),
            (bx_phy_address)DumpStart, 4096, buffer);
    }
}

// do the ShowData display work asynchronously, as a thread
void ShowData()
{
    unsigned int i;
    char *x;
    char *cols[18];
    char mdtxt[200];
    char tmphex[40];

    if (doDumpRefresh) {
        RefreshDataDump((Bit8u*)DataDump);
    }
    *mdtxt = 0;
    cols[0]= mdtxt + 1;     // the amount of storage needed for each column is complicated
    cols[1]= mdtxt + 20;
    cols[2]= mdtxt + 60;
    cols[3]= mdtxt + 64;
    cols[4]= mdtxt + 70;
    cols[5]= mdtxt + 74;
    cols[6]= mdtxt + 84;
    cols[7]= mdtxt + 88;
    cols[8]= mdtxt + 94;
    cols[9]= mdtxt + 100;
    cols[10]= mdtxt + 120;
    cols[11]= mdtxt + 124;
    cols[12]= mdtxt + 130;
    cols[13]= mdtxt + 134;
    cols[14]= mdtxt + 144;
    cols[15]= mdtxt + 148;
    cols[16]= mdtxt + 154;
    cols[17]= mdtxt + 160;
    doDumpRefresh = FALSE;
    StartListUpdate(DUMP_WND);

    x = DataDump;       // data dumps are ALWAYS 4K
    for(i = 0; i < 4096; i += 16)
    {
        if (In64Mode == FALSE)
            sprintf(cols[0],"0x%08X",(Bit32u) (DumpStart + i));
        else
            sprintf(cols[0],"0x" FMT_LLCAPX,DumpStart + i);

        *tmphex = 0;
        *cols[17] = 0;
        for(unsigned y = 0; y < 16; y++)
        {
            if ((DumpInAsciiMode & 1) != 0)
                // verify the char is printable, then append it to the "ascii" column
                FillDataX(cols[17],x[y],FALSE);

            if ((DumpInAsciiMode & 2) != 0)
            {
                // convert char to hex, build "endian" hex value, 2 digits at a time
                FillDataX(tmphex,x[y],TRUE);
                if (((y + 1) & (DumpAlign - 1)) == 0)
                {
                    strcpy (cols[y+2-DumpAlign], tmphex);
                    *tmphex = 0;        // FillDataX APPENDS, so you need to clear the buffer
                }
            }
        }
        InsertListRow(cols, 18, DUMP_WND, i>>4, 8); // 18 cols, list2, "no group"
        x+= 16;         // bump to the next row of data
    }

    RedrawColumns(DUMP_WND);
    EndListUpdate(DUMP_WND);
}


// build Register "display" names from lower case names
// (must build the pointer list while building the names)
void MakeRDnames()
{
    char *p = RDispName[0];       // first storage location
    for (int i=0; i <= EFER_Rnum; i++)
    {
        RDispName[i] = p;   // create the Name pointer
        const char *c = RegLCName[i];   // Register name in lower case

        if (UprCase != 0)
        {
            while (*c != 0)
                *(p++) = UCtable[(int) *(c++)]; // use lookup tbl for uppercase
        }
        else
        {
            while (*c != 0)
                *(p++) = *(c++);
        }
        *(p++) = 0;
    }
}

// generic initialization routine -- called once, only at startup
void DoAllInit()
{
    char *p;
    int i;

    doSimuInit = TRUE;
    CpuModeChange = TRUE;
    CurrentCPU = 0;     // need to init CPU info once only
    if (SingleCPU == FALSE)
        TotCPUs = BX_SMP_PROCESSORS;
    else
        TotCPUs = 1;

    // divide up the pre-allocated char buffer into smaller pieces
    p = bigbuf + outbufSIZE;    // point at the end of preallocated mem
    p -= 200;           // 200 bytes is enough for all the register names
    RDispName[0] = p;
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

    i = TOT_REG_NUM;        // fake up a color table -- there are just enough, currently
    int j = 7;      // color 7 = orange
    while (i > 0)
    {
        // change color when the loop goes below the base register number
        if (i == DR0_Rnum) --j;     // 6 Debug
        else if (i == XMM0_Rnum) --j;   // 8 or 16 XMM
        else if (i == ST0_Rnum) --j;    // 8 MMX/FPU
        else if (i == CR0_Rnum) --j;    // EFER and CR
        else if (i == GDTRnum) --j;     // Sys Registers
        else if (i == CS_Rnum) --j;     // Segments
        else if (i == EAX_Rnum) --j;    // GP Registers (32b)
        // below EAX is 64bit GP Registers and EFLAGS
        RegColor[--i] = j;
    }

    MakeXlatTables();   // create UpperCase and AsciiHex translation tables
    MakeRDnames();      // create Rnames from lower-case register names
    InitRegObjects();   // get/store all the bx_param_num_c objects for the registers
}

// refill whichever "data window" is active -- or param_tree
void RefreshDataWin()
{
    switch (DViewMode)
    {
        case VIEW_MEMDUMP:
            if (DumpInitted != FALSE)
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

// performs tasks whenever the simulation "breaks"
void OnBreak()
{
    int i = EFER_Rnum + 1;
    // check if Ptime has changed
    TakeInputFocus();

    // display the new ptime on the status bar
    char time_buf[20];
    sprintf (time_buf,"t= " FMT_LL "d", bx_pc_system.time_ticks());
    SetStatusText (2, time_buf);

    // remember register values from before the last run
    while (--i >= 0)
        PV[i] = rV[i];
    ladrmin = ladrmax;      // invalidate any old linear->phys mapping

    // then detect current CPU mode the *right* way -- look for changes
    // TODO: create param Objects for CS.d_b and cpu_mode for each CPU
    unsigned PrevCpuMode = CpuMode;
    CpuMode = BX_CPU(CurrentCPU)->get_cpu_mode();
    if (CpuMode == BX_MODE_LONG_64)
    {
        if (In64Mode == FALSE)          // Entering LongMode?
        {
            CpuModeChange = TRUE;
            In64Mode = TRUE;
            In32Mode = TRUE;            // In32Mode must be TRUE in LongMode
            ResizeColmns = TRUE;        // if so, some formatting has changed
        }
    }
    else
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
    if (doDumpRefresh != FALSE)
        RefreshDataWin();
}

static int HexFromAsk(const char* ask, char* b)       // this routine converts a user-typed hex string into binary bytes
{                   // it ignores any bigendian issues -- binary is converted front to end as chars
    int y = 0;
    int i = 0;
    for(;;)
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

static bool FindHex(const unsigned char* b1,int bs,const unsigned char* b2,int by)
{
    // search bs bytes of b1
    for(int i = 0; i < bs; i++)       // TODO: this loop could be a little more efficient.
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
        if (Match != FALSE)
            return TRUE;
    }
    return FALSE;
}

bool AskText(const char *title, const char *prompt, char *DefaultText)
{
    ask_str.title= title;
    ask_str.prompt= prompt;
    ask_str.reply= DefaultText;
    return ShowAskDialog();
}

// load new memory for a MemDump
// newDS = illegal (1) is a flag to ask the user for a DumpStart address
bool InitDataDump(bool isLinear, Bit64u newDS)
{
    bool retval = TRUE;
    bool MsgOnErr = FALSE;
    if (AtBreak == FALSE)
        return FALSE;
    if (((int) newDS & 0xf) != 0)       // legal addys must be on 16byte boundary
    {
        if (In64Mode == FALSE)
            sprintf(tmpcb,"0x%X",(Bit32u) DumpStart);
        else
            sprintf(tmpcb,"0x" FMT_LL "X",DumpStart);
        if (AskText("4K Memory Dump","4K Memory Dump -- Enter Address (use 0x for hex):",tmpcb) == FALSE)
            return FALSE;

        newDS = cvt64(tmpcb,FALSE);     // input either hex or decimal
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
        retval = ReadBxLMem(newDS,i,(Bit8u *)DataDump);
        if (retval != FALSE && len != 0)
            retval = ReadBxLMem(h,len,(Bit8u *)DataDump + i);
    }
    else {
        retval = (bool) bx_mem.dbg_fetch_mem(BX_CPU(CurrentCPU),
            (bx_phy_address)newDS, 4096, (Bit8u *)DataDump);
    }
    if (retval == FALSE)
    {
        // assume that the DataDump array is still valid -- fetch_mem should error without damage
        if (MsgOnErr != FALSE)
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
void ToggleSeeReg(int cmd)
{
    int i = cmd - CMD_EREG;
    if (i < 0 || i > 7)
        return;
    if (i == 4 || i == 5)
        ResizeColmns = TRUE;    // may need to resize the register value column

    SeeReg[i] ^= TRUE;
    SetMenuCheckmark ((int) SeeReg[i], i + CHK_CMD_EREG);
    if (AtBreak != FALSE)
        LoadRegList();  // do a register window update
}

void doNewWSize(int i)
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
        if (DViewMode == VIEW_MEMDUMP && DumpInitted != FALSE)
        {
            if (AtBreak == FALSE)
                doDumpRefresh = TRUE;
            else
                ShowData();
        }
    }
}

void ToggleGDT()
{
    if (AtBreak == FALSE)
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

void ToggleIDT()
{
    if (AtBreak == FALSE)
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

void TogglePAGE()
{
    if (AtBreak == FALSE)
        return;
    GrayMenuItem (0, CMD_WPTWR);
    GrayMenuItem (0, CMD_WPTRD);
    if (DViewMode == VIEW_PAGING || InPaging == FALSE)
        ShowMemData(FALSE);
    else
    {
        HideTree();
        DViewMode = VIEW_PAGING;    // currently displaying Paging info
        FillPAGE();
    }
}

void ToggleStack()
{
    if (AtBreak == FALSE)
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

void ToggleBrkpt()
{
    if (AtBreak == FALSE)
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

void TogglePTree()
{
    if (AtBreak == FALSE)
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

void doFind()
{
    unsigned int i, L;
    bool Select;
    char srchstr[100];
    if (AtBreak == FALSE)
        return;
    *tmpcb = 0;
        // read ASM text or MemDump data, find matches, select all matching lines
    if (DumpHasFocus == FALSE)
    {
        if (AskText("Find text in mnemonic lines","ASM Search text:",tmpcb) == FALSE)
            return;
        if (strchr(tmpcb,'*') == 0 && strchr(tmpcb,'?') == 0)
            sprintf(srchstr,"*%s*",tmpcb);
        else
            strcpy(srchstr,tmpcb);

        if (UprCase != FALSE)   // convert search string to uppercase if ASM is that way
            upr(srchstr);
        for(i = 0; i < (unsigned) AsmLineCount; i++)
        {
            GetLIText(ASM_WND, i, 2, tmpcb);       // retrieve the ASM column 2 text for row i
            Select = FALSE;
            if (IsMatching(tmpcb, srchstr, TRUE) != FALSE)
                Select = TRUE;
            SetLIState(ASM_WND, i, Select);
        }
    }
    else
    {
        if (AskText("Memory Dump Search",
            "Sequential hex bytes (e.g 00FEFA - no spaces), or ascii string (max. 16b):",tmpcb) == FALSE)
            return;

        int by = HexFromAsk(tmpcb,srchstr);     // by = len of binary search string

        // Find in all rows of 16 bytes -- must do rows, so they can be selected
        for(i = 0, L = 0; i < 4096; i += 16, L++)
        {
            Select = FALSE;
            if (by != 0 && FindHex((unsigned char *)DataDump + i,16,(unsigned char *)srchstr,by))
                Select = TRUE;
            SetLIState(DUMP_WND, L, Select);
        }

        // Try ascii for additional matches and selected lines
        Select = TRUE;          // this loop, only add selected lines to the display
        by = (int)strlen(tmpcb);
        for(i = 0, L = 0; i < 4096; i += 16, L++)
        {
            if (by != 0 && FindHex((unsigned char *)DataDump + i,16,(unsigned char *)tmpcb,by))
                SetLIState(DUMP_WND, L, Select);
        }
    }
}

void doStepN()
{
    // can't run sim until everything is ready
    if (AtBreak == FALSE || debug_cmd_ready != FALSE)
        return;
    sprintf (tmpcb,"%d",PrevStepNSize);
    if (AskText("Singlestep N times","Number of steps (use 0x for hex):",tmpcb) == FALSE)
        return;
    Bit32u i = (Bit32u) cvt64(tmpcb,FALSE);        // input either hex or decimal
    if (i == 0)
        return;
    PrevStepNSize = i;
    AtBreak = FALSE;
    StatusChange = TRUE;
    sprintf(debug_cmd, "s %d %d", CurrentCPU, i);
    debug_cmd_ready = TRUE;
}

// User wants a custom disassembly
void doDisAsm()
{
    int NumLines = DefaultAsmLines;
    if (AtBreak == FALSE)
        return;
    sprintf (tmpcb,"0x" FMT_LL "X",CurrentAsmLA);
    if (AskText("Disassemble",
        "Disassemble -- Enter Linear Start Address (use 0x for hex):",tmpcb) == FALSE)
        return;
    Bit64u h = cvt64(tmpcb,FALSE);             // input either hex or decimal
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
    BottomAsmLA = *AsmLA;
    int j = bottommargin;       // try to use this bottom margin on ASM window
    if (j > AsmLineCount)
        j = AsmLineCount;
    TopAsmLA = AsmLA[AsmLineCount - j];     //TopAsmLA is the scroll point
}

// Toggle all "selected" items as linear breakpoint on the ASM window
void SetBreak(int OneEntry)
{
    int L;
    if (AtBreak == FALSE)
        return;
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
    if (AtBreak == FALSE || SA_valid == FALSE)
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
    if (AtBreak == FALSE || L == -1 || L >= TOT_REG_NUM)
        return;

    int i = RitemToRnum[L];
    if (i > EFER_Rnum)      // TODO: extend this to more reg -- need display names for all
        return;
//  if (i > EFER_Rnum)
//      *tmpcb = 0;
//  else
        sprintf (tmpcb,"0x" FMT_LL "X", rV[i]);
    if (AskText("Change Register Value",RDispName[i],tmpcb))
    {
        Bit64u val = cvt64(tmpcb,TRUE);     // input either hex or decimal
#if BX_SUPPORT_X86_64
        if (i >= EAX_Rnum && i <= EIP_Rnum) // must use RAX-RIP when setting 32b registers
            i -= EAX_Rnum - RAX_Rnum;
#endif
        RegObject[CurrentCPU][i]->set(val); // the set function should be a bool, not a void
//      bool worked = RegObject[CurrentCPU][i]->set(val);
//      if (worked == FALSE)
//          DispMessage ("Bochs does not allow you to set that register","Selection Error");
//      else
            LoadRegList();      // update the register window
    }
}

// user wants to edit some memory
void SetMemLine(int L)
{
    // get base address of "line" of data -- each line (L) is 16 bytes
    char addrstr[64];
    Bit64u h = DumpStart + (L<<4);
    if (AtBreak == FALSE || L >= 256)
        return;

    if (LinearDump == FALSE)
        sprintf(addrstr,"Physical Address: 0x" FMT_LL "X",h);
    else
        sprintf(addrstr,"Linear Address: 0x" FMT_LL "X",h);

    unsigned char *u = (unsigned char *)(DataDump + (L<<4));    // important that it be unsigned!
    sprintf(tmpcb,"%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
        *u,u[1],u[2],u[3],u[4],u[5],u[6],u[7],u[8],u[9],u[10],u[11],u[12],u[13],u[14],u[15]);

    if (AskText("Change Memory Values",addrstr,tmpcb))
    {
        Bit8u newval;
        int err=0;
        char *x = tmpcb;
        upr(x);         // force input string to uppercase

        if (LinearDump)    // is h is a LINEAR address? Convert to physical!
        {
            // use the ReadBx function to calculate the lin->phys offset
            if (ReadBxLMem(h,0,(Bit8u *)addrstr) == FALSE) // "read" 0 bytes
                err = 2;
            else
                h -= l_p_offset;    // convert h to a physmem address
        }

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
                // convert the hex to a byte, and try to store the byte in bochs physmem
                sscanf (s,"%2X", (unsigned int*)&newval);
                if (bx_mem.dbg_set_mem(BX_CPU(CurrentCPU), (bx_phy_address) h, 1, &newval) == FALSE)
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

// Alt, Shift, Control keys are "down" if negative
// Normal return value is 0, return != 0 has OS-specific meaning.
int HotKey (int ww, int Alt, int Shift, int Control)
{
    if (Alt < 0){
        if (ww == '1')
            doNewWSize(0);
        else if (ww == '2')
            doNewWSize(1);
        else if (ww == '4')
            doNewWSize(2);
        else if (ww == '8')
            doNewWSize(3);
        else if (ww == '6')
            doNewWSize(4);
        else if (ww == VK_F2)
            TogglePAGE();
#if BX_SUPPORT_FPU
        else if (ww == VK_F3) ToggleSeeReg(CMD_FPUR);   // MMX/FPU toggle
#endif
        else if (ww == VK_F6)       // AltF6 = Read Watchpt
        {
            if (DumpHasFocus != FALSE)
                SetWatchpoint(&num_read_watchpoints,read_watchpoint);
        }
        else if (ww == VK_F7)           // Alt+F7 memdump hex toggle
        {
            int i = DumpInAsciiMode;
            i ^= 2;
            if (i != 0)
            {
                DumpInAsciiMode = i;
                i &= 2;
                SetMenuCheckmark (i, CHK_CMD_MHEX);
                GrayMenuItem (i, CMD_MASCII);
                PrevDAD = 0;        // force columns to resize
                if (DViewMode == VIEW_MEMDUMP && DumpInitted != FALSE)
                {
                    if (AtBreak == FALSE)
                        doDumpRefresh = TRUE;
                    else
                        ShowData();
                }
            }
        }
        return 0;
    }
    switch (ww)
    {
        case VK_ESCAPE:
            CommandHistoryIdx = 0;
            ClearInputWindow();
            ShowMemData(FALSE);         // force a "normal" MemDump window
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
            if (DumpInitted != FALSE)
                InitDataDump(LinearDump, DumpStart - 2048);
            break;

        case VK_NEXT:
            // Page down on the MemDump window by 2K
            if (DumpInitted != FALSE)
                InitDataDump(LinearDump, DumpStart + 2048);
            break;

        case VK_F2:
            if (Control < 0)
                ToggleGDT();
            else if (Shift < 0)
                ToggleIDT();
            else
                ToggleStack();
            break;

        case VK_F3:     // ^F3 = param tree, F3 = toggle syntax
            if (Control < 0)
                TogglePTree();
            else
            {
                bx_dbg_disassemble_switch_mode();
                if (AtBreak != FALSE)
                {
                    // do the standard ASM window fill sequence
                    Bit64u h = CurrentAsmLA;
                    CanDoLA(&h);
                    FillAsm(h, DefaultAsmLines);
                }
                else
                    BottomAsmLA = ~0;   // force an ASM autoload
            }
            break;

        case VK_F4:
            if (Shift < 0)          // Debug register toggle
                ToggleSeeReg(CMD_DREG);
            else if (Control >= 0)      // Refresh
            {
                BottomAsmLA = ~0;       // force an ASM autoload
                ResizeColmns = TRUE;    // force everything to repaint
                doDumpRefresh = TRUE;   // force a data window reload on a break
                if (AtBreak != FALSE)   // can't refresh the windows until a break!
                {
                    doUpdate();         // refresh the ASM and Register windows
                    RefreshDataWin();   // and whichever data window is up
                }
            }
            else {
                if (CpuSupportSSE)
                    ToggleSeeReg(CMD_XMMR);    // SSE toggle
            }
            break;

        case VK_F5:
            if (Shift < 0)          // ShiftF5 = Modechange brk toggle
            {
                // toggle mode_break on cpu0, use that value to reset all CPUs
                bool nmb = BX_CPU(0)->mode_break ^ TRUE;
                int j = TotCPUs;
                while (--j >= 0)
                    BX_CPU(j)->mode_break = nmb;
                SetMenuCheckmark ((int) nmb, CHK_CMD_MODEB);
            }
            else
            {
                // can't continue until everything is ready
                if (AtBreak != FALSE && debug_cmd_ready == FALSE)
                {
                    // The VGAW *MUST* be refreshed periodically -- it's best to use the timer.
                    // Which means that the sim cannot be directly run from this msglp thread.
                    *debug_cmd = 'c';   // send a fake "continue" command to the internal debugger
                    debug_cmd[1] = 0;
                    debug_cmd_ready = TRUE;
                    AtBreak = FALSE;
                    StatusChange = TRUE;
                }
            }
            break;

        case VK_F7:
            if (Control < 0)
                InitDataDump(0,(Bit64u) 1);    // ^F7 = PhysDump
            else if (Shift < 0)                // ShiftF7 = ascii toggle
            {
                int i = DumpInAsciiMode;
                i ^= 1;
                if (i != 0)
                {
                    DumpInAsciiMode = i;
                    i &= 1;
                    SetMenuCheckmark (i, CHK_CMD_MASCII);
                    GrayMenuItem (i, CMD_MHEX);
                    PrevDAD = 0;        // force columns to resize
                    if (DViewMode == VIEW_MEMDUMP && DumpInitted != FALSE)
                    {
                        if (AtBreak == FALSE)
                            doDumpRefresh = TRUE;
                        else
                            ShowData();
                    }
                }
            }
            else
                InitDataDump(1,(Bit64u) 1);     // F7 = LinDump
            break;

        case VK_F6:
            if (Control < 0)        // ^F6 = Breakpoint window
                ToggleBrkpt();
            else if (Shift < 0)     // ShiftF6 = Write Watchpt
            {
                if (DumpHasFocus == FALSE)
                    SetBreak(-1); // set or delete breakpoint(s) at the selected address(es)
                else
                    SetWatchpoint(&num_write_watchpoints,write_watchpoint);
            }
            else
            {
                if (DumpHasFocus == FALSE)      // F6 = Brkpt
                    SetBreak(-1); // set or delete breakpoint(s) at the selected address(es)
                else
                    SetWatchpoint(&num_write_watchpoints,write_watchpoint);
            }
            break;

        case VK_F8:
                // can't continue until everything is ready
            if (AtBreak != FALSE && debug_cmd_ready == FALSE)
            {
                *debug_cmd = 'p';   // send a fake "proceed" command to the internal debugger
                debug_cmd[1] = 0;
                debug_cmd_ready = TRUE;
                AtBreak = FALSE;
                StatusChange = TRUE;
            }
            break;

        case VK_F11:
            if (AtBreak != FALSE && debug_cmd_ready == FALSE)
            {
                bx_dbg_stepN_command(CurrentCPU, 1);        // singlestep
                StatusChange = TRUE;
                OnBreak();
            }
            break;

        case VK_F9:
            doStepN();      // ask user for a step #
            break;

        case 'C':           // ^c = break
            if (Control < 0)
            {
                SIM->debug_break();
            }
            break;

        case 'D':
            if (Control < 0)
                doDisAsm();
            break;

        case 'F':
            if (Control < 0)
                doFind();
            break;

        case VK_RIGHT:  // Win32: send a few virtual movement keys back into the Input window
        case VK_LEFT:
        case VK_END:
        case VK_HOME:
        case VK_DELETE:
            return -1;

        case VK_RETURN:
            // can't run a command until everything is ready
            if (AtBreak != FALSE && debug_cmd_ready == FALSE)
            {
                *tmpcb = 0;
                GetInputEntry(tmpcb);
                StatusChange = TRUE;
                if (*tmpcb == 0)            // Hitting <CR> on a blank line means SINGLESTEP
                {
                    bx_dbg_stepN_command(CurrentCPU, 1);        // singlestep
                    OnBreak();
                }
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
                    strcpy (debug_cmd,tmpcb);   // send the command into the bochs internal debugger
                    debug_cmd_ready = TRUE;
                    AtBreak = FALSE;
                    ClearInputWindow();         // prepare for the next command
                    CommandHistoryIdx = 0;      // and reset the history queue to the new end
                }
            }
    }       // end the switch
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
            if (AtBreak != FALSE && debug_cmd_ready == FALSE)
            {
                // The VGAW *MUST* be refreshed periodically -- it's best to use the timer.
                // Which means that the sim cannot be directly run from this msglp thread.
                *debug_cmd = 'c';   // send a fake "continue" command to the internal debugger
                debug_cmd[1] = 0;
                debug_cmd_ready = TRUE;
                AtBreak = FALSE;
                StatusChange = TRUE;
            }
            break;

        case CMD_STEP1: // step 1
            if (AtBreak != FALSE && debug_cmd_ready == FALSE)
            {
                bx_dbg_stepN_command(CurrentCPU, 1);        // singlestep
                StatusChange = TRUE;
                OnBreak();
            }
            break;

        case CMD_STEPN: // step N
            doStepN();
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
            if (AtBreak != FALSE)   // can't refresh the windows until a break!
            {
                doUpdate();         // refresh the ASM and Register windows
                RefreshDataWin();   // and whichever data window is up
            }
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
            if (AtBreak == FALSE)
                break;
            SingleCPU ^= TRUE;
            TotCPUs = 1;
            if (SingleCPU == FALSE)
                TotCPUs = BX_SMP_PROCESSORS;
            SetMenuCheckmark ((int) SingleCPU, CHK_CMD_ONECPU);
            VSizeChange();
            break;

        case CMD_DADEF: // set default # of disassembly lines in a list
            if (AtBreak == FALSE)
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
            if (AtBreak != FALSE)
            {
                // do the standard ASM window fill sequence
                Bit64u h = CurrentAsmLA;
                CanDoLA(&h);
                FillAsm(h, DefaultAsmLines);
            }
            else
                BottomAsmLA = ~0;       // force an ASM autoload
            break;

        case CMD_IOWIN:  // toggle display of internal debugger Input and Output windows
            if (AtBreak == FALSE)
                break;
            ShowIOWindows ^= TRUE;
            SetMenuCheckmark ((int) ShowIOWindows, CHK_CMD_IOWIN);
            VSizeChange();
            break;

        case CMD_SBTN:      // Toggle showing top pushbutton-row
            if (AtBreak == FALSE)
                break;
            ShowButtons ^= TRUE;
            SetMenuCheckmark ((int) ShowButtons, CHK_CMD_SBTN);
            VSizeChange();
            break;

        case CMD_UCASE:     // Toggle showing everything in uppercase
            UprCase ^= 1;
            SetMenuCheckmark ((int) UprCase, CHK_CMD_UCASE);
            MakeRDnames();
            if (AtBreak != FALSE)
            {
                LoadRegList();
                // do the standard ASM window fill sequence
                Bit64u h = CurrentAsmLA;
                CanDoLA(&h);
                FillAsm(h, DefaultAsmLines);
            }
            else
                BottomAsmLA = ~0;       // force an ASM autoload
            break;

        case CMD_MHEX:      // Toggle showing hex in Dump window
            i = DumpInAsciiMode;
            i ^= 2;
            if (i != 0)
            {
                DumpInAsciiMode = i;
                i &= 2;
                SetMenuCheckmark (i, CHK_CMD_MHEX);
                GrayMenuItem (i, CMD_MASCII);
                PrevDAD = 0;        // force columns to resize
                if (DViewMode == VIEW_MEMDUMP && DumpInitted != FALSE)
                {
                    if (AtBreak == FALSE)
                        doDumpRefresh = TRUE;
                    else
                        ShowData();
                }
            }
            break;

        case CMD_MASCII:    // Toggle showing ASCII in Dump window
            i = DumpInAsciiMode;
            i ^= 1;
            if (i != 0)
            {
                DumpInAsciiMode = i;
                i &= 1;
                SetMenuCheckmark (i, CHK_CMD_MASCII);
                GrayMenuItem (i, CMD_MHEX);
                PrevDAD = 0;        // force columns to resize
                if (DViewMode == VIEW_MEMDUMP && DumpInitted != FALSE)
                {
                    if (AtBreak == FALSE)
                        doDumpRefresh = TRUE;
                    else
                        ShowData();
                }
            }
            break;

        case CMD_LEND:      // Toggle Endianness for the MemDumps
            isLittleEndian ^= TRUE;
            SetMenuCheckmark ((int) isLittleEndian, CHK_CMD_LEND);
            if (DViewMode == VIEW_MEMDUMP && DumpInitted != FALSE)
            {
                if (AtBreak == FALSE)
                    doDumpRefresh = TRUE;
                else
                    ShowData();
            }
            break;

        case CMD_WS_1:      // set memory dump "wordsize"
            // "Align" = "wordsize" -- from 1 to 16
            doNewWSize(0);
            break;

        case CMD_WS_2:
            doNewWSize(1);
            break;

        case CMD_WS_4:
            doNewWSize(2);
            break;

        case CMD_WS_8:
            doNewWSize(3);
            break;

        case CMD_WS16:
            doNewWSize(4);
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
            if (AtBreak != FALSE)
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

BxEvent *enh_dbg_notify_callback(void *unused, BxEvent *event)
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

static void get_bxshare_path(char *path)
{
  const char *varptr = NULL;

#if BX_HAVE_GETENV
  varptr = getenv("BXSHARE");
#endif
  if (varptr != NULL) {
    sprintf(path, "%s", varptr);
  } else {
    varptr = get_builtin_variable("BXSHARE");
    if (varptr != NULL) {
      sprintf(path, "%s", varptr);
    } else {
      strcpy(path, ".");
    }
  }
}

void ReadSettings()
{
  FILE *fd = NULL;
  char line[512], path[BX_PATHNAME_LEN];
  char *ret, *param, *val;
  bool format_checked = 0;
  size_t len1 = 0, len2;
  int i;

  if (global_ini) {
    get_bxshare_path(path);
    sprintf(ini_path, "%s/bx_enh_dbg.ini", path);
    fd = fopen(ini_path, "r");
  }
  if (fd == NULL) {
    fd = fopen("bx_enh_dbg.ini", "r");
    if (fd == NULL) return;
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
          UprCase = atoi(val);
        } else if (!strcmp(param, "DumpInAsciiMode")) {
          DumpInAsciiMode = atoi(val);
        } else if (!strcmp(param, "isLittleEndian")) {
          isLittleEndian = !strcmp(val, "TRUE");
        } else if (!strcmp(param, "DefaultAsmLines")) {
          DefaultAsmLines = atoi(val);
        } else if (!strcmp(param, "DumpWSIndex")) {
          DumpWSIndex = atoi(val);
          DumpAlign = (1 << DumpWSIndex);
          PrevDAD = 0;
        } else if (!strcmp(param, "DockOrder")) {
          DockOrder = (short)strtoul(val, NULL, 16);
        } else if ((len1 == 15) && !strncmp(param, "ListWidthPix[", 13) && (param[14] == ']')) {
          if ((param[13] < '0') || (param[13] > '2')) {
            fprintf(stderr, "bx_enh_dbg.ini: invalid index for option SeeReg[x]\n");
            continue;
          }
          i = atoi(&param[13]);
          ListWidthPix[i] = atoi(val);
        } else if (!ParseOSSettings(param, val)) {
          fprintf(stderr, "bx_enh_dbg.ini: unknown option '%s'\n", line);
        }
      }
    }
  } while (!feof(fd));
  fclose(fd);
}

void WriteSettings()
{
  FILE *fd = NULL;
  int i;

  if (global_ini) {
    fd = fopen(ini_path, "w");
  } else {
    fd = fopen("bx_enh_dbg.ini", "w");
  }
  if (fd == NULL) return;
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

#endif
