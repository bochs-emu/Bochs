/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2024  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
/////////////////////////////////////////////////////////////////////////

//  Much of this file was written by:
//  David Ross
//  dross@pobox.com

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#include "param_names.h"
#include "keymap.h"
#include "iodev/iodev.h"
#if BX_WITH_WIN32

#include "zmouse.h"
#include "win32dialog.h"
#include "win32res.h"
#include "font/vga.bitmap.h"
// windows.h is included by bochs.h
#include <commctrl.h>
#include <process.h>

#define COMMAND_MODE_VKEY VK_F7

class bx_win32_gui_c : public bx_gui_c {
public:
  bx_win32_gui_c(void);
  DECLARE_GUI_VIRTUAL_METHODS();
  virtual void set_font(bool lg);
  virtual void draw_char(Bit8u ch, Bit8u fc, Bit8u bc, Bit16u xc, Bit16u yc,
                         Bit8u fw, Bit8u fh, Bit8u fx, Bit8u fy,
                         bool gfxcharw9, Bit8u cs, Bit8u ce, bool curs, bool font2);
  virtual void statusbar_setitem_specific(int element, bool active, bool w);
  virtual void get_capabilities(Bit16u *xres, Bit16u *yres, Bit16u *bpp);
  virtual void set_tooltip(unsigned hbar_id, const char *tip);
  virtual void set_mouse_mode_absxy(bool mode);
#if BX_SHOW_IPS
  virtual void show_ips(Bit32u ips_count);
#endif
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

#ifndef TBSTYLE_FLAT
#define TBSTYLE_FLAT 0x0800
#endif

// Keyboard/mouse stuff
#define SCANCODE_BUFSIZE    20
#define MOUSE_PRESSED       0x20000000
#define TOOLBAR_CLICKED     0x08000000
#define MOUSE_MOTION        0x22000000
#define FOCUS_CHANGED       0x44000000
#define BX_SYSKEY           (KF_UP|KF_REPEAT|KF_ALTDOWN)
void enq_key_event(Bit32u, Bit32u);
void enq_mouse_event(void);

struct QueueEvent {
  Bit32u key_event;
  int mouse_x;
  int mouse_y;
  int mouse_z;
  int mouse_button_state;
};
QueueEvent* deq_key_event(void);

static QueueEvent keyevents[SCANCODE_BUFSIZE];
static unsigned head = 0, tail = 0;
static int mouse_button_state = 0;
static int ms_xdelta = 0, ms_ydelta = 0, ms_zdelta = 0;
static int ms_lastx = 0, ms_lasty = 0;
static int ms_savedx = 0, ms_savedy = 0;
static BOOL mouseCaptureMode, mouseCaptureNew, mouseToggleReq;
static BOOL win32MouseModeAbsXY = 0;
static HANDLE workerThread = 0;
static DWORD workerThreadID = 0;
static int mouse_buttons = 3;
static bool win32_autoscale = 0;
static bool win32_nokeyrepeat = 0;
static bool win32_traphotkeys = 0;
HHOOK hKeyboardHook;

// Graphics screen stuff
static unsigned x_tilesize = 0;
static unsigned win32_max_xres = 0, win32_max_yres = 0;
static BITMAPINFO* bitmap_info=(BITMAPINFO*)0;
static RGBQUAD* cmap_index;  // indeces into system colormap
static HBITMAP MemoryBitmap = NULL;
static HDC MemoryDC = NULL;
static RECT updated_area;
static BOOL updated_area_valid = FALSE;
static HWND desktopWindow;
static RECT desktop;
static BOOL queryFullScreen = FALSE;
static unsigned desktop_x, desktop_y;
static unsigned max_client_x, max_client_y;
static BOOL toolbarVisible, statusVisible;
static BOOL fullscreenMode, inFullscreenToggle;

// Text mode screen stuff
static HBITMAP vgafont[2][256];
static int xChar = 8, yChar = 16;

// Headerbar stuff
HWND hwndTB, hwndSB;
static unsigned bx_bitmap_entries;
static struct {
  HBITMAP bmap;
  unsigned xdim;
  unsigned ydim;
} bx_bitmaps[BX_MAX_PIXMAPS];

static struct {
  unsigned bmap_id;
  void (*f)(void);
  const char *tooltip;
} win32_toolbar_entry[BX_MAX_HEADERBAR_ENTRIES];

static int win32_toolbar_entries;
static unsigned bx_hb_separator;

// Status Bar stuff
#if BX_SHOW_IPS
static BOOL ipsUpdate = FALSE;
static BOOL  hideIPS = FALSE;
static char ipsText[20];
#endif
#define BX_SB_MAX_TEXT_ELEMENTS    2
#define SIZE_OF_SB_ELEMENT        60
#define SIZE_OF_SB_MOUSE_MESSAGE 170
#define SIZE_OF_SB_IPS_MESSAGE    90
Bit32u SB_Led_Colors[3] = {0x0000FF00, 0x000040FF, 0x0000FFFF};
Bit32s SB_Edges[BX_MAX_STATUSITEMS+BX_SB_MAX_TEXT_ELEMENTS+1];
char SB_Text[BX_MAX_STATUSITEMS][10];
unsigned SB_Text_Elements;
bool SB_Active[BX_MAX_STATUSITEMS];
Bit8u SB_ActiveColor[BX_MAX_STATUSITEMS];

// Misc stuff
static unsigned dimension_x, dimension_y, current_bpp;
static unsigned stretched_x, stretched_y;
static unsigned stretch_factor;
static BOOL fix_size = FALSE;
#if BX_DEBUGGER && BX_DEBUGGER_GUI
static BOOL gui_debug = FALSE;
static BOOL win32_enh_dbg_global_ini = FALSE;
#endif
static HWND hotKeyReceiver = NULL;
static HWND saveParent = NULL;

static char szMouseEnable[40];
static char szMouseDisable[40];
static char szMouseTooltip[64];

static const char szAppName[] = "Bochs for Windows";
static const char szWindowName[] = "Bochs for Windows - Display";

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

LRESULT CALLBACK mainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK simWndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI UIThread(PVOID);
void SetStatusText(unsigned Num, const char *Text, bool active, Bit8u color=0);
void terminateEmul(int);
void create_vga_font(void);
void DrawBitmap(HDC, HBITMAP, int, int, int, int, int, int, Bit8u, Bit8u);
void updateUpdated(int,int,int,int);
static void win32_toolbar_click(int x);

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
    0,                  /* Todo: "Katakana" key (ibm 133) for Japanese 106 keyboard */
    0,
    0,
    0,                  /* Todo: "Ro" key (ibm 56) for Japanese 106 keyboard */
    0,
    0,
    0,
    0,
    0,
    0,                  /* Todo: "convert" key (ibm 132) for Japanese 106 keyboard */
    0,
    0,                  /* Todo: "non-convert" key (ibm 131) for Japanese 106 keyboard */
    0,
    0,                  /* Todo: "Yen" key (ibm 14) for Japanese 106 keyboard */
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
    BX_KEY_PRINT,
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
    BX_KEY_CTRL_BREAK,
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

void gen_key_event(Bit32u key, Bit32u press_release)
{
  EnterCriticalSection(&stInfo.keyCS);
  enq_key_event(key, press_release);
  LeaveCriticalSection(&stInfo.keyCS);
}

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam )
{
  if (nCode < 0 || nCode != HC_ACTION || (!mouseCaptureMode && !fullscreenMode))
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);

  KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
  Bit32u press_release = (p->flags & LLKHF_UP) ? BX_KEY_RELEASED : BX_KEY_PRESSED;
  BOOL bAltKeyDown = (p->flags & LLKHF_ALTDOWN);
  BOOL bControlKeyDown = GetAsyncKeyState(VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1); //checks ctrl key pressed

  if (p->vkCode == VK_TAB && bAltKeyDown) {
    gen_key_event(p->scanCode, press_release);
    return 1; //disable alt-tab
  }
  if (p->vkCode == VK_SPACE && bAltKeyDown) {
    gen_key_event(p->scanCode, press_release);
    return 1; //disable alt-space
  }
  if ((p->vkCode == VK_LWIN) || (p->vkCode == VK_RWIN)) {
    gen_key_event(p->vkCode | 0x100, press_release);
    return 1;//disable windows keys
  }
  if (p->vkCode == VK_ESCAPE && bAltKeyDown) {
    gen_key_event(p->scanCode, press_release);
    return 1;//disable alt-escape
  }
  if (p->vkCode == VK_ESCAPE && bControlKeyDown) {
    gen_key_event(p->scanCode, press_release);
    return 1; //disable ctrl-escape
  }

  return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

