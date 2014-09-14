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

#include "config.h"

#if BX_DEBUGGER && BX_DEBUGGER_GUI

#include "bochs.h"
#include "win32dialog.h"
#include "enh_dbg.h"

// Important Note! All the string manipulation functions assume one byte chars -- ie. "ascii",
// instead of "wide" chars. If there exists a compiler that automatically assumes wide chars
// (ie. 2 byte), then all the function names in here need to be changed to FORCE the compiler
// to use the "A" functions instead of the "W" functions. That is, strcpyA(), sprintfA(), etc.
// This will require setting macros for the non-os-specific code, too.

#include <commctrl.h>
#include <commdlg.h>

#ifdef _WIN64
#define DWL_MSGRESULT DWLP_MSGRESULT
#endif

// User Customizable initial settings:

// the Register color table
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

// END of User Customizable settings

COLORREF AsmColors[4] = {
    RGB(0,0,0),         // text foreground color is normally black
    RGB(0,100,0),       // current opcode is dark green and bold
    RGB(150,0,0),       // a breakpoint is red and italic
    RGB(0,0,200)        // both = blue, bold, AND italic
};

#ifndef LVIF_GROUPID
#define IS_WIN98
// this define may not exist in some compilers
#define LVIF_GROUPID    0
#endif

#ifndef IS_WIN98
// workaround broken code if IS_WIN98 not defined
#define IS_WIN98
#endif

// The wordsize popup is the 15th entry in the Opt menu -- COUNTING SEPARATORS
// Index = (entry number - 1) -- if the Opt menu is modified, then update this value
// -- or else the popup checkmarks won't work
#define WS_POPUP_IDX    14

void DockResize (int j, Bit32u x);      // need some function prototypes
void SetHorzLimits(void);
void ParseIDText(const char *x);
void ShowData();
void UpdateStatus();
void doUpdate();
void RefreshDataWin();
void OnBreak();
void ParseBkpt();
void SetBreak(int i);
void ChangeReg();
int HotKey (int ww, int Alt, int Shift, int Control);
void ActivateMenuItem (int LW);
void SetMemLine (int L);
void MakeBL(HTREEITEM *h_P, bx_param_c *p);

static unsigned LstTop = 0;
static Bit32u CurTimeStamp = 0;    // last mousedown time

// Handles to Windows and global stuff
HWND hY = NULL;     // complete parent window
HWND hL[3];         // 0=registers, 1=Asm, 2=MemDump
HWND hE_I;          // command input window
HWND hS_S;          // "status" window at bottom
HWND hE_O;          // debugger text output
HWND hT;            // param_tree window
HWND hBTN[5];       // button row
HWND hCPUt[BX_MAX_SMP_THREADS_SUPPORTED];   // "tabs" for the individual CPUs
HFONT CustomFont[4];
HFONT DefFont;
LOGFONT mylf;
HMENU hOptMenu;     // "Options" popup menu (needed to set check marks)
HMENU hViewMenu;    // "View" popup menu (needed to gray entries)
HMENU hCmdMenu;     // "Command" popup menu (needed to gray entries)
// one "defualtProc" for each edit window (Input and Output)
WNDPROC wEdit[2];
WNDPROC wBtn;       // all the buttons have the same Proc
WNDPROC wTreeView;
WNDPROC wListView;  // all the lists use the same Proc

//HANDLE hTCevent[BX_MAX_SMP_THREADS_SUPPORTED];    // Start/Sleep Control for cpu_loop threads

// window resizing/docking variables
unsigned CurXSize = 0;      // last known size of main client window
unsigned CurYSize = 0;
HCURSOR hCursResize;
HCURSOR hCursDock;
HCURSOR hCursArrow;

int SelectedEntry;          // keeps track of "clicks" on list entries

// base window "styles" for the 3 listviews.
long LVStyle[3] = {
    LVS_REPORT | WS_CHILD,
    LVS_SHOWSELALWAYS | LVS_REPORT | WS_CHILD,
    LVS_SHOWSELALWAYS | LVS_REPORT | WS_CHILD | WS_VISIBLE
};

TVINSERTSTRUCT tvis;    // tree-view generic item

Bit32u SelectedBID;

#define BTN_BASE            1024
#define MULTICPU_BTN_BASE   1030

bx_bool UpdInProgress[3];       // flag -- list update incomplete (not OK to paint)

INT_PTR CALLBACK A_DP(HWND hh,UINT mm,WPARAM ww,LPARAM ll)
{
    switch(mm)
    {
        case WM_INITDIALOG:
            SetWindowText(hh,ask_str.title);
            SetWindowText(GetDlgItem(hh,101),ask_str.prompt);
            SetWindowText(GetDlgItem(hh,102),ask_str.reply);
            return TRUE;

        case WM_COMMAND:
            if (LOWORD(ww) == IDOK)
            {
                GetWindowText(GetDlgItem(hh,102),ask_str.reply,100);
                EndDialog(hh,IDOK);
                return 0;
            }
            if (LOWORD(ww) == IDCANCEL)
            {
                EndDialog(hh,IDCANCEL);
                return 0;
            }
    }
    return 0;
}

bx_bool ShowAskDialog()
{
    bx_bool ret = FALSE;
    // The dialog box needs a caret, and will destroy the one in hE_I.
    // So destroy it myself, cleanly, and let it be recreated cleanly.
    CallWindowProc(*wEdit, hE_I,WM_KILLFOCUS,(WPARAM) 0,0);
    if (DialogBoxParam(GetModuleHandle(0),"DIALOG_AT",hY,A_DP,(LPARAM) 0) == IDOK)
        ret = TRUE;
    // recreate the caret in the Input window -- KILL and SET focus only work in pairs!
    CallWindowProc(*wEdit, hE_I,WM_SETFOCUS,(WPARAM) 0,0);
    return ret;
}

// Subclass the edit controls
LRESULT CALLBACK ThisEditProc(HWND hh, UINT mm, WPARAM ww, LPARAM ll)
{
    int i = 0;
    if (hh == hE_O) i = 1;  // easy way to detect which edit window
    switch (mm)
    {
        case WM_MOUSEMOVE:
            // turn off any sizing operation if the cursor strays off the listviews
            Sizing = 0;
            // also turn off any half-completed mouseclicks
            xClick = -1;
            break;
        case WM_KILLFOCUS:
        case WM_SETFOCUS:   // force the Input window to always have the caret
            return 0;   // SETFOCUS/KILLFOCUS commands should be sent directly to the hE_I defProc

        case WM_CHAR:
        case WM_SYSCHAR:
            // throw away all Enter keys (bell problems)
            if (ww == VK_RETURN)
                return 0;
                // force all typed input to the Input window (wEdit[0] = Input)
            CallWindowProc(*wEdit, hE_I, mm, ww, ll);
            return 0;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            // simply let parent know about all keydowns in these edit boxes
            SendMessage(GetParent(hh),mm,ww,ll);
            // the following keys cause annoying "bells" if sent on to the Original proc
            // -- so throw them away (return 0)
            if (ww == VK_RETURN || ww == VK_DOWN || ww == VK_UP)
                return 0;
    }
    return (CallWindowProc(wEdit[i], hh, mm, ww, ll));
}

// Subclass all the button controls together
LRESULT CALLBACK BtnProc(HWND hh, UINT mm, WPARAM ww, LPARAM ll)
{
    switch (mm)
    {
        case WM_MOUSEMOVE:
            // turn off any sizing operation if the cursor strays off the listviews
            Sizing = 0;
            // also turn off any half-completed mouseclicks
            xClick = -1;
            break;
        case WM_CHAR:
            // throw away any Enter keys (potential bell problems?)
            if (ww == VK_RETURN)
                return 0;
                // force any typed input to the Input window
            CallWindowProc((WNDPROC) *wEdit, hE_I, mm, ww, ll);
            return 0;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            // pass to parent any keys
            SendMessage(GetParent(hh),mm,ww,ll);
            if (ww == ' ')      // space chars cause buttons to activate
                return 0;
    }
    return (CallWindowProc(wBtn, hh, mm, ww, ll));
}

