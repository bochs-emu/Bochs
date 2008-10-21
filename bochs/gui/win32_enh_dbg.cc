//  BOCHS ENHANCED DEBUGGER Ver 1.0
//  (C) Chourdakis Michael, 2008
//  http://www.turboirc.com
//
//  Modified by Bruce Ewing
//

#include "bochs.h"
#include "cpu/cpu.h"
#include "win32dialog.h"
#include "wenhdbg_h.h"

#if BX_DEBUGGER

extern HWND hDebugDialog;
extern char *debug_cmd;
extern BOOL debug_cmd_ready;

INT_PTR CALLBACK A_DP(HWND hh2,UINT mm,WPARAM ww,LPARAM ll)
{
    static ASKTEXT* as = 0;
    switch(mm)
    {
        case WM_INITDIALOG:
            as = (ASKTEXT*)ll;
            SetWindowText(hh2,as->ti);
            SetWindowText(GetDlgItem(hh2,101),as->as);
            SetWindowText(GetDlgItem(hh2,102),as->re);
            if (as->DefT)
                SetTimer(hh2,1,as->DefT*1000,0);
            return TRUE;
        case WM_TIMER:
            PostMessage(hh2,WM_COMMAND,IDOK,0);
            KillTimer(hh2,1);
            return TRUE;
        case WM_COMMAND:
            if (HIWORD(ww) == EN_UPDATE)
                KillTimer(hh2,1);

            if (LOWORD(ww) == IDOK)
            {
                GetWindowText(GetDlgItem(hh2,102),as->re,100);
                EndDialog(hh2,IDOK);
                return 0;
            }
            if (LOWORD(ww) == IDCANCEL)
            {
                EndDialog(hh2,IDCANCEL);
                return 0;
            }
    }
    return 0;
}

bx_bool AskText(HWND hh,const TCHAR* ti,TCHAR* as,TCHAR* re,int DefT)
{
    ASKTEXT a = {ti,as,re,DefT};
    bx_bool ret = FALSE;
    // The dialog box needs a caret, and will destroy the one in hE_I.
    // So destroy it myself, cleanly, and let it be recreated cleanly.
    CallWindowProc(*wEdit, hE_I,WM_KILLFOCUS,(WPARAM) 0,0);
    if (DialogBoxParam(GetModuleHandle(0),"DIALOG_AT",hh,A_DP,(LPARAM)&a) == IDOK)
        ret = TRUE;
    // recreate the caret in the Input window
    CallWindowProc(*wEdit, hE_I,WM_SETFOCUS,(WPARAM) 0,0);
    SetFocus(hE_O);         // kludge to finish taking focus away from dialog box
    return ret;
}


// Match stuff
#define MATCH_TRUE 1
#define MATCH_FALSE 0
#define MATCH_ABORT -1

#define NEGATE_CLASS
#define OPTIMIZE_JUST_STAR

char UCtable[256];
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

int DoMatch(const char *text, const char *p, bx_bool IsCaseSensitive)
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
                if (IsCaseSensitive != FALSE)
                {
                    if (text[pT] != p[pP])
                        return MATCH_FALSE;
                    else
                        continue;
                }
                //         if (toupper(text[pT]) != toupper(p[pP]))
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
int VMatching(const char *text, const char *p, bx_bool IsCaseSensitive)
{
#ifdef OPTIMIZE_JUST_STAR
    if (p[0] == '*' && p[1] == '\0')
        return MATCH_TRUE;
#endif
    return (DoMatch(text, p, IsCaseSensitive) == MATCH_TRUE);
}

int IsMatching(const char *text, const char *p, bx_bool IsCaseSensitive)
{
    return VMatching(text, p, IsCaseSensitive);
}

// utility function for list resizing operation -- set LoX and HiX
// the resize operation exits if the mouse moves beyond LoX or HiX
void SetHorzLimits(int i)
{
    if (i < BarScrx[0] + 10)    // is it the left or right bar?
    {
        Resize_LoX = OneCharWide << 2;      // set horizontal limits
        i = ListWidthPix[(DockOrder >> 8) -1];  // col1 width
        // calculate end of col2 - 4 charwidths in parent coordinates
        Resize_HiX = i + ListWidthPix[CurCenterList] - (OneCharWide << 2);
        Sizing = 1;
    }
    else if (i > BarScrx[1] - 10)
    {
        i = ListWidthPix[(DockOrder >> 8) -1];  // col1 width
        Resize_LoX = i + (OneCharWide << 2);    // set horizontal limits
        // calculate total width - 4 charwidths in parent coordinates
        i = ListWidthPix[0] + ListWidthPix[1] + ListWidthPix[2];
        Resize_HiX = i - (OneCharWide << 2);
        Sizing = 2;
    }
    SetCursor(hCursResize);
}

void DockResize (HWND hh, LPARAM ll)    // ll is a child coordinate mouse-style Lparam
{
    int i = 2;      // assume either data or tree window is destination
    void MoveLists(HWND);
    if (hh == hL[0])
        i = 0;          // i = ListView destination index
    else if (hh == hL[1])
        i = 1;

    if (Sizing >= 10)       // dock operation
    {
        Sizing -= 10;           // calculate which list initiated dock = moving window
        if (Sizing != i)        // moving window = destination window is a no-op
        {
            // Convert Sizing and i into a table lookup index (j)
            // -- otherwise, the "algorithm" to compute new DockOrder is annoying
            int j = (Sizing*2 + ((Sizing | i) & 1)) *6;
            if (Sizing == 1)
                j = (Sizing*4 + (i & 2)) *3;

            // convert current DockOrder to a number from 0 to 5, add to j
            j += ((DockOrder >> 7) - 2) &6;
            if (((DockOrder >> 4) &3) > (DockOrder & 3))
                j += 1;
            DockOrder = nDock[j];
            MoveLists(hY);
        }
    }

    else            // resize operation
    {
        int idx, totpix;
        POINT pt;
        pt.x = LOWORD(ll);
        pt.y = 0;
        ClientToScreen(hh,&pt);
        ScreenToClient(hY,&pt);     // convert to parent's coordinates
        if (Sizing == 1)
        {
            idx = (DockOrder >> 8) -1;          // sizing the left bar
            totpix = ListWidthPix[idx] + ListWidthPix[CurCenterList];
            ListWidthPix[idx] = pt.x;
            ListWidthPix[CurCenterList] = totpix - pt.x;    // reset the widths of the left and center windows
        }
        else
        {
            pt.x -= ListWidthPix[(DockOrder >> 8) -1];      // caclulate new width of center window
            idx = (DockOrder & 3) -1;
            totpix = ListWidthPix[idx] + ListWidthPix[CurCenterList];
            ListWidthPix[CurCenterList] = pt.x;
            ListWidthPix[idx] = totpix - pt.x;  // reset the widths of the right and center windows
        }
        MoveLists(hY);
    }
    Sizing = 0;
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
        case WM_SETFOCUS:   // force the Input window to always have the caret
            return 0;   // SETFOCUS/KILLFOCUS commands are sent directly to the hE_I defProc
        case WM_KILLFOCUS:
            return 0;
        case WM_SHOWWINDOW: // Note: This doesn't work ... on an unhide, it didn't even GET this message!
            if (ww == 0)        // fall through to SetFocus on Output when Input window is "shown"
                return 0;
        case WM_LBUTTONDOWN:
            SetFocus(hE_O);     // this does more than just send a SETFOCUS message ...
            return 0;
        case WM_CHAR:
        case WM_SYSCHAR:
            // throw away all Enter keys (bell problems)
            if (ww == VK_RETURN)
                return 0;
                // force all typed input to the Input window (wEdit[0] = Input)
            CallWindowProc((WNDPROC) *wEdit, hE_I, mm, ww, ll);
            return 0;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            // simply let parent know about all keys hit in these edit boxes
            SendMessage(GetParent(hh),mm,ww,ll);
            // the following keys cause annoying "bells" if sent on to the Original proc
            // -- so throw them away (return 0)
            if (ww == VK_RETURN || ww == VK_DOWN || ww == VK_UP)
                return 0;
    }
    if (wEdit[i] != NULL)
        return (CallWindowProc(wEdit[i], hh, mm, ww, ll));
    else
        return 0;
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
    switch (mm)
    {
        case WM_CHAR:
            // throw away all Enter keys (or use them for something)
            if (ww != VK_RETURN && *wEdit != NULL)
            // force all typed input to the Input window
                CallWindowProc(*wEdit, hE_I, mm, ww, ll);
            return 0;

        case WM_MOUSEMOVE:      // need to play with the cursor during resize/dock operations
            pt.y = HIWORD(ll);
            pt.x = LOWORD(ll);      // get the listview client's X coord of the mouse
            if (xClick >= 0)        // saw a lbutton mousedown recently? (pre-dock mode)
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
                if (i >= 0 && Sizing != 0)
                    DockResize (hh, ll);
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
                    DockResize (hh, ll);
                else if (Sizing > 5)
                    SetCursor(hCursDock);
                else
                    SetCursor(hCursResize);
            }
            break;

        case WM_NCMOUSEMOVE:        // more cursor games
            if (Sizing == 0)
            {
                if (ww == HTBORDER)     // mouse is hovering or dragging over a resize bar or border
                {
                    i = LOWORD(ll);     // get the screen X coord of the mouse
                    if (i < BarScrx[0] + 10)    // is it the left or right bar?
                        Sizing = -2;
                    else if (i > BarScrx[1] - 10)
                        Sizing = -1;
                    Resize_LoX = BarClix[Sizing + 2] - OneCharWide; // set horizontal limits
                    Resize_HiX = BarClix[Sizing + 2] + OneCharWide;
                }
            }
            if (Sizing >= 10)       // docking operation?
                SetCursor(hCursDock);
            else if (Sizing != 0)
            {
                pt.y = HIWORD(ll);
                pt.x = LOWORD(ll);      // get the screen X coord of the mouse
                ScreenToClient(hY,&pt);     // convert to parent's coordinates
                // verify horizontal limits of the current resize operation
                if (pt.x > Resize_HiX || pt.x < Resize_LoX)
                    Sizing = 0;
                else
                    SetCursor(hCursResize);
            }
            break;

        case WM_NCLBUTTONDOWN:
            if (Sizing < 0)     // doing a resize?
                SetHorzLimits((int) LOWORD(ll));    // NC messages are in Screen coordinates
            SetFocus(hE_O);         // helps to make keystrokes go to Input window
            break;

        case WM_NCLBUTTONUP:
            Sizing = 0;     // a mouseup on a border can't be interpreted -- cancel everything
            xClick = -1;
            break;

        case WM_LBUTTONDOWN:
            if (Sizing < 0)     // doing a resize?
            {
                GetCursorPos(&pt);      // Screen coordinates
                SetHorzLimits((int) pt.x);
            }
            else        // set "pre-dock" mode
            {
                if (hh == hL[0])
                    SizeList = 10;      // remember which list to dock
                else if (hh == hL[1])
                    SizeList = 11;
                else
                    SizeList = 12;
                Resize_LoX = OneCharWide << 2;  // set horizontal limits
                i = ListWidthPix[0] + ListWidthPix[1] + ListWidthPix[2];
                Resize_HiX = i - (OneCharWide << 2);
                CurTimeStamp = GetMessageTime();
                xClick = LOWORD(ll);
                yClick = HIWORD(ll);
            }
            SetFocus(hE_O);         // helps to make keystrokes go to Input window
            // prevent default drag operations by returning 0
            // unfortunately, this also prevents clicks from happening automatically
            return 0;

        case WM_LBUTTONUP:
            // the mouseup COMPLETES a "click" or drag
            if (Sizing > 0)
                DockResize (hh, ll);
            else if (xClick >= 0)
            {
                pt.x = LOWORD(ll);
                pt.y = HIWORD(ll);
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
                        return 0;       // eat all the Tree mouseups
                    }
                    i = 0;              // deal with List Focus
                    DumpHasFocus = FALSE;
                    if (hh == hL[1])
                        i = 1;
                    else if (hh != hL[0])
                    {
                        DumpHasFocus = TRUE;
                        i = 2;
                        EnableMenuItem (hCmdMenu, CMD_BRKPT, MF_GRAYED);
                        if (DumpMode == 0)
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
                        if (i != 0)
                            CallWindowProc(wListView, hL[0], WM_KILLFOCUS, (WPARAM) 0, (LPARAM) 0);
                        if (i != 1)
                            CallWindowProc(wListView, hL[1], WM_KILLFOCUS, (WPARAM) 0, (LPARAM) 0);
                        if (i != 2)
                            CallWindowProc(wListView, hL[2], WM_KILLFOCUS, (WPARAM) 0, (LPARAM) 0);
                    }

                    lvht.pt.x = LOWORD(ll);     // do a hit test to get the iItem to select
                    lvht.pt.y = HIWORD(ll);
                    i = CallWindowProc(wListView, hh, LVM_HITTEST, (WPARAM) 0, (LPARAM) &lvht);
                    // i is a copy of the iItem number -- ignore anything <= 0
                    // deselect anyything that's selected
                    int j = CallWindowProc(wListView, hh, LVM_GETNEXTITEM, (WPARAM) -1, MAKELPARAM(LVNI_SELECTED, 0));
                    while (j >= 0)
                    {
                        // cancel the item's "selection"
                        CallWindowProc(wListView, hh, LVM_SETITEMSTATE, (WPARAM)j, (LPARAM)&lvi);
                        // and get the next one
                        j = CallWindowProc(wListView, hh, LVM_GETNEXTITEM, (WPARAM) -1, MAKELPARAM(LVNI_SELECTED, 0));
                    }
                    if (i >= 0)     // then select the one that got clicked
                    {
                        lvi.state = lvi.stateMask;  // value = "selected"
                        CallWindowProc(wListView, hh, LVM_SETITEMSTATE, (WPARAM)lvht.iItem, (LPARAM)&lvi);
                        if (hh == hL[2])
                        {
                            // if this was a MemDump click -- need to get the "Selected Address"
                            SelectedDataAddress = DumpStart + (i<<4);
                            // Clicked a Subitem? column numbers = byte offsets + 1
                            CallWindowProc(wListView, hh, LVM_SUBITEMHITTEST, 0, (LPARAM)&lvht);
                            if (lvht.iSubItem > 0)
                                SelectedDataAddress += lvht.iSubItem - 1;
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


bx_bool SpListView()                  // Superclasses a ListView control
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

bx_bool SpBtn()               // Superclasses a button control
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
void ShowEflags()
{
    Bit32u Efl = (Bit32u) rV[EFL_Rnum];
    int i = 16;
    char *cp = tmpcb + 6;

    sprintf(tmpcb,"IOPL=%1u", (Efl & 0x3000) >> 12);
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
    if (StatusChange == FALSE) return;  // avoid sending unnecessary messages/invalidations
    StatusChange = FALSE;

    if (AtBreak != FALSE)   // modify status line only during a break
    {
        ShowEflags();       // prints flags into tmpcb
        SendMessage(hS_S,SB_SETTEXT,3,(LPARAM)tmpcb);   // display eflags

        if (CpuModeChange != FALSE)     // Did CR0 bits or EFER bits change value?
        {
            CpuModeChange = FALSE;
            switch (CpuMode) {
                case BX_MODE_IA32_REAL:
                    if (In32Mode == FALSE)
                        SendMessage(hS_S,SB_SETTEXT,1,(LPARAM)"CPU: Real Mode (16)");
                    else
                        SendMessage(hS_S,SB_SETTEXT,1,(LPARAM)"CPU: Real Mode (32)");
                    break;
                case BX_MODE_IA32_V8086:
                    SendMessage(hS_S,SB_SETTEXT,1,(LPARAM)"CPU: V8086 Mode");
                    break;
                case BX_MODE_IA32_PROTECTED:
                    if (In32Mode == FALSE) {
                        if (InPaging != 0)
                            SendMessage(hS_S,SB_SETTEXT,1,(LPARAM)"CPU: PMode (16) (PG)");
                        else
                            SendMessage(hS_S,SB_SETTEXT,1,(LPARAM)"CPU: PMode (16)");
                    }
                    else {
                        if (InPaging != 0)
                            SendMessage(hS_S,SB_SETTEXT,1,(LPARAM)"CPU: PMode (32) (PG)");
                        else
                            SendMessage(hS_S,SB_SETTEXT,1,(LPARAM)"CPU: PMode (32)");
                    }
                    break;
                case BX_MODE_LONG_COMPAT:
                    SendMessage(hS_S,SB_SETTEXT,1,(LPARAM)"CPU: Compatibility Mode");
                    break;
                case BX_MODE_LONG_64:
                    SendMessage(hS_S,SB_SETTEXT,1,(LPARAM)"CPU: Long Mode");
                    break;
            }
        }
    }

    // set window color (active or gray), depending on AtBreak value
    if (AtBreak != PrevAtBreak)
    {
        PrevAtBreak = AtBreak;
        COLORREF c = RGB(255,255,255);
        if (AtBreak == FALSE)
        {
            SendMessage(hS_S,SB_SETTEXT,0,(LPARAM)"Running");
            c = RGB(210,210,210);       // emulation running -> gui inactive
        }
        else
            SendMessage(hS_S,SB_SETTEXT,0,(LPARAM)"Break");

        // this sets the background color OUTSIDE the area of the list, for A, D, R
        CallWindowProc(wListView, hL[2],LVM_SETBKCOLOR,0,c);
        CallWindowProc(wListView, hL[1],LVM_SETBKCOLOR,0,c);
        CallWindowProc(wListView, hL[0],LVM_SETBKCOLOR,0,c);

        InvalidateRect(hL[2],0,TRUE);
        InvalidateRect(hL[1],0,TRUE);
        InvalidateRect(hL[0],0,TRUE);
    }
}

// Read a copy of some emulated linear bochs memory
// Note: laddr + len must not cross a 4K boundary -- otherwise, there are no limits
bx_bool ReadBxLMem(Bit64u laddr, unsigned len, Bit8u *buf)
{
    bx_phy_address paddr;
    bx_bool retval = TRUE;

    // on same physical page as the last access?
    if (laddr < ladrmin || laddr > ladrmax)
    {
        // No -- create a new translation offset for the new page.
        if (laddr > (Bit64u) 0xffffffff && In64Mode == FALSE)
            return FALSE;
        bx_address la_4K = (bx_address) laddr & (~0xfff);
        ladrmin = la_4K;
        ladrmax = la_4K + 0xfff;
        retval = BX_CPU(CurrentCPU)->dbg_xlate_linear2phy(la_4K, &paddr);
        if (retval == FALSE)
            return FALSE;
        l_p_offset = la_4K - paddr;
    }
    paddr = (bx_phy_address)(laddr - l_p_offset);
    if (len != 0)
        retval = bx_mem.dbg_fetch_mem(BX_CPU(CurrentCPU), paddr, len, buf);
    return retval;
}


// "singlestep" disassembly lines from the internal debugger are sometimes ignored
bx_bool isSSDisasm(char *s)
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

// dump output from the bochs internal debugger to hE_O output window
void ParseUnknown(char *x)
{
    char *s = x;
    int i = 0;
    int overflow = 0;

    while (*s !=0 && *s != '\r' && *s != '\n' && DbgAppendPtr < tmpcb)
        *(DbgAppendPtr++)= *(s++);      // append the chars from x into the bigbuf
    if (DbgAppendPtr >= tmpcb)      // overflow error?
    {
        MessageBox(hY,"Debugger output cannot be parsed -- buffer overflow",0,MB_OK);
        DbgAppendPtr = bigbuf;      // throw away the line
        return;
    }

    *DbgAppendPtr = 0;
    if (*s == 0)            // automatically process only complete lines further
    {
        PO_Tdelay = 2;      // wait a half second, then force display of partial lines
        return;
    }
    PO_Tdelay = 0;          // line completed -- cancel any partial output time delay

    // restart DbgAppendPtr at the beginning of a new line buffer
    s= DbgAppendPtr= bigbuf;    // s -> raw register text line from debugger
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
    if ((i+overflow+3) > OutWinCnt)     // amt needed vs. space available
    {
        s = OutWindow;      // need to toss lines off beginning of OutWindow
        int j = OutWinCnt - overflow - 3;
        while (j < i)       // throw away one line at a time
        {
                // stop on any unprintable char < ' '
            while ((unsigned char)*s >= ' ' || *s == '\t')
            {
                ++s;
                ++j;    // increase available space as chars are tossed
            }
             // in reality, s must be pointing at a CRLF
            s += 2;
            j += 2;
        }
        OutWinCnt = j + overflow + 3;
        j = OutWinSIZE - OutWinCnt; // chars to copy, without the terminal zero
        while (j-- > 0)         // recopy the OutWindow buffer up
            *(p++) = *(s++);
    }
    else
        p = OutWindow + OutWinSIZE - OutWinCnt;
    OutWinCnt -= i + overflow + 2;
    *(p++) = '\r';          // end of buf only had a 0 in it,
    *(p++) = '\n';          // and needs CRLFs to display properly
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
    SetWindowText(hE_O,OutWindow);  // replace all text with modified buffer
//  SendMessage(hE_O,EM_SETSEL,0,-1);   // invisibly selects all, sets caret to end
    CallWindowProc(wEdit[1], hE_O,EM_SETSEL,OutWinSIZE,OutWinSIZE+1);   // sets caret to end only
    CallWindowProc(wEdit[1], hE_O,EM_SCROLLCARET,0,0);  // scrolls window to caret
    // Note: the caret is never actually visible in the Output window
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
    // copy the lower dwords of RAX - RBP to EAX - EBP (with 32bit truncation)
    i = RBP_Rnum + 1;
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
        if (NewPg == FALSE)
            EnableMenuItem (hViewMenu, CMD_PAGEV, MF_GRAYED);
        else
            EnableMenuItem (hViewMenu, CMD_PAGEV, MF_ENABLED);
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
#if BX_SUPPORT_SSE
    int i;
    Bit64u val = 0;
    bx_param_num_c *p;
    lviG.pszText = tmpce;
#ifndef IS_WIN98
    lviG.iGroupId = 4;
#endif
    if ((rV[CR0_Rnum] & 0xc) != 0)  // TS or EM flags in CR0 temporarily disable SSE
    {
        strcpy (tmpce, "SSE-off");
        lviG.iItem = LineCount;
        RitemToRnum[LineCount] = XMM0_Rnum;
        CallWindowProc(wListView,hL[0],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
        return ++LineCount;
    }

    // format: XMM[#] 00000000:00000000 (each 16 hex digits)
    LV_ITEM lvi = {LVIF_TEXT,0,2,0,0,(LPSTR) tmpcf,80,0,0};
    strcpy (tmpce, "XMM[0]");
    for (i = 0; i < BX_XMM_REGISTERS; i++)
    {
        if (i >= 10)
        {
            tmpce[4] = '1';
            tmpce[5] = i - 10 + '0';
            tmpce[6] = ']';
            tmpce[7] = 0;
        }
        else
            tmpce[4] = i + '0';
        lviG.iItem = LineCount;
        RitemToRnum[LineCount] = i + XMM0_Rnum;
        CallWindowProc(wListView,hL[0],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
        tmpce[6] = 0;
        p = RegObject[CurrentCPU][XMM0_hi + i];
        if (p != NULL)
            val = p->get64();   // get the value of "xmm(i)_hi" register
        else
            val = 0;
        *tmpcf = '0';       // I'm putting a hex value in the decimal column -- more room there!
        tmpcf[1] = 'x';
        sprintf (tmpcf + 2,Fmt64b[UprCase],val);
        strcpy (tmpcf + 18, " : ");
        p = RegObject[CurrentCPU][XMM0_Rnum + i];
        if (p != NULL)
            val = p->get64();       // "SSE.xmm[i]_lo"
        else
            val = 0;
        sprintf (tmpcf + 21,Fmt64b[UprCase], val);
        lvi.iItem = LineCount;
        CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) LineCount,(LPARAM) &lvi);
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

    lviG.pszText = tmpce;
#ifndef IS_WIN98
    lviG.iGroupId = 3;
#endif
    if ((rV[CR0_Rnum] & 0xc) != 0)  // TS or EM flags in CR0 temporarily disable MMX/FPU/SSE
    {
        strcpy (tmpce, "FPU-off");
        lviG.iItem = LineCount;
        RitemToRnum[LineCount] = ST0_Rnum;
        CallWindowProc(wListView,hL[0],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
        return ++LineCount;
    }

    // format: MM#|ST# 00000000:00000000 then FPU float value in "decimal" column
    LV_ITEM lvi = {LVIF_TEXT,0,0,0,0,(LPSTR) tmpcf,80,0,0};
    strcpy (tmpce, "MM0-ST0");
    i = 7;
    for (i = 0; i < 8; i++)
    {
        tmpce[2] = i + '0';
        tmpce[6] = i + '0';
        lviG.iItem = LineCount;
        RitemToRnum[LineCount] = i + ST0_Rnum;
        CallWindowProc(wListView,hL[0],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
        p = RegObject[CurrentCPU][ST0_Rnum + i];
        if (p != NULL)
            mmreg = p->get64(); // get the value of "mmx(i)" register
        else
            mmreg = 0;
        sprintf (tmpcf,Fmt32b[UprCase],GET32H(mmreg));
        strcpy (tmpcf + 8, " : ");
        sprintf (tmpcf + 11,Fmt32b[UprCase], GET32L(mmreg));
        lvi.iItem = LineCount;
        lvi.iSubItem = 1;
        CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) LineCount,(LPARAM) &lvi);
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
        sprintf (tmpcf,"%.3e",f);
        lvi.iSubItem = 2;
        CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) LineCount,(LPARAM) &lvi);
        ++LineCount;
    }
    strcpy (tmpce, "ST0.exp");
    for (i = 0; i < 8; i++)
    {
        tmpce[2] = i + '0';
        lviG.iItem = LineCount;
        RitemToRnum[LineCount] = i + ST0_exp;
        CallWindowProc(wListView,hL[0],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
        sprintf (tmpcf,Fmt16b[UprCase], exponent[i]);
        lvi.iItem = LineCount;
        lvi.iSubItem = 1;
        CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) LineCount,(LPARAM) &lvi);
        sprintf (tmpcf,"%u", exponent[i]);
        lvi.iSubItem = 2;
        CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) LineCount,(LPARAM) &lvi);
        ++LineCount;
    }
    return LineCount;
}

