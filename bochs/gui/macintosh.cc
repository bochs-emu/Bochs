/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2013  The Bochs Project
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

// macintosh.cc -- bochs GUI file for the Macintosh
// written by David Batterham <drbatter@progsoc.uts.edu.au>
// with contributions from Tim Senecal

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

// BOCHS INCLUDES
#include <MacTypes.h>

#include "param_names.h"
#include "bochs.h"
#include "iodev.h"

// decide whether to enable this file or not
#if BX_WITH_MACOS

#include "icon_bochs.h"
#include "font/vga.bitmap.h"

// MAC OS INCLUDES
#undef ACCESSOR_CALLS_ARE_FUNCTIONS
#define ACCESSOR_CALLS_ARE_FUNCTIONS 1
#include <Quickdraw.h>
#include <QuickdrawText.h>
#include <QDOffscreen.h>
#include <Icons.h>
#include <ImageCompression.h>
#include <Palettes.h>
#include <Windows.h>
#include <Memory.h>
#include <Events.h>
#include <TextUtils.h>
#include <ToolUtils.h>
#include <Dialogs.h>
#include <LowMem.h>
#include <Disks.h>
#include <CursorDevices.h>
#include <Menus.h>
#include <Sound.h>
#include <SIOUX.h>
#include <Devices.h>

// CONSTANTS

#define rMBarID    128
#define mApple     128
#define iAbout       1
#define mFile      129
#define iQuit        1
#define mEdit      130
#define mBochs     131
#define iFloppy      1
#define iCursor      3
#define iTool        4
#define iMenuBar     5
#define iFullScreen  6
#define iConsole     7
#define iSnapshot    9
#define iReset      10

#define SLEEP_TIME  0 // Number of ticks to surrender the processor during a WaitNextEvent()
// Change this to 15 or higher if you don't want Bochs to hog the processor!

#define FONT_WIDTH    8
#define FONT_HEIGHT  16

#define WINBITMAP(w)  (((GrafPtr)(w))->portBits)

#define ASCII_1_MASK  0x00FF0000
#define ASCII_2_MASK  0x000000FF

const RGBColor  black   = {0, 0, 0};
const RGBColor  white   = {0xFFFF, 0xFFFF, 0xFFFF};
const RGBColor  medGrey = {0xCCCC, 0xCCCC, 0xCCCC};
const RGBColor  ltGrey  = {0xEEEE, 0xEEEE, 0xEEEE};

class bx_macintosh_gui_c : public bx_gui_c {
public:
  bx_macintosh_gui_c (void) {}
  DECLARE_GUI_VIRTUAL_METHODS()
};

// declare one instance of the gui object and call macro to insert the
// plugin code
static bx_macintosh_gui_c *theGui = NULL;
IMPLEMENT_GUI_PLUGIN_CODE(macintosh)

#define LOG_THIS theGui->

// GLOBALS

WindowPtr            win, toolwin, fullwin, backdrop, hidden, SouixWin;
SInt16               gOldMBarHeight;
bx_bool              menubarVisible = true, cursorVisible = true;
RgnHandle            mBarRgn, cnrRgn;
unsigned             mouse_button_state = 0;
CTabHandle           gCTable;
PixMapHandle         gTile;
BitMap               *vgafont[256];
Rect                 srcTextRect, srcTileRect;
Point                scrCenter = {320, 240};
Ptr                  KCHR;
short                gheaderbar_y;
Point                prevPt;
unsigned             width, height, gMinTop, gMaxTop, gLeft;
GWorldPtr            gOffWorld;
Ptr                  gMyBuffer;
static unsigned      disp_bpp=8;
static EventModifiers oldMods = 0;
static unsigned int text_rows=25, text_cols=80;

// HEADERBAR STUFF
int             numPixMaps = 0, toolPixMaps = 0;
unsigned        bx_bitmap_left_xorigin  = 2; // pixels from left
unsigned        bx_bitmap_right_xorigin = 2; // pixels from right
//PixMapHandle  bx_pixmap[BX_MAX_PIXMAPS];
CIconHandle     bx_cicn[BX_MAX_PIXMAPS];

struct {
        CIconHandle cicn;
//      PixMapHandle pm;
        unsigned xdim;
        unsigned ydim;
        unsigned xorigin;
        unsigned yorigin;
        unsigned alignment;
        void (*f)(void);
} bx_tool_pixmap[BX_MAX_PIXMAPS];

// Event handlers
BX_CPP_INLINE void HandleKey(EventRecord *event, Bit32u keyState);
BX_CPP_INLINE void HandleToolClick(Point where);
void HandleMenuChoice(long menuChoice);
BX_CPP_INLINE void HandleClick(EventRecord *event);

// Update routines
void UpdateWindow(WindowPtr window);
void UpdateRgn(RgnHandle rgn);

// Show/hide UI elements
void HidePointer(void);
void ShowPointer(void);
void HideTools(void);
void ShowTools(void);
void HideMenubar(void);
void ShowMenubar(void);
void HideConsole(void);
void ShowConsole(void);

// Initialisation
void FixWindow(void);
void MacPanic(void);
void InitToolbox(void);
void CreateTile(void);
void CreateMenus(void);
void CreateWindows(void);
void CreateKeyMap(void);
void CreateVGAFont(void);
BitMap *CreateBitMap(unsigned width,  unsigned height);
PixMapHandle CreatePixMap(unsigned left, unsigned top, unsigned width,
        unsigned height, unsigned depth, CTabHandle clut);
unsigned char reverse_bitorder(unsigned char);

//this routine moves the initial window position so that it is entirely onscreen
//it is needed for os 8.x with appearance managaer
void FixWindow(void)
{
  RgnHandle wStruct;
  Region *wRgn;
  CWindowRecord *thing;
  Rect wRect;
  RgnHandle tStruct;
  Region *tRgn;
  Rect tRect;
  short MinVal;

  thing = (CWindowRecord *)win;
  wStruct = thing->strucRgn;
  wRgn = (Region *)*wStruct;
  wRect = wRgn->rgnBBox;

  thing = (CWindowRecord *)toolwin;
  tStruct = thing->strucRgn;
  tRgn = (Region *)*tStruct;
  tRect = tRgn->rgnBBox;

  if (wRect.left < 2)
  {
    gLeft = gLeft + (2 - wRect.left);
  }

  MinVal = tRect.bottom+2;
//MinVal = MinVal + GetMBarHeight();

  if (wRect.top < MinVal)
  {
//  gMinTop = gMinTop + (MinVal - wRect.top);
    gMaxTop = gMaxTop + (MinVal - wRect.top);
  }

  MoveWindow(win, gLeft, gMaxTop, false);
}