// the subclassed tree-view uses the same handler that the listview does
// subclass all the listviews at once
// Note: the Sizing varaible keeps track of whether the program is in resize or dock mode, and
// which windows are involved. It's complicated. If you don't like how I use the variable, tough.
LRESULT CALLBACK LVProc(HWND hh, UINT mm, WPARAM ww, LPARAM ll)
{
    int i;
    POINT pt;
    pt.y = HIWORD(ll);
    pt.x = LOWORD(ll);      // get coordinates of the mouse (assume a mouse message)

    // some compilers generate buggy code if this 'if' is put inside the switch!
    if (mm == WM_PAINT)
    {
            i = 2;
            if (hh == hL[ASM_WND]) i=1;
            else if (hh == hL[REG_WND]) i=0;
            if (UpdInProgress[i] != FALSE)  // if list is not complete, don't paint it
            {
                PAINTSTRUCT ps;
                BeginPaint (hh, &ps);
                EndPaint (hh, &ps);
                return 0;
            }
    }

    switch (mm)
    {
        case WM_CHAR:
            // throw away all Enter keys (or use them for something)
            if (ww != VK_RETURN && *wEdit != NULL)
            // force all typed input to the Input window
                CallWindowProc(*wEdit, hE_I, mm, ww, ll);
            return 0;

        case WM_MOUSEMOVE:      // need to play with the cursor during resize/dock operations
            // mouse position is in listview client's coordinates
            if (xClick >= 0)    // saw a lbutton mousedown recently? (pre-dock mode)
            {
                // cancel an impending "click" and go to dock (drag) mode if mouse is moving too much
                i= (OneCharWide >> 1);
                if (pt.x > xClick + i || pt.x < xClick - i)
                    xClick = -1;
                else if (pt.y > yClick + i || pt.y < yClick - i)
                    xClick = -1;
                // go into dock mode (without canceling the click) on a .5s time delay
                i = CurTimeStamp + 500;
                if (xClick < 0 || GetMessageTime() > i)     // Start a "dock" operation?
                    Sizing = SizeList;      // Sizing tells which ListView is being moved

                // if "drag" did not turn off, then I will get only one mousemove, with VK_LBUTTON showing UP
                i = GetKeyState(VK_LBUTTON);
                if (i >= 0 && Sizing != 0)  // this could be EITHER a dock or resize
                {
                    int j = 2;
                    if (hh == hL[REG_WND])
                        j = 0;          // j = ListView destination index
                    else if (hh == hL[ASM_WND])
                        j = 1;

                    ClientToScreen(hh,&pt);
                    ScreenToClient(hY,&pt);     // convert to parent's coordinates
                    DockResize (j, (Bit32u)pt.x);
                    Sizing = 0;
                }
            }
            if (Sizing != 0)
            {
                ClientToScreen(hh,&pt);
                ScreenToClient(hY,&pt);     // convert to parent's coordinates
                i = GetKeyState(VK_LBUTTON);
                // verify horizontal limits of the current resize or dock operation
                if (pt.x > Resize_HiX || pt.x < Resize_LoX)
                    Sizing = 0;
                else if (i >= 0 && Sizing > 0)
                {
                    int j = DUMP_WND;
                    if (hh == hL[REG_WND])
                        j = REG_WND;            // j = ListView destination index
                    else if (hh == hL[ASM_WND])
                        j = ASM_WND;
                    DockResize (j, (Bit32u)pt.x);
                    Sizing = 0;
                }
                else if (Sizing > 5)
                    SetCursor(hCursDock);
                else
                    SetCursor(hCursResize);
            }
            break;

        case WM_NCMOUSEMOVE:            // more cursor games
            ScreenToClient(hY,&pt);     // convert to parent's coordinates
            if (Sizing == 0)
            {
                if (ww == HTBORDER)     // mouse is hovering over a resize bar or border
                {
                    if (pt.x < BarClix[0] + 10) // is it the left or right bar?
                    {
                        Sizing = -2;
                        Resize_LoX = BarClix[0] - OneCharWide;  // set horizontal limits
                        Resize_HiX = BarClix[0] + OneCharWide;
                    }
                    else if (pt.x > BarClix[1] - 10)
                    {
                        Sizing = -1;
                        Resize_LoX = BarClix[1] - OneCharWide;  // set horizontal limits
                        Resize_HiX = BarClix[1] + OneCharWide;
                    }
                }
            }
            else if (Sizing >= 10)      // continuing docking operation?
                SetCursor(hCursDock);
            else
            {
                // verify horizontal limits of the current resize operation
                if (pt.x > Resize_HiX || pt.x < Resize_LoX)
                    Sizing = 0;
                else
                    SetCursor(hCursResize);
            }
            break;

        case WM_NCLBUTTONDOWN:          // NC messages are in Screen coordinates
            ScreenToClient(hY,&pt);     // convert to parent's coordinates
            if (Sizing < 0)             // doing a presize? -- enter full resize mode
            {
                SetHorzLimits();
                SetCursor(hCursResize);
                return 0;               // important to eat the mousedown (for scrollbars)
            }
            break;

        case WM_NCLBUTTONUP:
            i = Sizing;
            Sizing = 0;     // a mouseup on a border can't be interpreted -- cancel everything
            xClick = -1;
            if (i > 0)
                return 0;       // eat the mouseup if a sizing op is ending (for scrollbars)

        case WM_LBUTTONDOWN:
            if (Sizing < 0)     // doing a presize?
            {
                SetHorzLimits();
                SetCursor(hCursResize);
            }
            else        // set "pre-dock" mode
            {
                if (hh == hL[REG_WND])
                    SizeList = 10;      // remember which list to dock
                else if (hh == hL[ASM_WND])
                    SizeList = 11;
                else
                    SizeList = 12;
                Resize_LoX = OneCharWide << 2;  // set horizontal limits
                i = ListWidthPix[0] + ListWidthPix[1] + ListWidthPix[2];
                Resize_HiX = i - (OneCharWide << 2);
                CurTimeStamp = GetMessageTime();
                xClick = pt.x;
                yClick = pt.y;
            }
            // prevent default drag operations by returning 0
            // unfortunately, this also prevents clicks from happening automatically
            return 0;

        case WM_LBUTTONUP:
            // the mouseup COMPLETES a "click" or drag
            if (Sizing > 0)
            {
                int j = DUMP_WND;
                if (hh == hL[REG_WND])
                    j = REG_WND;            // j = ListView destination index
                else if (hh == hL[ASM_WND])
                    j = ASM_WND;
                GetCursorPos(&pt);          // Screen coordinates
                ScreenToClient(hY,&pt);     // convert to parent's coordinates
                DockResize (j, (Bit32u) pt.x);
                Sizing = 0;
            }
            else if (xClick >= 0)
            {
                // verify the mouseup was within a tight circle of the mousedown
                i= (OneCharWide >> 1) +1;
                if (pt.x < xClick + i && pt.x > xClick - i && pt.y < yClick + i && pt.y > yClick - i)
                {
                    LV_ITEM lvi;
                    lvi.stateMask = LVIS_SELECTED;
                    lvi.state = 0;

                    // emulate a regular click, for Trees or Lists
                    if (hh == hT)
                    {
                        TV_HITTESTINFO TreeHT;
                        TreeHT.pt.x = pt.x;     // find out which tree button got clicked
                        TreeHT.pt.y = pt.y;
                        HTREEITEM hTreeEnt;
                        hTreeEnt = (HTREEITEM) CallWindowProc(wTreeView,hT,TVM_HITTEST,(WPARAM) 0,(LPARAM) &TreeHT);
                        // then simulate a click on that button
                        if (hTreeEnt != NULL)       // make sure the click is actually on an entry
                            CallWindowProc(wTreeView,hT,TVM_EXPAND,(WPARAM) TVE_TOGGLE,(LPARAM) hTreeEnt);
                        xClick = -1;
                        Sizing = 0;
                        return 0;           // eat all the Tree mouseups
                    }
                    i = REG_WND;            // deal with List Focus
                    DumpHasFocus = FALSE;
                    if (hh == hL[ASM_WND])
                        i = ASM_WND;
                    else if (hh != hL[REG_WND])
                    {
                        DumpHasFocus = TRUE;
                        i = DUMP_WND;
                        EnableMenuItem (hCmdMenu, CMD_BRKPT, MF_GRAYED);
                        if (LinearDump == FALSE)
                        {
                            EnableMenuItem (hCmdMenu, CMD_WPTWR, MF_ENABLED);
                            EnableMenuItem (hCmdMenu, CMD_WPTRD, MF_ENABLED);
                        }
                        else
                        {
                            EnableMenuItem (hCmdMenu, CMD_WPTWR, MF_GRAYED);
                            EnableMenuItem (hCmdMenu, CMD_WPTRD, MF_GRAYED);
                        }
                    }
                    if (DumpHasFocus == FALSE)
                    {
                        EnableMenuItem (hCmdMenu, CMD_BRKPT, MF_ENABLED);
                        EnableMenuItem (hCmdMenu, CMD_WPTWR, MF_GRAYED);
                        EnableMenuItem (hCmdMenu, CMD_WPTRD, MF_GRAYED);
                    }
                    if (GetFocus() != hh)
                    {
                        CallWindowProc(wListView, hh, WM_SETFOCUS, (WPARAM) 0, (LPARAM) 0);
                        if (i != REG_WND)
                            CallWindowProc(wListView, hL[REG_WND], WM_KILLFOCUS, (WPARAM) 0, (LPARAM) 0);
                        if (i != ASM_WND)
                            CallWindowProc(wListView, hL[ASM_WND], WM_KILLFOCUS, (WPARAM) 0, (LPARAM) 0);
                        if (i != DUMP_WND)
                            CallWindowProc(wListView, hL[DUMP_WND], WM_KILLFOCUS, (WPARAM) 0, (LPARAM) 0);
                    }

                    LVHITTESTINFO lvht;         // do a hit test to get the iItem to select
                    lvht.pt.y = HIWORD(ll);
                    lvht.pt.x = LOWORD(ll);     // make sure to do any subitem test on the RIGHT point!
                    SelectedEntry = CallWindowProc(wListView, hh, LVM_SUBITEMHITTEST, (WPARAM) 0, (LPARAM) &lvht);
                    // SelectedEntry is a copy of the iItem number -- ignore anything <= 0
                    int Control = GetKeyState(VK_CONTROL);  // key is down if val is negative
                    if (Control >= 0)
                    {
                        // if no control key, deselect anyything that's selected
                        int j = CallWindowProc(wListView, hh, LVM_GETNEXTITEM, (WPARAM) -1, MAKELPARAM(LVNI_SELECTED, 0));
                        while (j >= 0)
                        {
                            // cancel the item's "selection"
                            CallWindowProc(wListView, hh, LVM_SETITEMSTATE, (WPARAM)j, (LPARAM)&lvi);
                            // and get the next one
                            j = CallWindowProc(wListView, hh, LVM_GETNEXTITEM, (WPARAM) -1, MAKELPARAM(LVNI_SELECTED, 0));
                        }
                    }
                    if (SelectedEntry >= 0)     // then select the one that got clicked
                    {
                        lvi.state = LVIS_SELECTED;
                        CallWindowProc(wListView, hh, LVM_SETITEMSTATE, (WPARAM)SelectedEntry, (LPARAM)&lvi);
                        if (hh == hL[DUMP_WND] && DViewMode == VIEW_MEMDUMP && LinearDump == FALSE)
                        {
                            // if this was a PhysDump click -- need to get the "Selected Address"
                            SA_valid = TRUE;
                            SelectedDataAddress = DumpStart + (SelectedEntry<<4);
                            // Clicked a Subitem? column numbers = byte offsets + 1
                            if (lvht.iSubItem > 0)
                                SelectedDataAddress += lvht.iSubItem - 1;
                        }
                        else if (hh == hL[DUMP_WND] && DViewMode == VIEW_BREAK)
                        {
                            // breakpoint or watchpoint click -- figure out WHICH breakpoint!
                            // SelectedBID is the breakpoint ID, if a breakpoint was clicked
                            if (SelectedEntry < WWP_BaseEntry && SelectedEntry < 256)
                            {
                                i = 0;
                                if (SelectedEntry > EndPhyEntry)
                                    i = 0x10000;                    // bit flag for Virtual Brk
                                else if (SelectedEntry > EndLinEntry)
                                    i = 0x20000;                    // bit flag for Phy Brkpt
                                SelectedBID = BrkpIDMap[SelectedEntry] | i;
                            }
                            else if (SelectedEntry >= WWP_BaseEntry && SelectedEntry < WWP_BaseEntry +  WWPSnapCount)
                            {
                                // if a watchpoint was clicked, get its index (0 to 15)
                                // and save the address for later verification
                                i = SelectedEntry - WWP_BaseEntry;
                                SelectedBID = 0x40000 | i;
                            }
                            else if (SelectedEntry >= RWP_BaseEntry && SelectedEntry < RWP_BaseEntry + RWPSnapCount)
                            {
                                i = SelectedEntry - RWP_BaseEntry;
                                SelectedBID = 0x80000 | i;
                            }
                        }
                    }
                }
            }
            xClick = -1;    // all drags/clicks have been processed, so reset
            Sizing = 0;
            return 0;       // eat all the mouseups
    }                   // end the switch
    if (hh == hT)       // if this is a param_tree message, use the proper windowProc
        return (CallWindowProc(wTreeView, hh, mm, ww, ll));
    else
        return (CallWindowProc(wListView, hh, mm, ww, ll));
}


