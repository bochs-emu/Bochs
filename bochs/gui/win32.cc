/////////////////////////////////////////////////////////////////////////
// $Id: win32.cc,v 1.84 2004-08-15 19:27:14 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

//  Much of this file was written by:
//  David Ross
//  dross@pobox.com

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#include "iodev.h"
#if BX_WITH_WIN32

#include "win32res.h"
#include "font/vga.bitmap.h"
// windows.h is included by bochs.h
#include <commctrl.h>
#include <process.h>

class bx_win32_gui_c : public bx_gui_c {
public:
  bx_win32_gui_c (void) {}
  DECLARE_GUI_VIRTUAL_METHODS()
  virtual void statusbar_setitem(int element, bx_bool active);
};

// declare one instance of the gui object and call macro to insert the
// plugin code
static bx_win32_gui_c *theGui = NULL;
IMPLEMENT_GUI_PLUGIN_CODE(win32)

#define LOG_THIS theGui->

#define EXIT_GUI_SHUTDOWN        1
#define EXIT_GMH_FAILURE         2
#define EXIT_FONT_BITMAP_ERROR   3
#define EXIT_NORMAL              4
#define EXIT_HEADER_BITMAP_ERROR 5

/*  FIXME: Should we add a bochsrc option to control the font usage? */
#define BX_USE_WINDOWS_FONTS 0

// Keyboard/mouse stuff
#define SCANCODE_BUFSIZE    20
#define MOUSE_PRESSED       0x20000000
#define HEADERBAR_CLICKED   0x08000000
#define MOUSE_MOTION        0x22000000
void enq_key_event(Bit32u, Bit32u);
void enq_mouse_event(void);

struct QueueEvent {
  Bit32u key_event;
  int mouse_x;
  int mouse_y;
  int mouse_button_state;
};
QueueEvent* deq_key_event(void);

static QueueEvent keyevents[SCANCODE_BUFSIZE];
static unsigned head=0, tail=0;
static int mouse_button_state = 0;
static int ms_xdelta=0, ms_ydelta=0;
static int ms_lastx=0, ms_lasty=0;
static int ms_savedx=0, ms_savedy=0;
static BOOL mouseCaptureMode, mouseCaptureNew, mouseToggleReq;
static unsigned long workerThread = 0;
static DWORD workerThreadID = 0;

// Graphics screen stuff
static unsigned x_tilesize = 0, y_tilesize = 0;
static BITMAPINFO* bitmap_info=(BITMAPINFO*)0;
static RGBQUAD* cmap_index;  // indeces into system colormap
static HBITMAP MemoryBitmap = NULL;
static HDC MemoryDC = NULL;
static RECT updated_area;
static BOOL updated_area_valid = FALSE;

// Headerbar stuff
HWND hwndTB, hwndSB;
unsigned bx_bitmap_entries;
struct {
  HBITMAP bmap;
  unsigned xdim;
  unsigned ydim;
} bx_bitmaps[BX_MAX_PIXMAPS];

static struct {
  unsigned bmap_id;
  void (*f)(void);
} bx_headerbar_entry[BX_MAX_HEADERBAR_ENTRIES];

static unsigned bx_headerbar_y = 0;
static unsigned bx_statusbar_y = 0;
static int bx_headerbar_entries;
static unsigned bx_hb_separator;

// Status Bar stuff
#define SIZE_OF_SB_ELEMENT        40
#define SIZE_OF_SB_FIRST_ELEMENT 160
long SB_Edges[BX_MAX_STATUSITEMS+2];
char SB_Text[BX_MAX_STATUSITEMS][10];
bx_bool SB_Active[BX_MAX_STATUSITEMS];

// Misc stuff
static unsigned dimension_x, dimension_y, current_bpp;
static unsigned stretched_x, stretched_y;
static unsigned stretch_factor=1;
static unsigned prev_cursor_x = 0;
static unsigned prev_cursor_y = 0;
static HBITMAP vgafont[256];
static unsigned x_edge=0, y_edge=0, y_caption=0;
static int xChar = 8, yChar = 16;
static unsigned int text_rows=25, text_cols=80;
static BOOL BxTextMode = TRUE;
static BOOL legacyF12 = FALSE;
#if !BX_USE_WINDOWS_FONTS
static Bit8u h_panning = 0, v_panning = 0;
static Bit16u line_compare = 1023;
#else
static HFONT hFont[3];
static int FontId = 2;
#endif

static char szAppName[] = "Bochs for Windows";
static char szWindowName[] = "Bochs for Windows - Display";

typedef struct {
  HINSTANCE hInstance;

  CRITICAL_SECTION drawCS;
  CRITICAL_SECTION keyCS;
  CRITICAL_SECTION mouseCS;

  int kill;  // reason for terminateEmul(int)
  BOOL UIinited;
  HWND mainWnd;
  HWND simWnd;
} sharedThreadInfo;

sharedThreadInfo stInfo;

LRESULT CALLBACK mainWndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK simWndProc (HWND, UINT, WPARAM, LPARAM);
VOID UIThread(PVOID);
void terminateEmul(int);
void create_vga_font(void);
static unsigned char reverse_bitorder(unsigned char);
void DrawBitmap (HDC, HBITMAP, int, int, int, int, int, int, DWORD, unsigned char);
void DrawChar (HDC, unsigned char, int, int, unsigned char cColor, int, int);
void updateUpdated(int,int,int,int);
static void headerbar_click(int x);
#if BX_USE_WINDOWS_FONTS
void InitFont(void);
void DestroyFont(void);
#endif


Bit32u win32_to_bx_key[2][0x100] =
{
  { /* normal-keys */
    /* 0x00 - 0x0f */
    0,
    BX_KEY_ESC,
    BX_KEY_1,
    BX_KEY_2,
    BX_KEY_3,
    BX_KEY_4,
    BX_KEY_5,
    BX_KEY_6,
    BX_KEY_7,
    BX_KEY_8,
    BX_KEY_9,
    BX_KEY_0,
    BX_KEY_MINUS,
    BX_KEY_EQUALS,
    BX_KEY_BACKSPACE,
    BX_KEY_TAB,
    /* 0x10 - 0x1f */
    BX_KEY_Q,
    BX_KEY_W,
    BX_KEY_E,
    BX_KEY_R,
    BX_KEY_T,
    BX_KEY_Y,
    BX_KEY_U,
    BX_KEY_I,
    BX_KEY_O,
    BX_KEY_P,
    BX_KEY_LEFT_BRACKET,
    BX_KEY_RIGHT_BRACKET,
    BX_KEY_ENTER,
    BX_KEY_CTRL_L,
    BX_KEY_A,
    BX_KEY_S,
    /* 0x20 - 0x2f */
    BX_KEY_D,
    BX_KEY_F,
    BX_KEY_G,
    BX_KEY_H,
    BX_KEY_J,
    BX_KEY_K,
    BX_KEY_L,
    BX_KEY_SEMICOLON,
    BX_KEY_SINGLE_QUOTE,
    BX_KEY_GRAVE,
    BX_KEY_SHIFT_L,
    BX_KEY_BACKSLASH,
    BX_KEY_Z,
    BX_KEY_X,
    BX_KEY_C,
    BX_KEY_V,
    /* 0x30 - 0x3f */
    BX_KEY_B,
    BX_KEY_N,
    BX_KEY_M,
    BX_KEY_COMMA,
    BX_KEY_PERIOD,
    BX_KEY_SLASH,
    BX_KEY_SHIFT_R,
    BX_KEY_KP_MULTIPLY,
    BX_KEY_ALT_L,
    BX_KEY_SPACE,
    BX_KEY_CAPS_LOCK,
    BX_KEY_F1,
    BX_KEY_F2,
    BX_KEY_F3,
    BX_KEY_F4,
    BX_KEY_F5,
    /* 0x40 - 0x4f */
    BX_KEY_F6,
    BX_KEY_F7,
    BX_KEY_F8,
    BX_KEY_F9,
    BX_KEY_F10,
    BX_KEY_PAUSE,
    BX_KEY_SCRL_LOCK,
    BX_KEY_KP_HOME,
    BX_KEY_KP_UP,
    BX_KEY_KP_PAGE_UP,
    BX_KEY_KP_SUBTRACT,
    BX_KEY_KP_LEFT,
    BX_KEY_KP_5,
    BX_KEY_KP_RIGHT,
    BX_KEY_KP_ADD,
    BX_KEY_KP_END,
    /* 0x50 - 0x5f */
    BX_KEY_KP_DOWN,
    BX_KEY_KP_PAGE_DOWN,
    BX_KEY_KP_INSERT,
    BX_KEY_KP_DELETE,
    0,
    0,
    BX_KEY_LEFT_BACKSLASH,
    BX_KEY_F11,
    BX_KEY_F12,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    /* 0x60 - 0x6f */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    /* 0x70 - 0x7f */
    0,                  /* Todo: "Katakana" key ( ibm 133 ) for Japanese 106 keyboard */
    0,
    0,
    0,                  /* Todo: "Ro" key ( ibm 56 ) for Japanese 106 keyboard */
    0,
    0,
    0,
    0,
    0,
    0,                  /* Todo: "convert" key ( ibm 132 ) for Japanese 106 keyboard */
    0,
    0,                  /* Todo: "non-convert" key ( ibm 131 ) for Japanese 106 keyboard */
    0,
    0,                  /* Todo: "Yen" key ( ibm 14 ) for Japanese 106 keyboard */
    0,
    0,
  },
  { /* extended-keys */
    /* 0x00 - 0x0f */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    /* 0x10 - 0x1f */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    BX_KEY_KP_ENTER,
    BX_KEY_CTRL_R,
    0,
    0,
    /* 0x20 - 0x2f */
    0,
    BX_KEY_POWER_CALC,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    /* 0x30 - 0x3f */
    0,
    0,
    BX_KEY_INT_HOME,
    0,
    0,
    BX_KEY_KP_DIVIDE,
    0,
    0,                  /* ?? BX_KEY_PRINT ( ibm 124 ) ?? */
    BX_KEY_ALT_R,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    /* 0x40 - 0x4f */
    0,
    0,
    0,
    0,
    0,
    BX_KEY_NUM_LOCK,
    0,
    BX_KEY_HOME,
    BX_KEY_UP,
    BX_KEY_PAGE_UP,
    0,
    BX_KEY_LEFT,
    0,
    BX_KEY_RIGHT,
    0,
    BX_KEY_END,
    /* 0x50 - 0x5f */
    BX_KEY_DOWN,
    BX_KEY_PAGE_DOWN,
    BX_KEY_INSERT,
    BX_KEY_DELETE,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    BX_KEY_WIN_L,
    BX_KEY_WIN_R,
    BX_KEY_MENU,
    BX_KEY_POWER_POWER,
    BX_KEY_POWER_SLEEP,
    /* 0x60 - 0x6f */
    0,
    0,
    0,
    BX_KEY_POWER_WAKE,
    0,
    BX_KEY_INT_SEARCH,
    BX_KEY_INT_FAV,
    0,
    BX_KEY_INT_STOP,
    BX_KEY_INT_FORWARD,
    BX_KEY_INT_BACK,
    BX_KEY_POWER_MYCOMP,
    BX_KEY_INT_MAIL,
    0,
    0,
    0,
  }
};

