//  Copyright (C) 2001  MandrakeSoft S.A.
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

#include "bochs.h"
#include "icon_bochs.h"
#include "font/vga.bitmap.h"
#include <windows.h>
#include <process.h>

#define LOG_THIS bx_gui.

#ifdef __MINGW32__
#if BX_SHOW_IPS
#include <windows32/CommonFunctions.h>
#endif
#endif

#define EXIT_GUI_SHUTDOWN        1
#define EXIT_GMH_FAILURE         2
#define EXIT_FONT_BITMAP_ERROR   3
#define EXIT_NORMAL              4
#define EXIT_HEADER_BITMAP_ERROR 5

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
static unsigned mouse_button_state = 0;
static int ms_xdelta=0, ms_ydelta=0;
static int ms_lastx=0, ms_lasty=0;
static int ms_savedx=0, ms_savedy=0;
static BOOL mouseCaptureMode = FALSE;
static unsigned long workerThread = NULL;
static DWORD workerThreadID = 0;

// Graphics screen stuff
static unsigned x_tilesize = 0, y_tilesize = 0;
static BITMAPINFO* bitmap_info=(BITMAPINFO*)0;
static RGBQUAD* cmap_index;  // indeces into system colormap
static HBITMAP MemoryBitmap = NULL;
static HDC MemoryDC = NULL;
static RECT updated_area;
static BOOL updated_area_valid = FALSE;
static Bit32u *VideoBits;

// Headerbar stuff
unsigned bx_bitmap_entries = 0;
struct {
  HBITMAP bmap;
  unsigned xdim;
  unsigned ydim;
} bx_bitmaps[BX_MAX_PIXMAPS];

static struct {
  HBITMAP bitmap;
  unsigned xdim;
  unsigned ydim;
  unsigned xorigin;
  unsigned yorigin;
  unsigned alignment;
  void (*f)(void);
} bx_headerbar_entry[BX_MAX_HEADERBAR_ENTRIES];

static unsigned bx_headerbar_y = 0;
static unsigned bx_headerbar_entries = 0;
static unsigned bx_bitmap_left_xorigin = 0;   // pixels from left
static unsigned bx_bitmap_right_xorigin = 0;  // pixels from right
static BOOL headerbar_changed = FALSE;

// Misc stuff
static unsigned dimension_x, dimension_y;
static unsigned stretched_x, stretched_y;
static unsigned stretch_factor=1;
static unsigned prev_block_cursor_x = 0;
static unsigned prev_block_cursor_y = 0;
static HBITMAP vgafont[256];
static unsigned x_edge=0, y_edge=0, y_caption=0;

static char szAppName[] = "Bochs for Windows";
static char szWindowName[] = "Bochs for Windows - Display";

typedef struct {
  HINSTANCE hInstance;

  CRITICAL_SECTION drawCS;
  CRITICAL_SECTION keyCS;
  CRITICAL_SECTION mouseCS;

  int kill;  // reason for terminateEmul(int)
  BOOL UIinited;
  HWND hwnd;
} sharedThreadInfo;

sharedThreadInfo stInfo;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
VOID UIThread(PVOID);
void terminateEmul(int);
void create_vga_font(void);
static unsigned char reverse_bitorder(unsigned char);
void DrawBitmap (HDC, HBITMAP, int, int, DWORD, unsigned char cColor);
void updateUpdated(int,int,int,int);
static void headerbar_click(int x);


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
  bx_state=( ( windows_state & MK_LBUTTON) ? 1 : 0 ) + ( ( windows_state & MK_RBUTTON) ? 2 : 0);
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
  // See bx_gui_c::specific_init
  if (x_tilesize != 0) {
    DeleteCriticalSection (&stInfo.drawCS);
    DeleteCriticalSection (&stInfo.keyCS);
    DeleteCriticalSection (&stInfo.mouseCS);
  }

  if (MemoryDC) DeleteDC (MemoryDC);
  if (MemoryBitmap) DeleteObject (MemoryBitmap);

  if ( bitmap_info) delete[] (char*)bitmap_info;

  for (unsigned c=0; c<256; c++)
    if (vgafont[c]) DeleteObject(vgafont[c]);

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
// th: a 'this' pointer to the gui class.  If a function external to the
//     class needs access, store this pointer and use later.
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