void MacPanic(void)
{
  StopAlert(200, NULL);
}

void InitToolbox(void)
{
  InitGraf(&qd.thePort);
  InitWindows();
  InitMenus();
  InitDialogs(nil);
  InitCursor();
  MaxApplZone();
  // Initialise the toolbox
}

void CreateTile(void)
{
  GDHandle  saveDevice;
  CGrafPtr  savePort;
  OSErr     err;
  unsigned  long p_f;
  long      theRowBytes = ((((long) (disp_bpp==24?32:(((disp_bpp+1)>>1)<<1)) * ((long) (srcTileRect.right-srcTileRect.left)) + 31) >> 5) << 2);

//if (SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->get())
//{

  GetGWorld(&savePort, &saveDevice);
  switch (disp_bpp) {
      case 1:
        p_f = k1MonochromePixelFormat;
        break;
      case 2:
        p_f = k2IndexedPixelFormat;
        break;
      case 4:
        p_f = k4IndexedPixelFormat;
        break;
      case 8:
        p_f = k8IndexedPixelFormat;
        break;
      case 15:
        p_f = k16LE555PixelFormat;
        break;
      case 16:
        p_f = k16LE565PixelFormat;
        break;
      case 24:
        //p_f = k24BGRPixelFormat;
        //break;
      case 32:
        p_f = k32ARGBPixelFormat;//k32BGRAPixelFormat;
       break;
  }

  BX_ASSERT((gMyBuffer = (Ptr)malloc(theRowBytes * (srcTileRect.bottom - srcTileRect.top))) != NULL);
  err = QTNewGWorldFromPtr(&gOffWorld, p_f,
      &srcTileRect, disp_bpp>8 ? NULL : gCTable, NULL, keepLocal, gMyBuffer, theRowBytes);
  if (err != noErr || gOffWorld == NULL)
    BX_PANIC(("mac: can't create gOffWorld; err=%hd", err));

  SetGWorld(gOffWorld, NULL);
  RGBForeColor(&black);
  RGBBackColor(&white);

  gTile = GetGWorldPixMap(gOffWorld);

  if (gTile != NULL)
  {
    NoPurgePixels(gTile);
    if (!LockPixels(gTile))
      BX_ERROR(("mac: can't LockPixels gTile"));
    if ((**gTile).pixelType != RGBDirect && (**gTile).pmTable != gCTable)
    {
      DisposeCTable(gCTable);
      gCTable = (**gTile).pmTable;
    }

    (**gCTable).ctFlags |= 0x4000;   //use palette manager indexes
    CTabChanged(gCTable);
  }
  else
    BX_PANIC(("mac: can't create gTile"));

  SetGWorld(savePort, saveDevice);

/*}
  else
  {
    gOffWorld = NULL;
    gTile = CreatePixMap(0, 0, srcTileRect.right, srcTileRect.bottom, 8, gCTable);
    if (gTile == NULL)
      BX_PANIC(("mac: can't create gTile"));
}*/
}

void CreateMenus(void)
{
  Handle menu;

  menu = GetNewMBar(rMBarID);   // get our menus from resource
  if (menu != nil)
  {
    SetMenuBar(menu);
    DisposeHandle(menu);
    AppendResMenu(GetMenuHandle(mApple), 'DRVR');          // add apple menu items
    DrawMenuBar();
  }
  else
    BX_PANIC(("can't create menu"));
}

void CreateWindows(void)
{
  int l, t, r, b;
  Rect winRect;

  SetRect(&winRect, 0, 0, qd.screenBits.bounds.right, qd.screenBits.bounds.bottom);
  backdrop = NewWindow(NULL, &winRect, "\p", false, plainDBox, (WindowPtr)-1, false, 0);

  width = 640;
  height = 480;
  gLeft = 4;
  gMinTop = 44;
  gMaxTop = 44 + gheaderbar_y;

  l = (qd.screenBits.bounds.right - width)/2;
  r = l + width;
  t = (qd.screenBits.bounds.bottom - height)/2;
  b = t + height;

  SetRect(&winRect, 0, 20, qd.screenBits.bounds.right, 22+gheaderbar_y);
  toolwin = NewCWindow(NULL, &winRect, "\pMacBochs 586", true, floatProc,
    (WindowPtr)-1, false, 0);
  if (toolwin == NULL)
    BX_PANIC(("mac: can't create tool window"));
  // Create a moveable tool window for the "headerbar"

  SetRect(&winRect, l, t, r, b);
  fullwin = NewCWindow(NULL, &winRect, "\p", false, plainDBox, (WindowPtr)-1, false, 1);

  SetRect(&winRect, gLeft, gMaxTop, gLeft+width, gMaxTop+height);
  win = NewCWindow(NULL, &winRect, "\pMacBochs 586", true, documentProc,
    (WindowPtr)-1, true, 1);
  if (win == NULL)
    BX_PANIC(("mac: can't create emulator window"));

  FixWindow();

  hidden = fullwin;

  HiliteWindow(win, true);

  SetPort(win);
}

// ::SPECIFIC_INIT()
//
// Called from gui.cc, once upon program startup, to allow for the
// specific GUI code (X11, Win32, ...) to be initialized.
//
// argc, argv: not used right now, but the intention is to pass native GUI
//     specific options from the command line.  (X11 options, Win32 options,...)
//
// headerbar_y:  A headerbar (toolbar) is display on the top of the
//     VGA window, showing floppy status, and other information.  It
//     always assumes the width of the current VGA mode width, but
//     it's height is defined by this parameter.