#if BX_SHOW_IPS
VOID CALLBACK MyTimer(HWND,UINT,UINT,DWORD);
#endif

static void cursorWarped()
{
  POINT pt = { 0, 0 };

  ClientToScreen(stInfo.simWnd, &pt);
  SetCursorPos(pt.x + stretched_x / 2, pt.y + stretched_y / 2);
  EnterCriticalSection(&stInfo.mouseCS);
  ms_savedx = stretched_x / 2;
  ms_savedy = stretched_y / 2;
  LeaveCriticalSection(&stInfo.mouseCS);
}

static void processMouseXY(int x, int y, int z, int windows_state, int implied_state_change)
{
  int bx_state;
  int old_bx_state;
  EnterCriticalSection(&stInfo.mouseCS);
  bx_state = ((windows_state & MK_LBUTTON) ? 1 : 0) + ((windows_state & MK_RBUTTON) ? 2 : 0) +
             ((windows_state & MK_MBUTTON) ? 4 : 0);
  old_bx_state = bx_state ^ implied_state_change;
  if (old_bx_state != mouse_button_state) {
    /* Make up for missing message */
    BX_INFO(("&&&missing mouse state change"));
    EnterCriticalSection(&stInfo.keyCS);
    enq_mouse_event();
    mouse_button_state = old_bx_state;
    enq_key_event(mouse_button_state, MOUSE_PRESSED);
    LeaveCriticalSection(&stInfo.keyCS);
  }
  ms_ydelta = ms_savedy - y;
  ms_xdelta = x - ms_savedx;
  ms_zdelta = z;
  ms_lastx = x;
  ms_lasty = y;
  if (bx_state!=mouse_button_state) {
    EnterCriticalSection(&stInfo.keyCS);
    enq_mouse_event();
    mouse_button_state = bx_state;
    enq_key_event(mouse_button_state, MOUSE_PRESSED);
    LeaveCriticalSection(&stInfo.keyCS);
  }
  if (mouseCaptureMode && !win32MouseModeAbsXY) {
    cursorWarped();
  }
  LeaveCriticalSection(&stInfo.mouseCS);
}

// GUI thread must be dead/done in order to call terminateEmul
void terminateEmul(int reason)
{
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

  delete[] (char*)bitmap_info;

  for (unsigned b=0; b<bx_bitmap_entries; b++)
    if (bx_bitmaps[b].bmap) DeleteObject(bx_bitmaps[b].bmap);
  for (unsigned c=0; c<256; c++) {
    if (vgafont[0][c]) DeleteObject(vgafont[0][c]);
    if (vgafont[1][c]) DeleteObject(vgafont[1][c]);
  }

  switch (reason) {
    case EXIT_GUI_SHUTDOWN:
      BX_INFO(("Window closed, exiting!"));
      break;
    case EXIT_GMH_FAILURE:
      BX_FATAL(("GetModuleHandle failure!"));
      break;
    case EXIT_FONT_BITMAP_ERROR:
      BX_FATAL(("Font bitmap creation failure!"));
      break;
    case EXIT_HEADER_BITMAP_ERROR:
      BX_FATAL(("Header bitmap creation failure!"));
      break;
    case EXIT_NORMAL:
      break;
  }
}


// WIN32 implementation of the bx_gui_c methods (see nogui.cc for details)

bx_win32_gui_c::bx_win32_gui_c()
{
  // prepare for possible fullscreen mode
  desktopWindow = GetDesktopWindow();
  GetWindowRect(desktopWindow, &desktop);
  desktop_x = desktop.right - desktop.left;
  desktop_y = desktop.bottom - desktop.top;
  max_client_x = desktop_x - 20;
  max_client_y = desktop_y - 80;
}


void bx_win32_gui_c::specific_init(int argc, char **argv, unsigned headerbar_y)
{
  int i;
  bool gui_ci;

  gui_ci = !strcmp(SIM->get_param_enum(BXPN_SEL_CONFIG_INTERFACE)->get_selected(), "win32config");
  put("WINGUI");

  hotKeyReceiver = stInfo.simWnd;
  fullscreenMode = FALSE;
  inFullscreenToggle = FALSE;
  BX_INFO(("Desktop window dimensions: %d x %d", desktop_x, desktop_y));

  static RGBQUAD black_quad={ 0, 0, 0, 0};
  stInfo.kill = 0;
  stInfo.UIinited = FALSE;
  InitializeCriticalSection(&stInfo.drawCS);
  InitializeCriticalSection(&stInfo.keyCS);
  InitializeCriticalSection(&stInfo.mouseCS);

  x_tilesize = this->x_tilesize;
  win32_max_xres = this->max_xres;
  win32_max_yres = this->max_yres;

  bx_bitmap_entries = 0;
  win32_toolbar_entries = 0;
  bx_hb_separator = 0;
  mouseCaptureMode = FALSE;
  mouseCaptureNew = FALSE;
  mouseToggleReq = FALSE;

  // parse win32 specific options
  Bit8u flags = BX_GUI_OPT_NOKEYREPEAT | BX_GUI_OPT_HIDE_IPS  | BX_GUI_OPT_CMDMODE;
  if (argc > 1) {
    for (i = 1; i < argc; i++) {
      if (!parse_common_gui_options(argv[i], flags)) {
        if (!strcmp(argv[i], "traphotkeys")) {
          BX_INFO(("trap system hotkeys for Bochs window"));
          win32_traphotkeys = 1;
        } else if (!strcmp(argv[i], "autoscale")) {
          win32_autoscale = 1;
        } else {
          BX_PANIC(("Unknown win32 option '%s'", argv[i]));
        }
      }
    }
  }

  win32_nokeyrepeat = gui_opts.nokeyrepeat;
  hideIPS = gui_opts.hide_ips;
#if BX_DEBUGGER && BX_DEBUGGER_GUI
  if (gui_opts.enh_dbg_enabled) {
    if (gui_ci) {
      gui_debug = TRUE;
      SIM->set_debug_gui(1);
      win32_enh_dbg_global_ini = gui_opts.enh_dbg_global_ini;
    } else {
      BX_PANIC(("Config interface 'win32config' is required for gui debugger"));
    }
  }
#endif

  mouse_buttons = GetSystemMetrics(SM_CMOUSEBUTTONS);
  BX_INFO(("Number of Mouse Buttons = %d", mouse_buttons));
  if ((SIM->get_param_enum(BXPN_MOUSE_TOGGLE)->get() == BX_MOUSE_TOGGLE_CTRL_MB) &&
      (mouse_buttons == 2)) {
    lstrcpy(szMouseEnable, "CTRL + Lbutton + Rbutton enables mouse ");
    lstrcpy(szMouseDisable, "CTRL + Lbutton + Rbutton disables mouse");
    lstrcpy(szMouseTooltip, "Enable mouse capture\nUse CTRL + Lbutton + Rbutton to release");
  } else {
    wsprintf(szMouseEnable, "%s enables mouse ", get_toggle_info());
    wsprintf(szMouseDisable, "%s disables mouse", get_toggle_info());
    wsprintf(szMouseTooltip, "Enable mouse capture\nUse %s to release", get_toggle_info());
  }

  stInfo.hInstance = GetModuleHandle(NULL);

  UNUSED(headerbar_y);
  dimension_x = 640;
  dimension_y = 480;
  current_bpp = 8;
  stretched_x = dimension_x;
  stretched_y = dimension_y;
  stretch_factor = 1;

  for (unsigned c = 0; c < 256; c++) {
    vgafont[0][c] = NULL;
    vgafont[1][c] = NULL;
  }
  create_vga_font();

  bitmap_info=(BITMAPINFO*)new char[sizeof(BITMAPINFOHEADER)+259*sizeof(RGBQUAD)]; // 256 + 3 entries for 16 bpp mode
  bitmap_info->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
  bitmap_info->bmiHeader.biWidth=x_tilesize;
  // Height is negative for top-down bitmap
  bitmap_info->bmiHeader.biHeight= -(LONG)y_tilesize;
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
  for (i=1; i<259; i++) {
    cmap_index[i] = cmap_index[0];
  }

  if (stInfo.hInstance)
    workerThread = CreateThread(NULL, 0, UIThread, NULL, 0, &workerThreadID);
  else
    terminateEmul(EXIT_GMH_FAILURE);

  // Wait until UI init is ready before continuing
  if ((stInfo.kill == 0) && (stInfo.UIinited == FALSE))
    Sleep(500);

  // Now set this thread's priority to below normal because this is where
  //  the emulated CPU runs, and it hogs the real CPU
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

  if (SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->get())
    BX_INFO(("private_colormap option ignored."));

  // load keymap tables
  if (SIM->get_param_bool(BXPN_KBD_USEMAPPING)->get()) {
    bx_keymap.loadKeymap(NULL);  // I have no function to convert X windows symbols
  }

  if (gui_ci) {
    dialog_caps = BX_GUI_DLG_ALL;
  }
  new_text_api = 1;
}