/* Macro to convert WM_ button state to BX button state */

#ifdef __MINGW32__
  VOID CALLBACK MyTimer(HWND,UINT,UINT,DWORD);
  void alarm(int);
  void bx_signal_handler(int);
#endif

static void processMouseXY( int x, int y, int windows_state, int implied_state_change)
{
  int bx_state;
  int old_bx_state;
  EnterCriticalSection( &stInfo.mouseCS);
  bx_state=( ( windows_state & MK_LBUTTON) ? 1 : 0 ) + ( ( windows_state & MK_RBUTTON) ? 2 : 0) +
           ( ( windows_state & MK_MBUTTON) ? 4 : 0);
  old_bx_state=bx_state ^ implied_state_change;
  if ( old_bx_state!=mouse_button_state)
  {
    /* Make up for missing message */
    BX_INFO(( "&&&missing mouse state change"));
    EnterCriticalSection( &stInfo.keyCS);
    enq_mouse_event();
    mouse_button_state=old_bx_state;
    enq_key_event( mouse_button_state, MOUSE_PRESSED);
    LeaveCriticalSection( &stInfo.keyCS);
  }
  ms_ydelta=ms_savedy-y;
  ms_xdelta=x-ms_savedx;
  ms_lastx=x;
  ms_lasty=y;
  if ( bx_state!=mouse_button_state)
  {
    EnterCriticalSection( &stInfo.keyCS);
    enq_mouse_event();
    mouse_button_state=bx_state;
    enq_key_event( mouse_button_state, MOUSE_PRESSED);
    LeaveCriticalSection( &stInfo.keyCS);
  }
  LeaveCriticalSection( &stInfo.mouseCS);
}

static void resetDelta()
{
  EnterCriticalSection( &stInfo.mouseCS);
  ms_savedx=ms_lastx;
  ms_savedy=ms_lasty;
  ms_ydelta=ms_xdelta=0;
  LeaveCriticalSection( &stInfo.mouseCS);
}

static void cursorWarped()
{
  EnterCriticalSection( &stInfo.mouseCS);
  EnterCriticalSection( &stInfo.keyCS);
  enq_mouse_event();
  LeaveCriticalSection( &stInfo.keyCS);
  ms_lastx=stretched_x/2;
  ms_lasty=stretched_y/2;
  ms_savedx=ms_lastx;
  ms_savedy=ms_lasty;
  LeaveCriticalSection( &stInfo.mouseCS);
}

// GUI thread must be dead/done in order to call terminateEmul
void terminateEmul(int reason) {

  // We know that Critical Sections were inited when x_tilesize has been set
  // See bx_win32_gui_c::specific_init
  if (x_tilesize != 0) {
    DeleteCriticalSection (&stInfo.drawCS);
    DeleteCriticalSection (&stInfo.keyCS);
    DeleteCriticalSection (&stInfo.mouseCS);
  }
  x_tilesize = 0;

  if (MemoryDC) DeleteDC (MemoryDC);
  if (MemoryBitmap) DeleteObject (MemoryBitmap);

  if ( bitmap_info) delete[] (char*)bitmap_info;

  for (unsigned b=0; b<bx_bitmap_entries; b++)
    if (bx_bitmaps[b].bmap) DeleteObject(bx_bitmaps[b].bmap);
  for (unsigned c=0; c<256; c++)
    if (vgafont[c]) DeleteObject(vgafont[c]);

  LOG_THIS setonoff(LOGLEV_PANIC, ACT_FATAL);

  switch (reason) {
  case EXIT_GUI_SHUTDOWN:
    BX_PANIC(("Window closed, exiting!"));
    break;
  case EXIT_GMH_FAILURE:
    BX_PANIC(("GetModuleHandle failure!"));
    break;
  case EXIT_FONT_BITMAP_ERROR:
    BX_PANIC(("Font bitmap creation failure!"));
    break;
  case EXIT_HEADER_BITMAP_ERROR:
    BX_PANIC(("Header bitmap creation failure!"));
    break;
  case EXIT_NORMAL:
    break;
  }
}


// ::SPECIFIC_INIT()
//
// Called from gui.cc, once upon program startup, to allow for the
// specific GUI code (X11, BeOS, ...) to be initialized.
//
// argc, argv: not used right now, but the intention is to pass native GUI
//     specific options from the command line.  (X11 options, BeOS options,...)
//
// tilewidth, tileheight: for optimization, graphics_tile_update() passes
//     only updated regions of the screen to the gui code to be redrawn.
//     These define the dimensions of a region (tile).
// headerbar_y:  A headerbar (toolbar) is display on the top of the
//     VGA window, showing floppy status, and other information.  It
//     always assumes the width of the current VGA mode width, but
//     it's height is defined by this parameter.