bx_bool SpListView()        // Superclasses a ListView control
{
    WNDCLASS wClass;
    GetClassInfo(GetModuleHandle(0), WC_LISTVIEW, &wClass);
    wListView = wClass.lpfnWndProc;
    wClass.hInstance = GetModuleHandle(0);
    wClass.lpszClassName = "sLV";
    wClass.lpfnWndProc = LVProc;
    if (RegisterClass(&wClass) == 0)
        return FALSE;
    return TRUE;
}

bx_bool SpBtn()             // Superclasses a button control
{
    WNDCLASS wClass;
    GetClassInfo(GetModuleHandle(0), "button", &wClass);
    wBtn = wClass.lpfnWndProc;
    wClass.hInstance = GetModuleHandle(0);
    wClass.lpszClassName = "sBtn";
    wClass.lpfnWndProc = BtnProc;
    if (RegisterClass(&wClass) == 0)
        return FALSE;
    return TRUE;
}

// set all TRUE flags to checked in the Options menu, gray out unsupported features
void SpecialInit()
{
    int i = 8;
    doSimuInit = FALSE;
    EnableMenuItem (hOptMenu, CMD_EREG, MF_GRAYED);
    EnableMenuItem (hCmdMenu, CMD_WPTWR, MF_GRAYED);
    EnableMenuItem (hCmdMenu, CMD_WPTRD, MF_GRAYED);
    EnableMenuItem (hOptMenu, CMD_XCEPT, MF_GRAYED);
    // 32bit registers are always displayed until longmode is active
    while (--i >= 0)
    {
        if (SeeReg[i] != FALSE)
            CheckMenuItem (hOptMenu, i + CMD_EREG, MF_CHECKED);
    }
    if (SingleCPU != FALSE)
        CheckMenuItem (hOptMenu, CMD_ONECPU, MF_CHECKED);
    if (BX_SMP_PROCESSORS == 1)
        EnableMenuItem (hOptMenu, CMD_ONECPU, MF_GRAYED);
    if (ShowButtons != FALSE)
        CheckMenuItem (hOptMenu, CMD_SBTN, MF_CHECKED);
    if (UprCase != 0)
        CheckMenuItem (hOptMenu, CMD_UCASE, MF_CHECKED);
    if (isLittleEndian != FALSE)
        CheckMenuItem (hOptMenu, CMD_LEND, MF_CHECKED);
    if (ignSSDisasm != FALSE)
        CheckMenuItem (hOptMenu, CMD_IGNSA, MF_CHECKED);
    if (ignoreNxtT != FALSE)
        CheckMenuItem (hOptMenu, CMD_IGNNT, MF_CHECKED);
    if (SeeRegColors != FALSE)
        CheckMenuItem (hOptMenu, CMD_RCLR, MF_CHECKED);
    if (DumpInAsciiMode == 0)       // prevent an illegal value
        DumpInAsciiMode = 3;
    if ((DumpInAsciiMode & 2) != 0)
        CheckMenuItem (hOptMenu, CMD_MHEX, MF_CHECKED);
    else        // one or the other MemDump mode must always be left on! (unchangeable)
        EnableMenuItem (hOptMenu, CMD_MASCII, MF_GRAYED);
    if ((DumpInAsciiMode & 1) != 0)
        CheckMenuItem (hOptMenu, CMD_MASCII, MF_CHECKED);
    else
        EnableMenuItem (hOptMenu, CMD_MHEX, MF_GRAYED);
    if (ShowIOWindows != FALSE)
        CheckMenuItem (hOptMenu, CMD_IOWIN, MF_CHECKED);

    HMENU wsMenu = GetSubMenu (hOptMenu, WS_POPUP_IDX);
    CheckMenuItem (wsMenu, CMD_WS_1+DumpWSIndex, MF_CHECKED);
    EnableMenuItem (hOptMenu, CMD_TREG, MF_GRAYED);     // not currently supported by bochs
    EnableMenuItem (hViewMenu, CMD_PAGEV, MF_GRAYED);
#if BX_SUPPORT_FPU == 0
    EnableMenuItem (hOptMenu, CMD_FPUR, MF_GRAYED);
#endif

    if (! CpuSupportSSE)
      EnableMenuItem (hOptMenu, CMD_XMMR, MF_GRAYED);
}

// append a whole row of text into a ListView, all at once
// Note: LineCount should start at 0
void InsertListRow(char *ColumnText[], int ColumnCount, int listnum, int LineCount, int grouping)
{
    LV_ITEM lvi = {LVIF_TEXT,LineCount,0,0,0,(LPSTR) *ColumnText,80,0,0};

//#ifndef IS_WIN98
//  lvi.iGroupId = grouping;
//#endif

    // insert data for the first column
    CallWindowProc(wListView,hL[listnum],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lvi);

    // loop over, and insert data for all additional columns
    lvi.mask = LVIF_TEXT;
    int i = 0;
    while (++i < ColumnCount)
    {
        lvi.iSubItem = i;
        lvi.pszText = ColumnText[i];
        CallWindowProc(wListView,hL[listnum],LVM_SETITEMTEXT,(WPARAM) LineCount,(LPARAM) &lvi);
    }
}

void SetSizeIOS(int WinSizeX, int WinSizeY)
{
    int i, j;
    TEXTMETRIC tm;
    // calculate the height/width of typical "glyphs" in the font
    HDC hdc = GetDC (hS_S);
    GetTextMetrics (hdc, &tm);
    ReleaseDC (hS_S, hdc);
    OneCharWide = tm.tmAveCharWidth;
    j= tm.tmAveCharWidth * 72;  // status bar contains about 72 char positions (without eflags)
    if (j > WinSizeX/2)         // leave half the status bar for the eflags
        j = WinSizeX/2;
    i = j*5 / 12;           // predefined proportions of Status "subwindows" = 2:5:5
    int modepos = j/6;
    int ptimepos = modepos + i;
    int eflpos = ptimepos + i;
    int p[4] = {modepos,ptimepos,eflpos,-1};

    // then set the dimensions of the I, O, and S child windows
    j = tm.tmHeight + tm.tmExternalLeading; // input window is the height of a char
    i = j >> 1;
    int sY = j + i;     // status window and buttons are  50% taller
    LstTop = 0;
    if (ShowButtons != FALSE)
    {
        // position the 5 command buttons
        LstTop = sY;
        i = WinSizeX / 5;
        SetWindowPos(hBTN[0],0,0,0,i,LstTop,SWP_SHOWWINDOW);
        SetWindowPos(hBTN[1],0,i,0,i,LstTop,SWP_SHOWWINDOW);
        SetWindowPos(hBTN[2],0,i*2,0,i,LstTop,SWP_SHOWWINDOW);
        SetWindowPos(hBTN[3],0,i*3,0,i,LstTop,SWP_SHOWWINDOW);
        SetWindowPos(hBTN[4],0,i*4,0,WinSizeX - 4*i,LstTop,SWP_SHOWWINDOW);
    }
    else
    {
        ShowWindow(hBTN[0],SW_HIDE);
        ShowWindow(hBTN[1],SW_HIDE);
        ShowWindow(hBTN[2],SW_HIDE);
        ShowWindow(hBTN[3],SW_HIDE);
        ShowWindow(hBTN[4],SW_HIDE);
    }
    if (TotCPUs > 1)
    {
        // MultiCPU simulation -- need CPU button row, too.
        int HorPos = 0;
        unsigned int CPUn = 0;
        i = WinSizeX / TotCPUs;
        while (CPUn < TotCPUs - 1)
        {
            SetWindowPos(hCPUt[CPUn],0,HorPos,LstTop,i,sY,SWP_SHOWWINDOW);
            ++CPUn;
            HorPos += i;
        }
        // use up any extra pixels on the last button
        SetWindowPos(hCPUt[CPUn],0,HorPos,LstTop,WinSizeX-HorPos,sY,SWP_SHOWWINDOW);
        LstTop += sY;
    }
    else if (BX_SMP_PROCESSORS > 1)     // only showing CPU0 on a SMP simulation
    {
        i = BX_SMP_PROCESSORS;          // so hide all the other CPU buttons
        while (--i >= 0)
            ShowWindow(hCPUt[i],SW_HIDE);
    }
    // The status win may not use all of its space: overlap lists or Input by STATUS_WIN_OVERLAP pixels
    i = WinSizeY - sY - j;
    if (ShowIOWindows == FALSE)
    {
        // caret functions will be ignored if sent multiple times (which is what is desired)
        CallWindowProc(*wEdit, hE_I,WM_KILLFOCUS,(WPARAM) 0,0); // destroy Input caret
        ShowWindow(hE_I,SW_HIDE);
        ShowWindow(hE_O,SW_HIDE);
        ListVerticalPix = WinSizeY - sY - LstTop + STATUS_WIN_OVERLAP;
    }
    else
    {
        CallWindowProc(*wEdit, hE_I,WM_SETFOCUS,(WPARAM) 0,0);  // create Input caret
        ListVerticalPix = (WinSizeY * 3) >> 2;      // 3/4 height
        SetWindowPos(hE_I,0,2,i + STATUS_WIN_OVERLAP,WinSizeX,j,SWP_SHOWWINDOW);
        SetWindowPos(hE_O,0,0,ListVerticalPix + LstTop,WinSizeX,
            i - ListVerticalPix - LstTop + (STATUS_WIN_OVERLAP/2),SWP_SHOWWINDOW);
    }
    SetWindowPos(hS_S,0,0,WinSizeY - sY,WinSizeX,sY,SWP_SHOWWINDOW);
    SendMessage(hS_S,SB_SETPARTS,4,(LPARAM)p);
}