void set_fullscreen_mode(BOOL enable)
{
  unsigned long mainStyle, simExStyle;

  if (enable) {
    if (desktop_y > 0) {
      if (!queryFullScreen) {
        MessageBox(NULL,
          "Going into fullscreen mode -- Alt-Enter to revert",
          "Going fullscreen",
          MB_APPLMODAL);
        queryFullScreen = TRUE;
        enq_key_event(0x38, BX_KEY_RELEASED); // send lost ALT keyup event
      }
      inFullscreenToggle = TRUE;
      BX_INFO(("entering fullscreen mode"));
      stretched_x = desktop_x;
      stretched_y = desktop_y;
    } else {
      return;
    }
    if (win32_autoscale) {
      stretch_factor = 1;
      while (((dimension_x * stretch_factor * 2) <= desktop_x) &&
             ((dimension_y * stretch_factor * 2) <= desktop_y)) {
        stretch_factor *= 2;
      }
      if (stretch_factor > 1) BX_INFO(("autoscale: factor = %d", stretch_factor));
    }
    // hide toolbar and status bars to get some additional space
    ShowWindow(hwndTB, SW_HIDE);
    ShowWindow(hwndSB, SW_HIDE);
    // hide title bar and border
    mainStyle = GetWindowLong(stInfo.mainWnd, GWL_STYLE);
    mainStyle &= ~WS_CAPTION;
    SetWindowLong(stInfo.mainWnd, GWL_STYLE, mainStyle);
    simExStyle = GetWindowLong(stInfo.simWnd, GWL_EXSTYLE);
    simExStyle &= ~WS_EX_CLIENTEDGE;
    SetWindowLong(stInfo.simWnd, GWL_EXSTYLE, simExStyle);
    // maybe need to adjust stInfo.simWnd here also?
    saveParent = SetParent(stInfo.mainWnd, desktopWindow);
    if (saveParent) {
      BX_DEBUG(("Saved parent window"));
      SetWindowPos(stInfo.mainWnd, HWND_TOPMOST, desktop.left, desktop.top,
       desktop.right, desktop.bottom, SWP_SHOWWINDOW);
    }
    fullscreenMode = TRUE;
    inFullscreenToggle = FALSE;
  } else {
    BX_INFO(("leaving fullscreen mode"));
    stretched_x = dimension_x;
    stretched_y = dimension_y;
    if (saveParent) {
      BX_DEBUG(("Restoring parent window"));
      SetParent(stInfo.mainWnd, saveParent);
      SetWindowPos(stInfo.mainWnd, HWND_NOTOPMOST,
        0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
      saveParent = NULL;
    }
    // put back the title bar, border, etc...
    mainStyle = GetWindowLong(stInfo.mainWnd, GWL_STYLE);
    mainStyle |= WS_CAPTION;
    SetWindowLong(stInfo.mainWnd, GWL_STYLE, mainStyle);
    simExStyle = GetWindowLong(stInfo.simWnd, GWL_EXSTYLE);
    simExStyle |= WS_EX_CLIENTEDGE;
    SetWindowLong(stInfo.simWnd, GWL_EXSTYLE, simExStyle);
    fullscreenMode = FALSE;
  }
}

void resize_main_window(BOOL disable_fullscreen)
{
  RECT R;
  int toolbar_y = 0;
  int statusbar_y = 0;

  if (IsWindowVisible(hwndTB)) {
    toolbarVisible = TRUE;
  }
  if (IsWindowVisible(hwndSB)) {
    statusVisible = TRUE;
  }

  if ((desktop_y > 0) && (dimension_y >= desktop_y)) {
    set_fullscreen_mode(true);
  } else {
    if (fullscreenMode && disable_fullscreen) {
      set_fullscreen_mode(false);
    }
    if (win32_autoscale) {
      stretch_factor = 1;
      if (!fullscreenMode) {
        while (((dimension_x * stretch_factor * 2) <= max_client_x) &&
               ((dimension_y * stretch_factor * 2) <= max_client_y)) {
          stretch_factor *= 2;
        }
      } else {
        while (((dimension_x * stretch_factor * 2) <= desktop_x) &&
               ((dimension_y * stretch_factor * 2) <= desktop_y)) {
          stretch_factor *= 2;
        }
      }
      if (stretch_factor > 1) BX_INFO(("autoscale: factor = %d", stretch_factor));
    }
    if (!fullscreenMode) {
      if (stretch_factor > 1) {
        stretched_x = dimension_x * stretch_factor;
        stretched_y = dimension_y * stretch_factor;
      } else {
        stretched_x = dimension_x;
        stretched_y = dimension_y;
      }
      if (toolbarVisible) {
        ShowWindow(hwndTB, SW_SHOW);
        GetWindowRect(hwndTB, &R);
        toolbar_y = R.bottom - R.top;
      }
      if (statusVisible) {
        ShowWindow(hwndSB, SW_SHOW);
        GetWindowRect(hwndSB, &R);
        statusbar_y = R.bottom - R.top;
      }
    }
    SetRect(&R, 0, 0, stretched_x, stretched_y);
    DWORD style = GetWindowLong(stInfo.simWnd, GWL_STYLE);
    DWORD exstyle = GetWindowLong(stInfo.simWnd, GWL_EXSTYLE);
    AdjustWindowRectEx(&R, style, FALSE, exstyle);
    style = GetWindowLong(stInfo.mainWnd, GWL_STYLE);
    AdjustWindowRect(&R, style, FALSE);
    SetWindowPos(stInfo.mainWnd, HWND_TOP, 0, 0, R.right - R.left,
                 R.bottom - R.top + toolbar_y + statusbar_y,
                 SWP_NOMOVE | SWP_NOZORDER);
  }
}

// This thread controls the GUI window.
DWORD WINAPI UIThread(LPVOID)
{
  MSG msg;
  HDC hdc;
  WNDCLASS wndclass;
  RECT wndRect;

  workerThreadID = GetCurrentThreadId();

  GetClassInfo(NULL, WC_DIALOG, &wndclass);
  wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
  wndclass.lpfnWndProc = mainWndProc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hInstance = stInfo.hInstance;
  wndclass.hIcon = LoadIcon (stInfo.hInstance, MAKEINTRESOURCE(ICON_BOCHS));
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName = szAppName;

  RegisterClass(&wndclass);

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

  RegisterClass(&wndclass);

  SetRect(&wndRect, 0, 0, stretched_x, stretched_y);
  DWORD sim_style = WS_CHILD;
  DWORD sim_exstyle = WS_EX_CLIENTEDGE;
  AdjustWindowRectEx(&wndRect, sim_style, FALSE, sim_exstyle);
  DWORD main_style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
  AdjustWindowRect(&wndRect, main_style, FALSE);
  stInfo.mainWnd = CreateWindow (szAppName,
                     szWindowName,
                     main_style,
                     CW_USEDEFAULT,
                     CW_USEDEFAULT,
                     wndRect.right - wndRect.left,
                     wndRect.bottom - wndRect.top,
                     NULL,
                     NULL,
                     stInfo.hInstance,
                     NULL);

  if (stInfo.mainWnd) {

    InitCommonControls();
    hwndTB = CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR) NULL,
               WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT, 0, 0, 0, 0, stInfo.mainWnd,
               (HMENU) 100, stInfo.hInstance, NULL);
    SendMessage(hwndTB, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);
    SendMessage(hwndTB, TB_SETBITMAPSIZE, 0, (LPARAM)MAKELONG(32, 32));

    hwndSB = CreateWindowEx(0, STATUSCLASSNAME, (PCTSTR)NULL,
                            WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, stInfo.mainWnd,
                            (HMENU) 0x7712, stInfo.hInstance, NULL);
    if (hwndSB) {
      unsigned elements;
      SB_Edges[0] = SIZE_OF_SB_MOUSE_MESSAGE + SIZE_OF_SB_ELEMENT;
      SB_Text_Elements = 1;
#if BX_SHOW_IPS
      if (!hideIPS) {
        SB_Edges[1] = SB_Edges[0] + SIZE_OF_SB_IPS_MESSAGE;
        SB_Text_Elements = 2;
      }
#endif
      for (elements = SB_Text_Elements; elements < (BX_MAX_STATUSITEMS+SB_Text_Elements); elements++)
        SB_Edges[elements] = SB_Edges[elements-1] + SIZE_OF_SB_ELEMENT;
      SB_Edges[elements] = -1;
      SendMessage(hwndSB, SB_SETPARTS, BX_MAX_STATUSITEMS+SB_Text_Elements+1, (LPARAM)&SB_Edges);
    }
    SetStatusText(0, szMouseEnable, TRUE);

    stInfo.simWnd = CreateWindowEx(sim_exstyle,
                      "SIMWINDOW",
                      "",
                      sim_style,
                      0,
                      0,
                      0,
                      0,
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
    SetFocus(stInfo.simWnd);

    ShowCursor(!mouseCaptureMode);
    if (mouseCaptureMode && !win32MouseModeAbsXY) {
      cursorWarped();
    }

    hdc = GetDC(stInfo.simWnd);
    MemoryBitmap = CreateCompatibleBitmap(hdc, win32_max_xres, win32_max_yres);
    MemoryDC = CreateCompatibleDC(hdc);
    ReleaseDC(stInfo.simWnd, hdc);

    if (MemoryBitmap && MemoryDC) {
      resize_main_window(FALSE);
      ShowWindow(stInfo.mainWnd, SW_SHOW);
#if BX_DEBUGGER && BX_DEBUGGER_GUI
      if (gui_debug) {
        bx_gui->init_debug_dialog(win32_enh_dbg_global_ini);
      }
#endif
#if BX_SHOW_IPS
      if (!hideIPS) {
        UINT idTimer = 2;
        SetTimer(stInfo.simWnd, idTimer, 1000, (TIMERPROC)MyTimer);
      }
#endif
      stInfo.UIinited = TRUE;

      bx_gui->clear_screen();

      while (GetMessage (&msg, NULL, 0, 0)) {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
      }
    }
  }

  stInfo.kill = EXIT_GUI_SHUTDOWN;

  return 0;
}

