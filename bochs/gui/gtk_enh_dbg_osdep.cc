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
#include "enh_dbg.h"

#include <gtk/gtk.h>
#include <glib.h>
#include <pthread.h>

// replace the Win32 COLORREF and RGB definitions, for GDK
#define  RGB(R,G,B) {(R<<16)|(G<<8)|B,(R*65535)/255,(G*65535)/255,(B*65535)/255}

// User Customizable Register color table:
// background "register type" colors indexed by RegColor value
const GdkColor ColorList[16] = {
    RGB(255,255,255),   // white
    RGB(200,255,255),   // blue (aqua)
    RGB(230,230,230),   // gray
    RGB(248,255,200),   // yellow
    RGB(216,216,255),   // purple
    RGB(200,255,200),   // green
    RGB(255,230,200),   // orange
    RGB(255,255,255),   // more user definable
    RGB(255,255,255),
    RGB(255,255,255),
    RGB(255,255,255),
    RGB(255,255,255),
    RGB(255,255,255),
    RGB(255,255,255),
    RGB(255,255,255),
    RGB(255,255,255)
};

// END of User Customizable settings

#define TreeParent  GtkTreeIter

void DockResize (int j, Bit32u x);      // need some function prototypes
void SetHorzLimits();
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
void MakeBL(TreeParent *tp, bx_param_c *p);

#define NUM_CHKS    25  // slight overestimate of # of Optmenu items with checkmarks
#define WSChkIdx    20  // "checkmark index" of the first "wordsize" menu item

// reverse mapping from command indexes to menu item widgets
GtkWidget *Cmd2MI[CMD_IDX_HI - CMD_IDX_LO + 1];

// GTK objects and global stuff
GtkTextMark* EndMark;
GtkTreeViewColumn *AllCols[24];
GtkCellRenderer *LV_Rend[3];
GdkCursor *SizeCurs;
GdkCursor *DockCurs;
gulong mmov_hID;

    // GTK containers
GtkWidget *window;
GtkWidget *menubar;
GtkWidget *CmdMenu;
GtkWidget *ViewMenu;
GtkWidget *OptMenu;
GtkWidget *HelpMenu;
GtkWidget *MainVbox;
GtkWidget *CmdBHbox;
GtkWidget *CpuBHbox;
GtkWidget *StatHbox;
GtkWidget *TreeTbl;
GtkWidget *IOVbox;
GtkWidget *ScrlWinOut;
GtkWidget *ScrlWin[3];
GtkWidget *LVHbox;
GtkWidget *LVEvtBox;
GtkWidget *VSepEvtBox1;
GtkWidget *VSepEvtBox2;

    // GTK menu widgets -- titles, then actual menu items
GtkWidget *CmdTitle;
GtkWidget *ViewTitle;
GtkWidget *OptTitle;
GtkWidget *HelpTitle;

GtkWidget *ContMI;
GtkWidget *StepMI;
GtkWidget *StepNMI;
GtkWidget *BreakMI;
GtkWidget *SetBrkMI;
GtkWidget *WatchWrMI;
GtkWidget *WatchRdMI;
GtkWidget *FindMI;
GtkWidget *RefreshMI;

GtkWidget *PhyDumpMI;
GtkWidget *LinDumpMI;
GtkWidget *StackMI;
GtkWidget *GDTMI;
GtkWidget *IDTMI;
GtkWidget *PageMI;
GtkWidget *ViewBrkMI;
GtkWidget *MemDumpMI;
GtkWidget *PTreeMI;
GtkWidget *DisAsmMI;

GtkWidget *ChkMIs[NUM_CHKS];    // menu items with checkmarks

//GtkWidget *XceptMI;
GtkWidget *DefDisMI;
GtkWidget *SyntaxMI;
GtkWidget *FontMI;
GtkWidget *WrdSizeMI;
GtkWidget *WSmenu;

GtkWidget *AboutMI;

GtkWidget *sep1;        // menu separators
GtkWidget *sep2;
GtkWidget *sep3;
GtkWidget *sep4;
GtkWidget *sep5;
GtkWidget *sep6;
GtkWidget *sep7;
GtkWidget *sep10;

GtkWidget *sep8;        // separators around the ListViews
GtkWidget *sep9;

    // GTK widgets
GtkWidget *CmdBtn[5];                   // "command" buttonrow
GtkWidget *CpuBtn[BX_MAX_SMP_THREADS_SUPPORTED];        // "CPU" buttonrow
GtkWidget *CpuB_label[BX_MAX_SMP_THREADS_SUPPORTED];    // "labels" on CPU buttons

GtkWidget *Stat[4];     // 4 entry "Status Bar"
GtkWidget *StatVSep1;
GtkWidget *StatVSep2;
GtkWidget *StatVSep3;

GtkWidget *IEntry;      // Singleline Input Text Window
GtkWidget *OText;       // Multiline, wrapping, Output Text Window

GtkWidget *PTree;       // bochs param_tree TreeView
GtkWidget *LV[3];       // Register, ASM, MemDump / ListViews (TreeViews)

// HIHI put all these colors in an array, and use #defines for them
// need a "medium gray" color for inactive lists
const GdkColor mgray = RGB(200,200,200);
const GdkColor white = {
    0xffffff,
    0xffff,     // need to specify cell background color sometimes
    0xffff,
    0xffff
};
GdkColor fg_red = RGB(255,0,0);
GdkColor fg_black = RGB(0,0,0);
const GdkColor AsmColors[4] = {
    RGB(0,0,0),
    RGB(0,100,0),
    RGB(150,0,0),
    RGB(0,0,200)
};
const GdkColor PDumpClr[4] = {
    RGB(0,0,0),
    RGB(255,0,150),
    RGB(0,170,255),
    RGB(130,255,0)
};

unsigned int CurXSize;      // last known size of main client window

int DumpSelRow;             // keeps track of "clicks" on MemDump entries
int SelectedEntry;          // keeps track of "clicks" on ASM entries
bx_bool UpdateOWflag = FALSE;
bx_bool HitBrkflag = FALSE;
bx_bool ForcingCheck = FALSE;   // using a hotkey to flip a menu item checkmark
int KeyStateShft;
int KeyStateAlt;
// guint PrevState = GDK_BUTTON1_MASK;  // turn on the "mouseup" bit, to detect the first mousedown
int SizingDelay = 0;
bx_bool EnterSizeMode = FALSE;
unsigned int CurScrX;

//bx_bool UpdInProgress[3];     // flag -- list update incomplete (not OK to paint)

char SelMem[260];       // flag array for which list rows are "selected"
char SelAsm[MAX_ASM];
char SelReg[TOT_REG_NUM + EXTRA_REGS];

// "run" the standard dialog box -- get text from the user
bx_bool ShowAskDialog()
{
    bx_bool ret = FALSE;
    static GtkWidget *dialog = NULL;
    static GtkWidget *AskEntry = NULL;
    static GtkWidget *AskPrompt = NULL;
    const gchar *entry_text;

    if (dialog == NULL)
    {
        dialog = gtk_dialog_new ();
        gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
        gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);
        gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
    }
// HIHI should the "transient" line be INSIDE the dialog creation "if"? -- when I tried it, it crashed?
    // The following causes this dialog to stay above the main window
//  gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (window));

    gtk_window_set_title (GTK_WINDOW (dialog), ask_str.title);

    if (AskEntry == NULL)
    {
        AskPrompt = gtk_label_new (ask_str.prompt);
        gtk_widget_show (AskPrompt);
        gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), AskPrompt, TRUE, TRUE, 5);
        AskEntry = gtk_entry_new ();
        // "Enter" (activate) = OK button
        gtk_entry_set_activates_default (GTK_ENTRY (AskEntry), TRUE);
        // increase the width of the Entry to about 80 chars
        gtk_entry_set_width_chars (GTK_ENTRY (AskEntry), 80);
        gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), AskEntry, TRUE, TRUE, 5);
    }

    gtk_widget_show (AskEntry);
    gtk_label_set_text (GTK_LABEL (AskPrompt), ask_str.prompt);
    gtk_widget_grab_focus(AskEntry);        // Entry must be visible before it can get focus
    // and it must have focus before any new text is set, or the dialog will crash
    gtk_entry_set_text (GTK_ENTRY (AskEntry), ask_str.reply);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
    {
        entry_text = gtk_entry_get_text (GTK_ENTRY (AskEntry));
        strcpy (ask_str.reply, entry_text);
        ret = TRUE;
    }

    // if any text remains in the Entry, the dialog box will crash on its next use
    gtk_entry_set_text (GTK_ENTRY (AskEntry), "");      // so clear it
    gtk_widget_grab_focus(IEntry);
    gtk_widget_hide (dialog);   // for reuse!
    return ret;
}

// set all TRUE flags to checked in the Options menu
void InitMenus()
{
//L exception notify is not implemented yet
    menubar = gtk_menu_bar_new();
    CmdMenu =   gtk_menu_new();
    ViewMenu = gtk_menu_new();
    OptMenu =   gtk_menu_new();
    HelpMenu = gtk_menu_new();

    CmdTitle = gtk_menu_item_new_with_label("Commands");        // each menu needs a title
    ViewTitle = gtk_menu_item_new_with_label("View");       // (and the titles are created as widgets)
    OptTitle = gtk_menu_item_new_with_label("Options");
    HelpTitle = gtk_menu_item_new_with_label("Help");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(CmdTitle), CmdMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(ViewTitle), ViewMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(OptTitle), OptMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(HelpTitle), HelpMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), CmdTitle);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), ViewTitle);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), OptTitle);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), HelpTitle);

    // build all the menu items
    // Commands Menu
    ContMI = gtk_menu_item_new_with_label("Continue [c]\t\t\t\t\tF5");
    StepMI = gtk_menu_item_new_with_label("Step [s]\t\t\t\t\t\tF11");
    StepNMI = gtk_menu_item_new_with_label("Step #...\t\t\t\t\t\tF9");
    BreakMI = gtk_menu_item_new_with_label("Break\t\t\t\t\t\tCtrl+C");
    SetBrkMI = gtk_menu_item_new_with_label("Breakpoint (ASM selected)\t\tF6");
    WatchWrMI = gtk_menu_item_new_with_label("Watch Write (PhysDump selected)\tShift+F6");
    WatchRdMI = gtk_menu_item_new_with_label("Watch Read (PhysDump selected)\tAlt+F6");
    FindMI = gtk_menu_item_new_with_label("Find...\t\t\t\t\t\tCtrl+F");
    RefreshMI = gtk_menu_item_new_with_label("Refresh Screen\t\t\t\t\tF4");

    // View Menu
    PhyDumpMI = gtk_menu_item_new_with_label("Physical MemDump...\tCtrl+F7");
    LinDumpMI = gtk_menu_item_new_with_label("Linear MemDump...\t\tF7");
    StackMI = gtk_menu_item_new_with_label("Stack\t\t\t\tF2");
    GDTMI = gtk_menu_item_new_with_label("GDT\t\t\t\t\tCtrl+F2");
    IDTMI = gtk_menu_item_new_with_label("IDT\t\t\t\t\tShift+F2");
    PageMI = gtk_menu_item_new_with_label("Page Table\t\t\tAlt+F2");
    ViewBrkMI = gtk_menu_item_new_with_label("Breakpoints\t\t\tCtrl+F6");
    MemDumpMI = gtk_menu_item_new_with_label("Current MemDump\t\tEsc");
    PTreeMI  = gtk_menu_item_new_with_label("Bochs param_tree\t\tCtrl+F3");
    DisAsmMI = gtk_menu_item_new_with_label("Disassemble...\t\t\tCtrl+D");
    // Options Menu - without checkmarks
    DefDisMI = gtk_menu_item_new_with_label("Default disassembled lines...");
    SyntaxMI = gtk_menu_item_new_with_label("Toggle Intel/ATT syntax\t\tF3");
    FontMI  = gtk_menu_item_new_with_label ("Font...");
    WrdSizeMI = gtk_menu_item_new_with_label("Set MemDump 'Wordsize'...");
    WSmenu = gtk_menu_new();

    // Options Menu - with checkmarks
    ChkMIs[MODE_BRK] = gtk_check_menu_item_new_with_label("Break on CPU mode change\tShift+F5");
    ChkMIs[ONE_CPU] = gtk_check_menu_item_new_with_label("Show CPU0 only");
    ChkMIs[U_CASE]  = gtk_check_menu_item_new_with_label("Text in UPPERCASE");
    ChkMIs[IO_WIN]  = gtk_check_menu_item_new_with_label("Show Input/Output Windows");
    ChkMIs[SHOW_BTN] = gtk_check_menu_item_new_with_label("Show Buttons");
    ChkMIs[MD_HEX] = gtk_check_menu_item_new_with_label("MemDump in Hex\t\t\tAlt+F7");
    ChkMIs[MD_ASC] = gtk_check_menu_item_new_with_label("MemDump in ASCII\t\t\tShift+F7");
    ChkMIs[L_END]  = gtk_check_menu_item_new_with_label("LittleEndian");
    ChkMIs[IGN_SA] = gtk_check_menu_item_new_with_label("Ignore ASM lines");
    ChkMIs[IGN_NT] = gtk_check_menu_item_new_with_label("Ignore Next at t=");
    ChkMIs[R_CLR] = gtk_check_menu_item_new_with_label("Colorize Register Types");
    ChkMIs[E_REG] = gtk_check_menu_item_new_with_label("Show 32bit Registers");
    ChkMIs[S_REG] = gtk_check_menu_item_new_with_label("Show Segment Registers");
    ChkMIs[SYS_R] = gtk_check_menu_item_new_with_label("Show System Registers");
    ChkMIs[C_REG] = gtk_check_menu_item_new_with_label("Show Control Registers");
    ChkMIs[FPU_R] = gtk_check_menu_item_new_with_label("Show MMX/FPU Registers\tAlt+F3");
    ChkMIs[XMM_R] = gtk_check_menu_item_new_with_label("Show SSE Registers\t\tCtrl+F4");
    ChkMIs[D_REG] = gtk_check_menu_item_new_with_label("Show Debug Registers\t\tShift+F4");