// get values of Debug registers from simulation
int FillDebugRegs(int itemnum)
{
    bx_param_num_c *bxp;
    Bit32u val[6];
    unsigned int i;
    LV_ITEM lvi = {LVIF_TEXT,0,1,0,0,(LPSTR) tmpcf,80,0,0};
    strcpy (tmpce,"dr0");
    if (UprCase != FALSE)
    {
        *tmpce = 'D';
        tmpce[1] = 'R';
    }
    lviG.pszText = tmpce;
    for(i = 0 ; i < 6 ; i++)
    {
        bxp = RegObject[CurrentCPU][DR0_Rnum + i];
        val[i] = 0;
        if (bxp != NULL)
             val[i] = bxp->get();
        RitemToRnum[itemnum] = i + DR0_Rnum;
        lviG.iItem = itemnum;
#ifndef IS_WIN98
        lviG.iGroupId = 5;
#endif
        CallWindowProc(wListView,hL[0],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
        sprintf(tmpcf,Fmt32b[UprCase],val[i]);
        lvi.iItem = itemnum;
        CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) itemnum,(LPARAM) &lvi);
        ++tmpce[2];     // change the name, the cheap way
        if (i == 3) tmpce[2] += 2;  // jump from "DR3" to "DR6"
        ++itemnum;
    }
    return itemnum;
}

// Disassemble a linear memory area, in a loop, loading text into ASM window
// completely update the ASM display with new data
void FillAsm(Bit64u LAddr, int MaxLines)
{
    Bit64u ReadAddr = LAddr;
    int BufLen = 0;
    int i, len;
    bx_bool BufEmpty;
    bx_bool Go = TRUE;
    char *s;
    char *p = bigbuf;   // just to avoid a compiler warning
    LV_ITEM lvi = {LVIF_TEXT,0,0,0,0,(LPSTR) 0,80,0,0};

    AsmLineCount = 0;           // initialize for disasm window update
    CallWindowProc(wListView,hL[1],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
    if (MaxLines > MAX_ASM)     // just for protection
        MaxLines = MAX_ASM;

    while (Go != FALSE)
    {
        // copydown buffer -- buffer size must be 4K + 16
        s= bigbuf;
        i= BufLen;      // BufLen is guaranteed < 16
        while (i-- > 0)
            *(s++)= *(p++);
        // load buffer, up to the next 4k boundary
        len = 4096 - (((int) ReadAddr) & 0xfff);    // calculate read amount
        Go = ReadBxLMem (ReadAddr, len, (Bit8u *) s);
        BufLen += len;
        ReadAddr += len;
        if (Go == FALSE)
            return;
        BufEmpty = FALSE;
        p= bigbuf;          // start at the beginning of the new buffer
        while (AsmLineCount < MaxLines && BufEmpty == FALSE)
        {
            // disassemble 1 line with a direct call, into tmpcf
            len = bx_disassemble.disasm(In32Mode, In64Mode, (bx_address) 0,
                (bx_address) LAddr, (Bit8u *) p, tmpcf);
            if (len <= BufLen)      // disassembly was successful?
            {
                AsmLA[AsmLineCount] = LAddr;        // save, and
                if (In64Mode == FALSE)      // "display" linear addy of the opcode
                    sprintf (tmpce,Fmt32b[UprCase],LAddr);
                else
                    sprintf (tmpce,Fmt64b[UprCase],LAddr);

                // create a new list entry
                lvi.iSubItem = 0;
                lvi.pszText = tmpce;
                lvi.iItem = AsmLineCount;
                CallWindowProc(wListView,hL[1],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lvi);
                BufLen -= len;  // used up len bytes from buffer
                LAddr += len;   // calculate next LAddr

                // then build the "bytes" column entry
                s = tmpce;
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
                    *(s++) = AsciiHex[ 2* i ];
                    *(s++) = AsciiHex[ 1+ 2*i ];
                }
                *s = 0;     // zero terminate the "bytes" string
                lvi.iSubItem = 1;
                CallWindowProc(wListView,hL[1],LVM_SETITEMTEXT,(WPARAM) AsmLineCount,(LPARAM) &lvi);
                // display the ASM text
                if (UprCase != FALSE)   // do any requested uppercase conversion on the text
                    upr(tmpcf);
                lvi.iSubItem = 2;
                lvi.pszText = tmpcf;
                CallWindowProc(wListView,hL[1],LVM_SETITEMTEXT,(WPARAM) AsmLineCount,(LPARAM) &lvi);
                ++AsmLineCount;
            }
            else
                BufEmpty = TRUE;
        }
        if (AsmLineCount >= MaxLines)       // disassembled enough lines?
            Go = FALSE;
    }
}