void MoveLists()
{
    int i, j, k;
    unsigned int StrtCol[3] = {0,0,0};
    // use DockOrder to figure out the start columns of each list
    // starting column of 2nd list = width of first list
    i = ListWidthPix[(DockOrder >> 8) -1];
    j = ((DockOrder>>4)& 3) -1;     // index of 2nd list
    StrtCol[j] = i;
    BarClix[0] = i;
    k = i + ListWidthPix[j];        // width of first + second
    StrtCol[(DockOrder & 3) -1] = k;
    BarClix[1] = k;
    // the center listview must be the only one with a border
    if (CurCenterList != j)
    {
        // remove the border on the previous center list, and put it on the new one
        SetWindowLong ( hL[CurCenterList], GWL_STYLE, LVStyle[CurCenterList]);
        SetWindowLong ( hL[j], GWL_STYLE, LVStyle[j] | WS_BORDER );
        CurCenterList = j;
        // the tree window should have the same border as the data window --
        // but unfortunately, SetWindowLong does not work properly on a treeview!
//      if (j == 2)
//          k=SetWindowLong ( hT, GWL_STYLE, TVS_DISABLEDRAGDROP | TVS_HASLINES | TVS_HASBUTTONS | WS_CHILD | WS_BORDER );
//      else
//          k=SetWindowLong ( hT, GWL_STYLE, TVS_DISABLEDRAGDROP | TVS_HASLINES | TVS_HASBUTTONS | WS_CHILD );
    }

    // On at least some versions of Win, you cannot mess with the positions
    // of subclassed ListView controls that still HAVE DATA IN THEM.
    // Therefore, delete all ListView data --
    //  then refresh the 3 main ListView windows afterward

    CallWindowProc(wListView,hL[DUMP_WND],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
    CallWindowProc(wListView,hL[ASM_WND],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
    CallWindowProc(wListView,hL[REG_WND],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);

    SetWindowPos(hL[REG_WND],0,StrtCol[0],LstTop,ListWidthPix[0],ListVerticalPix,SWP_HIDEWINDOW);
    SetWindowPos(hL[ASM_WND],0,StrtCol[1],LstTop,ListWidthPix[1],ListVerticalPix,SWP_HIDEWINDOW);
    SetWindowPos(hL[DUMP_WND],0,StrtCol[2],LstTop,ListWidthPix[2],ListVerticalPix,SWP_HIDEWINDOW);
    // tree window = same size/position as dump window
    SetWindowPos(hT,0,StrtCol[2],LstTop,ListWidthPix[2],ListVerticalPix,SWP_HIDEWINDOW);

    // size the last column of each list = total width of that list
    CallWindowProc(wListView, hL[REG_WND], LVM_SETCOLUMNWIDTH, 2, ListWidthPix[0]);
    CallWindowProc(wListView, hL[ASM_WND], LVM_SETCOLUMNWIDTH, 2, ListWidthPix[1]);
    CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, 17, ListWidthPix[2]);
    BottomAsmLA = ~0;       // force an ASM autoload next time, to resize it
    doDumpRefresh = TRUE;   // force a data window refresh on a break
    if (AtBreak != FALSE)   // can't refresh the windows until a break!
    {
        doUpdate();         // refresh the ASM and Register windows
        RefreshDataWin();   // and whichever data window is up
    }
}

// redraw/recalculate everything if the vertical window sizes are invalid
void VSizeChange()
{
    RECT rc;
    GetClientRect(hY,&rc);
    SetSizeIOS((int)rc.right,(int)rc.bottom);   // window sizes are font-dependent
    MoveLists();
}

// kludgy function to remove column header/autosizing from Fill routines
void RedrawColumns(int listnum)
{
    static bx_bool DumpCResize = FALSE; // flag to force column resize/autosize
    static int PrevDV = -1;             // type of previous Dump window that was displayed
    // ResizeColmns says whether font or 64bit mode has changed,
    // but the Dump windows need to "remember" seeing the flag
    if (ResizeColmns != FALSE)
        DumpCResize = TRUE;
    if (listnum == REG_WND)
    {
        if (ResizeColmns != FALSE)
            CallWindowProc(wListView, hL[REG_WND], LVM_SETCOLUMNWIDTH, 1, LVSCW_AUTOSIZE);
    }
    else if (listnum == ASM_WND)
    {
        // recalculate # of list items per page for all list windows
        AsmPgSize = CallWindowProc(wListView,hL[ASM_WND],LVM_GETCOUNTPERPAGE,(WPARAM) 0,(LPARAM) 0);
        if (AsmPgSize != 0)
            ListLineRatio = ListVerticalPix / AsmPgSize;
        CallWindowProc(wListView, hL[ASM_WND], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
    }
    else
    {
        char tmpc[32];
        int i = 17;
        // MemDump resizes/uses ALL columns -- there ARE no unused ones
        if (DViewMode != VIEW_MEMDUMP && PrevDV != DViewMode)
        {
            // force the widths of unused columns to 0, by squishing ALL the columns to 0 width
            while (--i > 0)
                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, i, 0);
        }

        LV_COLUMN lvgc = {LVCF_TEXT,0,0,tmpc};
        switch (DViewMode)
        {
            case VIEW_MEMDUMP:
                if (DumpInitted == FALSE)
                {
                    CallWindowProc(wListView,hL[DUMP_WND],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
                    strcpy (tmpc,"Address");            // "generic" header for col 0
                    CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
                    while (--i >= 0)    // (i always equals 17)
                        CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, i, LVSCW_AUTOSIZE_USEHEADER);
                }
                else
                {
                    if (DumpCResize != FALSE || PrevDV != DViewMode)
                        CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
                    if (PrevDAD != DumpAlign || PrevDV != DViewMode)
                    {
                        PrevDAD = DumpAlign;
                        for(i = 1 ; i < 17 ; i++)
                        {
                            // if dumping ONLY in ascii, 0 out ALL the other columns
                            if (((i - 1) & (DumpAlign - 1)) == 0 && (DumpInAsciiMode & 2) != 0)
                                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, i, LVSCW_AUTOSIZE);
                            else            // force the width of unused columns to 0
                                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, i, 0);
                        }
                    }
                }
                break;

            case VIEW_GDT:
                // 2 of the GDT columns are too narrow with a regular Autosize
                if (PrevDV == DViewMode && DumpCResize == FALSE)
                    return;
                strcpy (tmpc,"Index");          // header for col 0
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
                strcpy (tmpc,"Base Address");           // header for col 1
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 1,(LPARAM) &lvgc);
                strcpy (tmpc,"Size");           // header for col 2
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 2,(LPARAM) &lvgc);
                strcpy (tmpc,"DPL");            // header for col 3
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 3,(LPARAM) &lvgc);
                strcpy (tmpc,"Info");           // header for col 4 (17)
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 17,(LPARAM) &lvgc);
                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, 1, LVSCW_AUTOSIZE_USEHEADER);
                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, 3, LVSCW_AUTOSIZE_USEHEADER);
                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, 2, LVSCW_AUTOSIZE);
                break;

            case VIEW_IDT:
                if (PrevDV == DViewMode && DumpCResize == FALSE)
                    return;
                strcpy (tmpc,"Interrupt");          // header for col 0
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
                strcpy (tmpc,"L.Address");          // header for col 1 (17)
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 17,(LPARAM) &lvgc);
                // redo col0 to handle the wide header
                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE_USEHEADER);
                break;

            case VIEW_PAGING:
                if (PrevDV == DViewMode && DumpCResize == FALSE)
                    return;
                strcpy (tmpc,"L.Address");              // header for col 0
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
                strcpy (tmpc,"is mapped to P.Address"); // header for col 1 (17)
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 17,(LPARAM) &lvgc);
                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
                break;

            case VIEW_STACK:
                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, 1, LVSCW_AUTOSIZE);
                if (PrevDV == DViewMode && DumpCResize == FALSE)
                    return;
                strcpy (tmpc,"L.Address");              // header for col 0
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
                strcpy (tmpc,"Value");  // header for col 1
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 1,(LPARAM) &lvgc);
                strcpy (tmpc,"(dec.)"); // header for col 2 (17)
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 17,(LPARAM) &lvgc);
                break;

            case VIEW_BREAK:
                if (PrevDV == DViewMode && DumpCResize == FALSE)
                    return;
                strcpy (tmpc,"Address");                // header for col 0
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
                strcpy (tmpc,"Enabled");    // header for col 1
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 1,(LPARAM) &lvgc);
                strcpy (tmpc,"ID"); // header for col 2 (17)
                CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 17,(LPARAM) &lvgc);
                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, 1, LVSCW_AUTOSIZE_USEHEADER);
        }
        DumpCResize = FALSE;
        PrevDV = DViewMode;
    }
}

// hide/clear param_tree window when showing a different MemDump window (GDT, IDT, etc.)
void HideTree()
{
    ShowWindow(hT,SW_HIDE);
    CallWindowProc(wTreeView,hT,TVM_DELETEITEM,(WPARAM) 0,(LPARAM) TVI_ROOT);
}

void ShowFW()
{
    FWflag = TRUE;
    MessageBox(hY,"With more than 1000 lines in the ASM window, be careful not to minimize the app.\r\nIt will be very slow to reopen.",
        "Friendly warning:",MB_ICONINFORMATION);
}

void Invalidate(int i)
{
    InvalidateRect(hL[i],0,TRUE);
}

void SetStatusText(int column, const char *buf)
{
    SendMessage(hS_S,SB_SETTEXT, column,(LPARAM) buf);
}