void bx_gui_c::specific_init(bx_gui_c *th, int argc, char **argv, unsigned
			     tilewidth, unsigned tileheight,
			     unsigned headerbar_y) {
  th->setprefix("WGUI");
  static RGBQUAD black_quad={ 0, 0, 0, 0};
  stInfo.kill = 0;
  stInfo.UIinited = FALSE;
  InitializeCriticalSection(&stInfo.drawCS);
  InitializeCriticalSection(&stInfo.keyCS);
  InitializeCriticalSection(&stInfo.mouseCS);

  x_tilesize = tilewidth;
  y_tilesize = tileheight;

  stInfo.hInstance = GetModuleHandle(NULL);

  bx_headerbar_y = headerbar_y;
  dimension_x = 640;
  dimension_y = 480 + bx_headerbar_y;
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
  bitmap_info->bmiHeader.biSizeImage=x_tilesize*y_tilesize;
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
}


// This thread controls the GUI window.
VOID UIThread(PVOID pvoid) {
  MSG msg;
  HDC hdc;
  WNDCLASSEX wndclass;
  RECT wndRect;

  workerThreadID = GetCurrentThreadId();

  wndclass.cbSize = sizeof (wndclass);
  wndclass.style = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc = WndProc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hInstance = stInfo.hInstance;
  wndclass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName = szAppName;
  wndclass.hIconSm = LoadIcon (NULL, IDI_APPLICATION);

  RegisterClassEx (&wndclass);

  stInfo.hwnd = CreateWindow (szAppName,
			      szWindowName,
			      WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			      CW_USEDEFAULT,
			      CW_USEDEFAULT,
			      stretched_x + x_edge * 2,
			      stretched_y + y_edge * 2 + y_caption,
			      NULL,
			      NULL,
			      stInfo.hInstance,
			      NULL);

  if (stInfo.hwnd) {
    ShowWindow (stInfo.hwnd, SW_SHOW);
    UpdateWindow (stInfo.hwnd);

    ShowCursor(!mouseCaptureMode);
    GetWindowRect(stInfo.hwnd, &wndRect);
    SetCursorPos(wndRect.left + stretched_x/2 + x_edge,
      wndRect.top + stretched_y/2 + y_edge + y_caption);
    cursorWarped();

    hdc = GetDC(stInfo.hwnd);
    MemoryBitmap = CreateCompatibleBitmap(hdc, dimension_x, dimension_y);
    MemoryDC = CreateCompatibleDC(hdc);
    ReleaseDC(stInfo.hwnd, hdc);

    if (MemoryBitmap && MemoryDC) {
      stInfo.UIinited = TRUE;

      while (GetMessage (&msg, NULL, 0, 0)) {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
      }
    }
  }

  stInfo.kill = EXIT_GUI_SHUTDOWN;

  _endthread();
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
  HDC hdc, hdcMem;
  PAINTSTRUCT ps;
  RECT wndRect;
  POINT ptCursorPos;

  switch (iMsg) {
  case WM_CREATE:
    SetTimer (hwnd, 1, 330, NULL);
    if (mouseCaptureMode)
      SetWindowText(hwnd, "Bochs for Windows      [Press F12 to release mouse capture]");
    else
      SetWindowText(hwnd, "Bochs for Windows      [F12 enables the mouse in Bochs]");
    return 0;

  case WM_TIMER:
    // If mouse escaped, bring it back
    if ( mouseCaptureMode)
    {
      GetCursorPos( &ptCursorPos);
      GetWindowRect(hwnd, &wndRect);
      if ( ptCursorPos.x<wndRect.left || ptCursorPos.x>wndRect.right
        || ptCursorPos.y<( wndRect.top+BX_HEADER_BAR_Y*stretch_factor) || ptCursorPos.y>wndRect.bottom)
      {
        SetCursorPos(wndRect.left + stretched_x/2 + x_edge,
                   wndRect.top + stretched_y/2 + y_edge + y_caption);
        cursorWarped();
      }
    }
    
    return 0;

  case WM_PAINT:
    EnterCriticalSection(&stInfo.drawCS);
    hdc = BeginPaint (hwnd, &ps);

    hdcMem = CreateCompatibleDC (hdc);
    SelectObject (hdcMem, MemoryBitmap);

    StretchBlt(hdc, ps.rcPaint.left, ps.rcPaint.top,
           ps.rcPaint.right - ps.rcPaint.left + 1,
           ps.rcPaint.bottom - ps.rcPaint.top + 1, hdcMem,
           ps.rcPaint.left/stretch_factor, ps.rcPaint.top/stretch_factor,
            (ps.rcPaint.right - ps.rcPaint.left+1)/stretch_factor, (ps.rcPaint.bottom - ps.rcPaint.top+1)/stretch_factor,SRCCOPY);

    DeleteDC (hdcMem);
    EndPaint (hwnd, &ps);
    LeaveCriticalSection(&stInfo.drawCS);
    return 0;

  case WM_MOUSEMOVE:
    processMouseXY( LOWORD(lParam), HIWORD(lParam), wParam, 0);
    return 0;

  case WM_LBUTTONDOWN:
  case WM_LBUTTONDBLCLK:
    if (HIWORD(lParam) < BX_HEADER_BAR_Y * stretch_factor) {
      EnterCriticalSection(&stInfo.keyCS);
      enq_key_event(LOWORD(lParam)/stretch_factor, HEADERBAR_CLICKED);
      LeaveCriticalSection(&stInfo.keyCS);
    }
    processMouseXY( LOWORD(lParam), HIWORD(lParam), wParam, 1);

    return 0;

  case WM_LBUTTONUP:
    processMouseXY( LOWORD(lParam), HIWORD(lParam), wParam, 1);
    return 0;

  case WM_RBUTTONDOWN:
  case WM_RBUTTONDBLCLK:
  case WM_RBUTTONUP:
    processMouseXY( LOWORD(lParam), HIWORD(lParam), wParam, 2);

  case WM_CLOSE:
    return DefWindowProc (hwnd, iMsg, wParam, lParam);

  case WM_DESTROY:
    KillTimer (hwnd, 1);
    stInfo.UIinited = FALSE;

    PostQuitMessage (0);
    return 0;

  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
    if (wParam == VK_F12) {
      mouseCaptureMode = !mouseCaptureMode;
      ShowCursor(!mouseCaptureMode);
      ShowCursor(!mouseCaptureMode);   // somehow one didn't do the trick (win98)
      GetWindowRect(hwnd, &wndRect);
      SetCursorPos(wndRect.left + stretched_x/2 + x_edge,
                   wndRect.top + stretched_y/2 + y_edge + y_caption);
      cursorWarped();
      if (mouseCaptureMode)
	SetWindowText(hwnd, "Bochs for Windows      [Press F12 to release mouse capture]");
      else
	SetWindowText(hwnd, "Bochs for Windows      [F12 enables the mouse in Bochs]");
    } else {
      EnterCriticalSection(&stInfo.keyCS);
      enq_key_event(HIWORD (lParam) & 0xFF, BX_KEY_PRESSED);
      LeaveCriticalSection(&stInfo.keyCS);
    }
    return 0;

  case WM_KEYUP:
  case WM_SYSKEYUP:
    EnterCriticalSection(&stInfo.keyCS);
    enq_key_event(HIWORD (lParam) & 0xFF, BX_KEY_RELEASED);
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


void enq_key_event(Bit32u key, Bit32u press_release) {	
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

void bx_gui_c::handle_events(void) {
  Bit32u key;
  unsigned char scancode;

  // printf("# Hey!!!\n");

  if (stInfo.kill) terminateEmul(stInfo.kill);

  // Handle mouse moves
  enq_mouse_event();

  // Handle keyboard and mouse clicks
  EnterCriticalSection(&stInfo.keyCS);
  while (head != tail) {
    QueueEvent* queue_event=deq_key_event();
    if ( ! queue_event)
      break;
    // Bypass bx_devices.keyboard->gen_scancode so we may enter
    //  a scancode directly
    // bx_devices.keyboard->gen_scancode(deq_key_event());
    key = queue_event->key_event;
    if ( key==MOUSE_MOTION)
    {
      bx_devices.keyboard->mouse_motion( queue_event->mouse_x,
        queue_event->mouse_y, queue_event->mouse_button_state);
    }
    // Check for mouse buttons first
    else if ( key & MOUSE_PRESSED) {
      // printf("# click!\n");
      bx_devices.keyboard->mouse_motion( 0, 0, LOWORD(key));
    }
    else if (key & HEADERBAR_CLICKED) {
      headerbar_click(LOWORD(key));
    }
    else {
      // Its a key
      scancode = LOBYTE(LOWORD(key));
      // printf("# key = %d, scancode = %d\n",key,scancode);
      if (key & BX_KEY_RELEASED) scancode |= 0x80;
      bx_devices.keyboard->put_scancode(&scancode, 1);
    }
  }
  LeaveCriticalSection(&stInfo.keyCS);
}


// ::FLUSH()
//
// Called periodically, requesting that the gui code flush all pending
// screen update requests.

void bx_gui_c::flush(void) {
  EnterCriticalSection( &stInfo.drawCS);
  if (updated_area_valid) {
    // slight bugfix
	updated_area.right++;
	updated_area.bottom++;
	InvalidateRect( stInfo.hwnd, &updated_area, FALSE);
	updated_area_valid = FALSE;
  }
  LeaveCriticalSection( &stInfo.drawCS);
}


// ::CLEAR_SCREEN()
//
// Called to request that the VGA region is cleared.  Don't
// clear the area that defines the headerbar.

void bx_gui_c::clear_screen(void) {
  HGDIOBJ oldObj;

  if (!stInfo.UIinited) return;

  EnterCriticalSection(&stInfo.drawCS);

  oldObj = SelectObject(MemoryDC, MemoryBitmap);
  PatBlt(MemoryDC, 0, bx_headerbar_y, stretched_x, stretched_y -
         bx_headerbar_y, BLACKNESS);
  SelectObject(MemoryDC, oldObj);

  updateUpdated(0, bx_headerbar_y, dimension_x-1, dimension_y-1);

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
// format of old_text & new_text: each is 80*nrows*2 bytes long.
//     This represents 80 characters wide by 'nrows' high, with
//     each character being 2 bytes.  The first by is the
//     character value, the second is the attribute byte.
//     I currently don't handle the attribute byte.
//
// cursor_x: new x location of cursor
// cursor_y: new y location of cursor

void bx_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
			   unsigned long cursor_x, unsigned long cursor_y,
                           unsigned nrows) {
  HDC hdc;
  unsigned char cChar;
  unsigned i, x, y;
  unsigned nchars;

  if (!stInfo.UIinited) return;
	
  EnterCriticalSection(&stInfo.drawCS);

  hdc = GetDC(stInfo.hwnd);

  // Number of characters on screen, variable number of rows
  nchars = 80*nrows;
	
  if ( (prev_block_cursor_y*80 + prev_block_cursor_x) < nchars) {
    cChar = new_text[(prev_block_cursor_y*80 + prev_block_cursor_x)*2];
    DrawBitmap(hdc, vgafont[cChar], prev_block_cursor_x*8,
	              prev_block_cursor_y*16 + bx_headerbar_y, SRCCOPY, 
				  new_text[((prev_block_cursor_y*80 + prev_block_cursor_x)*2)+1]);
  }

  for (i=0; i<nchars*2; i+=2) {
    if ((old_text[i] != new_text[i]) ||
	(old_text[i+1] != new_text[i+1])) {

      cChar = new_text[i];

      x = (i/2) % 80;
      y = (i/2) / 80;

      DrawBitmap(hdc, vgafont[cChar], x*8, y*16 + bx_headerbar_y, SRCCOPY, new_text[i+1]);
    }
  }

  prev_block_cursor_x = cursor_x;
  prev_block_cursor_y = cursor_y;

  // now draw character at new block cursor location in reverse
  if ((cursor_y*80 + cursor_x) < nchars ) {
    cChar = new_text[(cursor_y*80 + cursor_x)*2];
	//reverse background and foreground colors
	char cAttr = new_text[((cursor_y*80 + cursor_x)*2)+1];
	cAttr = ((cAttr>>4)&0xf)+((cAttr&0xf)<<4);
    DrawBitmap(hdc, vgafont[cChar], cursor_x*8, cursor_y*16 + bx_headerbar_y,
	           SRCCOPY, cAttr);
  }

  ReleaseDC(stInfo.hwnd, hdc);

  LeaveCriticalSection(&stInfo.drawCS);
}


// ::PALETTE_CHANGE()
//
// Allocate a color in the native GUI, for this color, and put
// it in the colormap location 'index'.
// returns: 0=no screen update needed (color map change has direct effect)
//          1=screen updated needed (redraw using current colormap)

Boolean bx_gui_c::palette_change(unsigned index, unsigned red,
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

void bx_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0) {
  HDC hdc;
  HGDIOBJ oldObj;
  unsigned true_y0;

  true_y0 = y0 + bx_headerbar_y;

  EnterCriticalSection(&stInfo.drawCS);
  hdc = GetDC(stInfo.hwnd);

  oldObj = SelectObject(MemoryDC, MemoryBitmap);

  StretchDIBits( MemoryDC, x0, true_y0, x_tilesize, y_tilesize, 0, 0,
    x_tilesize, y_tilesize, tile, bitmap_info, DIB_RGB_COLORS, SRCCOPY);

  SelectObject(MemoryDC, oldObj);

  updateUpdated(x0, true_y0, x0 + x_tilesize - 1, true_y0 + y_tilesize - 1);

  ReleaseDC(stInfo.hwnd, hdc);
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

void bx_gui_c::dimension_update(unsigned x, unsigned y) {
if ( x==dimension_x && y+bx_headerbar_y==dimension_y)
    return;
  dimension_x = x;
  dimension_y = y + bx_headerbar_y;
  stretched_x = dimension_x;
  stretched_y = dimension_y;
  stretch_factor = 1;
  while ( stretched_x<400)
  {
    stretched_x *= 2;
    stretched_y *= 2;
    stretch_factor *= 2;
  }
  
  SetWindowPos(stInfo.hwnd, HWND_TOP, 0, 0, stretched_x + x_edge * 2,
              stretched_y+ y_edge * 2 + y_caption,
               SWP_NOMOVE | SWP_NOZORDER);
  headerbar_changed = TRUE;
  show_headerbar();
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

unsigned bx_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim,
				 unsigned ydim) {
  unsigned char *data;

  if (bx_bitmap_entries >= BX_MAX_PIXMAPS)
    terminateEmul(EXIT_HEADER_BITMAP_ERROR);

  bx_bitmaps[bx_bitmap_entries].bmap = CreateBitmap(xdim,ydim,1,1,NULL);
  if (!bx_bitmaps[bx_bitmap_entries].bmap)
    terminateEmul(EXIT_HEADER_BITMAP_ERROR);

  data = new unsigned char[ydim * xdim/8];
  for (unsigned i=0; i<ydim * xdim/8; i++)
    data[i] = reverse_bitorder(bmap[i]);
  SetBitmapBits(bx_bitmaps[bx_bitmap_entries].bmap, ydim * xdim/8, data);
  free(data);

  bx_bitmaps[bx_bitmap_entries].xdim = xdim;
  bx_bitmaps[bx_bitmap_entries].ydim = ydim;

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

unsigned bx_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment,
				    void (*f)(void)) {
  unsigned hb_index;

  if ( (bx_headerbar_entries+1) > BX_MAX_HEADERBAR_ENTRIES )
    terminateEmul(EXIT_HEADER_BITMAP_ERROR);

  bx_headerbar_entries++;
  hb_index = bx_headerbar_entries - 1;

  bx_headerbar_entry[hb_index].bitmap = bx_bitmaps[bmap_id].bmap;
  bx_headerbar_entry[hb_index].xdim   = bx_bitmaps[bmap_id].xdim;
  bx_headerbar_entry[hb_index].ydim   = bx_bitmaps[bmap_id].ydim;
  bx_headerbar_entry[hb_index].alignment = alignment;
  bx_headerbar_entry[hb_index].f = f;
  if (alignment == BX_GRAVITY_LEFT) {
    bx_headerbar_entry[hb_index].xorigin = bx_bitmap_left_xorigin;
    bx_headerbar_entry[hb_index].yorigin = 0;
    bx_bitmap_left_xorigin += bx_bitmaps[bmap_id].xdim;
  } else { // BX_GRAVITY_RIGHT
    bx_bitmap_right_xorigin += bx_bitmaps[bmap_id].xdim;
    bx_headerbar_entry[hb_index].xorigin = bx_bitmap_right_xorigin;
    bx_headerbar_entry[hb_index].yorigin = 0;
  }

  headerbar_changed = TRUE;

  return(hb_index);
}


// ::SHOW_HEADERBAR()
//
// Show (redraw) the current headerbar, which is composed of
// currently installed bitmaps.

void bx_gui_c::show_headerbar(void) {
  unsigned xorigin;
  HGDIOBJ oldObj;

  if (!headerbar_changed || !stInfo.UIinited) return;
	
  EnterCriticalSection(&stInfo.drawCS);
	
  oldObj = SelectObject(MemoryDC, MemoryBitmap);
  PatBlt (MemoryDC, 0, 0, dimension_x, bx_headerbar_y, BLACKNESS);
	
  for (unsigned i=0; i<bx_headerbar_entries; i++) {
    if (bx_headerbar_entry[i].alignment == BX_GRAVITY_LEFT)
      xorigin = bx_headerbar_entry[i].xorigin;
    else
      xorigin = dimension_x - bx_headerbar_entry[i].xorigin;
    DrawBitmap(MemoryDC, bx_headerbar_entry[i].bitmap, xorigin, 0, SRCCOPY, 0x7);
  }

  SelectObject(MemoryDC, oldObj);

  updateUpdated(0, 0, dimension_x-1, bx_headerbar_y-1);

  LeaveCriticalSection(&stInfo.drawCS);

  headerbar_changed = FALSE;
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

void bx_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id) {
  headerbar_changed = TRUE;
  bx_headerbar_entry[hbar_id].bitmap = bx_bitmaps[bmap_id].bmap;
  show_headerbar();
}


// ::EXIT()
//
// Called before bochs terminates, to allow for a graceful
// exit from the native GUI mechanism.
void bx_gui_c::exit(void) {
  printf("# In bx_gui_c::exit(void)!\n");

  // kill thread first...
  PostMessage(stInfo.hwnd, WM_CLOSE, 0, 0);

  // Wait until it dies
  while ((stInfo.kill == 0) && (workerThreadID != 0)) Sleep(500);

  if (!stInfo.kill) terminateEmul(EXIT_NORMAL);
}


void create_vga_font(void) {
  HDC hdc;
  unsigned char data[32];

  // VGA font is 8wide x 16high
  hdc = GetDC(stInfo.hwnd);
  for (unsigned c = 0; c<256; c++) {
    vgafont[c] = CreateBitmap(8,16,1,1,NULL);
    if (!vgafont[c]) terminateEmul(EXIT_FONT_BITMAP_ERROR);
    memset(data, 0, 16);
    for (unsigned i=0; i<16; i++)
      data[i*2] = reverse_bitorder(bx_vgafont[c].data[i]);
    SetBitmapBits(vgafont[c], 32, data);
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


void DrawBitmap (HDC hdc, HBITMAP hBitmap, int xStart, int yStart,
		 DWORD dwRop, unsigned char cColor) {
  BITMAP bm;
  HDC hdcMem;
  POINT ptSize, ptOrg;
  HGDIOBJ oldObj;

  hdcMem = CreateCompatibleDC (hdc);
  SelectObject (hdcMem, hBitmap);
  SetMapMode (hdcMem, GetMapMode (hdc));

  GetObject (hBitmap, sizeof (BITMAP), (LPVOID) &bm);

  ptSize.x = bm.bmWidth;
  ptSize.y = bm.bmHeight;

  DPtoLP (hdc, &ptSize, 1);

  ptOrg.x = 0;
  ptOrg.y = 0;
  DPtoLP (hdcMem, &ptOrg, 1);

  // BitBlt (hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, ptOrg.x,
  // 	    ptOrg.y, dwRop);

  oldObj = SelectObject(MemoryDC, MemoryBitmap);
//	BitBlt(MemoryDC, xStart, yStart, ptSize.x, ptSize.y, hdcMem, ptOrg.x,
//		  ptOrg.y, dwRop);
//Colors taken from Ralf Browns interrupt list.
//(0=black, 1=blue, 2=red, 3=purple, 4=green, 5=cyan, 6=yellow, 7=white)	  
//The highest background bit usually means blinking characters. No idea
//how to implement that so for now it's just implemented as color.
//Note: it is also possible to program the VGA controller to have the
//high bit for the foreground color enable blinking characters.
	const COLORREF crPal[16] = { 
									RGB(0 ,0 ,0 ),	//0 black 
									RGB(0 ,0 ,0x80 ),	//1 dark blue 
									RGB(0 ,0x80 ,0 ),	//2 dark green 
									RGB(0 ,0x80 ,0x80 ),		//3 dark cyan 
									RGB(0x80 ,0 ,0 ),	//4 dark red 
									RGB(0x80 ,0 ,0x80 ),		//5 dark magenta 
									RGB(0x80 ,0x80 ,0 ),		//6 brown
									RGB(0xC0 ,0xC0 ,0xC0 ),	//7 light gray 
									RGB(0x80 ,0x80 ,0x80 ),	//8 dark gray 
									RGB(0 ,0 ,0xFF ),	//9 light blue 
									RGB(0 ,0xFF ,0 ),	//10 green 
									RGB(0 ,0xFF ,0xFF ),		//11 cyan 
									RGB(0xFF ,0 ,0 ),	//12 light red 
									RGB(0xFF ,0 ,0xFF ),		//13 magenta 
									RGB(0xFF ,0xFF ,0 ),		//14 yellow 
									RGB(0xFF ,0xFF ,0xFF )};	//15 white 

	COLORREF crFore = SetTextColor(MemoryDC, crPal[(cColor>>4)&0xf]);
	COLORREF crBack = SetBkColor(MemoryDC, crPal[cColor&0xf]);
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
  y1*=stretch_factor;
  x2*=stretch_factor;
  y2*=stretch_factor;
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


void headerbar_click(int x) {
  int xorigin;

  for (unsigned i=0; i<bx_headerbar_entries; i++) {
    if (bx_headerbar_entry[i].alignment == BX_GRAVITY_LEFT)
      xorigin = bx_headerbar_entry[i].xorigin;
    else
      xorigin = dimension_x - bx_headerbar_entry[i].xorigin;
    if ( (x>=xorigin) && (x<(xorigin+int(bx_headerbar_entry[i].xdim))) ) {
      bx_headerbar_entry[i].f();
      return;
    }
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
  SetTimer(stInfo.hwnd,idTimer,time*1000,MyTimer);
}
#endif
#endif