// Reload the entire Register window (hL[0]) with data
void LoadRegList()
{
    int i, itemnum;     // TODO: This routine needs a big rewrite to make it pretty
    bx_bool showEreg = TRUE;
//  ShowWindow(hL[0],SW_HIDE);      // Hiding the windows causes way too much flicker
    FillRegs();         // get new values for rV local register array
    CallWindowProc(wListView,hL[0],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);   // destroy list
    // set general purpose Register ListView Item "group" = 0
    // groupId sets a little text header in the Listview -- but is not Pre-WinXP compatible
#ifndef IS_WIN98
        lviG.iGroupId = 0;
#endif

    LV_ITEM lvi = {LVIF_TEXT,0,0,0,0,(LPSTR) tmpce,80,0,0};
    // Display GP registers -- 64 bit registers first, if they exist
    itemnum = 0;
    if (In64Mode != FALSE)
    {
        showEreg = SeeReg[0];       // get user option setting for EAX, etc.
        for (i = RAX_Rnum ; i <= R15_Rnum ; i++)
        {
            RitemToRnum[itemnum] = i;   // always recreate the register -> itemnum mapping
            lviG.iItem = itemnum;
            lviG.pszText = RDispName[i];
            CallWindowProc(wListView,hL[0],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);

            lvi.iItem = itemnum;
            sprintf(tmpce,"%I64d",rV[i]);       // print the decimal column
            lvi.iSubItem = 2;
            CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) itemnum,(LPARAM) &lvi);

            sprintf(tmpce,Fmt64b[UprCase],rV[i]);   // and hex
            lvi.iSubItem = 1;
            CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) itemnum,(LPARAM) &lvi);
            ++itemnum;

        }
    }

    // then 32bit GP registers (if appropriate)
    if (showEreg != FALSE)
    {
        for (i = EAX_Rnum ; i <= EIP_Rnum ; i++)
        {
            RitemToRnum[itemnum] = i;
            lviG.iItem = itemnum;
            lviG.pszText = RDispName[i];

            if (In32Mode == FALSE && i == 26)   // Check for Real Mode (Pmode is TRUE in Long Mode)
            {
                rV[EIP_Rnum] &= 0xffff;     // in Real Mode, mask IP to 2 bytes
                ++(lviG.pszText);           // and shorten name to 2 letters
            }
            CallWindowProc(wListView,hL[0],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);

            lvi.iItem = itemnum;
            sprintf(tmpce,"%I64d",rV[i]);       // print the decimal column
            lvi.iSubItem = 2;
            CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) itemnum,(LPARAM) &lvi);

            sprintf(tmpce,Fmt32b[UprCase],(Bit32u)rV[i]);   // and hex
            lvi.iSubItem = 1;
            CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) itemnum,(LPARAM) &lvi);
            ++itemnum;
        }
    }
    // always insert eflags next
    RitemToRnum[itemnum] = EFL_Rnum;
    lviG.iItem = itemnum;
    lviG.pszText = RDispName[EFL_Rnum];
    CallWindowProc(wListView,hL[0],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);

    sprintf(tmpce,Fmt32b[UprCase],(Bit32u)rV[EFL_Rnum]);
    lvi.iItem = itemnum;        // SubItem is already 1
    CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) itemnum,(LPARAM) &lvi);
    ++itemnum;

    if (rV[EFL_Rnum] != PV[EFL_Rnum])
        StatusChange = TRUE;    // if eflags changed, force a status update

    // display Segment registers (if requested)
    if (SeeReg[1])
    {
#ifndef IS_WIN98
        lviG.iGroupId = 1;
#endif
        for(i = CS_Rnum ; i <= GS_Rnum ; i++)       // segment registers
        {
            RitemToRnum[itemnum] = i;
            lviG.iItem = itemnum;
            lviG.pszText = RDispName[i];
            CallWindowProc(wListView,hL[0],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
            sprintf(tmpce,Fmt16b[UprCase], rV[i] & 0xffff);
            lvi.iItem = itemnum;        // SubItem is already 1
            CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) itemnum,(LPARAM) &lvi);
            ++itemnum;
        }
    }
    // display System regsiters (if requested)
    // displaying these once is necessary for column resizing
    if (SeeReg[2] || ResizeColmns)
    {
#ifndef IS_WIN98
        lviG.iGroupId = 1;
#endif
        int j = TRRnum;
        if (In32Mode == FALSE)      // don't show lgdt or tr in Real mode
            j= IDTRnum;
        for(i = GDTRnum ; i <= j ; i++)
        {
            RitemToRnum[itemnum] = i;
            lviG.iItem = itemnum;
            lviG.pszText = RDispName[i];
            CallWindowProc(wListView,hL[0],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);

            if (i == GDTRnum || i == IDTRnum)
            {
                Bit16u limit = GDT_Len;
                if (i == IDTRnum)
                    limit = IDT_Len;
                if (In64Mode == FALSE)
                    sprintf(tmpce,xDT32Fmt[UprCase],(Bit32u)rV[i],limit);
                else
                    sprintf(tmpce,xDT64Fmt[UprCase],rV[i],limit);

            }
            else
                sprintf(tmpce,Fmt16b[UprCase], rV[i] & 0xffff); // lgdt, tr

            lvi.iItem = itemnum;        // SubItem is already 1
            CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) itemnum,(LPARAM) &lvi);
            ++itemnum;
        }
    }
    // display Control Registers (if requested)
    if (SeeReg[3])
    {
#ifndef IS_WIN98
        lviG.iGroupId = 2;
#endif
        for(i = CR0_Rnum ; i <= EFER_Rnum ; i++)
        {
            RitemToRnum[itemnum] = i;
            lviG.iItem = itemnum;
            lviG.pszText = RDispName[i];
            CallWindowProc(wListView,hL[0],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);

            sprintf(tmpce,"%08X",(Bit32u)rV[i]);
            lvi.iItem = itemnum;        // SubItem is already 1
            CallWindowProc(wListView,hL[0],LVM_SETITEMTEXT,(WPARAM) itemnum,(LPARAM) &lvi);
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
    // Load any optional STi, MMX, SSE, DRx, TRx register info into the Register window (hL[0])
#if BX_SUPPORT_FPU
    // MMX-FPU registers
    if (SeeReg[4] != FALSE)
        itemnum = FillMMX(itemnum);
#endif

#if BX_SUPPORT_SSE
    // SSE registers
    if (SeeReg[5] != FALSE)
        itemnum = FillSSE(itemnum);
#endif

    // Internal x86 Debug Registers
    if (SeeReg[6] != FALSE)
        itemnum = FillDebugRegs(itemnum);

//  if (SeeReg[7] != FALSE)             // Test registers are not supported yet in bochs
//      FillTRXRegs(itemnum);

    if (ResizeColmns != FALSE)  // resize Hex Value column sometimes
        CallWindowProc(wListView, hL[0], LVM_SETCOLUMNWIDTH, 1, LVSCW_AUTOSIZE);

    ShowWindow(hL[0],SW_SHOW);
}

// scroll ASM window so that the current line is in the "middle"
void doAsmScroll()
{
    int j;
    int CurTopIdx = CallWindowProc(wListView,hL[1],LVM_GETTOPINDEX,(WPARAM) 0,(LPARAM) 0);
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
        CallWindowProc(wListView,hL[1],LVM_SCROLL,(WPARAM) 0,(LPARAM) (ScrollLines * ListLineRatio));
    }
    InvalidateRect(hL[1],0,TRUE);   // "current opcode" in ASM window needs redrawing
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
    int j = TotCPUs;
    // get the param tree interface objects for every single register on all CPUs
    while (--j >= 0)
    {
        // RegObject[j]s are all initted to NULL when allocated in the BSS area
        // but it doesn't hurt anything to do it again, once
        int i = TOT_REG_NUM + EXTRA_REGS;
        while (--i >= 0)
            RegObject[j][i] = (bx_param_num_c *) NULL;
        sprintf (tmpce,"bochs.cpu%d",j);    // set the "cpu number" for cpu_list
        cpu_list = (bx_list_c *) SIM->get_param(tmpce,root_param);
        // TODO: in the next version, put all the names in an array, and loop
        // -- but that method is not compatible with bochs 2.3.7 or earlier
#if BX_SUPPORT_X86_64 == 0
        RegObject[j][EAX_Rnum] = SIM->get_param_num("EAX", cpu_list);
        RegObject[j][EBX_Rnum] = SIM->get_param_num("EBX", cpu_list);
        RegObject[j][ECX_Rnum] = SIM->get_param_num("ECX", cpu_list);
        RegObject[j][EDX_Rnum] = SIM->get_param_num("EDX", cpu_list);
        RegObject[j][ESI_Rnum] = SIM->get_param_num("ESI", cpu_list);
        RegObject[j][EDI_Rnum] = SIM->get_param_num("EDI", cpu_list);
        RegObject[j][EBP_Rnum] = SIM->get_param_num("EBP", cpu_list);
        RegObject[j][ESP_Rnum] = SIM->get_param_num("ESP", cpu_list);
        RegObject[j][EIP_Rnum] = SIM->get_param_num("EIP", cpu_list);
#else
        RegObject[j][RAX_Rnum] = SIM->get_param_num("RAX", cpu_list);
        RegObject[j][RBX_Rnum] = SIM->get_param_num("RBX", cpu_list);
        RegObject[j][RCX_Rnum] = SIM->get_param_num("RCX", cpu_list);
        RegObject[j][RDX_Rnum] = SIM->get_param_num("RDX", cpu_list);
        RegObject[j][RSI_Rnum] = SIM->get_param_num("RSI", cpu_list);
        RegObject[j][RDI_Rnum] = SIM->get_param_num("RDI", cpu_list);
        RegObject[j][RBP_Rnum] = SIM->get_param_num("RBP", cpu_list);
        RegObject[j][RSP_Rnum] = SIM->get_param_num("RSP", cpu_list);
        RegObject[j][RIP_Rnum] = SIM->get_param_num("RIP", cpu_list);
        RegObject[j][R8_Rnum] = SIM->get_param_num("R8", cpu_list);
        RegObject[j][R9_Rnum] = SIM->get_param_num("R9", cpu_list);
        RegObject[j][R10_Rnum] = SIM->get_param_num("R10", cpu_list);
        RegObject[j][R11_Rnum] = SIM->get_param_num("R11", cpu_list);
        RegObject[j][R12_Rnum] = SIM->get_param_num("R12", cpu_list);
        RegObject[j][R13_Rnum] = SIM->get_param_num("R13", cpu_list);
        RegObject[j][R14_Rnum] = SIM->get_param_num("R14", cpu_list);
        RegObject[j][R15_Rnum] = SIM->get_param_num("R15", cpu_list);
#endif
        RegObject[j][EFL_Rnum]= SIM->get_param_num("EFLAGS", cpu_list);
        RegObject[j][CS_Rnum]= SIM->get_param_num("CS.selector", cpu_list);
        RegObject[j][DS_Rnum]= SIM->get_param_num("DS.selector", cpu_list);
        RegObject[j][ES_Rnum]= SIM->get_param_num("ES.selector", cpu_list);
        RegObject[j][SS_Rnum]= SIM->get_param_num("SS.selector", cpu_list);
        RegObject[j][FS_Rnum]= SIM->get_param_num("FS.selector", cpu_list);
        RegObject[j][GS_Rnum]= SIM->get_param_num("GS.selector", cpu_list);
        RegObject[j][GDTRnum]= SIM->get_param_num("GDTR.base", cpu_list);
        RegObject[j][GDTR_Lim]= SIM->get_param_num("GDTR.limit", cpu_list);
        RegObject[j][IDTRnum]= SIM->get_param_num("IDTR.base", cpu_list);
        RegObject[j][IDTR_Lim]= SIM->get_param_num("IDTR.limit", cpu_list);
        RegObject[j][LDTRnum]= SIM->get_param_num("LDTR.base", cpu_list);
        RegObject[j][TRRnum]= SIM->get_param_num("TR.base", cpu_list);
        RegObject[j][CR0_Rnum]= SIM->get_param_num("CR0", cpu_list);
        RegObject[j][CR3_Rnum]= SIM->get_param_num("CR3", cpu_list);
#if BX_CPU_LEVEL >= 4
        RegObject[j][CR4_Rnum] = SIM->get_param_num("CR4", cpu_list);
#endif
#if BX_SUPPORT_X86_64
        RegObject[j][EFER_Rnum]= SIM->get_param_num("MSR.EFER", cpu_list);
#endif
#if BX_SUPPORT_FPU
        RegObject[j][ST0_Rnum]= SIM->get_param_num("FPU.st0.fraction", cpu_list);
        RegObject[j][ST1_Rnum]= SIM->get_param_num("FPU.st1.fraction", cpu_list);
        RegObject[j][ST2_Rnum]= SIM->get_param_num("FPU.st2.fraction", cpu_list);
        RegObject[j][ST3_Rnum]= SIM->get_param_num("FPU.st3.fraction", cpu_list);
        RegObject[j][ST4_Rnum]= SIM->get_param_num("FPU.st4.fraction", cpu_list);
        RegObject[j][ST5_Rnum]= SIM->get_param_num("FPU.st5.fraction", cpu_list);
        RegObject[j][ST6_Rnum]= SIM->get_param_num("FPU.st6.fraction", cpu_list);
        RegObject[j][ST7_Rnum]= SIM->get_param_num("FPU.st7.fraction", cpu_list);
        RegObject[j][ST0_exp]= SIM->get_param_num("FPU.st0.exp", cpu_list);
        RegObject[j][ST1_exp]= SIM->get_param_num("FPU.st1.exp", cpu_list);
        RegObject[j][ST2_exp]= SIM->get_param_num("FPU.st2.exp", cpu_list);
        RegObject[j][ST3_exp]= SIM->get_param_num("FPU.st3.exp", cpu_list);
        RegObject[j][ST4_exp]= SIM->get_param_num("FPU.st4.exp", cpu_list);
        RegObject[j][ST5_exp]= SIM->get_param_num("FPU.st5.exp", cpu_list);
        RegObject[j][ST6_exp]= SIM->get_param_num("FPU.st6.exp", cpu_list);
        RegObject[j][ST7_exp]= SIM->get_param_num("FPU.st7.exp", cpu_list);
#endif
#if BX_SUPPORT_SSE
        RegObject[j][XMM0_Rnum]= SIM->get_param_num("SSE.xmm00_lo", cpu_list);
        RegObject[j][XMM1_Rnum]= SIM->get_param_num("SSE.xmm01_lo", cpu_list);
        RegObject[j][XMM2_Rnum]= SIM->get_param_num("SSE.xmm02_lo", cpu_list);
        RegObject[j][XMM3_Rnum]= SIM->get_param_num("SSE.xmm03_lo", cpu_list);
        RegObject[j][XMM4_Rnum]= SIM->get_param_num("SSE.xmm04_lo", cpu_list);
        RegObject[j][XMM5_Rnum]= SIM->get_param_num("SSE.xmm05_lo", cpu_list);
        RegObject[j][XMM6_Rnum]= SIM->get_param_num("SSE.xmm06_lo", cpu_list);
        RegObject[j][XMM7_Rnum]= SIM->get_param_num("SSE.xmm07_lo", cpu_list);
        RegObject[j][XMM0_hi]= SIM->get_param_num("SSE.xmm00_hi", cpu_list);
        RegObject[j][XMM1_hi]= SIM->get_param_num("SSE.xmm01_hi", cpu_list);
        RegObject[j][XMM2_hi]= SIM->get_param_num("SSE.xmm02_hi", cpu_list);
        RegObject[j][XMM3_hi]= SIM->get_param_num("SSE.xmm03_hi", cpu_list);
        RegObject[j][XMM4_hi]= SIM->get_param_num("SSE.xmm04_hi", cpu_list);
        RegObject[j][XMM5_hi]= SIM->get_param_num("SSE.xmm05_hi", cpu_list);
        RegObject[j][XMM6_hi]= SIM->get_param_num("SSE.xmm06_hi", cpu_list);
        RegObject[j][XMM7_hi]= SIM->get_param_num("SSE.xmm07_hi", cpu_list);
#if BX_SUPPORT_X86_64
        RegObject[j][XMM8_Rnum]= SIM->get_param_num("SSE.xmm08_lo", cpu_list);
        RegObject[j][XMM9_Rnum]= SIM->get_param_num("SSE.xmm09_lo", cpu_list);
        RegObject[j][XMMA_Rnum]= SIM->get_param_num("SSE.xmm10_lo", cpu_list);
        RegObject[j][XMMB_Rnum]= SIM->get_param_num("SSE.xmm11_lo", cpu_list);
        RegObject[j][XMMC_Rnum]= SIM->get_param_num("SSE.xmm12_lo", cpu_list);
        RegObject[j][XMMD_Rnum]= SIM->get_param_num("SSE.xmm13_lo", cpu_list);
        RegObject[j][XMME_Rnum]= SIM->get_param_num("SSE.xmm14_lo", cpu_list);
        RegObject[j][XMMF_Rnum]= SIM->get_param_num("SSE.xmm15_lo", cpu_list);
        RegObject[j][XMM8_hi]= SIM->get_param_num("SSE.xmm08_hi", cpu_list);
        RegObject[j][XMM9_hi]= SIM->get_param_num("SSE.xmm09_hi", cpu_list);
        RegObject[j][XMMA_hi]= SIM->get_param_num("SSE.xmm00_hi", cpu_list);
        RegObject[j][XMMB_hi]= SIM->get_param_num("SSE.xmm11_hi", cpu_list);
        RegObject[j][XMMC_hi]= SIM->get_param_num("SSE.xmm12_hi", cpu_list);
        RegObject[j][XMMD_hi]= SIM->get_param_num("SSE.xmm13_hi", cpu_list);
        RegObject[j][XMME_hi]= SIM->get_param_num("SSE.xmm14_hi", cpu_list);
        RegObject[j][XMMF_hi]= SIM->get_param_num("SSE.xmm15_hi", cpu_list);
#endif      // 64bit
#endif      // SSE
        RegObject[j][DR0_Rnum]= SIM->get_param_num("DR0", cpu_list);
        RegObject[j][DR1_Rnum]= SIM->get_param_num("DR1", cpu_list);
        RegObject[j][DR2_Rnum]= SIM->get_param_num("DR2", cpu_list);
        RegObject[j][DR3_Rnum]= SIM->get_param_num("DR3", cpu_list);
        RegObject[j][DR6_Rnum]= SIM->get_param_num("DR6", cpu_list);
        RegObject[j][DR7_Rnum]= SIM->get_param_num("DR7", cpu_list);
// is there an #if for whether the test registers are supported?
//  RegObject[j][71]= SIM->get_param_num("TR3", cpu_list);
// {"TR3","TR4","TR5","TR6","TR7"};
    }
}


// set all TRUE flags to checked in the Options menu, gray out unsupported features
void InitMenus()
{
    int i = 8;
    doOneTimeInit = FALSE;
    EnableMenuItem (hOptMenu, CMD_EREG, MF_GRAYED);
    EnableMenuItem (hCmdMenu, CMD_WPTWR, MF_GRAYED);
    EnableMenuItem (hCmdMenu, CMD_WPTRD, MF_GRAYED);
    // 32bit registers are always displayed until longmode is active
    while (--i >= 0)
    {
        if (SeeReg[i] != FALSE)
            CheckMenuItem (hOptMenu, i + CMD_EREG, MF_CHECKED);
    }
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
    if ((DumpInAsciiMode & 2) != 0)
        CheckMenuItem (hOptMenu, CMD_MHEX, MF_CHECKED);
    if ((DumpInAsciiMode & 1) != 0)
        CheckMenuItem (hOptMenu, CMD_MASCII, MF_CHECKED);
    if (ShowIOWindows != FALSE)
        CheckMenuItem (hOptMenu, CMD_IOWIN, MF_CHECKED);

    HMENU wsMenu = GetSubMenu (hOptMenu, WS_POPUP_IDX);
    CheckMenuItem (wsMenu, CMD_WS_1, MF_CHECKED);
    EnableMenuItem (hOptMenu, CMD_TREG, MF_GRAYED);     // not currently supported by bochs
    EnableMenuItem (hViewMenu, CMD_PAGEV, MF_GRAYED);
#if BX_SUPPORT_FPU == 0
    EnableMenuItem (hOptMenu, CMD_FPUR, MF_GRAYED);
#endif

#if BX_SUPPORT_SSE == 0
    EnableMenuItem (hOptMenu, CMD_XMMR, MF_GRAYED);
#endif
}