//  ChkMIs[T_REG] = gtk_check_menu_item_new_with_label("Show Test Registers");
    ChkMIs[LOG_VIEW] = gtk_check_menu_item_new_with_label("Show log in output window");
    ChkMIs[WSChkIdx] = gtk_check_menu_item_new_with_label("1 byte\t\tAlt+1");
    ChkMIs[WSChkIdx+1] = gtk_check_menu_item_new_with_label("2 bytes\t\tAlt+2");
    ChkMIs[WSChkIdx+2] = gtk_check_menu_item_new_with_label("4 bytes\t\tAlt+4");
    ChkMIs[WSChkIdx+3] = gtk_check_menu_item_new_with_label("8 bytes\t\tAlt+8");
    ChkMIs[WSChkIdx+4] = gtk_check_menu_item_new_with_label("16 bytes\t\tAlt+6");

    // Help Menu
    AboutMI = gtk_menu_item_new_with_label("About...");

    sep1 = gtk_separator_menu_item_new();       // can't reuse menu separators
    sep2 = gtk_separator_menu_item_new();
    sep3 = gtk_separator_menu_item_new();
    sep4 = gtk_separator_menu_item_new();
    sep5 = gtk_separator_menu_item_new();
    sep6 = gtk_separator_menu_item_new();
    sep7 = gtk_separator_menu_item_new();
    sep10 = gtk_separator_menu_item_new();

    // insert all the menu items into each menu
    gtk_menu_shell_append(GTK_MENU_SHELL(CmdMenu), ContMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(CmdMenu), StepMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(CmdMenu), StepNMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(CmdMenu), BreakMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(CmdMenu), sep1);
    gtk_menu_shell_append(GTK_MENU_SHELL(CmdMenu), SetBrkMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(CmdMenu), WatchWrMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(CmdMenu), WatchRdMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(CmdMenu), sep2);
    gtk_menu_shell_append(GTK_MENU_SHELL(CmdMenu), FindMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(CmdMenu), RefreshMI);

    gtk_menu_shell_append(GTK_MENU_SHELL(ViewMenu), PhyDumpMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(ViewMenu), LinDumpMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(ViewMenu), StackMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(ViewMenu), GDTMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(ViewMenu), IDTMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(ViewMenu), PageMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(ViewMenu), ViewBrkMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(ViewMenu), MemDumpMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(ViewMenu), sep3);
    gtk_menu_shell_append(GTK_MENU_SHELL(ViewMenu), PTreeMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(ViewMenu), DisAsmMI);

    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[MODE_BRK]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[ONE_CPU]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), DefDisMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), SyntaxMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), sep4);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), FontMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[U_CASE]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[IO_WIN]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[SHOW_BTN]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), sep5);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[MD_HEX]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[MD_ASC]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[L_END]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), WrdSizeMI);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), sep6);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[IGN_SA]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[IGN_NT]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), sep7);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[R_CLR]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[E_REG]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[S_REG]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[SYS_R]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[C_REG]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[FPU_R]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[XMM_R]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[D_REG]);
//  gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[T_REG]);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), sep10);
    gtk_menu_shell_append(GTK_MENU_SHELL(OptMenu), ChkMIs[LOG_VIEW]);
    gtk_menu_shell_append(GTK_MENU_SHELL(HelpMenu), AboutMI);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(WrdSizeMI), WSmenu);        // set up popup menu
    gtk_menu_shell_append(GTK_MENU_SHELL(WSmenu), ChkMIs[WSChkIdx]);
    gtk_menu_shell_append(GTK_MENU_SHELL(WSmenu), ChkMIs[WSChkIdx+1]);
    gtk_menu_shell_append(GTK_MENU_SHELL(WSmenu), ChkMIs[WSChkIdx+2]);
    gtk_menu_shell_append(GTK_MENU_SHELL(WSmenu), ChkMIs[WSChkIdx+3]);
    gtk_menu_shell_append(GTK_MENU_SHELL(WSmenu), ChkMIs[WSChkIdx+4]);

    // init all the checkmarks

    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[E_REG]), SeeReg[0]);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[S_REG]), SeeReg[1]);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[SYS_R]), SeeReg[2]);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[C_REG]), SeeReg[3]);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[FPU_R]), SeeReg[4]);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[XMM_R]), SeeReg[5]);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[D_REG]), SeeReg[6]);
//  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ShowTRegMI), SeeReg[7]);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[ONE_CPU]), SingleCPU);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[IO_WIN]), ShowIOWindows);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[SHOW_BTN]), ShowButtons);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[U_CASE]), UprCase);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[L_END]), isLittleEndian);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[IGN_SA]), ignSSDisasm);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[IGN_NT]), ignoreNxtT);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[R_CLR]), SeeRegColors);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[LOG_VIEW]), LogView);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[WSChkIdx+DumpWSIndex]), TRUE);
    if (DumpInAsciiMode == 0)       // prevent an illegal value
        DumpInAsciiMode = 3;
    // I don't know why, but the next 2 "set_active" commands blow up if moved into SpecialInit
    if ((DumpInAsciiMode & 2) != 0)
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[MD_HEX]), TRUE);
    if ((DumpInAsciiMode & 1) != 0)
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[MD_ASC]), TRUE);
}

// add a whole row of text into a ListView, all at once
// Note: LineCount should start at 0
void InsertListRow(char *ColumnText[], int ColumnCount, int listnum, int LineCount, int grouping)
{
    char tmp[4];
    GtkTreeIter iter;
    GtkListStore *Database;
    *tmp = 0;
    Database = (GtkListStore *) gtk_tree_view_get_model( GTK_TREE_VIEW(LV[listnum]) );
    gtk_list_store_append (GTK_LIST_STORE(Database), &iter);  /* Acquire an iterator */
    // some rows of the register list only print 2 columns
    // the dump window always prints 18, but the first 17 may be blank and 0 width
    if (ColumnCount == 2)
        gtk_list_store_set (GTK_LIST_STORE(Database), &iter, 0, ColumnText[0], 1, ColumnText[1], 2,
            tmp, 3, (gint) LineCount, -1);
    else if (ColumnCount == 3)
    {
        gtk_list_store_set (GTK_LIST_STORE(Database), &iter, 0, ColumnText[0], 1, ColumnText[1], 2,
            ColumnText[2], 3, (gint) LineCount, -1);
    }
    else
    {
    // it might be easier to use the gtk_list_store_set_valuesv() function?
        gtk_list_store_set (GTK_LIST_STORE(Database), &iter, 0, ColumnText[0], 1, ColumnText[1],
            2, ColumnText[2], 3, ColumnText[3], 4, ColumnText[4], 5, ColumnText[5],
            6, ColumnText[6], 7, ColumnText[7], 8, ColumnText[8], 9, ColumnText[9],
            10, ColumnText[10], 11, ColumnText[11], 12, ColumnText[12], 13, ColumnText[13],
            14, ColumnText[14], 15, ColumnText[15], 16, ColumnText[16], 17, ColumnText[17], 18, (gint) LineCount, -1);
    }
}

void ShowFW()
{
    FWflag = TRUE;      // the X version of this should be stubbed
}

// try to get a list repaint to happen
void Invalidate(int i)
{
    GdkWindow *LstGdk;
    LstGdk = gtk_tree_view_get_bin_window (GTK_TREE_VIEW (LV[i]));
    gdk_window_invalidate_rect (LstGdk, NULL, TRUE);    // rect=NULL means "the whole window"
    gtk_widget_queue_draw(LV[i]);               // redraw the selected ListView "bin" window
}

void SetStatusText(int column, const char *buf)
{
    gtk_label_set_text(GTK_LABEL(Stat[column]), buf);
}

// called by the Find command -- only for LV[ASM] column 2!
void GetLIText(int listnum, int itemnum, int column, char *buf)
{
    char *value;
    GtkTreeIter iter;
    GtkListStore *Database;
    Database = (GtkListStore *) gtk_tree_view_get_model( GTK_TREE_VIEW(LV[listnum]) );
    // get an iter for row # itemnum
    gtk_tree_model_iter_nth_child (GTK_TREE_MODEL(Database), &iter, NULL, itemnum);
    // pull the text out of "row # iter" at the specified column
    gtk_tree_model_get(GTK_TREE_MODEL(Database), &iter, column, &value,  -1);
    strcpy (buf, value);
    g_free(value);
}

// get a bochs command from the user Input window
void GetInputEntry(char *buf)
{
    const gchar *gcp;
    gcp = gtk_entry_get_text(GTK_ENTRY(IEntry));
    strcpy (buf, gcp);
}

// highlights individual rows in ASM or Mem windows
void SetLIState(int listnum, int itemnum, bx_bool Select)
{
    char *Sel = SelAsm;
    int max = MAX_ASM;
    if (listnum == 2)
    {
        Sel = SelMem;
        max = 256;
    }
    if (itemnum >= max)
        itemnum = max-1;
    Sel[itemnum] = Select;
}

// Searches ASM or Reg list for the next (or first) selected row
int GetNextSelectedLI(int listnum, int StartPt)
{
    int L = StartPt;
    char *Sel = SelAsm;
    int end = MAX_ASM;
    bx_bool stop = FALSE;

    if (listnum == REG_WND)
    {
        Sel = SelReg;
        end = TOT_REG_NUM + EXTRA_REGS;
    }
    if (L < -1)
        L = -1;
    while (stop == FALSE)
    {
        if (++L >= end)
            stop = TRUE;
        else
        {
            if (Sel[L] != FALSE)
                stop = TRUE;
        }
    }
    if (L >= end)
        return -1;
    return L;
}

// returns the row number of the ASM list that is showing at the top of the window
// it is also easy to calculate AsmPgSize and ListLineRatio here,
// -- they MUST be calculated somewhere in the os_specific code
int GetASMTopIdx()
{
    GtkAdjustment *va;
//    GtkListStore *Database;
//    Database = (GtkListStore *) gtk_tree_view_get_model( GTK_TREE_VIEW(LV[ASM_WND]) );

    AsmPgSize = 0;
    va = gtk_tree_view_get_vadjustment ( GTK_TREE_VIEW(LV[ASM_WND]) );
    // calculate the number of vertical "pixels" in one row (as a fraction of the scroll range)
    if (AsmLineCount == 0)
        return 0;
    ListLineRatio = (int) (va->upper - va->lower) / AsmLineCount;
    if (ListLineRatio == 0)
        return 0;
    AsmPgSize = (int) va->page_size / ListLineRatio;
    return ((int) va->value / ListLineRatio);
}

// "pixels" is a multiple of (and therefore proportional to) ListLineRatio
// -- it does not technically have to really be a pixel count
void ScrollASM(int pixels)
{
    GtkAdjustment *va = gtk_tree_view_get_vadjustment ( GTK_TREE_VIEW(LV[ASM_WND]) );
    gtk_adjustment_set_value (GTK_ADJUSTMENT(va), va->value + pixels);
}

// handle checkmarks in the "wordsize" popup menu
// Note: setting/clearing a checkmark sends an immediate "activate" signal to the associated menuitem
void ToggleWSchecks(int newWS, int oldWS)
{
    if ((bx_bool) gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(ChkMIs[WSChkIdx + newWS])) == FALSE)
    {
        ForcingCheck = TRUE;        // prevent infinite checkmark loops
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[WSChkIdx + newWS]), TRUE);
    }
    if ((bx_bool) gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(ChkMIs[WSChkIdx + oldWS])) != FALSE)
    {
        ForcingCheck = TRUE;        // prevent infinite checkmark loops
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[WSChkIdx + oldWS]), FALSE);
    }
}

// repack the ScrollWindows into the ListHbox using a new DockOrder
// FirstListIndex is precalculated in MoveLists, so pass in the value
void RepackLists(int FirstListIndex)
{
    gtk_box_pack_start(GTK_BOX(LVHbox), ScrlWin[FirstListIndex], FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(LVHbox), VSepEvtBox1, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(LVHbox), ScrlWin[(DockOrder& 3) -1], FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(LVHbox), VSepEvtBox2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(LVHbox), ScrlWin[CurCenterList], TRUE, TRUE, 0);
}