void bx_win32_gui_c::specific_init(int argc, char **argv, unsigned
                                   tilewidth, unsigned tileheight,
                                   unsigned headerbar_y)
{
  int i;

  put("WGUI");
  static RGBQUAD black_quad={ 0, 0, 0, 0};
  stInfo.kill = 0;
  stInfo.UIinited = FALSE;
  InitializeCriticalSection(&stInfo.drawCS);
  InitializeCriticalSection(&stInfo.keyCS);
  InitializeCriticalSection(&stInfo.mouseCS);

  x_tilesize = tilewidth;
  y_tilesize = tileheight;

  bx_bitmap_entries = 0;
  bx_headerbar_entries = 0;
  bx_hb_separator = 0;
  mouseCaptureMode = FALSE;
  mouseCaptureNew = FALSE;
  mouseToggleReq = FALSE;

  stInfo.hInstance = GetModuleHandle(NULL);

  UNUSED(headerbar_y);
  dimension_x = 640;
  dimension_y = 480;
  current_bpp = 8;
  stretched_x = dimension_x;
  stretched_y = dimension_y;
  stretch_factor = 1;

  for(unsigned c=0; c<256; c++) vgafont[c] = NULL;
  create_vga_font();

  bitmap_info=(BITMAPINFO*)new char[sizeof(BITMAPINFOHEADER)+
    256*sizeof(RGBQUAD)];
  bitmap_info->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
  bitmap_info->bmiHeader.biWidth=x_tilesize;
  // Height is negative for top-down bitmap
  bitmap_info->bmiHeader.biHeight= -y_tilesize;
  bitmap_info->bmiHeader.biPlanes=1;
  bitmap_info->bmiHeader.biBitCount=8;
  bitmap_info->bmiHeader.biCompression=BI_RGB;
  bitmap_info->bmiHeader.biSizeImage=x_tilesize*y_tilesize*4;
  // I think these next two figures don't matter; saying 45 pixels/centimeter
  bitmap_info->bmiHeader.biXPelsPerMeter=4500;
  bitmap_info->bmiHeader.biYPelsPerMeter=4500;
  bitmap_info->bmiHeader.biClrUsed=256;
  bitmap_info->bmiHeader.biClrImportant=0;
  cmap_index=bitmap_info->bmiColors;
  // start out with all color map indeces pointing to Black
  cmap_index[0] = black_quad;
  for (unsigned i=1; i<256; i++) {
    cmap_index[i] = cmap_index[0];
  }

  x_edge = GetSystemMetrics(SM_CXFIXEDFRAME);
  y_edge = GetSystemMetrics(SM_CYFIXEDFRAME);
  y_caption = GetSystemMetrics(SM_CYCAPTION);

  if (stInfo.hInstance)
    workerThread = _beginthread (UIThread, 0, NULL);
  else
    terminateEmul(EXIT_GMH_FAILURE);

  // Wait for a window before continuing
  if ((stInfo.kill == 0) && (FindWindow(szAppName, NULL) == NULL))
    Sleep(500);

  // Now set this thread's priority to below normal because this is where
  //  the emulated CPU runs, and it hogs the real CPU
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

  if (bx_options.Oprivate_colormap->get ())
    BX_INFO(( "private_colormap option ignored."));

  // load keymap tables
  if(bx_options.keyboard.OuseMapping->get()) {
    bx_keymap.loadKeymap(NULL);  // I have no function to convert X windows symbols
    }

  // parse win32 specific options
  if (argc > 1) {
    for (i = 1; i < argc; i++) {
      if (!strcmp(argv[i], "legacyF12")) {
        legacyF12 = TRUE;
      } else {
        BX_PANIC(("Unknown win32 option '%s'", argv[i]));
      }
    }
  }
}


// This thread controls the GUI window.
VOID UIThread(PVOID pvoid) {
  MSG msg;
  HDC hdc;
  WNDCLASS wndclass;
  RECT wndRect, wndRect2;

  workerThreadID = GetCurrentThreadId();

  wndclass.style = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc = mainWndProc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hInstance = stInfo.hInstance;
  wndclass.hIcon = LoadIcon (stInfo.hInstance, MAKEINTRESOURCE(ICON_BOCHS));
  wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName = szAppName;

  RegisterClass (&wndclass);

  wndclass.style = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc = simWndProc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hInstance = stInfo.hInstance;
  wndclass.hIcon = NULL;
  wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName = "SIMWINDOW";

  RegisterClass (&wndclass);

  stInfo.mainWnd = CreateWindow (szAppName,
                     szWindowName,
                     WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                     CW_USEDEFAULT,
                     CW_USEDEFAULT,
                     dimension_x + x_edge * 2,
                     dimension_y + y_edge * 2 + y_caption,
                     NULL,
                     NULL,
                     stInfo.hInstance,
                     NULL);

  if (stInfo.mainWnd) {
    ShowWindow (stInfo.mainWnd, SW_SHOW);

    InitCommonControls();
    hwndTB = CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR) NULL,
               WS_CHILD | CCS_ADJUSTABLE, 0, 0, 0, 0, stInfo.mainWnd,
               (HMENU) 100, stInfo.hInstance, NULL);
    SendMessage(hwndTB, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);
    SendMessage(hwndTB, TB_SETBITMAPSIZE, 0, (LPARAM)MAKELONG(32, 32));
    ShowWindow(hwndTB, SW_SHOW);
    hwndSB = CreateStatusWindow(WS_CHILD | WS_VISIBLE, " CTRL + 3rd button enables mouse",
                                stInfo.mainWnd, 0x7712);
    if (hwndSB) {
      int elements;
      SB_Edges[0] = SIZE_OF_SB_FIRST_ELEMENT + SIZE_OF_SB_ELEMENT;   // Mouse info
      for (elements = 1; elements < (BX_MAX_STATUSITEMS+1); elements++)
        SB_Edges[elements] = SB_Edges[elements-1] + SIZE_OF_SB_ELEMENT;
      SB_Edges[elements] = -1;
      SendMessage(hwndSB, SB_SETPARTS, BX_MAX_STATUSITEMS+2, (long)&SB_Edges);
    }
    GetClientRect(hwndTB, &wndRect2);
    bx_headerbar_y = wndRect2.bottom;
    GetClientRect(hwndSB, &wndRect2);
    bx_statusbar_y = wndRect2.bottom;
    SetWindowPos(stInfo.mainWnd, NULL, 0, 0, stretched_x + x_edge * 2,
                  stretched_y + bx_headerbar_y + bx_statusbar_y + y_edge * 2
                  + y_caption, SWP_NOMOVE | SWP_NOZORDER);
    UpdateWindow (stInfo.mainWnd);
    GetClientRect(stInfo.mainWnd, &wndRect);
    MoveWindow(hwndSB, 0, wndRect.bottom - bx_statusbar_y, wndRect.right,
               bx_statusbar_y, TRUE);

    stInfo.simWnd = CreateWindow("SIMWINDOW",
                      "",
                      WS_CHILD,
                      0,
                      bx_headerbar_y,
                      wndRect.right,
                      wndRect.bottom - bx_headerbar_y - bx_statusbar_y,
                      stInfo.mainWnd,
                      NULL,
                      stInfo.hInstance,
                      NULL);

    /* needed for the Japanese versions of Windows */
    if (stInfo.simWnd) {
      HMODULE hm;
      hm = GetModuleHandle("USER32");
      if (hm) {
        BOOL (WINAPI *enableime)(HWND, BOOL);
        enableime = (BOOL (WINAPI *)(HWND, BOOL))GetProcAddress(hm, "WINNLSEnableIME");
        if (enableime) {
          enableime(stInfo.simWnd, FALSE);
          BX_INFO(("IME disabled"));
        }
      }
    }

    ShowWindow(stInfo.simWnd, SW_SHOW);
    UpdateWindow (stInfo.simWnd);
    SetFocus(stInfo.simWnd);

    ShowCursor(!mouseCaptureMode);
    GetWindowRect(stInfo.simWnd, &wndRect);
    SetCursorPos(wndRect.left + stretched_x/2, wndRect.top + stretched_y/2);
    cursorWarped();

    hdc = GetDC(stInfo.simWnd);
    MemoryBitmap = CreateCompatibleBitmap(hdc, BX_MAX_XRES, BX_MAX_YRES);
    MemoryDC = CreateCompatibleDC(hdc);
    ReleaseDC(stInfo.simWnd, hdc);

    if (MemoryBitmap && MemoryDC) {
      stInfo.UIinited = TRUE;

      bx_gui->clear_screen();

      while (GetMessage (&msg, NULL, 0, 0)) {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
      }
    }
  }

  stInfo.kill = EXIT_GUI_SHUTDOWN;

  _endthread();
}

void SetStatusText(int Num, const char *Text, bx_bool active)
{
  char StatText[MAX_PATH];

  if ((Num < 1) || (Num > BX_MAX_STATUSITEMS)) {
    StatText[0] = ' ';  // Add space to text in first and last item
  } else {
    StatText[0] = 9;  // Center the rest
  }
  lstrcpy(StatText+1, Text);
  if ((Num < 1) || (Num > BX_MAX_STATUSITEMS)) {
    SendMessage(hwndSB, SB_SETTEXT, Num, (long)StatText);
  } else {
    lstrcpy(SB_Text[Num-1], StatText);
    SB_Active[Num-1] = active;
    SendMessage(hwndSB, SB_SETTEXT, Num | SBT_OWNERDRAW, (long)SB_Text[Num-1]);
  }
  UpdateWindow(hwndSB);
}