void doUpdate()
{
    void FillStack();
    if (doOneTimeInit != FALSE)
        InitMenus();
    // begin an autoupdate of Register and Asm windows
    LoadRegList();      // build and show ListView
    ParseBkpt();        // get the linear breakpoint list
    if (DViewMode == VIEW_STACK)    // in stack view mode, keep the stack updated
        FillStack();
    CurrentAsmLA = BX_CPU(CurrentCPU)->get_laddr(BX_SEG_REG_CS, (bx_address) rV[RIP_Rnum]);
//  ShowWindow(hL[1],SW_HIDE);      // Hiding the windows causes way too much flicker
    if (CurrentAsmLA < BottomAsmLA || CurrentAsmLA > TopAsmLA)
    {
        Bit64u h = CurrentAsmLA;
        // generate a startLA (= h) that overlaps by topmargin, if possible
        CanDoLA(&h);

        FillAsm(h, DefaultAsmLines);
        if (ResizeColmns != FALSE)
        {
            // recalculate # of list items per page for all list windows
            AsmPgSize = CallWindowProc(wListView,hL[1],LVM_GETCOUNTPERPAGE,(WPARAM) 0,(LPARAM) 0);
            if (AsmPgSize != 0)
                ListLineRatio = ListVerticalPix / AsmPgSize;
            CallWindowProc(wListView, hL[1], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
        }

        // Set the scroll position for the new ASM window
        BottomAsmLA = *AsmLA;
        int j = bottommargin;       // try to use this bottom margin on ASM window
        if (j > AsmLineCount)
            j = AsmLineCount;
        TopAsmLA = AsmLA[AsmLineCount - j];     //TopAsmLA is the scroll point
    }
    doAsmScroll();          // ASM window may need to scroll
    ShowWindow(hL[1],SW_SHOW);
    ResizeColmns = FALSE;       // done with reformatting, if it was needed
    UpdateStatus();     // Mode and Eflags may have changed status
}
    
// squish the MemDump columns to 0 width, in preparation for resizing
void ZeroDColumns()
{
    int i = 17;     // never change the size of the last column
    while (--i >= 0)
        CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, i, 0);
}

// Fill the GDT ListView, reading GDT data directly from bochs linear mem
void FillGDT()
{
    Bit64u laddr;
    unsigned int i, j;
    unsigned int k = (GDT_Len + 1) / 8;
    char *info;
    Bit8u gdtbuf[8];
    LV_ITEM lvi = {LVIF_TEXT,0,0,0,0,(LPSTR) 0,100,0,0};
    doDumpRefresh = FALSE;

    laddr = rV[GDTRnum] & (~7);         // recover the GDT base address (force 8b align)
    CallWindowProc(wListView,hL[2],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
    ZeroDColumns();

    for(i = 0 ; i < k ; i++)
    {
        // read 2 dwords from bochs linear mem into "buffer"
        lviG.iItem = i;
        lviG.pszText = tmpce;
        sprintf(tmpce,"%02u (Selector 0x%04X)",i,i << 3);
        CallWindowProc(wListView,hL[2],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
        lvi.iItem = i;
        lvi.pszText = tmpce;
        if (ReadBxLMem(laddr, 8, gdtbuf) == FALSE)      // abort the current GDT dump on a memory error
        {
            sprintf (tmpce,"illegal address");
            lvi.iSubItem = 17;
            CallWindowProc(wListView,hL[2],LVM_SETITEMTEXT,(WPARAM) i,(LPARAM) &lvi);
            break;
        }
        laddr += 8;

        // enforce proper littleendianness on the gdtbuf bytes
        Bit32u limit = gdtbuf[0] | ((Bit32u) gdtbuf[1] << 8);
        limit |= ((Bit32u)gdtbuf[6] & 0xf) << 16;
        if ((gdtbuf[6] & 0x80) != 0)    // 'Granularity' bit = 4K limit multiplier
            limit = limit * 4096 + 4095;    // and the bottom 12 bits aren't tested

        info = GDTsT[0];    // default info string is blank
        j = 8;              // default GDT type (group) is blank
        if ((gdtbuf[5] & 0x10) == 0)    // 'S' bit clear = System segment
        {
#ifndef IS_WIN98
            lviG.iGroupId = 1;
            if (limit == 0 && (gdtbuf[5] & 0x80) == 0)      // 'P' (present) bit
                lviG.iGroupId = 0;
#endif
            // point to the approprate info string for the GDT system segment type
            info = GDTsT[(int) (gdtbuf[5] & 0xf)];
        }
        else        // it's an 'executable' code or data segment
        {
            j = (gdtbuf[6] & 0x60) >> 5;    // get the 'L' and 'D/B' bits
            if (j == 3)         // both bits set is illegal
                j = 6;
            else if ((gdtbuf[5] & 0x8) == 0)
                j += 3;         // data = 3 to 5, code = 0 to 2

#ifndef IS_WIN98
            lviG.iGroupId = j;      // use GroupIDs on XP and higher systems
#else
            info = GDTt2[j];        // otherwise, put descriptive text in "info" column
#endif
        }

        // enforce proper littleendianness on the gdtbuf bytes
        Bit32u base = gdtbuf[2] | ((Bit32u) gdtbuf[3] << 8);
        base |= (Bit32u) gdtbuf[4] << 16;
        base |= (Bit32u) gdtbuf[7] << 24;
        if ((gdtbuf[6] & 0x60) == 0x20)     // test for longmode segment
        {
            base = 0;       // the base is always 0 in longmode, with "no" limit
            sprintf(tmpcf,"0xFFFFFFFFFFFFFFFF");
        }
        else
            sprintf(tmpcf,"0x%X",limit);

        sprintf(tmpce,"0x%X",base);
        lvi.iSubItem = 1;
        CallWindowProc(wListView,hL[2],LVM_SETITEMTEXT,(WPARAM) i,(LPARAM) &lvi);

        sprintf(tmpce,"%u", (gdtbuf[5] & 0x60) >> 5);
        lvi.iSubItem = 3;
        CallWindowProc(wListView,hL[2],LVM_SETITEMTEXT,(WPARAM) i,(LPARAM) &lvi);

        lvi.iSubItem = 2;
        lvi.pszText = tmpcf;
        CallWindowProc(wListView,hL[2],LVM_SETITEMTEXT,(WPARAM) i,(LPARAM) &lvi);

        if (i == 0)
            info = GDTt2[7];    // call "Null" selector "unused"
        lvi.iSubItem = 17;
        lvi.pszText = info;
        CallWindowProc(wListView,hL[2],LVM_SETITEMTEXT,(WPARAM) i,(LPARAM) &lvi);
    }
    // 2 of the GDT columns are too narrow with a regular Autosize
    LV_COLUMN lvgc = {LVCF_TEXT,0,0,tmpce};
    strcpy (tmpce,"Index");         // header for col 0
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
    strcpy (tmpce,"Base");          // header for col 1
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 1,(LPARAM) &lvgc);
    strcpy (tmpce,"Size");          // header for col 2
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 2,(LPARAM) &lvgc);
    strcpy (tmpce,"DPL");           // header for col 3
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 3,(LPARAM) &lvgc);
    strcpy (tmpce,"Info");          // header for col 4 (17)
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 17,(LPARAM) &lvgc);
    CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, 1, LVSCW_AUTOSIZE_USEHEADER);
    CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, 3, LVSCW_AUTOSIZE_USEHEADER);
    CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
    CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, 2, LVSCW_AUTOSIZE);
    ShowWindow(hL[2],SW_SHOW);
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
    LV_ITEM lvi = {LVIF_TEXT,0,17,0,0,(LPSTR) tmpce,100,0,0};
    unsigned int mode = 0;
    if (In32Mode != FALSE)
        mode = 1;
    if (In64Mode != FALSE)
        mode = 2;
    doDumpRefresh = FALSE;

    entrysize = 4 << mode ;     // calculate the bytesize of the entries
    unsigned int k = (IDT_Len + 1) / entrysize;
    CallWindowProc(wListView,hL[2],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
    // force all column widths to 0
    ZeroDColumns();
    // recover the IDT linear base address (aligned)
    laddr = rV[IDTRnum] & (~(entrysize - 1));

    for(i = 0 ; i < k ; i++)
    {
//      sprintf(info,"0x%02X",i);
        *tmpce = AsciiHex[ 2* i ];
        tmpce[1] = AsciiHex[ 1+ 2*i ];
        tmpce[2] = 0;
        lviG.iItem = i;
        lviG.pszText = tmpce;
#ifndef IS_WIN98
        lviG.iGroupId = 8;  // IDT "group" is always blank
#endif
        int L = CallWindowProc(wListView,hL[2],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
        if (ReadBxLMem(laddr, entrysize, idtbuf) == FALSE)      // abort the current IDT dump on a memory error
        {
            sprintf (tmpce,"illegal address");
            CallWindowProc(wListView,hL[2],LVM_SETITEMTEXT,(WPARAM) L,(LPARAM) &lvi);
            break;
        }
        laddr += entrysize;
        // enforce proper littleendianness on the idtbuf bytes
        ofs = idtbuf[0] | ((Bit32u) idtbuf[1] << 8);
        sel = idtbuf[2] | ((Bit16u) idtbuf[3] << 8);

        switch (mode)
        {
            case 0:     // Real Mode
                sprintf(tmpce,"0x%04X:0x%04X", sel, ofs);
                break;

            case 1:     // Pmode
                ofs |= ((Bit32u) idtbuf[6] << 16) | ((Bit32u) idtbuf[7] << 24);
                sprintf(tmpce,"0x%04X:0x%08X", sel, ofs);
                // TODO: also print some flags from idtbuf[5], maybe, in another column
                break;

            case 2:     // Lmode
                Bit64u off64 = (Bit64u)(ofs | ((Bit32u) idtbuf[6] << 16) | ((Bit32u) idtbuf[7] << 24));
                off64 |= ((Bit64u) idtbuf[8] << 32) | ((Bit64u) idtbuf[9] << 40);
                off64 |= ((Bit64u) idtbuf[10] << 48) | ((Bit64u) idtbuf[11] << 56);
                sprintf(tmpce,"0x%04X:0x%016I64X", sel, off64);
                // TODO: also print some flags from idtbuf[5], maybe, in another column
                break;
        }

        lvi.iItem = L;
        CallWindowProc(wListView,hL[2],LVM_SETITEMTEXT,(WPARAM) L,(LPARAM) &lvi);
    }
    LV_COLUMN lvgc = {LVCF_TEXT,0,0,tmpce};
    strcpy (tmpce,"Interrupt");         // header for col 0
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
    strcpy (tmpce,"L.Address");         // header for col 1 (17)
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 17,(LPARAM) &lvgc);
    // redo col0 to handle the wide header
    CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE_USEHEADER);
    ShowWindow(hL[2],SW_SHOW);
}