// unpack the ScrollWindows from the ListHbox, then repack using a new DockOrder
// -- also calculate the X-coords of the "bars" between the lists
void MoveLists()
{
    int i;
    gtk_widget_ref (ScrlWin[0]);    // make sure the list widgets do not get deleted
    gtk_widget_ref (ScrlWin[1]);    // when they are removed from their container
    gtk_widget_ref (ScrlWin[2]);
    gtk_widget_ref (VSepEvtBox1);
    gtk_widget_ref (VSepEvtBox2);
    gtk_container_remove (GTK_CONTAINER(LVHbox), ScrlWin[0]);
    gtk_container_remove (GTK_CONTAINER(LVHbox), ScrlWin[1]);
    gtk_container_remove (GTK_CONTAINER(LVHbox), ScrlWin[2]);
    gtk_container_remove (GTK_CONTAINER(LVHbox), VSepEvtBox1);
    gtk_container_remove (GTK_CONTAINER(LVHbox), VSepEvtBox2);

    CurCenterList = ((DockOrder>>4)& 3) -1;     // index of center list
    i = (DockOrder >> 8) -1;            // index of left list

    RepackLists(i);     // pack widgets back into container in new DockOrder
    gtk_widget_unref (ScrlWin[0]);
    gtk_widget_unref (ScrlWin[1]);      // and get rid of the temporary "ref"s
    gtk_widget_unref (ScrlWin[2]);
    gtk_widget_unref (VSepEvtBox1);
    gtk_widget_unref (VSepEvtBox2);

    // use DockOrder to figure out the table X-coordinates of the 2 v-separators
    i = ListWidthPix[i];
//  BarClix[0] = i;
//  BarClix[1] = i + ListWidthPix[CurCenterList];

    BottomAsmLA = ~0;       // force an ASM autoload next time, to resize it
    doDumpRefresh = TRUE;   // force a data window refresh on a break
    if (AtBreak != FALSE)   // can't refresh the windows until a break!
    {
        doUpdate();         // refresh the ASM and Register windows
        RefreshDataWin();   // and whichever data window is up
    }
}

// modify the displayed items in the MainVbox
void VSizeChange()
{
    if (ShowButtons == FALSE)       // set the visibility of the Command Buttons
        gtk_widget_hide(CmdBHbox);
    else
        gtk_widget_show(CmdBHbox);

    if (BX_SMP_PROCESSORS > 1) {
        if (SingleCPU == FALSE) {       // set the visibility of the CPU Buttons
            gtk_widget_show(CpuBHbox);
        } else {
            gtk_widget_hide(CpuBHbox);
        }
    }

    if (ShowIOWindows == FALSE)
    {
        gtk_widget_hide(IOVbox);
        GTK_TABLE(TreeTbl)->nrows = 3;  // IO windows do not exist, so total table rows = 3
    }
    else
    {
        // Note: if an invisible Entry window contains text and is shown, the program can crash
        // so copy out any text, clear the Entry, make it visible, and then reload it
        const gchar *gcp;
        gcp = gtk_entry_get_text(GTK_ENTRY(IEntry));
        strcpy (tmpcb, gcp);
        gtk_entry_set_text(GTK_ENTRY(IEntry),"");
        GTK_TABLE(TreeTbl)->nrows = 4;  // IO windows exist, so total table rows = 4
        gtk_widget_show(IOVbox);
        gtk_widget_grab_focus(IEntry);  // Input window loses focus while it is hidden
        gtk_entry_set_text(GTK_ENTRY(IEntry),tmpcb);
    }
    gtk_widget_queue_draw(window);  // redraw everything
}

void TakeInputFocus()
{
    gtk_widget_grab_focus(IEntry);      // get the focus back from the VGA window
}

void MakeListsGray()
{
    if (AtBreak == FALSE)
    {
        SetStatusText(0, "Running");
    }
    else
        SetStatusText(0, "Break");

    // It's good to set the background color OUTSIDE the area of the lists
    // -- except that this seems difficult in gtk
    // So, the default behavior is not quite as pretty, but it is good enough.
}

// ParseIDText (*bochs*) calls this when the window needs to be redrawn with new text --
// funct must not call any GTK functions directly, because it is run from the bochs thread
void SetOutWinTxt()
{
    UpdateOWflag = TRUE;
}

// in the Win32 version, the caret is placed at the beginning of each selected history line,
// but the exact functionality is not important -- just do what is easy
void SelectHistory(int UpDown)
{
    // the History buffer is circular, so wrap 64 to 0, and -1 to 63
    CommandHistoryIdx = (CommandHistoryIdx + UpDown) & 63;
    gtk_entry_set_text(GTK_ENTRY(IEntry),CmdHistory[(CmdHInsert + CommandHistoryIdx) & 63]);
}

void ClearInputWindow()
{
    gtk_entry_set_text(GTK_ENTRY(IEntry),"");
}

// set == 0 to clear a check, 1 or 2 to set a check
void SetMenuCheckmark (int set, int ChkIdx)
{
    bx_bool flag = TRUE;
    if (set == 0)
        flag = FALSE;
    if ((bx_bool) gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(ChkMIs[ChkIdx])) != flag)
    {
        ForcingCheck = TRUE;
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ChkMIs[ChkIdx]), flag);
    }
}

// pass in FALSE or 0 to gray out a menu item
void GrayMenuItem (int flag, int CmdIndex)
{
//    GtkWidget *lbl;
    GtkWidget *MI = Cmd2MI[CmdIndex - CMD_IDX_LO + 1];
    // convert the Command Index to a Menu Item widget, and get its label
//    lbl = GTK_BIN(GTK_MENU_ITEM( MI ))->child;
    gtk_widget_set_sensitive ( MI, flag );
}


// finish initting things that must be initted in several stages
// -- this function is usually called only once, during the first doUpdate
void SpecialInit()
{
// Menu items can not be deactivated until after their signals are attached
    GrayMenuItem (0, CMD_EREG);
    GrayMenuItem (0, CMD_WPTWR);
    GrayMenuItem (0, CMD_WPTRD);
    GrayMenuItem (0, CMD_PAGEV);
//  GrayMenuItem (0, CMD_XCEPT);
    if (BX_SMP_PROCESSORS == 1)
        GrayMenuItem (0, CMD_ONECPU);
        // one or the other MemDump mode must always be left on! (unchangeable)
    if ((DumpInAsciiMode & 2) == 0)
        GrayMenuItem (0, CMD_MASCII);
    if ((DumpInAsciiMode & 1) == 0)
        GrayMenuItem (0, CMD_MHEX);

#if BX_SUPPORT_FPU == 0
    GrayMenuItem (0, CMD_FPUR);
#endif

    if (! CpuSupportSSE)
      GrayMenuItem (0, CMD_XMMR);

    doSimuInit = FALSE; // make sure this function is called once per simulation
}

// this routine is stubbed in the GTK version
void RedrawColumns(int listnum)
{
}

// autosize the columns that may be used by other dumps, once, on a DViewMode change
void SetAutoSize6_9()
{
    AllCols[6]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
    AllCols[7]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
    AllCols[8]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
    AllCols[9]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
}

// Show/Hide the typical pattern of Dump window columns (show the first n, hide the rest out to 17)
void ShowDListCols (int totcols)
{
    int i = 5;
    int firsthide = totcols + 5;        // convert from a col# to an acolnum
    while (++i < firsthide)
        AllCols[i]->visible = TRUE;
    while (i < 23)
        AllCols[i++]->visible = FALSE;
}

// OS-dependent code that runs before each ListFill routine of each type
void StartListUpdate(int listnum)
{
    static int PrevDV = -1;             // type of previous Dump window that was displayed
    GtkListStore *Database;
    // set the scroll position back to the very top
    GtkAdjustment *va = gtk_tree_view_get_vadjustment ( GTK_TREE_VIEW(LV[listnum]) );
    gtk_adjustment_set_value (GTK_ADJUSTMENT(va), 0);
    // then clear the database for the list that is updating
    Database = (GtkListStore *) gtk_tree_view_get_model( GTK_TREE_VIEW(LV[listnum]) );
    gtk_list_store_clear(GTK_LIST_STORE(Database));

    // a DumpAlign change is treated just like a DViewMode change
    if (DViewMode == VIEW_MEMDUMP && PrevDAD != DumpAlign)
        PrevDV = -1;

    // use Upd flag to prevent paint messages, cpu_loop calls, duplicate calls
//  UpdInProgress[listnum] = TRUE;
    if (ResizeColmns != FALSE || (listnum == DUMP_WND && PrevDV != DViewMode))
    {
        // autosize Reg "hex" column, Asm "L.Addr" column
        if (listnum == REG_WND)
            AllCols[1]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
        else if (listnum == ASM_WND)
            AllCols[3]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
        // otherwise, autosize/rename/hide all the proper Dump list columns
        else
        {
            switch (DViewMode)
            {
                case VIEW_MEMDUMP:
                    // fix the MemDump column "names"
                    if (DumpInitted == FALSE)
                    {
                        GtkTreeIter iter;
                        ShowDListCols (18);     // show all columns -- must be visible to set col titles!
                            // reset a "generic" header for col 0
                        gtk_tree_view_column_set_title(AllCols[6], "Address");
                        gtk_tree_view_column_set_title(AllCols[7], "0");
                        gtk_tree_view_column_set_title(AllCols[8], "1");
                        gtk_tree_view_column_set_title(AllCols[9], "2");
                        gtk_tree_view_column_set_title(AllCols[23], "Ascii");
                        SetAutoSize6_9();       // autosize the usual suspects
                        // add a blank line to "modify the store" -- which will activate autosize
                        gtk_list_store_append (GTK_LIST_STORE(Database), &iter);  /* Acquire an iterator */
                        gtk_list_store_set (GTK_LIST_STORE(Database), &iter, 0, "", -1);
                    }
                    else
                    {
                        AllCols[6]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
                        if (PrevDV == VIEW_MEMDUMP)     // autosizing address column only?
                            break;
                        PrevDAD = DumpAlign;
                        gtk_tree_view_column_set_title(AllCols[23], "Ascii");
                        if ((DumpInAsciiMode & 2) == 0)     // hex bit is turned on?
                        {
                                // if dumping ONLY in ascii, hide ALL the other columns
                            ShowDListCols (2);      // show only columns 0 and 17
                            break;
                        }
                        AllCols[7]->visible = TRUE;     // columns must be visible to set col titles!
                        AllCols[8]->visible = TRUE;
                        AllCols[9]->visible = TRUE;
                        gtk_tree_view_column_set_title(AllCols[7], "0");
                        gtk_tree_view_column_set_title(AllCols[8], "1");
                        gtk_tree_view_column_set_title(AllCols[9], "2");
                        for (int i = 7 ; i < 24 ; i++)
                        {
                            if (((i - 7) & (DumpAlign - 1)) == 0)       // either autosize,
                            {
                                AllCols[i]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
                                AllCols[i]->visible = TRUE;
                            }
                            else            // or set the column invisible
                                AllCols[i]->visible = FALSE;
                        }
                    }
                    break;

                case VIEW_GDT:
                    SetAutoSize6_9();           // autosize the usual suspects
                    if (PrevDV == VIEW_GDT)     // autosizing only?
                        break;
                    ShowDListCols (5);      // show 4 columns, hide the rest out to 17
                    gtk_tree_view_column_set_title(AllCols[6], "Index");        // header for col 0
                    gtk_tree_view_column_set_title(AllCols[7], "Base Address"); // header for col 1
                    gtk_tree_view_column_set_title(AllCols[8], "Size");         // header for col 2
                    gtk_tree_view_column_set_title(AllCols[9], "DPL");          // header for col 3
                    gtk_tree_view_column_set_title(AllCols[23], "Info");        // header for col 4 (17)
                    break;

                case VIEW_IDT:
                    AllCols[6]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
                    if (PrevDV == VIEW_IDT)     // autosizing only?
                        break;
                    ShowDListCols (2);      // show 1 column, hide the rest out to 17
                    gtk_tree_view_column_set_title(AllCols[6], "Interrupt");        // header for col 0
                    gtk_tree_view_column_set_title(AllCols[23], "L.Address");       // header for col 1 (17)
                    break;

                case VIEW_PAGING:
                    AllCols[6]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
                    if (PrevDV == VIEW_PAGING)      // autosizing only?
                        break;
                    ShowDListCols (2);      // show 1 column, hide the rest out to 17
                    gtk_tree_view_column_set_title(AllCols[6], "L.Address");                // header for col 0
                    gtk_tree_view_column_set_title(AllCols[23], "is mapped to P.Address");  // header for col 1 (17)
                    break;

                case VIEW_STACK:
                    AllCols[6]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
                    AllCols[7]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
                    if (PrevDV == VIEW_STACK)       // autosizing only?
                        break;
                    ShowDListCols (3);      // show 2 columns, hide the rest out to 17
                    gtk_tree_view_column_set_title(AllCols[6], "L.Address");    // header for col 0
                    gtk_tree_view_column_set_title(AllCols[7], "Value");        // header for col 1
                    gtk_tree_view_column_set_title(AllCols[23], "(dec.)");  // header for col 2 (17)
                    break;

                case VIEW_BREAK:
                    AllCols[6]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
                    AllCols[7]->column_type = GTK_TREE_VIEW_COLUMN_AUTOSIZE;
                    if (PrevDV == VIEW_STACK)       // autosizing only?
                        break;
                    ShowDListCols (3);      // show 2 columns, hide the rest out to 17
                    gtk_tree_view_column_set_title(AllCols[6], "Address");  // header for col 0
                    gtk_tree_view_column_set_title(AllCols[7], "Enabled");  // header for col 1
                    gtk_tree_view_column_set_title(AllCols[23], "ID");  // header for col 2 (17)
            }
            PrevDV = DViewMode;
        }
    }
}