void 
bx_win32_gui_c::statusbar_setitem(int element, bx_bool active)
{
  if (element < 0) {
    for (int i = 0; i < statusitem_count; i++) {
      SetStatusText(i+1, statusitem_text[i], active);
    }
  } else if (element < statusitem_count) {
    SetStatusText(element+1, statusitem_text[element], active);
  }
}

LRESULT CALLBACK mainWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
  DRAWITEMSTRUCT *lpdis;
  char *sbtext;

  switch (iMsg) {
  case WM_CREATE:
    bx_options.Omouse_enabled->set (mouseCaptureMode);
    if (mouseCaptureMode)
      SetStatusText(0, "CTRL + 3rd button disables mouse", TRUE);
    else
      SetStatusText(0, "CTRL + 3rd button enables mouse", TRUE);
    return 0;

  case WM_COMMAND:
    if (LOWORD(wParam) >= 101) {
      EnterCriticalSection(&stInfo.keyCS);
      enq_key_event(LOWORD(wParam)-101, HEADERBAR_CLICKED);
      LeaveCriticalSection(&stInfo.keyCS);
    }
    break;

  case WM_SETFOCUS:
    SetFocus(stInfo.simWnd);
    return 0;

  case WM_CLOSE:
    SendMessage(stInfo.simWnd, WM_CLOSE, 0, 0);
    break;

  case WM_DESTROY:
    PostQuitMessage (0);
    return 0;

  case WM_SIZE:
    SendMessage(hwndTB, TB_AUTOSIZE, 0, 0);
    break;

  case WM_DRAWITEM:
    lpdis = (DRAWITEMSTRUCT *)lParam;
    if (lpdis->hwndItem == hwndSB) {
      sbtext = (char *)lpdis->itemData;
      if (SB_Active[lpdis->itemID-1]) {
        SetBkColor(lpdis->hDC, 0x0000FF00);
      } else {
        SetBkMode(lpdis->hDC, TRANSPARENT);
        SetTextColor(lpdis->hDC, 0x00808080);
      }
      DrawText(lpdis->hDC, sbtext+1, lstrlen(sbtext)-1, &lpdis->rcItem, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      return TRUE;
    }
    break;

  }
  return DefWindowProc (hwnd, iMsg, wParam, lParam);
}

void SetMouseCapture()
{
  RECT wndRect;

  bx_options.Omouse_enabled->set (mouseCaptureMode);
  ShowCursor(!mouseCaptureMode);
  ShowCursor(!mouseCaptureMode);   // somehow one didn't do the trick (win98)
  GetWindowRect(stInfo.simWnd, &wndRect);
  SetCursorPos(wndRect.left + stretched_x/2, wndRect.top + stretched_y/2);
  cursorWarped();
  if (mouseCaptureMode)
    SetStatusText(0, "CTRL + 3rd button disables mouse", TRUE);
  else
    SetStatusText(0, "CTRL + 3rd button enables mouse", TRUE);
}

LRESULT CALLBACK simWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
  HDC hdc, hdcMem;
  PAINTSTRUCT ps;
  RECT wndRect;
  static BOOL mouseModeChange = FALSE;

  switch (iMsg) {
  case WM_CREATE:
#if BX_USE_WINDOWS_FONTS
    InitFont();
#endif
    SetTimer (hwnd, 1, 330, NULL);
    return 0;

  case WM_TIMER:
    if (mouseToggleReq && (GetActiveWindow() == stInfo.mainWnd)) {
      mouseCaptureMode = mouseCaptureNew;
      SetMouseCapture();
      mouseToggleReq = FALSE;
    }
    // If mouse escaped, bring it back
    if ( mouseCaptureMode)
    {
      GetWindowRect(hwnd, &wndRect);
      SetCursorPos(wndRect.left + stretched_x/2, wndRect.top + stretched_y/2);
      cursorWarped();
    }
    bx_options.Omouse_enabled->set (mouseCaptureMode);

    return 0;

  case WM_PAINT:
    EnterCriticalSection(&stInfo.drawCS);
    hdc = BeginPaint (hwnd, &ps);

    hdcMem = CreateCompatibleDC (hdc);
    SelectObject (hdcMem, MemoryBitmap);

    if (stretch_factor == 1) {
      BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top,
             ps.rcPaint.right - ps.rcPaint.left + 1,
             ps.rcPaint.bottom - ps.rcPaint.top + 1, hdcMem,
             ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
    } else {
      StretchBlt(hdc, ps.rcPaint.left, ps.rcPaint.top,
                 ps.rcPaint.right - ps.rcPaint.left + 1,
                 ps.rcPaint.bottom - ps.rcPaint.top + 1, hdcMem,
                 ps.rcPaint.left/stretch_factor, ps.rcPaint.top,
                 (ps.rcPaint.right - ps.rcPaint.left+1)/stretch_factor,
                 (ps.rcPaint.bottom - ps.rcPaint.top+1), SRCCOPY);
    }
    DeleteDC (hdcMem);
    EndPaint (hwnd, &ps);
    LeaveCriticalSection(&stInfo.drawCS);
    return 0;

  case WM_MOUSEMOVE:
    if (!mouseModeChange) {
      processMouseXY( LOWORD(lParam), HIWORD(lParam), wParam, 0);
    }
    return 0;

  case WM_LBUTTONDOWN:
  case WM_LBUTTONDBLCLK:
  case WM_LBUTTONUP:
    processMouseXY( LOWORD(lParam), HIWORD(lParam), wParam, 1);
    return 0;

  case WM_MBUTTONDOWN:
  case WM_MBUTTONDBLCLK:
  case WM_MBUTTONUP:
    if (wParam == (MK_CONTROL | MK_MBUTTON)) {
      mouseCaptureMode = !mouseCaptureMode;
      SetMouseCapture();
      mouseModeChange = TRUE;
    } else if (mouseModeChange && (iMsg == WM_MBUTTONUP)) {
      mouseModeChange = FALSE;
    } else {
      processMouseXY( LOWORD(lParam), HIWORD(lParam), wParam, 4);
    }
    return 0;

  case WM_RBUTTONDOWN:
  case WM_RBUTTONDBLCLK:
  case WM_RBUTTONUP:
    processMouseXY( LOWORD(lParam), HIWORD(lParam), wParam, 2);
    return 0;

  case WM_CLOSE:
    return DefWindowProc (hwnd, iMsg, wParam, lParam);

  case WM_DESTROY:
    KillTimer (hwnd, 1);
    stInfo.UIinited = FALSE;
#if BX_USE_WINDOWS_FONTS
    DestroyFont();
#endif
    return 0;

  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
    if (legacyF12 && (wParam == VK_F12)) {
      mouseCaptureMode = !mouseCaptureMode;
      SetMouseCapture();
      return 0;
    }
    EnterCriticalSection(&stInfo.keyCS);
    enq_key_event(HIWORD (lParam) & 0x01FF, BX_KEY_PRESSED);
    LeaveCriticalSection(&stInfo.keyCS);
    return 0;

  case WM_KEYUP:
  case WM_SYSKEYUP:
    EnterCriticalSection(&stInfo.keyCS);
    enq_key_event(HIWORD (lParam) & 0x01FF, BX_KEY_RELEASED);
    LeaveCriticalSection(&stInfo.keyCS);
    return 0;

  case WM_CHAR:
  case WM_DEADCHAR:
  case WM_SYSCHAR:
  case WM_SYSDEADCHAR:
    return 0;
  }

  return DefWindowProc (hwnd, iMsg, wParam, lParam);
}


void enq_key_event(Bit32u key, Bit32u press_release)
{
  static BOOL alt_pressed = FALSE;
  static BOOL ctrl_pressed = FALSE;
  static BOOL shift_pressed = FALSE;

  if (press_release == BX_KEY_PRESSED) {
    switch (key) {
      case 0x1d:
        if (ctrl_pressed)
          return;
        else
          ctrl_pressed = TRUE;
        break;
      case 0x2a:
        if (shift_pressed)
          return;
        else
          shift_pressed = TRUE;
        break;
      case 0x38:
        if (alt_pressed)
          return;
        else
          alt_pressed = TRUE;
        break;
    }
  }
  if (press_release == BX_KEY_RELEASED) {
    switch (key) {
      case 0x1d:
        ctrl_pressed = FALSE;
        break;
      case 0x2a:
        shift_pressed = FALSE;
        break;
      case 0x38:
        alt_pressed = FALSE;
        break;
    }
  }
  if (((tail+1) % SCANCODE_BUFSIZE) == head) {
    BX_ERROR(( "enq_scancode: buffer full"));
    return;
  }
  keyevents[tail].key_event = key | press_release;
  tail = (tail + 1) % SCANCODE_BUFSIZE;
}