void SetStatusText(unsigned Num, const char *Text, bool active, Bit8u color)
{
  char StatText[MAX_PATH];

  if ((Num < SB_Text_Elements) || (Num > (BX_MAX_STATUSITEMS+SB_Text_Elements))) {
    StatText[0] = ' ';  // Add space to text in 1st and last items
    lstrcpy(StatText+1, Text);
    SendMessage(hwndSB, SB_SETTEXT, Num, (LPARAM)StatText);
  } else {
    StatText[0] = 9;  // Center the rest
    lstrcpy(StatText+1, Text);
    lstrcpy(SB_Text[Num-SB_Text_Elements], StatText);
    SB_Active[Num-SB_Text_Elements] = active;
    SB_ActiveColor[Num-SB_Text_Elements] = color;
    SendMessage(hwndSB, SB_SETTEXT, Num | SBT_OWNERDRAW, (LPARAM)SB_Text[Num-SB_Text_Elements]);
  }
  UpdateWindow(hwndSB);
}

void bx_win32_gui_c::statusbar_setitem_specific(int element, bool active, bool w)
{
  Bit8u color = 0;
  if (w) {
    color = statusitem[element].auto_off ? 1 : 2;
  }
  SetStatusText(element+SB_Text_Elements, statusitem[element].text, active, color);
}