void EndListUpdate(int listnum)
{
    int i;

//  UpdInProgress[listnum] = FALSE;     // It's OK to paint the listview now
    // clear selections for each list on a list update
    if (listnum == REG_WND)
    {
        i = TOT_REG_NUM + EXTRA_REGS;
        while (--i >= 0)
            SelReg[i] = 0;
    }
    else if (listnum == ASM_WND)
    {
        i = MAX_ASM;
        while (--i >= 0)
            SelAsm[i] = 0;
    }
    else if (DViewMode == VIEW_MEMDUMP)
    {
        i = 256;
        while (--i >= 0)
            SelMem[i] = 0;
    }

    // AUTOSIZE turns off "user resize" mode! Turning resize mode back on will force
    // autosize back into GROW_ONLY mode -- which is what I want, anyway.
    i = 23;
    while (--i >= 0)
    {
        // the 3rd column in Reg and Asm lists is not resizable
        if (i != 2 && i != 5)
            gtk_tree_view_column_set_resizable  (AllCols[i], TRUE);
    }
//  Invalidate (listnum);
}

void DispMessage(const char *msg, const char *title)
{
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(GTK_WINDOW(window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_WARNING,
        GTK_BUTTONS_OK,
        "%s", msg);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// hide/clear param_tree window when showing a different MemDump window (GDT, IDT, etc.)
void HideTree()
{
    GtkTreeStore *treestore;
    if (DViewMode != VIEW_PTREE)
        return;
    treestore = (GtkTreeStore *) gtk_tree_view_get_model( GTK_TREE_VIEW(PTree) );
    gtk_container_remove (GTK_CONTAINER(ScrlWin[2]),PTree);
    gtk_container_add (GTK_CONTAINER(ScrlWin[2]), LV[2]);
    gtk_tree_store_clear(GTK_TREE_STORE(treestore));
}

// exit GDT/IDT/Paging/Stack/Tree -- back to the MemDump window
void ShowMemData(bx_bool initting)
{
    int i = 1;
    if (LinearDump == FALSE)
        i = 0;
    if (initting != FALSE)
    {
        GrayMenuItem (0, CMD_WPTWR);
        GrayMenuItem (0, CMD_WPTRD);
        // update column 0 to say whether its physmem or linear
        gtk_tree_view_column_set_title(AllCols[6], DC0txt[i]);
    }
    HideTree();
    gtk_widget_show(LV[DUMP_WND]);
    DViewMode = VIEW_MEMDUMP;       // returning to MemDump mode
    if (DumpInitted == FALSE)
    {
        StartListUpdate(DUMP_WND);  // autosize for a blank MemDump window
        EndListUpdate(DUMP_WND);
    }
    else
    {
        gtk_tree_view_column_set_title(AllCols[6], DC0txt[i]);
        if (AtBreak == FALSE)
            doDumpRefresh = TRUE;
        else
            ShowData();         // repopulates & resizes all columns
    }
}

// create the top layer of a parameter tree TreeView
void FillPTree()
{
    int n;
    GtkTreeStore *treestore;
    extern bx_list_c *root_param;
    // Note: don't multithread this display -- the user expects it to complete
    if (DViewMode != VIEW_PTREE)
    {
        gtk_container_remove (GTK_CONTAINER(ScrlWin[2]),LV[2]);
        gtk_container_add (GTK_CONTAINER(ScrlWin[2]), PTree);
    }
    doDumpRefresh = FALSE;
    treestore = (GtkTreeStore *) gtk_tree_view_get_model( GTK_TREE_VIEW(PTree) );
    gtk_tree_store_clear(GTK_TREE_STORE(treestore));
    // Note: all tree strings are hardcoded to be in tmpcb -- don't change!
    n = root_param->get_size();
    for (int i=0; i<n; i++)
        MakeBL(NULL, root_param->get(i));
    gtk_widget_show(PTree);
}

void MakeTreeChild (TreeParent *h_P, int ChildCount, TreeParent *h_TC)
{
    GtkTreeStore *treestore;
    treestore = (GtkTreeStore *) gtk_tree_view_get_model( GTK_TREE_VIEW(PTree) );
    gtk_tree_store_append(GTK_TREE_STORE(treestore), h_TC, h_P);            // create the "leaf"
    gtk_tree_store_set(GTK_TREE_STORE(treestore), h_TC, 0, tmpcb, -1);  // and put a name (+ data) on it
}

bx_bool NewFont()
{
    char *ofn;
    if (AtBreak == FALSE)
        return FALSE;
    // need to know the current font, to highlight it
    g_object_get( G_OBJECT( LV_Rend[0] ), "font", &ofn, NULL );
    GtkWidget *widget = gtk_font_selection_dialog_new ("Choose primary font");
    gtk_font_selection_dialog_set_font_name (GTK_FONT_SELECTION_DIALOG(widget), ofn);
    if (gtk_dialog_run (GTK_DIALOG (widget)) == GTK_RESPONSE_OK)
    {
        int i, width;
        char *fontname = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(widget));
        g_object_set( G_OBJECT( LV_Rend[0] ), "font", fontname, NULL );
        g_object_set( G_OBJECT( LV_Rend[1] ), "font", fontname, NULL );
        g_object_set( G_OBJECT( LV_Rend[2] ), "font", fontname, NULL );
        g_free (fontname);
        // calculate a new value for OneCharWide
        PangoLayout * layout = gtk_widget_create_pango_layout (LV[0], "M");
        PangoFontDescription * fontdesc;
        g_object_get( G_OBJECT( LV_Rend[0] ), "font-desc", &fontdesc, NULL ); 
        pango_layout_set_font_description (layout, fontdesc); 
        pango_layout_get_pixel_size (layout, &width, &i);
        pango_font_description_free (fontdesc);
        g_object_unref (layout);
        OneCharWide = width;        // The "width" I'm getting is about half what I expect
//      OneCharWide = width >> 1;   // pretend that an average char width is half an "M"
        if (OneCharWide > 12) OneCharWide = 12;
    }
    g_free (ofn);
    gtk_widget_destroy (widget);

    return TRUE;
}

// turn off any current sizing/docking mode
void sizing_cancel()
{
    // cancel presize, size, predock, and dock modes
    if (Sizing != 0)
    {
        gtk_event_box_set_above_child (GTK_EVENT_BOX(LVEvtBox), FALSE);
        gtk_event_box_set_above_child (GTK_EVENT_BOX(VSepEvtBox1), TRUE);
        gtk_event_box_set_above_child (GTK_EVENT_BOX(VSepEvtBox2), TRUE);
        if (Sizing < 10)    // cancel (p)resize mode motion detection
            g_signal_handler_disconnect (LVEvtBox, mmov_hID);
        gdk_window_set_cursor (GTK_WIDGET(LVEvtBox)->window, NULL); // change the cursor to normal
        Sizing = 0;
    }
    EnterSizeMode = FALSE;
    SizingDelay = 0;
    SizeList = 0;
    xClick = -1;
}

// conditionally begin a resize operation
void enter_resize_mode()
{
    // need reasonably accurate column widths in horz. limit calculations -- update widths
    int width0, width1;
    gint totwidth;
    gint height;
    EnterSizeMode = FALSE;      // presize mode may never have reset this
    // a GtkAllocation structure (inside every widget) is a rectangle: relative x, y, + width & height
    width0 = ScrlWin[0]->allocation.width;
    ListWidthPix[0] = width0;
    width1 = ScrlWin[1]->allocation.width;
    ListWidthPix[1] = width1;

    gdk_drawable_get_size (window->window, &totwidth, &height);
    ListWidthPix[2] = totwidth - (width0 + width1);

    if (Sizing < 0)         // in presize mode? -- enter full resize mode
        SetHorzLimits();
}

// begin a pre-dock operation -- cancel if a mouseup event happens
void Set_PreDock(int ListNum, GdkEventButton *event)
{
    // Sadly, in order to narrow the mousedown event to only the ListView (without the scrollbars)
    // information seems to get lost about which button is being pressed
    // -- but the button info seems not reliable anyway (currently) so it must be lived with

    // enter pre-dock mode, autoactivate full docking mode in 2 VGAW refresh time ticks
    xClick = (unsigned int) (event->x_root + 0.5);
    yClick = (unsigned int) (event->y_root + 0.5);
    SizeList = ListNum + 10;
    SizingDelay = 2;
}

// Select one entry in Register ListView
gboolean RegMouseDown_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    int row;
    GtkTreePath *path;
    gint *pathdat;
    bx_bool onrow;

    Set_PreDock (0, event);
    GrayMenuItem (0, CMD_WPTWR);        // disable watchpoints for physdumps (not selected)
    GrayMenuItem (0, CMD_WPTRD);
    GrayMenuItem (1, CMD_BRKPT);        // enable ASM breakpoints (window is selected by default)
    row = TOT_REG_NUM + EXTRA_REGS;
    while (--row >= 0)
        SelReg[row] = 0;
    onrow = (bx_bool) gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW(LV[0]),
        (gint) (event->x + 0.5), (gint) (event->y + 0.5), &path, NULL, NULL, NULL);
    if (onrow == FALSE)
        return FALSE;
    pathdat = gtk_tree_path_get_indices (path);
    row = (int) *pathdat;
    gtk_tree_path_free (path);
    if (row >= TOT_REG_NUM + EXTRA_REGS || row < 0)
        row = TOT_REG_NUM + EXTRA_REGS -1;
    SelReg[row] = TRUE;
    Invalidate (REG_WND);

    return FALSE;
}

// Select multiple entries in ASM ListView
gboolean AsmMouseDown_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GtkTreePath *path;
    gint *pathdat;
    bx_bool onrow;

    Set_PreDock (1, event);
    GrayMenuItem (0, CMD_WPTWR);        // disable watchpoints for physdumps (not selected)
    GrayMenuItem (0, CMD_WPTRD);
    GrayMenuItem (1, CMD_BRKPT);        // enable ASM breakpoints (window is selected)
    if ((event->state & GDK_CONTROL_MASK) == 0)
    {
        int i = MAX_ASM;
        while (--i >= 0)
            SelAsm[i] = 0;
    }
    onrow = (bx_bool) gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW(LV[1]),
        (gint) (event->x + 0.5), (gint) (event->y + 0.5), &path, NULL, NULL, NULL);
    if (onrow == FALSE)
        return FALSE;
    pathdat = gtk_tree_path_get_indices (path);
    SelectedEntry = (int) *pathdat;
    gtk_tree_path_free (path);
    if (SelectedEntry >= MAX_ASM || SelectedEntry < 0)
        SelectedEntry = MAX_ASM -1;
    SelAsm[SelectedEntry] ^= 1;
    Invalidate (ASM_WND);
    return FALSE;
}

// Select multiple entries in Phys or Linear Memdump ListView
gboolean MemMouseDown_cb(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    int column, cellx;
    GtkTreePath *path;
    gint *pathdat;
    bx_bool onrow;

    Set_PreDock (2, event);
    if (DViewMode != VIEW_MEMDUMP && DViewMode != VIEW_BREAK)
        return FALSE;
    if ((event->state & GDK_CONTROL_MASK) == 0)
    {
        int i = 256;
        while (--i >= 0)
            SelMem[i] = 0;
    }
    onrow = (bx_bool) gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW(LV[2]),
        (gint) (event->x + 0.5), (gint) (event->y + 0.5), &path, NULL, NULL, NULL);
    if (onrow == FALSE)
        return FALSE;
    pathdat = gtk_tree_path_get_indices (path);
    DumpSelRow = (int) *pathdat;
    gtk_tree_path_free (path);

    // handle enabling/disabling watchpoint menu items
    if (LinearDump == FALSE && DViewMode == VIEW_MEMDUMP)
    {
        GrayMenuItem (1, CMD_WPTWR);    // enable watchpoints for physdumps
        GrayMenuItem (1, CMD_WPTRD);
    }
    else
    {
        GrayMenuItem (0, CMD_WPTWR);    // disable watchpoints for lineardumps
        GrayMenuItem (0, CMD_WPTRD);
    }
    if (DViewMode != VIEW_MEMDUMP)      // VIEW_BREAK handling is complete now
    {
        GrayMenuItem (1, CMD_BRKPT);    // enable ASM breakpoints (window is selected by default)
        return FALSE;
    }
    GrayMenuItem (0, CMD_BRKPT);        // disable ASM breakpoints (window is not selected)

    cellx = 0;
    column = 6;     // loop over the Mem window columns, 6 to 23

    while (event->x >= cellx && column < 23)
    {
        int j = cellx += AllCols[column]->width;
        if (event->x >= j){
            ++column;
            cellx = j;
        }
    }
    if (DumpSelRow >= 256 || DumpSelRow < 0)
        DumpSelRow = 255;
    KeyStateShft = event->state & GDK_SHIFT_MASK;
    KeyStateAlt = event->state & GDK_MOD1_MASK;
    SelMem[DumpSelRow] ^= 1;
    // only set the SelDataAddy for memdumps with a wordsize of 1
    if (DumpAlign == 1 && column > 6)
    {
        SA_valid = TRUE;
        SelectedDataAddress = DumpStart + (DumpSelRow*16) + (column - 7);
    }
    Invalidate (DUMP_WND);
    return FALSE;
}