void enq_mouse_event(void)
{
  EnterCriticalSection( &stInfo.mouseCS);
  if ( ms_xdelta || ms_ydelta)
  {
    if (((tail+1) % SCANCODE_BUFSIZE) == head) {
      BX_ERROR(( "enq_scancode: buffer full" ));
      return;
    }
    QueueEvent& current=keyevents[tail];
    current.key_event=MOUSE_MOTION;
    current.mouse_x=ms_xdelta;
    current.mouse_y=ms_ydelta;
    current.mouse_button_state=mouse_button_state;
    resetDelta();
    tail = (tail + 1) % SCANCODE_BUFSIZE;
  }
  LeaveCriticalSection( &stInfo.mouseCS);
}

QueueEvent* deq_key_event(void) {
  QueueEvent* key;

  if ( head == tail ) {
    BX_ERROR(("deq_scancode: buffer empty"));
    return((QueueEvent*)0);
  }
  key = &keyevents[head];
  head = (head + 1) % SCANCODE_BUFSIZE;

  return(key);
}


// ::HANDLE_EVENTS()
//
// Called periodically (vga_update_interval in .bochsrc) so the
// the gui code can poll for keyboard, mouse, and other
// relevant events.

void bx_win32_gui_c::handle_events(void) {
  Bit32u key;
  Bit32u key_event;

  if (stInfo.kill) terminateEmul(stInfo.kill);

  // Handle mouse moves
  enq_mouse_event();

  // Handle keyboard and mouse clicks
  EnterCriticalSection(&stInfo.keyCS);
  while (head != tail) {
    QueueEvent* queue_event=deq_key_event();
    if ( ! queue_event)
      break;
    key = queue_event->key_event;
    if ( key==MOUSE_MOTION)
    {
      DEV_mouse_motion( queue_event->mouse_x,
        queue_event->mouse_y, queue_event->mouse_button_state);
    }
    // Check for mouse buttons first
    else if ( key & MOUSE_PRESSED) {
      DEV_mouse_motion( 0, 0, LOWORD(key));
    }
    else if (key & HEADERBAR_CLICKED) {
      headerbar_click(LOWORD(key));
    }
    else {
      if (key & 0x0100) {
        // It's an extended key
        switch (key & 0xff) {
          case 0x38:
            // This makes the "AltGr" key on European keyboards work
            DEV_kbd_gen_scancode(BX_KEY_CTRL_L | BX_KEY_RELEASED);
            break;
        }
      }
      key_event = win32_to_bx_key[(key & 0x100) ? 1 : 0][key & 0xff];
      if (key & BX_KEY_RELEASED) key_event |= BX_KEY_RELEASED;
      DEV_kbd_gen_scancode(key_event);
    }
  }
  LeaveCriticalSection(&stInfo.keyCS);
}


// ::FLUSH()
//
// Called periodically, requesting that the gui code flush all pending
// screen update requests.

void bx_win32_gui_c::flush(void) {
  EnterCriticalSection( &stInfo.drawCS);
  if (updated_area_valid) {
    // slight bugfix
	updated_area.right++;
	updated_area.bottom++;
	InvalidateRect( stInfo.simWnd, &updated_area, FALSE);
	updated_area_valid = FALSE;
  }
  LeaveCriticalSection( &stInfo.drawCS);
}


// ::CLEAR_SCREEN()
//
// Called to request that the VGA region is cleared.  Don't
// clear the area that defines the headerbar.

void bx_win32_gui_c::clear_screen(void) {
  HGDIOBJ oldObj;

  if (!stInfo.UIinited) return;

  EnterCriticalSection(&stInfo.drawCS);

  oldObj = SelectObject(MemoryDC, MemoryBitmap);
  PatBlt(MemoryDC, 0, 0, stretched_x, stretched_y, BLACKNESS);
  SelectObject(MemoryDC, oldObj);

  updateUpdated(0, 0, dimension_x-1, dimension_y-1);

  LeaveCriticalSection(&stInfo.drawCS);
}


// ::TEXT_UPDATE()
//
// Called in a VGA text mode, to update the screen with
// new content.
//
// old_text: array of character/attributes making up the contents
//           of the screen from the last call.  See below
// new_text: array of character/attributes making up the current
//           contents, which should now be displayed.  See below
//
// format of old_text & new_text: each is tm_info.line_offset*text_rows
//     bytes long. Each character consists of 2 bytes.  The first by is
//     the character value, the second is the attribute byte.
//
// cursor_x: new x location of cursor
// cursor_y: new y location of cursor
// tm_info:  this structure contains information for additional
//           features in text mode (cursor shape, line offset,...)
// nrows:    number of text rows (unused here)