void GetLIText(int listnum, int itemnum, int column, char *buf)
{
    LV_ITEM lvi = {LVIF_TEXT,itemnum,column,0,0,(LPSTR) buf,100,0,0};
    CallWindowProc(wListView, hL[listnum], LVM_GETITEMTEXT,(WPARAM) itemnum,(LPARAM) &lvi);
}

void TakeInputFocus()
{
    SetFocus(hY);               // get the focus back from the VGA window
}

void GetInputEntry(char *buf)
{
    GetWindowText(hE_I,buf,200);
}

void SetLIState(int listnum, int itemnum, bx_bool Select)
{
    // assume selected
    LV_ITEM lvi = {LVIF_STATE,itemnum,2,LVIS_SELECTED,LVIS_SELECTED,(LPSTR) 0,0,0,0};
    if (Select == FALSE)
        lvi.state = 0;
    CallWindowProc(wListView,hL[listnum],LVM_SETITEMSTATE,(WPARAM) itemnum,(LPARAM) &lvi);
}

int GetNextSelectedLI(int listnum, int StartPt)
{
    int L = (int) CallWindowProc(wListView, hL[listnum], LVM_GETNEXTITEM,
        (WPARAM) StartPt, MAKELPARAM(LVNI_SELECTED, 0));
    return L;
}

int GetASMTopIdx()
{
    int i = (int) CallWindowProc(wListView,hL[ASM_WND],LVM_GETTOPINDEX,(WPARAM) 0,(LPARAM) 0);
    return i;
}

void ScrollASM(int pixels)
{
    CallWindowProc(wListView,hL[ASM_WND],LVM_SCROLL,(WPARAM) 0,(LPARAM) pixels);
}

void ToggleWSchecks(int newWS, int oldWS)
{
    // the wordsize popup is the WS_POPUP_IDXth entry in the Opt menu -- COUNTING SEPARATORS
    HMENU wsMenu = GetSubMenu (hOptMenu, WS_POPUP_IDX);
    CheckMenuItem (wsMenu, oldWS, MF_UNCHECKED | MF_BYPOSITION);
    CheckMenuItem (wsMenu, newWS, MF_CHECKED | MF_BYPOSITION);
}

void MakeListsGray()
{
    COLORREF c = RGB(255,255,255);
    if (AtBreak == FALSE)
    {
        SetStatusText(0, "Running");
        c = RGB(210,210,210);       // emulation running -> gui inactive
    }
    else
        SetStatusText(0, "Break");

    // this sets the background color OUTSIDE the area of the list, for A, D, R
    CallWindowProc(wListView, hL[DUMP_WND],LVM_SETBKCOLOR,0,c);
    CallWindowProc(wListView, hL[ASM_WND],LVM_SETBKCOLOR,0,c);
    CallWindowProc(wListView, hL[REG_WND],LVM_SETBKCOLOR,0,c);

    InvalidateRect(hL[REG_WND],0,TRUE);     // Invalidate all 3 of the ListViews --
    InvalidateRect(hL[ASM_WND],0,TRUE);     // to force a redraw with the new background
    InvalidateRect(hL[DUMP_WND],0,TRUE);
}

// the following routine needs to replace the entire window's text,
// and then scroll the window to the end of the text
void SetOutWinTxt()
{
    SetWindowText(hE_O,OutWindow);  // replace all text with modified buffer
//  SendMessage(hE_O,EM_SETSEL,0,-1);   // invisibly selects all, sets caret to end
    CallWindowProc(wEdit[1], hE_O,EM_SETSEL,OutWinSIZE,OutWinSIZE+1);   // sets caret to end only
    CallWindowProc(wEdit[1], hE_O,EM_SCROLLCARET,0,0);  // scrolls window vertically to caret
    // Note: the caret is never actually visible in the Output window
}

// as it stands, the caret is placed at the beginning of each selected history line,
// but the exact functionality is not important -- just do what is easy
void SelectHistory(int UpDown)
{
    // the History buffer is circular, so wrap 64 to 0, and -1 to 63
    CommandHistoryIdx = (CommandHistoryIdx + UpDown) & 63;
    SetWindowText(hE_I,CmdHistory[(CmdHInsert + CommandHistoryIdx) & 63]);
    CallWindowProc(*wEdit, hE_I, EM_SETSEL,(WPARAM) -1,(LPARAM) -1);
}

void ClearInputWindow()
{
    SetWindowText(hE_I,"");
}

void SetMenuCheckmark (int flag, int CmdIndex)
{
    // only the OptMenu has checkmarks, so don't need to "calculate" the hMenu
    if (flag != 0)
        CheckMenuItem (hOptMenu, CmdIndex, MF_CHECKED);
    else
        CheckMenuItem (hOptMenu, CmdIndex, MF_UNCHECKED);
}

// do a quickie job of converting a menu Command Index into its "parent" menu handle
// -- this doesn't handle the Help menu (nothing there should ever need to be grayed or checked)
HMENU MenuFromIdx (int CmdIndex)
{
    if (CmdIndex < MI_FIRST_VIEWITEM)
        return hCmdMenu;
    if (CmdIndex < MI_FIRST_OPTITEM)
        return hViewMenu;
//  if (CmdIndex >= CMD_WS_1 && CmdIndex <= CMD_WS16)   HIHI currently the submenu DOES NOT HAVE a global HMENU
//      return h???Menu;
    return hOptMenu;
}

// pass in FALSE or 0 to gray out a menu item
void GrayMenuItem (int flag, int CmdIndex)
{
    HMENU hm = MenuFromIdx (CmdIndex);
    if (flag != 0)
        EnableMenuItem (hm, CmdIndex, MF_ENABLED);
    else
        EnableMenuItem (hm, CmdIndex, MF_GRAYED);
}

// kludgy thing to remove OS-dependent code from the front of Fill routines
void StartListUpdate(int listnum)
{
//  ShowWindow(hL[listnum],SW_HIDE);        // Hiding the windows causes way too much flicker
    UpdInProgress[listnum] = TRUE;
    CallWindowProc(wListView,hL[listnum],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
    // then set a flag that list[listnum] is being updated -- prevent paint messages, cpu_loop calls, duplicate calls
}

void EndListUpdate(int listnum)
{
    UpdInProgress[listnum] = FALSE;     // It's OK to paint the listview now
    Invalidate(listnum);                // -- but the window needs one last paint message
    ShowWindow(hL[listnum],SW_SHOW);
}

void DispMessage(const char *msg, const char *title)
{
    MessageBox (hY, msg, title, MB_OK);
}

// exit GDT/IDT/Paging/Stack/Tree -- back to the MemDump window
void ShowMemData(bx_bool initting)
{
    int i = 1;
    if (LinearDump == FALSE)
        i = 0;
    LV_COLUMN lvgc = {LVCF_TEXT,0,0,(LPSTR)DC0txt[i]};
    if (initting != FALSE)
    {
        GrayMenuItem (0, CMD_WPTWR);
        GrayMenuItem (0, CMD_WPTRD);
        // update column 0 to say whether its physmem or linear
        CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
    }
    lvgc.pszText = tmpcb;
    HideTree();
    ShowWindow(hL[DUMP_WND],SW_SHOW);
    DViewMode = VIEW_MEMDUMP;       // returning to MemDump mode
    // need to fix the MemDump column "names"
    strcpy (tmpcb,"0");                 // rebuild header for col 1
    CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 1,(LPARAM) &lvgc);
    // headers for cols 1 to 3, are just numbers 0 to 2
    *tmpcb = '1';
    CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 2,(LPARAM) &lvgc);
    *tmpcb = '2';
    CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 3,(LPARAM) &lvgc);
    strcpy (tmpcb,"Ascii");                 // rebuild header for col 17
    CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 17,(LPARAM) &lvgc);
    if (DumpInitted == FALSE)
        RedrawColumns(DUMP_WND);
    else
    {
        strcpy (tmpcb,DC0txt[i]);   // "real" header for col 0
        CallWindowProc(wListView,hL[DUMP_WND],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
        PrevDAD = 0;            // force a column resize HIHI getting rid of this line?
        if (AtBreak == FALSE)
            doDumpRefresh = TRUE;
        else
            ShowData();         // repopulates & resizes all columns
    }
}


// create the top layer of a parameter tree TreeView
void FillPTree()
{
    HTREEITEM h_PTroot;
    extern bx_list_c *root_param;
    // Note: don't multithread this display -- the user expects it to complete
    doDumpRefresh = FALSE;
    ShowWindow(hL[DUMP_WND],SW_HIDE);
    TreeView_DeleteAllItems(hT);
    // Note: all tree strings are hardcoded to be in tmpcb -- don't change!
    strcpy (tmpcb, "bochs parameter tree");
    tvis.hParent = NULL;        // create the root node
    tvis.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_STATE;
    tvis.hInsertAfter = (HTREEITEM)TVI_FIRST;
    tvis.item.pszText = tmpcb;
    tvis.item.cchTextMax = 250;
    tvis.item.cChildren = 1;
    tvis.item.state = TVIS_EXPANDED;        // it must be expanded to show the first layer
    tvis.item.stateMask = TVIS_EXPANDED;
    h_PTroot = TreeView_InsertItem(hT,&tvis);
    tvis.item.mask = TVIF_TEXT | TVIF_CHILDREN; // don't expand any other layers
    int i = root_param->get_size();
    while (--i >= 0)
        MakeBL(&h_PTroot, root_param->get(i));
    ShowWindow(hT,SW_SHOW);
}

void MakeTreeChild (HTREEITEM *h_P, int ChildCount, HTREEITEM *h_TC)
{
    tvis.item.cChildren = 0;
    if (ChildCount > 0)
        tvis.item.cChildren = 1;
    tvis.hParent = *h_P;        // create leaves/recurse branches
    *h_TC = (HTREEITEM) CallWindowProc(wTreeView,hT,TVM_INSERTITEM,(WPARAM) 0,(LPARAM) &tvis);
}