// Change the CPU display
void CPUb_cb(GtkWidget *widget, gpointer CPUnum)    // "CPUnum" is the value supplied at handler creation
{
    unsigned int newCPU = GPOINTER_TO_INT(CPUnum);
    if (CurrentCPU != newCPU)
    {
        // change text on CurrentCPU button to lowercase and BOLD with markup
        strcpy (tmpcb, "<b>cpu0</b>");
        tmpcb[6] = CurrentCPU + '0';
        gtk_label_set_markup (GTK_LABEL(CpuB_label[CurrentCPU]), tmpcb);
        // change text on newCPU button to uppercase and BOLD with markup
        strcpy (tmpcb, "<b>CPU0</b>");
        tmpcb[6] = newCPU + '0';
        gtk_label_set_markup (GTK_LABEL(CpuB_label[newCPU]), tmpcb);
        CurrentCPU = newCPU;
        BottomAsmLA = ~0;       // force an ASM autoload, to repaint
        if (AtBreak != FALSE)   // if at a break, pretend it just happened
            OnBreak();          // refresh the ASM and Register windows
    }
}

// calls the proper GUI command on activate events -- non breaking
void nbCmd_cb(GtkWidget *widget, gpointer CmdIdx)       // "CmdIdx" is the value supplied at handler creation
{
    if (ForcingCheck == FALSE)      // using a hotkey to flip a menu item checkmark?
        ActivateMenuItem (GPOINTER_TO_INT (CmdIdx));
    ForcingCheck = FALSE;
}

// calls the proper GUI command on activate events, with a sim break
void Cmd_cb(GtkWidget *widget, gpointer CmdIdx)     // "CmdIdx" is the value supplied at handler creation
{
    if (AtBreak == FALSE)
    {
//      DoBreak = TRUE; -- for multithreading
        SIM->debug_break();     // On a menu click always break (with some exceptions)
    }
    ActivateMenuItem (GPOINTER_TO_INT (CmdIdx));
}

// code to run on exit -- arguments are ignored
void Close_cb(GtkWidget *widget, gpointer data)
{
    GtkListStore *Database;
    GtkTreeStore *treestore;
    // clear all the "store" databases
    Database = (GtkListStore *) gtk_tree_view_get_model( GTK_TREE_VIEW(LV[0]) );
    gtk_list_store_clear(GTK_LIST_STORE(Database));
    Database = (GtkListStore *) gtk_tree_view_get_model( GTK_TREE_VIEW(LV[1]) );
    gtk_list_store_clear(GTK_LIST_STORE(Database));
    Database = (GtkListStore *) gtk_tree_view_get_model( GTK_TREE_VIEW(LV[2]) );
    gtk_list_store_clear(GTK_LIST_STORE(Database));
    treestore = (GtkTreeStore *) gtk_tree_view_get_model( GTK_TREE_VIEW(PTree) );
    gtk_tree_store_clear(GTK_TREE_STORE(treestore));

    bx_user_quit = 1;
    SIM->debug_break();
    // holding one extra reference to both ScrlWin[2] widgets
    // but I "sank" PTree myself, so it is my understanding that I need to "destroy" it myself
    gtk_widget_destroy (PTree);
    gtk_widget_unref (PTree);
    gtk_widget_unref (LV[2]);
    gdk_cursor_unref (SizeCurs);
    gdk_cursor_unref (DockCurs);

    if (!SIM->is_wx_selected()) {
      gtk_main_quit();
    }
    window = NULL;
}

// there is only one widget that receives keystrokes, the Input Entry widget
// -- process those keydowns for hotkeys and special keys
gboolean keydown_event_cb(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    gboolean ret = FALSE;   // assume the key is NOT a hotkey -- enter it into the widget
    int key = (int) (event->keyval & 0xffff);
    int Control, Shift, Alt;
    Control= Shift= Alt= 0;
    if ((event->state & GDK_CONTROL_MASK) != 0)
        Control = -1;
    if ((event->state & GDK_SHIFT_MASK) != 0)
        Shift = -1;
    if ((event->state & GDK_MOD1_MASK) != 0)        // this is the Alt key mask
        Alt = -1;

    switch (event->keyval)
    {
        case  'c':
        case  'd':
        case  'f':
            if (Control == 0) break;
            key &= 0xdf;            // convert to upper case
        case  VK_UP:
        case  VK_DOWN:
        case  VK_PRIOR:
        case  VK_NEXT:
        case  VK_ESCAPE:
        case  VK_F2:
        case  VK_F3:
        case  VK_F4:
        case  VK_F5:
        case  VK_F6:
        case  VK_F7:
        case  VK_F8:
        case  VK_F9:
        case  VK_F11:
        case  VK_RETURN:
            HotKey (key, Alt, Shift, Control);
            ret = TRUE;
    }
    return ret;
}

// Input window appears to be hidden -- main window received a keydown.
// Force the event to redirect to the hidden IEntry widget
gboolean redirect_keydown(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    // send the event ONLY to IEntry -- do not try to propogate it beyond
    gtk_widget_event(IEntry, event);
    return TRUE;
}

// table (that contains the main windows!) is being resized/moved
// Note: the window gets MANY of these events where the size remains constant
gboolean TblSizeEvent(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    int i, j, k;
    bx_bool unchanged = FALSE;
    gint WinSizeX;
    gint WinSizeY;
    GdkWindow *TblGdk = gtk_widget_get_parent_window(TreeTbl);

    gdk_drawable_get_size (TblGdk, &WinSizeX, &WinSizeY);

    // don't "resize" on minimize, if window is too small, or if width is (almost) unchanged
    // -- five pixels spread over 3 windows is negligible
    if (CurXSize > (unsigned int)WinSizeX - 8 && CurXSize <= (unsigned int)WinSizeX)
        unchanged = TRUE;
    if (WinSizeY > 100 && WinSizeX > 150 && unchanged == FALSE)
    {
        CurXSize = WinSizeX -4;     // assume 4 pixels total for the two VSeparators

        // need to recalculate List widths, with the same proportions as before
        i = ListWidthPix[0] + ListWidthPix[1] + ListWidthPix[2];
        if (i == 0)
            i = 1;
        j = (ListWidthPix[2] * CurXSize) / i;
        k = (ListWidthPix[1] * CurXSize) / i;
        i = OneCharWide << 2;
        if (j < i) j = i;
        if (k < i) k = i;
        ListWidthPix[0] = CurXSize - k - j;     // Register list
        ListWidthPix[1] = k;                    // Asm
        ListWidthPix[2] = j;                    // MemDump
        ListVerticalPix = (WinSizeY * 3) /4;
        // set the size of the two ScrlWins that are not in the middle
        // (the middle one automatically takes the rest)
        i= CurCenterList -1;
        if (i < 0)          // wrap -1 to 2
            i = 2;
    // -1 means "let the window manager decide how big to make it"
        gtk_widget_set_size_request(ScrlWin[i], ListWidthPix[i], -1);
        i = CurCenterList + 1;
        if (i == 3)     // wrap 3 to 0
            i = 0;
        gtk_widget_set_size_request(ScrlWin[i], ListWidthPix[i], -1);
    }
    return FALSE;
}

// Note: for some reason, the path shouldn't be freed
void Reg_DblClick (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data)
{
    sizing_cancel();
    if (AtBreak == FALSE)
        return;
    ChangeReg();
}

void Asm_DblClick (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data)
{
    sizing_cancel();
    if (AtBreak == FALSE)
        return;
    SetBreak(SelectedEntry);
}

void Mem_DblClick (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data)
{
    sizing_cancel();
    if (AtBreak == FALSE || (DViewMode != VIEW_MEMDUMP && DViewMode != VIEW_BREAK))
        return;

    if (DViewMode == VIEW_MEMDUMP)
    {
        if (KeyStateShft != 0 && LinearDump == FALSE)
            SetWatchpoint(&num_write_watchpoints,write_watchpoint);

        else if (KeyStateAlt != 0 && LinearDump == FALSE)
            SetWatchpoint(&num_read_watchpoints,read_watchpoint);

        else
//L HIHI Fix how this works in the Windoze version to match GTK?
            SetMemLine(DumpSelRow);
    }
    else            // doubleclick to delete a breakpoint in Breakpoint dump
    {
//L HIHI pre-test DumpSelRow < 256
        if (DumpSelRow < WWP_BaseEntry)
        {
            if (BrkpIDMap[DumpSelRow] == 0)
                ;   // dblclick was on an invalid row
            else if (DumpSelRow > EndPhyEntry)
                bx_dbg_del_vbreak(BrkpIDMap[DumpSelRow]);   // delete Virtual Brk
            else if (DumpSelRow > EndLinEntry)
                bx_dbg_del_pbreak(BrkpIDMap[DumpSelRow]);   // delete Phy Brkpt
            else
            {
                bx_dbg_del_lbreak(BrkpIDMap[DumpSelRow]);   // delete Lin Brkpt
                ParseBkpt();        // get a new set of linear breakpoints
                Invalidate (1);     // breakpoint colors may have changed
            }
        }
        else if (DumpSelRow >= WWP_BaseEntry && DumpSelRow < WWP_BaseEntry + WWPSnapCount)
        {
            // if a watchpoint was clicked, get its index (0 to 15)
            int i = DumpSelRow - WWP_BaseEntry;
            if (WWP_Snapshot[i] == write_watchpoint[i].addr)
                DelWatchpoint(write_watchpoint, &num_write_watchpoints, i);
        }
        else if (DumpSelRow >= RWP_BaseEntry && DumpSelRow < RWP_BaseEntry + RWPSnapCount)
        {
            int i = DumpSelRow - RWP_BaseEntry;
            if (RWP_Snapshot[i] == read_watchpoint[i].addr)
                DelWatchpoint(read_watchpoint, &num_read_watchpoints, i);
        }
        RefreshDataWin();       // show the NEW set of break/watchpoints
    }
}

// enforce that resize moves remain within the window
gboolean LvLeave(GtkWidget *widget, GdkEventCrossing *event, gpointer data)
{
    if (Sizing == 0 && xClick < 0)      // anything to cancel?
        return FALSE;
    // calculate the position of the top left corner of the LV Event Box
    unsigned int x_org = (unsigned int) (event->x_root - event->x + 0.5);
    unsigned int y_org = (unsigned int) (event->y_root - event->y + 0.5);
    unsigned int absx = (unsigned int) (event->x_root + 0.5);
    unsigned int absy = (unsigned int) (event->y_root + 0.5);
    gint width;
    gint height;
    // get the width and height of the ListView hbox window
    gdk_drawable_get_size (event->window, &width, &height);

    // cursor moved outside the box (not just onto a vert. separator)?
    if (absx < x_org || absx >= (x_org + width) || absy < y_org || absy >= (y_org + height))
        sizing_cancel();
    return FALSE;
}

// doing a resizing operation -- keep track of the cursor position
gboolean LV_MouseMove(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    int Cur_CliX = (int) (event->x + 0.5);

    if (EnterSizeMode != FALSE)
    {
        // need the "relative" X coordinate of the cursor (wrt the LVEB window)
        int PrevCliX = CurScrX - (int)(event->x_root - event->x + 0.5);
        int i= OneCharWide >> 1;
        Resize_LoX = PrevCliX - i;  // set presize horizontal limits
        Resize_HiX = PrevCliX + i;
        EnterSizeMode = FALSE;
    }
    if (Sizing != 0)
    {
        // enforce horizontal limits of the current resize operation
        if (Cur_CliX > Resize_HiX || Cur_CliX < Resize_LoX)
            sizing_cancel();
    }
    return FALSE;
}

// begin a resizing operation
gboolean LV_MouseDown(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
// pre-size mode activated the LVEventBox, and now the user has clicked the mouse
    enter_resize_mode();    // updates ListWidthPix, verifies presize mode
    return TRUE;
}

// mouse is on top of a separator between listviews -- enter presize mode
gboolean VsEnter(GtkWidget *widget, GdkEventCrossing *event, gpointer SepNum)
{
    int VsepNum = GPOINTER_TO_INT( SepNum );

    // turn on presize mode, if some kind of sizing isn't already running
    if (Sizing == 0)
    {
        Sizing = VsepNum - 2;       // Sizing = -1 or -2 for "presize"
        gdk_window_set_cursor (GTK_WIDGET(LVEvtBox)->window, SizeCurs);
        EnterSizeMode = TRUE;
        CurScrX = (unsigned int) (event->x_root + 0.5);
        gtk_event_box_set_above_child (GTK_EVENT_BOX(LVEvtBox), TRUE);
        gtk_event_box_set_above_child (GTK_EVENT_BOX(VSepEvtBox1), FALSE);
        gtk_event_box_set_above_child (GTK_EVENT_BOX(VSepEvtBox2), FALSE);
        mmov_hID = g_signal_connect (G_OBJECT (LVEvtBox), "motion_notify_event",
            G_CALLBACK (LV_MouseMove), NULL);
    }
    return FALSE;
}