void bx_win32_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
			   unsigned long cursor_x, unsigned long cursor_y,
                           bx_vga_tminfo_t tm_info, unsigned nrows)
{
  HDC hdc;
  unsigned char data[64];
  Bit8u *old_line, *new_line, *text_base;
  Bit8u cAttr, cChar;
  unsigned int curs, hchars, offset, rows, x, y, xc, yc, yc2, cs_y;
  Bit8u cfwidth, cfheight, cfheight2, font_col, font_row, font_row2;
  Bit8u split_textrow, split_fontrows;
  BOOL forceUpdate = FALSE, split_screen;

  if (!stInfo.UIinited) return;

  EnterCriticalSection(&stInfo.drawCS);

  UNUSED(nrows);
  if (charmap_updated) {
    for (unsigned c = 0; c<256; c++) {
      if (char_changed[c]) {
        memset(data, 0, sizeof(data));
        BOOL gfxchar = tm_info.line_graphics && ((c & 0xE0) == 0xC0);
        for (unsigned i=0; i<yChar; i++) {
          data[i*2] = vga_charmap[c*32+i];
          if (gfxchar) {
            data[i*2+1] = (data[i*2] << 7);
          }
        }
        SetBitmapBits(vgafont[c], 64, data);
        char_changed[c] = 0;
      }
    }
    forceUpdate = TRUE;
    charmap_updated = 0;
  }

  hdc = GetDC(stInfo.simWnd);

#if !BX_USE_WINDOWS_FONTS
  if((tm_info.h_panning != h_panning) || (tm_info.v_panning != v_panning)) {
    forceUpdate = 1;
    h_panning = tm_info.h_panning;
    v_panning = tm_info.v_panning;
  }
  if(tm_info.line_compare != line_compare) {
    forceUpdate = 1;
    line_compare = tm_info.line_compare;
  }
#endif

  // first invalidate character at previous and new cursor location
  if((prev_cursor_y < text_rows) && (prev_cursor_x < text_cols)) {
    curs = prev_cursor_y * tm_info.line_offset + prev_cursor_x * 2;
    old_text[curs] = ~new_text[curs];
  }
  if((tm_info.cs_start <= tm_info.cs_end) && (tm_info.cs_start < yChar) &&
     (cursor_y < text_rows) && (cursor_x < text_cols)) {
    curs = cursor_y * tm_info.line_offset + cursor_x * 2;
    old_text[curs] = ~new_text[curs];
  } else {
    curs = 0xffff;
  }

#if !BX_USE_WINDOWS_FONTS
  rows = text_rows;
  if (v_panning) rows++;
  y = 0;
  cs_y = 0;
  text_base = new_text - tm_info.start_address;
  split_textrow = (line_compare + v_panning) / yChar;
  split_fontrows = ((line_compare + v_panning) % yChar) + 1;
  split_screen = 0;
  do {
    hchars = text_cols;
    if (h_panning) hchars++;
    if (split_screen) {
      yc = line_compare + cs_y * yChar + 1;
      font_row = 0;
      if (rows == 1) {
        cfheight = (dimension_y - line_compare - 1) % yChar;
        if (cfheight == 0) cfheight = yChar;
      } else {
        cfheight = yChar;
      }
    } else if (v_panning) {
      if (y == 0) {
        yc = 0;
        font_row = v_panning;
        cfheight = yChar - v_panning;
      } else {
        yc = y * yChar - v_panning;
        font_row = 0;
        if (rows == 1) {
          cfheight = v_panning;
        } else {
          cfheight = yChar;
        }
      }
    } else {
      yc = y * yChar;
      font_row = 0;
      cfheight = yChar;
    }
    if (!split_screen && (y == split_textrow)) {
      if (split_fontrows < cfheight) cfheight = split_fontrows;
    }
    new_line = new_text;
    old_line = old_text;
    x = 0;
    offset = cs_y * tm_info.line_offset;
    do {
      if (h_panning) {
        if (hchars > text_cols) {
          xc = 0;
          font_col = h_panning;
          cfwidth = xChar - h_panning;
        } else {
          xc = x * xChar - h_panning;
          font_col = 0;
          if (hchars == 1) {
            cfwidth = h_panning;
          } else {
            cfwidth = xChar;
          }
        }
      } else {
        xc = x * xChar;
        font_col = 0;
        cfwidth = xChar;
      }
      if (forceUpdate || (old_text[0] != new_text[0])
          || (old_text[1] != new_text[1])) {
        cChar = new_text[0];
        cAttr = new_text[1];
        DrawBitmap(hdc, vgafont[cChar], xc, yc, cfwidth, cfheight, font_col,
                   font_row, SRCCOPY, cAttr);
        if (offset == curs) {
          if (font_row == 0) {
            yc2 = yc + tm_info.cs_start;
            font_row2 = tm_info.cs_start;
            cfheight2 = tm_info.cs_end - tm_info.cs_start + 1;
          } else {
            if (v_panning > tm_info.cs_start) {
              yc2 = yc;
              font_row2 = font_row;
              cfheight2 = tm_info.cs_end - v_panning + 1;
            } else {
              yc2 = yc + tm_info.cs_start - v_panning;
              font_row2 = tm_info.cs_start;
              cfheight2 = tm_info.cs_end - tm_info.cs_start + 1;
            }
          }
          cAttr = ((cAttr >> 4) & 0xF) + ((cAttr & 0xF) << 4);
          DrawBitmap(hdc, vgafont[cChar], xc, yc2, cfwidth, cfheight2, font_col,
                     font_row2, SRCCOPY, cAttr);
        }
      }
      x++;
      new_text+=2;
      old_text+=2;
      offset+=2;
    } while (--hchars);
    if (!split_screen && (y == split_textrow)) {
      new_text = text_base;
      forceUpdate = 1;
      cs_y = 0;
      if (tm_info.split_hpanning) h_panning = 0;
      rows = ((dimension_y - line_compare + yChar - 2) / yChar) + 1;
      split_screen = 1;
    } else {
      y++;
      cs_y++;
      new_text = new_line + tm_info.line_offset;
      old_text = old_line + tm_info.line_offset;
    }
  } while (--rows);

  h_panning = tm_info.h_panning;
#else
  rows = text_rows;
  y = 0;
  do {
    hchars = text_cols;
    yc = y * yChar;
    new_line = new_text;
    old_line = old_text;
    x = 0;
    offset = y * tm_info.line_offset;
    do {
      xc = x * xChar;
      if (forceUpdate || (old_text[0] != new_text[0])
          || (old_text[1] != new_text[1])) {
        cChar = new_text[0];
        cAttr = new_text[1];
        DrawChar(hdc, cChar, xc, yc, cAttr, 1, 0);
        if (offset == curs) {
          DrawChar(hdc, cChar, xc, yc, cAttr, tm_info.cs_start, tm_info.cs_end);
        }
      }
      x++;
      new_text+=2;
      old_text+=2;
      offset+=2;
    } while (--hchars);
    y++;
    new_text = new_line + tm_info.line_offset;
    old_text = old_line + tm_info.line_offset;
  } while (--rows);
#endif

  prev_cursor_x = cursor_x;
  prev_cursor_y = cursor_y;

  ReleaseDC(stInfo.simWnd, hdc);

  LeaveCriticalSection(&stInfo.drawCS);
}

  int
bx_win32_gui_c::get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)
{
  if (OpenClipboard(stInfo.simWnd)) {
    HGLOBAL hg = GetClipboardData(CF_TEXT);
    char *data = (char *)GlobalLock(hg);
    *nbytes = strlen(data);
    *bytes = new Bit8u[1 + *nbytes];
    BX_INFO (("found %d bytes on the clipboard", *nbytes));
    memcpy (*bytes, data, *nbytes+1);
    BX_INFO (("first byte is 0x%02x", *bytes[0]));
    GlobalUnlock(hg);
    CloseClipboard();
    return 1;
    // *bytes will be freed in bx_keyb_c::paste_bytes or
    // bx_keyb_c::service_paste_buf, using delete [].
  } else {
    BX_ERROR (("paste: could not open clipboard"));
    return 0;
  }
}

  int
bx_win32_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
{
  if (OpenClipboard(stInfo.simWnd)) {
    HANDLE hMem = GlobalAlloc(GMEM_ZEROINIT, len);
    EmptyClipboard();
    lstrcpy((char *)hMem, text_snapshot);
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
    GlobalFree(hMem);
    return 1;
  } else {
    BX_ERROR (("copy: could not open clipboard"));
    return 0;
  }
}


// ::PALETTE_CHANGE()
//
// Allocate a color in the native GUI, for this color, and put
// it in the colormap location 'index'.
// returns: 0=no screen update needed (color map change has direct effect)
//          1=screen updated needed (redraw using current colormap)

bx_bool bx_win32_gui_c::palette_change(unsigned index, unsigned red,
                                 unsigned green, unsigned blue) {
  cmap_index[index].rgbRed = red;
  cmap_index[index].rgbBlue = blue;
  cmap_index[index].rgbGreen = green;
  return(1);
}


// ::GRAPHICS_TILE_UPDATE()
//
// Called to request that a tile of graphics be drawn to the
// screen, since info in this region has changed.
//
// tile: array of 8bit values representing a block of pixels with
//       dimension equal to the 'tilewidth' & 'tileheight' parameters to
//       ::specific_init().  Each value specifies an index into the
//       array of colors you allocated for ::palette_change()
// x0: x origin of tile
// y0: y origin of tile
//
// note: origin of tile and of window based on (0,0) being in the upper
//       left of the window.

void bx_win32_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0) {
  HDC hdc;
  HGDIOBJ oldObj;

  EnterCriticalSection(&stInfo.drawCS);
  hdc = GetDC(stInfo.simWnd);

  oldObj = SelectObject(MemoryDC, MemoryBitmap);

  StretchDIBits( MemoryDC, x0, y0, x_tilesize, y_tilesize, 0, 0,
    x_tilesize, y_tilesize, tile, bitmap_info, DIB_RGB_COLORS, SRCCOPY);

  SelectObject(MemoryDC, oldObj);

  updateUpdated(x0, y0, x0 + x_tilesize - 1, y0 + y_tilesize - 1);

  ReleaseDC(stInfo.simWnd, hdc);
  LeaveCriticalSection(&stInfo.drawCS);
}



// ::DIMENSION_UPDATE()
//
// Called when the VGA mode changes it's X,Y dimensions.
// Resize the window to this size, but you need to add on
// the height of the headerbar to the Y value.
//
// x: new VGA x size
// y: new VGA y size (add headerbar_y parameter from ::specific_init().
// fheight: new VGA character height in text mode
// fwidth : new VGA character width in text mode
// bpp : bits per pixel in graphics mode