bx_bool NewFont()
{
    LOGFONT lf;
    if (AtBreak == FALSE)
        return FALSE;
    GetObject(DefFont,sizeof(lf),&lf);
    CHOOSEFONT cF = {0};
    cF.lStructSize = sizeof(cF);
    cF.hwndOwner = hY;
    cF.lpLogFont = &lf;
    cF.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;
    if (ChooseFont(&cF) == FALSE)
        return FALSE;

    ShowWindow(hL[REG_WND],SW_HIDE);        // font is changed before MoveLists is called
    ShowWindow(hL[ASM_WND],SW_HIDE);
    ShowWindow(hL[DUMP_WND],SW_HIDE);
    ShowWindow(hS_S,SW_HIDE);
    ShowWindow(hE_I,SW_HIDE);
    if (*CustomFont != DefFont)             // destroy all variations of the prev. font
        DeleteObject (*CustomFont);
    DeleteObject (CustomFont[1]);
    DeleteObject (CustomFont[2]);
    DeleteObject (CustomFont[3]);
    *CustomFont = CreateFontIndirect(&lf);
    // create a bold version of the deffont
    lf.lfWeight = FW_BOLD;
    CustomFont[1] = CreateFontIndirect (&lf);
    // create a bold + italic version of the deffont
    lf.lfItalic = 1;
    CustomFont[3] = CreateFontIndirect (&lf);
    // create an italic version of the deffont (turn off bold)
    lf.lfWeight = FW_NORMAL;
    CustomFont[2] = CreateFontIndirect (&lf);

    CallWindowProc(wListView,hL[REG_WND],WM_SETFONT,(WPARAM)*CustomFont,MAKELPARAM(TRUE,0));
    CallWindowProc(wListView,hL[ASM_WND],WM_SETFONT,(WPARAM)*CustomFont,MAKELPARAM(TRUE,0));
    CallWindowProc(wListView,hL[DUMP_WND],WM_SETFONT,(WPARAM)*CustomFont,MAKELPARAM(TRUE,0));
    CallWindowProc(*wEdit,hE_I,WM_SETFONT,(WPARAM)*CustomFont,MAKELPARAM(TRUE,0));
    SendMessage(hS_S,WM_SETFONT,(WPARAM)*CustomFont,MAKELPARAM(TRUE,0));
    return TRUE;
}