void bx_macintosh_gui_c::specific_init(int argc, char **argv, unsigned headerbar_y)
{
  put("MACGUI");
  InitToolbox();

  //SouixWin = FrontWindow();

  atexit(MacPanic);

  gheaderbar_y = headerbar_y;

  CreateKeyMap();

  gCTable = GetCTable(128);
  BX_ASSERT (gCTable != NULL);
  CTabChanged(gCTable); //(*gCTable)->ctSeed = GetCTSeed();
  SetRect(&srcTextRect, 0, 0, FONT_WIDTH, FONT_HEIGHT);
  SetRect(&srcTileRect, 0, 0, x_tilesize, y_tilesize);

  CreateMenus();
  CreateVGAFont();
  CreateTile();
  CreateWindows();

  GetMouse(&prevPt);
  SetEventMask(everyEvent);

  SIOUXSettings.setupmenus = false;
  SIOUXSettings.autocloseonquit = true;
  SIOUXSettings.asktosaveonclose = false;
  SIOUXSettings.standalone = false;

  UNUSED(argc);
  UNUSED(argv);

  //HideWindow(SouixWin);
}

// HandleKey()
//
// Handles keyboard-related events.

BX_CPP_INLINE void HandleKey(EventRecord *event, Bit32u keyState)
{
  UInt32    key;
  UInt32    trans;
  static UInt32 transState = 0;

  key = event->message & charCodeMask;

  if (event->modifiers & cmdKey)
  {
    HandleMenuChoice(MenuKey(key));
  }
//else if (FrontWindow() == SouixWin)
//{
//  SIOUXHandleOneEvent(event);
//}
  else
  {
/*  if (event->modifiers & shiftKey)
      DEV_kbd_gen_scancode(BX_KEY_SHIFT_L | keyState);
    if (event->modifiers & controlKey)
      DEV_kbd_gen_scancode(BX_KEY_CTRL_L | keyState);
    if (event->modifiers & optionKey)
      DEV_kbd_gen_scancode(BX_KEY_ALT_L | keyState);*/

    key = (event->message & keyCodeMask) >> 8;

    trans = KeyTranslate(KCHR, key, &transState);
    if ((trans == BX_KEY_PRINT) && ((oldMods & optionKey) || (oldMods & rightOptionKey)))
      trans = BX_KEY_ALT_SYSREQ;
    if ((trans == BX_KEY_PAUSE) && ((oldMods & controlKey) || (oldMods & rightControlKey)))
      trans = BX_KEY_CTRL_BREAK;

    // KeyTranslate maps Mac virtual key codes to any type of character code
    // you like (in this case, Bochs key codes). Much nicer than a huge switch
    // statement!

    if (trans > 0)
      DEV_kbd_gen_scancode(trans | keyState);

/*  if (event->modifiers & shiftKey)
      DEV_kbd_gen_scancode(BX_KEY_SHIFT_L | BX_KEY_RELEASED);
    if (event->modifiers & controlKey)
      DEV_kbd_gen_scancode(BX_KEY_CTRL_L | BX_KEY_RELEASED);
    if (event->modifiers & optionKey)
      DEV_kbd_gen_scancode(BX_KEY_ALT_L | BX_KEY_RELEASED);*/
  }
}

// HandleToolClick()
//
// Handles mouse clicks in the Bochs tool window

BX_CPP_INLINE void HandleToolClick(Point where)
{
  unsigned i;
  int xorigin;
  Rect bounds;

  SetPort(toolwin);
  GlobalToLocal(&where);
  for (i=0; i<toolPixMaps; i++)
  {
    if (bx_tool_pixmap[i].alignment == BX_GRAVITY_LEFT)
      xorigin = bx_tool_pixmap[i].xorigin;
    else
      xorigin = toolwin->portRect.right - bx_tool_pixmap[i].xorigin;
    SetRect(&bounds, xorigin, 0, xorigin+32, 32);
    if (PtInRect(where, &bounds))
      bx_tool_pixmap[i].f();
  }
  theGui->show_headerbar();
}

BX_CPP_INLINE void ResetPointer(void)
{
#if 0
  CursorDevice *theMouse;
  if (true)
  {
    theMouse = NULL;
    CrsrDevNextDevice(&theMouse);
    CrsrDevMoveTo(theMouse, (long)scrCenter.h, (long)scrCenter.v);
  }
#endif

#define MouseCur 0x082C
#define MouseTemp 0x0828
#define MouseNew 0x08CE
#define MouseAttached 0x08CF

  *(Point *)MouseCur = scrCenter;
  *(Point *)MouseTemp = scrCenter;
  *(Ptr)MouseNew = *(Ptr)MouseAttached;
  //*(char *)CrsrNew = 0xFF;
}

// HandleClick()
//
// Handles mouse click events.


void HandleMenuChoice(long menuChoice)
{
  OSErr           err = noErr;
  short           item, menu, i;
  short           daRefNum;
  Rect            bounds;
  Str255          daName;
  WindowRef       newWindow;
  DialogPtr       theDlog;

  item = LoWord(menuChoice);
  menu = HiWord(menuChoice);

  switch(menu) {
  case mApple:
    switch(item) {
    case iAbout:
      theDlog = GetNewDialog(128, NULL, (WindowPtr)-1);
      ModalDialog(NULL, &i);
      DisposeDialog(theDlog);
      break;

    default:
      GetMenuItemText(GetMenuHandle(mApple), item, daName);
      daRefNum = OpenDeskAcc(daName);
      break;
    }
    break;

  case mFile:
    switch(item) {
    case iQuit:
      BX_PANIC(("User terminated"));
      break;

    default:
      break;
    }
    break;

  case mBochs:
    switch(item) {
    case iFloppy:
      DiskEject(1);
      break;

    case iCursor:
      if (cursorVisible)
        HidePointer();
      else
        ShowPointer();
      break;

    case iTool:
      if (IsWindowVisible(toolwin))
        HideTools();
      else
        ShowTools();
      break;

    case iMenuBar:
      if (menubarVisible)
        HideMenubar();
      else
        ShowMenubar();
      break;

    case iFullScreen:
      if (cursorVisible || IsWindowVisible(toolwin) || menubarVisible)
      {
        if (cursorVisible)
          HidePointer();
        if (menubarVisible)
          HideMenubar();
        if (IsWindowVisible(toolwin))
          HideTools();
      }
      else
      {
        if (!cursorVisible)
          ShowPointer();
        if (!menubarVisible)
          ShowMenubar();
        if (!IsWindowVisible(toolwin))
          ShowTools();
      }
      break;

    case iConsole:
      if (IsWindowVisible(SouixWin))
        HideConsole();
      else
        ShowConsole();
      break;

    case iSnapshot:
      // the following will break if snapshot is not last bitmap button instantiated
      bx_tool_pixmap[toolPixMaps-1].f();
      break;
    }

  default:
    break;
  }

  HiliteMenu(0);
}