// insert one entry into the Paging data list -- col0 in tmpce, col1 in tmpcf
void AddPagingLine(int LC)
{
    LV_ITEM lvi = {LVIF_TEXT,LC,17,0,0,(LPSTR) tmpcf,80,0,0};
    lviG.iItem = LC;
    lviG.pszText = tmpce;
    CallWindowProc(wListView,hL[2],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
    CallWindowProc(wListView,hL[2],LVM_SETITEMTEXT,(WPARAM) LC,(LPARAM) &lvi);
}

// lifted from bx_dbg_dump_table in dbg_main of the internal debugger
void FillPAGE()
{
    Bit32u lin, start_lin, curlin; // show only low 32 bit
    bx_phy_address phy;
    Bit64u start_phy, phy64;
    int LineCount = 0;
    doDumpRefresh = FALSE;
#ifndef IS_WIN98
    lviG.iGroupId = 8;      // Paging "group" is always blank
#endif

    CallWindowProc(wListView,hL[2],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
    curlin = lin = 0;   // always start at linear address 0
    start_lin = 1;      // force a mismatch on the first line
    start_phy = 2;
    while (LineCount < 1024 && curlin != 0xfffff000)
    {
        // get translation lin -> phys, and verify mapping is legal
        if (BX_CPU(CurrentCPU)->dbg_xlate_linear2phy(lin, &phy) != FALSE)
        {
            phy64 = phy;
            if ((lin - start_lin) != (phy64 - start_phy))
            {
                if (start_lin != 1)
                {
                    sprintf (tmpce,"0x%08X - 0x%08X",start_lin, lin - 1);
                    sprintf (tmpcf,"0x%016I64X - 0x%016I64X",start_phy, start_phy + (lin-1-start_lin));
                    AddPagingLine (LineCount);
                    ++LineCount;
                }
                start_lin = lin;
                start_phy = phy64;
            }
        }
        else
        {
            if (start_lin != 1)
            {
                sprintf (tmpce,"0x%08X - 0x%08X",start_lin, lin - 1);
                sprintf (tmpcf,"0x%016I64X - 0x%016I64X",start_phy, start_phy + (lin-1-start_lin));
                AddPagingLine (LineCount);
                ++LineCount;
            }
            start_lin = 1;
            start_phy = 2;
        }
        curlin = lin;
        lin += 0x1000;  // then test the next 4K page in the loop
    }
    if (start_lin != 1)     // need to output one last line?
    {
        sprintf (tmpce,"0x%08X - 0x%08X", start_lin, -1);
        sprintf (tmpcf,"0x%016I64X - 0x%016I64X",start_phy, start_phy -1 -start_lin);
        AddPagingLine (LineCount);
    }
    ZeroDColumns();
    LV_COLUMN lvgc = {LVCF_TEXT,0,0,tmpce};
    strcpy (tmpce,"L.Address");             // header for col 0
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
    strcpy (tmpce,"is mapped to P.Address");    // header for col 1 (17)
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 17,(LPARAM) &lvgc);
    CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
    ShowWindow(hL[2],SW_SHOW);
}


// build the stack display
void FillStack()
{
    Bit64u StackLA, EndLA;
    unsigned int len, i, wordsize, overlap;
    int j;
    bx_bool LglAddy;
    bx_bool HasNeg = FALSE;
    bx_bool UpdateDisp;
    char *cp, *cpp;

    doDumpRefresh = FALSE;
    lviG.pszText = tmpcf;
    StackLA = (Bit64u) BX_CPU(CurrentCPU)->get_laddr(BX_SEG_REG_SS, (bx_address) rV[RSP_Rnum]);

    if (PStackLA == 1)              // illegal value requests a full refresh
        PStackLA = StackLA ^ 0x4000;        // force a non-match below

    wordsize = 4;       // assume Pmode
    if (In32Mode == FALSE)
        wordsize = 2;
    else if (In64Mode != FALSE)
        wordsize = 8;
    len = STACK_ENTRIES * wordsize;

    // TODO: enforce that tmpce is wordsize aligned
    // also -- enforce that StackLA is wordsize aligned
    cp = tmpce;
    i = (unsigned int) StackLA & 0xfff; // where is stack bottom, in its 4K memory page?
    if (i > 0x1000 - len)               // does len cross a 4K boundary?
    {
        unsigned int ReadSize = 0x1000 - i;
        // read up to the 4K boundary, then try to read the last chunk
        if (ReadBxLMem (StackLA, ReadSize, (Bit8u *) cp) == FALSE)
        {
            // no data to show -- just an error message
            sprintf (tmpcf,"illegal address");
            CallWindowProc(wListView,hL[2],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
            CallWindowProc(wListView,hL[2],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
            CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
            return;
        }
        LglAddy = ReadBxLMem (StackLA + ReadSize, len + i - 0x1000, (Bit8u *) cp + ReadSize);
        if (LglAddy == FALSE)
            len = ReadSize;
    }
    else
        ReadBxLMem (StackLA, len, (Bit8u *) cp);

    UpdateDisp = CpuModeChange;     // calculate which stack entries have changed
    cp = tmpce;
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
            InvalidateRect(hL[2],0,TRUE);   // Invalidate ONCE to turn off all the red stuff
        StkInvOnce = TRUE;
        return;
    }
    CallWindowProc(wListView,hL[2],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
    StkInvOnce = FALSE;
    PStackLA = StackLA;
    cp = tmpce;
    cpp = PrevStack;
    j = len;
    while (--j >= 0)
        *(cpp++)= *(cp++);      // copy the stack to the Prev buffer
    j= STACK_ENTRIES * 8 - len;
    while (--j >= 0)
        *(cpp++)= 0;            // zero out the unused tail end of the prev buffer
    cp = tmpce;                 // the following display loop runs on the cp pointer

#ifndef IS_WIN98
        lviG.iGroupId = 8;  // Stack "group" is always blank
#endif
    LV_ITEM lvi = {LVIF_TEXT,0,0,0,0,(LPSTR) tmpcf,0,0,0};
    EndLA = StackLA + len - 1;
    i = 0;
    while (StackLA < EndLA)
    {
        lviG.iItem = i;
        lvi.iItem = i;
        lvi.iSubItem = 1;
        if (In64Mode == FALSE)
        {
            int tmp;
            sprintf (tmpcf,Fmt32b[1],StackLA);
            CallWindowProc(wListView,hL[2],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
            if (In32Mode == FALSE)
                tmp = *((Bit16s *) cp);
            else
                tmp = *((Bit32s *) cp);
            if (tmp < 0)
                HasNeg = TRUE;
            sprintf (tmpcf,"0x%X",tmp);
            CallWindowProc(wListView,hL[2],LVM_SETITEMTEXT,(WPARAM) i,(LPARAM) &lvi);
            sprintf (tmpcf,"%d",tmp);
        }
        else
        {
            Bit64s tmp = *((Bit64s *) cp);
            // don't bother testing negative -- 64b values are all the same length
            sprintf (tmpcf,Fmt64b[1],StackLA);
            CallWindowProc(wListView,hL[2],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
            sprintf (tmpcf,Fmt64b[1],tmp);
            CallWindowProc(wListView,hL[2],LVM_SETITEMTEXT,(WPARAM) i,(LPARAM) &lvi);
            sprintf (tmpcf,"%I64d",tmp);
        }
        lvi.iSubItem = 17;
        CallWindowProc(wListView,hL[2],LVM_SETITEMTEXT,(WPARAM) i,(LPARAM) &lvi);
        StackLA += wordsize;
        cp += wordsize;
        ++i;
    }
    if (StackSized == 0 || (StackSized == 1 && HasNeg != FALSE))
    {
        // force all column widths to 0
        ZeroDColumns();
        LV_COLUMN lvgc = {LVCF_TEXT,0,0,tmpce};
        strcpy (tmpce,"L.Address");             // header for col 0
        CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
        strcpy (tmpce,"Value"); // header for col 1
        CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 1,(LPARAM) &lvgc);
        strcpy (tmpce,"(dec.)");    // header for col 2 (17)
        CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 17,(LPARAM) &lvgc);
        CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
        CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, 1, LVSCW_AUTOSIZE);
        StackSized = 1;     // may need to resize a second time
        if (HasNeg != FALSE)
            StackSized = 2;
    }
    ShowWindow(hL[2],SW_SHOW);
}


// recurse displaying each leaf/branch of param_tree -- with values for each leaf
void MakeBL(HTREEITEM h_P, bx_param_c *p)
{
    HTREEITEM h_new;
    bx_list_c *as_list = NULL;
    int i = 0;
    strcpy (tmpce, p->get_name());
    int j = strlen (tmpce);
    tvis.item.cChildren = 0;
    switch (p->get_type())
    {
        case BXT_PARAM_NUM:
            if (((bx_param_num_c*)p)->get_base() == BASE_DEC)
                sprintf (tmpce + j,": %I64d",((bx_param_num_c*)p)->get64());
            else
                sprintf (tmpce + j,": 0x%0I64X",((bx_param_num_c*)p)->get64());
            break;
        case BXT_LIST:
            as_list = (bx_list_c *)p;
            tvis.item.cChildren = 1;
            i = as_list->get_size();
            break;
        case BXT_PARAM_BOOL:
            sprintf (tmpce + j,": %s",((bx_param_bool_c*)p)->get()?"true":"false");
            break;
        case BXT_PARAM_ENUM:
            sprintf (tmpce + j,": %s",((bx_param_enum_c*)p)->get_selected());
            break;
        case BXT_PARAM_STRING:
            if (((bx_param_string_c*)p)->get_options()->get() & bx_param_string_c::RAW_BYTES)
            {
                char *cp = tmpce + j;
                unsigned char *rp = (unsigned char *)((bx_param_string_c*)p)->getptr();
                char sc = ((bx_param_string_c*)p)->get_separator();
                int k = ((bx_param_string_c*)p)->get_maxsize();
                *(cp++) = ':';
                *(cp++) = ' ';
                while (k-- > 0)
                {
                    *(cp++) = AsciiHex[2* *rp];
                    *(cp++) = AsciiHex[2* *rp + 1];
                    *(cp++) = sc;
                }
                *--cp = 0;  // overwrite the last separator char
            } 
            else
                sprintf (tmpce + j,": %s",((bx_param_string_c*)p)->getptr());
            break;
        case BXT_PARAM_DATA:
            sprintf (tmpce + j,": binary data, size=%d",((bx_shadow_data_c*)p)->get_size());
    }
    tvis.hParent = h_P;     // create leaves/recurse branches
    h_new = (HTREEITEM) CallWindowProc(wTreeView,hT,TVM_INSERTITEM,(WPARAM) 0,(LPARAM) &tvis);
    if (i > 0)
    {
        while (--i >= 0)
            MakeBL(h_new, as_list->get(i));
        // sort this level of the tree alphabetically
//      CallWindowProc(wTreeView,hT,TVM_SORTCHILDREN,(WPARAM) 0,(LPARAM) h_new);
    }
}


void FillPTree()
{
    HTREEITEM h_PTroot;
    int i;
    extern bx_list_c *root_param;
    // Note: don't multithread this display -- the user expects it to complete
    doDumpRefresh = FALSE;
    ShowWindow(hL[2],SW_HIDE);
    TreeView_DeleteAllItems(hT);
    strcpy (tmpce, "bochs parameter tree");
    tvis.hParent = NULL;        // create the root node
    tvis.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_STATE;
    tvis.hInsertAfter = (HTREEITEM)TVI_FIRST;
    tvis.item.pszText = tmpce;
    tvis.item.cchTextMax = 250;
    tvis.item.cChildren = 1;
    tvis.item.state = TVIS_EXPANDED;        // it must be expanded to show the first layer
    tvis.item.stateMask = TVIS_EXPANDED;
    h_PTroot = TreeView_InsertItem(hT,&tvis);
    tvis.item.mask = TVIF_TEXT | TVIF_CHILDREN; // don't expand any other layers
    i = root_param->get_size();
    while (--i >= 0)
        MakeBL(h_PTroot, root_param->get(i));
    // sort this level of the tree alphabetically
//  CallWindowProc(wTreeView,hT,TVM_SORTCHILDREN,(WPARAM) 0,(LPARAM) h_PTroot);
    ShowWindow(hT,SW_SHOW);
}


// performs endian byteswapping the hard way, for a Data dump
void FillDataX(char* t,char C,bx_bool doHex)
{
    char *d = tmpce + 100;  // the beginning of tmpce is being used by ShowData/SD_Thread
    if (isLittleEndian == FALSE || doHex == FALSE)
        d = t + strlen(t);  // bigendian can always be appended directly
    *d = C;
    d[1] = 0;
    if (isprint(C) == 0)
        *d = '.';

    if (doHex != FALSE)
    {
        *d = AsciiHex[ 2* (unsigned char)C ];
        d[1] = AsciiHex[ 1+ 2* (unsigned char)C ];
        d[2] = 0;
        if (isLittleEndian != FALSE)    // little endian => reverse hex digits
        {
            strcat(d,t);
            strcpy(t,d);    // so append the new bytes to the FRONT of t
        }
    }
}

// do the ShowData display work asynchronously, as a thread
void ShowData()
{
    unsigned int i;
    char *x;
    LV_ITEM lvi = {LVIF_TEXT,0,0,0,0,(LPSTR) 0,80,0,0};

    doDumpRefresh = FALSE;
//  ShowWindow(hL[2],SW_HIDE);      // Hiding the windows causes way too much flash
    CallWindowProc(wListView,hL[2],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);

    lviG.pszText = tmpce;
#ifndef IS_WIN98
    lviG.iGroupId = 8;      // MemDump "group" is always blank
#endif
    x = DataDump;       // data dumps are ALWAYS 4K
    for(i = 0 ; i < 4096 ; i += 16)
    {
        if (In64Mode == FALSE)
            sprintf(tmpce,"0x%08X",(Bit32u) (DumpStart + i));
        else
            sprintf(tmpce,"0x%016I64X",DumpStart + i);
        lviG.iItem = i>>4;
        int L = CallWindowProc(wListView,hL[2],LVM_INSERTITEM,(WPARAM) 0,(LPARAM) &lviG);
        lvi.iItem = L;

        *tmpce = 0;
        *tmpcf = 0;
        for(unsigned int y = 0 ; y < 16 ; y++)
        {
            if ((DumpInAsciiMode & 1) != 0)
                FillDataX(tmpcf,x[y],FALSE);

            if ((DumpInAsciiMode & 2) != 0)
            {
                FillDataX(tmpce,x[y],TRUE);
                if (((y + 1) & (DumpAlign - 1)) == 0)
                {
                    lvi.pszText = tmpce;
                    lvi.iSubItem = y+2-DumpAlign;
                    CallWindowProc(wListView,hL[2],LVM_SETITEMTEXT,(WPARAM) L,(LPARAM) &lvi);
                    *tmpce = 0;     // FillDataX APPENDS, so you need to clear the buffer
                }
            }
        }
        lvi.pszText = tmpcf;
        lvi.iSubItem = 17;
        if ((DumpInAsciiMode & 1) != 0)
            CallWindowProc(wListView,hL[2],LVM_SETITEMTEXT,(WPARAM) L,(LPARAM) &lvi);
        x+= 16;         // bump to the next row of data
    }

    CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
    if (PrevDAD != DumpAlign)
    {
        for(i = 1 ; i < 17 ; i++)
        {
            // if dumping ONLY in ascii, 0 out ALL the other columns
            if (((i - 1) & (DumpAlign - 1)) == 0 && (DumpInAsciiMode & 2) != 0)
                CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, i, LVSCW_AUTOSIZE);
            else            // force the width of unused columns to 0
                CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, i, 0);
        }
    }
    PrevDAD = DumpAlign;
    ShowWindow(hL[2],SW_SHOW);
}


// build Register "display" names from lower case names
// (must build the pointer list while building the names)
void MakeRDnames()
{
    char *p, *c;

    p = RDispName[0];       // first storage location
    for (int i=0; i < 41; i++)
    {
        RDispName[i] = p;   // create the Name pointer
        c = RegLCName[i];   // Register name in lower case

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

    i = TotCPUs;
    while (--i >= 0)
        BX_CPU(i)->mode_break = FALSE;
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
    p -= 512;
    tmpcf = p;
    p -= 512;
    tmpce = p;
    p -= 512;
    tmpcd = p;
    p -= 512;
    AsciiHex = p;       // storage for the binary->ascii table
    p -= 512;           // 2 "hex" bytes per byte value
    tmpcb = p;

    i = TOT_REG_NUM;        // fake up a color table -- there are just enough, currently
    int j = 7;      // color 7 = orange
    while (i > 0)
    {
        // change color when the loop goes below the base register number
//      if (i == TRXR) --j;     // 5 TRX registers -- currently don't exist
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

// Autosize "fixed size" ListView columns, every time the font changes.
// This also runs once during the init sequence.
void doAutoSize()
{
    ResizeColmns = TRUE;        // force the "variable size" columns to resize, too
    // resize the "fixed size" columns (once only, perhaps)
    if (doOneTimeInit == TRUE)
        CallWindowProc(wListView, hL[1], LVM_SETCOLUMNWIDTH, 1, LVSCW_AUTOSIZE_USEHEADER);  // "bytes" column
    CallWindowProc(wListView, hL[0], LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE_USEHEADER);      // RegName column

    for (int i = 0; i < 17; i++)
        CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, i, LVSCW_AUTOSIZE_USEHEADER);
}

// exit GDT/IDT/Paging/Stack/Tree -- back to the MemDump window
void ShowMemData()
{
    LV_COLUMN lvgc = {LVCF_TEXT,0,0,tmpcb};
    ShowWindow(hT,SW_HIDE);
    ShowWindow(hL[2],SW_SHOW);
    DViewMode = VIEW_MEMDUMP;       // returning to MemDump mode
    CallWindowProc(wTreeView,hT,TVM_DELETEITEM,(WPARAM) 0,(LPARAM) TVI_ROOT);
    // need to fix the MemDump column "names"
    strcpy (tmpcb,"0");                 // rebuild header for col 1
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 1,(LPARAM) &lvgc);
    // headers for cols 1 to 5, are just numbers 0 to 4
    *tmpcb = '1';
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 2,(LPARAM) &lvgc);
    *tmpcb = '2';
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 3,(LPARAM) &lvgc);
    strcpy (tmpcb,"Ascii");                 // rebuild header for col 17
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 17,(LPARAM) &lvgc);
    if (DumpInitted == FALSE)
    {
        // GDT/IDT/PAGING data may be in the window
        CallWindowProc(wListView,hL[2],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
        strcpy (tmpcb,"Address");           // "generic" header for col 0
        CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
        int i = 17;     // never change the size of the last column
        while (--i >= 0)        // can't repopulate -- just resize
            CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, i, LVSCW_AUTOSIZE_USEHEADER);
    }
    else
    {
        strcpy (tmpcb,DC0txt[DumpMode]);    // "real" header for col 0
        CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
        PrevDAD = 0;            // force a column resize
        ShowData();             // repopulates & resizes all columns
    }
}


// refill whichever "data window" (hL[2]) is active -- or param_tree
void RefreshDataWin()
{
    switch (DViewMode)
    {
        case VIEW_MEMDUMP:
            if (DumpInitted != FALSE)
                ShowData();
            else
                ShowWindow(hL[2],SW_SHOW);
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
        case VIEW_PTREE:
            FillPTree();
    }
}

// performs tasks whenever the simulation "breaks"
void OnBreak()
{
    int i = EFER_Rnum + 1;
    // check if Ptime has changed
    NewPtime = bx_pc_system.time_ticks();
    if (PrevPtime == NewPtime)      // if not, nothing really changed
    {
        UpdateStatus();     // Updates if there really was a status change, at least
        return;
    }
    // display the new ptime on the status bar
    sprintf (tmpcb,"t= %I64d",NewPtime);
    SendMessage(hS_S,SB_SETTEXT,2,(LPARAM) tmpcb);
    PrevPtime = NewPtime;

    // remember register values from before the last run
    while (--i >= 0)
        PV[i] = rV[i];
    ladrmin = ladrmax;      // invalidate any old linear->phys mapping

    // then detect current CPU mode the *right* way -- look for changes
    // TODO: create param Objects for CS.d_b and cpu_mode for each CPU
    CpuMode = BX_CPU(CurrentCPU)->get_cpu_mode();
    if (CpuMode == BX_MODE_LONG_64)
    {
        if (In64Mode == FALSE)          // Entering LongMode?
        {
            CpuModeChange = TRUE;
            In64Mode = TRUE;
            In32Mode = TRUE;            // In32Mode must be TRUE in LongMode
            ResizeColmns = TRUE;        // if so, some formatting has changed
            StackSized = 0;
        }
    }
    else
    {
        bx_bool d_b = BX_CPU(CurrentCPU)->sregs[BX_SEG_REG_CS].cache.u.segment.d_b;
        if (In32Mode != d_b || In64Mode != FALSE)
        {
            CpuModeChange = TRUE;
            In64Mode = FALSE;
            In32Mode = d_b;
        }
    }
    if (CpuModeChange != FALSE)
    {
        if (In64Mode == FALSE)
            EnableMenuItem (hOptMenu, CMD_EREG, MF_GRAYED);
        else
            EnableMenuItem (hOptMenu, CMD_EREG, MF_ENABLED);
        BottomAsmLA = ~0;       // force an ASM autoload
        StatusChange = TRUE;
    }
    doUpdate();     // do a full "autoupdate"
    if (doDumpRefresh != FALSE)
        RefreshDataWin();
}

int HexFromAsk(char* ask,char* b)       // this routine converts a user-typed hex string into binary bytes
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