gboolean LV_MouseUp(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    SizingDelay = 0;    // cancel pre-dock mode on every mouseup
    SizeList = 0;       // -- this will not affect any other sizing modes
    xClick = -1;
    return FALSE;
}

// either cancel or complete a dock or sizing
gboolean LEB_MouseUp(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    int width0, width1, fList, oldcList, dest;
    gint totwidth, heightLV, x, zip;
    if (Sizing != 0 || xClick >= 0)     // complete a resize or dock operation
    {
        int cx = (int)(event->x + 0.5);
        int cy = (int)(event->y + 0.5);
        heightLV = ScrlWin[0]->allocation.height;
        gdk_drawable_get_size (window->window, &totwidth, &zip);
        dest = fList = (DockOrder >> 8) -1;

        if (Sizing < 10)    // complete a resize operation
        {
            width0 = cx;
            width1 = ListWidthPix[fList] - cx;  // "delta" pixels (pos or neg)
            if (Sizing == 1)    // moving Vsep1?
                gtk_widget_set_size_request(ScrlWin[fList], cx, -1);
            else
            {
                dest = (DockOrder & 3) -1;
                width0 = totwidth - cx - 4; // assume the Vseps are 2 pixels wide
                width1 = ListWidthPix[dest] - width0;   // "delta" pixels (pos or neg)
                gtk_widget_set_size_request(ScrlWin[dest], width0, -1);
            }
            ListWidthPix[dest] = width0;
            ListWidthPix[CurCenterList] += width1;  // tack the delta pixels onto ctr list
        }
        else    // complete a Docking operation
        {
            // get "local" x mouse coordinate value (main window relative)
            gdk_window_get_pointer (GTK_WIDGET(LVEvtBox)->window, &x, &zip, NULL);
            // verify that mouseup was within a LV window
            if (x >= 0 && cy >= 0 && x < totwidth && cy < heightLV)
            {
                // update the widths of the LVs
                width0 = ScrlWin[0]->allocation.width;
                ListWidthPix[0] = width0;
                width1 = ScrlWin[1]->allocation.width;
                ListWidthPix[1] = width1;
                ListWidthPix[2] = totwidth - (width0 + width1);
                oldcList = CurCenterList;
                if ((unsigned) x < ListWidthPix[fList])
                    dest = fList;
                else if ((unsigned) x < ListWidthPix[fList] + ListWidthPix[CurCenterList])
                    dest = CurCenterList;
                else dest = (DockOrder & 3) -1;
                DockResize (dest, (Bit32u) 0);      // ParentX is not used for docking
                // if the center list moved, it will not keep its size properly
                if (oldcList != CurCenterList)
                {
                    gtk_widget_set_size_request(ScrlWin[CurCenterList], -1, -1);
                    gtk_widget_set_size_request(ScrlWin[oldcList], ListWidthPix[oldcList], -1);
                }
            }
        }
        sizing_cancel();
    }
    return TRUE;
}

// one-time init function to make all the buttons/menu items work
void AttachSignals()
{
    int i;
    g_signal_connect (G_OBJECT(window), "destroy",
        GTK_SIGNAL_FUNC(Close_cb), (gpointer) NULL);
    g_signal_connect (G_OBJECT(CmdBtn[0]), "clicked", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) (glong) BtnLkup[0]);
    g_signal_connect (G_OBJECT(CmdBtn[1]), "clicked", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) (glong) BtnLkup[1]);
    g_signal_connect (G_OBJECT(CmdBtn[2]), "clicked", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) (glong) BtnLkup[2]);
    g_signal_connect (G_OBJECT(CmdBtn[3]), "clicked", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) (glong) BtnLkup[3]);
    g_signal_connect (G_OBJECT(CmdBtn[4]), "clicked", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) (glong) BtnLkup[4]);

    i = BX_SMP_PROCESSORS;
    if (i > 1)
    {
        while (--i >= 0)
            g_signal_connect (G_OBJECT(CpuBtn[i]), "clicked", GTK_SIGNAL_FUNC(CPUb_cb), (gpointer) (glong) i);
    }