BX_CPP_INLINE void HandleClick(EventRecord *event)
{
  short part;
  WindowPtr whichWindow;
  Rect dRect;

  part = FindWindow(event->where, &whichWindow);

  switch(part)
  {
    case inContent:
      if (whichWindow == win)
      {
        if (win != FrontWindow())
          SelectWindow(win);
        if (event->modifiers & cmdKey)
          mouse_button_state |= 0x02;
        else
          mouse_button_state |= 0x01;
      }
      else if (whichWindow == toolwin)
      {
        HiliteWindow(win, true);
        HandleToolClick(event->where);
      }
      else if (whichWindow == backdrop)
      {
        SelectWindow(win);
      }
      else if (whichWindow == SouixWin)
      {
        SelectWindow(SouixWin);
      }
      break;

    case inDrag:
      dRect = qd.screenBits.bounds;
      if (IsWindowVisible(toolwin))
        dRect.top = gMaxTop;
      DragWindow(whichWindow, event->where, &dRect);
      break;

    case inMenuBar:
      HandleMenuChoice(MenuSelect(event->where));
      break;
  }
}

void UpdateWindow(WindowPtr window)
{
  GrafPtr oldPort;
  Rect    box;

  GetPort(&oldPort);

  SetPort(window);
  BeginUpdate(window);

  if (window == win)
  {
    box = window->portRect;
    DEV_vga_redraw_area(box.left, box.top, box.right, box.bottom);
  }
  else if (window == backdrop)
  {
    box = window->portRect;
    FillRect(&box, &qd.black);
  }
  else if (window == toolwin)
  {
    theGui->show_headerbar();
  }

  EndUpdate(window);
  SetPort(oldPort);
}

// ::HANDLE_EVENTS()
//
// Called periodically (vga_update_interval in .bochsrc) so the
// the gui code can poll for keyboard, mouse, and other
// relevant events.

void bx_macintosh_gui_c::handle_events(void)
{
  EventRecord event;
  Point mousePt;
  int dx, dy;
  int oldMods1=0;
  EventModifiers newMods;
  unsigned curstate;
  GrafPtr oldport;

  curstate = mouse_button_state;      //so we can compare the old and the new mouse state

  if (WaitNextEvent(everyEvent, &event, SLEEP_TIME, NULL))
  {
    switch(event.what)
    {
      case nullEvent:
        break;

      case mouseDown:
        HandleClick(&event);
        break;

      case mouseUp:
        if (event.modifiers & cmdKey)
          mouse_button_state &= ~0x02;
        else
          mouse_button_state &= ~0x01;
        break;

      case keyDown:
      case autoKey:
        oldMods1 = event.modifiers;
        HandleKey(&event, BX_KEY_PRESSED);
        break;

      case keyUp:
        event.modifiers = oldMods1;
        HandleKey(&event, BX_KEY_RELEASED);
        break;

      case updateEvt:
        if ((WindowPtr)event.message == SouixWin)
          SIOUXHandleOneEvent(&event);
        else
          UpdateWindow((WindowPtr)event.message);
        break;

      case diskEvt:
        floppyA_handler();

      default:
        break;
    }
  }
  else if (oldMods != (newMods = (event.modifiers & 0xfe00)))
  {
    if ((newMods ^ oldMods) & shiftKey)
      DEV_kbd_gen_scancode(BX_KEY_SHIFT_L | ((newMods & shiftKey)?BX_KEY_PRESSED:BX_KEY_RELEASED));
    if ((newMods ^ oldMods) & alphaLock)
      DEV_kbd_gen_scancode(BX_KEY_CAPS_LOCK | ((newMods & alphaLock)?BX_KEY_PRESSED:BX_KEY_RELEASED));
    if ((newMods ^ oldMods) & optionKey)
      DEV_kbd_gen_scancode(BX_KEY_ALT_L | ((newMods & optionKey)?BX_KEY_PRESSED:BX_KEY_RELEASED));
    if ((newMods ^ oldMods) & controlKey)
      DEV_kbd_gen_scancode(BX_KEY_CTRL_L | ((newMods & controlKey)?BX_KEY_PRESSED:BX_KEY_RELEASED));
    if ((newMods ^ oldMods) & rightShiftKey)
      DEV_kbd_gen_scancode(BX_KEY_SHIFT_R | ((newMods & rightShiftKey)?BX_KEY_PRESSED:BX_KEY_RELEASED));
    if ((newMods ^ oldMods) & rightOptionKey)
      DEV_kbd_gen_scancode(BX_KEY_ALT_R | ((newMods & rightOptionKey)?BX_KEY_PRESSED:BX_KEY_RELEASED));
    if ((newMods ^ oldMods) & rightControlKey)
      DEV_kbd_gen_scancode(BX_KEY_CTRL_R | ((newMods & rightControlKey)?BX_KEY_PRESSED:BX_KEY_RELEASED));
    oldMods = newMods;
  }

  GetPort(&oldport);
  SetPort(win);

  GetMouse(&mousePt);

//if mouse has moved, or button has changed state
  if ((!EqualPt(mousePt, prevPt)) || (curstate != mouse_button_state))
  {
    dx = mousePt.h - prevPt.h;
    dy = prevPt.v - mousePt.v;

    DEV_mouse_motion(dx, dy, 0, mouse_button_state, 0);

    if (!cursorVisible)
    {
      SetPt(&scrCenter, 320, 240);
      LocalToGlobal(&scrCenter);
      ResetPointer();         //next getmouse should be 320, 240
      SetPt(&mousePt, 320, 240);
    }
  }

  prevPt = mousePt;

  SetPort(oldport);
}

// ::FLUSH()
//
// Called periodically, requesting that the gui code flush all pending
// screen update requests.

void bx_macintosh_gui_c::flush(void)
{
  // an opportunity to make the Window Manager happy.
  // not needed on the macintosh....
}

// ::CLEAR_SCREEN()
//
// Called to request that the VGA region is cleared.    Don't
// clear the area that defines the headerbar.