void bx_win32_gui_c::dimension_update(unsigned x, unsigned y, unsigned fheight, unsigned fwidth, unsigned bpp)
{
  RECT R;

  BxTextMode = (fheight > 0);
  if (BxTextMode) {
    text_cols = x / fwidth;
    text_rows = y / fheight;
#if BX_USE_WINDOWS_FONTS
    if (fheight >= 14) {
      FontId = 2;
      xChar = 8;
      yChar = 16;
    } else if (fheight >= 12) {
      FontId = 1;
      xChar = 8;
      yChar = 14;
    } else {
      FontId = 0;
      xChar = 8;
      yChar = 12;
    }
    if (fwidth != xChar) {
      x = x * 8 / fwidth;
    }
    if (fheight != yChar) {
      y = y * yChar / fheight;
    }
#else
    xChar = fwidth;
    yChar = fheight;
#endif
  }

  if ( x==dimension_x && y==dimension_y && bpp==current_bpp)
    return;
  dimension_x = x;
  dimension_y = y;
  current_bpp = bpp;
  stretched_x = dimension_x;
  stretched_y = dimension_y;
  stretch_factor = 1;
  if (BxTextMode && (stretched_x<400)) {
    stretched_x *= 2;
    stretch_factor *= 2;
  }
  
  bitmap_info->bmiHeader.biBitCount = bpp;
  if (bpp == 16)
  {
    bitmap_info->bmiHeader.biCompression = BI_BITFIELDS;
    static RGBQUAD red_mask   = {0x00, 0xF8, 0x00, 0x00};
    static RGBQUAD green_mask = {0xE0, 0x07, 0x00, 0x00};
    static RGBQUAD blue_mask  = {0x1F, 0x00, 0x00, 0x00};
    bitmap_info->bmiColors[0] = red_mask;
    bitmap_info->bmiColors[1] = green_mask;
    bitmap_info->bmiColors[2] = blue_mask;
  }
  else
  {
    bitmap_info->bmiHeader.biCompression = BI_RGB;
    if (bpp == 15)
    {
      bitmap_info->bmiHeader.biBitCount = 16;
    }
  }

  SetWindowPos(stInfo.mainWnd, HWND_TOP, 0, 0, stretched_x + x_edge * 2,
               stretched_y + bx_headerbar_y + bx_statusbar_y + y_edge * 2
               + y_caption, SWP_NOMOVE | SWP_NOZORDER);
  GetClientRect(stInfo.mainWnd, &R);
  MoveWindow(hwndSB, 0, R.bottom-bx_statusbar_y, stretched_x,
             bx_statusbar_y, TRUE);
  UpdateWindow(hwndSB);
  MoveWindow(stInfo.simWnd, 0, bx_headerbar_y, stretched_x, stretched_y, TRUE);

  BX_INFO (("dimension update x=%d y=%d fontheight=%d fontwidth=%d bpp=%d", x, y, fheight, fwidth, bpp));

  host_xres = x;
  host_yres = y;
  host_bpp = bpp;
}


// ::CREATE_BITMAP()
//
// Create a monochrome bitmap of size 'xdim' by 'ydim', which will
// be drawn in the headerbar.  Return an integer ID to the bitmap,
// with which the bitmap can be referenced later.
//
// bmap: packed 8 pixels-per-byte bitmap.  The pixel order is:
//       bit0 is the left most pixel, bit7 is the right most pixel.
// xdim: x dimension of bitmap
// ydim: y dimension of bitmap

unsigned bx_win32_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim,
				 unsigned ydim) {
  unsigned char *data;
  TBADDBITMAP tbab;

  if (bx_bitmap_entries >= BX_MAX_PIXMAPS)
    terminateEmul(EXIT_HEADER_BITMAP_ERROR);

  bx_bitmaps[bx_bitmap_entries].bmap = CreateBitmap(xdim,ydim,1,1,NULL);
  if (!bx_bitmaps[bx_bitmap_entries].bmap)
    terminateEmul(EXIT_HEADER_BITMAP_ERROR);

  data = new unsigned char[ydim * xdim/8];
  for (unsigned i=0; i<ydim * xdim/8; i++)
    data[i] = 255 - reverse_bitorder(bmap[i]);
  SetBitmapBits(bx_bitmaps[bx_bitmap_entries].bmap, ydim * xdim/8, data);
  delete [] data;
  data = NULL;

  bx_bitmaps[bx_bitmap_entries].xdim = xdim;
  bx_bitmaps[bx_bitmap_entries].ydim = ydim;

  tbab.hInst = NULL;
  tbab.nID = (UINT)bx_bitmaps[bx_bitmap_entries].bmap;
  SendMessage(hwndTB, TB_ADDBITMAP, 1, (LPARAM)&tbab);

  bx_bitmap_entries++;
  return(bx_bitmap_entries-1); // return index as handle
}


// ::HEADERBAR_BITMAP()
//
// Called to install a bitmap in the bochs headerbar (toolbar).
//
// bmap_id: will correspond to an ID returned from
//     ::create_bitmap().  'alignment' is either BX_GRAVITY_LEFT
//     or BX_GRAVITY_RIGHT, meaning install the bitmap in the next
//     available leftmost or rightmost space.
// alignment: is either BX_GRAVITY_LEFT or BX_GRAVITY_RIGHT,
//     meaning install the bitmap in the next
//     available leftmost or rightmost space.
// f: a 'C' function pointer to callback when the mouse is clicked in
//     the boundaries of this bitmap.

unsigned bx_win32_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment,
				    void (*f)(void)) {
  unsigned hb_index;
  TBBUTTON tbb[1];

  if ( (bx_headerbar_entries+1) > BX_MAX_HEADERBAR_ENTRIES )
    terminateEmul(EXIT_HEADER_BITMAP_ERROR);

  bx_headerbar_entries++;
  hb_index = bx_headerbar_entries - 1;

  memset(tbb,0,sizeof(tbb));
  if (bx_hb_separator==0) {
    tbb[0].iBitmap = 0;
    tbb[0].idCommand = 0;
    tbb[0].fsState = 0;
    tbb[0].fsStyle = TBSTYLE_SEP;
    SendMessage(hwndTB, TB_ADDBUTTONS, 1,(LPARAM)(LPTBBUTTON)&tbb);
  }
  tbb[0].iBitmap = bmap_id;
  tbb[0].idCommand = hb_index + 101;
  tbb[0].fsState = TBSTATE_ENABLED;
  tbb[0].fsStyle = TBSTYLE_BUTTON;
  if (alignment == BX_GRAVITY_LEFT) {
    SendMessage(hwndTB, TB_INSERTBUTTON, bx_hb_separator,(LPARAM)(LPTBBUTTON)&tbb);
    bx_hb_separator++;
  } else { // BX_GRAVITY_RIGHT
    SendMessage(hwndTB, TB_INSERTBUTTON, bx_hb_separator+1, (LPARAM)(LPTBBUTTON)&tbb);
  }

  bx_headerbar_entry[hb_index].bmap_id = bmap_id;
  bx_headerbar_entry[hb_index].f = f;

  return(hb_index);
}


// ::SHOW_HEADERBAR()
//
// Show (redraw) the current headerbar, which is composed of
// currently installed bitmaps.

void bx_win32_gui_c::show_headerbar(void)
{
  RECT R;

  SendMessage(hwndTB, TB_AUTOSIZE, 0, 0);
  GetWindowRect(hwndTB, &R);
  if (bx_headerbar_y != (R.bottom - R.top + 1)) {
    bx_headerbar_y = R.bottom - R.top + 1;
    SetWindowPos(stInfo.mainWnd, HWND_TOP, 0, 0, stretched_x + x_edge * 2,
                 stretched_y + bx_headerbar_y + bx_statusbar_y + y_edge * 2
                 + y_caption, SWP_NOMOVE | SWP_NOZORDER);
    GetClientRect(stInfo.mainWnd, &R);
    MoveWindow(hwndSB, 0, R.bottom - bx_statusbar_y, stretched_x,
               bx_statusbar_y, TRUE);
    MoveWindow(stInfo.simWnd, 0, bx_headerbar_y, stretched_x, stretched_y, TRUE);
  }
}


// ::REPLACE_BITMAP()
//
// Replace the bitmap installed in the headerbar ID slot 'hbar_id',
// with the one specified by 'bmap_id'.  'bmap_id' will have
// been generated by ::create_bitmap().  The old and new bitmap
// must be of the same size.  This allows the bitmap the user
// sees to change, when some action occurs.  For example when
// the user presses on the floppy icon, it then displays
// the ejected status.
//
// hbar_id: headerbar slot ID
// bmap_id: bitmap ID

void bx_win32_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
  if (bmap_id != bx_headerbar_entry[hbar_id].bmap_id) {
    bx_headerbar_entry[hbar_id].bmap_id = bmap_id;
    SendMessage(hwndTB, TB_CHANGEBITMAP, (WPARAM)hbar_id+101, (LPARAM)
                MAKELPARAM(bmap_id, 0));
  }
}


// ::EXIT()
//
// Called before bochs terminates, to allow for a graceful
// exit from the native GUI mechanism.
void bx_win32_gui_c::exit(void) {
  printf("# In bx_win32_gui_c::exit(void)!\n");

  // kill thread first...
  PostMessage(stInfo.mainWnd, WM_CLOSE, 0, 0);

  // Wait until it dies
  while ((stInfo.kill == 0) && (workerThreadID != 0)) Sleep(500);

  if (!stInfo.kill) terminateEmul(EXIT_NORMAL);
}