// Main Window Proc for our Dialog
LRESULT CALLBACK B_WP(HWND hh,UINT mm,WPARAM ww,LPARAM ll)
{
    unsigned i;
    extern bx_bool vgaw_refresh;

    switch(mm)
    {
        case WM_CREATE:
        {
            HFONT hF = (HFONT)GetStockObject(OEM_FIXED_FONT);
            DefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

            // Register Window
            char* txt0[] = {"Reg Name","Hex Value","Decimal"};
            LV_COLUMN lvc = {LVCF_SUBITEM | LVCF_TEXT,LVCFMT_LEFT,0,txt0[0]};
            hL[REG_WND] = CreateWindowEx(0,"sLV","",LVStyle[0],0,0,100,100,hh,(HMENU)1001,GetModuleHandle(0),0);
            // Note; WM_CREATE only happens once, so don't bother eliminating these SendMessage macros
            ListView_InsertColumn(hL[REG_WND],0,&lvc);
            lvc.pszText = txt0[1];
            ListView_InsertColumn(hL[REG_WND],1,&lvc);
            lvc.pszText = txt0[2];
            ListView_InsertColumn(hL[REG_WND],2,&lvc);

            // Enable the groupID's for the register window
#ifndef IS_WIN98
            // GroupID's are only supported on XP or higher -- verify Win Version
            // the group stuff may COMPILE correctly, but still may fail at runtime
            Bit8u MajWinVer, MinWinVer;
            Bit32u PackedVer = GetVersion();
            bx_bool Group_OK = TRUE;
            MajWinVer = (Bit8u)(PackedVer & 0xff);      // Major version # is in the LOW byte
            MinWinVer = (Bit8u)((PackedVer>>8) & 0xff);
            if (MajWinVer > 5 || (MajWinVer == 5 && MinWinVer >= 1))     // is it XP or higher?
            {
                wchar_t* txt[] = {L"General Purpose",L"Segment",L"Control",L"MMX",L"SSE",L"Debug",L"Test",L"Other"};
                ListView_EnableGroupView(hL[REG_WND],TRUE);
                for(i = 0 ; i < 8 ; i++)
                {
                    LVGROUP group1 = {0};
                    group1.cbSize = sizeof(LVGROUP);
                    group1.mask = LVGF_HEADER | LVGF_GROUPID;
                    group1.pszHeader = txt[i];
                    group1.iGroupId = i;
                    ListView_InsertGroup(hL[REG_WND], -1, &group1);
                }
            }
            else
                Group_OK = FALSE;
#endif
            SendMessage(hL[REG_WND], LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

            // Asm Window
            hL[ASM_WND] = CreateWindowEx(0,"sLV","",LVStyle[1] | WS_BORDER,0,0,1,1,hh,(HMENU)1000,GetModuleHandle(0),0);
            CurCenterList = 1;          // ASM window starts with the border
            char* txt3[] = {"L.Address","Bytes","Mnemonic"};

            lvc.pszText = txt3[0];
            ListView_InsertColumn(hL[ASM_WND],0,&lvc);
            lvc.pszText = txt3[1];
            ListView_InsertColumn(hL[ASM_WND],1,&lvc);
            lvc.pszText = txt3[2];
            ListView_InsertColumn(hL[ASM_WND],2,&lvc);
//          SendMessage(hL[ASM_WND], LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,
//              LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

            // Input / Output
            hE_I = CreateWindowEx(0,"edit","",WS_CHILD | WS_VISIBLE,0,0,1,1,hh,(HMENU)1004,GetModuleHandle(0),0);
            // without AUTOHSCROLL, output window text is always supposed to wordwrap
            hE_O = CreateWindowEx(0,"edit","",WS_VSCROLL | WS_BORDER | WS_CHILD | WS_VISIBLE | ES_MULTILINE,0,0,1,1,hh,(HMENU)1003,GetModuleHandle(0),0);
            SendMessage(hE_I,WM_SETFONT,(WPARAM)DefFont, MAKELPARAM(TRUE,0));
            SendMessage(hE_O,WM_SETFONT,(WPARAM)hF,MAKELPARAM(TRUE,0));

            // subclass both the edit windows together
            *wEdit = (WNDPROC)SetWindowLongPtr(hE_I, GWLP_WNDPROC, (LONG_PTR)ThisEditProc);
            wEdit[1] = (WNDPROC)SetWindowLongPtr(hE_O, GWLP_WNDPROC, (LONG_PTR)ThisEditProc);

            // Status
            hS_S = CreateWindowEx(0,STATUSCLASSNAME,"",WS_CHILD | WS_VISIBLE,0,0,1,1,hh,(HMENU)1006,GetModuleHandle(0),0);
            SendMessage(hS_S,WM_SETFONT,(WPARAM)DefFont,MAKELPARAM(TRUE,0));
            int p[4] = {100,250,400,-1};
            SendMessage(hS_S,SB_SETPARTS,4,(LPARAM)p);

            // Dump Window
            hL[DUMP_WND] = CreateWindowEx(0,"sLV","",LVStyle[2],0,0,100,100,hh,(HMENU)1002,GetModuleHandle(0),0);
            //SendMessage(hL[DUMP_WND],WM_SETFONT,(WPARAM)DefFont,MAKELPARAM(TRUE,0));
            strcpy (bigbuf, "Address");
            lvc.pszText = bigbuf;
            ListView_InsertColumn(hL[DUMP_WND],0,&lvc);
            bigbuf[1] = 0;                  // use bigbuf to create 1 byte "strings"
            for(i = 1 ; i < 17 ; i++)
            {
                if (i < 11)
                    *bigbuf = i - 1 + '0';
                else
                    *bigbuf = i - 11 + 'A';
                ListView_InsertColumn(hL[DUMP_WND],i,&lvc);
            }
            strcpy (bigbuf, "Ascii");
            ListView_InsertColumn(hL[DUMP_WND],17,&lvc);
//          SendMessage(hL[DUMP_WND], LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER, HIHI
//              LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

#ifndef IS_WIN98
            if (Group_OK)
            {
                wchar_t* txt5[] = {L"16-bit code",L"64-bit code",L"32-bit code",L"16-bit data",L"64-bit data",L"32-bit data",L"Illegal"};
                ListView_EnableGroupView(hL[DUMP_WND],TRUE);
                LVGROUP group1 = {0};
                group1.cbSize = sizeof(LVGROUP);
                group1.mask = LVGF_HEADER | LVGF_GROUPID;
                for(int i = 0 ; i < 9 ; i++)
                {
                    group1.pszHeader = txt5[i];
                    group1.iGroupId = i;
                    ListView_InsertGroup(hL[DUMP_WND], -1, &group1);
                }
            }
#endif
            hT = CreateWindowEx(0,WC_TREEVIEW,"",TVS_DISABLEDRAGDROP | TVS_HASLINES | TVS_HASBUTTONS | WS_CHILD | WS_BORDER,
                0,0,1,1,hh,(HMENU)1010,GetModuleHandle(0),0);
            //SendMessage(hT,WM_SETFONT,(WPARAM)DefFont,MAKELPARAM(TRUE,0));
            // Use the same messagehandler for the tree window as for ListViews
            wTreeView = (WNDPROC)SetWindowLongPtr(hT, GWLP_WNDPROC, (LONG_PTR)LVProc);
            // create button rows
            int j = BX_SMP_PROCESSORS;
            Bit32u WStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;
            if (j > 1)
            {
                while (--j > 0)
                {
                    sprintf (bigbuf, "cpu%d",j);        // number all the CPU buttons
                    hCPUt[j] = CreateWindowEx(0,"sBtn",bigbuf,WStyle,0,0,1,1,hh,(HMENU)(1030+j),GetModuleHandle(0),0);
                }
                strcpy (bigbuf, "CPU0");        // Handle CPU0 specially -- it is "selected"
                hCPUt[0] = CreateWindowEx(0,"sBtn",bigbuf,WStyle,0,0,1,1,hh,(HMENU)MULTICPU_BTN_BASE,GetModuleHandle(0),0);
            }
            j = 5;
            while (--j >= 0)
                hBTN[j] = CreateWindowEx(0,"sBtn",BTxt[j],WStyle,0,0,1,1,hh,(HMENU)(BTN_BASE+j),GetModuleHandle(0),0);

            // any other OS-specific GUI initialization goes here:
            // Autosize a few columns once
            // RegName column
            CallWindowProc(wListView, hL[REG_WND], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE_USEHEADER);
            // "bytes" column
            CallWindowProc(wListView, hL[ASM_WND], LVM_SETCOLUMNWIDTH, 1, LVSCW_AUTOSIZE_USEHEADER);
            // and the empty MemDump columns
            for (int i = 0; i < 17; i++)
                CallWindowProc(wListView, hL[DUMP_WND], LVM_SETCOLUMNWIDTH, i, LVSCW_AUTOSIZE_USEHEADER);
            // load the extra resizing/docking cursors
            hCursResize = LoadCursor (NULL,IDC_SIZEWE);
            hCursDock = LoadCursor (NULL,IDC_CROSS);
            SetTimer(hh,2,500,NULL);    // request timer ticks every half a second (to update VGAW)
            UpdInProgress[0] = FALSE;
            UpdInProgress[1] = FALSE;
            UpdInProgress[2] = FALSE;
            ResizeColmns = TRUE;    // do the initial resize on hex, address columns
            // Input window caret is created in SetSizeIOS, when the WM_SIZE message arrives
            break;
        }

        case WM_SIZE:
        {
            RECT rc;
            int i, j, k;
            bx_bool unchanged = FALSE;

            if (hY == NULL)     // sometimes WM_SIZE is called before OSInit completes!
                hY = hh;
            // Mapping mode is MM_TEXT (Def), so Yvalues increase DOWN, all values are in pixels.
            GetClientRect(hY,&rc);
            // don't "resize" on minimize, if window is too small, or "unchanged"
            if (CurYSize == (unsigned int)rc.bottom && CurXSize == (unsigned int)rc.right)
                unchanged = TRUE;
            if (rc.bottom > 100 && rc.right > 150 && unchanged == FALSE)
            {
                // set up the Input/Output/Status windows
                CurYSize = rc.bottom;
                CurXSize = rc.right;
                SetSizeIOS(CurXSize,CurYSize);

                // need to recalculate List widths, with the same proportions as before
                i = ListWidthPix[0] + ListWidthPix[1] + ListWidthPix[2];
                if (i == 0)
                    i = 1;
                j = (ListWidthPix[2] * CurXSize) / i;
                k = (ListWidthPix[1] * CurXSize) / i;
                ListWidthPix[0] = CurXSize - k - j;     // Register list
                ListWidthPix[1] = k;                    // Asm
                ListWidthPix[2] = j;                    // MemDump
                MoveLists();
            }
            break;
        }

        case WM_CHAR:
        {
            if (ww != VK_RETURN)
                CallWindowProc(*wEdit, hE_I, WM_CHAR, (WPARAM) ww, (LPARAM) ll);
            break;
        }

        case WM_SYSKEYDOWN:     // ww == key + ALT
        {
            HotKey ((int)ww, -1, 0, 0);     // currently, no hotkeys have both Alt and something else
            break;
        }

        case WM_KEYDOWN:
        {
            int Control = GetKeyState(VK_CONTROL);  // key is down if val is negative
            int Shift = GetKeyState(VK_SHIFT);
            if (HotKey ((int)ww, 0, Shift, Control) < 0)
                CallWindowProc (*wEdit, hE_I, WM_KEYDOWN, (LPARAM) ww, (LPARAM) ll);
            return 0;
        }
        case WM_COMMAND:
        {
            int LW = LOWORD(ww);

            if (LW >= BTN_BASE && LW <= BTN_BASE +4)    // convert button IDs to command IDs
                LW = BtnLkup [LW - BTN_BASE];
            else if (LW >= MULTICPU_BTN_BASE && LW < MULTICPU_BTN_BASE + BX_MAX_SMP_THREADS_SUPPORTED)
            {
                unsigned int newCPU = LW - MULTICPU_BTN_BASE;
                if (CurrentCPU != newCPU)
                {
                    // change text on CurrentCPU button to lowercase
                    strcpy (tmpcb, "cpu0");
                    tmpcb[3] = CurrentCPU + '0';
                    SendMessage (hCPUt[CurrentCPU],WM_SETTEXT,(WPARAM) 0 ,(LPARAM) tmpcb);
                    // change text on newCPU button to UPPERCASE
                    strcpy (tmpcb, "CPU0");
                    tmpcb[3] = newCPU + '0';
                    SendMessage (hCPUt[newCPU],WM_SETTEXT,(WPARAM) 0 ,(LPARAM) tmpcb);
                    CurrentCPU = newCPU;
                    BottomAsmLA = ~0;       // force an ASM autoload, to repaint
                    if (AtBreak != FALSE)   // if at a break, pretend it just happened
                        OnBreak();          // refresh the ASM and Register windows
                }
                return 0;
            }
            if (LW >= CMD_BREAK && LW < CMD_MODEB)      // Does not include "Step"s or Options
            {
                if (AtBreak == FALSE)
                {
                    SIM->debug_break();     // On a menu click always break (with some exceptions)
                }
            }
            ActivateMenuItem (LW);  // run the code to perform the selected menu task

            return 0;
        }
        case WM_NOTIFY:
        {
            // key down
            NMHDR* n = (NMHDR*)ll;
            if (n->code == LVN_KEYDOWN)
            {
                NMLVKEYDOWN* key = (NMLVKEYDOWN*)ll;        // pass any keystrokes from listview up to parent
                SendMessage(hh,WM_KEYDOWN,key->wVKey,0);
            }
            if (n->code == (UINT)NM_CUSTOMDRAW && n->hwndFrom == hL[ASM_WND]) // custom drawing of ASM window
            {
                // highlight the breakpoints, and current opcode, if any
                NMLVCUSTOMDRAW *d = (NMLVCUSTOMDRAW *) ll;
                if (d->nmcd.dwDrawStage == CDDS_PREPAINT)
                    return CDRF_NOTIFYITEMDRAW;

                if (d->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)   // select the "active" ASM line
                {
                    unsigned int fgclr = 0;         // normal text color is black
                    d->clrTextBk = RGB(255,255,255);        // background is white
                    if (!AtBreak)
                        d->clrTextBk = RGB(210,210,210);    // unless sim is "running"

                    bx_address h = (bx_address) AsmLA[d->nmcd.dwItemSpec];
                    if (h == CurrentAsmLA)
                        fgclr = 1;              // current opcode is colored dark green
                    int j= BreakCount;
                    while (--j >= 0)            // loop over all breakpoints
                    {
                        // brk list is sorted -- if the value goes too low, end the loop
                        // And I know for a fact that some complers are soooo stupid that they
                        // will repeat the following test twice, unless you force them not to.
                        bx_address i = BrkLAddr[j] - h;
//                      if (BrkLAddr[j] < h)
                        if (i < 0)
                            j = 0;      // force the loop to end if it goes too far
//                      else if (BrkLAddr[j] == h)
                        else if (i == 0)
                            fgclr |= 2;     // change color if on a breakpoint
                    }
                    d->clrText = AsmColors[fgclr];
                    if (fgclr != 0)     // if this row has color, process its subitems further
                    {
                        SetWindowLong(hh, DWL_MSGRESULT, CDRF_NOTIFYSUBITEMDRAW);
                        return CDRF_NOTIFYSUBITEMDRAW;
                    }
                }
                else if (d->nmcd.dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT) && d->iSubItem == 2)
                {
                    // add extra visual feedback to a "colored" ASM mnemonic cell
                    // -- but don't try to recalculate breakpoints! Just "guess" from the color.
                    int fontattrib = 3;
                    if (d->clrText == AsmColors[1])     // green = current RIP = bold
                        fontattrib = 1;
                    else if (d->clrText == AsmColors[2])    // red = brkpt = italic
                        fontattrib = 2;
                    SelectObject (d->nmcd.hdc, CustomFont[fontattrib]);
                    return CDRF_NEWFONT | CDRF_DODEFAULT;
                }
                break;
            }
            if (n->code == (UINT)NM_CUSTOMDRAW && n->hwndFrom == hL[DUMP_WND])    // custom drawing of data window
            {
                NMLVCUSTOMDRAW *d = (NMLVCUSTOMDRAW *) ll;
                if (d->nmcd.dwDrawStage == CDDS_PREPAINT)
                    return CDRF_NOTIFYITEMDRAW;

                d->clrTextBk = RGB(255,255,255);        // background is white
                if (!AtBreak)
                    d->clrTextBk = RGB(210,210,210);    // unless sim is "running"

                else if (DViewMode == VIEW_STACK && d->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
                {
                    // highlight changed lines in stack
                    int j = d->nmcd.dwItemSpec;
                    if (j < 0 || j >= STACK_ENTRIES)    // make sure the line # is legal
                        ;
                    else if (StackEntChg[j] != FALSE)
                        d->clrText = RGB(255,0,0);      // changed entry = red
                }

                if (DumpAlign != 1 || LinearDump != FALSE || DViewMode != VIEW_MEMDUMP)
                    break;

                // highlight any physical watchpoints in physdump mode
                if (d->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
                {
                    SetWindowLong(hh, DWL_MSGRESULT, CDRF_NOTIFYSUBITEMDRAW);
                    return CDRF_NOTIFYSUBITEMDRAW;
                }

                if (d->nmcd.dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT))
                {
                    d->clrText = RGB(0,0,0);            // assume black text
                    if (d->iSubItem != 0)
                    {
                        // For each subitem, calculate its equivalent physmem address
                        bx_phy_address h = (bx_phy_address) DumpStart +
                            16*(d->nmcd.dwItemSpec) + d->iSubItem -1;
                        int j = num_write_watchpoints;
                        int i = num_read_watchpoints;
                        while (j > 0)
                        {
                            if (write_watchpoint[--j].addr == h)
                            {
                                d->clrTextBk = RGB(0,0,0);      // black background
                                d->clrText = RGB(255,0,150);    // write watchpoint
                                j = -1;         // on a match j<0 -- else j == 0
                            }
                        }
                        while (--i >= 0)
                        {
                            if (read_watchpoint[i].addr == h)
                            {
                                if (j < 0)      // BOTH read and write
                                    d->clrText = RGB(0,170,255);
                                else
                                {
                                    d->clrTextBk = RGB(0,0,0);      // black background
                                    d->clrText = RGB(130,255,0);    // read watchpoint
                                }
                                i = 0;          // end the loop on a match
                            }
                        }
                    }
                }
                break;
            }
            if (n->code == (UINT)NM_CUSTOMDRAW && n->hwndFrom == hL[REG_WND]) // custom drawing of register window
            {
                // highlight changed registers
                NMLVCUSTOMDRAW *d = (NMLVCUSTOMDRAW *) ll;
                if (d->nmcd.dwDrawStage == CDDS_PREPAINT)
                    return CDRF_NOTIFYITEMDRAW;

                if (d->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
                {
                    int i = d->nmcd.dwItemSpec;
                    Bit8u ClrFlgs = RegColor[ RitemToRnum[i] ];
                    d->clrText = RGB(0,0,0);
                    d->clrTextBk = RGB(255,255,255);

                    // RitemToRnum converts a ListView row number to the corresponding Register number.
                    // RegColor has the 0x80 bit set if the register is supposed to be red.
                    // Background color index is in the low nibble of ClrFlgs/RegColor.
                    if (SeeRegColors != FALSE)
                        d->clrTextBk = ColorList[ClrFlgs &7];
                    if ((ClrFlgs & 0x80) != 0)      // should this register be red?
                        d->clrText = RGB(255,0,0);
                    if (!AtBreak)
                        d->clrTextBk = RGB(210,210,210);    // gray out the background if "running"

                }
                break;
            }
            if (n->code == (UINT)NM_DBLCLK)
            {
                if (AtBreak == FALSE)
                    break;

                if (n->hwndFrom == hL[REG_WND])
                    ChangeReg();
                else if (n->hwndFrom == hL[ASM_WND])    // doubleclick a breakpoint on ASM window
                    SetBreak(SelectedEntry);

                else if (n->hwndFrom == hL[DUMP_WND])
                {
                    if (DViewMode == VIEW_MEMDUMP)  // Change values in memory locations
                    {
                        if (GetKeyState(VK_SHIFT) < 0)
                            SetWatchpoint(&num_write_watchpoints,write_watchpoint);

                        else if (GetKeyState(VK_MENU) < 0)  // ALT keys
                            SetWatchpoint(&num_read_watchpoints,read_watchpoint);

                        else
                        {
                            int L = CallWindowProc(wListView, hL[DUMP_WND], LVM_GETNEXTITEM,(WPARAM) -1,MAKELPARAM(LVNI_SELECTED, 0));
                            if (L >= 0)
                                SetMemLine(L);
                        }
                    }
                    else if (DViewMode == VIEW_BREAK)   // delete breakpoint in Break window
                    {
                        // HIHI I should be using SelectedEntry, instead of SelectedBID, I think
                        // HIHI -- the BID variable is not making my life easier
                        i = SelectedBID & 0xf;
                        if (SelectedBID >= 0x80000)         // read watchpoint
                        {
                            if (RWP_Snapshot[i] == read_watchpoint[i].addr)
                                DelWatchpoint(read_watchpoint, &num_read_watchpoints, i);
                        }
                        else if (SelectedBID >= 0x40000)    // write watchpoint
                        {
                            if (WWP_Snapshot[i] == write_watchpoint[i].addr)
                                DelWatchpoint(write_watchpoint, &num_write_watchpoints, i);
                        }
                        else
                        {
                            i = SelectedBID & 0xffff;
                            if (i != 0) // determine the breakpoint TYPE
                            {
                                if (SelectedBID > 0x20000)
                                    bx_dbg_del_pbreak(i);
                                else if (SelectedBID > 0x10000)
                                    bx_dbg_del_vbreak(i);
                                else    // linear brkpt
                                    bx_dbg_del_lbreak(SelectedBID);
                            }
                            ParseBkpt();
                            InvalidateRect(hL[ASM_WND],0,TRUE); // breakpoint colors may have changed
                        }
                        RefreshDataWin();
                    }
                }
            }
            break;
        }
        case WM_TIMER:
        {
            if (PO_Tdelay > 0)      // output window is delaying display of a partial line?
            {
                if (--PO_Tdelay == 0)   // on a timeout, add a lf to complete the line
                    ParseIDText ("\n");
            }
            UpdateStatus();
            vgaw_refresh = TRUE;    // ask bochs to update its own VGA window
            break;
        }
        case WM_NCMOUSEMOVE:
        {
            // turn off any sizing operation if the cursor strays off the listviews
            Sizing = 0;
            // also turn off any half-completed mouseclicks
            xClick = -1;
            break;
        }
        case WM_CLOSE:
        {
            bx_user_quit = 1;
            SIM->debug_break();
            KillTimer(hh,2);
            if (*CustomFont != DefFont)
                DeleteObject (*CustomFont);
            DeleteObject(CustomFont[1]);
            DeleteObject(CustomFont[2]);
            DeleteObject(CustomFont[3]);
            DestroyWindow(hY);
            hY = NULL;
            break;
        }
    }
    return DefWindowProc(hh,mm,ww,ll);
}

void HitBreak()
{
    // Sim is at a "break".
    if (AtBreak == FALSE)
    {
        AtBreak = TRUE;
        StatusChange = TRUE;
    }
    if (doDumpRefresh != FALSE)
        RefreshDataWin();
    OnBreak();
}

// This function must be called immediately after bochs starts
bx_bool OSInit()
{
    TEXTMETRIC tm;

    if (doOneTimeInit) {
        InitCommonControls();   // start the common control dll
        SpListView();       // create superclass for listviews to use when they are created
        SpBtn();            // same for buttons
        WNDCLASSEX wC = {0};
        wC.cbSize = sizeof(wC);
        wC.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        hCursArrow = LoadCursor(NULL,IDC_ARROW);
        wC.hCursor = hCursArrow;
        wC.hInstance = GetModuleHandle(0);
        wC.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS | CS_DBLCLKS;
        wC.lpfnWndProc = B_WP;
        wC.cbWndExtra = sizeof(void*);
        wC.lpszClassName = "bochs_dbg_x";
        wC.hIcon = LoadIcon(GetModuleHandle(0),"ICON_D");
        wC.hIconSm = LoadIcon(GetModuleHandle(0),"ICON_D");
        RegisterClassEx(&wC);
        doOneTimeInit = FALSE;
    }
    CurXSize = 0;
    CurYSize = 0;
    HMENU hTopMenu = LoadMenu(GetModuleHandle(0),"MENU_1");     // build the menus from the resource
    hOptMenu = GetSubMenu (hTopMenu, 2);                // need the main menu handles
    hViewMenu = GetSubMenu (hTopMenu, 1);
    hCmdMenu = GetSubMenu (hTopMenu, 0);
    hY = CreateWindowEx(0,"bochs_dbg_x","Bochs Enhanced Debugger",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
        0,hTopMenu,GetModuleHandle(0),0);
    if (hY == NULL)
        return FALSE;
    HDC hdc = GetDC (hY);
    *CustomFont = DefFont;  // create the deffont with modded attributes (bold, italic)
    memset(&mylf, 0, sizeof(LOGFONT));
    GetTextFace(hdc, LF_FULLFACESIZE, mylf.lfFaceName); // (constant is max length of a fontname)
    GetTextMetrics (hdc, &tm);
    ReleaseDC (hY, hdc);
    mylf.lfHeight = -(tm.tmHeight);     // request a TOTAL font height of tmHeight
    // create a bold version of the deffont
    mylf.lfWeight = FW_BOLD;
    CustomFont[1] = CreateFontIndirect (&mylf);
    // create a bold + italic version of the deffont
    mylf.lfItalic = 1;
    CustomFont[3] = CreateFontIndirect (&mylf);
    // create an italic version of the deffont (turn off bold)
    mylf.lfWeight = FW_NORMAL;
    CustomFont[2] = CreateFontIndirect (&mylf);
    return TRUE;
}

void CloseDialog()
{
  SendMessage(hY, WM_CLOSE, 0, 0);
}

// recurse displaying each leaf/branch of param_tree -- with values for each leaf
void MakeBL(HTREEITEM *h_P, bx_param_c *p)
{
    HTREEITEM h_new;
    bx_list_c *as_list = NULL;
    int i = 0;
    char tmpstr[BX_PATHNAME_LEN];
    strcpy(tmpcb, p->get_name());
    int j = strlen (tmpcb);
    switch (p->get_type())
    {
        case BXT_PARAM_NUM:
            if (((bx_param_num_c*)p)->get_base() == BASE_DEC)
                sprintf (tmpcb + j,": " FMT_LL "d",((bx_param_num_c*)p)->get64());
            else
                sprintf (tmpcb + j,": 0x" FMT_LL "X",((bx_param_num_c*)p)->get64());
            break;
        case BXT_LIST:
            as_list = (bx_list_c *)p;
            i = as_list->get_size();
            break;
        case BXT_PARAM_BOOL:
            sprintf (tmpcb + j,": %s",((bx_param_bool_c*)p)->get()?"true":"false");
            break;
        case BXT_PARAM_ENUM:
            sprintf (tmpcb + j,": %s",((bx_param_enum_c*)p)->get_selected());
            break;
        case BXT_PARAM_STRING:
            ((bx_param_string_c*)p)->sprint(tmpstr, BX_PATHNAME_LEN, 0);
            sprintf(tmpcb + j,": %s", tmpstr);
            break;
        case BXT_PARAM_DATA:
            sprintf (tmpcb + j,": binary data, size=%d",((bx_shadow_data_c*)p)->get_size());
    }
    MakeTreeChild (h_P, i, &h_new);
    if (i > 0)
    {
        while (--i >= 0)
            MakeBL(&h_new, as_list->get(i));    // recurse for all children that are lists
    }
}

bx_bool ParseOSSettings(const char *param, const char *value)
{
  // TODO: handle win32-specific settings here
  return 0;
}

void WriteOSSettings(FILE *fd)
{
  // TODO: handle win32-specific settings here
}

#endif