void bx_macintosh_gui_c::clear_screen(void)
{
  SetPort(win);

  RGBForeColor(&black);
  RGBBackColor(&white);

  FillRect(&win->portRect, &qd.black);
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
// format of old_text & new_text: each is tm_info->line_offset*text_rows
//     bytes long. Each character consists of 2 bytes.  The first by is
//     the character value, the second is the attribute byte.
//
// cursor_x: new x location of cursor
// cursor_y: new y location of cursor
// tm_info:  this structure contains information for additional
//           features in text mode (cursor shape, line offset,...)

void bx_macintosh_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
                 unsigned long cursor_x, unsigned long cursor_y,
                 bx_vga_tminfo_t *tm_info)
{
  int           i;
  unsigned char achar;
  int           x, y;
  static int    previ;
  int           cursori;
  Rect          destRect;
  RGBColor      fgColor, bgColor;
  GrafPtr       oldPort;
  unsigned      nchars;
  OSErr         theError;

  GetPort(&oldPort);

  SetPort(win);

  //current cursor position
  cursori = (cursor_y * text_cols + cursor_x) * 2;

  // Number of characters on screen, variable number of rows
  nchars = text_cols * text_rows;

  for (i=0; i<nchars*2; i+=2)
  {
    if (i == cursori || i == previ || new_text[i] != old_text[i] || new_text[i+1] != old_text[i+1])
    {
      achar = new_text[i];

//    fgColor = (**gCTable).ctTable[new_text[i+1] & 0x0F].rgb;
//    bgColor = (**gCTable).ctTable[(new_text[i+1] & 0xF0) >> 4].rgb;

//    RGBForeColor(&fgColor);
//    RGBBackColor(&bgColor);

      if (SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->get())
      {
        PmForeColor(new_text[i+1] & 0x0F);
        PmBackColor((new_text[i+1] & 0xF0) >> 4);
      }
      else
      {
        fgColor = (**gCTable).ctTable[new_text[i+1] & 0x0F].rgb;
        bgColor = (**gCTable).ctTable[(new_text[i+1] & 0xF0) >> 4].rgb;

        RGBForeColor(&fgColor);
        RGBBackColor(&bgColor);
      }

      x = ((i/2) % text_cols)*FONT_WIDTH;
      y = ((i/2) / text_cols)*FONT_HEIGHT;

      SetRect(&destRect, x, y, x+FONT_WIDTH, y+FONT_HEIGHT);

      CopyBits(vgafont[achar], &WINBITMAP(win),
                                &srcTextRect, &destRect, srcCopy, NULL);
      if ((theError = QDError()) != noErr)
        BX_ERROR(("mac: CopyBits returned %hd", theError));

      if (i == cursori)   //invert the current cursor block
      {
        InvertRect(&destRect);
      }
    }
  }

  //previous cursor position
  previ = cursori;

  SetPort(oldPort);
}

int bx_macintosh_gui_c::get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)
{
  return 0;
}

int bx_macintosh_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
{
  return 0;
}

// ::PALETTE_CHANGE()
//
// Allocate a color in the native GUI, for this color, and put
// it in the colormap location 'index'.
// returns: 0=no screen update needed (color map change has direct effect)
//          1=screen updated needed (redraw using current colormap)

bx_bool bx_macintosh_gui_c::palette_change(Bit8u index, Bit8u red, Bit8u green, Bit8u blue)
{
  PaletteHandle thePal, oldpal;
  GDHandle  saveDevice;
  CGrafPtr  savePort;
  GrafPtr   oldPort;

/*if (gOffWorld != NULL) //(SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->get())
  {
    GetGWorld(&savePort, &saveDevice);

    SetGWorld(gOffWorld, NULL);
  }*/

  if ((**gTile).pixelType != RGBDirect)
  {
    GetPort(&oldPort);
    SetPort(win);

    (**gCTable).ctTable[index].value = index;
    (**gCTable).ctTable[index].rgb.red = (red << 8);
    (**gCTable).ctTable[index].rgb.green = (green << 8);
    (**gCTable).ctTable[index].rgb.blue = (blue << 8);

    SetEntries(index, index, (**gCTable).ctTable);

    CTabChanged(gCTable);

    SetPort(oldPort);
  }

/*if (gOffWorld != NULL) //(SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->get())
    SetGWorld(savePort, saveDevice);*/

  if (SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->get())
  {
    thePal = NewPalette(index, gCTable, pmTolerant, 0x5000);
    oldpal = GetPalette(win);

    SetPalette(win, thePal, false);
    SetPalette(fullwin, thePal, false);
    SetPalette(hidden, thePal, false);

    return(1);
  }

  return((**gTile).pixelType != RGBDirect);
}

// ::GRAPHICS_TILE_UPDATE()
//
// Called to request that a tile of graphics be drawn to the
// screen, since info in this region has changed.
//
// tile: array of 8bit values representing a block of pixels with
//       dimension equal to the 'x_tilesize' & 'y_tilesize' members.
//       Each value specifies an index into the
//       array of colors you allocated for ::palette_change()
// x0: x origin of tile
// y0: y origin of tile
//
// note: origin of tile and of window based on (0,0) being in the upper
//       left of the window.

void bx_macintosh_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
  Rect      destRect;
  OSErr     theError;
  Ptr       theBaseAddr;
  GrafPtr   oldPort;
/*GDHandle  saveDevice;
  CGrafPtr  savePort;

  GetGWorld(&savePort, &saveDevice);

  SetGWorld(gOffWorld, NULL); */
  GetPort(&oldPort);
  SetPort(win);
  destRect = srcTileRect;
  OffsetRect(&destRect, x0, y0);

  //(**gTile).baseAddr = (Ptr)tile;
  if ((theBaseAddr = GetPixBaseAddr(gTile)) == NULL)
    BX_PANIC(("mac: gTile has NULL baseAddr (offscreen buffer purged)"));
  else if (disp_bpp == 24 || disp_bpp == 32) {
    for (unsigned iY = 0; iY < (srcTileRect.bottom-srcTileRect.top); iY++) {
      Bit8u *iA = ((Bit8u *)theBaseAddr) + iY * GetPixRowBytes(gTile);
      for (unsigned iX = 0; iX < (srcTileRect.right-srcTileRect.left); iX++) {
        iA[iX*4 + 3] = tile[((srcTileRect.right-srcTileRect.left)*iY+iX)*(disp_bpp>>3)];
        iA[iX*4 + 2] = tile[((srcTileRect.right-srcTileRect.left)*iY+iX)*(disp_bpp>>3) + 1];
        iA[iX*4 + 1] = tile[((srcTileRect.right-srcTileRect.left)*iY+iX)*(disp_bpp>>3) + 2];
        iA[iX*4] = disp_bpp == 24 ? 0 : tile[((srcTileRect.right-srcTileRect.left)*iY+iX)*4 + 3];
      }
    }
  } else {
    BlockMoveData(tile, theBaseAddr, (srcTileRect.bottom-srcTileRect.top) * GetPixRowBytes(gTile));
  }
  RGBForeColor(&black);
  RGBBackColor(&white);
  CopyBits(GetPortBitMapForCopyBits(gOffWorld), &WINBITMAP(win),
                                                &srcTileRect, &destRect, srcCopy, NULL);
  if ((theError = QDError()) != noErr)
    BX_ERROR(("mac: CopyBits returned %hd", theError));
  SetPort(oldPort);