bx_bool FindHex(unsigned char* b1,int bs,unsigned char* b2,int by)
{
    // search bs bytes of b1
    for(int i = 0 ; i < bs ; i++)       // TODO: this loop could be a little more efficient.
    {                       // -- it just scans an input byte string against DataDump memory
        bx_bool Match = TRUE;
        for(int y = 0 ; y < by ; y++)
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

// load new memory for a MemDump
// newDS = illegal (1) is a flag to ask the user for a DumpStart address
bx_bool InitDataDump(int dType, Bit64u newDS)
{
    bx_bool retval = TRUE;
    bx_bool MsgOnErr = FALSE;
    char *s = tmpcb;
    if (((int) newDS & 0xf) != 0)       // legal addys must be on 16byte boundary
    {
        if (In64Mode == FALSE)
            sprintf(tmpcb,"0x%X",(Bit32u) DumpStart);
        else
            sprintf(tmpcb,"0x%I64X",DumpStart);
        if (AskText(hY,"4K Memory Dump","4K Memory Dump -- Enter Address (use 0x for hex):",tmpcb,0) == FALSE)
            return FALSE;

        while (*s == ' ')       // allow user to enter whitespace
            ++s;
        if (*s == '0' && (s[1] =='X' || s[1] == 'x'))   // and either hex or decimal
            sscanf (s+2,"%I64X",&newDS);
        else
            sscanf (s,"%I64d",&newDS);
        newDS &= ~15;                   // force Mem Dump to be 16b aligned
        MsgOnErr = TRUE;
    }

    // load 4k DataDump array from bochs emulated linear (dType=0) or physical memory
    if (dType != 0)
    {
        // cannot read linear mem across a 4K boundary -- so break the read in two
        // -- calculate location of 4K boundary (h):
        unsigned int len = (int) newDS & 0xfff;
        unsigned int i = 4096 - len;
        Bit64u h = newDS + i;
        retval = ReadBxLMem (newDS,i,(Bit8u *)DataDump);
        if (retval != FALSE && len != 0)
            retval = ReadBxLMem (h,len,(Bit8u *)DataDump + i);
    }
    else
        retval = (bx_bool) bx_mem.dbg_fetch_mem( BX_CPU(CurrentCPU),
            (bx_phy_address)newDS, 4096, (Bit8u *)DataDump);
    if (retval == FALSE)
    {
    // assume that the DataDump array is still valid -- fetch_mem should error without damage
        if (MsgOnErr != FALSE)
            MessageBox (hY,"Address range was not legal memory","Memory Error", MB_ICONERROR);
        return retval;
    }
    EnableMenuItem (hCmdMenu, CMD_WPTWR, MF_GRAYED);
    EnableMenuItem (hCmdMenu, CMD_WPTRD, MF_GRAYED);
    DumpInitted = TRUE;     // OK to refresh the Dump window in the future (it has data)
    DumpStart = newDS;
    DumpMode = dType;       // finalize dump mode, since it worked
    // update column 0 to say whether its physmem or linear
    LV_COLUMN lvgc = {LVCF_TEXT,0,0,DC0txt[DumpMode]};
    CallWindowProc(wListView,hL[2],LVM_SETCOLUMN,(WPARAM) 0,(LPARAM) &lvgc);
    ShowMemData();      // Display DataDump using preset parameters
    return TRUE;
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
        SetWindowPos(hBTN[1],0,0,0,i,LstTop,SWP_SHOWWINDOW);
        SetWindowPos(hBTN[2],0,i,0,i,LstTop,SWP_SHOWWINDOW);
        SetWindowPos(hBTN[3],0,i*2,0,i,LstTop,SWP_SHOWWINDOW);
        SetWindowPos(hBTN[4],0,i*3,0,i,LstTop,SWP_SHOWWINDOW);
        SetWindowPos(hBTN[0],0,i*4,0,WinSizeX - 4*i,LstTop,SWP_SHOWWINDOW);
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
    // The status win may not use all of its space: overlap lists or Input by STATUS_WIN_OVERLAP pixels
    i = WinSizeY - sY - j;
    if (ShowIOWindows == FALSE)
    {
        // need to keep caret functions in PAIRS
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
    SetFocus(hE_O);     // focus on Output window works best
}

// hh is the handle to the main parent window -- hY may not work yet
void MoveLists(HWND hh)
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
        // but unfortunately, SetWindowLong does not work properly on the treeview!
//      if (j == 2)
//          k=SetWindowLong ( hT, GWL_STYLE, TVS_DISABLEDRAGDROP | TVS_HASLINES | TVS_HASBUTTONS | WS_CHILD | WS_BORDER );
//      else
//          k=SetWindowLong ( hT, GWL_STYLE, TVS_DISABLEDRAGDROP | TVS_HASLINES | TVS_HASBUTTONS | WS_CHILD );
    }

    // On at least some versions of Win, you cannot mess with the positions
    // of subclassed ListView controls that still HAVE DATA IN THEM.
    // Therefore, delete all ListView data --
    //  then refresh the 3 main ListView windows afterward

    CallWindowProc(wListView,hL[2],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
    CallWindowProc(wListView,hL[1],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
    CallWindowProc(wListView,hL[0],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);

    SetWindowPos(hL[0],0,StrtCol[0],LstTop,ListWidthPix[0],ListVerticalPix,SWP_HIDEWINDOW);
    SetWindowPos(hL[1],0,StrtCol[1],LstTop,ListWidthPix[1],ListVerticalPix,SWP_HIDEWINDOW);
    SetWindowPos(hL[2],0,StrtCol[2],LstTop,ListWidthPix[2],ListVerticalPix,SWP_HIDEWINDOW);
    // tree window = same size/position as dump window
    SetWindowPos(hT,0,StrtCol[2],LstTop,ListWidthPix[2],ListVerticalPix,SWP_HIDEWINDOW);

    // calculate the SCREEN coordinates of the two "divider bars"
    POINT pt;
    pt.y = 0;
    pt.x = BarClix[0];
    ClientToScreen(hh,&pt);
    BarScrx[0] = (unsigned short) pt.x;
    pt.y = 0;
    pt.x = BarClix[1];
    ClientToScreen(hh,&pt);
    BarScrx[1] = (unsigned short) pt.x;

    // size the last column of each list = total width of that list
    CallWindowProc(wListView, hL[0], LVM_SETCOLUMNWIDTH, 2, ListWidthPix[0]);
    CallWindowProc(wListView, hL[1], LVM_SETCOLUMNWIDTH, 2, ListWidthPix[1]);
    CallWindowProc(wListView, hL[2], LVM_SETCOLUMNWIDTH, 17, ListWidthPix[2]);
    doAutoSize();   // autosize all other ListView columns for the new width
    BottomAsmLA = ~0;       // force an ASM autoload next time, to resize it
    PrevDAD = 0;            // force Data list autosize on next refresh
    doDumpRefresh = TRUE;   // force a data window refresh on a break
    if (AtBreak != FALSE)   // can't refresh the windows until a break!
    {
        doUpdate();         // refresh the ASM and Register windows
        RefreshDataWin();   // and whichever data window is up
    }
    else
        BottomAsmLA = ~0;       // force an ASM autoload, to repaint
}

// redraw/recalculate everything if the old window sizes are invalid
void doShowAll(HWND hh)
{
    RECT rc;
    GetClientRect(hh,&rc);
    SetSizeIOS((int)rc.right,(int)rc.bottom);   // window sizes are font-dependent
    // must clear listviews before MoveLists()
    CallWindowProc(wListView,hL[2],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
    CallWindowProc(wListView,hL[1],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
    CallWindowProc(wListView,hL[0],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
    MoveLists(hh);
    doAutoSize();
}
                
// User is changing which registers are displaying in the Register list
void ToggleSeeReg(int LW)
{
    int i = LW - CMD_EREG;
    if (i < 0 || i > 7)
        return;
    if (i == 4 || i == 5)
        ResizeColmns = TRUE;    // may need to resize the register value column

    SeeReg[i] ^= TRUE;
    if (SeeReg[i] == FALSE)
        CheckMenuItem (hOptMenu, LW, MF_UNCHECKED);
    else
        CheckMenuItem (hOptMenu, LW, MF_CHECKED);
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
    // the wordsize popup is the 11th entry in the Opt menu -- COUNTING SEPARATORS
        HMENU wsMenu = GetSubMenu (hOptMenu, WS_POPUP_IDX);
        CheckMenuItem (wsMenu, j, MF_UNCHECKED | MF_BYPOSITION);
        CheckMenuItem (wsMenu, i, MF_CHECKED | MF_BYPOSITION);
        DumpAlign = 1<<i;
        if (DViewMode == VIEW_MEMDUMP && DumpInitted != FALSE)
            ShowData();
    }
}

void ToggleGDT()
{
    EnableMenuItem (hCmdMenu, CMD_WPTWR, MF_GRAYED);
    EnableMenuItem (hCmdMenu, CMD_WPTRD, MF_GRAYED);
    if (DViewMode == VIEW_GDT || (GDT_Len & 7) != 7 || (unsigned) GDT_Len >= 0x10000)
    {
        if (DViewMode != VIEW_GDT)
            MessageBox(hY,"GDT limit is illegal",0,MB_OK);
        ShowMemData();
    }
    else
    {
        ShowWindow(hT,SW_HIDE);
        CallWindowProc(wTreeView,hT,TVM_DELETEITEM,(WPARAM) 0,(LPARAM) TVI_ROOT);
        // force all column widths to 0, then autosize later
        FillGDT();
        DViewMode = VIEW_GDT;       // displaying a GDT
    }
}

void ToggleIDT()
{
    EnableMenuItem (hCmdMenu, CMD_WPTWR, MF_GRAYED);
    EnableMenuItem (hCmdMenu, CMD_WPTRD, MF_GRAYED);
    if (DViewMode == VIEW_IDT || (IDT_Len & 3) != 3 || (unsigned) IDT_Len >= 0x10000)
    {
        if (DViewMode != VIEW_IDT)
            MessageBox(hY,"IDT limit is illegal",0,MB_OK);
        ShowMemData();
    }
    else
    {
        ShowWindow(hT,SW_HIDE);
        CallWindowProc(wTreeView,hT,TVM_DELETEITEM,(WPARAM) 0,(LPARAM) TVI_ROOT);
        FillIDT();
        DViewMode = VIEW_IDT;       // displaying an IDT
    }
}

void TogglePAGE()
{
    EnableMenuItem (hCmdMenu, CMD_WPTWR, MF_GRAYED);
    EnableMenuItem (hCmdMenu, CMD_WPTRD, MF_GRAYED);
    if (DViewMode == VIEW_PAGING)
        ShowMemData();
    else
    {
        ShowWindow(hT,SW_HIDE);
        CallWindowProc(wTreeView,hT,TVM_DELETEITEM,(WPARAM) 0,(LPARAM) TVI_ROOT);
        FillPAGE();
        DViewMode = VIEW_PAGING;    // currently displaying Paging info
    }
}

void ToggleStack()
    {
    EnableMenuItem (hCmdMenu, CMD_WPTWR, MF_GRAYED);
    EnableMenuItem (hCmdMenu, CMD_WPTRD, MF_GRAYED);
    if (DViewMode == VIEW_STACK)
        ShowMemData();
    else
        {
        ShowWindow(hT,SW_HIDE);
        CallWindowProc(wTreeView,hT,TVM_DELETEITEM,(WPARAM) 0,(LPARAM) TVI_ROOT);
        StackSized = 0;     // need to autosize the stack display columns
        PStackLA = 1;       // flag to force a full refresh
        FillStack();
        DViewMode = VIEW_STACK;     // currently displaying stack
    }
}

void TogglePTree()
{
    EnableMenuItem (hCmdMenu, CMD_WPTWR, MF_GRAYED);
    EnableMenuItem (hCmdMenu, CMD_WPTRD, MF_GRAYED);
    if (DViewMode == VIEW_PTREE)
        ShowMemData();
    else
    {
        DViewMode = VIEW_PTREE;     // currently displaying param_tree
        CallWindowProc(wListView,hL[2],LVM_DELETEALLITEMS,(WPARAM) 0,(LPARAM) 0);
        FillPTree();        // get all info from param_tree into tree-view window
    }
}


void doFind()
{
    unsigned int i, L;
    *tmpcb = 0;
        // prepare to read ASM column 2, then set Selected state of each line on/off
    LV_ITEM lvi = {LVIF_STATE,0,2,0,LVIS_SELECTED,(LPSTR) tmpcb,100,0,0};
    if (DumpHasFocus == FALSE)
    {
        if (AskText(hY,"Find text in mnemonic lines","ASM Search text:",tmpcb,0) == FALSE)
            return;
        if (strchr(tmpcb,'*') == 0 && strchr(tmpcb,'?') == 0)
            sprintf(tmpcd,"*%s*",tmpcb);
        else
            strcpy(tmpcd,tmpcb);

        if (UprCase != FALSE)   // convert search string to uppercase if ASM is that way
            upr(tmpcd);
        L = CallWindowProc(wListView,hL[1],LVM_GETITEMCOUNT,(WPARAM) 0,(LPARAM) 0);
        for(i = 0 ; i < L ; i++)
        {
            lvi.mask = LVIF_TEXT;
            lvi.iItem = i;
            // get column 2 text from ASM window
            CallWindowProc(wListView,hL[1],LVM_GETITEMTEXT,(WPARAM) i,(LPARAM) &lvi);
            lvi.mask = LVIF_STATE;
            lvi.state = 0;          // assume unselected
            // if it's a match, set the state to Selected
            if (IsMatching(tmpcb,tmpcd,TRUE))
                lvi.state = LVIS_SELECTED;
            CallWindowProc(wListView,hL[1],LVM_SETITEMSTATE,(WPARAM) i,(LPARAM) &lvi);
        }
    }
    else
    {
        if (AskText(hY,"Memory Dump Search",
            "Sequential hex bytes (e.g 00FEFA - no spaces), or ascii string (max. 16b):",tmpcb,0) == FALSE)
            return;

        int by = HexFromAsk(tmpcb,tmpcd);       // by = len of binary search string

        // Find in all rows of 16 bytes -- must do rows, so they can be selected
        for(i = 0,L = 0 ; i < 4096 ; i += 16, L++)
        {
            lvi.iItem = L;
            lvi.state = 0;          // assume unselected
            // if it's a match, change state to Selected
            if (by != 0 && FindHex((unsigned char *)DataDump + i,16,(unsigned char *)tmpcd,by))
                lvi.state = LVIS_SELECTED;
            CallWindowProc(wListView,hL[2],LVM_SETITEMSTATE,(WPARAM) L,(LPARAM) &lvi);
        }

        // Try ascii for additional matches and selected lines
        lvi.state = LVIS_SELECTED;
        strcpy(tmpcd,tmpcb);
        by = strlen(tmpcd);
        for(i = 0,L = 0 ; i < 4096 ; i += 16, L++)
        {
            lvi.iItem = L;
            if (by != 0 && FindHex((unsigned char *)DataDump + i,16,(unsigned char *)tmpcd,by))
                CallWindowProc(wListView,hL[2],LVM_SETITEMSTATE,(WPARAM) L,(LPARAM) &lvi);
        }
    }
}

void doStepN()
    {
    Bit32u i;
    char *s = tmpcb;
    sprintf (tmpcb,"%d",PrevStepNSize);
    if (AskText(hY,"Singlestep N times","Number of steps (use 0x for hex):",tmpcb,0) == FALSE)
        return;
    while (*s == ' ')       // allow user to enter whitespace
        ++s;
    if (*s == '0' && (s[1] =='X' || s[1] == 'x'))   // and either hex or decimal
        sscanf (s+2,"%x",&i);
    else
        sscanf (s,"%d",&i);
    if (i == 0)
        return;
    PrevStepNSize = i;
    AtBreak = FALSE;
    StatusChange = TRUE;
    bx_dbg_stepN_command(i);
    AtBreak = TRUE;
    StatusChange = TRUE;
    OnBreak();
}

void ShowFW()
{
    FWflag = TRUE;
MessageBox(hY,"With more than 1000 lines in the ASM window, be careful not to minimize the app.\r\nIt will be very slow to reopen.",
        "Friendly warning:",MB_ICONINFORMATION);
}

// User wants a custom disassembly
void doDisAsm()
{
    char *s = tmpcb;
    int NumLines = DefaultAsmLines;
    Bit64u h;
    sprintf (tmpcb,"0x%I64X",CurrentAsmLA);
    if (AskText(hY,"Disassemble",
        "Disassemble -- Enter Linear Start Address (use 0x for hex):",tmpcb,0) == FALSE)
        return;
    while (*s == ' ')       // allow user to enter whitespace
        ++s;
    if (*s == '0' && (s[1] =='X' || s[1] == 'x'))   // and either hex or decimal
        sscanf (s+2,"%I64x",&h);
    else
        sscanf (s,"%I64d",&h);
    sprintf (tmpcb,"%d",NumLines);
    if (AskText(hY,"Disassemble","Number of lines: (Max. 2048)",tmpcb,0) == FALSE)
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
void SetBreak()
{
    int L = CallWindowProc(wListView, hL[1], LVM_GETNEXTITEM,(WPARAM) -1,MAKELPARAM(LVNI_SELECTED, 0));
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
            int BpId = bx_dbg_lbreakpoint_command(bkRegular, nbrk);
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
        // start the next search at item L
        L = CallWindowProc(wListView, hL[1], LVM_GETNEXTITEM,(WPARAM) L,MAKELPARAM(LVNI_SELECTED, 0));
    }
}

void SetWatchpoint(unsigned * num_watchpoints, bx_phy_address * watchpoint)
{
    int iExist1 = -1;
    int i = (int) *num_watchpoints;
    // the list is unsorted -- test all of them
    while (--i >= 0)
    {
        if (watchpoint[i] == SelectedDataAddress)
        {
            iExist1 = i;
            i = 0;
        }
    }
    if (iExist1 >= 0)
    {
        // existing watchpoint, remove by copying the list down
        while (++iExist1 < (int) *num_watchpoints)
            watchpoint[iExist1 - 1] = watchpoint[iExist1];
        -- *num_watchpoints;
    }
    else
    {
        // Set a watchpoint to last clicked address -- the list is not sorted
        if (*num_watchpoints >= 16)
            MessageBox (hY,"Too many of that type of watchpoint. Max. 16",
                "Table Overflow", MB_ICONERROR) ;
        else
            watchpoint[(*num_watchpoints)++] = (bx_phy_address) SelectedDataAddress;
    }
}

// Main Window Proc for our Dialog
LRESULT CALLBACK B_WP(HWND hh,UINT mm,WPARAM ww,LPARAM ll)
{
    unsigned int i;
    extern unsigned num_write_watchpoints;
    extern unsigned num_read_watchpoints;
    extern bx_phy_address write_watchpoint[];
    extern bx_phy_address read_watchpoint[];

    switch(mm)
    {
        case WM_CREATE:
        {
            HFONT hF = (HFONT)GetStockObject(OEM_FIXED_FONT);
            HFONT hF2 = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

            // Register Window
            char* txt0[] = {"Reg Name","Hex Value","Decimal"};
            LV_COLUMN lvc = {LVCF_SUBITEM | LVCF_TEXT,LVCFMT_LEFT,0,txt0[0]};
            hL[0] = CreateWindowEx(0,"sLV","",LVStyle[0],0,0,1,1,hh,(HMENU)1001,GetModuleHandle(0),0);
            // Note; WM_CREATE only happens once, so don't bother eliminating these SendMessage macros
            ListView_InsertColumn(hL[0],0,&lvc);
            lvc.pszText = txt0[1];
            ListView_InsertColumn(hL[0],1,&lvc);
            lvc.pszText = txt0[2];
            ListView_InsertColumn(hL[0],2,&lvc);

        //      Enable the groupID's for the register window
#ifndef IS_WIN98
            // GroupID's are only supported on XP or higher -- verify Win Version
            // the group stuff may COMPILE correctly, but still may fail at runtime
            Bit8u MajWinVer, MinWinVer;
            Bit32u PackedVer = GetVersion();
            bx_bool Group_OK = TRUE;
            MajWinVer = (Bit8u)(PackedVer & 0xff);      // Major version # is in the LOW byte
            MinWinVer = (Bit8u)((PackedVer>>8) & 0xff);
            if (MajWinVer > 5 || (MajWinVer == 5 && MinWinVer >= 1)     // is it XP or higher?
            {
                wchar_t* txt[] = {L"General Purpose",L"Segment",L"Control",L"MMX",L"SSE",L"Debug",L"Test",L"Other"};
                ListView_EnableGroupView(hL[0],TRUE);
                for(i = 0 ; i < 8 ; i++)
                {
                    LVGROUP group1 = {0};
                    group1.cbSize = sizeof(LVGROUP);
                    group1.mask = LVGF_HEADER | LVGF_GROUPID;
                    group1.pszHeader = txt[i];
                    group1.iGroupId = i;
                    ListView_InsertGroup(hL[0], -1, &group1);
                }
            }
            else
                Group_OK = FALSE;
#endif
            SendMessage(hL[0], LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,
                LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);


            // Asm Window
            hL[1] = CreateWindowEx(0,"sLV","",LVStyle[1] | WS_BORDER,0,0,1,1,hh,(HMENU)1000,GetModuleHandle(0),0);
            CurCenterList = 1;          // ASM window starts with the border
            char* txt3[] = {"L.Address","Bytes","Mnemonic"};

            lvc.pszText = txt3[0];
            ListView_InsertColumn(hL[1],0,&lvc);
            lvc.pszText = txt3[1];
            ListView_InsertColumn(hL[1],1,&lvc);
            lvc.pszText = txt3[2];
            ListView_InsertColumn(hL[1],2,&lvc);
            SendMessage(hL[1], LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,
                LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);


            // Input / Output
            hE_I = CreateWindowEx(0,"edit","",WS_CHILD | WS_VISIBLE,0,0,1,1,hh,(HMENU)1004,GetModuleHandle(0),0);
            // without AUTOHSCROLL, output window text is always supposed to wordwrap
            hE_O = CreateWindowEx(0,"edit","",WS_VSCROLL | WS_BORDER | WS_CHILD | WS_VISIBLE | ES_MULTILINE,0,0,1,1,hh,(HMENU)1003,GetModuleHandle(0),0);
            SendMessage(hE_I,WM_SETFONT,(WPARAM)hF2,MAKELPARAM(TRUE,0));
            SendMessage(hE_O,WM_SETFONT,(WPARAM)hF,MAKELPARAM(TRUE,0));
            // subclass both the edit windows together
            *wEdit = (WNDPROC) SetWindowLong (hE_I,GWL_WNDPROC,(long) ThisEditProc);
            wEdit[1] = (WNDPROC) SetWindowLong (hE_O,GWL_WNDPROC,(long) ThisEditProc);

            // Status
            hS_S = CreateWindowEx(0,STATUSCLASSNAME,"",WS_CHILD | WS_VISIBLE,0,0,1,1,hh,(HMENU)1006,GetModuleHandle(0),0);
            SendMessage(hS_S,WM_SETFONT,(WPARAM)hF2,MAKELPARAM(TRUE,0));
            int p[4] = {100,250,400,-1};
            SendMessage(hS_S,SB_SETPARTS,4,(LPARAM)p);

            // Dump Window
            hL[2] = CreateWindowEx(0,"sLV","",LVStyle[2],0,0,1,1,hh,(HMENU)1002,GetModuleHandle(0),0);
            //SendMessage(hL[2],WM_SETFONT,(WPARAM)hF2,MAKELPARAM(TRUE,0));
            strcpy (bigbuf, "Address");
            lvc.pszText = bigbuf;
            ListView_InsertColumn(hL[2],0,&lvc);
            bigbuf[1] = 0;                  // use bigbuf to create 1 byte "strings"
            for(i = 1 ; i < 17 ; i++)
            {
                if (i < 11)
                    *bigbuf = i - 1 + '0';
                else
                    *bigbuf = i - 11 + 'A';
                ListView_InsertColumn(hL[2],i,&lvc);
            }
            strcpy (bigbuf, "Ascii");
            ListView_InsertColumn(hL[2],17,&lvc);
            SendMessage(hL[2], LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,
                LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

#ifndef IS_WIN98
            if (Group_OK)
            {
                wchar_t* txt5[] = {L"16-bit code",L"64-bit code",L"32-bit code",L"16-bit data",L"64-bit data",L"32-bit data",L"Illegal"};
                ListView_EnableGroupView(hL[2],TRUE);
                LVGROUP group1 = {0};
                group1.cbSize = sizeof(LVGROUP);
                group1.mask = LVGF_HEADER | LVGF_GROUPID;
                for(int i = 0 ; i < 9 ; i++)
                {
                    group1.pszHeader = txt5[i];
                    group1.iGroupId = i;
                    ListView_InsertGroup(hL[2], -1, &group1);
                }
            }
#endif
            hT = CreateWindowEx(0,WC_TREEVIEW,"",TVS_DISABLEDRAGDROP | TVS_HASLINES | TVS_HASBUTTONS | WS_CHILD | WS_BORDER,
                0,0,1,1,hh,(HMENU)1010,GetModuleHandle(0),0);
            //SendMessage(hT,WM_SETFONT,(WPARAM)hF2,MAKELPARAM(TRUE,0));
            // Use the same messagehandler for the tree window as for ListViews
            wTreeView = (WNDPROC) SetWindowLong (hT,GWL_WNDPROC,(long) LVProc);

/*          TotCPUs = SIM->get_param_num(BXPN_CPU_NPROCESSORS)->get() *
                     SIM->get_param_num(BXPN_CPU_NCORES)->get() *
                     SIM->get_param_num(BXPN_CPU_NTHREADS)->get();
            if (TotCPUs == 0 || TotCPUs > BX_MAX_SMP_THREADS_SUPPORTED)
                TotCPUs = 1;    // this case will never happen
*/
            TotCPUs = BX_SMP_PROCESSORS;

            // create button rows
            CurrentCPU = 0;
            int j = TotCPUs;
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

            hCursResize = LoadCursor (NULL,IDC_SIZEWE);
            hCursDock = LoadCursor (NULL,IDC_CROSS);
            SetTimer(hh,2,500,NULL);    // get timer ticks every half a second (update VGAW)
            DoAllInit();                // any custom GUI initialization goes in here
            SetFocus(hE_O);     // focus on Output window works best
            break;
        }

        case WM_SIZE:
        {
            RECT rc;
            int i, j, k;

            // Mapping mode is MM_TEXT (Def), so Yvalues increase DOWN, all values are in pixels.
            GetClientRect(hh,&rc);
            // don't resize child windows on a minimize, or if client window is too small
            // TODO: save the prev size values -- no resize if they are the same
            //   -- also can skip some resizing steps if horizontal size does not change
            if (rc.bottom > 100 && rc.right > 150)
            {
                // set up the Input/Output/Status windows
                SetSizeIOS((int)rc.right,(int)rc.bottom);

                // need to recalculate List widths, with the same proportions as before
                i = ListWidthPix[0] + ListWidthPix[1] + ListWidthPix[2];
                if (i == 0)
                    i = 1;
                j = (ListWidthPix[2] * rc.right) / i;
                k = (ListWidthPix[1] * rc.right) / i;
                ListWidthPix[0] = rc.right - k - j;     // Register list
                ListWidthPix[1] = k;                    // Asm
                ListWidthPix[2] = j;                    // MemDump
                MoveLists(hh);
            }
            break;
        }

        case WM_SYSKEYDOWN:     // ww == key + ALT
        {
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
            else if (ww == VK_F3) ToggleSeeReg(139);        // MMX/FPU toggle
#endif
            else if (ww == VK_F7)           // Alt+F7 memdump hex toggle
            {
                i = DumpInAsciiMode;
                i ^= 2;
                if (i != 0)
                {
                    DumpInAsciiMode = i;
                    if ((DumpInAsciiMode & 2) != 0)
                        CheckMenuItem (hOptMenu, 122, MF_CHECKED);
                    else
                        CheckMenuItem (hOptMenu, 122, MF_UNCHECKED);
                    PrevDAD = 0;        // force columns to resize
                    ShowData();
                }
            }
            break;
        }

        case WM_KEYDOWN:
        {
            int Control = GetKeyState(VK_CONTROL);  // key is down if val is negative   
            int Shift = GetKeyState(VK_SHIFT);

            switch (ww)
            {
              case VK_ESCAPE:
                CommandHistoryIdx = 0;
                SetWindowText(hE_I,"");
                ShowMemData();              // force a "normal" MemDump window
                break;

              case VK_UP:
                // History from nextmost previous command
                CommandHistoryIdx = (CommandHistoryIdx - 1) & 63;   // wrap -1 to 63 (circular)
                SetWindowText(hE_I,CmdHistory[(CmdHInsert + CommandHistoryIdx) & 63]);
                CallWindowProc(*wEdit, hE_I, EM_SETSEL,(WPARAM) -1,(LPARAM) -1);
                CallWindowProc(*wEdit, hE_I, EM_SCROLLCARET,(WPARAM) 0,(LPARAM) 0);
                break;

              case VK_DOWN:
                // Next History command
                CommandHistoryIdx = (CommandHistoryIdx + 1) & 63;   // wrap -1 to 63 (circular)
                SetWindowText(hE_I,CmdHistory[(CmdHInsert + CommandHistoryIdx) & 63]);
                CallWindowProc(*wEdit, hE_I, EM_SETSEL,(WPARAM) -1,(LPARAM) -1);
                CallWindowProc(*wEdit, hE_I, EM_SCROLLCARET,(WPARAM) 0,(LPARAM) 0);
                break;

              case VK_PRIOR:
                // Page up on the MemDump window by 2K
                if (DumpInitted != FALSE)
                    InitDataDump(DumpMode, DumpStart - 2048);
                break;

              case VK_NEXT:
                // Page down on the MemDump window by 2K
                if (DumpInitted != FALSE)
                    InitDataDump(DumpMode, DumpStart + 2048);
                break;

              case VK_F2:
                if (Control < 0)
                    ToggleGDT();
                else if (Shift < 0)
                    ToggleIDT();
                else
                    ToggleStack();
                break;

              case VK_F3:       // ^F3 = param tree, F3 = toggle syntax
                if (Control < 0)
                    TogglePTree();
                else
                {
                    Bit64u h = CurrentAsmLA;
                    bx_disassemble.toggle_syntax_mode();
                    // do the standard ASM window fill sequence
                    CanDoLA(&h);
                    FillAsm(h, DefaultAsmLines);
                }
                break;

              case VK_F4:
                if (Shift < 0)          // Debug register toggle
                    ToggleSeeReg(CMD_DREG);                 
                else if (Control >= 0)      // Refresh
                {
                    BottomAsmLA = ~0;       // force an ASM autoload
                    doDumpRefresh = TRUE;   // force a data window refresh on a break
                    if (AtBreak != FALSE)   // can't refresh the windows until a break!
                    {
                        doUpdate();         // refresh the ASM and Register windows
                        RefreshDataWin();   // and whichever data window is up
                    }
                }
#if BX_SUPPORT_SSE
                else ToggleSeeReg(CMD_XMMR);    // SSE toggle
#endif
                break;

              case VK_F5:
                    // can't continue until everything is ready
                if (AtBreak != FALSE && debug_cmd_ready == FALSE && dbgOn)
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

              case VK_F7:
                if (Control < 0)
                    InitDataDump(0,(Bit64u)  1);    // ^F7 = PhysDump
                else if (Shift < 0)         // ShiftF7 = ascii toggle
                {
                    i = DumpInAsciiMode;
                    i ^= 1;
                    if (i != 0)
                    {
                        DumpInAsciiMode = i;
                        if ((DumpInAsciiMode & 1) != 0)
                            CheckMenuItem (hOptMenu, CMD_MASCII, MF_CHECKED);
                        else
                            CheckMenuItem (hOptMenu, CMD_MASCII, MF_UNCHECKED);
                        PrevDAD = 0;        // force columns to resize
                        ShowData();
                    }
                }
                else
                    InitDataDump(1,(Bit64u) 1);     // F7 = LinDump
                break;

              case VK_F6:
                if (Control < 0)        // ^F6 = Read Watchpt
                {
                    if (DumpHasFocus != FALSE)
                        SetWatchpoint(&num_read_watchpoints,read_watchpoint);
                }
                else if (Shift < 0)         // ShiftF6 = Modechange brk toggle
                {
                    // toggle mode_break on cpu0, use that value to reset all CPUs
                    bx_bool nmb = BX_CPU(0)->mode_break ^ TRUE;
                    int j = TotCPUs;
                    while (--j >= 0)
                        BX_CPU(j)->mode_break = nmb;
                    if (nmb != FALSE)
                        CheckMenuItem (hOptMenu, CMD_MODEB, MF_CHECKED);
                    else
                        CheckMenuItem (hOptMenu, CMD_MODEB, MF_UNCHECKED);
                }
                else
                {
                    if (DumpHasFocus == FALSE)      // F6 Brkpt / Write Watchpt
                        SetBreak();
                    else
                        SetWatchpoint(&num_write_watchpoints,write_watchpoint);
                }
                break;

              case VK_F11:
                if (AtBreak != FALSE && debug_cmd_ready == FALSE && dbgOn)
                {
                    bx_dbg_stepN_command(1);        // singlestep
                    StatusChange = TRUE;
                }
                break;

              case VK_F9:
                    // can't run sim until everything is ready
                if (AtBreak != FALSE && debug_cmd_ready == FALSE && dbgOn)
                    doStepN();      // ask user for a step #
                break;

              case 'C':         // ^c = break
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

              case VK_RIGHT:    // send a few virtual movement keys back into the Input window
              case VK_LEFT:
              case VK_END:
              case VK_HOME:
                CallWindowProc (*wEdit, hE_I, mm, ww, ll);
                break;

              case VK_RETURN:
                // can't do a command until everything is ready
                if (AtBreak != FALSE && debug_cmd_ready == FALSE && dbgOn)
                {
                    *tmpcb = 0;
                    GetWindowText(hE_I,tmpcb,200);
                    if (*tmpcb == 0)            // Hitting <CR> on a blank line means SINGLESTEP
                    {
                        bx_dbg_stepN_command(1);        // singlestep
                    }
                    else
                    {
                        // deal with the command history:
                        if (strlen(tmpcb) > 79)
                            MessageBox (hY,"Running command, but history has an 80 char Max.",
                                "Command history overflow", MB_OK);
                        else
                        {
                            strcpy (CmdHistory[CmdHInsert], tmpcb);
                            CmdHInsert = (CmdHInsert + 1) & 63;     // circular buffer, 0 to 63
                        }
                        strcpy (debug_cmd,tmpcb);   // send the command into the bochs internal debugger
                        debug_cmd_ready = TRUE;
                        AtBreak = FALSE;
                        SetWindowText(hE_I,"");     // clear the "input" window for the next command
                        CommandHistoryIdx = 0;      // and reset the history queue to the new end
                    }
                    StatusChange = TRUE;
                }
            }       // end the switch
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
                    PrevPtime = 0;          // force a full update
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

            switch(LW)
            {

                case CMD_CONT: // run/go/continue
                    if (AtBreak != FALSE && debug_cmd_ready == FALSE && dbgOn)
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
                    if (AtBreak != FALSE && debug_cmd_ready == FALSE && dbgOn)
                    {
                        bx_dbg_stepN_command(1);        // singlestep
                        StatusChange = TRUE;
                    }
                    break;

                case CMD_STEPN: // step N
                        // can't run sim until everything is ready
                    if (AtBreak != FALSE && debug_cmd_ready == FALSE && dbgOn)
                        doStepN();
                    break;

                case CMD_BREAK: // break/stop the sim
                    // SIM->debug_break() only "break"s the internal debugger
                    SIM->debug_break();
                    break;

                case CMD_BRKPT: // set or delete a breakpoint at the selected address
                    SetBreak();
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
                    doDumpRefresh = TRUE;   // force a data window refresh on a break
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

                case CMD_CMEM:  // view current MemDump -- acts like "cancel"
                    CommandHistoryIdx = 0;
                    SetWindowText(hE_I,"");
                    ShowMemData();              // force a "normal" MemDump window
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
                    bx_bool nmb = BX_CPU(0)->mode_break ^ TRUE;
                    int j = TotCPUs;
                    while (--j >= 0)
                        BX_CPU(j)->mode_break = nmb;
                    if (nmb != FALSE)
                        CheckMenuItem (hOptMenu, CMD_MODEB, MF_CHECKED);
                    else
                        CheckMenuItem (hOptMenu, CMD_MODEB, MF_UNCHECKED);
                    break;
                }

                case CMD_DADEF: // set default # of disassembly lines in a list
                    sprintf (tmpcb,"%d",DefaultAsmLines);
                    if (AskText(hh,"Disassembly default linecount","Max. 2048:",tmpcb,0) == FALSE)
                        return 0;
                    sscanf (tmpcb,"%d",&i);
                    if (i > 0 && i <= 2048)
                        DefaultAsmLines = i;
                    if (i > 1000 && FWflag == FALSE)    // friendly warning
                        ShowFW();
                    break;

                case CMD_ATTI:      // Toggle ASM Syntax
                {
                    bx_disassemble.toggle_syntax_mode();
                    // do the standard ASM window fill sequence
                    Bit64u h = CurrentAsmLA;
                    CanDoLA(&h);
                    FillAsm(h, DefaultAsmLines);
                    break;
                }

                case CMD_IOWIN:  // toggle display of internal debugger Input and Output windows
                    ShowIOWindows ^= TRUE;
                    if (ShowIOWindows != 0)
                        CheckMenuItem (hOptMenu, CMD_IOWIN, MF_CHECKED);
                    else
                        CheckMenuItem (hOptMenu, CMD_IOWIN, MF_UNCHECKED);
                    doShowAll(hh);
                    break;

                case CMD_SBTN:      // Toggle showing top pushbutton-row
                    ShowButtons ^= TRUE;
                    if (ShowButtons != 0)
                        CheckMenuItem (hOptMenu, CMD_SBTN, MF_CHECKED);
                    else
                        CheckMenuItem (hOptMenu, CMD_SBTN, MF_UNCHECKED);
                    doShowAll(hh);
                    break;

                case CMD_UCASE:     // Toggle showing everything in uppercase
                {
                    UprCase ^= 1;
                    if (UprCase != 0)
                        CheckMenuItem (hOptMenu, CMD_UCASE, MF_CHECKED);
                    else
                        CheckMenuItem (hOptMenu, CMD_UCASE, MF_UNCHECKED);
                    MakeRDnames();
                    // the ASM window will be updated when it gets updated
                    LoadRegList();
                    // do the standard ASM window fill sequence
                    Bit64u h = CurrentAsmLA;
                    CanDoLA(&h);
                    FillAsm(h, DefaultAsmLines);
                    break;
                }

                case CMD_MHEX:      // Toggle showing hex in Dump window
                    i = DumpInAsciiMode;
                    i ^= 2;
                    if (i != 0)
                    {
                        DumpInAsciiMode = i;
                        if ((DumpInAsciiMode & 2) != 0)
                            CheckMenuItem (hOptMenu, CMD_MHEX, MF_CHECKED);
                        else
                            CheckMenuItem (hOptMenu, CMD_MHEX, MF_UNCHECKED);
                        PrevDAD = 0;        // force columns to resize
                        if (DViewMode == VIEW_MEMDUMP && DumpInitted != FALSE)
                            ShowData();
                    }
                    break;

                case CMD_MASCII:    // Toggle showing ASCII in Dump window
                    i = DumpInAsciiMode;
                    i ^= 1;
                    if (i != 0)
                    {
                        DumpInAsciiMode = i;
                        if ((DumpInAsciiMode & 1) != 0)
                            CheckMenuItem (hOptMenu, CMD_MASCII, MF_CHECKED);
                        else
                            CheckMenuItem (hOptMenu, CMD_MASCII, MF_UNCHECKED);
                        PrevDAD = 0;        // force columns to resize
                        if (DViewMode == VIEW_MEMDUMP && DumpInitted != FALSE)
                            ShowData();
                    }
                    break;

                case CMD_LEND:      // Toggle Endianness for the MemDumps
                    isLittleEndian ^= TRUE;
                    if (isLittleEndian != FALSE)
                        CheckMenuItem (hOptMenu, CMD_LEND, MF_CHECKED);
                    else
                        CheckMenuItem (hOptMenu, CMD_LEND, MF_UNCHECKED);
                    if (DViewMode == VIEW_MEMDUMP && DumpInitted != FALSE)
                        ShowData();
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
                    if (ignSSDisasm != FALSE)
                        CheckMenuItem (hOptMenu, CMD_IGNSA, MF_CHECKED);
                    else
                        CheckMenuItem (hOptMenu, CMD_IGNSA, MF_UNCHECKED);
                    break;

                case CMD_IGNNT:     // Toggle NextT ignoring
                    ignoreNxtT ^= TRUE;
                    if (ignoreNxtT != FALSE)
                        CheckMenuItem (hOptMenu, CMD_IGNNT, MF_CHECKED);
                    else
                        CheckMenuItem (hOptMenu, CMD_IGNNT, MF_UNCHECKED);
                    break;

                case CMD_RCLR:      // Toggle Register Coloring
                    SeeRegColors ^= TRUE;
                    if (SeeRegColors != FALSE)
                        CheckMenuItem (hOptMenu, CMD_RCLR, MF_CHECKED);
                    else
                        CheckMenuItem (hOptMenu, CMD_RCLR, MF_UNCHECKED);
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
                    ToggleSeeReg(LW);
                    break;

                case CMD_ABOUT:     // "About" box
                    MessageBox(hh,"Bochs Enhanced Debugger, Version 1.0\r\nCopyright (C) Chourdakis Michael.\r\nModified by Bruce Ewing",
                        "About",MB_OK | MB_ICONINFORMATION);
                    break;

                case CMD_FONT: // font
                    HFONT hF = (HFONT) CallWindowProc(*wEdit,hE_I,WM_GETFONT,0,0);
                    LOGFONT lf;
                    GetObject(hF,sizeof(lf),&lf);
                    CHOOSEFONT cF = {0};
                    cF.lStructSize = sizeof(cF);
                    cF.hwndOwner = hh;
                    cF.lpLogFont = &lf;
                    cF.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;
                    if (ChooseFont(&cF) == FALSE)
                        return 0;

                    ShowWindow(hL[0],SW_HIDE);
                    ShowWindow(hL[1],SW_HIDE);
                    ShowWindow(hL[2],SW_HIDE);
                    ShowWindow(hS_S,SW_HIDE);
                    ShowWindow(hE_I,SW_HIDE);
                    if (CustomFont != NULL)
                        DeleteObject (CustomFont);
                    CustomFont = CreateFontIndirect(&lf);
                    DeleteObject(hF);
                    CallWindowProc(wListView,hL[0],WM_SETFONT,(WPARAM)CustomFont,MAKELPARAM(TRUE,0));
                    CallWindowProc(wListView,hL[1],WM_SETFONT,(WPARAM)CustomFont,MAKELPARAM(TRUE,0));
                    CallWindowProc(wListView,hL[2],WM_SETFONT,(WPARAM)CustomFont,MAKELPARAM(TRUE,0));
                    CallWindowProc(*wEdit,hE_I,WM_SETFONT,(WPARAM)CustomFont,MAKELPARAM(TRUE,0));
                    SendMessage(hS_S,WM_SETFONT,(WPARAM)CustomFont,MAKELPARAM(TRUE,0));
                    doShowAll(hh);
            }
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
            if (n->code == NM_CUSTOMDRAW && n->hwndFrom == hL[1])   // custom drawing of ASM window
            {
                // highlight the breakpoints, and current opcode, if any
                NMLVCUSTOMDRAW *d = (NMLVCUSTOMDRAW *) ll;
                if (d->nmcd.dwDrawStage == CDDS_PREPAINT)
                    return CDRF_NOTIFYITEMDRAW;

                if (d->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)   // select the "active" ASM line
                {
                    d->clrTextBk = RGB(255,255,255);        // background is white
                    if (!AtBreak)
                        d->clrTextBk = RGB(210,210,210);    // unless sim is "running"

                    unsigned char r=0, g=0, b=0;        // normal text color is black

                    bx_address h = (bx_address) AsmLA[d->nmcd.dwItemSpec];
                    if (h == CurrentAsmLA)
                        g = 100;                // current opcode is colored dark green
                    int j= BreakCount;
                    while (--j >= 0)            // loop over all breakpoints
                    {
                        // brk list is sorted -- if the value goes too low, end the loop
                        // And I know for a fact that some complers are soooo stupid that they
                        // will repeat the following test twice, unless you force them not to.
                        register bx_address i = BrkLAddr[j] - h;
//                      if (BrkLAddr[j] < h)
                        if (i < 0)
                            j = 0;      // force the loop to end if it goes too far
//                      else if (BrkLAddr[j] == h)
                        else if (i == 0)
                        {
                            g = j= 0;
                            if (h == CurrentAsmLA)
                                b = 200;        // breakpoint @ current opcode = blue
                            else
                                r = 150;        // active breakpoint is red
                        }
                    }
                    d->clrText = RGB(r,g,b);
                }
                break;
            }
            if (n->code == NM_CUSTOMDRAW && n->hwndFrom == hL[2])   // custom drawing of data window
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

                if (DumpAlign != 1 || DumpMode != 0 || DViewMode != VIEW_MEMDUMP)
                    break;

                // highlight any physical watchpoints in physical DumpMode
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
                            if (write_watchpoint[--j] == h)
                            {
                                d->clrTextBk = RGB(0,0,0);      // black background
                                d->clrText = RGB(255,0,150);    // write watchpoint
                                j = -1;         // on a match j<0 -- else j == 0
                            }
                        }
                        while (--i >= 0)
                        {
                            if (read_watchpoint[i] == h)
                            {
                                if (j < 0)          // BOTH read and write
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
            if (n->code == NM_CUSTOMDRAW && n->hwndFrom == hL[0])   // custom drawing of register window
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
            if (n->code == NM_DBLCLK)
            {
                if (AtBreak == FALSE)
                    break;

                if (n->hwndFrom == hL[0])
                {
                    // Change a register
                    int L = CallWindowProc(wListView, hL[0], LVM_GETNEXTITEM,(WPARAM) -1,MAKELPARAM(LVNI_SELECTED, 0));
                    if (L == -1 || L >= TOT_REG_NUM)
                        break;      // should be impossible

                    int i = RitemToRnum[L];
                    if (i > EFER_Rnum)      // only able to do up to EFER for now
                        break;
                    char *d1;
                    d1 = RDispName[i];
                    sprintf (tmpcb,"0x%I64X", rV[i]);
                    if (AskText(hh,"Change Register Value",d1,tmpcb,0))
                    {
                        Bit64s val;
                        char *s = tmpcb;
                        while (*s == ' ')       // allow user to enter whitespace
                            ++s;
                        if (*s == '0' && (s[1] =='X' || s[1] == 'x'))   // and either hex or decimal
                            sscanf (s+2,"%I64X",&val);
                        else
                            sscanf(tmpcb,"%I64d",&val);
                        RegObject[CurrentCPU][i]->set(val);         // the set function should be a bool, not a void
//                      bx_bool worked = RegObject[CurrentCPU][i]->set(val);
//                      if (worked == FALSE)
//                          MessageBox (hh,"Bochs does not allow you to set that register","Selection Error", MB_ICONERROR) ;
//                      else
                            LoadRegList();      // update the register window
                    }
                }
                if (n->hwndFrom == hL[1])   // doubleclick a breakpoint on ASM window
                    SetBreak();

                if (n->hwndFrom == hL[2] && DViewMode == VIEW_MEMDUMP)  // Change values in memory locations
                {
                    int L = CallWindowProc(wListView, hL[2], LVM_GETNEXTITEM,(WPARAM) -1,MAKELPARAM(LVNI_SELECTED, 0));
                    i = 0;      // flag if setting watchpoints
                    if (GetKeyState(VK_SHIFT) < 0)
                    {
                        i = 1;
                        SetWatchpoint(&num_write_watchpoints,write_watchpoint);
                    }
                    if (GetKeyState(VK_CONTROL) < 0)
                    {
                        i = 1;
                        SetWatchpoint(&num_read_watchpoints,read_watchpoint);
                    }
                    if (L == -1 || i != 0)      // L should never be -1, Exit after a watchpoint.
                        break;

                    // get base address of "line" of data -- each line (L) is 16 bytes
                    Bit64u h = DumpStart + (L<<4);

                    if (DumpMode == 0)
                        sprintf(tmpcb,"Physical Address: 0x%I64X",h);
                    else
                        sprintf(tmpcb,"Linear Address: 0x%I64X",h);

                    unsigned char *u = (unsigned char *)(DataDump + (L<<4));    // important that it be unsigned!
                    sprintf(tmpcd,"%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
                        *u,u[1],u[2],u[3],u[4],u[5],u[6],u[7],u[8],u[9],u[10],u[11],u[12],u[13],u[14],u[15]);

                    if (AskText(hh,"Change Memory Values",tmpcb,tmpcd,0))
                    {
                        Bit8u newval;
                        int err=0;
                        char *x = tmpcd;
                        upr(x);         // force input string to uppercase

                        if (DumpMode != 0)      // is h is a LINEAR address? Convert to physical!
                        {
                            // use the ReadBx function to calculate the lin->phys offset
                            if (ReadBxLMem (h,0,(Bit8u *)tmpcb) == FALSE)   // "read" 0 bytes
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
                                sscanf (s,"%2X",&newval);
                                if ( bx_mem.dbg_set_mem( (bx_phy_address) h, 1, &newval) == FALSE )
                                    err = 2;
                                ++h;                    // bump to the next mem address
                                while (*x == ' ')       // scan past whitespace
                                    ++x;
                            }
                        }

                        if (err != 0)
                        {
                            if (err == 1)
                                MessageBox (hh,"Improper char hex format","Input Format Error", MB_ICONERROR);
                            else
                                MessageBox (hh,"Illegal memory address error?","Memory Error", MB_ICONERROR);
                        }
                        ShowData();     // refresh the data dump, even if there were errors
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
                    ParseUnknown ("\n");
            }
            UpdateStatus();
            DEV_vga_refresh();
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
            if (CustomFont != NULL)
                DeleteObject (CustomFont);
            DestroyWindow(hDebugDialog);
            hDebugDialog = NULL;
            UnregisterClass("sLV",GetModuleHandle(0));
            break;
        }
        case WM_USER:
        {
            // ww == 0x1234, ll = 0 - bochs started running a command
            //          11 = 1 - bochs stopped on some type of break 
            //          ll = 2 - bochs is ready to run a command
            // ww == 0x5678 - output text from debugger in available in lparam

            if (ww == 0x1234 && ll == 2)
                dbgOn = TRUE;       // bochs internal debugger is able to accept commands now

            if (ww != 0x1234 || ll != 0)
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

            if (ww == 0x5678)   // bochs internal debugger produced some text?
            {
                if (strstr((char*)ll,"HALTED"))
                    break;

                ParseUnknown((char*)ll);    // Process all debugger text output
            }
            return 0;
        }
    }
    return DefWindowProc(hh,mm,ww,ll);
}

ATOM BRegister()
{
    InitCommonControls();
    SpListView();
    SpBtn();
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
    ATOM P1 = RegisterClassEx(&wC);
    return P1;
}

void RefreshDebugDialog()
{
}

void InitDebugDialog(HWND mainwnd)
{
    BRegister();
    HMENU hTopMenu = LoadMenu(GetModuleHandle(0),"MENU_1");
    hOptMenu = GetSubMenu (hTopMenu, 2);
    hViewMenu = GetSubMenu (hTopMenu, 1);
    hCmdMenu = GetSubMenu (hTopMenu, 0);
    hY = CreateWindowEx(0,"bochs_dbg_x","Bochs Enhanced Debugger (Type 2)",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
        0,hTopMenu,GetModuleHandle(0),0);
    if (hY == NULL)
        DebugBreak();

    hDebugDialog = hY;
    return;
}

#endif