LRESULT CALLBACK mainWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
  DRAWITEMSTRUCT *lpdis;
  char *sbtext;
  NMHDR *lpnmh;
  TOOLTIPTEXT *lpttt;
  int idTT, hbar_id;

  switch (iMsg) {
  case WM_CREATE:
    SetStatusText(0, szMouseEnable, TRUE);
    if (win32_traphotkeys) {
      hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    }
    return 0;

  case WM_COMMAND:
    if (LOWORD(wParam) >= 101) {
      EnterCriticalSection(&stInfo.keyCS);
      enq_key_event(LOWORD(wParam)-101, TOOLBAR_CLICKED);
      LeaveCriticalSection(&stInfo.keyCS);
    }
    break;

  case WM_SETFOCUS:
    SetFocus(stInfo.simWnd);
    return 0;

  case WM_KILLFOCUS:
    enq_key_event(0, FOCUS_CHANGED);
    return 0;

  case WM_CLOSE:
    SendMessage(stInfo.simWnd, WM_CLOSE, 0, 0);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  case WM_SIZE:
    if (!IsIconic(hwnd)) {
      int x, y;
      SendMessage(hwndTB, TB_AUTOSIZE, 0, 0);
      SendMessage(hwndSB, WM_SIZE, 0, 0);
      // now fit simWindow to mainWindow
      int rect_data[] = { 1, 0, IsWindowVisible(hwndTB),
         100, IsWindowVisible(hwndSB), 0x7712, 0, 0 };
      RECT R;
      GetEffectiveClientRect(hwnd, &R, rect_data);
      x = R.right - R.left;
      y = R.bottom - R.top;
      MoveWindow(stInfo.simWnd, R.left, R.top, x, y, TRUE);
      GetClientRect(stInfo.simWnd, &R);
      x = R.right - R.left;
      y = R.bottom - R.top;
      if (!inFullscreenToggle && ((x != (int)stretched_x) || (y != (int)stretched_y))) {
        BX_ERROR(("Sim client size(%d, %d) != stretched size(%d, %d)!",
          x, y, stretched_x, stretched_y));
        if (!saveParent) fix_size = TRUE; // no fixing if fullscreen
      }
    }
    break;

  case WM_DRAWITEM:
    lpdis = (DRAWITEMSTRUCT *)lParam;
    if (lpdis->hwndItem == hwndSB) {
      sbtext = (char *)lpdis->itemData;
      if (SB_Active[lpdis->itemID-SB_Text_Elements]) {
        SetBkColor(lpdis->hDC, SB_Led_Colors[SB_ActiveColor[lpdis->itemID-SB_Text_Elements]]);
      } else {
        SetBkMode(lpdis->hDC, TRANSPARENT);
        SetTextColor(lpdis->hDC, 0x00808080);
      }
      DrawText(lpdis->hDC, sbtext+1, lstrlen(sbtext)-1, &lpdis->rcItem, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      return TRUE;
    }
    break;

  case WM_NOTIFY:
    lpnmh = (LPNMHDR)lParam;
    if (lpnmh->code == TTN_NEEDTEXT) {
      lpttt = (LPTOOLTIPTEXT)lParam;
      idTT = (int)wParam;
      hbar_id = idTT - 101;
      if (SendMessage(hwndTB, TB_GETSTATE, idTT, 0) && win32_toolbar_entry[hbar_id].tooltip != NULL) {
        lstrcpy(lpttt->szText, win32_toolbar_entry[hbar_id].tooltip);
      }
    }
    return FALSE;
    break;

  }
  return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void SetMouseToggleInfo()
{
  if (mouseCaptureMode) {
    SetStatusText(0, szMouseDisable, TRUE);
  } else {
    SetStatusText(0, szMouseEnable, TRUE);
  }
}

void SetMouseCapture()
{
  POINT pt = { 0, 0 };
  RECT  re;

  if (mouseToggleReq) {
    mouseCaptureMode = mouseCaptureNew;
    mouseToggleReq = FALSE;
  } else {
    SIM->get_param_bool(BXPN_MOUSE_ENABLED)->set(mouseCaptureMode);
  }
  ShowCursor(!mouseCaptureMode);
  ShowCursor(!mouseCaptureMode);   // somehow one didn't do the trick (win98)
  if (mouseCaptureMode && !win32MouseModeAbsXY) {
    cursorWarped();
  }
  if (mouseCaptureMode) {
    ClientToScreen(stInfo.simWnd, &pt);
    re.left = pt.x;
    re.top = pt.y;
    re.right = pt.x + stretched_x;
    re.bottom = pt.y + stretched_y;
    ClipCursor(&re);
  } else {
    ClipCursor(NULL);
  }
  SetMouseToggleInfo();
}

LRESULT CALLBACK simWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
  HDC hdc, hdcMem;
  PAINTSTRUCT ps;
  bool mouse_toggle = 0;
  int toolbar_cmd = -1;
  Bit8u kmodchange = 0;
  bool keymod = 0;
  static BOOL mouseModeChange = FALSE;

  switch (iMsg) {

  case WM_CREATE:
    SetTimer(hwnd, 1, 250, NULL);
    return 0;

  case WM_TIMER:
    if (mouseToggleReq && (GetActiveWindow() == stInfo.mainWnd)) {
      SetMouseCapture();
    }
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
                 ps.rcPaint.left/stretch_factor, ps.rcPaint.top/stretch_factor,
                 (ps.rcPaint.right - ps.rcPaint.left+1)/stretch_factor,
                 (ps.rcPaint.bottom - ps.rcPaint.top+1)/stretch_factor, SRCCOPY);
    }
    DeleteDC (hdcMem);
    EndPaint (hwnd, &ps);
    LeaveCriticalSection(&stInfo.drawCS);
    return 0;

  case WM_SIZE:
    if (mouseCaptureMode) {
      POINT pt = { 0, 0 };
      RECT  re;
      ClientToScreen(stInfo.simWnd, &pt);
      re.left = pt.x;
      re.top = pt.y;
      re.right = pt.x + stretched_x;
      re.bottom = pt.y + stretched_y;
      ClipCursor(&re);
    }
    break;

  case WM_MOUSEMOVE:
    if ((LOWORD(lParam) == ms_savedx) && (HIWORD(lParam) == ms_savedy)) {
      // Ignore mouse event generated by SetCursorPos().
      return 0;
    }
    if (!mouseModeChange) {
      processMouseXY(LOWORD(lParam), HIWORD(lParam), 0, (int) wParam, 0);
    }
    return 0;

  case WM_MOUSEWHEEL:
    if (!mouseModeChange) {
      // WM_MOUSEWHEEL returns x and y relative to the main screen.
      // WM_MOUSEMOVE below returns x and y relative to the current view.
      POINT pt;
      pt.x = LOWORD(lParam);
      pt.y = HIWORD(lParam);
      ScreenToClient(stInfo.simWnd, &pt);
      processMouseXY(pt.x, pt.y, (Bit16s) HIWORD(wParam) / 120, LOWORD(wParam), 0);
    }
    return 0;

  case WM_LBUTTONDOWN:
  case WM_LBUTTONDBLCLK:
  case WM_LBUTTONUP:
    if (mouse_buttons == 2) {
      if ((wParam & MK_LBUTTON) == MK_LBUTTON) {
        if (bx_gui->mouse_toggle_check(BX_MT_LBUTTON, 1)) {
          mouseCaptureMode = !mouseCaptureMode;
          SetMouseCapture();
          mouseModeChange = TRUE;
        }
      } else if (mouseModeChange && (iMsg == WM_LBUTTONUP)) {
        bx_gui->mouse_toggle_check(BX_MT_LBUTTON, 0);
        mouseModeChange = FALSE;
      } else {
        processMouseXY(LOWORD(lParam), HIWORD(lParam), 0, (int) wParam, 1);
      }
      return 0;
    }
    processMouseXY(LOWORD(lParam), HIWORD(lParam), 0, (int) wParam, 1);
    return 0;

  case WM_MBUTTONDOWN:
  case WM_MBUTTONDBLCLK:
  case WM_MBUTTONUP:
    if ((wParam & MK_MBUTTON) == MK_MBUTTON) {
      if (bx_gui->mouse_toggle_check(BX_MT_MBUTTON, 1)) {
        mouseCaptureMode = !mouseCaptureMode;
        SetMouseCapture();
        mouseModeChange = TRUE;
      }
    } else if (mouseModeChange && (iMsg == WM_MBUTTONUP)) {
      bx_gui->mouse_toggle_check(BX_MT_MBUTTON, 0);
      mouseModeChange = FALSE;
    } else {
      processMouseXY(LOWORD(lParam), HIWORD(lParam), 0, (int) wParam, 4);
    }
    return 0;

  case WM_RBUTTONDOWN:
  case WM_RBUTTONDBLCLK:
  case WM_RBUTTONUP:
    if (mouse_buttons == 2) {
      if ((wParam & MK_RBUTTON) == MK_RBUTTON) {
        if (bx_gui->mouse_toggle_check(BX_MT_RBUTTON, 1)) {
          mouseCaptureMode = !mouseCaptureMode;
          SetMouseCapture();
          mouseModeChange = TRUE;
        }
      } else if (mouseModeChange && (iMsg == WM_RBUTTONUP)) {
        bx_gui->mouse_toggle_check(BX_MT_RBUTTON, 0);
        mouseModeChange = FALSE;
      } else {
        processMouseXY(LOWORD(lParam), HIWORD(lParam), 0, (int) wParam, 2);
      }
      return 0;
    }
    processMouseXY(LOWORD(lParam), HIWORD(lParam), 0, (int) wParam, 2);
    return 0;

  case WM_CLOSE:
    return DefWindowProc (hwnd, iMsg, wParam, lParam);

  case WM_DESTROY:
    KillTimer(hwnd, 1);
    stInfo.UIinited = FALSE;
    return 0;

  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
    // check modifier keys
    if (wParam == VK_SHIFT) {
      kmodchange = bx_gui->set_modifier_keys(BX_MOD_KEY_SHIFT, 1);
      keymod = 1;
    } else if (wParam == VK_CONTROL) {
      kmodchange = bx_gui->set_modifier_keys(BX_MOD_KEY_CTRL, 1);
      keymod = 1;
    } else if (wParam == VK_MENU) {
      kmodchange = bx_gui->set_modifier_keys(BX_MOD_KEY_ALT, 1);
      keymod = 1;
    } else if (wParam == VK_CAPITAL) {
      kmodchange = bx_gui->set_modifier_keys(BX_MOD_KEY_CAPS, 1);
      keymod = 1;
    }
    // mouse capture toggle-check
    if (kmodchange == BX_MOD_KEY_CTRL) {
      mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_CTRL, 1);
    } else if (kmodchange == BX_MOD_KEY_ALT) {
      mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_ALT, 1);
    } else if (wParam == VK_F10) {
      mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_F10, 1);
    } else if (wParam == VK_F12) {
      mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_F12, 1);
    }
    if (mouse_toggle) {
      mouseCaptureMode = !mouseCaptureMode;
      SetMouseCapture();
      return 0;
    }
    if (bx_gui->command_mode_active()) {
      if (bx_gui->get_modifier_keys() == 0) {
        if (wParam == 'A') {
          toolbar_cmd = 0; // Floppy A
        } else if (wParam == 'B') {
          toolbar_cmd = 1; // Floppy B
        } else if (wParam == 'C') {
          toolbar_cmd = 10; // Copy
        } else if (wParam == 'F') {
          if (!saveParent) {
            set_fullscreen_mode(TRUE);
            bx_gui->set_fullscreen_mode(1);
          } else {
            resize_main_window(TRUE);
            bx_gui->set_fullscreen_mode(0);
          }
        } else if (wParam == 'M') {
          bx_gui->marklog_handler();
        } else if (wParam == 'P') {
          toolbar_cmd = 9; // Paste
        } else if (wParam == 'R') {
          toolbar_cmd = 6; // Reset
        } else if (wParam == 'S') {
          toolbar_cmd = 8; // Snapshot
        } else if (wParam == 'U') {
          toolbar_cmd = 11; // User
        }
      } else if (bx_gui->get_modifier_keys() == BX_MOD_KEY_SHIFT) {
        if (wParam == 'C') {
          toolbar_cmd = 7; // Config
        } else if (wParam == 'P') {
          toolbar_cmd = 4; // Power
        } else if (wParam == 'S') {
          toolbar_cmd = 5; // Suspend
        }
      }
      if (!keymod) {
        bx_gui->set_command_mode(0);
        SetMouseToggleInfo();
      }
      if (toolbar_cmd >= 0) {
        EnterCriticalSection(&stInfo.keyCS);
        enq_key_event((Bit32u)toolbar_cmd, TOOLBAR_CLICKED);
        LeaveCriticalSection(&stInfo.keyCS);
        return 0;
      }
      if (wParam != COMMAND_MODE_VKEY) {
        return 0;
      }
    } else {
      if (bx_gui->has_command_mode() && (bx_gui->get_modifier_keys() == 0) &&
          (wParam == COMMAND_MODE_VKEY)) {
        bx_gui->set_command_mode(1);
        SetStatusText(0, "Command mode", TRUE);
        return 0;
      }
    }
    EnterCriticalSection(&stInfo.keyCS);
    if (((lParam & 0x40000000) == 0) || !win32_nokeyrepeat) {
      enq_key_event(HIWORD (lParam) & 0x01FF, BX_KEY_PRESSED);
    }
    LeaveCriticalSection(&stInfo.keyCS);
    return 0;

  case WM_KEYUP:
  case WM_SYSKEYUP:
    // check if it's keyup, alt key, non-repeat
    // see http://msdn2.microsoft.com/en-us/library/ms646267.aspx
    if ((wParam == VK_RETURN) &&
        ((HIWORD(lParam) & BX_SYSKEY) == (KF_ALTDOWN | KF_UP))) {
      if (!saveParent) {
        set_fullscreen_mode(TRUE);
      } else {
        resize_main_window(TRUE);
      }
    } else {
      // check modifier keys
      if (wParam == VK_SHIFT) {
        kmodchange = bx_gui->set_modifier_keys(BX_MOD_KEY_SHIFT, 0);
      } else if (wParam == VK_CONTROL) {
        kmodchange = bx_gui->set_modifier_keys(BX_MOD_KEY_CTRL, 0);
      } else if (wParam == VK_MENU) {
        kmodchange = bx_gui->set_modifier_keys(BX_MOD_KEY_ALT, 0);
      }
      // mouse capture toggle-check
      if (kmodchange == BX_MOD_KEY_CTRL) {
        bx_gui->mouse_toggle_check(BX_MT_KEY_CTRL, 0);
      } else if (kmodchange == BX_MOD_KEY_ALT) {
        bx_gui->mouse_toggle_check(BX_MT_KEY_ALT, 0);
      } else if (wParam == VK_F10) {
        bx_gui->mouse_toggle_check(BX_MT_KEY_F10, 0);
      } else if (wParam == VK_F12) {
        bx_gui->mouse_toggle_check(BX_MT_KEY_F12, 0);
      }
      EnterCriticalSection(&stInfo.keyCS);
      enq_key_event(HIWORD (lParam) & 0x01FF, BX_KEY_RELEASED);
      LeaveCriticalSection(&stInfo.keyCS);
    }
    return 0;

  case WM_SYSCHAR:
    // check if it's keydown, alt key, non-repeat
    // see http://msdn2.microsoft.com/en-us/library/ms646267.aspx
    if (wParam == VK_RETURN) {
      if ((HIWORD(lParam) & BX_SYSKEY) == KF_ALTDOWN) {
        if (!saveParent) {
          set_fullscreen_mode(TRUE);
        } else {
          resize_main_window(TRUE);
        }
      }
    }
  case WM_CHAR:
  case WM_DEADCHAR:
  case WM_SYSDEADCHAR:
    return 0;
  }
  return DefWindowProc (hwnd, iMsg, wParam, lParam);
}