//SetGWorld(savePort, saveDevice);
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

void bx_macintosh_gui_c::dimension_update(unsigned x, unsigned y, unsigned fheight, unsigned fwidth, unsigned bpp)
{
  if ((bpp != 1) && (bpp != 2) && (bpp != 4) && (bpp != 8) && (bpp != 15) && (bpp != 16) && (bpp != 24) && (bpp != 32)) {
    BX_PANIC(("%d bpp graphics mode not supported yet", bpp));
  }
  guest_textmode = (fheight > 0);
  guest_xres = x;
  guest_yres = y;
  guest_bpp = bpp;
  if (bpp != disp_bpp) {
    free(gMyBuffer);
    if ((**gTile).pixelType == RGBDirect)
      gCTable = GetCTable(128);
    DisposeGWorld(gOffWorld);
    disp_bpp = bpp;
    CreateTile();
  }
  if (guest_textmode) {
    text_cols = x / fwidth;
    text_rows = y / fheight;
    if (fwidth != 8) {
      x = x * 8 / fwidth;
    }
    if (fheight != 16) {
      y = y * 16 / fheight;
    }
  }

  if (x != width || y != height)
  {
    SizeWindow(win, x, y, false);
    SizeWindow(fullwin, x, y, false);
    SizeWindow(hidden, x, y, false);
    width = x;
    height = y;
  }

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

// rewritten by tim senecal to use the cicn (color icon) resources instead

unsigned bx_macintosh_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
  unsigned i;
  unsigned char *data;
  long row_bytes, bytecount;

  bx_cicn[numPixMaps] = GetCIcon(numPixMaps+128);
  BX_ASSERT(bx_cicn[numPixMaps]);

  numPixMaps++;

  return(numPixMaps-1);
}

// ::HEADERBAR_BITMAP()
//
// Called to install a bitmap in the bochs headerbar (toolbar).
//
// bmap_id: will correspond to an ID returned from
//     ::create_bitmap().  'alignment' is either BX_GRAVITY_LEFT
//     or BX_GRAVITY_RIGHT, meaning install the bitmap in the next
//     available leftmost or rightmost space.
// f: a 'C' function pointer to callback when the mouse is clicked in
//     the boundaries of this bitmap.

unsigned bx_macintosh_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
{
  unsigned hb_index;

  toolPixMaps++;
  hb_index = toolPixMaps-1;
//bx_tool_pixmap[hb_index].pm = bx_pixmap[bmap_id];
  bx_tool_pixmap[hb_index].cicn = bx_cicn[bmap_id];
  bx_tool_pixmap[hb_index].alignment = alignment;
  bx_tool_pixmap[hb_index].f = f;

  if (alignment == BX_GRAVITY_LEFT)
  {
    bx_tool_pixmap[hb_index].xorigin = bx_bitmap_left_xorigin;
    bx_tool_pixmap[hb_index].yorigin = 0;
//  bx_bitmap_left_xorigin += (**bx_pixmap[bmap_id]).bounds.right;
    bx_bitmap_left_xorigin += 34;
  }
  else
  {
//  bx_bitmap_right_xorigin += (**bx_pixmap[bmap_id]).bounds.right;
    bx_bitmap_right_xorigin += 34;
    bx_tool_pixmap[hb_index].xorigin = bx_bitmap_right_xorigin;
    bx_tool_pixmap[hb_index].yorigin = 0;
  }
  return(hb_index);
}

// ::SHOW_HEADERBAR()
//
// Show (redraw) the current headerbar, which is composed of
// currently installed bitmaps.

void bx_macintosh_gui_c::show_headerbar(void)
{
  Rect destRect;
  int  i, xorigin;

  SetPort(toolwin);
  RGBForeColor(&medGrey);
  FillRect(&toolwin->portRect, &qd.black);
  for (i=0; i<toolPixMaps; i++)
  {
    if (bx_tool_pixmap[i].alignment == BX_GRAVITY_LEFT)
      xorigin = bx_tool_pixmap[i].xorigin;
    else
      xorigin = toolwin->portRect.right - bx_tool_pixmap[i].xorigin;

    SetRect(&destRect, xorigin, 0, xorigin+32, 32);

    //changed, simply plot the cicn for that button, in the prescribed rectangle
    PlotCIcon(&destRect, bx_tool_pixmap[i].cicn);
  }
  RGBForeColor(&black);

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

void bx_macintosh_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
//bx_tool_pixmap[hbar_id].pm = bx_pixmap[bmap_id];
  bx_tool_pixmap[hbar_id].cicn = bx_cicn[bmap_id];
  show_headerbar();
}

// ::EXIT()
//
// Called before bochs terminates, to allow for a graceful
// exit from the native GUI mechanism.

void bx_macintosh_gui_c::exit(void)
{
  if (!menubarVisible)
    ShowMenubar(); // Make the menubar visible again
  InitCursor();
}

#if 0
void bx_macintosh_gui_c::snapshot_handler(void)
{
  PicHandle ScreenShot;
  long val;

  SetPort(win);

  ScreenShot = OpenPicture(&win->portRect);

  CopyBits(&win->portBits, &win->portBits, &win->portRect, &win->portRect, srcCopy, NULL);

  ClosePicture();

  val = ZeroScrap();

  HLock((Handle)ScreenShot);
  PutScrap(GetHandleSize((Handle)ScreenShot), 'PICT', *ScreenShot);
  HUnlock((Handle)ScreenShot);

  KillPicture(ScreenShot);
}
#endif