// activate the menu items
    g_signal_connect (G_OBJECT(ContMI), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_CONT);
    g_signal_connect (G_OBJECT(StepMI), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_STEP1);
    g_signal_connect (G_OBJECT(StepNMI), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_STEPN);
    g_signal_connect (G_OBJECT(BreakMI), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_BREAK);
    g_signal_connect (G_OBJECT(SetBrkMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_BRKPT);
    Cmd2MI[CMD_BRKPT - CMD_IDX_LO + 1] = SetBrkMI;
    g_signal_connect (G_OBJECT(WatchWrMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_WPTWR);
    Cmd2MI[CMD_WPTWR - CMD_IDX_LO + 1] = WatchWrMI;
    g_signal_connect (G_OBJECT(WatchRdMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_WPTRD);
    Cmd2MI[CMD_WPTRD - CMD_IDX_LO + 1] = WatchRdMI;
    g_signal_connect (G_OBJECT(FindMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_FIND);
    g_signal_connect (G_OBJECT(RefreshMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_RFRSH);

    g_signal_connect (G_OBJECT(PhyDumpMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_PHYDMP);
    g_signal_connect (G_OBJECT(LinDumpMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_LINDMP);
    g_signal_connect (G_OBJECT(StackMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_STACK);
    g_signal_connect (G_OBJECT(GDTMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_GDTV);
    g_signal_connect (G_OBJECT(IDTMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_IDTV);
    g_signal_connect (G_OBJECT(PageMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_PAGEV);
    Cmd2MI[CMD_PAGEV - CMD_IDX_LO + 1] = PageMI;
    g_signal_connect (G_OBJECT(ViewBrkMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_VBRK);
    g_signal_connect (G_OBJECT(MemDumpMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_CMEM);
    g_signal_connect (G_OBJECT(PTreeMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_PTREE);
    g_signal_connect (G_OBJECT(DisAsmMI), "activate", GTK_SIGNAL_FUNC(Cmd_cb), (gpointer) CMD_DISASM);

    g_signal_connect (G_OBJECT(ChkMIs[CHK_CMD_MODEB]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_MODEB);
    g_signal_connect (G_OBJECT(ChkMIs[CHK_CMD_ONECPU]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_ONECPU);
    Cmd2MI[CMD_ONECPU - CMD_IDX_LO + 1] = ChkMIs[CHK_CMD_ONECPU];
    g_signal_connect (G_OBJECT(DefDisMI), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_DADEF);
    g_signal_connect (G_OBJECT(SyntaxMI), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_ATTI);
    g_signal_connect (G_OBJECT(FontMI), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_FONT);
    g_signal_connect (G_OBJECT(ChkMIs[CHK_CMD_UCASE]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_UCASE);
    g_signal_connect (G_OBJECT(ChkMIs[CHK_CMD_IOWIN]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_IOWIN);
    g_signal_connect (G_OBJECT(ChkMIs[CHK_CMD_SBTN]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_SBTN);
    g_signal_connect (G_OBJECT(ChkMIs[CHK_CMD_MHEX]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_MHEX);
    Cmd2MI[CMD_MHEX - CMD_IDX_LO + 1] = ChkMIs[CHK_CMD_MHEX];
    g_signal_connect (G_OBJECT(ChkMIs[CHK_CMD_MASCII]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_MASCII);
    Cmd2MI[CMD_MASCII - CMD_IDX_LO + 1] = ChkMIs[CHK_CMD_MASCII];
    g_signal_connect (G_OBJECT(ChkMIs[CHK_CMD_LEND]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_LEND);
    g_signal_connect (G_OBJECT(ChkMIs[CHK_CMD_IGNSA]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_IGNSA);
    g_signal_connect (G_OBJECT(ChkMIs[CHK_CMD_IGNNT]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_IGNNT);
    g_signal_connect (G_OBJECT(ChkMIs[CHK_CMD_RCLR]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_RCLR);
    g_signal_connect (G_OBJECT(ChkMIs[CHK_CMD_EREG]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_EREG);
    Cmd2MI[CMD_EREG - CMD_IDX_LO + 1] = ChkMIs[CHK_CMD_EREG];
    g_signal_connect (G_OBJECT(ChkMIs[S_REG]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_SREG);
    g_signal_connect (G_OBJECT(ChkMIs[SYS_R]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_SYSR);
    g_signal_connect (G_OBJECT(ChkMIs[C_REG]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_CREG);
    g_signal_connect (G_OBJECT(ChkMIs[FPU_R]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_FPUR);
    Cmd2MI[CMD_FPUR - CMD_IDX_LO + 1] = ChkMIs[FPU_R];
    g_signal_connect (G_OBJECT(ChkMIs[XMM_R]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_XMMR);
    Cmd2MI[CMD_XMMR - CMD_IDX_LO + 1] = ChkMIs[XMM_R];
    g_signal_connect (G_OBJECT(ChkMIs[D_REG]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_DREG);
    Cmd2MI[CMD_DREG - CMD_IDX_LO + 1] = ChkMIs[D_REG];
//  g_signal_connect (G_OBJECT(ChkMIs[T_REG]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_TREG);
//  Cmd2MI[CMD_TREG - CMD_IDX_LO + 1] = ChkMIs[T_REG];
    g_signal_connect (G_OBJECT(ChkMIs[CHK_CMD_LOGVIEW]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_LOGVIEW);

    g_signal_connect (G_OBJECT(AboutMI), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_ABOUT);

    g_signal_connect (G_OBJECT(ChkMIs[WSChkIdx]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_WS_1);
    g_signal_connect (G_OBJECT(ChkMIs[WSChkIdx+1]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_WS_2);
    g_signal_connect (G_OBJECT(ChkMIs[WSChkIdx+2]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_WS_4);
    g_signal_connect (G_OBJECT(ChkMIs[WSChkIdx+3]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_WS_8);
    g_signal_connect (G_OBJECT(ChkMIs[WSChkIdx+4]), "activate", GTK_SIGNAL_FUNC(nbCmd_cb), (gpointer) CMD_WS16);

    g_signal_connect (G_OBJECT (IEntry), "key_press_event", G_CALLBACK (keydown_event_cb), NULL);
    g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (redirect_keydown), NULL);
    g_signal_connect (LV[0], "button_press_event", G_CALLBACK (RegMouseDown_cb), NULL);
    g_signal_connect (LV[0], "row-activated", GTK_SIGNAL_FUNC(Reg_DblClick), (gpointer) 0);
    g_signal_connect (LV[1], "button_press_event", G_CALLBACK (AsmMouseDown_cb), NULL);
    g_signal_connect (LV[1], "row-activated", GTK_SIGNAL_FUNC(Asm_DblClick), (gpointer) 1);
    g_signal_connect (LV[2], "button_press_event", G_CALLBACK (MemMouseDown_cb), NULL);
    g_signal_connect (LV[2], "row-activated", GTK_SIGNAL_FUNC(Mem_DblClick), NULL);

    g_signal_connect (G_OBJECT (LVEvtBox), "leave_notify_event", G_CALLBACK (LvLeave), NULL);
    g_signal_connect (G_OBJECT (VSepEvtBox1), "enter_notify_event", G_CALLBACK (VsEnter), (gpointer) 0);
    g_signal_connect (G_OBJECT (VSepEvtBox2), "enter_notify_event", G_CALLBACK (VsEnter), (gpointer) 1);
    g_signal_connect (G_OBJECT (LVEvtBox), "button_press_event", G_CALLBACK (LV_MouseDown), NULL);
    g_signal_connect (G_OBJECT (LVEvtBox), "button_release_event", G_CALLBACK (LEB_MouseUp), NULL);
    g_signal_connect (G_OBJECT (LV[0]), "button_release_event", G_CALLBACK (LV_MouseUp), NULL);
    g_signal_connect (G_OBJECT (LV[1]), "button_release_event", G_CALLBACK (LV_MouseUp), NULL);
    g_signal_connect (G_OBJECT (LV[2]), "button_release_event", G_CALLBACK (LV_MouseUp), NULL);
}

// force the screen to "get current" every half a second
static gboolean VGAWrefreshTick(GtkWidget *widget)
{
    if (bx_user_quit != FALSE)      // may be in the middle of a "destroy"
        return FALSE;
    if (SizingDelay > 0)
    {
        GdkDisplay * display = gdk_display_get_default();
        gint x;
        gint y;     // enter pre-dock mode
        int i, j;
        --SizingDelay;
        gdk_display_get_pointer (display, NULL, &x, &y, NULL);  // absolute x and y mouse coords
        i = x - xClick;
        j = y - yClick;     // calculate the pixel delta from the mousedown event to now
        if (i < 0) i= -i;
        if (j < 0) j= -j;
        if ((SizingDelay == 1 && (i > OneCharWide || j > OneCharWide)) || SizingDelay == 0)
        {
        // transition from predock to full dock mode
            xClick = -1;
            if (SizeList == 0) return FALSE;
            gtk_event_box_set_above_child (GTK_EVENT_BOX(VSepEvtBox1), FALSE);
            gtk_event_box_set_above_child (GTK_EVENT_BOX(VSepEvtBox2), FALSE);
            gtk_event_box_set_above_child (GTK_EVENT_BOX(LVEvtBox), TRUE);
            Sizing = SizeList;      // Sizing tells which ListView is being moved
            gdk_window_set_cursor (GTK_WIDGET(LVEvtBox)->window, DockCurs);
        }
    }
    UpdateStatus();
    if (PO_Tdelay > 0)      // output window is delaying display of a partial line?
    {
        if (--PO_Tdelay == 0)   // on a timeout, add a lf to complete the line
            ParseIDText ("\n");
    }
    Invalidate (0);
    Invalidate (1);
    if (DViewMode != VIEW_PTREE)
        Invalidate (2);
    vgaw_refresh = TRUE;
    return TRUE;
}

// Need to keep the bochs thread and the gtk_main threads separate -- bochs thread must not
// call GTK functions -- so it needs to set flags to initiate OnBreak, and OutputText updates
static gboolean BochsUpdateTick(GtkWidget *widget)
{
    if (bx_user_quit != FALSE)      // may be in the middle of a "destroy"
        return FALSE;
    if (UpdateOWflag != FALSE)
    {
        // replace the entire Output window's text, then scroll the window to the end
        GtkTextBuffer *OBuffer;     // pull model out of the widget
        UpdateOWflag = FALSE;
        OBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(OText));
        gtk_text_buffer_set_text(OBuffer, OutWindow, -1);
        gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(OText), EndMark, (gdouble) 0, TRUE, (gdouble) 1, (gdouble) 1);
    }
    if (HitBrkflag != FALSE)
        OnBreak();
    HitBrkflag = FALSE;
    return TRUE;
}


// called from *bochs* at a simulation break, or other ID command completion --
// so it must not call GTK functions directly, only through a shared memory flag
void HitBreak()
{
    if (AtBreak == FALSE)
    {
        AtBreak = TRUE;
        StatusChange = TRUE;
    }
    if (bx_user_quit == FALSE)      // may be in the middle of a "destroy"
        HitBrkflag = TRUE;
}

// special paint procedure for any colors on ListViews
void ListClr_PaintCb(GtkTreeViewColumn *col,
        GtkCellRenderer *renderer,
        GtkTreeModel *model,
        GtkTreeIter *iter,
        gpointer acolnum)
{
    int colnum = GPOINTER_TO_INT ( acolnum );
    gint rownum = 0;

    if (AtBreak == FALSE)
        g_object_set(renderer, "cell-background-gdk", &mgray, "cell-background-set", TRUE, NULL);
    else if (colnum <3)
    {
        // painting REG_WND -- recover the row number for this iter
        gtk_tree_model_get (model, iter, 3, &rownum, -1);
//L HIHI verify rownum in all 3 sections

        if (colnum == 0 && SelReg[rownum] != FALSE)     // is this row selected?
        {
            // if selected, foreground of column 0 = white, background = blue
            g_object_set(renderer, "foreground-gdk", &white, "foreground-set", TRUE, NULL);
            g_object_set(renderer, "cell-background-gdk", &AsmColors[3], "cell-background-set", TRUE, NULL);
            return;
        }
            // set fg (and bg if requested) according to color list
        Bit8u ClrFlgs = RegColor[ RitemToRnum[rownum] ];
        // RitemToRnum converts a ListView row number to the corresponding Register number.
        // RegColor has the 0x80 bit set if the register is supposed to be red.
        // Background color index is in the low nibble of ClrFlgs/RegColor.
        if (SeeRegColors != FALSE)
            g_object_set(renderer, "cell-background-gdk", &ColorList[ClrFlgs &7], "cell-background-set", TRUE, NULL);
        else
            g_object_set(renderer, "cell-background-gdk", &white, "cell-background-set", TRUE, NULL);

        if ((ClrFlgs & 0x80) != 0)      // should this register be red?
            g_object_set(renderer, "foreground-gdk", &fg_red, "foreground-set", TRUE, NULL);
        else
            g_object_set(renderer, "foreground-gdk", &fg_black, "foreground-set", TRUE, NULL);
    }
    else if (colnum < 6)
    {
        // painting ASM_WND -- recover the row number and "Linear Address" for this iter
        gtk_tree_model_get (model, iter, 3, &rownum, -1);
        bx_address h = (bx_address) AsmLA[rownum];

        if (colnum == 3 && SelAsm[rownum] != FALSE)     // is this row selected?
        {
            // if selected, foreground of column 0 = white, background = blue
            g_object_set(renderer, "foreground-gdk", &white, "foreground-set", TRUE, NULL);
            g_object_set(renderer, "cell-background-gdk", &AsmColors[3], "cell-background-set", TRUE, NULL);
            g_object_set(renderer, "weight", PANGO_WEIGHT_NORMAL, "weight-set", TRUE,
                "style", PANGO_STYLE_NORMAL, "style-set", TRUE, NULL);
            return;
        }
        // clear out any gray background
        g_object_set(renderer, "cell-background-gdk", &white, "cell-background-set", TRUE, NULL);
        // use an ellipsis ... on the bytes column
        if (colnum == 4)
            g_object_set(renderer, "ellipsize", PANGO_ELLIPSIZE_END, "ellipsize-set", TRUE, NULL);
        else
            g_object_set(renderer, "ellipsize", PANGO_ELLIPSIZE_NONE, "ellipsize-set", TRUE, NULL);
        int AsmClrNum = 0;          // assume black text
        if (h == CurrentAsmLA)
            AsmClrNum = 1;          // current opcode is colored dark green
        int j= BreakCount;
        while (--j >= 0)            // loop over all breakpoints
        {
            // brk list is sorted -- if the value goes too low, end the loop
            if (BrkLAddr[j] < h)
                j = 0;      // force the loop to end if it goes too far
            else if (BrkLAddr[j] == h)
            {
                j= 0;
                if (h == CurrentAsmLA)
                    AsmClrNum = 3;  // breakpoint @ current opcode = blue
                else
                    AsmClrNum = 2;  // active breakpoint is red
            }
        }
        g_object_set(renderer, "foreground-gdk", &AsmColors[AsmClrNum], "foreground-set", TRUE, NULL);
        // extra visuals for current RIP and breakpoints -- use bold and italic on mnemonics
        if (colnum == 5 && AsmClrNum != 0)
        {
            if ((AsmClrNum&1) != 0)     // current RIP
                g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD, "weight-set", TRUE, NULL);
            if ((AsmClrNum&2) != 0)     // breakpoint
                g_object_set(renderer, "style", PANGO_STYLE_ITALIC, "style-set", TRUE, NULL);
        }
        else
        {
            g_object_set(renderer, "weight", PANGO_WEIGHT_NORMAL, "weight-set", TRUE,
                "style", PANGO_STYLE_NORMAL, "style-set", TRUE, NULL);
        }
    }
    else
    {
        // painting some kind of dump window -- stack and physdump are special
        // clear out any gray background
        gtk_tree_model_get (model, iter, 18, &rownum, -1);
        g_object_set(renderer, "cell-background-gdk", &white, "cell-background-set", TRUE, NULL);
        if (DViewMode == VIEW_STACK)
        {
            // highlight changed lines in stack
            if (rownum < 0 || rownum >= STACK_ENTRIES)  // make sure the line # is legal
                ;
            else if (StackEntChg[rownum] != FALSE)
                g_object_set(renderer, "foreground-gdk", &fg_red, "foreground-set", TRUE, NULL);
            else
                g_object_set(renderer, "foreground-gdk", &fg_black, "foreground-set", TRUE, NULL);
            return;
        }

        // is this row selected (only applies to Memdump windows)?
        if (colnum == 6 && SelMem[rownum] != FALSE && DViewMode == VIEW_MEMDUMP)
        {
            // if selected, foreground of column 0 = white, background = blue
            g_object_set(renderer, "foreground-gdk", &white, "foreground-set", TRUE, NULL);
            g_object_set(renderer, "cell-background-gdk", &AsmColors[3], "cell-background-set", TRUE, NULL);
            return;
        }
        // the only other special painting is for Phys Dump columns, with byte DumpAlign (watchpoints)
        int DmpClrNum = 0;          // assume black text
        if (colnum > 6 && DumpAlign == 1 && LinearDump == FALSE && DViewMode == VIEW_MEMDUMP)
        {
            // For each cell, calculate its equivalent physmem address
            // byte offset = 0 means colnum = 7
            bx_phy_address h = (bx_phy_address) DumpStart + 16*rownum + colnum - 7;
            int j = num_write_watchpoints;
            int i = num_read_watchpoints;
            while (j > 0)
            {
                if (write_watchpoint[--j].addr == h)
                {
                    DmpClrNum = 1;  // write watchpoint
                    j = -1;         // on a match j<0 -- else j == 0
                }
            }
            while (--i >= 0)
            {
                if (read_watchpoint[i].addr == h)
                {
                    if (j < 0)         // BOTH read and write
                        DmpClrNum = 2;
                    else
                        DmpClrNum = 3; // read watchpoint
                    i = 0;             // end the loop on a match
                }
            }
        }
        g_object_set(renderer, "foreground-gdk", &PDumpClr[DmpClrNum], "foreground-set", TRUE, NULL);
        if (DmpClrNum != 0)
            g_object_set(renderer, "cell-background-gdk", &fg_black, "cell-background-set", TRUE, NULL);
    }
}

// build the columns for each of the three main lists
void MakeList(int listnum, const char *ColNameArray[])
{
    int i;
    int nColumns = 3;
    char txt[4];
    GtkListStore *Database;     // "model" that contains the actual data
    GtkCellRenderer *renderer;  // column/cell drawing object
    GtkTreeViewColumn *column;  // column object
    GtkTreeSelection *sel;      // need the tree's selection object

    sel = gtk_tree_view_get_selection (GTK_TREE_VIEW( LV[listnum]) );
    gtk_tree_selection_set_mode (sel, GTK_SELECTION_NONE);

    // format for next funct is ( # of columns, datatype, ... ); -- really stupid argument arrangement!
    if (listnum != DUMP_WND)
        Database = gtk_list_store_new (4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
    else        // 18 columns
    {
        nColumns = 18;
        Database = gtk_list_store_new (19, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
            G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
            G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
            G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
    }

    char *cp = (char*)ColNameArray[0];
    *txt = '0' -1;
    txt[1] = 0;
    // allocate one renderer for each ListView
    renderer = gtk_cell_renderer_text_new();
    LV_Rend[listnum]= renderer;
    for (i=0 ; i < nColumns ; i++)
    {
        // when nColumns == 18, there are only two entries in ColNameArray
        if (nColumns == 3)
            cp = (char*)ColNameArray[i];
        column = gtk_tree_view_column_new_with_attributes(cp,
            renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(LV[listnum]), column);
        gtk_tree_view_column_set_cell_data_func(column, renderer, ListClr_PaintCb, (gpointer) (glong) ((listnum*3) + i), NULL);

    // Note: column and renderer are not technically "GTK Objects", so don't unref them
        cp = txt;
        ++*cp;      // cheap way to create "0" through "9" and "A" through "F"
        if (i == 10) *cp = 'A';
        else if (i == 16) cp = (char*)ColNameArray[1];
    // must store copies of all the columns, to change column titles and other "properties" later
        AllCols[(listnum*3) + i] = column;
    }

    gtk_tree_view_set_model(GTK_TREE_VIEW(LV[listnum]), GTK_TREE_MODEL(Database));
    // ListView now holds a reference to Database -- don't need the local reference anymore
    g_object_unref (G_OBJECT (Database));
}

// gtk version of multithreading (doesn't seem to work on some platforms?):
/*
// this function must be called as a new multithread, from the initialization code
// -- it runs the main gtk "event loop", which spawns all the "events",
// "repaint"s, and callbacks -- and never returns, except on a "quit"
gpointer EventLp(gpointer data)
{
    gtk_main();
    g_thread_exit(0);
    return (gpointer) NULL;
}

void MakeGTKthreads()
{
    // do gtk multithreading initialization FIRST -- before ANYTHING ELSE
    GError *error = NULL;
#ifndef G_THREADS_ENABLED
    printf ("init err1: thread support not enabled in glib\n");
    return FALSE;
#endif
#ifndef G_THREADS_IMPL_POSIX
    printf ("init err2: pthread package not found\n");
    return FALSE;
#endif
    if (g_thread_supported() == FALSE)
    {
        printf ("init err3: pthreads/multithreading is not running? No multithread support.\n");
        return FALSE;
    }
    g_thread_init(NULL);
    g_thread_create (EventLp, (gpointer) NULL, FALSE, &error);
}
*/

// multithreading using pure posix threads -- not glib threads
void * EventLp(void *data)
{
    gtk_main();
//  pthread_exit(0);
    return NULL;
}

void MakeGTKthreads()
{
    pthread_t hThread;
//  g_thread_init(NULL);    // if I put this in, it gives me an "undefined reference" of all crazy things!
    pthread_create (&hThread, NULL, EventLp, NULL);
}

// This function must be called immediately after bochs starts
bx_bool OSInit()
{
    int i, argc;
    char *argv[2], **argvp;
    const char* LColNames[] = {
        "Reg Name","Hex Value","Decimal","L.Address","Bytes","Mnemonic","Address","Ascii"
    };
    GtkTextBuffer *Tbuffer;
    GtkTextIter Titer;
    GtkWidget *tmpLbl;
    *bigbuf = 0;            // gtk flames out if you pass in a NULL -- sheesh
    argv[0] = bigbuf;   // so I really do have to fake up an "argv" list
    argv[1] = NULL;
    argvp = argv;
    argc = 1;

    CurXSize = 0;
    if (!SIM->is_wx_selected()) {
      // you MUST call gtk_init, even with faked arguments, because it inits GDK and Glib
      if (gtk_init_check(&argc, &argvp) == FALSE) {
        fprintf(stderr, "gtk init failed, can not access display?\n");
        return FALSE;
      }
    }
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Bochs Enhanced Debugger");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
    gtk_window_maximize(GTK_WINDOW(window));

    MainVbox = gtk_vbox_new(FALSE, 0);      // vbox that contains EVERYTHING

    gtk_container_add(GTK_CONTAINER(window), MainVbox);

    // make/init the menu "containers" (shells) and menu items
    InitMenus();

    // build the StatusBar
    StatHbox = gtk_hbox_new(FALSE, 3);
    Stat[0] = gtk_label_new("Break");
    gtk_label_set_width_chars (GTK_LABEL(Stat[0]), 7);
    Stat[1] = gtk_label_new("CPU:");
    gtk_label_set_width_chars (GTK_LABEL(Stat[1]), 23);
    Stat[2] = gtk_label_new("t=0");
    gtk_label_set_width_chars (GTK_LABEL(Stat[2]), 19);
    Stat[3] = gtk_label_new("IOPL");
    StatVSep1 = gtk_vseparator_new();
    StatVSep2 = gtk_vseparator_new();
    StatVSep3 = gtk_vseparator_new();
    gtk_box_pack_start(GTK_BOX(StatHbox), Stat[0], FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(StatHbox), StatVSep1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(StatHbox), Stat[1], FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(StatHbox), StatVSep2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(StatHbox), Stat[2], FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(StatHbox), StatVSep3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(StatHbox), Stat[3], TRUE, TRUE, 0);
    // set LEFT, instead of the default centered "justification" of status text
    gtk_misc_set_alignment(GTK_MISC(Stat[0]), (gfloat) 0., (gfloat) 0.5);
    gtk_misc_set_alignment(GTK_MISC(Stat[1]), (gfloat) 0., (gfloat) 0.5);
    gtk_misc_set_alignment(GTK_MISC(Stat[2]), (gfloat) 0., (gfloat) 0.5);
    gtk_misc_set_alignment(GTK_MISC(Stat[3]), (gfloat) 0., (gfloat) 0.5);

    gtk_box_pack_start(GTK_BOX(MainVbox), menubar, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(MainVbox), StatHbox, FALSE, FALSE, 0);

    // build the table that holds the TreeView/Input&Output windows
    TreeTbl = gtk_table_new(4, 1, TRUE);    // proportion the remaining space in quarters, vertically
    sep8 = gtk_vseparator_new();            // vertical separators between the ListViews
    sep9 = gtk_vseparator_new();
    VSepEvtBox1 = gtk_event_box_new();      // seps need event boxes to catch "Enter" events
    VSepEvtBox2 = gtk_event_box_new();
    gtk_container_add (GTK_CONTAINER(VSepEvtBox1), sep8);
    gtk_container_add (GTK_CONTAINER(VSepEvtBox2), sep9);
    gtk_widget_add_events(VSepEvtBox1, GDK_ENTER_NOTIFY_MASK);      // masks must be set immediately after creation
    gtk_widget_add_events(VSepEvtBox2, GDK_ENTER_NOTIFY_MASK);
    IOVbox = gtk_vbox_new(FALSE, 0);        // stack the Input Entry and Output TextView vertically

    // create Input window
    IEntry = gtk_entry_new();
    gtk_entry_set_max_length (GTK_ENTRY(IEntry), 190);

    // create Output window
    ScrlWinOut = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(ScrlWinOut), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    OText = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(OText), FALSE);    // set it to "read only"
    GTK_WIDGET_UNSET_FLAGS(OText, GTK_CAN_FOCUS);
    gtk_container_add (GTK_CONTAINER(ScrlWinOut), OText);
// a TextView *already has* a viewport, so DON'T add another one -- or it won't scroll properly!
//  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(ScrlWinOut), OText);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(OText), GTK_WRAP_WORD);
    // create a textmark that always points to the very end of OText
    Tbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(OText));
    gtk_text_buffer_get_iter_at_offset (Tbuffer, &Titer, 0);
    EndMark = gtk_text_buffer_create_mark (Tbuffer, NULL, &Titer, FALSE);

    gtk_box_pack_start(GTK_BOX(IOVbox), ScrlWinOut, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(IOVbox), IEntry, FALSE, FALSE, 0);
    LVHbox = gtk_hbox_new(FALSE, 0);        // to hold the ListView windows
    LVEvtBox = gtk_event_box_new();     // need to catch "leave" and "move" events for ListViews
    gtk_container_add (GTK_CONTAINER(LVEvtBox), LVHbox);
    gtk_widget_add_events(LVEvtBox, GDK_LEAVE_NOTIFY_MASK | GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK);

    // make Command and CPU Button rows
    CmdBHbox = gtk_hbox_new(TRUE, 0);

    for (i=0 ; i < 5 ; i++)
    {
        CmdBtn[i] = gtk_button_new();
        tmpLbl = gtk_label_new ("");
        sprintf (bigbuf, "<b>%s</b>",BTxt[i]);      // create "bold" pango markup for buttons
        gtk_label_set_markup (GTK_LABEL(tmpLbl), bigbuf);
        gtk_container_add (GTK_CONTAINER(GTK_BUTTON(CmdBtn[i])), tmpLbl);
        GTK_WIDGET_UNSET_FLAGS(CmdBtn[i], GTK_CAN_FOCUS);
        gtk_box_pack_start(GTK_BOX(CmdBHbox), CmdBtn[i], TRUE, TRUE, 0);
    }
    gtk_box_pack_start(GTK_BOX(MainVbox), CmdBHbox, FALSE, FALSE, 0);
    // CPU button row does not exist for only 1 cpu
    if (BX_SMP_PROCESSORS > 1)
    {
        CpuBHbox = gtk_hbox_new(TRUE, 0);
        gtk_box_pack_start(GTK_BOX(MainVbox), CpuBHbox, FALSE, FALSE, 0);
        i = 0;
        strcpy (bigbuf, "<b>CPU0</b>");     // Handle CPU0 specially -- it is "selected"
        while (i < BX_SMP_PROCESSORS)
        {
            CpuBtn[i] = gtk_button_new();
            tmpLbl = gtk_label_new ("");
            CpuB_label[i] = tmpLbl;
            gtk_label_set_markup (GTK_LABEL(tmpLbl), bigbuf);
            gtk_container_add (GTK_CONTAINER(GTK_BUTTON(CpuBtn[i])), tmpLbl);
            GTK_WIDGET_UNSET_FLAGS(CpuBtn[i], GTK_CAN_FOCUS);
            gtk_box_pack_start(GTK_BOX(CpuBHbox), CpuBtn[i], TRUE, TRUE, 0);
            sprintf (bigbuf, "<b>cpu%d</b>",++i);       // create the next CPU button label
        }
    }

    gtk_box_pack_end(GTK_BOX(MainVbox), TreeTbl, TRUE, TRUE, 0);

    gtk_table_attach_defaults(GTK_TABLE(TreeTbl), LVEvtBox, 0, 1, 0, 3);  // order = LRTB
    gtk_table_attach_defaults(GTK_TABLE(TreeTbl), IOVbox, 0, 1, 3, 4);
    VSizeChange();

    // build the 3 ListView windows
    i = 3;
    while (--i >= 0)
    {
        ScrlWin[i] = gtk_scrolled_window_new( NULL, NULL );
        // always want a horizontal scrollbar, to divide lists from Output window visually
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(ScrlWin[i]), GTK_POLICY_ALWAYS, GTK_POLICY_AUTOMATIC);
        LV[i] = gtk_tree_view_new();
        GTK_WIDGET_UNSET_FLAGS(LV[i], GTK_CAN_FOCUS);
        gtk_container_add (GTK_CONTAINER(ScrlWin[i]), LV[i]);
// all TreeViews *already have* a viewport, so DON'T add another one with the ScrollWindow, or it won't scroll properly!
        MakeList(i, &LColNames[i*3]);
    }
    CurCenterList = ((DockOrder>>4)& 3) -1;     // index of center list
    RepackLists((DockOrder >> 8) -1);           // index of left list

    // make the param_tree widget
    GtkTreeViewColumn *treecol;
    GtkCellRenderer *renderer;
    GtkTreeStore *treestore;
    PTree = gtk_tree_view_new();
    treecol = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(treecol, "bochs parameter tree");
    gtk_tree_view_append_column(GTK_TREE_VIEW(PTree), treecol);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(treecol, renderer, TRUE);
    gtk_tree_view_column_add_attribute(treecol, renderer, "text", 0);   // pull display text from treestore col 0
    GTK_WIDGET_UNSET_FLAGS(PTree, GTK_CAN_FOCUS);
    treestore = gtk_tree_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(PTree), GTK_TREE_MODEL(treestore));
    g_object_unref(treestore);      // PTree now references treestore itself

    gtk_widget_ref (PTree);     // keep an extra ref to both ScrlWin[2] widgets,
    gtk_widget_ref (LV[2]);     // so they don't get deleted when removed from their container
    gtk_object_sink (GTK_OBJECT (PTree));   // the PTree reference is "floating", so "sink" it

    AttachSignals();        // this must be called AFTER InitMenus()
    TakeInputFocus();
    gtk_widget_show_all(window);

    // In order to catch Enter/Leave events, you need a widget with a Gdk Window. Neither hboxes or VSeps have them,
    // so it is necessary to add event boxes. The windows should be invisible, and when I want them to WORK, they
    // must be ON TOP of the child widgets. LVEvtBox is only needed in resize/dock mode.
    gtk_event_box_set_visible_window (GTK_EVENT_BOX(LVEvtBox), FALSE);
    gtk_event_box_set_above_child (GTK_EVENT_BOX(LVEvtBox), FALSE);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX(VSepEvtBox1), FALSE);
    gtk_event_box_set_above_child (GTK_EVENT_BOX(VSepEvtBox1), TRUE);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX(VSepEvtBox2), FALSE);
    gtk_event_box_set_above_child (GTK_EVENT_BOX(VSepEvtBox2), TRUE);
    // build the sizing/docking cursors
    SizeCurs = gdk_cursor_new (GDK_SB_H_DOUBLE_ARROW);
    DockCurs = gdk_cursor_new (GDK_CROSSHAIR);
    gdk_cursor_ref (SizeCurs);
    gdk_cursor_ref (DockCurs);

    // the OneCharWide pixel value is used to limit docking and resizing movements
    int width;
    PangoLayout * layout = gtk_widget_create_pango_layout (LV[0], "M");
    PangoFontDescription * fontdesc;
    g_object_get( G_OBJECT( LV_Rend[0] ), "font-desc", &fontdesc, NULL ); 
    pango_layout_set_font_description (layout, fontdesc); 
    pango_layout_get_pixel_size (layout, &width, &i);
    pango_font_description_free (fontdesc);
    g_object_unref (layout);
    OneCharWide = width;        // The "width" I'm getting is about half what I expect
//  OneCharWide = width >> 1;   // pretend that an average char width is half an "M"
    if (OneCharWide > 12) OneCharWide = 12;

//  UpdInProgress[REG_WND] = FALSE;
//  UpdInProgress[ASM_WND] = FALSE;
//  UpdInProgress[DUMP_WND] = FALSE;
    ResizeColmns = TRUE;    // force an initial resize on hex, address columns

    g_timeout_add(500, (GSourceFunc) VGAWrefreshTick, (gpointer) window);
    g_timeout_add(50, (GSourceFunc) BochsUpdateTick, (gpointer) window);

    g_signal_connect (G_OBJECT (window), "configure_event", G_CALLBACK (TblSizeEvent), NULL);
    // could use: gtk_tree_view_column_get_width(), etc.
    AllCols[4]->fixed_width = AllCols[4]->width;
    AllCols[4]->column_type = GTK_TREE_VIEW_COLUMN_FIXED;

    if (!SIM->is_wx_selected()) {
      MakeGTKthreads();
    }
    return TRUE;
}

void CloseDialog()
{
  if (window != NULL) {
    gtk_widget_destroy(window);
  }
}

// recurse displaying each leaf/branch of param_tree -- with values for each leaf
void MakeBL(TreeParent *h_P, bx_param_c *p)
{
    TreeParent h_new;
    bx_list_c *as_list = NULL;
    int n = 0;
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
            n = as_list->get_size();
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
    MakeTreeChild (h_P, n, &h_new);
    if (n > 0)
    {
        for (int i=0; i<n; i++)
            MakeBL(&h_new, as_list->get(i));    // recurse for all children that are lists
    }
}

bx_bool ParseOSSettings(const char *param, const char *value)
{
  // TODO: handle GTK-specific settings here
  return 0;
}

void WriteOSSettings(FILE *fd)
{
  // TODO: handle GTK-specific settings here
}

#endif