void enq_key_event(Bit32u key, Bit32u press_release)
{
  static BOOL alt_pressed_l = FALSE;
  static BOOL alt_pressed_r = FALSE;
  static BOOL ctrl_pressed_l = FALSE;
  static BOOL ctrl_pressed_r = FALSE;
  static BOOL shift_pressed_l = FALSE;
  static BOOL shift_pressed_r = FALSE;

  if (press_release == FOCUS_CHANGED) {
    alt_pressed_l = FALSE;
    alt_pressed_r = FALSE;
    ctrl_pressed_l = FALSE;
    ctrl_pressed_r = FALSE;
    shift_pressed_l = FALSE;
    shift_pressed_r = FALSE;
  } else if (press_release == BX_KEY_PRESSED) {
    // Windows generates multiple keypresses when holding down these keys
    switch (key) {
      case 0x1d:
        if (ctrl_pressed_l)
          return;
        ctrl_pressed_l = TRUE;
        break;
      case 0x2a:
        if (shift_pressed_l)
          return;
        shift_pressed_l = TRUE;
        break;
      case 0x36:
        if (shift_pressed_r)
          return;
        shift_pressed_r = TRUE;
        break;
      case 0x38:
        if (alt_pressed_l)
          return;
        alt_pressed_l = TRUE;
        break;
      case 0x011d:
        if (ctrl_pressed_r)
          return;
        ctrl_pressed_r = TRUE;
        break;
      case 0x0138:
        if (alt_pressed_r)
          return;
        // This makes the "AltGr" key on European keyboards work
        if (ctrl_pressed_l) {
          enq_key_event(0x1d, BX_KEY_RELEASED);
        }
        alt_pressed_r = TRUE;
        break;
    }
  } else {
    switch (key) {
      case 0x1d:
        if (!ctrl_pressed_l)
          return;
        ctrl_pressed_l = FALSE;
        break;
      case 0x2a:
        shift_pressed_l = FALSE;
        break;
      case 0x36:
        shift_pressed_r = FALSE;
        break;
      case 0x38:
        alt_pressed_l = FALSE;
        break;
      case 0x011d:
        ctrl_pressed_r = FALSE;
        break;
      case 0x0138:
        alt_pressed_r = FALSE;
        break;
    }
  }
  if (((tail+1) % SCANCODE_BUFSIZE) == head) {
    BX_ERROR(("enq_scancode: buffer full"));
    return;
  }
  keyevents[tail].key_event = key | press_release;
  tail = (tail + 1) % SCANCODE_BUFSIZE;
}

void enq_mouse_event(void)
{
  EnterCriticalSection(&stInfo.mouseCS);
  if (ms_xdelta || ms_ydelta || ms_zdelta) {
    if (((tail+1) % SCANCODE_BUFSIZE) == head) {
      LeaveCriticalSection(&stInfo.mouseCS);
      BX_ERROR(("enq_scancode: buffer full"));
      return;
    }
    QueueEvent& current=keyevents[tail];
    current.key_event=MOUSE_MOTION;
    if (win32MouseModeAbsXY) {
      current.mouse_x = ms_lastx * 0x7fff / dimension_x;
      current.mouse_y = ms_lasty * 0x7fff / dimension_y;
    } else {
      current.mouse_x = ms_xdelta;
      current.mouse_y = ms_ydelta;
    }
    current.mouse_z = ms_zdelta;
    current.mouse_button_state = mouse_button_state;
    ms_ydelta = ms_xdelta = ms_zdelta = 0;
    tail = (tail + 1) % SCANCODE_BUFSIZE;
  }
  LeaveCriticalSection(&stInfo.mouseCS);
}

QueueEvent* deq_key_event(void)
{
  QueueEvent* key;

  if (head == tail) {
    BX_ERROR(("deq_scancode: buffer empty"));
    return((QueueEvent*)0);
  }
  key = &keyevents[head];
  head = (head + 1) % SCANCODE_BUFSIZE;

  return(key);
}