// UpdateRgn()
//
// Updates the screen after the menubar and round corners have been hidden

void UpdateRgn(RgnHandle rgn)
{
  WindowPtr window;

  window = FrontWindow();
  PaintBehind(window, rgn);
  CalcVisBehind(window, rgn);
}

// HidePointer()
//
// Hides the Mac mouse pointer

void HidePointer()
{
  HiliteMenu(0);
  HideCursor();
  SetPort(win);
  SetPt(&scrCenter, 320, 240);
  LocalToGlobal(&scrCenter);
  ResetPointer();
  GetMouse(&prevPt);
  cursorVisible = false;
  CheckItem(GetMenuHandle(mBochs), iCursor, false);
}

// ShowPointer()
//
// Shows the Mac mouse pointer

void ShowPointer()
{
  InitCursor();
  cursorVisible = true;
  CheckItem(GetMenuHandle(mBochs), iCursor, true);
}

// HideTools()
//
// Hides the Bochs toolbar

void HideTools()
{
  HideWindow(toolwin);
  if (menubarVisible)
  {
    MoveWindow(win, gLeft, gMinTop, false);
  }
  else
  {
    MoveWindow(hidden, gLeft, gMinTop, false);
  }
  CheckItem(GetMenuHandle(mBochs), iTool, false);
  HiliteWindow(win, true);
}

// ShowTools()
//
// Shows the Bochs toolbar

void ShowTools()
{
  if (menubarVisible)
  {
    MoveWindow(win, gLeft, gMaxTop, false);
  }
  else
  {
    MoveWindow(hidden, gLeft, gMaxTop, false);
  }
  ShowWindow(toolwin);
  BringToFront(toolwin);
  SelectWindow(toolwin);
  HiliteWindow(win, true);
//theGui->show_headerbar();
  CheckItem(GetMenuHandle(mBochs), iTool, true);
  HiliteWindow(win, true);
}

// HideMenubar()
//
// Hides the menubar (obviously)

void HideMenubar()
{
  Rect      mBarRect;
  RgnHandle grayRgn;

  grayRgn = LMGetGrayRgn();
  gOldMBarHeight = GetMBarHeight();
  LMSetMBarHeight(0);
  mBarRgn = NewRgn();
  mBarRect = qd.screenBits.bounds;
  mBarRect.bottom = mBarRect.top + gOldMBarHeight;
  RectRgn(mBarRgn, &mBarRect);
  UnionRgn(grayRgn, mBarRgn, grayRgn);
  UpdateRgn(mBarRgn);

  cnrRgn = NewRgn();
  RectRgn(cnrRgn, &qd.screenBits.bounds);
  DiffRgn(cnrRgn, grayRgn, cnrRgn);
  UnionRgn(grayRgn, cnrRgn, grayRgn);
  UpdateRgn(mBarRgn);

  HideWindow(win);
  ShowWindow(backdrop);
  SelectWindow(backdrop);
  hidden = win;
  win = fullwin;
  ShowWindow(win);

  SelectWindow(win);
  menubarVisible = false;
  CheckItem(GetMenuHandle(mBochs), iMenuBar, false);
}

// ShowMenubar()
//
// Makes the menubar visible again so other programs will display correctly.

void ShowMenubar()
{
  RgnHandle grayRgn;
  GrafPtr   savePort;

  grayRgn = LMGetGrayRgn();
  LMSetMBarHeight(gOldMBarHeight);
  DiffRgn(grayRgn, mBarRgn, grayRgn);
  DisposeRgn(mBarRgn);
  DrawMenuBar();

  GetPort(&savePort);
  SetPort(LMGetWMgrPort());
  SetClip(cnrRgn);
  FillRgn(cnrRgn, &qd.black);
  SetPort(savePort);
  DiffRgn(grayRgn, cnrRgn, grayRgn);
  DisposeRgn(cnrRgn);

  HideWindow(backdrop);
  win = hidden;
  hidden = fullwin;
  HideWindow(hidden);
  ShowWindow(win);
  HiliteWindow(win, true);

  menubarVisible = true;
  CheckItem(GetMenuHandle(mBochs), iMenuBar, true);
}

void HideConsole()
{
  HideWindow(SouixWin);
  CheckItem(GetMenuHandle(mBochs), iConsole, false);
}

void ShowConsole()
{
  ShowWindow(SouixWin);
  SelectWindow(SouixWin);
  CheckItem(GetMenuHandle(mBochs), iConsole, true);
}

// CreateKeyMap()
//
// Create a KCHR data structure to map Mac virtual key codes to Bochs key codes