void create_vga_font(void) {
  unsigned char data[64];

  // VGA font is 8 or 9 wide and up to 32 high
  for (unsigned c = 0; c<256; c++) {
    vgafont[c] = CreateBitmap(9,32,1,1,NULL);
    if (!vgafont[c]) terminateEmul(EXIT_FONT_BITMAP_ERROR);
    memset(data, 0, sizeof(data));
    for (unsigned i=0; i<16; i++)
      data[i*2] = reverse_bitorder(bx_vgafont[c].data[i]);
    SetBitmapBits(vgafont[c], 64, data);
  }
}


unsigned char reverse_bitorder(unsigned char b) {
  unsigned char ret=0;

  for (unsigned i=0; i<8; i++) {
    ret |= (b & 0x01) << (7-i);
    b >>= 1;
  }

  return(ret);
}


COLORREF GetColorRef(unsigned char attr)
{
  return RGB(cmap_index[attr].rgbRed, cmap_index[attr].rgbGreen,
             cmap_index[attr].rgbBlue);
}


void DrawBitmap (HDC hdc, HBITMAP hBitmap, int xStart, int yStart, int width,
                 int height, int fcol, int frow, DWORD dwRop, unsigned char cColor) {
  BITMAP bm;
  HDC hdcMem;
  POINT ptSize, ptOrg;
  HGDIOBJ oldObj;

  hdcMem = CreateCompatibleDC (hdc);
  SelectObject (hdcMem, hBitmap);
  SetMapMode (hdcMem, GetMapMode (hdc));

  GetObject (hBitmap, sizeof (BITMAP), (LPVOID) &bm);

  ptSize.x = width;
  ptSize.y = height;

  DPtoLP (hdc, &ptSize, 1);

  ptOrg.x = fcol;
  ptOrg.y = frow;
  DPtoLP (hdcMem, &ptOrg, 1);

  oldObj = SelectObject(MemoryDC, MemoryBitmap);

//Colors taken from Ralf Browns interrupt list.
//(0=black, 1=blue, 2=red, 3=purple, 4=green, 5=cyan, 6=yellow, 7=white)
//The highest background bit usually means blinking characters. No idea
//how to implement that so for now it's just implemented as color.
//Note: it is also possible to program the VGA controller to have the
//high bit for the foreground color enable blinking characters.

	COLORREF crFore = SetTextColor(MemoryDC, GetColorRef(DEV_vga_get_actl_pal_idx((cColor>>4)&0xf)));
	COLORREF crBack = SetBkColor(MemoryDC, GetColorRef(DEV_vga_get_actl_pal_idx(cColor&0xf)));
	BitBlt (MemoryDC, xStart, yStart, ptSize.x, ptSize.y, hdcMem, ptOrg.x,
		  ptOrg.y, dwRop);
	SetBkColor(MemoryDC, crBack);
	SetTextColor(MemoryDC, crFore);

  SelectObject(MemoryDC, oldObj);

  updateUpdated(xStart, yStart, ptSize.x + xStart - 1, ptSize.y + yStart - 1);

  DeleteDC (hdcMem);
}


void updateUpdated(int x1, int y1, int x2, int y2) {
  x1*=stretch_factor;
  x2*=stretch_factor;
  if (!updated_area_valid) {
    updated_area.left = x1 ;
    updated_area.top = y1 ;
    updated_area.right = x2 ;
    updated_area.bottom = y2 ;
  } else {
    if (x1 < updated_area.left) updated_area.left = x1 ;
    if (y1 < updated_area.top) updated_area.top = y1 ;
    if (x2 > updated_area.right) updated_area.right = x2 ;
    if (y2 > updated_area.bottom) updated_area.bottom = y2;
  }

  updated_area_valid = TRUE;
}


void headerbar_click(int x)
{
  if (x < bx_headerbar_entries) {
    bx_headerbar_entry[x].f();
  }
}

#ifdef __MINGW32__
#if BX_SHOW_IPS
VOID CALLBACK MyTimer(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime)
{
  bx_signal_handler(SIGALRM);
}

void alarm (int time)
{
  UINT idTimer;
  SetTimer(stInfo.simWnd,idTimer,time*1000,MyTimer);
}
#endif
#endif

  void
bx_win32_gui_c::mouse_enabled_changed_specific (bx_bool val)
{
  if ((val != mouseCaptureMode) && !mouseToggleReq) {
    mouseToggleReq = TRUE;
    mouseCaptureNew = val;
  }
}

#if BX_USE_WINDOWS_FONTS

void DrawChar (HDC hdc, unsigned char c, int xStart, int yStart,
               unsigned char cColor, int cs_start, int cs_end) {
  HDC hdcMem;
  POINT ptSize, ptOrg;
  HGDIOBJ oldObj;
  char str[2];
  HFONT hFontOld;

  hdcMem = CreateCompatibleDC (hdc);
  SetMapMode (hdcMem, GetMapMode (hdc));
  ptSize.x = xChar;
  ptSize.y = yChar;

  DPtoLP (hdc, &ptSize, 1);

  ptOrg.x = 0;
  ptOrg.y = 0;

  DPtoLP (hdcMem, &ptOrg, 1);

  oldObj = SelectObject(MemoryDC, MemoryBitmap);
  hFontOld=(HFONT)SelectObject(MemoryDC, hFont[FontId]);

//Colors taken from Ralf Browns interrupt list.
//(0=black, 1=blue, 2=red, 3=purple, 4=green, 5=cyan, 6=yellow, 7=white)
//The highest background bit usually means blinking characters. No idea
//how to implement that so for now it's just implemented as color.
//Note: it is also possible to program the VGA controller to have the
//high bit for the foreground color enable blinking characters.

  COLORREF crFore = SetTextColor(MemoryDC, GetColorRef(DEV_vga_get_actl_pal_idx(cColor&0xf)));
  COLORREF crBack = SetBkColor(MemoryDC, GetColorRef(DEV_vga_get_actl_pal_idx((cColor>>4)&0xf)));
  str[0]=c;
  str[1]=0;

  int y = FontId == 2 ? 16 : 8;

  TextOut(MemoryDC, xStart, yStart, str, 1);
  if (cs_start <= cs_end && cs_start < y)
  {
    RECT rc;
    SetBkColor(MemoryDC, GetColorRef(cColor&0xf));
    SetTextColor(MemoryDC, GetColorRef((cColor>>4)&0xf));
    rc.left = xStart+0;
    rc.right = xStart+xChar;
    if (cs_end >= y)
      cs_end = y-1;
    rc.top = yStart+cs_start*yChar/y;
    rc.bottom = yStart+(cs_end+1)*yChar/y;
    ExtTextOut(MemoryDC, xStart, yStart, ETO_CLIPPED|ETO_OPAQUE, &rc, str, 1, NULL);
  }

  SetBkColor(MemoryDC, crBack);
  SetTextColor(MemoryDC, crFore);

  SelectObject(MemoryDC, hFontOld);
  SelectObject(MemoryDC, oldObj);

  updateUpdated(xStart, yStart, ptSize.x + xStart - 1, ptSize.y + yStart - 1);

  DeleteDC (hdcMem);
}

void InitFont(void)
{
  LOGFONT lf;
  int i;

  lf.lfWidth = 8;
  lf.lfEscapement = 0;
  lf.lfOrientation = 0;
  lf.lfWeight = FW_MEDIUM;
  lf.lfItalic = FALSE;
  lf.lfUnderline=FALSE;
  lf.lfStrikeOut=FALSE;
  lf.lfCharSet=OEM_CHARSET;
  lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
  lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
  lf.lfQuality=DEFAULT_QUALITY;
  lf.lfPitchAndFamily=FIXED_PITCH | FF_DONTCARE;
  wsprintf(lf.lfFaceName, "Lucida Console");

  for (i=0; i < 3; i++)
  {
    lf.lfHeight = 12 + i * 2;
    hFont[i]=CreateFontIndirect(&lf);
  }
}

void DestroyFont(void)
{
  int i;
  for(i = 0; i < 3; i++)
  {
    DeleteObject(hFont[i]);
  }
}

#endif /* if BX_USE_WINDOWS_FONTS */

#endif /* if BX_WITH_WIN32 */