void bx_win32_gui_c::handle_events(void)
{
  Bit32u key;
  Bit32u key_event;

  if (stInfo.kill) terminateEmul(stInfo.kill);

  if (fix_size) {
    resize_main_window(FALSE);
    fix_size = FALSE;
  }

  // Handle mouse moves
  enq_mouse_event();

  // Handle keyboard and mouse clicks
  EnterCriticalSection(&stInfo.keyCS);
  while (head != tail) {
    QueueEvent* queue_event = deq_key_event();
    if (!queue_event)
      break;
    key = queue_event->key_event;
    if (key == MOUSE_MOTION)
    {
      DEV_mouse_motion(queue_event->mouse_x, queue_event->mouse_y,
                       queue_event->mouse_z, queue_event->mouse_button_state, win32MouseModeAbsXY);
    }
    else if (key == FOCUS_CHANGED) {
      DEV_kbd_release_keys();
    }
    // Check for mouse buttons first
    else if (key & MOUSE_PRESSED) {
      DEV_mouse_motion(0, 0, 0, LOWORD(key), 0);
    }
    else if (key & TOOLBAR_CLICKED) {
      win32_toolbar_click(LOWORD(key));
    }
    else {
      key_event = win32_to_bx_key[(key & 0x100) ? 1 : 0][key & 0xff];
      if (key & BX_KEY_RELEASED) key_event |= BX_KEY_RELEASED;
      DEV_kbd_gen_scancode(key_event);
    }
  }
  LeaveCriticalSection(&stInfo.keyCS);
#if BX_SHOW_IPS
  if (ipsUpdate) {
    SetStatusText(1, ipsText, 1);
    ipsUpdate = FALSE;
  }
#endif
}


void bx_win32_gui_c::flush(void)
{
  EnterCriticalSection(&stInfo.drawCS);
  if (updated_area_valid) {
    // slight bugfix
	updated_area.right++;
	updated_area.bottom++;
	InvalidateRect(stInfo.simWnd, &updated_area, FALSE);
	updated_area_valid = FALSE;
  }
  LeaveCriticalSection(&stInfo.drawCS);
}

void bx_win32_gui_c::clear_screen(void)
{
  HGDIOBJ oldObj;

  if (!stInfo.UIinited) return;

  EnterCriticalSection(&stInfo.drawCS);

  oldObj = SelectObject(MemoryDC, MemoryBitmap);
  PatBlt(MemoryDC, 0, 0, stretched_x, stretched_y, BLACKNESS);
  SelectObject(MemoryDC, oldObj);

  updateUpdated(0, 0, dimension_x-1, dimension_y-1);

  LeaveCriticalSection(&stInfo.drawCS);
}


void bx_win32_gui_c::set_font(bool lg)
{
  Bit8u data[64], i;

  for (unsigned m = 0; m < 2; m++) {
    for (unsigned c = 0; c < 256; c++) {
      if (char_changed[m][c]) {
        memset(data, 0, sizeof(data));
        BOOL gfxchar = lg && ((c & 0xE0) == 0xC0);
        for (i=0; i<(unsigned)yChar; i++) {
          data[i*2] = vga_charmap[m][c*32+i];
          if (gfxchar) {
            data[i*2+1] = (data[i*2] << 7);
          }
        }
        SetBitmapBits(vgafont[m][c], 64, data);
        char_changed[m][c] = 0;
      }
    }
  }
}

void bx_win32_gui_c::draw_char(Bit8u ch, Bit8u fc, Bit8u bc, Bit16u xc, Bit16u yc,
                               Bit8u fw, Bit8u fh, Bit8u fx, Bit8u fy,
                               bool gfxcharw9, Bit8u cs, Bit8u ce, bool curs, bool font2)
{
  HDC hdc;

  if (!stInfo.UIinited) return;

  EnterCriticalSection(&stInfo.drawCS);
  hdc = GetDC(stInfo.simWnd);
  Bit8u map = (font2) ? 1 : 0;
  DrawBitmap(hdc, vgafont[map][ch], xc, yc, fw, fh, fx, fy, fc, bc);
  if (curs && (ce >= fy) && (cs < (fh + fy))) {
    if (cs > fy) {
      yc += (cs - fy);
      fh -= (cs - fy);
    }
    if ((ce - cs + 1) < fh) {
      fh = ce - cs + 1;
    }
    DrawBitmap(hdc, vgafont[map][ch], xc, yc, fw, fh, fx, cs, bc, fc);
  }
  ReleaseDC(stInfo.simWnd, hdc);
  LeaveCriticalSection(&stInfo.drawCS);
}

void bx_win32_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
                                 unsigned long cursor_x, unsigned long cursor_y,
                                 bx_vga_tminfo_t *tm_info)
{
  // present for compatibility
}

int bx_win32_gui_c::get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)
{
  if (OpenClipboard(stInfo.simWnd)) {
    HGLOBAL hg = GetClipboardData(CF_TEXT);
    char *data = (char *)GlobalLock(hg);
    *nbytes = (Bit32s)strlen(data);
    *bytes = new Bit8u[1 + *nbytes];
    BX_INFO(("found %d bytes on the clipboard", *nbytes));
    memcpy (*bytes, data, *nbytes+1);
    BX_INFO(("first byte is 0x%02x", *bytes[0]));
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

int bx_win32_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
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


bool bx_win32_gui_c::palette_change(Bit8u index, Bit8u red, Bit8u green,
                                       Bit8u blue) {
  if ((current_bpp == 16) && (index < 3)) {
    cmap_index[256+index].rgbRed = red;
    cmap_index[256+index].rgbBlue = blue;
    cmap_index[256+index].rgbGreen = green;
    return 0;
  } else {
    cmap_index[index].rgbRed = red;
    cmap_index[index].rgbBlue = blue;
    cmap_index[index].rgbGreen = green;
  }
  return(1);
}


void bx_win32_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
  HDC hdc;
  HGDIOBJ oldObj;
  unsigned yt = y_tilesize;

  if ((y0 + y_tilesize) > dimension_y) {
	yt = dimension_y - y0;
  }
  EnterCriticalSection(&stInfo.drawCS);
  hdc = GetDC(stInfo.simWnd);

  oldObj = SelectObject(MemoryDC, MemoryBitmap);

  StretchDIBits(MemoryDC, x0, y0, x_tilesize, yt, 0, 0,
    x_tilesize, yt, tile, bitmap_info, DIB_RGB_COLORS, SRCCOPY);

  SelectObject(MemoryDC, oldObj);

  updateUpdated(x0, y0, x0 + x_tilesize - 1, y0 + yt - 1);

  ReleaseDC(stInfo.simWnd, hdc);
  LeaveCriticalSection(&stInfo.drawCS);
}



void bx_win32_gui_c::dimension_update(unsigned x, unsigned y, unsigned fheight, unsigned fwidth, unsigned bpp)
{
  guest_textmode = (fheight > 0);
  guest_fwidth = fwidth;
  guest_fheight = fheight;
  guest_xres = x;
  guest_yres = y;
  if (guest_textmode) {
    yChar = fheight;
    xChar = fwidth;
  }

  if ((x == dimension_x) && (y == dimension_y) && (bpp == current_bpp))
    return;
  if (fullscreenMode) {
	clear_screen();
	flush();
  }
  dimension_x = x;
  dimension_y = y;

  if ((desktop_y > 0) && ((x > desktop_x) | (y > desktop_y))) {
    BX_ERROR(("dimension_update(): resolution of out of desktop bounds - screen only partly visible"));
  }
  // stretch small simulation window (compatibility code)
  if (!win32_autoscale) {
    stretch_factor = 1;
    if (x < 400) {
      stretch_factor = 2;
    }
  }

  if (bpp != current_bpp) {
    if (bpp == 15) {
      bitmap_info->bmiHeader.biBitCount = 16;
    } else {
      bitmap_info->bmiHeader.biBitCount = bpp;
    }
    if (bpp == 16) {
      bitmap_info->bmiHeader.biCompression = BI_BITFIELDS;
      static RGBQUAD red_mask   = {0x00, 0xF8, 0x00, 0x00};
      static RGBQUAD green_mask = {0xE0, 0x07, 0x00, 0x00};
      static RGBQUAD blue_mask  = {0x1F, 0x00, 0x00, 0x00};
      bitmap_info->bmiColors[256] = bitmap_info->bmiColors[0];
      bitmap_info->bmiColors[257] = bitmap_info->bmiColors[1];
      bitmap_info->bmiColors[258] = bitmap_info->bmiColors[2];
      bitmap_info->bmiColors[0] = red_mask;
      bitmap_info->bmiColors[1] = green_mask;
      bitmap_info->bmiColors[2] = blue_mask;
    } else {
      if (current_bpp == 16) {
        bitmap_info->bmiColors[0] = bitmap_info->bmiColors[256];
        bitmap_info->bmiColors[1] = bitmap_info->bmiColors[257];
        bitmap_info->bmiColors[2] = bitmap_info->bmiColors[258];
      }
      bitmap_info->bmiHeader.biCompression = BI_RGB;
    }
  }
  current_bpp = guest_bpp = bpp;

  resize_main_window(FALSE);

  BX_INFO(("dimension update x=%d y=%d fontheight=%d fontwidth=%d bpp=%d", x, y, fheight, fwidth, bpp));

  host_xres = x;
  host_yres = y;
  host_bpp = bpp;
}


unsigned bx_win32_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
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
  tbab.nID = (UINT_PTR)bx_bitmaps[bx_bitmap_entries].bmap;
  SendMessage(hwndTB, TB_ADDBITMAP, 1, (LPARAM)&tbab);

  bx_bitmap_entries++;
  return(bx_bitmap_entries-1); // return index as handle
}