void CreateKeyMap(void)
{
  const unsigned char KCHRHeader [258] = { 0, 1 };

  const unsigned char KCHRTable [130] = {
    0,
    1,
    BX_KEY_A,
    BX_KEY_S,
    BX_KEY_D,
    BX_KEY_F,
    BX_KEY_H,
    BX_KEY_G,
    BX_KEY_Z,
    BX_KEY_X,
    BX_KEY_C,
    BX_KEY_V,
    BX_KEY_LEFT_BACKSLASH,
    BX_KEY_B,
    BX_KEY_Q,
    BX_KEY_W,
    BX_KEY_E,
    BX_KEY_R,
    BX_KEY_Y,
    BX_KEY_T,
    BX_KEY_1,
    BX_KEY_2,
    BX_KEY_3,
    BX_KEY_4,
    BX_KEY_6,
    BX_KEY_5,
    BX_KEY_EQUALS,
    BX_KEY_9,
    BX_KEY_7,
    BX_KEY_MINUS,
    BX_KEY_8,
    BX_KEY_0,
    BX_KEY_RIGHT_BRACKET,
    BX_KEY_O,
    BX_KEY_U,
    BX_KEY_LEFT_BRACKET,
    BX_KEY_I,
    BX_KEY_P,
    BX_KEY_ENTER,
    BX_KEY_L,
    BX_KEY_J,
    BX_KEY_SINGLE_QUOTE,
    BX_KEY_K,
    BX_KEY_SEMICOLON,
    BX_KEY_BACKSLASH,
    BX_KEY_COMMA,
    BX_KEY_SLASH,
    BX_KEY_N,
    BX_KEY_M,
    BX_KEY_PERIOD,
    BX_KEY_TAB,
    BX_KEY_SPACE,
    BX_KEY_GRAVE,
    BX_KEY_BACKSPACE,
    BX_KEY_KP_ENTER,
    BX_KEY_ESC,
    0, // 0x36 (record button)
    0, // 0x37 (cmd key)
    0, // 0x38 (left shift)
    0, // 0x39 (caps lock)
    0, // 0x3A (left option/alt)
    0, // 0x3B (left ctrl)
    0, // 0x3C (right shift)
    0, // 0x3D (right option/alt)
    0, // 0x3E (right ctrl)
    0, // 0x3F (fn key -- laptops)
    0, // 0x40
    BX_KEY_KP_DELETE, // KP_PERIOD
    0, // 0x42 (move right/multiply)
    BX_KEY_KP_MULTIPLY,
    0, // 0x44
    BX_KEY_KP_ADD,
    0, // 0x46 (move left/add)
    BX_KEY_NUM_LOCK,
    0, // 0x48 (move down/equals)
    0, // 0x49
    0, // 0x4A
    BX_KEY_KP_DIVIDE,
    BX_KEY_KP_ENTER,
    0, // 0x4D (move up/divide)
    BX_KEY_KP_SUBTRACT,
    0, // 0x4F
    0, // 0x50
    BX_KEY_EQUALS, // 0x51 (kp equals)
    BX_KEY_KP_INSERT, // 0x52 (kp 0)
    BX_KEY_KP_END, // 0x53 (kp 1)
    BX_KEY_KP_DOWN, // 0x54 (kp 2)
    BX_KEY_KP_PAGE_DOWN, // 0x55 (kp 3)
    BX_KEY_KP_LEFT, // 0x56 (kp 4)
    BX_KEY_KP_5,
    BX_KEY_KP_RIGHT, // 0x58 (kp 6)
    BX_KEY_KP_HOME, // 0x59 (kp 7)
    0, // 0x5A
    BX_KEY_KP_UP, // 0x5B (kp 8)
    BX_KEY_KP_PAGE_UP, // 0x5C (kp 9)
    0, // 0x5D
    0, // 0x5E
    0, // 0x5F
    BX_KEY_F5,
    BX_KEY_F6,
    BX_KEY_F7,
    BX_KEY_F3,
    BX_KEY_F8,
    BX_KEY_F9,
    0, // 0x66
    BX_KEY_F11,
    0, // 0x68
    BX_KEY_PRINT, // 0x69 (print screen)
    0, // 0x6A
    BX_KEY_SCRL_LOCK, // 0x6B (scroll lock)
    0, // 0x6C
    BX_KEY_F10,
    BX_KEY_MENU, // 0x6E
    BX_KEY_F12,
    0, // 0x70
    BX_KEY_PAUSE, // 0x71 (pause)
    BX_KEY_INSERT,
    BX_KEY_HOME,
    BX_KEY_PAGE_UP,
    BX_KEY_DELETE,
    BX_KEY_F4,
    BX_KEY_END,
    BX_KEY_F2,
    BX_KEY_PAGE_DOWN,
    BX_KEY_F1,
    BX_KEY_LEFT,
    BX_KEY_RIGHT,
    BX_KEY_DOWN,
    BX_KEY_UP
  };

  KCHR = NewPtrClear(390);
  if (KCHR == NULL)
    BX_PANIC(("mac: can't allocate memory for key map"));

  BlockMove(KCHRHeader, KCHR, sizeof(KCHRHeader));
  BlockMove(KCHRTable, Ptr(KCHR + sizeof(KCHRHeader)), sizeof(KCHRTable));
}

// CreateVGAFont()
//
// Create an array of PixMaps for the PC screen font

void CreateVGAFont(void)
{
  int i, x;
  unsigned char *fontData, curPixel;
  long row_bytes, bytecount;

  for (i=0; i<256; i++)
  {
    vgafont[i] = CreateBitMap(FONT_WIDTH, FONT_HEIGHT);
    row_bytes = (*(vgafont[i])).rowBytes;
    bytecount = row_bytes * FONT_HEIGHT;
    fontData = (unsigned char *)NewPtrClear(bytecount);

    for (x=0; x<16; x++)
    {
      //curPixel = ~(bx_vgafont[i].data[x]);
      curPixel = (bx_vgafont[i].data[x]);
      fontData[x*row_bytes] = reverse_bitorder(curPixel);
    }

    vgafont[i]->baseAddr = Ptr(fontData);
  }
}

// CreateBitMap()
// Allocate a new bitmap and fill in the fields with appropriate
// values.

BitMap *CreateBitMap(unsigned width,  unsigned height)
{
  BitMap *bm;
  long   row_bytes;

  row_bytes = ((width + 31) >> 5) << 2;
  bm = (BitMap *)calloc(1, sizeof(BitMap));
  if (bm == NULL)
    BX_PANIC(("mac: can't allocate memory for pixmap"));
  SetRect(&bm->bounds, 0, 0, width, height);
  bm->rowBytes = row_bytes;
  // Quickdraw allocates a new color table by default, but we want to
  // use one we created earlier.

  return bm;
}

// CreatePixMap()
// Allocate a new pixmap handle and fill in the fields with appropriate
// values.

PixMapHandle CreatePixMap(unsigned left, unsigned top, unsigned width,
        unsigned height, unsigned depth, CTabHandle clut)
{
  PixMapHandle pm;
  long         row_bytes;

  row_bytes = (((long) depth * ((long) width) + 31) >> 5) << 2;
  pm = NewPixMap();
  if (pm == NULL)
    BX_PANIC(("mac: can't allocate memory for pixmap"));
  (**pm).bounds.left = left;
  (**pm).bounds.top = top;
  (**pm).bounds.right = left+width;
  (**pm).bounds.bottom = top+height;
  (**pm).pixelSize = depth;
  (**pm).rowBytes = row_bytes | 0x8000;

  DisposeCTable((**pm).pmTable);
  (**pm).pmTable = clut;
  // Quickdraw allocates a new color table by default, but we want to
  // use one we created earlier.

  return pm;
}

unsigned char reverse_bitorder(unsigned char b)
{
  unsigned char ret=0;

  for (unsigned i=0; i<8; i++)
  {
    ret |= (b & 0x01) << (7-i);
    b >>= 1;
  }

  return(ret);
}

void bx_macintosh_gui_c::mouse_enabled_changed_specific (bx_bool val)
{
}

#endif /* if BX_WITH_MACOS */