unsigned bx_win32_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
{
  unsigned hb_index;
  TBBUTTON tbb[1];

  if ((win32_toolbar_entries+1) > BX_MAX_HEADERBAR_ENTRIES)
    terminateEmul(EXIT_HEADER_BITMAP_ERROR);

  hb_index = win32_toolbar_entries++;

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

  win32_toolbar_entry[hb_index].bmap_id = bmap_id;
  win32_toolbar_entry[hb_index].f = f;
  win32_toolbar_entry[hb_index].tooltip = NULL;

  return(hb_index);
}


void bx_win32_gui_c::show_headerbar(void)
{
  if (!IsWindowVisible(hwndTB)) {
    SendMessage(hwndTB, TB_AUTOSIZE, 0, 0);
    ShowWindow(hwndTB, SW_SHOW);
    resize_main_window(FALSE);
    bx_gui->set_tooltip(bx_gui->get_mouse_headerbar_id(), szMouseTooltip);
  }
}


void bx_win32_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
  if (bmap_id != win32_toolbar_entry[hbar_id].bmap_id) {
    win32_toolbar_entry[hbar_id].bmap_id = bmap_id;
    bool is_visible = IsWindowVisible(hwndTB);
    if (is_visible) {
      ShowWindow(hwndTB, SW_HIDE);
    }
    SendMessage(hwndTB, TB_CHANGEBITMAP, (WPARAM)hbar_id+101, (LPARAM)
                MAKELPARAM(bmap_id, 0));
    SendMessage(hwndTB, TB_AUTOSIZE, 0, 0);
    if (is_visible) {
      ShowWindow(hwndTB, SW_SHOW);
    }
  }
}


void bx_win32_gui_c::exit(void)
{
#if BX_DEBUGGER && BX_DEBUGGER_GUI
  if (SIM->has_debug_gui()) {
    close_debug_dialog();
  }
#endif

  // kill thread first...
  PostMessage(stInfo.mainWnd, WM_CLOSE, 0, 0);

  // Wait until it dies
  while ((stInfo.kill == 0) && (workerThreadID != 0)) Sleep(500);

  if (bx_user_quit) {
    terminateEmul(EXIT_NORMAL);
  } else {
    terminateEmul(stInfo.kill);
  }
}


void create_vga_font(void)
{
  unsigned char data[64];

  // VGA font is 8 or 9 wide and up to 32 high
  for (unsigned m = 0; m < 2; m++) {
    for (unsigned c = 0; c<256; c++) {
      vgafont[m][c] = CreateBitmap(9,32,1,1,NULL);
      if (!vgafont[m][c]) terminateEmul(EXIT_FONT_BITMAP_ERROR);
      memset(data, 0, sizeof(data));
      for (unsigned i=0; i<16; i++)
        data[i*2] = reverse_bitorder(bx_vgafont[c].data[i]);
      SetBitmapBits(vgafont[m][c], 64, data);
    }
  }}


COLORREF GetColorRef(Bit8u pal_idx)
{
  return RGB(cmap_index[pal_idx].rgbRed, cmap_index[pal_idx].rgbGreen,
             cmap_index[pal_idx].rgbBlue);
}


void DrawBitmap(HDC hdc, HBITMAP hBitmap, int xStart, int yStart, int width,
                int height, int fcol, int frow, Bit8u fgcolor, Bit8u bgcolor)
{
  BITMAP bm;
  HDC hdcMem;
  POINT ptSize, ptOrg;
  HGDIOBJ oldObj;

  hdcMem = CreateCompatibleDC(hdc);
  SelectObject(hdcMem, hBitmap);
  SetMapMode(hdcMem, GetMapMode (hdc));

  GetObject(hBitmap, sizeof (BITMAP), (LPVOID) &bm);

  ptSize.x = width;
  ptSize.y = height;

  DPtoLP(hdc, &ptSize, 1);

  ptOrg.x = fcol;
  ptOrg.y = frow;
  DPtoLP(hdcMem, &ptOrg, 1);

  oldObj = SelectObject(MemoryDC, MemoryBitmap);

  COLORREF crFore = SetTextColor(MemoryDC, GetColorRef(bgcolor));
  COLORREF crBack = SetBkColor(MemoryDC, GetColorRef(fgcolor));
  if (xChar > 9) {
    StretchBlt(MemoryDC, xStart, yStart, ptSize.x, ptSize.y, hdcMem, ptOrg.x,
              ptOrg.y, ptSize.x / 2, ptSize.y, SRCCOPY);
  } else {
    BitBlt(MemoryDC, xStart, yStart, ptSize.x, ptSize.y, hdcMem, ptOrg.x,
           ptOrg.y, SRCCOPY);
  }
  SetBkColor(MemoryDC, crBack);
  SetTextColor(MemoryDC, crFore);

  SelectObject(MemoryDC, oldObj);

  updateUpdated(xStart, yStart, ptSize.x + xStart - 1, ptSize.y + yStart - 1);

  DeleteDC(hdcMem);
}


void updateUpdated(int x1, int y1, int x2, int y2)
{
  x1 *= stretch_factor;
  y1 *= stretch_factor;
  x2 *= stretch_factor;
  y2 *= stretch_factor;
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


void win32_toolbar_click(int x)
{
  if (x < win32_toolbar_entries) {
    win32_toolbar_entry[x].f();
  }
}

void bx_win32_gui_c::mouse_enabled_changed_specific(bool val)
{
  if ((val != (bool)mouseCaptureMode) && !mouseToggleReq) {
    mouseToggleReq = TRUE;
    mouseCaptureNew = val;
  }
}

void bx_win32_gui_c::get_capabilities(Bit16u *xres, Bit16u *yres, Bit16u *bpp)
{
  if (desktop_y > 0) {
    *xres = desktop_x;
    *yres = desktop_y;
    *bpp = 32;
  } else {
    *xres = 1024;
    *yres = 768;
    *bpp = 32;
  }
}

void bx_win32_gui_c::set_tooltip(unsigned hbar_id, const char *tip)
{
  win32_toolbar_entry[hbar_id].tooltip = tip;
}

void bx_win32_gui_c::set_mouse_mode_absxy(bool mode)
{
  win32MouseModeAbsXY = mode;
}

#if BX_SHOW_IPS
VOID CALLBACK MyTimer(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime)
{
  bx_show_ips_handler();
}

void bx_win32_gui_c::show_ips(Bit32u ips_count)
{
  if (!ipsUpdate) {
    sprintf(ipsText, "IPS: %3.3fM", ips_count / 1000000.0);
    ipsUpdate = TRUE;
  }
}
#endif

#endif /* if BX_WITH_WIN32 */
